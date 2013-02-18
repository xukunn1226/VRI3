#include "D3D9DrvPCH.h"

FD3D9VertexBufferPtr FD3D9DynamicDrv::CreateVertexBuffer(UINT Size, void* ResourceData, UBOOL bIsDynamic)
{
	check(Size > 0);

	const DWORD Usage = bIsDynamic ? (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY) : 0;
	const D3DPOOL Pool = bIsDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	IDirect3DVertexBuffer9* D3DVertexBuffer;
	VERIFYD3DRESULT(_Direct3DDevice->CreateVertexBuffer(
											Size, 
											Usage, 
											0, 
											Pool, 
											(IDirect3DVertexBuffer9**)&D3DVertexBuffer, NULL));

	FD3D9VertexBuffer* VertexBuffer = new FD3D9VertexBuffer(D3DVertexBuffer);
	if( ResourceData )
	{
		void* Buffer = LockVertexBuffer(VertexBuffer, 0, Size, FALSE);
		check(Buffer);
		appMemcpy(Buffer, ResourceData, Size);
		UnlockVertexBuffer(VertexBuffer);
	}

	return VertexBuffer;
}

void* FD3D9DynamicDrv::LockVertexBuffer(FD3D9VertexBufferPtr VertexBuffer, UINT Offset, UINT Size, UBOOL bReadOnly)
{
	D3DVERTEXBUFFER_DESC VertexBufferDesc;
	VERIFYD3DRESULT(VertexBuffer->GetD3DResource()->GetDesc(&VertexBufferDesc));

	const UBOOL bIsDynamic = (VertexBufferDesc.Usage & D3DUSAGE_DYNAMIC) != 0;

	const DWORD LockFlags = bIsDynamic ? D3DLOCK_DISCARD : (bReadOnly ? D3DLOCK_READONLY : 0);

	void* Data = NULL;
	VERIFYD3DRESULT(VertexBuffer->GetD3DResource()->Lock(Offset, Size, &Data, LockFlags));
	return Data;
}

void FD3D9DynamicDrv::UnlockVertexBuffer(FD3D9VertexBufferPtr VertexBuffer)
{
	VERIFYD3DRESULT(VertexBuffer->GetD3DResource()->Unlock());
}