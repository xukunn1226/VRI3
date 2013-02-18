#include "EnginePCH.h"

map<FString, FString>	GShaderFileCache;		//	Filename to FileContent

const TCHAR* ShaderPlatformToText(EShaderPlatform Platform)
{
	switch(Platform)
	{
	case SP_SM2:
		return TEXT("SM2");
	case SP_SM3:
		return TEXT("SM3");
	case SP_SM4:
		return TEXT("SM4");
	default:
		return TEXT("Unknown");
	}
}

FString LoadShaderSourceFile(const TCHAR* Filename)
{
	FString strFileContent;

	FString strShaderFilename = FStringUtil::Sprintf(TEXT("%s%s%s"), appBaseDir(), appShaderDir(), Filename);

	if( !FStringUtil::EndWith( strShaderFilename, FString(TEXT(".vsf")) ) )
	{
		strShaderFilename += TEXT(".vsf");
	}

	map<FString, FString>::iterator it = GShaderFileCache.find(strShaderFilename.c_str());
	if( it == GShaderFileCache.end() )
	{
		appLoadFileToString(strFileContent, strShaderFilename.c_str());

		GShaderFileCache[strShaderFilename] = strFileContent;
	}
	else
	{
		strFileContent = it->second;
	}

	return strFileContent;
}

void FShaderParameterMap::AddParameter(const TCHAR* ParameterName, WORD RegisterIndex, WORD RegisterCount, WORD bIsSampler)
{
	FParameterAllocation Alloc;
	Alloc.RegisterIndex = RegisterIndex;
	Alloc.RegisterCount = RegisterCount;
	Alloc.bIsSampler	= bIsSampler;
	ParameterTable[ParameterName] = Alloc;
}

UBOOL FShaderParameterMap::FindParameter(const TCHAR* ParameterName, WORD& OutRegisterIndex, WORD& OutRegisterCount, WORD& OutbIsSampler) const
{
	map<FString, FShaderParameterMap::FParameterAllocation>::const_iterator it = ParameterTable.find(ParameterName);
	if( it == ParameterTable.end() )
	{
		return FALSE;
	}
	else
	{
		OutRegisterIndex = it->second.RegisterIndex;
		OutRegisterCount = it->second.RegisterCount;
		OutbIsSampler	 = it->second.bIsSampler;
		return TRUE;
	}
}


extern UBOOL D3D9CompileShader(
	const TCHAR* ShaderFilename,
	const TCHAR* FunctionName,
	FShaderTarget Target,
	const FShaderCompilerEnvironment& Environment,
	FShaderCompilerOutput& OutData,
	UBOOL bDebugDump = FALSE
	);


UBOOL CompileShader(
	const TCHAR* ShaderFilename,
	const TCHAR* FunctionName,
	FShaderTarget Target,
	const FShaderCompilerEnvironment& InEnvironment,
	FShaderCompilerOutput& OutData,
	UBOOL bDebugDump
	)
{
	UBOOL bResult = FALSE;
	FShaderCompilerEnvironment Environment(InEnvironment);
	Environment.Definitions[TEXT("VERTEXSHADER")] = (Target.Frequency == SF_Vertex ? TEXT("1") : TEXT("0"));
	Environment.Definitions[TEXT("PIXELSHADER")]  = (Target.Frequency == SF_Pixel ? TEXT("1") : TEXT("0"));

	if( Target.Platform == SP_SM3 || Target.Platform == SP_SM2 )
	{
		bResult = D3D9CompileShader(ShaderFilename, FunctionName, Target, Environment, OutData, bDebugDump);
	}

	return bResult;
}

void CheckShaderSourceFiles()
{
}