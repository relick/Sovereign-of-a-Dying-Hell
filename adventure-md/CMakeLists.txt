cmake_minimum_required(VERSION 4.0)

project(Adventure-MD C CXX ASM)

find_package(SGDK REQUIRED)

add_library(adventure-md)
target_link_libraries(adventure-md PRIVATE SGDK::mdcpp)
target_sources(adventure-md
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES
    include/adventure-md/CharacterAnimator.hpp
    include/adventure-md/CharacterData.hpp
    include/adventure-md/ChoiceSystem.hpp
    include/adventure-md/Constants.hpp
    include/adventure-md/Coroutine.hpp
    include/adventure-md/Debug.hpp
    include/adventure-md/Declare.hpp
    include/adventure-md/DialoguePrinter2.hpp
    include/adventure-md/FadeOps.hpp
    include/adventure-md/FadeOps.inl
    include/adventure-md/FontData.hpp
    include/adventure-md/Game.hpp
    include/adventure-md/GameRoutines.hpp
    include/adventure-md/IntroWorld.hpp
    include/adventure-md/PaletteOps.hpp
    include/adventure-md/PaletteOps.inl
    include/adventure-md/Scene.hpp
    include/adventure-md/SceneDefines.hpp
    include/adventure-md/Script.hpp
    include/adventure-md/SFXManager.hpp
    include/adventure-md/SpriteManager.hpp
    include/adventure-md/TileData.hpp
    include/adventure-md/TileOps.hpp
    include/adventure-md/TitleWorld.hpp
    include/adventure-md/TypeSafeID.hpp
    include/adventure-md/Version.hpp
    include/adventure-md/VNWorld.hpp
    include/adventure-md/VoteMode.hpp
    include/adventure-md/Worlds.hpp
)
target_sources(adventure-md
  PRIVATE
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
)
md_target_resources(adventure-md
  PUBLIC
    res/fonts.res
    res/intro.res
    res/vn.res
    res/voting.res
)