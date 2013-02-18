#include "CorePCH.h"

IMPLEMENT_BASE_CLASS(VObject);


VClass::VClass(VClass* SuperClass, const FName& ClassName, void (*InClassStaticConstructor)(VObject**))
: _Super(SuperClass),
  _ClassName(ClassName),
  _ClassStaticConstructor(InClassStaticConstructor),
  _ClassUnique(0)
{
	VObject::RegisterClassTemplate(this);
}

FClassTemplates::FClassTemplates()
{
	_ClassTemplatesMap.clear();
}

FClassTemplates::~FClassTemplates()
{
	_ClassTemplatesMap.clear();
}

void FClassTemplates::RegisterClass(const FName& ClassName, VClass* ClassTemplate)
{
	map<FName, VClass*>::iterator it;
	it = _ClassTemplatesMap.find(ClassName);
	if( it == _ClassTemplatesMap.end() )
	{
		_ClassTemplatesMap[ClassName] = ClassTemplate;
	}
	else
	{
		debugf(NAME_Warning, TEXT("Class %s has been registered!"), ClassName.toString().c_str());
	}
}

void FClassTemplates::UnregisterClass(const FName& ClassName)
{
	map<FName, VClass*>::iterator it;
	it = _ClassTemplatesMap.find(ClassName);
	if( it != _ClassTemplatesMap.end() )
	{
		_ClassTemplatesMap.erase(it);
	}
}

const VClass* FClassTemplates::FindClassTemplate(const FName &ClassName)
{
	map<FName, VClass*>::iterator it;
	it = _ClassTemplatesMap.find(ClassName);
	if( it != _ClassTemplatesMap.end() )
	{
		return it->second;
	}
	return NULL;
}


//---------------- VObject static variable
UBOOL VObject::_GObjInitialized = FALSE;
VObject* VObject::_GObjHash[OBJECT_HASH_BINS];
VObject* VObject::_GObjHashOuter[OBJECT_HASH_BINS];
vector<VObject*> VObject::_GObjObjects;
vector<VObject*> VObject::_GObjAvailable;


VObject::~VObject()
{
	ConditionalDestroy();
}

void VObject::ConditionalDestroy()
{
}

void VObject::StaticInit()
{
	_GObjInitialized = TRUE;
	appMemzero(_GObjHash, sizeof(VObject*) * OBJECT_HASH_BINS);
	appMemzero(_GObjHashOuter, sizeof(VObject*) * OBJECT_HASH_BINS);
}

void VObject::StaticExit()
{
	_GObjInitialized = FALSE;
}

void VObject::AddObject(INT Index)
{
	if( Index == INDEX_NONE )
	{
		Index = _GObjObjects.size();
		_GObjObjects.push_back(NULL);
	}

	_GObjObjects[Index] = this;
	_Index = Index;
	HashObject();
}

void VObject::HashObject()
{
	INT iHash = 0;
	iHash = GetObjectHash(_Name);
	_HashNext = _GObjHash[iHash];
	_GObjHash[iHash] = this;
}

void VObject::UnhashObject()
{
	INT iHash = 0;
	VObject** Hash = NULL;

	iHash = GetObjectHash(_Name);
	Hash = &_GObjHash[iHash];

	while(*Hash != NULL)
	{
		if( *Hash != this )
		{
			Hash = &(*Hash)->_HashNext;
		}
		else
		{
			*Hash = (*Hash)->_HashNext;
			break;
		}
	}
}

const VClass* VObject::LoadClass(const FName& ClassName)
{
	const VClass* Class = _ClassTemplates->FindClassTemplate(ClassName);
	if( Class == NULL )
	{
		debugf(NAME_Warning, TEXT("Not found the archetype: %s"), ClassName.toString().c_str());
	}
	return Class;
}

VObject* VObject::StaticAllocateObject(const VClass* InClass, FName InName)
{
	if( !InClass )
	{
		debugf(TEXT("Invalid class for object %s"), InName.toString().c_str());
		return NULL;
	}

	VObject* Obj = NULL;
	if( InName == NAME_None )
	{
		InName = MakeUniqueObjectName(InClass);
	}
	else
	{
		Obj = StaticFindObject(InClass, InName);
	}

	if( Obj == NULL )
	{
		(*InClass->_ClassStaticConstructor)(&Obj);

		Obj->_Index = INDEX_NONE;
		Obj->_HashNext = NULL;
		Obj->_Name = InName;

		Obj->AddObject(INDEX_NONE);
	}

	return Obj;
}

FName VObject::MakeUniqueObjectName(const VClass* InClass, FName BaseName /* = NAME_None */)
{
	check(InClass);
	VClass* Class = (VClass*)InClass;
	if( BaseName == NAME_None )
	{
		BaseName = InClass->GetClassName();
	}

	EName BaseNameIndex = (EName)BaseName.GetIndex();
	FName TestName;
	do
	{
		TestName = FName(BaseNameIndex, ++(Class->_ClassUnique));
	} 
	while (StaticFindObject(InClass, TestName));

	return TestName;
}

VObject* VObject::StaticConstructObject(const VClass* InClass, FName InName)
{
	return StaticAllocateObject(InClass, InName);
}

VObject* VObject::StaticFindObject(const VClass* InClass, FName ObjectName, UBOOL bExactClass)
{
	INT iHash = GetObjectHash(ObjectName);
	for(VObject* Hash = _GObjHash[iHash]; Hash; Hash = Hash->_HashNext)
	{
		if( Hash->GetFName() == ObjectName 
			&& (InClass == NULL || bExactClass ? Hash->GetClass() == InClass : Hash->IsKindOf(InClass)) )
		{
			return Hash;
		}
	}

	return NULL;
}

VObject* VObject::StaticLoadObject(const FName& ClassName, const TCHAR* InName)
{
	return NULL;
}

