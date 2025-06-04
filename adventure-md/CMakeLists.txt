cmake_minimum_required(VERSION 4.0)

project(Adventure-MD C CXX ASM)

find_package(SGDK REQUIRED)

set(header_files
  CharacterAnimator.hpp
  CharacterData.hpp
  ChoiceSystem.hpp
  Constants.hpp
  Coroutine.hpp
  Debug.hpp
  Declare.hpp
  DialoguePrinter2.hpp
  FadeOps.hpp
  FadeOps.inl
  FontData.hpp
  Game.hpp
  GameRoutines.hpp
  IntroWorld.hpp
  PaletteOps.hpp
  PaletteOps.inl
  Saves.hpp
  Scene.hpp
  SceneDefines.hpp
  Script.hpp
  SFXManager.hpp
  SpriteManager.hpp
  TileData.hpp
  TileOps.hpp
  TitleWorld.hpp
  TypeSafeID.hpp
  Version.hpp
  VNWorld.hpp
  VoteMode.hpp
  Worlds.hpp
)
list(TRANSFORM header_files PREPEND include/adventure-md/)

set(source_files
  CharacterAnimator.cpp
  ChoiceSystem.cpp
  DialoguePrinter2.cpp
  FontData.cpp
  Game.cpp
  IntroWorld.cpp
  SFXManager.cpp
  SpriteManager.cpp
  TitleWorld.cpp
  Version.cpp
  VNWorld.cpp
  VoteMode.cpp
)
list(TRANSFORM source_files PREPEND src/)

set(resource_files
  fonts.res
  intro.res
  vn.res
  voting.res
)
list(TRANSFORM resource_files PREPEND res/)

macro(make_adventuremd_lib suffix)
  add_library(adventure-md${suffix})
  target_link_libraries(adventure-md${suffix} PRIVATE SGDK::mdcpp)
  set_target_properties(adventure-md${suffix} PROPERTIES SGDK_GENERATED_PREFIX adventure-md)
  target_sources(adventure-md${suffix} PUBLIC FILE_SET HEADERS BASE_DIRS include FILES ${header_files})
  target_sources(adventure-md${suffix} PRIVATE ${source_files})
  md_target_resources(adventure-md${suffix} PUBLIC ${resource_files})
endmacro()

make_adventuremd_lib("")
target_compile_definitions(adventure-md PUBLIC WEB_BUILD=0)

make_adventuremd_lib("_web")
target_compile_definitions(adventure-md_web PUBLIC WEB_BUILD=1)