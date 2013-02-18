#include "D3D9DrvPCH.h"

FD3D9VertexShaderPtr FD3D9DynamicDrv::CreateVertexShader(const vector<BYTE> &Code)
{
	IDirect3DVertexShader9*	D3DVertexShader;
	VERIFYD3DRESULT(_Direct3DDevice->CreateVertexShader(
										(DWORD*)&Code.at(0), 
										(IDirect3DVertexShader9**)&D3DVertexShader));

	FD3D9VertexShader* VertexShader = new FD3D9VertexShader(D3DVertexShader);

	return VertexShader;
}

FD3D9PixelShaderPtr FD3D9DynamicDrv::CreatePixelShader(const vector<BYTE> &Code)
{
	IDirect3DPixelShader9*	D3DPixelShader;
	VERIFYD3DRESULT(_Direct3DDevice->CreatePixelShader(
										(DWORD*)&Code.at(0), 
										(IDirect3DPixelShader9**)&D3DPixelShader));

	FD3D9PixelShader* PixelShader = new FD3D9PixelShader(D3DPixelShader);
	return PixelShader;
}

FD3D9BoundShaderStatePtr FD3D9DynamicDrv::CreateBoundShaderState(const FD3D9VertexDeclarationPtr VertexDeclaration,
																 const FD3D9VertexShaderPtr VertexShader,
																 const FD3D9PixelShaderPtr PixelShader)
{
	//FD3D9BoundShaderStatePtr BoundShaderState = new FD3D9BoundShaderState();
	//BoundShaderState->VertexDeclaration = VertexDeclaration;
	//BoundShaderState->VertexShader = VertexShader;
	//BoundShaderState->PixelShader = PixelShader;

	//return BoundShaderState;
	return NULL;
}

void FD3D9DynamicDrv::SetBoundShaderState(FD3D9BoundShaderStatePtr BoundShaderState)
{
	_Direct3DDevice->SetVertexDeclaration(BoundShaderState->VertexDeclaration);
	_Direct3DDevice->SetVertexShader(BoundShaderState->VertexShader);
	_Direct3DDevice->SetPixelShader(BoundShaderState->PixelShader);
}