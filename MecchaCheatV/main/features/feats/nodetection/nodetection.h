#pragma once
#include "Includes.h"

namespace MecchaCheatV::Features::Player
{
	class NoDetection : public FeatureCore
	{
	public:
		NoDetection();
		~NoDetection() override = default;
		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnRender() override {}
		void OnMenuRender() override;
		void NoDetectionHandler();
	};
}