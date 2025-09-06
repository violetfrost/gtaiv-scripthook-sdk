#pragma once

#include "../ScriptHook/Types.h"
#include <windows.h>

class Settings
{
public:
    enum eDisplayUnit
    {
        UnitMPH,
        UnitKPH
    };

private:
    eDisplayUnit m_DisplayUnit;
    b8 m_bForceWide;
    f32 m_fForceWideMultiplier;

    ch m_szConfigPath[ MAX_PATH ];
    HMODULE m_hModule;
    
public:
    void Initialize();
    void Save();

    inline void SetModule( HMODULE hModule ) { m_hModule = hModule; }

    inline eDisplayUnit GetDisplayUnit() const { return m_DisplayUnit; }
    inline void SetDisplayUnit(eDisplayUnit unit) { m_DisplayUnit = unit; }

    inline b8 GetForceWide() const { return m_bForceWide; }
    inline f32 GetForceWideMultiplier() const { return m_fForceWideMultiplier; }

};