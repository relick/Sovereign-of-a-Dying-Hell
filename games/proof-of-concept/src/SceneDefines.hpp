#pragma once

#include "Game.hpp"
#include "VNWorld.hpp"
#include "Version.hpp"

// Make sure to include last, to avoid breaking other code

#define wait co_yield {}
#define end co_return

#define scene(bg) io_vn.SetBG(io_game, bg)

#define play_music(music, fadeInSeconds, loop) io_vn.StartMusic(music, fix16ToInt(fix16Mul(FIX16(fadeInSeconds), FramesPerSecond())), loop)

#define portrait(chara, pose)
#define hide_portrait()

#define show(chara, pose) io_vn.SetCharacter(io_game, #chara, #pose)
#define hide() io_vn.HideCharacter(io_game)

#define think(text) io_vn.SetCharName(nullptr); io_vn.SetText("(" text ")"); wait

#define say(chara_name, text) io_vn.SetCharName(#chara_name); io_vn.SetText(text); wait