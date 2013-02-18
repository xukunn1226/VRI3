#include "EnginePCH.h"

IMPLEMENT_CLASS(VWindowClient);


HWND GGameWindow = NULL;
DWORD GGameWindowStyle = 0;
INT GGameWindowPosX = 0;
INT GGameWindowPosY = 0;
INT GGameWindowWidth = 100;
INT GGameWindowHeight = 100;
UBOOL GGameWindowUsingStartupWindowProc = FALSE;
UBOOL GWindowActive = FALSE;
/** Width of the primary monitor, in pixels. */
INT GPrimaryMonitorWidth = 0;
/** Height of the primary monitor, in pixels. */
INT GPrimaryMonitorHeight = 0;
/** Rectangle of the work area on the primary monitor (excluding taskbar, etc) in "virtual screen coordinates" (pixels). */
RECT GPrimaryMonitorWorkRect;
/** Virtual screen rectangle including all monitors. */
RECT GVirtualScreenRect;

void appShowGameWindow()
{
	if( GGameWindow )
	{
		extern DWORD	GGameWindowStyle;
		extern INT		GGameWindowPosX;
		extern INT		GGameWindowPosY;
		extern INT		GGameWindowWidth;
		extern INT		GGameWindowHeight;

		// Convert position from screen coordinates to workspace coordinates.
		HMONITOR Monitor = MonitorFromWindow(GGameWindow, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO MonitorInfo;
        MonitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo( Monitor, &MonitorInfo );
        INT PosX = GGameWindowPosX - MonitorInfo.rcWork.left;
        INT PosY = GGameWindowPosY - MonitorInfo.rcWork.top;

		// release old messages using the old StartupWindowProc
		appWinPumpMessages();

		SetWindowLong(GGameWindow, GWL_STYLE, GGameWindowStyle);
		SetWindowLong(GGameWindow, GWL_WNDPROC, (LONG)VWindowClient::StaticWndProc);
		GGameWindowUsingStartupWindowProc = FALSE;

		WINDOWPLACEMENT wp;
		appMemzero(&wp, sizeof(WINDOWPLACEMENT));
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(GGameWindow, &wp);
		wp.flags = 0;
		wp.showCmd = SW_SHOWNORMAL;
		wp.rcNormalPosition.left = PosX;
		wp.rcNormalPosition.right = PosX + GGameWindowWidth;
		wp.rcNormalPosition.top = PosY;
		wp.rcNormalPosition.bottom = PosY + GGameWindowHeight;
		SetWindowPlacement(GGameWindow, &wp);
		UpdateWindow(GGameWindow);

		appWinPumpMessages();
	}
}

FWindowViewport::FWindowViewport(VWindowClient* InClient,
								 FViewportClient* InViewportClient,
								 const TCHAR* InName,
								 UINT InSizeX,
								 UINT InSizeY,
								 UBOOL bIsFullscreen,
								 HWND ParentWindow,
								 INT InPosX,
								 INT InPosY)
: FViewport(InViewportClient)
{
	_WindowClient = InClient;

	_Name = InName;

	_Window = NULL;
	_ParentWindow = ParentWindow;
	_Minimized = FALSE;
	_Maximized = FALSE;
	_Resizing = FALSE;
	_PerformingSize = FALSE;

	_SystemMouseCursor = MC_Arrow;
	_bCapturingMouseInput = FALSE;

	_WindowClient->_Viewports.push_back(this);

	if( InPosX == -1 )
	{
		InPosX = GPrimaryMonitorWidth > InSizeX ? (GPrimaryMonitorWidth - InSizeX)/2 : 0;
	}
	if( InPosY == -1 )
	{
		InPosY = GPrimaryMonitorHeight > InSizeY ? (GPrimaryMonitorHeight - InSizeY)/2 : 0;
	}

	Resize(InSizeX, InSizeY, bIsFullscreen, InPosX, InPosY);

	::SetActiveWindow(_Window);

	for(INT KeyIndex = 0; KeyIndex < 256; ++KeyIndex)
	{
		map<BYTE, FString>::const_iterator it;
		it = _WindowClient->_KeyVirtualToName.find(KeyIndex);
		if( it != _WindowClient->_KeyVirtualToName.end() )
		{
			_KeyStates[KeyIndex] = ::GetKeyState(KeyIndex) & 0x8000;
		}
	}
}

FWindowViewport::~FWindowViewport()
{
	if( _WindowClient != NULL )
	{
		for(UINT MsgIndex = 0; MsgIndex < _WindowClient->_DeferredMessages.size(); ++MsgIndex)
		{
			FDeferredMessage& DeferredMessage = _WindowClient->_DeferredMessages[MsgIndex];
			if( DeferredMessage.Viewport == this )
			{
				_WindowClient->_DeferredMessages.erase(_WindowClient->_DeferredMessages.begin() + MsgIndex);
			}
		}
	}
}

UBOOL FWindowViewport::KeyState(const TCHAR *Key) const
{
	if( _WindowClient )
	{
		map<FString, BYTE>::const_iterator it;
		it = _WindowClient->_KeyNameToVirtual.find(Key);
		if( it != _WindowClient->_KeyNameToVirtual.end() )
		{
			return _KeyStates[it->second];
		}
	}
	return FALSE;
}

UBOOL FWindowViewport::HasFocus() const
{
	HWND FocusWnd = ::GetFocus();
	return FocusWnd == _Window;
}

UBOOL FWindowViewport::IsCursorVisible() const
{
	CURSORINFO CursorInfo;
	CursorInfo.cbSize = sizeof(CURSORINFO);
	UBOOL bVisible = (::GetCursorInfo(&CursorInfo) != 0);
	bVisible = bVisible && (CursorInfo.flags&CURSOR_SHOWING) && (CursorInfo.hCursor != NULL);
	return bVisible;
}

void FWindowViewport::ShowCursor(UBOOL bVisible)
{
	UBOOL bIsCursorVisible = IsCursorVisible();
	if( bVisible && !bIsCursorVisible )
	{
		if( _PreCaptureMousePos.x >= 0 && _PreCaptureMousePos.y >= 0 )
		{
			::SetCursorPos(_PreCaptureMousePos.x, _PreCaptureMousePos.y);
		}

		while( ::ShowCursor(TRUE) < 0 );
		_PreCaptureMousePos.x = -1;
		_PreCaptureMousePos.y = -1;
	}
	else if( !bVisible && bIsCursorVisible )
	{
		while( ::ShowCursor(FALSE) >= 0 );

		_PreCaptureMousePos.x = -1;
		_PreCaptureMousePos.y = -1;
		::GetCursorPos(&_PreCaptureMousePos);
	}
}

void FWindowViewport::CaptureMouse( UBOOL bCapture )
{
	HWND CapturedWindow = ::GetCapture();		// the window that has captured the mouse
	UBOOL bIsMouseCaptured = (CapturedWindow == _Window);
	_bCapturingMouseInput = bCapture;
	if( bCapture && !bIsMouseCaptured )
	{
		::SetCapture(_Window);
	}
	else if( !bCapture && bIsMouseCaptured )
	{
		::ReleaseCapture();
	}
}

UBOOL FWindowViewport::UpdateMouseCursor(UBOOL bSetCursor)
{
	UBOOL bHandled = FALSE;
	HCURSOR NewCursor = NULL;
	if( IsFullscreen() )
	{
		bHandled = bSetCursor;
		_SystemMouseCursor = MC_None;
	}
	else
	{
		INT MouseX = GetMouseX();
		INT MouseY = GetMouseY();
		_SystemMouseCursor = MC_Arrow;
		if( _ViewportClient )
		{
			_SystemMouseCursor = _ViewportClient->GetCursor(this, MouseX, MouseY);
		}

		if( MouseX < 0 || MouseY < 0 || MouseX >= GetSizeX() || MouseY >= GetSizeY() )
		{
			bHandled = FALSE;
		}
		else if( _SystemMouseCursor == MC_None && HasFocus() )
		{
			bHandled = bSetCursor;
		}
		else
		{
			LPCTSTR CursorResource = IDC_ARROW;
			HINSTANCE HInstanceForCursor = NULL;
			switch(_SystemMouseCursor)
			{
			case MC_Arrow:		CursorResource = IDC_ARROW; break;
			case MC_Cross:		CursorResource = IDC_CROSS; break;
			case MC_SizeAll:	CursorResource = IDC_SIZEALL; break;
			case MC_Hand:		CursorResource = IDC_HAND; break;
			case MC_GrabHand:
			case MC_Custom:
				//CursorResource = MAKEINTRESOURCE(IDCUR_GRABHAND);
				HInstanceForCursor = hInstance;
				break;
			}
			NewCursor = ::LoadCursor(HInstanceForCursor, CursorResource);
			bHandled = bSetCursor;
		}
	}
	if( bHandled )
	{
		::SetCursor(NewCursor);
	}
	return bHandled;
}

INT FWindowViewport::GetMouseX()
{
	POINT p;
	::GetCursorPos(&p);
	::ScreenToClient(_Window, &p);
	return p.x;
}

INT FWindowViewport::GetMouseY()
{
	POINT p;
	::GetCursorPos(&p);
	::ScreenToClient(_Window, &p);
	return p.y;
}

void FWindowViewport::GetMousePoint(FVector2 &MousePt)
{
	POINT p;
	::GetCursorPos(&p);
	::ScreenToClient(_Window, &p);
	MousePt.x = p.x;
	MousePt.y = p.y;
}

// 非capture mouse时调用此接口更新鼠标位置
void FWindowViewport::SetMousePoint(INT X, INT Y)
{
	_PreCaptureMousePos.x = X;
	_PreCaptureMousePos.y = Y;
	::ClientToScreen(_Window, &_PreCaptureMousePos);
	::SetCursorPos(_PreCaptureMousePos.x, _PreCaptureMousePos.y);
}

void FWindowViewport::InvalidateDisplay()
{
	::InvalidateRect(_Window, NULL, FALSE);
}

void FWindowViewport::ProcessInput(FLOAT DeltaTime)
{
}

void FWindowViewport::Resize(UINT NewSizeX, UINT NewSizeY, UBOOL NewFullscreen, INT InPosX, INT InPosY)
{
	if( _PerformingSize )
	{
		return;
	}

	if( NewFullscreen && !_ParentWindow )
	{
		GDynamicDrv->GetSupportedResolution(NewSizeX, NewSizeY);
	}

	_PerformingSize = TRUE;
	UBOOL bWasFullscreen = IsFullscreen();
	UBOOL bUpdateWindow = FALSE;

	DWORD WindowStyle;
	UBOOL bShowWindow = TRUE;
	UINT WindowWidth = NewSizeX;
	UINT WindowHeight = NewSizeY;
	UINT WindowPosX = InPosX;
	UINT WindowPosY = InPosY;

	if( _ParentWindow )
	{
		NewFullscreen = FALSE;
		WindowStyle = WS_CHILD | WS_CLIPSIBLINGS;
	}
	else
	{
		if( NewFullscreen )
		{
			WindowStyle = WS_POPUP | WS_SYSMENU;
		}
		else
		{
			WindowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_BORDER | WS_CAPTION;
		}
	}

	// adjust window size base on the style
	RECT WindowRect;
	WindowRect.left = 0;
	WindowRect.top = 0;
	WindowRect.right = NewSizeX;
	WindowRect.bottom = NewSizeY;
	::AdjustWindowRect(&WindowRect, WindowStyle, FALSE);
	WindowWidth = WindowRect.right - WindowRect.left;
	WindowHeight = WindowRect.bottom - WindowRect.top;

	if( NewFullscreen )
	{
		WindowPosX = 0;
		WindowPosY = 0;
		WindowWidth = NewSizeX;
		WindowHeight = NewSizeY;
	}
	else if( _Window == NULL )
	{
		INT ScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
		INT ScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		if( WindowPosX == -1 )
		{
			WindowPosX = (ScreenWidth - WindowWidth) / 2;
		}

		if( WindowPosY == -1 )
		{
			WindowPosY = (ScreenHeight - WindowHeight) / 2;
		}
	}
	else
	{
		RECT WindowRect;
		::GetWindowRect(_Window, &WindowRect);
		WindowPosX = WindowRect.left;
		WindowPosY = WindowRect.top;
	}

	if( !_ParentWindow && !NewFullscreen )
	{
		RECT ScreenRect;
		ScreenRect.left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
		ScreenRect.right = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		ScreenRect.top = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
		ScreenRect.bottom = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		if( (WindowPosX + WindowWidth) > ScreenRect.right )
		{
			WindowPosX = ScreenRect.right - WindowWidth;
		}
		if( (WindowPosX) < ScreenRect.left )
		{
			WindowPosX = ScreenRect.left;
		}
		if( (WindowPosY + WindowHeight) > ScreenRect.bottom )
		{
			WindowPosY = ScreenRect.bottom - WindowHeight;
		}
		if( (WindowPosY) < ScreenRect.top )
		{
			WindowPosY = ScreenRect.top;
		}
	}

	if( _Window == NULL )
	{
		if( GGameWindow )
		{
			_Window = GGameWindow;
			SetWindowText(_Window, _Name.c_str());
			bShowWindow = FALSE;
		}
		else
		{
			_Window = CreateWindowEx(WS_EX_APPWINDOW, _WindowClient->_WindowClassName.c_str(), _Name.c_str(), WindowStyle,
									 WindowPosX, WindowPosY, WindowWidth, WindowHeight, _ParentWindow, NULL, hInstance, this);
		}
		check(_Window);
	}
	else
	{
		bUpdateWindow = TRUE;
	}

	if( GGameWindow && GGameWindow == _Window && GGameWindowUsingStartupWindowProc )
	{
		GGameWindowStyle = WindowStyle;
		GGameWindowPosX = WindowPosX;
		GGameWindowPosY = WindowPosY;
		GGameWindowWidth = WindowWidth;
		GGameWindowHeight = WindowHeight;

		if( NewFullscreen )
		{
			appShowGameWindow();
		}
	}

	if( NewFullscreen )
	{
		while(::ShowCursor(FALSE) < 0 );
	}

	// switch to window
	if( bWasFullscreen && !NewFullscreen )
	{
		while(::ShowCursor(TRUE) < 0);

		UpdateRenderDevice(NewFullscreen, NewSizeX, NewSizeY);

		if( bUpdateWindow )
		{
			UpdateWindow(NewFullscreen, WindowStyle, WindowPosX, WindowPosY, WindowWidth, WindowHeight);
		}
	}
	else
	{
		if( bUpdateWindow )
		{
			UpdateWindow(NewFullscreen, WindowStyle, WindowPosX, WindowPosY, WindowWidth, WindowHeight);
		}

		UpdateRenderDevice(NewFullscreen, NewSizeX, NewSizeY);
	}

	if( bShowWindow )
	{
		::ShowWindow(_Window, SW_SHOW);
		::UpdateWindow(_Window);
	}
	_PerformingSize = FALSE;
}

void FWindowViewport::UpdateWindow(UBOOL bIsFullscreen, DWORD WindowStyle, INT WindowPosX, INT WindowPosY, INT WindowWidth, INT WindowHeight)
{
	LONG CurrentWindowStyle = GetWindowLong(_Window, GWL_STYLE);
	LONG CurrentWindowStyleEx = GetWindowLong(_Window, GWL_EXSTYLE);

	UINT Flags = SWP_NOZORDER;
	if( (CurrentWindowStyle & WindowStyle) != WindowStyle )
	{
		SetWindowLong(_Window, GWL_STYLE, WindowStyle);
		Flags |= SWP_FRAMECHANGED;
	}

	if( !_ParentWindow && !bIsFullscreen )
	{
		HWND hWndInsertAfter = HWND_TOP;
		if( CurrentWindowStyleEx & WS_EX_TOPMOST )
		{
			hWndInsertAfter = HWND_NOTOPMOST;
			Flags &= ~SWP_NOZORDER;
		}
		::SetWindowPos(_Window, hWndInsertAfter, WindowPosX, WindowPosY, WindowWidth, WindowHeight, Flags);
	}
}

void FWindowViewport::UpdateRenderDevice(UBOOL bIsFullscreen, UINT NewSizeX, UINT NewSizeY)
{
	if( NewSizeX && NewSizeY )
	{
		UpdateViewport(FALSE, NewSizeX, NewSizeY, bIsFullscreen);
	}
	else if( NewSizeX && !NewSizeY )
	{
		NewSizeY = 1;
		UpdateViewport(FALSE, NewSizeX, NewSizeY, bIsFullscreen);
	}
	else if( !NewSizeX && NewSizeY )
	{
		UpdateViewport(FALSE, NewSizeX, NewSizeY, bIsFullscreen);
	}
}

void FWindowViewport::Destroy()
{
	_ViewportClient = NULL;

	// destroy D3DViewport
	UpdateViewport(TRUE, 0, 0, FALSE);

	DestroyWindow(_Window);
	_Window = NULL;
}

void FWindowViewport::ShutdownAfterError()
{
	if( _Window )
	{
		DestroyWindow(_Window);
		_Window = NULL;
	}
}

LONG FWindowViewport::ViewportWndProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	if( _WindowClient->FindViewport(this) == NULL )
	{
		return DefWindowProc(_Window, Message, wParam, lParam);
	}

	switch(Message)
	{
	case WM_CLOSE:
	case WM_CHAR:
	case WM_SYSCHAR:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		_WindowClient->DeferMessage(this, Message, wParam, lParam);
		return 0;		// If an application processes this message, it should return zero

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		_WindowClient->DeferMessage(this, Message, wParam, lParam);
		return 1;

	case WM_PAINT:
	case WM_SETCURSOR:
		_WindowClient->DeferMessage(this, Message, wParam, lParam);
		return DefWindowProc(_Window, Message, wParam, lParam);

	case WM_MOUSEMOVE:
	case WM_ENTERSIZEMOVE:
	case WM_CAPTURECHANGED:
	case WM_ACTIVATE:
	case WM_EXITSIZEMOVE:
	case WM_SIZE:
	case WM_SIZING:
		_WindowClient->DeferMessage(this, Message, wParam, lParam);
		return DefWindowProc(_Window, Message, wParam, lParam);
	case WM_ACTIVATEAPP:
		if( wParam == TRUE )
		{
			GWindowActive = TRUE;
		}
		else
		{
			GWindowActive = FALSE;
		}
		return DefWindowProc(_Window, Message, wParam, lParam);

	case WM_DESTROY:
		_Window = NULL;
		return 0;

	case WM_MOUSEACTIVATE:
		if( LOWORD(lParam) != HTCLIENT )
		{
			_PreventCapture = TRUE;
		}
		_WindowClient->DeferMessage(this, Message, wParam, lParam);
		return MA_ACTIVATE;

	case WM_ERASEBKGND:
		return 1;

	case WM_POWERBROADCAST:
		switch( wParam )
		{
		case PBT_APMQUERYSUSPEND:
		case PBT_APMQUERYSTANDBY:
			return BROADCAST_QUERY_DENY;
		}
		return DefWindowProc(_Window, Message, wParam, lParam );

	case WM_NCHITTEST:
		if( IsFullscreen() )
		{
			return HTCLIENT;
		}
		return DefWindowProc(_Window, Message, wParam, lParam);

	case WM_QUERYENDSESSION:
		debugf(TEXT(""));
		return DefWindowProc(_Window, Message, wParam, lParam);

	case WM_ENDSESSION:
		if( wParam )
		{
			debugf(TEXT(""));
			appRequestExit(FALSE);
			return TRUE;
		}
		return DefWindowProc(_Window, Message, wParam, lParam);

	default:
		return DefWindowProc(_Window, Message, wParam, lParam);
	}
}

void FWindowViewport::ProcessDeferredMessage(const FDeferredMessage &DeferredMessage)
{
	const WPARAM wParam = DeferredMessage.wParam;
	const LPARAM lParam = DeferredMessage.lParam;

	switch(DeferredMessage.Message)
	{
	case WM_CLOSE:
		if( _ViewportClient )
		{
			_ViewportClient->CloseRequested(this);
		}
		break;
	case WM_PAINT:
		if( _ViewportClient )
		{
			_ViewportClient->RedrawRequested(this);
		}
		break;
	case WM_ACTIVATE:
		break;
	case WM_MOUSEMOVE:
		{
			INT X = LOWORD(lParam);
			INT Y = HIWORD(lParam);

			if( _ViewportClient )
			{
				if( _bCapturingMouseInput )
				{
					_ViewportClient->CapturedMouseMove(this, X, Y);
				}
				else
				{
					_ViewportClient->MouseMove(this, X, Y);
				}
			}
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			UINT KeyCode = wParam;
			switch(KeyCode)
			{
			case VK_MENU:
				{
					if( DeferredMessage.KeyStates.RightMenu & 0x8000 )
					{
						KeyCode = VK_RMENU;
					}
					else if( DeferredMessage.KeyStates.LeftMenu & 0x8000 )
					{
						KeyCode = VK_LMENU;
					}
					break;
				}
			case VK_CONTROL:
				{
					if( DeferredMessage.KeyStates.RightControl & 0x8000 )
					{
						KeyCode = VK_RCONTROL;
					}
					else if( DeferredMessage.KeyStates.LeftControl & 0x8000 )
					{
						KeyCode = VK_LCONTROL;
					}
					break;
				}
			case VK_SHIFT:
				{
					if( DeferredMessage.KeyStates.RightShift & 0x8000 )
					{
						KeyCode = VK_RSHIFT;
					}
					else if( DeferredMessage.KeyStates.LeftShift & 0x8000 )
					{
						KeyCode = VK_LSHIFT;
					}
					break;
				}
			};

			FString KeyName = _WindowClient->GetVirtualKeyName(KeyCode);
			if( KeyName != TEXT("") )
			{
				UBOOL bOldKeyState = _KeyStates[KeyCode];
				_KeyStates[KeyCode] = TRUE;

				if( _ViewportClient )
				{
					_ViewportClient->InputKey(this, 0, KeyName, bOldKeyState ? IE_Repeat : IE_Pressed);
				}
			}
		}
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			UINT KeyCode = wParam;
			switch(KeyCode)
			{
			case VK_MENU:
				{
					if( _KeyStates[VK_RMENU] && !(DeferredMessage.KeyStates.RightMenu & 0x8000) )
					{
						KeyCode = VK_RMENU;
					}
					else if( _KeyStates[VK_LMENU] && !(DeferredMessage.KeyStates.LeftMenu & 0x8000) )
					{
						KeyCode = VK_LMENU;
					}
					break;
				}
			case VK_CONTROL:
				{
					if( _KeyStates[VK_RCONTROL] && !(DeferredMessage.KeyStates.RightControl & 0x8000) )
					{
						KeyCode = VK_RCONTROL;
					}
					else if( _KeyStates[VK_LCONTROL] && !(DeferredMessage.KeyStates.LeftControl & 0x8000) )
					{
						KeyCode = VK_LCONTROL;
					}
					break;
				}
			case VK_SHIFT:
				{
					if( _KeyStates[VK_RSHIFT] && !(DeferredMessage.KeyStates.RightShift & 0x8000) )
					{
						KeyCode = VK_RSHIFT;
					}
					else if( _KeyStates[VK_LSHIFT] && !(DeferredMessage.KeyStates.LeftShift & 0x8000) )
					{
						KeyCode = VK_LSHIFT;
					}
					break;
				}
			};

			FString KeyName = _WindowClient->GetVirtualKeyName(KeyCode);
			if( KeyName != TEXT("") )
			{
				_KeyStates[KeyCode] = FALSE;

				if( _ViewportClient )
				{
					_ViewportClient->InputKey(this, 0, KeyName, IE_Released);
				}
			}
		}
		break;

	case WM_CHAR:
	case WM_SYSCHAR:
		{
			TCHAR Character = TCHAR(wParam);
			if( _ViewportClient )
			{
				_ViewportClient->InputChar(this, 0, Character);
			}
		}
		break;
	case WM_SETCURSOR:	// 鼠标在窗口内，且非CAPTURE时发送
		UpdateMouseCursor(TRUE);
		break;
	case WM_SETFOCUS:
		if( _ViewportClient && !_PreventCapture )
		{
			_ViewportClient->ReceivedFocus(this);
		}
		UpdateMouseCursor(TRUE);
		break;
	case WM_KILLFOCUS:
		if( _ViewportClient )
		{
			_ViewportClient->LostFocus(this);
		}
		if( _bCapturingMouseInput )
		{
			CaptureMouse(FALSE);
			OnMouseButtonUp( WM_LBUTTONUP, 0 );
			OnMouseButtonUp( WM_MBUTTONUP, 0 );
			OnMouseButtonUp( WM_RBUTTONUP, 0 );
			OnMouseButtonUp( WM_XBUTTONUP, 0 );
			OnMouseButtonUp( WM_XBUTTONUP, 0 ); // Note: need two XBUTTONUP (for both xbuttons)
		}
		break;
	case WM_CAPTURECHANGED:
		if( _bCapturingMouseInput )
		{
			CaptureMouse(FALSE);
			OnMouseButtonUp( WM_LBUTTONUP, 0 );
			OnMouseButtonUp( WM_MBUTTONUP, 0 );
			OnMouseButtonUp( WM_RBUTTONUP, 0 );
			OnMouseButtonUp( WM_XBUTTONUP, 0 );
			OnMouseButtonUp( WM_XBUTTONUP, 0 ); // Note: need two XBUTTONUP (for both xbuttons)
		}
		break;
	case WM_MOUSEACTIVATE:
		UpdateMouseCursor(TRUE);
		break;
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
		OnMouseDoubleClick(DeferredMessage.Message, wParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
		CaptureMouse(TRUE);
		OnMouseButtonDown(DeferredMessage.Message, wParam);
		UpdateMouseCursor(TRUE);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
		{
			UBOOL bAnyButtonDown = FALSE;
			switch(DeferredMessage.Message)
			{
			case WM_LBUTTONUP: bAnyButtonDown = KeyState(TEXT("KEY_RightMouseButton")) || KeyState(TEXT("KEY_MiddleMouseButton")) || KeyState(TEXT("KEY_ThumbMouseButton")); break;
			case WM_RBUTTONUP: bAnyButtonDown = KeyState(TEXT("KEY_LeftMouseButton")) || KeyState(TEXT("KEY_MiddleMouseButton")) || KeyState(TEXT("KEY_ThumbMouseButton")); break;
			case WM_MBUTTONUP: bAnyButtonDown = KeyState(TEXT("KEY_LeftMouseButton")) || KeyState(TEXT("KEY_RightMouseButton")) || KeyState(TEXT("KEY_ThumbMouseButton")); break;
			case WM_XBUTTONUP:
				{
					UBOOL bXButton1 = (GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON1) ? TRUE : FALSE;	// Is XBUTTON1 currently pressed?
					UBOOL bXButton2 = (GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON2) ? TRUE : FALSE;	// Is XBUTTON2 currently pressed?
					if ( KeyState(TEXT("KEY_ThumbMouseButton")) && !bXButton1 )								// Did XBUTTON1 get released?
					{
						bAnyButtonDown = KeyState(TEXT("KEY_LeftMouseButton")) || KeyState(TEXT("KEY_RightMouseButton")) || KeyState(TEXT("KEY_MiddleMouseButton")) || KeyState(TEXT("KEY_ThumbMouseButton2"));
					}
					else
					{
						bAnyButtonDown = KeyState(TEXT("KEY_LeftMouseButton")) || KeyState(TEXT("KEY_RightMouseButton")) || KeyState(TEXT("KEY_MiddleMouseButton")) || KeyState(TEXT("KEY_ThumbMouseButton"));
					}
				}
				break;
			}
			if( !bAnyButtonDown )
			{
				CaptureMouse(FALSE);
			}
			OnMouseButtonUp(DeferredMessage.Message, wParam);
		}
		break;
	case WM_ENTERSIZEMOVE:
		_Resizing = TRUE;
		break;
	case WM_EXITSIZEMOVE:
		_Resizing = FALSE;
		HandleSizeChange();
		break;
	case WM_SIZE:
		if( !_PerformingSize )
		{
			if( wParam == SIZE_MINIMIZED )
			{
				_Minimized = TRUE;
				_Maximized = FALSE;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				_Minimized = FALSE;
				_Maximized = TRUE;
				HandleSizeChange();
			}
			else if( wParam == SIZE_RESTORED )
			{
				if( _Maximized )
				{
					_Maximized = FALSE;
					HandleSizeChange();
				}
				else if( _Minimized )
				{
					_Minimized = FALSE;
					HandleSizeChange();
				}
				else if( !_Resizing )
				{
					HandleSizeChange();
				}
			}
		}
		break;
	case WM_SIZING:
		{
			int ii = 0;
		}
		break;
	}
}

void FWindowViewport::Tick(FLOAT DeltaTime)
{
}

void FWindowViewport::HandleSizeChange()
{
	if( _ViewportClient )
	{
		RECT ClientRect;
		::GetClientRect(_Window, &ClientRect);

		UINT NewSizeX = ClientRect.right - ClientRect.left;
		UINT NewSizeY = ClientRect.bottom - ClientRect.top;
		if( !IsFullscreen() && (NewSizeX != GetSizeX() || NewSizeY != GetSizeY()) )
		{
			Resize(NewSizeX, NewSizeY, FALSE);

			if( _ViewportClient )
			{
				_ViewportClient->ReceivedFocus(this);
			}
		}
	}
}

void FWindowViewport::OnMouseButtonDown(UINT Message, WPARAM wParam)
{
	UINT KeyCode;
	switch(Message)
	{
	case WM_LBUTTONDOWN:
		KeyCode = VK_LBUTTON;
		break;
	case WM_MBUTTONDOWN:
		KeyCode = VK_MBUTTON;
		break;
	case WM_RBUTTONDOWN:
		KeyCode = VK_RBUTTON;
		break;
	case WM_XBUTTONDOWN:
		KeyCode = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2;
		break;
	default:
		return;
	}

	FString KeyName;
	if( _WindowClient )
	{
		KeyName = _WindowClient->GetVirtualKeyName(KeyCode);
		if( KeyName != TEXT("") )
		{
			_KeyStates[KeyCode] = TRUE;
		}
	}

	if( _ViewportClient )
	{
		::SetFocus(_Window);
		_ViewportClient->InputKey(this, 0, KeyName, IE_Pressed);
	}
}

void FWindowViewport::OnMouseButtonUp(UINT Message, WPARAM wParam)
{
	UINT KeyCode;
	switch(Message)
	{
	case WM_LBUTTONUP:
		KeyCode = VK_LBUTTON;
		break;
	case WM_MBUTTONUP:
		KeyCode = VK_MBUTTON;
		break;
	case WM_RBUTTONUP:
		KeyCode = VK_RBUTTON;
		break;
	case WM_XBUTTONUP:
		KeyCode = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? VK_XBUTTON2 : VK_XBUTTON1;
		break;
	default:
		return;
	}

	_PreventCapture = FALSE;

	FString KeyName;
	if( _WindowClient )
	{
		KeyName = _WindowClient->GetVirtualKeyName(KeyCode);
		if( KeyName != TEXT("") )
		{
			_KeyStates[KeyCode] = FALSE;
		}
	}

	if( _ViewportClient )
	{
		_ViewportClient->InputKey(this, 0, KeyName, IE_Released);
	}
}

void FWindowViewport::OnMouseDoubleClick(UINT Message, WPARAM wParam)
{
	UINT KeyCode;
	switch(Message)
	{
	case WM_LBUTTONDBLCLK:
		KeyCode = VK_LBUTTON;
		break;
	case WM_MBUTTONDBLCLK:
		KeyCode = VK_MBUTTON;
		break;
	case WM_RBUTTONDBLCLK:
		KeyCode = VK_RBUTTON;
		break;
	case WM_XBUTTONDBLCLK:
		KeyCode = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2;
		break;
	default:
		return;
	}

	FString KeyName;
	if( _WindowClient )
	{
		KeyName = _WindowClient->GetVirtualKeyName(KeyCode);
		if( KeyName != TEXT("") )
		{
			_KeyStates[KeyCode] = TRUE;
		}
	}

	if( _ViewportClient )
	{
		_ViewportClient->InputKey(this, 0, KeyName, IE_DoubleClick);
	}
}

/////////////////////////////////////////////////////////////
//  FWindowClient Implementation
////////////////////////////////////////////////////////////
vector<FWindowViewport*> VWindowClient::_Viewports;

VWindowClient::VWindowClient()
{
	// init virtual key table
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_LBUTTON, TEXT("KEY_LeftMouseButton")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_RBUTTON, TEXT("KEY_RightMouseButton")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_MBUTTON, TEXT("KEY_MiddleMouseButton")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_XBUTTON1, TEXT("KEY_ThumbMouseButton")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_XBUTTON2, TEXT("KEY_ThumbMouseButton2")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_BACK, TEXT("KEY_BackSpace")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_TAB, TEXT("KEY_Tab")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_RETURN, TEXT("KEY_Enter")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_PAUSE, TEXT("KEY_Pause")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_CAPITAL, TEXT("KEY_CapsLock")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_ESCAPE, TEXT("KEY_Escape")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_SPACE, TEXT("KEY_SpaceBar")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_PRIOR, TEXT("KEY_PageUp")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NEXT, TEXT("KEY_PageDown")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_END, TEXT("KEY_End")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_HOME, TEXT("KEY_Home")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_LEFT, TEXT("KEY_Left")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_RIGHT, TEXT("KEY_Right")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_UP, TEXT("KEY_Up")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_DOWN, TEXT("KEY_Down")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_INSERT, TEXT("KEY_Insert")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_DELETE, TEXT("KEY_Delete")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(0x30, TEXT("KEY_Zero")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x31, TEXT("KEY_One")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x32, TEXT("KEY_Two")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x33, TEXT("KEY_Three")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x34, TEXT("KEY_Four")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x35, TEXT("KEY_Five")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x36, TEXT("KEY_Six")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x37, TEXT("KEY_Seven")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x38, TEXT("KEY_Eight")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x39, TEXT("KEY_Nine")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(0x41, TEXT("KEY_A")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x42, TEXT("KEY_B")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x43, TEXT("KEY_C")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x44, TEXT("KEY_D")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x45, TEXT("KEY_E")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x46, TEXT("KEY_F")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x47, TEXT("KEY_G")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x48, TEXT("KEY_H")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x49, TEXT("KEY_I")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x4A, TEXT("KEY_J")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x4B, TEXT("KEY_K")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x4C, TEXT("KEY_L")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x4D, TEXT("KEY_M")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x4E, TEXT("KEY_N")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x4F, TEXT("KEY_O")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x50, TEXT("KEY_P")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x51, TEXT("KEY_Q")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x52, TEXT("KEY_R")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x53, TEXT("KEY_S")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x54, TEXT("KEY_T")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x55, TEXT("KEY_U")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x56, TEXT("KEY_V")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x57, TEXT("KEY_W")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x58, TEXT("KEY_X")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x59, TEXT("KEY_Y")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(0x5A, TEXT("KEY_Z")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMPAD0, TEXT("KEY_NumPadZero")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMPAD9, TEXT("KEY_NumPadOne")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMPAD1, TEXT("KEY_NumPadTwo")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMPAD2, TEXT("KEY_NumPadThree")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMPAD3, TEXT("KEY_NumPadFour")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMPAD4, TEXT("KEY_NumPadFive")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMPAD5, TEXT("KEY_NumPadSix")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMPAD6, TEXT("KEY_NumPadSeven")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMPAD7, TEXT("KEY_NumPadEight")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMPAD8, TEXT("KEY_NumPadNine")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_MULTIPLY, TEXT("KEY_Multiply")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_ADD, TEXT("KEY_Add")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_SUBTRACT, TEXT("KEY_Subtract")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_DECIMAL, TEXT("KEY_Decimal")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_DIVIDE, TEXT("KEY_Divide")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F1, TEXT("KEY_F1")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F2, TEXT("KEY_F2")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F3, TEXT("KEY_F3")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F4, TEXT("KEY_F4")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F5, TEXT("KEY_F5")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F6, TEXT("KEY_F6")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F7, TEXT("KEY_F7")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F8, TEXT("KEY_F8")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F9, TEXT("KEY_F9")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F10, TEXT("KEY_F10")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F11, TEXT("KEY_F11")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_F12, TEXT("KEY_F12")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_NUMLOCK, TEXT("KEY_NumLock")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_SCROLL, TEXT("KEY_ScrollLock")));

	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_LSHIFT, TEXT("KEY_LeftShift")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_RSHIFT, TEXT("KEY_RightShift")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_LCONTROL, TEXT("KEY_LeftControl")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_RCONTROL, TEXT("KEY_RightControl")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_LMENU, TEXT("KEY_LeftAlt")));
	_KeyVirtualToName.insert(pair<BYTE, FString>(VK_RMENU, TEXT("KEY_RightAlt")));

	map<BYTE, FString>::iterator FindIt;
	for(UINT KeyIndex = 0; KeyIndex < 256; ++KeyIndex)
	{
		FindIt = _KeyVirtualToName.find(KeyIndex);
		if( FindIt != _KeyVirtualToName.end() )
		{
			_KeyNameToVirtual.insert(pair<FString, BYTE>(FindIt->second, KeyIndex));
		}
	}
}

void VWindowClient::Init()
{
	_WindowClassName = FString(TEXT("VREngine")) + FString(TEXT("VWindowClient"));
}

UBOOL VWindowClient::Exec(const TCHAR* Cmd, FOutputDevice& Ar)
{
	return FALSE;
}

void VWindowClient::Tick(FLOAT DeltaTime)
{
	ProcessDeferredMessages();

	for(INT i = 0; i < (INT)_Viewports.size(); ++i)
	{
		_Viewports.at(i)->Tick(DeltaTime);
	}

	ProcessInput(DeltaTime);

	vector<FWindowViewport*>::iterator it;
	for( it = _Viewports.begin(); it != _Viewports.end(); )
	{
		if( (*it)->GetWindow() == NULL )
		{
			delete *it;
			it = _Viewports.erase(it);
		}
		else
		{
			++it;
		}
	}
}

FViewport* VWindowClient::CreateViewport(FViewportClient* ViewportClient, const TCHAR *InName, UINT SizeX, UINT SizeY, UBOOL bIsFullscreen)
{
	return new FWindowViewport(this, ViewportClient, InName, SizeX, SizeY, bIsFullscreen, NULL);
}

FViewport* VWindowClient::CreateChildViewport(FViewportClient* ViewportClient, const TCHAR *InName, void *InParentWindow, UINT InPosX, UINT InPosY, UINT SizeX, UINT SizeY)
{
	return new FWindowViewport(this, ViewportClient, InName, SizeX, SizeY, FALSE, (HWND)InParentWindow, InPosX, InPosY);
}

void VWindowClient::CloseViewport(FViewport *Viewport)
{
	FWindowViewport* WindowViewport = (FWindowViewport*)Viewport;
	WindowViewport->Destroy();
}

void VWindowClient::DeferMessage(FWindowViewport* Viewport, UINT Message, WPARAM wParam, LPARAM lParam)
{
	FDeferredMessage DeferredMessage;
	DeferredMessage.Viewport = Viewport;
	DeferredMessage.Message = Message;
	DeferredMessage.wParam = wParam;
	DeferredMessage.lParam = lParam;
	DeferredMessage.KeyStates.LeftControl = ::GetKeyState(VK_LCONTROL);
	DeferredMessage.KeyStates.LeftShift = ::GetKeyState(VK_LSHIFT);
	DeferredMessage.KeyStates.RightControl = ::GetKeyState(VK_RCONTROL);
	DeferredMessage.KeyStates.RightShift = ::GetKeyState(VK_RSHIFT);
	DeferredMessage.KeyStates.LeftMenu = ::GetKeyState(VK_LMENU);
	DeferredMessage.KeyStates.RightMenu = ::GetKeyState(VK_RMENU);

	_DeferredMessages.push_back(DeferredMessage);
}

void VWindowClient::ProcessDeferredMessages()
{
	for(INT i = 0; i < (INT)_DeferredMessages.size(); ++i)
	{
		FDeferredMessage DeferredMessage = _DeferredMessages.at(i);
		DeferredMessage.Viewport->ProcessDeferredMessage(DeferredMessage);
	}
	_DeferredMessages.clear();
}

void VWindowClient::ProcessInput(FLOAT DeltaTime)
{
	for(INT ViewportIndex = 0; ViewportIndex < (INT)_Viewports.size(); ++ViewportIndex)
	{
		_Viewports.at(ViewportIndex)->ProcessInput(DeltaTime);
	}
}

LONG VWindowClient::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (Message == WM_POWERBROADCAST)
	{
		switch( wParam )
		{
			case PBT_APMQUERYSUSPEND:
			case PBT_APMQUERYSTANDBY:
				return BROADCAST_QUERY_DENY;
		}
	}

	INT i = 0;
	for( ; i < (INT)_Viewports.size(); ++i )
	{
		if( _Viewports.at(i) && _Viewports.at(i)->GetWindow() == hWnd )
		{
			break;
		}
	}

	if( i == (INT)_Viewports.size() )
	{
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}
	else
	{
		return _Viewports.at(i)->ViewportWndProc(Message, wParam, lParam);
	}
}

void VWindowClient::Destroy()
{
	for(INT i = 0; i < (INT)_Viewports.size(); ++i)
	{
		_Viewports.at(i)->Destroy();
	}
	_Viewports.clear();

	SetCapture(NULL);
	ClipCursor(NULL);
}

void VWindowClient::ShutdownAfterError()
{
	SetCapture(NULL);
	ClipCursor(NULL);
	while(ShowCursor(TRUE) < 0);

	for(INT i = 0; i < (INT)_Viewports.size(); ++i)
	{
		_Viewports.at(i)->ShutdownAfterError();
	}
}

FString VWindowClient::GetVirtualKeyName(INT KeyCode) const
{
	if( KeyCode < 255 )
	{
		map<BYTE, FString>::const_iterator it;
		it = _KeyVirtualToName.find(KeyCode);
		if( it != _KeyVirtualToName.end() )
		{
			return it->second;
		}
	}
	return TEXT("");
}

const FWindowViewport* VWindowClient::FindViewport(const FWindowViewport *Viewport)
{
	vector<FWindowViewport*>::const_iterator it = find(_Viewports.begin(), _Viewports.end(), Viewport);
	if( it != _Viewports.end() )
	{
		return *it;
	}
	return NULL;
}