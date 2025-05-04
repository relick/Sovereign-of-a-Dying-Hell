#pragma once

#include "Declare.hpp"
#include "SpriteManager.hpp"
#include "TileData.hpp"

#include <array>

namespace Game
{

//------------------------------------------------------------------------------
/// Blits text to a tile buffer in RAM then creates DMA calls for execution in VBlank
//------------------------------------------------------------------------------
class DialoguePrinter2
{
	Game* m_game{nullptr};
	FontData const* m_fonts{nullptr};

	bool m_vramInitialised{ false };
	bool m_spritesInitialised{ false };

	VBlankCallbackID m_dmaCallbackID{};

	SFXID m_beeps;
	bool m_textUsesDescFont{ false };
	char const* m_curText{nullptr};
	u16 m_curTextLen{0};
	u16 m_curTextIndex{0};
	bool m_lastCharWasSpace{true};
	u16 m_x{0}; // In pixels
	u16 m_y{0}; // In tiles

	char const* m_curName{nullptr};

	std::array<Tiles::Tile, 128> m_tiles{};

	bool m_nameOnLeft{true};

	std::array<SpriteHandle, 4> m_nameSprites{};
	std::array<SpriteHandle, 24> m_textSprites{};
	SpriteHandle m_nextArrow{};

	u8 m_arrowTimer{0};

	bool m_nameTileRefresh{false};
	u16 m_lineTileRefreshStart{UINT16_MAX};
	u16 m_lineTileRefreshEnd{0};

	bool m_doneAllText{ false };

	s8 m_timer{ 0 };

public:
	// Sets up tiles and tilemap
	DialoguePrinter2(Game& io_game, FontData const& i_fonts);
	~DialoguePrinter2();

	void SetName(char const* i_name, bool i_left);
	void SetText(char const* i_text, SFXID i_beeps, bool i_useDescFont);

	// Advances render and queues DMA
	void Update();
	bool Done() const;

	// Action from player to skip or progress
	void Next();

private:
	void SetupSprites();

	// Returns false when no more can be drawn until user progresses
	bool DrawChar();

	void QueueNameDMA();
	void QueueTextDMA(u16 i_tileIndex);
};

}