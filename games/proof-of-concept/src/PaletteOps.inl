#pragma once

#include "PaletteOps.hpp"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
namespace Detail
{

//------------------------------------------------------------------------------
inline void WaitForStartOfNextLine()
{
	// Wait for end of this line.
	while (GET_HCOUNTER < 156)
	{
	}

	// Wait for start of next line.
	while (GET_HCOUNTER >= 156)
	{
	}
}

//------------------------------------------------------------------------------
inline bool PauseZ80()
{
#if (HALT_Z80_ON_DMA != 0)
	if (!Z80_isBusTaken())
	{
		Z80_requestBus(FALSE);
		return true;
	}
#endif // HALT_Z80_ON_DMA
	return false;
}

//------------------------------------------------------------------------------
inline void StartZ80(bool i_z80Stopped)
{
#if (HALT_Z80_ON_DMA != 0)
	// re-enable Z80 after DMA
	if (i_z80Stopped)
	{
		Z80_releaseBus();
	}
#endif // HALT_Z80_ON_DMA
}

//------------------------------------------------------------------------------
inline void SetupPaletteDMA(u16 const* i_pal)
{
	vu16 *pw = reinterpret_cast<vu16 *>(VDP_CTRL_PORT);

	// Set VDP auto inc
	*pw = 0x8F00 | sizeof(u16);

	// Setup DMA length (in word here)
	// 16 is the length (hard coded for palette)
	*pw = 0x9300 + (16 & 0xff);
	*pw = 0x9400 + ((16 >> 8) & 0xff);

	u32 fromAddr = (u32)(i_pal);

	// Setup DMA address
	fromAddr >>= 1;
	*pw = 0x9500 + (fromAddr & 0xff);
	fromAddr >>= 8;
	*pw = 0x9600 + (fromAddr & 0xff);
	fromAddr >>= 8;
	*pw = 0x9700 + (fromAddr & 0x7f);
}

//------------------------------------------------------------------------------
template<During t_When, u16 t_PalNum>
void TriggerPaletteDMA()
{
	// Various macros to make the asm a little bit more readable
	#define CONCAT_INST(INST, SRC, DEST) INST " " SRC ", " DEST
	#define lea(ADDR, DEST_DREG) asm(CONCAT_INST("lea", "%a0", "%" DEST_DREG)::"i"(ADDR))
	#define pushl_val(VAL) asm(CONCAT_INST("move.l", "%0", "-(%%sp)")::"i"(VAL))
	#define pushl_reg(SRC_REG) asm(CONCAT_INST("move.l", SRC_REG, "-(%sp)"))
	#define movew_val_reg(VAL, DEST_DREG) asm(CONCAT_INST("move.w", "%0", "%" DEST_DREG)::"i"(VAL))
	#define movel_val_reg(VAL, DEST_DREG) asm(CONCAT_INST("move.l", "%0", "%" DEST_DREG)::"i"(VAL))
	#define movew_reg_mem(SRC_REG, DEST_AREG) asm(CONCAT_INST("move.w", SRC_REG, "(" DEST_AREG ")"))
	#define movel_reg_mem(SRC_REG, DEST_AREG) asm(CONCAT_INST("move.l", SRC_REG, "(" DEST_AREG ")"))
	#define popl_mem(DEST_AREG) asm(CONCAT_INST("move.l", "(%sp)+", "(" DEST_AREG ")"))
	#define popl_reg(DEST_DREG) asm(CONCAT_INST("move.l", "(%sp)+", DEST_DREG))

	// Setup for timing critical section
	{
		// gcc prefers registers d0-d2/a0-a2 strongly; and seems to avoid using d3+/a3+
		// Going with 3 and 4 should be a pretty safe bet that gcc won't interfere
		// Note: if it's an issue, just replace the while loop with more manual asm. Then everything between the pushes and pops is completely controlled by us.

		// Save registers to respect ABI
		//pushl_reg("%d5"); // d5 will be used to store the DMA command
		pushl_reg("%d4"); // d4 will be used to store the command for enabling VDP
		pushl_reg("%d3"); // d3 will be used to store the command for disabling VDP
		pushl_reg("%a3"); // a3 will be used to store the VDP ctrl port address

		// Load values
		movew_val_reg(0x8100 | 0x74, "%d4"); // enable VDP command
		movew_val_reg(0x8100 | 0x34, "%d3"); // disable VDP command
		lea(VDP_CTRL_PORT, "%a3");

		// Put DMA command into memory (apparently, popping from the stack is the fastest way to deliver the command)
		pushl_val(VDP_DMA_CRAM_ADDR((u32)(t_PalNum * 16 * sizeof(u16))));
		// Newly added: the above was a lie, doing it from register is faster.
		//movel_val_reg(VDP_DMA_CRAM_ADDR((u32)(t_PalNum * 16 * sizeof(u16))), "%d5");
	}
	
	// Wait until precise point near end of line (160 = end of line, but latency means we need to stop a little earlier)
	// 150 is as early as looks good on Blastem. TODO: test on real HW. Should be room to push a little bit later
	while (GET_HCOUNTER < 150) {}

	// Timing critical section!!
	// Note that it's only necessary to disable VDP when in the active display. But no need to do anything else differently otherwise
	{
		if constexpr (t_When == During::Active)
		{
			// Disable VDP
			movew_reg_mem("%d3", "%a3");
		}

		// Initiate DMA
		popl_mem("%a3");
		//movel_reg_mem("%d5", "%a3");

		if constexpr (t_When == During::Active)
		{
			// Enable VDP
			movew_reg_mem("%d4", "%a3");
		}
	}

	// Cleanup for timing critical section
	{
		// Restore registers to respect ABI
		popl_reg("%a3");
		popl_reg("%d3");
		popl_reg("%d4");
		//popl_reg("%d5");
	}

	// Clean away macros
	#undef CONCAT_INST
	#undef lea
	#undef pushl_val
	#undef pushl_reg
	#undef movew_val_reg
	#undef movew_reg_mem
	#undef popl_mem
	#undef popl_reg
}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
namespace System
{

//------------------------------------------------------------------------------
void SetColour_CTRL
(
	u16 i_index
)
{
	*((vu32 *)VDP_CTRL_PORT) = VDP_WRITE_CRAM_ADDR((u32)(i_index * sizeof(u16)));
}

//------------------------------------------------------------------------------
void SetColour_DATA
(
	u16 i_value
)
{
	*((vu16 *)VDP_DATA_PORT) = i_value;
}

//------------------------------------------------------------------------------
// Stripped down from DMA_doDmaFast to specifically update palette mid-frame, at the cost of sprites
//------------------------------------------------------------------------------
template <During t_When, u16 t_PalNum>
void SetPalette_Fast
(
	u16 const* i_pal
)
{
	Detail::SetupPaletteDMA(i_pal);

	Detail::WaitForStartOfNextLine();

	// We can't wait for the timing critical section to disable Z80, so we'll do it here
	// which will potentially cost us a line's worth of Z80 but no more
	bool const z80Stopped = Detail::PauseZ80();

	Detail::TriggerPaletteDMA<t_When, t_PalNum>();

	Detail::StartZ80(z80Stopped);
}

//------------------------------------------------------------------------------
// Should be able to do 2 palettes in 3 lines instead of 4 with this
//------------------------------------------------------------------------------
template <During t_When, u16 t_PalNum0, u16 t_PalNum1>
void Set2Palette_Fast
(
	u16 const* i_pal0,
	u16 const* i_pal1
)
{
	Detail::SetupPaletteDMA(i_pal0);

	// Because of HInt lateness, it's more reliable to wait for start of next line the first time we enter the function
	Detail::WaitForStartOfNextLine();

	bool const z80Stopped = Detail::PauseZ80();

	Detail::TriggerPaletteDMA<t_When, t_PalNum0>();

	Detail::SetupPaletteDMA(i_pal1);
	Detail::TriggerPaletteDMA<t_When, t_PalNum1>();

	Detail::StartZ80(z80Stopped);
}

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
namespace Palettes
{

//------------------------------------------------------------------------------
template<u8 t_PalLen>
void MinusOne
(
	u16* i_dstPal,
	u16 const* i_srcPal
)
{
	for (u8 i = 0; i < t_PalLen; ++i)
	{
		u8 const r = (i_srcPal[i] & VDPPALETTE_REDMASK) >> VDPPALETTE_REDSFT;
		u8 const g = (i_srcPal[i] & VDPPALETTE_GREENMASK) >> VDPPALETTE_GREENSFT;
		u8 const b = (i_srcPal[i] & VDPPALETTE_BLUEMASK) >> VDPPALETTE_BLUESFT;
		i_dstPal[i] = RGB3_3_3_TO_VDPCOLOR(
			(r > 0) ? r - 1 : 0,
			(g > 0) ? g - 1 : 0,
			(b > 0) ? b - 1 : 0
		);
	}
}

//------------------------------------------------------------------------------
template<u8 t_PalLen>
void Halve
(
	u16* i_dstPal,
	u16 const* i_srcPal
)
{
	for (u8 i = 0; i < t_PalLen; ++i)
	{
		i_dstPal[i] = RGB3_3_3_TO_VDPCOLOR(
			(i_srcPal[i] & VDPPALETTE_REDMASK) >> (VDPPALETTE_REDSFT + 1),
			(i_srcPal[i] & VDPPALETTE_GREENMASK) >> (VDPPALETTE_GREENSFT + 1),
			(i_srcPal[i] & VDPPALETTE_BLUEMASK) >> (VDPPALETTE_BLUESFT + 1)
		);
	}
}

//------------------------------------------------------------------------------
template<u8 t_PalLen>
void Tint
(
	u16* i_dstPal,
	u16 const* i_srcPal,
	u8 i_r,
	u8 i_g,
	u8 i_b
)
{
	for (u8 i = 0; i < t_PalLen; ++i)
	{
		i_dstPal[i] = RGB3_3_3_TO_VDPCOLOR(
			((i_srcPal[i] & VDPPALETTE_REDMASK) + i_r) >> (VDPPALETTE_REDSFT + 1),
			((i_srcPal[i] & VDPPALETTE_GREENMASK) + i_g) >> (VDPPALETTE_GREENSFT + 1),
			((i_srcPal[i] & VDPPALETTE_BLUEMASK) + i_b) >> (VDPPALETTE_BLUESFT + 1)
		);
	}
}

}