#include "Includes.h"

using namespace MecchaCheatV;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall Hooks::HkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (menu.Open)
    {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

        switch (uMsg)
        {
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            return TRUE;
        }
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