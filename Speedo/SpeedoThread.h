#pragma once

#include "../ScriptHook/ScriptThread.h"
#include "Scripting.h"
#include "SpeedoRender.h"
#include "Settings.h"

#include "../ScriptHook/Services/Menu.h"
#include "../ScriptHook/Services/KeyboardHook.h"

class SpeedoThread : 
    public ScriptThread,
    public IKeyboardHookHandler,        // To handle keyboard hook events
    public IMenuHandler                 // To handle menu events (These two could go in a separate class as well)
{
private:
    Scripting::Player GetPlayer();
    Scripting::Ped GetPlayerPed();
    f32 GetPedCarSpeed(Scripting::Ped p);

    SpeedoRender m_SpeedoRender;
    Settings m_Settings;
    b8 m_bEnable;

    // Menu stuff
    enum
    {
        MenuEnableDisable,

        MenuChangeUnits,
        MenuChangeUnitsMPH,
        MenuChangeUnitsKPH,
        
        MenuAbout,
        MenuAboutLine1,
        MenuAboutLine2,
    };
    IMenu *m_pMenu;
    void CreateMenu();
    void OnMenuParentChanged( IMenu *menu, u32 parentId ) { /* dont care */ }
    void OnMenuSelectionChanged( IMenu *menu, u32 id ) { /* dont care */ }
    void OnMenuSelected( IMenu *menu, u32 id );

    // Keyboard hook stuff
    void OnKeyboardHookEvent( const IKeyboardHookHandler::KeyEventArgs &args );

protected:
    void RunScript();
    void OnKill();

public:
    SpeedoThread();

    void SetModule( HMODULE hModule )
    {
        m_SpeedoRender.SetResourceModule( hModule );
        m_Settings.SetModule( hModule );
    }
};