#ifndef VRI3_D3D9DRV_D3D9UTIL_H_
#define VRI3_D3D9DRV_D3D9UTIL_H_

extern FString GetD3D9ErrorString(HRESULT ErrorCode);

extern void VerifyD3DResult(HRESULT Result, const ANSICHAR* ErrorCode, const ANSICHAR* Filename, UINT Line);

extern void VerifyD3DCreateTextureResult(HRESULT Result, const ANSICHAR* ErrorCode, const ANSICHAR* Filename, UINT Line,
										 UINT SizeX, UINT SizeY, BYTE D3DFormat, UINT NumMips, DWORD Flags);

#if DO_CHECK
#define VERIFYD3DRESULT(x) VerifyD3DResult(x, #x, __FILE__, __LINE__);
#define VERIFYD3DCREATETEXTURERESULT(x, SizeX, SizeY, D3DFormat, NumMips, Flags) VerifyD3DCreateTextureResult(x, #x, __FILE__, __LINE__, SizeX, SizeY, D3DFormat, NumMips, Flags);
#else
#define VERIFYD3DRESULT(x) (x)
#define VERIFYD3DCREATETEXTURERESULT(x, SizeX, SizeY, D3DFormat, NumMips, Flags) (x)
#endif

FORCEINLINE D3DCUBEMAP_FACES GetD3DCubeFace(ECubeFace Face)
{
	switch(Face)
	{
	case CF_PosX:
	default:
		return D3DCUBEMAP_FACE_POSITIVE_X;
	case CF_NegX:
		return D3DCUBEMAP_FACE_NEGATIVE_X;
	case CF_PosY:
		return D3DCUBEMAP_FACE_POSITIVE_Y;
	case CF_NegY:
		return D3DCUBEMAP_FACE_NEGATIVE_Y;
	case CF_PosZ:
		return D3DCUBEMAP_FACE_POSITIVE_Z;
	case CF_NegZ:
		return D3DCUBEMAP_FACE_NEGATIVE_Z;
	};
}

#endif