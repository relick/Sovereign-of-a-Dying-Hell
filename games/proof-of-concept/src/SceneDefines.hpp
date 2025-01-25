#pragma once

#include "Game.hpp"
#include "VNWorld.hpp"
#include "Version.hpp"

// Make sure to include last, to avoid breaking other code

#define wait co_yield {}
#define end co_return

#define scene(bg) io_vn.SetBG(io_game, bg)

#define play_music(music, fadeInSeconds, loop) io_vn.StartMusic(music, fix16ToInt(fix16Mul(FIX16(fadeInSeconds), FramesPerSecond())), loop)

#define wait_for_tasks() { bool waited = false; io_game.QueueLambdaTask([&waited] -> Game::Task { waited = true; co_return; }); while (!waited) { co_yield{}; } }

#define portrait(chara, pose)
#define hide_portrait()

#define show(chara, pose) io_vn.SetCharacter(io_game, script.chara, script.chara ## _ ## pose)
#define hide() io_vn.HideCharacter(io_game, false)

#define think(text) io_vn.SetCharName(-1); io_vn.SetText("(" text ")"); wait

#define say(chara, text) io_vn.SetCharName(script.chara); io_vn.SetText(text); wait

#define choice(choiceArr) {}
#define timed_choice(timeInSeconds, choiceArr) {}

#define vpunch()