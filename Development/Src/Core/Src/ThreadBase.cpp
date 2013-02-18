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
		// ������ͬ�����ƣ�����һ������context switch�����������쳣
		// ��һ��˵����AddQueuedWork & ReturnToPool����һ�̵߳���ʱ��
		// ����ͬʱ��һ�����ݽ���д��������������������ͬ������
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