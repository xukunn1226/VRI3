#ifndef VRI3_ENGINE_RENDERTARGET_H_
#define VRI3_ENGINE_RENDERTARGET_H_

/**
 * A render target
 */
class FRenderTarget
{
public:
	virtual ~FRenderTarget() {}

	virtual const FD3D9SurfacePtr& GetRenderTargetSurface() const;

	virtual UINT GetSizeX() const = 0;
	virtual UINT GetSizeY() const = 0;

	UBOOL ReadPixels(vector<FLinearColor>& OutputBuffer, ECubeFace CubeFace = CF_PosX);

protected:
	FD3D9SurfacePtr		_RenderTargetSurface;
};


#endif