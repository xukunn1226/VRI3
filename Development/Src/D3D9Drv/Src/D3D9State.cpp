#include "D3D9DrvPCH.h"

static D3DTEXTUREADDRESS TranslateAddressMode(ESamplerAddressMode AddressMode)
{
	switch(AddressMode)
	{
	case SA_Clamp:	return D3DTADDRESS_CLAMP;
	case SA_Mirror:	return D3DTADDRESS_MIRROR;
	default:	return D3DTADDRESS_WRAP;
	};
}

static DWORD TranslateMipBias(ESamplerMipMapLODBias MipBias)
{
	switch(MipBias)
	{
	case SM_Get4:	return 0;
	default:	return 0;
	};
}

static D3DCULL TranslateCullMode(ERasterizerCullMode CullMode)
{
	switch(CullMode)
	{
	case RC_CW:		return D3DCULL_CW;
	case RC_CCW:	return D3DCULL_CCW;
	default:		return D3DCULL_NONE;
	};
}

static D3DFILLMODE TranslateFillMode(ERasterizerFillMode FillMode)
{
	switch(FillMode)
	{
	case RF_Point:		return D3DFILL_POINT;
	case RF_Wireframe:	return D3DFILL_WIREFRAME;
	default:			return D3DFILL_SOLID;
	};
};

static D3DCMPFUNC TranslateCompareFunction(ECompareFunction CompareFunction)
{
	switch(CompareFunction)
	{
	case CF_Less:			return D3DCMP_LESS;
	case CF_LessEqual:		return D3DCMP_LESSEQUAL;
	case CF_Greater:		return D3DCMP_GREATER;
	case CF_GreaterEqual:	return D3DCMP_GREATEREQUAL;
	case CF_Equal:			return D3DCMP_EQUAL;
	case CF_NotEqual:		return D3DCMP_NOTEQUAL;
	case CF_Never:			return D3DCMP_NEVER;
	default:				return D3DCMP_ALWAYS;
	};
}

static D3DSTENCILOP TranslateStencilOp(EStencilOp StencilOp)
{
	switch(StencilOp)
	{
	case SO_Keep:		return D3DSTENCILOP_KEEP;
	case SO_Zero:		return D3DSTENCILOP_ZERO;
	case SO_Replace:	return D3DSTENCILOP_REPLACE;
	case SO_IncrSat:	return D3DSTENCILOP_INCRSAT;
	case SO_DecrSat:	return D3DSTENCILOP_DECRSAT;
	case SO_Invert:		return D3DSTENCILOP_INVERT;
	case SO_Incr:		return D3DSTENCILOP_INCR;
	default:			return D3DSTENCILOP_DECR;
	};
}

static D3DBLENDOP TranslateBlendOp(EBlendOp BlendOp)
{
	switch(BlendOp)
	{
	case BO_Add:		return D3DBLENDOP_ADD;
	case BO_Subtract:	return D3DBLENDOP_SUBTRACT;
	case BO_Min:		return D3DBLENDOP_MIN;
	default:			return D3DBLENDOP_MAX;
	};
}

static D3DBLEND TranslateBlendFactor(EBlendFactor BlendFactor)
{
	switch(BlendFactor)
	{
	case BF_One:				return D3DBLEND_ONE;
	case BF_SourceColor:		return D3DBLEND_SRCCOLOR;
	case BF_InverseSourceColor:	return D3DBLEND_INVSRCCOLOR;
	case BF_SourceAlpha:		return D3DBLEND_SRCALPHA;
	case BF_InverseSourceAlpha:	return D3DBLEND_INVSRCALPHA;
	case BF_DestAlpha:			return D3DBLEND_DESTALPHA;
	case BF_InverseDestAlpha:	return D3DBLEND_INVDESTALPHA;
	case BF_DestColor:			return D3DBLEND_DESTCOLOR;
	case BF_InverseDestColor:	return D3DBLEND_INVDESTCOLOR;
	default:					return D3DBLEND_ZERO;
	};
}


FD3D9SamplerState FD3D9DynamicDrv::CreateSamplerState(const FSamplerStateInitializer& Initializer)
{
	FD3D9SamplerState SamplerState;
	SamplerState.AddressU = TranslateAddressMode(Initializer.AddressU);
	SamplerState.AddressV = TranslateAddressMode(Initializer.AddressV);
	SamplerState.AddressW = TranslateAddressMode(Initializer.AddressW);
	SamplerState.MipMapLODBias = TranslateMipBias(Initializer.MipBias);

	switch(Initializer.Filter)
	{
	case SF_Point:
		SamplerState.MinFilter = D3DTEXF_POINT;
		SamplerState.MagFilter = D3DTEXF_POINT;
		SamplerState.MipFilter = D3DTEXF_POINT;
		break;
	case SF_Bilinear:
		SamplerState.MinFilter = D3DTEXF_LINEAR;
		SamplerState.MagFilter = D3DTEXF_LINEAR;
		SamplerState.MipFilter = D3DTEXF_POINT;
		break;
	case SF_Trilinear:
		SamplerState.MinFilter = D3DTEXF_LINEAR;
		SamplerState.MagFilter = D3DTEXF_LINEAR;
		SamplerState.MipFilter = D3DTEXF_LINEAR;
		break;
	case SF_AnisotropicPoint:
		SamplerState.MinFilter = D3DTEXF_ANISOTROPIC;
		SamplerState.MagFilter = D3DTEXF_LINEAR;
		SamplerState.MipFilter = D3DTEXF_POINT;
		break;
	case SF_AnisotropicLinear:
		SamplerState.MinFilter = D3DTEXF_ANISOTROPIC;
		SamplerState.MagFilter = D3DTEXF_LINEAR;
		SamplerState.MipFilter = D3DTEXF_LINEAR;
		break;
	};

	return SamplerState;
}

FD3D9RasterizerState FD3D9DynamicDrv::CreateRasterizerState(const FRasterizerStateInitializer &Initializer)
{
	FD3D9RasterizerState RasterizerState;
	RasterizerState.FillMode = TranslateFillMode(Initializer.FillMode);
	RasterizerState.CullMode = TranslateCullMode(Initializer.CullMode);
	RasterizerState.DepthBias = Initializer.DepthBias;
	RasterizerState.SlopeScaleDepthBias = Initializer.SlopeScaleDepthBias;

	return RasterizerState;
}

void FD3D9DynamicDrv::SetRasterizerStateImmediate(const FRasterizerStateInitializer& Initializer)
{
	_Direct3DDevice->SetRenderState(D3DRS_FILLMODE, TranslateFillMode(Initializer.FillMode));
	_Direct3DDevice->SetRenderState(D3DRS_CULLMODE, TranslateCullMode(Initializer.CullMode));

	extern FLOAT GDepthBiasOffset;
	_Direct3DDevice->SetRenderState(D3DRS_DEPTHBIAS, FLOAT_TO_DWORD(Initializer.DepthBias + GDepthBiasOffset));
	_Direct3DDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, FLOAT_TO_DWORD(Initializer.SlopeScaleDepthBias));
}

FD3D9DepthState FD3D9DynamicDrv::CreateDepthState(const FDepthStateInitializer &Initializer)
{
	FD3D9DepthState DepthState;
	DepthState.bZEnable = Initializer.DepthTest != CF_Always || Initializer.bEnableDepthWrite;
	DepthState.bZWriteEnable = Initializer.bEnableDepthWrite;
	DepthState.ZFunc = TranslateCompareFunction(Initializer.DepthTest);

	return DepthState;
}

FD3D9StencilState FD3D9DynamicDrv::CreateStencilState(const FStencilStateInitializer &Initializer)
{
	FD3D9StencilState StencilState;
	StencilState.bStencilEnable = Initializer.bEnableFrontFaceStencil || Initializer.bEnableBackFaceStencil;
	StencilState.bTwoSidedStencilMode = Initializer.bEnableBackFaceStencil;
	StencilState.StencilFunc = TranslateCompareFunction(Initializer.FrontFaceStencilTest);
	StencilState.StencilFail = TranslateStencilOp(Initializer.FrontFaceStencilFailOp);
	StencilState.StencilZFail = TranslateStencilOp(Initializer.FrontFaceDepthFailOp);
	StencilState.StencilPass = TranslateStencilOp(Initializer.FrontFacePassStencilOp);
	StencilState.CCWStencilFunc = TranslateCompareFunction(Initializer.BackFaceStencilTest);
	StencilState.CCWStencilFail = TranslateStencilOp(Initializer.BackFaceStencilFailOp);
	StencilState.CCWStencilZFail = TranslateStencilOp(Initializer.BackFaceDepthFailOp);
	StencilState.CCWStencilPass = TranslateStencilOp(Initializer.BackFacePassStencilOp);
	StencilState.StencilReadMask = Initializer.StencilReadMask;
	StencilState.StencilWriteMask  = Initializer.StencilWriteMask;
	StencilState.StencilRef = Initializer.StencilRef;

	return StencilState;
}

FD3D9BlendState FD3D9DynamicDrv::CreateBlendState(const FBlendStateInitializer &Initializer)
{
	FD3D9BlendState BlendState;
	BlendState.bAlphaBlendEnable = 
		Initializer.ColorBlendOperation != BO_Add || Initializer.ColorDestBlendFactor != BF_Zero || Initializer.ColorSourceBlendFactor != BF_One ||
		Initializer.AlphaBlendOperation != BO_Add || Initializer.AlphaDestBlendFactor != BF_Zero || Initializer.AlphaSourceBlendFactor != BF_One;
	BlendState.ColorBlendOperation = TranslateBlendOp(Initializer.ColorBlendOperation);
	BlendState.ColorSourceBlendFactor = TranslateBlendFactor(Initializer.ColorSourceBlendFactor);
	BlendState.ColorDestBlendFactor = TranslateBlendFactor(Initializer.ColorDestBlendFactor);

	BlendState.bSeparateAlphaBlendEnable = 
		Initializer.AlphaSourceBlendFactor != Initializer.ColorSourceBlendFactor ||
		Initializer.ColorSourceBlendFactor != Initializer.ColorDestBlendFactor;
	BlendState.AlphaBlendOperation = TranslateBlendOp(Initializer.AlphaBlendOperation);
	BlendState.AlphaSourceBlendFactor = TranslateBlendFactor(Initializer.AlphaSourceBlendFactor);
	BlendState.AlphaDestBlendFactor = TranslateBlendFactor(Initializer.AlphaDestBlendFactor);

	BlendState.bAlphaTestEnable = Initializer.AlphaTest != CF_Always;
	BlendState.AlphaFunc = TranslateCompareFunction(Initializer.AlphaTest);
	BlendState.AlphaRef = Initializer.AlphaRef;

	return BlendState;
}