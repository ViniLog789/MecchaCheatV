#pragma once
#include "Includes.h"

namespace MecchaCheatV::Features::Player
{
	class SetName : public FeatureCore
	{
	public:
		SetName();
		~SetName() override = default;
		void OnActivate() override;
		void OnDeactivate() override;
		void OnRender() override {}
		void OnMenuRender() override;
	private:
		void SetPlayerName(const std::string& newName) { if (Utils::GetAcknowledgedPawn()) Utils::ChangeName(Utils::GetAcknowledgedPawn(), newName); }
		std::string GetPlayerName() { if (Utils::GetAcknowledgedPawn()) return Utils::GetAcknowledgedPawn()->GetName(); return ""; }
		std::string origName = "";
		bool nameChanged = false;
	};
}