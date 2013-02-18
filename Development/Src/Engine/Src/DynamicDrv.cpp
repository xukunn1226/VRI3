#include "EnginePCH.h"

extern FDynamicDrv* D3D9CreateDrv();

FDynamicDrv* GDynamicDrv = NULL;

void DrvInit()
{
	if( !GDynamicDrv )
	{
		GDynamicDrv = D3D9CreateDrv();
	}
}

void DrvExit()
{
	delete GDynamicDrv;
	GDynamicDrv = NULL;
}