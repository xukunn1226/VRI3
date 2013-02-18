#include "CorePCH.h"

void FOutputDeviceDebug::Serialize(const TCHAR* Data, EName Event)
{
	TCHAR Temp[MAX_SPRINTF] = TEXT("");
	appSprintf(Temp, TEXT("%s : %s\r\n"), FName::SafeString(Event).c_str(), Data);
	appOutputDebugString(Temp);
}

///////////////////////////// FOutputDeviceFile ////////////////////////
FOutputDeviceFile::FOutputDeviceFile(const TCHAR *InFilename, UBOOL bDisableBackup)
: _LogAr(NULL), _bOpened(FALSE), _bDisableBackup(bDisableBackup)
{
	if( InFilename )
	{
		appStrncpy(_Filename, InFilename, ARRAY_COUNT(_Filename));
	}
	else
	{
		_Filename[0] = 0;
	}
}

void FOutputDeviceFile::Shutdown()
{
	if( _LogAr )
	{
		Logf(NAME_Log, TEXT("Log file closed: %s"), appTimestamp());
		delete _LogAr;
		_LogAr = NULL;
	}
}

void FOutputDeviceFile::Flush()
{
	if( _LogAr )
	{
		_LogAr->Flush();
	}
}

static void CreateBackupCopy(TCHAR* Filename)
{
	if( GFileManager->FileSize(Filename) > 0 )
	{
		FString strSystemTime = appSystemTimeString();
		FString Name, Ext;
		FStringUtil::SplitBaseFilename(Filename, Name, Ext);
		FString BackupFilename = FStringUtil::Sprintf(TEXT("%s%s%s.%s"), Name.c_str(), TEXT("-backup-"), strSystemTime.c_str(), Ext.c_str());
		GFileManager->Copy(BackupFilename.c_str(), Filename, FALSE);
	}
}

void FOutputDeviceFile::Serialize(const TCHAR *Data, EName Event)
{
	if( !_LogAr )
	{
		if( !_Filename[0] )
		{
			appStrcpy(_Filename, appBaseDir());
			appStrcat(_Filename, appGameLogDir());
			appStrcat(_Filename, TEXT("Launch.log"));
		}

		if( !_bDisableBackup && !_bOpened )
		{
			CreateBackupCopy(_Filename);
		}

		_LogAr = GFileManager->CreateFileWriter(_Filename, EFW_AllowRead | (_bOpened ? EFW_Append : 0));

		if( _LogAr )
		{
			_bOpened = TRUE;
#if _UNICODE
			WORD UnicodeBOM = UNICODE_BOM;
			_LogAr->Serialize(&UnicodeBOM, 2);
#endif
			Logf(NAME_Log, TEXT("Log file open: %s"), appTimestamp());
		}
	}

	if( _LogAr )
	{
		TCHAR TCH[1024] = TEXT("");
		appStrcat(TCH, FName::SafeString(Event).c_str());
		appStrcat(TCH, TEXT(": "));

		//INT i = 0;
		//for( i = 0; TCH[i]; ++i );
		//_LogAr->Serialize(TCH, i * sizeof(TCHAR));
		_LogAr->Serialize(&TCH[0], appStrlen(&TCH[0]) * sizeof(TCHAR));

		//for( i = 0; Data[i]; ++i );
		//_LogAr->Serialize((void*)Data, i * sizeof(TCHAR));
		_LogAr->Serialize(const_cast<TCHAR*>(&Data[0]), appStrlen(&Data[0]) * sizeof(TCHAR));

		_LogAr->Serialize(TEXT("\r\n"), 2 * sizeof(TCHAR));

		_LogAr->Flush();
	}
}

///////////////////////////// FOutputDeviceRedirector ////////////////////////
FOutputDeviceRedirector::FOutputDeviceRedirector()
: _MasterThreadID(appGetCurrentThreadId())
{}

void FOutputDeviceRedirector::AddOutputDevice(FOutputDevice *OutputDevice)
{
	FScopeLock Lock(&_SyncObject);

	vector<FOutputDevice*>::iterator it;
	it = find(_OutputDevices.begin(), _OutputDevices.end(), OutputDevice);
	if( it == _OutputDevices.end() )
	{
		_OutputDevices.push_back(OutputDevice);
	}
}

void FOutputDeviceRedirector::RemoveOutputDevice(FOutputDevice *OutputDevice)
{
	FScopeLock Lock(&_SyncObject);

	vector<FOutputDevice*>::iterator it;
	it = find(_OutputDevices.begin(), _OutputDevices.end(), OutputDevice);
	if( it != _OutputDevices.end() )
	{
		_OutputDevices.erase(it);
	}
}

UBOOL FOutputDeviceRedirector::IsRedirectTo(FOutputDevice *OutputDevice)
{
	FScopeLock Lock(&_SyncObject);

	UBOOL FindIt = TRUE;

	vector<FOutputDevice*>::iterator it;
	it = find(_OutputDevices.begin(), _OutputDevices.end(), OutputDevice);
	if( it == _OutputDevices.end() )
	{
		FindIt = FALSE;
	}

	return FindIt;
}

// Flush其他线程的LOG
void FOutputDeviceRedirector::UnsyncFlushThreadedLogs()
{
	for(INT LineIndex = 0; LineIndex < _BufferedLines.size(); ++LineIndex)
	{
		const FBufferedLine& BuferedLine = _BufferedLines.at(LineIndex);
		for(INT DeviceIndex = 0; DeviceIndex < _OutputDevices.size(); ++DeviceIndex)
		{
			_OutputDevices.at(DeviceIndex)->Serialize(BuferedLine.Data.c_str(), BuferedLine.Event);
		}
	}
	_BufferedLines.clear();
}

void FOutputDeviceRedirector::FlushThreadedLogs()
{
	FScopeLock Lock(&_SyncObject);
	UnsyncFlushThreadedLogs();
}

void FOutputDeviceRedirector::Serialize(const TCHAR *Data, EName Event)
{
	FScopeLock Lock(&_SyncObject);

	// 非主线程或输出设备没有时，缓存LOG
	if( appGetCurrentThreadId() != _MasterThreadID
		|| _OutputDevices.size() == 0 )
	{
		_BufferedLines.push_back(FBufferedLine(Data, Event));
	}
	else
	{
		UnsyncFlushThreadedLogs();
		for(INT DeviceIndex = 0; DeviceIndex < _OutputDevices.size(); ++DeviceIndex)
		{
			_OutputDevices.at(DeviceIndex)->Serialize(Data, Event);
		}
	}
}

void FOutputDeviceRedirector::Flush()
{
	if( appGetCurrentThreadId() == _MasterThreadID )
	{
		FScopeLock Lock(&_SyncObject);
		for(INT DeviceIndex = 0; DeviceIndex < _OutputDevices.size(); ++DeviceIndex)
		{
			_OutputDevices.at(DeviceIndex)->Flush();
		}
	}
}

void FOutputDeviceRedirector::Shutdown()
{
	check(appGetCurrentThreadId() == _MasterThreadID);

	FScopeLock Lock(&_SyncObject);
	UnsyncFlushThreadedLogs();
	for(INT DeviceIndex = 0; DeviceIndex < _OutputDevices.size(); ++DeviceIndex)
	{
		_OutputDevices.at(DeviceIndex)->Shutdown();
	}
	_OutputDevices.clear();
}

void FOutputDeviceRedirector::SetCurrentThreadAsMasterThread()
{
	FlushThreadedLogs();
	_MasterThreadID = appGetCurrentThreadId();
}

///////////////////////////// FOutputDeviceConsole ////////////////////////
FOutputDeviceConsole::FOutputDeviceConsole()
{}

FOutputDeviceConsole::~FOutputDeviceConsole()
{}

void FOutputDeviceConsole::Show(UBOOL bShow)
{
	if( bShow )
	{
		if( !ConsoleHandle )
		{
			AllocConsole();
			ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

			if( ConsoleHandle )
			{
				COORD Size;
				Size.X = 600;
				Size.Y = 1400;

				SetConsoleScreenBufferSize(ConsoleHandle, Size);

				RECT WindowRect;
				::GetWindowRect(GetConsoleWindow(), &WindowRect);

				::SetWindowPos(GetConsoleWindow(), HWND_TOP, WindowRect.left, WindowRect.top, 0, 0, SWP_NOSIZE | SWP_NOSENDCHANGING | SWP_NOZORDER);

				appSetConsoleCtrlHandler();
			}
		}
	}
	else if( ConsoleHandle )
	{
		ConsoleHandle = NULL;
		FreeConsole();
	}
}

UBOOL FOutputDeviceConsole::IsShow()
{
	return ConsoleHandle != NULL;
}

void FOutputDeviceConsole::Serialize(const TCHAR *Data, EName Event)
{
	if( ConsoleHandle )
	{
		if( Event == NAME_Color )
		{
			TCHAR ColorFlag[10];
			appMemset(ColorFlag, 0, sizeof(TCHAR) * ARRAY_COUNT(ColorFlag));
			appStrncpy(ColorFlag, Data, ARRAY_COUNT(ColorFlag));
			for(TCHAR* S = ColorFlag; *S; ++S)
			{
				*S -= '0';
			}

			SetConsoleTextAttribute(ConsoleHandle,
				(ColorFlag[0] ? FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED	: 0) |
				(ColorFlag[1] ? FOREGROUND_RED			: 0) |
				(ColorFlag[2] ? FOREGROUND_GREEN		: 0) |
				(ColorFlag[3] ? FOREGROUND_BLUE			: 0) |
				(ColorFlag[4] ? FOREGROUND_INTENSITY	: 0) |
				(ColorFlag[5] ? BACKGROUND_RED			: 0) |
				(ColorFlag[6] ? BACKGROUND_GREEN		: 0) |
				(ColorFlag[7] ? BACKGROUND_BLUE			: 0) |
				(ColorFlag[8] ? BACKGROUND_INTENSITY	: 0) );
		}
		else
		{
			TCHAR TCH[1024] = TEXT("");
			appStrcat(TCH, FName::SafeString(Event).c_str());
			appStrcat(TCH, TEXT(": "));

			TCHAR OutputString[MAX_SPRINTF] = TEXT("");
			appSprintf(OutputString, TEXT("%s: %s%s"), FName::SafeString(Event).c_str(), Data, TEXT("\r\n"));

			CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
			SMALL_RECT	WindowRect;

			if( GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleInfo) && ConsoleInfo.srWindow.Top > 0 )
			{
				WindowRect.Top = -1;
				WindowRect.Bottom = -1;
				WindowRect.Left = 0;
				WindowRect.Right = 0;

				SetConsoleWindowInfo(ConsoleHandle, FALSE, &WindowRect);
			}

			CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
			WORD OldColorAttrs;
			if( Event == NAME_Warning )
			{
				GetConsoleScreenBufferInfo(ConsoleHandle, &csbiInfo);
				OldColorAttrs = csbiInfo.wAttributes;
				SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

				DWORD Written;
				WriteConsole(ConsoleHandle, OutputString, appStrlen(OutputString), &Written, NULL);

				SetConsoleTextAttribute(ConsoleHandle, OldColorAttrs);
			}
			else if( Event == NAME_Error )
			{
				GetConsoleScreenBufferInfo(ConsoleHandle, &csbiInfo);
				OldColorAttrs = csbiInfo.wAttributes;
				SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);

				DWORD Written;
				WriteConsole(ConsoleHandle, OutputString, appStrlen(OutputString), &Written, NULL);

				SetConsoleTextAttribute(ConsoleHandle, OldColorAttrs);
			}
			else
			{
				DWORD Written;
				WriteConsole(ConsoleHandle, OutputString, appStrlen(OutputString), &Written, NULL);
			}

		}
	}
}
