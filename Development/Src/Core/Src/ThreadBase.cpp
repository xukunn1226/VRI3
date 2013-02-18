#include "CorePCH.h"

FQueuedThreadPoolBase::~FQueuedThreadPoolBase()
{
	if( _SyncWork != NULL )
	{
		GSynchronizeFactory->Destroy(_SyncWork);
	}
}

UBOOL FQueuedThreadPoolBase::CreateSyncObject()
{
	check(_SyncWork == NULL);
	_SyncWork = GSynchronizeFactory->CreateCriticalSection();
	return _SyncWork != NULL;
}

void FQueuedThreadPoolBase::Destroy()
{
	{
		// 必须有同步控制，否则一旦发生context switch将导致数据异常
		// 进一步说明，AddQueuedWork & ReturnToPool当另一线程调用时，
		// 可能同时对一份数据进行写操作，类似情况均必须加同步控制
		FScopeLock Lock(_SyncWork);
		for(UINT i = 0; i < _QueuedWork.size(); ++i)
		{
			_QueuedWork.at(i)->Abandon();
		}
		_QueuedWork.clear();
	}
	{
		FScopeLock Lock(_SyncWork);
		for(UINT i = 0; i < _QueuedThread.size(); ++i)
		{
			_QueuedThread.at(i)->Kill(TRUE, INFINITE, TRUE);
		}
		_QueuedThread.clear();
	}
}

void FQueuedThreadPoolBase::AddQueuedWork(FQueuedWork* InQueuedWork)
{
	check(InQueuedWork != NULL);
	check(_SyncWork != NULL);

	FQueuedThread* Thread = NULL;
	FScopeLock Lock(_SyncWork);
	if( _QueuedThread.size() > 0 )
	{
		Thread = _QueuedThread.back();
		_QueuedThread.pop_back();		
	}
	if( Thread != NULL )
	{
		Thread->DoWork(InQueuedWork);
	}
	else
	{
		_QueuedWork.push_back(InQueuedWork);
	}
}

void FQueuedThreadPoolBase::ReturnToPool(FQueuedThread* InQueuedThread)
{
	check(InQueuedThread != NULL);

	FQueuedWork* Work = NULL;
	FScopeLock Lock(_SyncWork);
	if( _QueuedWork.size() > 0 )
	{
		Work = _QueuedWork.front();
		_QueuedWork.erase(_QueuedWork.begin());
	}
	if( Work != NULL )
	{
		InQueuedThread->DoWork(Work);
	}
	else
	{
		_QueuedThread.push_back(InQueuedThread);
	}
}