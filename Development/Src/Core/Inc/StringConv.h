#ifndef VRI3_CORE_STRINGCONV_H_
#define VRI3_CORE_STRINGCONV_H_

class FTCHARToANSI_Convert
{
	DWORD CodePage;

public:
	FORCEINLINE FTCHARToANSI_Convert(DWORD InCodePage = CP_ACP)
		: CodePage(InCodePage)
	{
	}

	FORCEINLINE ANSICHAR* Convert(const TCHAR* Source, ANSICHAR* Dest, DWORD Size)
	{
		DWORD LengthW = (DWORD)lstrlenW(Source) + 1;

		DWORD LengthA = LengthW * 2;
		if( LengthA > Size )
		{
			Dest = new char[LengthA * sizeof(ANSICHAR)];
		}

		WideCharToMultiByte(CodePage, 0, Source, LengthW, Dest, LengthA, NULL, NULL);
		
		return Dest;
	}
};

class FANSIToTCHAR_Convert
{
public:
	FORCEINLINE TCHAR* Convert(const ANSICHAR* Source, TCHAR* Dest, DWORD Size)
	{
		DWORD Length = (DWORD)lstrlenA(Source) + 1;

		if( Length > Size )
		{
			Dest = new TCHAR[Length * sizeof(TCHAR)];
		}

		MultiByteToWideChar(CP_ACP, 0, Source, Length, Dest, Length);

		return Dest;
	}
};

template< typename CONVERT_TO, typename CONVERT_FROM, typename BASE_CONVERTER,
	DWORD DefaultConversionSize = 256 >
class TStringConversion : public BASE_CONVERTER
{
	CONVERT_TO Buffer[DefaultConversionSize];

	CONVERT_TO* ConvertedString;

	TStringConversion();

public:
	inline TStringConversion(const CONVERT_FROM* Source)
	{
		if( Source )
		{
			ConvertedString = BASE_CONVERTER::Convert(Source, Buffer, DefaultConversionSize);
		}
		else
		{
			ConvertedString = NULL;
		}
	}

	inline ~TStringConversion()
	{
		if( ConvertedString != Buffer && ConvertedString != NULL )
		{
			delete[] ConvertedString;
		}
	}

	inline operator CONVERT_TO*() const
	{
		return ConvertedString;
	}
};

typedef TStringConversion<TCHAR, ANSICHAR, FANSIToTCHAR_Convert> FANSIToTCHAR;
typedef TStringConversion<ANSICHAR, TCHAR, FTCHARToANSI_Convert> FTCHARToANSI;

#endif