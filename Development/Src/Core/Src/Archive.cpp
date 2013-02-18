#include "CorePCH.h"
#include "Archive.h"

FArchive& operator<< (FArchive& Ar, TCHAR*& Str)
{
	INT SaveNum;
	const UBOOL bIsLoading = Ar.IsLoading();
	if( !bIsLoading )
	{
		// > 0 for ANSICHAR, < 0  for UNICHAR
		SaveNum = appIsPureAnsi(Str) ? (INT)_tcslen(Str) : -(INT)_tcslen(Str);
	}
	Ar << SaveNum;

	if( bIsLoading )
	{
		Str = new TCHAR[Abs(SaveNum) + 1];

		if( SaveNum >= 0 )
		{
			ANSICHAR* AnsiString = new ANSICHAR[SaveNum];
			Ar.Serialize(AnsiString, sizeof(ANSICHAR) * SaveNum);
			for( INT i = 0; i < SaveNum + 1; ++i )
			{
				Str[i] = FromAnsi(AnsiString[i]);
			}
			delete[] AnsiString;
		}
		else
		{
			Ar.Serialize(Str, sizeof(UNICHAR) * Abs(SaveNum));
		}
	}
	else
	{
		if( SaveNum >= 0 )
		{
			ANSICHAR* AnsiString = new ANSICHAR[SaveNum];
			for( INT i=0; i<SaveNum + 1; i++ )
			{
				AnsiString[i] = ToAnsi(Str[i]);
			}
			Ar.Serialize( AnsiString, sizeof(ANSICHAR) * SaveNum );
			delete[] AnsiString;
		}
		else
		{
			Ar.Serialize(Str, sizeof(UNICHAR) * Abs(SaveNum));
		}
	}

	return Ar;
}

FArchive& operator<< (FArchive& Ar, FString& Str)
{
	INT SaveNum;
	const UBOOL bIsLoading = Ar.IsLoading();
	if( !bIsLoading )
	{
		// > 0 for ANSICHAR, < 0  for UNICHAR
		SaveNum = appIsPureAnsi(Str.c_str()) ? (INT)Str.size() : -(INT)Str.size();
	}
	Ar << SaveNum;

	if( bIsLoading )
	{
		//Str = new TCHAR[Abs(SaveNum) + 1];
		Str.resize(Abs(SaveNum));

		if( SaveNum >= 0 )
		{
			ANSICHAR* AnsiString = new ANSICHAR[SaveNum];
			Ar.Serialize(AnsiString, sizeof(ANSICHAR) * SaveNum);
			for( INT i = 0; i < SaveNum + 1; ++i )
			{
				Str[i] = FromAnsi(AnsiString[i]);
			}
			delete[] AnsiString;
		}
		else
		{
			Ar.Serialize(&Str.at(0), sizeof(UNICHAR) * Abs(SaveNum));
		}
	}
	else
	{
		if( SaveNum >= 0 )
		{
			ANSICHAR* AnsiString = new ANSICHAR[SaveNum];
			for( INT i=0; i<SaveNum + 1; i++ )
			{
				AnsiString[i] = ToAnsi(Str[i]);
			}
			Ar.Serialize( AnsiString, sizeof(ANSICHAR) * SaveNum );
			delete[] AnsiString;
		}
		else
		{
			Ar.Serialize((void*)Str.data(), sizeof(UNICHAR) * Abs(SaveNum));
		}
	}
	return Ar;
}