/*******************************************************************
	材质系统数据结构定义
	VS, PS, Platform, ConstantTable
********************************************************************/

#ifndef VRI3_ENGINE_SHADERCOMPILER_H_
#define VRI3_ENGINE_SHADERCOMPILER_H_

enum EShaderFrequency
{
	SF_Vertex	=	0,
	SF_Pixel	=	1,
};

enum EShaderPlatform
{
	SP_SM2			=	0,
	SP_SM3			=	1,
	SP_SM4			=	2,
	SP_NumPlatforms	=	3,
};

struct FShaderCompilerEnvironment
{
	map<FString, FString>	IncludeFiles;			// KEY[Filename] to VALUE[MaterialCode]
	map<FString, FString>	Definitions;			// KEY[Define] to VALUE["1" or "0"]
};

struct FShaderTarget
{
	BYTE	Frequency;			// vertex or pixel
	BYTE	Platform;			// SM2, SM3...
};

extern const TCHAR* ShaderPlatformToText(EShaderPlatform Platform);

class FShaderParameterMap
{
public:
	void AddParameter(const TCHAR* ParameterName, WORD RegisterIndex, WORD RegisterCount, WORD bIsSampler);
	UBOOL FindParameter(const TCHAR* ParameterName, WORD& OutRegisterIndex, WORD& OutRegisterCount, WORD& bIsSampler) const;

	DWORD GetCRC() const
	{
		DWORD ParameterCRC = 0;
		for(map<FString, FParameterAllocation>::const_iterator it = ParameterTable.begin(); it != ParameterTable.end(); ++it)
		{
			const FString& strParameterName = it->first;
			const FParameterAllocation& kParameterValue = it->second;
			ParameterCRC = appMemCrc(strParameterName.c_str(), strParameterName.size() * sizeof(TCHAR), ParameterCRC);
			ParameterCRC = appMemCrc(&kParameterValue, sizeof(FParameterAllocation), ParameterCRC);
		}
		return ParameterCRC;
	}

private:
	struct FParameterAllocation
	{
		WORD	RegisterIndex;				// Zero-based index of the constant in the table
		WORD	RegisterCount;				// Number of registers that contain data
		WORD	bIsSampler;					// true, indicate the parameter is a sampler
	};
	map<FString, FParameterAllocation>		ParameterTable;		// constant name to parameter struct
};

struct FShaderCompilerOutput
{
	FShaderCompilerOutput()
		: NumInstructions(0)
	{}

	FShaderTarget			Target;
	vector<BYTE>			ShaderCode;
	FShaderParameterMap		ParameterMap;
	UINT					NumInstructions;
};

extern EShaderPlatform	GShaderPlatform;

FString LoadShaderSourceFile(const TCHAR* Filename);


extern UBOOL CompileShader(
	const TCHAR* ShaderFilename,
	const TCHAR* FunctionName,
	FShaderTarget Target,
	const FShaderCompilerEnvironment& InEnvironment,
	FShaderCompilerOutput& OutData,
	UBOOL bDebugDump = FALSE
	);

// load shader source file in directory \Engine\Shaders
void CheckShaderSourceFiles();

/**
 * Pads a shader parameter value to the an integer number of shader registers
 * 
 * @param	Value	A pointer to the shader parameter value
 * @param	NumBytes	The number of bytes in the shader parameter value
 * @return	A pointer to the padded shader parameter value
 */
FORCEINLINE const void* GetPaddedShaderParameterValue(const void* Value, UINT NumBytes)
{
	const UINT NumPaddedBytes = Align(NumBytes, ShaderArrayElementAlignBytes);
	if( NumPaddedBytes != NumBytes )
	{
		static vector<BYTE> PaddedShaderParameterValueBuffer;
		if( PaddedShaderParameterValueBuffer.size() < NumPaddedBytes )
		{
			PaddedShaderParameterValueBuffer.clear();
			PaddedShaderParameterValueBuffer.reserve(NumPaddedBytes);
		}

		appMemcpy(&PaddedShaderParameterValueBuffer.at(0), Value, NumBytes);
		appMemzero(&PaddedShaderParameterValueBuffer.at(NumBytes), NumPaddedBytes - NumBytes);

		return &PaddedShaderParameterValueBuffer.at(0);
	}
	else
	{
		return Value;
	}
}

#endif