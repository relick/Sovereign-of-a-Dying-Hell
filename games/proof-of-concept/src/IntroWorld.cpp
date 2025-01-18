#include "Worlds.hpp"
#include "Game.hpp"
#include "Version.hpp"

#include <genesis.h>
#include "res_bg.h"

static u16 myTileInd = TILE_USER_INDEX;

namespace Game
{

void IntroWorld::Init
(
	Game& io_game
)
{
	VDP_drawImageEx(VDPPlane::BG_B, &logo, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, myTileInd), 0, 0, false, true);
	myTileInd += logo.tileset->numTile;
}

void IntroWorld::Shutdown
(
	Game& io_game
)
{
}

void IntroWorld::Run
(
	Game& io_game
)
{
	if (JOY_readJoypad(JOY_1) != 0)
	{
		m_timer = FIX16(6);
	}

	if (m_timer > FIX16(0.5) && !m_fadeInStarted)
	{
		PAL_fadeInPalette(PAL0, logo.palette->data, FramesPerSecond(), true);
		m_fadeInStarted = true;
	}
	else if (m_timer > FIX16(2) && !m_fadeOutStarted)
	{
		PAL_fadeOutPalette(PAL0, FramesPerSecond(), true);
		m_fadeOutStarted = true;
	}
	else if (m_timer > FIX16(3.5))
	{
		PAL_interruptFade();
		PAL_setPalette(PAL0, palette_black, DMA_QUEUE);
		io_game.RequestNextWorld(std::make_unique<VNWorld>());
	}
	m_timer += FrameStep();
}
}