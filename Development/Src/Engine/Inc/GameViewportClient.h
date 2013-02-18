#ifndef VRI3_ENGINE_GAMEVIEWPORT_H__
#define VRI3_ENGINE_GAMEVIEWPORT_H__

/**
 * VGameViewportClient is the engine's interface to a game viewport, rendering, audio and input subsystem
 */
class VGameViewportClient : public VObject, public FViewportClient
{
	DECLARE_CLASS(VGameViewportClient, VObject);

public:
	VGameViewportClient();

	UBOOL Init(FString& Error);

	void InitStaticData();

	INT InsertInteraction(VInteractionPtr NewInteraction, INT InIndex = -1);

	virtual void DetachViewportClient();

	virtual void SetViewport(FViewport* InViewport);

	void GetViewportSize(FVector2& OutViewportSize);

	virtual void Tick(FLOAT DeltaTime);

	virtual void Draw(FViewport* Viewport);

	virtual void RedrawRequested(FViewport* Viewport) { Viewport->Draw(); }

	virtual void CloseRequested(FViewport* Viewport);

	/**
	* @return True to consume the key event, false to pass it on
	*/
	virtual UBOOL InputKey(FViewport* Viewport, INT ControllerId, const FString& Key, EInputEvent Event);

	virtual UBOOL InputAxis(FViewport* Viewport, INT ControllerId, const FString& Key, FLOAT Delta, FLOAT DeltaTime);

	virtual UBOOL InputChar(FViewport* Viewport, INT ControllerId, TCHAR Character);

	virtual void MouseMove(FViewport* Viewport, INT X, INT Y);

	/**
	* Called when the mouse is moved while a window input capture is in effect
	*/
	virtual void CapturedMouseMove(FViewport* Viewport, INT MouseX, INT MouseY);

	virtual EMouseCursor GetCursor(FViewport* Viewport, INT X, INT Y);

	virtual void LostFocus(FViewport* Viewport) {}

	virtual void ReceivedFocus(FViewport* Viewport) {}

	virtual UBOOL IsFocused(FViewport* Viewport) { return TRUE; }

public:
	FViewport*		_Viewport;

	// a list of interactions
	vector<VInteractionPtr>		_GlobalInteractions;

	VConsolePtr					_Console;
	FName						_ConsoleClassName;

	VUIInteractionPtr			_UIInteraction;
	FName						_UIInteractionClassName;

	VPlayerInteractionPtr		_PlayerInteraction;
	FName						_PlayerInteractionClassName;
};
SmartPointer(VGameViewportClient);

#endif