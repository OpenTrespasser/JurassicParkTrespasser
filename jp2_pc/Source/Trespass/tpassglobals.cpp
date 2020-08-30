

#include "precomp.h"
#pragma hdrstop

#include "..\Lib\Sys\reg.h"
#include "..\lib\sys\reginit.hpp"
#include "resource.h"
#include "tpassglobals.h"
#include "rasterdc.hpp"
#include "cdib.h"
#include "uiwnd.h"
#include "uidlgs.h"
#include "main.h"


extern HINSTANCE    g_hInst;
extern HWND		    g_hwnd;
extern CMainWnd *   g_pMainWnd;

CTPassGlobals               g_CTPassGlobals;
PFNWORLDLOADNOTIFY          g_pfnWorldLoadNotify;
uint32                      g_u4NotifyParam;
const float                 g_fMaxPower = 3.0f;
const float                 g_fMinPower = 0.1f;
const float                 g_afGamma[10] =
{
    3.0f,
    2.6f,
    2.2f,
    1.8f,
    1.4f,
    1.0f,
    0.6f,
    0.3f,
    0.0f,
};

void LineColour(int, int, int) {}

uint32 __stdcall TPassLoadNotify(uint32 dwContext, 
                                 uint32 dwParam1, 
                                 uint32 dwParam2, 
                                 uint32 dwParam3)
{
    if (dwContext == 0)
    {
        return 0;
    }

    return ((CLoaderWnd*)dwContext)->HandleNotify(dwParam1, dwParam2, dwParam3);
}


//+--------------------------------------------------------------------------
//
//  Function:   GetNextAvailSaveName
//
//  Synopsis:   Returns the file name that should be used with the chosen
//              iBaseExt.  If iBaseExt == 0 then we should find a new file
//              to place the name into
//
//  Arguments:  [pszFileName] -- Destination name
//              [icFileLen]   -- max length of destination name 
//              [iBaseExt]    -- base extension to use (ie. 000 to 999)
//
//  Returns:    BOOL - TRUE  - if a file name was found
//                     FALSE - otherwise
//
//  History:    01-Jun-97    SHernd  Created
//
//---------------------------------------------------------------------------
BOOL GetNextAvailSaveName(LPSTR pszFileName, int icFileLen, int iBaseExt)
{
    char        szPath[_MAX_PATH];
    char        szBase[_MAX_PATH];
    char        szSavePrefix[40];
    int         iLen;
    BOOL        bFound;

    LoadString(g_hInst, 
               IDS_SAVEGAMEPREFIX, 
               szSavePrefix, 
               sizeof(szSavePrefix));

    GetFileLoc(FA_INSTALLDIR, szBase, sizeof(szBase));
    strcat(szBase, szSavePrefix);
    strcat(szBase, ".");

    if (iBaseExt != -1)
    {
        wsprintf(pszFileName, "%s%03i", szBase, iBaseExt);

        return TRUE;
    }

    iBaseExt = 0;
    bFound = FALSE;
    strcpy(szPath, szBase);
    iLen = strlen(szPath);
    while (!bFound)
    {
        szPath[iLen] = '\0';
        wsprintf(&szPath[iLen], "%03i", iBaseExt++);
        if (GetFileAttributes(szPath) == 0xFFFFFFFF)
        {
            bFound = TRUE;
            strcpy(pszFileName, szPath);
        }
    }

    return bFound;
}



CTPassGlobals::CTPassGlobals()
	: m_apsamRandoms{ nullptr }
{
    m_prasBkgnd = NULL;
    m_prasMiniBkgnd = NULL;

    m_padbMenu = NULL;
    m_psamBkgnd = NULL;
    m_psamButton = NULL;
	bInGame = false;
	bHardReset = false;
}


CTPassGlobals::~CTPassGlobals()
{
    FreeMenuAudio();
}


void CTPassGlobals::SetupBackground()
{
    IDirectDrawSurface4 *   pSurface;
    HRESULT                 hr;
    HDC                     hdc;

    pSurface = prasMainScreen->GetPrimarySurface();
    hr = pSurface->GetDC(&hdc);

    delete m_prasBkgnd;
    m_prasBkgnd = new CRasterDC(hdc,
                                prasMainScreen->iWidthFront,
                                prasMainScreen->iHeightFront,
                                16);

    pSurface->ReleaseDC(hdc);

    delete m_prasMiniBkgnd;
    m_prasMiniBkgnd = new CRasterDC(g_hwnd, 100, 75, 16);

    hdc = m_prasBkgnd->hdcGet();

    BitBlt(hdc, 
           0, 
           0, 
           m_prasBkgnd->iWidth, 
           m_prasBkgnd->iHeight,
           NULL,
           0,
           0,
           BLACKNESS);

    m_prasBkgnd->ReleaseDC(hdc);
}


void CTPassGlobals::CaptureBackground(bool bBackbuffer /* = false */)
{
    HDC     hdcSrc;
    HDC     hdcDst;
    HDC     hdcMini;

    IDirectDrawSurface4 *   pSurface;
    HRESULT                 hr;

    if (bBackbuffer)
    {
        pSurface = prasMainScreen->pddsDraw4;
    }
    else
    {
        pSurface = prasMainScreen->GetPrimarySurface();
    }

    hr = pSurface->GetDC(&hdcSrc);

    hdcDst = m_prasBkgnd->hdcGet();

    POINT basepoint = { 0,0 };
    ClientToScreen(g_hwnd, &basepoint);
	
    BitBlt(hdcDst, 
           0, 
           0, 
           m_prasBkgnd->iWidth, 
           m_prasBkgnd->iHeight,
           hdcSrc,
           basepoint.x,
           basepoint.y,
           SRCCOPY);

    pSurface->ReleaseDC(hdcSrc);
    m_prasBkgnd->ReleaseDC(hdcDst);

    hdcDst = m_prasBkgnd->hdcGet();
    hdcMini = m_prasMiniBkgnd->hdcGet();

	SetStretchBltMode(hdcMini, COLORONCOLOR);
    StretchBlt(hdcMini, 0, 0, m_prasMiniBkgnd->iWidth, m_prasMiniBkgnd->iHeight,
               hdcDst, 0, 0, m_prasBkgnd->iWidth, m_prasBkgnd->iHeight,
               SRCCOPY);

    m_prasBkgnd->ReleaseDC(hdcDst);
    m_prasMiniBkgnd->ReleaseDC(hdcMini);

    DarkenBackground();
}


void CTPassGlobals::DarkenBackground()
{
    LPBYTE      pbBase;
    LPBYTE      pb;
    int         iPitch;
    int         iHeight;
    int         iWidth;
    int         x;
    int         y;
    int         iOffset;
    BYTE        bRed;
    BYTE        bGreen;
    BYTE        bBlue;
    BOOL        b565;

    m_prasBkgnd->Lock();
    pbBase = (LPBYTE)m_prasBkgnd->pSurface;
    pb = pbBase;

    iPitch = m_prasBkgnd->iLineBytes();
    iWidth = m_prasBkgnd->iWidth;
    iHeight = m_prasBkgnd->iHeight;

    iOffset = 0;

    b565 = prasMainScreen->pxf.cposG.u1WidthDiff != 3;

    for (y = 0; y < iHeight; y++)
    {
#if 0
        // This does a paterned black pixel
        iOffset = !iOffset;
        if (iOffset)
        {
            x = 1;
        }
        else
        {
            x = 0;
        }

        pb += (x * 2);

        for (; x < iWidth; x += 2)
        {
            *(LPWORD)pb = 0;
            pb += 4;
        }
#else
        // This darkens the screen by 50%
        for (x = 0; x < iWidth; x++)
        {
            SeperateColor(*(LPWORD)pb, bRed, bGreen, bBlue);
            bRed /= 2;
            bGreen /= 2;
            bBlue /= 2;
            *(LPWORD)pb = GetColor(bRed, bGreen, bBlue);
            pb += 2;
        }
#endif

        pbBase += iPitch;
        pb = pbBase;
    }

    m_prasBkgnd->Unlock();
}


// Load as scene, adding the data directory to the path.
int CTPassGlobals::LoadLevel(LPCSTR pszName)
{
    int     iRet;
    char    szFile[_MAX_PATH];

    GetFileLoc(FA_DATADRIVE, szFile, sizeof(szFile));
    strcat(szFile, pszName);

    iRet = LoadScene(szFile, (LPSTR)pszName);

    return iRet;
}


void CTPassGlobals::ClearVidBuffers()
{
	if (!prasMainScreen)
		return;
	d3dDriver.SetFlipClear(false);
	prasMainScreen->Clear(0);
	prasMainScreen->Flip();
	prasMainScreen->Clear(0);
	prasMainScreen->Flip();
	prasMainScreen->Clear(0);
	prasMainScreen->Flip();
	d3dDriver.SetFlipClear(true);
}


void CTPassGlobals::HardScreenReset(BOOL b_reset_world)
{
	if (!bHardReset)
	{
		int i_width;
		int i_height;

		bGetDimensions(i_width, i_height);
		if (prasMainScreen->iWidthFront != i_width || prasMainScreen->iHeightFront != i_height)
			SetupGameScreen();
		return;
	}

	d3dDriver.Purge();
	d3dDriver.Uninitialize();
	if (b_reset_world)
		wWorld.Reset();
	prasMainScreen->uRefs = 1;
	std::destroy_at(&prasMainScreen);
	g_initDD.ReleaseAll();
	g_initDD.BaseInit();

	SetupGameScreen();

	// Reset flag for future use.
	bHardReset = false;
}

int CTPassGlobals::LoadScene(LPSTR pszScene, LPSTR pszOrigSCN)
{
    int         iRet;
    BOOL        bUseDlg;
    CLoaderWnd  dlg(g_pMainWnd->m_pUIMgr);
    CLoaderWnd * pdlg = &dlg;

	// Ignore alt-tab's.
	BOOL b_old_ignore = CUIWnd::bIgnoreSysKey;
	CUIWnd::bIgnoreSysKey = TRUE;

    Assert(strlen(pszScene) + 1 < sizeof(char) * _MAX_PATH);

    strcpy(m_szLastScene, pszScene);

    if (pszOrigSCN)
    {
        strcpy(m_szSCN, pszOrigSCN);
    }

    bUseDlg = GetRegValue("ShowProgressBar", TRUE);

    // Set this static variable to true.  If there is an error it will
    // get set to false and no one else sets it back to true.
    CLoadImageDirectory::bImageLoader = true;

	// Drastic but necessary: Restart the window.
	HardScreenReset(TRUE);

    if (bUseDlg)
    {
        dlg.SetupUIWnd();
        g_pfnWorldLoadNotify = TPassLoadNotify;
        g_u4NotifyParam = (uint32)pdlg;
    }
    else
    {
        pdlg = NULL;
        g_pfnWorldLoadNotify = NULL;
        g_u4NotifyParam = 0;
    }

    (g_pfnWorldLoadNotify)(g_u4NotifyParam, 6, 0, 0);

    // Loads the scene.  If a reset is required, this function handles it.
	iRet = wWorld.bLoadScene(pszScene, 
                             (PFNWORLDLOADNOTIFY)TPassLoadNotify, 
                             (uint32)pdlg);


    d3dDriver.Restore();

    if (bUseDlg)
    {
        dlg.DestroyUIWnd();
    }

    g_pfnWorldLoadNotify = NULL;
    g_u4NotifyParam = 0;

	// Create the input object as required.
	gpInputDeemone->Capture(true);

    RefreshAudioSettings();

	bHardReset = false;

	// Restore alt-tab's.
	CUIWnd::bIgnoreSysKey = b_old_ignore;

    return iRet;
}


BOOL CTPassGlobals::bSaveScene(LPSTR pszScene)
{
    char szFileSCN[_MAX_PATH];

	// Save to file
    GetFileLoc(FA_INSTALLDIR, szFileSCN, sizeof(szFileSCN));

    strcat(szFileSCN, pszScene);

    // Save the world
	if (!wWorld.bSaveWorld(szFileSCN))
    {
        TraceError(("CTPassGlobals::SaveGame() -- "
                    "bSaveWorld Failed"));
		return FALSE;
    }

    return TRUE;
}


BOOL CTPassGlobals::LoadLastScene()
{
    BOOL    bRet;
    char    sz[_MAX_PATH];

    GetFileLoc(FA_DATADRIVE, sz, sizeof(sz));

    strcat(sz, m_szSCN);

    bRet = wWorld.bLoadScene(sz, (PFNWORLDLOADNOTIFY)TPassLoadNotify, 0);
    RefreshAudioSettings();

    return bRet;
}


BOOL CTPassGlobals::DeleteGame(LPSTR pszName)
{
    char        szFile[_MAX_PATH];

    GetFileLoc(FA_INSTALLDIR, szFile, sizeof(szFile));

    strcat(szFile, pszName);
    DeleteFile(szFile);

    return TRUE;
}


BOOL CTPassGlobals::SaveNamedGame(LPCSTR pszFile, LPCSTR pszName)
{
    char        szFName[SAVENAME_SIZE + 1];
    CDib        dib;
    MEMDIBINFO  memdib;
    LPBYTE      pb;
    DWORD       dwLen;

    //
    // Open the save file.
    //
	CSaveFile sf(pszFile, false, true);	
	if (!sf.bValidFile)
    {
		return FALSE;
    }

    //
    // Write the save name into the files
    //
    Assert(strlen(pszName) <= SAVENAME_SIZE);
    strcpy(szFName, pszName);
    szFName[SAVENAME_SIZE] = '\0';
    sf.bSave("SaveGameName", szFName, sizeof(szFName));

    //
    // Write the original SCN name into the filef
    //
    sf.bSave("SaveSCNName", m_szSCN, sizeof(m_szSCN));

    //
    // Write the bmp into the file
    //
    GetSaveBmp(&dib);
    dwLen = dib.CalculateWriteSize();

    pb = new BYTE[dwLen];
    memdib.pbBase = pb;
    memdib.pbCurr = pb;
    memdib.dwSize = dwLen;

    dib.WriteToStream((VOID*)&memdib, (PFNDIBWRITE)MemDibWrite);
    sf.bSave("SaveBMP", (const char*)memdib.pbBase, (int)memdib.dwSize);

    delete [] pb;

    //
    // Now actually write the save game
    //
    if (!wWorld.bSaveTheWorld(&sf))
    {
        TraceError(("CTPassGlobals::SaveGame() -- "
                    "bSaveWorld Failed"));
    }

    return TRUE;
}


BOOL CTPassGlobals::SaveGame(LPCSTR pszName, int iBaseExt)
{
    char        szFile[_MAX_PATH];

    GetNextAvailSaveName(szFile, sizeof(szFile), iBaseExt);

    SaveNamedGame(szFile, pszName);

    return TRUE;
}


void CTPassGlobals::GetSaveBmp(CDib * pdib)
{
    CRasterDC * pras24;
    HDC         hdcSrc;
    HDC         hdcDst;
    LPBYTE      pbSrc;
    LPBYTE      pbDst;
    int         i;
    int         j;

    pras24 = new CRasterDC(g_hwnd, 100, 75, 24);
	hdcDst = pras24->hdcGet();

	// Save out a blank bitmap if a real one cannot be found.
	if (!m_prasMiniBkgnd || !m_prasMiniBkgnd->hdcGet())
	{
		BitBlt(hdcDst, 0, 0, pras24->iWidth, pras24->iHeight, 0, 0, 0, BLACKNESS);
	}
	else
	{
		hdcSrc = m_prasMiniBkgnd->hdcGet();

		SetStretchBltMode(hdcDst, COLORONCOLOR);
		StretchBlt(hdcDst, 0, 0, pras24->iWidth, pras24->iHeight,
				   hdcSrc, 0, 0, m_prasMiniBkgnd->iWidth, m_prasMiniBkgnd->iHeight,
				   SRCCOPY);

		m_prasMiniBkgnd->ReleaseDC(hdcSrc);
	}
	pras24->ReleaseDC(hdcDst);

    pdib->Create(pras24->iWidth, pras24->iHeight, 24, 0);

	pbDst = pdib->GetBits();
    pras24->Lock();
    pbSrc = (LPBYTE)pras24->pSurface;

	for (i = 0; i < pdib->GetHeight(); i++)
    {
        pbDst = pdib->GetPixelPointer(0, (pdib->GetHeight() - 1) - i);

		for (j = 0; j < pdib->GetWidth(); j++)
		{
			*pbDst++ = *pbSrc;
			*pbDst++ = *(pbSrc+1);
			*pbDst++ = *(pbSrc+2);
			pbSrc += 3;
		}
	}

    pras24->Unlock();

    delete pras24;
}


void CTPassGlobals::ResetScreen(BOOL bWithInit)
{
    if (bWithInit)
    {
        SetupGameScreen();
        g_pMainWnd->m_pUIMgr->ResizeScreen();
    }
    else
    {
        prasMainScreen->Clear(0);
        ChangeViewportSize(0, 0, false);
    }

    gmlGameLoop.Paint();
	prenMain->pScreenRender->EndFrame();

    SetupBackground();
    CaptureBackground(true);
}

void CTPassGlobals::CreateMenuAudioDatabase()
{
    char        szFile[_MAX_PATH];

    GetFileLoc(FA_INSTALLDIR, szFile, sizeof(szFile));
    strcat(szFile, "menu.tpa");

    m_padbMenu = new CAudioDatabase(szFile);

}

void CTPassGlobals::SetupMenuAudio()
{
    int         i;
    char        szSamples[13][25] =
    {
        "DINO - TREX DIST MISC",
        "DINO - TREX DIST A",
        "DINO - TREX DIST B",
        "DINO - RAPT DIST A",
        "DINO - RAPT DIST B",
        "DINO - RAPT DIST C",
        "BIRD 01",
        "BIRD 02",
        "BIRD 03",
        "BIRD 04",
        "BIRD 05",
        "BIRD 06",
        "BIRD 07",
    };

    m_psamBkgnd = CAudio::psamCreateSample(sndhndHashIdentifier("OPTIONS - MAIN LOOP"),
                                   m_padbMenu,
                                   /*AU_CREATE_STREAM | */AU_CREATE_STEREO);

    for (i = 0; i < 13; i++)
    {
        m_apsamRandoms[i] = CAudio::psamCreateSample(sndhndHashIdentifier(szSamples[i]),
                                             m_padbMenu,
                                             AU_CREATE_STATIC | AU_CREATE_STEREO);
    }

    SetupButtonAudio();
}


void CTPassGlobals::SetupButtonAudio()
{
    m_psamButton = CAudio::psamCreateSample(sndhndHashIdentifier("DINO - TREX FOOT"),
                                            m_padbMenu,
                                            AU_CREATE_STATIC | AU_CREATE_STEREO);
}


void CTPassGlobals::FreeMenuAudio()
{
    int     i;

    if (m_psamBkgnd)
    {
        m_psamBkgnd->Stop();
        delete m_psamBkgnd;
        m_psamBkgnd = NULL;
    }

    if (m_psamButton)
    {
        //hey, button sounds are short so lets wait for it to finish
        //we've been chopping the button sound on menu shutdown
        //waiting for the sound to finish takes ~ 1 sec. We shouldn't need that much
        if (m_psamButton->u4Status() == AU_BUFFER_STATUS_PLAYING)
        {
            Sleep(600);
        }

        m_psamButton->Stop();
        delete m_psamButton;
        m_psamButton = NULL;
    }

    for (i = 0; i < 13; i++)
    {
        if (m_apsamRandoms[i])
        {
            m_apsamRandoms[i]->Stop();
            delete m_apsamRandoms[i];
            m_apsamRandoms[i] = NULL;
        }
    }

    if (m_padbMenu)
    {
        delete m_padbMenu;
        m_padbMenu = NULL;
    }
}


void CTPassGlobals::StartBackgroundAudio()
{
    if (!m_psamBkgnd)
    {
        FreeMenuAudio();
        SetupMenuAudio();
    }

    if (!m_psamBkgnd)
    {
        return;
    }

    m_psamBkgnd->bPlay(AU_PLAY_LOOPED);
}


void CTPassGlobals::StopBackgroundAudio()
{
    if (m_psamBkgnd)
    {
        m_psamBkgnd->Stop();
    }
}


void CTPassGlobals::PlayRandomAudio()
{
    int     i;

    i = rand() % 13;

    if (!m_apsamRandoms[i])
    {
        return;
    }

    m_apsamRandoms[i]->bPlay(AU_PLAY_ONCE);
}


void CTPassGlobals::PlayButtonAudio()
{
    if (!m_psamButton)
    {
        return;
    }

    m_psamButton->Stop();
    m_psamButton->bPlay(AU_PLAY_ONCE);
}


void CTPassGlobals::InitGamma()
{
	float   fPower;
    int     iSetting;

    iSetting = GetGamma();
    
    fPower = float(g_afGamma[iSetting]) * (g_fMaxPower - g_fMinPower) / 10.0;

    //
    // Update the cluts
    //
    CClu::gcfMain.Set(CClu::gcfMain.tOutputRange, fPower);

	pcdbMain.UpdateCluts();

    //
    // Force an update of the Terrain
    //
    CWDbQueryTerrain().tGet()->Rebuild(false);
}


int  CTPassGlobals::GetGamma()
{
    return GetRegValue(REG_KEY_GAMMA, DEFAULT_GAMMA);
}


void CTPassGlobals::SetGamma(int iGamma)
{
    SetRegValue(REG_KEY_GAMMA, iGamma);
}


void CTPassGlobals::RenderQualityUp()
{
    int iQuality;

    iQuality = GetRenderQuality();
    if (iQuality == 4)
    {
        return;
    }

    MiddleMessage(IDS_RENDER_QUALITY_CHANGING);

    SetRenderQuality(++iQuality);
	ClearVidBuffers();
}


void CTPassGlobals::RenderQualityDn()
{
    int iQuality;

    iQuality = GetRenderQuality();
    if (iQuality == 0)
    {
        return;
    }

    MiddleMessage(IDS_RENDER_QUALITY_CHANGING);

    SetRenderQuality(--iQuality);
	ClearVidBuffers();
}


int  CTPassGlobals::GetRenderQuality()
{
    return GetRegValue(REG_KEY_RENDERING_QUALITY, DEFAULT_RENDERING_QUALITY);
}


void CTPassGlobals::SetRenderQuality(int iQuality)
{
    if (iQuality < 0)
    {
        iQuality = 0;
    }
    else if (iQuality > 4)
    {
        iQuality = 4;
    }

    SetRegValue(REG_KEY_RENDERING_QUALITY, iQuality);
    SetQualitySetting(iQuality);
	ResetScreen(false);
	ClearVidBuffers();
}


void CTPassGlobals::SaveDefaultKeys()
{
    memcpy(m_DefaultKeyMapping, 
           km_DefaultKeyMapping, 
           sizeof(m_DefaultKeyMapping));
}


void CTPassGlobals::RestoreDefaultKeys()
{
    memcpy(km_DefaultKeyMapping, 
           m_DefaultKeyMapping, 
           sizeof(m_DefaultKeyMapping));
}


void CTPassGlobals::GetSavedKeys()
{
    SKeyMapping     aKeyMap[KEYMAP_COUNT];

    if (GetRegData(REG_KEY_KEYMAP, (LPBYTE)&aKeyMap, sizeof(aKeyMap)) ==
        sizeof(aKeyMap))
    {
        memcpy(km_DefaultKeyMapping, 
               aKeyMap, 
               sizeof(aKeyMap));
    }
}


void CTPassGlobals::PersistKeys()
{
    SetRegData(REG_KEY_KEYMAP, 
               (LPBYTE)&km_DefaultKeyMapping, 
               sizeof(SKeyMapping) * KEYMAP_COUNT);
}


