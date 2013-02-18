#include "EnginePCH.h"
#include "ShaderManager.h"

void SetVertexShaderBool(
	const FShaderParameter& Parameter,
	UBOOL Value)
{
	if( Parameter.GetBufferCount() > 0 )
	{
		GDynamicDrv->SetVertexShaderBoolParameter(
			NULL,
			Parameter.GetBufferIndex(),
			Value);
	}
}

extern void SetPixelShaderBool(
   const FShaderParameter& Parameter,
   UBOOL Value)
{
	if( Parameter.GetBufferCount() > 0 )
	{
		GDynamicDrv->SetPixelShaderBoolParameter(
			NULL,
			Parameter.GetBufferIndex(),
			Value);
	}
}

list<FShaderBuilder*>*& FShaderBuilder::GetShaderBuilderList()
{
	static list<FShaderBuilder*>* ShaderBuilderSet = new list<FShaderBuilder*>();
	return ShaderBuilderSet;
}

map<FString, FShaderBuilder*>& FShaderBuilder::GetShaderNameToBuilderMap()
{
	static map<FString, FShaderBuilder*> NameToBuilder;
	return NameToBuilder;
}

void FShaderBuilder::RegisterShader(FShader *InShader)
{
	m_mapShaderId.insert(pair<FGuid, FShader*>(InShader->GetId(), InShader));
	m_mapShaderCode.insert(pair<FShaderKey, FShader*>(InShader->GetShaderKey(), InShader));
}

void FShaderBuilder::UnregisterShader(FShader *InShader)
{
	m_mapShaderId.erase(InShader->GetId());
	m_mapShaderCode.erase(InShader->GetShaderKey());
}

FShader* FShaderBuilder::FindShaderById(const FGuid &Id) const
{
	map<FGuid, FShader*>::const_iterator it = m_mapShaderId.find(Id);
	return it != m_mapShaderId.end() ? it->second : NULL;
}

FShader* FShaderBuilder::FindShaderByOutput(const FShaderCompilerOutput& Output) const
{
	map<FShaderKey, FShader*>::const_iterator it = m_mapShaderCode.find(FShaderKey(Output.ShaderCode, Output.ParameterMap));
	return it != m_mapShaderCode.end() ? it->second : NULL;
}

DWORD FShaderBuilder::GetSourceCRC()
{
	return ::GetShaderFileCRC(GetShaderName());
}

UBOOL FShaderBuilder::CompileShader(EShaderPlatform Platform, const FShaderCompilerEnvironment& InEnvironment, FShaderCompilerOutput& Output)
{
	FShaderCompilerEnvironment Environment = InEnvironment;
	(*m_pfnModifyCompilationEnvironmentRef)(Platform, Environment);

	FShaderTarget Target;
	Target.Platform = Platform;
	Target.Frequency = m_nFrequency;

	return ::CompileShader(m_sSourceFilename, m_sFunctionName, Target, InEnvironment, Output, TRUE);
}

FArchive& operator<<(FArchive& Ar, FShaderBuilder*& Builder)
{
	if( Ar.IsLoading() )
	{
		TCHAR* FactoryName;
		Ar << FactoryName;

		Builder = NULL;

		map<FString, FShaderBuilder*>::iterator it = FShaderBuilder::GetShaderNameToBuilderMap().find(FactoryName);
		if( it != FShaderBuilder::GetShaderNameToBuilderMap().end() )
		{
			Builder = it->second;
		}
	}
	else
	{
		TCHAR* Name = const_cast<TCHAR*>(Builder->GetShaderName());
		Ar << (TCHAR*)Name;
	}
	return Ar;
}


FShader::FShader(const CompileShaderInitializer& Initializer)
: m_pkShaderBuilder(Initializer.m_pkBuilder),
  m_kShaderKey(Initializer.m_arrCode, Initializer.m_kParameterMap),
  m_kShaderTarget(Initializer.m_kTarget),
  m_nNumInstructions(Initializer.m_nNumInstructions),
  m_nNumInitRefs(0)
{
	m_nId = appCreateGuid();
	m_pkShaderBuilder->RegisterShader(this);
}

void FShader::InitDrv()
{
	if( m_kShaderTarget.Platform != GShaderPlatform )
	{
		appErrorf(TEXT("FShader:: Can't init shader with wrong platform"));
		return;
	}

	if( m_kShaderTarget.Frequency == SF_Vertex )
	{
		m_pkVertexShader = GDynamicDrv->CreateVertexShader(m_kShaderKey.m_arrCode);
	}
	else if( m_kShaderTarget.Frequency == SF_Pixel )
	{
		m_pkPixelShader = GDynamicDrv->CreatePixelShader(m_kShaderKey.m_arrCode);
	}
}

void FShader::ReleaseDrv()
{
	m_pkVertexShader.SafeRelease();
	m_pkPixelShader.SafeRelease();
}
//
//void FShader::AddRef()
//{
//	++NumRefs;
//}
//
//void FShader::Release()
//{
//	if( --NumRefs == 0 )
//	{
//		m_pkShaderBuilder->UnregisterShader(this);
//
//		BeginReleaseResource(this);
//
//		delete this;
//	}
//}

void FShader::BeginInit()
{
	++m_nNumInitRefs;

	if( m_nNumInitRefs == 1 )
	{
		::BeginInitResource(this);
	}
}

void FShader::BeginRelease()
{
	--m_nNumInitRefs;

	check(m_nNumInitRefs >= 0);
	if( m_nNumInitRefs == 0 )
	{
		m_pkShaderBuilder->UnregisterShader(this);

		::BeginReleaseResource(this);

		delete this;
	}
}

const FD3D9VertexShaderPtr& FShader::GetVertexShader()
{
	if( !IsInitialized() )
	{
		BeginInitResource(this);
	}
	return m_pkVertexShader;
}

const FD3D9PixelShaderPtr& FShader::GetPixelShader()
{
	if( !IsInitialized() )
	{
		BeginInitResource(this);
	}
	return m_pkPixelShader;
}

UBOOL FShader::Serialize(FArchive& Ar)
{
	BYTE TargetPlatform = m_kShaderTarget.Platform;
	BYTE TargetFrequency = m_kShaderTarget.Frequency;
	Ar << TargetPlatform << TargetFrequency;
	m_kShaderTarget.Platform = TargetPlatform;
	m_kShaderTarget.Frequency = TargetFrequency;

	UINT NumArray;
	if( Ar.IsLoading() )
	{
		Ar << NumArray;
		m_kShaderKey.m_arrCode.reserve(NumArray);
	}
	else
	{
		NumArray = m_kShaderKey.m_arrCode.size();
		Ar << NumArray;
	}
	Ar.Serialize((void*)&m_kShaderKey.m_arrCode, NumArray);
	Ar << m_kShaderKey.m_nParameterMapCRC;
	Ar << m_nId << m_pkShaderBuilder;
	if( Ar.IsLoading() )
	{
		m_pkShaderBuilder->RegisterShader(this);
	}

	Ar << m_nNumInstructions;

	return TRUE;
}

FArchive& operator<<(FArchive& Ar, FShader*& Ref)
{
	if( Ar.IsLoading() )
	{
		FGuid ShaderId;
		FShaderBuilder* pkBuilder = NULL;

		Ar << ShaderId << pkBuilder;

		Ref = NULL;
		if( pkBuilder )
		{
			Ref = pkBuilder->FindShaderById(ShaderId);
		}
	}
	else
	{
		if( Ref )
		{
			FGuid Id = Ref->GetId();
			FShaderBuilder* pkBuilder = Ref->GetShaderBuilder();
			Ar << Id << pkBuilder;
		}
		else
		{
			FGuid Id(0,0,0,0);
			FShaderBuilder* pkBuilder = NULL;
			Ar << Id << pkBuilder;
		}
	}
	return Ar;
}



FShaderBuilder* FindShaderBuilderByName(const TCHAR* ShaderName)
{
	list<FShaderBuilder*>::iterator it = FShaderBuilder::GetShaderBuilderList()->begin();
	for(; it != FShaderBuilder::GetShaderBuilderList()->end(); ++it)
	{
		if( !appStricmp( (*it)->GetShaderName(), ShaderName ) )
		{
			return *it;
		}
	}

	//map<FString, FShaderBuilder*>::iterator it = FShaderBuilder::GetShaderNameToBuilderMap().find(ShaderName);
	//if( it != FShaderBuilder::GetShaderNameToBuilderMap().end() )
	//{
	//	return it->second;
	//}
	return NULL;
}

void GetShaderIncludes(const TCHAR* Filename, vector<FString>& OutIncludeFilenames, UINT DepthLimit)
{
	FString FileContent = LoadShaderSourceFile(Filename);
	check(FileContent.size() > 0);

	TCHAR* IncludeBegin = appStrstr(FileContent.c_str(), TEXT("#include "));

	UINT SearchCount = 0; 
	const UINT MaxSearchCount = 20;
	while(IncludeBegin != NULL && SearchCount < MaxSearchCount && DepthLimit > 0)
	{
		TCHAR* IncludeFilenameBegin = appStrstr(IncludeBegin, TEXT("\""));
		TCHAR* IncludeFilenameEnd = appStrstr(IncludeFilenameBegin + 1, TEXT("\""));
		FString IncludeFilename = FStringUtil::LeftStr(IncludeFilenameBegin, (INT)(IncludeFilenameEnd - IncludeFilenameBegin + 1));

		if( IncludeFilename == TEXT("Material.vsf"))
		{
			IncludeFilename = TEXT("MaterialTemplate.vsf");
		}

		if( IncludeFilename != TEXT("VertexFactory.vsf") )
		{
			GetShaderIncludes(IncludeFilename.c_str(), OutIncludeFilenames, DepthLimit - 1);
			FString Filename, Extension;
			FStringUtil::SplitBaseFilename(IncludeFilename, Filename, Extension);
			OutIncludeFilenames.push_back(Filename);
		}

		IncludeBegin = appStrstr(IncludeFilenameEnd + 1, TEXT("#include "));
		++SearchCount;
	}
}

map<FString, DWORD>	GShaderCRCCache;

DWORD GetShaderFileCRC(const TCHAR* Filename, DWORD CombineCRC)
{
	map<FString, DWORD>::iterator it;
	it = GShaderCRCCache.find(Filename);
	if( it != GShaderCRCCache.end() )
	{
		return it->second;
	}

	vector<FString> IncludeFilenames;
	GetShaderIncludes(Filename, IncludeFilenames);

	for(UINT IncludeIndex = 0; IncludeIndex < IncludeFilenames.size(); ++IncludeIndex)
	{
		FString IncludeFileContent = LoadShaderSourceFile(IncludeFilenames.at(IncludeIndex).c_str());
		CombineCRC = appMemCrc(IncludeFileContent.c_str(), sizeof(TCHAR) * IncludeFileContent.size(), CombineCRC);
	}

	FString FileContent = LoadShaderSourceFile(Filename);
	CombineCRC = appMemCrc(FileContent.c_str(), sizeof(TCHAR) * FileContent.size(), CombineCRC);

	GShaderCRCCache[Filename] = CombineCRC;

	return CombineCRC;
}