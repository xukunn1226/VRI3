#ifndef VRI3_ENGINE_DYNAMICDRV_H_
#define VRI3_ENGINE_DYNAMICDRV_H_

#if defined(DEBUG) || defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

#include <d3d9.h>
#include <d3dx9.h>

#include "D3D9Resources.h"
#include "D3D9State.h"
#include "D3D9Viewport.h"
#include "D3D9Util.h"


class FDynamicDrv
{
public:
	virtual ~FDynamicDrv() {}

	// generic render state
	virtual FD3D9SamplerState CreateSamplerState(const FSamplerStateInitializer& Initializer) = 0;

	virtual FD3D9RasterizerState CreateRasterizerState(const FRasterizerStateInitializer& Initializer) = 0;

	virtual FD3D9DepthState CreateDepthState(const FDepthStateInitializer& Initializer) = 0;

	virtual FD3D9StencilState CreateStencilState(const FStencilStateInitializer& Initializer) = 0;

	virtual FD3D9BlendState CreateBlendState(const FBlendStateInitializer& Initializer) = 0;
	
	virtual void SetSamplerState(UINT SamplerIndex, FD3D9SamplerState SamplerState, FD3D9TexturePtr Texture, INT MaxMipLevel) = 0;

	virtual void SetRasterizerState(FD3D9RasterizerState RasterizerState) = 0;

	virtual void SetRasterizerStateImmediate(const FRasterizerStateInitializer& Initializer) = 0;

	virtual void SetDepthState(FD3D9DepthState DepthState) = 0;

	virtual void SetStencilState(FD3D9StencilState StencilState) = 0;

	virtual void SetBlendState(FD3D9BlendState BlendState) = 0;

	// operation about Vertex and Pixel Shader
	virtual FD3D9VertexDeclarationPtr CreateVertexDeclaration(const FVertexDeclarationElementList& Elements) = 0;

	virtual FD3D9VertexShaderPtr CreateVertexShader(const vector<BYTE>& Code) = 0;

	virtual FD3D9PixelShaderPtr CreatePixelShader(const vector<BYTE>& Code) = 0;

	virtual FD3D9BoundShaderStatePtr CreateBoundShaderState(const FD3D9VertexDeclarationPtr VertexDeclaration, const FD3D9VertexShaderPtr VertexShader, const FD3D9PixelShaderPtr PixelShader) = 0;

	virtual void SetBoundShaderState(FD3D9BoundShaderStatePtr BoundShaderState) = 0;

	// operation about Vertex and Index Buffer
	virtual FD3D9IndexBufferPtr CreateIndexBuffer(UINT Stride, UINT Size, void* ResourceData, UBOOL bIsDynamic) = 0;

	virtual void* LockIndexBuffer(FD3D9IndexBufferPtr IndexBuffer, UINT Offset, UINT Size) = 0;

	virtual void UnlockIndexBuffer(FD3D9IndexBufferPtr IndexBuffer) = 0;

	virtual FD3D9VertexBufferPtr CreateVertexBuffer(UINT Size, void* ResourceData, UBOOL bIsDynamic) = 0;

	virtual void* LockVertexBuffer(FD3D9VertexBufferPtr VertexBuffer, UINT Offset, UINT Size, UBOOL bReadOnly) = 0;

	virtual void UnlockVertexBuffer(FD3D9VertexBufferPtr VertexBuffer) = 0;

	virtual void SetVertexShaderParameter(FD3D9VertexShaderPtr VertexShader, UINT BaseIndex, UINT NumBytes, const void* NewValue) = 0;

	virtual void SetVertexShaderBoolParameter(FD3D9VertexShaderPtr VertexShader, UINT BaseIndex, UBOOL NewValue) = 0;

	virtual void SetPixelShaderParameter(FD3D9PixelShaderPtr PixelShader, UINT BaseIndex, UINT NumBytes, const void* NewValue) = 0;

	virtual void SetPixelShaderBoolParameter(FD3D9PixelShaderPtr PixelShader, UINT BaseIndex, UBOOL NewValue) = 0;

	virtual void SetViewParameters(const FMatrix& ViewProjMatrix, const FVector4& ViewOrigin) = 0;

	virtual void SetStreamSource(UINT StreamIndex, FD3D9VertexBufferPtr VertexBuffer, UINT Stride, UBOOL bUseInstanceIndex, UINT NumInstances) = 0;

	// operation about Texture
	virtual FD3D9Texture2DPtr CreateTexture2D(UINT SizeX, UINT SizeY, BYTE Format, UINT NumMips, DWORD Flags) = 0;

	virtual void* LockTexture2D(FD3D9Texture2DPtr Texture, UINT MipIndex, UBOOL bIsDataBeingWrittenTo, UINT& DestStride) = 0;

	virtual void UnlockTexture2D(FD3D9Texture2DPtr Texture, UINT MipIndex) = 0;

	virtual FD3D9TextureCubePtr CreateTextureCube(UINT Size, BYTE Format, UINT NumMips, DWORD Flags) = 0;

	virtual void* LockTextureCubeFace(FD3D9TextureCubePtr TextureCube, UINT FaceIndex, UINT MipIndex, UINT& DstStride) = 0;

	virtual void UnlockTextureCubeFace(FD3D9TextureCubePtr TextureCube, UINT FaceIndex, UINT MipIndex) = 0;

	virtual void CopyTexture2D(FD3D9Texture2DPtr DstTexture2D, UINT MipIndex, INT BaseSizeX, INT BaseSizeY, INT Format, FD3D9Texture2DPtr SrcTexture2D) = 0;

	virtual void CopyMipToMipAsync(FD3D9Texture2DPtr SrcTexture2D, INT SrcMipIndex, FD3D9Texture2DPtr DstTexture2D, INT DstMipIndex, INT Size) = 0;

	virtual FD3D9SurfacePtr CreateTargetableSurface(UINT SizeX, UINT SizeY, BYTE Format, DWORD Flags) = 0;

	virtual FD3D9SurfacePtr CreateTargetableCubeSurface(UINT Size, BYTE Format, FD3D9TextureCubePtr TargetTexture, ECubeFace CubeFace, DWORD Flags) = 0;

	virtual void CopyToResolveTarget(FD3D9SurfacePtr SourceSurface, FD3D9SurfacePtr DestinationSurface) = 0;

	virtual void ReadSurfaceData(FD3D9SurfacePtr InSurface, UINT MinX, UINT MinY, UINT MaxX, UINT MaxY, vector<BYTE>& OutData, ECubeFace CubeFace) = 0;

	// 
	virtual void SetColorWriteEnable(UBOOL bEnable) = 0;

	virtual void SetColorWriteMask(UINT ColorWriteMask) = 0;

	virtual void SetScissorRect(UBOOL bEnable, UINT MinX, UINT MaxX, UINT MinY, UINT MaxY) = 0;

	virtual void SetViewport(UINT MinX, UINT MaxX, UINT MinY, UINT MaxY, UINT MinZ, UINT MaxZ) = 0;

	virtual void SetRenderTarget(FD3D9SurfacePtr NewRenderTarget, FD3D9SurfacePtr NewDepthStencilTarget) = 0;

	virtual void SetMRTRenderTarget(INT TargetIndex, FD3D9SurfacePtr NewRenderTarget) = 0;

	virtual void SetMRTColorWriteEnable(UINT TargetIndex, UBOOL bEnable) = 0;

	virtual void SetMRTColorWriteMask(UINT TargetIndex, UINT ColorWriteMask) = 0;

	// operation about Viewport
	virtual void BeginDrawingViewport(FD3D9ViewportPtr Viewport) = 0;

	virtual void EndDrawingViewport(FD3D9ViewportPtr Viewport, UBOOL bPresent) = 0;

	virtual FD3D9SurfacePtr GetViewportBackbuffer(FD3D9ViewportPtr Viewport) = 0;

	virtual FD3D9ViewportPtr CreateViewport(void* WindowHandle, UINT SizeX, UINT SizeY, UBOOL bIsFullscreen) = 0;

	virtual void ResizeViewport(FD3D9ViewportPtr Viewport, UINT SizeX, UINT SizeY, UBOOL bIsFullscreen) = 0;

	virtual void Tick(FLOAT DeltaTime) = 0;

	virtual UBOOL IsDrawingViewport() = 0;

	// Draw Primitive
	virtual void DrawPrimitive(UINT PrimitiveType, UINT BaseVertexIndex, UINT NumPrimitives) = 0;

	virtual void DrawIndexedPrimitive(FD3D9IndexBufferPtr IndexBuffer, UINT PrimitiveType, UINT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT NumPrimitives) = 0;

	virtual void DrawPrimitiveUP(UINT PrimitiveType, UINT NumPrimitives, const void* VertexData, UINT VertexStride) = 0;

	virtual void DrawIndexedPrimitiveUP(UINT PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT NumPrimitives, const void* IndexData, UINT IndexStride, const void* VertexData, UINT VertexStride) = 0;

	virtual void Clear(UBOOL bClearColor, const FColor& Color, UBOOL bClearDepth, FLOAT Depth, UBOOL bClearStencil, DWORD Stencil) = 0;

	virtual void CleanupD3DDevice() = 0;

	virtual void GetSupportedResolution(UINT& Width, UINT& Height) = 0;

	virtual UBOOL GetAvailableResolutions(vector<FScreenResolution>& Resolutions, UBOOL bIgnoreRefreshRate) = 0;
};

extern FDynamicDrv* GDynamicDrv;

extern void DrvInit();

extern void DrvExit();

#endif