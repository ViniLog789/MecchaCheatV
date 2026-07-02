#pragma once
#include "Includes.h"

namespace MecchaCheatV::Features::Player
{
	class FOVChanger : public FeatureCore
	{
	public:
		FOVChanger();
		~FOVChanger() override = default;
		void OnActivate() override;
		void OnDeactivate() override;
		void OnRender() override {}
		void OnMenuRender() override;
	private:
		bool SetFOV(float newFov) {
			auto aPlayerController = Utils::GetPlayerController();
			if (!aPlayerController || !aPlayerController)
				return false;

			aPlayerController->FOV(newFov);
			return true;
		}

		bool resetDefault() { return SetFOV(90.0f); }
	};
}