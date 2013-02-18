#include "D3D9DrvPCH.h"

FD3D9SurfacePtr FD3D9DynamicDrv::CreateTargetableSurface(
	UINT SizeX, 
	UINT SizeY, 
	BYTE Format, 
	DWORD Flags)
{
	UBOOL bDepthFormat = (Format == PF_DepthStencil || Format == PF_ShadowDepth || Format == PF_FilteredShadowDepth || Format == PF_D24);

	if( Flags&TargetSurfCreate_Dedicated )
	{
		IDirect3DTexture9* D3DTargetableTexture;
		VERIFYD3DRESULT(_Direct3DDevice->CreateTexture(
			SizeX, 
			SizeY,
			1,
			bDepthFormat ? D3DUSAGE_DEPTHSTENCIL : D3DUSAGE_RENDERTARGET,
			(D3DFORMAT)GPixelFormats[Format].PlatformFormat,
			D3DPOOL_DEFAULT,
			&D3DTargetableTexture,
			NULL));
		FD3D9Texture2D* TargetableTexture = new FD3D9Texture2D(FALSE, FALSE, D3DTargetableTexture);

		IDirect3DSurface9*	D3DTargetableSurface;
		VERIFYD3DRESULT(D3DTargetableTexture->GetSurfaceLevel(0, &D3DTargetableSurface));

		return new FD3D9Surface(TargetableTexture, D3DTargetableSurface);
	}
	else
	{
		if( bDepthFormat )
		{
			IDirect3DSurface9*	D3DTargetableSurface;
			VERIFYD3DRESULT(_Direct3DDevice->CreateDepthStencilSurface(
				SizeX,
				SizeY,
				(D3DFORMAT)GPixelFormats[Format].PlatformFormat,
				D3DMULTISAMPLE_NONE,
				0,
				TRUE,
				&D3DTargetableSurface,
				NULL));

			return new FD3D9Surface(NULL, D3DTargetableSurface);
		}
		else
		{
			IDirect3DSurface9*	D3DTargetableSurface;
			VERIFYD3DRESULT(_Direct3DDevice->CreateRenderTarget(
				SizeX,
				SizeY,
				(D3DFORMAT)GPixelFormats[Format].PlatformFormat,
				D3DMULTISAMPLE_NONE,
				0,
				(Flags&TargetSurfCreate_Readable),		// lockable flag
				&D3DTargetableSurface,
				NULL));

			return new FD3D9Surface(NULL, D3DTargetableSurface);
		}
	}
	return NULL;
}

FD3D9SurfacePtr FD3D9DynamicDrv::CreateTargetableCubeSurface(UINT Size, BYTE Format, FD3D9TextureCubePtr TargetTexture, ECubeFace CubeFace, DWORD Flags)
{
	//if( !TargetTexture )
	//{
	//	check(TEXT("No Target cube texture specified!"));
	//}
	//else
	//{
	//	checkf(!(Flags&TargetSurfCreate_Readable), TEXT("Cannot allocate resolvable surfaces with the readable flag"));

	//	TRefCountPtr<IDirect3DSurface9> Surface;
	//	VERIFYD3DRESULT((*TargetTexture)->GetCubeMapSurface(GetD3DCubeFace(CubeFace), 0, Surface.GetInitReference()));

	//	return new FD3D9Surface(TargetTexture, Surface.GetReference());
	//}

	return NULL;
}

/**
 * Copy the contents of the source rectangle to the destination rectangle
 * WARNING: no support for Depth and Stencil Surface
 */
void FD3D9DynamicDrv::CopyToResolveTarget(FD3D9SurfacePtr SourceSurface, FD3D9SurfacePtr DestinationSurface)
{
	if( SourceSurface->_Texture2D && DestinationSurface->_Texture2D
		&& SourceSurface->_Texture2D != DestinationSurface->_Texture2D )
	{
		D3DSURFACE_DESC	SrcSurfaceDesc;
		D3DSURFACE_DESC DstSurfaceDesc;

		VERIFYD3DRESULT(SourceSurface->GetD3DResource()->GetDesc(&SrcSurfaceDesc));
		VERIFYD3DRESULT(DestinationSurface->GetD3DResource()->GetDesc(&DstSurfaceDesc));

		check(SrcSurfaceDesc.Pool == D3DPOOL_DEFAULT && DstSurfaceDesc.Pool == D3DPOOL_DEFAULT);

		//UBOOL bDepthUsage = (SrcSurfaceDesc.Usage == D3DUSAGE_DEPTHSTENCIL);

		//if( bDepthUsage )
		//{
		//	// The source and destination surfaces must be plain depth stencil surfaces (not textures)
		//	check( !SourceSurface->Texture2D && !DestSurface->Texture2D);

		//}

		VERIFYD3DRESULT(_Direct3DDevice->StretchRect((IDirect3DSurface9*)SourceSurface->GetD3DResource(), 
						NULL, 
						(IDirect3DSurface9*)DestinationSurface->GetD3DResource(),
						NULL, 
						D3DTEXF_NONE));
	}
}

void FD3D9DynamicDrv::ReadSurfaceData(FD3D9SurfacePtr InSurface, UINT MinX, UINT MinY, UINT MaxX, UINT MaxY, std::vector<BYTE> &OutData, ECubeFace CubeFace)
{
	UINT SizeX = MaxX - MinX + 1;
	UINT SizeY = MaxY - MinY + 1;

	D3DSURFACE_DESC SurfaceDesc;
	VERIFYD3DRESULT(InSurface->GetD3DResource()->GetDesc(&SurfaceDesc));

	check(SurfaceDesc.Format == D3DFMT_A8R8G8B8 || SurfaceDesc.Format == D3DFMT_A16B16G16R16F);

	// create a temp 2d texture to copy rendertarget to
	IDirect3DTexture9* Texture2D;
	VERIFYD3DRESULT(_Direct3DDevice->CreateTexture(
		SurfaceDesc.Width,
		SurfaceDesc.Height,
		1,
		0,
		SurfaceDesc.Format,
		D3DPOOL_SYSTEMMEM,
		&Texture2D,
		NULL));

	IDirect3DSurface9*	DestSurface;
	VERIFYD3DRESULT(Texture2D->GetSurfaceLevel(0, &DestSurface));

	// copy render target data to memory
	VERIFYD3DRESULT(_Direct3DDevice->GetRenderTargetData(InSurface->GetD3DResource(), DestSurface));


	OutData.resize(SizeX * SizeY * sizeof(FLinearColor));

	D3DLOCKED_RECT	LockedRect;
	RECT Rect;
	Rect.top	= MinY;
	Rect.bottom = MaxY + 1;
	Rect.left	= MinX;
	Rect.right	= MaxX + 1;

	VERIFYD3DRESULT(DestSurface->LockRect(&LockedRect, &Rect, D3DLOCK_READONLY));

	if( SurfaceDesc.Format == D3DFMT_A8R8G8B8 )
	{
		for(UINT Y = MinY; Y <= MaxY; ++Y)
		{
			BYTE* SrcData = (BYTE*)LockedRect.pBits + (Y - MinY) * LockedRect.Pitch;
			BYTE* DstData = (BYTE*)&OutData + (Y - MinY) * SizeX * sizeof(FLinearColor);
			appMemcpy(DstData, SrcData, SizeX * sizeof(FLinearColor));
		}
	}
	else if( SurfaceDesc.Format == D3DFMT_A16B16G16R16F )
	{
		FPlane MinValue(0.0f, 0.0f, 0.0f, 0.0f);
		FPlane MaxValue(1.0f, 1.0f, 1.0f, 1.0f);

		for(UINT Y = MinY; Y <= MaxY; ++Y)
		{
			WORD* SrcPtr = (WORD*)((BYTE*)LockedRect.pBits + (Y - MinY) * LockedRect.Pitch);

			for(UINT X = MinX; X <= MaxX; ++X)
			{
				MinValue.x = Min<FLOAT>(SrcPtr[0], MinValue.x);
				MinValue.y = Min<FLOAT>(SrcPtr[1], MinValue.y);
				MinValue.z = Min<FLOAT>(SrcPtr[2], MinValue.z);
				MinValue.d = Min<FLOAT>(SrcPtr[3], MinValue.d);

				MaxValue.x = Max<FLOAT>(SrcPtr[0], MaxValue.x);
				MaxValue.y = Max<FLOAT>(SrcPtr[1], MaxValue.y);
				MaxValue.z = Max<FLOAT>(SrcPtr[2], MaxValue.z);
				MaxValue.d = Max<FLOAT>(SrcPtr[3], MaxValue.d);

				SrcPtr += 4;
			}
		}

		for(UINT Y = MinY; Y <= MaxY; ++Y)
		{
			WORD* SrcPtr = (WORD*)((BYTE*)LockedRect.pBits + (Y - MinY) * LockedRect.Pitch/* + MinX * sizeof(WORD) * 4*/);
			FLinearColor* DstPtr = (FLinearColor*)((BYTE*)&OutData + (Y - MinY) * SizeX * sizeof(FLinearColor));

			for(UINT X = MinX; X <= MaxX; ++X)
			{
				FLinearColor NormalizedColor(
					SrcPtr[0] * 255.0f / (MaxValue.x - MinValue.x),
					SrcPtr[1] * 255.0f / (MaxValue.y - MinValue.y),
					SrcPtr[2] * 255.0f / (MaxValue.z - MinValue.z),
					SrcPtr[3] * 255.0f / (MaxValue.d - MinValue.d));

				appMemcpy(DstPtr++, &NormalizedColor, sizeof(FLinearColor));
				SrcPtr += 4;
			}
		}
	}

	VERIFYD3DRESULT(DestSurface->UnlockRect());

	Texture2D->Release();
	DestSurface->Release();
}