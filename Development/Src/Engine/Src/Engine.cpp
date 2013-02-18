#include "EnginePCH.h"

VEnginePtr GEngine = NULL;

IMPLEMENT_CLASS(VEngine);

VEngine::VEngine()
: _GameViewport(NULL), 
  _GameClient(NULL)
{
}

VEngine::~VEngine()
{
}

void VEngine::InitStaticData()
{
	_GameViewportClass = VObject::LoadClass(FName(TEXT("VWindowClient")));
	_GameClientClass = VObject::LoadClass(FName(TEXT("VGameViewportClient")));

	//UBOOL Result = GConfigSystem.GetSectionKeyValue(ECT_DefaultEngine, TEXT("Engine"), TEXT("Client"), _GameClientClassName);
}

void VEngine::Init()
{
	InitStaticData();

	if( ParseParam(appCmdLine(), TEXT("FIXEDSEED")) )
	{
		appRandInit(0);
	}
	else
	{
		appRandInit(appCycles());
	}

	// Init client
	_GameClient = Cast<VWindowClient>(VObject::StaticConstructObject(_GameViewportClass));
	_GameClient->Init();

	// Init GameViewport
	VGameViewportClientPtr ViewportClient = NULL;
	if( _GameClient )
	{
		ViewportClient = Cast<VGameViewportClient>(VObject::StaticConstructObject(_GameClientClass));
		_GameViewport = ViewportClient;
	}

	if( ViewportClient )
	{
		FString Error;
		if( !ViewportClient->Init(Error) )
		{
			appErrorf(TEXT("%s"), Error.c_str());
			return;
		}

		FViewport* Viewport = _GameClient->CreateViewport(ViewportClient, TEXT("Product"), 800, 600, FALSE);
		_GameViewport->SetViewport(Viewport);
	}

	GLog->Log(NAME_Init, TEXT("Game engine initialized"));
}

void VEngine::PreExit()
{
	_GameClient->Destroy();
	_GameClient = NULL;
}

void VEngine::RedrawViewports(UBOOL bShouldPresent /* = TRUE */)
{
	if( _GameViewport != NULL && _GameViewport->_Viewport != NULL )
	{
		_GameViewport->_Viewport->Draw(bShouldPresent);
	}
}

void VEngine::Tick(FLOAT DeltaTime)
{
	if( DeltaTime < 0.0f )
	{
		debugf(TEXT("Negative delta time!!"));
	}

	// tick the client, dispatch messages
	if( _GameClient )
	{
		_GameClient->Tick(DeltaTime);
	}

	CleanupGameViewport();

	if( _GameViewport == NULL )
	{
		appRequestExit(0);
		return;
	}

	// tick the world
	//GWorld->Tick(DeltaSeconds);

	// tick the viewport
	if( _GameViewport != NULL )
	{
		_GameViewport->Tick(DeltaTime);
	}

	static UBOOL bFirstTime = TRUE;
	if( bFirstTime )
	{
		bFirstTime = FALSE;
		extern void appShowGameWindow();
		appShowGameWindow();
	}

	// render the world
	//GDynamicDrv->Clear(TRUE, FColor(1.0f, 0.0f, 0.0f), TRUE, 1.0f, FALSE, 0);
	//RedrawViewports(TRUE);
	
	//GDynamicDrv->SetViewport(0, 800, 0, 600, 0, 1);
	//GDynamicDrv->BeginDrawingViewport(m_pGameClient->_Viewports[0]->GetD3DViewport());
	//GDynamicDrv->EndDrawingViewport(m_pGameClient->_Viewports[0]->GetD3DViewport(), TRUE);
}

void VEngine::CleanupGameViewport()
{
	if( _GameViewport != NULL && _GameViewport->_Viewport == NULL )
	{
		_GameViewport->DetachViewportClient();
		_GameViewport = NULL;
	}
}

FViewport* VEngine::GetAViewport() const
{
	if( _GameViewport != NULL )
	{
		return _GameViewport->_Viewport;
	}
	return NULL;
}

VClient* VEngine::GetClient() const
{
	return _GameClient;
}