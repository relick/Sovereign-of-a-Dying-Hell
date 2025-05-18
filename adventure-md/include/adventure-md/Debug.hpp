#pragma once

#include "adventure-md/Declare.hpp"

#include <csignal>

#if DEBUG
inline u16 volatile s_debugBreakpointHook = 0;
inline void DebugBreakpointHook()
{
	s_debugBreakpointHook = 1;
}
#define Error(...) kprintf(__VA_ARGS__); DebugBreakpointHook()
#define Assert(CHECK, ...) if (!(CHECK)) { Error(__VA_ARGS__); }
#else
#define Error(...)
#define Assert(CHECK, ...)
#endif