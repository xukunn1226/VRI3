#include "CorePCH.h"

/*-----------------------------------------------------------------------------
	FOutputDevice implementation.
-----------------------------------------------------------------------------*/
void FOutputDevice::Log(const TCHAR* log)
{
	Serialize(log, NAME_Log);
}

void FOutputDevice::Log(EName Event, const TCHAR *log)
{
	Serialize(log, Event);
}

void FOutputDevice::Log(const FString& log)
{
	Serialize(log.c_str(), NAME_Log);
}

void FOutputDevice::Log(EName Event, const FString& log)
{
	Serialize(log.c_str(), Event);
}

void FOutputDevice::Logf(const TCHAR* log, ...)
{
	TCHAR TempStr[MAX_SPRINTF];
	GET_VARARGS( TempStr, ARRAY_COUNT(TempStr), ARRAY_COUNT(TempStr)-1, log, log );

	Serialize(TempStr, NAME_Log);
}

void FOutputDevice::Logf(EName Event, const TCHAR* log, ...)
{
	TCHAR TempStr[MAX_SPRINTF];
	GET_VARARGS( TempStr, ARRAY_COUNT(TempStr), ARRAY_COUNT(TempStr)-1, log, log );

	Serialize(TempStr, Event);
}