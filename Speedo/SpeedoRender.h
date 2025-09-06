#pragma once

#include "../ScriptHook/Services/D3DHook.h"
#include "Settings.h"
#include <d3dx9.h>

class SpeedoRender :
    public ID3DDeviceHook
{
private:
    IDirect3DTexture9 *m_pSpeedoTexture[2];
    IDirect3DTexture9 *m_pNeedleTexture;

    ID3DXSprite *m_pSprite;

    D3DXVECTOR3 m_vecScale;
    D3DXVECTOR3 m_vecPosition;
    D3DXVECTOR3 m_vecSpeedoScale;
    D3DXVECTOR3 m_vecNeedleTranslation;
    D3DXVECTOR3 m_vecNeedleRotationCenter;

    f32 m_fAngleZero;
    f32 m_fAnglePerUnitSpeed;
    
    f32 m_fCurrentSpeed;
    b8 m_bSpeedoVisible;
    u32 m_nSpeedoColor;

    Settings *m_pSettings;

    HMODULE m_hModule;

public:
    SpeedoRender();

    inline void SetSettings( Settings *settings ) { m_pSettings = settings; }
    inline void SetCurrentSpeed( f32 speed ) { m_fCurrentSpeed = speed; }
    inline void SetVisible( b8 visible ) { m_bSpeedoVisible = visible; }
    
    inline void SetResourceModule( HMODULE hModule ) { m_hModule = hModule; }

    void InstallHook();
    void RemoveHook();

    void OnCreateDevice(ID3DDevice *pDevice, PresentParameters *pPresentParameters) { /* nothing to do here */ }
    void OnResetDevice(ID3DDevice *pDevice, PresentParameters *pPresentParameters);
    void OnLostDevice(ID3DDevice *pDevice);
    void OnDestroyDevice(ID3DDevice *pDevice) { /* nothing to do here */ }
    void OnRender(ID3DDevice *pDevice);
};
