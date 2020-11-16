/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of TerrainObj.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/TerrainObj.cpp                                          $
 * 
 * 31    98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 30    8/25/98 8:39p Rvande
 * Ambiguous access resolved
 * 
 * 29    98.08.24 6:14p Mmouni
 * Added error for bump-mapped terrain texture objects.
 * 
 * 28    8/22/98 11:55p Mmouni
 * Added warning for large and/or complex terrain objects.
 * 
 * 27    7/20/98 10:14p Rwyatt
 * Removed description text in final mode
 * 
 * 26    7/10/98 10:11p Agross
 * Informative assert for bad terrain texture clut
 * 
 * 25    98/06/29 16:19 Speter
 *  Rendering functions now take the CInstance* parameter.
 * 
 * 24    98/06/09 21:30 Speter
 * Added fFriction member. Commented out unused fTexelsPerMetre code.
 * 
 * 23    5/05/98 12:54p Agrant
 * fixed terrainobj iGetDescription
 * 
 * 22    98/04/09 13:11 Speter
 * Adjusted for CMessageMove::etType.
 * 
 * 21    3/22/98 5:03p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 **********************************************************************************************/



#include "common.hpp"
#include "TerrainObj.hpp"

#include "Lib/Audio/Material.hpp"

#include "Shape.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"

#include "Lib/Sys/DebugConsole.hpp"


//**********************************************************************************************
//
// CTerrainObj implementation.
//
	
	ptr<CClut> CTerrainObj::pclutTerrain = 0;
	CColour CTerrainObj::clrAverage;
//	float CTerrainObj::fMaxTexelsPerMetre;

	//******************************************************************************************
	CTerrainObj::CTerrainObj(const SInit& init)
		: CInstance(init)
	{
		//
		// Mostly, we do nothing special for this object.  
		// But, since all terrain textures must share a single palette, we find the palette
		// here, store it in the terrain texture module, and make sure all have the same palette.
		//

		// Snag the height.
		//  "CTrnObjX"
		iHeight = strGetInstanceName()[7] - 'A';

		Init();
	}

	//******************************************************************************************
	CTerrainObj::CTerrainObj
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CInstance(pgon, pload, h_object, pvtable, pinfo)
	{
		iHeight = 0;
		matSoundMaterial = matHashIdentifier( "TERRAIN" );
		fFriction = fDEFAULT_FRICTION;

		// Should interpret text props.  For now, will simply use defaults.
		SETUP_TEXT_PROCESSING(pvtable, pload)
		IF_SETUP_OBJECT_HANDLE(h_object)
		{
			bFILL_INT(iHeight, esHeight);

			const CEasyString* pestr = 0;
			bFILL_pEASYSTRING(pestr, esSoundMaterial);
			if (pestr)
			{
				matSoundMaterial = matHashIdentifier(pestr->strData());
			}

			bFILL_FLOAT(fFriction,	esFriction);
		}
		END_OBJECT
		END_TEXT_PROCESSING

		// Get mesh information.
		rptr_const<CRenderType> prdt = prdtGetRenderInfo();

		if (prdt)
		{
			rptr_const<CShape> psh = rptr_const_dynamic_cast(CShape, prdt);

			if (psh)
			{
				// Warn if the geometry is tool complex.
				if (psh->iNumVertices() > 128 || psh->iNumPolygons() > 128)
				{
					char str_buffer[256];
					sprintf(str_buffer, "WARNING: TerrainObj (%s) is too complex! %d vertices, %d faces.\n", 
										strGetInstanceName(), psh->iNumVertices(), psh->iNumPolygons());
					dprintf(str_buffer);
				}
			}

			// Warn if the object covers too much area.
			if (prdt->bvGet().rGetVolume(fGetScale()) > 512.0f*512.0f)
			{
				char str_buffer[256];
				sprintf(str_buffer, "WARNING: TerrainObj (%s) is too large! %f square meters.\n", 
									strGetInstanceName(), prdt->bvGet().rGetVolume(fGetScale()));
				dprintf(str_buffer);
			}
		}
		Init();
 	}

	CInstance* CTerrainObj::pinsCopy() const
	{
		AlwaysAssert(!pinfoProperties->setUnique[CInfo::eitTHIS]);

		CTerrainObj* pto = new CTerrainObj;

		*pto = *this;

		return pto;
	}


	//******************************************************************************************
	void CTerrainObj::Init()
	{
		//
		// Mostly, we do nothing special for this object.  
		// But, since all terrain textures must share a single palette, we find the palette
		// here, store it in the terrain texture module, and make sure all have the same palette.
		//
		
		// No real reason for iHeight to be restricted like this, but anything outside this range is 
		//	probably an error.
		Assert(iHeight >= 0);
		Assert(iHeight < 128);

//		fTexelsPerMetre = 0;

		rptr_const<CShape> psh = rptr_const_dynamic_cast(CShape, prdtGetRenderInfo());

		#if (VER_TEST)
		{
			//
			// Error if it is bump-mapped.
			//
			rptr<CMesh> pmsh = pmshGetMesh();
			if (pmsh)
			{
				// Check for bump-mapping.
				bool b_bump_mapped = false;
				for (uint u = 0; u < pmsh->pasfSurfaces.uLen; ++u)
				{
					if (pmsh->pasfSurfaces[u].ptexTexture && pmsh->pasfSurfaces[u].ptexTexture->seterfFeatures[erfBUMP])
					{
						b_bump_mapped = true;
						break;
					}
				}

				if (b_bump_mapped)
				{
					char str_buffer[256];
					sprintf(str_buffer, "Terrain object is bump-mapped!  (%s)\n", strGetInstanceName());
					
					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}
			}
		}	
		#endif
		
		// Determine the world resolution of this texture, and update max.
		aptr<CShape::CPolyIterator> ppi = psh->pPolyIterator(this);
		for (CShape::CPolyIterator& pi = *ppi; pi.bNext(); )
		{
			// To store the terrain clut, obtain the texture of the first polygon.
			const CTexture* ptex = pi.ptexTexture();
			ptr<CClut> pclut = ptex->ppcePalClut->pclutClut;
			if (!(const CClut*)pclutTerrain)
			{
				pclutTerrain = pclut;
				clrAverage = ptex->prasGetTexture()->clrFromPixel(ptex->tpSolid);
			}

#if VER_TEST
			if ((const CClut*)pclutTerrain != (const CClut*)pclut)
			{
				char str_buffer[256];
				sprintf(str_buffer, "Terrain object with bad CLUT!  (%s)\n", strGetInstanceName());
				
				if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
					DebuggerBreak();
			}
#endif
/*
			// UNUSED

			// Calculate the gradients based on the first 3 vertices, because
			// gradients are supposed to be constant across the polygon.

			CVector2<> v2_01 = pi.v3Point(1) - pi.v3Point(0);
			CVector2<> v2_02 = pi.v3Point(2) - pi.v3Point(0);

			// U and V derivatives.
			CVector2<> v2_t01 = pi.tcTexCoord(1) - pi.tcTexCoord(0);
			CVector2<> v2_t02 = pi.tcTexCoord(2) - pi.tcTexCoord(0);

			CVector2<> v2_du
			(
				v2_t01.tX * v2_02.tY - v2_t02.tX * v2_01.tY,
				v2_t01.tX * v2_02.tX - v2_t02.tX * v2_01.tX
			);
			TReal r_du = v2_du.tLen() * ptex->prasGetTexture()->iWidth;

			CVector2<> v2_dv
			(
				v2_t01.tY * v2_02.tY - v2_t02.tY * v2_01.tY,
				v2_t01.tY * v2_02.tX - v2_t02.tY * v2_01.tX
			);
			TReal r_dv = v2_dv.tLen() * ptex->prasGetTexture()->iHeight;

			// Take the maximum of the gradient magnitudes, 
			// and divide the derivatives by the area cross product.
			TReal r_div = (v2_01 ^ v2_02) * fGetScale();
			TReal r_res = Max(r_du, r_dv) / r_div;
			SetMax(fTexelsPerMetre, r_res);
*/
		}

//		SetMax(fMaxTexelsPerMetre, fTexelsPerMetre);

		// Also, make sure its height is zero.
		CVector3<> v3 = v3Pos();
		v3.tZ = 0;
		SetPos(v3);
	}

	//*****************************************************************************************
	void CTerrainObj::Move(const CPlacement3<>& p3_new, CEntity* pet_sender)
	{
		// Remember the current placement.
		CPlacement3<> p3_old = pr3Presence();

		// Do not allow Z axis movement.
		CPlacement3<> p3_new_mod = p3_new;
		p3_new_mod.v3Pos.tZ = p3_old.v3Pos.tZ;

		// The world database really knows how to deal with moved partitions.
		wWorld.Move(this, p3_new_mod, wWorld.ppartTerrainPartitionList());

		// Dispatch a move message.
		CMessageMove(CMessageMove::etMOVED, this, p3_old, pet_sender).Dispatch();
	}

#if VER_TEST
	//*****************************************************************************************
	int CTerrainObj::iGetDescription
	(
		char *	pc_buffer,
		int		i_buffer_len
	)
	{
		char buffer[128];
		sprintf(buffer,"Sound Material Hash: %x\nHeight: %d\n",matSoundMaterial, iHeight);

		Assert(strlen(pc_buffer) < i_buffer_len);

		strcpy(pc_buffer, buffer);
		return strlen(pc_buffer);
	}
#endif

