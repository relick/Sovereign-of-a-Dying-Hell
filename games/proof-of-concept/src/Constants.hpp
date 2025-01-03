#pragma once

#include <types.h>

// Sys constants
inline constexpr u16 c_screenWidthTiles = 40;
inline constexpr u16 c_screenHeightTiles = 28;
inline constexpr u16 c_screenWidthPx = c_screenWidthTiles << 3;
inline constexpr u16 c_screenHeightPx = c_screenHeightTiles << 3;

// Game constants
inline constexpr u16 c_textFrameHeight = 5;
inline constexpr u16 c_textFramePos = c_screenHeightTiles - c_textFrameHeight;