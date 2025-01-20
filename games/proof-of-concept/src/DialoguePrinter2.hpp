#pragma once

#include "Declare.hpp"

#include <genesis.h>

#include <array>
#include <string>

namespace Game
{

struct PackedPixel
{
	// 4 bits per pixel
	u8 m_value{0};

	u8 Pixel0() const { return m_value >> 4; }
	u8 Pixel1() const { return m_value & 0xF; }
	u8 operator[](u16 const i_pixel) const { return i_pixel == 0 ? Pixel0() : Pixel1(); }
};

// Align to 32 bit as we'll cast to u32* when blitting and uploading to VRAM via SGDK
struct alignas(4) Tile
{
	// 8 rows of 8 pixels
	std::array<PackedPixel, 32> m_pixels{};

	// Get pixel from 0 to 63
	u8 operator[](u16 const i_pixel) const { return m_pixels[i_pixel >> 1][i_pixel & 1];}
};

struct Char
{
	u16 m_srcIndex{0};
	u8 m_charWidth{8};
};

//------------------------------------------------------------------------------
/// Blits text to a tile buffer in RAM then creates DMA calls for execution in VBlank
/// TODO: First pass fills tilemap into Window plane. It would be better to use sprites instead to free up the tilemap space for the text tiles themselves. But this will potentially conflict with the palette swap so be careful.
/// TODO: First pass puts tiles over the SGDK font. It's easier to work with in VRAM if these are moved to between tilemaps - There's 128 spare tiles for starters between planes A and B.
//------------------------------------------------------------------------------
class DialoguePrinter2
{
	Game* m_game{nullptr};

	VBlankCallbackID m_dmaCallbackID{};

	char const* m_curText{nullptr};
	u16 m_curTextLen{0};
	u16 m_curTextIndex{0};
	bool m_lastCharWasSpace{true};
	u16 m_x{0}; // In pixels
	u16 m_y{0}; // In tiles

	std::array<Tile, 128> m_tiles{};

	TileSet const* m_textFont{nullptr};
	std::array<Char, 96> m_textFontData{}; // TODO: optimise by using proper ascii

	TileSet const* m_nameFont{nullptr};
	std::array<u16, 26> m_nameFontData{};
	bool m_nameOnLeft{true};

	std::array<SpriteID, 4> m_nameSprites{};
	std::array<SpriteID, 27> m_textSprites{};
	SpriteID m_nextArrow{};

	u8 m_arrowTimer{0};

	bool m_nameTileRefresh{true};
	u16 m_lineTileRefreshStart{0};
	u16 m_lineTileRefreshEnd{u16(m_tiles.size())};

public:
	// Sets up tiles and tilemap
	void Init(Game& io_game, TileSet const& i_textFont, TileSet const& i_nameFont);
	void Shutdown();

	void SetName(char const* i_name, bool i_left);
	void SetText(char const *i_text);

	// Advances render and queues DMA
	bool Update();

	// Action from player to skip or progress
	void Next();

private:
	void SetupSprites();

	// Returns false when no more can be drawn until user progresses
	bool DrawChar();

	void QueueNameDMA();
	void QueueCharacterDMA(u16 i_charIndex);
};

}