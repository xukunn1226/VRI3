#include "D3D9DrvPCH.h"


FString GetD3DErrorString(HRESULT ErrorCode)
{
	FString ErrorText;
#define D3DERR(x)	case x : ErrorText = TEXT(#x);	break;

	switch(ErrorCode)
	{
		D3DERR(D3D_OK)
		D3DERR(D3DERR_WRONGTEXTUREFORMAT)
		D3DERR(D3DERR_UNSUPPORTEDCOLOROPERATION)
		D3DERR(D3DERR_UNSUPPORTEDCOLORARG)
		D3DERR(D3DERR_UNSUPPORTEDALPHAOPERATION)
		D3DERR(D3DERR_UNSUPPORTEDALPHAARG)
		D3DERR(D3DERR_TOOMANYOPERATIONS)
		D3DERR(D3DERR_CONFLICTINGTEXTUREFILTER)
		D3DERR(D3DERR_UNSUPPORTEDFACTORVALUE)
		D3DERR(D3DERR_CONFLICTINGRENDERSTATE)
		D3DERR(D3DERR_UNSUPPORTEDTEXTUREFILTER)
		D3DERR(D3DERR_CONFLICTINGTEXTUREPALETTE)
		D3DERR(D3DERR_DRIVERINTERNALERROR)
		D3DERR(D3DERR_NOTFOUND)
		D3DERR(D3DERR_MOREDATA)
		D3DERR(D3DERR_DEVICELOST)
		D3DERR(D3DERR_DEVICENOTRESET)
		D3DERR(D3DERR_NOTAVAILABLE)
		D3DERR(D3DERR_OUTOFVIDEOMEMORY)
		D3DERR(D3DERR_INVALIDDEVICE)
		D3DERR(D3DERR_INVALIDCALL)
		D3DERR(D3DERR_DRIVERINVALIDCALL)
		D3DERR(D3DXERR_INVALIDDATA)
		D3DERR(E_OUTOFMEMORY)
	default: ErrorText = FStringUtil::Sprintf(TEXT("Unknown Error Code"));
	}

#undef D3DERR
	return ErrorText;
}

FString GetD3DTextureFormatString(D3DFORMAT TextureFormat)
{
	FString FormatText;
#define TEXTURECASE(x)	case x : FormatText = TEXT(#x);	break;

	switch(TextureFormat)
	{
		TEXTURECASE(D3DFMT_A8R8G8B8)
		TEXTURECASE(D3DFMT_X8R8G8B8)
		TEXTURECASE(D3DFMT_DXT1)
		TEXTURECASE(D3DFMT_DXT3)
		TEXTURECASE(D3DFMT_DXT5)
		TEXTURECASE(D3DFMT_A16B16G16R16F)
		TEXTURECASE(D3DFMT_A32B32G32R32F)
		TEXTURECASE(D3DFMT_UNKNOWN)
		TEXTURECASE(D3DFMT_L8)
		TEXTURECASE(D3DFMT_UYVY)
		TEXTURECASE(D3DFMT_D24S8)
		TEXTURECASE(D3DFMT_D24X8)
		TEXTURECASE(D3DFMT_R32F)
		TEXTURECASE(D3DFMT_G16R16)
		TEXTURECASE(D3DFMT_G16R16F)
		TEXTURECASE(D3DFMT_G32R32F)
		TEXTURECASE(D3DFMT_A2B10G10R10)
		TEXTURECASE(D3DFMT_A16B16G16R16)
	default: FormatText = FStringUtil::Sprintf(TEXT("Unknown Texture Format"));
	}

#undef TEXTURECASE
	return FormatText;
}

FString GetD3DTextureFlagString(DWORD TextureFlags)
{
	FString TextureText;

	if( TextureFlags & D3DUSAGE_DEPTHSTENCIL )
	{
		TextureText += FString(TEXT("D3DUSAGE_DEPTHSTENCIL"));
	}

	if( TextureFlags & D3DUSAGE_RENDERTARGET )
	{
		TextureText += FString(TEXT("D3DUSAGE_RENDERTARGET"));
	}

	return TextureText;
}

void VerifyD3DResult(HRESULT Result, const ANSICHAR* ErrorCode, const ANSICHAR* Filename, UINT Line)
{
	if( FAILED(Result) )
	{
		const FString& ErrorString = GetD3DErrorString(Result);
		debugf(NAME_Error, TEXT("%s failed \n at %s:%u \n with error %s"), ANSI_TO_TCHAR(ErrorCode), ANSI_TO_TCHAR(Filename), Line, ErrorString);
	}
}

void VerifyD3DCreateTextureResult(HRESULT Result, const ANSICHAR* ErrorCode, const ANSICHAR* Filename, UINT Line,
								  UINT SizeX, UINT SizeY, BYTE D3DFormat, UINT NumMips, DWORD Flags)
{
	if( FAILED(Result) )
	{
		const FString& ErrorString = GetD3DErrorString(Result);
		const FString& D3DFormatString = GetD3DTextureFormatString((D3DFORMAT)GPixelFormats[D3DFormat].PlatformFormat);
		debugf(
			NAME_Error,
			TEXT("%s failed \n at %s:%u with error %s, \n SizeX=%i, SizeY=%i, Format=%s=%s, NumMips=%i, Flags=%s"),
			ANSI_TO_TCHAR(ErrorCode),
			ANSI_TO_TCHAR(Filename),
			Line,
			ErrorString,
			SizeX,
			SizeY,
			GPixelFormats[D3DFormat].Name,
			D3DFormatString,
			NumMips,
			GetD3DTextureFlagString(Flags));
	}
}