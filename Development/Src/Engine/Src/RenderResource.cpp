#include "EnginePCH.h"

list<FRenderResource*>*	FRenderResource::_ResourceList = NULL;

list<FRenderResource*>*& FRenderResource::GetResourceList()
{
	if( _ResourceList == NULL )
	{
		_ResourceList = new list<FRenderResource*>;
	}
	return _ResourceList;
}

FRenderResource::~FRenderResource()
{
	if( _bInitialized )
	{
		appErrorf(TEXT("FRenderResource '%s' was deleted without being released first!"), GetFriendlyName().c_str());
	}
}

void FRenderResource::InitResource()
{
	if( !_bInitialized )
	{
		list<FRenderResource*>* ResourceList = GetResourceList();
		ResourceList->push_back(this);

		if( GIsDrvInitialized )
		{
			InitDynamicDrv();
			InitDrv();
		}

		_bInitialized = TRUE;
	}
}

void FRenderResource::ReleaseResource()
{
	if( _bInitialized )
	{
		if( GIsDrvInitialized )
		{
			ReleaseDynamicDrv();
			ReleaseDrv();
		}

		list<FRenderResource*>* ResourceList = GetResourceList();
		ResourceList->remove(this);

		_bInitialized = FALSE;
	}
}

void FRenderResource::UpdateResource()
{
	if( _bInitialized && GIsDrvInitialized )
	{
		ReleaseDrv();
		ReleaseDynamicDrv();
		InitDrv();
		InitDynamicDrv();
	}
}

void BeginInitResource(FRenderResource* Resource)
{
	Resource->InitResource();
}

void BeginReleaseResource(FRenderResource* Resource)
{
	Resource->ReleaseResource();
}

void BeginUpdateResource(FRenderResource* Resource)
{
	Resource->UpdateResource();
}

TGlobalResource<FNullColorVertexBuffer>	GNullColorVertexBuffer;