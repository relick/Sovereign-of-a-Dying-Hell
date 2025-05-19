#pragma once

#include "adventure-md/Declare.hpp"

#include <array>

namespace Palettes
{

template<u16 t_PalSize>
struct FadeOp
{
	u16 m_counter{};
	u16* m_dstPal{};

	std::array<s16, t_PalSize> m_palR{};
	std::array<s16, t_PalSize> m_palG{};
	std::array<s16, t_PalSize> m_palB{};

	std::array<s16, t_PalSize> m_stepR{};
	std::array<s16, t_PalSize> m_stepG{};
	std::array<s16, t_PalSize> m_stepB{};

	explicit operator bool() const { return m_counter > 0; }
	bool DoFadeStep();
};

// io_startDstPal should hold the starting palette, and will also be used as the target for fading
template<u16 t_PalSize>
inline FadeOp<t_PalSize> CreateFade(u16* io_startDstPal, u16 const* i_endPal, u16 i_numFrame);

}

#include "adventure-md/FadeOps.inl"