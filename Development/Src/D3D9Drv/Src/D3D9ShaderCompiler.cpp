#include "D3D9DrvPCH.h"

class FD3DIncludeEnvironment : public ID3DXInclude
{
public:
	STDMETHOD(Open)(D3DXINCLUDE_TYPE Type, LPCSTR Name, LPCVOID ParentData, LPCVOID* Data, UINT* NumBytes)
	{
		FString Filename(ANSI_TO_TCHAR(Name));
		FString ShaderCode;

		map<FString, FString>::iterator it = _Environment.IncludeFiles.find(Filename);
		if( it != _Environment.IncludeFiles.end() )
		{
			ShaderCode = it->second;
		}
		else
		{
			ShaderCode = LoadShaderSourceFile(Filename.c_str());
		}

		ANSICHAR* Code = new ANSICHAR[ShaderCode.length() + 1];
		appStrncpyANSI(Code, TCHAR_TO_ANSI(ShaderCode.c_str()), ShaderCode.length() + 1);

		*Data = (LPCVOID)Code;
		*NumBytes = ShaderCode.length();

		return D3D_OK;
	}

	STDMETHOD(Close)(LPCVOID Data)
	{
		delete Data;
		return D3D_OK;
	}

	FD3DIncludeEnvironment(const FShaderCompilerEnvironment& InEnvironment)
		: _Environment(InEnvironment)
	{
	}

private:
	FShaderCompilerEnvironment	_Environment;
};

/** Preprocesses a shader without performing compilation, and dump it out for debugging*/
void D3D9PreProcessShader(
	const TCHAR* strFilename,
	const FString& strShaderFile,
	vector<D3DXMACRO>& Defines,
	const FD3DIncludeEnvironment& Environment,
	const TCHAR* strShaderPath
	)
{
	TRefCountPtr<ID3DXBuffer>	ShaderCode;
	TRefCountPtr<ID3DXBuffer>	ErrorText;

	FTCHARToANSI AnsiShaderFile(strShaderFile.c_str());
	FD3DIncludeEnvironment IncludeEnvironment(Environment);
	HRESULT ret = D3DXPreprocessShader( (ANSICHAR*)AnsiShaderFile, 
										strShaderFile.size(), 
										&Defines.at(0), 
										&IncludeEnvironment,
										ShaderCode.GetInitReference(),
										ErrorText.GetInitReference()
										);

	if( FAILED(ret) )
	{
		debugf(NAME_Warning, TEXT("Preprocess failed for shader %s: %s"), strFilename, ANSI_TO_TCHAR(ErrorText->GetBufferPointer()));
	}
	else
	{
		TCHAR Tmp[MAX_SPRINTF];
		appSprintf(Tmp, TEXT("%s%s.pre"), strShaderPath, strFilename);
		appSaveStringToFile(ANSI_TO_TCHAR(ShaderCode->GetBufferPointer()), Tmp);
	}
}

struct FD3D9ConstantDesc
{
	enum { MaxNameLength = 256 };

	char	Name[MaxNameLength];
	UBOOL	bIsSampler;
	UINT	RegisterIndex;
	UINT	RegisterCount;
};

static UBOOL D3D9CompileShaderWrapper(
	const TCHAR* ShaderFilename,
	const TCHAR* FunctionName,
	const TCHAR* ShaderProfile,
	DWORD CompileFlags,
	const FShaderCompilerEnvironment& InEnvironment,
	const vector<D3DXMACRO>& Macros,
	FShaderCompilerOutput& Output,
	vector<FD3D9ConstantDesc>& Constants,
	FString& DisassemblyString,
	FString& ErrorString
	)
{
	const FString& SourceFile = LoadShaderSourceFile(ShaderFilename);

	TRefCountPtr<ID3DXBuffer> Shader;
	TRefCountPtr<ID3DXBuffer> Errors;
	FD3DIncludeEnvironment Environment(InEnvironment);
	TRefCountPtr<ID3DXConstantTable> ConstantTable;
	FTCHARToANSI AnsiSourceFile(SourceFile.c_str());
	HRESULT Result = D3DXCompileShader(
						AnsiSourceFile, 
						SourceFile.size(), 
						&Macros[0], 
						&Environment, 
						TCHAR_TO_ANSI(FunctionName), 
						TCHAR_TO_ANSI(ShaderProfile),
						CompileFlags, 
						Shader.GetInitReference(),
						Errors.GetInitReference(), 
						ConstantTable.GetInitReference());

	if( FAILED(Result) )
	{
		ErrorString = TEXT("Compile Failed without warnings!");
		void* ErrorBuffer = Errors ? Errors->GetBufferPointer() : NULL;
		if( ErrorBuffer )
		{
			FANSIToTCHAR Convert((ANSICHAR*)ErrorBuffer);
			ErrorString = Convert;
		}
	}
	else
	{
		// get the ShaderCode
		INT NumShaderBytes = Shader->GetBufferSize();
		Output.ShaderCode.resize(NumShaderBytes);
		appMemcpy(&Output.ShaderCode[0], Shader->GetBufferPointer(), NumShaderBytes);

		// collect the constant table
		D3DXCONSTANTTABLE_DESC ConstantTableDesc;
		VERIFYD3DRESULT(ConstantTable->GetDesc(&ConstantTableDesc));
		for(UINT ConstantIndex = 0; ConstantIndex < ConstantTableDesc.Constants; ++ConstantIndex)
		{
			D3DXHANDLE ConstantHandle = ConstantTable->GetConstant(NULL, ConstantIndex);

			D3DXCONSTANT_DESC ConstantDesc;
			UINT NumConstants = 1;
			VERIFYD3DRESULT(ConstantTable->GetConstantDesc(ConstantHandle, &ConstantDesc, &NumConstants));

			FD3D9ConstantDesc NamedConstantDesc;
			appStrcpyANSI(NamedConstantDesc.Name, ConstantDesc.Name);
			NamedConstantDesc.bIsSampler = ConstantDesc.RegisterSet == D3DXRS_SAMPLER;
			NamedConstantDesc.RegisterIndex = ConstantDesc.RegisterIndex;
			NamedConstantDesc.RegisterCount = ConstantDesc.RegisterCount;
			Constants.push_back(NamedConstantDesc);
		}

		// disassemble a shader
		TRefCountPtr<ID3DXBuffer> DisassemblyBuffer;
		VERIFYD3DRESULT(D3DXDisassembleShader((const DWORD*)Shader->GetBufferPointer(), FALSE, NULL, DisassemblyBuffer.GetInitReference()));
		DisassemblyString = ANSI_TO_TCHAR((ANSICHAR*)DisassemblyBuffer->GetBufferPointer());
	}

	return TRUE;
}

// vps编译接口
UBOOL D3D9CompileShader(
	const TCHAR* ShaderFilename,
	const TCHAR* FunctionName,
	FShaderTarget Target,
	const FShaderCompilerEnvironment& Environment,
	FShaderCompilerOutput& Output,
	UBOOL bDebugDump = FALSE
	)
{
	// translate the input Environment's defines to D3DXMACROs
	vector<D3DXMACRO> Macros;
	for( map<FString, FString>::const_iterator it = Environment.Definitions.begin(); it != Environment.Definitions.end(); ++it )
	{
		FString Name = it->first;
		FString Definition = it->second;

		D3DXMACRO Macro;
		Macro.Name = new ANSICHAR[Name.size()+1];
		appStrncpyANSI((ANSICHAR*)Macro.Name, TCHAR_TO_ANSI(Name.c_str()), Name.size()+1);
		Macro.Definition = new ANSICHAR[Definition.size()+1];
		appStrncpyANSI((ANSICHAR*)Macro.Definition, TCHAR_TO_ANSI(Definition.c_str()), Definition.size()+1);

		Macros.push_back(Macro);
	}

	DWORD CompileFlags = D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY;
	const TCHAR* ShaderPath = appShaderDir();

	TCHAR Profile[32];
	if( Target.Platform == SP_SM3 )
	{
		if( Target.Frequency == SF_Pixel )
		{
			appStrcpy(Profile, TEXT("ps_3_0"));
		}
		else if( Target.Frequency == SF_Vertex )
		{
			appStrcpy(Profile, TEXT("vs_3_0"));
		}
	}
	else if( Target.Platform == SP_SM2 )
	{
		if( Target.Frequency == SF_Pixel )
		{
			appStrcpy(Profile, TEXT("ps_2_0"));
		}
		else if( Target.Frequency == SF_Vertex )
		{
			appStrcpy(Profile, TEXT("vs_2_0"));
		}
	}

	vector<FD3D9ConstantDesc> Constants;
	FString DisassemblyString;
	FString ErrorString;
	UBOOL bCompileFailed = TRUE;
	bCompileFailed = !D3D9CompileShaderWrapper(
		ShaderFilename,
		FunctionName,
		Profile,
		CompileFlags,
		Environment,
		Macros,
		Output,
		Constants,
		DisassemblyString,
		ErrorString
		);

	if( bCompileFailed )
	{
		FString ErrorShaderFile = FStringUtil::Sprintf(TEXT("%sErrorShader-%s.vps"), ShaderPath, ShaderFilename);
		appSaveStringToFile(ErrorString, ErrorShaderFile.c_str());
	}

	if( bCompileFailed || bDebugDump )
	{
		const FString SourceFile = LoadShaderSourceFile(ShaderFilename);
		D3D9PreProcessShader(ShaderFilename, SourceFile, Macros, Environment, ShaderPath);
	}

	for(UINT i = 0; i < Macros.size(); ++i)
	{
		delete Macros[i].Name;
		delete Macros[i].Definition;
	}
	Macros.clear();

	UINT NumInstructions = 0;
	if( !bCompileFailed )
	{
		// fetch the number of instruction
		Parse(DisassemblyString.c_str(), TEXT("// approximately"), (DWORD&)NumInstructions);

		// 在ShaderModel3.0下支持的指令数，最少为512，最多至D3DCAPS9.MaxVertexShader30InstructionSlots
		// 因指令数在运行时刻检查，所以在编译器给出提示
		if( NumInstructions > 512 && Target.Platform == SP_SM3 )
		{
			bCompileFailed = TRUE;
			debugf(NAME_Warning, TEXT("Shader use too many instructions for Shader Model 3.0 (Used %u, Minimum guaranteed in SM3.0 is 512)"), NumInstructions);
		}
	}

	if( bCompileFailed )
	{
		return FALSE;
	}

	check(Output.ShaderCode.size() > 0);

	for(UINT ConstantIndex = 0; ConstantIndex < Constants.size(); ++ConstantIndex)
	{
		const FD3D9ConstantDesc& Desc = Constants.at(ConstantIndex);
		if( Desc.bIsSampler )
		{
			Output.ParameterMap.AddParameter(
				ANSI_TO_TCHAR(Desc.Name),
				Desc.RegisterIndex,
				Desc.RegisterCount,
				1
				);
		}
		else
		{
			Output.ParameterMap.AddParameter(
				ANSI_TO_TCHAR(Desc.Name),
				Desc.RegisterIndex * sizeof(FLOAT) * 4,
				Desc.RegisterCount * sizeof(FLOAT) * 4,
				0
				);
		}
	}

	Output.Target = Target;
	Output.NumInstructions = NumInstructions;

	if( bDebugDump )
	{
		FString ASMShaderFile = FStringUtil::Sprintf(TEXT("%s%s.asm"), ShaderPath, ShaderFilename);

		appSaveStringToFile(DisassemblyString, ASMShaderFile.c_str());
	}

	return TRUE;
}