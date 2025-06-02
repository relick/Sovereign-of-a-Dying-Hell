#pragma once

#include "adventure-md/Declare.hpp"
#include "adventure-md/Game.hpp"
#include "adventure-md/VNWorld.hpp"
#include "adventure-md/Version.hpp"

// Make sure to include last, to avoid breaking other code

#define wait co_yield {}
#define end co_return

#define scene(BG) io_vn.SetBG(io_game, BG)

#define play_music(MUSIC, LOOP) io_vn.StartMusic(io_game, MUSIC, LOOP); wait_for_tasks()
#define stop_music() io_vn.StopMusic(io_game); wait_for_tasks()
#define play_sfx(SFX)

#define wait_for_tasks() io_vn.WaitForTasks(io_game); wait

#define face(CHARA, POSE) io_vn.SetPortrait(io_game, c_ ## CHARA ## _face_ ## POSE)
#define hide_face() io_vn.HidePortrait(io_game)

#define show(CHARA, POSE) if (!io_vn.CharacterShown()) { io_vn.ClearMode(io_game); } io_vn.SetCharacterVisual(io_game, c_ ## CHARA ## _ ## POSE ## _pose); wait_for_tasks()
#define hide() io_vn.ClearMode(io_game); io_vn.HideCharacterVisual(io_game, false); wait_for_tasks()

#define desc(TEXT) hide_face(); io_vn.SetText(io_game, &Game::c_special_desc, TEXT, script.desc_beeps); wait

#define think(TEXT) hide_face(); io_vn.SetText(io_game, nullptr, "(" TEXT ")", Game::SFXID{}); wait
#define think_face(CHARA, FACE, TEXT) io_vn.SetText(io_game, nullptr, "(" TEXT ")", Game::SFXID{}); face(CHARA, FACE); wait

#define say(CHARA, TEXT) hide_face(); io_vn.SetText(io_game, &c_ ## CHARA, TEXT, script. CHARA ## _beeps); wait
#define say_hidden(CHARA, TEXT) hide_face(); io_vn.SetText(io_game, (c_ ## CHARA).m_showOnLeft ? &Game::c_special_l_hidden : &Game::c_special_r_hidden, TEXT, script. CHARA ## _beeps); wait
#define say_face(CHARA, FACE, TEXT) io_vn.SetText(io_game, &c_ ## CHARA, TEXT, script. CHARA ## _beeps); face(CHARA, FACE); wait

#define choice(...) static constexpr std::array _choices = { __VA_ARGS__ }; io_vn.Choice(io_game, _choices); wait
#define get_choice_result() *(io_vn.GetChoiceResult()); wait_for_tasks()
#define timed_choice(TIME_IN_SECONDS, CHOICE_SPAN) io_vn.TimedChoice(io_game, CHOICE_SPAN, FIX16(TIME_IN_SECONDS)); wait
#define get_timed_choice_result() io_vn.GetChoiceResult()

// Use HideCharacterVisual to quickly put away the VOTE X'D graphic
#define start_vote(VOTE_PARAMS) /*play_music(voting, false);*/ io_vn.StartVote(io_game, VOTE_PARAMS); wait; io_vn.HideCharacterVisual(io_game, false)
#define get_vote_result() io_vn.GetVoteResult()

// TODO
#define vpunch()

// TODO
#define cg(CG)