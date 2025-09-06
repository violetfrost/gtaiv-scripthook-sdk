#include <windows.h>
#include "SpeedoThread.h"
#include "../ScriptHook/ScriptHookManager.h"
#include "../ScriptHook/Version.h"

static SpeedoThread _speedoThread;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH: 
        {
            // Sample version independent version checking (for version 0.3 and newer)
            if (ScriptHookManager::GetScriptHookVersion() < ScriptHookVersion)
            {
                const ch *szMessage = "The Speedometer mod requires ScriptHook.dll version "
                    ScriptHookVersionIdentifier " or newer in order to function.";
                MessageBoxA( NULL, szMessage, "Version Mismatch", MB_OK | MB_ICONEXCLAMATION );
                return FALSE;
            }

            // Pass on our module handle to the thread for loading resources, etc
            _speedoThread.SetModule( hModule );

			ScriptHookManager::RegisterThread( &_speedoThread );

            break;
        }
        case DLL_PROCESS_DETACH:
        {
            break;
        }
    }

    return TRUE;
}