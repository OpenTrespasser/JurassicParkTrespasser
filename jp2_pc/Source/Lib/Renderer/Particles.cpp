/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Implementation of Particles.hpp.
 *
 * To do:
 *		Don't even create particles whose origin is out of the camera view.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Particles.cpp                                            $
 * 
 * 45    10/03/98 5:02p Pkeet
 * Removed the normalized camera bias.
 * 
 * 44    98/10/03 4:43 Speter
 * Oops. Default gravity should be 5, not 0 to 5 range.
 * 
 * 43    98.09.30 10:38p Mmouni
 * Added support bUseTerrainColour.
 * 
 * 42    98/09/28 2:46 Speter
 * Removed clamp on particle effect intensity.
 * 
 * 41    98.09.24 1:40a Mmouni
 * Shrunk CRenderPolygon by a large amount.
 * Multiple list depth sort stuff is now on a compile switch.
 * 
 * 40    98/09/23 15:09 Speter
 * Corrected projection code to take account of depth. Reverted v3Position() to old behaviour
 * that causes tail to exactly follow head.
 * 
 * 39    98/09/23 0:12 Speter
 * Turned down wind. Stricter screen size limits.
 * 
 * 38    98/09/19 14:47 Speter
 * Changed method of randomising particle params, for more control. Added a wind component. Dust
 * is now constant screen size.
 * 
 * 37    98/09/17 3:42 Speter
 * Made dust trajectory asymptotic, controlled by fAccelStuck; fAccelUnstuck now enables regular
 * gravity. Added screen min and max sizes to fCamSize, so particles are always shown. Shrink
 * solid particles with age.Removed fScreenSize() functions. 
 * 
 * 36    9/08/98 10:25p Pkeet
 * Added a dust particle type.
 * 
 * 35    9/08/98 7:09p Pkeet
 * Fixed sorting for fluid particles.
 * 
 * 34    9/08/98 6:31p Pkeet
 * Fixed bug of solid particles not sorting against the terrain.
 * 
 * 33    98/09/05 23:24 Speter
 * Added max particle limit.
 * 
 * 32    98/08/28 19:50 Speter
 * Added CParticlesCreate::fIntensity(); Create() now takes a strength param in event units,
 * rather than an interp param.
 * 
 * 31    8/27/98 1:51p Asouth
 * one change for MSL (vs. HP STL); statics scoped to the class that defines them
 * 
 * 30    98/08/27 1:01 Speter
 * Put a clamp on particle effect intensity (2.0 times nominal max). Fixed bug involving small
 * step times.
 * 
 * 29    98/08/25 19:05 Speter
 * Replaced sStickTime with fLength parameter, which calculates former. Increased Z depth sort
 * bias. Again fixed acceleration calc.
 * 
 * 28    8/21/98 11:07p Rwycko
 * Fixed several bugs in text prop loading.
 * 
 * 27    98/08/21 18:05 Speter
 * Added text prop loading. Moved fAccelStuck and fAccelUnstuck to SParticleParams. Added
 * fThreshold, bLogScale. Added default constructors. Fixed stuck acceleration calculation.
 * Added bias to screen Z to make sorting better.
 * 
 **********************************************************************************************/

//
// Defines and pragmas.
//
#include "common.hpp"
#include "Particles.hpp"

#include "Lib/W95/Direct3D.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "D3DTypes.h"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "PipeLine.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Std/PrivSelf.hpp"

#include "RenderDefs.hpp"
#include "Camera.hpp"
#include "ScreenRender.hpp"
#include "PipeLineHeap.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/Transform/Matrix2.hpp"

#include "Lib/Groff/VTParse.hpp"

#include "Lib/GeomDBase/Terrain.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"

#include <map>
#include <math.h>
#include <memory.h>

#define bDEBUG_PARTICLES (1 && BUILDVER_MODE != MODE_FINAL)

#if bDEBUG_PARTICLES
	#include "Lib/Sys/DebugConsole.hpp"
#endif // bDEBUG_DIRECT3D

//
// Macros.
//


//
// Module specific constants.
//

// A really long time for a particle.
const TSec sReallyOld		= 1000.0f;
const int iMAX_PARTICLES	= 300;

// The default wind velocity vector.
const CVector3<> v3Wind		(0.25f, 0.0f, 0.0f);

// Near clip values for normalized camera space.
const float  fNearClipNorm  = 0.1f / 1000.0f;
const uint32 u4NearClipNorm = u4FromFloat(fNearClipNorm);

// Screen size limits (pixels).
const float fScrSizeMin		= 1.0f;
const float fScrSizeMax		= 5.0f;

// Points for an equilateral triangle.
CVector3<> v3TriA = CVector3<>( 0.0f,   0.0f, 1.0f);
CVector3<> v3TriB = CVector3<>( 0.866f, 0.0f, -0.5f);
CVector3<> v3TriC = CVector3<>(-0.866f, 0.0f, -0.5f);

CProfileStat psParticlesUpdate("Particles Upd", &proProfile.psRender),
			 psParticlesRender("Particles Ren", &proProfile.psRender);

//
// Module specific variables.
//

// Random number for the module.
static CRandom rndParticles;


//
// Internal class definitions.
//

//******************************************************************************************
//
class CTextureColours : public std::map< TD3DPixel, CTexture*, std::less<TD3DPixel> >
//
// Less than comparison class for texture colours.
//
// Prefix: texcol
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors and destructors.
	//

	// Default constructor.
	CTextureColours()
	{
	}

	// Destructor.
	~CTextureColours()
	{
		Reset();
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Reset()
	{
		iterator it = begin();
		for (; it != end(); ++it)
		{
			CTexture* ptex = (*it).second;
			delete ptex;
		}
		erase(begin(), end());
	}
	//
	// Releases all shared surfaces and removes them from the STL map.
	//
	//**************************************

	//******************************************************************************************
	//
	CTexture* ptexFind
	(
		CColour clr
	)
	//
	// Returns a pointer to a shared surface if it has one with the given hash value, otherwise
	// returns zero.
	//
	//**************************************
	{
		iterator it = find(clr.d3dcolGetD3DColour());
		if (it == end())
		{
			CTexture* ptex = new CTexture();

			// Add the colour to the texture for software and hardware.
			ptex->tpSolid      = prasMainScreen->pixFromColour(clr);
			ptex->d3dpixColour = clr.d3dcolGetD3DColour();

			// Add the texture to the map.
		#ifdef __MWERKS__
			insert(pair<const TD3DPixel, CTexture*>(clr.d3dcolGetD3DColour(), ptex));
		#else
			insert(std::pair<TD3DPixel, CTexture*>(clr.d3dcolGetD3DColour(), ptex));
		#endif

			// Find the colour back.
			it = find(clr.d3dcolGetD3DColour());
		}
		return (*it).second;
	}
};

class CCreateParticles::CPriv : public CCreateParticles
{
public:

	//*************************************************************************************
	//
	void SetTexture
	(
		CTextureColours* ptexcol
	)
	//
	// Set the texture based on the colour.
	//
	//**************************
	{
		Assert(ptexcol);

		// Get the texture associated with the colour.
		ptexTexture = ptexcol->ptexFind(clrColour);
	}

	//*********************************************************************************************
	//
	int iCreateNumParticles
	(
		TSec s_period = 1.0		// Time scaling for number of particles created.
	) const
	//
	// Returns:
	//		A random integral number of particles, based on min and max ranges, and period.
	//
	//**************************
	{
		// Randomise the number of particles created based on ranges and s_period.
		float f_num = rndParticles(ppMin.fNumParticles, ppMax.fNumParticles) * s_period;

		// Convert float to an int, and randomly apply the remainder.
		int i_num = iPosFloatCast(f_num);
		if (rndParticles(0.0, 1.0) < f_num - i_num)
			i_num++;

		return i_num;
	}

};


//*********************************************************************************************
//
class CParticleBase
//
// Object describes a single renderable particle.
//
// Prefix: prt
//
// Notes:
//		The class contains a prebuilt polygon with its own vertex list as a means of
//		simplifying and speeding up construction of polygons to add to the polygon list.
//
//**************************
{
public:

	TSec       sAge;				// Amount of time the particle is alive.
	TSec       sLifespan;			// Amount of time the particle will last.
	TSec       sStickTime;			// Amount of time the particle will 'stick.'
	bool       bRendered;			// Set to 'true' if the particle is rendered at least once.
	bool       bOneFrame;			// Allow the particle to exist for only one frame.
	float      fAngle;				// Angle of particle.
	float      fAngularVelocity;	// Angular velocity of particle.
	float      fAccelStuck;
	float      fAccelUnstuck;
	float      fSize;
	CVector3<> v3StartPosition;		// Starting position of the particle.
	CVector3<> v3StartVelocity;		// Starting velocity of the particle.
	float	   fStartVelocity;
	float	   fFinalDist;			// For dust.

	SRenderVertex  rvVertices[4];
	SRenderVertex* prvVertices[4];
	CRenderPolygon rpolyPolygon;

	CTexture* ptexTexture;

	static float fScreenWidth;
	static float fScreenHeight;

public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CParticleBase()
		: ptexTexture(0)
	{
		SetPolygon();
	}

	// Copy constructor.
	CParticleBase(const CParticleBase& particle)
	{
		memcpy(this, &particle, sizeof(particle));
		SetPolygon();
	}

	// Parameter constructor.
	CParticleBase(const CVector3<>& v3_origin, const CDir3<>& d3_dir, const CCreateParticles& crt);


	//*****************************************************************************************
	//
	// Member functions.
	//
	
	//*****************************************************************************************
	//
	void Update
	(
		TSec sec
	)
	//
	// Updates the particles by physically moving them.
	//
	//**************************
	{
		sAge += sec;
	}

	//*****************************************************************************************
	//
	virtual bool bAdd
	(
		const CCamera& cam,
		const CTransform3<>& tf3_to_norm_cam
	)
	//
	// Prepares the particle to add to the renderer.
	//
	//**************************
	{
		Assert(0);
		return false;
	}

	//*****************************************************************************************
	//
	bool bRetired
	(
	)
	//
	// Returns 'true' if the particle is too old to live.
	//
	//**************************
	{
		if (bOneFrame)
		{
			return bRendered;
		}
		return sAge >= sLifespan;
	}

	//*****************************************************************************************
	//
	virtual CVector3<> v3Position
	(
		TSec sec	// Time to determine the position of the particle for.
	) const;
	//
	// Returns the position of the particle at the given time.
	//
	//**************************

protected:

	//*****************************************************************************************
	bool bNotInScreen();

	//*****************************************************************************************
	bool bNotInCamera();

	//*****************************************************************************************
	virtual float fCamSize(const CCamera& cam, float f_cam_y) const;

	//*****************************************************************************************
	void SetPolygon();

#if bDEBUG_PARTICLES
	//*****************************************************************************************
	void Validate();
#endif // bDEBUG_PARTICLES

};


//*********************************************************************************************
//
class CParticleFluid : public CParticleBase
//
// Object describes a single renderable particle.
//
// Prefix: prt
//
// Notes:
//		The class contains a prebuilt polygon with its own vertex list as a means of
//		simplifying and speeding up construction of polygons to add to the polygon list.
//
//**************************
{
public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CParticleFluid()
		: CParticleBase()
	{
		Initialize();
	}

	// Copy constructor.
	CParticleFluid(const CParticleFluid& particle)
		: CParticleBase(particle)
	{
		Initialize();
	}

	// Parameter constructor.
	CParticleFluid(const CVector3<>& v3_origin, const CDir3<>& d3_dir, const CCreateParticles& crt)
		: CParticleBase(v3_origin, d3_dir, crt)
	{
		Initialize();
	}


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual bool bAdd
	(
		const CCamera& cam,
		const CTransform3<>& tf3_to_norm_cam
	) override;
	//
	// Prepares the particle to add to the renderer.
	//
	//**************************

protected:

	//*****************************************************************************************
	void Initialize();

};


//*********************************************************************************************
//
class CParticleSolid : public CParticleBase
//
// Object describes a single renderable particle.
//
// Prefix: prt
//
// To do:
//		Implement particle rotation if required.
//
//**************************
{
public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CParticleSolid()
		: CParticleBase()
	{
		Initialize();
	}

	// Copy constructor.
	CParticleSolid(const CParticleSolid& particle)
		: CParticleBase(particle)
	{
		Initialize();
	}

	// Parameter constructor.
	CParticleSolid(const CVector3<>& v3_origin, const CDir3<>& d3_dir, const CCreateParticles& crt)
		: CParticleBase(v3_origin, d3_dir, crt)
	{
		Initialize();
	}


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual bool bAdd
	(
		const CCamera& cam,
		const CTransform3<>& tf3_to_norm_cam
	) override;
	//
	// Prepares the particle to add to the renderer.
	//
	//**************************

protected:

	//*****************************************************************************************
	void Initialize();

};


//*********************************************************************************************
//
class CParticleDust : public CParticleSolid
//
// Object describes a single renderable dust particle.
//
// Prefix: prt
//
// To do:
//		Implement particle rotation if required.
//
//**************************
{
public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CParticleDust()
		: CParticleSolid()
	{
		Setup();
	}

	// Copy constructor.
	CParticleDust(const CParticleSolid& particle)
		: CParticleSolid(particle)
	{
		Setup();
	}

	// Parameter constructor.
	CParticleDust(const CVector3<>& v3_origin, const CDir3<>& d3_dir, const CCreateParticles& crt)
		: CParticleSolid(v3_origin, d3_dir, crt)
	{
		Setup();
	}


	//*****************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual CVector3<> v3Position(TSec sec) const override;

protected:

	//*****************************************************************************************
	void Setup();

	//*****************************************************************************************
	virtual float fCamSize(const CCamera& cam, float f_cam_y) const override
	{
		// fSize refers to screen pixels, so convert to camera space.
		return fSize * f_cam_y / (fScreenWidth * 0.5f);
	}

};

//*********************************************************************************************
//
class CParticleList : public std::list<CParticleBase>
//
// Object describes a list of particles.
//
// Prefix: pprt
//
// Notes:
//		This is a trivial class to allow a simplified interface into the main particle system
//		class.
//
//**************************
{
public:
};


//
// Class implementations.
//

//*********************************************************************************************
//
// Class CParticleBase implementation.
//

	//*****************************************************************************************
	CParticleBase::CParticleBase(const CVector3<>& v3_origin, const CDir3<>& d3_dir, const CCreateParticles& crt)
		: ptexTexture(crt.ptexTexture), sAge(0.0f), bRendered(false), bOneFrame(false)
	{
		Assert(ptexTexture);

		SetPolygon();

		// Determine the lifespan of the particle.
		bOneFrame = crt.ppMax.sLifespan < 0.01f;

		// Get random parameters.
		//SParticleParams pp = crt.ppMin.ppRandom(crt.ppMax);
		SParticleParams pp = crt.ppMin.ppInterpolate(crt.ppMax, rndParticles(0.0, 1.0));
		fSize     = pp.fSize;
		sLifespan = pp.sLifespan;

		// Set the initial position of the particle.
		v3StartPosition = v3_origin;

		// Set the 'stick' time from length. Either velocity and/or stuck acceleration
		// can produce length. Take the min of the 2, while ensuring that we don't
		// divide by 0.
		sStickTime = Min
		(
			pp.fLength / Max(pp.fVelocity, .0001f),
			fSqrt(2.0 * pp.fLength / Max(pp.fAccelStuck, .0001f))
		);

		fAccelStuck = pp.fAccelStuck;
		fAccelUnstuck = pp.fAccelUnstuck;

		if (bOneFrame)
		{
			v3StartVelocity = CVector3<>(0.0f, 0.0f, 0.0f);
		}
		else
		{
			// Get a randomly determined velocity.
			v3StartVelocity  = d3_dir;
			v3StartVelocity *= pp.fVelocity;

			Update(rndParticles(0.005f, Max(0.005f, crt.sBeginAge)));
		}
	}

	//*****************************************************************************************
	CVector3<> CParticleBase::v3Position(TSec sec) const
	{
		CVector3<> v3_s = v3StartPosition;

		// Clamp the time.
		if (sec < 0.0f)
		{
			return v3_s;
		}

		//
		// A little bit of Newton for the 'Z' component:
		//
		//		s1 = s0 + v * t + 0.5 * (a' * t' * t' + a * t * t)
		//
		// And for the 'X' and 'Y' components:
		//
		//		s1 = s0 + v * t
		//
		v3_s    += v3StartVelocity * sec;

		// Apply stuck and unstuck acceleration.
		if (sAge <= sStickTime)
		{
			// Still stuck. This is the forward position; apply stuck acceleration.
			Assert(sec >= 0.0f);
			v3_s.tZ -= 0.5f * fAccelStuck * Sqr(sec);
		}
		else
		{
			// Now unstuck; apply stuck and unstuck acceleration.
			v3_s.tZ -= 0.5f * fAccelStuck * Sqr(sStickTime);
			v3_s.tZ -= 0.5f * fAccelUnstuck * Sqr(sec - sStickTime);
		}

		return v3_s;
	}

	//*****************************************************************************************
	//
	float CParticleBase::fCamSize
	(
		const CCamera& cam,
		float f_cam_y
	) const
	//
	// Returns the size of the particle in screenspace.
	//
	//**************************
	{
		float f_far = cam.campropGetProperties().rFarClipPlaneDist;

		// Calculate the dimension scaled for normalized camera space.
		float f_size = fSize / f_far;

		// Clamp the size to appropriate screen values.
		float f_to_cam = f_cam_y / (fScreenWidth * 0.5f);
		SetMinMax(f_size, fScrSizeMin * f_to_cam, fScrSizeMax * f_to_cam);

		// Return the calculated dimension.
		return f_size;
	}

	//*****************************************************************************************
	//
	bool CParticleBase::bNotInScreen
	(
	)
	//
	// Returns 'true' if the particle is touching a screen edge or entirely off the screen.
	//
	//**************************
	{
		// Test vertices.
		for (int i = 0; i < rpolyPolygon.paprvPolyVertices.uLen; ++i)
		{
			CVector3<> v3 = rvVertices[i].v3Screen;
			if (v3.tX < 1.0f)
				return true;
			if (v3.tX > fScreenWidth)
				return true;
			if (v3.tY < 1.0f)
				return true;
			if (v3.tY > fScreenHeight)
				return true;
			if (v3.tZ <= 0.0f)
				return true;
		}
		return false;
	}

	//*****************************************************************************************
	//
	bool CParticleBase::bNotInCamera
	(
	)
	//
	// Returns 'true' if the particle is touching or outside of the camera frustrum.
	//
	//**************************
	{
		// Test vertices.
		for (int i = 0; i < rpolyPolygon.paprvPolyVertices.uLen; ++i)
		{
			CVector3<> v3 = rvVertices[i].v3Cam;

			// Get an integer representation of y and test for a negative value.
			uint32 u4_y = u4FromFloat(v3.tY);

			// Fail if behind or on the camera.
			if (u4_y & 0x80000000)
				return true;
			if (u4_y < u4NearClipNorm)
				return true;

			// Fail if x or z is out of bounds.
			if ((u4FromFloat(v3.tX) & 0x7FFFFFFF) >= u4_y)
				return true;
			if ((u4FromFloat(v3.tZ) & 0x7FFFFFFF) >= u4_y)
				return true;
		}
		return false;
	}

	//*****************************************************************************************
	//
	void CParticleBase::SetPolygon
	(
	)
	//
	// Class sets up the required polygon structures.
	//
	//**************************
	{
		rvVertices[0].cvIntensity = 1.0f;
		rvVertices[0].tcTex.tX    = 0.0f;
		rvVertices[0].tcTex.tY    = 0.0f;
		
		rvVertices[1].cvIntensity = 1.0f;
		rvVertices[1].tcTex.tX    = 1.0f;
		rvVertices[1].tcTex.tY    = 0.0f;
		
		rvVertices[2].cvIntensity = 1.0f;
		rvVertices[2].tcTex.tX    = 1.0f;
		rvVertices[2].tcTex.tY    = 1.0f;
		
		rvVertices[3].cvIntensity = 1.0f;
		rvVertices[3].tcTex.tX    = 0.0f;
		rvVertices[3].tcTex.tY    = 1.0f;

		prvVertices[0] = &rvVertices[0];
		prvVertices[1] = &rvVertices[1];
		prvVertices[2] = &rvVertices[2];
		prvVertices[3] = &rvVertices[3];

		rpolyPolygon.paprvPolyVertices.atArray = prvVertices;
		rpolyPolygon.paprvPolyVertices.uLen    = 4;
		rpolyPolygon.seterfFace                = Set(erfTRAPEZOIDS);
		rpolyPolygon.ptexTexture               = ptexTexture;
		rpolyPolygon.cvFace                    = 1.0f;
		rpolyPolygon.iFogBand                  = 0;
		rpolyPolygon.rPlaneTolerance           = 0.001;
		rpolyPolygon.iMipLevel                 = 0;	
		rpolyPolygon.bAccept                   = true;
		rpolyPolygon.eamAddressMode            = eamTileNone;
		rpolyPolygon.bPrerasterized            = false;

#if (VER_DEBUG)
		rpolyPolygon.pshOwningShape            = 0;
#endif

		if (d3dDriver.bUseD3D())
		{
			rpolyPolygon.ehwHardwareFlags = ehwFill;
			rpolyPolygon.bFullHardware    = true;
		}
		else
		{
			rpolyPolygon.ehwHardwareFlags = ehwSoftware;
			rpolyPolygon.bFullHardware    = false;
		}
	}

#if bDEBUG_PARTICLES
	//*****************************************************************************************
	//
	void CParticleBase::Validate
	(
	)
	//
	// Rigorous asserts to verify the particle's data.
	//
	//**************************
	{
		uint u     = 0;
		uint u_len = rpolyPolygon.paprvPolyVertices.uLen;

		AlwaysAssert(u_len == 3 || u_len == 4);
		for (u = 0; u < u_len; ++u)
		{
			SRenderVertex& rv = rvVertices[u];
			AlwaysAssert(rv.v3Screen.tZ > 0.0f);
			AlwaysAssert(rv.v3Cam.tY > 0.0f);
		}
	}
#endif // bDEBUG_PARTICLES


//*********************************************************************************************
//
// CParticleFluid implementation.
//

	//*****************************************************************************************
	bool CParticleFluid::bAdd(const CCamera& cam, const CTransform3<>& tf3_to_norm_cam)
	{
		// Set the rendered flag.
		bRendered = true;

		CVector3<> v3_a = v3Position(sAge - sStickTime) * tf3_to_norm_cam;
		CVector3<> v3_b = v3Position(sAge) * tf3_to_norm_cam;

		// Special case of a particle that has not moved yet.
		if (bOneFrame)
			v3_a = v3_b;

		// Extend along the length of the particle, in screen space.
		CVector3<> v3_dir = v3_b - v3_a;
		v3_dir.tY = 0.0f;
		float f_len = v3_dir.tLen();
		if (f_len <= 0.0f)
			v3_dir = CVector3<>(0.0f, 0.0f, 1.0f);
		else
			v3_dir /= f_len;

		float f_size = fCamSize(cam, v3_b.tY);

		// Vertices along the direction.
		rvVertices[0].v3Cam = v3_b + v3_dir * f_size;
		rvVertices[2].v3Cam = v3_a - v3_dir * f_size;

		// Vertices on the perpendicular.
		Swap(v3_dir.tX, v3_dir.tZ);
		v3_dir.tZ *= -1.0f;
		rvVertices[3].v3Cam = v3_b + v3_dir * f_size;
		rvVertices[1].v3Cam = v3_b - v3_dir * f_size;

		// Project the points because the vertices are not in the regular pipeline.
		rvVertices[0].v3Screen = cam.ProjectPoint(rvVertices[0].v3Cam);
		rvVertices[1].v3Screen = cam.ProjectPoint(rvVertices[1].v3Cam);
		rvVertices[2].v3Screen = cam.ProjectPoint(rvVertices[2].v3Cam);
		rvVertices[3].v3Screen = cam.ProjectPoint(rvVertices[3].v3Cam);

		// For efficiency in depth sorting, set all camera Y's and screen Z's equal.
		rvVertices[3].v3Cam.tY = rvVertices[2].v3Cam.tY = rvVertices[1].v3Cam.tY = rvVertices[0].v3Cam.tY;
		rvVertices[3].v3Screen.tZ = rvVertices[2].v3Screen.tZ = rvVertices[1].v3Screen.tZ = rvVertices[0].v3Screen.tZ;

		//
		// If this particle is touching or off the screen, kill it.
		//
		if (bNotInScreen())
		{
			sAge = sReallyOld;
			return false;
		}

		// Set appropriate flags.
		Initialize();
		rpolyPolygon.bPrerasterized = false;

	#if bDEBUG_PARTICLES
		Validate();
	#endif

		return true;
	}

	//*****************************************************************************************
	//
	void CParticleFluid::Initialize
	(
	)
	//
	// Class sets up the required structures.
	//
	//**************************
	{
		if (rpolyPolygon.bFullHardware)
		{
			if (d3dDriver.bUseAlpha())
			{
				rpolyPolygon.ehwHardwareFlags = ehwFluid;
			}
		}
	}


//*********************************************************************************************
//
// CParticleSolid implementation.
//

	//*****************************************************************************************
	bool CParticleSolid::bAdd(const CCamera& cam, const CTransform3<>& tf3_to_norm_cam)
	{
		// Set the rendered flag.
		bRendered = true;

		// Get the position of the particle in normalized camera space.
		CVector3<> v3_norm_cam = v3Position(sAge) * tf3_to_norm_cam;

		// Get the size of the particle in normalized camera space.
		float f_size = fCamSize(cam, v3_norm_cam.tY);

		// Shrink the particle over time.
		f_size *= (sLifespan - sAge) / sLifespan;

		// Extend outward from the centroid of the particle.
		rvVertices[0].v3Cam = v3_norm_cam + v3TriC * f_size;
		rvVertices[1].v3Cam = v3_norm_cam + v3TriB * f_size;
		rvVertices[2].v3Cam = v3_norm_cam + v3TriA * f_size;

		/*
		// If this particle is touching or off the screen, kill it.
		if (bNotInCamera())
		{
			sAge = sReallyOld;
			return false;
		}
		*/

		// Project the points because the vertices are not in the regular pipeline.
		rvVertices[0].v3Screen = cam.ProjectPoint(rvVertices[0].v3Cam);
		rvVertices[1].v3Screen = cam.ProjectPoint(rvVertices[1].v3Cam);
		rvVertices[2].v3Screen = cam.ProjectPoint(rvVertices[2].v3Cam);

		// If this particle is touching or off the screen, kill it.
		if (bNotInScreen())
		{
			sAge = sReallyOld;
			return false;
		}

		// Set appropriate flags.
		Initialize();

		// Set appropriate flags.
		rpolyPolygon.bPrerasterized = false;

	#if bDEBUG_PARTICLES
		Validate();
	#endif

		return true;
	}

	//*****************************************************************************************
	//
	void CParticleSolid::Initialize
	(
	)
	//
	// Class sets up the required structures.
	//
	//**************************
	{
		rvVertices[3].v3Screen = CVector3<>(1.0f, 1.0f, 0.1f);
		rpolyPolygon.paprvPolyVertices.uLen = 3;
		if (rpolyPolygon.bFullHardware)
		{
			rpolyPolygon.ehwHardwareFlags = ehwFill;
		}
	}


//*********************************************************************************************
//
// CParticleDust implementation.
//

	//*****************************************************************************************
	CVector3<> CParticleDust::v3Position(TSec sec) const
	{
		CVector3<> v3_s = v3StartPosition;

		// Clamp the time.
		if (sec <= 0.0f)
			return v3_s;

		// Wacky computation of distance.
		TSec s_effective = sec * fFinalDist / (fStartVelocity * sec + fFinalDist);
		v3_s += v3StartVelocity * s_effective;

		// Add regular acceleration.
		v3_s.tZ -= 0.5f * fAccelUnstuck * Sqr(sec);

		// Add wind velocity. Scale it according to how "light" the particle is.
		// The closer fAccelUnstuck is to real gravity, the heavier the particle).
		v3_s += v3Wind * (1.0f - fAccelUnstuck * 0.1f) * sec;

		return v3_s;
	}

	//*****************************************************************************************
	void CParticleDust::Setup()
	{
		AlwaysAssert(fAccelStuck != 0.0f);

		// Get the velocity and direction of the particle.
		fStartVelocity = v3StartVelocity.tLen();
		AlwaysAssert(fStartVelocity != 0.0f);
		CVector3<> v3_dir = v3StartVelocity;
		v3_dir /= fStartVelocity;

		fFinalDist = fSqrt(fStartVelocity / fAccelStuck);
	}


//*****************************************************************************************
//
// Struct SParticleParams implementation.
//

	//*************************************************************************************
	//
	// Member functions implementations.
	//

	//*************************************************************************************
	SParticleParams::SParticleParams()
	{
		fSpinRadPerSec = dDegreesToRadians(360.0f);
		fNumParticles  = 8.0f;
		fVelocity      = 4.0f;
		sLifespan      = 1.0f;
		fSize          = 0.01f;
		fLength		   = 0.04f;
		fSpreadRadians = dDegreesToRadians(60.0f);
		fThreshold     = 1.0f;
		fAccelStuck    = 1.0f;
		fAccelUnstuck  = 5.0f;
	}

	//*************************************************************************************
	SParticleParams SParticleParams::ppInterpolate(const SParticleParams& pp, float f_t) const
	{
		SParticleParams pp_ret;

		float f_s = 1.0f - f_t;
		pp_ret.fNumParticles  = f_s * fNumParticles  + f_t * pp.fNumParticles;
		pp_ret.fVelocity      = f_s * fVelocity      + f_t * pp.fVelocity;
		pp_ret.sLifespan      = f_s * sLifespan      + f_t * pp.sLifespan;
		pp_ret.fLength	      = f_s * fLength		 + f_t * pp.fLength;
		pp_ret.fAccelStuck    = f_s * fAccelStuck    + f_t * pp.fAccelStuck;
		pp_ret.fAccelUnstuck  = f_s * fAccelUnstuck  + f_t * pp.fAccelUnstuck;
		pp_ret.fSize          = f_s * fSize          + f_t * pp.fSize;
		pp_ret.fSpinRadPerSec = f_s * fSpinRadPerSec + f_t * pp.fSpinRadPerSec;
		pp_ret.fSpreadRadians = f_s * fSpreadRadians + f_t * pp.fSpreadRadians;
		return pp_ret;
	}

	//*************************************************************************************
	SParticleParams SParticleParams::operator *(float f_t) const
	{
		SParticleParams pp_ret;

		pp_ret.fNumParticles  = f_t * fNumParticles;
		pp_ret.fVelocity      = f_t * fVelocity;
		pp_ret.sLifespan      = f_t * sLifespan;
		pp_ret.fLength        = f_t * fLength;
		pp_ret.fAccelStuck    = f_t * fAccelStuck;
		pp_ret.fAccelUnstuck  = f_t * fAccelUnstuck;
		pp_ret.fSize          = f_t * fSize;
		pp_ret.fSpinRadPerSec = f_t * fSpinRadPerSec;
		pp_ret.fSpreadRadians = f_t * fSpreadRadians;

		return pp_ret;
	}

	//*************************************************************************************
	SParticleParams SParticleParams::ppRandom(const SParticleParams& pp) const
	{
		SParticleParams pp_ret;

		float f_age_size = rndParticles(0.0, 1.0);
		pp_ret.fNumParticles  = rndParticles(0.0, 1.0) * (pp.fNumParticles - fNumParticles) + fNumParticles;
		pp_ret.fVelocity      = rndParticles(0.0, 1.0) * (pp.fVelocity - fVelocity) + fVelocity;
		pp_ret.sLifespan      = f_age_size * (pp.sLifespan - sLifespan) + sLifespan;
		pp_ret.fSize          = f_age_size * (pp.fSize - fSize) + fSize;
		pp_ret.fLength	      = rndParticles(0.0, 1.0) * (pp.fLength - fLength) + fLength;
		pp_ret.fAccelStuck    = rndParticles(0.0, 1.0) * (pp.fAccelStuck - fAccelStuck) + fAccelStuck;
		pp_ret.fAccelUnstuck  = f_age_size * (pp.fAccelUnstuck - fAccelUnstuck) + fAccelUnstuck;
		pp_ret.fSpreadRadians = rndParticles(0.0, 1.0) * (pp.fSpreadRadians - fSpreadRadians) + fSpreadRadians;
		pp_ret.fSpinRadPerSec = rndParticles(0.0, 1.0) * (pp.fSpinRadPerSec - fSpinRadPerSec) + fSpinRadPerSec;
		return pp_ret;
	}


//*****************************************************************************************
//
// Class CCreateParticles implementation.
//

	//*************************************************************************************
	//
	// Class CCreateParticles constructors.
	//

	//*************************************************************************************
	CCreateParticles::CCreateParticles(CColour clr)
		: clrColour(clr), ptexTexture(0), bUseTerrainColour(false)
	{
		eptParticleType		= eptFluid;
		bLogScale			= true;
		iPriority			= 0;
		fTopMin				= 0.25f;

		// Set most min values to 0 by default.
		ppMin				= ppMax * 0.0f;
		ppMin.fAccelStuck   = 1.0f;
		ppMin.fAccelUnstuck = 5.0f;
	}

	//*************************************************************************************
	CCreateParticles::CCreateParticles
	(
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable	// Value Table.
	)
	{
		// First set everything to defaults.
		eptParticleType		= eptFluid;
		bLogScale			= true;
		iPriority			= 0;
		fTopMin				= 0.25f;
		bUseTerrainColour	= false;

		// Set default minimum params to all be some fraction of max params.
		ppMin				= ppMax * 0.0f;
		ppMin.fAccelStuck   = 1.0f;
		ppMin.fAccelUnstuck = 5.0f;

		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_object)
		{
			bool b;
			if (bFILL_BOOL(b, esSolid))
				eptParticleType = b ? eptSolid : eptFluid;
			if (bFILL_BOOL(b, esDust))
				eptParticleType = b ? eptDust : eptParticleType;

			bFILL_BOOL(bLogScale, esLog);
			bFILL_INT(iPriority, esPriority);

			int i_red, i_green, i_blue;
			if (bFILL_INT(i_red, esR) && bFILL_INT(i_green, esG) && bFILL_INT(i_blue, esB))
			{
				clrColour = CColour(i_red, i_green, i_blue);
			}

			// Load the min and max sub-objects.
			for (ESymbol es = esMin; es <= esMax; es = ESymbol((int)es + 1))
			{
				IF_SETUP_OBJECT_SYMBOL(es)
				{
					SParticleParams* ppp = es == esMin ? &ppMin : &ppMax;

					bFILL_FLOAT(ppp->fNumParticles,		esNumber);
					bFILL_FLOAT(ppp->fVelocity,			esVelocity);
					bFILL_FLOAT(ppp->sLifespan,			esLifespan);
					bFILL_FLOAT(ppp->fAccelUnstuck,		esGravity);
					if (!bFILL_FLOAT(ppp->fAccelStuck,	esStuckGravity))
						ppp->fAccelStuck = ppp->fAccelUnstuck;

					bFILL_FLOAT(ppp->fSize,				esSize);
					bFILL_FLOAT(ppp->fLength,			esLength);
					if (bFILL_FLOAT(ppp->fSpreadRadians,esSpread))
						// It was actually given in degrees, so convert.
						ppp->fSpreadRadians *= dDEGREES;

					bFILL_FLOAT(ppp->fThreshold,		esThreshold);
				}
				END_OBJECT;
			}
		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}

	//*************************************************************************************
	//
	// Class CCreateParticles member functions.
	//
	CColour clrScaleSat(CColour clr, float f_scale)
	{
		return CColour
		(
			Min(iRound(clr.u1Red * f_scale), 255),
			Min(iRound(clr.u1Green * f_scale), 255),
			Min(iRound(clr.u1Blue * f_scale), 255)
		);
	}

	//*************************************************************************************
	void CCreateParticles::Create(CVector3<> v3_origin, CDir3<> d3_dir, TSec s_period)
	{
		// Set the particle colour based on terrain colour if desired.
		if (bUseTerrainColour)
		{
			// Get the terrain pointer.
			CTerrain* ptrr = CWDbQueryTerrain().tGet();

			if (ptrr)
			{
				// Get the colour from the terrain base texture at this point.
				CColour clrTerrain = ptrr->clrGetBaseTextureColour(v3_origin.tX, v3_origin.tY);
				clrColour = clrTerrain + CColour(80,80,80);
			}
		}

		// For angle, ignore min spread, just use max.
		priv_self.SetTexture(Particles.ptexcolTextures);
		sBeginAge = Particles.sPreviousStep;

		// Construct a rotation.
		CMatrix3<> mx3 = CRotate3<>(d3ZAxis, d3_dir);

		int i_num_particles = priv_selfc.iCreateNumParticles(s_period);

		for (int i_new_particle = 0; i_new_particle < i_num_particles; ++i_new_particle)
		{
			// Enforce a reasonable maximum.
			if (Particles.pprtParticleList->size() >= iMAX_PARTICLES)
				break;

			// Create a random rotation.
			float f_phi    = rndParticles(0.0, Min(ppMax.fSpreadRadians, dPI));
			float f_theta  = rndParticles(0.0, d2_PI);
			float f_sin_phi = sin(f_phi);

			CDir3<> d3_dir(cos(f_theta) * f_sin_phi, sin(f_theta) * f_sin_phi, cos(f_phi), true);

			// Construct a particle.
			d3_dir *= mx3;

			switch (eptParticleType)
			{
				case eptSolid:
					Particles.pprtParticleList->push_back(CParticleSolid(v3_origin, d3_dir, *this));
				break;

				case eptDust:
					Particles.pprtParticleList->push_back(CParticleDust(v3_origin, d3_dir, *this));
				break;

				case eptFluid:
					Particles.pprtParticleList->push_back(CParticleFluid(v3_origin, d3_dir, *this));
				break;

				default:
					Assert(0);
			}	
		}
	}

	//*********************************************************************************************
	void CCreateParticles::Create(CVector3<> v3_origin, CDir3<> d3_dir, TSec s_period, float f_strength)
	{
		float f_interp = fIntensity(f_strength);
		if (f_interp < 0.0f)
			return;
		//SetMin(f_interp, 1.0f);

		// Create a new maximum param set by interpolating between the min and max values,
		// using fTopMin as a base. Retain the minimum param set.
		CCreateParticles crt = *this;
		f_interp = f_interp * (1.0f - fTopMin) + fTopMin;
		crt.ppMax = ppMin.ppInterpolate(ppMax, f_interp);

		crt.Create(v3_origin, d3_dir, s_period);
	}

	//*************************************************************************************
	float CCreateParticles::fIntensity(float f_strength) const
	{
		return (f_strength       - ppMin.fThreshold) /
			   (ppMax.fThreshold - ppMin.fThreshold);
	}

	//*************************************************************************************
	CCreateParticles CCreateParticles::crtInterpolate(const CCreateParticles& crt, float f_t) const
	{
		CCreateParticles crt_ret = *this;

		crt_ret.ppMin = ppMin.ppInterpolate(crt.ppMin, f_t);
		crt_ret.ppMax = ppMax.ppInterpolate(crt.ppMax, f_t);

		return crt_ret;
	}

	//*************************************************************************************
	CCreateParticles CCreateParticles::operator *(float f_t) const
	{
		CCreateParticles crt_ret = *this;

		crt_ret.ppMin = ppMin * f_t;
		crt_ret.ppMax = ppMax * f_t;

		return crt_ret;
	}


//*********************************************************************************************
//
// Class CParticles implementation.
//

	//*****************************************************************************************
	//
	// CParticles constructor and destructor.
	//

	//*****************************************************************************************
	CParticles::CParticles()
		: sPreviousStep(0.0f)
	{
		pprtParticleList = new CParticleList();
		ptexcolTextures = new CTextureColours();
	}

	//*****************************************************************************************
	CParticles::~CParticles()
	{
		Reset();
		ptexcolTextures->Reset();
		delete pprtParticleList;
		delete ptexcolTextures;
	}

	//*****************************************************************************************
	//
	// CParticles member function.
	//
	
	//*****************************************************************************************
	void CParticles::Reset()
	{
		ptexcolTextures->Reset();
		pprtParticleList->erase(pprtParticleList->begin(), pprtParticleList->end());
	}

	//*****************************************************************************************
	void CParticles::Update(TSec sec)
	{
		CCycleTimer	ctmr;

		CParticleList::iterator it;

		// Filter large delta times.
		SetMinMax(sec, 0.025f, 0.25f);

		// Update particles.
		for (it = pprtParticleList->begin(); it != pprtParticleList->end(); ++it)
		{
			(*it).Update(sec);
		}

		// Remove old particles.
		for (it = pprtParticleList->begin(); it != pprtParticleList->end();)
		{
			CParticleList::iterator it_remove = it;
			++it;
			if ((*it_remove).bRetired())
				pprtParticleList->erase(it_remove);
		}

		// Store the step time.
		sPreviousStep = sec;

		psParticlesUpdate.Add(ctmr(), pprtParticleList->size());
	}

	//*****************************************************************************************
	void CParticles::Add(const CCamera& cam, CPipelineHeap& rplhHeap)
	{
		CCycleTimer ctmr;

		Assert(prasMainScreen.ptGet());

		// Set the width and the height of the particle screen.
		CParticleBase::fScreenWidth  = float(prasMainScreen->iWidth  - 1);
		CParticleBase::fScreenHeight = float(prasMainScreen->iHeight - 1);

		// Get the world to normalized camera space transform.
		CTransform3<> tf3_to_norm_cam(cam.tf3ToNormalisedCamera());

		// Iterate through the particle list and render them.
		CParticleList::iterator it = pprtParticleList->begin();
		for (; it != pprtParticleList->end(); ++it)
		{	
			if ((*it).bAdd(cam, tf3_to_norm_cam))
			{
				CRenderPolygon& rpoly = *rplhHeap.darpolyPolygons.paAlloc(1);
				rpoly = (*it).rpolyPolygon;
				rpoly.paprvPolyVertices = (*it).rpolyPolygon.paprvPolyVertices;
			}
		}

		psParticlesRender.Add(ctmr(), pprtParticleList->size());
	}


//
// Global variables.
//
CParticles Particles;

float CParticleBase::fScreenWidth  = 0.0f;
float CParticleBase::fScreenHeight = 0.0f;