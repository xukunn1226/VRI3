/**	@brief : OutputDevice.h  abstract interface and implementation of output device
 *	@author : xukun
 *	@data : 08.05.01
 */

#ifndef VRI3_CORE_OUTPUTDEVICE_H_
#define VRI3_CORE_OUTPUTDEVICE_H_

class FOutputDevice
{
public:
	virtual ~FOutputDevice() {}

	virtual void Flush() {}
	/** 关闭DEVICE时的清空函数，不能在析构函数中调用，因为静态或全局对象不会“delete” */
	virtual void Shutdown() {}

	virtual void Serialize( const TCHAR* Data, EName Event ) = 0;

	void Log(const TCHAR* log);
	void Log( EName Event, const TCHAR* log );
	void Log(const FString& log);
	void Log( EName Event, const FString& log );

	void Logf( const TCHAR* log, ... );
	void Logf( EName Event, const TCHAR* log, ... );
};

/*
 * LOG device collector 
 */
class FOutputDeviceRedirectorBase : public FOutputDevice
{
public:
	virtual void AddOutputDevice(FOutputDevice* OutputDevice) = 0;

	virtual void RemoveOutputDevice(FOutputDevice* OutputDevice) = 0;

	virtual UBOOL IsRedirectTo(FOutputDevice* OutputDevice) = 0;

	virtual void FlushThreadedLogs() = 0;

	virtual void SetCurrentThreadAsMasterThread() = 0;
};

class FNullOut : public FOutputDevice
{
public:
	void Serialize(const TCHAR* Data, EName Event) {}
};

#endif