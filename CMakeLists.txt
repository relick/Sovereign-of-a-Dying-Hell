cmake_minimum_required(VERSION 4.0)

project(Sovereign C CXX ASM)

add_subdirectory(adventure-md)

find_package(SGDK REQUIRED)

md_add_rom(sovereign SGDK::mdcpp)

md_rom_title(sovereign "Sovereign of a Dying Hell v0.3")
md_rom_revision(sovereign 3)
md_rom_copyright(sovereign "KDev" June 2025)

md_rom_use_devices(sovereign 3_BUTTON)
md_rom_enable_sram(sovereign SAVING ODD_8)

target_link_libraries(sovereign PRIVATE adventure-md)
target_include_directories(sovereign
  PRIVATE
    src 
)
target_sources(sovereign
  PRIVATE
    src/main.cpp
    src/Jam15_Scenes.cpp
    src/Jam15_Script.cpp
)
md_target_resources(sovereign
  PRIVATE
    res/bg.res
    res/chara.res
    res/music.res
    res/sfx.res
    res/spr.res
)