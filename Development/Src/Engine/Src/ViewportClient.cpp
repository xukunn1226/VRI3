#include "EnginePCH.h"

IMPLEMENT_CLASS(VClient);

FViewport::FViewport(FViewportClient* InViewportClient)
: _ViewportClient(InViewportClient),
  _SizeX(0), _SizeY(0), 
  _bIsFullscreen(FALSE), 
  _D3DViewport(NULL)
{
}

void FViewport::BeginRenderFrame()
{
	GDynamicDrv->BeginDrawingViewport(GetD3DViewport());
	UpdateRenderTargetSurfaceRHIToCurrentBackBuffer();
}

void FViewport::EndRenderFrame(UBOOL bPresent)
{
	GDynamicDrv->EndDrawingViewport(GetD3DViewport(), bPresent);
}

void FViewport::UpdateRenderTargetSurfaceRHIToCurrentBackBuffer()
{
	_RenderTargetSurface = GDynamicDrv->GetViewportBackbuffer(GetD3DViewport());
}

void FViewport::Draw(UBOOL bPresent)
{
	BeginRenderFrame();

	if( _ViewportClient )
	{
		_ViewportClient->Draw(this);
	}

	EndRenderFrame(bPresent);
}

/**
 * Viewport的创建、更新、销毁接口
 */
void FViewport::UpdateViewport(UBOOL bDestroyed, UINT NewSizeX, UINT NewSizeY, UBOOL bIsFullscreen)
{
	_SizeX = NewSizeX;
	_SizeY = NewSizeY;
	_bIsFullscreen = bIsFullscreen;

	BeginReleaseResource(this);

	if( bDestroyed )
	{
		if( IsValidRef(_D3DViewport) )
		{
			_D3DViewport.SafeRelease();
		}
	}
	else
	{
		if( IsValidRef(_D3DViewport) )
		{
			GDynamicDrv->ResizeViewport(_D3DViewport, _SizeX, _SizeY, _bIsFullscreen);
		}
		else
		{
			_D3DViewport = GDynamicDrv->CreateViewport(GetWindow(), _SizeX, _SizeY, _bIsFullscreen);
		}

		BeginInitResource(this);
	}
}

void FViewport::InitDynamicDrv()
{
	UpdateRenderTargetSurfaceRHIToCurrentBackBuffer();
}

void FViewport::ReleaseDynamicDrv()
{
	_RenderTargetSurface.SafeRelease();
}
