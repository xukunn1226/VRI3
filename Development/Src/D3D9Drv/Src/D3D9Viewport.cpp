#include "D3D9DrvPCH.h"

FD3D9Viewport::FD3D9Viewport(FD3D9DynamicDrv *D3DDrv, void *WindowHandle, UINT SizeX, UINT SizeY, UBOOL IsFullscreen)
:	_D3DDrv(D3DDrv), 
	_WindowHandle(WindowHandle), 
	_SizeX(SizeX), 
	_SizeY(SizeY), 
	_bFullscreen(IsFullscreen)
{
	_D3DDrv->_Viewports.push_back(this);
	_D3DDrv->UpdateD3DDeviceFromViewport();
}

FD3D9Viewport::~FD3D9Viewport()
{
	vector<FD3D9Viewport*>::iterator it;
	for( it = _D3DDrv->_Viewports.begin(); it != _D3DDrv->_Viewports.end(); ++it )
	{
		if( (*it) == this )
		{
			_D3DDrv->_Viewports.erase(it);
			break;
		}
	}
	_D3DDrv->UpdateD3DDeviceFromViewport();
}

void FD3D9Viewport::Resize(UINT SizeX, UINT SizeY, UBOOL bFullscreen)
{
	_SizeX = SizeX;
	_SizeY = SizeY;
	_bFullscreen = bFullscreen;
	_D3DDrv->UpdateD3DDeviceFromViewport();
}

//------------------------------------------------------------------------------------------
FD3D9ViewportPtr FD3D9DynamicDrv::CreateViewport(void *WindowHandle, UINT SizeX, UINT SizeY, UBOOL bIsFullscreen)
{
	return new FD3D9Viewport(this, WindowHandle, SizeX, SizeY, bIsFullscreen);
}

void FD3D9DynamicDrv::ResizeViewport(FD3D9ViewportPtr Viewport, UINT SizeX, UINT SizeY, UBOOL bIsFullscreen)
{
	Viewport->Resize(SizeX, SizeY, bIsFullscreen);
}

void FD3D9DynamicDrv::Tick(FLOAT DeltaTime)
{
	if( _bDeviceLost )
	{
		UpdateD3DDeviceFromViewport();
	}
}

FD3D9SurfacePtr FD3D9DynamicDrv::GetViewportBackbuffer(FD3D9ViewportPtr Viewport)
{
	return _BackBuffer;
}

UBOOL FD3D9DynamicDrv::IsDrawingViewport()
{
	return _DrawingViewport != NULL;
}

void FD3D9DynamicDrv::BeginDrawingViewport(FD3D9ViewportPtr Viewport)
{
	check( !_DrawingViewport );
	_DrawingViewport = Viewport;

	_Direct3DDevice->BeginScene();

	for(UINT SamplerIndex = 0; SamplerIndex < 16; ++SamplerIndex)
	{
		_Direct3DDevice->SetSamplerState(SamplerIndex, D3DSAMP_MAXANISOTROPY, 1);		// temporary setting
	}

	SetRenderTarget(_BackBuffer, NULL);
}

void FD3D9DynamicDrv::EndDrawingViewport(FD3D9ViewportPtr Viewport, UBOOL bPresent)
{
	check( _DrawingViewport.GetReference() == Viewport.GetReference() );
	_DrawingViewport = NULL;

	_Direct3DDevice->SetRenderTarget(0, _BackBuffer->GetD3DResource());
	_Direct3DDevice->SetDepthStencilSurface(NULL);

	for(UINT TextureIndex = 0; TextureIndex < 16; ++TextureIndex)
	{
		_Direct3DDevice->SetTexture(TextureIndex, NULL);
	}

	_Direct3DDevice->SetVertexShader(NULL);

	for(UINT StreamIndex = 0; StreamIndex < 16; ++StreamIndex)
	{
		_Direct3DDevice->SetStreamSource(StreamIndex, NULL, 0, 0);
		_Direct3DDevice->SetStreamSourceFreq(StreamIndex, 1);
	}

	_Direct3DDevice->SetIndices(NULL);
	_Direct3DDevice->SetPixelShader(NULL);

	_Direct3DDevice->EndScene();

	if( bPresent )
	{
		HRESULT Result = S_OK;
		if( Viewport->IsFullscreen() )
		{
			Result = _Direct3DDevice->Present(NULL, NULL, NULL, NULL);
		}
		else
		{
			RECT DstRect;
			if( GetClientRect((HWND)Viewport->GetWindowHandle(), &DstRect) )
			{
				RECT SourceRect;
				SourceRect.left = SourceRect.top = 0;
				SourceRect.right = Viewport->GetSizeX();
				SourceRect.bottom = Viewport->GetSizeY();
				if( DstRect.right > 0 && DstRect.bottom > 0 )
				{
					Result = _Direct3DDevice->Present(&SourceRect, NULL, (HWND)Viewport->GetWindowHandle(), NULL);
				}
			}
		}

		if( Result == D3DERR_DEVICELOST || Result == E_FAIL )
		{
			_bDeviceLost = TRUE;
		}
		else
		{
			VERIFYD3DRESULT(Result);
		}
	}
}