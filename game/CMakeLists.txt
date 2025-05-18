cmake_minimum_required(VERSION 4.0)

project(Jam15 C CXX ASM)

find_package(SGDK REQUIRED)

md_add_rom(sovereign SGDK::mdcpp)

md_rom_title(sovereign "Sovereign of a Dying Hell v0.3")
md_rom_revision(sovereign 3)
md_rom_copyright(sovereign "KDev" June 2025)

md_rom_use_devices(sovereign 3_BUTTON)
md_rom_enable_sram(sovereign SAVING ODD_8)

target_include_directories(sovereign
  PRIVATE
    src
)
target_sources(sovereign
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
md_target_resources(sovereign
  PRIVATE
    res/fonts.res
    res/intro.res
    res/vn.res
    res/voting.res
    res/Jam15/bg.res
    res/Jam15/chara.res
    res/Jam15/music.res
    res/Jam15/sfx.res
    res/Jam15/spr.res
)