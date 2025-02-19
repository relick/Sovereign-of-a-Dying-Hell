#pragma once

#include "Declare.hpp"
#include "CharacterData.hpp"

#include "Jam15/res_chara.h"

namespace Jam15
{

//------------------------------------------------------------------------------
/// Zanmu
//------------------------------------------------------------------------------
inline constexpr Game::AnimFrames<5> c_zanmu_blink_anim = { {
	{ &zanmu_blink_1, Game::AnimFrameDuration::Variable(60, 300), },
	{ &zanmu_blink_2, Game::AnimFrameDuration::Fixed(4), },
	{ &zanmu_blink_3, Game::AnimFrameDuration::Fixed(3), },
	{ &zanmu_blink_4, Game::AnimFrameDuration::Fixed(2), },
	{ &zanmu_blink_5, Game::AnimFrameDuration::Fixed(4), },
} };

inline constexpr Game::Pose c_zanmu_neutral_pose = {
	&zanmu_tileset,
	&zanmu_pal,
	c_zanmu_blink_anim
};

inline constexpr Game::Poses<1> c_zanmu_poses = {
	c_zanmu_neutral_pose,
};

inline constexpr Game::Character c_zanmu = {
	"ZANMU", true,
	c_zanmu_poses,
};

//------------------------------------------------------------------------------
/// Hisami
//------------------------------------------------------------------------------
inline constexpr Game::AnimFrames<1> c_hisami_placeholder = { {
	{ &hisami_placeholder, Game::AnimFrameDuration::Infinite(), },
} };

inline constexpr Game::Pose c_hisami_neutral_pose = {
	&hisami_tileset,
	&hisami_pal,
	c_hisami_placeholder
};

inline constexpr Game::Poses<1> c_hisami_poses = {
	c_hisami_neutral_pose,
};

inline constexpr Game::Character c_hisami = {
	"HISAMI", false,
	c_hisami_poses,
};

//------------------------------------------------------------------------------
/// Suika
//------------------------------------------------------------------------------
inline constexpr Game::AnimFrames<1> c_suika_placeholder = { {
	{ &suika_placeholder, Game::AnimFrameDuration::Infinite(), },
} };

inline constexpr Game::Pose c_suika_neutral_pose = {
	&suika_tileset,
	&suika_pal,
	c_suika_placeholder
};

inline constexpr Game::Poses<1> c_suika_poses = {
	c_suika_neutral_pose,
};

inline constexpr Game::Character c_suika = {
	"SUIKA", false,
	c_suika_poses,
};

//------------------------------------------------------------------------------
/// Yuugi
//------------------------------------------------------------------------------
inline constexpr Game::AnimFrames<1> c_yuugi_placeholder = { {
	{ &yuugi_placeholder, Game::AnimFrameDuration::Infinite(), },
} };

inline constexpr Game::Pose c_yuugi_neutral_pose = {
	&yuugi_tileset,
	&yuugi_pal,
	c_yuugi_placeholder
};

inline constexpr Game::Poses<1> c_yuugi_poses = {
	c_yuugi_neutral_pose,
};

inline constexpr Game::Character c_yuugi = {
	"YUUGI", false,
	c_yuugi_poses,
};

//------------------------------------------------------------------------------
/// Yuuma
//------------------------------------------------------------------------------
inline constexpr Game::AnimFrames<1> c_yuuma_placeholder = { {
	{ &yuuma_placeholder, Game::AnimFrameDuration::Infinite(), },
} };

inline constexpr Game::Pose c_yuuma_neutral_pose = {
	&yuuma_tileset,
	&yuuma_pal,
	c_yuuma_placeholder
};

inline constexpr Game::Poses<1> c_yuuma_poses = {
	c_yuuma_neutral_pose,
};

inline constexpr Game::Character c_yuuma = {
	"YUUMA", false,
	{},
};

//------------------------------------------------------------------------------
/// Various without images
//------------------------------------------------------------------------------
// A generic councillor on the left
inline constexpr Game::Character c_councilL = {
	"KISHIN", true,
	{},
};
// A generic councillor on the right
inline constexpr Game::Character c_councilR = {
	"KISHIN", false,
	{},
};

// The shadowy kishin
inline constexpr Game::Character c_kishin = {
	"HIDDEN KISHIN", false,
	{},
};

}