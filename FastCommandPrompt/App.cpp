#include "App.h"
#include <stdio.h>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#pragma comment(lib, "lua.lib")

namespace App
{

unsigned int MainWindowWidth = 720;
unsigned int MainWindowHeight = 720;
char StatusText[255];

struct MinimalLuaInpterpreter
{
public:
    lua_State* MainLuaState = nullptr;

    MinimalLuaInpterpreter()
    {
        if (MainLuaState = luaL_newstate())
        {
            luaL_openlibs(MainLuaState);
        }
        
    }
    MinimalLuaInpterpreter(const MinimalLuaInpterpreter&) = delete;
    MinimalLuaInpterpreter(const MinimalLuaInpterpreter&&) = delete;
    MinimalLuaInpterpreter& operator = (const MinimalLuaInpterpreter&) = delete;

    /*!
        \brief push one string and execute the corresponding command
        \return 1 for error
    */
    int DoCommand(const char * pCommandStr)
    {
        if (!MainLuaState)
        {
            sprintf_s(StatusText, "lua state is nullptr, DoCommand failed.");
            return 1;
        }
        if (!lua_pushstring(MainLuaState, pCommandStr))
        {
            sprintf_s(StatusText, "push command to lua_state failed");
            return 1;
        }

        lua_setglobal(MainLuaState, "command");

        if (luaL_dofile(MainLuaState, "controller.lua"))
        {
            sprintf_s(StatusText, "do file failed");
            return 1;
        }

        return 0;
    }
};


int EditTextCallBack(ImGuiTextEditCallbackData* data)
{
    printf("edit text callback: %s, \t last character is %c\n", data->Buf, data->EventChar);
    return 0;
}

int Main()
{
    static bool show_demo_window = true;
    static bool show_another_window = false;
    static bool show_app_main_window = true;
    static float color[3];

    static float f = 0.0f;
    static int counter = 0;

    static MinimalLuaInpterpreter LuaInterpreter;

    // force the main imgui window fill the native window.
    ImGuiWindowFlags mainAppWndFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f)); 
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(MainWindowWidth), static_cast<float>(MainWindowHeight)));
    ImGui::Begin("Hello, world!", &show_app_main_window/* indicating no close */, mainAppWndFlags);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    static char InputBuf[255];
    ImGui::SetKeyboardFocusHere();
    if (ImGui::InputText("input your text", InputBuf, sizeof(InputBuf), 0, EditTextCallBack))
    {
        printf("deal with input: %s\n", InputBuf);
        LuaInterpreter.DoCommand(InputBuf);
    }
    ImGui::Text(StatusText);
    ImGui::End();

    return 0;
}

}// namespace App
