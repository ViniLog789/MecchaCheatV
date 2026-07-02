#include "Includes.h"
#include "../../features/features_includes.h"

using namespace MecchaCheatV;

void Hooks::HkProcessEvent(SDK::UObject* Object, SDK::UFunction* Function, void* Params)
{
	LOG_CALL_UPDATE("Called ProcessEvent");

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