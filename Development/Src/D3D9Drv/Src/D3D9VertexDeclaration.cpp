#include "D3D9DrvPCH.h"

/**
D3DVERTEXELEMENT9 decl[] =

{

	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},

	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},

	{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1},

	{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2},

	D3DDECL_END()

};

// 数据成员的语义（: usage-type [usage-index]）定义这个映射
struct VS_INPUT
{

vector position    : POSITION;

vector normal     : NORMAL0;

vector faceNormal1 : NORMAL1;

vector faceNormal2 : NORMAL2;

};
*/
FD3D9VertexDeclarationCache::FKey::FKey(const FVertexDeclarationElementList &InElements)
{
	for(UINT Index = 0; Index < InElements.size(); ++Index)
	{
		D3DVERTEXELEMENT9 VertexElement;
		VertexElement.Stream = InElements.at(Index).StreamIndex;
		VertexElement.Offset = InElements.at(Index).Offset;
		switch(InElements.at(Index).Type)
		{
		case VET_FLOAT1:	VertexElement.Type = D3DDECLTYPE_FLOAT1;	break;
		case VET_FLOAT2:	VertexElement.Type = D3DDECLTYPE_FLOAT2;	break;
		case VET_FLOAT3:	VertexElement.Type = D3DDECLTYPE_FLOAT3;	break;
		case VET_FLOAT4:	VertexElement.Type = D3DDECLTYPE_FLOAT4;	break;
		case VET_UByte4:	VertexElement.Type = D3DDECLTYPE_UBYTE4;	break;
		case VET_UByte4N:	VertexElement.Type = D3DDECLTYPE_UBYTE4N;	break;
		case VET_Color:		VertexElement.Type = D3DDECLTYPE_D3DCOLOR;	break;
		case VET_Short2:	VertexElement.Type = D3DDECLTYPE_SHORT2;	break;
		case VET_Short2N:	VertexElement.Type = D3DDECLTYPE_SHORT2N;	break;
		case VET_Half2:		VertexElement.Type = D3DDECLTYPE_FLOAT16_2;	break;
		};
		VertexElement.Method = D3DDECLMETHOD_DEFAULT;
		switch(InElements.at(Index).Usage)
		{
		case VEU_Position:		VertexElement.Usage = D3DDECLUSAGE_POSITION;	break;
		case VEU_TextureCoord:	VertexElement.Usage = D3DDECLUSAGE_TEXCOORD;	break;
		case VEU_BlendWeight:	VertexElement.Usage = D3DDECLUSAGE_BLENDWEIGHT;	break;
		case VEU_BlendIndices:	VertexElement.Usage = D3DDECLUSAGE_BLENDINDICES;break;
		case VEU_Normal:		VertexElement.Usage = D3DDECLUSAGE_NORMAL;		break;
		case VEU_Tangent:		VertexElement.Usage = D3DDECLUSAGE_TANGENT;		break;
		case VEU_Binormal:		VertexElement.Usage = D3DDECLUSAGE_BINORMAL;	break;
		case VEU_Color:			VertexElement.Usage = D3DDECLUSAGE_COLOR;		break;
		};
		VertexElement.UsageIndex = InElements.at(Index).UsageIndex;

		VertexElements.push_back(VertexElement);
	}

	D3DVERTEXELEMENT9 EndElement = D3DDECL_END();
	VertexElements.push_back(EndElement);

	Hash = appMemCrc(&VertexElements, sizeof(D3DVERTEXELEMENT9) * VertexElements.size());
}

UBOOL FD3D9VertexDeclarationCache::FKey::operator== (const FKey& Other) const
{
	return VertexElements.size() == Other.VertexElements.size() &&
		!appMemcmp( GetVertexElements(), Other.GetVertexElements(), sizeof(D3DVERTEXELEMENT9) * VertexElements.size() );
}

//bool FD3D9VertexDeclarationCache::FKey::operator< (const FKey& Other) const
//{
//	if( Hash > Other.Hash )
//	{
//		return FALSE;
//	}
//
//	return TRUE;
//}

FD3D9VertexDeclarationPtr FD3D9VertexDeclarationCache::GetVertexDeclaration(const FKey& Declaration)
{
	map<DWORD, FD3D9VertexDeclarationPtr>::iterator it;
	it = _VertexDeclarationMap.find(Declaration.GetHash());

	if( it == _VertexDeclarationMap.end() )
	{
		FD3D9VertexDeclarationPtr	NewDeclaration;
		_D3DDrv->GetDevice()->CreateVertexDeclaration(Declaration.GetVertexElements(),
			(IDirect3DVertexDeclaration9**)NewDeclaration.GetInitReference());
		_VertexDeclarationMap[Declaration.GetHash()] = NewDeclaration;
		return NewDeclaration;
	}

	return it->second;
}

FD3D9VertexDeclarationPtr FD3D9DynamicDrv::CreateVertexDeclaration(const FVertexDeclarationElementList &Elements)
{
	return _VertexDeclarationCache.GetVertexDeclaration(FD3D9VertexDeclarationCache::FKey(Elements));
}