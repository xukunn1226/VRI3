#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__

class FShaderParameter
{
public:
	FShaderParameter(): m_nRegisterCount(0) {}
	void Bind(const FShaderParameterMap& ParameterMap, const TCHAR* ParameterName)
	{
		if( !ParameterMap.FindParameter(ParameterName, m_nRegisterIndex, m_nRegisterCount, m_bIsSampler) )
		{
			appErrorf(TEXT("Failed to bind shader parameter %s"), ParameterName);
		}
	}
	friend FArchive& operator<< (FArchive& Ar, FShaderParameter& Param)
	{
		return Ar << Param.m_nRegisterIndex << Param.m_nRegisterCount << Param.m_bIsSampler;
	}
	UBOOL IsBound() const { return m_nRegisterCount > 0; }
	WORD GetBufferIndex() const { return m_nRegisterIndex; }
	WORD GetBufferCount() const { return m_nRegisterCount; }
	UBOOL IsSampler() const { return m_bIsSampler > 0; }

private:
	WORD	m_nRegisterIndex;
	WORD	m_nRegisterCount;			// if the shader parameter is "sampler", m_nRegisterCount denotes samples count
										// otherwise denotes number of bytes
	WORD	m_bIsSampler;				// true meaning the shader parameter is "sampler"
};

// set a vertex shader constant; e.g float4, float4[NUM]
template<class ParameterType>
void SetVertexShaderValue(
	const FShaderParameter& Parameter,
	const ParameterType& Value,
	UINT ElementIndex = 0)
{
	const UINT AlignedTypeSize = Align(sizeof(ParameterType), ShaderArrayElementAlignBytes);
	const UINT NumBytesToSet = Min<INT>(sizeof(ParameterType), Parameter.GetBufferCount() - ElementIndex * AlignedTypeSize);
	if( NumBytesToSet > 0 )
	{
		GDynamicDrv->SetVertexShaderParameter(
			NULL, 
			Parameter.GetBufferIndex() + ElementIndex * AlignedTypeSize, 
			NumBytesToSet, 
			&Value);
	}
}

extern void SetVertexShaderBool(
	const FShaderParameter& Parameter,
	UBOOL Value);

// set a pixel shader constant; e.g float4, float4[NUM]
template<class ParameterType>
void SetPixelShaderValue(
	const FShaderParameter& Parameter,
	const ParameterType& Value,
	UINT ElementIndex = 0)
{
	const UINT AlignedTypeSize = Align(sizeof(ParameterType), ShaderArrayElementAlignBytes);
	const UINT NumBytesToSet = Min<INT>(sizeof(ParameterType), Parameter.GetBufferCount() - ElementIndex * AlignedTypeSize);
	if( NumBytesToSet > 0 )
	{
		GDynamicDrv->SetPixelShaderParameter(
			NULL, 
			Parameter.GetBufferIndex() + ElementIndex * AlignedTypeSize, 
			NumBytesToSet, 
			&Value);
	}
}

extern void SetPixelShaderBool(
	const FShaderParameter& Parameter,
	UBOOL Value);

template<class ParameterType>
void SetVertexShaderValues(
	const FShaderParameter& Parameter,
	const ParameterType* Value,
	UINT NumElements,
	UINT BaseElementIndex = 0)
{
	const UINT AlignedTypeSize = Align(sizeof(ParameterType), ShaderArrayElementAlignBytes);
	const UINT NumBytesToSet = Min<INT>(NumElements * sizeof(ParameterType), Parameter.GetBufferCount() - BaseElementIndex * AlignedTypeSize);
	if( NumBytesToSet > 0 )
	{
		GDynamicDrv->SetVertexShaderParameter(
			NULL,
			Parameter.GetBufferIndex() + BaseElementIndex * AlignedTypeSize,
			NumBytesToSet,
			Value);
	}
}

template<class ParameterType>
void SetPixelShaderValues(
	const FShaderParameter& Parameter,
	const ParameterType* Value,
	UINT NumElements,
	UINT BaseElementIndex = 0)
{
	const UINT AlignedTypeSize = Align(sizeof(ParameterType), ShaderArrayElementAlignBytes);
	const UINT NumBytesToSet = Min<INT>(NumElements * sizeof(ParameterType), Parameter.GetBufferCount() - BaseElementIndex * AlignedTypeSize);
	if( NumBytesToSet > 0 )
	{
		GDynamicDrv->SetPixelShaderParameter(
			NULL,
			Parameter.GetBufferIndex() + BaseElementIndex * AlignedTypeSize,
			NumBytesToSet,
			Value);
	}
}

FORCEINLINE void SetTextureParameter(
	const FShaderParameter& Parameter,
	const FTexture* Texture,
	UINT ElementIndex = 0)
{
	if( Parameter.IsBound() && Parameter.IsSampler() )
	{
		check(ElementIndex < Parameter.GetBufferCount() );
		GDynamicDrv->SetSamplerState(
			Parameter.GetBufferIndex() + ElementIndex,
			Texture->_SamplerState,
			Texture->_Texture,
			0);
	}
}

FORCEINLINE void SetTextureParameter(
	const FShaderParameter& Parameter,
	const FD3D9SamplerState& SamplerState,
	const FD3D9TexturePtr Texture,
	UINT ElementIndex = 0)
{
	if( Parameter.IsBound() && Parameter.IsSampler() )
	{
		check(ElementIndex < Parameter.GetBufferCount());
		GDynamicDrv->SetSamplerState(
			Parameter.GetBufferIndex() + ElementIndex,
			SamplerState,
			Texture,
			0);
	}
}


class FShaderKey
{
public:
	FShaderKey()
		: m_nParameterMapCRC(0)
	{}

	FShaderKey(const vector<BYTE>& InCode, const FShaderParameterMap& InParameterMap)
		: m_arrCode(InCode)
	{
		m_nParameterMapCRC = InParameterMap.GetCRC();
		m_nKeyHash = appMemCrc(&m_arrCode.at(0), m_arrCode.size()) ^ m_nParameterMapCRC;
	}

	bool operator <(const FShaderKey& Ref) const
	{
		check( m_nKeyHash != 0 && Ref.m_nKeyHash != 0 );

		if( m_nKeyHash < Ref.m_nKeyHash )
		{
			return TRUE;
		}
		return FALSE;
	}
	vector<BYTE>	m_arrCode;
	DWORD			m_nParameterMapCRC;
	DWORD			m_nKeyHash;
};

class FShader : public FRenderResource/*, public FRefCountedObject*/
{
	friend class FShaderBuilder;
public:
	struct CompileShaderInitializer
	{
		class FShaderBuilder*			m_pkBuilder;
		FShaderTarget					m_kTarget;				// frequency and platform
		const vector<BYTE>&				m_arrCode;
		const FShaderParameterMap&		m_kParameterMap;
		UINT							m_nNumInstructions;

		CompileShaderInitializer(
			FShaderBuilder* InBuilder, 
			const FShaderCompilerOutput& CompilerOutput
			): 
			m_pkBuilder(InBuilder), 
			m_kTarget(CompilerOutput.Target), 
			m_arrCode(CompilerOutput.ShaderCode),
			m_kParameterMap(CompilerOutput.ParameterMap), 
			m_nNumInstructions(CompilerOutput.NumInstructions)
		{}
	};

	FShader()
		: m_pkShaderBuilder(NULL), m_nNumInitRefs(0)
	{}

	FShader(const CompileShaderInitializer& Initializer);
	virtual ~FShader() {}

	static void ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment) {}

	// FRenderResource interface
	virtual void InitDrv();
	virtual void ReleaseDrv();

	// reference count
	//void AddRef();
	//void Release();

	void BeginInit();
	void BeginRelease();

	virtual UBOOL Serialize(FArchive& Ar);

	const FD3D9VertexShaderPtr& GetVertexShader();
	const FD3D9PixelShaderPtr& GetPixelShader();

	const FGuid& GetId() const { return m_nId; }
	FShaderBuilder* GetShaderBuilder() const { return m_pkShaderBuilder; }
	const vector<BYTE>& GetShaderCode() const { return m_kShaderKey.m_arrCode; }
	const FShaderKey& GetShaderKey() const { return m_kShaderKey; }
	UINT GetNumInstructions() const { return m_nNumInstructions; }

	friend FArchive& operator<< (FArchive& Ar, FShader*& Shader);

private:
	FShaderBuilder*			m_pkShaderBuilder;
	FShaderKey				m_kShaderKey;

	FShaderTarget			m_kShaderTarget;
	FD3D9VertexShaderPtr	m_pkVertexShader;
	FD3D9PixelShaderPtr		m_pkPixelShader;

	FGuid					m_nId;
	UINT					m_nNumInstructions;

	UINT					m_nNumInitRefs;			// necessary?
};
//typedef TRefCountPtr<FShader>	FShaderPtr;


/*
 *	@brief		shader文件（vs or ps）的生成器
 *				相同类型的shader均由同类型的FShaderBuilder生成
 */
class FShaderBuilder
{
public:
	typedef class FShader* (*ConstructSerialized)();
	typedef void (*ModifyCompilationEnvironment)(EShaderPlatform, FShaderCompilerEnvironment&);

	static list<FShaderBuilder*>*& GetShaderBuilderList();

	static map<FString, FShaderBuilder*>& GetShaderNameToBuilderMap();

	/* Minimal initialization constructor */
	FShaderBuilder(
		const TCHAR* InShaderName,
		const TCHAR* InSourceFilename,
		const TCHAR* InFunctionName,
		DWORD InFrequency,
		ConstructSerialized InConstructSerializedRef,
		ModifyCompilationEnvironment InModifyCompilationEnvironmentRef
		)
		: m_sShaderName(InShaderName),
		  m_sSourceFilename(InSourceFilename),
		  m_sFunctionName(InFunctionName),
		  m_nFrequency(InFrequency),
		  m_pfnConstructSerializedRef(InConstructSerializedRef),
		  m_pfnModifyCompilationEnvironmentRef(InModifyCompilationEnvironmentRef)
	{
		list<FShaderBuilder*>* BuilderList = GetShaderBuilderList();
		BuilderList->push_back(this);

		GetShaderNameToBuilderMap().insert(pair<FString, FShaderBuilder*>(FString(InShaderName), this));
	}

	void RegisterShader(FShader* InShader);

	void UnregisterShader(FShader* InShader);

	FShader* FindShaderById(const FGuid& Id) const;

	FShader* FindShaderByOutput(const FShaderCompilerOutput& Output) const;

	FShader* ConstructSerialization() const
	{
		return (*m_pfnConstructSerializedRef)();
	}

	DWORD GetFrequency() const
	{
		return m_nFrequency;
	}

	const TCHAR* GetShaderName() const
	{
		return m_sShaderName;
	}

	const TCHAR* GetShaderFilename() const
	{
		return m_sSourceFilename;
	}

	UINT GetNumShaders() const
	{
		return m_mapShaderId.size();
	}

	virtual DWORD GetSourceCRC();

	friend FArchive& operator<< (FArchive& Ar, FShaderBuilder*& Builder);

	virtual class FGlobalShaderBuilder* GetGlobalShaderBuilder() { return NULL; }
	virtual class FMaterialShaderBuilder* GetMaterialShaderBuilder() { return NULL; }
	virtual class FMeshMaterialShaderBuilder* GetMeshMaterialShaderBuilder() { return NULL; }

protected:
	UBOOL CompileShader(EShaderPlatform Platform, const FShaderCompilerEnvironment& InEnvironment, FShaderCompilerOutput& Output);

private:
	const TCHAR*			m_sShaderName;
	const TCHAR*			m_sSourceFilename;
	const TCHAR*			m_sFunctionName;
	DWORD					m_nFrequency;

	ConstructSerialized				m_pfnConstructSerializedRef;
	ModifyCompilationEnvironment	m_pfnModifyCompilationEnvironmentRef;

	map<FGuid, FShader*>			m_mapShaderId;
	map<FShaderKey, FShader*>		m_mapShaderCode;
};

#define DECLARE_SHADER_BUILDER(ShaderClass, ShaderMetaShortcut) \
	public: \
	typedef F##ShaderMetaShortcut##ShaderBuilder		ShaderMetaBuilder; \
	static ShaderMetaBuilder	StaticBuilder; \
	static FShader* ConstructSerializedInstance() { return new ShaderClass(); } \
	static FShader* ConstructCompiledInstance(const ShaderMetaBuilder::CompileShaderInitializer& Initializer) \
	{ return new ShaderClass(Initializer); }

#define IMPLEMENT_SHADER_BUILDER(ShaderClass, SourceFilename, FunctionName, Frequency) \
	ShaderClass::ShaderMetaBuilder ShaderClass::StaticBuilder( \
		TEXT(#ShaderClass), \
		SourceFilename, \
		FunctionName, \
		Frequency, \
		ShaderClass::ConstructSerializedInstance, \
		ShaderClass::ConstructCompiledInstance, \
		ShaderClass::ModifyCompilationEnvironment);


/*
 *	@brief	由相同类型的ShaderBuilder生成的所有shader集合
 */
template<class ShaderMetaBuilder>
class TShaderMap
{
public:
	TShaderMap()
		: m_nResourceInitCount(0)
	{}

	FShader* GetShader(FShaderBuilder* pkBuilder) const
	{
		map<FShaderBuilder*, FShader*>::const_iterator it;
		it = m_mapShaders.find(pkBuilder);
		if( it != m_mapShaders.end() )
		{
			return it->second;
		}
		return NULL;
	}

	UBOOL HasShader(ShaderMetaBuilder* pkBuilder) const
	{
		map<FShaderBuilder*, FShader*>::const_iterator it;
		it = m_mapShaders.find(pkBuilder);
		return (it != m_mapShaders.end()) && (it->second != NULL);
	}

	void Serialize(FArchive& Ar)
	{
		if( Ar.IsLoading() )
		{
			UINT NumShaders = 0;
			Ar << NumShaders;
		}
		else
		{
			UINT NumShaders = GetShaderNum();
			Ar << NumShaders;

			for(map<FShaderBuilder*, FShader*>::const_iterator it = m_mapShaders.begin();
				it != m_mapShaders.end(); ++it)
			{
			}
		}
	}

	void AddShader(ShaderMetaBuilder* pkBuilder, FShader* pkShader)
	{
		m_mapShaders[pkBuilder] = pkShader;
		for(UINT ResourceInitCount = 0; ResourceInitCount < m_nResourceInitCount; ++ResourceInitCount)
		{
			pkShader->BeginInit();
		}
	}

	void Merge(const TShaderMap<ShaderMetaBuilder>* OtherShaderMap)
	{
		check(OtherShaderMap);
		map<FGuid, FShader*> OtherShaders;
		OtherShaderMap->GetShaderList(OtherShaders);
		for(map<FGuid, FShader*>::iterator it = OtherShaders.begin(); it != OtherShaders.end(); ++it)
		{
			FShader* CurrentShader = it->second;
			check(CurrentShader);
			ShaderMetaBuilder* CurrentShaderBuilder = (ShaderMetaBuilder*)CurrentShader->GetShaderBuilder();
			if( !HasShader(CurrentShaderBuilder) )
			{
				AddShader(CurrentShaderBuilder, CurrentShader);
			}
		}
	}

	void RemoveShader(ShaderMetaBuilder* pkBuilder)
	{
		m_mapShaders.erase(pkBuilder);
	}

	void GetShaderList(map<FGuid, FShader*>& OutShaders) const
	{
		for(map<FShaderBuilder*, FShader*>::const_iterator it = m_mapShaders.begin(); it != m_mapShaders.end(); ++it)
		{
			if( it->second )
			{
				OutShaders[it->second->GetId()] = it->second;
			}
		}
	}

	void BeginInit()
	{
		for(map<FShaderBuilder*, FShader*>::iterator it = m_mapShader.begin(); it != m_mapShaders.end(); ++it)
		{
			if( it->second )
			{
				it->second->BeginInit();
			}
		}
		++m_nResourceInitCount;
	}

	void BeginRelease()
	{
		for(map<FShaderBuilder*, FShader*>::iterator it = m_mapShader.begin(); it != m_mapShaders.end(); ++it)
		{
			if( it->second )
			{
				it->second->BeginRelease();
			}
		}
		--m_nResourceInitCount;
		check(m_nResourceInitCount >= 0);
	}

	UBOOL IsEmpty() const
	{
		return m_mapShaders.size() == 0 ? TRUE : FALSE;
	}

	UINT GetShaderNum() const
	{
		return m_mapShaders.size();
	}
private:
	map<FShaderBuilder*, FShader*>		m_mapShaders;
	INT									m_nResourceInitCount;
};


extern FShaderBuilder* FindShaderBuilderByName(const TCHAR* ShaderName);

extern void GetShaderIncludes(const TCHAR* Filename, vector<FString>& OutIncludeFilenames, UINT DepthLimit = 7);

extern DWORD GetShaderFileCRC(const TCHAR* Filename, DWORD CombineCRC = 0);

#endif