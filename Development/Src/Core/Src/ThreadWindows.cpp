#include "CorePCH.h"
#include "ThreadWindows.h"

FSynchronizeFactory*	GSynchronizeFactory = NULL;
FThreadFactory*			GThreadFactory		= NULL;
FQueuedThreadPool*		GThreadPool			= NULL;

/*
 *	http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
 */
#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName( DWORD dwThreadID, char* threadName)
{
	Sleep(10);
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}


FEventWin::FEventWin()
{
	_Event = NULL;
}

FEventWin::~FEventWin()
{
	if( _Event != NULL )
	{
		CloseHandle(_Event);
	}
}

UBOOL FEventWin::Create(UBOOL bManualReset, const TCHAR *InName)
{
	// 默认创建为非激发态
	_Event = CreateEvent(NULL, bManualReset, 0, InName);
	return _Event != NULL;
}

void FEventWin::Trigger()
{
	check(_Event);
	SetEvent(_Event);
}

void FEventWin::Reset()
{
	check(_Event);
	ResetEvent(_Event);
}

void FEventWin::Pulse()
{
	check(_Event);
	PulseEvent(_Event);
}

UBOOL FEventWin::Wait(DWORD WaitTime)
{
	check(_Event);
	return WaitForSingleObject(_Event, WaitTime) == WAIT_OBJECT_0;
}

void FEventWin::Lock()
{
	WaitForSingleObject(_Event, INFINITE);
}

void FEventWin::Unlock()
{
	PulseEvent(_Event);
}


FSynchronizeFactoryWin::FSynchronizeFactoryWin()
{
}

FCriticalSection* FSynchronizeFactoryWin::CreateCriticalSection()
{
	return new FCriticalSection();
}

FEvent* FSynchronizeFactoryWin::CreateSynchEvent(UBOOL bManualReset, const TCHAR* InName)
{
	FEvent* Event = new FEventWin();
	if( Event->Create(bManualReset, InName) == FALSE )
	{
		delete Event;
		Event = NULL;
	}
	return Event;
}

void FSynchronizeFactoryWin::Destroy(FSynchronize* InSynchObject)
{
	delete InSynchObject;
}


FRunnableThreadWin::FRunnableThreadWin()
: _Thread(NULL), _Runnable(NULL), _InitSyncEvent(NULL),
  _bShouldDeleteRunnable(FALSE), _bShouldDeleteSelf(FALSE),
  _ThreadID(0)
{
}

FRunnableThreadWin::~FRunnableThreadWin()
{
	if(_Thread != NULL)
	{
		Kill(TRUE);
	}
}

UBOOL FRunnableThreadWin::Create(FRunnable *InRunnable, 
								const TCHAR *ThreadName, 
								UBOOL bShouldDeleteSelf, 
								UBOOL bShouldDeleteRunnable,
								DWORD InStackSize,
								EThreadPriority InThreadPriority)
{
	check(InRunnable);
	_Runnable = InRunnable;
	_bShouldDeleteSelf = bShouldDeleteSelf;
	_bShouldDeleteRunnable = bShouldDeleteRunnable;
	_ThreadPriority = InThreadPriority;

	// 确保返回之前，Runnable已经Init()，默认非激发态
	_InitSyncEvent = GSynchronizeFactory->CreateSynchEvent(TRUE);

	_Thread = CreateThread(NULL, InStackSize, ThreadProc, this, 0, &_ThreadID);
	if( _Thread == NULL )
	{
		if( _bShouldDeleteRunnable )
		{
			delete _Runnable;
		}
		_Runnable = NULL;
	}
	else
	{
		// 等待Runnable的初始化完成
		_InitSyncEvent->Wait(INFINITE);
		SetThreadName(_ThreadID, ThreadName ? TCHAR_TO_ANSI(ThreadName) : "Unnamed ThreadName");
	}

	GSynchronizeFactory->Destroy(_InitSyncEvent);
	_InitSyncEvent = NULL;
	return _Thread != NULL;
}

void FRunnableThreadWin::SetThreadPriority(EThreadPriority NewPriority)
{
	if( NewPriority != _ThreadPriority )
	{
		NewPriority = _ThreadPriority;
		::SetThreadPriority(_Thread,
			_ThreadPriority == eThreadPriNormal ? THREAD_PRIORITY_NORMAL :
			_ThreadPriority == eThreadPriAboveNormal ? THREAD_PRIORITY_ABOVE_NORMAL :
			THREAD_PRIORITY_BELOW_NORMAL);
	}
}

void FRunnableThreadWin::SetPreferredProcessor(DWORD ProcessNum)
{
	check(_Thread);
	appSetThreadIdealProcessor(_Thread, ProcessNum);
}

void FRunnableThreadWin::Suspend(UBOOL bShouldSuspend)
{
	check(_Thread);
	if( bShouldSuspend )
	{
		SuspendThread(_Thread);
	}
	else
	{
		ResumeThread(_Thread);
	}
}

void FRunnableThreadWin::WaitForCompletion()
{
	check(_Thread);
	WaitForSingleObject(_Thread, INFINITE);
}

UBOOL FRunnableThreadWin::Kill(UBOOL bShouldWait, DWORD WaitTime)
{
	check(_Runnable);
	UBOOL bExitOK = TRUE;
	if( _Runnable )
	{
		_Runnable->Stop();
	}

	if( bShouldWait )
	{
		if( WaitForSingleObject(_Thread, WaitTime) == WAIT_TIMEOUT )
		{
			TerminateThread(_Thread, -1);
			bExitOK = FALSE;
		}
	}
	CloseHandle(_Thread);
	_Thread = NULL;

	if( _Runnable && _bShouldDeleteRunnable )
	{
		delete _Runnable;
		_Runnable = NULL;
	}
	if( _bShouldDeleteSelf )
	{
		GThreadFactory->Destroy(this);
	}
	return bExitOK;
}

DWORD FRunnableThreadWin::GetThreadID()
{
	return _ThreadID;
}

DWORD WINAPI FRunnableThreadWin::ThreadProc(LPVOID pThis)
{
	check(pThis);
	return ((FRunnableThreadWin*)pThis)->Run();
}

DWORD FRunnableThreadWin::Run()
{
	check(_Runnable);
	DWORD ExitCode = 1;
	if( _Runnable->Init() == TRUE )
	{
		_InitSyncEvent->Trigger();		// 激发Event，使Create返回
		ExitCode = _Runnable->Run();
		_Runnable->Exit();
	}
	else
	{
		_InitSyncEvent->Trigger();
	}

	if( _bShouldDeleteRunnable )
	{
		delete _Runnable;
		_Runnable = NULL;
	}
	if( _bShouldDeleteSelf )
	{
		CloseHandle(_Thread);
		_Thread = NULL;
		GThreadFactory->Destroy(this);
	}
	return ExitCode;
}


FRunnableThread* FThreadFactoryWin::CreateThread(FRunnable* InRunnable, const TCHAR* ThreadName,
									  UBOOL bShouldDeleteSelf, UBOOL bShouldDeleteRunnable,
									  DWORD InStackSize, EThreadPriority InThreadPriority)
{
	FRunnableThreadWin* NewThread = new FRunnableThreadWin();
	if( NewThread )
	{
		if( NewThread->Create(InRunnable, ThreadName, bShouldDeleteSelf, 
							  bShouldDeleteRunnable, InStackSize, InThreadPriority) == FALSE )
		{
			Destroy(NewThread);
			NewThread = NULL;
		}
	}
	return NewThread;
}

void FThreadFactoryWin::Destroy(FRunnableThread* InThread)
{
	delete InThread;
}


FQueuedThreadWin::FQueuedThreadWin()
: _DoWorkEvent(NULL), 
  _ThreadHanle(NULL),
  _ThreadID(0), _QueuedWork(NULL),
  _TimeToExit(FALSE), 
  _SyncWork(NULL),
  _OwningThreadPool(NULL)
{
}

FQueuedThreadWin::~FQueuedThreadWin()
{
	if( _ThreadHanle != NULL )
	{
		Kill(TRUE);
	}
}

UBOOL FQueuedThreadWin::Create(FQueuedThreadPool *InPool, DWORD InStackSize)
{
	check(_OwningThreadPool == NULL && _ThreadHanle == NULL);

	_OwningThreadPool = InPool;
	_DoWorkEvent = GSynchronizeFactory->CreateSynchEvent();
	_SyncWork = GSynchronizeFactory->CreateCriticalSection();

	if( _DoWorkEvent != NULL && _SyncWork != NULL )
	{
		_ThreadHanle = CreateThread(NULL, InStackSize, ThreadProc, this, 0, &_ThreadID);
	}
	if( _ThreadHanle == NULL )
	{
		_OwningThreadPool = NULL;

		if( _DoWorkEvent != NULL )
		{
			GSynchronizeFactory->Destroy(_DoWorkEvent);
		}
		_DoWorkEvent = NULL;

		if( _SyncWork != NULL )
		{
			GSynchronizeFactory->Destroy(_SyncWork);
		}
		_SyncWork = NULL;
	}
	else
	{
		Sleep(1);
		SetThreadName(_ThreadID, "PoolThread");
	}
	return _ThreadHanle != NULL;
}

UBOOL FQueuedThreadWin::Kill(UBOOL bShouldWait, DWORD WaitTime, UBOOL bShouldDeleteSelf)
{
	UBOOL bExitOK = TRUE;
	InterlockedExchange((LONG*)&_TimeToExit, TRUE);

	// 唤醒等待中的线程,否则_DoWorkEvent->Wait()一直等待下去
	_DoWorkEvent->Trigger();

	if( bShouldWait == TRUE )
	{
		if( WaitForSingleObject(_ThreadHanle, WaitTime) == WAIT_TIMEOUT )
		{
			TerminateThread(_ThreadHanle, -1);
			bExitOK = FALSE;
		}
	}
	CloseHandle(_ThreadHanle);
	_ThreadHanle = NULL;

	GSynchronizeFactory->Destroy(_DoWorkEvent);
	_DoWorkEvent = NULL;
	GSynchronizeFactory->Destroy(_SyncWork);
	_SyncWork = NULL;
	_TimeToExit = FALSE;
	if( bShouldDeleteSelf )
	{
		delete this;
	}
	return bExitOK;
}

// 当线程处于空闲状态，调用此接口激活线程
void FQueuedThreadWin::DoWork(FQueuedWork *InQueuedWork)
{
	{
		// 与接口Run()中对_QueuedWork的操作类似，避免context switch发生，
		// 而造成逻辑异常，故必须为_QueuedWork操作设定同步控制
		FScopeLock Lock(_SyncWork);
		// check检查在同步控制之后，此时才能确保_QueuedWork==NULL
		check(_QueuedWork == NULL && "不能同时执行多个任务");
		_QueuedWork = InQueuedWork;
	}
	_DoWorkEvent->Trigger();	// 自动重置型EVENT，激活等待中的线程，然后重置为非激发态
}

DWORD WINAPI FQueuedThreadWin::ThreadProc(LPVOID pThis)
{
	((FQueuedThreadWin*)pThis)->Run();
	return 0;
}

void FQueuedThreadWin::Run()
{
	while( _TimeToExit == FALSE )
	{
		_DoWorkEvent->Wait();
		{
			FScopeLock Lock(_SyncWork);
			if( _QueuedWork != NULL )
			{
				_QueuedWork->DoWork();
				_QueuedWork->Dispose();
				_QueuedWork = NULL;
			}
		}

		// context switch原故，当线程被要求释放时不能返回至线程池
		if( _TimeToExit == FALSE )
		{
			_OwningThreadPool->ReturnToPool(this);
		}
	}
}

FQueuedThreadPoolWin::~FQueuedThreadPoolWin()
{
	if( _QueuedThread.size() > 0 )
	{
		Destroy();
	}
}

UBOOL FQueuedThreadPoolWin::Create(DWORD NumQueuedThread, DWORD InStackSize)
{
	UBOOL bSuccessful = CreateSyncObject();
	if( bSuccessful )
	{
		FScopeLock Lock(_SyncWork);
		for(DWORD i = 0; i < NumQueuedThread && bSuccessful; ++i)
		{
			FQueuedThread* Thread = new FQueuedThreadWin();
			if( Thread->Create(this, InStackSize) )
			{
				_QueuedThread.push_back(Thread);
			}
			else
			{
				bSuccessful = FALSE;
				delete Thread;
			}
		}
	}
	if( bSuccessful == FALSE )
	{
		Destroy();
	}
	return bSuccessful;
}