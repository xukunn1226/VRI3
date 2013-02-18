#ifndef VRI3_ENGINE_STATICSTATES_H_
#define VRI3_ENGINE_STATICSTATES_H_

template<ESamplerFilter Filter = SF_Bilinear, ESamplerAddressMode AddressU = SA_Clamp, ESamplerAddressMode AddressV = SA_Clamp, ESamplerAddressMode AddressW = SA_Clamp, ESamplerMipMapLODBias MipBias = SM_None>
class TStaticSamplerState
{
public:
	static FD3D9SamplerState CreateRHI()
	{
		FSamplerStateInitializer Initializer = { Filter, AddressU, AddressV, AddressW, MipBias };
		return GDynamicDrv->CreateSamplerState(Initializer);
	}
};

template<ERasterizerFillMode FillMode = RF_Solid, ERasterizerCullMode CullMode = RC_None>
class TStaticRasterizerState
{
public:
	static FD3D9RasterizerState CreateRHI()
	{
		FRasterizerStateInitializer Initializer = { FillMode, CullMode, 0, 0 };
		return GDynamicDrv->CreateRasterizerState(Initializer);
	}
};

template<UBOOL bEnableDepthWrite = TRUE, ECompareFunction DepthTest = CF_LessEqual>
class TStaticDepthState
{
public:
	static FD3D9DepthState CreateRHI()
	{
		FDepthStateInitializer Initializer = { bEnableDepthWrite, DepthTest };
		return GDynamicDrv->CreateDepthState(Initializer);
	}
};

template<
	UBOOL bEnableFrontFaceStencil = FALSE,
	ECompareFunction FrontFaceStencilTest = CF_Always,
	EStencilOp FrontFaceStencilFailStencilOp = SO_Keep,
	EStencilOp FrontFaceDepthFailStencilOp = SO_Keep,
	EStencilOp FrontFacePassStencilOp = SO_Keep,
	UBOOL bEnableBackFaceStencil = FALSE,
	ECompareFunction BackFaceStencilTest = CF_Always,
	EStencilOp BackFaceStencilFailStencilOp = SO_Keep,
	EStencilOp BackFaceDepthFailStencilOp = SO_Keep,
	EStencilOp BackFacePassStencilOp = SO_Keep,
	DWORD StencilReadMask = 0xFFFFFFFF,
	DWORD StencilWriteMask = 0xFFFFFFFF,
	DWORD StencilRef = 0
	>
class TStaticStencilState
{
public:
	static FD3D9StencilState CreateRHI()
	{
		FStencilStateInitializer Initializer(
			bEnableFrontFaceStencil,
			FrontFaceStencilTest,
			FrontFaceStencilFailStencilOp,
			FrontFaceDepthFailStencilOp,
			FrontFacePassStencilOp,
			bEnableBackFaceStencil,
			BackFaceStencilTest,
			BackFaceStencilFailStencilOp,
			BackFaceDepthFailStencilOp,
			BackFacePassStencilOp,
			StencilReadMask,
			StencilWriteMask,
			StencilRef
			);
		return GDynamicDrv->CreateStencilState(Initializer);
	}
};

/**
* Alpha blending happens on GPU's as:
* FinalColor.rgb = SourceColor * ColorSrcBlend (ColorBlendOp) DestColor * ColorDestBlend;
* if (BlendState->bSeparateAlphaBlendEnable)
*		FinalColor.a = SourceAlpha * AlphaSrcBlend (AlphaBlendOp) DestAlpha * AlphaDestBlend;
* else
*		Alpha blended the same way as rgb
* 
* So for example, TStaticBlendState<BO_Add,BF_SourceAlpha,BF_InverseSourceAlpha,BO_Add,BF_Zero,BF_One> produces:
* FinalColor.rgb = SourceColor * SourceAlpha + DestColor * (1 - SourceAlpha);
* FinalColor.a = SourceAlpha * 0 + DestAlpha * 1;
*/
template<
	EBlendOp ColorBlendOp = BO_Add,
	EBlendFactor ColorSrcBlend = BF_One,
	EBlendFactor ColorDestBlend = BF_Zero,
	EBlendOp AlphaBlendOp = BO_Add,
	EBlendFactor AlphaSrcBlend = BF_One,
	EBlendFactor AlphaDestBlend = BF_Zero,
	ECompareFunction AlphaTest = CF_Always,
	BYTE AlphaRef = 255
>
class TStaticBlendState
{
public:
	static FD3D9BlendState CreateRHI()
	{
		FBlendStateInitializer Initializer = 
		{
			ColorBlendOp,
			ColorSrcBlend,
			ColorDestBlend,
			AlphaBlendOp,
			AlphaSrcBlend,
			AlphaDestBlend,
			AlphaTest,
			AlphaRef
		};
		return GDynamicDrv->CreateBlendState(Initializer);
	}
};

#endif