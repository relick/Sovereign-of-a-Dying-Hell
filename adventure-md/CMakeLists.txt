cmake_minimum_required(VERSION 4.0)

project(Adventure-MD C CXX ASM)

find_package(SGDK REQUIRED)

add_library(adventure-md)
target_link_libraries(adventure-md PRIVATE SGDK::mdcpp)

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
target_sources(adventure-md PUBLIC FILE_SET HEADERS BASE_DIRS include FILES ${header_files})

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
target_sources(adventure-md PRIVATE ${source_files})

set(resource_files
  fonts.res
  intro.res
  vn.res
  voting.res
)
list(TRANSFORM resource_files PREPEND res/)
md_target_resources(adventure-md PUBLIC ${resource_files})