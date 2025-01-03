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
	s_timerStep = IsNTSC() ? 5 : 6;
}

}