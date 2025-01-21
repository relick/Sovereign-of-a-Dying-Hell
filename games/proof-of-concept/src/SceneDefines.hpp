#pragma once

// Make sure to include last, to avoid breaking other code

#define wait co_yield {}
#define end co_return

#define scene(bg)

#define play_music(music)

#define portrait(chara, pose)
#define hide_portrait()

#define show(chara, pose)
#define hide()

#define think(text) io_dp.SetName("", false); io_dp.SetText("(" text ")"); wait

#define say(chara_name, text) io_dp.SetName(chara_name, true); io_dp.SetText(text); wait