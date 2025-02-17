#pragma once

#include "Declare.hpp"

#include <array>

namespace Tiles
{

struct PackedPixel
{
	// 4 bits per pixel
	u8 m_value{0};

	u8 Pixel0() const { return m_value >> 4; }
	u8 Pixel1() const { return m_value & 0xF; }
	u8 operator[](u16 const i_pixel) const { return i_pixel == 0 ? Pixel0() : Pixel1(); }
};

// Align to 32 bit, as it should match alignment with the u32 const* data SGDK provides
struct alignas(4) Tile
{
	// 8 rows of 8 pixels
	std::array<PackedPixel, 32> m_pixels{};

	// Get pixel from 0 to 63
	u8 operator[](u8 const i_pixel) const { return m_pixels[i_pixel >> 1][i_pixel & 1];}

    u32* AsRawRows() { return reinterpret_cast<u32*>(this); }
    u32 const* AsRawRows() const { return reinterpret_cast<u32 const*>(this); }
};

inline Tile const* AsTiles(u32 const* i_sgdkTileData)
{
    return reinterpret_cast<Tile const*>(i_sgdkTileData);
}

}