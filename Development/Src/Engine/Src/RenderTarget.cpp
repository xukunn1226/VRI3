#include "EnginePCH.h"

const FD3D9SurfacePtr& FRenderTarget::GetRenderTargetSurface() const
{
	return _RenderTargetSurface;
}

UBOOL FRenderTarget::ReadPixels(vector<FLinearColor>& OutputBuffer, ECubeFace CubeFace)
{
	OutputBuffer.resize(GetSizeX() * GetSizeY() * sizeof(FLinearColor));
	GDynamicDrv->ReadSurfaceData(_RenderTargetSurface, 0, 0, GetSizeX(), GetSizeY(), *(vector<BYTE>*)&OutputBuffer[0], CubeFace);
	return FALSE;
}

