#ifndef THREAD_BASE_H_
#define THREAD_BASE_H_

/*
 *	同步对象基类，如CriticalSection，Event
 */
class FSynchronize
{
public:
	virtual ~FSynchronize()
	{
	}
};

class FEvent : public FSynchronize
{
public:
	/*
	 * 创建EVENT
	 * @param bManualReset 是否创建需手动重置的对象
	 * @param InName 是否创建一个可被共享的EVENT对象，不可与
	 *				 semaphore, mutex, waitable timer, job, 
	 *				 or file-mapping同名，否则返回ERROR_INVALID_HANDLE
	 * @return 创建成功返回TRUE，否则FALSE
	 */
	virtual UBOOL Create(UBOOL bManualReset = FALSE, const TCHAR* InName = NULL) = 0;

	/*
	* 设置EVENT为激发状态（signaled state），任何等待的线程被激活
	*/
	virtual void Trigger() = 0;

	/*
	* 设置EVENT为非激发状态（non-signaled state），对于一个已经处于非激发状态的EVENT对象无效
	*/
	virtual void Reset() = 0;

	/*
	* 使EVENT对象发生一次脉冲变化，从非激发状态变成激发状态，再变成非激发，整个操作是原子的
	* 若是手动重置型Event，Pulse将激活所有等待的线程，自动重置型Event只激活一个等待的线程
	*/
	virtual void Pulse() = 0;

	/*
	* 等待EVENT对象为激发状态
	*/
	virtual UBOOL Wait(DWORD WaitTime = INFINITE) = 0;
};

class FSynchronizeFactory
{
public:
	virtual class FCriticalSection* CreateCriticalSection() = 0;

	virtual FEvent* CreateSynchEvent(UBOOL bManualReset = FALSE, const TCHAR* InName = NULL) = 0;

	virtual void Destroy(FSynchronize* InSynchObject) = 0;
};

extern FSynchronizeFactory* GSynchronizeFactory;

/*
 * 任何由线程驱动的对象的基类.使用模式为Init(), Run(), Exit()
 */
class FRunnable
{
public:
	virtual ~FRunnable() {}

	/*
	 * 初始化函数，若失败不会执行Run(),并返回一错误码
	 */
	virtual UBOOL Init() = 0;

	/*
	 * @return	the exit code of runnable object
	 */
	virtual DWORD Run() = 0;

	/*
	 * 线程结束时调用
	 */
	virtual void Exit() = 0;

	/*
	* 当线程提前结束时被调用
	*/
	virtual void Stop() = 0;
};

enum EThreadPriority
{
	eThreadPriNormal,
	eThreadPriAboveNormal,
	eThreadPriBelowNormal,
};

/*
 * 对线程操作的封装，创建
 */
class FRunnableThread
{
public:
	virtual ~FRunnableThread() {}

	/*
	 * 设置线程优先级
	 */
	virtual void SetThreadPriority(EThreadPriority NewPriority) = 0;

	/*
	 * 设置优先运行此线程的CPU
	 */
	virtual void SetPreferredProcessor(DWORD ProcessNum) = 0;

	/*
	 * 设置线程的挂起或执行状态
	 */
	virtual void Suspend(UBOOL bShouldSuspend = TRUE) = 0;

	/*
	 * 等待直至线程完成
	 */
	virtual void WaitForCompletion() = 0;

	/*
	 * 终止线程的运行，终止前可等待一定时间以确定线程是否死锁
	 * 并调用runnable object的Stop()
	 */
	virtual UBOOL Kill(UBOOL bShouldWait = FALSE, DWORD WaitTime = INFINITE) = 0;

	virtual DWORD GetThreadID() = 0;
};

class FThreadFactory
{
public:
	virtual FRunnableThread* CreateThread(FRunnable* InRunnable, const TCHAR* ThreadName,
										  UBOOL bShouldDeleteSelf = FALSE, UBOOL bShouldDeleteRunnable = FALSE,
										  DWORD InStackSize = 0, EThreadPriority InThreadPriority = eThreadPriNormal) = 0;

	virtual void Destroy(FRunnableThread* InThread) = 0;
};

extern FThreadFactory* GThreadFactory;


/*
 *	This interface is a type of runnable object
 */
class FQueuedWork
{
public:
	virtual ~FQueuedWork() {}

	virtual void DoWork() = 0;

	/**
	* Tells the queued work that it is being abandoned so that it can do
	* per object clean up as needed. This will only be called if it is being
	* abandoned before completion. NOTE: This requires the object to delete
	* itself using whatever heap it was allocated in.
	*/
	virtual void Abandon() = 0;

	/**
	* This method is also used to tell the object to cleanup but not before
	* the object has finished it's work.
	*/ 
	virtual void Dispose() = 0;
};

/*
 * This is the interface used for all poolable threads.
 */
class FQueuedThread
{
public:
	virtual ~FQueuedThread() {}

	/*
	 * 创建函数
	 * 
	 * @param	InPool	线程所属线程池
	 * @param	InStackSize	创建的堆栈大小
	 *
	 * @return	创建成功返回TRUE
	 */
	virtual UBOOL Create(class FQueuedThreadPool* InPool, DWORD InStackSize = 0) = 0;

	virtual UBOOL Kill(UBOOL bShouldWait = FALSE, DWORD WaitTime = INFINITE, UBOOL bShouldDeleteSelf = FALSE) = 0;

	/*
	 * 由线程池驱动的线程工作接口，当线程IDLE态时调用
	 */
	virtual void DoWork(FQueuedWork* InQueuedWork) = 0;
};


/*
 * 线程池基类
 */
class FQueuedThreadPool
{
public:
	virtual ~FQueuedThreadPool() {}

	/*
	 * 为线程池创建一定数量的线程
	 * @param	NumQueuedThread		创建的线程数量
	 * @param	InStackSize			线程堆栈大小
	 * @return	创建成功返回TRUE，否则FALSE
	 */
	virtual UBOOL Create(DWORD NumQueuedThread, DWORD InStackSize = (32 * 1024)) = 0;

	virtual void Destroy() = 0;

	/*
	 * 把一个任务交给线程池，若有空闲线程则立即执行，否则等待
	 */
	virtual void AddQueuedWork(FQueuedWork* InQueuedWork) = 0;

	/*
	 * 当线程将任务执行完毕，即刻返回线程池等待下一个任务
	 */
	virtual void ReturnToPool(FQueuedThread* InQueuedThread) = 0;
};

extern FQueuedThreadPool* GThreadPool;

class FQueuedThreadPoolBase : public FQueuedThreadPool
{
public:
	virtual ~FQueuedThreadPoolBase();

	UBOOL CreateSyncObject();

	virtual void Destroy();

	/*
	* 把一个任务交给线程池，若有空闲线程则立即执行，否则等待
	*/
	virtual void AddQueuedWork(FQueuedWork* InQueuedWork);

	/*
	* 当线程将任务执行完毕，即刻返回线程池等待下一个任务
	*/
	virtual void ReturnToPool(FQueuedThread* InQueuedThread);

protected:
	FQueuedThreadPoolBase()
	{
		_SyncWork = NULL;
	}

	FCriticalSection*		_SyncWork;

	vector<FQueuedWork*>	_QueuedWork;

	vector<FQueuedThread*>	_QueuedThread;
};


#include "ThreadWindows.h"

class FScopeLock
{
public:
	FScopeLock(FCriticalSection* InSyncObject)
		: _SyncObject(InSyncObject)
	{
		check(_SyncObject);
		_SyncObject->Lock();
	}

	~FScopeLock()
	{
		check(_SyncObject);
		_SyncObject->Unlock();
	}

private:
	FCriticalSection*	_SyncObject;

	// disable these interface
	FScopeLock();
	FScopeLock(FScopeLock* InScopeLock);
	FScopeLock& operator=(const FScopeLock&);
};


#endif