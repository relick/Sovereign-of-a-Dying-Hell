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
	if (m_timer < FIX16(1))
	{
		VDP_setVerticalScroll(VDP_getTextPlane(), -fix16ToInt(fix16Mul(intToFix16(c_screenHeightPx / 2), m_timer)));
	}
	else if (m_timer < FIX16(2) && !m_fadeOutStarted)
	{
		PAL_fadeOutAll(FramesPerSecond(), true);
		m_fadeOutStarted = true;
	}
	else if (m_timer > FIX16(3))
	{
		io_game.RequestNextWorld(std::make_unique<VNWorld>());
	}
	m_timer += FrameStep();
}

}