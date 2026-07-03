#include "Includes.h"
#include "../../features/features_includes.h"

using namespace MecchaCheatV;

void Hooks::HkProcessEvent(SDK::UObject* Object, SDK::UFunction* Function, void* Params)
{
	LOG_CALL_UPDATE("Called ProcessEvent");

	static thread_local bool is_executing_hook = false;
	if (is_executing_hook)
	{
		return Globals::hookedProcessEvent(Object, Function, Params);
	}

    if (Globals::ForTests)
    {
        Globals::ForTests = false;
		NOTIFY_INFO_QUICK("Test called!");

		/*
		auto player = reinterpret_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_C*>(Utils::GetAcknowledgedPawn());

		auto fn = player->Class->GetFunction(
			"BP_FirstPersonCharacter_cLeon_Character_C",
			"Provocation(Server)"
		);

		if (fn)
			player->ProcessEvent(fn, nullptr);
		*/
    }

	is_executing_hook = true;

	CALL_METHOD_IF_ACTIVE(Player, SprintMultiplier, SprintMultiplierHandler);
	CALL_METHOD_IF_ACTIVE(Player, NoDetection, NoDetectionHandler);

	is_executing_hook = false;

	if (Function)
	{
		std::string fname = Function->GetName();
		std::string outer = Function->Outer ? Function->Outer->GetName() : std::string();

		if (Globals::needTeleport && Utils::GetAcknowledgedPawn() && Utils::isObjectValid(Utils::GetAcknowledgedPawn()))
			Utils::ProcessTeleport(Utils::GetAcknowledgedPawn());

		if (fname == "OnRep_BodyVisibility")
			CALL_METHOD_IF_ACTIVE_ARGS(Player, AlwaysVisible, AlwaysVisibleHandle, Object);
		
        if (fname == "PaintTick")
			CALL_METHOD_IF_ACTIVE_ARGS(Player, AutoDissShadow, AutoDissShadowHandle, Object);
	}

	Globals::hookedProcessEvent(Object, Function, Params);
}