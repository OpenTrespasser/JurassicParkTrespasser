/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Test code for the terrain.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/TerrainTest.hpp                                                $
 * 
 * 21    98/04/05 20:39 Speter
 * Added include, due to changed GUITools.hpp.
 * 
 * 20    98/02/10 13:08 Speter
 * Changed original CSArray to CMSArray.
 * 
 * 19    1/16/98 3:57p Mlange
 * Now uses CQueryRect instead of terrain query class.
 * 
 * 18    97/12/19 20:27 Speter
 * Removed TINMesh references.
 * 
 * 17    12/18/97 1:37p Mlange
 * Changed the wavelet transform algorithm to properly use the estDISC_DIAGONAL_1_3 state flag.
 * 
 * 16    12/16/97 5:31p Mlange
 * Updated for changes to quad tree classes.
 * 
 * 15    12/03/97 8:06p Mlange
 * Updated for new wavelet quad tree fastheap allocation function.
 * 
 * 14    10/15/97 6:51p Mlange
 * Re-enabled testing of quad tree queries. Updated for dynamic allocation of the CFastHeap for
 * quad tree types. Now rebuilds and tests the wavelet transform every 50 steps.
 * 
 * 13    10/14/97 2:29p Mlange
 * Updated for the new multiresolution wavelet transform classes.
 * 
 * 12    9/19/97 11:01a Mlange
 * Updated for new integer wavelet transform.
 * 
 * 11    8/05/97 11:35a Mlange
 * Temporary version! The wavelet coeficient is now represented by a class instead of a simple
 * typedef.
 * 
 * 10    7/09/97 3:35p Mlange
 * Query test code operational again. Now also tests queries that go off the edge of the world.
 * 
 * 9     6/30/97 3:16p Mlange
 * Renamed CQuadNode to CQuadNodeTIN.
 * 
 * 8     6/27/97 3:53p Mlange
 * Updated for new wavelet classes' interface. Temporarily commented-out the query test code.
 * Now checks all wavelet vertices instead of just the ones that are part of a TIN mesh.
 * 
 * 7     6/03/97 6:51p Mlange
 * No longer uses the coeficient significance classes.
 * 
 * 6     6/03/97 12:01p Mlange
 * Implemented the terrain query test code.
 * 
 * 5     5/22/97 10:18a Mlange
 * Updated for changes to the multiresolution classes interface.
 * 
 * 4     5/08/97 5:12p Mlange
 * Split the wavelet classes over several source files. Renamed CWavelet2D to CMultiRes2D.
 * 
 * 3     5/08/97 11:37a Mlange
 * Updated for changes to wavelet transform interface.
 * 
 * 2     4/23/97 3:37p Mlange
 * Temporary version!
 * 
 * 1     4/09/97 7:38p Mlange
 * Initial (empty) revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_GUIAPP_TERRAINTEST_HPP
#define HEADER_GUIAPP_TERRAINTEST_HPP

#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/GeomDBase/WaveletCoef.hpp"
#include "Lib/View/Raster.hpp"

namespace NMultiResolution
{
	class CQuadNodeTIN;
	class CQuadRootTIN;
	class CQuadNodeQuery;
	class CQuadRootQuery;
	class CQueryRect;
	class CTransformedDataHeader;
};

using NMultiResolution::CQuadNodeTIN;
using NMultiResolution::CQuadRootTIN;
using NMultiResolution::CQuadNodeQuery;
using NMultiResolution::CQuadRootQuery;
using NMultiResolution::CQueryRect;
using NMultiResolution::CTransformedDataHeader;
using NMultiResolution::CCoef;

//**********************************************************************************************
//
// Declarations for CQueryTest.
//
#define iMAX_QUERY_TESTS	15


//**********************************************************************************************
//
class CQueryTest
//
// Query test class.
//
// Prefix: qt
//
//**************************************
{
	int iLifeStepsRemaining;					// Negative if dead, positive if living, death or rebirth at zero.

	CDir3<> d3MoveDir;							// If living, the direction the query moves in each step.

	CBoundVolSphere bvsVolume;					// The volume to query.

	CPresence3<> pr3Pos;						// Position of the query volume in world space (no rotation is applied).

public:
	CQueryRect* pqrQuery;		// The current active query, or null if dead.


	//*****************************************************************************************
	//
	// Constructors and destructor.
	//
	CQueryTest()
		: pqrQuery(0), iLifeStepsRemaining(0)
	{
	}

	~CQueryTest();


	//*****************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Update
	(
		CQuadRootQuery* pqnq_root
	);
	//
	// Updates the query for each step.
	//
	//**********************************
};



//**********************************************************************************************
//
// Declarations for CTerrainTest.
//
#define uMAX_TEST_TERRAIN_STORAGE_KB	5000u	// Max memory available for quad tree data type allocations.

class CFastHeap;

//**********************************************************************************************
//
class CTerrainTest : public CSubsystem
//
// Subsystem for testing the terrain.
//
// Prefix: trrt
//
//**************************************
{
	CTransformedDataHeader* ptdhData;

	CRasterT<CCoef>* prasQuadTreeVertices;
	CRasterT<CCoef>* prasTransformVertices;
	CRasterT<CCoef>* prasOrigVertices;
	CRasterT<CCoef>* prasOrigWavelets;

	CQuadRootTIN*   pqntinRoot;
	CQuadRootQuery* pqnqRoot;
	
	int iNumTestSteps;						// Number of test steps performed.

	int iNumCoefCheckedTIN;					// Number of coefficients tested this iteration for the TIN.
	int iNumCoefCheckedQuery;				// Number of coefficients tested this iteration for the query.

	CMSArray<CQueryTest, iMAX_QUERY_TESTS> saqtQueries;

public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//
	CTerrainTest();

	~CTerrainTest();


	//*****************************************************************************************
	//
	// Member functions.
	//

private:
	//******************************************************************************************
	//
	void TestInitTransform();
	//
	// Tests and initialises the wavelet analysing transform.
	//
	//**********************************


	//******************************************************************************************
	//
	void DeleteAll();
	//
	// Delete all dynamic allocated types.
	//
	//**********************************


	//******************************************************************************************
	//
	void RecurseTree
	(
		const CQuadNodeTIN* pqntin
	);
	//
	// Rercurses the quad tree and initialises 'prasQuadTreeVertices' and 'prasQuadTreeWavelets'.
	//
	//**********************************


	//*****************************************************************************************
	//
	// Overrides.
	//

	void Process(const CMessageStep&) override;
	void Process(const CMessagePaint& msgpaint) override;
};



namespace NMultiResolution
{

	//**********************************************************************************************
	//
	class CTransform
	//
	//
	//
	// Prefix: tf
	//
	//**************************************
	{
		CRasterT<CCoef>* prasTransformed;

		int  iLevel;
		uint uSize;

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		CTransform(CRasterT<CCoef>* pras_analyse);

		CTransform(CRasterT<CCoef>* pras_synthesise, CCoef cf_root);


		//******************************************************************************************
		//
		// Member functions.
		//

	private:
		//******************************************************************************************
		//
		void InterpolateMissingData();
		//
		//
		//
		//**************************************

	public:
		//******************************************************************************************
		//
		void Analyse();
		//
		//
		//
		//**************************************


		//******************************************************************************************
		//
		void Synthesise();
		//
		//
		//
		//**************************************


		//******************************************************************************************
		//
		int iGetLevel() const
		//
		//
		//
		//**************************************
		{
			return iLevel;
		}


		//******************************************************************************************
		//
		CCoef cfGetRoot() const
		//
		//
		//
		//**************************************
		{
			Assert(iDimScaling() == 1);
			return prasTransformed->tPix(0, 0);
		}


		//******************************************************************************************
		//
		CCoef cfRead
		(
			int i_x, int i_y
		) const
		//
		//
		//
		//**************************************
		{
			return prasTransformed->tPix(i_x, i_y);
		}


		//******************************************************************************************
		//
		CCoef cfReadReflected
		(
			int i_x,     int i_y,
			int i_x_off, int i_y_off
		) const;
		//
		//
		//
		//**************************************


	private:
		//******************************************************************************************
		//
		int iDimScaling() const
		//
		//
		//
		//**************************************
		{
			return uSize >> -iLevel;
		}


		//******************************************************************************************
		//
		int iScalingStride() const
		//
		//
		//
		//**************************************
		{
			return 1 << -iLevel;
		}


		//******************************************************************************************
		//
		CCoef cfPredict
		(
			int i_x, int i_y
		) const;
		//
		//
		//
		//**************************************


		//******************************************************************************************
		//
		CCoef cfLift4
		(
			int i_x, int i_y
		) const;
		//
		//
		//
		//**************************************


		//******************************************************************************************
		//
		CCoef cfLift8
		(
			int i_x, int i_y
		) const;
		//
		//
		//
		//**************************************


		//
		// Prevent copying of the transform.
		//
		CTransform(const CTransform&);

		CTransform& operator =(const CTransform&);
	};
};


#endif
