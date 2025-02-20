#pragma once

#include "Declare.hpp"
#include "GameRoutines.hpp"
#include "TileData.hpp"

namespace Tiles
{

consteval std::array<u16, 64*32> FillEmptyPlane()
{
	std::array<u16, 64 * 32> arr;
	arr.fill(TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, c_reservedClearTile));
	return arr;
}

consteval std::array<u16, 64*32> FillEmptyPlane_Highlight()
{
	std::array<u16, 64 * 32> arr;
	arr.fill(TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, c_reservedClearTile));
	return arr;
}

inline constexpr std::array<u16, 64 * 32> c_emptyPlane = FillEmptyPlane();
inline constexpr std::array<u16, 64 * 32> c_emptyPlane_Highlight = FillEmptyPlane_Highlight();

//------------------------------------------------------------------------------
inline Game::Task ClearMap_Full
(
	u16 i_planeAddr,
	std::array<u16, 64 * 32> const& i_emptyPlane
)
{
	// Simply schedule DMA
	while (!DMA_queueDma(DMA_VRAM, (void*)i_emptyPlane.data(), i_planeAddr, i_emptyPlane.size(), 2))
	{
		co_yield{};
	}

	co_return;
}

//------------------------------------------------------------------------------
// Based on VDP_setTileMapEx
//------------------------------------------------------------------------------
inline Game::Task SetMap_Full
(
	u16 i_planeAddr,
	u16 const* i_tilemap,
	u16 i_mapWidth,
	u16 i_mapHeight,
	u16 i_basetile
)
{
	//AutoProfileScope profile("SetTileMap_Full: %lu");

	u16 const* src = (u16 const*)FAR_SAFE(i_tilemap, mulu(i_mapWidth, i_mapHeight) * 2);

	// we can increment both index and palette
	u16 const baseinc = i_basetile & (TILE_INDEX_MASK | TILE_ATTR_PALETTE_MASK);
	// we can only do logical OR on priority and HV flip
	u16 const baseor = i_basetile & (TILE_ATTR_PRIORITY_MASK | TILE_ATTR_VFLIP_MASK | TILE_ATTR_HFLIP_MASK);

	// get temp buffer and schedule DMA
	u16 const bufSize = mulu(planeWidth, i_mapHeight);

	u16* buf = nullptr;
	while (!(buf = static_cast<u16*>(DMA_allocateAndQueueDma(DMA_VRAM, i_planeAddr, bufSize, 2))))
	{
		co_yield{};
	}

	// Disable ints whilst we fill the DMA buffer
	//SYS_disableInts();
	u16 const bufInc = (planeWidth - i_mapWidth);

	u16 const quarterWidth = i_mapWidth >> 2;

	u16 i = i_mapHeight;
	if (i_basetile != 0)
	{
		while (i--)
		{
			// then prepare data in buffer that will be transferred by DMA
			u16 r = quarterWidth;

			// prepare map data for row update
			while (r--)
			{
				*buf++ = baseor | (*src++ + baseinc);
				*buf++ = baseor | (*src++ + baseinc);
				*buf++ = baseor | (*src++ + baseinc);
				*buf++ = baseor | (*src++ + baseinc);
			}

			r = i_mapWidth & 3;
			// prepare map data for row update
			while (r--) *buf++ = baseor | (*src++ + baseinc);

			buf += bufInc;
		}
	}
	else
	{
		while (i--)
		{
			std::copy(src, src + i_mapWidth, buf);
			src += i_mapWidth;
			buf += planeWidth;
		}
	}

	//SYS_enableInts();

	co_return;
}

enum class WipeDir
{
	Up,
	Down,
};

//------------------------------------------------------------------------------
template<WipeDir t_Dir, u16 t_Speed = 2>
inline Game::Task SetMap_Wipe
(
	u16 i_planeAddr,
	u16 const* i_tilemap,
	u16 i_mapWidth,
	u16 i_mapHeight,
	u16 i_basetile
)
{
	//AutoProfileScope profile("SetMap_Wipe: %lu");

	u16 const* src = (u16 const*)FAR_SAFE(i_tilemap, mulu(i_mapWidth, i_mapHeight) * 2);

	// we can increment both index and palette
	u16 const baseinc = i_basetile & (TILE_INDEX_MASK | TILE_ATTR_PALETTE_MASK);
	// we can only do logical OR on priority and HV flip
	u16 const baseor = i_basetile & (TILE_ATTR_PRIORITY_MASK | TILE_ATTR_VFLIP_MASK | TILE_ATTR_HFLIP_MASK);

	// Disable ints whilst we fill the DMA buffer
	//SYS_disableInts();

	u16 addr = i_planeAddr;
	s16 const addrInc = (t_Dir == WipeDir::Up ? -1 : 1) * planeWidth * 2;
	s16 const baseTileSrcInc = (t_Dir == WipeDir::Up ? -1 : 0) * i_mapWidth * 2;
	s16 const copySrcInc = (t_Dir == WipeDir::Up ? -1 : 1) * i_mapWidth;

	if constexpr (t_Dir == WipeDir::Up)
	{
		// Start addr/src on last line and work up
		addr += (planeWidth * (i_mapHeight - 1) * 2);
		src += mulu(i_mapWidth, i_mapHeight - 1);
	}

	u16 const quarterWidth = i_mapWidth >> 2;
	u16 rowsDone = 0;
	u16 i = i_mapHeight;
	while (i--)
	{
		// get temp buffer and schedule DMA
		u16* buf = nullptr;
		while (!(buf = static_cast<u16*>(DMA_allocateAndQueueDma(DMA_VRAM, addr, i_mapWidth, 2))))
		{
			co_yield{};
		}

		if (i_basetile != 0)
		{
			// then prepare data in buffer that will be transferred by DMA
			u16 r = quarterWidth;

			// prepare map data for row update
			while (r--)
			{
				*buf++ = baseor | (*src++ + baseinc);
				*buf++ = baseor | (*src++ + baseinc);
				*buf++ = baseor | (*src++ + baseinc);
				*buf++ = baseor | (*src++ + baseinc);
			}

			r = i_mapWidth & 3;
			// prepare map data for row update
			while (r--) *buf++ = baseor | (*src++ + baseinc);

			src += baseTileSrcInc;
		}
		else
		{
			std::copy(src, src + i_mapWidth, buf);
			src += copySrcInc;
		}

		addr += addrInc;

		++rowsDone;
		if (rowsDone >= t_Speed)
		{
			rowsDone = 0;
			co_yield{};
		}
	}

	//SYS_enableInts();

	co_return;
}

//------------------------------------------------------------------------------
// Based on VDP_drawImageEx in part
//------------------------------------------------------------------------------
template<u16 t_ChunkShift = 5> // shift 5 => 32 tile chunks
inline Game::Task LoadTiles_Chunked
(
	TileSet const* i_tileset,
	u16 i_tileIndex
)
{
	//AutoProfileScope profile("LoadTiles_Chunked: %lu");
	u16 constexpr chunkSize = 1 << t_ChunkShift;

	u16 const tileChunks = i_tileset->numTile >> t_ChunkShift;
	u16 const baseTileIndex = i_tileIndex & TILE_INDEX_MASK;
	u16 tileIndex = baseTileIndex << 5;
	u16 const tileInc = 1 << (t_ChunkShift + 5);
	Tile const* srcTiles = AsTiles(i_tileset->tiles);
	u16 const srcTilesInc = chunkSize;
	for(u16 i = 0; i < tileChunks; ++i)
	{
		while (!DMA_queueDma(DMA_VRAM, (void*)srcTiles, tileIndex, chunkSize * 16, 2))
		{
			co_yield {};
		}
		srcTiles += srcTilesInc;
		tileIndex += tileInc;
	}
	u16 const remainder = i_tileset->numTile - (tileChunks << t_ChunkShift);
	if (remainder > 0)
	{
		while (!DMA_queueDma(DMA_VRAM, (void*)srcTiles, tileIndex, remainder * 16, 2))
		{
			co_yield {};
		}
	}

	co_return;
}

}