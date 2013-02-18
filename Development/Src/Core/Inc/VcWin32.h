/**	@brief : VcWin32.h
 *	@author : xukun
 *	@data : 08.04.18
 */

#ifndef VRI3_CORE_VCWIN32_H_
#define VRI3_CORE_VCWIN32_H_

#if !_WINDOWS
#error _WINDOWS not defined
#endif

// 编译器最低需求
CHECK_AT_COMPILE_TIME(_MSC_VER >= 1500 && _MSC_FULL_VER >= 150030729, VisualStudio2008_SP1_Required);

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

/*----------------------------------------------------------------------------
	Platform specifics types and defines.
----------------------------------------------------------------------------*/
// Undo any Windows defines.
#undef BYTE
#undef WORD
#undef DWORD
#undef INT
#undef FLOAT
#undef MAXBYTE
#undef MAXWORD
#undef MAXDWORD
#undef MAXINT

// Make sure HANDLE is defined.
#ifndef _WINDOWS_
	#define HANDLE void*
	#define HINSTANCE void*
#endif

#define FORCEINLINE		__forceinline

// Unsigned base types.
typedef unsigned __int8		BYTE;		// 8-bit  unsigned.
typedef unsigned __int16	WORD;		// 16-bit unsigned.
typedef unsigned __int32	UINT;		// 32-bit unsigned.
typedef unsigned long		DWORD;		// defined in windows.h

typedef unsigned __int64	QWORD;		// 64-bit unsigned.

// Signed base types.
typedef	signed __int8		SBYTE;		// 8-bit  signed.
typedef signed __int16		SWORD;		// 16-bit signed.
typedef signed __int32 		INT;		// 32-bit signed.
typedef long				LONG;		// defined in windows.h

typedef signed __int64		SQWORD;		// 64-bit signed.

// Character types.
typedef char				ANSICHAR;	// An ANSI character. normally a signed type.
typedef wchar_t				UNICHAR;	// A unicode character. normally a signed type.
typedef wchar_t				WCHAR;		// defined in windows.h

// Other base types.
typedef UINT				UBOOL;		// Boolean 0 (false) or 1 (true).
typedef float				FLOAT;		// 32-bit IEEE floating point.
typedef double				DOUBLE;		// 64-bit IEEE double.

#ifdef _WIN64
typedef SQWORD				PTRINT;
typedef QWORD				UPTRINT;
#else
typedef INT					PTRINT;		// Integer large enough to hold a pointer.
typedef UINT				UPTRINT;
#endif

// Variable arguments.
INT appGetVarArgs( TCHAR* Dest, SIZE_T DestSize, INT Count, const TCHAR*& Fmt, va_list ArgPtr );
INT appGetVarArgsAnsi( ANSICHAR* Dest, SIZE_T DestSize, INT Count, const ANSICHAR*& Fmt, va_list ArgPtr );

#define GET_VARARGS(msg,msgsize,len,lastarg,fmt) { va_list ap; va_start(ap,lastarg);appGetVarArgs(msg,msgsize,len,fmt,ap); }
#define GET_VARARGS_ANSI(msg,msgsize,len,lastarg,fmt) { va_list ap; va_start(ap,lastarg);appGetVarArgsAnsi(msg,msgsize,len,fmt,ap); }
#define GET_VARARGS_RESULT(msg,msgsize,len,lastarg,fmt,result) { va_list ap; va_start(ap,lastarg); result = appGetVarArgs(msg,msgsize,len,fmt,ap); }
#define GET_VARARGS_RESULT_ANSI(msg,msgsize,len,lastarg,fmt,result) { va_list ap; va_start(ap,lastarg); result = appGetVarArgsAnsi(msg,msgsize,len,fmt,ap); }

#define MAX_SPRINTF 1024
INT appSprintf( TCHAR* Dest, const TCHAR* Fmt, ... );
INT appSprintfANSI( char* Dest, const char* Fmt, ... );

// some warning to disable
#pragma warning(disable : 4244)			// convert to float, possible lose of data
#pragma warning(disable : 4100)			// unreferenced formal parameter										
#pragma warning(disable : 4189)
#pragma warning(disable : 4355)			// this used in base initializer list                                   
#pragma warning(disable : 4127)			// Conditional expression is constant									
#pragma warning(disable : 4018)

// String conversion classes
#include "StringConv.h"

/**
 *	字符转换宏生存周期很短（具体见实现），只能作为函数参数被调用
 *	切记不可：const char* ConvertTo = TCHAR_TO_ANSI(ConvertFrom)
 */
#define TCHAR_TO_ANSI(str) (ANSICHAR*)FTCHARToANSI((const TCHAR*)str)
#define ANSI_TO_TCHAR(str) (TCHAR*)FANSIToTCHAR((const ANSICHAR*)str)

// Strings.
#define LINE_TERMINATOR TEXT("\r\n")
#define PATH_SEPARATOR TEXT("\\")
#define appIsPathSeparator( Ch )	((Ch) == PATH_SEPARATOR[0])

void appSetConsoleCtrlHandler();

void appGetEnvironmentVariable(const TCHAR* VariableName, TCHAR* Result, INT ResultLength);

// Math functions
inline FLOAT appCopySign( FLOAT A, FLOAT B ) { return _copysign(A, B); }
inline FLOAT appExp( FLOAT Value ) { return expf(Value); }
inline FLOAT appLoge( FLOAT Value ) {	return logf(Value); }
inline FLOAT appFmod( FLOAT Y, FLOAT X ) { return fmodf(Y,X); }
inline FLOAT appSin( FLOAT Value ) { return sinf(Value); }
inline FLOAT appAsin( FLOAT Value ) { return asinf( (Value<-1.f) ? -1.f : ((Value<1.f) ? Value : 1.f) ); }
inline FLOAT appCos( FLOAT Value ) { return cosf(Value); }
inline FLOAT appAcos( FLOAT Value ) { return acosf( (Value<-1.f) ? -1.f : ((Value<1.f) ? Value : 1.f) ); }
inline FLOAT appTan( FLOAT Value ) { return tanf(Value); }
inline FLOAT appAtan( FLOAT Value ) { return atanf(Value); }
inline FLOAT appAtan2( FLOAT Y, FLOAT X ) { return atan2f(Y,X); }
inline FLOAT appSqrt( FLOAT Value ) { return sqrt(Value); }
inline FLOAT appInvSqrt(FLOAT Value) { return 1.0f / sqrt(Value); }
inline FLOAT appPow( FLOAT A, FLOAT B ) { return powf(A,B); }
inline UBOOL appIsNaN( FLOAT A ) { return _isnan(A) != 0; }
inline UBOOL appIsFinite( FLOAT A ) { return _finite(A); }
inline INT appFloor( FLOAT F ) { return floorf(F); }
inline INT appCeil( FLOAT Value ) { return ceilf(Value); }
inline INT appRound(FLOAT F) { return appFloor(F+0.5f); }
inline INT appRand() { return rand(); }
inline void appRandInit(INT Seed) { srand( Seed ); }
inline FLOAT appFrand() { return rand() / (FLOAT)RAND_MAX; }
inline FLOAT appFractional( FLOAT Value ) { return Value - appFloor(Value); }


/************************************************************************/
/*	 Timing Functions                                                   */
/************************************************************************/
extern DOUBLE GSecondsPerCycle;
extern DOUBLE GStartTime;

inline DOUBLE appSeconds()
{
	LARGE_INTEGER Cycles;
	QueryPerformanceCounter(&Cycles);

	return Cycles.QuadPart * GSecondsPerCycle;
}

inline DWORD appCycles()
{
	LARGE_INTEGER Cycles;
	QueryPerformanceCounter(&Cycles);
	return Cycles.QuadPart;
}

inline void appInitTiming()
{
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	GSecondsPerCycle = 1.0f / Frequency.QuadPart;
	GStartTime = appSeconds();
}

class FGuid appCreateGuid();

extern void appPlatformPreInit();

extern void appPlatformInit();

extern void appPlatformPostInit();

extern "C"
{
	extern HINSTANCE	hInstance;
}

void appWinPumpMessages();

void appWinPumpSentMessages();

#endif
