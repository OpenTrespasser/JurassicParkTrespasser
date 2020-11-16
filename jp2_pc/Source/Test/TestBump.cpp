/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Tests game engine by running a rudimentary bump mapper.
 * Not yet team style compliant.
 *
 ***********************************************************************************************
 *
 * $Log: /JP2_PC/Source/Test/TestBump.cpp $
 * 
 * 23    96/12/09 16:09 Speter
 * Removed CTimer calls.
 * 
 * 22    96/11/11 15:30 Speter
 * 
 * 21    96/10/28 15:00 Speter
 * Changed CEventHandler to CAppShell.
 * 
 * 20    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal>
 * with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent
 * transform types, and prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 19    96/09/23 17:05 Speter
 * Removed reference to b2Sided.
 * 
 * 18    96/09/12 16:31 Speter
 * Incorporated new TReflectVal usage.
 * 
 * 17    96/07/31 15:53 Speter
 * Added timing stats.
 * 
 * 16    96/07/29 11:07 Speter
 * Changed to be compatible with removal of CVector typedef.
 * 
 * 15    96/07/25 10:52 Speter
 * Added code for gammatesting (GAMMATEST flag).
 * Changed CPalette to CPal.
 * 
 * 14    96/07/23 11:10 Speter
 * Lots of changes, none you care about.
 * 
 * 13    96/07/08 20:05 Speter
 * First mostly working version of lighting code.
 * 
 * 12    96/07/03 12:59 Speter
 * Changed include statements.
 * Added debug info in full-screen raster.
 * 
 * 11    96/07/01 22:32 Speter
 * Updated for Raster changes from code review.
 * Added some compile-time constants.
 * 
 * 10    96/06/26 22:08 Speter
 * Now use CPlacement3 rather than CTransform3 for object placement.
 * 
 * 9     96/06/26 16:44 Speter
 * Updated for new transform modules.
 * 
 * 8     96/06/26 15:13 Speter
 * Changed timing to reset on every raster change.
 * 
 * 7     96/06/04 16:04 Speter
 * Added CPP symbols for different behaviors.
 * 
 * 6     96/05/31 12:50 Speter
 * Now loads texture from Standard.bmp.
 * Added second template type to CRasterBump class, for texture type.
 * 
 * 5     96/05/23 17:02 Speter
 * Added tons of casts to eliminate warnings.  Happy?
 * Removed version which created texture map of same depth as raster.
 * Now using new CRasterDraw module to get raster-dependent templates.
 * 
 * 4     96/05/16 16:48 Speter
 * Changed to interface with new AppEvent and AppShell modules.
 * Changed to use new CRotation3 matrices, CAngle and FastTrig types.
 * 
 * 3     96/05/13 14:20 Speter
 * Changed to reflect CTransform changes.
 * Added near plane clipping in inner loop.
 * Moved Asserts to texture coord validity to Raster module.
 * 
 * 2     96/05/09 11:24 Speter
 * Updated code to use new AttachPalette function.
 * 
 * 1     96/05/08 20:11 Speter
 * TestMain implementation which tests game engine by running a
 * rudimentary bump map demo.  Note: Formerly known as Test/Render.cpp.
 * 
 **********************************************************************************************/

// #include <math.h>
#include <memory.h>

#include "common.hpp"
#include "Lib/Std/Array.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/Transform/Transform.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Types/FixedP.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Shell/AppShell.hpp"
#include "scan.hpp"

#pragma warning(disable: 4244)
#define GAMMATEST	0

/****************************************************************************/
typedef float	real;
typedef fixed	TScan;

#define ZCORRECT		1
#define LOADTEXTURE		0
#define LOADBUMP		0
#define PALETTEMONO		1

#if LOADBUMP
	typedef uint8	TBump;
	const int iBUMP_ONE = 32;
#else
	typedef int16	TBump;
	const int iBUMP_ONE = 128;				// The bump value for a one pixel height.
#endif

CMaterial matObject = matMETAL;


#define iTEXT_WIDTH		400
#define iTEXT_HEIGHT	100
#define iTEXT_DEPTH		8

typedef CVector3<real>	CVertex;		// prefix vt
typedef CVector2<real>	CTexCoord;		// prefix tcd

class CCoord
	: public CVertex
{
public:
	CTexCoord	tcdTex;

	CCoord()
	{
	}

	CCoord(const CVector3<real>& v3, const CTexCoord& tcd)
		: CVertex(v3), tcdTex(tcd)
	{
	}

	CCoord operator + (const CCoord& coord)
	{
		return CCoord((const CVertex&)*this + (const CVertex&)coord, tcdTex + coord.tcdTex);
	}

	CCoord& operator += (const CCoord& coord)
	{
		(CVertex&)*this += (const CVertex&)coord;
		tcdTex += coord.tcdTex;
		return *this;
	}

	CCoord operator - (const CCoord& coord)
	{
		return CCoord((const CVertex&)*this - (const CVertex&)coord, tcdTex - coord.tcdTex);
	}

	CCoord& operator -= (const CCoord& coord)
	{
		(CVertex&)*this -= (const CVertex&)coord;
		tcdTex -= coord.tcdTex;
		return *this;
	}

	CCoord operator * (real r)
	{
		return CCoord((const CVertex&)*this * r, tcdTex * r);
	}

	CCoord operator / (real r)
	{
		return CCoord((const CVertex&)*this / r, tcdTex / r);
	}
};

typedef CDir3<real>	CBumpDir;		// bmp

/****************************************************************************/
//
//	A few globals which let me transform a square in 3d.
//

bool bBumpMap = false;

const real rSIZE = 0.4;
const int iVERTICES = 4;

CCoord acoordPoly[iVERTICES] = 
{
	CCoord(CVertex(-rSIZE, -rSIZE, 0), CTexCoord(0, 0)),
	CCoord(CVertex(-rSIZE,  rSIZE, 0), CTexCoord(0, 1)),
	CCoord(CVertex( rSIZE,  rSIZE, 0), CTexCoord(1, 1)),
	CCoord(CVertex( rSIZE, -rSIZE, 0), CTexCoord(1, 0))
};

CDir3<real> d3Poly = 
	(acoordPoly[1] - acoordPoly[0]) ^ (acoordPoly[2] - acoordPoly[0]);
CColour clrPoly(30, 250, 180);
const int iCOLOUR_START = 10;
const int iCOLOUR_SHADES = 128;

//
// Light.
//

CLightAmbient		ltaAmbient(0.4);
CLightDirectional	ltdDir(0.6);

CLightList			ltlLights;
//CLight& ltlLights = ltaAmbient;

//
// Motion.
//

CPlacement3<real> p3Obj;				// The current object placement.
CTransform3<real> tf3Screen;			// The screen transform.
CVector3<real>	v3RadPerS;				// Rotation vector.

//
// Material.
//

CRaster* prasTexture;					// The texture to map.
CRaster* prasBumps;						// The bump map.
CRaster* prasTextureBumps;				// Combined texture and bump map.
CRasterSimple<CBumpDir> *prasNormals;	// Store normals of the bump map.

/****************************************************************************/
//
class CRasterBump
//
//**************************************
{
public:

	void* operator new(size_t);

	CRasterBump
	(
		CRaster& ras,						// The raster this object will draw to.
		CRaster& rast						// The raster this object will draw from.
	);

	//******************************************************************************************
	//
	virtual void RenderTexture
	(
		CScanPoly<real, CCoord>& scanp,		// Scan-polygon iterator.
		int i_light							// The lighting value to use.
	)
	{
	}

	//******************************************************************************************
	//
	virtual void RenderBump
	(
		CScanPoly<real, CCoord>& scanp,			// Scan-polygon iterator.
		CRasterSimple<CBumpDir>& ras_normals,	// Normal-map encoding bumps.
		CLight& lt								// The lighting to use.
	)
	{
	}

	//
	//**********************************

protected:

	CRasterBump()
	{
	}
};

//**********************************************************************************************
//
template<class R, class T> class CRasterBumpT: public CRasterBump
//
//**************************************
{
public:
	R&	Ras;
	T&	RasT;

	CRasterBumpT(R& ras, T& rast)
		: Ras(ras), RasT(rast)
	{
	}
	
	//******************************************************************************************
	//
	// Overrides.
	//

	void RenderTexture
	(
		CScanPoly<real, CCoord>& scanp,		// Scan-polygon iterator.
		int i_light							// The lighting value to use.
	);

	//******************************************************************************************
	//
	void RenderBump
	(
		CScanPoly<real, CCoord>& scanp,			// Scan-polygon iterator.
		CRasterSimple<CBumpDir>& ras_normals,	// Normal-map encoding bumps.
		CLight& lt								// The lighting to apply.
	);
};

//**********************************************************************************************
//
// CRasterBump implementation.
//

	void* CRasterBump::operator new(size_t)
	{
		return ::operator new(sizeof(CRasterBumpT<CRaster8, CRaster8>));
	}

	#define TryCRaster(pv, R, T, ras, rast)				\
	{													\
		R* pr;											\
		if (pr = dynamic_cast<R*>(&ras))				\
		{												\
			::new(pv) CRasterBumpT<R, T>(*pr, rast);	\
			return;										\
		}												\
	}

	
	#define ConstructRasterBump(pv, T, ras, rast)		\
	{													\
		TryCRaster(pv, CRaster8, T, ras, rast);			\
		TryCRaster(pv, CRaster16, T, ras, rast);		\
		TryCRaster(pv, CRaster24, T, ras, rast);		\
		Assert(0);										\
	}

	#define TryTRaster(TRas)							\
	{													\
		TRas*	tr;										\
		if (tr = dynamic_cast<TRas*>(&rast))			\
			ConstructRasterBump(this, TRas, ras, *tr);	\
	}

	CRasterBump::CRasterBump(CRaster& ras, CRaster& rast)
	{	
		TryTRaster(CRaster8);
		TryTRaster(CRaster16);
		TryTRaster(CRaster24);
		Assert(0);
	}

CRasterBump* pRasBump;					// The object to draw to.

/****************************************************************************/
//
//	Now the operations.
//

/****************************************************************************/

static fixed A = 2, B = 3, C, D;

void Junk()
{
//	C = A * B;
	D = B / A;
/*
	CSArray<int, 6> ari;
	ari << 5 << 4 << 3;

	if (ari[2] < ari[1])
	{
		CPArray<float> arf;
		arf.Set(3);
		if (arf.uLen > 2)
		{
			arf[2] = 2.0;
			arf[1] = 1.0;
			arf[0] = 0.0;
		}
	}
	ari << 10 << 20 << 30 << 40 << 50 << 60;
*/
}

inline int irand(int i_range)
{
	return rand() % (2*i_range) - i_range;
}

CRaster* prasMakeBumpTexture
(
	CRaster&	ras_texture
)
{
	CRaster* pras_bump = new CRasterMem(ras_texture.iWidth, ras_texture.iHeight, 32);

	for (int i_y = 0; i_y < ras_texture.iHeight; i_y++)
	{
		ras_texture.pAddress(0, i_y);
		pras_bump->pAddress(0, i_y);

		for (int i_x = 0; i_x < ras_texture.iWidth; i_x++)
		{
			CColour clr = ras_texture.clrFromPixel(ras_texture.pixGet());
			uint8 u1_max = Max(Max(clr.u1Red, clr.u1Green), clr.u1Blue);
			clr = clr * (1.0f / u1_max);
			clr.u1Flags = u1_max;
			pras_bump->PutPixel(clr);
		}
	}

	return pras_bump;
}

void Init()
{
	Junk();

	ltdDir.p3Ref() = CRotate3<>(d3ZAxis, CDir3<>(0, -1, -1));

	ltlLights.push_back(&ltaAmbient);
	ltlLights.push_back(&ltdDir);

	//
	// Create our texture.
	// First check to see whether already correct bit depth;
	//

	int i_y;

	const int iTEXTURE_DIM = 256;

#if LOADTEXTURE
	prasTexture = prasReadBMP("BinData\\Texture.bmp");
#else
	const int iDITHER = 8;

	prasTexture = new CRasterMem(iTEXTURE_DIM, iTEXTURE_DIM, 24);

	// Fill it with a colour gradient, using blue and green.
	for (i_y = 0; i_y < iTEXTURE_DIM; i_y++) 
	{
		prasTexture->pAddress(0, i_y);
		for (int i_x = 0; i_x < iTEXTURE_DIM; i_x++) 
		{
			prasTexture->PutPixel(prasTexture->pixFromColour(CColour(
				0,
				MinMax((i_y * 255 + irand(iDITHER*iTEXTURE_DIM)) / (iTEXTURE_DIM-1), 0, 255),
				MinMax((i_x * 255 + irand(iDITHER*iTEXTURE_DIM)) / (iTEXTURE_DIM-1), 0, 255)
			)));
		}
	}
#endif

#if LOADBUMP
	prasBumps = prasReadBMP("BinData\\Bump.bmp");
#else
	//
	// Create a bump map, a 16-bit height map, same dimensions as texture.
	//
	prasBumps = new CRasterMem(prasTexture->iWidth, prasTexture->iHeight, 16);

	// Fill it with a ripple.
	float f_width = (float) Max(prasTexture->iWidth, prasTexture->iHeight);
	for (i_y = 0; i_y < prasTexture->iHeight; i_y++) 
	{
		int16* pi2_bump = (int16*) prasBumps->pAddress(0, i_y);
		for (int i_x = 0; i_x < prasTexture->iWidth; i_x++) 
		{
			float f_radius = 
				(float) hypot(i_x - prasTexture->iWidth/2, i_y - prasTexture->iHeight/2) / f_width;
#if 0
			// Create a simple cone.
			*pi2_bump++ = int16 (100 * iBUMP_ONE * (1.0 - f_radius));
#elif 0
			// Create a concentric ripple, diminishing with radius.
			*pi2_bump++ =
				int16 (20 * iBUMP_ONE * (1.0 + cos(24.0 * f_radius)) / 2.0 * (1.0 - f_radius));
#elif 0
			// Create little knobs.
			const int iRADIUS = 6;
			int i_xm = i_x % (4*iRADIUS) - (2*iRADIUS), i_ym = i_y % (4*iRADIUS) - (2*iRADIUS);
			float f_dist = hypot(i_xm, i_ym) / iRADIUS;
	#if 0
			// Flat knobs.
			*pi2_bump++ = iRADIUS * iBUMP_ONE * (f_dist <= 1.0);
	#else
			// Round knobs.
			*pi2_bump++ = f_dist < 1.0 ? (int16) (iRADIUS * iBUMP_ONE * cos(f_dist)) : 0;
	#endif
#elif 0
			// Create a criss-crossed wave.
			*pi2_bump++ =
				int16 (10 * iBUMP_ONE * (2.0 + sin(i_x / 10.0) + sin(i_y / 6.0)) / 4.0);
#elif 1
			// Make another shape.

			const int iRADIUS = iTEXTURE_DIM/4;
			int i_xm = i_x % (iTEXTURE_DIM/2) - iRADIUS, i_ym = i_y - iTEXTURE_DIM/2;
			float f_dist = hypot(i_xm, i_ym) / iRADIUS;

			*pi2_bump++ = f_dist < 1.0 ? 
				(int16) (iRADIUS * iBUMP_ONE * cos(f_dist)) +
					(f_dist < 0.1 ? iRADIUS/10 * iBUMP_ONE * cos(f_dist*10) : 0)
				: 0;
#endif
		}
	}
#endif

	//
	// Create the normal map from the bump map.
	//
	prasNormals = new CRasterSimple<CBumpDir>(prasBumps->iWidth, prasBumps->iHeight);
	for (i_y = 0; i_y < prasBumps->iHeight; i_y++) 
	{
		TBump* pi_bump = (TBump*) prasBumps->pAddress(0, i_y);
		CBumpDir* pd3 = prasNormals->ptAddress(0, i_y);

		for (int i_x = 0; i_x < prasBumps->iWidth; i_x++)
		{
			*pd3++ = CBumpDir
			(
				real(i_x? MinMax((int)pi_bump[-1] - (int)*pi_bump, -iBUMP_ONE, iBUMP_ONE) : 0),
				real(i_y? MinMax((int)pi_bump[-prasBumps->iLinePixels] - (int)*pi_bump, -iBUMP_ONE, iBUMP_ONE) : 0),
				real(-iBUMP_ONE)
			);
			pi_bump++;
		}
	}
}

//**********************************************************************************************
//
template<class T> class CScreenTranslate: public CTransform3<T>
//
// Contains a matrix that will translate a 3D point by x and y in screenspace,
// and also add z to the worldspace z value.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructor.
	//

	//**********************************************************************************************
	//
	CScreenTranslate
	(
		T t_x, T t_y,			// Translation in screenspace.
		T t_z					// Bias to add to z values.
	)
	//
	//**********************************
	{
		//
		// If vector (x,y,z) converts to (x/(z+t_z), y/(z+t_z)) in screenspace,
		// then in order to add t_x and t_y to the destination:
		//
		//	x/(z+t_z) + t_x = (x + t_x*z + t_x*t_z)/(z+t_z)
		//
		// we must add t_x*z + t_x*t_z to x.
		//
		//
		// x			y			z
		// =			=			=
		// x
		//				y
		// + t_x*z		+ t_y*z		z
		// + t_x*t_z	+ t_y*t_z	+ t_z

		mx3T.v3Z.tX = t_x;
		mx3T.v3Z.tY = t_y;
	    v3T.tX = t_x * t_z;
		v3T.tY = t_y * t_z;
		v3T.tZ = t_z;
	}
};

void NewRaster(CRaster& ras)
{
	// Create an appropriate drawing object.
	delete pRasBump;
	pRasBump = new CRasterBump(ras, *prasTexture);

	if (ras.iPixelBits == 8) 
	{
		// It's palettised; create and attach a palette.
		CPal* ppal = new CPal;

#if PALETTEMONO
		// Create a greyscale palette.
		for (int i = 0; i < 256; i++)
		{
			ppal->aclrPalette[i].u1Blue  = 
			ppal->aclrPalette[i].u1Green =
			ppal->aclrPalette[i].u1Red   = i;
		}
		ras.AttachPalette(ppal, &pxfMono);
#else
		// Create an 8-bit colour cube palette.
		int i = 0;
		for (int i_r = 0; i_r < 8; i_r++)
			for (int i_g = 0; i_g < 8; i_g++)
				for (int i_b = 0; i_b < 4; i_b++) 
				{
					if (i == 255)
						break;
					ppal->aclrPalette[i].u1Blue = (i_b*255) / 3;
					ppal->aclrPalette[i].u1Green = (i_g*255) / 7;
					ppal->aclrPalette[i].u1Red = (i_r*255) / 7;
					i++;
				}
		ras.AttachPalette(ppal, &pxf332);
#endif
	}

	// Create the screen transform.

	int i_dim = Min(ras.iWidth, ras.iHeight);
	tf3Screen = CScale3<real>((i_dim * ras.fAspectRatio), -i_dim, 1)
		* CScreenTranslate<real>(ras.iWidth / 2, ras.iHeight / 2, 1);
}

/****************************************************************************/
void Step()
{
	p3Obj *= CRotate3<real>(v3RadPerS * f_ms / 1000);
/*
	static CAngle angle = 0;

	angle += CAngle((double) (r_rad_per_ms * tmr.msElapsed()) );
	p3Obj = CRotate3<real>();
#if ROTATEZ
	p3Obj *= CRotate3<real>('z', angle);
#endif
#if ROTATEX
	p3Obj *= TransformAt(CRotate3<real>('x', angle<<1), CVector3<real>(0, 0.2, 0));
#endif
*/
}

/****************************************************************************/
template<class CRas, class TRas> void CRasterBumpT<CRas, TRas>::RenderTexture
(
	CScanPoly<real, CCoord>& scanp,		// Scan-polygon iterator.
	int i_light							// The lighting value to use.
)
{
	for (; scanp++; ) 
	{
#if !ZCORRECT
		if (scanp.iX1 >= scanp.iX2)
			continue;

		scanp.coordLeft.tcdTex /= scanp.coordLeft.tZ;
		scanp.coordRight.tcdTex /= scanp.coordRight.tZ;

		SetMinMax(scanp.coordLeft.tcdTex.tX, 0, RasT.iWidth-1);
		SetMinMax(scanp.coordLeft.tcdTex.tY, 0, RasT.iHeight-1);
		SetMinMax(scanp.coordRight.tcdTex.tX, 0, RasT.iWidth-1);
		SetMinMax(scanp.coordRight.tcdTex.tY, 0, RasT.iHeight-1);

		scanp.coordInc.tcdTex = (scanp.coordRight.tcdTex - scanp.coordLeft.tcdTex) / (scanp.iX2 - scanp.iX1);
#endif

		CRas::TPix* p_addr = Ras.ptAddress(scanp.iX1, scanp.iY);

		for (int i_x = scanp.iX1; i_x < scanp.iX2; i_x++)
		{
#if ZCORRECT
			real r_zinv = 1 / scanp.coordLeft.tZ;
			int i_u = (int) (scanp.coordLeft.tcdTex.tX * r_zinv);
			int i_v = (int) (scanp.coordLeft.tcdTex.tY * r_zinv);
#else
			int i_u = (int) scanp.coordLeft.tcdTex.tX;
			int i_v = (int) scanp.coordLeft.tcdTex.tY;
#endif
			*p_addr++ = Ras.CRas::pixFromColour(
				RasT.TRas::clrFromPixel(RasT.TRas::pixGet(i_u, i_v))
				* (uint8) i_light
			);
			scanp.coordLeft.tcdTex += scanp.coordInc.tcdTex;
#if ZCORRECT
			scanp.coordLeft.tZ += scanp.coordInc.tZ;
#endif
		}
	}
}

/****************************************************************************/

template<class CRas, class TRas> void CRasterBumpT<CRas, TRas>::RenderBump
(
	CScanPoly<real, CCoord>& scanp,		// Scan-polygon iterator.
	CRasterSimple<CBumpDir>& ras_normals,
										// Normal-map encoding bumps.
	CLight& lt							// The lighting to apply.
)
{
	for (; scanp++; ) 
	{
#if !ZCORRECT
		if (scanp.iX1 >= scanp.iX2)
			continue;

		scanp.coordLeft.tcdTex /= scanp.coordLeft.tZ;
		scanp.coordRight.tcdTex /= scanp.coordRight.tZ;

		SetMinMax(scanp.coordLeft.tcdTex.tX, 0, RasT.iWidth-1);
		SetMinMax(scanp.coordLeft.tcdTex.tY, 0, RasT.iHeight-1);
		SetMinMax(scanp.coordRight.tcdTex.tX, 0, RasT.iWidth-1);
		SetMinMax(scanp.coordRight.tcdTex.tY, 0, RasT.iHeight-1);

		scanp.coordInc.tcdTex = (scanp.coordRight.tcdTex - scanp.coordLeft.tcdTex) / (scanp.iX2 - scanp.iX1);
#endif

		CRas::TPix* p_addr = Ras.ptAddress(scanp.iX1, scanp.iY);

		for (int i_x = scanp.iX1; i_x < scanp.iX2; i_x++)
		{
#if ZCORRECT
			real r_zinv = 1 / scanp.coordLeft.tZ;
			int i_u = (int) (scanp.coordLeft.tcdTex.tX * r_zinv);
			int i_v = (int) (scanp.coordLeft.tcdTex.tY * r_zinv);
#else
			int i_u = (int) scanp.coordLeft.tcdTex.tX;
			int i_v = (int) scanp.coordLeft.tcdTex.tY;
#endif

			// Get the normal for this bump map, and transform it.
			CBumpDir* pd3Bump = ras_normals.ptAddress(i_u, i_v);

			TReflectVal lv = lt.lvGetLighting(CVector3<>(0, 0, 0), *pd3Bump, matObject);

			CColour clr = RasT.TRas::clrFromPixel(
				RasT.TRas::pixGet(i_u, i_v)
			);
			clr = clr * (lv / rvMAX_COLOURED);
			*p_addr++ = Ras.CRas::pixFromColour(clr);

			scanp.coordLeft.tcdTex += scanp.coordInc.tcdTex;
#if ZCORRECT
			scanp.coordLeft.tZ += scanp.coordInc.tZ;
#endif
		}
	}
}

void ConShowRaster(CConsoleBuffer& con, CRasterWin& rasw)
{
	rasw.Unlock();
	HDC hdc = rasw.hdcGet();
	con.SetTransparency(true);
	con.Show(hdc, rasw.iHeight, 1);
	con.SetTransparency(false);
	rasw.ReleaseDC(hdc);
}

/****************************************************************************/
void Paint(CRaster& ras, CRasterBump& rasb)
{
	ras.Clear(0);

	// Draw the colourmap in the upper left.
	if (ras.iPixelBits == 8) 
	{
		for (int i = 0; i < 256; i++)
			ras.Rect(i, SRect((i%16)*8, (i/16)*8, 8, 8));
	}

//	msPalette += tmr.msElapsed();

	// Construct the total transformation for the coords.
	CTransform3<real> tf3_total = p3Obj * tf3Screen;

	// Figure the transformed light direction.

	ltlLights.SetViewingContext(~p3Obj, CDir3<>(0, 0, -1) * ~p3Obj);
	TReflectVal lv = ltlLights.lvGetLighting(CVector3<>(0, 0, 0), d3Poly, matObject);
	int i_light = lv * (TReflectVal) 255 / rvMAX_COLOURED;

	Assert(bWithin(i_light, 0, 255));

	TPixel pix = ras.pixFromColour(CColour(clrPoly * (uint8) i_light));

	CCoord acoordScr[iVERTICES];
	for (int i = 0; i < iVERTICES; i++)
	{
		CVector3<real> v3 = acoordPoly[i] * tf3_total;

		acoordScr[i].tZ = 1 / v3.tZ;
		acoordScr[i].tX = v3.tX * acoordScr[i].tZ;
		acoordScr[i].tY = v3.tY * acoordScr[i].tZ;
		acoordScr[i].tcdTex.tX = (prasTexture->iWidth-1) * acoordPoly[i].tcdTex.tX * acoordScr[i].tZ;
		acoordScr[i].tcdTex.tY = (prasTexture->iHeight-1) * acoordPoly[i].tcdTex.tY * acoordScr[i].tZ;
	}

	// Draw the polygon.
	if (bBumpMap)
		rasb.RenderBump(CScanPoly<real, CCoord>(
			acoordScr, iVERTICES, 0, 0, ras.iWidth, ras.iHeight
		), *prasNormals, ltlLights);
	else
		rasb.RenderTexture(CScanPoly<real, CCoord>(
			acoordScr, iVERTICES, 0, 0, ras.iWidth, ras.iHeight
		), i_light);

	CRasterWin *prasw = dynamic_cast<CRasterWin*>(&ras);

	if (StatDB.iCount)
	{
		conStd.SetCursorPosition(0, 0);
		conStd.Print("Screen %d x %d x %d, Video %d, Aspect %.4f     \n",
			ras.iWidth, ras.iHeight, ras.iPixelBits, prasw->setVideoMem.iMask, ras.fAspectRatio);
		conStd.Print("BumpMap = %d, LightDir = %.2f, LightAmbient = %.2f, SpecPower %.2f    \n",
			bBumpMap, ltdDir.lvIntensity, ltaAmbient.lvIntensity, matObject.fSpecularFalloff);
		conStd.Print("Frame %d, Clear %d, Paint %d, Text %d, Flip %d      \n", 
			StatDB["Frame"]	/ StatDB.iCount,
			StatDB["Clear"]	/ StatDB.iCount, 
			StatDB["Render"]/ StatDB.iCount, 
			StatDB["Text"]	/ StatDB.iCount,
			StatDB["Flip"]	/ StatDB.iCount
		);
		if (prasw->bWindowed)
			conStd.Show();
	}

	if (!prasw->bWindowed)
		ConShowRaster(conStd, *prasw);
}

class CAppShellBump: public CAppShell
{
protected:
#if GAMMATEST
	uint8	u1Intensity;
	CColour	clrTest;
#endif

	int iShiftSense()
	{
		return bKeyState(VK_SHIFT) ? 1 : -1;
	}

public:
	void Init()
	{
#if GAMMATEST
		u1Intensity = 128;
		clrTest = CColour(255, 255, 255);
#else
		static bool b_init = false;
		if (b_init)
			return;
		b_init = true;
		SetContinuous(1);
		::Init();
#endif
	}

	void NewRaster()
	{
#if !GAMMATEST
		Init();
		::NewRaster(*prasMainScreen);
		HideCursor(!prasMainScreen->bWindowed);
#endif
	}

/*
	void Resize()
	{
	}

	void Paused(bool b_paused)
	{
	}
*/

	void Step()
	{
#if !GAMMATEST
		::Step();
		Repaint();
#endif
	}

	void Paint()
	{
#if GAMMATEST
		prasMainScreen->Clear(0);

		// Draw a patch of solid and stippled rects.
		const int i_size = 32;
		for (int i_y = i_size*2; i_y <= prasMainScreen->iHeight - i_size; i_y += i_size)
			for (int i_x = 0; i_x <= prasMainScreen->iWidth - i_size; i_x += i_size)
			{
				if ((i_x + i_y) % (i_size*2) == 0)
				{
					// Draw solid rect.
					prasMainScreen->Rect(
						prasMainScreen->pixFromColour(clrTest * u1Intensity),
						SRect(i_x, i_y, i_size, i_size)
					);
				}
				else
				{
					// Draw black/white stipples.
					TPixel pix_black = prasMainScreen->pixFromColour(CColour(0, 0, 0));
					TPixel pix_white = prasMainScreen->pixFromColour(clrTest);

					for (int i_v = 0; i_v < i_size; i_v++)
						for (int i_u = 0; i_u < i_size; i_u++)
						{
							prasMainScreen->PutPixel(
								i_x + i_u, i_y + i_v, 
								(i_u + i_v) % 2 == 0 ? pix_black : pix_white
							);
						}
				}
			}

		conStd.SetCursorPosition(0, 0);
		/*
			0.5 = I^g
			log(I) 0.5 = g
			g = log 0.5 / log(I)
		*/
		float f_gamma = log(0.5) / log((float)u1Intensity / 255.0);
		conStd.Print("Intensity = %d, Gamma = %f, 1/Gamma = %f\n",
			u1Intensity, f_gamma, 1.0f / f_gamma);
		ConShowRaster(conStd, *prasMainScreen);
#else
		::Paint(*prasMainScreen, *pRasBump);
#endif
	}

	void KeyPress(int i_key)
	{
#if GAMMATEST
		switch (i_key)
		{
			case VK_ADD:
				u1Intensity ++;
				break;
			case VK_SUBTRACT:
				u1Intensity --;
				break;
			case VK_MULTIPLY:
				clrTest.u1Blue ^= 255;
				if (!clrTest.u1Blue)
				{
					clrTest.u1Green ^= 255;
					if (!clrTest.u1Green)
						clrTest.u1Red ^= 255;
				}
				break;
		}
				
		Repaint();
#else
		switch (i_key)
		{
			case 'P':
				if (bKeyState(VK_SHIFT))
					// Shifted = up.
					matObject.fSpecularFalloff *= 2;
				else
					// Unshifted = down.
					matObject.fSpecularFalloff /= 2;
				break;

			case 'D':
				ltdDir.lvIntensity += 0.1 * iShiftSense();
				SetMinMax(ltdDir.lvIntensity, 0, 1);
				break;

			case 'A':
				ltaAmbient.lvIntensity += 0.1 * iShiftSense();
				SetMinMax(ltaAmbient.lvIntensity, 0, 1);
				break;

			case 'X':
				v3RadPerS.tX += 0.1 * iShiftSense();
				break;

			case 'Y':
				v3RadPerS.tY += 0.1 * iShiftSense();
				break;

			case 'Z':
				v3RadPerS.tZ += 0.1 * iShiftSense();
				break;

			case 'R':
				v3RadPerS = CVector3<real>(0, 0, 0);
				break;

			case 'B':
				bBumpMap ^= 1;
				StatDB.Reset();
				break;
		}
#endif
	}
};

CAppShell* pappMain = new CAppShellBump;

