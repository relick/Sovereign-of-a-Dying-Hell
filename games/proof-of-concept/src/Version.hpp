#pragma once

#include <types.h>
#include <maths.h>

// Useful variables for handling region differences
namespace System
{
// These access the registers directly
bool IsNTSC();
bool IsJapan();

// Run once
void StaticInit_Version();

inline f16 s_timerStep = FIX16(0.02);
}

// Use these for a region-free life
// FrameStep = the timing of 1 frame, so can make timers in f16 in numbers of seconds
#define USE_REGIONFREE_TIMERS 1
#define FORCE_PAL 0
#define FORCE_NTSC 0

#if USE_REGIONFREE_TIMERS

inline constexpr f16 FrameStep()
{
#if FORCE_PAL
	return FIX16(0.02);
#elif FORCE_NTSC
	return FIX16(0.0166666666666);
#else
	return System::s_timerStep;
#endif
}

#else

inline constexpr f16 FrameStep() { return FIX16(0.02); }

#endif