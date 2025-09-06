#pragma once

#include "../ScriptHook/ScriptThread.h"
#include "../ScriptHook/ScriptingEnums.h"

class CustomThread : 
	public ScriptThread
{
private:
	enum eStateMachine
	{
		StateDefault,
		StateSpawnCar,
		StateChangeSkin,
	};

	eStateMachine m_State;
	
	Scripting::eModel m_CarModel;
	Scripting::eModel m_SkinModel;

protected:
    // We want a Tick based script, so we override RunTick.
	void RunTick();

public:
	CustomThread();
};