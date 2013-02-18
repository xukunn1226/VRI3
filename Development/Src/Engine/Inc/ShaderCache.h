#ifndef __SHADERCACHE_H__
#define __SHADERCACHE_H__

extern FString GetGlobalShaderCacheFilename(EShaderPlatform InPlatform);

extern class FShaderCache* GetGlobalShaderCache(EShaderPlatform InPlatform);

class FShaderCache
{
public:
	FShaderCache(EShaderPlatform InPlatform)
		: m_nPlatform(InPlatform)
	{}

	static const DWORD* GetShaderBuilderCRC(FShaderBuilder* InShaderBuilder, EShaderPlatform InPlatform);

	static void SetShaderBuilderCRC(FShaderBuilder* InShaderBuilder, DWORD InCRC, EShaderPlatform InPlatform);

	static void RemoveShaderBuilderCRC(FShaderBuilder* InShaderBuilder, EShaderPlatform InPlatform);

	void Load(FArchive& Ar);

	void Save(FArchive& Ar, const map<FGuid, FShader*>& InShaders);

private:
	map<FShaderBuilder*, DWORD>		m_mapShaderBuilderCRC;
	BYTE							m_nPlatform;
};
#endif