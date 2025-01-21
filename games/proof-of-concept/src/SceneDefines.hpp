#pragma once

// Make sure to include last, to avoid breaking other code

#define wait co_yield 0

#define scene(bg)

#define portrait(chara, pose)

#define show(chara, pose)

#define think(text) io_dp.SetName("", false); io_dp.SetText(text); wait

#define say(chara_name, text) io_dp.SetName(chara_name, true); io_dp.SetText(text); wait