#ifndef VRI3_D3D9DRV_D3D9DRV_H_
#define VRI3_D3D9DRV_D3D9DRV_H_

#include <d3d9.h>
#include <d3dx9.h>

#include "D3D9Util.h"
#include "D3D9Resources.h"
#include "D3D9State.h"
#include "D3D9Viewport.h"

/** Reuses vertex declarations that are identical. */
class FD3D9VertexDeclarationCache
{
public:
	/** Key used to map a set of vertex element */
	class FKey
	{
	public:
		FKey(const FVertexDeclarationElementList& InElements);

		UBOOL operator== (const FKey& Other) const;

		//bool operator< (const FKey& Other) const;

		D3DVERTEXELEMENT9* GetVertexElements()
		{
			return &VertexElements.at(0);
		}

		const D3DVERTEXELEMENT9* GetVertexElements() const
		{
			return &VertexElements.at(0);
		}

		DWORD GetHash() const { return Hash; }

	private:
		vector<D3DVERTEXELEMENT9>	VertexElements;

		DWORD						Hash;
	};

	FD3D9VertexDeclarationCache(FD3D9DynamicDrv* InD3DDrv)
		: _D3DDrv(InD3DDrv)
	{}

	FD3D9VertexDeclarationPtr GetVertexDeclaration(const FKey& Declaration);

private:
	FD3D9DynamicDrv*	_D3DDrv;

	map<DWORD, FD3D9VertexDeclarationPtr>	_VertexDeclarationMap;
};

class FD3D9DynamicDrv : public FDynamicDrv
{
public:
	friend class FD3D9Viewport;

	FD3D9DynamicDrv();

	~FD3D9DynamicDrv();

	void UpdateD3DDeviceFromViewport();

	IDirect3DDevice9* GetDevice() const
	{
		return _Direct3DDevice;
	}

	// generic render state
	virtual FD3D9SamplerState CreateSamplerState(const FSamplerStateInitializer& Initializer);

	virtual FD3D9RasterizerState CreateRasterizerState(const FRasterizerStateInitializer& Initializer);

	virtual FD3D9DepthState CreateDepthState(const FDepthStateInitializer& Initializer);

	virtual FD3D9StencilState CreateStencilState(const FStencilStateInitializer& Initializer);

	virtual FD3D9BlendState CreateBlendState(const FBlendStateInitializer& Initializer);
	
	virtual void SetSamplerState(UINT SamplerIndex, FD3D9SamplerState SamplerState, FD3D9TexturePtr Texture, INT MaxMipLevel);

	virtual void SetRasterizerState(FD3D9RasterizerState RasterizerState);

	virtual void SetRasterizerStateImmediate(const FRasterizerStateInitializer& Initializer);

	virtual void SetDepthState(FD3D9DepthState DepthState);

	virtual void SetStencilState(FD3D9StencilState StencilState);

	virtual void SetBlendState(FD3D9BlendState BlendState);

	// operation about Vertex and Pixel Shader
	virtual FD3D9VertexDeclarationPtr CreateVertexDeclaration(const FVertexDeclarationElementList& Elements);

	virtual FD3D9VertexShaderPtr CreateVertexShader(const vector<BYTE>& Code);

	virtual FD3D9PixelShaderPtr CreatePixelShader(const vector<BYTE>& Code);

	virtual FD3D9BoundShaderStatePtr CreateBoundShaderState(const FD3D9VertexDeclarationPtr VertexDeclaration, const FD3D9VertexShaderPtr VertexShader, const FD3D9PixelShaderPtr PixelShader);

	virtual void SetBoundShaderState(FD3D9BoundShaderStatePtr BoundShaderState);

	// operation about Vertex and Index Buffer
	virtual FD3D9IndexBufferPtr CreateIndexBuffer(UINT Stride, UINT Size, void* ResourceData, UBOOL bIsDynamic);

	virtual void* LockIndexBuffer(FD3D9IndexBufferPtr IndexBuffer, UINT Offset, UINT Size);

	virtual void UnlockIndexBuffer(FD3D9IndexBufferPtr IndexBuffer);

	virtual FD3D9VertexBufferPtr CreateVertexBuffer(UINT Size, void* ResourceData, UBOOL bIsDynamic);

	virtual void* LockVertexBuffer(FD3D9VertexBufferPtr VertexBuffer, UINT Offset, UINT Size, UBOOL bReadOnly);

	virtual void UnlockVertexBuffer(FD3D9VertexBufferPtr VertexBuffer);

	virtual void SetVertexShaderParameter(FD3D9VertexShaderPtr VertexShader, UINT BaseIndex, UINT NumBytes, const void* NewValue);

	virtual void SetVertexShaderBoolParameter(FD3D9VertexShaderPtr VertexShader, UINT BaseIndex, UBOOL NewValue);

	virtual void SetPixelShaderParameter(FD3D9PixelShaderPtr PixelShader, UINT BaseIndex, UINT NumBytes, const void* NewValue);

	virtual void SetPixelShaderBoolParameter(FD3D9PixelShaderPtr PixelShader, UINT BaseIndex, UBOOL NewValue);

	virtual void SetViewParameters(const FMatrix& ViewProjMatrix, const FVector4& ViewOrigin);

	virtual void SetStreamSource(UINT StreamIndex, FD3D9VertexBufferPtr VertexBuffer, UINT Stride, UBOOL bUseInstanceIndex, UINT NumInstances);

	// operation about Texture
	virtual FD3D9Texture2DPtr CreateTexture2D(UINT SizeX, UINT SizeY, BYTE Format, UINT NumMips, DWORD Flags);

	virtual void* LockTexture2D(FD3D9Texture2DPtr Texture, UINT MipIndex, UBOOL bIsDataBeingWrittenTo, UINT& DestStride);

	virtual void UnlockTexture2D(FD3D9Texture2DPtr Texture, UINT MipIndex);

	virtual FD3D9TextureCubePtr CreateTextureCube(UINT Size, BYTE Format, UINT NumMips, DWORD Flags);

	virtual void* LockTextureCubeFace(FD3D9TextureCubePtr TextureCube, UINT FaceIndex, UINT MipIndex, UINT& DstStride);

	virtual void UnlockTextureCubeFace(FD3D9TextureCubePtr TextureCube, UINT FaceIndex, UINT MipIndex);

	virtual void CopyTexture2D(FD3D9Texture2DPtr DstTexture2D, UINT MipIndex, INT BaseSizeX, INT BaseSizeY, INT Format, FD3D9Texture2DPtr SrcTexture2D);

	virtual void CopyMipToMipAsync(FD3D9Texture2DPtr SrcTexture2D, INT SrcMipIndex, FD3D9Texture2DPtr DstTexture2D, INT DstMipIndex, INT Size);

	virtual FD3D9SurfacePtr CreateTargetableSurface(UINT SizeX, UINT SizeY, BYTE Format, DWORD Flags);

	virtual FD3D9SurfacePtr CreateTargetableCubeSurface(UINT Size, BYTE Format, FD3D9TextureCubePtr TargetTexture, ECubeFace CubeFace, DWORD Flags);

	virtual void CopyToResolveTarget(FD3D9SurfacePtr SourceSurface, FD3D9SurfacePtr DestinationSurface);

	virtual void ReadSurfaceData(FD3D9SurfacePtr InSurface, UINT MinX, UINT MinY, UINT MaxX, UINT MaxY, vector<BYTE>& OutData, ECubeFace CubeFace);

	// 
	virtual void SetColorWriteEnable(UBOOL bEnable);

	virtual void SetColorWriteMask(UINT ColorWriteMask);

	virtual void SetScissorRect(UBOOL bEnable, UINT MinX, UINT MaxX, UINT MinY, UINT MaxY);

	virtual void SetViewport(UINT MinX, UINT MaxX, UINT MinY, UINT MaxY, UINT MinZ, UINT MaxZ);

	virtual void SetRenderTarget(FD3D9SurfacePtr NewRenderTarget, FD3D9SurfacePtr NewDepthStencilTarget);

	virtual void SetMRTRenderTarget(INT TargetIndex, FD3D9SurfacePtr NewRenderTarget);

	virtual void SetMRTColorWriteEnable(UINT TargetIndex, UBOOL bEnable);

	virtual void SetMRTColorWriteMask(UINT TargetIndex, UINT ColorWriteMask);

	// operation about Viewport
	virtual void BeginDrawingViewport(FD3D9ViewportPtr Viewport);

	virtual void EndDrawingViewport(FD3D9ViewportPtr Viewport, UBOOL bPresent);

	virtual FD3D9SurfacePtr GetViewportBackbuffer(FD3D9ViewportPtr Viewport);

	virtual FD3D9ViewportPtr CreateViewport(void* WindowHandle, UINT SizeX, UINT SizeY, UBOOL bIsFullscreen);

	virtual void ResizeViewport(FD3D9ViewportPtr Viewport, UINT SizeX, UINT SizeY, UBOOL bIsFullscreen);

	virtual void Tick(FLOAT DeltaTime);

	virtual UBOOL IsDrawingViewport();

	// Draw Primitive
	virtual void DrawPrimitive(UINT PrimitiveType, UINT BaseVertexIndex, UINT NumPrimitives);

	virtual void DrawIndexedPrimitive(FD3D9IndexBufferPtr IndexBuffer, UINT PrimitiveType, UINT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT NumPrimitives);

	virtual void DrawPrimitiveUP(UINT PrimitiveType, UINT NumPrimitives, const void* VertexData, UINT VertexStride);

	virtual void DrawIndexedPrimitiveUP(UINT PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT NumPrimitives, const void* IndexData, UINT IndexStride, const void* VertexData, UINT VertexStride);

	virtual void Clear(UBOOL bClearColor, const FColor& Color, UBOOL bClearDepth, FLOAT Depth, UBOOL bClearStencil, DWORD Stencil);

	virtual void CleanupD3DDevice();

	virtual void GetSupportedResolution(UINT& Width, UINT& Height);

	virtual UBOOL GetAvailableResolutions(vector<FScreenResolution>& Resolutions, UBOOL bIgnoreRefreshRate);

private:
	static const UINT _NumBytesPerShaderRegister = sizeof(FLOAT) * 4;

	TRefCountPtr<IDirect3D9>			_Direct3D;

	TRefCountPtr<IDirect3DDevice9>		_Direct3DDevice;

	TRefCountPtr<FD3D9Surface>			_BackBuffer;

	vector<FD3D9Viewport*>				_Viewports;

	FD3D9ViewportPtr					_DrawingViewport;

	UBOOL								_bDeviceLost;

	UINT								_DeviceSizeX;

	UINT								_DeviceSizeY;

	HWND								_DeviceWindow;

	UBOOL								_bIsFullscreen;

	D3DCAPS9							_DeviceCaps;

	FD3D9VertexDeclarationCache			_VertexDeclarationCache;
};


#endif