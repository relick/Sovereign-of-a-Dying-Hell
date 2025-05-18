#pragma once

#include "adventure-md/Declare.hpp"
#include "adventure-md/Worlds.hpp"

#include <memory>

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

	std::unique_ptr<Script> m_script;

public:
	IntroWorld(std::unique_ptr<Script> i_script);

private:
	WorldRoutine Init(Game &io_game) override;
	WorldRoutine Shutdown(Game &io_game) override;
	void Run(Game &io_game) override;

	void DMAScrollData();
};

}