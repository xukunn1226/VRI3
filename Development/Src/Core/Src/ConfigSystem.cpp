#include "CorePCH.h"

const TCHAR* DefaultConfigFilename[] = 
{
	TEXT("DefaultCore")
	TEXT("DefaultEngine"),
};

void FConfigSystem::Init()
{
	for(INT i = 0; i < ECT_Size; ++i)
	{
		const TCHAR* ConfigFilename = DefaultConfigFilename[i];
		LoadConfig(ConfigFilename);
	}
}

UBOOL FConfigSystem::LoadConfig(const TCHAR* ConfigFilename)
{
	return FALSE;
}

UBOOL FConfigSystem::GetSectionKeyValue(EConfigType Type, const TCHAR* Section, const TCHAR* Key, FString& Value)
{
	return FALSE;
}