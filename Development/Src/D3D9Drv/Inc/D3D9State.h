#ifndef VRI3_D3D9DRV_D3D9STATE_H_
#define VRI3_D3D9DRV_D3D9STATE_H_

struct FD3D9SamplerState
{
	D3DTEXTUREFILTERTYPE	MagFilter;
	D3DTEXTUREFILTERTYPE	MinFilter;
	D3DTEXTUREFILTERTYPE	MipFilter;
	D3DTEXTUREADDRESS		AddressU;
	D3DTEXTUREADDRESS		AddressV;
	D3DTEXTUREADDRESS		AddressW;
	DWORD					MipMapLODBias;
};

struct FD3D9RasterizerState
{
	D3DFILLMODE		FillMode;
	D3DCULL			CullMode;
	FLOAT			DepthBias;
	FLOAT			SlopeScaleDepthBias;
};

struct FD3D9DepthState
{
	UBOOL			bZEnable;
	UBOOL			bZWriteEnable;
	D3DCMPFUNC		ZFunc;
};

struct FD3D9StencilState
{
	UBOOL			bStencilEnable;
	UBOOL			bTwoSidedStencilMode;
	D3DCMPFUNC		StencilFunc;
	D3DSTENCILOP	StencilFail;
	D3DSTENCILOP	StencilZFail;
	D3DSTENCILOP	StencilPass;
	D3DCMPFUNC		CCWStencilFunc;
	D3DSTENCILOP	CCWStencilFail;
	D3DSTENCILOP	CCWStencilZFail;
	D3DSTENCILOP	CCWStencilPass;
	DWORD			StencilReadMask;
	DWORD			StencilWriteMask;
	DWORD			StencilRef;
};

struct FD3D9BlendState
{
	UBOOL			bAlphaBlendEnable;
	D3DBLENDOP		ColorBlendOperation;
	D3DBLEND		ColorSourceBlendFactor;
	D3DBLEND		ColorDestBlendFactor;
	UBOOL			bSeparateAlphaBlendEnable;
	D3DBLENDOP		AlphaBlendOperation;
	D3DBLEND		AlphaSourceBlendFactor;
	D3DBLEND		AlphaDestBlendFactor;
	UBOOL			bAlphaTestEnable;
	D3DCMPFUNC		AlphaFunc;
	DWORD			AlphaRef;
};

#endif