#include "LaunchPCH.h"


static FOutputDeviceFile			Log;
static FOutputDeviceConsole			LogConsole;
static FFileManagerWindows			FileManager;
static FSynchronizeFactoryWin		SynchronizeFactory;
static FThreadFactoryWin			ThreadFactory;
static FQueuedThreadPoolWin			ThreadPool;

extern "C" {HINSTANCE hInstance;}
INT		GGameIcon = 131/*IDI_ICON_GameDemo*/;

void appUpdateTime()
{
	//static DOUBLE LastTime = appSeconds();
	//static UBOOL bIsUsingFixedTimeStep = FALSE;

	//const UBOOL bUseFixedTimeStep = GIsBenchmarking;

	//if( bUseFixedTimeStep )
	//{
	//	bIsUsingFixedTimeStep = TRUE;
	//	GDeltaTime = GFixedDeltaTime;
	//	LastTime = GCurrentTime;
	//	GCurrentTime += GDeltaTime;
	//}
	//else
	{
		GCurrentTime = appSeconds();

		const FLOAT DeltaTime = GCurrentTime - GLastTime;

		FLOAT MaxTickRate = 60.f;
		FLOAT WaitTime = 0.0f;
		WaitTime = Max<FLOAT>(1.0 / MaxTickRate - DeltaTime, 0.0f);
		if( WaitTime > 0.0f )
		{
			DOUBLE WaitStartTime = GCurrentTime;
			while( GCurrentTime - WaitStartTime < WaitTime )
			{
				GCurrentTime = appSeconds();
				appSleep(0);
			}
		}

		GDeltaTime = GCurrentTime - GLastTime;
		GLastTime = GCurrentTime;
	}
}

INT FEngineLoop::PreInit(const TCHAR *strCmdLine)
{
	GGameThreadId = appGetCurrentThreadId();

	// initialize global variable about thread
	GSynchronizeFactory = &SynchronizeFactory;
	GThreadFactory = &ThreadFactory;
	GThreadPool = &ThreadPool;
	INT NumThreadsInThreadPool = 1;
	SYSTEM_INFO SI;
	GetSystemInfo(&SI);
	NumThreadsInThreadPool = Max<INT>(1, SI.dwNumberOfProcessors - 1);
	GThreadPool->Create(NumThreadsInThreadPool);

	appInit(strCmdLine, &Log, &LogConsole, &FileManager);

	// initialize rhi
	DrvInit();

	if( GIsRequestingExit )
	{
		if( GEngine != NULL )
		{
			GEngine->PreExit();
		}
		appPreExit();
		return 1;
	}

	GIsBenchmarking = ParseParam(strCmdLine, TEXT("BENCHMARK"));

	// register window class, init window produce
	appPlatformPostInit();

	return 0;
}

INT FEngineLoop::Init()
{
	// todo: verify all shader source

	// todo: load the global shaders


	// init engine
	FName GameEngineClassName(TEXT("VEngine"));
	GEngine = ExactCast<VEngine>(VObject::StaticConstructorObject(GameEngineClassName));
	check(GEngine);

	debugf(TEXT("Initializing Engine..."));
	GEngine->Init();
	debugf(TEXT("Initializing Engine Completed..."));

	// init variable for time
	GCurrentTime = appSeconds();
	MaxTickTime = 0.f;
	TotalTickTime = 0.f;

	return 0;
}

void FEngineLoop::Exit()
{
	if( GEngine != NULL )
	{
		GEngine->PreExit();
		GEngine = NULL;
	}
	appPreExit();
}

void FEngineLoop::Tick()
{
	GLog->FlushThreadedLogs();

	appUpdateTime();

	// tick engine
	GEngine->Tick(GDeltaTime);

	GDynamicDrv->Tick(GDeltaTime);

	GFrameCounter++;
	TotalTickTime += GDeltaTime;

	appWinPumpMessages();
}