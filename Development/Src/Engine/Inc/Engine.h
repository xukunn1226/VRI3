#ifndef VRI3_ENGINE_ENGINE_H_
#define VRI3_ENGINE_ENGINE_H_

/**
 * the base class of the engine application, include global object class
 * e.g. Font, console, 
 */
class VEngine : public VObject
{
	DECLARE_CLASS(VEngine, VObject);

public:
	VEngine();

	virtual ~VEngine();

	virtual void Init();

	virtual void PreExit();

	virtual void Tick(FLOAT DeltaTime);

	void RedrawViewports(UBOOL bShouldPresent = TRUE);

	virtual void InitStaticData();

	/*
	 * clean up the viewport
	 */
	void CleanupGameViewport();

	virtual FViewport* GetAViewport() const;

	virtual VClient* GetClient() const;

private:
	//Font			_SmallFont;
	//FString			_SmallFontName;

	//Font			_MediumFont;
	//FString			_MediumFontName;

	//Font			_LargeFont;
	//FString			_LargeFontName;

	VGameViewportClientPtr	_GameViewport;
	const VClass*					_GameViewportClass;

	VClientPtr				_GameClient;
	const VClass*					_GameClientClass;
	//FName					_GameClientClassName;
};
SmartPointer(VEngine);

extern VEnginePtr	GEngine;

#endif