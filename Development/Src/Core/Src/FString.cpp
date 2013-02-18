#include "CorePCH.h"
#include "FString.h"

void FStringUtil::Trim( FString& str, UBOOL bRight, UBOOL bLeft )
{
	const FString delim = TEXT(" \t\r");

	if(bRight)
	{
		str.erase(str.find_last_not_of(delim) + 1);
	}
	if(bLeft)
	{
		str.erase(0, str.find_first_not_of(delim));
	}
}

void FStringUtil::ToLowerCase(FString &str)
{
	transform(str.begin(), str.end(), str.begin(), tolower);
}

void FStringUtil::ToUpperCase(FString &str)
{
	transform(str.begin(), str.end(), str.begin(), toupper);
}

FString FStringUtil::LeftStr(const FString &str, INT Count)
{
	return str.substr(0, Clamp(Count, 0, (INT)str.length()));
}

FString FStringUtil::RightStr(const FString &str, INT Count)
{
	return str.substr(str.length() - Clamp(Count, 0, (INT)str.length()), Count);
}

FString FStringUtil::MidStr(const FString& str, INT Start, INT Count)
{
	DWORD End = Start + Count;
	Start = Clamp((DWORD)Start, (DWORD)0, (DWORD)str.size());
	End = Clamp((DWORD)End, (DWORD)Start, (DWORD)str.size());
	return str.substr(Start, End - Start);
}

INT FStringUtil::FindStr(const FString& Src, const FString &SubStr, INT StartPosition, UBOOL bIgnoreCase)
{
	string::size_type pos;
	if( !bIgnoreCase )
	{
		pos = Src.find(SubStr, Clamp(StartPosition, 0, (INT)Src.length()));
	}
	else
	{
		FString TempSrc(Src);
		FString TempSubStr(SubStr);

		FStringUtil::ToLowerCase(TempSrc);
		FStringUtil::ToLowerCase(TempSubStr);

		pos = TempSrc.find(TempSubStr, Clamp(StartPosition, 0, (INT)TempSrc.length()));
	}

	return pos != string::npos ? (INT)pos : INDEX_NONE;
}

INT FStringUtil::FindStr(const FString& Src, const TCHAR* SubStr, INT StartPosition, UBOOL bIgnoreCase)
{
	return FStringUtil::FindStr(Src, FString(SubStr), StartPosition, bIgnoreCase);
}

UBOOL FStringUtil::StartWith(const FString &Str, const FString &InPrefix)
{
    size_t thisLen = Str.length();
    size_t patternLen = InPrefix.length();
    if (thisLen < patternLen || patternLen == 0)
        return FALSE;

    FString startOfThis = Str.substr(0, patternLen);
    //if (lowerCase)
    //    StringUtil::toLowerCase(startOfThis);

    return (startOfThis == InPrefix);
}

UBOOL FStringUtil::StartWith(const TCHAR *Str, const TCHAR *InPrefix)
{
	return FStringUtil::StartWith(FString(Str), InPrefix);
}

UBOOL FStringUtil::EndWith(const FString &Str, const FString &InPostfix)
{
    size_t thisLen = Str.length();
    size_t patternLen = InPostfix.length();
    if (thisLen < patternLen || patternLen == 0)
        return false;

    FString endOfThis = Str.substr(thisLen - patternLen, patternLen);
    //if (lowerCase)
    //    StringUtil::toLowerCase(endOfThis);

    return (endOfThis == InPostfix);
}

UBOOL FStringUtil::EndWith(const TCHAR *Str, const TCHAR *InPostfix)
{
	return FStringUtil::EndWith(FString(Str), InPostfix);
}

// e.g: FullFilename[C:\MyDir\Test.txt]		Filename[Test.txt]		Path[C:\MyDir\]
void FStringUtil::SplitFilename(const FString &FullFilename, FString &Filename, FString &Path)
{
	FString TempFilename = FullFilename;
	// replace \ with / first
	std::replace(TempFilename.begin(), TempFilename.end(), '\\', '/');

	size_t pos = TempFilename.find_last_of('/');

	if( pos == FString::npos )
	{
		Path.clear();
		Filename = FullFilename;
	}
	else
	{
		Filename = TempFilename.substr( pos + 1, TempFilename.size() - pos - 1 );
		Path = TempFilename.substr(0, pos + 1);
	}
}

// e.g: Fullname[Test.txt]		Filename[Test]		Extension[txt]
void FStringUtil::SplitBaseFilename(const FString& Fullname, FString& Filename, FString& Extension)
{
	size_t pos = Fullname.find_last_of(TEXT("."));
	if( pos == FString::npos )
	{
		Extension.clear();
		Filename = Fullname;
	}
	else
	{
		Extension = Fullname.substr( pos + 1 );
		Filename = Fullname.substr( 0, pos );
	}
}

// e.g: FullFilename[C:\MyDir\Test.txt]		Filename[Test]		Extension[txt]		Path[C:\MyDir\]
void FStringUtil::SplitFilename(const FString &FullFilename, FString &Filename, FString &Extension, FString &Path)
{
	FString Fullname;
	FStringUtil::SplitFilename(FullFilename, Fullname, Path);
	FStringUtil::SplitBaseFilename(Fullname, Filename, Extension);
}

INT FStringUtil::ParseIntoArray(const FString& str, vector<FString>& strArray, const TCHAR* Delim)
{
	strArray.clear();

	INT Index = FStringUtil::FindStr(str, Delim);
	INT DelimLength = appStrlen(Delim);

	FString strItem;
	FString strTemp = str;
	while(Index >= 0)
	{
		strItem = FStringUtil::LeftStr(strTemp, Index);
		strArray.push_back(strItem);
		strTemp = FStringUtil::MidStr(strTemp, Index + DelimLength, strTemp.size());
		Index = FStringUtil::FindStr(strTemp, Delim);
	}

	if( !strTemp.empty() )
	{
		strArray.push_back(strTemp);
	}

	return strArray.size();
}

FString FStringUtil::Sprintf( const TCHAR* Fmt, ... )
{
	TCHAR Dest[MAX_SPRINTF];
	GET_VARARGS( Dest, ARRAY_COUNT(Dest), ARRAY_COUNT(Dest)-1, Fmt, Fmt );

	return FString(Dest);
}


void appItoaAppend( INT InNum,FString &NumberString )
{
	SQWORD	Num					= InNum; // This avoids having to deal with negating -MAXINT-1
	const TCHAR*	NumberChar[11]		= { TEXT("0"), TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"), TEXT("5"), TEXT("6"), TEXT("7"), TEXT("8"), TEXT("9"), TEXT("-") };
	UBOOL	bIsNumberNegative	= FALSE;
	TCHAR	TempNum[16];		// 16 is big enough
	INT		TempAt				= 16; // fill the temp string from the top down.

	// Correctly handle negative numbers and convert to positive integer.
	if( Num < 0 )
	{
		bIsNumberNegative = TRUE;
		Num = -Num;
	}

	TempNum[--TempAt] = 0; // NULL terminator

	// Convert to string assuming base ten and a positive integer.
	do 
	{
		TempNum[--TempAt] = *NumberChar[Num % 10];
		Num /= 10;
	} while( Num );

	// Append sign as we're going to reverse string afterwards.
	if( bIsNumberNegative )
	{
		TempNum[--TempAt] = *NumberChar[10];
	}

	NumberString += TempNum + TempAt;
}

FString appItoa( INT InNum )
{
	FString NumberString;
	appItoaAppend(InNum,NumberString );
	return NumberString;
}