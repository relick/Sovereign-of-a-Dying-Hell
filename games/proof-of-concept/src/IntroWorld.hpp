#pragma once

#include "Declare.hpp"
#include "Worlds.hpp"

namespace Game
{

class IntroWorld
	: public World
{
	std::array<s16, c_screenHeightPx> m_lineTable{};
	u16 m_sineScroll{0};
	VBlankCallbackID m_fxScrollID{};

	bool m_joyUnpressed = false;

	f16 m_timer = 0;
	bool m_fadeInStarted = false;
	bool m_fadeOutStarted = false;

private:
	WorldRoutine Init(Game &io_game) override;
	WorldRoutine Shutdown(Game &io_game) override;
	void Run(Game &io_game) override;

	void DMAScrollData();
};

}