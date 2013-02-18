#include "CorePCH.h"

/*-----------------------------------------------------------------------------
	Global variables.
-----------------------------------------------------------------------------*/
#if UNICODE
UBOOL		GUnicode		= TRUE;
#else
UBOOL		GUnicode		= FALSE;
#endif

UBOOL							GIsRequestingExit	= FALSE;

static FOutputDeviceRedirector	LogRecorder;
FOutputDeviceRedirectorBase*	GLog				= &LogRecorder;
FOutputDeviceConsole*			GLogConsole			= NULL;
FOutputDeviceFile*				GLogFile			= NULL;
static FNullOut					NullOut;
FOutputDevice*					GNullOut			= &NullOut;

TCHAR							GGameName[64]		= TEXT("Example");

DOUBLE							GSecondsPerCycle	= 0.0;
QWORD							GNumTimingCodeCalls	= 0;
FGlobalMath						GMath;
map<UINT, FString>				GNames;
FFileManager*					GFileManager		= NULL;

DOUBLE							GStartTime;
DOUBLE							GCurrentTime		= 0.0f;
DOUBLE							GLastTime			= 0.0f;
DOUBLE							GDeltaTime			= 1.0 / 30.0f;
DOUBLE							GFixedDeltaTime		= 1.0 / 30.0f;
QWORD							GFrameCounter		= 0;

UBOOL							GIsBenchmarking		= FALSE;

UINT							GNumHardwareThreads	= 1;

DWORD							GGameThreadId		= 0;

UBOOL							GIsBuildMachine		= FALSE;

FConfigSystem					GConfigSystem;



/*-----------------------------------------------------------------------------
	File implementation
-----------------------------------------------------------------------------*/
const TCHAR* appBaseDir()
{
	static TCHAR Result[512]=TEXT("");
	if( !Result[0] )
	{
		// Get directory this executable was launched from.
#if UNICODE
		if( GUnicode )
		{
			ANSICHAR ACh[256];
			GetModuleFileNameA( NULL, ACh, 256 );
			MultiByteToWideChar( CP_ACP, 0, ACh, -1, Result, 512 );
		}
		else
#endif
		{
			GetModuleFileName( NULL, Result, 512 );
		}
		INT StringLength = appStrlen(Result);

		if(StringLength > 0)
		{
			--StringLength;
			for(; StringLength > 0; StringLength-- )
			{
				if( Result[StringLength - 1] == PATH_SEPARATOR[0] || Result[StringLength - 1] == '/' )
				{
					break;
				}
			}
		}
		Result[StringLength] = 0;
	}
	return Result;
}

const TCHAR* appEngineDir()
{
	static TCHAR Dest[MAX_SPRINTF] = TEXT("");
	appSprintf(Dest, TEXT("..\\Engine\\"));
	return Dest;
}

const TCHAR* appEngineConfigDir()
{
	static TCHAR Dest[MAX_SPRINTF] = TEXT("");
	const TCHAR* EngineDir = appEngineDir();
	appSprintf(Dest, TEXT("%sConfig\\"), EngineDir);
	return Dest;
}

const TCHAR* appShaderDir()
{
	static TCHAR Result[256] = TEXT("..\\Engine\\Shaders\\");
	return Result;
}

TCHAR GCmdLine[16384] = TEXT("");

const TCHAR* appCmdLine()
{
	return GCmdLine;
}

const TCHAR* appGameDir()
{
	static TCHAR Dest[MAX_SPRINTF] = TEXT("");
	appSprintf(Dest, TEXT("..\\%sGame\\"), GGameName);
	return Dest;
}

const TCHAR* appGameConfigDir()
{
	static TCHAR Dest[MAX_SPRINTF] = TEXT("");
	appSprintf(Dest, TEXT("%s%s"), appGameDir(), TEXT("Config\\"));
	return Dest;
}

const TCHAR* appGameLogDir()
{
	static TCHAR Dest[MAX_SPRINTF] = TEXT("");
	appSprintf(Dest, TEXT("%s%s"), appGameDir(), TEXT("Log\\"));
	return Dest;
}

const TCHAR* appGameContentDir()
{
	static TCHAR Dest[MAX_SPRINTF] = TEXT("");
	appSprintf(Dest, TEXT("%s%s"), appGameDir(), TEXT("Content\\"));
	return Dest;
}

/*-----------------------------------------------------------------------------
	Load&Save String functions.
-----------------------------------------------------------------------------*/
UBOOL appLoadFileToString(FString& Result, const TCHAR* Filename, FFileManager* FileManager)
{
	FArchive* Reader = FileManager->CreateFileReader(Filename);
	if( !Reader )
	{
		return FALSE;
	}

	INT Size = Reader->TotalSize();
	BYTE* Buffer = new BYTE[Size];
	Reader->Serialize(Buffer, Size);
	UBOOL Success = Reader->Close();
	delete Reader;

	appBufferToString(Result, Buffer, Size);
	delete Buffer;

	return Success;
}

void appBufferToString(FString& String, const BYTE* Buffer, INT Size)
{
	if( Size >= 2 && !(Size & 1) && Buffer[0] == 0xff && Buffer[1] == 0xfe )
	{
		// Unicode
		String.resize(Size/2 - 1);		// exclude the BOM
		for(INT i = 0; i < (Size/2) - 1; ++i)
		{
			// INTEL处理器在存储多字节时总是低位优先，在内存中顺序则是“0xfe”、“0xff”
			String[i] = FromUnicode( (WORD)Buffer[i * 2 + 2] + (WORD)Buffer[i * 2 + 3] * 256 );
		}
	}
	else
	{
		// Ansi
		String.resize(Size);
		TCHAR* Temp = new TCHAR[Size+1];
		INT res = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, (LPCSTR)Buffer, Size, Temp, (Size + 1) * sizeof(TCHAR));
		if( res > 0 )
		{
			String = FString(Temp, res);
		}
		else
		{
			// if the conversion failed.
			for(INT i = 0; i < Size; ++i)
			{
				String.at(i) = FromAnsi(Buffer[i]);
			}
		}
		delete[] Temp;
	}
}

UBOOL appLoadFileToArray(vector<BYTE>& Buffer, const TCHAR* Filename, FFileManager* FileManager)
{
	FArchive* Reader = FileManager->CreateFileReader(Filename);
	if( !Reader )
	{
		return FALSE;
	}

	INT Size = Reader->TotalSize();
	Buffer.clear();
	Buffer.resize(Size);

	//BYTE* Temp = new BYTE[Size];
	//Reader->Serialize(Temp, Size);
	Reader->Serialize(&Buffer.at(0), Size);
	UBOOL Success = Reader->Close();
	delete Reader;

	//for(INT i = 0; i < Size; ++i)
	//{
	//	Buffer[i] = Temp[i];
	//}
	//delete[] Temp;

	return Success;
}

UBOOL appSaveStringToFile(const FString& String, const TCHAR* Filename, UBOOL bForceSaveAsAnsi, FFileManager* FileManager)
{
	if( !String.size() )
	{
		return FALSE;
	}

	FArchive* Ar = FileManager->CreateFileWriter(Filename);
	if( !Ar )
	{
		return FALSE;
	}

	UBOOL bSaveAsUnicode = FALSE, bSaveAsAnsi = FALSE, bSuccess = TRUE;
	for(INT i = 0; i < String.size(); ++i)
	{
		if( String.at(i) != FromAnsi(ToAnsi(String.at(i))) )
		{
			// meaning exist unicode char
			if( bForceSaveAsAnsi )
			{
				bSaveAsAnsi = TRUE;
			}
			else
			{
				UNICHAR BOM = UNICODE_BOM;
				Ar->Serialize(&BOM, sizeof(BOM));
				bSaveAsUnicode = TRUE;
			}
			break;
		}
	}

	if( bSaveAsUnicode )
	{
		Ar->Serialize((void*)String.c_str(), String.size() * sizeof(TCHAR));
	}
	else if( bSaveAsAnsi )
	{
		FTCHARToANSI Convert(String.c_str());
		Ar->Serialize((ANSICHAR*)Convert, String.size());
	}
	else
	{	// pure ansi
		vector<ANSICHAR> AnsiBuffer;
		for(INT i = 0; i < String.length(); ++i)
		{
			AnsiBuffer.push_back(ToAnsi(String[i]));
		}
		Ar->Serialize((ANSICHAR*)&AnsiBuffer.at(0), String.length());
	}

	bSuccess = Ar->Close();
	delete Ar;
	if( !bSuccess )
	{
		GFileManager->Delete(Filename);
	}

	return bSuccess;
}

UBOOL appSaveArrayToFile(const vector<BYTE>& Buffer, const TCHAR* Filename, FFileManager* FileManager)
{
	FArchive* Writer = FileManager->CreateFileWriter(Filename);
	if( !Writer )
	{
		return FALSE;
	}

	Writer->Serialize((void*)(&Buffer), Buffer.size());
	UBOOL Success = Writer->Close();
	delete Writer;

	return Success;
}

/*-----------------------------------------------------------------------------
	Global init and exit.
-----------------------------------------------------------------------------*/
void appInit(const TCHAR* strCmdLine, FOutputDeviceFile* InLogFile, FOutputDeviceConsole* InLogConsole, FFileManager* InFileManager)
{
	FName::StaticInit();

	appStrncpy(GCmdLine, strCmdLine, ARRAY_COUNT(GCmdLine));

	appPlatformPreInit();

	appInitTiming();

	GFileManager = InFileManager;

	GLogConsole = InLogConsole;
	GLog->AddOutputDevice(InLogFile);
	GLog->AddOutputDevice(InLogConsole);

	if( ParseParam(GCmdLine, TEXT("GIsBuildMachine")) )
	{
		GIsBuildMachine = TRUE;
	}

	if( appIsDebuggerPresent() || GIsBuildMachine )
	{
		GLog->AddOutputDevice(new FOutputDeviceDebug());
	}

	if( GLogConsole && appStrstr(appCmdLine(), TEXT("log")) )
	{
		GLogConsole->Show(TRUE);
	}

	appPlatformInit();
}

void appPreExit()
{
	debugf(NAME_Exit, TEXT("Preparing to exit..."));
	if( GThreadPool != NULL )
	{
		GThreadPool->Destroy();
	}
}

void appExit()
{
	debugf(NAME_Exit, TEXT("Exiting..."));
	GLog->Shutdown();
	GLog = NULL;
}

const TCHAR* RemoveExeName(const TCHAR* strCmdLine)
{
	if( *strCmdLine=='\"' )
	{
		strCmdLine++;
		while( *strCmdLine && *strCmdLine!='\"' )
		{
			strCmdLine++;
		}
		if( *strCmdLine )
		{
			strCmdLine++;
		}
	}
	while( *strCmdLine && *strCmdLine!=' ' )
	{
		strCmdLine++;
	}
	// skip over any spaces at the start, which Vista likes to toss in multiple
	while (*strCmdLine == ' ')
	{
		strCmdLine++;
	}
	return strCmdLine;
}