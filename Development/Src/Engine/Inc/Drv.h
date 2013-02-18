#ifndef VRI3_ENGINE_DRV_H_
#define VRI3_ENGINE_DRV_H_

extern UBOOL GIsDrvInitialized;

/** Max number of miplevels in a texture */
enum { MAX_TEXTURE_MIP_COUNT = 13 };

/** Max number of vertex elements which can be used by a vertex declaration */
enum { MaxVertexElementCount = 16 };

/** The alignment in bytes between elements of array shader parameters. */
enum { ShaderArrayElementAlignBytes = 16 };

extern INT	 GMaxTextureMipCount;

extern UBOOL GSupportHardwarePCF;

/** TRUE if the rendering hardware supports vertex instancing. */
extern UBOOL GSupportsVertexInstancing;

/** TRUE if D24 textures can be created and sampled, retrieving 4 neighboring texels in a single lookup. */
extern UBOOL GSupportsFetch4;

/** The offset from the upper left corner of a pixel to the position which is used to sample textures for fragments of that pixel. see also DX SDK "Texture Coordinates" */
extern FLOAT GPixelCenterOffset;

extern class FVertexElementTypeSupportInfo GVertexElementTypeSupport;

// see also: D3DTEXTUREFILTERTYPE
enum ESamplerFilter
{
	SF_Point,
	SF_Bilinear,
	SF_Trilinear,
	SF_AnisotropicPoint,
	SF_AnisotropicLinear,
};

// see also: D3DTEXTUREADDRESS
enum ESamplerAddressMode
{
	SA_Wrap,
	SA_Clamp,
	SA_Mirror
};

enum ESamplerMipMapLODBias
{
	SM_None,
	SM_Get4
};

// see also: D3DFILLMODE
enum ERasterizerFillMode
{
	RF_Point,
	RF_Wireframe,
	RF_Solid
};

// see also: D3DCULL
enum ERasterizerCullMode
{
	RC_None,
	RC_CW,
	RC_CCW						// default value is CCW
};

enum EColorWriteMask
{
	CW_Red		=	0x01,
	CW_Green	=	0x02,
	CW_Blue		=	0x04,
	CW_Alpha	=	0x08,

	CW_RGB		=	0x07,		// CW_Red | CW_Green | CW_Blue
	CW_RGBA		=	0x0f		// CW_Red | CW_Green | CW_Blue | CW_Alpha
};

// see also: D3DCMPFUNC
// ZFunc, StencilFunc and AlphaFunc
enum ECompareFunction
{
	CF_Less,
	CF_LessEqual,
	CF_Greater,
	CF_GreaterEqual,
	CF_Equal,
	CF_NotEqual,
	CF_Never,
	CF_Always
};

// see also: D3DSTENCILOP
enum EStencilOp
{
	SO_Keep,
	SO_Zero,
	SO_Replace,
	SO_IncrSat,
	SO_DecrSat,
	SO_Invert,
	SO_Incr,
	SO_Decr
};

// see also: D3DBLENDOP
enum EBlendOp
{
	BO_Add,
	BO_Subtract,
	BO_Min,
	BO_Max
};

// see also: D3DBLEND
enum EBlendFactor
{
	BF_Zero,
	BF_One,

	BF_SourceColor,
	BF_InverseSourceColor,
	BF_SourceAlpha,
	BF_InverseSourceAlpha,

	BF_DestAlpha,
	BF_InverseDestAlpha,
	BF_DestColor,
	BF_InverseDestColor
};

// see also: D3DDECLTYPE
enum EVertexElementType
{
	VET_None,
	VET_FLOAT1,
	VET_FLOAT2,
	VET_FLOAT3,
	VET_FLOAT4,
	VET_UByte4,				// unsigned byte
	VET_UByte4N,			// each byte normalized by dividing with 255.0f
	VET_Color,				// RGBA order, unsigned bytes mapped to 0 to 1 range
	VET_Short2,				// signed short expanded to (value, value, 0, 1)
	VET_Short2N,			// signed short, expanded to (first short/32767.0, second short/32767.0, 0, 1)
	VET_Half2,				//	16 bit float using 1 bit sign, 5 bit exponent, 10 bit mantissa ( a = m x be, m(mantissa) )
	VET_Max
};

// see also: D3DDECLUSAGE
enum EVertexElementUsage
{
	VEU_Position,
	VEU_TextureCoord,
	VEU_BlendWeight,
	VEU_BlendIndices,
	VEU_Normal,
	VEU_Tangent,
	VEU_Binormal,
	VEU_Color
};

enum ECubeFace
{
	CF_PosX,
	CF_NegX,
	CF_PosY,
	CF_NegY,
	CF_PosZ,
	CF_NegZ,
	CF_Max
};

enum EPixelFormat
{
	PF_Unknown				=	0,
	PF_A32B32G32R32F		=	1,
	PF_A8R8G8B8				=	2,
	PF_G8					=	3,
	PF_G16					=	4,
	PF_DXT1					=	5,
	PF_DXT3					=	6,
	PF_DXT5					=	7,
	PF_FloatRGB				=	8,
	PF_FloatRGBA			=	9,
	PF_DepthStencil			=	10,
	PF_ShadowDepth			=	11,
	PF_FilteredShadowDepth	=	12,
	PF_R32F					=	13,
	PF_G16R16				=	14,
	PF_G16R16F				=	15,
	PF_G32R32F				=	16,
	PF_A2B10G10R10			=	17,
	PF_A16B16G16R16			=	18,
	PF_D24					=	19,
	PF_MAX					=	20,
};

// vertex data layout, see also: D3DVERTEXELEMENT9
struct FVertexElement
{
	BYTE	StreamIndex;
	BYTE	Offset;
	BYTE	Type;
	BYTE	Usage;
	BYTE	UsageIndex;					// 用于标识多个相同用途的顶点分量
	UBOOL	bUseInstanceIndex;
	UINT	NumVerticesPerInstance;

	FVertexElement() {}
	FVertexElement(BYTE InStreamIndex, BYTE InOffset, BYTE InType, BYTE InUsage, BYTE InUsageIndex, UBOOL InbUseInstanceIndex = FALSE, UINT InNumVerticesPerInstance = 0)
		: StreamIndex(InStreamIndex),
			Offset(InOffset),
			Type(InType),
			Usage(InUsage),
			UsageIndex(InUsageIndex),
			bUseInstanceIndex(InbUseInstanceIndex),
			NumVerticesPerInstance(InNumVerticesPerInstance)
	{}

	void operator=(const FVertexElement& Other)
	{
		StreamIndex = Other.StreamIndex;
		Offset = Other.Offset;
		Type = Other.Type;
		Usage = Other.Usage;
		UsageIndex = Other.UsageIndex;
		bUseInstanceIndex = Other.bUseInstanceIndex;
		NumVerticesPerInstance = Other.NumVerticesPerInstance;
	}
};
typedef vector<FVertexElement>	FVertexDeclarationElementList;

class FVertexElementTypeSupportInfo
{
public:
	FVertexElementTypeSupportInfo()
	{
		for(UINT i = 0; i < VET_Max; ++i)
		{
			ElementCaps[i] = TRUE;
		}
	}

	UBOOL IsSupported(EVertexElementType ElementType) { return ElementCaps[ElementType]; }
	void SetSupported(EVertexElementType ElementType, UBOOL bIsSupported) { ElementCaps[ElementType] = bIsSupported; }

private:
	UBOOL ElementCaps[VET_Max];
};

// creator of CreateSamplerState()
struct FSamplerStateInitializer
{
	ESamplerFilter			Filter;
	ESamplerAddressMode		AddressU;
	ESamplerAddressMode		AddressV;
	ESamplerAddressMode		AddressW;
	ESamplerMipMapLODBias	MipBias;
};

// creator of CreateRasterizerState()
struct FRasterizerStateInitializer
{
	ERasterizerFillMode		FillMode;
	ERasterizerCullMode		CullMode;
	FLOAT					DepthBias;
	FLOAT					SlopeScaleDepthBias;
};

// creator of CreateDepthState
struct FDepthStateInitializer
{
	UBOOL					bEnableDepthWrite;
	ECompareFunction		DepthTest;
};

// creator of CreateStencilState
struct FStencilStateInitializer
{
	UBOOL					bEnableFrontFaceStencil;
	ECompareFunction		FrontFaceStencilTest;
	EStencilOp				FrontFaceStencilFailOp;
	EStencilOp				FrontFaceDepthFailOp;
	EStencilOp				FrontFacePassStencilOp;

	UBOOL					bEnableBackFaceStencil;
	ECompareFunction		BackFaceStencilTest;
	EStencilOp				BackFaceStencilFailOp;
	EStencilOp				BackFaceDepthFailOp;
	EStencilOp				BackFacePassStencilOp;

	DWORD					StencilReadMask;
	DWORD					StencilWriteMask;
	DWORD					StencilRef;

	FStencilStateInitializer(
		UBOOL	bInEnableFrontFaceStencil = FALSE,
		ECompareFunction InFrontFaceStencilTest = CF_Always,
		EStencilOp InFrontFaceStencilFailOp = SO_Keep,
		EStencilOp InFrontFaceDepthFailOp = SO_Keep,
		EStencilOp InFrontFacePassStencilOp = SO_Keep,
		UBOOL bInEnableBackFaceStencil = FALSE,
		ECompareFunction InBackFaceStencilTest = CF_Always,
		EStencilOp InBackFaceStencilFailOp = SO_Keep,
		EStencilOp InBackFaceDepthFailOp = SO_Keep,
		EStencilOp InBackFacePassStencilOp = SO_Keep,
		DWORD InStencilReadMask = 0xFFFFFFFF,
		DWORD InStencilWriteMask = 0xFFFFFFFF,
		DWORD InStencilRef = 0) :
		bEnableFrontFaceStencil(bInEnableFrontFaceStencil),
		FrontFaceStencilTest(InFrontFaceStencilTest),
		FrontFaceStencilFailOp(InFrontFaceStencilFailOp),
		FrontFaceDepthFailOp(InFrontFaceDepthFailOp),
		FrontFacePassStencilOp(InFrontFacePassStencilOp),
		bEnableBackFaceStencil(bInEnableBackFaceStencil),
		BackFaceStencilTest(InBackFaceStencilTest),
		BackFaceStencilFailOp(InBackFaceStencilFailOp),
		BackFaceDepthFailOp(InBackFaceDepthFailOp),
		BackFacePassStencilOp(InBackFacePassStencilOp),
		StencilReadMask(InStencilReadMask),
		StencilWriteMask(InStencilWriteMask),
		StencilRef(InStencilRef)
	{
	}
};

struct FBlendStateInitializer
{
	EBlendOp				ColorBlendOperation;
	EBlendFactor			ColorSourceBlendFactor;
	EBlendFactor			ColorDestBlendFactor;
	EBlendOp				AlphaBlendOperation;
	EBlendFactor			AlphaSourceBlendFactor;
	EBlendFactor			AlphaDestBlendFactor;
	ECompareFunction		AlphaTest;
	BYTE					AlphaRef;
};

enum EPrimitiveType
{
	PT_TriangleList		=	0,
	PT_TriangleStrip	=	1,
	PT_LineList			=	2,
	PT_QuadList			=	3
};

enum EParticleEmitterType
{
	PET_None = 0,		// Not a particle emitter
	PET_Sprite = 1,		// Sprite particle emitter
	PET_SubUV = 2,		// SubUV particle emitter
	PET_Mesh = 3,		// Mesh emitter

	//PET_NumBits = 2
};

// Pixel shader constants that are reserved by the engine
enum EPixelShaderRegisters
{
	PSR_ColorBiasFactor			= 0,			// Factor applied to the color output from the pixelshader
	PSR_ScreenPositionScaleBias = 1,			// Converts projection-space XY coordinates to texture-space UV coordinates
	PSR_MinZ_MaxZ_Ratio			= 2,			// Converts device Z values to clip-space W values
	PSR_MaxPixelShaderRegister	= 4
};

// Vertex shader constants that are reserved by the engine
enum EVertexShaderRegisters
{
	VSR_ViewProjMatrix			= 0,			// View-projection matrix, transforming from World space to Projection space
	VSR_ViewOrigin				= 4,			// World space position of the view's origin (camera position)
	VSR_PreViewTranslation		= 5,
	VSR_MaxVertexShaderRegister = 6
};

/** 
 *	Resource Usage Flag	- for vertex and index buffer
*/
enum EResourceUsageFlag
{
	RUF_Static					=	0,		// the resource will be created, filled and never repacked
	RUF_Dynamic					=	1		// the resource will be repacked in-frequently
};

/**
*	Screen Resoultion
*/
struct FScreenResolution
{
	DWORD	Width;
	DWORD	Height;
	DWORD	RefreshRate;
};

enum ETextureCreateFlags
{
	TexCreate_SRGB				=	1<<0,
	TexCreate_ResolveTarget		=	1<<1,
	TexCreate_DepthStencil		=	1<<2,
	TexCreate_Dynamic			=	1<<3
};

enum ESurfaceCreateFlags
{
	// without this the surface may simple be an alias for the texture's memory. Note that you must still resolve the surface.
	TargetSurfCreate_Dedicated	=	1<<0,
	TargetSurfCreate_Readable	=	1<<1,
};

struct SPixelFormatInfo
{
	const TCHAR*	Name;
	INT				BlockSizeX;
	INT				BlockSizeY;
	INT				BlockSizeZ;
	INT				BlockBytes;
	INT				NumComponents;

	DWORD			PlatformFormat;		// e.g.	D3DFORMAT
	UBOOL			Supported;			// Whether the texture format is supported on the specified platform
};
extern SPixelFormatInfo	GPixelFormats[];

enum EMouseCursor
{
	MC_None,
	MC_Arrow,
	MC_Cross,
	MC_SizeAll,
	MC_Hand,
	MC_GrabHand,
	MC_Custom
};


#endif