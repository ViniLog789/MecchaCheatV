#pragma once
#include "Includes.h"
#include "widgets/HostOnly.h"

namespace MecchaCheatV
{
	class Menu
	{
	public:
		static void Initialize();
		static void NewYear();
		static void Render();
		static void Toggle();

		static inline bool Open = false;
		static inline bool Initialized = false;
		static inline int currentTab = 0;
	};

	inline Menu menu;
}