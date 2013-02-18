#ifndef VRI3_D3D9DRV_D3D9VIEWPORT_H_
#define VRI3_D3D9DRV_D3D9VIEWPORT_H_

class FD3D9Viewport : public FRefCountedObject
{
public:
	FD3D9Viewport(class FD3D9DynamicDrv* D3DDrv, void* WindowHandle, UINT SizeX, UINT SizeY, UBOOL IsFullscreen);
	~FD3D9Viewport();

	void Resize(UINT SizeX, UINT SizeY, UBOOL bFullscreen);

	void* GetWindowHandle() const { return _WindowHandle; }
	UINT  GetSizeX() const { return _SizeX; }
	UINT  GetSizeY() const { return _SizeY; }
	UBOOL IsFullscreen() const { return _bFullscreen; }

private:
	FD3D9DynamicDrv*	_D3DDrv;
	void*				_WindowHandle;
	UINT				_SizeX;
	UINT				_SizeY;
	UBOOL				_bFullscreen;
};

typedef TRefCountPtr<FD3D9Viewport>		FD3D9ViewportPtr;

#endif