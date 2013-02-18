/**	@brief : RefCounting.h  reference count and smart pointer
 *	@author : xukun
 *	@data : 08.04.18
 */

#ifndef VRI3_CORE_REFCOUNTING_H_
#define VRI3_CORE_REFCOUNTING_H_

class FRefCountedObject
{
public:
	FRefCountedObject(): NumRefs(0) {}
	virtual ~FRefCountedObject() {}
	virtual DWORD AddRef() const
	{
		return DWORD(++NumRefs);
	}
	virtual DWORD Release() const
	{
		DWORD Refs = DWORD(--NumRefs);
		if(Refs == 0)
		{
			delete this;
		}
		return Refs;
	}
	DWORD GetRefCount() const
	{
		return DWORD(NumRefs);
	}
public:
	mutable INT NumRefs;
};

template<typename T>
class TRefCountPtr
{
public:
	TRefCountPtr():
		_Object(NULL)
	{}

	TRefCountPtr(T* InObject,UBOOL bAddRef = TRUE)
	{
		_Object = InObject;
		if(_Object && bAddRef)
		{
			_Object->AddRef();
		}
	}

	TRefCountPtr(const TRefCountPtr& Copy)
	{
		_Object = Copy._Object;
		if(_Object)
		{
			_Object->AddRef();
		}
	}

	~TRefCountPtr()
	{
		if(_Object)
		{
			_Object->Release();
		}
	}

	TRefCountPtr& operator=(T* InObject)
	{
		// Call AddRef before Release, in case the new reference is the same as the old reference.
		T* OldObject = _Object;
		_Object = InObject;
		if(_Object)
		{
			_Object->AddRef();
		}
		if(OldObject)
		{
			OldObject->Release();
		}
		return *this;
	}
	
	TRefCountPtr& operator=(const TRefCountPtr& InPtr)
	{
		return *this = InPtr._Object;
	}

	UBOOL operator==(const TRefCountPtr& Other) const
	{
		return _Object == Other._Object;
	}

	UBOOL operator!=(const TRefCountPtr& Other) const
	{
		return _Object != Other._Object;
	}

	UBOOL operator==(const T* InObject) const
	{
		return _Object == InObject;
	}

	UBOOL operator!=(const T* InObject) const
	{
		return _Object != InObject;
	}

	T* operator->() const
	{
		return _Object;
	}

	T& operator*() const
	{
		return *_Object;
	}

	operator T*() const
	{
		return _Object;
	}

	T** GetInitReference()
	{
		*this = NULL;
		return &_Object;
	}

	T* GetReference() const
	{
		return _Object;
	}

	friend UBOOL IsValidRef(const TRefCountPtr& Reference)
	{
		return Reference._Object != NULL;
	}

	void SafeRelease()
	{
		*this = NULL;
	}

	DWORD GetRefCount()
	{
		if(_Object)
		{
			_Object->AddRef();
			return _Object->Release();
		}
		else
		{
			return 0;
		}
	}

private:
	T*	_Object;
};

#define SmartPointer(classname)	\
	class classname;	\
	typedef TRefCountPtr<classname>	classname##Ptr;	

#endif