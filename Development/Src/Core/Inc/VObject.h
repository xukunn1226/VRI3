#ifndef VRI3_CORE_OBJECT_H_
#define VRI3_CORE_OBJECT_H_

/**
 * collector of all CLASS INFO of VObject
 */
class FClassTemplates
{
public:
	FClassTemplates();
	~FClassTemplates();

	void RegisterClass(const FName& ClassName, class VClass* ClassTemplate);

	void UnregisterClass(const FName& ClassName);

	const class VClass* FindClassTemplate(const FName& ClassName);

private:
	map<FName, class VClass*>	_ClassTemplatesMap;
};

/**
 * 封装类信息
 */
class VClass
{
public:
	typedef void (*pfn)(VObject** pOutObject);

	VClass*		_Super;						// parent class
	FName		_ClassName;					// class name
	pfn			_ClassStaticConstructor;	// pointer of function constructor
	INT			_ClassUnique;

	VClass(VClass* SuperClass, const FName& ClassName, void (*InClassStaticConstructor)(VObject**));

	FName GetClassName() const { return _ClassName; }

	const VClass* GetSuperClass() const { return _Super; }
};

#define DECLARE_BASE_CLASS(TClass)	\
	private: \
		static FClassTemplates*		_ClassTemplates; \
	public: \
		static void RegisterClassTemplate(VClass* InClass) \
		{ \
			if( !_ClassTemplates ) \
			{ \
				_ClassTemplates = new FClassTemplates(); \
			} \
			_ClassTemplates->RegisterClass(InClass->GetClassName(), InClass); \
		} \
	private: \
		static VClass				_StaticClass; \
		VClass*						_Class;	\
	protected: \
		static void InternalStaticConstructor(TClass** OutObject) {} \
	public: \
		typedef TClass	ThisClass; \
		static VClass* StaticClass() \
		{ \
			return &_StaticClass; \
		} \
		virtual UBOOL IsExactKindOf(const VClass* InClass) const \
		{ \
			return TClass::StaticClass() == InClass; \
		} \
		virtual UBOOL IsKindOf(const VClass* InClass) const \
		{ \
			const VClass* TempClass = TClass::StaticClass(); \
			while(TempClass) \
			{ \
				if( TempClass == InClass ) \
				{ \
					return TRUE; \
				} \
				TempClass = TempClass->GetSuperClass(); \
			} \
			return FALSE; \
		} \
		virtual TClass* DynamicCast(const VClass* InClass) const \
		{ \
			return IsKindOf(InClass) ? (TClass*)this : NULL; \
		} \
		virtual VClass* GetClass() const \
		{ \
			return TClass::StaticClass(); \
		}

#define DECLARE_CLASS(TClass, TSuperClass)	\
	private: \
		static VClass	_StaticClass; \
		VClass*						_Class;	\
	protected: \
		static void InternalStaticConstructor(TClass** OutObject) \
		{ \
			(TClass*)*OutObject = new TClass(); \
		} \
	public: \
		typedef TSuperClass Super; \
		static VClass* StaticClass() \
		{ \
			return &_StaticClass; \
		} \
		virtual UBOOL IsExactKindOf(const VClass* InClass) const \
		{ \
			return TClass::StaticClass() == InClass; \
		} \
		virtual UBOOL IsKindOf(const VClass* InClass) const \
		{ \
			const VClass* TempClass = TClass::StaticClass(); \
			while(TempClass) \
			{ \
				if( TempClass == InClass ) \
				{ \
					return TRUE; \
				} \
				TempClass = TempClass->GetSuperClass(); \
			} \
			return FALSE; \
		} \
		virtual TClass* DynamicCast(const VClass* InClass) const \
		{ \
			return IsKindOf(InClass) ? (TClass*)this : NULL; \
		} \
		virtual VClass* GetClass() const \
		{ \
			return TClass::StaticClass(); \
		}


#define IMPLEMENT_BASE_CLASS(TClass) \
	FClassTemplates* TClass::_ClassTemplates = NULL; \
	VClass TClass::_StaticClass(NULL, TEXT(#TClass), (void(*)(VObject**))&TClass::InternalStaticConstructor);

#define IMPLEMENT_CLASS(TClass) \
	VClass TClass::_StaticClass(TClass::Super::StaticClass(), TEXT(#TClass), (void(*)(VObject**))&TClass::InternalStaticConstructor);




#define OBJECT_HASH_BINS 8192

FORCEINLINE INT GetObjectHash(FName ObjectName)
{
	return (ObjectName.GetIndex() ^ ObjectName.GetNumber()) & (OBJECT_HASH_BINS - 1);
}

FORCEINLINE INT GetObjectOuterHash(FName ObjectName, PTRINT Outer)
{
	return ((ObjectName.GetIndex() ^ ObjectName.GetNumber()) ^ (Outer >> 4)) & (OBJECT_HASH_BINS - 1);
}

class VObject : public FRefCountedObject
{
	DECLARE_BASE_CLASS(VObject);

	virtual ~VObject();

	static void StaticInit();

	static void StaticExit();

	/**
	 * Load VClass according to ClassName
	 */
	static const VClass* LoadClass(const FName& ClassName);

	static VObject* StaticAllocateObject(const VClass* InClass, FName InName = NAME_None);

	static FName MakeUniqueObjectName(const VClass* InClass, FName BaseName = NAME_None);

	/**
	 * Create a new instance of an object, if there is an object withe the same InName in memory, 
	 * the object will be found and return
	 */
	static VObject* StaticConstructObject(const VClass* InClass, FName InName = NAME_None);

	static VObject* StaticFindObject(const VClass* InClass, FName ObjectName, UBOOL bExactClass = FALSE);

	static VObject* StaticLoadObject(const FName& ClassName, const TCHAR* InName);
	
	virtual void ConditionalDestroy();

public:
	FORCEINLINE const FName GetFName() const
	{
		return _Index != INDEX_NONE ? _Name : TEXT("<uninitialized>");
	}

	FORCEINLINE static const FName GetClassName()
	{
		return StaticClass()->GetClassName();
	}

private:
	void AddObject(INT Index);

	void HashObject();

	void UnhashObject();

private:
	/** Name of the object */
	FName			_Name;

	/** Index of object into GObjObjects array */
	INT				_Index;

	/** Object this object resides in */
	VObject*		_Outer;

	/** Next object in hash bin */
	VObject*		_HashNext;

	static UBOOL				_GObjInitialized;

	static VObject*				_GObjHash[OBJECT_HASH_BINS];

	static VObject*				_GObjHashOuter[OBJECT_HASH_BINS];

	/** List of all objects */
	static vector<VObject*>		_GObjObjects;

	/** Available object indices */
	static vector<VObject*>		_GObjAvailable;
};
SmartPointer(VObject);


#endif