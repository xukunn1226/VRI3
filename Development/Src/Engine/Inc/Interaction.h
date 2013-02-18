#ifndef VRI3_ENGINE_INTERACTION_H__
#define VRI3_ENGINE_INTERACTION_H__

class VInteraction : public VObject
{
	DECLARE_CLASS(VInteraction, VObject);

public:
	VInteraction() {}

	virtual void Init() {};

	virtual void Tick(FLOAT DeltaTime) {};

	virtual UBOOL InputKey(FViewport* Viewport, INT ControllerId, const FString& Key, EInputEvent Event) { return FALSE; }

	virtual UBOOL InputAxis(FViewport* Viewport, INT ControllerId, const FString& Key, FLOAT Delta, FLOAT DeltaTime) { return FALSE; }

	virtual UBOOL InputChar(FViewport* Viewport, INT ControllerId, TCHAR Character) { return FALSE; }
};
SmartPointer(VInteraction);

#endif