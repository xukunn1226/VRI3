#ifndef THREAD_BASE_H_
#define THREAD_BASE_H_

/*
 *	ͬ��������࣬��CriticalSection��Event
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
	 * ����EVENT
	 * @param bManualReset �Ƿ񴴽����ֶ����õĶ���
	 * @param InName �Ƿ񴴽�һ���ɱ������EVENT���󣬲�����
	 *				 semaphore, mutex, waitable timer, job, 
	 *				 or file-mappingͬ�������򷵻�ERROR_INVALID_HANDLE
	 * @return �����ɹ�����TRUE������FALSE
	 */
	virtual UBOOL Create(UBOOL bManualReset = FALSE, const TCHAR* InName = NULL) = 0;

	/*
	* ����EVENTΪ����״̬��signaled state�����κεȴ����̱߳�����
	*/
	virtual void Trigger() = 0;

	/*
	* ����EVENTΪ�Ǽ���״̬��non-signaled state��������һ���Ѿ����ڷǼ���״̬��EVENT������Ч
	*/
	virtual void Reset() = 0;

	/*
	* ʹEVENT������һ������仯���ӷǼ���״̬��ɼ���״̬���ٱ�ɷǼ���������������ԭ�ӵ�
	* �����ֶ�������Event��Pulse���������еȴ����̣߳��Զ�������Eventֻ����һ���ȴ����߳�
	*/
	virtual void Pulse() = 0;

	/*
	* �ȴ�EVENT����Ϊ����״̬
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
 * �κ����߳������Ķ���Ļ���.ʹ��ģʽΪInit(), Run(), Exit()
 */
class FRunnable
{
public:
	virtual ~FRunnable() {}

	/*
	 * ��ʼ����������ʧ�ܲ���ִ��Run(),������һ������
	 */
	virtual UBOOL Init() = 0;

	/*
	 * @return	the exit code of runnable object
	 */
	virtual DWORD Run() = 0;

	/*
	 * �߳̽���ʱ����
	 */
	virtual void Exit() = 0;

	/*
	* ���߳���ǰ����ʱ������
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
 * ���̲߳����ķ�װ������
 */
class FRunnableThread
{
public:
	virtual ~FRunnableThread() {}

	/*
	 * �����߳����ȼ�
	 */
	virtual void SetThreadPriority(EThreadPriority NewPriority) = 0;

	/*
	 * �����������д��̵߳�CPU
	 */
	virtual void SetPreferredProcessor(DWORD ProcessNum) = 0;

	/*
	 * �����̵߳Ĺ����ִ��״̬
	 */
	virtual void Suspend(UBOOL bShouldSuspend = TRUE) = 0;

	/*
	 * �ȴ�ֱ���߳����
	 */
	virtual void WaitForCompletion() = 0;

	/*
	 * ��ֹ�̵߳����У���ֹǰ�ɵȴ�һ��ʱ����ȷ���߳��Ƿ�����
	 * ������runnable object��Stop()
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
	 * ��������
	 * 
	 * @param	InPool	�߳������̳߳�
	 * @param	InStackSize	�����Ķ�ջ��С
	 *
	 * @return	�����ɹ�����TRUE
	 */
	virtual UBOOL Create(class FQueuedThreadPool* InPool, DWORD InStackSize = 0) = 0;

	virtual UBOOL Kill(UBOOL bShouldWait = FALSE, DWORD WaitTime = INFINITE, UBOOL bShouldDeleteSelf = FALSE) = 0;

	/*
	 * ���̳߳��������̹߳����ӿڣ����߳�IDLE̬ʱ����
	 */
	virtual void DoWork(FQueuedWork* InQueuedWork) = 0;
};


/*
 * �̳߳ػ���
 */
class FQueuedThreadPool
{
public:
	virtual ~FQueuedThreadPool() {}

	/*
	 * Ϊ�̳߳ش���һ���������߳�
	 * @param	NumQueuedThread		�������߳�����
	 * @param	InStackSize			�̶߳�ջ��С
	 * @return	�����ɹ�����TRUE������FALSE
	 */
	virtual UBOOL Create(DWORD NumQueuedThread, DWORD InStackSize = (32 * 1024)) = 0;

	virtual void Destroy() = 0;

	/*
	 * ��һ�����񽻸��̳߳أ����п����߳�������ִ�У�����ȴ�
	 */
	virtual void AddQueuedWork(FQueuedWork* InQueuedWork) = 0;

	/*
	 * ���߳̽�����ִ����ϣ����̷����̳߳صȴ���һ������
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
	* ��һ�����񽻸��̳߳أ����п����߳�������ִ�У�����ȴ�
	*/
	virtual void AddQueuedWork(FQueuedWork* InQueuedWork);

	/*
	* ���߳̽�����ִ����ϣ����̷����̳߳صȴ���һ������
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