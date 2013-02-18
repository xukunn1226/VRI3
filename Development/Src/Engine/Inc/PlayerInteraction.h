#ifndef VRI3_ENGINE_PLAYERINTERACTION_H__
#define VRI3_ENGINE_PLAYERINTERACTION_H__

class VPlayerInteraction : public VInteraction
{
	DECLARE_CLASS(VPlayerInteraction, VInteraction);

public:
	VPlayerInteraction() {};

	virtual void Init() {};

	virtual void Tick(FLOAT DeltaTime) {};

	virtual UBOOL InputKey(FViewport* Viewport, INT ControllerId, const FString& Key, EInputEvent Event) { return FALSE; }

	virtual UBOOL InputAxis(FViewport* Viewport, INT ControllerId, const FString& Key, FLOAT Delta, FLOAT DeltaTime) { return FALSE; }

	virtual UBOOL InputChar(FViewport* Viewport, INT ControllerId, TCHAR Character) { return FALSE; }
};
SmartPointer(VPlayerInteraction);

#endif