/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Functions for box model.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/Xob_bc.hpp                                                $
 * 
 * 41    98/10/02 17:07 Speter
 * Added iTerrainSound and fTerrainFriction.
 * 
 * 40    98/09/30 19:00 Speter
 * SuperOrient-skip optimisation.
 * 
 * 39    98/09/28 2:47 Speter
 * Added CheckPelvis().
 * 
 * 38    98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 37    9/16/98 12:35a Agrant
 * added new energy calculations
 * 
 * 36    9/15/98 10:57p Jpaque
 * Moved var to solve init order problem.
 * 
 * 35    9/04/98 4:36p Agrant
 * added floats property
 * 
 * 34    98/09/02 21:22 Speter
 * New physics sleep management.
 * 
 * 33    98/08/27 0:59 Speter
 * Commented Moved[].
 * 
 * 32    8/26/98 11:38p Asouth
 * added full forward decl of ostream
 * 
 * 31    98/08/23 15:19 Speter
 * Separate movable/immovable box limits.
 * 
 * 30    98/08/22 21:47 Speter
 * Added translational magnet params.
 * 
 * 29    98/07/20 13:15 Speter
 * XobReset(), DumpXobState(). Removed CreateSimpleBoxModel.
 * 
 * 28    98/07/08 1:04 Speter
 * Now pass pelvis linkage params to CreateBoxModel; removed SetBoxID; removed bio_bc param.
 * Added hinge_xzero.
 * 
 * 27    98/06/30 21:40 Speter
 * Changed hinge params to vectors, added hinge_drive.
 * 
 * 26    98/06/28 17:27 Speter
 * Added arguments for magnet parameters. Moved object sleep code to same point as bedtime test.
 * Coalesced magnet break loops, and removed Break[] array.
 * 
 * 25    98/06/25 17:16 Speter
 * Removed externs to physics boundary values.
 * 
 * 24    6/22/98 3:07p Sblack
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_XOBBC_HPP
#define HEADER_LIB_PHYSICS_XOBBC_HPP


#include "Lib/Physics/pelvis_def.h"

class CInstance;
class CEntityWater;

#ifdef __MWERKS__
 // MSL uses templated IO streams; this is the full forward decl
 namespace std {
  template<class charT, class traits> class basic_ostream;
  template<class charT> struct char_traits;
  typedef basic_ostream<char,char_traits<char> > ostream; 
 };
#else
#include <iostream>
#endif

typedef float	TVectorPair[2][3];

// This is the only place the following constants are defined
#define iMAX_PHYSICS_OBJECTS	50
#define iMAX_PHYSICS_MOVABLE	20
#define iMAX_BOX_ELEMENTS		10

// Aliases used in physics files.
#define GUYS					iMAX_PHYSICS_OBJECTS
#define MAX_ELEMENTS			iMAX_BOX_ELEMENTS
#define	COMPOUND_OVERSPEC		8*MAX_ELEMENTS 

//**********************************************************************************************
class CXob
{
public:

	// Constant data.
	float	State[7][3];
	float	Data[20];						//Stuff pertaining to the state...
	float	Extents[6];						//Min and max extents of superbox, in local space.
	float	Radius;							//Max radius of superbox.
	float	ExtentRatio;					//Max ratio of dimensions.
	bool	Movable;						//Whether each one is movable...
	bool	Anchored;						//Whether the box is anchored at its CM.
											//Bit 1 = moved this frame. Bit 2 = moved ever.
	bool	Floats;							//Whether it floats...
	bool	Ignorable_DOF[6];				//1 for live axes, 0 for dead...

	// Sub-box constant data.
	bool	Rotated[iMAX_BOX_ELEMENTS];			//Whether each element has an orientation.
	float	SuperData[iMAX_BOX_ELEMENTS][6];	//Super Boxes...
	float	SuperOrient[MAX_ELEMENTS][3][3];	//SuperBox orientation relative to element 0...
	float	TTotal[MAX_ELEMENTS][3];			//Stress measurements...
	float	Breakages[MAX_ELEMENTS];			//Breakage constant for each element (FLT_MAX if unbreakable).
	int		Sound[MAX_ELEMENTS];				//Sound material of each element.

	CInstance*	Instances[MAX_ELEMENTS+1];	// The instances corresponding to each box element.
											// Not used directly by Xob_bc (CInstance is opaque),
											// but required for tracking.
	// Variable data.
	int		Info;							// Current state of activation: 0 to INFO_MAX.
	int		Moved;							//Whether each one has been moved...
	float	Bedtime;						//Store current activity of each object.
	float	SleepThreshold;					//Per-box threshold for sleeping.
	bool	bUpdating;						//Semaphore for world update.
	bool	bHitAnother;
	int		iActivatedFrame;				//Frame this object was activated.
	int		iLastTouched;					//Frame this object was last touched by an animal.
	float	fVelocityExpand;				//Cached current expansion for velocity.
	float	fInitialEnergy;					//The kinetic energy of each guy at the start of this render frame.
	float	Wz;								//World compressive Z...

private:

	float	Transform[9];					//Cached matrix transform for current state.

	//		For Compound Boxes...
	//		---------------------
	int		comp_index;
	float	position[COMPOUND_OVERSPEC][3];
	float	SubInp[COMPOUND_OVERSPEC][2][3];

	//		Inter-box interaction...
	int8	BC_semaphore[GUYS];

public:

	bool	Do_It[GUYS];					//Whether to interact each pair of boxes...
	bool	Intersected[GUYS];				//Whether the boxes intersect
											//(may not be interacting...used for turning off ignoring).

	//		For external BC interactions...
	//		-------------------------------
	//		Constant data...
	int		PelvisModel;					//Each box's associated pelvis model, or -1 if none.
	int		PelvisElem;						//The element of the pelvis model, or -1 if none.

	//		Variable data...
	float	Tau[3];							//For a variety of reasons...
	float	Xin[2][3];						//0 are forces, 1 are moments...
	float	Impulse_Queue[6];				//What it says.  Applied at start of frame...

public:

	//**********************************************************************************************
	void Create
	(
		int elements,						//The number of subelements...
		float magnets[MAX_ELEMENTS][3],		//Magnet locations in frame 0...
		float strength[MAX_ELEMENTS],		//Magnet breaking strength...
		float extents[MAX_ELEMENTS][6],		//Element ofsets and sizes...
		float orient[MAX_ELEMENTS][3][3],	//Element orientations...
		float mass[MAX_ELEMENTS],			//Element masses...
		float slip[MAX_ELEMENTS],			//and slip...
		int material[MAX_ELEMENTS],			//and sounds...
		bool movable,						//Whether it can move...
		bool b_floats,						//Whether it can float
		bool live_axes[3],					//1 for live, 0 for dead...
		float hinge_strength,				//Restoring force, 0..10...
		float hinge_damp,					//Hinge friction, 0..10...
		float hinge_drive,					//Hinge driving force, ???
		bool hinge_limits,					//Whether hinge has limits; if so see below...
		float hinge_xzero[3],				//Local x direction vector for zero-point.
		float hinge_xmin[3],				//World limits of local x direction vector for rotation.
		float hinge_xmax[3],	
		float init[7][3],					//Initial state of element 0...
		int pelvis,							//This box intimately, sensuously intertwined with bio model.
		int pelvis_elem
	);

	//**********************************************************************************************
	inline void Back_Element( float &A, float &B, float &C, int element ) const
	{
		if (Rotated[element])
		{
	float	X = A,
			Y = B,
			Z = C;

			A = X*SuperOrient[element][0][0] + Y*SuperOrient[element][0][1] + Z*SuperOrient[element][0][2];
			B = X*SuperOrient[element][1][0] + Y*SuperOrient[element][1][1] + Z*SuperOrient[element][1][2];
			C = X*SuperOrient[element][2][0] + Y*SuperOrient[element][2][1] + Z*SuperOrient[element][2][2];
		}
	}

	//**********************************************************************************************
	inline void From_Element( float &A, float &B, float &C, int element ) const
	{
		if (Rotated[element])
		{
	float	X = A,
			Y = B,
			Z = C;

			A = X*SuperOrient[element][0][0] + Y*SuperOrient[element][1][0] + Z*SuperOrient[element][2][0];
			B = X*SuperOrient[element][0][1] + Y*SuperOrient[element][1][1] + Z*SuperOrient[element][2][1];
			C = X*SuperOrient[element][0][2] + Y*SuperOrient[element][1][2] + Z*SuperOrient[element][2][2];
		}
	}

	//**********************************************************************************************
	static void PD_Integrate
	( 
		float timestep 
	);
	//
	// Run the physics for the given timestep.
	//
	//**************************************

	//**********************************************************************************************
	static void Reset();
	//
	// Resets the physics system state.
	//
	//**************************************

	//**********************************************************************************************
	static void XOBResize( float vec[3] );
	//	Takes a vector and resizes it for physics box use...

	//**********************************************************************************************
	static void DumpStateAll
	( 
		std::ostream& os
	);
	//
	// Dumps the state to a text file.
	//
	//**************************************

	//**********************************************************************************************
	void ApplyImpulse
	(
		float W[3],		// World coordinate of impulse.
		float V[3]		// World impulse vector (kg m/s).
	);

	//
	// These functions require world database access, are are implemented in InfoBox.cpp.
	//

	//******************************************************************************************
	float fTerrainMaxHeight();

	//*****************************************************************************************
	float fTerrainHeight
	(
		float f_x, float f_y			// Position at which to query height.
	);

	//*****************************************************************************************
	float fTerrainHeight
	(
		float f_x, float f_y,			// Position at which to query height.
		float af_normal[3]				// Returned terrain normal at this point.
	);

	//*****************************************************************************************
	float fTerrainHeight
	(
		float f_x, float f_y,			// Position at which to query height.
		float f_zmin,					// Minimum value we're interested in;
										// if below this, just return anything below f_zmin.
		float af_normal[3]				// Returned terrain normal at this point.
	);

	//*****************************************************************************************
	int iTerrainEdges
	(
		TVectorPair* apvp_edges[],		// Array of edge pointers to fill.
		int i_max						// Max size of array.
	);
	//
	// Returns:
	//		The number of edges in the array (up to max).
	//
	//**********************************

	//*****************************************************************************************
	float fWaterHeight
	(
		float f_x, float f_y,			// World location to check.
		CEntityWater** ppetw			// Returned water entity, for subsequent interaction.
	);

	//*****************************************************************************************
	inline float fWaterHeight
	(
		float f_x, float f_y
	)
	{
		CEntityWater* petw;
		return fWaterHeight(f_x, f_y, &petw);
	}

	//*****************************************************************************************
	int iTerrainSound
	(
		float f_x, float f_y			// World location to check.
	);
	//
	// Returns:
	//		The sound material at this location on the terrain.
	//
	//**********************************

	//*****************************************************************************************
	float fTerrainFriction
	(
		float f_x, float f_y			// World location to check.
	);
	//
	// Returns:
	//		The friction at this location on the terrain.
	//
	//**********************************

	//*****************************************************************************************
	float fGetKineticEnergy();

	//*****************************************************************************************
	const char* strInstanceName(int i_elem);

	//*****************************************************************************************
	void CheckPelvis();

	//*****************************************************************************************
	// For debugging and other reasons, it's nice to identify a box by its index in the array.
	int Index() const;

private:

	void Stuff_Terr_Moments(
						 float mine[3],						//My point of contact,
						 float BC_n[3],						//The normal of the BC vector,
						 float BC_val,						//The macgitude of the contact,
						 float ad,  float bd,  float gd		//My angle-converted first moments...
						 );

	void scrub_state( float ad, float bd, float gd, float a0, float a1, float a2, float a3 );

	void KillRVel( float V[3], float P[3] );

	void plane_interp( float Px, float Py, float Pz, float Vx, float Vy, float Vz, CXob& you);

	void Stuff_Contacts( CXob& you,
						float Xdotdot, float Ydotdot, float Zdotdot,
						float V_x, float V_y, float V_z,
						float A, float B, float C,
						int superme, int superyou );

	void get_BC_moments( CXob& you,
						 float your[3],						//Your point of contact,
						 float mine[3],						//My point of contact,
						 float BC_n[3],						//The normal of the BC vector,
						 float BC_val,						//The macgitude of the contact,
						 float day, float dby, float dgy,	//Your angle-converted first moments,
						 float ad,  float bd,  float gd,	//My angle-converted first moments...
						 int superme, int superyou );

	void pipe_quad_BC();

	void Make_Transform();

	void Assert_Transform() const;

	inline void From( float &X, float &Y, float &Z ) const
	{
#if VER_DEBUG
		Assert_Transform();
#endif


	float	x = X,	
			y = Y,
			z = Z;

			X = x*Transform[0]
			  + y*Transform[1]
			  + z*Transform[2];
			
			Y = x*Transform[3]
			  + y*Transform[4]
			  + z*Transform[5];

			Z = x*Transform[6]
			  + y*Transform[7]
			  + z*Transform[8];
	}


	inline void Back( float &X, float &Y, float &Z ) const
	{
#if VER_DEBUG
		Assert_Transform();
#endif

	float	x = X,	
			y = Y,
			z = Z;

			X = x*Transform[0]
			  + y*Transform[3]
			  + z*Transform[6];
			
			Y = x*Transform[1]
			  + y*Transform[4]
			  + z*Transform[7];

			Z = x*Transform[2]
			  + y*Transform[5]
			  + z*Transform[8];
	}

	static void PD_Integrator_Internals( float delta_t );

	void DumpState(std::ostream& os);

	//
	// Integration stages.
	//

	inline void IntegrateInit();

	inline void ToBio();

	inline void IntegrateBox(float delta_t);

	inline void FromBio();

	inline void IntegrateFinish();
};

extern CXob		Xob[GUYS];							// The array of xobs.
extern float	BioTag[NUM_PELVISES][PELVIS_DOF];	//Applied inputs to BioTags...

extern const int INFO_MAX;						//Number of steps in which we decrease object's activity.
												//Compute physics while > 0.
//		For optimizing the transforms...
//		--------------------------------
extern float	MAX_TIMESTEP;
extern float	PHY_sleep;
extern const float fSLEEP_DEFAULT;

extern float	fBreakage;						//Min breakage constant 
												//(overrides Breakages and allows easy debug adjustment).
extern bool		bPhysicsIntegrating;			// Lock used for debugging.

//**********************************************************************************************
inline int CXob::Index() const
{
	return this - Xob;
}

#endif