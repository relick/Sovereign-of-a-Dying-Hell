#include "Version.hpp"

namespace System
{
bool IsNTSC()
{
	u8 const version = *(u8*)0xA10001;
	return (version & (1 << 6)) == 0;
}
bool IsJapan()
{
	u8 const version = *(u8*)0xA10001;
	return (version & (1 << 7)) == 0;
}

void StaticInit_Version()
{
	s_timerStep = IsNTSC() ? FIX16(0.0166666666666) : FIX16(0.02);
	s_fps = IsNTSC() ? 60 : 50;
}

}