#pragma once

#include "Declare.hpp"

#ifndef WEB_BUILD
#define WEB_BUILD 1
#endif

// Sys constants
inline constexpr u16 c_screenWidthTiles = 40;
inline constexpr u16 c_screenHeightTiles = 28;
inline constexpr u16 c_screenWidthPx = c_screenWidthTiles << 3;
inline constexpr u16 c_screenHeightPx = c_screenHeightTiles << 3;

inline constexpr u16 c_maxSpriteCount = 80;

inline constexpr u16 c_vramEndTile = 2048;
inline constexpr u16 c_reservedClearTile = c_vramEndTile - 1;
inline constexpr u16 c_portraitSizeTiles = 4;
inline constexpr u16 c_portraitSizePx = c_portraitSizeTiles << 3;
inline constexpr u16 c_reservedPortraitTileStart = c_reservedClearTile - (c_portraitSizeTiles * c_portraitSizeTiles);
inline constexpr u16 c_reservedTileCount = c_vramEndTile - c_reservedPortraitTileStart;

// Large tile space (1536 tiles)
inline constexpr u16 c_tilesStart = 0;
inline constexpr u16 c_tilesStartAddr = c_tilesStart * 32;
inline constexpr u16 c_tilesEnd = 1536; // Map B starts here
inline constexpr u16 c_tilesEndAddr = c_tilesEnd * 32;
inline constexpr u16 c_tilesCount = c_tilesEnd - c_tilesStart;

// Mid tile space between maps and tables (128 tiles)
inline constexpr u16 c_midTilesStart = 1664; // Map B ends here
inline constexpr u16 c_midTilesStartAddr = c_midTilesStart * 32;
inline constexpr u16 c_midTilesEnd = 1792; // Map A starts here
inline constexpr u16 c_midTilesEndAddr = c_midTilesEnd * 32;
inline constexpr u16 c_midTilesCount = c_midTilesEnd - c_midTilesStart;

// Small tile space after all maps and tables (76 - reserved)
inline constexpr u16 c_extraTilesStart = 1972; // Sprite list is last thing in VRAM and it ends here
inline constexpr u16 c_extraTilesStartAddr = c_extraTilesStart * 32;
inline constexpr u16 c_extraTilesEnd = 2048 - c_reservedTileCount;
inline constexpr u16 c_extraTilesEndAddr = c_extraTilesEnd * 32;
inline constexpr u16 c_extraTilesCount = c_extraTilesEnd - c_extraTilesStart;

// Game constants
inline constexpr u16 c_textFrameHeight = 5;
inline constexpr u16 c_textFramePos = 23;