#include "App.h"
#include <stdio.h>
#include <vector>
#include <string>

extern "C"
{
#include <Lua/Lua-5.3.4/src/lua.h>
#include <Lua/Lua-5.3.4/src/lauxlib.h>
#include <Lua/Lua-5.3.4/src/lualib.h>
}

namespace App
{

HWND ConsoleHwnd = 0;
HWND NativeHwnd = 0;
bool IsNativeWindowHiden = false;
bool ToBeHiden = false;
bool NextBeHiden = false;

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

        std::vector<std::string> readyInitScripts;
        readyInitScripts.push_back("initialize.lua");
        readyInitScripts.push_back("../initialize.lua");
        bool isInitSuccess = false;
        // loop all init scripts until success one.
        for (const auto& initScript : readyInitScripts)
        {
            printf("try init with %s\n", initScript.c_str());
            isInitSuccess = ! (luaL_dofile(MainLuaState, initScript.c_str()));
            if (isInitSuccess)
            {
                isInitSuccess = true;
                break;
            }
        }
        
        if (isInitSuccess)
        {
            printf("lua initialize success\n");
        }
        else
        {
            sprintf_s(StatusText, "do file failed");
            printf("load init script failed.!");
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

        
        if (luaL_dostring(MainLuaState, "CallController();"))
        {
            const char * errorMessage = lua_tostring(MainLuaState, -1);
            sprintf_s(StatusText, "do file failed %s", errorMessage);
            printf("DoCommand failed: %s\n", errorMessage);
            return 1;
        }

        sprintf_s(StatusText, "");
        return 0;
    }
};

void HideConsoleWindow()
{
    ShowWindow(ConsoleHwnd, SW_HIDE);
}

void HideNativeWindow()
{
    ImGui::GetIO().InputCharacters[0] = 0;
    if (IsNativeWindowHiden)
    {
        return;
    }

    IsNativeWindowHiden = true;
    // hide current window
    ShowWindow(NativeHwnd, SW_HIDE);

    // focus on the last window.
    HWND nextWnd = GetWindow(NativeHwnd, GW_HWNDNEXT);

    while (true)
    {
        HWND temp = GetParent(nextWnd);
        if (temp == 0) break;
        nextWnd = temp;
    }

    //SetFocus(hwndc);
    SetForegroundWindow(nextWnd);
    SetCapture(nextWnd);
    SetFocus(nextWnd);
    SetActiveWindow(nextWnd);
    EnableWindow(nextWnd, TRUE);
}

void ShowNativeWindow()
{
    if (IsNativeWindowHiden == false)
    {
        return;
    }

    IsNativeWindowHiden = false;
    // show this window
    ShowWindow(NativeHwnd, SW_NORMAL);
    //SetFocus(hwnd);
    SetActiveWindow(NativeHwnd);
    SetForegroundWindow(NativeHwnd);
    SetCapture(NativeHwnd);
    SetFocus(NativeHwnd);
    SetActiveWindow(NativeHwnd);
    EnableWindow(NativeHwnd, TRUE);
}

void ToggleNativeWindow()
{
    if (IsNativeWindowHiden)
    {
        ShowNativeWindow();
    }
    else
    {
        HideNativeWindow();
    }
}

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
    if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive())
    {
        ImGui::SetKeyboardFocusHere();
    }
    ImGui::InputText("input your text", InputBuf, sizeof(InputBuf));

    if (ImGui::IsKeyReleased(VK_RETURN))
    {
        printf("deal with input: %s\n", InputBuf);
        if (InputBuf[0] != '\0')
        {
            LuaInterpreter.DoCommand(InputBuf);
            App::HideNativeWindow();
        }
    }
    ImGui::Text(StatusText);

    return 0;
}

}// namespace App
