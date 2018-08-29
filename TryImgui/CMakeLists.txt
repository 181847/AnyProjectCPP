# build CommonClass static library
cmake_minimum_required(VERSION 3.7.1)

set (TRY_IMGUI
	TryImgui.cpp
	${MY_LIBRARY_PATH}/imgui/imgui.cpp
	${MY_LIBRARY_PATH}/imgui/imgui_demo.cpp
	${MY_LIBRARY_PATH}/imgui/imgui_draw.cpp
	${MY_LIBRARY_PATH}/imgui/examples/imgui_impl_dx11.cpp
	${MY_LIBRARY_PATH}/imgui/examples/imgui_impl_win32.cpp)
	
# set the CommonClass directory
add_executable(TryImgui ${TRY_IMGUI})
	
target_include_directories(TryImgui PRIVATE ${MY_LIBRARY_PATH}/imgui)

message(STATUS "source files = " "${TRY_IMGUI}")
