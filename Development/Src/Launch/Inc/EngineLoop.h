#ifndef VRI3_ENGINELOOP_H_
#define VRI3_ENGINELOOP_H_

class FEngineLoop
{
public:
	INT PreInit(const TCHAR* strCmdLine);

	INT Init();

	void Exit();

	void Tick();

private:
	DOUBLE		TotalTickTime;
	DOUBLE		MaxTickTime;
};

#endif