#include "D3D9DrvPCH.h"

FD3D9IndexBufferPtr FD3D9DynamicDrv::CreateIndexBuffer(UINT Stride, UINT Size, void* ResourceData, UBOOL bIsDynamic)
{
	check( Size > 0 );

	const DWORD Usage = bIsDynamic ? (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY) : 0;
	const D3DPOOL Pool = bIsDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
	const D3DFORMAT Format = (Stride == sizeof(WORD) ? D3DFMT_INDEX16 : D3DFMT_INDEX32);

	IDirect3DIndexBuffer9* D3DIndexBuffer;
	VERIFYD3DRESULT(_Direct3DDevice->CreateIndexBuffer(
											Size, 
											Usage, 
											Format, 
											Pool, 
											(IDirect3DIndexBuffer9**)&D3DIndexBuffer, NULL));

	FD3D9IndexBuffer* IndexBuffer = new FD3D9IndexBuffer(D3DIndexBuffer);
	if( ResourceData )
	{
		void* Buffer = LockIndexBuffer(IndexBuffer, 0, Size);
		check(Buffer);
		appMemcpy(Buffer, ResourceData, Size);
		UnlockIndexBuffer(IndexBuffer);
	}

	return IndexBuffer;
}

void* FD3D9DynamicDrv::LockIndexBuffer(FD3D9IndexBufferPtr IndexBuffer, UINT Offset, UINT Size)
{
	D3DINDEXBUFFER_DESC IndexBufferDesc;
	VERIFYD3DRESULT(IndexBuffer->GetD3DResource()->GetDesc(&IndexBufferDesc));

	const UBOOL bIsDynamic = (IndexBufferDesc.Usage & D3DUSAGE_DYNAMIC) != 0;

	// For dynamic IBs, discard the previous contents before locking.
	const DWORD LockFlags = bIsDynamic ? D3DLOCK_DISCARD : 0;

	void* Data = NULL;
	VERIFYD3DRESULT(IndexBuffer->GetD3DResource()->Lock(Offset, Size, &Data, LockFlags));
	return Data;
}

void FD3D9DynamicDrv::UnlockIndexBuffer(FD3D9IndexBufferPtr IndexBuffer)
{
	VERIFYD3DRESULT(IndexBuffer->GetD3DResource()->Unlock());
}