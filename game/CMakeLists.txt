cmake_minimum_required(VERSION 4.0)

project(Jam15 C CXX ASM)

find_package(SGDK REQUIRED)

md_add_rom(soadh SGDK::mdcpp src/boot/rom_head.c src/boot/sega.s)
target_include_directories(soadh
  PRIVATE
    src
    res
)
target_sources(soadh
  PRIVATE
    src/main.cpp
    src/CharacterAnimator.cpp
    src/ChoiceSystem.cpp
    src/DialoguePrinter2.cpp
    src/FontData.cpp
    src/Game.cpp
    src/IntroWorld.cpp
    src/SFXManager.cpp
    src/SpriteManager.cpp
    src/TitleWorld.cpp
    src/Version.cpp
    src/VNWorld.cpp
    src/VoteMode.cpp
    src/Jam15/Jam15_Scenes.cpp
    src/Jam15/Jam15_Script.cpp
)
md_target_resources(soadh
  PRIVATE
    res/res_fonts.res
    res/res_intro.res
    res/res_vn.res
    res/res_voting.res
    res/Jam15/res_bg.res
    res/Jam15/res_chara.res
    res/Jam15/res_music.res
    res/Jam15/res_sfx.res
    res/Jam15/res_spr.res
)