#include "Includes.h"

using namespace MecchaCheatV;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall Hooks::HkWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) // add support Unity 6000
{
    /*
    POINT mPos;
    GetCursorPos(&mPos);
    ScreenToClient(Renderer::Window, &mPos);

    ImGui::GetIO().MousePos.x = mPos.x;
    ImGui::GetIO().MousePos.y = mPos.y;
    */

    if (menu.Open)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
            return true;
    }

    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
    {
        if (wParam == MenuToggleKey)
            menu.Toggle();

        if (wParam == VK_END)
        {
            NOTIFY_WARNING_QUICK("Emergency unload...");
            Globals::CheatWork = false;
        }

        if (wParam == VK_HOME)
        {
            Globals::MenuToggleKey = VK_RETURN;
            NOTIFY_INFO_QUICK("Menu key reset to ENTER.");
        }
    }

    return CallWindowProc(hooking->OriginalWndproc, hWnd, uMsg, wParam, lParam);
}