#include "Worlds.hpp"
#include "Game.hpp"
#include "Version.hpp"

#include <genesis.h>
#include "res_bg.h"

namespace Game
{

void IntroWorld::Init
(
	Game& io_game
)
{
	VDP_drawImageEx(VDPPlane::BG_B, &logo, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, 0), 0, 0, false, true);

	VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);
	m_fxScrollID = io_game.AddVBlankCallback([this]{ DMAScrollData(); });
}

void IntroWorld::Shutdown
(
	Game& io_game
)
{
	io_game.RemoveVBlankCallback(m_fxScrollID);

	for (u8 i = 0; i < m_lineTable.size(); i++)
	{
		m_lineTable[i] = 0;
	}
	DMAScrollData();
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
	
	// Update scroll
	for (u16 i = 0; i < m_lineTable.size(); i++)
	{
		m_lineTable[i] = fix16Mul(sinFix16((i << 5) + m_sineScroll), m_timer - FIX16(1.75)) >> 4;
		if(i&1)
		{
			m_lineTable[i] = -m_lineTable[i];
		}
	}

	m_sineScroll += 4;
}

void IntroWorld::DMAScrollData()
{
	VDP_setHorizontalScrollLine(BG_B, 0, m_lineTable.data(), m_lineTable.size(), DMA);
}

}