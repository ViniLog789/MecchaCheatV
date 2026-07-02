#pragma once
#include "Includes.h"

namespace MecchaCheatV::Features::Player
{
	class AlwaysVisible : public FeatureCore // for some reason, i associate this name with love
	{
	public:
		AlwaysVisible();
		~AlwaysVisible() override = default;

		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnRender() override {}
		void OnMenuRender() override;
		void AlwaysVisibleHandle(SDK::UObject* Object);
	};
}