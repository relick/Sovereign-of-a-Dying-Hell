#pragma once

#include "Declare.hpp"
#include "CharacterData.hpp"

#include "res_chara.h"

namespace TestZanmu
{

inline constexpr Game::AnimFrames<5> c_zanmu_blink_anim = { {
	{ &zanmu_blink_1, 287, },
	{ &zanmu_blink_2, 4, },
	{ &zanmu_blink_3, 3, },
	{ &zanmu_blink_4, 2, },
	{ &zanmu_blink_5, 4, },
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

}