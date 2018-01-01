/**********************************************************************
 *<
	FILE: winutil.h

	DESCRIPTION: Misc. windows related functions

	CREATED BY: Rolf Berteig

	HISTORY: 1-6-95 file created

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __WINUTIL__
#define __WINUTIL__


float CoreExport GetWindowFloat(HWND hwnd,BOOL *valid=NULL);
int CoreExport GetWindowInt(HWND hwnd,BOOL *valid=NULL);
BOOL CoreExport SetWindowTextInt( HWND hwnd, int i );
BOOL CoreExport SetWindowTextFloat( HWND hwnd, float f, int precision=3 );
BOOL CoreExport SetDlgItemFloat( HWND hwnd, int idControl, float val );
float CoreExport GetDlgItemFloat( HWND hwnd, int idControl );
void CoreExport SetDlgFont( HWND hDlg, HFONT hFont );
void CoreExport SlideWindow( HWND hwnd, int x, int y );
void CoreExport StretchWindow( HWND hwnd, int w, int h );
BOOL CoreExport CenterWindow(HWND hWndChild, HWND hWndParent);
void CoreExport GetClientRectP( HWND hwnd, Rect *rect );
void CoreExport DrawIconButton( HDC hdc, HBITMAP hBitmap, Rect& wrect, Rect& brect, BOOL in );
int CoreExport GetListHieght( HWND hList );
void CoreExport ShadedVertLine( HDC hdc, int x, int y0, int y1, BOOL in );
void CoreExport ShadedHorizLine( HDC hdc, int y, int x0, int x1, BOOL in );
void CoreExport ShadedRect( HDC hdc, RECT& rect );
void CoreExport Rect3D( HDC hdc, RECT& rect, BOOL in );
void CoreExport WhiteRect3D( HDC hdc, RECT rect, BOOL in );
void CoreExport DrawButton( HDC hdc, RECT rect, BOOL in );
void CoreExport XORDottedRect( HWND hwnd, IPoint2 p0, IPoint2 p1 );
void CoreExport XORDottedCircle( HWND hwnd, IPoint2 p0, IPoint2 p1 );
void CoreExport XORDottedPolyline( HWND hwnd, int count, IPoint2 *pts);
void CoreExport XORRect(HDC hdc, RECT& r, int border=1);
void CoreExport MakeButton2State(HWND hCtrl);
void CoreExport MakeButton3State(HWND hCtrl);
int CoreExport GetCheckBox(HWND hw, int id);
void CoreExport SetCheckBox(HWND hw, int id, BOOL b);
BOOL CoreExport DoesFileExist(const TCHAR *file);

// Delete superfluous zeroes from float string: 1.2300000 -> 1.23
void CoreExport StripTrailingZeros(TCHAR* buf);

template<class T> void LimitValue( T& value, T min, T max )
	{
	if ( value < min ) value = min;
	if ( value > max ) value = max;
	}


#define MAKEPOINT( lparam, pt ) { pt.x = (short)LOWORD(lparam); pt.y = (short)HIWORD(lparam); }

#endif
