#pragma once

#include "Declare.hpp"
#include "CharacterData.hpp"

#include "Jam15/res_chara.h"

namespace Jam15
{

inline constexpr Game::AnimFrames<5> c_zanmu_blink_anim = { {
	{ &zanmu_blink_1, Game::AnimFrameDuration::Variable(60, 300), },
	{ &zanmu_blink_2, Game::AnimFrameDuration::Fixed(4), },
	{ &zanmu_blink_3, Game::AnimFrameDuration::Fixed(3), },
	{ &zanmu_blink_4, Game::AnimFrameDuration::Fixed(2), },
	{ &zanmu_blink_5, Game::AnimFrameDuration::Fixed(4), },
} };

inline constexpr Game::Pose c_zanmu_blink_pose = {
	&zanmu_tileset,
	& zanmu_pal,
	c_zanmu_blink_anim
};

inline constexpr Game::Poses<1> c_zanmu_poses = {
	c_zanmu_blink_pose,
};

inline constexpr Game::Character c_zanmu = {
	"ZANMU", true,
	c_zanmu_poses,
};

inline constexpr Game::Character c_hisami = {
	"HISAMI", false,
	{},
};

inline constexpr Game::Character c_suika = {
	"SUIKA", false,
	{},
};

inline constexpr Game::Character c_yuugi = {
	"YUUGI", false,
	{},
};

inline constexpr Game::Character c_yuuma = {
	"YUUMA", false,
	{},
};

inline constexpr Game::Character c_kishin = {
	"?????", false,
	{},
};

}