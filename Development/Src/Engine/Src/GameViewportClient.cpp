#include "EnginePCH.h"

IMPLEMENT_CLASS(VGameViewportClient);

VGameViewportClient::VGameViewportClient()
: _Console(NULL), _UIInteraction(NULL), _PlayerInteraction(NULL)
{
}

void VGameViewportClient::InitStaticData()
{
	_ConsoleClassName = FName(TEXT("VConsole"));		// from config
	_UIInteractionClassName = FName(TEXT("VUIInteraction"));
	_PlayerInteractionClassName = FName(TEXT("VPlayerInteraction"));
}

UBOOL VGameViewportClient::Init(FString& OutError)
{
	InitStaticData();

	TCHAR Error[MAX_SPRINTF] = TEXT("");

	// add console interaction
	_Console = ExactCast<VConsole>(VObject::StaticConstructObject(VObject::LoadClass(FName(_ConsoleClassName))));
	if( _Console == NULL )
	{
		appSprintf(Error, TEXT("Failed to create console system, in that illegal console class name(%s)"), _ConsoleClassName);
		OutError = Error;
		return FALSE;
	}
	if( InsertInteraction(_Console.GetReference()) == -1 )
	{
		appSprintf(Error, TEXT("Faile to add interaction to global interaction array"));
		OutError = Error;
		return FALSE;
	}

	// add ui interaction
	_UIInteraction = ExactCast<VUIInteraction>(VObject::StaticConstructObject(VObject::LoadClass(FName(_UIInteractionClassName))));
	if( _UIInteraction == NULL )
	{
		appSprintf(Error, TEXT("Failed to create ui interaction system, in that illegal ui interaction class name(%s)"), _ConsoleClassName);
		OutError = Error;
		return FALSE;
	}
	if( InsertInteraction(_UIInteraction.GetReference()) == -1 )
	{
		appSprintf(Error, TEXT("Faile to add interaction to global interaction array"));
		OutError = Error;
		return FALSE;
	}

	// add player interaction
	_PlayerInteraction = ExactCast<VPlayerInteraction>(VObject::StaticConstructObject(VObject::LoadClass(FName(_PlayerInteractionClassName))));
	if( _PlayerInteraction == NULL )
	{
		appSprintf(Error, TEXT("Failed to create player interaction system, in that illegal player interaction class name(%s)"), _PlayerInteractionClassName);
		OutError = Error;
		return FALSE;
	}
	if( InsertInteraction(_PlayerInteraction.GetReference()) == -1 )
	{
		appSprintf(Error, TEXT("Faile to add interaction to global interaction array"));
		OutError = Error;
		return FALSE;
	}

	return TRUE;
}

/**
 * Insert an interaction into the GlobalInteractions array at the specified index
 * @param	NewInteraction		the interaction should be inserted into the array
 * @param	InIndex				the position in the GlobalInteractions array to insert the element
 *								if no value is specified, inserts the interaction into the end of array
 *
 * @return	the position in the GlobalInteractions array where the element was placed, or -1 if the element
 *			wasn't added to the array for some reason
 */
INT VGameViewportClient::InsertInteraction(VInteractionPtr NewInteraction, INT InIndex /* = -1 */)
{
	INT Position;

	Position = -1;
	if( NewInteraction != NULL )
	{
		if( InIndex == -1 )
		{
			InIndex = _GlobalInteractions.size();
		}

		if( InIndex >= 0 )
		{
			Position = Clamp<INT>(InIndex, 0, _GlobalInteractions.size());

			_GlobalInteractions.insert(_GlobalInteractions.begin()+Position, NewInteraction);
			NewInteraction->Init();
		}
		else
		{
			debugf(TEXT("Invalid insert index specified: %d"), InIndex);
		}
	}
	return Position;
}

void VGameViewportClient::DetachViewportClient()
{
	if( _UIInteraction != NULL )
	{
		//_UIInteraction->ShutdownUI();
	}
	_UIInteraction = NULL;

	_Console = NULL;
	_PlayerInteraction = NULL;

	_GlobalInteractions.clear();
}

void VGameViewportClient::Tick(FLOAT DeltaTime)
{
	// tick all interactions
	for(INT i = 0; i < _GlobalInteractions.size(); ++i)
	{
		_GlobalInteractions[i]->Tick(DeltaTime);
	}
}

void VGameViewportClient::SetViewport(FViewport* InViewport)
{
	_Viewport = InViewport;

	if( _UIInteraction != NULL )
	{
		//_UIInteraction->SetRenderViewport(InViewport);
	}
}

/**
 * cut the link between ViewportClient and Viewport
 */
void VGameViewportClient::CloseRequested(FViewport* Viewport)
{
	check(Viewport == this->_Viewport);
	GEngine->GetClient()->CloseViewport(Viewport);
	SetViewport(NULL);
}

void VGameViewportClient::Draw(FViewport* Viewport)
{
	// render scene object

	// render post process

	// render ui scene
}

void VGameViewportClient::GetViewportSize(FVector2& OutViewportSize)
{
	if( _Viewport != NULL )
	{
		OutViewportSize.x = _Viewport->GetSizeX();
		OutViewportSize.y = _Viewport->GetSizeY();
	}
}

UBOOL VGameViewportClient::InputKey(FViewport *Viewport, INT ControllerId, const FString &Key, EInputEvent Event)
{
	UBOOL bResult = FALSE;
	for(INT InteractionIndex = 0; InteractionIndex < _GlobalInteractions.size(); ++InteractionIndex)
	{
		bResult = bResult || _GlobalInteractions[InteractionIndex]->InputKey(Viewport, ControllerId, Key, Event);
	}
	return bResult;
}

UBOOL VGameViewportClient::InputAxis(FViewport* Viewport, INT ControllerId, const FString& Key, FLOAT Delta, FLOAT DeltaTime)
{
	UBOOL bResult = FALSE;
	for(INT InteractionIndex = 0; InteractionIndex < _GlobalInteractions.size(); ++InteractionIndex)
	{
		bResult = bResult || _GlobalInteractions[InteractionIndex]->InputAxis(Viewport, ControllerId, Key, Delta, DeltaTime);
	}
	return bResult;
}

UBOOL VGameViewportClient::InputChar(FViewport* Viewport, INT ControllerId, TCHAR Character)
{
	UBOOL bResult = FALSE;
	for(INT InteractionIndex = 0; InteractionIndex < _GlobalInteractions.size(); ++InteractionIndex)
	{
		bResult = bResult || _GlobalInteractions[InteractionIndex]->InputChar(Viewport, ControllerId, Character);
	}
	return bResult;
}

void VGameViewportClient::MouseMove(FViewport *Viewport, INT X, INT Y)
{
}

void VGameViewportClient::CapturedMouseMove(FViewport* Viewport, INT MouseX, INT MouseY)
{
}

EMouseCursor VGameViewportClient::GetCursor(FViewport* Viewport, INT X, INT Y)
{
	return MC_Arrow;
}