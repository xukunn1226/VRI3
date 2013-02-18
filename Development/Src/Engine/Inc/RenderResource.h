#ifndef VRI3_ENGINE_RENDERRESOURCE_H_
#define VRI3_ENGINE_RENDERRESOURCE_H_

class FRenderResource
{
public:
	FRenderResource():
	  _bInitialized(FALSE)
	  {}

	virtual ~FRenderResource();

	virtual void InitResource();

	virtual void ReleaseResource();

	virtual void UpdateResource();

	virtual FString GetFriendlyName() const { return TEXT("Undefined"); }

	UBOOL IsInitialized() const { return _bInitialized; }

	static list<FRenderResource*>*& GetResourceList();

	virtual void InitDynamicDrv() {};

	virtual void ReleaseDynamicDrv() {};

	virtual void InitDrv() {};

	virtual void ReleaseDrv() {};

protected:
	UBOOL _bInitialized;

	static list<FRenderResource*>*	_ResourceList;
};

extern void BeginInitResource(FRenderResource* Resource);

extern void BeginReleaseResource(FRenderResource* Resource);

extern void BeginUpdateResource(FRenderResource* Resource);

/**
* A texture
*/
class FTexture : public FRenderResource
{
public:
	FTexture() {}

	virtual ~FTexture() {}

	virtual void ReleaseDrv()
	{
		_Texture = NULL;
	}

	virtual UINT GetSizeX() const
	{
		return 0;
	}

	virtual UINT GetSizeY() const
	{
		return 0;
	}

	virtual FString GetFriendlyName() const { return TEXT("FTexture"); }


	FD3D9TexturePtr		_Texture;

	FD3D9SamplerState	_SamplerState;
};

/**
 * A vertex buffer
 */
class FVertexBuffer : public FRenderResource
{
public:
	virtual ~FVertexBuffer() {}

	virtual void ReleaseDrv()
	{
		_VertexBuffer = NULL;
	}

	virtual FString GetFriendlyName() const { return TEXT("FVertexBuffer"); }

	FD3D9VertexBufferPtr	_VertexBuffer;
};

/**
* A vertex buffer with single color component
*/
class FNullColorVertexBuffer : public FVertexBuffer
{
public:
	virtual void InitDrv()
	{
		_VertexBuffer = GDynamicDrv->CreateVertexBuffer(sizeof(DWORD), NULL, FALSE);
		DWORD* Vertices = (DWORD*)GDynamicDrv->LockVertexBuffer(_VertexBuffer, 0, sizeof(DWORD), FALSE);
		Vertices[0] = FLinearColor(255, 255, 255, 255).DWColor();
		GDynamicDrv->UnlockVertexBuffer(_VertexBuffer);
	}
};

template<class ResourceType>
class TGlobalResource : public ResourceType
{
public:
	TGlobalResource()
	{
		BeginInitResource((ResourceType*)this);
	}

	virtual ~TGlobalResource()
	{
		BeginReleaseResource((ResourceType*)this);
	}
};

extern TGlobalResource<FNullColorVertexBuffer>	GNullColorVertexBuffer;

class FIndexBuffer : public FRenderResource
{
public:
	virtual ~FIndexBuffer() {}

	virtual void ReleaseDrv()
	{
		_IndexBuffer = NULL;
	}

	virtual FString GetFriendlyName() const { return TEXT("FIndexBuffer"); }

	FD3D9IndexBufferPtr		_IndexBuffer;
};

#endif