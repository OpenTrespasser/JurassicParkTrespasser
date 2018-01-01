/**********************************************************************
 *<
	FILE: gfx.h

	DESCRIPTION: main graphics system include file.

	CREATED BY: Don Brittain

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#if !defined(_GFX_H_)

#define _GFX_H_

#include "geomlib.h"
#include "export.h"
#include "mtl.h"

#define WM_SHUTDOWN			(WM_USER+2001)
#define WM_INIT_COMPLETE	(WM_USER+2002)

#define GW_MAX_FILE_LEN		128
#define GW_MAX_CAPTION_LEN	128

#define GW_MAX_VERTS		32

typedef BOOL	(*HitFunc)(int, int, void *);

// Rendering modes
#define GW_NO_ATTS			0x0000
#define GW_WIREFRAME		0x0001
#define GW_ILLUM			0x0002
#define GW_FLAT				0x0004
#define GW_SPECULAR			0x0008
#define GW_TEXTURE			0x0010
#define GW_Z_BUFFER			0x0020
#define GW_PERSP_CORRECT	0x0040
#define GW_BACKCULL			0x0100
#define GW_TWO_SIDED		0x0200
#define GW_WIDE_LINES		0x0400	// not implemented!
#define GW_PICK				0x1000
#define GW_BOX_MODE			0x2000
#define GW_ALL_EDGES		0x4000

#define GW_LIGHTING			(GW_ILLUM | GW_SPECULAR)


// spotlight shapes
#define GW_SHAPE_RECT		0
#define GW_SHAPE_CIRCULAR	1

// texture tiling
#define GW_TEX_NO_TILING	0
#define GW_TEX_REPEAT		1
#define GW_TEX_MIRROR		2

// View volume clip flags
#define GW_LEFT_PLANE		0x0100
#define GW_RIGHT_PLANE		0x0200
#define GW_BOTTOM_PLANE		0x0400
#define GW_TOP_PLANE		0x0800
#define GW_FRONT_PLANE		0x1000
#define GW_BACK_PLANE		0x2000
#define GW_PLANE_MASK		0x3f00

// edge styles
#define GW_EDGE_SKIP		0
#define GW_EDGE_VIS			1
#define GW_EDGE_INVIS		2

// buffer types (for dual-plane stuff)
#define BUF_F_BUFFER		0
#define BUF_Z_BUFFER		1

// support method return values
// DONT_KNOW should be returned for all non-driver dependent queries!
#define GW_DONT_KNOW		0
#define GW_DOES_SUPPORT		0x0008
#define GW_DOES_NOT_SUPPORT 0x0080
#define GW_ALWAYS_OFF		(0x0020 | GW_DOES_NOT_SUPPORT)
#define GW_ALWAYS_ON		(0x0002 | GW_DOES_SUPPORT)
#define GW_CAN_TOGGLE		(0x0004 | GW_DOES_SUPPORT)

// support queries
#define GW_SPT_TEXTURES		1
#define GW_SPT_TXT_CORRECT	2

// light types
enum LightType { OMNI_LGT, SPOT_LGT, DIRECT_LGT, AMBIENT_LGT };

// Light attenuation types -- not fully implemented
enum Attenuation { NO_ATTEN, LINEAR_ATTEN, INVERSE_ATTEN };

// General 3D light structure
class Light {
public:
    DllExport Light();
    LightType		type;
    Point3			color;
    Attenuation		attenType;
    float			atten;
    float			intensity;
    float			angle;
	int				shape;
	float			aspect;
	int				overshoot;
};

enum CameraType { PERSP_CAM, ORTHO_CAM };

// General camera structure
class Camera {
public:
	DllExport Camera();
	void			setPersp(float f, float asp)
						{ type = PERSP_CAM; persp.fov = f; 
						  persp.aspect = asp; makeMatrix(); }
	void			setOrtho(float l, float t, float r, float b)
						{ type = ORTHO_CAM; ortho.left = l; ortho.top = t; 
						  ortho.right = r; ortho.bottom = b; makeMatrix(); }
	void			setClip(float h, float y) 
						{ hither = h; yon = y; makeMatrix(); }
	CameraType		getType(void)	{ return type; }
	float			getHither(void) { return hither; }
	float			getYon(void)	{ return yon; }
	DllExport void	reset();
	void			getProj(float mat[4][4])	
						{ memcpy(mat, proj, 16 * sizeof(float)); }
private:
	DllExport void	makeMatrix();
	float			proj[4][4];
	CameraType		type;
	union {
	    struct {
            float	fov;
            float	aspect;
		} persp;
		struct {
		    float	left;
		    float	right;
		    float	bottom;
		    float	top;
		} ortho;
	};
	float			hither;
	float			yon;
};

const double pi        = 3.141592653589793;
const double piOver180 = 3.141592653589793 / 180.0;

// Color types (used by setColor)
enum ColorType { LINE_COLOR, FILL_COLOR, TEXT_COLOR, CLEAR_COLOR };

// Marker types
enum MarkerType  { POINT_MRKR, HOLLOW_BOX_MRKR, PLUS_SIGN_MRKR, 
						   ASTERISK_MRKR, X_MRKR, BIG_BOX_MRKR };

// Region types (for built-in hit-testing)
#define POINT_RGN	0x0001
#define	RECT_RGN	0x0002
#define CIRCLE_RGN	0x0004
#define FENCE_RGN	0x0008

typedef struct tagCIRCLE
{
    LONG  x;
    LONG  y;
	LONG  r;
} CIRCLE;

class HitRegion {
public:
	int				type;
	int				crossing;// not used for point
	int				epsilon; // not used for rect or circle
	union {
		POINT		pt;
		RECT		rect;
		CIRCLE		circle;
		POINT *		pts;
	};
};

inline int ABS(const int x) { return (x > 0) ? x : -x; }

typedef void (*GFX_ESCAPE_FN)(void *);


// driver types for getDriver() method
#define GW_DRV_RENDERER		0
#define GW_DRV_DEVICE		1

// graphics window setup structure
class GWinSetup {
public:
    DllExport GWinSetup();
    TCHAR		caption[GW_MAX_CAPTION_LEN];
	TCHAR		renderer[GW_MAX_FILE_LEN];
	TCHAR		device[GW_MAX_FILE_LEN];
	DWORD		winStyle;
	POINT		size;
	POINT		place;
	int			id;
};

// abstract graphics window class
class GraphicsWindow {
public:
	virtual	~GraphicsWindow() {}
	virtual int		getVersion() = 0;
	virtual TCHAR * getDriver(int which) = 0;
	virtual TCHAR * getDriverString(void) = 0;
	virtual HWND	getHWnd(void) = 0;
	virtual	BOOL	canConfig(void) = 0;
	virtual void	config(HWND hWnd) = 0;
	virtual int		querySupport(int what) = 0;

    virtual void	setColor(ColorType t, float r, float g, float b) = 0;
	virtual void	resetUpdateRect() = 0;
	virtual void	enlargeUpdateRect(RECT *rp) = 0;
	virtual int		getUpdateRect(RECT *rp) = 0;
    virtual void	updateScreen() = 0;

	virtual void	setPos(int x, int y, int w, int h) = 0;
	virtual int		getWinSizeX() = 0;
	virtual int		getWinSizeY() = 0;
	virtual DWORD	getWinDepth(void) = 0;
	virtual DWORD	getHitherCoord(void) = 0;
	virtual DWORD	getYonCoord(void) = 0;
	virtual void	getTextExtents(TCHAR *text, SIZE *sp) = 0;

	virtual BOOL	setBufAccess(int which, int b) = 0;
	virtual BOOL	getBufAccess(int which) = 0;
	virtual BOOL	getBufSize(int which, int *size) = 0;
	virtual BOOL	getBuf(int which, int size, void *buf) = 0;
	virtual BOOL	setBuf(int which, int size, void *buf, RECT *rp) = 0;
	virtual BOOL	getDIB(BITMAPINFO *bmi, int *size) = 0;
	virtual BOOL	setBackgroundDIB(BITMAPINFO *bmi) = 0;
	virtual DWORD	getTextureHandle(BITMAPINFO *bmi) = 0;
	virtual void	freeTextureHandle(DWORD handle) = 0;
	virtual BOOL	setTextureByHandle(DWORD handle) = 0;
	virtual BOOL	setTextureDIB(BITMAPINFO *bmi) = 0;
	virtual BOOL	setTextureTiling(int u, int v, int w=GW_TEX_NO_TILING) = 0;
	virtual int		getTextureTiling(int which) = 0;

	virtual void	beginFrame() = 0;
	virtual void	endFrame() = 0;
	virtual void	setViewport(int x, int y, int w, int h) = 0;
    virtual void	clearScreen(RECT *rp, int useBkg = FALSE) = 0;
    virtual void	setTransform(const Matrix3 &m) = 0;
    virtual void	setTexTransform(const Matrix3 &m) = 0;
	virtual BOOL	getFlipped(void)=0;
	virtual void	setSkipCount(int c) = 0;
	virtual int		getSkipCount(void) = 0;
    virtual void	setRndLimits(DWORD l) = 0;
	virtual DWORD 	getRndLimits(void) = 0;
	virtual DWORD 	getRndMode(void) = 0;
	virtual int		getMaxLights(void) = 0;
    virtual void	setLight(int num, const Light *l) = 0;
	virtual void	setLightExclusion(DWORD exclVec) = 0;
    virtual void	setCamera(const Camera &c) = 0;
	virtual void	setCameraMatrix(float mat[4][4], Matrix3 *invTM, int persp, float hither, float yon) = 0;
	virtual void	getCameraMatrix(float mat[4][4], Matrix3 *invTM, int *persp, float *hither, float *yon) = 0;
    virtual void	setMaterial(const Material &m) = 0;
	virtual Material *getMaterial(void) = 0;

	virtual DWORD	transPointFWin(const Point3 *in, Point3 *out) = 0;
	virtual DWORD	transPointIWin(const Point3 *in, IPoint3 *out) = 0;
	virtual void	lightVertex(const Point3 &pos, const Point3 &nor, Point3 &rgb) = 0;

	virtual void	fWinText(Point3 *xyz, TCHAR *s) = 0;
	virtual void	fWinMarker(Point3 *xyz, MarkerType type) = 0;
	virtual void	fWinPolyline(int ct, Point3 *xyz, Point3 *rgb, Point3 *uvw, int closed, int *es) = 0;
	virtual void	fWinPolygon(int ct, Point3 *xyz, Point3 *rgb, Point3 *uvw) = 0;

	virtual void	iWinText(IPoint3 *xyz, TCHAR *s) = 0;
	virtual void	iWinMarker(IPoint3 *xyz, MarkerType type) = 0;
	virtual void	iWinPolyline(int ct, IPoint3 *xyz, Point3 *rgb, Point3 *uvw, int closed, int *es) = 0;
	virtual void	iWinPolygon(int ct, IPoint3 *xyz, Point3 *rgb, Point3 *uvw) = 0;

    virtual void 	text(Point3 *xyz, TCHAR *s) = 0;
    virtual void	marker(Point3 *xyz, MarkerType type) = 0;
	virtual void	polyline(int ct, Point3 *xyz, Point3 *rgb, Point3 *uvw, int closed, int *es) = 0;
	virtual void 	polygon(int ct, Point3 *xyz, Point3 *rgb, Point3 *uvw) = 0;

	virtual void	setHitRegion(HitRegion *rgn) = 0;
	virtual void	clearHitCode(void) = 0;
	virtual BOOL	checkHitCode(void) = 0;
	virtual DWORD	getHitDistance(void) = 0;

	virtual int		isPerspectiveView(void) = 0;
	virtual float	interpWorld(Point3 *world1, Point3 *world2, float sParam, Point3 *interpPt) = 0;

	virtual void	escape(GFX_ESCAPE_FN fn, void *data) = 0;

protected:
	DWORD			winStyle;
	TCHAR			caption[GW_MAX_CAPTION_LEN];
};

// for Windows int coords with origin at upper-left
inline int iWinIsFacingBack(const IPoint3 &v0, const IPoint3 &v1, const IPoint3 &v2, int flip=0 )
{
	int s = ( (v0[0]-v1[0])*(v2[1]-v1[1]) - (v2[0]-v1[0])*(v0[1]-v1[1]) ) < 0;
	return flip ? !s : s;
}

// for float coords with origin at lower-left
inline int fWinIsFacingBack(const Point3 &v0, const Point3 &v1, const Point3 &v2, int flip=0 )
{
	int s = ( (v0[0]-v1[0])*(v2[1]-v1[1]) - (v2[0]-v1[0])*(v0[1]-v1[1]) ) > 0.0f;
	return flip ? !s : s;
}

DllExport GraphicsWindow *createGW(HWND hWnd, GWinSetup &gws);

DllExport void getRegionRect(HitRegion *hr, RECT *rect);
DllExport BOOL pointInRegion(int x, int y, HitRegion *hr);

DllExport int distToLine(int x, int y, int *p1, int *p2);
DllExport int lineCrossesRect(RECT *rc, int *p1, int *p2);
DllExport int segCrossesCircle(int cx, int cy, int r, int *p1, int *p2);
DllExport BOOL insideTriangle(IPoint3 &p0, IPoint3 &p1, IPoint3 &p2, IPoint3 &q);
DllExport int getZfromTriangle(IPoint3 &p0, IPoint3 &p1, IPoint3 &p2, IPoint3 &q);

	
#endif // _GFX_H_

