#pragma once

#include "Declare.hpp"

#include <array>
#include <optional>

namespace Palettes
{

struct FadeOp
{
	u16 m_size{};
	u16 m_counter{};
	u16* m_dstPal{};

	std::array<s16, 32> m_palR{};
	std::array<s16, 32> m_palG{};
	std::array<s16, 32> m_palB{};

	std::array<s16, 32> m_stepR{};
	std::array<s16, 32> m_stepG{};
	std::array<s16, 32> m_stepB{};

	explicit operator bool() const { return m_counter > 0; }
	bool DoFadeStep();
};

// io_startDstPal should hold the starting palette, and will also be used as the target for fading
// i_size max is 32
FadeOp CreateFade(u16* io_startDstPal, u16 const* i_endPal, u16 i_size, u16 i_numFrame);

}