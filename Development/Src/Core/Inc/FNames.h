#ifndef VRI3_CORE_NAME_H_
#define VRI3_CORE_NAME_H_

/************************************************************************/
/*  GLOBAL NAME DEFINES                                                 */
/************************************************************************/

typedef INT NAME_INDEX;

/** maximum size of name */
enum { NAME_SIZE = 1024 };

const UINT NameHashBucketCount = 65536;

enum EFindName
{
	/** Find a name; return 0 if it doesn't exist. */
	FNAME_Find,

	/** Find a name or add it if it doesn't exist. */
	FNAME_Add,

	/** Finds a name and replaces it. Adds it if missing */
	FNAME_Replace,
};

/*------------------------------------------------------------
 *	FNameEntry
 *------------------------------------------------------------*/

#define NAME_UNICODE_MASK 0x1
#define NAME_INDEX_SHIFT 1

/**
 * A global name, as stored in the global name table
 */
struct FNameEntry
{
public:
	FNameEntry() {}

	FORCEINLINE INT GetIndex() const
	{
		return _Index >> NAME_INDEX_SHIFT;
	}

	FORCEINLINE UBOOL IsUnicode() const
	{
		return _Index & NAME_UNICODE_MASK;
	}

	FString GetNameString() const;

	void AppendNameToString(FString& String);

	/**
	* @return case insensitive hash of name
	*/
	DWORD GetNameHash() const;

	INT GetNameLength() const;

	/**
	 * Compares name without looking at case
	 */
	UBOOL IsEqual(const ANSICHAR* InName) const;
	UBOOL IsEqual(const TCHAR* InName) const;

	FORCEINLINE ANSICHAR* GetAnsiName()
	{
		check(!IsUnicode());
		return _AnsiName;
	}

	FORCEINLINE TCHAR* GetUniName()
	{
		check(IsUnicode());
		return _UniName;
	}

	friend FNameEntry* AllocateNameEntry( const void* Name, NAME_INDEX Index, FNameEntry* HashNext, UBOOL bIsPureAnsi );

public:
	/** pointer to the next entry in this hash linked list */
	FNameEntry*		_HashNext;

private:
	NAME_INDEX		_Index;		// 最后一位记录是否UNICODE

	union
	{
		ANSICHAR	_AnsiName[NAME_SIZE];
		TCHAR		_UniName[NAME_SIZE];
	};

};
FNameEntry* AllocateNameEntry( const void* Name, NAME_INDEX Index, FNameEntry* HashNext, UBOOL bIsPureAnsi );


/**
* Public name, available to the world.  Names are stored as a combination of
* an index into a table of unique strings and an instance number.
* Names are case-insensitive.
*/
class FName
{
public:
	FName() {}

	FName(const TCHAR* InName, INT InNumber, EFindName FindType = FNAME_Add);

	FName(const TCHAR* InName, EFindName FindType = FNAME_Add);

	FName( const ANSICHAR* Name, EFindName FindType=FNAME_Add );

	FName(EName Index, INT InNumber = 0)
		: _Index(Index), _Number(InNumber) 
	{}

	NAME_INDEX GetIndex() const { return _Index; }

	INT GetNumber() const { return _Number; }

	const FString GetNameString() const
	{
		return _Names[_Index]->GetNameString();
	}

	FString toString() const;

	void toString(FString& Out) const;

	void AppendString(FString& Out) const;

	UBOOL operator==(const FName& Other) const
	{
		return _Index == Other._Index && _Number == Other._Number;
	}

	UBOOL operator!=(const FName& Other) const
	{
		return _Index != Other._Index || _Number != Other._Number;
	}

	UBOOL operator==(const TCHAR* Other) const;

	bool operator<(const FName& Other) const
	{
		if( _Index == Other._Index )
		{
			return _Number < Other._Number;
		}
		return _Index < Other._Index;
	}

	UBOOL IsValid() const
	{
		return _Index >= 0 && _Index < _Names.size() && _Names[_Index] != NULL;
	}

	/**
	* Compares name to passed in one. Sort is alphabetical ascending.
	*
	* @param	Other	Name to compare this against
	* @return	< 0 is this < Other, 0 if this == Other, > 0 if this > Other
	*/
	INT Compare(const FName& Other) const;

	static void StaticInit();

	static FString SafeString(EName Index, INT InstanceNumber = 0)
	{
		return GetInitialized() 
			? (Index >= 0 && Index < _Names.size() && _Names[Index] 
				? FName(Index, InstanceNumber).toString() : FString(TEXT("INVALID")))
			: FString(TEXT("UNINITIALIZED"));
	}

	static FNameEntry* GetEntry(INT i)
	{
		return _Names[i];
	}

	static UBOOL GetInitialized()
	{
		return GetIsInitialized();
	}

	static UBOOL& GetIsInitialized()
	{
		static UBOOL bIsIntialized = 0;
		return bIsIntialized;
	}

	static void Hardcode(FNameEntry* InEntry);

private:
	/**
	* Shared initialization code (between two constructors)
	* 
	* @param InName String name of the name/number pair
	* @param InNumber Number part of the name/number pair
	* @param FindType Operation to perform on names
	* @param bSplitName If TRUE, this function will attempt to split a number off of the string portion (turning Rocket_17 to Rocket and number 17)
	*/
	void Init(const TCHAR* InName, INT InNumber, EFindName FindType, UBOOL bSplitName = TRUE);

	void Init(const ANSICHAR* InName, INT InNumber, EFindName FindType );

	/**
	* Helper function to split an old-style name (Class_Number, ie Rocket_17) into
	* the component parts usable by new-style FNames. Only use results if this function
	* returns TRUE.
	*
	* @param OldName		Old-style name
	* @param NewName		Ouput string portion of the name/number pair
	* @param NewNameLen		Size of NewName buffer (in TCHAR units)
	* @param NewNumber		Number portion of the name/number pair
	*
	* @return TRUE if the name was split, only then will NewName/NewNumber have valid values
	*/
	static UBOOL SplitNameWithCheck(const TCHAR* OldName, TCHAR* NewName, INT NewNameLen, INT& NewNumber);

private:
	NAME_INDEX		_Index;			// Index into the _Names array
	
	INT				_Number;		// instance number

	static vector<FNameEntry*>	_Names;

	static FNameEntry*			_NameHash[NameHashBucketCount];
};


#endif