#ifndef VRI3_CORE_STATS_H_
#define VRI3_CORE_STATS_H_

const INT STAT_HISTORY_SIZE = 60;

template<class T> inline T InitialStatValue() { return 0; }
template<> inline FLOAT InitialStatValue<FLOAT>() { return 0.f; }

template<class T> class TStatsHistory
{
public:
	TStatsHistory()
		: _HistoryCount(0), _Total(0.0), _Max(0.0)
	{
		for(DWORD Index = 0; Index < STAT_HISTORY_SIZE; ++Index)
		{
			_History[Index] = InitialStatValue<T>();
		}
	}

	inline DOUBLE GetAverage() const
	{
		return _Total / (DOUBLE)STAT_HISTORY_SIZE;
	}

	inline DOUBLE GetMax() const
	{
		return _Max;
	}

	inline void AddToHistory(T Value)
	{
		DWORD Index = (_HistoryCount++) % STAT_HISTORY_SIZE;

		_Total += (DOUBLE)Value - (DOUBLE)_History[Index];
		_History[Index] = Value;

		if( (DOUBLE)Value > _Max )
		{
			_Max = Value;
		}
	}
private:
	T		_History[STAT_HISTORY_SIZE];
	DWORD	_HistoryCount;
	DOUBLE	_Total;
	DOUBLE	_Max;
};

class FFPSCounter : public TStatsHistory<DOUBLE>
{
public:
	FFPSCounter()
		: _LastTime(0.0), _DeltaTime(0.0)
	{}

	inline void Update(DOUBLE NewTime)
	{
		_DeltaTime = NewTime - _LastTime;
		_LastTime = NewTime;
		AddToHistory(_DeltaTime);
	}

	inline DOUBLE GetDeltaTime() { return _DeltaTime; }

private:
	DOUBLE	_LastTime;
	DOUBLE	_DeltaTime;
};
extern FFPSCounter	GFPSCounter;

#endif