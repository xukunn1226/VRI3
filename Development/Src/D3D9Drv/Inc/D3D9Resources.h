#ifndef VRI3_D3D9DRV_D3D9RESOURCES_H_
#define VRI3_D3D9DRV_D3D9RESOURCES_H_

/**
 * basic class of all d3d resource
 */
class FD3DResource : public FRefCountedObject
{
public:
	virtual DWORD Release() const
	{
		DWORD Refs = DWORD(--NumRefs);
		if(Refs == 0)
		{
			RemoveResourceRef();
			delete this;
		}
		return Refs;
	}

protected:
	virtual void RemoveResourceRef() const {}
};


template<class D3DResourceType>
class TD3D9Resource : public FD3DResource
{
public:
	TD3D9Resource(D3DResourceType* InResource = NULL)
		: _Resource(InResource)
	{}

	virtual void RemoveResourceRef() const
	{
		if( _Resource )
		{
			_Resource->Release();
		}
	}

	D3DResourceType* GetD3DResource() const
	{
		return _Resource;
	}

	D3DResourceType** GetInitD3DResource()
	{
		return &_Resource;
	}

	UBOOL IsValidD3DResource()
	{
		return _Resource != NULL;
	}

private:
	D3DResourceType*		_Resource;
};
typedef TD3D9Resource<IDirect3DIndexBuffer9>		FD3D9IndexBuffer;
typedef TD3D9Resource<IDirect3DVertexBuffer9>		FD3D9VertexBuffer;
typedef TD3D9Resource<IDirect3DVertexShader9>		FD3D9VertexShader;
typedef TD3D9Resource<IDirect3DPixelShader9>		FD3D9PixelShader;
typedef TD3D9Resource<IDirect3DVertexDeclaration9>	FD3D9VertexDeclaration;


typedef TRefCountPtr<FD3D9IndexBuffer>			FD3D9IndexBufferPtr;
typedef TRefCountPtr<FD3D9VertexBuffer>			FD3D9VertexBufferPtr;
typedef TRefCountPtr<FD3D9VertexShader>			FD3D9VertexShaderPtr;
typedef TRefCountPtr<FD3D9PixelShader>			FD3D9PixelShaderPtr;
typedef TRefCountPtr<FD3D9VertexDeclaration>	FD3D9VertexDeclarationPtr;



class FD3D9OcclusionQuery : public TD3D9Resource<IDirect3DQuery9>
{
public:
	FD3D9OcclusionQuery(IDirect3DQuery9* InResource)
		: TD3D9Resource<IDirect3DQuery9>(InResource), _bResultIsCached(FALSE)
	{}

	DWORD				_Result;
	UBOOL				_bResultIsCached;
};
typedef TRefCountPtr<FD3D9OcclusionQuery>	FD3D9OcclusionQueryPtr;


template<class D3DResourceType>
class TD3D9Texture : public TD3D9Resource<D3DResourceType>
{
public:
	TD3D9Texture(UBOOL bInSRGB = FALSE, UBOOL bInDynamic = FALSE, D3DResourceType* InResource = NULL)
		: TD3D9Resource<D3DResourceType>(InResource), _bSRGB(bInSRGB), _bDynamic(bInDynamic)
	{}

	UBOOL IsSRGB() const
	{
		return _bSRGB;
	}

	UBOOL IsDynamic() const
	{
		return _bDynamic;
	}

private:
	UBOOL				_bSRGB;
	UBOOL				_bDynamic;
};
typedef TD3D9Texture<IDirect3DBaseTexture9>				FD3D9Texture;
typedef TD3D9Texture<IDirect3DTexture9>					FD3D9Texture2D;
typedef TD3D9Texture<IDirect3DCubeTexture9>				FD3D9TextureCube;

typedef TRefCountPtr<FD3D9Texture>						FD3D9TexturePtr;
typedef TRefCountPtr<FD3D9Texture2D>					FD3D9Texture2DPtr;
typedef TRefCountPtr<FD3D9TextureCube>					FD3D9TextureCubePtr;


class FD3D9Surface : public TD3D9Resource<IDirect3DSurface9>
{
public:
	FD3D9Surface(FD3D9Texture2DPtr InTexture2D = NULL, IDirect3DSurface9* InSurface = NULL)
		: TD3D9Resource<IDirect3DSurface9>(InSurface), _Texture2D(InTexture2D)
	{}

	virtual void RemoveResourceRef() const
	{
		if( _Texture2D != NULL && _Texture2D->IsValidD3DResource() )
		{
			_Texture2D = NULL;
		}

		TD3D9Resource<IDirect3DSurface9>::RemoveResourceRef();
	}

	mutable FD3D9Texture2DPtr		_Texture2D;
};
typedef TRefCountPtr<FD3D9Surface>				FD3D9SurfacePtr;





//
//template<class D3DResourceType>
//class TD3D9Resource : public D3DResourceType
//{
//};
//
//typedef TD3D9Resource<IDirect3DIndexBuffer9>		FD3D9IndexBuffer;
//typedef TD3D9Resource<IDirect3DVertexBuffer9>		FD3D9VertexBuffer;
//typedef TD3D9Resource<IDirect3DVertexShader9>		FD3D9VertexShader;
//typedef TD3D9Resource<IDirect3DPixelShader9>		FD3D9PixelShader;
//typedef TD3D9Resource<IDirect3DVertexDeclaration9>	FD3D9VertexDeclaration;
//
//typedef TRefCountPtr<FD3D9IndexBuffer>				FD3D9IndexBufferPtr;
//typedef TRefCountPtr<FD3D9VertexBuffer>				FD3D9VertexBufferPtr;
//typedef TRefCountPtr<FD3D9VertexShader>				FD3D9VertexShaderPtr;
//typedef TRefCountPtr<FD3D9PixelShader>				FD3D9PixelShaderPtr;
//typedef TRefCountPtr<FD3D9VertexDeclaration>		FD3D9VertexDeclarationPtr;

//
//template<class D3DResourceType>
//class TD3D9Texture : public FRefCountedObject, 
//					 public TRefCountPtr<D3DResourceType>
//{
//public:
//	TD3D9Texture(UBOOL bInSRGB = FALSE, UBOOL bInDynamic = FALSE, D3DResourceType* InResource = NULL)
//		: bSRGB(bInSRGB), bDynamic(bInDynamic), TRefCountPtr<D3DResourceType>(InResource)
//	{
//	}
//
//	UBOOL IsSRGB() const
//	{
//		return bSRGB;
//	}
//
//	UBOOL IsDynamic() const
//	{
//		return bDynamic;
//	}
//
//private:
//	UBOOL				bSRGB;
//	UBOOL				bDynamic;
//};
//typedef TD3D9Texture<IDirect3DBaseTexture9>				FD3D9Texture;
//typedef TD3D9Texture<IDirect3DTexture9>					FD3D9Texture2D;
//typedef TD3D9Texture<IDirect3DCubeTexture9>				FD3D9TextureCube;
//
//typedef TRefCountPtr<FD3D9Texture>						FD3D9TexturePtr;
//typedef TRefCountPtr<FD3D9Texture2D>					FD3D9Texture2DPtr;
//typedef TRefCountPtr<FD3D9TextureCube>					FD3D9TextureCubePtr;
//

//
//class FD3D9Surface : public FRefCountedObject,
//					 public TRefCountPtr<IDirect3DSurface9>
//{
//public:
//	/** Dedicated texture when not rendering directly to resolve target texture surface */
//	FD3D9Texture2DPtr		Texture2D;
//	/** 2d texture to resolve surface to */
//	FD3D9Texture2DPtr		ResolveTargetTexture2D;
//
//	FD3D9Surface(
//		FD3D9Texture2DPtr InResolveTargetTexture,
//		FD3D9Texture2DPtr InTexture2D = NULL, 
//		IDirect3DSurface9* InSurface = NULL)
//		: 
//		ResolveTargetTexture2D(InResolveTargetTexture),
//		Texture2D(InTexture2D), 
//		TRefCountPtr<IDirect3DSurface9>(InSurface)
//	{}
//};
//typedef TRefCountPtr<FD3D9Surface>				FD3D9SurfacePtr;


class FD3D9BoundShaderState : public FRefCountedObject
{
public:
	TRefCountPtr<IDirect3DVertexDeclaration9>	VertexDeclaration;
	TRefCountPtr<IDirect3DVertexShader9>		VertexShader;
	TRefCountPtr<IDirect3DPixelShader9>			PixelShader;
	
	virtual DWORD Release() const
	{
		if( VertexDeclaration )
			VertexDeclaration->Release();
		if( VertexShader )
			VertexShader->Release();
		if( PixelShader )
			PixelShader->Release();

		DWORD Refs = DWORD(--NumRefs);
		if( Refs == 0 )
		{
			delete this;
		}
		return Refs;
	}
	virtual DWORD AddRef() const
	{
		if( VertexDeclaration )
			VertexDeclaration->AddRef();
		if( VertexShader )
			VertexShader->AddRef();
		if( PixelShader )
			PixelShader->AddRef();

		return DWORD(++NumRefs);
	}

	UBOOL operator== (const FD3D9BoundShaderState& Other) const
	{
		return (VertexDeclaration == Other.VertexDeclaration && VertexShader == Other.VertexShader && PixelShader == Other.PixelShader);
	}
};
typedef TRefCountPtr<FD3D9BoundShaderState>		FD3D9BoundShaderStatePtr;

#endif