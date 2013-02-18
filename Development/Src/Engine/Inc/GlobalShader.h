#ifndef __GLOBALSHADER_H__
#define __GLOBALSHADER_H__

#include "ShaderManager.h"

class FGlobalShaderBuilder : public FShaderBuilder
{
public:
	typedef FShader::CompileShaderInitializer	CompileShaderInitializer;
	typedef FShader* (*ConstructCompiledType)(const CompileShaderInitializer&);

	FGlobalShaderBuilder(
		const TCHAR* InShaderName,
		const TCHAR* InSourceFilename,
		const TCHAR* InFunctionName,
		DWORD InFrequency,
		ConstructSerialized InConstructSerializedRef,
		ConstructCompiledType InConstructCompiledRef,
		ModifyCompilationEnvironment InModifyCompilationEnvironment
		):
		FShaderBuilder(InShaderName, InSourceFilename, InFunctionName, InFrequency, InConstructSerializedRef, InModifyCompilationEnvironment),
		m_pfnConstructCompiledRef(InConstructCompiledRef)
	{}

	FShader* CompileShader(EShaderPlatform Platform);

	virtual FGlobalShaderBuilder* GetGlobalShaderBuilder() { return this; }

private:
	ConstructCompiledType	m_pfnConstructCompiledRef;
};


class FGlobalShader : public FShader
{
	DECLARE_SHADER_BUILDER(FGlobalShader, Global);
public:
	FGlobalShader() : FShader() {}

	FGlobalShader(const ShaderMetaBuilder::CompileShaderInitializer& Initializer)
		: FShader(Initializer)
	{}
};

class FOneColorVertexShader : public FGlobalShader
{
	DECLARE_SHADER_BUILDER(FGlobalShader, Global);
public:
	FOneColorVertexShader() : FGlobalShader() {}

	FOneColorVertexShader(const ShaderMetaBuilder::CompileShaderInitializer& Initializer)
		: FGlobalShader(Initializer)
	{}
};

class FOneColorPixelShader : public FGlobalShader
{
	DECLARE_SHADER_BUILDER(FGlobalShader, Global);
public:
	FOneColorPixelShader() : FGlobalShader() {}

	FOneColorPixelShader(const ShaderMetaBuilder::CompileShaderInitializer& Initializer)
		: FGlobalShader(Initializer)
	{
		m_kColorParameter.Bind(Initializer.m_kParameterMap, TEXT("DrawColor"));
	}

	virtual UBOOL Serialize(FArchive& Ar)
	{
		Ar << m_kColorParameter;
		return TRUE;
	}

	FShaderParameter	m_kColorParameter;
};

/** serialize the global shader map */
extern void SerializeGlobalShaders(EShaderPlatform Platform, FArchive& Ar);

extern void VerifyGlobalShaders(EShaderPlatform Platform = GShaderPlatform);

extern TShaderMap<FGlobalShaderBuilder>* GetGlobalShaderMap(EShaderPlatform Platform = GShaderPlatform);





#endif