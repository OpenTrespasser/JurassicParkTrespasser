/***********************************************************************************************
 * Copyright (c) DreamWorks Interactive 1996
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/GroffIO.cpp                                                $
 * 
 * 156   9/27/98 2:13a Mmouni
 * Fixed load progress to be more consistent.
 * 
 * 155   98.09.23 3:32p Mmouni
 * Corrected interpolated position for double joints.
 * 
 * 154   98.09.22 12:26a Mmouni
 * Changed so that the unlit property is on a per mesh instead of per texture basis.
 * 
 * 153   9/08/98 8:49p Rwyatt
 * Put all CTextures into the image loader fast heap
 * 
 * 152   98.09.08 2:34p Mmouni
 * Made changes to support creation of non CMesh objects for invisible geometry.
 * 
 * 151   98.09.07 5:25p Mmouni
 * Changed pmshLoadObject to prdtLoadObject and changed instancing to use CRenderType instead
 * of CMesh.
 * 
 * 
 * 150   9/05/98 11:10p Agrant
 * now position and orientation use different ratios
 * 
 * 149   9/05/98 10:12p Agrant
 * Fixed poor conditional to get deltaY back for animating textures
 * 
 * 148   9/04/98 6:05p Agrant
 * Now checks for zero size before calling fioFile::iRead/iWrite.
 * Added AlwaysAssert() if optimized groff save fails.
 * 
 * 147   9/03/98 8:25p Kmckis
 * changed the assert distance between a biomesh center and it's joints to 20 M so the
 * Brachiosaur will load at bigger sizes
 * 
 * 146   8/27/98 1:33p Shernd
 * Improved callback updates
 * 
 * 145   8/26/98 4:42p Asouth
 * Moved a loop variable out of loop scope into the enclosing block
 * 
 * 144   8/25/98 7:27p Agrant
 * Allow non-ideally named objects to have detail reduced version, but do not allow Foo-34 to
 * have them.
 * 
 * 143   8/25/98 9:17a Shernd
 * Added callback functionality
 * 
 * 142   98.08.21 7:39p Mmouni
 * Now re-writes optimzed GROFF files.
 * 
 * 141   8/21/98 3:35p Agrant
 * detail redu warning now at 128
 * 
 * 140   8/20/98 5:38p Mmouni
 * Changes for additional clut control text props.
 * 
 * 139   98.08.15 8:57p Mmouni
 * Fixed animated textures for bio-meshes.
 * 
 * 138   98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 137   98.08.10 4:22p Mmouni
 * Added support for bio meshes having CMeshAnimating properties.
 * Cleaned up CMeshAnimating member access.
 * 
 * 136   8/04/98 2:36p Agrant
 * allow detail reduction warning to be turned off
 * 
 * 135   98.08.03 6:41p Mmouni
 * Changed mesh substitution error messages.
 * 
 * 134   98.07.28 4:36p Mmouni
 * Bounding volume is now constructed from wrap points if possible.
 * 
 * 133   7/24/98 8:31p Rwycko
 * fixed broken assert.  That Drew is an idiot.
 * 
 * 132   7/21/98 8:57p Agrant
 * say which mesh needs the detail reduced version
 * 
 * 131   7/20/98 7:08p Agrant
 * added freeze frame text prop for animated textures
 * 
 * 130   98.07.17 6:32p Mmouni
 * Fixed error where shodow mesh was causing main mesh to be re-scaled instead of being
 * re-scaled itself.
 * 
 * 129   98/07/14 20:12 Speter
 * Indented info message.
 * 
 * 128   7/14/98 4:08p Agrant
 * more info in BUMP CURVES dout
 * 
 * 127   98.07.10 12:56p Mmouni
 * Added extra info to missing substitute mesh message.
 * 
 * 126   98/07/09 16:43 Speter
 * Pivot points now stored in object scale.
 * 
 * 125   7/08/98 10:00p Rwycko
 * Fixed to avoid infinite loop with animating textures with a 0 or less Interval.
 * 
 * 124   98/07/08 1:03 Speter
 * Set v3Pivot for invisible meshes too. Fixed bug in substitute meshes; was marking every
 * point as different; now compare groff points vs. groff points
 * 
 * 123   98/07/01 23:36 Speter
 * Blow off assert on imported normal.
 * 
 * 122   98/06/29 16:24 Speter
 * Fixed loading of specular angles. Moved calls of ApplyCurves to CMehs(), and
 * CBioMesh::MakeRelative() to CBioMesh(). Removed call to MakeRelative() or original
 * substitute mesh.
 * 
 * 121   98.06.28 4:28p Mmouni
 * Added support for setting default clut start and stop colors.
 * 
 * 120   6/28/98 12:49a Kmckis
 * made the assert for constructing joint names from the biomesh object name an AlwaysAssert so
 * it can be easier determined in release mode
 * 
 * 119   6/26/98 7:25p Agrant
 * Allow 2 track animating textures.
 * 
 * 118   98.06.26 11:36a Mmouni
 * Fixed problem with custom clut colours.
 * 
 * 117   6/24/98 5:48p Agrant
 * Load Ratio method for double joints
 * 
 * 116   6/24/98 4:52p Agrant
 * Double joint loading fixes
 * 
 * 115   6/23/98 3:01p Kmckis
 * PVA joint assignment in text prop hacking
 * 
 * 114   98.06.21 6:08p Mmouni
 * Fixed problems with hack for loading animatied textures.
 * 
 **********************************************************************************************/

// #include "GUIApp/StdAfx.h"

#include "common.hpp"
#include "GroffIO.hpp"
#include "Lib/Sys/SysLog.hpp"
#include "Lib/Sys/SmartBuffer.hpp"
#include "Lib/Groff/FileIO.hpp"
#include "Lib/Groff/Groff.hpp"
#include "Lib/Groff/GroffLoader.hpp"
#include "Lib/GeomDBase/Skeleton.hpp"
#include "Lib/Loader/LoadTexture.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/PlatonicInstance.hpp"
#include "Lib/Loader/PVA.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/GeomDBase/InvisibleShape.hpp"

#include "Lib/Renderer/LightBlend.hpp"

#include "Lib/Sys/DebugConsole.hpp"

#include "Lib/Sys/Profile.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Loader/ImageLoader.hpp"



#define VER_DETAIL_REDUCTION_WARNING      VER_TEST
//#define VER_DETAIL_REDUCTION_WARNING      FALSE


#define bADJUST_BIOMESH_CENTRE	1

#define bVECTOR_POS_FINITE(v)\
	(v.tX >= 0 && _finite(v.tX) && v.tY >= 0 && _finite(v.tY))

#define bVECTOR_FINITE(v)\
	(_finite(v.tX) && _finite(v.tY))


// Groffy globals.
extern uint g_u_Version_Number;
extern bool bOptimizedGROFF;


//**********************************************************************************************
//
static inline CVector3<> v3Vector
(
	const fvector3& fv3					// Vector to convert in fv3 format.
)
//
// Returns:
//		The vector in CVector3<> format.
//
//**************************************
{
	return CVector3<>(fv3.X, fv3.Y, fv3.Z);
}

//**********************************************************************************************
CPresence3<> pr3Presence(const CGroffObjectName& gon)
{
	return CPresence3<>
	(
	 	CRotate3<>(d3XAxis, gon.fv3Rotation.X) * 
		CRotate3<>(d3YAxis, gon.fv3Rotation.Y) * 
		CRotate3<>(d3ZAxis, gon.fv3Rotation.Z),
		gon.fScale,
		CVector3<>(gon.fv3Position.X, gon.fv3Position.Y, gon.fv3Position.Z)
	);
}

//**********************************************************************************************
CGroffIO::CGroffIO()
{
#ifdef DEBUG_MODE
	slLogfile.Open("\\Export\\Logfiles\\GroffLoader.log");
#endif
}


//**********************************************************************************************
CGroffIO::~CGroffIO()
{
#ifdef DEBUG_MODE
	slLogfile.Close();
#endif
}


//**********************************************************************************************
bool CGroffIO::bOpenFile(const char* str_export_filename)
{
	// Open the Groff output file.  Were we successful?
	if (!fioFile.bOpen(str_export_filename, eRead))
	{
		// We were not able to open the file.  Return an error.
		return false;
	}

	// Atempt to load the image into memory.
	if (!fioFile.bReadImage(pfnWorldNotify, ui32_NotifyParam))
	{
		// It is not 10, so report the error.
		sprintf(str_message, "bOpenFile:  Unable to load Groff image into memory.");
		slLogfile.Msg(str_message);

		// Return an error.
		return false;
	}

	uint u_version_number;
	// Attempt to load the object file header.
	if (!bLoadHeader(u_version_number))
	{
		// Close the file.
		fioFile.bClose();

		// Return an error.
		return false;
	}

	// Is the groff version number compatible with GROFF version ID?
	if (u_version_number > GROFF_VERSION_ID || u_version_number < 11)
	{
		// No!  Report the error.
		sprintf(str_message, "bOpenFile:  Groff file version number reported as %d.", u_version_number);
		slLogfile.Msg(str_message);

		// Return an error.
		return false;
	}

	// Return a successful result
	return true;
}


//**********************************************************************************************
bool CGroffIO::bObjectInfo
(
	CGroffObjectInfo&   goi_info, 
	CValueTable&	    vtab_value_table
)
{
	//
	// File should now be open, so load the region and build an object list.
	//
	TSectionHandle seh_region;

	// Attempt to lookup the region section handle for this file.
	seh_region = fioFile.sehLookup(".region");

	// Were we successful?
	if (seh_region == 0)
	{
		// We were unable to add this object to the list.
		slLogfile.Msg("bFileInfo: Unable to locate the region object.");

		// Return an error.
		return false;

	}
	// Load the region into memory an construct the object list from it.
	CGroffRegion gr_region;

	if (!bLoadRegion(seh_region, gr_region))
	{
		// We were unable to load the region.  Return an error.
		return false;
	};

	// Do we have any attributes associated with this file?
	if (gr_region.sehAttributeSection != 0)
	{
		// Yes!  Attempt to load the value table.  Were we successful?
		if (!bLoadAttributes(gr_region.sehAttributeSection, vtab_value_table))
		{
			// Log the error.
			slLogfile.Msg("bObjectInfo: Unable to load the value table.");

			// No!  We were unable to load the value table.  Return an error.
			return false;
		}
	}

	// Setup the object count in the user structure,
	goi_info.uObjectCount = gr_region.uObjectCount;

	// Attempt to build an info structure for the user.
	if (!goi_info.bConfigure())
	{
		// Report the error.
		sprintf(str_message, "bObjectInfo: Unable to grow object info record to hold %d objects.", gr_region.uObjectCount);
		slLogfile.Msg(str_message);

		// Return an error result.
		return false;
	}

	CCycleTimer ctmr;

	// Setup so we dont call update too often.
	uint u_step = gr_region.uObjectCount / 100;
	uint u_next_count = 0;

	// Loop through the object list and return the object names and handles.
	CHandle h_att_handle;
	for (uint u_i = 0; u_i < gr_region.uObjectCount; u_i++)
	{
        if (pfnWorldNotify && u_i >= u_next_count)
        {
			u_next_count = u_i + u_step;

			// 10% - 15% of load.
			int i_percent = 10 + u_i * 5 / gr_region.uObjectCount;
            (pfnWorldNotify)(ui32_NotifyParam, 1, i_percent, 100);
        }

		// Get the objects section handle.
		TSectionHandle	seh_handle   = gr_region.agocObjectList[u_i].sehObject;
		TSymbolHandle	syh_symbol   = gr_region.agocObjectList[u_i].syhObjectName;
		fvector3		fv3_position = gr_region.agocObjectList[u_i].fv3Position;
		fvector3		fv3_rotation = gr_region.agocObjectList[u_i].fv3Rotation;
		float			f_scale		 = gr_region.agocObjectList[u_i].fScale;

		// Do we have any sttributes?
		if (gr_region.sehAttributeSection != 0)
		{
			// Yes!  Then copy any potential handles.
			h_att_handle = gr_region.agocObjectList[u_i].hAttributeHandle;
		}
		else
		{
			// No!  Set the handle to NULL.
			h_att_handle = hNULL_HANDLE;
		}

		// Attempt to load the object symbol name.
		const char* str_object_name = fioFile.strLookup(syh_symbol);

		// Were we successful?
		if (str_object_name)
		{
			// We found a symbol, so attempt to add it to the user object info structure.
			if (!goi_info.bAddObject(u_i, seh_handle, str_object_name, fv3_position, fv3_rotation, f_scale, h_att_handle))
			{
				// We were unable to add this object to the list.
				sprintf(str_message, "bFileInfo: Unable to add object :%s: to the object list.", str_object_name);
				slLogfile.Msg(str_object_name);

				// Return an error.
				return false;
			}
		}
	}

	extern CProfileStat psObjNameLookup;
	psObjNameLookup.Add(ctmr(), 1);

	// Return a successful result.
	return true;
}


//**********************************************************************************************
bool CGroffIO::bSaveOptimizedFile(const char *str_filename)
{
	if (!bOptimizedGROFF)
	{
		// Close the exiting file and create a new one.
		fioFile.bClose();

		dout << "Saving optimized GROFF as " << str_filename << "\n";

		// Attempt to re-write the header in the file.
		TSectionHandle seh_handle = fioFile.sehLookup(".header");

		// Were we successful?
		if (seh_handle == 0)
		{
			// No! Return an error.
			return false;
		}

		// Setup groff info in the header.
		CGroffHeader gh_header;
		gh_header.uMagicNumber = MAGIC_NUMBER;
		gh_header.uVersion = g_u_Version_Number | 0x80000000;

		// Seek to beginning of section.
		if (fioFile.iSeek(seh_handle, 0) != 0)
		{
			// Return an error.
			return false;
		}

		// Write out the file header.
		if (fioFile.iWrite(seh_handle, &gh_header, sizeof(CGroffHeader)) != sizeof(CGroffHeader))
		{
			// Return an error.
			return false;
		}

		// Create new file.
		if (fioFile.bOpen(str_filename, eWrite))
		{
			// Write it.
			if (fioFile.bWriteImage())
				return true;
		}
	}

	return false;
}


//**********************************************************************************************
bool CGroffIO::bCloseFile()
{
	// Delete the file image then close the file.
	fioFile.bDeleteImage();

	// Close the file.
	return fioFile.bClose();
}


const TReal rDIST_TOLERANCE	= 0.01;


//**********************************************************************************************
void CGroffIO::LoadLowerMeshes
(
	rptr<CMesh> pmsh, 
	const CGroffObjectName* pgon, 
	CLoadWorld*	pload,
	CValueTable* pvtable,
	TReal		r_instance_scale
)
//
// Load any lower-detail meshes that exist.
//
//**************************************
{
	Assert(pgon);
	Assert(pload);
	Assert(pvtable);

	// Shadow pointer.
	rptr<CShape> psh_shadow = rptr_nonconst(pmsh->pshGetTerrainShape());

//	'rptr<CShape>::rptr<CShape>(const CRPtr0 &)' : cannot convert parameter 1 from 'rptr_const<CShape>' to 'const CRPtr0 &' 

	// Object that needs a shadow mesh.
	rptr<CMesh>  pmsh_needs_shadow = pmsh; 

	if (!bIsPlatonicIdealName(pgon->strObjectName))
	{
		// If we have a name with no number, go ahead and snag the lower detail meshes.
		char buffer[128];

		bGetPlatonicIdealName(pgon->strObjectName, buffer);
		if (buffer[0] != 0)
			return;
	}

	// We now use text props.  
	SETUP_TEXT_PROCESSING(pvtable, pload)
	SETUP_OBJECT_HANDLE(pgon->hAttributeHandle)
	{
		// This could be far more elegant, but I'll wait for nested objects and array idioms.
		const CEasyString* pestr_submodel = 0;
		CPresence3<> pr3_dummy;
		const CGroffObjectName* pgon_submodel = 0;
		
		if (bFILL_pEASYSTRING(pestr_submodel, ESymbol(esDetailShadow)))
		{
			Assert(pestr_submodel);
			pgon_submodel = pload->goiInfo.pgonFindObject(pestr_submodel->strData());

			// Trying to assign a model as its own detail reduced version.
			AlwaysAssert(pgon_submodel != pgon);

			if (pgon_submodel)
			{
				// Load the inferior mesh and attach it.
				rptr<CRenderType> prdt_shadow = CRenderType::prdtFindShared
				(
					pgon_submodel,				
					pload,				
					pgon_submodel->hAttributeHandle,				
					pvtable
				);

				Assert(prdt_shadow);
				psh_shadow = rptr_dynamic_cast(CShape, prdt_shadow);
				Assert(psh_shadow);
				rptr<CMesh> pmsh_temp = rptr_dynamic_cast(CMesh, prdt_shadow);

				// Now rescale the mesh so that it matches the original mesh scale.
				TReal r_scale_factor = pgon_submodel->fScale / r_instance_scale;
				pmsh_temp->Rescale(r_scale_factor);
			}
			else
			{
				// Didn't find the specified shadow mesh.
#if VER_TEST
					char str_buffer[1024];
					sprintf(str_buffer, 
							"%s\n\nMissing shadow mesh:\n%s needs %s", 
							__FILE__, 
							pgon->strObjectName,
							pgon_submodel->strObjectName);

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
#endif
			}
		}

		for (int i = 0; i < 4; ++i)
		{
			if (bFILL_pEASYSTRING(pestr_submodel, ESymbol(esDetail1 + i)))
			{
				Assert(pestr_submodel);
				pgon_submodel = pload->goiInfo.pgonFindObject(pestr_submodel->strData());

				// Trying to assign a model as its own detail reduced version.
				AlwaysAssert(pgon_submodel != pgon);

				if (pgon_submodel)
				{
					// Get the radius, set the min screen area with it.
					float f_length_in_pixels = 0.0f;
					AlwaysVerify(bFILL_FLOAT(f_length_in_pixels, ESymbol(esA00 + i + 1)));
					
					// Do we use X, Y, or Z to determine the area?
					bool bX = false;
					bool bY = false;
					bool bZ = false;
					bFILL_BOOL(bX, esX);
					bFILL_BOOL(bY, esY);
					bFILL_BOOL(bZ, esZ);


					float f_axis_length = pmsh->bvbVolume.v3GetMax().tZ;
					if (bX)
					{
						f_axis_length = pmsh->bvbVolume.v3GetMax().tX;
						Assert(!bY && !bZ);
					}
					else if (bY)
					{
						f_axis_length = pmsh->bvbVolume.v3GetMax().tY;
						Assert(!bX && !bZ);
					}
					else
					{
						// Default to Z axis specification.
						f_axis_length = pmsh->bvbVolume.v3GetMax().tZ;
					}

					// Want to change to next detail level when selected axis gets less than "f_length_in_pixels"
					// However, we are ignoring rotation, so we will assume the object is showing us the entire 
					// axis in question flat on, and we will assume that pixels are square.
					
					// The radius is some percentage higher than the axis in question.  Increase the pixel spec by that amount.

					float f_area = (f_length_in_pixels) / (f_axis_length);		// Calc pixel/box units
					f_area = f_area * pmsh->bvbVolume.fMaxExtent();				// Mul ratio times radius in box units, 
					f_area = f_area * f_area;									//then square it all to get area.
					pmsh->rMinScreenArea = f_area * .48729f;

					// Load the inferior mesh and attach it.
					rptr<CRenderType> prdt_coarse = CRenderType::prdtFindShared
					(
						pgon_submodel,				
						pload,				
						pgon_submodel->hAttributeHandle,				
						pvtable
					);

					Assert(prdt_coarse);
					Assert(!pmsh->pshCoarser);
					pmsh->pshCoarser = rptr_dynamic_cast(CShape, prdt_coarse);
					Assert(pmsh->pshCoarser);
					pmsh = rptr_dynamic_cast(CMesh, prdt_coarse);
					Assert(pmsh);

					// Now rescale the mesh so that it matches the original mesh scale.
					TReal r_scale_factor = pgon_submodel->fScale / r_instance_scale;
					pmsh->Rescale(r_scale_factor);
				}
				else
				{
					// Submodel not found!
#if VER_TEST
					char str_buffer[1024];
					sprintf(str_buffer, 
							"%s\n\nMissing detail reduced mesh:\n%s needs %s", 
							__FILE__, 
							pgon->strObjectName,
							pestr_submodel->strData());

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
#endif
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	pmsh_needs_shadow->SetShadowShape(psh_shadow);
	END_OBJECT_HANDLE;
	END_TEXT_PROCESSING;
}

//**********************************************************************************************
int CGroffIO::iNumJoints(const CGroffObjectInfo& goi, const CGroffObjectName& gon)
{
	// Construct the joint object name pattern to look for.
	char str_joint_prefix[100];
	sprintf(str_joint_prefix, strJOINT_PREFIX "%s%%d", gon.strObjectName);

	// Construct the mesh object name pattern to look for.
	char str_mesh_prefix[100];
	sprintf(str_mesh_prefix, strMESH_PREFIX "%s%%d", gon.strObjectName);

	// Find joint or mesh object with maximum index.
	// If there are missing intermediate objects, that will be caught later.
	int i_joints = 0;

	for (uint u = 0; u < goi.uObjectCount; u++)
	{
		int i_index;

		// Match with joint or mesh name pattern.
		if 
		(
			sscanf(goi.gonObjectList[u].strObjectName, str_joint_prefix, &i_index) == 1 ||
			sscanf(goi.gonObjectList[u].strObjectName, str_mesh_prefix, &i_index) == 1
		)
			SetMax(i_joints, i_index+1);
	}
	return i_joints;
}


//**********************************************************************************************
bool bGroffSaveMeshHeap
(
	CFileIO&		fioFile,			// File I/O object.
	TSectionHandle	seh_dest,			// Setction to save data to.
	CMesh::CHeap&	mh,					// Mesh heap data to save.
	TSectionHandle	seh_material,		// Material section handle to save.
	bool			b_need_default,		// Flag indicated we use the default colour.
	CColour			clr_default			// Default colour to save.
)
{
	int i, i_temp;

#define WRITE_TO_SECTION(ptr, size) \
	if ((size) && fioFile.iWrite(seh_dest, (ptr), (size)) != (size)) goto ERROR

	// Seek to beginning of section.
	if (fioFile.iSeek(seh_dest, 0) != 0)
		goto ERROR;

	// Save handle to mapping.
	WRITE_TO_SECTION(&seh_material, sizeof(seh_material));

	// Save default color flag.
	i_temp = b_need_default;
	WRITE_TO_SECTION(&i_temp, sizeof(i_temp));

	// Save default color.
	WRITE_TO_SECTION(&clr_default.u4Value, sizeof(clr_default.u4Value));
	
	// Save pivot.
	WRITE_TO_SECTION(&mh.v3Pivot, sizeof(mh.v3Pivot));

	// Write lengths.
	WRITE_TO_SECTION(&mh.mav3Points.uLen, sizeof(mh.mav3Points.uLen));
	WRITE_TO_SECTION(&mh.mamvVertices.uLen, sizeof(mh.mamvVertices.uLen));
	WRITE_TO_SECTION(&mh.mapmvVertices.uLen, sizeof(mh.mapmvVertices.uLen));
	WRITE_TO_SECTION(&mh.mav3Wrap.uLen, sizeof(mh.mav3Wrap.uLen));
	WRITE_TO_SECTION(&mh.mampPolygons.uLen, sizeof(mh.mampPolygons.uLen));

	// Save mesh heap points.
	WRITE_TO_SECTION(mh.mav3Points.atArray, mh.mav3Points.uLen*sizeof(CVector3<>));

	// Save mesh heap vertices.
	for (i = 0; i < mh.mamvVertices.uLen; i++)
		mh.mamvVertices[i].pv3Point = 
				reinterpret_cast<CVector3<>*>(mh.mamvVertices[i].pv3Point - mh.mav3Points.atArray);

	WRITE_TO_SECTION(mh.mamvVertices.atArray, mh.mamvVertices.uLen*sizeof(CMesh::SVertex));

	// Save mesh heap vertex pointers.
	for (i = 0; i < mh.mapmvVertices.uLen; i++)
		mh.mapmvVertices[i] = 
				reinterpret_cast<CMesh::SVertex*>(mh.mapmvVertices[i] - mh.mamvVertices.atArray);

	WRITE_TO_SECTION(mh.mapmvVertices.atArray, mh.mapmvVertices.uLen*sizeof(CMesh::SVertex*));

	// Save mesh heap polygons.
	for (i = 0; i < mh.mampPolygons.uLen; i++)
	{
		mh.mampPolygons[i].papmvVertices.atArray = 
				reinterpret_cast<CMesh::SVertex**>(mh.mampPolygons[i].papmvVertices.atArray - mh.mapmvVertices.atArray);

		mh.mampPolygons[i].pSurface = 
				reinterpret_cast<CMesh::SSurface*>(mh.mampPolygons[i].pSurface - mh.masfSurfaces.atArray);
	}

	WRITE_TO_SECTION(mh.mampPolygons.atArray, mh.mampPolygons.uLen*sizeof(CMesh::SPolygon));

	// Save mesh heap wrap points.
	WRITE_TO_SECTION(mh.mav3Wrap.atArray, mh.mav3Wrap.uLen*sizeof(CVector3<>));

	return true;

ERROR:
	return false;
}


//**********************************************************************************************
bool bGroffLoadMeshHeap
(
	CFileIO&		fioFile,			// File I/O object.
	TSectionHandle	seh_source,			// Setction to load data from.
	CMesh::CHeap&	mh,					// Mesh heap data to load.
	TSectionHandle&	seh_material,		// Material section handle to load.
	bool&			b_need_default,		// Flag indicated we use the default colour.
	CColour&		clr_default			// Default colour to load.
)
{
	int i, i_temp;
	uint u_len;

#define READ_FROM_SECTION(ptr, size) \
	if ((size) && fioFile.iRead(seh_source, (ptr), (size)) != (size)) goto ERROR

	// Read handle to mapping.
	READ_FROM_SECTION(&seh_material, sizeof(seh_material));

	// Read default color flag.
	READ_FROM_SECTION(&i_temp, sizeof(i_temp));
	b_need_default = i_temp;

	// Read default color.
	READ_FROM_SECTION(&clr_default.u4Value, sizeof(clr_default.u4Value));
	
	// Read pivot.
	READ_FROM_SECTION(&mh.v3Pivot, sizeof(mh.v3Pivot));

	// Read lengths.
	READ_FROM_SECTION(&u_len, sizeof(u_len));
	mh.mav3Points.Grow(u_len);
	READ_FROM_SECTION(&u_len, sizeof(u_len));
	mh.mamvVertices.Grow(u_len);
	READ_FROM_SECTION(&u_len, sizeof(u_len));
	mh.mapmvVertices.Grow(u_len);
	READ_FROM_SECTION(&u_len, sizeof(u_len));
	mh.mav3Wrap.Grow(u_len);
	READ_FROM_SECTION(&u_len, sizeof(u_len));
	mh.mampPolygons.Grow(u_len);

	// Read mesh heap points.
	READ_FROM_SECTION(mh.mav3Points.atArray, mh.mav3Points.uLen*sizeof(CVector3<>));

	// Read mesh heap vertices.
	READ_FROM_SECTION(mh.mamvVertices.atArray, mh.mamvVertices.uLen*sizeof(CMesh::SVertex));

	for (i = 0; i < mh.mamvVertices.uLen; i++)
		mh.mamvVertices[i].pv3Point = 
				&mh.mav3Points[reinterpret_cast<int>(mh.mamvVertices[i].pv3Point)];

	// Read mesh heap vertex pointers.
	READ_FROM_SECTION(mh.mapmvVertices.atArray, mh.mapmvVertices.uLen*sizeof(CMesh::SVertex*));

	for (i = 0; i < mh.mapmvVertices.uLen; i++)
		mh.mapmvVertices[i] = 
				&mh.mamvVertices[reinterpret_cast<int>(mh.mapmvVertices[i])];

	// Read mesh heap polygons.
	READ_FROM_SECTION(mh.mampPolygons.atArray, mh.mampPolygons.uLen*sizeof(CMesh::SPolygon));

	for (i = 0; i < mh.mampPolygons.uLen; i++)
	{
		mh.mampPolygons[i].papmvVertices.atArray = 
				&mh.mapmvVertices[reinterpret_cast<int>(mh.mampPolygons[i].papmvVertices.atArray)];
	}

	// Read mesh heap wrap points.
	READ_FROM_SECTION(mh.mav3Wrap.atArray, mh.mav3Wrap.uLen*sizeof(CVector3<>));

	mh.bOptimized = true;

	return true;

ERROR:
	return false;
}

//**********************************************************************************************
rptr<CRenderType> CGroffIO::prdtLoadObject
	(
		const CGroffObjectName*	pgon,				// Pointer to GROFF name.
		CLoadWorld*				pload,				// Pointer to loader.
		const CHandle&			h_obj,				// Handle to the base object in the value table.
		CValueTable*			pvtable				// Pointer to the value table.
	)
{
	CCycleTimer ctmr;
	extern CProfileStat psMiscLoad;
	extern CProfileStat psGroffGeometry;
	extern CProfileStat psFaces;
	extern CProfileStat psNormalsNVerts;

	TSectionHandle seh_handle = pgon->sehObjectSection;
	CGroffObject go_object;
	float f_pseudo_radius = 0.0f;

	// First attempt to load the object. Were we successful?
	if (!bLoadObject(seh_handle, go_object))
	{
		// No! return an error result.
		return rptr0;
	}

	// Does this object have a geometry definition?
	if (go_object.sehGeometry == 0)
	{
		// No! Return an error.
		return rptr0;
	}

	// Now do all text-prop dependent processing.	
	SETUP_TEXT_PROCESSING(pvtable, pload)
	SETUP_OBJECT_HANDLE(h_obj)
	{
		bool b_textures = pload->bTextures;
		const CEasyString* pes_dummy;
		bool b_visible = true;
		CPresence3<> pr3_adjust;

		// Get the class name of the object, used later for creating the wrap.
		const CEasyString* pestr_class = 0;
		bFILL_pEASYSTRING(pestr_class, esClass);

		// Get the physics type name, as that is used to get mesh-specific data.
		const CEasyString* pestr_type = 0;
		bFILL_pEASYSTRING(pestr_type, esType);

		// Assume the mesh is animating if it has an esAnim00 prop.
		bool b_animating = bFILL_pEASYSTRING(pes_dummy, esAnim00);

		// Find out if the mesh is visible!
		bFILL_BOOL(b_visible, esVisible);

		// Decide on the normal computation method.
		int i_normal_method = enlGUESS;
		bFILL_INT(i_normal_method, esNormals);

		// All bump maps are non-curved regardless of the vertex normals, unless explicitly specified
		bool b_curved = false;
		bFILL_BOOL(b_curved, esCurved);

		// Obtain the heap on which to place mesh data.
		CMesh::CHeap& mh_build = CMesh::mhGetBuildHeap();

		// Check to see if this model has an associated skeleton.
		int i_joints = 0; 
		int i_double_joints = 0;
		bFILL_INT(i_joints, esNumJoints);
		bFILL_INT(i_double_joints, esNumDoubleJoints);

		// Section handle of the material definitions for this object.
		TSectionHandle seh_material = 0;
		CGroffMaterial gmat_material;

		// Flag that indicates if this mesh uses the default material (and its color).
		bool b_need_default_material = false;
		CColour clr_default_color;

		psMiscLoad.Add(ctmr());

		if (bOptimizedGROFF && i_joints == 0 && b_visible)
		{
			//
			// Read optimzed mesh heap info from GROFF.
			//
			if (!bGroffLoadMeshHeap(fioFile, go_object.sehGeometry, mh_build, seh_material, 
												b_need_default_material, clr_default_color))
			{
				return rptr0;
			}

			if (seh_material)
			{
				// Attempt to load the material definition.  Were we successful?
				if (!bLoadMaterial(seh_material, gmat_material))
				{
					// No! Return an error.
					return rptr0;
				}

				// Are there any materials defined in the material list?
				if (gmat_material.uTextureCount == 0)
				{
					// No! Return an error.
					return rptr0;
				}
			}
		}
		else
		{
			//
			// Read mesh heap info from GROFF.
			//
			CGroffGeometry gg_geometry;

			// Attempt to load the geometry. Were we successful?
			if (!bLoadGeometry(go_object.sehGeometry, gg_geometry))
			{
				// No!  Return an error.
				return rptr0;
			}

			Assert(gg_geometry.uVertexCount <= mh_build.mav3Points.uMax);
			Assert(gg_geometry.uFaceCount <= mh_build.mampPolygons.uMax);

			// Load the vertices into the list.
			uint u_i;
			for (u_i = 0; u_i < gg_geometry.uVertexCount; u_i++)
			{
				mh_build.mav3Points << v3Vector(gg_geometry.afv3Vertex[u_i]);
			}

			// Is the mesh invisible?
			if (!b_visible)
			{
				CVector3<> v3_pivot;

				// Get pivot, translating from world to object scale.
				v3_pivot.tX = gg_geometry.fv3PivotOffset.X / pgon->fScale;
				v3_pivot.tY = gg_geometry.fv3PivotOffset.Y / pgon->fScale;
				v3_pivot.tZ = gg_geometry.fv3PivotOffset.Z / pgon->fScale;

				// Yes!  We just need points and pivot, not faces, verts, normals, or textures!
				rptr<CInvisibleShape> invsh_return = rptr_new CInvisibleShape(mh_build.mav3Points, v3_pivot);

				// Reset the mesh heap.
				mh_build.Reset();

				psGroffGeometry.Add(ctmr());
				return rptr_cast(CRenderType, invsh_return);
			}

			//
			//	Now, load the rest of the mesh data.
			//
			CGroffMapping gmap_mapping;

			if (go_object.sehMapping)
			{
				// Attempt to load the mapping definition.  Were we successful?
				if (!bLoadMapping(go_object.sehMapping, gmap_mapping))
				{
					// No! Return an error.
					return rptr0;
				}

				seh_material = gmap_mapping.sehMaterial;

				if (seh_material)
				{
					// Attempt to load the material definition.  Were we successful?
					if (!bLoadMaterial(seh_material, gmat_material))
					{
						// No! Return an error.
						return rptr0;
					}

					// Are there any materials defined in the material list?
					if (gmat_material.uTextureCount == 0)
					{
						// No! Return an error.
						return rptr0;
					}
				}

				if (i_joints == 0 && b_visible)
				{
					// Reset the mapping section to be empty.
					fioFile.bResetSection(go_object.sehMapping);

					// Save the material section handle here.
					fioFile.iWrite(go_object.sehMapping, &seh_material, sizeof(seh_material));
				}
			}

			psGroffGeometry.Add(ctmr());

			//
			// We now have everything we need to build the mesh.
			//

			// Create the texture coordinate list.
			CLArray(CTexCoord, patcTexCoord, gmap_mapping.uTVertexCount);

			// Load the texture coordinates into the list.
			for (u_i = 0; u_i < gmap_mapping.uTVertexCount; u_i++)
			{
				// The Y texture coordinate used to be reversed here.
				// But it no longer is, now that BMP files are read in the proper order.
				patcTexCoord[u_i] = CTexCoord(gmap_mapping.afv2TVertex[u_i].X, gmap_mapping.afv2TVertex[u_i].Y);

#if VER_TEST
				if (!bVECTOR_FINITE(patcTexCoord[u_i]))
				{
					char buffer[512];
					sprintf(buffer, "%s\nInfinite texture coordinate in %s!\n", __FILE__, pgon->strObjectName);

					if (bTerminalError(ERROR_ASSERTFAIL, true, buffer, __LINE__))
						DebuggerBreak();
				}
#endif
			}

#if VER_TEST
			if (b_curved)
			{
				dout << "  BUMP CURVES in " << pgon->strObjectName << "\n";
			}
#endif

			// Make sure that we have a valid normal setting.
			Assert(i_normal_method >= enlBEGIN && i_normal_method < enlEND);

			psMiscLoad.Add(ctmr());

			// Load the normals and the vertices.
 			for (u_i = 0; u_i < gg_geometry.uFaceCount; u_i++)
			{
				// Do we have any texture vertices?
				if (gmap_mapping.uTVertexCount > 0)
				{
					// Do we use the MAX normals?
					if (i_normal_method == enlSPECIFIED)
					{
						// Yes!  Use the imported normals.
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].X], 
							CDir3<>(v3Vector(gg_geometry.afv3VertexNormal[u_i*3])),
							patcTexCoord[gmap_mapping.auv3TFace[u_i].X]
						);
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].Y], 
							CDir3<>(v3Vector(gg_geometry.afv3VertexNormal[u_i*3+1])),
							patcTexCoord[gmap_mapping.auv3TFace[u_i].Y]
						);
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].Z], 
							CDir3<>(v3Vector(gg_geometry.afv3VertexNormal[u_i*3+2])),
							patcTexCoord[gmap_mapping.auv3TFace[u_i].Z]
						);
					}
					else
					{
						// No!  Use our own normals.
						// Ignore the imported vertex normals.  Let them be calculated automatically.
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].X], 
							patcTexCoord[gmap_mapping.auv3TFace[u_i].X]
						);
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].Y], 
							patcTexCoord[gmap_mapping.auv3TFace[u_i].Y]
						);
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].Z], 
							patcTexCoord[gmap_mapping.auv3TFace[u_i].Z]
						);
					}
				}
				else
				{
					// Do we use the MAX normals?
					if (i_normal_method == enlSPECIFIED)
					{
						// Yes!  Use the imported normals.
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].X], 
							CDir3<>(v3Vector(gg_geometry.afv3VertexNormal[u_i*3])),
							CTexCoord(0.0f, 0.0f)
						);
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].Y], 
							CDir3<>(v3Vector(gg_geometry.afv3VertexNormal[u_i*3+1])),
							CTexCoord(0.0f, 0.0f)
						);
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].Z], 
							CDir3<>(v3Vector(gg_geometry.afv3VertexNormal[u_i*3+2])),
							CTexCoord(0.0f, 0.0f)
						);
					}
					else
					{
						// No!  Use our own normals.
						// Ignore the imported vertex normals.  Let them be calculated automatically.
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].X], 
							CTexCoord(0.0f, 0.0f)
						);
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].Y], 
							CTexCoord(0.0f, 0.0f)
						);
						new(mh_build.mamvVertices) CMesh::SVertex
						(
							&mh_build.mav3Points[gg_geometry.auv3Face[u_i].Z], 
							CTexCoord(0.0f, 0.0f)
						);
					}
				}
			}

			psNormalsNVerts.Add(ctmr());

			// Get wrap points.
			if (gg_geometry.uWrapVertexCount > 0)
			{
				// Copy vertices to mesh.
				for (int u_i = 0; u_i < gg_geometry.uWrapVertexCount; u_i++)
					mh_build.mav3Wrap << v3Vector(gg_geometry.afv3WrapVertex[u_i]);
			}

			// See if all faces have a material.
			if (gg_geometry.uFaceCount != gmap_mapping.uTFaceCount)
			{
				// Set the default material color.
				CColour clr((int)gg_geometry.uv3DefaultColor.X, 
							(int)gg_geometry.uv3DefaultColor.Y, 
							(int)gg_geometry.uv3DefaultColor.Z);

				b_need_default_material = true;
				clr_default_color = clr;
			}

			// Build the face list.
			for (u_i = 0; u_i < gg_geometry.uFaceCount; u_i++)
			{
				// Create a polygon, specifying the location and number of the vertices used.
				CMesh::SPolygon mp(mh_build, u_i*3, 3);

				// If we have face materials.
				if (u_i < gmap_mapping.uTFaceCount)
				{
					// Assign the correct surface index to the face.
					mp.pSurface = reinterpret_cast<CMesh::SSurface*>(gmap_mapping.auTFaceMaterialID[u_i]);
				}
				else
				{
					// Use default material.
					mp.pSurface = reinterpret_cast<CMesh::SSurface*>(gmat_material.uTextureCount);
				}

				mh_build.mampPolygons << mp;
			}

			// Get the pivot point of the mesh.
			mh_build.v3Pivot.tX = gg_geometry.fv3PivotOffset.X / pgon->fScale;
			mh_build.v3Pivot.tY = gg_geometry.fv3PivotOffset.Y / pgon->fScale;
			mh_build.v3Pivot.tZ = gg_geometry.fv3PivotOffset.Z / pgon->fScale;
		}

		{
			CGroffMaterialName gmatn_name;

			if (seh_material)
			{
				// Attempt to build the material name list.  Were we successful?
				if (!gmatn_name.bBuildNames(fioFile, gmat_material))
				{
					// No! Return an error.
					return rptr0;
				}
			}

			//
			// Texture properties!
			//
			float f_bumpiness = fDefaultBumpiness;
			bFILL_FLOAT(f_bumpiness, esBumpiness);

			bool b_load_bumpmaps = true;
			bFILL_BOOL(b_load_bumpmaps, esBumpmaps);

			bFILL_FLOAT(f_pseudo_radius, esPRadius);

			//  Material...
			float f_diffuse		   = rvMAX_COLOURED;
			float f_specular	   = 0.0;
			float f_specular_angle = 0.0;
			bool b_reflect		   = false;
			bool b_refract		   = false;
			float f_refract_index  = 1.0;
			float f_emissive	   = 0.0;
			bool  b_texture        = true;

			// Get the values.
			bFILL_FLOAT(f_diffuse,        esDiffuse);
			bFILL_FLOAT(f_specular,       esSpecular);
			bFILL_FLOAT(f_specular_angle, esSpecularAngle);
			bFILL_BOOL(b_reflect,         esReflect);
			bFILL_BOOL(b_refract,         esRefract);
			bFILL_FLOAT(f_refract_index,  esRefractIndex);
			bFILL_FLOAT(f_emissive,       esEmissive);
			bFILL_BOOL(b_texture,         esTexture);

			// Fill the material data structure.
			const CMaterial* pmat = CMaterial::pmatFindShared
				(
					f_diffuse,
					f_specular,
					angwFromAngle(f_specular_angle * dDEGREES),
					b_reflect,
					b_refract,
					f_refract_index,
					f_emissive
				);

			// Set global default clut start and stop colors.
			IF_SETUP_OBJECT_SYMBOL(esDefaultClutStartColor)
			{
				int i_red;
				int i_green;
				int i_blue;

				if (bFILL_INT(i_red, esR) && bFILL_INT(i_green, esG) && bFILL_INT(i_blue, esB))
				{
					clrDefStartRamp = CColour(i_red, i_green, i_blue);
				}
			}
			END_OBJECT_SYMBOL

			IF_SETUP_OBJECT_SYMBOL(esDefaultClutStopColor)
			{
				int i_red;
				int i_green;
				int i_blue;

				if (bFILL_INT(i_red, esR) && bFILL_INT(i_green, esG) && bFILL_INT(i_blue, esB))
				{
					clrDefEndRamp = CColour(i_red, i_green, i_blue);
				}
			}
			END_OBJECT_SYMBOL

			IF_SETUP_OBJECT_SYMBOL(esDefaultClutReflectionColor)
			{
				int i_red;
				int i_green;
				int i_blue;

				if (bFILL_INT(i_red, esR) && bFILL_INT(i_green, esG) && bFILL_INT(i_blue, esB))
				{
					clrDefReflection = CColour(i_red, i_green, i_blue);
				}
			}
			END_OBJECT_SYMBOL


			// Read CLUT start and stop colors.
			SPalClutControl pccSettings;

			pccSettings.b_use_clut_colours = false;
			pccSettings.clrClutStart = clrDefStartRamp;
			pccSettings.clrClutStop = clrDefEndRamp;
			pccSettings.clrClutReflection = clrDefReflection;
			IF_SETUP_OBJECT_SYMBOL(esClutStartColor)
			{
				int i_red;
				int i_green;
				int i_blue;

				if (bFILL_INT(i_red, esR) && bFILL_INT(i_green, esG) && bFILL_INT(i_blue, esB))
				{
					pccSettings.clrClutStart = CColour(i_red, i_green, i_blue);
					pccSettings.b_use_clut_colours = true;
				}
			}
			END_OBJECT_SYMBOL

			IF_SETUP_OBJECT_SYMBOL(esClutStopColor)
			{
				int i_red;
				int i_green;
				int i_blue;

				if (bFILL_INT(i_red, esR) && bFILL_INT(i_green, esG) && bFILL_INT(i_blue, esB))
				{
					pccSettings.clrClutStop = CColour(i_red, i_green, i_blue);
					pccSettings.b_use_clut_colours = true;
				}
			}
			END_OBJECT_SYMBOL

			IF_SETUP_OBJECT_SYMBOL(esClutReflectionColor)
			{
				int i_red;
				int i_green;
				int i_blue;

				if (bFILL_INT(i_red, esR) && bFILL_INT(i_green, esG) && bFILL_INT(i_blue, esB))
				{
					pccSettings.clrClutReflection = CColour(i_red, i_green, i_blue);
					pccSettings.b_use_clut_colours = true;
				}
			}
			END_OBJECT_SYMBOL

			// Load the texture associated with the shape.
			if (b_textures)
			{
				// Load and construct the texture for each surface in this mesh.
				psMiscLoad.Add(ctmr());

				// Loop through the materials and load them.
				for (int i_mat_num = 0; i_mat_num < gmat_material.uTextureCount; i_mat_num++)
				{
					rptr<CTexture> ptex;

					// Use whole object materila by default.
					const CMaterial* pmat_sub = pmat;

					// Sub-material properties?
					IF_SETUP_OBJECT_SYMBOL(ESymbol(esSubMaterial01 + i_mat_num))
					{
						// Default material values.
						float f_diffuse		   = rvMAX_COLOURED;
						float f_specular	   = 0.0;
						float f_specular_angle = 0.0;
						bool b_reflect		   = false;
						bool b_refract		   = false;
						float f_refract_index  = 1.0;
						float f_emissive	   = 0.0;
						bool  b_texture        = true;

						// Get any values set for this sub-material.
						bFILL_FLOAT(f_diffuse,        esDiffuse);
						bFILL_FLOAT(f_specular,       esSpecular);
						bFILL_FLOAT(f_specular_angle, esSpecularAngle);
						bFILL_BOOL(b_reflect,         esReflect);
						bFILL_BOOL(b_refract,         esRefract);
						bFILL_FLOAT(f_refract_index,  esRefractIndex);
						bFILL_FLOAT(f_emissive,       esEmissive);
						bFILL_BOOL(b_texture,         esTexture);

						// Fill the material data structure.
						pmat_sub = CMaterial::pmatFindShared
							(
								f_diffuse,
								f_specular,
								angwFromAngle(f_specular_angle * dDEGREES),
								b_reflect,
								b_refract,
								f_refract_index,
								f_emissive
							);
					}
					END_OBJECT_SYMBOL

					// Is there a texture on this sucker?
					if (strlen(gmatn_name.strTextureMap[i_mat_num]))
					{
						// Create the bitmap texture.
						ptex = ptexGetTexture
						(
							gmatn_name.strTextureMap[i_mat_num],
							b_load_bumpmaps ? gmatn_name.strBumpMap[i_mat_num] : "",
							f_bumpiness, 
							pmat_sub,
							pccSettings
						);
					}
					else
					{
						// Create solid color texture.
						CColour clr((int)gmat_material.auv3MaterialColor[i_mat_num].X, 
									(int)gmat_material.auv3MaterialColor[i_mat_num].Y, 
									(int)gmat_material.auv3MaterialColor[i_mat_num].Z);

						// Create the texture (use default material to save CLUT space).
						void* pv_tex = CLoadImageDirectory::pvAllocate( sizeof(CTexture) );
						ptex = rptr_new(pv_tex) CTexture(clr, &matDEFAULT);
					}

					if (!ptex)
						TerminalError(ERROR_UNABLETOLOAD_MESH, true, gmatn_name.strTextureMap[i_mat_num]);

					// Create the material on the mesh heap.
					new(mh_build.masfSurfaces) CMesh::SSurface
					(
						ptex
					);
				}

				extern CProfileStat psTextures;
				psTextures.Add(ctmr());
			}

			// See if we need to create a default material.
			if (b_need_default_material)
			{
				void* pv_tex = CLoadImageDirectory::pvAllocate( sizeof(CTexture) );

				// Create the texture (use default material to save CLUT space).
				rptr<CTexture> ptex = rptr_new(pv_tex) CTexture(clr_default_color, &matDEFAULT);

				// Create the material on the mesh heap.
				new(mh_build.masfSurfaces) CMesh::SSurface
				(
					ptex
				);
			}

			psMiscLoad.Add(ctmr());

			// Set the material pointers for all the polygons.
			for (uint u_i = 0; u_i < mh_build.mampPolygons.uLen; u_i++)
			{
				// Convert from a pointer to an index now that we have the surfaces set up.
				mh_build.mampPolygons[u_i].pSurface = 
						&mh_build.masfSurfaces[reinterpret_cast<int>(mh_build.mampPolygons[u_i].pSurface.ptGet())];

			}

			psFaces.Add(ctmr());

			// The mesh we're making.
			rptr<CMesh> pmsh_return;

			// Need to know if we are splitting or merging.
			bool b_split = CMesh::bSplitPolygons;
			bFILL_BOOL(b_split, esSplit);

			bool b_merge = CMesh::bMergePolygons;
			bFILL_BOOL(b_merge, esMerge);

			// Do we have a biomesh?
			if (i_joints == 0)
			{
				// No! We have a regular non-skeletal mesh.

				// Do we have an animating mesh?
				if (!b_animating)
				{
					bool b_planted = false;
					bFILL_BOOL(b_planted, esPlanted);

					// Is this mesh planted?
					if (b_planted)
					{
						// Yes!  Plant it.
						float f_distance = 0;
						bFILL_FLOAT(f_distance, esHeight);

						//pv_mesh = new(CMesh::pvGlobalHeap) char[sizeof(CMesh)];
						pmsh_return = rptr_cast(CMesh, rptr_new(CMesh::fhGlobalMesh) CMeshPlanted(mh_build, ::pr3Presence(*pgon), f_distance, (ENormal) i_normal_method, b_merge, b_split, b_curved));
						MEMLOG_ADD_COUNTER(emlMeshHeap, sizeof(CMeshPlanted));
					}
					else
					{
						// No.  Normal mesh.
						pmsh_return = rptr_new(CMesh::fhGlobalMesh) CMesh(mh_build, (ENormal) i_normal_method, b_merge, b_split, b_curved);
						MEMLOG_ADD_COUNTER(emlMeshHeap, sizeof(CMesh));
					}
				}
				else
				{
					// Yes! Make an animating mesh.
					rptr<CMeshAnimating> pma = rptr_new(CMesh::fhGlobalMesh) CMeshAnimating(mh_build, (ENormal) i_normal_method, b_merge, b_split, b_curved);
					MEMLOG_ADD_COUNTER(emlMeshHeap, sizeof(CMeshAnimating));
					pmsh_return = rptr_cast(CMesh, pma);

					//
					// Get the animation data.
					//
					float f_interval = 0.04f;				// 1/25th of a second.
					bFILL_FLOAT(f_interval, esInterval);
					pma->SetInterval(f_interval);

					float f_dx = 0.0f, f_dy = 0.0f;
					bool b_delta =	bFILL_FLOAT(f_dx, esDeltaX);
					b_delta =		bFILL_FLOAT(f_dy, esDeltaY) || b_delta;
					if (b_delta)
					{
						pma->SetCoordDeltas(f_dx, f_dy);
					}

					// Now snag the base animating texture and see if it is present.
					const CEasyString *pestr_texture;
					const CEasyString *pestr_bump;
					bool b_keep_going = false;
					char ac_texture[512];
					char ac_bump[512];

					// Get the first anim texture name.
					bFILL_pEASYSTRING(pestr_texture, esAnim00);

					//  Add all those textures.
					int i_texture_index = 0;
					do
					{
						Assert(i_texture_index < iMAX_ANIMATION_FRAMES);

						//
						// HACK HACK HACK - Get the texture directory.
						//
						extern char* strMipFileCache;
						char *str_start = strMipFileCache;

						AlwaysAssert(str_start);

						strcpy(ac_texture, str_start);
						strcat(ac_texture, pestr_texture->strData());

						// Get the bumpmap, if any.
						if (bFILL_pEASYSTRING(pestr_bump, ESymbol(esAnimB00 + i_texture_index)))
						{
							strcpy(ac_bump, str_start);
							strcat(ac_bump, pestr_bump->strData());
						}
						else
						{
							ac_bump[0] = '\0';
						}

						// Open the material.
						rptr<CTexture> ptex = ptexGetTexture
						(
							ac_texture,
							ac_bump,
							f_bumpiness, 
							pmat,
							pccSettings
						);

						// Take the material and tell the mesh to add it to the cycle list.
						if (ptex)
						{
							pma->AddTexture(ptex);
						}

						// Make sure it worked.
						Assert(ptex);

						// Next texture.
						++i_texture_index;

						if (i_texture_index >= iMAX_ANIMATION_FRAMES)
							b_keep_going = false;
						else
							b_keep_going = bFILL_pEASYSTRING(pestr_texture, ESymbol(esAnim00 + i_texture_index));

					} while (b_keep_going);

					int i_track_two = 0;
					if (bFILL_INT(i_track_two, esTrackTwo))
						pma->SetTrackTwo(i_track_two);

					int i_freeze_frame = -1;
					if (bFILL_INT(i_freeze_frame, esFreezeFrame))
						pma->SetFreezeFrame(i_freeze_frame);

					int i_material_no = -1;
					if (bFILL_INT(i_material_no, esAnimSubMaterial))
						pma->SetSurface(i_material_no - 1);
				}

				if (!bOptimizedGROFF)
				{
					// Save updated data out of the mesh heap.
					bool b_ret = bGroffSaveMeshHeap(fioFile, go_object.sehGeometry, mh_build, seh_material, 
													b_need_default_material, clr_default_color);
					AlwaysAssert(b_ret);
				}

				// Reset the mesh heap.
				mh_build.Reset();
			}
			else
			{
				// Yes!  We have a biomesh.
				CPresence3<> pr3_obj = pr3Presence(*pgon);
				CPresence3<> pr3_inv_obj = ~pr3_obj;

				// Allocate array for the joints.
				CPArray< CTransform3<> > patf3_joints(i_joints + i_double_joints);

				// Allocate array for the joint linkages.
				CPArray<uint>	pau_joint_links(mh_build.mav3Points.uLen);

				// Allocate arrays for double joints.
				CPArray<int>	pai_double_first(i_double_joints);
				CPArray<int>	pai_double_second(i_double_joints);
				CPArray<float>	paf_double_pos_ratio(i_double_joints);
				CPArray<float>	paf_double_rot_ratio(i_double_joints);

				// Set to an invalid value for consistency checking.
				pau_joint_links.Fill(uint(-1));
				
				char str_object[100];
				int i_PVA = -1;
				bFILL_INT(i_PVA, esPVA);
				dout << "PVA = " << i_PVA << "\n";

				// Must have a valid PVA index.
				Assert(i_PVA >= -2);

				int i = 0;
				// Are we using boring joints?
				if (i_PVA == -1)
				{
					// Yes!  Use identity presences.
					for (i = 0; i < i_joints; i++)
					// Store the placement info for the joint, transformed to object space.
						patf3_joints[i] = CPlacement3<>();
				}
				else
				{
#if VER_TEST
					// Save the mesh location for comparison with the joint locations.
					CVector3<> v3_mesh_location = ::pr3Presence(*pgon).v3Pos;
#endif

					char temp_buffer[256];
					bool b_has_ideal = bGetPlatonicIdealName(pgon->strObjectName, temp_buffer);
					if (b_has_ideal)
					{
						// Chop off the zeroes.
						temp_buffer[strlen(temp_buffer) - 3] = '\0';
					}
					else
					{
						// No ideal name.  Go with actual name.
						strcpy(temp_buffer, pgon->strObjectName);
					}

					// No! Read the joints and submeshes.
					for (i = 0; i < i_joints + i_double_joints; i++)
					{
						// Construct the joint object name.  Ignore any -XX suffix.
						sprintf(str_object, strJOINT_PREFIX "%s%02d", temp_buffer, i);
						const CGroffObjectName* pgon = pload->goiInfo.pgonFindObject(str_object);

						AlwaysAssert(pgon);
						
						CVector3<> v3_jpos = ::pr3Presence(*pgon).v3Pos;
						//dout << "Joint " << i << "\t" << v3_jpos.tX << "\t" << v3_jpos.tY << "\t" << v3_jpos.tZ << "\n";

#if VER_TEST
						if ((v3_jpos - v3_mesh_location).tLenSqr() > 400.0f)
						{
							// Joint is more than 10m away from center of biomesh.  This is probably a bug.
							AlwaysAssert(false);
						}
#endif


						// Store the placement info for the joint, transformed to object space.
						patf3_joints[i] = (pr3Presence(*pgon) * pr3_inv_obj).p3Placement();

						// Is this a double joint?
						if (i >= i_joints)
						{
							// Yes!  Get the text prop data for the joint.
							SETUP_TEXT_PROCESSING(pvtable, pload)
							SETUP_OBJECT_HANDLE(pgon->hAttributeHandle)
							{
								int i_first = 0;
								int i_second = 0;

								bool b; 
								b = bFILL_INT(i_first, esAnim00);
								Assert(b);
								pai_double_first[i - i_joints] = i_first;

								b = bFILL_INT(i_second,ESymbol(esAnim00+1));
								Assert(b);
								pai_double_second[i - i_joints] = i_second;

								float f = 0.0f;
								bFILL_FLOAT(f, esRatio);
								Assert(f >= 0.0f);
								Assert(f <= 1.0f);
								paf_double_pos_ratio[i - i_joints] = f;
								
								// Move the joint to the interpolated position between the parent joints!
								patf3_joints[i].v3Pos = (patf3_joints[i_first].v3Pos  * f) +
														(patf3_joints[i_second].v3Pos * (1.0f - f));

								bFILL_FLOAT(f, esRotationRatio);
								Assert(f >= 0.0f);
								Assert(f <= 1.0f);
								paf_double_rot_ratio[i - i_joints] = f;
							}
							END_OBJECT;
							END_TEXT_PROCESSING;
						}

						// Is this a text prop based PVA?
						if (i_PVA == -2)
						{	
							// Yes!  Grab verts from props!
							SETUP_TEXT_PROCESSING(pvtable, pload)
							SETUP_OBJECT_HANDLE(pgon->hAttributeHandle)
							{
								int i_array = 0;
								int i_vertex = -1;

								bool b = false; 
								b = bFILL_INT(i_vertex, esA00);

								while (b)
								{
									// Set the vert to rely on joint i
									pau_joint_links[i_vertex] = i;
	
									// Get the next array entry.
									++i_array;
									b = bFILL_INT(i_vertex, ESymbol(esA00 + i_array));
								}
							}
							END_OBJECT;
							END_TEXT_PROCESSING;
						}
					}
				}

				JoinJointsAndPoints(pau_joint_links, i_PVA);

	#if VER_DEBUG
				// Make sure all vertices were mapped.
				int i_unmatched = 0;

				for (i = 0; i < pau_joint_links.uLen; i++)
				{
					if (pau_joint_links[i] == uint(-1))
					{
						i_unmatched++;

						char str_buffer[1024];
						sprintf(str_buffer, 
								"%s\n\nMissing assignment for vert %d in %s.\n", 
								__FILE__, 
								i,
								pgon->strObjectName);

						if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
							DebuggerBreak();
					}
				}
				AlwaysAssert(i_unmatched == 0);
	#endif // VER_DEBUG


				// Keep track of new max coords, in case we need to rescale.
				TReal r_max_coord = 1.0;

				// Offset entire thing by the centre joint's position, because this should be at the origin.
				CVector3<> v3_offset = -v3JointOrigin(i_PVA, patf3_joints, i_joints - 1);

				if (!v3_offset.bIsZero())
				{
		#if !bADJUST_BIOMESH_CENTRE
					// The model should be defined and exported with joint 0 at the origin.
					Assert(0);
		#else
					// Translation must be done.

					// Keep track of new max coords, in case we need to rescale.
					r_max_coord = 0.0;

					// Do all points.
					for (int i = 0; i < mh_build.mav3Points.uLen; i++)
					{
						mh_build.mav3Points[i] += v3_offset;
						SetMax(r_max_coord, Abs(mh_build.mav3Points[i].tX));
						SetMax(r_max_coord, Abs(mh_build.mav3Points[i].tY));
						SetMax(r_max_coord, Abs(mh_build.mav3Points[i].tZ));
					}

					// Do all joints.
					for (i = 0; i < patf3_joints.uLen; i++)
						patf3_joints[i].v3Pos += v3_offset;

					// Adjust the object offset to compensate.
					pr3_adjust.v3Pos -= v3_offset;

					if (r_max_coord != 1.0)
					{
						// Oh, no, we must do a second pass and rescale everything.
						TReal r_rescale = 1.0 / r_max_coord;

						// Do all points.
						for (int i = 0; i < mh_build.mav3Points.uLen; i++)
							mh_build.mav3Points[i] *= r_rescale;

						// Do all joints.
						for (i = 0; i < patf3_joints.uLen; i++)
							patf3_joints[i].v3Pos *= r_rescale;

						// Adjust object scale to compensate.
						pr3_adjust.rScale *= r_max_coord;
					}
		#endif
				}

				// Do we have any mesh substitutions?
				bool b_substitutions = false;
				const CEasyString* pestr_temp = 0;
				b_substitutions = bFILL_pEASYSTRING(pestr_temp, esA00);
			
				// Pointer to animating mesh in case this bio-mesh has any animating properties.
				rptr<CMeshAnimating> pma_temp;

				if (b_substitutions)
				{
					rptr<CSubBioMesh> psbmsh_return = rptr_new(CMesh::fhGlobalMesh) CSubBioMesh
					(
						mh_build,
						patf3_joints,
						pau_joint_links,
						pai_double_first,
						pai_double_second,
						paf_double_pos_ratio,
						paf_double_rot_ratio
					);
					MEMLOG_ADD_COUNTER(emlMeshHeap, sizeof(CSubBioMesh));
					
					pma_temp = rptr_cast(CMeshAnimating, psbmsh_return);
					pmsh_return = rptr_cast(CMesh, psbmsh_return);

					// Add the substitutions!
					CSubstitute* psub_original = new CSubstitute();

					char ai_changing[1024];
					for (int i = 1023; i >= 0; --i)
					{
						ai_changing[i] = false;
					}


					int i_sub = 0;
					const CEasyString* pestr_sub = 0;
					while (bFILL_pEASYSTRING(pestr_sub, ESymbol(esA00 + i_sub)))
					{
						Assert(pestr_sub);
						CGroffObjectName* pgon_sub = pload->goiInfo.pgonFindObject(pestr_sub->strData());

						#if VER_TEST
							// Trying to assign a model as its own detail reduced version.
							if (pgon == pgon_sub)
							{
								SetCacheText("A mesh cannot substitute for itself.");
								AlwaysAssert(0);
							}

							// Missing substitue mesh.
							if (!pgon_sub)
							{
								char str_temp[256];
								strcpy(str_temp, "Couldn't find subsitute mesh -> ");
								strcat(str_temp, pestr_sub->strData());
								SetCacheText(str_temp);
								AlwaysAssert(0);
							}
						#endif

						if (pgon_sub)
						{
							//
							// Load just the points for this model.
							// These points will correspond with the mesh points only if polygon merging 
							// has been turned off for the mesh. This is always the case for biomeshes.
							//

							// Load the object. 
							CGroffObject go_object_sub;
							AlwaysVerify(bLoadObject(pgon_sub->sehObjectSection, go_object_sub));
							AlwaysAssert(go_object_sub.sehGeometry);

							// Load the geometry.
							CGroffGeometry gg_geometry_sub;
							AlwaysVerify(bLoadGeometry(go_object_sub.sehGeometry, gg_geometry_sub));

							// Alias an array of CVector3's to gg_geometry_sub's point array.
							CPArray< CVector3<> > pav3_points(gg_geometry_sub.uVertexCount, 
								(CVector3<>*)gg_geometry_sub.afv3Vertex);

							AlwaysAssert(pav3_points.uLen == pmsh_return->pav3Points.uLen);
							AlwaysAssert(pav3_points.uLen < 1024);

							// This substitution set.
							CSubstitute* psub = new CSubstitute();

							TReal r_sub_scale = pgon_sub->fScale / pgon->fScale;
							TReal r_rescale = 1.0 / r_max_coord;

							// Find the non-matching verts, and add them to the substitution.
							for (i = pav3_points.uLen - 1; i >= 0; --i)
							{
								CVertexReplacement vr;

								// Scale to match the main object.
								vr.v3Position = pav3_points[i] * r_sub_scale;
								
								// Apply the offset and scale we did to the main object.
								vr.v3Position += v3_offset;
								vr.v3Position *= r_rescale;

								vr.iIndex = i;

								if (Fuzzy(vr.v3Position) != pmsh_return->pav3Points[i])
								{
									// Save the differing point in the new substitution.
									// Apply the same transformations to this point as we applied to the biomesh.
									psub->lvrVertexReplacements.push_back(vr);
				
									// Mark this vertex as likely to change in the original substituion.
									ai_changing[i] = true;
								}
							}

							// Print out info about the substituition.
							dout << "Mesh: " << pgon_sub->strObjectName << " Substitutes " <<
								psub->lvrVertexReplacements.size() << " vertices.\n";

							// Save the substitution.
							psbmsh_return->MakeRelative(psub, patf3_joints);
							psbmsh_return->lpsubSubstitutes.push_back(psub);
						}
						// And set up the next one.
						++i_sub;
					}

					for (i = 1023; i >= 0; --i)
					{
						if (ai_changing[i])
						{
							// Save the differing point in the new substitution.
							CVertexReplacement vr;
							vr.iIndex = i;
							vr.v3Position = pmsh_return->pav3Points[i];

							psub_original->lvrVertexReplacements.push_back(vr);
						}
					}

					// Save the substitution.
					// Do NOT make original sub relative, as it's derived from already relative points.
					psbmsh_return->lpsubSubstitutes.push_front(psub_original);
				}
				else
				{
					rptr<CBioMesh> pbmsh_return = rptr_new(CMesh::fhGlobalMesh) CBioMesh
					(
						mh_build,
						patf3_joints,
						pau_joint_links,
						pai_double_first,
						pai_double_second,
						paf_double_pos_ratio,
						paf_double_rot_ratio
					);
					MEMLOG_ADD_COUNTER(emlMeshHeap, sizeof(CBioMesh));

					pma_temp = rptr_cast(CMeshAnimating, pbmsh_return);
					pmsh_return = rptr_cast(CMesh, pbmsh_return);
				}

				//
				// Set up texture animation props.
				//
				if (b_animating)
				{
					// Now snag the base animating texture and see if it is present.
					const CEasyString *pestr_texture;
					const CEasyString *pestr_bump;
					bool b_keep_going = false;
					char ac_texture[512];
					char ac_bump[512];

					// Get the first anim texture name.
					bFILL_pEASYSTRING(pestr_texture, esAnim00);

					//  Add all those textures.
					int i_texture_index = 0;
					do
					{
						Assert(i_texture_index < iMAX_ANIMATION_FRAMES);

						//
						// HACK HACK HACK - Get the texture directory.
						//
						extern char* strMipFileCache;
						char *str_start = strMipFileCache;

						AlwaysAssert(str_start);

						strcpy(ac_texture, str_start);
						strcat(ac_texture, pestr_texture->strData());

						// Get the bumpmap, if any.
						if (bFILL_pEASYSTRING(pestr_bump, ESymbol(esAnimB00 + i_texture_index)))
						{
							strcpy(ac_bump, str_start);
							strcat(ac_bump, pestr_bump->strData());
						}
						else
						{
							ac_bump[0] = '\0';
						}

						// Open the material.
						rptr<CTexture> ptex = ptexGetTexture
						(
							ac_texture,
							ac_bump,
							f_bumpiness, 
							pmat,
							pccSettings
						);

						// Take the material and tell the mesh to add it to the cycle list.
						if (ptex)
						{
							pma_temp->AddTexture(ptex);
						}

						// Make sure it worked.
						Assert(ptex);

						// Next texture.
						++i_texture_index;

						if (i_texture_index >= iMAX_ANIMATION_FRAMES)
							b_keep_going = false;
						else
							b_keep_going = bFILL_pEASYSTRING(pestr_texture, ESymbol(esAnim00 + i_texture_index));

					} while (b_keep_going);

					int i_material_no = -1;
					if (bFILL_INT(i_material_no, esAnimSubMaterial))
						pma_temp->SetSurface(i_material_no - 1);
				}
			}

			// Bio-meshes do not have thier mesh data saved.

			// Reset the mesh heap.
			mh_build.Reset();

			extern CProfileStat psMeshConstruction;
			psMeshConstruction.Add(ctmr());

			// Set the default screen size to zero.
			pmsh_return->rMinScreenArea = 0.0f;

			// Set the pivot point of the mesh.
			pmsh_return->v3Pivot = mh_build.v3Pivot;

			// Create a shrink wrap for the mesh if required.
			psMiscLoad.Add(ctmr());

			// Only do wrap stuff if we don't already have a wrap.
			if (pmsh_return->pav3Wrap.uLen == 0)
			{
				bool b_create_wrap = pload->bCreateWrap;
				if (b_create_wrap && pestr_class)
					b_create_wrap = *pestr_class != "CTerrainObj";

				// Override the wrap bool with the text prop setting.
				bFILL_BOOL(b_create_wrap, esWrap);

				// But don't create it for terrain texture objects....
				if (b_create_wrap)
				{
					// Create wrap.
					pmsh_return->CreateWrap();
				}
				else
				{
					// Use the bounding box instead.
					pmsh_return->CreateWrapBox();
				}
			}

			extern CProfileStat psShrinkWrap;
			psShrinkWrap.Add(ctmr());

			// If the object is flat-shaded, make all of its surfaces flat shaded.
			if (!b_texture)
			{
				pmsh_return->MakeNoTexture();
			}

			// Find out if the mesh is lit.
			bool b_unlit = false;
			bFILL_BOOL(b_unlit, esUnlit);

			pmsh_return->bUnlit = b_unlit;

			//
			// Apply alpha stuff. 
			//
			int i_alpha_channel;
			bool b_create_mips = true;		// assume by default that mips are required.

			bFILL_BOOL(b_create_mips, esMipMap);

			psMiscLoad.Add(ctmr());
			if (b_create_mips)
			{
				pmsh_return->CreateMipMaps();
			}
			else
			{
				// Call the create mips function with the minimum mip level being 0 (the top level)
				pmsh_return->CreateMipMaps(0);
			}
			extern CProfileStat psMipMap;
			psMipMap.Add(ctmr());

			// Has the user specified an alpha channel?
			if (bFILL_INT(i_alpha_channel, esAlphaChannel))
			{
				// Yes!  Use it.
				ApplyAlphaColour(*pmsh_return, i_alpha_channel);
			}

			// Get the detail reduced versions, if they exist.
			LoadLowerMeshes(pmsh_return, pgon, pload, pvtable, pgon->fScale);
			pmsh_return->AdjustBoundingVolumeForDetailReducedVersions();

#if VER_DETAIL_REDUCTION_WARNING
			ctmr();

			// Verify that complicated meshes have detail reduced versions.
			if (pmsh_return->pampPolygons.uLen > 128 && !pmsh_return->pshCoarser)			
			{
				dout << " " << pmsh_return->pampPolygons.uLen << " polygons, but no detail reduced version of " << pgon->strObjectName << "\n";
			}

			extern CProfileStat psPrint;
			psPrint.Add(ctmr());
#endif

			// Set the pseudo-radius value for the mesh.
			pmsh_return->fPRadius = f_pseudo_radius;

			return rptr_cast(CRenderType, pmsh_return);
		}
	}
	END_OBJECT;
	END_TEXT_PROCESSING;

	// We have failed somehow to get the needed text props.
	AlwaysAssert(false);
	return rptr0;
}

//******************************************************************************************
//
bool bGetObject
(
	CObjectValue **ppoval_value,			// The value to set.
	const CHandle& h_symbol,			// The symbol to find.
	CObjectValue* poval_parent,	// The object containing the value.
	CValueTable* pvtable,			// The vtable to use.	
	int *pi_best_guess_index			// The best place to start looking for the symbol.
)
//  Returns true if successful, else false.  If false, does not modify "*b"
{
	if (!poval_parent)
		return false;


	CBaseValue* pbv = pbvLookupValue(h_symbol, poval_parent, pvtable, *pi_best_guess_index);

	if (pbv && pbv->evtType() == evtOBJECT)
	{
		// Found the symbol!

		// Stuff the return value.
		// We are expecting a bool.
		*ppoval_value = povalCast(pbv);

		// Look at the next property next time.
		(*pi_best_guess_index)++;

		// Let the caller know that we have succeeded.
		return true;
	}
	else
		return false;
}
