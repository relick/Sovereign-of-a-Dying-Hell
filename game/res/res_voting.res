PALETTE voting_palette "voting/barcorners.png"

TILESET voting_time_0 "voting/0.png" NONE NONE COLUMN
TILESET voting_time_1 "voting/1.png" NONE NONE COLUMN
TILESET voting_time_2 "voting/2.png" NONE NONE COLUMN
TILESET voting_time_3 "voting/3.png" NONE NONE COLUMN
TILESET voting_time_4 "voting/4.png" NONE NONE COLUMN
TILESET voting_time_5 "voting/5.png" NONE NONE COLUMN
TILESET voting_time_6 "voting/6.png" NONE NONE COLUMN
TILESET voting_time_7 "voting/7.png" NONE NONE COLUMN
TILESET voting_time_8 "voting/8.png" NONE NONE COLUMN
TILESET voting_time_9 "voting/9.png" NONE NONE COLUMN
TILESET voting_time_10 "voting/10.png" NONE NONE COLUMN

TILESET voting_bar_corners "voting/barcorners.png" NONE NONE ROW
TILESET voting_bar_mids "voting/barmiddle.png" NONE NONE ROW
TILESET voting_bar_midline "voting/midline.png" NONE NONE COLUMN

# FYI, cursor has to use PAL3, for shadow mode
TILESET voting_cursor "voting/cursor.png" NONE NONE COLUMN

# Sillhouettes get their own special palette
PALETTE voting_sil "voting/left1.png"
TILESET voting_sil_left1 "voting/left1.png" NONE NONE COLUMN
TILESET voting_sil_left2 "voting/left2.png" NONE NONE COLUMN
TILESET voting_sil_left3 "voting/left3.png" NONE NONE COLUMN
TILESET voting_sil_left4 "voting/left4.png" NONE NONE COLUMN
TILESET voting_sil_left5 "voting/left5.png" NONE NONE COLUMN
TILESET voting_sil_left6 "voting/left6.png" NONE NONE COLUMN
TILESET voting_sil_left7 "voting/left7.png" NONE NONE COLUMN
TILESET voting_sil_right1 "voting/right1.png" NONE NONE COLUMN
TILESET voting_sil_right2 "voting/right2.png" NONE NONE COLUMN
TILESET voting_sil_right3 "voting/right3.png" NONE NONE COLUMN
TILESET voting_sil_right4 "voting/right4.png" NONE NONE COLUMN
TILESET voting_sil_right5 "voting/right5.png" NONE NONE COLUMN
TILESET voting_sil_right6 "voting/right6.png" NONE NONE COLUMN
TILESET voting_sil_right7 "voting/right7.png" NONE NONE COLUMN

TILESET voting_passed_vote_set "voting/Passed_Vote.png"
TILEMAP voting_passed_vote_map "voting/Passed_Vote.png" voting_passed_vote_set
TILESET voting_passed_passed_set "voting/Passed_Passed.png"
TILEMAP voting_passed_passed_map "voting/Passed_Passed.png" voting_passed_passed_set
TILESET voting_failed_vote_set "voting/Failed_Vote.png"
TILEMAP voting_failed_vote_map "voting/Failed_Vote.png" voting_failed_vote_set
TILESET voting_failed_failed_set "voting/Failed_Failed.png"
TILEMAP voting_failed_failed_map "voting/Failed_Failed.png" voting_failed_failed_set

TILESET voting_influence_bar "voting/influencebar.png" NONE NONE ROW
TILESET voting_influence_bar_text_set "voting/influencetext.png"
TILEMAP voting_influence_bar_text_map "voting/influencetext.png" voting_influence_bar_text_set