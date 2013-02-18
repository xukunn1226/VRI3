#ifndef VRI3_ENGINE_VIEWPORTCLIENT_H_
#define VRI3_ENGINE_VIEWPORTCLIENT_H_

enum EInputEvent
{
	IE_Pressed		=	0,
	IE_Released		=	1,
	IE_Repeat		=	2,
	IE_DoubleClick	=	3,
	IE_Axis			=	4,
	IE_Max			=	5
};

/**
 * 对D3DVIEWPORT, I/O操作的封装
 */
class FViewport : public FRenderTarget, public FRenderResource
{
public:
	FViewport(class FViewportClient* InViewportClient);

	virtual ~FViewport() {}

	virtual void*	GetWindow() = 0;

	// New MouseCapture/MouseLock API
	virtual UBOOL	HasMouseCapture() const				{ return TRUE; }
	virtual UBOOL	HasFocus() const					{ return TRUE; }
	virtual void	CaptureMouse( UBOOL bCapture )		{ }
	virtual void	ShowCursor( UBOOL bVisible)	{ }
	virtual UBOOL	UpdateMouseCursor(UBOOL bSetCursor)	{ return TRUE; }

	virtual UBOOL	IsCursorVisible() const { return TRUE; }

	virtual UBOOL	KeyState(const TCHAR* Key) const = 0;

	virtual INT		GetMouseX() = 0;
	virtual INT		GetMouseY() = 0;
	virtual void	GetMousePoint(FVector2& MousePt) = 0;
	virtual void	SetMousePoint(INT x, INT y) = 0;
	virtual UBOOL	IsFullscreen() const { return _bIsFullscreen; }

	virtual void	InvalidateDisplay() = 0;

	virtual void	ProcessInput(FLOAT DeltaTime) = 0;

	virtual void	BeginRenderFrame();

	virtual void	EndRenderFrame(UBOOL bPresent);

	// FRenderTarget interface
	virtual UINT GetSizeX() const { return _SizeX; }
	virtual UINT GetSizeY() const { return _SizeY; }
	virtual FLOAT GetAspectRatio() const { return (FLOAT)GetSizeX() / (FLOAT)GetSizeY(); }

	const FD3D9ViewportPtr& GetD3DViewport() const { return _D3DViewport; }
	void Resize(UINT NewSizeX, UINT NewSizeY, UBOOL bFullscreen, INT InPosX = -1, INT InPosY = -1) {}
	
	virtual void Draw(UBOOL bPresent = TRUE);

	FViewportClient* GetClient() const { return _ViewportClient; }

	/**
	* Update the render target surface RHI to the current back buffer 
	*/
	void UpdateRenderTargetSurfaceRHIToCurrentBackBuffer();

protected:
	FViewportClient*	_ViewportClient;

	void UpdateViewport(UBOOL bDestroyed, UINT NewSizeX, UINT NewSizeY, UBOOL bIsFullscreen);
	
	virtual void InitDynamicDrv();

	virtual void ReleaseDynamicDrv();

private:
	UBOOL				_bIsFullscreen;

	UINT				_SizeX;
	UINT				_SizeY;

	FD3D9ViewportPtr	_D3DViewport;
};

/**
 * An abstract interface to a viewport's client
 * The viewport's client processes input received by the viewport, and draws the viewport.
 */
class FViewportClient
{
public:
	virtual void Draw(FViewport* Viewport) {}

	virtual void RedrawRequested(FViewport* Viewport) { Viewport->Draw(); }

	virtual void CloseRequested(FViewport* Viewport) {}

	/**
	 * @return True to consume the key event, false to pass it on
	 */
	virtual UBOOL InputKey(FViewport* Viewport, INT ControllerId, const FString& Key, EInputEvent Event) { return FALSE; }

	virtual UBOOL InputAxis(FViewport* Viewport, INT ControllerId, const FString& Key, FLOAT Delta, FLOAT DeltaTime) { return FALSE; }

	virtual UBOOL InputChar(FViewport* Viewport, INT ControllerId, TCHAR Character) { return FALSE; }

	virtual void MouseMove(FViewport* Viewport, INT X, INT Y) {}

	/**
	 * Called when the mouse is moved while a window input capture is in effect
	 */
	virtual void CapturedMouseMove(FViewport* Viewport, INT MouseX, INT MouseY) {}

	virtual EMouseCursor GetCursor(FViewport* Viewport, INT X, INT Y) { return MC_Arrow; }

	virtual void LostFocus(FViewport* Viewport) {}

	virtual void ReceivedFocus(FViewport* Viewport) {}
	
	virtual UBOOL IsFocused(FViewport* Viewport) { return TRUE; }
};

class VClient : public VObject
{
	DECLARE_CLASS(VClient, VObject);

public:
	virtual ~VClient() {}

	virtual void Init() {}

	virtual void Tick(FLOAT DeltaTime) {}

	virtual UBOOL Exec(const TCHAR* Cmd, FOutputDevice& Ar = *GLog) { return FALSE; }

	virtual FViewport* CreateViewport(FViewportClient* ViewportClient, const TCHAR* InName, UINT SizeX, UINT SizeY, UBOOL bIsFullscreen = FALSE) { return NULL; }
	virtual FViewport* CreateChildViewport(FViewportClient* ViewportClient, const TCHAR* InName, void* InParentWindow, UINT InPosX = -1, UINT InPosY = -1, UINT SizeX = 0, UINT SizeY = 0) { return NULL; }
	virtual void CloseViewport(FViewport* Viewport) {}
	virtual void Destroy() {}

	virtual FString GetVirtualKeyName(INT KeyCode) const { return TEXT(""); }
};
SmartPointer(VClient);

#endif