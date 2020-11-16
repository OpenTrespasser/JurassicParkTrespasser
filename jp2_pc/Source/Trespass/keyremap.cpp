//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       keyremap.h
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    05-Aug-98   SHernd   Created
//
//---------------------------------------------------------------------------


#include "precomp.h"
#pragma hdrstop

#include "tpassglobals.h"
#include "supportfn.hpp"
#include "resource.h"
#include "dialogs.h"
#include "main.h"
#include "uiwnd.h"
#include "uidlgs.h"
#include "../Lib/Sys/reg.h"
#include "../lib/sys/reginit.hpp"
#include "keyremap.h"

extern HINSTANCE    g_hInst;
bool                bInvertMouse = true;

//
// This is a definition for a VK_ key to string table lookup
//
struct
{
    UINT    uiVirtKey;
    UINT    uiStrID;
} g_aVKSTRINGS[] =
{
    { VK_LBUTTON,  IDS_STR_KEY_MAP + 0 }, 	// Left Mouse
    { VK_RBUTTON,  IDS_STR_KEY_MAP + 1 }, 	// Right Mouse
    { VK_CANCEL,  IDS_STR_KEY_MAP + 2 }, 	// Cancel
    { VK_MBUTTON,  IDS_STR_KEY_MAP + 3 }, 	// Middle Mouse
    { VK_BACK,  IDS_STR_KEY_MAP + 4 }, 	// Back
    { VK_TAB,  IDS_STR_KEY_MAP + 5 }, 	// Tab
    { VK_CLEAR,  IDS_STR_KEY_MAP + 6 }, 	// Clear
    { VK_RETURN,  IDS_STR_KEY_MAP + 7 }, 	// Return
    { VK_SHIFT,  IDS_STR_KEY_MAP + 8 }, 	// Shift
    { VK_CONTROL,  IDS_STR_KEY_MAP + 9 }, 	// Control
    { VK_MENU,  IDS_STR_KEY_MAP + 10 }, 	// Menu
    { VK_PAUSE,  IDS_STR_KEY_MAP + 11 }, 	// Pause
    { VK_CAPITAL,  IDS_STR_KEY_MAP + 12 }, 	// Capital
    { VK_ESCAPE,  IDS_STR_KEY_MAP + 13 }, 	// Escape
    { VK_SPACE,  IDS_STR_KEY_MAP + 14 }, 	// Space
    { VK_PRIOR,  IDS_STR_KEY_MAP + 15 }, 	// Prior
    { VK_NEXT,  IDS_STR_KEY_MAP + 16 }, 	// Next
    { VK_END,  IDS_STR_KEY_MAP + 17 }, 	// End
    { VK_HOME,  IDS_STR_KEY_MAP + 18 }, 	// Home
    { VK_LEFT,  IDS_STR_KEY_MAP + 19 }, 	// Left
    { VK_UP,  IDS_STR_KEY_MAP + 20 }, 	// Up
    { VK_RIGHT,  IDS_STR_KEY_MAP + 21 }, 	// Right
    { VK_DOWN,  IDS_STR_KEY_MAP + 22 }, 	// Down
    { VK_SELECT,  IDS_STR_KEY_MAP + 23 }, 	// Select
    { VK_PRINT,  IDS_STR_KEY_MAP + 24 }, 	// Print
    { VK_EXECUTE,  IDS_STR_KEY_MAP + 25 }, 	// Execute
    { VK_SNAPSHOT,  IDS_STR_KEY_MAP + 26 }, 	// Snapshot
    { VK_INSERT,  IDS_STR_KEY_MAP + 27 }, 	// Insert
    { VK_DELETE,  IDS_STR_KEY_MAP + 28 }, 	// Delete
    { VK_HELP,  IDS_STR_KEY_MAP + 29 }, 	// Help
    { VK_LWIN,  IDS_STR_KEY_MAP + 30 }, 	// L Win
    { VK_RWIN,  IDS_STR_KEY_MAP + 31 }, 	// R Win
    { VK_APPS,  IDS_STR_KEY_MAP + 32 }, 	// Apps
    { VK_NUMPAD0,  IDS_STR_KEY_MAP + 33 }, 	// Numpad 0
    { VK_NUMPAD1,  IDS_STR_KEY_MAP + 34 }, 	// Numpad 1
    { VK_NUMPAD2,  IDS_STR_KEY_MAP + 35 }, 	// Numpad 2
    { VK_NUMPAD3,  IDS_STR_KEY_MAP + 36 }, 	// Numpad 3
    { VK_NUMPAD4,  IDS_STR_KEY_MAP + 37 }, 	// Numpad 4
    { VK_NUMPAD5,  IDS_STR_KEY_MAP + 38 }, 	// Numpad 5
    { VK_NUMPAD6,  IDS_STR_KEY_MAP + 39 }, 	// Numpad 6
    { VK_NUMPAD7,  IDS_STR_KEY_MAP + 40 }, 	// Numpad 7
    { VK_NUMPAD8,  IDS_STR_KEY_MAP + 41 }, 	// Numpad 8
    { VK_NUMPAD9,  IDS_STR_KEY_MAP + 42 }, 	// Numpad 9
    { VK_MULTIPLY,  IDS_STR_KEY_MAP + 43 }, 	// *
    { VK_ADD,  IDS_STR_KEY_MAP + 44 }, 	// +
    { VK_SEPARATOR,  IDS_STR_KEY_MAP + 45 }, 	// |
    { VK_SUBTRACT,  IDS_STR_KEY_MAP + 46 }, 	// -
    { VK_DECIMAL,  IDS_STR_KEY_MAP + 47 }, 	// .
    { VK_DIVIDE,  IDS_STR_KEY_MAP + 48 }, 	// /
    { VK_F1,  IDS_STR_KEY_MAP + 49 }, 	// F1
    { VK_F2,  IDS_STR_KEY_MAP + 50 }, 	// F2
    { VK_F3,  IDS_STR_KEY_MAP + 51 }, 	// F3
    { VK_F4,  IDS_STR_KEY_MAP + 52 }, 	// F4
    { VK_F5,  IDS_STR_KEY_MAP + 53 }, 	// F5
    { VK_F6,  IDS_STR_KEY_MAP + 54 }, 	// F6
    { VK_F7,  IDS_STR_KEY_MAP + 55 }, 	// F7
    { VK_F8,  IDS_STR_KEY_MAP + 56 }, 	// F8
    { VK_F9,  IDS_STR_KEY_MAP + 57 }, 	// F9
    { VK_F10,  IDS_STR_KEY_MAP + 58 }, 	// F10
    { VK_F11,  IDS_STR_KEY_MAP + 59 }, 	// F11
    { VK_F12,  IDS_STR_KEY_MAP + 60 }, 	// F12
    { VK_F13,  IDS_STR_KEY_MAP + 61 }, 	// F13
    { VK_F14,  IDS_STR_KEY_MAP + 62 }, 	// F14
    { VK_F15,  IDS_STR_KEY_MAP + 63 }, 	// F15
    { VK_F16,  IDS_STR_KEY_MAP + 64 }, 	// F16
    { VK_F17,  IDS_STR_KEY_MAP + 65 }, 	// F17
    { VK_F18,  IDS_STR_KEY_MAP + 66 }, 	// F18
    { VK_F19,  IDS_STR_KEY_MAP + 67 }, 	// F19
    { VK_F20,  IDS_STR_KEY_MAP + 68 }, 	// F20
    { VK_F21,  IDS_STR_KEY_MAP + 69 }, 	// F21
    { VK_F22,  IDS_STR_KEY_MAP + 70 }, 	// F22
    { VK_F23,  IDS_STR_KEY_MAP + 71 }, 	// F23
    { VK_F24,  IDS_STR_KEY_MAP + 72 }, 	// F24
    { VK_NUMLOCK,  IDS_STR_KEY_MAP + 73 }, 	// Num Lock
    { VK_SCROLL,  IDS_STR_KEY_MAP + 74 }, 	// Scroll Lock
    { 0xC0, IDS_STR_KEY_MAP + 75 }, // `
    { 0xBD, IDS_STR_KEY_MAP + 76 }, // Minus key
    { 0xBB, IDS_STR_KEY_MAP + 77 }, // =
    { 0xDB, IDS_STR_KEY_MAP + 78 }, // [
    { 0xDD, IDS_STR_KEY_MAP + 79 }, // ]
    { 0xDC, IDS_STR_KEY_MAP + 80 }, // "\\"
    { 0xBA, IDS_STR_KEY_MAP + 81 }, // ;
    { 0xDE, IDS_STR_KEY_MAP + 82 }, // '
    { 0xBC, IDS_STR_KEY_MAP + 83 }, // ,
    { 0xBE, IDS_STR_KEY_MAP + 84 }, // .
    { 0xBF, IDS_STR_KEY_MAP + 85 }, // /
};

int g_iSizeVKString = sizeof(g_aVKSTRINGS)/sizeof(g_aVKSTRINGS[0]);


UINT        g_auiVKReserved[] =
{
    VK_ESCAPE,  // Menu
    VK_F1,      // Help
    VK_F11,     // Cheat Line
    VK_F12,     // Keyremapping Dialog
    VK_SNAPSHOT,     // Screen Capture
    0xBD,       // Minus
    0xBB,       // =
    0xDC,       // "\\"
    0xDB,       // [
    0xDD,       // ]
    0x5B,       // Left Windows Key
    0x5C,       // Right Windows Key
};

int g_iSizeVKReserved = sizeof(g_auiVKReserved) / sizeof(g_auiVKReserved[0]);


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


BOOL IsReserved(UINT vk)
{
    int     i;

    for (i = 0; i < g_iSizeVKReserved; i++)
    {
        if (vk == g_auiVKReserved[i])
        {
            return TRUE;
        }
    }

    return FALSE;
}


void VirtKeyToString(UINT vk, LPSTR psz, int icSize)
{
    int     i;

    if (vk == 0)
    {
        LoadString(g_hInst, IDS_STR_KEY_MAP_UNKNOWN, psz, icSize);
    }
    else if ((vk >= '0' && vk <= '9') ||
             (vk >= 'A' && vk <= 'Z'))
    {
        psz[0] = (char)vk;
        psz[1] = '\0';
    }
    else
    {
        for (i = 0; i < g_iSizeVKString; i++)
        {
            if (vk == g_aVKSTRINGS[i].uiVirtKey)
            {
                LoadString(g_hInst, g_aVKSTRINGS[i].uiStrID, psz, icSize);
            }
        }
    }
}


uint32 g_aiCmds[] = { uBITKEY_WALK, 
                      uBITKEY_RUN, 
                      uBITKEY_BACKUP,
                      uBITKEY_LEFT,
                      uBITKEY_RIGHT,
                      uCMD_JUMP,
                      uCMD_CROUCH,
                      uCMD_USE,
                      uCMD_SHIFT,
                      uCMD_HAND,
                      uCMD_GRAB,
                      uCMD_STOW,
                      uCMD_REPLAYVO,
                      uCMD_THROW,
                      uCMD_CONTROL,
                    };

//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CControlsWnd::CControlsWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
    m_bKeyWait = false;
    m_bIgnoreKey = false;
}

CControlsWnd::~CControlsWnd()
{
}


BOOL CControlsWnd::OnCreate()
{
    if (!CUIDlg::OnCreate())
    {
        TraceError(("CControlsWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    CenterUIWindow(this);

    m_iDefGore = GetRegValue(REG_KEY_GORE, DEFAULT_GORE);
    m_iGore = m_iDefGore;
    m_iDefInvertMouse = GetRegValue(REG_KEY_INVERTMOUSE, DEFAULT_INVERTMOUSE);
    m_iInvertMouse = m_iDefInvertMouse;

    InitialUpdate();

    return TRUE;
}


void CControlsWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "controls.ddf");
}


void CControlsWnd::OnActivateApp(BOOL fActivate, DWORD dwThreadId)
{
    CUITextbox *    pText;

    if (!m_bKeyWait)
    {
        return;
    }

    // Restore the original setup
    m_bKeyWait = FALSE;

    pText = (CUITextbox*)GetUICtrl(m_iCmdNum + 1030);
    if (!pText)
    {
        return;
    }

    pText->SetFGColor(m_crFG);
    pText->SetBkColor(m_crBK);
    pText->SetTransparentBk(m_bTrans);
}


void CControlsWnd::InitialUpdate()
{
    int     i;

    memcpy(m_CurrKeys, km_DefaultKeyMapping, sizeof(m_CurrKeys));

    for (i = 0; i < KEYMAP_COUNT; i++)
    {
        UpdateCmdText(i, m_CurrKeys[CmdToIndex(i)].u4_KeyCode);
    }

    UpdateGore();
    UpdateInvertMouse();
}


void CControlsWnd::UIButtonUp(CUIButton * pbutton)
{
    switch (pbutton->GetID())
    {
        // Cancel
        case 1000:
            EndUIWnd(0);
            break;

        // OK
        case 1001:
            SetRegValue(REG_KEY_GORE, m_iGore);
            SetRegValue(REG_KEY_INVERTMOUSE, m_iInvertMouse);
            bInvertMouse = m_iInvertMouse ? true : false;

            memcpy(km_DefaultKeyMapping, m_CurrKeys, sizeof(m_CurrKeys));
            g_CTPassGlobals.PersistKeys();
            EndUIWnd(1);
            break;

        // Defaults
        case 1002:
            g_CTPassGlobals.RestoreDefaultKeys();
            m_iGore = DEFAULT_GORE;
            m_iInvertMouse = DEFAULT_INVERTMOUSE;
            InitialUpdate();
            break;
    }
}


void CControlsWnd::UpdateCmdText(int iCmd, UINT vk)
{
    char            sz[50];
    CUITextbox *    pTextBox;
    
    VirtKeyToString(vk, sz, sizeof(sz));

    pTextBox = (CUITextbox*)GetUICtrl(iCmd + 1030);
    if (!pTextBox)
    {
        return;
    }

    pTextBox->SetText(sz);
}


BOOL CControlsWnd::IsAlreadyUsed(UINT vk, int & iIndex)
{
    int     i;

    for (i = 0; i < KEYMAP_COUNT; i++)
    {
        if (m_CurrKeys[i].u4_KeyCode == vk)
        {
            iIndex = i;
            return TRUE;
        }
    }

    iIndex = -1;
    return FALSE;
}



void CControlsWnd::UpdateGore()
{
    char            sz[50];
    CUITextbox *    pTextBox;

    LoadString(g_hInst, IDS_GORE_0 + m_iGore, sz, sizeof(sz));

    pTextBox = (CUITextbox*)GetUICtrl(100);
    if (!pTextBox)
    {
        return;
    }

    pTextBox->SetText(sz);
}


void CControlsWnd::UpdateInvertMouse()
{
    CUICheckbox *   pcheck;

    pcheck = (CUICheckbox *)GetUICtrl(102);
    if (!pcheck)
    {
        return;
    }

    pcheck->SetDown(m_iInvertMouse);
}


void CControlsWnd::UIHotspotClick(CUIHotspot * pctrl, BOOL bDown)
{
    if (!bDown)
    {
        return;
    }

    if (pctrl->GetID() < 1000)
    {
        // Gore Toggle
        if (pctrl->GetID() == 101)
        {
            m_iGore = ++m_iGore % 2;

            UpdateGore();
        }
        else if (pctrl->GetID() == 103)
        {
            m_iInvertMouse = !m_iInvertMouse;

            UpdateInvertMouse();
        }

        return;
    }

    m_bKeyWait = TRUE;

    // Convert the control ID to a command #
    m_iCmdNum = pctrl->GetID() - 1010;
    
    CUITextbox *    pText;

    pText = (CUITextbox*)GetUICtrl(m_iCmdNum + 1030);
    if (!pText)
    {
        return;
    }

    m_crFG = pText->GetFGColor();
    m_crBK = pText->GetBkColor();
    m_bTrans = pText->GetTransparentBk();
    strcpy(m_sz, pText->GetText());

    pText->SetFGColor(RGB(255, 0, 0));
    pText->SetBkColor(RGB(255, 255, 255));
    pText->SetTransparentBk(FALSE);
    pText->SetText("");
}


void CControlsWnd::RegisterVK(UINT vk)
{
    int             iIndex;
    int             iUsed;
    CUITextbox *    pText;

    m_bKeyWait = FALSE;

    // for the current key we will remap the item then update the
    // text for the associated text box
    iIndex = CmdToIndex(m_iCmdNum);

    pText = (CUITextbox*)GetUICtrl(m_iCmdNum + 1030);
    if (!pText)
    {
        return;
    }

    pText->SetFGColor(m_crFG);
    pText->SetBkColor(m_crBK);
    pText->SetTransparentBk(m_bTrans);

    if (IsReserved(vk))
    {
        if (vk != VK_LWIN && vk != VK_RWIN)
        {
            m_bIgnoreKey = TRUE;
            m_uiIgnoreKey = vk;
        }

        pText->SetText(m_sz);
    }
    else
    {
        // If the key is already mapped then update that key
        if (IsAlreadyUsed(vk, iUsed))
        {
            m_CurrKeys[iUsed].u4_KeyCode = 0;

            UpdateCmdText(IndexToCmd(iUsed), 0);
        }

        m_CurrKeys[iIndex].u4_KeyCode = vk;
        UpdateCmdText(m_iCmdNum, vk);
    }
}


void CControlsWnd::OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (m_bIgnoreKey && vk == m_uiIgnoreKey && !fDown)
    {
        m_bIgnoreKey = FALSE;
        return;
    }
    else if (m_bIgnoreKey)
    {
        return;
    }

    if (!m_bKeyWait)
    {
        CUIDlg::OnKey(vk, fDown, cRepeat, flags);
        return;
    }

    RegisterVK(vk);
}

void CControlsWnd::OnMButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (!m_bKeyWait)
    {
        CUIDlg::OnMButtonDown(fDoubleClick, x, y, keyFlags);
        return;
    }

    RegisterVK(VK_MBUTTON);
}


void CControlsWnd::OnLButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (!m_bKeyWait)
    {
        CUIDlg::OnLButtonDown(fDoubleClick, x, y, keyFlags);
        return;
    }

    RegisterVK(VK_LBUTTON);
}


void CControlsWnd::OnRButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (!m_bKeyWait)
    {
        CUIDlg::OnRButtonDown(fDoubleClick, x, y, keyFlags);
        return;
    }

    RegisterVK(VK_RBUTTON);
}


int CControlsWnd::CmdToIndex(int iCmd)
{
    int     i;
    uint32  u4;

    u4 = g_aiCmds[iCmd];

    for (i = 0; m_CurrKeys[i].u4_KeyCode != 0xffffffff; i++)
    {
        if (m_CurrKeys[i].u4_Action == u4)
        {
            return i;
        }
    }

    return -1;
}


int CControlsWnd::IndexToCmd(int iIndex)
{
    int     i;
    uint32  u4;

    u4 = m_CurrKeys[iIndex].u4_Action;

    for (i = 0; i < sizeof(g_aiCmds) / sizeof(g_aiCmds[0]); i++)
    {
        if (u4 == g_aiCmds[i])
        {
            return i;
        }
    }

    return -1;
}


