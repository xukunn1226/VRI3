#ifndef VRI3_CORE_FILE_H_
#define VRI3_CORE_FILE_H_

enum EAppMsgType
{
	AMT_OK,
	AMT_YesNo,
	AMT_OKCancel,
	AMT_YesNoCancel,
	AMT_CancelRetryContinue,
	AMT_YesNoYesAllNoAll,
	AMT_YesNoYesAllNoAllCancel,
};

enum EAppReturnType
{
	ART_No,
	ART_Yes,
	ART_YesAll,
	ART_NoAll,
	ART_Cancel,
};



/*-----------------------------------------------------------------------------
	Game/ mod specific directories.
-----------------------------------------------------------------------------*/
const TCHAR* appBaseDir();

const TCHAR* appEngineDir();

const TCHAR* appEngineConfigDir();

const TCHAR* appShaderDir();

const TCHAR* appCmdLine();

const TCHAR* appGameDir();

const TCHAR* appGameConfigDir();

const TCHAR* appGameLogDir();

const TCHAR* appGameContentDir();

/*-----------------------------------------------------------------------------
	Timing functions.
-----------------------------------------------------------------------------*/
void appSystemTime( INT& Year, INT& Month, INT& DayOfWeek, INT& Day, INT& Hour, INT& Min, INT& Sec, INT& MSec );

FString appSystemTimeString( void );

const TCHAR* appTimestamp();

void appSleep( FLOAT Seconds );

void appSleepInfinite();

/*-----------------------------------------------------------------------------
	Load&Save String functions.
-----------------------------------------------------------------------------*/
void appBufferToString(FString& String, const BYTE* Buffer, INT Size);

UBOOL appLoadFileToString(FString& Result, const TCHAR* Filename, FFileManager* FileManager = GFileManager);

UBOOL appLoadFileToArray(vector<BYTE>& Buffer, const TCHAR* Filename, FFileManager* FileManager = GFileManager);

UBOOL appSaveStringToFile(const FString& String, const TCHAR* Filename, UBOOL bForceSaveAsAnsi = FALSE, FFileManager* FileManager = GFileManager);

UBOOL appSaveArrayToFile(const vector<BYTE>& Buffer, const TCHAR* Filename, FFileManager* FileManager = GFileManager);

/*-----------------------------------------------------------------------------
Character type functions.
-----------------------------------------------------------------------------*/
#define UPPER_LOWER_DIFF	32

inline TCHAR appToUpper(TCHAR c)
{
	//if( c >= TEXT('a') && c <= TEXT('z'))
	//{
	//	return c - UPPER_LOWER_DIFF;
	//}
	//return c;
	return _totupper(c);
}

inline TCHAR appToLower(TCHAR c)
{
	//if( c >= TEXT('A') && c <= TEXT('Z'))
	//{
	//	return c + UPPER_LOWER_DIFF;
	//}
	//return c;
	return _totlower(c);
}

inline UBOOL appIsUpper(TCHAR c)
{
	//return c >= TEXT('A') && c <= TEXT('Z');
	return _istupper(c) != 0;
}

inline UBOOL appIsLower(TCHAR c)
{
	//return c >= TEXT('a') && c <= TEXT('z');
	return _istlower(c) != 0;
}

inline UBOOL appIsAlpha(TCHAR c)
{
	return (c >= TEXT('A') && c <= TEXT('Z')) || (c >= TEXT('a') && c <= TEXT('z'));
}

inline UBOOL appIsDigit(TCHAR c)
{
	return c >= TEXT('0') && c <= TEXT('9');
}

inline UBOOL appIsAlnum(TCHAR c)
{
	return appIsAlpha(c) || appIsDigit(c);
}

inline UBOOL appIsWhitespace(TCHAR c)
{
	return c == TEXT(' ') || c == TEXT('\t');
}

inline UBOOL appIsLineBreak(TCHAR c)
{
	return c == TEXT('\n');
}

inline UBOOL appIsSpace(TCHAR c)
{
	return _istspace(c) != 0;
}

/** ±êµã·ûºÅ */
inline UBOOL appIsPunct(TCHAR c)
{
	return _istpunct(c) != 0;
}
/*-----------------------------------------------------------------------------
	String functions.
-----------------------------------------------------------------------------*/
inline TCHAR* appStrcpy( TCHAR* Dest, SIZE_T DestCount, const TCHAR* Src )
{
	_tcscpy_s(Dest, DestCount, Src);
	return Dest;
}

template<SIZE_T DestCount>
inline TCHAR* appStrcpy( TCHAR (&Dest)[DestCount], const TCHAR* Src )
{
	return appStrcpy(Dest, DestCount, Src);
}

inline ANSICHAR* appStrcpy( ANSICHAR* Dest, SIZE_T DestCount, const ANSICHAR* Src )
{
	strcpy_s(Dest, DestCount, Src);
	return Dest;
}

template<SIZE_T DestCount>
inline ANSICHAR* appStrcpy( ANSICHAR (&Dest)[DestCount], const ANSICHAR* Src )
{
	return appStrcpy(Dest, DestCount, Src);
}

// append a string
inline TCHAR* appStrcat( TCHAR* Dest, SIZE_T DestCount, const TCHAR* Src )
{
	_tcscat_s(Dest, DestCount, Src);
	return Dest;
}

template<SIZE_T DestCount>
inline TCHAR* appStrcat( TCHAR (&Dest)[DestCount], const TCHAR* Src )
{
	return appStrcat(Dest, DestCount, Src);
}

// convert a string uppercase
inline TCHAR* appStrupr( TCHAR* Dest, SIZE_T DestCount )
{
	_tcsupr_s(Dest, DestCount);
	return Dest;
}

template<SIZE_T DestCount>
inline TCHAR* appStrupr( TCHAR (&Dest)[DestCount] )
{
	return appStrupr(Dest, DestCount);
}

inline ANSICHAR* appStrcpyANSI( ANSICHAR* Dest, SIZE_T DestCount, const ANSICHAR* Src )
{
	strcpy_s(Dest, DestCount, Src);
	return Dest;
}

template<SIZE_T DestCount>
inline ANSICHAR* appStrcpyANSI( ANSICHAR (&Dest)[DestCount], const ANSICHAR* Src )
{
	return appStrcpyANSI(Dest, DestCount, Src);
}

inline ANSICHAR* appStrcatANSI( ANSICHAR* Dest, SIZE_T DestCount, const ANSICHAR* Src )
{
	strcat_s(Dest, DestCount, Src);
	return Dest;
}

template<SIZE_T DestCount>
inline ANSICHAR* appStrcatANSI( ANSICHAR (&Dest)[DestCount], const ANSICHAR* Src )
{
	return appStrcatANSI(Dest, DestCount, Src);
}

inline INT appStrlen( const ANSICHAR* String ) { return strlen( String ); }
inline INT appStrlen( const TCHAR* String ) { return _tcslen( String ); }
inline TCHAR* appStrstr( const TCHAR* String, const TCHAR* Find ) { return (TCHAR*)_tcsstr( String, Find ); }		// Returns a pointer to the first occurrence of a search string in a string.
inline TCHAR* appStrchr( const TCHAR* String, INT c ) { return (TCHAR*)_tcschr( String, c ); }						// Find a character in a string
inline TCHAR* appStrrchr( const TCHAR* String, INT c ) { return (TCHAR*)_tcsrchr( String, c ); }					// Scan a string for the last occurrence of a character.
inline INT appStrcmp( const TCHAR* String1, const TCHAR* String2 ) { return _tcscmp( String1, String2 ); }			// Compare strings
inline INT appStricmp( const ANSICHAR* String1, const ANSICHAR* String2 )  { return _stricmp( String1, String2 ); }
inline INT appStricmp( const TCHAR* String1, const TCHAR* String2 )  { return _tcsicmp( String1, String2 ); }		// Compare strings ( Case insensitive )
inline INT appStrncmp( const TCHAR* A, const TCHAR* B, INT Count ) { return _tcsncmp( A, B, Count ); }				// Compares the first n bytes of two multibyte-character strings
inline INT appStrnicmp( const TCHAR* A, const TCHAR* B, INT Count ) { return _tcsnicmp( A, B, Count ); }			// Compares the first n bytes of two multibyte-character strings ( Case insensitive )
inline INT appAtoi( const TCHAR* String ) { return _tstoi( String ); }
inline FLOAT appAtof( const TCHAR* String ) { return _tstof( String ); }
inline DOUBLE appAtod( const TCHAR* String ) { return _tcstod( String, NULL ); }
inline INT appStrtoi( const TCHAR* Start, TCHAR** End, INT Base ) { return _tcstoul( Start, End, Base ); }

inline TCHAR* appStrncpy( TCHAR* Dest, const TCHAR* Src, INT MaxLen )
{
	_tcsncpy_s(Dest, MaxLen, Src, MaxLen-1);
	return Dest;
}

inline TCHAR* appStrncat( TCHAR* Dest, const TCHAR* Src, INT MaxLen )
{
	INT Len = appStrlen(Dest);
	TCHAR* NewDest = Dest + Len;
	if( (MaxLen-=Len) > 0 )
	{
		appStrncpy( NewDest, Src, MaxLen );
	}
	return Dest;
}

inline ANSICHAR* appStrncpyANSI( ANSICHAR* Dest, const ANSICHAR* Src, INT MaxLen )
{
	return (ANSICHAR*)strncpy_s(Dest,MaxLen,Src,MaxLen-1);
}

extern DWORD GCRCTable[];

/** Case insensitive string hash function. */
inline DWORD appStrihash( const TCHAR* Data )
{
	DWORD Hash=0;
	while( *Data )
	{
		TCHAR Ch = appToUpper(*Data++);
		WORD  B  = Ch;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ GCRCTable[(Hash ^ B) & 0x000000FF];
		B        = Ch>>8;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ GCRCTable[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}

/** Case insensitive string hash function. */
inline DWORD appStrihash( const ANSICHAR* Data )
{
	DWORD Hash=0;
	while( *Data )
	{
		TCHAR Ch = appToUpper(*Data++);
		WORD  B  = Ch;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ GCRCTable[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}

void appItoaAppend( INT InNum,FString &NumberString );

FString appItoa( INT InNum );

/*-----------------------------------------------------------------------------
	Memory functions.
-----------------------------------------------------------------------------*/
inline void* appMemmove( void* Dest, const void* Src, INT Count )
{
	return memmove( Dest, Src, Count );
}

inline INT appMemcmp( const void* Buf1, const void* Buf2, INT Count )
{
	return memcmp( Buf1, Buf2, Count );
}

inline UBOOL appMemIsZero( const void* V, int Count )
{
	BYTE* B = (BYTE*)V;
	while( Count-- > 0 )
		if( *B++ != 0 )
			return 0;
	return 1;
}

inline DWORD appMemCrc(const void* InData, INT Length, DWORD CRC = 0)
{
	BYTE* Data = (BYTE*)InData;
	CRC = ~CRC;
	for( INT i=0; i<Length; i++ )
		CRC = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ Data[i]];
	return ~CRC;
}

#define appMemset( Dest, C, Count )			memset( Dest, C, Count )
	
#define appMemcpy( Dest, Src, Count )		memcpy( Dest, Src, Count )

#define appMemzero( Dest, Count )			memset( Dest, 0, Count )

/*-----------------------------------------------------------------------------
	Parsing functions.
-----------------------------------------------------------------------------*/
inline UBOOL Parse(const TCHAR* Stream, const TCHAR* Match, DWORD& Value)
{
	const TCHAR* Temp = appStrstr(Stream, Match);
	TCHAR* End;
	if( !Temp )
		return FALSE;
	Value = appStrtoi(Temp + appStrlen(Match), &End, 10);

	return TRUE;
}

inline UBOOL ParseParam(const TCHAR* Command, const TCHAR* Find)
{
	TCHAR* Result = appStrstr(Command, Find);
	return Result != NULL;
}

/*-----------------------------------------------------------------------------
	Character type functions.
-----------------------------------------------------------------------------*/
inline UBOOL appIsPureAnsi( const TCHAR* Str )
{
	for( ; *Str; Str++ )
	{
		if( *Str>0xff )
		{
			return 0;
		}
	}
	return 1;
}

inline UBOOL appIsAbsolutePath(const TCHAR* Path)
{
	if( appIsAlpha(Path[0]) && Path[1] == ':' && Path[2] == 0 )
		return TRUE;
	else if( appIsAlpha(Path[0]) && Path[1] == ':' && Path[2] ==  '\\' )
		return TRUE;
	else if( appIsAlpha(Path[0]) && Path[1] == ':' && Path[2] == '/' )
		return TRUE;

	return FALSE;
}

inline UBOOL appIsRelativePath(const TCHAR* Path)
{
	if( Path[0] == '.' && ( Path[1] == '\\' || Path[1] == '/' ) )
		return TRUE;
	else if( Path[0] == '.' && Path[1] == '.' && ( Path[2] == '\\' || Path[2] == '/' ) )
		return TRUE;

	return FALSE;
}

/*-----------------------------------------------------------------------------
	Check macros for assertions.
-----------------------------------------------------------------------------*/
#if DO_CHECK

	#define check(expr)			{ if(!(expr)) appFailAssert(#expr, __FILE__, __LINE__); }
	#define checkf(expr,...)	{ if(!(expr)) appFailAssert(#expr, __FILE__, __LINE__, ##__VA_ARGS__); }
	#define checkMsg(expr,msg)		{ if(!(expr)) {      appFailAssert( #expr " : " #msg , __FILE__, __LINE__ ); }  }

#else

	#define check(expr)			__noop
	#define checkf(expr,...)	__noop
	#define checkMsg(expr,msg)	__noop

#endif

/*-----------------------------------------------------------------------------
	Global init and exit.
-----------------------------------------------------------------------------*/
void appInit(const TCHAR* strCmdLine, FOutputDeviceFile* InLogFile, FOutputDeviceConsole* InLogConsole, FFileManager* InFileManager);
void appExit();
void appPreExit();

const TCHAR* RemoveExeName(const TCHAR* strCmdLine);

#endif