#include "D3D9DrvPCH.h"

#define IsDeviceLost() (_Direct3DDevice->TestCooperativeLevel() != D3D_OK)

/**
 * Set sampler state
 * 
 * @param MaxMipLevel	最大的细节层数[0, n-1]，0为最大，默认值为0
 */
void FD3D9DynamicDrv::SetSamplerState(UINT SamplerIndex, FD3D9SamplerState SamplerState, FD3D9TexturePtr Texture, INT MaxMipLevel)
{
	_Direct3DDevice->SetTexture(SamplerIndex, Texture->GetD3DResource());
	_Direct3DDevice->SetSamplerState(SamplerIndex, D3DSAMP_SRGBTEXTURE, Texture->IsSRGB());
	_Direct3DDevice->SetSamplerState(SamplerIndex, D3DSAMP_MINFILTER, SamplerState.MinFilter);
	_Direct3DDevice->SetSamplerState(SamplerIndex, D3DSAMP_MAGFILTER, SamplerState.MagFilter);
	_Direct3DDevice->SetSamplerState(SamplerIndex, D3DSAMP_MIPFILTER, SamplerState.MipFilter);
	_Direct3DDevice->SetSamplerState(SamplerIndex, D3DSAMP_ADDRESSU, SamplerState.AddressU);
	_Direct3DDevice->SetSamplerState(SamplerIndex, D3DSAMP_ADDRESSV, SamplerState.AddressV);
	_Direct3DDevice->SetSamplerState(SamplerIndex, D3DSAMP_ADDRESSW, SamplerState.AddressW);
	_Direct3DDevice->SetSamplerState(SamplerIndex, D3DSAMP_MIPMAPLODBIAS, SamplerState.MipMapLODBias);
	_Direct3DDevice->SetSamplerState(SamplerIndex, D3DSAMP_MAXMIPLEVEL, MaxMipLevel);
}

void FD3D9DynamicDrv::SetRasterizerState(FD3D9RasterizerState RasterizerState)
{
	_Direct3DDevice->SetRenderState(D3DRS_FILLMODE, RasterizerState.FillMode);
	_Direct3DDevice->SetRenderState(D3DRS_CULLMODE, RasterizerState.CullMode);

	extern FLOAT GDepthBiasOffset;
	_Direct3DDevice->SetRenderState(D3DRS_DEPTHBIAS, FLOAT_TO_DWORD(RasterizerState.DepthBias + GDepthBiasOffset));
	_Direct3DDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, FLOAT_TO_DWORD(RasterizerState.SlopeScaleDepthBias));
}

void FD3D9DynamicDrv::SetDepthState(FD3D9DepthState DepthState)
{
	_Direct3DDevice->SetRenderState(D3DRS_ZENABLE, DepthState.bZEnable);
	_Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, DepthState.bZWriteEnable);
	_Direct3DDevice->SetRenderState(D3DRS_ZFUNC, DepthState.ZFunc);
}

void FD3D9DynamicDrv::SetStencilState(FD3D9StencilState StencilState)
{
	_Direct3DDevice->SetRenderState(D3DRS_STENCILENABLE, StencilState.bStencilEnable);
	_Direct3DDevice->SetRenderState(D3DRS_STENCILFUNC, StencilState.StencilFunc);
	_Direct3DDevice->SetRenderState(D3DRS_STENCILFAIL, StencilState.StencilFail);
	_Direct3DDevice->SetRenderState(D3DRS_STENCILZFAIL, StencilState.StencilZFail);
	_Direct3DDevice->SetRenderState(D3DRS_STENCILPASS, StencilState.StencilPass);

	_Direct3DDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, StencilState.bTwoSidedStencilMode);
	_Direct3DDevice->SetRenderState(D3DRS_CCW_STENCILFUNC, StencilState.CCWStencilFunc);
	_Direct3DDevice->SetRenderState(D3DRS_CCW_STENCILFAIL, StencilState.CCWStencilFail);
	_Direct3DDevice->SetRenderState(D3DRS_CCW_STENCILZFAIL, StencilState.CCWStencilZFail);
	_Direct3DDevice->SetRenderState(D3DRS_CCW_STENCILPASS, StencilState.CCWStencilPass);

	_Direct3DDevice->SetRenderState(D3DRS_STENCILMASK, StencilState.StencilReadMask);
	_Direct3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK, StencilState.StencilWriteMask);
	_Direct3DDevice->SetRenderState(D3DRS_STENCILREF, StencilState.StencilRef);
}

void FD3D9DynamicDrv::SetBlendState(FD3D9BlendState BlendState)
{
	_Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, BlendState.bAlphaBlendEnable);
	_Direct3DDevice->SetRenderState(D3DRS_BLENDOP, BlendState.ColorBlendOperation);
	_Direct3DDevice->SetRenderState(D3DRS_SRCBLEND, BlendState.ColorSourceBlendFactor);
	_Direct3DDevice->SetRenderState(D3DRS_DESTBLEND, BlendState.ColorDestBlendFactor);
	_Direct3DDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, BlendState.bSeparateAlphaBlendEnable);
	_Direct3DDevice->SetRenderState(D3DRS_BLENDOPALPHA, BlendState.AlphaBlendOperation);
	_Direct3DDevice->SetRenderState(D3DRS_SRCBLENDALPHA, BlendState.AlphaSourceBlendFactor);
	_Direct3DDevice->SetRenderState(D3DRS_DESTBLENDALPHA, BlendState.AlphaDestBlendFactor);

	_Direct3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, BlendState.bAlphaTestEnable);
	_Direct3DDevice->SetRenderState(D3DRS_ALPHAFUNC, BlendState.AlphaFunc);
	_Direct3DDevice->SetRenderState(D3DRS_ALPHAREF, BlendState.AlphaRef);
}

void FD3D9DynamicDrv::SetColorWriteEnable(UBOOL bEnable)
{
	DWORD Value = D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED;
	_Direct3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, bEnable ? Value : 0);
}

void FD3D9DynamicDrv::SetColorWriteMask(UINT ColorWriteMask)
{
	DWORD Value;
	Value = (ColorWriteMask & CW_Alpha) ? D3DCOLORWRITEENABLE_ALPHA : 0;
	Value |= (ColorWriteMask & CW_Red) ? D3DCOLORWRITEENABLE_RED : 0;
	Value |= (ColorWriteMask & CW_Green) ? D3DCOLORWRITEENABLE_GREEN : 0;
	Value |= (ColorWriteMask & CW_Blue) ? D3DCOLORWRITEENABLE_BLUE : 0;
	_Direct3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, Value);
}

static const D3DRENDERSTATETYPE MRTColorWriteEnableStates[4] = 
	{D3DRS_COLORWRITEENABLE,D3DRS_COLORWRITEENABLE1,D3DRS_COLORWRITEENABLE2,D3DRS_COLORWRITEENABLE3};

void FD3D9DynamicDrv::SetMRTColorWriteEnable(UINT TargetIndex, UBOOL bEnable)
{
	DWORD Value = D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED;
	_Direct3DDevice->SetRenderState(MRTColorWriteEnableStates[TargetIndex], bEnable ? Value : 0);
}

void FD3D9DynamicDrv::SetMRTColorWriteMask(UINT TargetIndex, UINT ColorWriteMask)
{
	DWORD Value;
	Value = (ColorWriteMask & CW_Alpha) ? D3DCOLORWRITEENABLE_ALPHA : 0;
	Value |= (ColorWriteMask & CW_Red) ? D3DCOLORWRITEENABLE_RED : 0;
	Value |= (ColorWriteMask & CW_Green) ? D3DCOLORWRITEENABLE_GREEN : 0;
	Value |= (ColorWriteMask & CW_Blue) ? D3DCOLORWRITEENABLE_BLUE : 0;
	_Direct3DDevice->SetRenderState(MRTColorWriteEnableStates[TargetIndex], Value);
}

void FD3D9DynamicDrv::SetScissorRect(UBOOL bEnable, UINT MinX, UINT MaxX, UINT MinY, UINT MaxY)
{
	if( bEnable )
	{
		RECT ScissorRect;
		ScissorRect.left = MinX;
		ScissorRect.right = MaxX;
		ScissorRect.top = MinY;
		ScissorRect.bottom = MaxY;
		_Direct3DDevice->SetScissorRect(&ScissorRect);
	}
	_Direct3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, bEnable);
}

// MinZ，MaxZ表示将此范围内的场景渲染出来，而不起裁剪场景作用
void FD3D9DynamicDrv::SetViewport(UINT MinX, UINT MaxX, UINT MinY, UINT MaxY, UINT MinZ, UINT MaxZ)
{
	D3DVIEWPORT9 Viewport = { MinX, MinY, MaxX - MinX, MaxY - MinY, MinZ, MaxZ };

	if( Viewport.Width > 0 && Viewport.Height > 0 )
	{
		_Direct3DDevice->SetViewport(&Viewport);
	}
}

void FD3D9DynamicDrv::SetRenderTarget(FD3D9SurfacePtr NewRenderTarget, FD3D9SurfacePtr NewDepthStencilTarget)
{
	for(UINT TextureIndex = 0; TextureIndex < 16; ++TextureIndex)
	{
		_Direct3DDevice->SetTexture(TextureIndex, NULL);
	}

	if( !NewRenderTarget )
	{
		check(_BackBuffer);
		_Direct3DDevice->SetRenderTarget(0, _BackBuffer->GetD3DResource());
	}
	else
	{
		_Direct3DDevice->SetRenderTarget(0, NewRenderTarget->GetD3DResource());
	}

	_Direct3DDevice->SetDepthStencilSurface(NewDepthStencilTarget ? NewDepthStencilTarget->GetD3DResource() : NULL);

	// when the back buffer is set, and set the correct viewport
	if( _DrawingViewport && (!NewRenderTarget || NewRenderTarget == _BackBuffer) )
	{
		D3DVIEWPORT9 Viewport = { 0, 0, _DrawingViewport->GetSizeX(), _DrawingViewport->GetSizeY(), 0.0f, 1.0f };
		_Direct3DDevice->SetViewport(&Viewport);
	}
}

void FD3D9DynamicDrv::SetMRTRenderTarget(INT TargetIndex, FD3D9SurfacePtr NewRenderTarget)
{
	for(UINT TextureIndex = 0; TextureIndex < 16; ++TextureIndex)
	{
		_Direct3DDevice->SetTexture(TextureIndex, NULL);
	}
	_Direct3DDevice->SetRenderTarget(TargetIndex, NewRenderTarget ? NewRenderTarget->GetD3DResource() : NULL);
}

/** see also: DirectX document —— Efficiently Drawing Multiple Instances of Geometry (Direct3D 9) */
void FD3D9DynamicDrv::SetStreamSource(UINT StreamIndex, FD3D9VertexBufferPtr VertexBuffer, UINT Stride, UBOOL bUseInstanceIndex, UINT NumInstances)
{
	DWORD Frequency = 1;
	if( bUseInstanceIndex || NumInstances > 1 )
	{
		Frequency = bUseInstanceIndex ? 
			(D3DSTREAMSOURCE_INSTANCEDATA | 1) :
			(D3DSTREAMSOURCE_INDEXEDDATA | NumInstances);
	}

	_Direct3DDevice->SetStreamSource(StreamIndex, VertexBuffer->GetD3DResource(), 0, Stride);
	// Frequency = 1, meaning reset the vertex stream frequency back to its default state
	_Direct3DDevice->SetStreamSourceFreq(StreamIndex, Frequency);
}

void FD3D9DynamicDrv::SetVertexShaderParameter(FD3D9VertexShaderPtr VertexShader, UINT BaseIndex, UINT NumBytes, const void *NewValue)
{
	_Direct3DDevice->SetVertexShaderConstantF( 
			BaseIndex / _NumBytesPerShaderRegister, 
			(FLOAT*)GetPaddedShaderParameterValue(NewValue, NumBytes),
			(NumBytes + _NumBytesPerShaderRegister - 1) / _NumBytesPerShaderRegister );
}

void FD3D9DynamicDrv::SetVertexShaderBoolParameter(FD3D9VertexShaderPtr VertexShader, UINT BaseIndex, UBOOL NewValue)
{
	_Direct3DDevice->SetVertexShaderConstantB( BaseIndex / _NumBytesPerShaderRegister, (BOOL*)&NewValue, 1 );
}

void FD3D9DynamicDrv::SetPixelShaderParameter(FD3D9PixelShaderPtr PixelShader, UINT BaseIndex, UINT NumBytes, const void* NewValue)
{
	_Direct3DDevice->SetPixelShaderConstantF(	
			BaseIndex / _NumBytesPerShaderRegister,
			(FLOAT*)GetPaddedShaderParameterValue(NewValue, NumBytes),
			(NumBytes + _NumBytesPerShaderRegister - 1) / _NumBytesPerShaderRegister );
}

void FD3D9DynamicDrv::SetPixelShaderBoolParameter(FD3D9PixelShaderPtr PixelShader, UINT BaseIndex, UBOOL NewValue)
{
	_Direct3DDevice->SetPixelShaderConstantB( BaseIndex / _NumBytesPerShaderRegister, (BOOL*)&NewValue, 1 );
}

/**
 * Set engine shader parameter for the view
 */
void FD3D9DynamicDrv::SetViewParameters(const FMatrix &ViewProjMatrix, const FVector4 &ViewOrigin)
{
	_Direct3DDevice->SetVertexShaderConstantF(VSR_ViewProjMatrix, (const FLOAT*)&ViewProjMatrix, 4);
	_Direct3DDevice->SetVertexShaderConstantF(VSR_ViewOrigin, (const FLOAT*)&ViewOrigin, 1);
}

static D3DPRIMITIVETYPE GetD3DPrimitiveType(UINT PrimitiveType)
{
	switch(PrimitiveType)
	{
	case PT_TriangleList: return D3DPT_TRIANGLELIST;
	case PT_TriangleStrip: return D3DPT_TRIANGLESTRIP;
	case PT_LineList: return D3DPT_LINELIST;
	default: debugf(TEXT("Unknown primitive type: %u"),PrimitiveType);
	};
	return D3DPT_TRIANGLELIST;
}

void FD3D9DynamicDrv::DrawPrimitive(UINT PrimitiveType, UINT BaseVertexIndex, UINT NumPrimitives)
{
	check(NumPrimitives > 0);
	if( !IsDeviceLost() )
	{
		if( NumPrimitives > 0 )
		{
			VERIFYD3DRESULT(_Direct3DDevice->DrawPrimitive(
				GetD3DPrimitiveType(PrimitiveType), 
				BaseVertexIndex, 
				NumPrimitives));
		}
	}
}

void FD3D9DynamicDrv::DrawIndexedPrimitive(FD3D9IndexBufferPtr IndexBuffer, 
										   UINT PrimitiveType, 
										   UINT BaseVertexIndex, 
										   UINT MinIndex,
										   UINT NumVertices,
										   UINT StartIndex,
										   UINT NumPrimitives)
{
	check(NumPrimitives > 0);
	if( !IsDeviceLost() )
	{
		_Direct3DDevice->SetIndices(IndexBuffer->GetD3DResource());
		if( NumPrimitives > 0 )
		{
			_Direct3DDevice->DrawIndexedPrimitive(
				GetD3DPrimitiveType(PrimitiveType),
				BaseVertexIndex,
				MinIndex,
				NumVertices,
				StartIndex,
				NumPrimitives);
		}
	}
}

void FD3D9DynamicDrv::DrawPrimitiveUP(UINT PrimitiveType, UINT NumPrimitives, const void *VertexData, UINT VertexStride)
{
	if( !IsDeviceLost() )
	{
		_Direct3DDevice->SetStreamSourceFreq(0, 1);

		_Direct3DDevice->DrawPrimitiveUP(
			GetD3DPrimitiveType(PrimitiveType), 
			NumPrimitives, 
			VertexData, 
			VertexStride);
	}
}

void FD3D9DynamicDrv::DrawIndexedPrimitiveUP(UINT PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT NumPrimitives, const void *IndexData, UINT IndexStride, const void *VertexData, UINT VertexStride)
{
	if( !IsDeviceLost() )
	{
		_Direct3DDevice->SetStreamSourceFreq(0, 1);

		_Direct3DDevice->DrawIndexedPrimitiveUP(GetD3DPrimitiveType(PrimitiveType),
												  MinVertexIndex,
												  NumVertices,
												  NumPrimitives,
												  IndexData,
												  IndexStride == sizeof(WORD) ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
												  VertexData,
												  VertexStride);
	}
}

void FD3D9DynamicDrv::Clear(UBOOL bClearColor, const FColor &Color, UBOOL bClearDepth, FLOAT Depth, UBOOL bClearStencil, DWORD Stencil)
{
	DWORD Flags = 0;
	if( bClearColor )
	{
		Flags |= D3DCLEAR_TARGET;
	}
	if( bClearDepth )
	{
		Flags |= D3DCLEAR_ZBUFFER;
	}
	if( bClearStencil )
	{
		Flags |= D3DCLEAR_STENCIL;
	}

	FLinearColor QuantizedColor;
	QuantizedColor = Color.ConvertToLinear();
	_Direct3DDevice->Clear(0, NULL, Flags, D3DCOLOR_RGBA(QuantizedColor.R, QuantizedColor.G, QuantizedColor.B, QuantizedColor.A), Depth, Stencil);
}