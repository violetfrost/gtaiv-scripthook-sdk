#include "CustomFiberThread.h"
#include "Scripting.h"
#include "../ScriptHook/Log.h"

#include <windows.h>

// Pull in all our scripting functions/types
using namespace Scripting;

CustomFiberThread::CustomFiberThread()
{
    // Give your own name here!
	SetName("CustomFiberThread");
}

// Some helper functions

Player CustomFiberThread::GetPlayer()
{
	Player playerIndex = ConvertIntToPlayerIndex(GetPlayerId());
	return playerIndex;
}

Scripting::Ped CustomFiberThread::GetPlayerPed()
{
	Ped ped;
	GetPlayerChar(GetPlayer(), &ped);
	return ped;
}


void CustomFiberThread::SpawnCar(eModel model)
{
	RequestModel(model);

	while(!HasModelLoaded(model))
	{
		Wait(0);
	}

	LogInfo("Car model available... spawning it!");

	Ped ped = GetPlayerPed();

	Vehicle vehicle;
	f32 x,y,z;

	GetCharCoordinates(ped, &x, &y, &z);

	CreateCar(model, x, y, z, &vehicle, true);

	MarkModelAsNoLongerNeeded(model);

}

void CustomFiberThread::ChangePlayerSkin(eModel model)
{
	RequestModel(model);

	while(!HasModelLoaded(model))
	{
		Wait(0);
	}

	LogInfo("Skin model available... spawning it!");

	eInteriorRoomKey roomKey;
	
	GetKeyForCharInRoom(GetPlayerPed(), &roomKey);

	ChangePlayerModel(GetPlayer(), model);

	SetRoomForCharByKey(GetPlayerPed(), roomKey);

	MarkModelAsNoLongerNeeded(model);

}

void CustomFiberThread::TeleportToWaypoint(Scripting::Ped &ped)
{
    Blip b = GetFirstBlipInfoId(BLIP_WAYPOINT);
    if(b.IsValid())
    {
        Vector3 v;
        GetBlipCoords(b, &v);

        LogInfo("Teleporting to %f, %f", v.X, v.Y);

        // Thanks to Prince-Link for this magical Z coord detection code...
        SetCharCoordinates(ped, v.X, v.Y, v.Z);
        while(v.Z == 0.0f) // The chance that ground Z is 0.0 _exactly_ is really small
        {
            GetGroundZFor3DCoord(v.X, v.Y, 1000, &v.Z);
            Wait(0);
        }

        SetCharCoordinates(ped, v.X, v.Y, v.Z);
    }
    else
    {
        LogError("No way point found to teleport to.");
    }

}


// The real script

void CustomFiberThread::RunScript()
{
	// This is a fiber thread, so we use an loop to run the contents of this script.
	// The thread will terminate when we return from this function.

	while(IsThreadAlive())
	{

        if ((GetAsyncKeyState(VK_F4) & 1) != 0)
        {
            LogInfo("Teleporting player to way point");

            TeleportToWaypoint( GetPlayerPed() );
        }
		else if ((GetAsyncKeyState(VK_F5) & 1) != 0)
		{
			LogInfo("Spawning a random car");

			Vehicle vehicle;
			u32 modelHash;
			ScriptAny unknown;

			f32 x,y,z;
			GetCharCoordinates(GetPlayerPed(), &x, &y, &z);

			GetRandomCarModelInMemory(1, &modelHash, &unknown);
			CreateCar(modelHash, x, y, z, &vehicle, true);
		}
		else if ((GetAsyncKeyState(VK_F6) & 1) != 0)
		{
			LogInfo("Granting player $1000");

			AddScore(GetPlayer(), 1000);
		}
		else if ((GetAsyncKeyState(VK_F7) & 1) != 0)
		{
			LogInfo("Requested a MODEL_BANSHEE spawn");

			SpawnCar(MODEL_BANSHEE);
		}
		else if ((GetAsyncKeyState(VK_F8) & 1) != 0)
		{
			LogInfo("Changing the player skin");

			ChangePlayerSkin(MODEL_IG_JOHNNYBIKER);
		}
		else if ((GetAsyncKeyState(VK_F9) & 1) != 0)
		{
			LogInfo("Changing the player skin back to Niko's");

			ChangePlayerSkin(MODEL_PLAYER);
		}

		// Call Wait() so we can process other scripts/game code
		// You must call Wait(...) in your loop code for a fiber thread!
		Wait(100);

	}


}
