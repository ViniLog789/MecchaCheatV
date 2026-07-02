#pragma once
#include "Includes.h"

namespace MecchaCheatV::Features::Visuals
{
	class PlayerESP : public FeatureCore
	{
	public:
		PlayerESP();
		~PlayerESP() override = default;

		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnRender() override;
		void OnMenuRender() override;

		bool DrawBox(SDK::AActor* actor, float& outMinX, float& outMinY, float& outMaxX, float& outMaxY);
		void DrawSkeleton(SDK::USkeletalMeshComponent* Mesh, SDK::APlayerController* PC, ImDrawList* drawList, ImU32 color, float thickness);
		void DrawCornerBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness);
		void DrawFullBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness);
		void DrawRoundedBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness);
		ImU32 GetPlayerColor(SDK::AActor* actor, bool isHunter, bool isVisible);
		void DrawSnapline(ImDrawList* drawList, const SDK::FVector2D& target, ImU32 color, float thickness);
		std::string GetRoleText(SDK::AActor* actor);
	};
}