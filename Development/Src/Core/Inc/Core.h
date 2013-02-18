/**	@brief : Core.h  setup about platform and system
 *	@author : xukun
 *	@data : 08.04.18
 */

#ifndef VRI3_CORE_CORE_H_
#define VRI3_CORE_CORE_H_

#include "Build.h"

#if _MSC_VER
	#define CHECK_AT_COMPILE_TIME(expr, msg) typedef char COMPILE_ERROR_##msg[1][(expr)]
#endif

#if _MSC_VER 
	#include "VcWin32.h"
#endif

// Global constants.
enum {MAXBYTE		= 0xff       };
enum {MAXWORD		= 0xffffU    };
enum {MAXDWORD		= 0xffffffffU};
enum {MAXSBYTE		= 0x7f       };
enum {MAXSWORD		= 0x7fff     };
enum {MAXINT		= 0x7fffffff };
enum {UNICODE_BOM	= 0xfeff	 };
enum {INDEX_NONE	= -1         };

// Aspect ratio constants
const FLOAT AspectRatio4x3 = 1.33333f;
const FLOAT AspectRatio5x4 = 1.25f;
const FLOAT AspectRatio16x9 = 1.77778f;
const FLOAT InvAspectRatio4x3 = 0.75f;
const FLOAT InvAspectRatio5x4 = 0.8f;
const FLOAT InvAspectRatio16x9 = 0.56249f;

#ifdef UNICODE

    #ifndef _TEXT_DEFINED
	#undef TEXT
	#define TEXT(s) L##s
    #endif

	inline TCHAR    FromAnsi   ( ANSICHAR In ) { return (BYTE)In;                        }
	inline TCHAR    FromUnicode( UNICHAR In  ) { return In;                              }
	inline ANSICHAR ToAnsi     ( TCHAR In    ) { return (WORD)In<0x100 ? In : MAXSBYTE;  }
	inline UNICHAR  ToUnicode  ( TCHAR In    ) { return In;                              }

#else
	#undef TEXT
	#define TEXT(s) s

	inline TCHAR    FromAnsi   ( ANSICHAR In ) { return In;								 }
	inline TCHAR    FromUnicode( UNICHAR In  ) { return (WORD)In<0x100 ? In : MAXSBYTE;  }
	inline ANSICHAR ToAnsi     ( TCHAR In    ) { return In;								 }
	inline UNICHAR  ToUnicode  ( TCHAR In    ) { return (BYTE)In;                        }

#endif

#ifdef _DEBUG
	#if defined _MSC_VER
		#define appIsDebuggerPresent	IsDebuggerPresent
		#define appDebugBreak()			(IsDebuggerPresent() ? (DebugBreak(), 1) : 1)
	#endif
#else
	#if defined _MSC_VER
		#define appIsDebuggerPresent	FALSE
		#define appDebugBreak()			1
	#endif
#endif

#define ARRAY_COUNT( array ) \
	( sizeof(array) / sizeof((array)[0]) )

/*-----------------------------------------------------------------------------
	Logging and critical errors.
-----------------------------------------------------------------------------*/
void appRequestExit(UBOOL Force);

void appOutputDebugString(const TCHAR* Message);

void appOutputDebugStringf( const TCHAR *Format, ... );

void appFailAssertDebug(const ANSICHAR* Expr, const ANSICHAR* File, INT Line, const TCHAR* Format = TEXT(""), ... );

#define appFailAssert(expr, file, line, ...)	{ if(appIsDebuggerPresent()) appFailAssertDebug(expr, file, line, ##__VA_ARGS__); appDebugBreak(); }

#define appErrorf(...)	(( appIsDebuggerPresent() ? appFailAssertDebug("appErrorf", __FILE__, __LINE__, ##__VA_ARGS__),1 : 1), appDebugBreak())

// 以对话框形式提示自定义的错误信息
void appDebugMessagef(const TCHAR* Fmt, ...);

// 以对话框形式提示错误码
void appGetLastError();

const TCHAR* appGetSystemErrorMessage();

// std includes
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <algorithm>

using namespace std;


#if UNICODE
typedef basic_string<wchar_t>	FString;
#else
typedef basic_string<char>		FString;
#endif

#if NO_LOG || FINAL_RELEASE
	#define debugf		__noop
#else
	#define debugf		GLog->Logf
#endif

/*----------------------------------------------------------------------------
	Forward declarations.
----------------------------------------------------------------------------*/
class VObject;
class FOutputDevice;
class FOutputDeviceError;
class FOutputDeviceConsole;
class FOutputDeviceFile;
class FOutputDeviceRedirectorBase;
class FArchive;
class FFileManager;

/*-----------------------------------------------------------------------------
	Global variables.
-----------------------------------------------------------------------------*/
extern UBOOL							GUnicode;
extern UBOOL							GIsRequestingExit;

extern FOutputDeviceRedirectorBase*		GLog;
extern FOutputDeviceConsole*			GLogConsole;
extern FOutputDeviceFile*				GLogFile;
extern FOutputDevice*					GNullOut;

extern TCHAR							GGameName[64];

extern DOUBLE							GSecondsPerCycle;
extern QWORD							GNumTimingCodeCalls;
extern class FGlobalMath				GMath;
extern map<UINT, FString>				GNames;

extern FFileManager*					GFileManager;

extern DOUBLE							GStartTime;
extern DOUBLE							GCurrentTime;
extern DOUBLE							GLastTime;
extern DOUBLE							GDeltaTime;
extern DOUBLE							GFixedDeltaTime;
extern QWORD							GFrameCounter;

extern UBOOL							GIsBenchmarking;

extern UINT								GNumHardwareThreads;

extern DWORD							GGameThreadId;

extern UBOOL							GIsBuildMachine;

extern class FConfigSystem				GConfigSystem;


#include "NameDefine.h"


// normal includes
#include "RefCounting.h"
#include "FString.h"
#include "Template.h"
#include "File.h"
#include "FNames.h"
#include "ThreadBase.h"
#include "OutputDevice.h"
#include "OutputDeviceImpl.h"
#include "Archive.h"
#include "ArchiveImpl.h"
#include "FileSystem.h"
#include "FMath.h"
#include "Color.h"
#include "Cast.h"
#include "Stats.h"
#include "VObject.h"
#include "VPackage.h"
#include "ConfigSystem.h"

class FGuid
{
public:
	DWORD A, B, C, D;
	DWORD Crc;

	FGuid(): Crc(0) {}
	FGuid(DWORD InA, DWORD InB, DWORD InC, DWORD InD)
		: A(InA), B(InB), C(InC), D(InD), Crc(0)
	{
		Crc = appMemCrc(this, sizeof(DWORD) * 4);
	}

	void CalcCRC()
	{
		Crc = appMemCrc(this, sizeof(DWORD) * 4);
	}

	UBOOL IsValid() const
	{
		return ( A | B | C | D ) != 0;
	}

	void Invalidate()
	{
		A = B = C = D = 0;
	}

	friend UBOOL operator==(const FGuid& X, const FGuid& Y)
	{
		return ((X.A ^ Y.A) | (X.B ^ Y.B) | (X.C ^ Y.C) | (X.D ^ Y.D)) == 0;
	}

	friend UBOOL operator!=(const FGuid& X, const FGuid& Y)
	{
		return ((X.A ^ Y.A) | (X.B ^ Y.B) | (X.C ^ Y.C) | (X.D ^ Y.D)) != 0;
	}

	FGuid& operator=(const FGuid& Id)
	{
		A = Id.A; B = Id.B; C = Id.C; D = Id.D;	
		Crc = Id.Crc;
		return *this;
	}

	bool operator<(const FGuid& Ref) const
	{
		check( Crc != 0 && Ref.Crc != 0 );

		if( GetTypeHash() < Ref.GetTypeHash() )
			return TRUE;

		return FALSE;
	}

	DWORD& operator[]( INT Index )
	{
		check(Index>=0);
		check(Index<4);
		switch(Index)
		{
		case 0: return A;
		case 1: return B;
		case 2: return C;
		case 3: return D;
		}

		return A;
	}

	const DWORD& operator[]( INT Index ) const
	{
		check(Index>=0);
		check(Index<4);
		switch(Index)
		{
		case 0: return A;
		case 1: return B;
		case 2: return C;
		case 3: return D;
		}

		return A;
	}

	DWORD GetTypeHash() const
	{
		check(Crc != 0);
		return Crc;
	}

	friend FArchive& operator<<( FArchive& Ar, FGuid& G )
	{
		Ar << G.A << G.B << G.C << G.D;
		if( Ar.IsLoading() )
		{
			G.Crc = appMemCrc(&G, sizeof(DWORD) * 4);
		}
		return Ar;
	}

	FString String() const
	{
		return FStringUtil::Sprintf( TEXT("%08X%08X%08X%08X"), A, B, C, D );
	}
};


#endif