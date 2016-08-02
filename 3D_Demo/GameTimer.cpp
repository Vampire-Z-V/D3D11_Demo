#include "GameTimer.h"
#include <Windows.h>


GameTimer::GameTimer()
:	mSecondsPerCount(0.0),
	mDeltaTime(-1.0),
	mBaseTime(0),
	mPausedTime(0),
	mStopTime(0),
	mPrevTime(0),
	mCurrTime(0),
	mStopped(false)
{
	__int64 countPerSeconds;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countPerSeconds);
	mSecondsPerCount = 1.0 / (double)countPerSeconds;
}

float GameTimer::TotalTime() const
{
	if (mStopped)
	{
		return (float)( ( ( mStopTime - mPausedTime ) - mBaseTime )*mSecondsPerCount );
	}
	else
	{
		return (float)( ( ( mCurrTime - mPausedTime ) - mBaseTime )*mSecondsPerCount );
	}
}

float GameTimer::DeltaTime() const
{
	return (float)mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mPausedTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	if (mStopped)
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		mPausedTime += ( startTime - mStopTime );
		mStopTime = 0;
		mPrevTime = startTime;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		__int64 stopTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&stopTime);

		mStopTime = stopTime;
		mStopped = true;
	}
}

void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
	}
	else
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mCurrTime = currTime;
		mDeltaTime = ( mCurrTime - mPrevTime )*mSecondsPerCount;
		mPrevTime = mCurrTime;

		if (mDeltaTime < 0.0)
		{
			mDeltaTime = 0.0;
		}
	}
}


