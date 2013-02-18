/**	@brief : FString.h	extension of STL string
 *	@author : xukun
 *	@data : 08.04.29
 */
#ifndef __FSTRINGEX_H__
#define __FSTRINGEX_H__


class FStringUtil 
{
public:
	static void Trim( FString& str, UBOOL bRight = TRUE, UBOOL bLeft = TRUE );

	static void ToLowerCase( FString& str );

	static void ToUpperCase( FString& str );

	static FString LeftStr( const FString& str, INT Count );

	static FString RightStr( const FString& str, INT Count );

	static FString MidStr( const FString& str, INT Start, INT Count );

	static INT	FindStr( const FString& Src, const TCHAR* SubStr, INT StartPosition = 0, UBOOL bIgnoreCase = FALSE );

	static INT	FindStr( const FString& Src, const FString& SubStr, INT StartPosition = 0, UBOOL bIgnoreCase = FALSE );

	static UBOOL StartWith( const FString& Str, const FString& InPrefix );

	static UBOOL StartWith( const TCHAR* Str, const TCHAR* InPrefix );

	static UBOOL EndWith( const FString& Str, const FString& InPostfix );

	static UBOOL EndWith( const TCHAR* Str, const TCHAR* InPostfix );

	static void	SplitFilename(const FString& FullFilename, FString& Filename, FString& Path);

	static void SplitBaseFilename(const FString& Fullname, FString& Filename, FString& Extension);

	static void SplitFilename(const FString& FullFilename, FString& Filename, FString& Extension, FString& Path);

	static INT ParseIntoArray(const FString& str, vector<FString>& strArray, const TCHAR* Delim);

	static FString Sprintf( const TCHAR* Fmt, ... );

};

#endif