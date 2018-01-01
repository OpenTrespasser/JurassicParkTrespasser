//---------------------------------------------------------------------------
//
//  Copyright DreamWorks Interactive 1995
//
//	File:           utils.cpp
//
//  Description:    
//
//  Classes:
//  
//  Functions:
//
// 	History:	    13-Sep-95	SHernd		Created
//				
//---------------------------------------------------------------------------



#include "precomp.h"
#pragma hdrstop

#include "utils.h"



#define DATE_DAYOFMONTH 0x000F
#define DATE_MONTH      0x01F0
#define DATE_YEAR       0xFF00

#define TIME_SECOND     0x001F
#define TIME_MINUTE     0x07E0
#define TIME_HOUR       0xF800


void
FileTimeToString(FILETIME * pft, LPSTR pszText)
{
    WORD wDate;
    WORD wTime;

    FileTimeToDosDateTime(pft, &wDate, &wTime);

    wsprintf(pszText,
             "%i/%i/%i %i:%02i.%02i",
             (wDate & DATE_DAYOFMONTH),
             (wDate & DATE_MONTH) >> 5,
             ((wDate & DATE_YEAR) >> 9) + 80,
             (wTime & TIME_HOUR) >> 11,
             (wTime & TIME_MINUTE) >> 5,
             (wTime & TIME_SECOND) * 2);
}



void 
SizeToString(DWORD dwSize, LPSTR pszText)
{
    if (dwSize <= (5 * 1024))
    {
        wsprintf(pszText, "%i %s", dwSize, "bytes");
    }
    else if (dwSize < ((10 * 1024) * 1024))
    {
        dwSize = dwSize / 1024;
        wsprintf(pszText, "%i %s", dwSize, "KB");
    }
    else
    {
        wsprintf(pszText, 
                 "%i.%i %s", 
                 dwSize / (1024 * 1024), 
                 (dwSize / ((1024 * 1024) / 10)) % 10,
                 "MB");
    }
}




//
// Returns the Currently Selected item within a treeview
//
HTREEITEM
TreeView_GetSelected(HWND hwnd)
{
    HTREEITEM   hitem = NULL;
    TV_ITEM     tvi;
    BOOL        bDone = FALSE;

    tvi.mask = TVIF_STATE | TVIF_HANDLE;
    tvi.stateMask = TVIS_SELECTED;
    hitem = TreeView_GetRoot(hwnd);

    while (!bDone)
    {
        tvi.hItem = hitem;
        TreeView_GetItem(hwnd, &tvi);

        // If the item is selected then we are finished 
        if (tvi.state & TVIS_SELECTED)
        {
            bDone = TRUE;
        }
        else
        {
            // get the next visible item.  A not visible item can't have
            // the .
            hitem = TreeView_GetNextVisible(hwnd, tvi.hItem);
            if (hitem == NULL)
            {
                bDone = TRUE;
            }
        }
    }

    return hitem;
}



HTREEITEM
TreeView_GetNextPossible(HWND hwnd, HTREEITEM htreeitem)
{
    HTREEITEM   hitemret;

    hitemret = TreeView_GetChild(hwnd, htreeitem);
    if (hitemret == NULL)
    {
        hitemret = TreeView_GetNextSibling(hwnd, htreeitem);
        while ((hitemret == NULL) && (htreeitem != NULL))
        {
            htreeitem = TreeView_GetParent(hwnd, htreeitem);
            if (htreeitem == NULL)
            {
                break;
            }

            hitemret = TreeView_GetNextSibling(hwnd, htreeitem);
        }
    }

    return hitemret;
}



//+--------------------------------------------------------------------------
//
//  Function:   TreeView_GetNthChild
//
//  Synopsis:   Retrieves the Nth index of the child under the Parent
//
//  Arguments:  [hwndTree]  -- TreeView window in reference to
//              [htiParent] -- parent to retrieve children of
//              [iChild]    -- index of child to retrieve
//
//  Returns:    HTREEITEM -- Index of nth child or NULL if unsuccessful
//
//  History:    27-Sep-95   SHernd   Created
//
//---------------------------------------------------------------------------
HTREEITEM
TreeView_GetNthChild(HWND hwndTree, HTREEITEM htiParent, int iChild)
{
    HTREEITEM   hti;

    hti = TreeView_GetChild(hwndTree, htiParent);

    while ((iChild > 0) && (hti != NULL))
    {
        hti = TreeView_GetNextSibling(hwndTree, hti);
    }

    return hti;
}



void 
ListView_DeselectAll(HWND hwndList)
{
    int iItem;

    for (iItem = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
         iItem != -1;
         iItem = ListView_GetNextItem(hwndList, iItem, LVNI_SELECTED))
    {
        ListView_SetItemState(hwndList, iItem, 0, LVIF_STATE);
    }
}



BOOL 
CenterWindow(HWND hwndChild, HWND hwndParent)
{
    RECT    rChild, rParent;
    int     wChild, hChild, wParent, hParent;
    int     wScreen, hScreen, xNew, yNew;
    HDC     hdc;

    // Get the Height and Width of the child window
    GetWindowRect (hwndChild, &rChild);
    wChild = rChild.right - rChild.left;
    hChild = rChild.bottom - rChild.top;

    // Get the Height and Width of the parent window
    if (hwndParent == NULL)
    {
        SetRect(&rParent, 
                0, 
                0, 
                GetSystemMetrics(SM_CXSCREEN), 
                GetSystemMetrics(SM_CYSCREEN));
    }
    else
    {
        GetWindowRect (hwndParent, &rParent);
    }

    wParent = rParent.right - rParent.left;
    hParent = rParent.bottom - rParent.top;

    // Get the display limits
    hdc = GetDC (hwndChild);
    wScreen = GetDeviceCaps (hdc, HORZRES);
    hScreen = GetDeviceCaps (hdc, VERTRES);
    ReleaseDC (hwndChild, hdc);

    // Calculate new X position, then adjust for screen
    xNew = rParent.left + ((wParent - wChild) /2);
    if (xNew < 0) {
            xNew = 0;
    } else if ((xNew+wChild) > wScreen) {
            xNew = wScreen - wChild;
    }

    // Calculate new Y position, then adjust for screen
    yNew = rParent.top  + ((hParent - hChild) /2);
    if (yNew < 0) {
            yNew = 0;
    } else if ((yNew+hChild) > hScreen) {
            yNew = hScreen - hChild;
    }

    // Set it, and return
    return SetWindowPos (hwndChild, NULL,
            xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}



void 
appendSlash(LPSTR szSrc)
{
    LPSTR sz2 = szSrc;

    while (*sz2)
    {
        szSrc = sz2;
        sz2 = AnsiNext(szSrc);
    }

    if (sz2 != szSrc + 1 || *szSrc != '\\')
    {
        *sz2++ = '\\';
        *sz2 = '\0';
    }
}


void 
stripSlash(LPSTR szSrc)
{
    LPSTR sz2 = szSrc;

    while (*sz2)
    {
        szSrc = sz2;
        sz2 = AnsiNext(szSrc);
    }

    if (szSrc == szSrc + 1
       && *szSrc == '\\')
    {
        *szSrc = '\0';
    }
}




