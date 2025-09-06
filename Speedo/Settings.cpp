#include "Settings.h"
#include "../ScriptHook/ScriptHookManager.h"
#include "../ScriptHook/Services/Config.h"

void Settings::Initialize()
{
    // Determine our current path, and thus the config path
    GetModuleFileNameA( m_hModule, m_szConfigPath, sizeof(m_szConfigPath) );
    
    // Change the extension from .dll or .asi to .config
    u32 i = strlen(m_szConfigPath);
    while( i > 0 && m_szConfigPath[i] != '.' ) i--;
    strcpy_s( m_szConfigPath+i, sizeof(m_szConfigPath)-i, ".config" );

    // Create a config
    IConfigService *configService = ScriptHookManager::RequestService<IConfigService>("Config");
    IConfig *config = configService->Create(IConfigService::ConfigTypeXml);

    // Set some defaults
    config->Set("speedo", "units", "mph");
    config->Set("speedo", "force_wide", 0);
    config->Set("speedo", "force_wide_multiplier", 0.83f);

    // See if the config file exists, and if so, load it!
    WIN32_FIND_DATAA wfd;
    if ( FindFirstFileA(m_szConfigPath, &wfd) != INVALID_HANDLE_VALUE )
    {
        config->Load( m_szConfigPath );
    }

    // Convert our config values into settings value... this speeds
    // up calls to retrieve the values later on (i.e. no string conversion)

    const ch *units = config->GetString("speedo", "units");
    m_DisplayUnit = (strcmp(units, "kph") == 0) ? UnitKPH : UnitMPH;   // By default we shall use mph

    m_bForceWide = config->GetInteger("speedo", "force_wide") == 1;

    m_fForceWideMultiplier = config->GetFloat("speedo", "force_wide_multiplier");

    // Release the config (not delete!). We didn't create it, we shouldn't delete it.
    // Note that we don't ever delete/release configService... the lifetime of configService
    // is managed by the service provider... not the consumer.
    config->Release();
}

void Settings::Save()
{
    // We continue here with the assumption that Initialize() has been called
    
    // Create a config
    IConfigService *configService = ScriptHookManager::RequestService<IConfigService>("Config");
    IConfig *config = configService->Create(IConfigService::ConfigTypeXml);

    // Set our values
    config->Set( "speedo", "units",  m_DisplayUnit == UnitKPH ? "kph" : "mph" );
    config->Set( "speedo", "force_wide",  m_bForceWide ? 1 : 0 );
    config->Set( "speedo", "force_wide_multiplier", m_fForceWideMultiplier );

    // Save
    config->Save( m_szConfigPath );

    // Release the config
    config->Release();
}