#pragma once

#include "Declare.hpp"
#include "Game.hpp"
#include "VNWorld.hpp"
#include "Version.hpp"

// Make sure to include last, to avoid breaking other code

#define wait co_yield {}
#define end co_return

#define scene(BG) io_vn.SetBG(io_game, BG)

#define play_music(MUSIC, FADE_IN_SECONDS, LOOP) io_vn.StartMusic(MUSIC, fix16ToInt(fix16Mul(FIX16(FADE_IN_SECONDS), FramesPerSecond())), LOOP)
#define play_sfx(SFX)

#define wait_for_tasks() io_vn.WaitForTasks(io_game); wait

#define face(CHARA, POSE) io_vn.SetPortrait(io_game, c_ ## CHARA ## _face_ ## POSE)
#define hide_face() io_vn.HidePortrait(io_game)

#define show(CHARA, POSE) if (!io_vn.CharacterShown()) { io_vn.ClearMode(io_game); } io_vn.SetCharacterVisual(io_game, c_ ## CHARA ## _ ## POSE ## _pose); wait_for_tasks()
#define hide() io_vn.ClearMode(io_game); io_vn.HideCharacterVisual(io_game, false); wait_for_tasks()

#define desc(TEXT) hide_face(); io_vn.SetText(io_game, &Game::c_special_desc, TEXT, script.desc_beeps); wait

#define think(TEXT) hide_face(); io_vn.SetText(io_game, nullptr, "(" TEXT ")", std::nullopt); wait
#define think_face(CHARA, FACE, TEXT) io_vn.SetText(io_game, nullptr, "(" TEXT ")", std::nullopt); face(CHARA, FACE); wait

#define say(CHARA, TEXT) hide_face(); io_vn.SetText(io_game, &c_ ## CHARA, TEXT, script. CHARA ## _beeps); wait
#define say_hidden(CHARA, TEXT) hide_face(); io_vn.SetText(io_game, (c_ ## CHARA).m_showOnLeft ? &Game::c_special_l_hidden : &Game::c_special_r_hidden, TEXT, script. CHARA ## _beeps); wait
#define say_face(CHARA, FACE, TEXT) io_vn.SetText(io_game, &c_ ## CHARA, TEXT, script. CHARA ## _beeps); face(CHARA, FACE); wait

#define choice(...) static constexpr std::array _choices = { __VA_ARGS__ }; io_vn.Choice(io_game, _choices); wait
#define get_choice_result() *(io_vn.GetChoiceResult()); wait_for_tasks()
#define timed_choice(TIME_IN_SECONDS, CHOICE_SPAN) io_vn.TimedChoice(io_game, CHOICE_SPAN, FIX16(TIME_IN_SECONDS)); wait
#define get_timed_choice_result() io_vn.GetChoiceResult()

#define start_vote(VOTE_PARAMS) io_vn.StartVote(io_game, VOTE_PARAMS); wait
#define get_vote_result() io_vn.GetVoteResult()

// TODO
#define vpunch()

// TODO
#define cg(CG)