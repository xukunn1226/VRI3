#ifndef VRI3_CORE_CONFIGSYSTEM_H__
#define VRI3_CORE_CONFIGSYSTEM_H__

enum EConfigType
{
	ECT_DefaultCore,
	ECT_DefaultEngine,
	ECT_Size
};

/*
 * 加载制定配置文件
 */
class FConfigSystem
{
public:
	struct FConfigSection
	{
		FString					_SectionName;
		map<FString, FString>	_MapKeys;
	};

	struct FConfigInfo
	{
		EConfigType				_ConfigType;
		vector<FConfigSection>	_Sections;
	};

public:
	FConfigSystem() {}
	~FConfigSystem() {}

	void Init();

	UBOOL LoadConfig(const TCHAR* ConfigFilename);

	UBOOL GetSectionKeyValue(EConfigType Type, const TCHAR* Section, const TCHAR* Key, FString& Value);
 
private:
	vector<FConfigInfo>		_ConfigInfo;
};

#endif