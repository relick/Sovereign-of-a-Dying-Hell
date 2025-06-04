cmake_minimum_required(VERSION 4.0)

project(Sovereign C CXX ASM)

add_subdirectory(adventure-md)

find_package(SGDK REQUIRED)

macro(make_sovereign_rom suffix)
  md_add_rom(sovereign${suffix} SGDK::mdcpp)

  md_rom_title(sovereign${suffix} "Sovereign of a Dying Hell v0.3")
  md_rom_revision(sovereign${suffix} 3)
  md_rom_copyright(sovereign${suffix} "KDev" June 2025)

  md_rom_use_devices(sovereign 3_BUTTON)
  md_rom_enable_sram(sovereign SAVING ODD_8)

  target_link_libraries(sovereign${suffix} PRIVATE adventure-md${suffix})
  target_include_directories(sovereign${suffix}
    PRIVATE
      src 
  )
  target_sources(sovereign${suffix}
    PRIVATE
      src/main.cpp
      src/Jam15_Scenes.cpp
      src/Jam15_Script.cpp
  )
  md_target_resources(sovereign${suffix}
    PRIVATE
      res/bg.res
      res/chara.res
      res/music.res
      res/sfx.res
      res/spr.res
  )
endmacro()

make_sovereign_rom("")
make_sovereign_rom("_web")

# Distribution
if(JAM_INSTALL)
  # MD build
  set(ROM_TARGET_NAME sovereign)
  install(TARGETS ${ROM_TARGET_NAME} DESTINATION .)

  # Blastem emu dist
  configure_file(distribute_src/emu/rungame.bat.in ${CMAKE_CURRENT_BINARY_DIR}/dist_win/rungame.bat @ONLY)
  install(CODE "file(COPY \"$<TARGET_FILE:sovereign>\" DESTINATION \"${CMAKE_CURRENT_BINARY_DIR}/dist_win\")")
  install(CODE "file(COPY \"${CMAKE_CURRENT_SOURCE_DIR}/distribute_src/emu/blastem_win/\" DESTINATION \"${CMAKE_CURRENT_BINARY_DIR}/dist_win/blastem\")")
  install(CODE "file(ARCHIVE_CREATE OUTPUT \"${CMAKE_CURRENT_BINARY_DIR}/${ROM_TARGET_NAME}_win.zip\" PATHS \"$<TARGET_FILE_NAME:sovereign>\" \"rungame.bat\" \"blastem\" FORMAT \"zip\" WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/dist_win\")")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${ROM_TARGET_NAME}_win.zip DESTINATION .)

  configure_file(distribute_src/emu/rungame.sh.in ${CMAKE_CURRENT_BINARY_DIR}/dist_macos/rungame.sh @ONLY)
  install(CODE "file(COPY \"$<TARGET_FILE:sovereign>\" DESTINATION \"${CMAKE_CURRENT_BINARY_DIR}/dist_macos\")")
  install(CODE "file(COPY \"${CMAKE_CURRENT_SOURCE_DIR}/distribute_src/emu/blastem_macos/\" DESTINATION \"${CMAKE_CURRENT_BINARY_DIR}/dist_macos/blastem\")")
  install(CODE "file(ARCHIVE_CREATE OUTPUT \"${CMAKE_CURRENT_BINARY_DIR}/${ROM_TARGET_NAME}_macos.zip\" PATHS \"$<TARGET_FILE_NAME:sovereign>\" \"rungame.sh\" \"blastem\" FORMAT \"zip\" WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/dist_macos\")")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${ROM_TARGET_NAME}_macos.zip DESTINATION .)

  configure_file(distribute_src/emu/rungame.sh.in ${CMAKE_CURRENT_BINARY_DIR}/dist_linux/rungame.sh @ONLY)
  install(CODE "file(COPY \"$<TARGET_FILE:sovereign>\" DESTINATION \"${CMAKE_CURRENT_BINARY_DIR}/dist_linux\")")
  install(CODE "file(COPY \"${CMAKE_CURRENT_SOURCE_DIR}/distribute_src/emu/blastem_linux/\" DESTINATION \"${CMAKE_CURRENT_BINARY_DIR}/dist_linux/blastem\")")
  install(CODE "file(ARCHIVE_CREATE OUTPUT \"${CMAKE_CURRENT_BINARY_DIR}/${ROM_TARGET_NAME}_linux.zip\" PATHS \"$<TARGET_FILE_NAME:sovereign>\" \"rungame.sh\" \"blastem\" FORMAT \"zip\" WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/dist_linux\")")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${ROM_TARGET_NAME}_linux.zip DESTINATION .)

  # Web build and web emu dist
  set(ROM_TARGET_NAME sovereign_web)
  configure_file(distribute_src/web/index.html.in ${CMAKE_CURRENT_BINARY_DIR}/dist_web/index.html @ONLY)
  install(CODE "file(COPY \"$<TARGET_FILE:sovereign>\" DESTINATION \"${CMAKE_CURRENT_BINARY_DIR}/dist_web\")")
  install(CODE "file(ARCHIVE_CREATE OUTPUT \"${CMAKE_CURRENT_BINARY_DIR}/${ROM_TARGET_NAME}.zip\" PATHS \"$<TARGET_FILE_NAME:sovereign>\" \"index.html\" FORMAT \"zip\" WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/dist_web\")")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${ROM_TARGET_NAME}.zip DESTINATION .)
endif()
