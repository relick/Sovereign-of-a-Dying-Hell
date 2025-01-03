#include "Worlds.hpp"
#include "Game.hpp"

#include <genesis.h>
#include "res_bg.h"
#include "res_fonts.h"

#include <cstring>

namespace Game
{

void GameWorld::Init
(
	Game& io_game
)
{
	VDP_setWindowVPos(true, c_textFramePos);
	VDP_setTextPlane(VDPPlane::WINDOW);
	VDP_setTextPriority(0);

	VDP_drawImage(VDP_getTextPlane(), &text_frame, 0, c_textFramePos);
	VDP_setTextPalette(PAL0);
	VDP_loadFont(&vn_font, TransferMethod::DMA);

	m_printer.PushLine("The quick brown fox, jumps over the lazy dog. \n\"Amazing'!??! (3*3+2); or: £3.50 :) :) :)");
}

void GameWorld::Shutdown
(
	Game& io_game
)
{
}

void GameWorld::Run
(
	Game& io_game
)
{
	u16 buttons = JOY_readJoypad(JOY_1);

	static bool pressed = false;
	if (buttons != 0)
	{
		if (!pressed)
			m_printer.Next();
		pressed = true;
	}
	else
	{
		pressed = false;
	}

	static u16 time = 0;
	if(time == 3)
	{
		m_printer.Update();
		time = 0;
	}
	else
	{
		++time;
	}

}

}