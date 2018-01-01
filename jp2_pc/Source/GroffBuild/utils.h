//---------------------------------------------------------------------------
//
//  Copyright DreamWorks Interactive 1995
//
//	File:           utils.h
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


#ifndef __UTILS_H__
#define __UTILS_H__

#define IsButtonDown(iKey) (GetKeyState(iKey) < 0)


void ListView_DeselectAll(HWND hwndList);
HTREEITEM TreeView_GetSelected(HWND hwnd);
HTREEITEM TreeView_GetNextPossible(HWND hwnd, HTREEITEM htreeitem);
HTREEITEM TreeView_GetNthChild(HWND hwndTree, HTREEITEM htiParent, int iChild);
void FileTimeToString(FILETIME * pft, LPSTR pszText);
void SizeToString(DWORD dwSize, LPSTR pszText);
BOOL CenterWindow(HWND hwndChild, HWND hwndParent);

void stripSlash(LPSTR szSrc);
void appendSlash(LPSTR szSrc);



#endif // __UTILS_H__
