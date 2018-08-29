# build CommonClass static library
cmake_minimum_required(VERSION 3.7.1)

set (DEVELOP_SUIT_AND_CASE
	DevelopSuitAndCase.cpp)

# set the CommonClass directory
add_executable(DevelopSuitAndCase ${DEVELOP_SUIT_AND_CASE})

message(STATUS "source files = " "${DEVELOP_SUIT_AND_CASE}")
