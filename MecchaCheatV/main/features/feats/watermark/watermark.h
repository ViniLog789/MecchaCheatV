#pragma once
#include "Includes.h"

namespace MecchaCheatV::Features::Visuals
{
	class Watermark : public FeatureCore
	{
	public:
		Watermark();
		~Watermark() override = default;

		void OnActivate() override {};
		void OnDeactivate() override {};
		void OnRender() override;
		void OnMenuRender() override;

	private:
		std::string CachedText;
	};
}