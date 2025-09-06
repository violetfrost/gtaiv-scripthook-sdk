#pragma once

#include "../ScriptHook/ScriptThread.h"
#include "../ScriptHook/ScriptingEnums.h"
#include "../ScriptHook/ScriptingTypes.h"

class CustomFiberThread : 
	public ScriptThread
{
private:
	Scripting::Player GetPlayer();
	Scripting::Ped GetPlayerPed();

	void SpawnCar(Scripting::eModel model);
	void ChangePlayerSkin(Scripting::eModel model);
    void TeleportToWaypoint(Scripting::Ped &ped);

protected:
    // We don't want a Tick based script, so we override RunScript.
	void RunScript();

public:
	CustomFiberThread();
};