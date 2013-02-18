#include "EnginePCH.h"

INT GMaxTextureMipCount = MAX_TEXTURE_MIP_COUNT;

FLOAT GDepthBiasOffset = 0.f;

UBOOL GSupportHardwarePCF = FALSE;

UBOOL GIsDrvInitialized = FALSE;

UBOOL GSupportsVertexInstancing = FALSE;

UBOOL GSupportsFetch4 = FALSE;

EShaderPlatform GShaderPlatform = SP_SM3;

FLOAT GPixelCenterOffset = 0.5f;

// see also D3DFORMAT
SPixelFormatInfo GPixelFormats[]	=
{
	//	Name					BlockSizeX	BlockSizeY	BlockSizeZ	BlockBytes	NumComponents	PlatformFormat	Supported
	{ TEXT("Unknown"),			0,			0,			0,			0,			0,				0,				0			},
	{ TEXT("A32B32G32R32F"),	1,			1,			1,			16,			4,				0,				1			},
	{ TEXT("A8R8G8B8"),			1,			1,			1,			4,			4,				0,				1			},
	{ TEXT("G8"),				1,			1,			1,			1,			1,				0,				1			},
	{ TEXT("DXT1"),				4,			4,			1,			8,			3,				0,				1			},
	{ TEXT("DXT3"),				4,			4,			1,			16,			4,				0,				1			},
	{ TEXT("DXT5"),				4,			4,			1,			16,			4,				0,				1			},
	{ TEXT("FloatRGB"),			1,			1,			1,			0,			3,				0,				0			},
	{ TEXT("FloatRGBA"),		1,			1,			1,			0,			4,				0,				0			},
	{ TEXT("DepthStencil"),		1,			1,			1,			0,			1,				0,				0			},
	{ TEXT("ShadowDepth"),		1,			1,			1,			4,			1,				0,				0			},
	{ TEXT("FilteredShadowDepth"), 1,		1,			1,			4,			1,				0,				0			},
	{ TEXT("R32F"),				1,			1,			1,			4,			1,				0,				1			},
	{ TEXT("G16R16"),			1,			1,			1,			4,			2,				0,				1			},
	{ TEXT("G16R16F"),			1,			1,			1,			4,			2,				0,				1			},
	{ TEXT("G32R32F"),			1,			1,			1,			8,			2,				0,				1			},
	{ TEXT("A2B10G10R10"),		1,			1,			1,			4,			4,				0,				1			},
	{ TEXT("A16B16G16R16"),		1,			1,			1,			8,			4,				0,				1			},
	{ TEXT("D24"),				1,			1,			1,			3,			1,				0,				1			},
};

FVertexElementTypeSupportInfo GVertexElementTypeSupport;
