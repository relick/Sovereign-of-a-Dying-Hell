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
	vu16 *pw = reinterpret_cast<vu16 *>(VDP_CTRL_PORT);

	// Set VDP auto inc
	*pw = 0x8F00 | sizeof(u16);

	// Setup DMA length (in word here)
	*pw = 0x9300 + (t_Length & 0xff);
	*pw = 0x9400 + ((t_Length >> 8) & 0xff);

	u32 fromAddr = (u32)(i_pal);

	// Setup DMA address
	fromAddr >>= 1;
	*pw = 0x9500 + (fromAddr & 0xff);
	fromAddr >>= 8;
	*pw = 0x9600 + (fromAddr & 0xff);
	fromAddr >>= 8;
	*pw = 0x9700 + (fromAddr & 0x7f);

	// force storing DMA command into memory
	//vu32 cmdbuf[1];
	//cmdbuf[0] = VDP_DMA_CRAM_ADDR((u32)(t_PalNum * 16 * sizeof(u16)));

	// then force issuing DMA from memory word operand
	//u16 *cmdbufp = (u16 *)cmdbuf;

#if (HALT_Z80_ON_DMA != 0)
	bool busTaken = Z80_isBusTaken();
	// disable Z80 before processing DMA
	if (!busTaken)
	{
		Z80_requestBus(FALSE);
	}
#endif // HALT_Z80_ON_DMA

	while (GET_HCOUNTER < 156)
	{
	}

	while (GET_HCOUNTER >= 156)
	{
	}

#define CONCAT_INST(INST, SRC, DEST) INST " " SRC ", " DEST
#define ASM_lea(ADDR, DEST_DREG) asm(CONCAT_INST("lea", "%a0", "%" DEST_DREG)::"i"(ADDR))
#define ASM_pushl_val(VAL) asm(CONCAT_INST("move.l", "%0", "-(%%sp)")::"i"(VAL))
#define ASM_pushl_reg(SRC_REG) asm(CONCAT_INST("move.l", SRC_REG, "-(%sp)"))
#define ASM_movew_val_reg(VAL, DEST_DREG) asm(CONCAT_INST("move.w", "%0", "%" DEST_DREG)::"i"(VAL))
#define ASM_movew_reg_mem(SRC_REG, DEST_AREG) asm(CONCAT_INST("move.w", SRC_REG, "(" DEST_AREG ")"))
#define ASM_popl_mem(DEST_AREG) asm(CONCAT_INST("move.l", "(%sp)+", "(" DEST_AREG ")"))
#define ASM_popl_reg(DEST_DREG) asm(CONCAT_INST("move.l", "(%sp)+", DEST_DREG))

	// Registers 3-4 seem to basically never be used by gcc.
	ASM_pushl_reg("%d4");
	ASM_pushl_reg("%d3");
	ASM_pushl_reg("%a3");
	ASM_movew_val_reg(0x8100 | 0x74, "%d4"); // enableVDP
	ASM_movew_val_reg(0x8100 | 0x34, "%d3"); // disableVDP
	ASM_lea(VDP_CTRL_PORT, "%a3");
	ASM_pushl_val(VDP_DMA_CRAM_ADDR((u32)(t_PalNum * 16 * sizeof(u16))));

	// disableVDP = 0x8100 | 0x34;
	// enableVDP = 0x8100 | 0x74;
	// asm("move.l %d4,-(%sp)");
	// asm("move.l %d3,-(%sp)");
	// asm("move.l %a3,-(%sp)");
	// asm("move.w %0,%%d3\n\t" ::"i"(0x8100 | 0x34));
	// asm("move.w #-32396,%d4");
	// asm("lea 12582916,%a3");
	// asm("move.l #-1073741696,-(%sp)");

	// 146 is earliest in blastem when the display is turned off for a whole line beforehand.
	while (GET_HCOUNTER < 150)
	{
	}

	if constexpr (t_When == During::HBlank)
	{
		// Disable VDP
		//*pw = disableVDP;
		//asm("move.w %d3,(%a3)");
		ASM_movew_reg_mem("%d3", "%a3");
	}

	// first command word
	//*pw = cmdbufp[0];
	//asm("move.w (%sp)+,(%a3)");

	// trigger DMA (second word command wrote from memory to avoid possible failure on some MD)
	//*pw = cmdbufp[1];
	//asm("move.w (%sp)+,(%a3)");

	//*pw32 = VDP_DMA_CRAM_ADDR((u32)(t_PalNum * 16 * sizeof(u16)));
	//asm("move.l (%sp)+,(%a3)");
	ASM_popl_mem("%a3");

	if constexpr (t_When == During::HBlank)
	{
		// Enable VDP
		//*pw = enableVDP;
		//asm("move.w %d4,(%a3)");
		ASM_movew_reg_mem("%d4", "%a3");
	}

	// asm("move.l (%sp)+,%a3");
	// asm("move.l (%sp)+,%d3");
	// asm("move.l (%sp)+,%d4");
	ASM_popl_reg("%a3");
	ASM_popl_reg("%d3");
	ASM_popl_reg("%d4");

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
		// 136 seems perfect on both emulator and real HW
		// 134 was too soon on real HW (flickering on right side)
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
using CallbackFn = void();
template <u8 t_LineToShow, CallbackFn* T_Callback, u8 t_Line = t_LineToShow - 2>
HINTERRUPT_CALLBACK HInt_TextFrameDMA()
{
	if (GET_VCOUNTER == t_Line)
	{
		System::SetPalette_Fast<During::HBlank, PAL0>(s_textFramePalette);
		T_Callback();
	}
}

}