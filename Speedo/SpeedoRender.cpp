// Much of the code here is inspried by spookie's VC speedo mod.

#include "SpeedoRender.h"
#include "Scripting.h"
#include "../ScriptHook/ScriptHookManager.h"
#include "resource.h"

using namespace Scripting;

#define SafeRelease( x ) if ((x) != NULL) { (x)->Release(); (x) = NULL; }

SpeedoRender::SpeedoRender()
{
    m_pSpeedoTexture[Settings::UnitMPH] = NULL;
    m_pSpeedoTexture[Settings::UnitKPH] = NULL;
    m_pNeedleTexture = NULL;
    m_pSprite = NULL;

    m_bSpeedoVisible = true;
    m_nSpeedoColor = 0x00FFFFFF;
    m_fCurrentSpeed = 0.0f;
}

void SpeedoRender::InstallHook()
{
    // Convenience method for installing the hook, doesn't have to be in here really.
    ID3DHookService *pService = ScriptHookManager::RequestService<ID3DHookService>( "D3DHook" );
    pService->AddDeviceHook( this );
}

void SpeedoRender::RemoveHook()
{
    // Convenience method for uninstalling the hook, doesn't have to be in here really.
    ID3DHookService *pService = ScriptHookManager::RequestService<ID3DHookService>( "D3DHook" );
    pService->RemoveDeviceHook( this );
}

void SpeedoRender::OnResetDevice( ID3DDevice *pDevice, PresentParameters *pPresentParameters )
{
    if (m_pSpeedoTexture[Settings::UnitMPH] == NULL)
    {
        D3DXCreateTextureFromResourceEx(pDevice, m_hModule, MAKEINTRESOURCE(IDB_SPEEDO), 
                    256, 256, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 
                    D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_pSpeedoTexture[Settings::UnitMPH]);
    }

    if (m_pSpeedoTexture[Settings::UnitKPH] == NULL)
    {
        D3DXCreateTextureFromResourceEx(pDevice, m_hModule, MAKEINTRESOURCE(IDB_SPEEDO_KPH), 
            256, 256, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 
            D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_pSpeedoTexture[Settings::UnitKPH]);
    }

    if (m_pNeedleTexture == NULL)
    {
        D3DXCreateTextureFromResourceEx(pDevice, m_hModule, MAKEINTRESOURCE(IDB_NEEDLE), 
            128, 128, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 
            D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_pNeedleTexture);
    }

    if (m_pSprite == NULL)
    {
        D3DXCreateSprite(pDevice, &m_pSprite);
    }

    u32 w = pPresentParameters->BackBufferWidth;
    u32 h = pPresentParameters->BackBufferHeight;
    m_vecScale = D3DXVECTOR3( w/800.0f, h/600.0f, 0.0f );
    m_vecSpeedoScale = D3DXVECTOR3( 1.0f, 1.0f, 0.0f );
    m_vecPosition = D3DXVECTOR3( 18.0f, 411.0f, 0.0f );
    m_vecNeedleTranslation = D3DXVECTOR3( 78.0f, 81.0f, 0.0f );
    m_vecNeedleRotationCenter = D3DXVECTOR3( 20.0f, 17.0f, 0.0f );

    // Do some adjustments for some very specific cases...
    i32 ar = (i32)( (m_vecScale.x / m_vecScale.y) * 10 );
    if (ar != 10 && ar != 11 && ar != 16)
    {
        m_vecPosition.x = 17.0f;
        m_vecPosition.y = 410.0f;
    }
    if (w == 1280 && h == 1024)
    {
        m_vecSpeedoScale = D3DXVECTOR3( 0.95f, 0.98f, 0.0f );
    }

    if (m_pSettings->GetForceWide())
    {
        m_vecScale.x = m_vecScale.y * m_pSettings->GetForceWideMultiplier();
    }
    else
    {
        m_vecScale.x = m_vecScale.y;
    }

    m_fAngleZero = 4.54f; // radian angles to 0
    m_fAnglePerUnitSpeed = m_fAngleZero / 200.0f;
}

void SpeedoRender::OnLostDevice( ID3DDevice *pDevice )
{
    SafeRelease( m_pSpeedoTexture[Settings::UnitMPH] );
    SafeRelease( m_pSpeedoTexture[Settings::UnitKPH] );
    SafeRelease( m_pNeedleTexture );
    SafeRelease( m_pSprite );
}

void SpeedoRender::OnRender( ID3DDevice *pDevice )
{
    D3DXMATRIX rotationMatrix, needleMatrix, speedoMatrix;

    if ( !m_bSpeedoVisible && (m_nSpeedoColor & 0xFF000000) == 0 )
    {
        // Nothing to render, so lets back off
        return;
    }

    // Careful which native functions you call from here (different Win32 thread than Scripting)
    if (IsPauseMenuActive() || IsScreenFadingOut() || IsScreenFadedOut())
    {
        return;
    }

    if (m_bSpeedoVisible && (m_nSpeedoColor & 0xFF000000) != 0xFF000000)
    {
        // Fade in
        m_nSpeedoColor += 0x05000000;
    }

    if (!m_bSpeedoVisible && (m_nSpeedoColor & 0xFF000000) != 0)
    {
        // Fade out
        m_nSpeedoColor -= 0x05000000;
    }

    // Set up the matrix for the speedo meter
    D3DXMatrixIdentity(&speedoMatrix);
    // Scaling
    speedoMatrix.m[0][0] = m_vecScale.x * m_vecSpeedoScale.x;
    speedoMatrix.m[1][1] = m_vecScale.y * m_vecSpeedoScale.y;
    // Translation
    speedoMatrix.m[3][0] = m_vecPosition.x * m_vecScale.x;
    speedoMatrix.m[3][1] = m_vecPosition.y * m_vecScale.y;

    // Create a rotation matrix based on the speed we are going at
    D3DXMatrixRotationZ(&rotationMatrix, -m_fAngleZero+m_fAnglePerUnitSpeed*m_fCurrentSpeed);

    // Set up the needle matrix bsaed on the speedo's matrix and the rotation matrix
    D3DXMatrixMultiply(&needleMatrix, &rotationMatrix, &speedoMatrix);
    needleMatrix.m[3][0] += (m_vecNeedleTranslation.x + m_vecNeedleRotationCenter.x) * m_vecScale.x * m_vecSpeedoScale.x;
    needleMatrix.m[3][1] += (m_vecNeedleTranslation.y + m_vecNeedleRotationCenter.y) * m_vecScale.y * m_vecSpeedoScale.y;

    m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

    Settings::eDisplayUnit unit = m_pSettings->GetDisplayUnit();
    m_pSprite->SetTransform(&speedoMatrix);
    m_pSprite->Draw(m_pSpeedoTexture[unit], NULL, NULL, NULL, m_nSpeedoColor);

    m_pSprite->SetTransform(&needleMatrix);
    m_pSprite->Draw(m_pNeedleTexture, NULL, &m_vecNeedleRotationCenter, NULL, m_nSpeedoColor);

    m_pSprite->End();
}