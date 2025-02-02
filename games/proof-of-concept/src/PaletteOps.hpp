#pragma once

#include "Declare.hpp"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
namespace System
{

//------------------------------------------------------------------------------
inline void SetColour_CTRL(u16 i_index);
inline void SetColour_DATA(u16 i_value);

//------------------------------------------------------------------------------
// Stripped down from DMA_doDmaFast to specifically update palette mid-frame, at the cost of sprites
//------------------------------------------------------------------------------
template <During t_When, u16 t_PalNum>
inline void SetPalette_Fast(u16 const *i_pal);

//------------------------------------------------------------------------------
// Should be able to do 2 palettes in 3 lines instead of 4 with this
//------------------------------------------------------------------------------
template <During t_When, u16 t_PalNum0, u16 t_PalNum1>
inline void Set2Palette_Fast(u16 const *i_pal0, u16 const *i_pal1);

}

namespace Palettes
{

//------------------------------------------------------------------------------
// Mini-helper for 3-bit RGB values, split into separate 1-byte components for manipulation (but still ultimately 3-bit colour)
//------------------------------------------------------------------------------
struct RGB3Colour
{
	u8 r; u8 g; u8 b;

	constexpr RGB3Colour(u8 i_r, u8 i_g, u8 i_b)
		: r{ i_r }, g{ i_g }, b{ i_b }
	{}
	constexpr RGB3Colour(u16 i_vdpColour)
		: r{ static_cast<u8>((i_vdpColour & VDPPALETTE_REDMASK) >> VDPPALETTE_REDSFT) }
		, g{ static_cast<u8>((i_vdpColour & VDPPALETTE_GREENMASK) >> VDPPALETTE_GREENSFT) }
		, b{ static_cast<u8>((i_vdpColour & VDPPALETTE_BLUEMASK) >> VDPPALETTE_BLUESFT) }
	{}
	constexpr RGB3Colour(u32 i_24bit)
		: RGB3Colour{ static_cast<u16>(RGB24_TO_VDPCOLOR(i_24bit)) }
	{}
	constexpr u16 ToVDPColour() const
	{
		return RGB3_3_3_TO_VDPCOLOR(r, g, b);
	}
	constexpr RGB3Colour& operator+=(RGB3Colour const& i_o)
	{
		r += i_o.r; g += i_o.g; b += i_o.b; return *this;
	}
	constexpr RGB3Colour& operator-=(RGB3Colour const& i_o)
	{
		r = (r > i_o.r) ? (r - i_o.r) : 0;
		g = (g > i_o.g) ? (g - i_o.g) : 0;
		b = (b > i_o.b) ? (b - i_o.b) : 0;
		return *this;
	}
	constexpr RGB3Colour& operator>>=(u8 i_shift)
	{
		r >>= i_shift; g >>= i_shift; b >>= i_shift; return *this;
	}
};

//------------------------------------------------------------------------------
template<u8 t_PalLen = 16> inline void MinusOne(u16 const* i_srcPal, u16* i_dstPal);
template<u8 t_PalLen = 16> inline void Halve(u16 const* i_srcPal, u16* i_dstPal);
template<u8 t_PalLen = 16> inline void Tint(u16 const* i_srcPal, u16* i_dstPal, RGB3Colour i_col);

//------------------------------------------------------------------------------
// All code here works on the basis BG text frame is using PAL0
inline u16 const* s_bgTextFramePalette{ nullptr };
inline void SetBGTextFramePalette(u16 const* i_pal) { s_bgTextFramePalette = i_pal; }
//------------------------------------------------------------------------------
// And char text frame is using PAL1
inline u16 const* s_charTextFramePalette{ nullptr };
inline void SetCharTextFramePalette(u16 const* i_pal) { s_charTextFramePalette = i_pal; }

//------------------------------------------------------------------------------
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
		System::SetColour_DATA(s_bgTextFramePalette[t_Line - t_BaseLine]);
		if constexpr (t_Line - t_BaseLine < 5)
		{
			System::SetColour_CTRL(t_Line - t_BaseLine + 1);
			System::SetColour_DATA(s_bgTextFramePalette[t_Line - t_BaseLine + 1]);
			System::SetColour_CTRL(t_Line - t_BaseLine + 2);
			System::SetColour_DATA(s_bgTextFramePalette[t_Line - t_BaseLine + 2]);
		}
		else
		{
			// Reset for next frame
			SYS_setHIntCallback(&HInt_TextFrame<t_LineToShow>);
		}
	}
}

//------------------------------------------------------------------------------
// Requires 2 lines for DMA setup and execution, to show changes on t_LineToShow
using CallbackFn = void();
template <u16 t_PalNum, u8 t_LineToShow, CallbackFn* T_Callback, u8 t_Line = t_LineToShow - 2>
HINTERRUPT_CALLBACK HInt_TextFrameDMA()
{
	if (GET_VCOUNTER == t_Line)
	{
		System::SetPalette_Fast<During::Active, t_PalNum>(s_bgTextFramePalette);
		T_Callback();
	}
}

//------------------------------------------------------------------------------
using CallbackFn = void();
template <u16 t_PalNum0, u16 t_PalNum1, bool t_BGFirst, u8 t_LineToShow, CallbackFn *T_Callback, u8 t_Line = t_LineToShow - 2>
HINTERRUPT_CALLBACK HInt_TextFrameDMA2()
{
	if (GET_VCOUNTER == t_Line)
	{
		if constexpr (t_BGFirst)
		{
			System::Set2Palette_Fast<During::Active, t_PalNum0, t_PalNum1>(s_bgTextFramePalette, s_charTextFramePalette);
		}
		else
		{
			System::Set2Palette_Fast<During::Active, t_PalNum1, t_PalNum0>(s_charTextFramePalette, s_bgTextFramePalette);
		}
		T_Callback();
	}
}

}

#include "PaletteOps.inl"