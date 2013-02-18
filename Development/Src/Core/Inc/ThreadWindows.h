#ifndef THREAD_WINDOWS_H_
#define THREAD_WINDOWS_H_

/**
* Atomically increments the value pointed to and returns that to the caller
*/
FORCEINLINE INT appInterlockedIncrement(volatile INT* Value)
{
	return (INT)InterlockedIncrement((LPLONG)Value);
}
/**
* Atomically decrements the value pointed to and returns that to the caller
*/
FORCEINLINE INT appInterlockedDecrement(volatile INT* Value)
{
	return (INT)InterlockedDecrement((LPLONG)Value);
}
/**
* Atomically adds the amount to the value pointed to and returns the old
* value to the caller
*/
FORCEINLINE INT appInterlockedAdd(volatile INT* Value,INT Amount)
{
	return (INT)InterlockedExchangeAdd((LPLONG)Value,(LONG)Amount);
}
/**
* Atomically swaps two values returning the original value to the caller
*/
FORCEINLINE INT appInterlockedExchange(volatile INT* Value,INT Exchange)
{
	return (INT)InterlockedExchange((LPLONG)Value,(LONG)Exchange);
}
/**
* Atomically compares the value to comperand and replaces with the exchange
* value if they are equal and returns the original value
*/
FORCEINLINE INT appInterlockedCompareExchange(INT* Dest,INT Exchange,INT Comperand)
{
	return (INT)InterlockedCompareExchange((LPLONG)Dest,(LONG)Exchange,(LONG)Comperand);
}
/**
* Atomically compares the pointer to comperand and replaces with the exchange
* pointer if they are equal and returns the original value
*/
FORCEINLINE void* appInterlockedCompareExchangePointer(void** Dest,void* Exchange,void* Comperand)
{
	return InterlockedCompareExchangePointer(Dest,Exchange,Comperand);
}

/**
* Returns a pseudo-handle to the currently executing thread.
*/
FORCEINLINE HANDLE appGetCurrentThread(void)
{
	return GetCurrentThread();
}

/**
* Returns the currently executing thread's id
*/
FORCEINLINE DWORD appGetCurrentThreadId(void)
{
	return GetCurrentThreadId();
}

/**
* Sets the preferred processor for a thread.
*
* @param	ThreadHandle		handle for the thread to set affinity for
* @param	PreferredProcessor	zero-based index of the processor that this thread prefers
*
* @return	the number of the processor previously preferred by the thread, MAXIMUM_PROCESSORS
*			if the thread didn't have a preferred processor, or (DWORD)-1 if the call failed.
*/
FORCEINLINE DWORD appSetThreadIdealProcessor( HANDLE ThreadHandle, DWORD PreferredProcessor )
{
	return SetThreadIdealProcessor(ThreadHandle,PreferredProcessor);
}


class FCriticalSection : public FSynchronize
{
public:
	FORCEINLINE FCriticalSection()
	{
		InitializeCriticalSection(&_CriticalSection);
	}

	FORCEINLINE ~FCriticalSection()
	{
		DeleteCriticalSection(&_CriticalSection);
	}

	FORCEINLINE void Lock()
	{
		if( TryEnterCriticalSection(&_CriticalSection) == 0 )
		{
			EnterCriticalSection(&_CriticalSection);
		}
	}

	FORCEINLINE void Unlock()
	{
		LeaveCriticalSection(&_CriticalSection);
	}

private:
	CRITICAL_SECTION _CriticalSection;
};

class FEventWin : public FEvent
{
public:
	FEventWin();

	virtual ~FEventWin();

	/*
	* 创建EVENT
	* @param bManualReset 是否创建需手动重置的对象
	* @param InName 是否创建一个可被共享的EVENT对象，不可与
	*				 semaphore, mutex, waitable timer, job, 
	*				 or file-mapping同名，否则返回ERROR_INVALID_HANDLE
	* @return 创建成功返回TRUE，否则FALSE
	*/
	virtual UBOOL Create(UBOOL bManualReset = FALSE, const TCHAR* InName = NULL);

	/*
	* 设置EVENT为激发状态（signaled state），任何等待的线程被激活
	*/
	virtual void Trigger();

	/*
	* 设置EVENT为非激发状态（non-signaled state），对于一个已经处于非激发状态的EVENT对象无效
	*/
	virtual void Reset();

	/*
	* 使EVENT对象发生一次脉冲变化，从非激发状态变成激发状态，再变成非激发，整个操作是原子的
	* 若是手动重置型Event，Pulse将激活所有等待的线程，自动重置型Event只激活一个等待的线程
	*/
	virtual void Pulse();

	/*
	* 等待EVENT对象为激发状态
	*/
	virtual UBOOL Wait(DWORD WaitTime = INFINITE);

	/*
	 * 锁定EVENT，即等待EVENT为激发态才返回，否则一直等待
	 */
	virtual void Lock();

	/*
	 * 解锁EVENT，同Pulse
	 */
	virtual void Unlock();

private:
	HANDLE _Event;
};

class FSynchronizeFactoryWin : public FSynchronizeFactory
{
public:
	FSynchronizeFactoryWin();

	virtual FCriticalSection* CreateCriticalSection();

	virtual FEvent* CreateSynchEvent(UBOOL bManualReset = FALSE, const TCHAR* InName = NULL);

	virtual void Destroy(FSynchronize* InSynchObject);
};

/*
 * 线程基本操作的封装，并负责对Runnable object和自身生命周期的管理
 */
class FRunnableThreadWin : public FRunnableThread
{
public:
	FRunnableThreadWin();

	virtual ~FRunnableThreadWin();

	/*
	 * 创建线程，且立即执行
	 *
	 * @param	InRunnable	the runnable object to execute
	 *
	 * @return	线程创建且初始化成功则返回TRUE，否则FALSE
	 */
	UBOOL Create(FRunnable* InRunnable, const TCHAR* ThreadName,
				UBOOL bShouldDeleteSelf = FALSE, UBOOL bShouldDeleteRunnable = FALSE,
				DWORD InStackSize = 0, EThreadPriority InThreadPriority = eThreadPriNormal);

	/*
	* 设置线程优先级
	*/
	virtual void SetThreadPriority(EThreadPriority NewPriority);

	/*
	* 设置优先运行此线程的CPU
	*/
	virtual void SetPreferredProcessor(DWORD ProcessNum);

	/*
	* 设置线程的挂起或执行状态
	*/
	virtual void Suspend(UBOOL bShouldSuspend = TRUE);

	/*
	* 等待直至线程完成
	*/
	virtual void WaitForCompletion();

	/*
	* 终止线程的运行，终止前可等待一定时间以确定线程是否死锁
	* 并调用runnable object的Stop()
	*/
	virtual UBOOL Kill(UBOOL bShouldWait = FALSE, DWORD WaitTime = 0);

	virtual DWORD GetThreadID();

private:
	HANDLE		_Thread;

	FRunnable*	_Runnable;		// 线程执行的对象

	FEvent*		_InitSyncEvent;	// make sure that Init() has been completed before allowing the main thread to continue

	UBOOL		_bShouldDeleteRunnable;

	UBOOL		_bShouldDeleteSelf;

	EThreadPriority	_ThreadPriority;

	DWORD		_ThreadID;

	/*
	 * 线程入口函数
	 */
	static DWORD WINAPI ThreadProc(LPVOID pThis);

	DWORD Run();
};

class FThreadFactoryWin : public FThreadFactory
{
public:
	virtual FRunnableThread* CreateThread(FRunnable* InRunnable, const TCHAR* ThreadName,
		UBOOL bShouldDeleteSelf = FALSE, UBOOL bShouldDeleteRunnable = FALSE,
		DWORD InStackSize = 0, EThreadPriority InThreadPriority = eThreadPriNormal);

	virtual void Destroy(FRunnableThread* InThread);
};


class FQueuedThreadWin : public FQueuedThread
{
public:
	FQueuedThreadWin();

	virtual ~FQueuedThreadWin();

	/*
	* 创建函数
	* 
	* @param	线程所属线程池
	* @param	创建的堆栈大小
	*
	* @return	创建成功返回TRUE，不同于FRunnableThread的Create接口，返回时不保证调用了FRunnable.Init()
	*/
	virtual UBOOL Create(class FQueuedThreadPool* InPool, DWORD InStackSize);

	virtual UBOOL Kill(UBOOL bShouldWait  = FALSE, DWORD WaitTime  = INFINITE, UBOOL bShouldDeleteSelf = FALSE);

	/*
	* 由线程池驱动的线程工作接口，当线程IDLE态时调用
	*/
	virtual void DoWork(FQueuedWork* InQueuedWork);

private:
	FEvent*			_DoWorkEvent;		// 通知线程要开工了，自动重置EVENT

	HANDLE			_ThreadHanle;

	DWORD			_ThreadID;

	FQueuedWork*	_QueuedWork;

	UBOOL			_TimeToExit;

	FCriticalSection*	_SyncWork;

	FQueuedThreadPool*	_OwningThreadPool;

	static DWORD WINAPI ThreadProc(LPVOID pThis);

	void Run();
};

class FQueuedThreadPoolWin : public FQueuedThreadPoolBase
{
public:
	virtual ~FQueuedThreadPoolWin();

	/*
	* 为线程池创建一定数量的线程
	* @param	NumQueuedThread		创建的线程数量
	* @param	InStackSize			线程堆栈大小
	* @return	创建成功返回TRUE，否则FALSE
	*/
	virtual UBOOL Create(DWORD NumQueuedThread, DWORD InStackSize = (32 * 1024));
};






#endif