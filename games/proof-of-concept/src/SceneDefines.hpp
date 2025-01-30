#pragma once

#include "Game.hpp"
#include "VNWorld.hpp"
#include "Version.hpp"

// Make sure to include last, to avoid breaking other code

#define wait co_yield {}
#define end co_return

#define scene(bg) io_vn.SetBG(io_game, bg)

#define play_music(music, fadeInSeconds, loop) io_vn.StartMusic(music, fix16ToInt(fix16Mul(FIX16(fadeInSeconds), FramesPerSecond())), loop)

#define wait_for_tasks() io_vn.WaitForTasks(io_game); wait

#define portrait(chara, pose)
#define hide_portrait()

#define show(chara, pose) io_vn.SetCharacter(io_game, script.chara, script.chara ## _ ## pose)
#define hide() io_vn.HideCharacter(io_game, false)

#define think(text) io_vn.SetText(io_game, -1, "(" text ")"); wait

#define say(chara, text) io_vn.SetText(io_game, script.chara, text); wait

#define choice(choiceArr) {}
#define timed_choice(timeInSeconds, choiceArr) {}

#define vpunch()