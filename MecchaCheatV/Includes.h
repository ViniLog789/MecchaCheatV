#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <atomic>
#include <d3d11.h>
#include <dxgi.h>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <filesystem>
#include <random>
#include <chrono>
#include <thread>

#include "Globals.h"
#include "main/sdk/SDK.hpp"
#include "main/logger/logger.h"
#include "main/utils/utils.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h"
#include "detours/include/detours.h"
#include "nlohmann/json.hpp"
#include "main/menu/renderer.h"

#include "main/utils/utils.h"
#include "main/config/config.h"
#include "main/utils/notifications.h"

#include "main/menu/menu.h"
#include "main/features/settings.h"
#include "main/features/features.h"
#include "main/hooking/hooks.h"
#include "main/hooking/hooking.h"