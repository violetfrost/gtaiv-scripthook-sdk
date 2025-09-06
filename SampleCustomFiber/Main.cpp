#include <windows.h>

#include "CustomFiberThread.h"

#include "../ScriptHook/ScriptHookManager.h"

static CustomFiberThread _customThread;

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH: 
        {
			ScriptHookManager::RegisterThread( &_customThread );

            break;
        }
        case DLL_PROCESS_DETACH:
        {
            break;
        }
    }

    return TRUE;
}