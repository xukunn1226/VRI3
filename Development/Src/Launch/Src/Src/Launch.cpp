#include "LaunchPCH.h"
#include "DbgHelp.h"

//HINSTANCE hInstance = NULL;
//HWND GGameWindow = NULL;
//INT		GGameIcon = 131/*IDI_ICON_GameDemo*/;
//UBOOL GGameWindowUsingStartupWindowProc = FALSE;

FEngineLoop		GEngineLoop;
FFPSCounter		GFPSCounter;

extern INT CreateMiniDump( LPEXCEPTION_POINTERS ExceptionInfo )
{
	HANDLE FileHandle = CreateFileW(TEXT(""), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// Write a minidump.
	if( FileHandle != INVALID_HANDLE_VALUE )
	{
		MINIDUMP_EXCEPTION_INFORMATION DumpExceptionInfo;

		DumpExceptionInfo.ThreadId			= GetCurrentThreadId();
		DumpExceptionInfo.ExceptionPointers	= ExceptionInfo;
		DumpExceptionInfo.ClientPointers	= true;

		MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), FileHandle, MiniDumpWithIndirectlyReferencedMemory, &DumpExceptionInfo, NULL, NULL );
		CloseHandle( FileHandle );
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

static INT GameMain(const TCHAR* CmdLine, HINSTANCE hInInstance, HINSTANCE hPrevInstance, INT nCmdShow)
{
	CmdLine = RemoveExeName(CmdLine);

	INT Error = GEngineLoop.PreInit(CmdLine);
	if( Error != 0 || GIsRequestingExit )
	{
		return Error;
	}

	Error = GEngineLoop.Init();

	// for test
	if(0)
	{
		INT NumBytes = Align(10, 16);
		NumBytes = Align(17, 16);

		FD3D9Texture2DPtr Tex = GDynamicDrv->CreateTexture2D(1, 1, PF_A8R8G8B8, 1, 0);
		
		LPDIRECT3DSURFACE9				g_pSurf = NULL;
		Tex->GetD3DResource()->GetSurfaceLevel(0, &g_pSurf);
		g_pSurf->Release();

		IDirect3DSurface9* DestinationSurface;
		//LPDIRECT3DSURFACE9				g_pSurf1 = NULL;
		(*Tex).GetD3DResource()->GetSurfaceLevel(0, &DestinationSurface);
		FD3D9SurfacePtr Surface = new FD3D9Surface(NULL, DestinationSurface);
		Surface = NULL;
		//DestinationSurface.GetReference()->Release();

		//Tex = NULL;
	}




	while( !GIsRequestingExit )
	{
		GFPSCounter.Update(appSeconds());

		GEngineLoop.Tick();
	}

	GEngineLoop.Exit();

	return Error;
}

INT WINAPI WinMain( HINSTANCE hInInstance, HINSTANCE hPrevInstance, char*, INT nCmdShow )
{
	appStrcpy(GGameName, TEXT("GameDemo"));

	INT Error = 0;
	hInstance = hInInstance;
	const TCHAR* CmdLine = GetCommandLine();

#if defined(_DEBUG)
	if( TRUE )
#else
	if( appIsDebuggerPresent() )
#endif
	{
		Error = GameMain(CmdLine, hInInstance, hPrevInstance, nCmdShow);
	}
	else
	{
		__try
		{
			Error = GameMain(CmdLine, hInInstance, hPrevInstance, nCmdShow);
		}
		__except( CreateMiniDump(GetExceptionInformation()) )
		{
			// Crash
			Error = 1;
			appRequestExit(TRUE);
		}
	}

	appExit();

	return Error;
} 