#pragma once
#include "Includes.h"

namespace MecchaCheatV
{
	struct Hooks
	{
		// Hook menu
		static HRESULT HkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
		static LRESULT HkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
		// others hooks
		static void HkProcessEvent(SDK::UObject* Object, SDK::UFunction* Function, void* Params);
	};
}
