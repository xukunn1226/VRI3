#include "EnginePCH.h"
#include "ShaderCache.h"

FString GetShaderCacheFilename(const TCHAR* Filename, const TCHAR* Extension, EShaderPlatform Platform)
{
	return FStringUtil::Sprintf(TEXT("%s%s-%s.%s"), appGameDir(), Filename, ShaderPlatformToText(Platform), Extension);
}

FString GetGlobalShaderCacheFilename(EShaderPlatform InPlatform)
{
	return GetShaderCacheFilename(TEXT("GlobalShaderCache"), TEXT(".bin"), InPlatform);
}

FShaderCache* GGlobalShaderCache[SP_NumPlatforms];

FShaderCache* GetGlobalShaderCache(EShaderPlatform InPlatform)
{
	if( !GGlobalShaderCache[InPlatform] )
	{
		GGlobalShaderCache[InPlatform] = new FShaderCache(InPlatform);
	}
	return GGlobalShaderCache[InPlatform];
}

const DWORD* FShaderCache::GetShaderBuilderCRC(FShaderBuilder *InShaderBuilder, EShaderPlatform InPlatform)
{
	if( InShaderBuilder->GetGlobalShaderBuilder() )
	{
		FShaderCache* ShaderCache = GetGlobalShaderCache(InPlatform);
		check(ShaderCache);
		
		map<FShaderBuilder*, DWORD>::iterator it = ShaderCache->m_mapShaderBuilderCRC.find(InShaderBuilder);
		return (it != ShaderCache->m_mapShaderBuilderCRC.end() ? &it->second : NULL);
	}
	return NULL;
}

void FShaderCache::SetShaderBuilderCRC(FShaderBuilder *InShaderBuilder, DWORD InCRC, EShaderPlatform InPlatform)
{
	if( InShaderBuilder->GetGlobalShaderBuilder() )
	{
		FShaderCache* ShaderCache = GetGlobalShaderCache(InPlatform);
		check(ShaderCache);

		ShaderCache->m_mapShaderBuilderCRC[InShaderBuilder] = InCRC;
	}
}

void FShaderCache::RemoveShaderBuilderCRC(FShaderBuilder *InShaderBuilder, EShaderPlatform InPlatform)
{
	if( InShaderBuilder->GetGlobalShaderBuilder() )
	{
		FShaderCache* ShaderCache = GetGlobalShaderCache(InPlatform);
		check(ShaderCache);

		ShaderCache->m_mapShaderBuilderCRC.erase(InShaderBuilder);
	}
}

void FShaderCache::Load(FArchive& Ar)
{
	Ar << m_nPlatform;

	UINT NumShaderBuilderCRC = 0;
	Ar << NumShaderBuilderCRC;
	for(UINT IndexBuilder = 0; IndexBuilder < NumShaderBuilderCRC; ++IndexBuilder)
	{
		FShaderBuilder* ShaderBuilder = NULL;
		Ar << ShaderBuilder;
		DWORD CRC = 0;
		Ar << CRC;

		if( ShaderBuilder )
		{
			m_mapShaderBuilderCRC[ShaderBuilder] = CRC;
		}
	}

	// serialize the global shaders
	UINT NumShaders = 0;
	UINT NumDesertedShaders = 0; 
	UINT NumRedundantShaders = 0;
	vector<FString> OutdatedShaderBuilders;

	Ar << NumShaders;
	for(UINT IndexShader = 0; IndexShader < NumShaders; ++IndexShader)
	{
		FShaderBuilder* ShaderBuilder = NULL;
		FGuid ShaderId;
		Ar << ShaderBuilder << ShaderId;

		INT SkipOffset = 0;
		Ar << SkipOffset;

		if( !ShaderBuilder )
		{
			++NumDesertedShaders;
			Ar.Seek(SkipOffset);		// this shader builder doesn't exist any more, skip the shader
		}
		else
		{
			DWORD CurrentCRC = 0;
			DWORD SavedCRC = 0;

			// 比较当前与之前版本的CRC，检测是否有改动
			CurrentCRC = ShaderBuilder->GetSourceCRC();
			map<FShaderBuilder*, DWORD>::const_iterator it = m_mapShaderBuilderCRC.find(ShaderBuilder);
			if( it != m_mapShaderBuilderCRC.end() )
			{
				SavedCRC = it->second;
			}

			FShader* Shader = ShaderBuilder->FindShaderById(ShaderId);
			if( Shader )
			{
				++NumRedundantShaders;
				Ar.Seek(SkipOffset);		// has already exist, skip it		
			}
			else if( SavedCRC != CurrentCRC )
			{
				++NumDesertedShaders;
				Ar.Seek(SkipOffset);
				if( SavedCRC != 0 )			// denote SHADER BUILDER exists, but it has changed
				{
					OutdatedShaderBuilders.push_back(ShaderBuilder->GetShaderName());
				}
			}
			else
			{
				// the shader is compatiable, create it
				Shader = ShaderBuilder->ConstructSerialization();

				UBOOL bShaderHasOutdatedParameters = Shader->Serialize(Ar);
				if( bShaderHasOutdatedParameters )
				{
					ShaderBuilder->UnregisterShader(Shader);
					delete Shader;
				}
				check(Ar.Tell() == SkipOffset);
			}
		}
	}

	if( OutdatedShaderBuilders.size() > 0 )
	{
		debugf(TEXT("Skip %d outdated FShaderBuilder"), OutdatedShaderBuilders.size());
		for(UINT IndexBuilder = 0; IndexBuilder < OutdatedShaderBuilders.size(); ++IndexBuilder)
		{
			debugf(TEXT("	%s"), OutdatedShaderBuilders.at(IndexBuilder).c_str());
		}
	}

	if( NumShaders > 0 )
	{
		debugf(TEXT("Loaded %d shaders (%d deserted, %d redundant)"), NumShaders, NumDesertedShaders, NumRedundantShaders);
	}
}

void FShaderCache::Save(FArchive& Ar, const map<FGuid, FShader*>& InShaders)
{
	Ar << m_nPlatform;

	// serialize the global shader crc
	UINT NumShaderBuilderCRC = m_mapShaderBuilderCRC.size();
	Ar << NumShaderBuilderCRC;
	map<FShaderBuilder*, DWORD>::iterator it;
	for( it = m_mapShaderBuilderCRC.begin(); it != m_mapShaderBuilderCRC.end(); ++it )
	{
		FShaderBuilder* ShaderBuilder = it->first;
		Ar << ShaderBuilder;
		Ar << it->second;
	}

	// serialize the global shaders
	UINT NumShaders = InShaders.size();
	Ar << NumShaders;
	for( map<FGuid, FShader*>::const_iterator it = InShaders.begin(); it != InShaders.end(); ++it )
	{
		FShader* Shader = it->second;

		// shader builder的序列化，在加载时可用于检测此类型的shader是否仍存在
		FShaderBuilder* ShaderBuilder = Shader->GetShaderBuilder();
		FGuid ShaderId = Shader->GetId();
		Ar << ShaderBuilder << ShaderId;

		// 占个位先。。。应该记录序列化此shader的结束位置
		INT SkipOffset = Ar.Tell();
		Ar << SkipOffset;

		Shader->Serialize(Ar);

		INT EndOffset = Ar.Tell();
		Ar.Seek(SkipOffset);		// 定位回之前位置
		Ar << EndOffset;			// 记录此shader的结束位置
		Ar.Seek(EndOffset);			// 定位结束位置，继续下一个shader的序列化
	}
}