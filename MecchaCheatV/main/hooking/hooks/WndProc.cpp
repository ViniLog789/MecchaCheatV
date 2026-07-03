#include "Includes.h"

using namespace MecchaCheatV;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall Hooks::HkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
    {
        if (wParam == MenuToggleKey)
        {
            menu.Toggle();
            return TRUE;
        }

        if (wParam == VK_END)
        {
            NOTIFY_WARNING_QUICK("Emergency unload...");
            Globals::CheatWork = false;
            return TRUE;
        }

        if (wParam == VK_HOME)
        {
            Globals::MenuToggleKey = VK_RETURN;
            NOTIFY_INFO_QUICK("Menu key reset to ENTER.");
            return TRUE;
        }

		if (wParam == VK_DELETE)
		{
            Globals::IsDebugging = true;
            Globals::IsCalledLogs = true;
			NOTIFY_INFO_QUICK("Test mode and called logs switched.");
			return TRUE;
		}
    }

    if (menu.Open)
    {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

        switch (uMsg)
        {
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_INPUT:
            return TRUE;
        }
    }

    return CallWindowProc(hooking->OriginalWndproc, hWnd, uMsg, wParam, lParam);
}