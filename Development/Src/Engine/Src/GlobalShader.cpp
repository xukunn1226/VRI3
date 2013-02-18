#include "EnginePCH.h"
#include "GlobalShader.h"

TShaderMap<FGlobalShaderBuilder>*	GGlobalShaderMap[SP_NumPlatforms];


IMPLEMENT_SHADER_BUILDER(FOneColorVertexShader, TEXT("OneColorShader"), TEXT("MainVertexShader"), SF_Vertex);
IMPLEMENT_SHADER_BUILDER(FOneColorPixelShader, TEXT("OneColorShader"), TEXT("MainPixelShader"), SF_Pixel);

FShader* FGlobalShaderBuilder::CompileShader(EShaderPlatform Platform)
{
	FShaderCompilerEnvironment Environment;
	FShaderCompilerOutput Output;

	// 将被序列化，用于检查此shader source file是否有改变
	DWORD CurrentCRC = GetSourceCRC();
	FShaderCache::SetShaderBuilderCRC(this, CurrentCRC, Platform);

	if( !FShaderBuilder::CompileShader(Platform, Environment, Output) )
	{
		return NULL;
	}

	FShader* Shader = NULL;
	Shader = (*m_pfnConstructCompiledRef)(CompileShaderInitializer(this, Output));

	return Shader;
}

TShaderMap<FGlobalShaderBuilder>* GetGlobalShaderMap(EShaderPlatform Platform)
{
	if( !GGlobalShaderMap[Platform] )
	{
		GGlobalShaderMap[Platform] = new TShaderMap<FGlobalShaderBuilder>();

		FArchive* Ar = GFileManager->CreateFileReader(GetGlobalShaderCacheFilename(Platform).c_str());
		if( Ar )
		{
			SerializeGlobalShaders(Platform, *Ar);
			delete Ar;
		}

		VerifyGlobalShaders(Platform);
	}
	return GGlobalShaderMap[Platform];
}

void SerializeGlobalShaders(EShaderPlatform Platform, FArchive& Ar)
{
	TShaderMap<FGlobalShaderBuilder>& GlobalShaderMap = *GetGlobalShaderMap(Platform);
	FShaderCache* GlobalShaderCache = GetGlobalShaderCache(Platform);
	check(GlobalShaderCache);

	static DWORD GlobalShaderTag = 0xd7b12af1;
	if( Ar.IsLoading() )
	{
		DWORD Tag = 0;
		Ar << Tag;
		checkf(Tag == GlobalShaderTag, TEXT("Global shader binary file is missing tag"));
	}
	else
	{
		DWORD Tag = GlobalShaderTag;
		Ar << Tag;
	}

	// serialize global shaders
	if( Ar.IsLoading() )
	{
		GlobalShaderCache->Load(Ar);
	}
	else
	{
		map<FGuid, FShader*> Shaders;
		GlobalShaderMap.GetShaderList(Shaders);
		GlobalShaderCache->Save(Ar, Shaders);
	}

	// serialize the global shader map
	GlobalShaderMap.Serialize(Ar);
}

void VerifyGlobalShaders(EShaderPlatform Platform)
{

}
