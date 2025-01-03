#pragma once

#include <types.h>

// Useful variables for handling region differences
namespace System
{
// These access the registers directly
bool IsNTSC();
bool IsJapan();

// Run once
void StaticInit_Version();

inline u16 s_timerStep = 5;
}

// Use these for a region-free life
#define USE_REGIONFREE_TIMERS 0
#define FORCE_PAL 0
#define FORCE_NTSC 0

#if USE_REGIONFREE_TIMERS

// TODO: this is going to be really slow because of all the mults and divs.
// There is probably a better way but just need to be careful on how to potentially do it at runtime
inline constexpr u16 TimerStep()
{
#if FORCE_PAL
	return 6;
#elif FORCE_NTSC
	return 5;
#else
	return System::s_timerStep;
#endif
}
inline constexpr u16 RealToTimer(u16 i_realStep) { return i_realStep * TimerStep(); }
inline constexpr u16 TimerToReal(u16 i_timerStep) { return i_timerStep / TimerStep(); }

#else

inline constexpr u16 TimerStep() { return 1; }
inline constexpr u16 RealToTimer(u16 i_realStep) { return i_realStep; }
inline constexpr u16 TimerToReal(u16 i_timerStep) { return i_timerStep; }

#endif