#pragma once

#include "Declare.hpp"

#include <genesis.h>

namespace System
{
inline void SetColour_CTRL(u16 i_index)
{
	*((vu32 *)VDP_CTRL_PORT) = VDP_WRITE_CRAM_ADDR((u32)(i_index * sizeof(u16)));
}
inline void SetColour_DATA(u16 i_value)
{
	*((vu16 *)VDP_DATA_PORT) = i_value;
}

// Stripped down from DMA_doDmaFast to specifically update palette mid-frame, at the cost of sprites
template <During t_When, u16 t_PalNum, u8 t_Length = 16>
inline void SetPalette_Fast(u16 const *i_pal)
{
	VDP_setAutoInc(sizeof(u16));

	vu16 *pw = (vu16 *)VDP_CTRL_PORT;

	// Setup DMA length (in word here)
	*pw = 0x9300 + (t_Length & 0xff);
	*pw = 0x9400 + ((t_Length >> 8) & 0xff);

	u32 fromAddr = (u32)i_pal;

	// Setup DMA address
	fromAddr >>= 1;
	*pw = 0x9500 + (fromAddr & 0xff);
	fromAddr >>= 8;
	*pw = 0x9600 + (fromAddr & 0xff);
	fromAddr >>= 8;
	*pw = 0x9700 + (fromAddr & 0x7f);

	// force storing DMA command into memory
	vu32 cmdbuf[1];
	cmdbuf[0] = VDP_DMA_CRAM_ADDR((u32)(t_PalNum * 16) * sizeof(u16));

	// then force issuing DMA from memory word operand
	u16 *cmdbufp = (u16 *)cmdbuf;

	if constexpr (t_When == During::VBlank)
	{
		// first command word, typically run before Z80 halt if not mid frame
		*pw = *cmdbufp++;
	}

#if (HALT_Z80_ON_DMA != 0)
	bool busTaken = Z80_isBusTaken();
	// disable Z80 before processing DMA
	if (!busTaken)
	{
		Z80_requestBus(FALSE);
	}
#endif // HALT_Z80_ON_DMA

	if constexpr (t_When == During::HBlank)
	{
		VDP_setEnable(FALSE);

		// Haven't checked 140 on real HW, but 150 was a little too late.
		while (GET_HCOUNTER < 140)
		{
		}

		// first command word, delayed until after wait when mid frame
		*pw = *cmdbufp++;
	}

	// trigger DMA (second word command wrote from memory to avoid possible failure on some MD)
	*pw = *cmdbufp;

	if constexpr (t_When == During::HBlank)
	{
		VDP_setEnable(TRUE);
	}

#if (HALT_Z80_ON_DMA != 0)
	// re-enable Z80 after DMA
	if (!busTaken)
	{
		Z80_releaseBus();
	}
#endif // HALT_Z80_ON_DMA
}
}

namespace Game
{

// All code here works on the basis text frame is using PAL0
inline u16 const* s_textFramePalette;
inline void SetTextFramePalette(Palette const& i_pal) { s_textFramePalette = i_pal.data; }

// Requires 1 line prior to prepare colours to start on t_LineToShow
template <u8 t_LineToShow, u8 t_BaseLine = t_LineToShow - 1, u8 t_Line = t_BaseLine>
HINTERRUPT_CALLBACK HInt_TextFrame()
{
	if (GET_VCOUNTER == t_Line)
	{
		if constexpr (t_Line - t_BaseLine < 6)
		{
			SYS_setHIntCallback(&HInt_TextFrame<t_LineToShow, t_BaseLine, t_Line + 1>);
		}
		System::SetColour_CTRL(t_Line - t_BaseLine);
		// 136 seems enough on both emulator and real HW
		while (GET_HCOUNTER < 136)
		{
		}
		System::SetColour_DATA(s_textFramePalette[t_Line - t_BaseLine]);
		if constexpr (t_Line - t_BaseLine < 5)
		{
			System::SetColour_CTRL(t_Line - t_BaseLine + 1);
			System::SetColour_DATA(s_textFramePalette[t_Line - t_BaseLine + 1]);
			System::SetColour_CTRL(t_Line - t_BaseLine + 2);
			System::SetColour_DATA(s_textFramePalette[t_Line - t_BaseLine + 2]);
		}
		else
		{
			// Reset for next frame
			SYS_setHIntCallback(&HInt_TextFrame<t_LineToShow>);
		}
	}
}

// Requires 2 lines for DMA setup and execution, to show changes on t_LineToShow
template <u8 t_LineToShow, u8 t_Line = t_LineToShow - 2>
HINTERRUPT_CALLBACK HInt_TextFrameDMA()
{
	if (GET_VCOUNTER == t_Line)
	{
		System::SetPalette_Fast<During::HBlank, PAL0>(s_textFramePalette);
	}
}

}