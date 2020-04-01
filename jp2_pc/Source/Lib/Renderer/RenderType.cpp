/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of 'RenderType.hpp.'
 *
 * Bugs:
 *		
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderType.cpp                                          $
 * 
 * 13    98.09.08 2:35p Mmouni
 * Made changes to support creation of non CMesh objects for invisible geometry.
 * 
 * 12    98.09.07 5:25p Mmouni
 * Changed pmshLoadObject to prdtLoadObject and changed instancing to use CRenderType instead
 * of CMesh.
 * 
 * 
 * 11    7/21/98 8:59p Agrant
 * allow less verbose loading
 * 
 * 10    98.05.19 5:16p Mmouni
 * Added profile stat for object name printing.
 * 
 * 9     5/14/98 8:06p Agrant
 * Removed the defunct poval_renderer argument form  rendertype constructor
 * 
 * 8     98/02/26 15:38 Speter
 * Removed Hack.hpp.
 * 
 * 7     98/02/26 13:54 Speter
 * Removed obsolete hack-based functions, which used deleted SSurfaceData type.
 * 
 * 6     1/26/98 11:19a Agrant
 * Ported an enum from hack.hpp
 * 
 * 5     11/07/97 6:18p Agrant
 * Removed dead code.
 * 
 * 4     10/28/97 7:18p Agrant
 * Started moving functionality for mesh loading into CGroffIO class
 * 
 * 3     10/24/97 7:30p Agrant
 * Began prop based mesh loader,
 * used version switch on GROFF file for backwards compatability
 * 
 * 2     10/24/97 11:29a Agrant
 * Now it actually compiles!
 * 
 * 1     10/24/97 10:52a Agrant
 * initial revision
 * 
 *********************************************************************************************/

#include "Common.hpp"
#include "Lib/Renderer/RenderType.hpp"

#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/LoadTexture.hpp"

#include "Lib\Renderer\LightBlend.hpp"

#include "Lib/Groff/VTParse.hpp"

#include "Lib/Std/Hash.hpp"

#include "Lib/Sys/DebugConsole.hpp"

#include "Lib\Sys\Profile.hpp"


extern std::map<uint32, SMeshInstance, std::less<uint32> > mapMeshInstances;
extern uint g_u_Version_Number;


// Stolen from the old hack.cpp.
enum EMATVType
{
	eMAT_BEGIN,
	eMAT_DEFAULT=eMAT_BEGIN,
	eMAT_METAL,
	eMAT_WATER,
	eMAT_PRELIT,
	eMAT_SHINY,
	eMAT_END
};

//*********************************************************************************************
//
// CRenderType function implementations.
//

	//*****************************************************************************************
	CVector3<> CRenderType::v3GetPhysicsBox() const
	{
		return CVector3<>(0.0f, 0.0f, 0.0f);
	}

	//*****************************************************************************************
	CVector3<> CRenderType::v3GetPivot() const
	{
		return CVector3<>(0.0f, 0.0f, 0.0f);
	}

	//*********************************************************************************************
	const rptr<CRenderType> CRenderType::prdtFindShared
	(
		const CGroffObjectName*	pgon,				// Pointer to GROFF name.
		CLoadWorld*				pload,				// Pointer to loader.
		const CHandle&			h_obj,				// Handle to the base object in the value table.
		CValueTable*			pvtable				// Pointer to the value table.
	)
	{
		// Prepare instancing.
		rptr<CRenderType> prdt_object;	// Pointer to the mesh being loaded.
		char str_object_name[256];	// Object's name.
		uint32 u4_hash;				// Object's hash value.

		// Cache the object name for error reporting.
		SetCacheText(pgon->strObjectName);

		// Create a string for hashing.
		strcpy(str_object_name, pgon->strObjectName);

		// Replace '-' character with a null (end of string) character.
		for (int i_char = 0; i_char < strlen(str_object_name); i_char++)
		{
			if (str_object_name[i_char] == '-')
			{
				str_object_name[i_char] = 0;
				break;
			}
			Assert(i_char < 256);
		}

		// Get the hash value for the object.
		u4_hash = u4Hash((const void*)str_object_name, strlen(str_object_name),	true);
		CPresence3<> pr3_adjust;

		SMeshInstance mshi = mapMeshInstances[u4_hash];

		if (mshi.prdtMesh)
		{
			// Tell the log the object was instanced.
			conLoadLog.Print("%s (Instanced).\n", str_object_name);

			pr3_adjust = mshi.pr3Adjust;

			// Return with an instanced version of the object.
			return mshi.prdtMesh;
		}

		//
		// Load the object for the first time.
		//

		CCycleTimer ctmr;

		// Tell the log the object was loaded for the first time..
		conLoadLog.Print("%s (Loaded).\n", str_object_name);
		if (CLoadWorld::bVerbose)
		{
			dout << "Loading " << str_object_name << '\n';
		}

		extern CProfileStat psPrint;
		psPrint.Add(ctmr());

		// Now actually load the mesh.
		prdt_object = pload->gioFile.prdtLoadObject(pgon, pload, h_obj, pvtable);
		
		// Report an error if mesh loading was not successful, and return a null pointer.
		if (!prdt_object)
		{
			TerminalError(ERROR_UNABLETOLOAD_MESH, true, pgon->strObjectName);
			return prdt_object;
		}

		// Save the mesh for future instancing.
		mapMeshInstances[u4_hash] = SMeshInstance(prdt_object, pr3_adjust);

		return prdt_object;
	}

