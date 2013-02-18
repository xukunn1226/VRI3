#include "CorePCH.h"

FString FNameEntry::GetNameString() const
{
	if( IsUnicode() )
	{
		return FString(_UniName);
	}
	else
	{
		return FString(FANSIToTCHAR((ANSICHAR*)_AnsiName));
	}
}

void FNameEntry::AppendNameToString(FString& String)
{
	if( IsUnicode() )
	{
		String += FString(_UniName);
	}
	else
	{
		String += FString(FANSIToTCHAR((ANSICHAR*)_AnsiName));
	}
}

DWORD FNameEntry::GetNameHash() const
{
	if( IsUnicode() )
	{
		return appStrihash(_UniName);
	}
	else
	{
		return appStrihash(_AnsiName);
	}
}

INT FNameEntry::GetNameLength() const
{
	if( IsUnicode() )
	{
		return appStrlen(_UniName);
	}
	else
	{
		return appStrlen(_AnsiName);
	}
}

UBOOL FNameEntry::IsEqual(const ANSICHAR* InName) const
{
	if( IsUnicode() )
	{
		return FALSE;
	}
	else
	{
		return appStricmp(_AnsiName, InName) == 0;
	}
}

UBOOL FNameEntry::IsEqual(const TCHAR* InName) const
{
	if( IsUnicode() )
	{
		return appStricmp(_UniName, InName) == 0;
	}
	else
	{
		return FALSE;
	}
}

FNameEntry* AllocateNameEntry( const void* Name, NAME_INDEX Index, FNameEntry* HashNext, UBOOL bIsPureAnsi )
{
	const INT NameLen  = bIsPureAnsi ? appStrlen((ANSICHAR*)Name) : appStrlen((TCHAR*)Name);
	FNameEntry* Entry = new FNameEntry;
	Entry->_Index = (Index << NAME_INDEX_SHIFT) | (bIsPureAnsi ? 0 : NAME_UNICODE_MASK);
	Entry->_HashNext = HashNext;
	if( bIsPureAnsi )
	{
		appStrcpy(Entry->GetAnsiName(), NameLen+1, (ANSICHAR*)Name);
	}
	else
	{
		appStrcpy(Entry->GetUniName(), NameLen+1, (TCHAR*)Name);
	}
	return Entry;
}


//  FName static variable
vector<FNameEntry*>	FName::_Names;

FNameEntry*			FName::_NameHash[NameHashBucketCount];

FName::FName(const TCHAR *InName, INT InNumber, EFindName FindType)
{
	Init(InName, InNumber, FindType);
}

FName::FName(const TCHAR* InName, EFindName FindType)
{
	Init(InName, 0, FindType);
}

FName::FName(const ANSICHAR* InName, EFindName FindType)
{
	Init(InName, 0, FindType);
}

/**
 * Initialization code
 *
 * @param InName		String name of the name/number pair
 * @param InNumber		Number part of the name/number pair
 * @param FindType		Operations to perform on names
 * @param bSplitName	If TRUE, the constructor will attempt to split a number off of the string portion
 */
void FName::Init(const TCHAR *InName, INT InNumber, EFindName FindType, UBOOL bSplitName)
{
	check(appStrlen(InName) <= NAME_SIZE);

	if( !GetInitialized() )
	{
		StaticInit();
	}

	TCHAR TempName[NAME_SIZE];
	INT TempNumber;
	if( InNumber == 0 && bSplitName == TRUE )
	{
		if( SplitNameWithCheck(InName, TempName, ARRAY_COUNT(TempName), TempNumber) )
		{
			InName = TempName;
			InNumber = TempNumber;
		}
	}

	check(InName);

	if( !InName[0] )
	{
		_Index = 0;
		_Number = 0;
		return;
	}

	_Number = InNumber;

	INT iHash;
	ANSICHAR AnsiName[NAME_SIZE];
	UBOOL bIsPureAnsi = appIsPureAnsi(InName);
	if( bIsPureAnsi )
	{
		appStrncpyANSI(AnsiName, TCHAR_TO_ANSI(InName), ARRAY_COUNT(AnsiName));
		iHash = appStrihash(AnsiName) & (ARRAY_COUNT(_NameHash) - 1);
	}
	else
	{
		iHash = appStrihash(InName) & (ARRAY_COUNT(_NameHash) - 1);
	}

	for( FNameEntry* Hash = _NameHash[iHash]; Hash; Hash = Hash->_HashNext )
	{
		if( (bIsPureAnsi && Hash->IsEqual(AnsiName))
		 || (!bIsPureAnsi && Hash->IsEqual(InName)) )
		{
			_Index = Hash->GetIndex();

			if( FindType == FNAME_Replace )
			{
				if( bIsPureAnsi )
				{
					appStrcpy(Hash->GetAnsiName(), Hash->GetNameLength() + 1, AnsiName);
				}
				else
				{
					appStrcpy(Hash->GetUniName(), Hash->GetNameLength() + 1, InName);
				}
			}
			return;
		}
	}

	// Don't find the name
	if( FindType == FNAME_Find )
	{
		_Index = 0;
		_Number = 0;
		return;
	}

	_Index = _Names.size() + 1;

	const void* NewName = NULL;
	if( bIsPureAnsi )
	{
		NewName = AnsiName;
	}
	else
	{
		NewName = InName;
	}

	FNameEntry* Entry = AllocateNameEntry(NewName, _Index, _NameHash[iHash], bIsPureAnsi);
	_Names.push_back(Entry);
	_NameHash[iHash] = Entry;
}

void FName::Init(const ANSICHAR* InName, INT InNumber, EFindName FindType )
{
	check(appStrlen(InName)<=NAME_SIZE);
	check(InName);

	if( !GetInitialized() )
	{
		StaticInit();
	}

	// If empty name was specified, return NAME_None.
	if( !InName[0] )
	{
		_Index = 0;
		_Number = 0;
		return;
	}

	// set the number
	_Number = InNumber;

	// Hash value of string. Depends on whether the name is going to be ansi or unicode.
	INT iHash = appStrihash(InName) & (ARRAY_COUNT(_NameHash)-1);

	// Try to find the name in the hash.
	for( FNameEntry* Hash=_NameHash[iHash]; Hash; Hash=Hash->_HashNext )
	{
		// Compare the passed in string
		if(Hash->IsEqual(InName))
		{
			// Found it in the hash.
			_Index = Hash->GetIndex();

			// Check to see if the caller wants to replace the contents of the
			// FName with the specified value. This is useful for compiling
			// script classes where the file name is lower case but the class
			// was intended to be uppercase.
			if (FindType == FNAME_Replace)
			{
				// This should be impossible due to the compare above
				// This *must* be true, or we'll overwrite memory when the
				// copy happens if it is longer
				check(appStrlen(InName) == Hash->GetNameLength());
				// Can't rely on the template override for static arrays since the safe crt version of strcpy will fill in
				// the remainder of the array of NAME_SIZE with 0xfd.  So, we have to pass in the length of the dynamically allocated array instead.
				appStrcpy(Hash->GetAnsiName(),Hash->GetNameLength()+1,InName);
			}

			return;
		}
	}

	// Didn't find name.
	if( FindType==FNAME_Find )
	{
		// Not found.
		_Index = 0;
		_Number = 0;
		return;
	}

	_Index = _Names.size() + 1;

	FNameEntry* Entry = AllocateNameEntry(InName, _Index, _NameHash[iHash], TRUE);
	_Names.push_back(Entry);
	_NameHash[iHash] = Entry;
}

UBOOL FName::SplitNameWithCheck(const TCHAR* OldName, TCHAR* NewName, INT NewNameLen, INT& NewNumber)
{
	UBOOL bSucceeded = FALSE;
	const INT OldNameLength = appStrlen(OldName);

	if( OldNameLength > 0 )
	{
		const TCHAR* LastChar = OldName + (OldNameLength - 1);

		const TCHAR* Ch = LastChar;
		if( *Ch >= '0' && *Ch <= '9' )		// 最后字符为数字
		{
			// 从后往前查找第一个非数字字符
			while( *Ch >= '0' && *Ch <= '9' && Ch > OldName )
			{
				Ch--;
			}

			if( *Ch == '_')
			{
				// 如果"_"后有多数字，且第一个数字为0，则不拆分.但NAME_0是支持的
				if( Ch[1] != '0' || LastChar - Ch == 1 )
				{
					NewNumber = appAtoi(Ch + 1);

					appStrncpy(NewName, OldName, Min<INT>(Ch - OldName + 1, NewNameLen));

					bSucceeded = TRUE;
				}
			}
		}
	}

	return bSucceeded;
}

void FName::AppendString(FString &Out) const
{
	check(_Index < _Names.size());
	check(_Names[_Index]);
	FNameEntry* Entry = _Names[_Index];
	Entry->AppendNameToString(Out);
	if( _Number != 0 )	// if instance number is 0, don't need "NAME_0" presentation
	{
		Out += TEXT("_");
		appItoaAppend(_Number, Out);
	}
}

void FName::toString(FString& Out) const
{
	check(_Index < _Names.size());
	check(_Names[_Index]);
	AppendString(Out);
}

FString FName::toString() const
{
	FString Out;
	toString(Out);
	return Out;
}

INT FName::Compare(const FName& Other) const
{
	if( GetIndex() == Other.GetIndex() )
	{
		return GetNumber() - Other.GetNumber();
	}
	else
	{
		FNameEntry* ThisEntry = _Names[_Index];
		FNameEntry* OtherEntry = Other._Names[_Index];

		if( ThisEntry->IsUnicode() != OtherEntry->IsUnicode() )
		{
			return appStricmp(ThisEntry->IsUnicode() ? ThisEntry->GetUniName() : ANSI_TO_TCHAR(ThisEntry->GetAnsiName()),
							  OtherEntry->IsUnicode() ? OtherEntry->GetUniName() : ANSI_TO_TCHAR(OtherEntry->GetAnsiName()));
		}
		else if( ThisEntry->IsUnicode() )	// Both are unicode
		{
			return appStricmp(ThisEntry->GetUniName(), OtherEntry->GetUniName());
		}
		else	// Both are ansi
		{
			return appStricmp(ThisEntry->GetAnsiName(), OtherEntry->GetAnsiName());
		}
	}
}

/** Hardcode a name */
void FName::Hardcode(FNameEntry* InEntry)
{
	INT iHash = InEntry->GetNameHash() & (ARRAY_COUNT(_NameHash)-1);
	InEntry->_HashNext = _NameHash[iHash];
	_NameHash[iHash] = InEntry;

	for(INT i = _Names.size(); i <= InEntry->GetIndex(); ++i)
	{
		_Names.push_back(NULL);
	}
	_Names[InEntry->GetIndex()] = InEntry;
}

UBOOL FName::operator==(const TCHAR* Other) const
{
	check(Other);
	check(_Index < _Names.size());
	FNameEntry* Entry = _Names[_Index];
	check(Entry);

	TCHAR TempBuffer[NAME_SIZE];
	INT TempNumber = 0;
	
	if( SplitNameWithCheck(Other, TempBuffer, ARRAY_COUNT(TempBuffer), TempNumber) )
	{
		Other = TempBuffer;
	}

	UBOOL bMatch = FALSE;
	if( TempNumber == _Number &&
		appStricmp( Other, Entry->IsUnicode() ? Entry->GetUniName() : ANSI_TO_TCHAR(Entry->GetAnsiName()) ) )
	{
		bMatch = TRUE;
	}
	return bMatch;
}

/////////////////////////////////////////////////////////////////////////////
// CRC 32 polynomial.
#define CRC32_POLY 0x04c11db7

/** 
* Helper class for initializing the global GCRCTable
*/
class FCRCTableInit
{
public:
	/**
	* Constructor
	*/
	FCRCTableInit()
	{
		// Init CRC table.
		for( DWORD iCRC=0; iCRC<256; iCRC++ )
		{
			for( DWORD c=iCRC<<24, j=8; j!=0; j-- )
			{
				GCRCTable[iCRC] = c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
			}
		}
	}	
};
DWORD GCRCTable[256];

void FName::StaticInit()
{
	static FCRCTableInit GCRCTableInit;

	check(GetInitialized() == FALSE);

	GetIsInitialized() = 1;

	// init hash table
	for(INT HashIndex = 0; HashIndex < ARRAY_COUNT(FName::_NameHash); ++HashIndex)
	{
		_NameHash[HashIndex] = NULL;
	}

	// init hardcoded names
	#define REGISTER_NAME(num, name)	Hardcode(AllocateNameEntry(#name, num, NULL, TRUE));
	#include "NameDefine.h"
}