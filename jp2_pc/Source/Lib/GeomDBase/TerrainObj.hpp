/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/TerrainObj.hpp                                          $
 * 
 * 20    98.09.12 5:39p Mmouni
 * Removed override of pshGetShape that was used to hide/show terrain crosshairs.
 * 
 * 19    98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 18    7/20/98 10:14p Rwyatt
 * Removed description text in final mode
 * 
 * 17    98/06/09 21:30 Speter
 * Added fFriction member. Commented out unused fTexelsPerMetre code.
 * 
 * 16    3/10/98 3:17p Agrant
 * Added pinsCopy, allowing better, faster instancing.
 * 
 * 15    98/01/16 12:32 Speter
 * Added resolution field.
 * 
 * 14    1/15/98 7:04p Agrant
 * Added SoundMaterial and a desription function
 * 
 * 13    12/11/97 4:37p Agrant
 * Changes to allow terrain texture editing in GUIApp
 * 
 * 12    97/11/11 15:28 Speter
 * Fixed terrain clut bug by making sure CTerrainObjs share a clut (and material), not just a
 * palette.
 * 
 * 11    11/01/97 6:25p Agrant
 * Terrain texture objects need their own SetPos function so they do not use the world database!
 * 
 * 10    10/28/97 7:10p Agrant
 * Added the iHeight field to CTerrainObj to allow sorting by it rather than name
 * 
 * 9     10/16/97 1:58p Agrant
 * Value table constructor now takes a CInfo pointer to allow easy instancing of all CInfo data.
 * 
 * 8     9/09/97 8:58p Agrant
 * Made shell for value table constructor
 * 
 * 7     97/08/04 15:08 Speter
 * Changed iAverageIndex to clrAverage.
 * 
 * 6     97/07/07 13:49 Speter
 * Replaced bContainsMesh() with pshGetShape() and bContainsShape().
 * 
 * 5     97/06/10 21:54 Speter
 * Now calculates iAverageIndex for terrain texture palette.
 * 
 * 4     97-05-06 15:58 Speter
 * Added CPal* static member, for common terrain source palettes.
 * 
 * 3     5/01/97 3:47p Rwyatt
 * Renamed calls to ptCastInstance to ptCast. This is because it no longer casts from an
 * instance but from a partition. Renaming the function to ptCastPartition would have caused
 * more work later if another class is added under cPartition. All casts have base
 * implementations in CPartition rather than CInstance
 * 
 * 2     4/30/97 6:02p Rwyatt
 * Added an override for bContainsMesh which always returns FALSE.
 * 
 * 1     97-04-30 12:17 Speter
 * 
 * 1     97-04-29 14:56 Speter
 * New file, offers a mere hint of future functionality.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_TERRAINOBJ_HPP
#define HEADER_LIB_GEOMDBASE_TERRAINOBJ_HPP

#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/View/Colour.hpp"
#include "Lib/Audio/SoundDefs.hpp"

class CClut;

//**********************************************************************************************
//
class CTerrainObj: public CInstance
//
// Prefix: trobj
//
// A special instance that contains a simplified mesh for rendering onto the terrain texture.
// It is not rendered as part of the main scene.
//
//**************************************
{
public:
	static ptr<CClut>	pclutTerrain;		// The single palette used for terrain textures.
	static CColour		clrAverage;			// The average terrain colour.
//	static float		fMaxTexelsPerMetre;	// The maximum world texture resolution of all terrain objs.

	int					iHeight;			// The height for determining render order (higher numbers placed on top)
//	float				fTexelsPerMetre;	// Maximum world texture resolution of this object.
	TSoundMaterial		matSoundMaterial;	// The sound material of the terrain texture.
	float				fFriction;			// Friction value for this segment of terrain.

	//******************************************************************************************
	//
	// Constructor.
	//
private:
	CTerrainObj(){};

public:
	CTerrainObj(const SInit& init);

	CTerrainObj
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	void Init();	// Called by all constructors.

	//*****************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	// override the default implementation in cPartition which assigns NULL.
	virtual void Cast(CTerrainObj** ptrrobj) override
	{
		*ptrrobj=this;
	}

	bool operator< (const CTerrainObj& to)
	{
		return iHeight < to.iHeight;
	}

	//*****************************************************************************************
	virtual void SetPos(const CVector3<>& v3_pos) override
	{
		pr3Pres.v3Pos = v3_pos;
	}

	//*****************************************************************************************
	virtual void Move(const CPlacement3<>& p3_new, CEntity* pet_sender = 0) override;

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif

	//*****************************************************************************************
	virtual CInstance* pinsCopy() const override;
	
	//******************************************************************************************
	//
	// Member functions.
	//



};

#endif
