#include "Worlds.hpp"
#include "Game.hpp"
#include "Version.hpp"

#include <genesis.h>
#include "res_fonts.h"

namespace Game
{

void IntroWorld::Init
(
	Game& io_game
)
{
	PAL_setPalette(PAL0, intro_font_pal.data, TransferMethod::DMA);
	VDP_loadFont(&intro_font, TransferMethod::DMA);
	VDP_setTextPalette(PAL0);

	VDP_drawText("kasha.dev", 15, 0);
}

void IntroWorld::Shutdown
(
	Game& io_game
)
{
	VDP_setVerticalScroll(VDP_getTextPlane(), 0);
	VDP_clearPlane(VDP_getTextPlane(), true);
}

void IntroWorld::Run
(
	Game& io_game
)
{
	if (m_timer < RealToTimer(c_screenHeightPx / 2))
	{
		VDP_setVerticalScroll(VDP_getTextPlane(), -1 * TimerToReal(m_timer));
	}
	else if (m_timer > RealToTimer(150))
	{
		io_game.RequestNextWorld(std::make_unique<GameWorld>());
	}
	m_timer += TimerStep();
}

}