# WIP: This does naught right now. Keep using the makefile

cmake_minimum_required(VERSION 3.12)
project(Adventure-MD)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(../../SDKs/SGDKplusplus)
# SGDK library exists now

add_executable(ProofOfConcept main.cpp rom_head.c sega.s)
target_link_libraries(ProofOfConcept SGDK)