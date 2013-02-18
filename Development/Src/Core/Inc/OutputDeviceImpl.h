#ifndef VRI3_CORE_OUTPUTDEVICEIMPL_H_
#define VRI3_CORE_OUTPUTDEVICEIMPL_H_

const TCHAR* const COLOR_BLACK			= TEXT("0000");

const TCHAR* const COLOR_DARK_RED		= TEXT("1000");
const TCHAR* const COLOR_DARK_GREEN		= TEXT("0100");
const TCHAR* const COLOR_DARK_BLUE		= TEXT("0010");
const TCHAR* const COLOR_DARK_YELLOW	= TEXT("1100");
const TCHAR* const COLOR_DARK_CYAN		= TEXT("0110");
const TCHAR* const COLOR_DARK_PURPLE	= TEXT("1010");
const TCHAR* const COLOR_DARK_WHITE		= TEXT("1110");
const TCHAR* const COLOR_GRAY			= COLOR_DARK_WHITE;

const TCHAR* const COLOR_RED			= TEXT("1001");
const TCHAR* const COLOR_GREEN			= TEXT("0101");
const TCHAR* const COLOR_BLUE			= TEXT("0011");
const TCHAR* const COLOR_YELLOW			= TEXT("1101");
const TCHAR* const COLOR_CYAN			= TEXT("0111");
const TCHAR* const COLOR_PURPLE			= TEXT("1011");
const TCHAR* const COLOR_WHITE			= TEXT("1111");

const TCHAR* const COLOR_NONE			= TEXT("");


class FOutputDeviceDebug : public FOutputDevice
{
public:
	virtual void Serialize( const TCHAR* Data, EName Event);
};

class FOutputDeviceFile : public FOutputDevice
{
public:
	FOutputDeviceFile( const TCHAR* InFilename = NULL, UBOOL bDisableBackup = FALSE );

	void Shutdown();
	void Flush();

	void Serialize( const TCHAR* Data, EName Event );

private:
	FArchive*	_LogAr;
	TCHAR		_Filename[1024];
	UBOOL		_bOpened;
	UBOOL		_bDisableBackup;
};

class FOutputDeviceRedirector : public FOutputDeviceRedirectorBase
{
public:
	FOutputDeviceRedirector();

	virtual void AddOutputDevice(FOutputDevice* OutputDevice);

	virtual void RemoveOutputDevice(FOutputDevice* OutputDevice);

	virtual UBOOL IsRedirectTo(FOutputDevice* OutputDevice);

	virtual void Serialize(const TCHAR* Data, EName Event);

	virtual void Flush();

	virtual void Shutdown();

	virtual void FlushThreadedLogs();

	virtual void SetCurrentThreadAsMasterThread();

private:
	void UnsyncFlushThreadedLogs();

private:
	DWORD		_MasterThreadID;

	struct FBufferedLine
	{
		FString		Data;
		EName		Event;

		FBufferedLine(const TCHAR* InData, EName InEvent)
			: Data(InData), Event(InEvent)
		{}
	};
	vector<FBufferedLine>	_BufferedLines;

	vector<FOutputDevice*>	_OutputDevices;

	FCriticalSection		_SyncObject;
};

class FOutputDeviceConsole : public FOutputDevice
{
public:
	FOutputDeviceConsole();
	~FOutputDeviceConsole();

	virtual void Show(UBOOL bShow);

	virtual UBOOL IsShow();

	virtual void Serialize(const TCHAR* Data, EName Event);

private:
	HANDLE ConsoleHandle;
};

#endif