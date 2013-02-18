#ifndef VRI3_ENGINE_WINCLIENT_H_
#define VRI3_ENGINE_WINCLIENT_H_

class FWindowViewport;

extern UBOOL GWindowActive;

struct FDeferredMessage
{
	FWindowViewport*	Viewport;
	UINT				Message;
	WPARAM				wParam;
	LPARAM				lParam;

	struct
	{
		SHORT	LeftControl;
		SHORT	RightControl;
		SHORT	LeftShift;
		SHORT	RightShift;
		SHORT	LeftMenu;
		SHORT	RightMenu;
	} KeyStates;
};

class VWindowClient : public VClient
{
	DECLARE_CLASS(VWindowClient, VClient);

public:
	static vector<FWindowViewport*>		_Viewports;

	FString								_WindowClassName;

	map<BYTE, FString>					_KeyVirtualToName;
	map<FString, BYTE>					_KeyNameToVirtual;

	vector<FDeferredMessage>			_DeferredMessages;

public:
	VWindowClient();

	virtual ~VWindowClient() {}

	virtual void Init();

	virtual void Tick(FLOAT DeltaTime);

	virtual UBOOL Exec(const TCHAR* Cmd, FOutputDevice& Ar = *GLog);

	virtual FViewport* CreateViewport(FViewportClient* ViewportClient, const TCHAR* InName, UINT SizeX, UINT SizeY, UBOOL bIsFullscreen = FALSE);
	virtual FViewport* CreateChildViewport(FViewportClient* ViewportClient, const TCHAR* InName, void* InParentWindow, UINT InPosX = -1, UINT InPosY = -1, UINT SizeX = 0, UINT SizeY = 0);
	virtual void CloseViewport(FViewport* Viewport);
	virtual void Destroy();

	static LONG APIENTRY StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	void DeferMessage(FWindowViewport* Viewport, UINT Message, WPARAM wParam, LPARAM lParam);
	void ProcessDeferredMessages();
	void ShutdownAfterError();

	void ProcessInput(FLOAT DeltaTime);

	FString GetVirtualKeyName(INT KeyCode) const;

	static const FWindowViewport* FindViewport(const FWindowViewport* Viewport);
};
SmartPointer(VWindowClient);


class FWindowViewport : public FViewport
{
public:
	FWindowViewport(
		VWindowClient* InClient,
		FViewportClient* InViewportClient,
		const TCHAR* InName,
		UINT InSizeX,
		UINT InSizeY,
		UBOOL bIsFullscreen,
		HWND ParentWindow,
		INT InPosX = -1,
		INT InPosY = -1
		);

	virtual ~FWindowViewport();

	virtual void* GetWindow() {return _Window;}
	virtual HWND  GetParentWindow() { return _ParentWindow; }

	FString	GetName() { return _Name; }

	// New MouseCapture/MouseLock API
	virtual UBOOL	HasMouseCapture() const { return _bCapturingMouseInput; }
	virtual UBOOL	HasFocus() const;
	virtual void	CaptureMouse( UBOOL bCapture );
	virtual void	ShowCursor( UBOOL bVisible);
	virtual UBOOL	UpdateMouseCursor(UBOOL bSetCursor);

	virtual UBOOL	IsCursorVisible() const;

	virtual UBOOL	KeyState(const TCHAR* Key) const;

	virtual INT		GetMouseX();
	virtual INT		GetMouseY();
	virtual void	GetMousePoint(FVector2& MousePt);
	virtual void	SetMousePoint(INT X, INT Y);

	virtual void	InvalidateDisplay();

	virtual void	ProcessInput(FLOAT DeltaTime);

	virtual void	Resize(UINT NewSizeX, UINT NewSizeY, UBOOL NewFullscreen, INT InPosX = -1, INT InPosY = -1);

	UBOOL	GetResizing() { return _Resizing; }
	UBOOL	GetPerformingSize() { return _PerformingSize; }
	void	SetResizing(UBOOL bResizing) { _Resizing = bResizing; }
	void	SetPerformingResize(UBOOL bPerformingSize) { _PerformingSize = bPerformingSize; }

	void	Destroy();
	void	ShutdownAfterError();
	LONG	ViewportWndProc(UINT Message, WPARAM wParam, LPARAM lParam);
	void	ProcessDeferredMessage(const FDeferredMessage& DeferredMessage);
	void	Tick(FLOAT DeltaTime);

private:
	VWindowClient*	_WindowClient;

	HWND			_Window;
	HWND			_ParentWindow;

	FString			_Name;

	UBOOL			_Minimized;
	UBOOL			_Maximized;
	UBOOL			_Resizing;
	UBOOL			_PerformingSize;

	UBOOL			_KeyStates[256];

	// New MouseCapture/MouseLock API
	EMouseCursor	_SystemMouseCursor;
	UBOOL			_bCapturingMouseInput;
	UBOOL			_PreventCapture;
	POINT			_PreCaptureMousePos;		// 记录capture mouse之前的鼠标位置

	void HandleSizeChange();
	void UpdateWindow(UBOOL bIsFullscreen, DWORD WindowStyle, INT WindowPosX, INT WindowPosY, INT WindowWidth, INT WindowHeight);
	void UpdateRenderDevice(UBOOL bIsFullscreen, UINT NewSizeX, UINT NewSizeY);

	void OnMouseButtonDown(UINT Message, WPARAM wParam);
	void OnMouseButtonUp(UINT Message, WPARAM wParam);
	void OnMouseDoubleClick(UINT Message, WPARAM wParam);
};


#endif