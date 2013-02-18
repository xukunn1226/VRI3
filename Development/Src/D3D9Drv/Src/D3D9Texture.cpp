#include "D3D9DrvPCH.h"

static DWORD GetD3DTextureUsageFlags(DWORD Flags)
{
	DWORD D3DUsageFlags = 0;
	if( Flags & TexCreate_ResolveTarget )
	{
		D3DUsageFlags |= D3DUSAGE_RENDERTARGET;
	}
	if( Flags & TexCreate_DepthStencil )
	{
		D3DUsageFlags |= D3DUSAGE_DEPTHSTENCIL;
	}
	return D3DUsageFlags;
}

static D3DPOOL GetD3DTexturePool(DWORD Flags)
{
	if( Flags & (TexCreate_ResolveTarget | TexCreate_DepthStencil) )
	{
		return D3DPOOL_DEFAULT;
	}
	else
	{
		return D3DPOOL_MANAGED;
	}
}

FD3D9Texture2DPtr FD3D9DynamicDrv::CreateTexture2D(UINT SizeX, UINT SizeY, BYTE Format, UINT NumMips, DWORD Flags)
{
	FD3D9Texture2D* Texture = new FD3D9Texture2D( Flags&TexCreate_SRGB, Flags&TexCreate_Dynamic);
	VERIFYD3DCREATETEXTURERESULT( _Direct3DDevice->CreateTexture(	
										SizeX, 
										SizeY, 
										NumMips, 
										GetD3DTextureUsageFlags(Flags), 
										(D3DFORMAT)GPixelFormats[Format].PlatformFormat,
										GetD3DTexturePool(Flags), 
										Texture->GetInitD3DResource(), 
										NULL),
										SizeX, SizeY, Format, NumMips, GetD3DTextureUsageFlags(Flags));
	return Texture;
}

void* FD3D9DynamicDrv::LockTexture2D(FD3D9Texture2DPtr Texture, UINT MipIndex, UBOOL bIsDataBeingWrittenTo, UINT &DestStride)
{
	D3DLOCKED_RECT LockedRect;
	DWORD LockFlags = D3DLOCK_NOSYSLOCK;
	if( !bIsDataBeingWrittenTo )
	{
		LockFlags |= D3DLOCK_READONLY;
	}
	if( Texture->IsDynamic() )
	{
		LockFlags |= D3DLOCK_DISCARD;
	}

	VERIFYD3DRESULT(Texture->GetD3DResource()->LockRect(MipIndex, &LockedRect, NULL, LockFlags));
	DestStride = LockedRect.Pitch;
	return LockedRect.pBits;
}

void FD3D9DynamicDrv::UnlockTexture2D(FD3D9Texture2DPtr Texture, UINT MipIndex)
{
	VERIFYD3DRESULT(Texture->GetD3DResource()->UnlockRect(MipIndex));
}

FD3D9TextureCubePtr FD3D9DynamicDrv::CreateTextureCube(UINT Size, BYTE Format, UINT NumMips, DWORD Flags)
{
	FD3D9TextureCubePtr Texture = new FD3D9TextureCube(Flags&TexCreate_SRGB, Flags&TexCreate_Dynamic);
	VERIFYD3DRESULT( _Direct3DDevice->CreateCubeTexture(	Size, 
											NumMips,
											GetD3DTextureUsageFlags(Flags),
											(D3DFORMAT)GPixelFormats[Format].PlatformFormat,
											GetD3DTexturePool(Flags),
											Texture->GetInitD3DResource(),
											NULL));
	return Texture;
}

void* FD3D9DynamicDrv::LockTextureCubeFace(FD3D9TextureCubePtr TextureCube, UINT FaceIndex, UINT MipIndex, UINT &DstStride)
{
	D3DLOCKED_RECT LockedRect;
	VERIFYD3DRESULT( TextureCube->GetD3DResource()->LockRect( (D3DCUBEMAP_FACES)FaceIndex, MipIndex, &LockedRect, NULL, 0 ) );
	DstStride = LockedRect.Pitch;
	return LockedRect.pBits;
}

void FD3D9DynamicDrv::UnlockTextureCubeFace(FD3D9TextureCubePtr TextureCube, UINT FaceIndex, UINT MipIndex)
{
	VERIFYD3DRESULT( TextureCube->GetD3DResource()->UnlockRect( (D3DCUBEMAP_FACES)FaceIndex, MipIndex ) );
}

/**
* @param BaseSizeX - width of the texture (base level)
* @param BaseSizeY - height of the texture (base level)
*/
void FD3D9DynamicDrv::CopyTexture2D(FD3D9Texture2DPtr DstTexture2D, UINT MipIndex, INT BaseSizeX, INT BaseSizeY, INT Format, FD3D9Texture2DPtr SrcTexture2D)
{
	INT MipSizeX = Max((INT)GPixelFormats[Format].BlockSizeX, BaseSizeX >> MipIndex);
	INT MipSizeY = Max((INT)GPixelFormats[Format].BlockSizeY, BaseSizeY >> MipIndex);

	UINT DstStride;
	BYTE* DstData = (BYTE*)LockTexture2D(DstTexture2D, MipIndex, TRUE, DstStride);

	{
		UINT SrcStride;
		BYTE* SrcData = (BYTE*)LockTexture2D(SrcTexture2D, MipIndex, FALSE, SrcStride);

		DWORD PitchBytes = (MipSizeX / GPixelFormats[Format].BlockSizeX) * GPixelFormats[Format].BlockBytes;

		DWORD SizeBytes = PitchBytes * MipSizeY;

		appMemcpy(DstData, SrcData, SizeBytes);

		UnlockTexture2D(SrcTexture2D, MipIndex);
	}

	UnlockTexture2D(DstTexture2D, MipIndex);
}

/**
* Note that the mip must be the same size and of the same format
*/
void FD3D9DynamicDrv::CopyMipToMipAsync(FD3D9Texture2DPtr SrcTexture2D, INT SrcMipIndex, FD3D9Texture2DPtr DstTexture2D, INT DstMipIndex, INT Size)
{
	UINT SrcStride;
	UINT DstStride;

	void* SrcData = LockTexture2D(SrcTexture2D, SrcMipIndex, FALSE, SrcStride);
	void* DstData = LockTexture2D(DstTexture2D, DstMipIndex, TRUE, DstStride);
	check( SrcStride == DstStride );
	appMemcpy(DstData, SrcData, Size);
	UnlockTexture2D(SrcTexture2D, SrcMipIndex);
	UnlockTexture2D(DstTexture2D, DstMipIndex);
}
