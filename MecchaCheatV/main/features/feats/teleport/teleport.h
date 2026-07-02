#pragma once
#include "Includes.h"

namespace MecchaCheatV::Features::Player
{
	class Teleport : public FeatureCore
	{
	public:
		Teleport();
		~Teleport() override = default;

		void OnActivate() override;
		void OnDeactivate() override;
		void OnRender() override {}
		void OnMenuRender() override;
	private:
		std::vector<SDK::APawn*> cachedPlayers;
		int selectedPlayerIndex = -1;
		void UpdatePlayers();
	};
}