#include "SpeedoThread.h"
#include "../ScriptHook/Log.h"
#include "../ScriptHook/ScriptHookManager.h"

#include "../ScriptHook/Services/Menu.h"
#include "../ScriptHook/Services/KeyboardHook.h"

using namespace Scripting;

SpeedoThread::SpeedoThread()
{
    // Give your own name here!
	SetName("Speedometer");
    
    m_pMenu = NULL;
    m_bEnable = true;
}

Player SpeedoThread::GetPlayer()
{
	Player playerIndex = ConvertIntToPlayerIndex(GetPlayerId());
	return playerIndex;
}

Ped SpeedoThread::GetPlayerPed()
{
	Ped ped;
	GetPlayerChar(GetPlayer(), &ped);
	return ped;
}

f32 SpeedoThread::GetPedCarSpeed(Ped p)
{
    Vehicle v;
    GetCarCharIsUsing(p, &v);

    f32 speed;
    GetCarSpeed(v, &speed);

    return speed;
}

void SpeedoThread::CreateMenu()
{
    // Create a menu!
    IMenuService *menuService = ScriptHookManager::RequestService<IMenuService>( "Menu" );
    m_pMenu = menuService->CreateMenu();
    
    m_pMenu->SetTitle("Speedometer");

    m_pMenu->AddItem(MenuEnableDisable, m_bEnable ? "Disable Speedometer" : "Enable Speedometer" );

    m_pMenu->AddItem(MenuChangeUnits, "Change Units");
    m_pMenu->SetTitle(MenuChangeUnits, "Speedometer - Change Units");
    m_pMenu->AddItem(MenuChangeUnits, MenuChangeUnitsMPH, "Miles Per Hour (MPH)");
    m_pMenu->AddItem(MenuChangeUnits, MenuChangeUnitsKPH, "Kilometers Per Hour (KPH)");

    m_pMenu->AddItem(MenuAbout, "About");
    m_pMenu->SetTitle(MenuAbout, "Speedometer - About");
    m_pMenu->AddItem(MenuAbout, MenuAboutLine1, "Speedometer Sample Mod");
    m_pMenu->AddItem(MenuAbout, MenuAboutLine2, "Copyright (C) 2009, aru");

    m_pMenu->SetEventHandler( this );
}

void SpeedoThread::OnMenuSelected( IMenu *menu, u32 id )
{
    if ( id == MenuChangeUnitsKPH )
    {
        m_Settings.SetDisplayUnit( Settings::UnitKPH );
        menu->SetCurrentParent( IMenu::RootMenuId );
        
        m_Settings.Save();
    }
    else if ( id == MenuChangeUnitsMPH )
    {
        m_Settings.SetDisplayUnit( Settings::UnitMPH );
        menu->SetCurrentParent( IMenu::RootMenuId );

        m_Settings.Save();
    }
    else if ( id == MenuEnableDisable )
    {
        m_bEnable = !m_bEnable;
        m_pMenu->SetItem(MenuEnableDisable, m_bEnable ? "Disable Speedometer" : "Enable Speedometer" );
    }
}

void SpeedoThread::OnKeyboardHookEvent( const IKeyboardHookHandler::KeyEventArgs &args )
{
    // NOTE:
    // Keyboard hook events are fired in a system thread that may not necessarily be
    // the same as the script running system thread. You should really not call any natives 
    // in here. Instead, set a variable flag here and process the logic for the key event 
    // in the RunScript() function.

    // Show the menu on ALT+F10
    if (
          !args.WasKeyDownBefore  /* key was just pressed down */
          && args.IsAltPressed
          && args.VirtualKey == VK_F10
       )
    {
        LogDebug("Keyboard Event: F10");

        // Show the menu
        m_pMenu->Show();
    }
}

void SpeedoThread::RunScript()
{
    const f32 mphConversion = 2.2369f; // m/s -> mph
    const f32 kphConversion = 3.6f; // m/s -> kph


    // Load some settings
    m_Settings.Initialize();

    m_SpeedoRender.SetSettings( &m_Settings );

    // Attach our renderer hook
    m_SpeedoRender.InstallHook();

    // Create our Menu
    CreateMenu();

    // Install a keyboard hook to detect when to load the menu
    // (This is overkill, but this is just as a sample)
    IKeyboardHookService *kbhService = 
        ScriptHookManager::RequestService<IKeyboardHookService>( "KeyboardHook" );
    kbhService->AddHandler( this );

	while(IsThreadAlive())
	{
        if (m_bEnable)
        {
            Ped p = GetPlayerPed();
            if (IsCharInAnyCar(p))
            {
                f32 speed = GetPedCarSpeed(p);

                if (m_Settings.GetDisplayUnit() == Settings::UnitKPH)
                {
                    speed *= kphConversion;
                }
                else
                {
                    speed *= mphConversion;
                }

                if (speed > 200.0f) speed = 200.0f;

                m_SpeedoRender.SetCurrentSpeed( speed );
                m_SpeedoRender.SetVisible( true );
            }
            else
            {
                m_SpeedoRender.SetVisible( false );
                m_SpeedoRender.SetCurrentSpeed( 0.0f );
            }
        }
        else
        {
            m_SpeedoRender.SetVisible( false );
            m_SpeedoRender.SetCurrentSpeed( 0.0f );            
        }

		Wait(100);
	}
}

void SpeedoThread::OnKill()
{
    // Remove our keyboard hook
    IKeyboardHookService *kbhService = 
        ScriptHookManager::RequestService<IKeyboardHookService>( "KeyboardHook" );
    kbhService->RemoveHandler( this );

    // Release the menu if one has been made
    if (m_pMenu)
    {
        m_pMenu->Release();
        m_pMenu = NULL;
    }

    // Remove the render hook
    m_SpeedoRender.RemoveHook();

    ScriptThread::OnKill();
}
