# build CommonClass static library
cmake_minimum_required(VERSION 3.7.1)

set (FAST_COMMAND_PROMPT_SOURCE_FILES
	App.cpp
	FastCommandPrompt.cpp
	ImguiWrapImageDX11.cpp
	${MY_LIBRARY_PATH}/imgui/imgui.cpp
	${MY_LIBRARY_PATH}/imgui/imgui_demo.cpp
	${MY_LIBRARY_PATH}/imgui/imgui_draw.cpp
	${MY_LIBRARY_PATH}/imgui/examples/imgui_impl_dx11.cpp
	${MY_LIBRARY_PATH}/imgui/examples/imgui_impl_win32.cpp)
	
# set the CommonClass directory
add_executable(FastCommandPrompt ${FAST_COMMAND_PROMPT_SOURCE_FILES})

find_library(LUA_LIB
	NAMES lua
	PATHS "${MY_LIBRARY_PATH}/Lua/Lua-5.3.4/dll")
	
target_link_libraries(FastCommandPrompt ${LUA_LIB})
target_include_directories(FastCommandPrompt PRIVATE ${MY_LIBRARY_PATH}/imgui)

# copy needed scripts to the output path of FastCommandPrompt
get_target_property(
	CONST_FAST_COMMAND_PROMPT_OUTPUT_PATH
	FastCommandPrompt
	RUNTIME_OUTPUT_DIRECTORY
	)
message(STATUS "the output path of " ${FastCommandPrompt} " is " ${CONST_FAST_COMMAND_PROMPT_OUTPUT_PATH})
message(STATUS " build type is  " ${CMAKE_BUILD_TYPE})

file(
	COPY initialize.lua controller.lua clipboard.dll
	DESTINATION ${CONST_FAST_COMMAND_PROMPT_OUTPUT_PATH}
)

message(STATUS "source files = " "${FAST_COMMAND_PROMPT_SOURCE_FILES}")
