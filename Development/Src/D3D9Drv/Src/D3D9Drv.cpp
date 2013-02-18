#include "D3D9DrvPCH.h"

FDynamicDrv* D3D9CreateDrv()
{
	return new FD3D9DynamicDrv();
}

FD3D9DynamicDrv::FD3D9DynamicDrv()
:	_bDeviceLost(FALSE),
	_DeviceSizeX(0),
	_DeviceSizeY(0),
	_DeviceWindow(NULL),
	_bIsFullscreen(FALSE),
	_VertexDeclarationCache(this)
{
	*_Direct3D.GetInitReference() = Direct3DCreate9(D3D_SDK_VERSION);

	if( !_Direct3D )
	{
		debugf(TEXT("Init DirectX SDK Failed!"));
		appErrorf(TEXT("Please install DirectX 9.0c or later"));
	}

	if( !D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION) )
	{
		appErrorf(TEXT("The D3DX9 Version does not match what the application was built with (%d)"), D3DX_SDK_VERSION);
	}

	UINT AdapterIndex = D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType = DEBUG_SHADERS ? D3DDEVTYPE_REF : D3DDEVTYPE_HAL;
	if( _Direct3D->GetDeviceCaps(AdapterIndex, DeviceType, &_DeviceCaps) == D3D_OK )
	{
		debugf(TEXT("Pixel shader version is %0x"), _DeviceCaps.PixelShaderVersion);
		if( (_DeviceCaps.PixelShaderVersion & 0xff00) < 0x0200 )
		{
			debugf(TEXT("Required at least Shader Model 2.0"));
		}

		GVertexElementTypeSupport.SetSupported(VET_UByte4,	(_DeviceCaps.DeclTypes & D3DDTCAPS_UBYTE4) ? TRUE : FALSE);
		GVertexElementTypeSupport.SetSupported(VET_UByte4N, (_DeviceCaps.DeclTypes & D3DDTCAPS_UBYTE4N) ? TRUE : FALSE);
		GVertexElementTypeSupport.SetSupported(VET_Short2N, (_DeviceCaps.DeclTypes & D3DDTCAPS_SHORT2N) ? TRUE : FALSE);
		GVertexElementTypeSupport.SetSupported(VET_Half2,	(_DeviceCaps.DeclTypes & D3DDTCAPS_FLOAT16_2) ? TRUE : FALSE);

		UBOOL IsSupportShaderModel3 = (_DeviceCaps.PixelShaderVersion & 0xff00) >= 0x0300;

		if( !IsSupportShaderModel3 )
		{
			GShaderPlatform = SP_SM2;
		}
	}

	D3DDISPLAYMODE DisplayMode;
	VERIFYD3DRESULT(_Direct3D->GetAdapterDisplayMode(AdapterIndex, &DisplayMode));

	// Determines whether a surface format is available as a specified resource type and can be used as a texture, 
	// depth-stencil buffer, or render target, or any combination of the three, on a device representing this adapter
	if( FAILED(_Direct3D->CheckDeviceFormat(	
				AdapterIndex, 
				DeviceType,
				DisplayMode.Format,
				D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
				D3DRTYPE_TEXTURE,
				D3DFMT_A16B16G16R16F)) )
	{
		GShaderPlatform = SP_SM2;
	}

	if ( FAILED(_Direct3D->CheckDeviceFormat(
				AdapterIndex,
				DeviceType,
				DisplayMode.Format,
				D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
				D3DRTYPE_TEXTURE,
				D3DFMT_A32B32G32R32F)))
	{
		GPixelFormats[PF_A32B32G32R32F].Supported = FALSE;
	}

	GPixelFormats[PF_Unknown			].PlatformFormat = D3DFMT_UNKNOWN;
	GPixelFormats[PF_A32B32G32R32F		].PlatformFormat = D3DFMT_A32B32G32R32F;
	GPixelFormats[PF_A8R8G8B8			].PlatformFormat = D3DFMT_A8R8G8B8;
	GPixelFormats[PF_G8					].PlatformFormat = D3DFMT_L8;
	GPixelFormats[PF_DXT1				].PlatformFormat = D3DFMT_DXT1;
	GPixelFormats[PF_DXT3				].PlatformFormat = D3DFMT_DXT3;
	GPixelFormats[PF_DXT5				].PlatformFormat = D3DFMT_DXT5;
	GPixelFormats[PF_DepthStencil		].PlatformFormat = D3DFMT_D24S8;
	GPixelFormats[PF_ShadowDepth		].PlatformFormat = D3DFMT_D24X8;
	GPixelFormats[PF_FilteredShadowDepth].PlatformFormat = D3DFMT_D24X8;
	GPixelFormats[PF_R32F				].PlatformFormat = D3DFMT_R32F;
	GPixelFormats[PF_G16R16				].PlatformFormat = D3DFMT_G16R16;
	GPixelFormats[PF_G16R16F			].PlatformFormat = D3DFMT_G16R16F;
	GPixelFormats[PF_G32R32F			].PlatformFormat = D3DFMT_G32R32F;
	GPixelFormats[PF_D24				].PlatformFormat = (D3DFORMAT)(MAKEFOURCC('D','F','2','4'));
}

FD3D9DynamicDrv::~FD3D9DynamicDrv()
{
	GIsDrvInitialized = FALSE;
}

void FD3D9DynamicDrv::UpdateD3DDeviceFromViewport()
{
	if( _Viewports.size() )
	{
		check(_Direct3D);

		FD3D9Viewport* NewFullscreenViewport = NULL;
		UINT	MaxViewportSizeX = 0,
				MaxViewportSizeY = 0;

		for(UINT ViewportIndex = 0; ViewportIndex < _Viewports.size(); ++ViewportIndex)
		{
			FD3D9Viewport* D3DViewport = _Viewports.at(ViewportIndex);

			MaxViewportSizeX = Max(MaxViewportSizeX, D3DViewport->GetSizeX());
			MaxViewportSizeY = Max(MaxViewportSizeY, D3DViewport->GetSizeY());

			if( D3DViewport->IsFullscreen() )
			{
				check(!NewFullscreenViewport);
				NewFullscreenViewport = D3DViewport;

				UINT Width = D3DViewport->GetSizeX();
				UINT Height = D3DViewport->GetSizeY();
				GetSupportedResolution(Width, Height);
				check( Width == D3DViewport->GetSizeX() && Height == D3DViewport->GetSizeY() );
			}
		}

		UINT AdapterIndex = D3DADAPTER_DEFAULT;
		D3DDEVTYPE DeviceType = DEBUG_SHADERS ? D3DDEVTYPE_REF : D3DDEVTYPE_HAL;

		UINT NewDeviceSizeX = MaxViewportSizeX;
		UINT NewDeviceSizeY = MaxViewportSizeY;
		HWND NewDeviceWindow = NewFullscreenViewport ? (HWND)NewFullscreenViewport->GetWindowHandle() : (HWND)_Viewports.at(0)->GetWindowHandle();
		UBOOL NewDeviceFullscreen = NewFullscreenViewport ? TRUE : FALSE;

		// 判断D3DDevice是否需要重建
		// 1、no 3d device；2、lost device；3、switch to window mode or vice versa;4、resize
		UBOOL bRecreateDevice = FALSE;
		if( !_Direct3DDevice )
		{
			bRecreateDevice = TRUE;
		}
		else
		{
			if( _bDeviceLost )
			{
				HWND FocusWindow = ::GetFocus();
				if( FocusWindow == NULL || ::IsIconic(FocusWindow) )	// determines whether the specified window is minimized
				{
					return;
				}
				FocusWindow = ::GetFocus();
				bRecreateDevice = TRUE;
			}

			if( !GIsRequestingExit )
			{
				if( NewDeviceFullscreen != _bIsFullscreen )
				{
					bRecreateDevice = TRUE;
				}

				if( NewDeviceFullscreen )
				{
					if( _DeviceSizeX != NewDeviceSizeX || _DeviceSizeY != NewDeviceSizeY )
					{
						bRecreateDevice = TRUE;
					}
				}
				else
				{
					if( _DeviceSizeX < NewDeviceSizeX || _DeviceSizeY < NewDeviceSizeY )
					{
						bRecreateDevice = TRUE;
					}
				}

				if( _DeviceWindow != NewDeviceWindow )
				{
					bRecreateDevice = TRUE;
				}
			}
		}

		if( bRecreateDevice )
		{
			HRESULT Result;

			D3DPRESENT_PARAMETERS PresentParameters;
			appMemzero(&PresentParameters, sizeof(D3DPRESENT_PARAMETERS));
			PresentParameters.BackBufferCount			= 1;
			PresentParameters.BackBufferFormat			= D3DFMT_A8R8G8B8;
			PresentParameters.BackBufferWidth			= NewDeviceSizeX;
			PresentParameters.BackBufferHeight			= NewDeviceSizeY;
			PresentParameters.SwapEffect				= NewDeviceFullscreen ? D3DSWAPEFFECT_DISCARD : D3DSWAPEFFECT_COPY;
			PresentParameters.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
			PresentParameters.EnableAutoDepthStencil	= FALSE;
			//PresentParameters.AutoDepthStencilFormat	= D3DFMT_D16;
			PresentParameters.Windowed					= !NewDeviceFullscreen;
			PresentParameters.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
			PresentParameters.hDeviceWindow				= NewDeviceWindow;

			// meaning lost device
			if( _Direct3DDevice )
			{
				// Release dynamic resources and render targets.
				for(list<FRenderResource*>::iterator it = FRenderResource::GetResourceList()->begin(); it != FRenderResource::GetResourceList()->end() ; ++it)
				{
					(*it)->ReleaseDynamicDrv();
				}

				// for global scene render target
				{
				}

				_BackBuffer = NULL;

				do
				{
					if( FAILED(Result = _Direct3DDevice->Reset(&PresentParameters)) )
					{
						appSleep(1.0f);
					}
				}
				while(FAILED(Result));

				// recreate the dynamic resource and render target
				IDirect3DSurface9* D3DBackBuffer;
				VERIFYD3DRESULT(_Direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &D3DBackBuffer));
				_BackBuffer = new FD3D9Surface((FD3D9Texture2D*)NULL, D3DBackBuffer);

				//// Reinitialize dynamic resources and render targets.
				for(list<FRenderResource*>::iterator it = FRenderResource::GetResourceList()->begin(); it != FRenderResource::GetResourceList()->end() ; ++it)
				{
					(*it)->InitDynamicDrv();
				}
			}
			else
			{
				INT MipCount = 0;
				while( (1 << MipCount) <= Min<INT>(_DeviceCaps.MaxTextureHeight, _DeviceCaps.MaxTextureWidth) )
				{
					++MipCount;
				}
				GMaxTextureMipCount = MipCount;
				GMaxTextureMipCount = Min<INT>(GMaxTextureMipCount, MAX_TEXTURE_MIP_COUNT);

				if( (_DeviceCaps.PixelShaderVersion & 0xff00) < 0x0200 )
				{
					appErrorf(TEXT("Device does not support pixel shaders 2.0 or greater.  PixelShaderVersion=%08x"),_DeviceCaps.PixelShaderVersion);
				}

				GSupportsVertexInstancing = (_DeviceCaps.PixelShaderVersion & 0xff00) >= 0x0300;

				D3DDISPLAYMODE DisplayMode;
				VERIFYD3DRESULT(_Direct3D->GetAdapterDisplayMode(AdapterIndex, &DisplayMode));

				UBOOL bAllowFloatingPointRenderTarget = TRUE;
				if( FAILED(_Direct3D->CheckDeviceFormat(	AdapterIndex, 
															DeviceType,
															DisplayMode.Format,
															D3DUSAGE_RENDERTARGET,
															D3DRTYPE_TEXTURE,
															D3DFMT_A16B16G16R16F)) )
				{
					bAllowFloatingPointRenderTarget = FALSE;
				}

				GPixelFormats[PF_FloatRGB	].PlatformFormat	= bAllowFloatingPointRenderTarget ? D3DFMT_A16B16G16R16F : D3DFMT_A8R8G8B8;
				GPixelFormats[PF_FloatRGB	].BlockBytes		= bAllowFloatingPointRenderTarget ? 8 : 4;
				GPixelFormats[PF_FloatRGBA	].PlatformFormat	= bAllowFloatingPointRenderTarget ? D3DFMT_A16B16G16R16F : D3DFMT_A8R8G8B8;
				GPixelFormats[PF_FloatRGBA	].BlockBytes		= bAllowFloatingPointRenderTarget ? 8 : 4;

				GSupportHardwarePCF = SUCCEEDED(_Direct3D->CheckDeviceFormat(
															AdapterIndex,
															DeviceType,
															DisplayMode.Format,
															D3DUSAGE_DEPTHSTENCIL,
															D3DRTYPE_TEXTURE,
															D3DFMT_D24X8));

				if( GShaderPlatform != SP_SM2 )
				{
					GSupportsFetch4 = SUCCEEDED(_Direct3D->CheckDeviceFormat(
						AdapterIndex,
						DeviceType,
						DisplayMode.Format,
						D3DUSAGE_DEPTHSTENCIL,
						D3DRTYPE_TEXTURE,
						(D3DFORMAT)(MAKEFOURCC('D','F','2','4'))));
				}

				if( FAILED(_Direct3D->CheckDeviceFormat(AdapterIndex,
															DeviceType,
															DisplayMode.Format,
															D3DUSAGE_RENDERTARGET,
															D3DRTYPE_TEXTURE,
															D3DFMT_R32F)) )
				{
					appErrorf(TEXT("Device does not support 1x32 FP render target format."));
				}

				DWORD CreationFlags =	D3DCREATE_FPU_PRESERVE | 
										D3DCREATE_HARDWARE_VERTEXPROCESSING | 
										D3DCREATE_DISABLE_DRIVER_MANAGEMENT;

				while(1)
				{
					UINT AdapterNumber = D3DADAPTER_DEFAULT;
#if !SHIPPING
					for(UINT Index = 0; Index < _Direct3D->GetAdapterCount(); ++Index)
					{
						D3DADAPTER_IDENTIFIER9 Identifier;
						HRESULT res = _Direct3D->GetAdapterIdentifier(Index, 0, &Identifier);
						if( appStrstr(ANSI_TO_TCHAR(Identifier.Description), TEXT("PerfHUD")) != NULL )
						{
							AdapterNumber = Index;
							DeviceType = D3DDEVTYPE_REF;
							break;
						}
					}
#endif
					Result = _Direct3D->CreateDevice(
												AdapterNumber,
												DeviceType,
												NewDeviceWindow,
												CreationFlags,
												&PresentParameters,
												_Direct3DDevice.GetInitReference());

					if( Result != D3DERR_DEVICELOST )
					{
						break;
					}

					appSleep(0.5f);
				}

				VERIFYD3DRESULT(Result);

				if( FAILED(_Direct3D->CheckDeviceFormat(
												AdapterIndex,
												DeviceType,
												DisplayMode.Format,
												D3DUSAGE_RENDERTARGET,
												D3DRTYPE_TEXTURE,
												D3DFMT_G16R16)) )
				{
					GPixelFormats[PF_G16R16].PlatformFormat = D3DFMT_A8R8G8B8;
				}
				else
				{
					GPixelFormats[PF_G16R16].PlatformFormat = D3DFMT_G16R16;
				}

				if( FAILED(_Direct3D->CheckDeviceFormat(
												AdapterIndex,
												DeviceType,
												DisplayMode.Format,
												D3DUSAGE_RENDERTARGET,
												D3DRTYPE_TEXTURE,
												D3DFMT_A2B10G10R10)) )
				{
					GPixelFormats[PF_A2B10G10R10].PlatformFormat = D3DFMT_A8R8G8B8;
				}
				else
				{
					GPixelFormats[PF_A2B10G10R10].PlatformFormat = D3DFMT_A2B10G10R10;
				}

				if( !bAllowFloatingPointRenderTarget )
				{
					GPixelFormats[PF_A16B16G16R16].PlatformFormat = D3DFMT_A8R8G8B8;
				}
				else
				{
					GPixelFormats[PF_A16B16G16R16].PlatformFormat = D3DFMT_A16B16G16R16;
				}

				IDirect3DSurface9* D3DBackBuffer;
				VERIFYD3DRESULT(_Direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &D3DBackBuffer));
				_BackBuffer = new FD3D9Surface((FD3D9Texture2D*)NULL, D3DBackBuffer);

				check(!GIsDrvInitialized);
				GIsDrvInitialized = TRUE;
				// 通知所有尚未初始化的resource（当device尚未创建时建立的）
				for(list<FRenderResource*>::iterator it = FRenderResource::GetResourceList()->begin(); it != FRenderResource::GetResourceList()->end() ; ++it)
				{
					(*it)->InitDrv();
					(*it)->InitDynamicDrv();
				}

				_DeviceSizeX = NewDeviceSizeX;
				_DeviceSizeY = NewDeviceSizeY;
				_DeviceWindow = NewDeviceWindow;
				_bIsFullscreen = NewDeviceFullscreen;
				_bDeviceLost = FALSE;

				for(UINT ViewportIndex = 0; ViewportIndex < _Viewports.size(); ++ViewportIndex)
				{
					FD3D9Viewport* D3DViewport = _Viewports.at(ViewportIndex);
					::PostMessage((HWND)D3DViewport->GetWindowHandle(), WM_PAINT, 0, 0);
				}
			}
		}
	}
	else
	{
		CleanupD3DDevice();
		_BackBuffer = NULL;
		_Direct3DDevice = NULL;
		_Direct3D = NULL;
	}
}

void FD3D9DynamicDrv::CleanupD3DDevice()
{
	if( GIsDrvInitialized )
	{
		//release all initialized render resource
		for(list<FRenderResource*>::iterator it = FRenderResource::GetResourceList()->begin(); it != FRenderResource::GetResourceList()->end() ; ++it)
		{
			(*it)->ReleaseDrv();
			(*it)->ReleaseDynamicDrv();
		}

		GIsDrvInitialized = FALSE;
	}
}

void FD3D9DynamicDrv::GetSupportedResolution(UINT &Width, UINT &Height)
{
	check(_Direct3D);

	UINT AdapterIndex = D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType = DEBUG_SHADERS ? D3DDEVTYPE_REF : D3DDEVTYPE_HAL;

	D3DDISPLAYMODE BestMode = { 0, 0, 0, D3DFMT_A8R8G8B8 };
	UINT InitializedMode = FALSE;
	UINT NumAdapterModes = _Direct3D->GetAdapterModeCount(AdapterIndex, D3DFMT_X8R8G8B8);
	for(UINT ModeIndex = 0; ModeIndex < NumAdapterModes; ++ModeIndex)
	{
		D3DDISPLAYMODE DisplayMode;
		_Direct3D->EnumAdapterModes(AdapterIndex, D3DFMT_X8R8G8B8, ModeIndex, &DisplayMode);

		UBOOL bIsEqualOrBetterWidth = Abs((INT)DisplayMode.Width - (INT)Width) <= Abs((INT)BestMode.Width - (INT)Width);
		UBOOL bIsEqualOrBetterHeight = Abs((INT)DisplayMode.Height - (INT)Height) <= Abs((INT)BestMode.Height - (INT)Height);

		if( !InitializedMode || (bIsEqualOrBetterWidth && bIsEqualOrBetterHeight) )
		{
			BestMode = DisplayMode;
			InitializedMode = TRUE;
		}
	}

	check(InitializedMode);
	Width = BestMode.Width;
	Height = BestMode.Height;
}

UBOOL FD3D9DynamicDrv::GetAvailableResolutions(vector<FScreenResolution>& Resolutions, UBOOL bIgnoreRefreshRate)
{
	if( !_Direct3D )
	{
		return FALSE;
	}

	UINT AdapterIndex = D3DADAPTER_DEFAULT;
	UINT NumAdapterModes = _Direct3D->GetAdapterModeCount(AdapterIndex, D3DFMT_X8R8G8B8);
	for(UINT ModeIndex = 0; ModeIndex < NumAdapterModes; ++ModeIndex)
	{
		D3DDISPLAYMODE DisplayMode;
		HRESULT D3DResult = _Direct3D->EnumAdapterModes(AdapterIndex, D3DFMT_X8R8G8B8, ModeIndex, &DisplayMode);
		if( D3DResult == D3D_OK )
		{
			UBOOL bAddIt = TRUE;
			for(UINT CheckIndex = 0; CheckIndex < Resolutions.size(); ++CheckIndex)
			{
				FScreenResolution& CheckResolution = Resolutions[CheckIndex];
				if( CheckResolution.Width == DisplayMode.Width && CheckResolution.Height == DisplayMode.Height )
				{
					bAddIt = FALSE;
					break;
				}
			}

			if( bAddIt )
			{
				FScreenResolution Resolution;
				Resolution.Width = DisplayMode.Width;
				Resolution.Height = DisplayMode.Height;
				Resolution.RefreshRate = DisplayMode.RefreshRate;
				Resolutions.push_back(Resolution);
			}
		}
	}
	return TRUE;
}
