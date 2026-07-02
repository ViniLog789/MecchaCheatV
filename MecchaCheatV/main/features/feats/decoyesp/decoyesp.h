#pragma once
#include "Includes.h"

namespace MecchaCheatV::Features::Visuals
{
	class DecoyESP : public FeatureCore
	{
	public:
		DecoyESP();
		~DecoyESP() override = default;

		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnRender() override;
		void OnMenuRender() override;

	private:
		bool DrawBox(SDK::AActor* actor, float& outMinX, float& outMinY, float& outMaxX, float& outMaxY);
		void DrawCornerBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness);
		void DrawFullBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness);
		void DrawRoundedBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness);
		void DrawSnapline(ImDrawList* drawList, const SDK::FVector2D& target, ImU32 color, float thickness);
		ImU32 GetDecoyColor();
	};
}