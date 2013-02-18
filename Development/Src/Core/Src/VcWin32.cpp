/**	@brief : VcWin32.cpp
 *	@author : xukun
 *	@data : 08.04.20
 */

#include "CorePCH.h"

#include <stdio.h>
#include "objbase.h"

/*----------------------------------------------------------------------------
	Output Debug Message
----------------------------------------------------------------------------*/
int MessageBoxExt( EAppMsgType MsgType, HWND HandleWnd, const TCHAR* Text, const TCHAR* Caption );

UBOOL CALLBACK MessageBoxDlgProc( HWND HandleWnd, UINT Message, WPARAM WParam, LPARAM LParam );

void appRequestExit(UBOOL Force)
{
	if(Force)
	{
		ExitProcess(1);
	}
	else
	{
		PostQuitMessage(0);
		GIsRequestingExit = TRUE;
	}
}

void appOutputDebugString(const TCHAR* Message)
{
	OutputDebugString(Message);
}

void appOutputDebugStringf( const TCHAR *Format, ... )
{
	TCHAR TempStr[4096];
	GET_VARARGS( TempStr, ARRAY_COUNT(TempStr), ARRAY_COUNT(TempStr)-1, Format, Format );
	appOutputDebugString( TempStr );
}

void appFailAssertDebug(const ANSICHAR* Expr, const ANSICHAR* File, INT Line, const TCHAR* Format, ... )
{
	TCHAR TempStr[4096];
	GET_VARARGS( TempStr, ARRAY_COUNT(TempStr), ARRAY_COUNT(TempStr) - 1, Format, Format );
	appOutputDebugStringf(TEXT("%s(%i): Assertion failed: %s\n%s\n"), ANSI_TO_TCHAR(File), Line, ANSI_TO_TCHAR(Expr), TempStr);
}

void appDebugMessagef(const TCHAR* Fmt, ...)
{
	TCHAR TempStr[4096];
	GET_VARARGS(TempStr, ARRAY_COUNT(TempStr), ARRAY_COUNT(TempStr)-1, Fmt, Fmt);
	MessageBox(NULL, TempStr, TEXT("appDebugMessagef"), MB_OK|MB_SYSTEMMODAL);
}

void appGetLastError()
{
	TCHAR TempStr[MAX_SPRINTF]=TEXT("");
	appSprintf( TempStr, TEXT("GetLastError : %d\n"), GetLastError());
	MessageBox( NULL, TempStr, TEXT("System Error"), MB_OK|MB_SYSTEMMODAL );
}

const TCHAR* appGetSystemErrorMessage()
{
	static TCHAR Msg[256];
	*Msg = 0;

	DWORD Err = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, Err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), Msg, 256, NULL);
	if( appStrchr(Msg, '\r') )
	{
		*appStrchr(Msg, '\r') = TEXT('0');
	}
	if( appStrchr(Msg, '\n') )
	{
		*appStrchr(Msg, '\n') = TEXT('0');
	}

	return Msg;
}

/*-----------------------------------------------------------------------------
	Output Debug Message
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
	Timing.
-----------------------------------------------------------------------------*/
void appSystemTime(INT& Year, INT& Month, INT& DayOfWeek, INT& Day, INT& Hour, INT& Min, INT& Sec, INT& MSec)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	Year		= st.wYear;
	Month		= st.wMonth;
	DayOfWeek	= st.wDayOfWeek;
	Day			= st.wDay;
	Hour		= st.wHour;
	Min			= st.wMinute;
	Sec			= st.wSecond;
	MSec		= st.wMilliseconds;
}

FString appSystemTimeString(void)
{
	INT Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec;

	appSystemTime( Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec );

	FString CurrentTime = FStringUtil::Sprintf( TEXT("%i.%02i.%02i-%02i.%02i.%02i"), Year, Month, Day, Hour, Min, Sec );

	return CurrentTime;
}

inline TCHAR* appStrDate( TCHAR* Dest, SIZE_T DestSize )
{
	return (TCHAR*)_tstrdate_s(Dest,DestSize);
}

inline TCHAR* appStrTime( TCHAR* Dest, SIZE_T DestSize )
{
	return (TCHAR*)_tstrtime_s(Dest,DestSize);
}

const TCHAR* appTimestamp()
{
	static TCHAR Result[1024] = TEXT("");
	appStrDate(Result, ARRAY_COUNT(Result));
	appStrcat(Result, 1024, TEXT(" "));
	appStrTime(Result + appStrlen(Result), ARRAY_COUNT(Result) - appStrlen(Result));

	return Result;
}

void appSleep( FLOAT Seconds )
{
	Sleep( (DWORD)(Seconds * 1000.0f) );
}

void appSleepInfinite()
{
	Sleep(INFINITE);
}

/*-----------------------------------------------------------------------------
	Formatted printing and messages.
-----------------------------------------------------------------------------*/
INT appGetVarArgs( TCHAR* Dest, SIZE_T DestSize, INT Count, const TCHAR*& Fmt, va_list ArgPtr)
{
	INT Result = _vsntprintf_s(Dest, DestSize, Count, Fmt, ArgPtr);
	va_end(ArgPtr);
	return Result;
}

INT appGetVarArgsAnsi( ANSICHAR* Dest, SIZE_T DestSize, INT Count, const ANSICHAR*& Fmt, va_list ArgPtr )
{
	INT Result = _vsnprintf_s(Dest, DestSize, Count, Fmt, ArgPtr);
	va_end(ArgPtr);
	return Result;
}

// 确保Dest buffer >= MAX_SPRINTF
INT appSprintf(TCHAR* Dest, const TCHAR* Fmt, ... )
{
	INT Ret;
	GET_VARARGS_RESULT( Dest, MAX_SPRINTF, MAX_SPRINTF - 1, Fmt, Fmt, Ret );
	return Ret;
}

INT appSprintfANSI(char* Dest, const char* Fmt, ... )
{
	INT Ret;
	GET_VARARGS_RESULT_ANSI( Dest, MAX_SPRINTF, MAX_SPRINTF - 1, Fmt, Fmt, Ret );
	return Ret;
}

static BOOL WINAPI ConsoleCtrlHander(DWORD Type)
{
	if( GLog )
	{
		GLog->Flush();
	}

	if( !GIsRequestingExit )
	{
		PostQuitMessage(0);
		GIsRequestingExit = 1;
	}
	else
	{
		ExitProcess(0);
	}
	return TRUE;
}

void appSetConsoleCtrlHandler()
{
	SetConsoleCtrlHandler(ConsoleCtrlHander, TRUE);
}

void appGetEnvironmentVariable(const TCHAR* VariableName, TCHAR* Result, INT ResultLength)
{
	DWORD error = GetEnvironmentVariable(VariableName, Result, ResultLength);
	if( error <= 0 )
	{
		*Result = 0;
	}
}

/*-----------------------------------------------------------------------------
	Guids.
-----------------------------------------------------------------------------*/
FGuid appCreateGuid()
{
	FGuid id(0, 0, 0, 0);
	CoCreateGuid( (GUID*)&id );
	id.CalcCRC();
	return id;
}

// 初始化时的窗口过程
LRESULT CALLBACK StartupWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		// Prevent power management
	case WM_POWERBROADCAST:
		{
			switch( wParam )
			{
			case PBT_APMQUERYSUSPEND:
			case PBT_APMQUERYSTANDBY:
				return BROADCAST_QUERY_DENY;
			}
		}
	}

	return DefWindowProc(hWnd, Message, wParam, lParam);
}

extern INT GGameIcon;

extern HWND GGameWindow;

extern UBOOL GGameWindowUsingStartupWindowProc;

/** Width of the primary monitor, in pixels. */
extern INT GPrimaryMonitorWidth;
/** Height of the primary monitor, in pixels. */
extern INT GPrimaryMonitorHeight;
/** Rectangle of the work area on the primary monitor (excluding taskbar, etc) in "virtual screen coordinates" (pixels). */
extern RECT GPrimaryMonitorWorkRect;
/** Virtual screen rectangle including all monitors. */
extern RECT GVirtualScreenRect;

void appPlatformPreInit()
{
	// Check Windows version.
	OSVERSIONINFOEX OsVersionInfo = { 0 };
	OsVersionInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );
	GetVersionEx( ( LPOSVERSIONINFO )&OsVersionInfo );

	// Get the total screen size of the primary monitor.
	GPrimaryMonitorWidth = ::GetSystemMetrics( SM_CXSCREEN );
	GPrimaryMonitorHeight = ::GetSystemMetrics( SM_CYSCREEN );
	GVirtualScreenRect.left = ::GetSystemMetrics( SM_XVIRTUALSCREEN );
	GVirtualScreenRect.top = ::GetSystemMetrics( SM_YVIRTUALSCREEN );
	GVirtualScreenRect.right = ::GetSystemMetrics( SM_CXVIRTUALSCREEN );
	GVirtualScreenRect.bottom = ::GetSystemMetrics( SM_CYVIRTUALSCREEN );

	// Get the screen rect of the primary monitor, exclusing taskbar etc.
	SystemParametersInfo( SPI_GETWORKAREA, 0, &GPrimaryMonitorWorkRect, 0 );
}

void appPlatformInit()
{
	if( GIsBenchmarking )
	{
		srand(0);
	}
	else
	{
		srand(time(NULL));
	}
}

void appPlatformPostInit()
{
	WNDCLASSEX Cls;
	appMemzero(&Cls, sizeof(Cls));
	Cls.cbSize = sizeof(Cls);
	Cls.style = CS_OWNDC;
	Cls.hInstance = hInstance;
	Cls.lpfnWndProc = StartupWindowProc;
	Cls.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(GGameIcon));
	Cls.hCursor = ::LoadCursor(hInstance, MAKEINTRESOURCE(GGameIcon));
	Cls.hIconSm = ::LoadIcon(hInstance, MAKEINTRESOURCE(GGameIcon));
	Cls.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);

	FString WindowClassName = FString(TEXT("VREngine")) + FString(TEXT("VWindowClient"));
	Cls.lpszClassName = WindowClassName.c_str();

	INT ret = RegisterClassEx(&Cls);


	DWORD WindowStyle;
	WindowStyle = WS_VISIBLE | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_BORDER | WS_CAPTION;
	INT ScreenWidth  = GPrimaryMonitorWidth;
	INT ScreenHeight = GPrimaryMonitorHeight;
	INT WindowWidth  = ScreenWidth / 2;
	INT WindowHeight = ScreenHeight / 2;
	INT WindowPosX = (ScreenWidth - WindowWidth ) / 2;
	INT WindowPosY = (ScreenHeight - WindowHeight ) / 2;

	GGameWindowUsingStartupWindowProc = TRUE;
	GGameWindow = CreateWindowEx(WS_EX_APPWINDOW, WindowClassName.c_str(), TEXT("VR_APP"), WindowStyle, WindowPosX, WindowPosY, WindowWidth, WindowHeight, NULL, NULL, hInstance, NULL);
	ShowWindow(GGameWindow, SW_SHOWMINIMIZED);
}

// pump window message
void appWinPumpMessages()
{
	MSG Msg;
	while( PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE) )
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

/**
*	Processes sent window messages only.
*/
void appWinPumpSentMessages()
{
	MSG Msg;
	PeekMessage(&Msg,NULL,0,0,PM_NOREMOVE | PM_QS_SENDMESSAGE);
}

//void appShowGameWindow()
//{
//	if( GGameWindow )
//	{
//
//	}
//}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/
//const TCHAR* appBaseDir()
//{
//	static TCHAR Result[512]=TEXT("");
//	if( !Result[0] )
//	{
//		// Get directory this executable was launched from.
//#if UNICODE
//		if( GUnicode )
//		{
//			ANSICHAR ACh[256];
//			GetModuleFileNameA( NULL, ACh, 256 );
//			MultiByteToWideChar( CP_ACP, 0, ACh, -1, Result, 512 );
//		}
//		else
//#endif
//		{
//			GetModuleFileName( NULL, Result, 512 );
//		}
//		INT StringLength = appStrlen(Result);
//
//		if(StringLength > 0)
//		{
//			--StringLength;
//			for(; StringLength > 0; StringLength-- )
//			{
//				if( Result[StringLength - 1] == PATH_SEPARATOR[0] || Result[StringLength - 1] == '/' )
//				{
//					break;
//				}
//			}
//		}
//		Result[StringLength] = 0;
//	}
//	return Result;
//}