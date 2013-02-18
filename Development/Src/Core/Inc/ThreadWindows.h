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
	* ����EVENT
	* @param bManualReset �Ƿ񴴽����ֶ����õĶ���
	* @param InName �Ƿ񴴽�һ���ɱ������EVENT���󣬲�����
	*				 semaphore, mutex, waitable timer, job, 
	*				 or file-mappingͬ�������򷵻�ERROR_INVALID_HANDLE
	* @return �����ɹ�����TRUE������FALSE
	*/
	virtual UBOOL Create(UBOOL bManualReset = FALSE, const TCHAR* InName = NULL);

	/*
	* ����EVENTΪ����״̬��signaled state�����κεȴ����̱߳�����
	*/
	virtual void Trigger();

	/*
	* ����EVENTΪ�Ǽ���״̬��non-signaled state��������һ���Ѿ����ڷǼ���״̬��EVENT������Ч
	*/
	virtual void Reset();

	/*
	* ʹEVENT������һ������仯���ӷǼ���״̬��ɼ���״̬���ٱ�ɷǼ���������������ԭ�ӵ�
	* �����ֶ�������Event��Pulse���������еȴ����̣߳��Զ�������Eventֻ����һ���ȴ����߳�
	*/
	virtual void Pulse();

	/*
	* �ȴ�EVENT����Ϊ����״̬
	*/
	virtual UBOOL Wait(DWORD WaitTime = INFINITE);

	/*
	 * ����EVENT�����ȴ�EVENTΪ����̬�ŷ��أ�����һֱ�ȴ�
	 */
	virtual void Lock();

	/*
	 * ����EVENT��ͬPulse
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
 * �̻߳��������ķ�װ���������Runnable object�������������ڵĹ���
 */
class FRunnableThreadWin : public FRunnableThread
{
public:
	FRunnableThreadWin();

	virtual ~FRunnableThreadWin();

	/*
	 * �����̣߳�������ִ��
	 *
	 * @param	InRunnable	the runnable object to execute
	 *
	 * @return	�̴߳����ҳ�ʼ���ɹ��򷵻�TRUE������FALSE
	 */
	UBOOL Create(FRunnable* InRunnable, const TCHAR* ThreadName,
				UBOOL bShouldDeleteSelf = FALSE, UBOOL bShouldDeleteRunnable = FALSE,
				DWORD InStackSize = 0, EThreadPriority InThreadPriority = eThreadPriNormal);

	/*
	* �����߳����ȼ�
	*/
	virtual void SetThreadPriority(EThreadPriority NewPriority);

	/*
	* �����������д��̵߳�CPU
	*/
	virtual void SetPreferredProcessor(DWORD ProcessNum);

	/*
	* �����̵߳Ĺ����ִ��״̬
	*/
	virtual void Suspend(UBOOL bShouldSuspend = TRUE);

	/*
	* �ȴ�ֱ���߳����
	*/
	virtual void WaitForCompletion();

	/*
	* ��ֹ�̵߳����У���ֹǰ�ɵȴ�һ��ʱ����ȷ���߳��Ƿ�����
	* ������runnable object��Stop()
	*/
	virtual UBOOL Kill(UBOOL bShouldWait = FALSE, DWORD WaitTime = 0);

	virtual DWORD GetThreadID();

private:
	HANDLE		_Thread;

	FRunnable*	_Runnable;		// �߳�ִ�еĶ���

	FEvent*		_InitSyncEvent;	// make sure that Init() has been completed before allowing the main thread to continue

	UBOOL		_bShouldDeleteRunnable;

	UBOOL		_bShouldDeleteSelf;

	EThreadPriority	_ThreadPriority;

	DWORD		_ThreadID;

	/*
	 * �߳���ں���
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
	* ��������
	* 
	* @param	�߳������̳߳�
	* @param	�����Ķ�ջ��С
	*
	* @return	�����ɹ�����TRUE����ͬ��FRunnableThread��Create�ӿڣ�����ʱ����֤������FRunnable.Init()
	*/
	virtual UBOOL Create(class FQueuedThreadPool* InPool, DWORD InStackSize);

	virtual UBOOL Kill(UBOOL bShouldWait  = FALSE, DWORD WaitTime  = INFINITE, UBOOL bShouldDeleteSelf = FALSE);

	/*
	* ���̳߳��������̹߳����ӿڣ����߳�IDLE̬ʱ����
	*/
	virtual void DoWork(FQueuedWork* InQueuedWork);

private:
	FEvent*			_DoWorkEvent;		// ֪ͨ�߳�Ҫ�����ˣ��Զ�����EVENT

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
	* Ϊ�̳߳ش���һ���������߳�
	* @param	NumQueuedThread		�������߳�����
	* @param	InStackSize			�̶߳�ջ��С
	* @return	�����ɹ�����TRUE������FALSE
	*/
	virtual UBOOL Create(DWORD NumQueuedThread, DWORD InStackSize = (32 * 1024));
};






#endif