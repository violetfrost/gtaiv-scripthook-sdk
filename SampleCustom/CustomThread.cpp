#include "CustomThread.h"
#include "Scripting.h"
#include "../ScriptHook/Log.h"

#include <windows.h>

// Pull in all our scripting functions/types
using namespace Scripting;

CustomThread::CustomThread()
{
    // Give your own name here!
	SetName("CustomThread");

	m_State = StateDefault;
}

void CustomThread::RunTick()
{
	// There should be no infinite loops in this function. If you need to wait for something 
	// to become available, set a local static variable and return. Ideally you would implement
	// this is as a state machine as follows:

	switch(m_State)
	{
	case StateDefault:	// Default state... check for key presses, 
						// and do anything that we don't have to wait for
		{
			if ((GetAsyncKeyState(VK_F5) & 1) != 0)
			{
				LogInfo("Spawning a random car");

				Ped ped;
				Vehicle vehicle;
				u32 modelHash;
				ScriptAny unknown;

				Player playerIndex = ConvertIntToPlayerIndex(GetPlayerId());
				GetPlayerChar(playerIndex, &ped);

				f32 x,y,z;
				GetCharCoordinates(ped, &x, &y, &z);

				GetRandomCarModelInMemory(1, &modelHash, &unknown);
				CreateCar(modelHash, x, y, z, &vehicle, true);
			}
			else if ((GetAsyncKeyState(VK_F6) & 1) != 0)
			{
				LogInfo("Granting player $1000");

				Player playerIndex = ConvertIntToPlayerIndex(GetPlayerId());
				AddScore(playerIndex, 1000);
			}
			else if ((GetAsyncKeyState(VK_F7) & 1) != 0)
			{
				LogInfo("Requested a MODEL_BANSHEE spawn");

				m_CarModel = MODEL_BANSHEE;

				RequestModel(m_CarModel);

				// Change our state so that we can spawn the car when available
				m_State = StateSpawnCar;
			}	
			else if ((GetAsyncKeyState(VK_F8) & 1) != 0)
			{
				LogInfo("Changing the player skin");

				m_SkinModel = MODEL_F_Y_CDRESS_01;

				RequestModel(m_SkinModel);

				// Change our state so that we can change skin when available
				m_State = StateChangeSkin;
			}
			else if ((GetAsyncKeyState(VK_F9) & 1) != 0)
			{
				LogInfo("Changing the player skin back to Niko's");

				m_SkinModel = MODEL_PLAYER;

				RequestModel(m_SkinModel);

				// Change our state so that we can change skin when available
				m_State = StateChangeSkin;
			}
		}
		break;

	case StateSpawnCar:	// Spawn the car model when its available
		{
			if (HasModelLoaded(m_CarModel))
			{
				LogInfo("Car model available... spawning it!");

				Ped ped;
				Vehicle vehicle;
				f32 x,y,z;

				Player playerIndex = ConvertIntToPlayerIndex(GetPlayerId());
				GetPlayerChar(playerIndex, &ped);

				GetCharCoordinates(ped, &x, &y, &z);

				CreateCar(m_CarModel, x, y, z, &vehicle, true);

				MarkModelAsNoLongerNeeded(m_CarModel);

				m_State = StateDefault;
			}
		}
		break;

	case StateChangeSkin:	// Change the player skin when its available
		{
			if (HasModelLoaded(m_SkinModel))
			{
				LogInfo("Skin model available... spawning it!");

				Player playerIndex = ConvertIntToPlayerIndex(GetPlayerId());

				eInteriorRoomKey roomKey;
				Ped ped;

				GetPlayerChar(playerIndex, &ped);
				GetKeyForCharInRoom(ped, &roomKey);

				ChangePlayerModel(playerIndex, m_SkinModel);

				GetPlayerChar(playerIndex, &ped);
				SetRoomForCharByKey(ped, roomKey);

				MarkModelAsNoLongerNeeded(m_SkinModel);

				m_State = StateDefault;
			}
		}
		break;

	}


}
