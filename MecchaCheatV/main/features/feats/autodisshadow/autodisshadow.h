#pragma once
#include "Includes.h"

namespace MecchaCheatV::Features::Player
{
	class AutoDissShadow : public FeatureCore
	{
	public:
		AutoDissShadow();
		~AutoDissShadow() override = default;

		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnRender() override {}
		void OnMenuRender() override;
		void AutoDissShadowHandle(SDK::UObject* Object);
	};
}