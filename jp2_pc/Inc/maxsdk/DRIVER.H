#if !defined(_DRIVER_H_)

#define _DRIVER_H_

// configuration values based on GW_ flags in gfx.h!
#define DRV_WIDE_LINES		0x0400
#define DRV_STIPPLE			0x0800
#define DRV_INVIS_EDGES		0x4000

// color types
#define DRV_LINE_COLOR		0
#define DRV_FILL_COLOR		1
#define DRV_TEXT_COLOR		2
#define DRV_CLEAR_COLOR		3

// connect types
#define DRV_EDGE_SKIP		0
#define DRV_EDGE_VIS		1
#define DRV_EDGE_INVIS		2

// export types
#define DRV_EXP_BITMAP		0
#define DRV_EXP_METAFILE	1

// buffer types
#define DRV_F_BUFFER		0
#define DRV_Z_BUFFER		1

// maximum vertices in a line or polygon
#define DRV_MAX_VERTS		64

// texture type names
#define TEX_MAX_NAME		32

#ifndef WM_SHUTDOWN
#define WM_SHUTDOWN		(WM_USER+2001)
#endif
			  
typedef struct {
	int			size;
	TCHAR		desc[TEX_MAX_NAME];
} TexData;

typedef void (*TexLookup)(TexData *td, float *rgb, 
				float *uvw, float *horzDelta, float *vertDelta, float *clr);


#endif // _DRIVER_H_
