#include "StandardTypes.hpp"
#include "ObjectDef.hpp"

//**********************************************************************************************
// Must undefine GetMessage as it gets defined inside of the Max.h include hierarchy, most 
// likely it is a windows entry point.
//

#undef GetMessage

#include "Lib/Sys/SysLog.hpp"
#include "Lib/Groff/FileIO.hpp"
#include "Lib/Groff/Groff.hpp"
#include "Tools/GroffExp/Export.hpp"
#include "Tools/GroffExp/GUIInterface.hpp"

extern CGUIInterface guiInterface;


bool CGroffExport::bWriteToSection(CObjectDef* pod_node, TSectionHandle seh_handle, void* data, uint u_size, const char* str_error_message)
{
	int i_actual_size;

	// Write out the file header.
	i_actual_size = fioFile.iWrite(seh_handle, data, u_size);

	if (i_actual_size != (int) u_size)
	{
		// Something went wrong so report the error and return an error.
		slLogfile.Printf("%s\n\nbWriteToSection: File IO error message: %s\n", str_error_message, fioFile.GetMessage());
		slLogfile.Printf("bWriteToSection: Node address: 0x%X, Handle: 0x%X, Buffer: 0x%X, Requested size %d, Actual size: %d\n", 
			pod_node, seh_handle, data, u_size, i_actual_size);
		
		// Do we have a valid object node?
		if (pod_node != 0)
		{
			slLogfile.Printf("bWriteToSection: Node ID %d, Object name: %s\n", pod_node->iID, pod_node->estrObjectName.strData());
		}

		// Write out the file header.
		fioFile.iWrite(seh_handle, data, u_size);

		// Return an error.
		return false;
	}

	// Return a successful result.
	return true;
}


bool CGroffExport::bSaveMaterial(CObjectDef* pod_node, TSectionHandle& seh_handle)
{
	// Is there a material definition?
	if (pod_node->uMaterialCount == 0)
	{
		// No! Return a zero handle and a successful result.
		seh_handle = 0;

		return true;
	}

	// Attempt to setup a material object.
	CGroffMaterial gmat_material;
	
	gmat_material.uTextureCount = pod_node->uMaterialCount;

	// Attempt to configure this object.
	if (!gmat_material.bConfigure())
	{
		// Something went wrong so report the error and return an error.
		slLogfile.Printf("bSaveMaterial: Unable to configure material object for object:%s:\n", pod_node->estrObjectName.strData());
	}

	// Attempt to create an object section.
	seh_handle = fioFile.sehCreate(".material", gMATERIAL);

	// Were we successful?
	if (seh_handle == 0)
	{
		// Something went wrong so report the error and return an error.
		slLogfile.Printf("bSaveMaterial: Unable to create material section for :%s:\n", pod_node->estrObjectName.strData());

		// Return an error.
		return false;
	}

	// Loop through all the materials and convert them from symbols to handles.
	for (uint u_i = 0; u_i < pod_node->uMaterialCount; u_i++)
	{
		// Are there any texture maps?
		if (pod_node->astrTextureMap != 0)
		{
			// First attempt to add the name to the symbol table.
			if (pod_node->astrTextureMap[u_i] != 0)
			{
				gmat_material.asyhTextureHandle[u_i] = fioFile.syhInsert(pod_node->astrTextureMap[u_i]);

				// Were we successful?
				if (gmat_material.asyhTextureHandle[u_i] == 0)
				{
					// Something went wrong so report the error and return an error.
					slLogfile.Printf("bSaveMaterial: Unable to register symbol :%s: to symbol table.\n", pod_node->astrTextureMap[u_i]);

					// No! Return an error.
					return false;
				}
			}
			else
			{
				// Set the symbol handle to 0.
				gmat_material.asyhTextureHandle[u_i] = 0;
			}
		}

		// Are there any opacity maps?
		if (pod_node->astrOpacityMap != 0)
		{
			// First attempt to add the name to the symbol table.
			if (pod_node->astrOpacityMap[u_i] != 0)
			{
				gmat_material.asyhOpacityHandle[u_i] = fioFile.syhInsert(pod_node->astrOpacityMap[u_i]);

				// Were we successful?
				if (gmat_material.asyhOpacityHandle[u_i] == 0)
				{
					// Something went wrong so report the error and return an error.
					slLogfile.Printf("bSaveMaterial: Unable to register symbol :%s: to symbol table.\n", pod_node->astrOpacityMap[u_i]);

					// No! Return an error.
					return false;
				}
			}
			else
			{
				// Set the symbol handle to 0.
				gmat_material.asyhOpacityHandle[u_i] = 0;
			}
		}

		// Are there any bump maps?
		if (pod_node->astrBumpMap != 0)
		{
			// First attempt to add the name to the symbol table.
			if (pod_node->astrBumpMap[u_i] != 0)
			{
				gmat_material.asyhBumpHandle[u_i] = fioFile.syhInsert(pod_node->astrBumpMap[u_i]);

				// Were we successful?
				if (gmat_material.asyhBumpHandle[u_i] == 0)
				{
					// Something went wrong so report the error and return an error.
					slLogfile.Printf("bSaveMaterial: Unable to register symbol :%s: to symbol table.\n", pod_node->astrBumpMap[u_i]);

					// No! Return an error.
					return false;
				}
			}
			else
			{
				// Set the symbol handle to 0.
				gmat_material.asyhBumpHandle[u_i] = 0;
			}
		}
	}

	// Attempt to write the material definition to the image.
	if (!bWriteToSection(pod_node, seh_handle, &gmat_material.uTextureCount, sizeof(uint),
		"bSaveMaterial: Unable to write the material count."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, gmat_material.asyhTextureHandle, sizeof(uint) * gmat_material.uTextureCount,
		"bSaveMaterial: Unable to write the texture map handle list."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, gmat_material.asyhOpacityHandle, sizeof(uint) * gmat_material.uTextureCount,
		"bSaveMaterial: Unable to write the opacity map handle list."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, gmat_material.asyhBumpHandle, sizeof(uint) * gmat_material.uTextureCount,
		"bSaveMaterial: Unable to write the bumpmap handle list."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &gmat_material.fAmbient, sizeof(float),
		"bSaveMaterial: Unable to write the ambient lighting term."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &gmat_material.fDiffuse, sizeof(float),
		"bSaveMaterial: Unable to write the diffuse lighting term."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &gmat_material.fSpecular, sizeof(float),
		"bSaveMaterial: Unable to write the specular lighting term."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &gmat_material.fOpacity, sizeof(float),
		"bSaveMaterial: Unable to write the opacity value."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &gmat_material.fBumpiness, sizeof(float),
		"bSaveMaterial: Unable to write the bumpiness value."))
		return false;

	// Return a successful result.
	return true;
}


bool CGroffExport::bSaveMapping(CObjectDef* pod_node, TSectionHandle& seh_handle)
{
	// Is there a mapping definition?
	if (pod_node->uTextureFaceCount == 0 && pod_node->uTextureVertexCount == 0)
	{
		// No! Return a zero handle and a successful result.
		seh_handle = 0;

		return true;
	}

	CGroffMapping gmap_mapping;

	// Attempt to create an object section.
	seh_handle = fioFile.sehCreate(".mapping", gMAPPING);

	// Were we successful?
	if (seh_handle == 0)
	{
		// Something went wrong so report the error and return an error.
		slLogfile.Printf("bSaveMapping: Unable to create mapping section for :%s:\n", pod_node->estrObjectName.strData());

		// Return an error.
		return false;
	}

	// Attempt to add the material section. Were we successful?
	if (!bSaveMaterial(pod_node, gmap_mapping.sehMaterial))
	{
		// No! Return an error;
		return false;
	}

	// Attempt to write the class info to the image.
	if (!bWriteToSection(pod_node, seh_handle, &gmap_mapping.sehMaterial, sizeof(TSectionHandle),
		"bSaveMapping: Unable to write the material handle."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &pod_node->uTextureVertexCount, sizeof(uint),
		"bSaveMapping: Unable to write the texture vertex count."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &pod_node->uTextureFaceCount, sizeof(uint),
		"bSaveMapping: Unable to write the texture face count."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, pod_node->afv2TextureVertex, sizeof(fvector2) * pod_node->uTextureVertexCount,
		"bSaveMapping: Unable to write the texture vertices."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, pod_node->auv3TextureFace, sizeof(uvector3) * pod_node->uTextureFaceCount,
		"bSaveMapping: Unable to write the texture faces."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, pod_node->auFaceMaterialIndex, sizeof(uint) * pod_node->uTextureFaceCount,
		"bSaveMapping: Unable to write the face material indices."))
		return false;

	// Return a successful result.
	return true;
}


bool CGroffExport::bSaveGeometry(CObjectDef* pod_node, TSectionHandle& seh_handle)
{
	// Is there a valid geometry definition?
	if (pod_node->uVertexCount == 0 && pod_node->uFaceCount == 0 && pod_node->uVertexNormalCount == 0)
	{
		// No!  Return a zero handle and a successful result.
		seh_handle = 0;

		return true;
	}

	CGroffGeometry gg_geometry;

	// Attempt to create an object section.
	seh_handle = fioFile.sehCreate(".geometry", gGEOMETRY);

	// Were we successful?
	if (seh_handle == 0)
	{
		// Something went wrong so report the error and return an error.
		slLogfile.Printf("bSaveGeometry: Unable to create geometry section for :%s:\n", pod_node->estrObjectName.strData());

		// No!  Return an error;
		return false;
	}
	
	// Attempt to write the data to the image.
	if (!bWriteToSection(pod_node, seh_handle, &pod_node->uVertexCount, sizeof(uint),
		"bSaveGeometry: Unable to write vertex count."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &pod_node->uFaceCount, sizeof(uint), 
		"bSaveGeometry: Unable to write face count."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &pod_node->uVertexNormalCount, sizeof(uint),
		"bSaveGeometry: Unable to write vertex normal count."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &pod_node->uv3DefaultColor.X, sizeof(uvector3),
		"bSaveGeometry: Unable to write the default color."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, pod_node->afv3Vertex, sizeof(fvector3) * pod_node->uVertexCount,
		"bSaveGeometry: Unable to write vertices."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, pod_node->auv3Face, sizeof(uvector3) * pod_node->uFaceCount,
		"bSaveGeometry: Unable to write faces."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, pod_node->afv3FaceNormal, sizeof(fvector3) * pod_node->uFaceCount,
		"bSaveGeometry: Unable to write face normals."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, pod_node->afv3VertexNormal, sizeof(fvector3) * pod_node->uVertexNormalCount,
		"bSaveGeometry: Unable to write vertex normals."))
		return false;


	// Return a successful result.
	return true;
}


bool CGroffExport::bSaveObject(CObjectDef* pod_node, TSectionHandle& seh_handle, TSymbolHandle& syh_name)
{
	CGroffObject go_object;

	// First attempt to add the object name to the symbol table.
	go_object.syhObjectName = fioFile.syhInsert(pod_node->estrObjectName.strData());

	// Were we successful?
	if (go_object.syhObjectName == 0)
	{
		// Something went wrong so report the error and return an error.
		slLogfile.Printf("bSaveObject: Unable to register symbol :%s: to symbol table.\n", pod_node->estrObjectName.strData());

		// No! Return an error.
		return false;
	}

	// Return the symbol handle for convenience.
	syh_name = go_object.syhObjectName;

	// Attempt to create an object section.
	seh_handle = fioFile.sehCreate(pod_node->estrObjectName.strData(), gOBJECT);

	// Were we successful?
	if (seh_handle == 0)
	{
		// Something went wrong so report the error and return an error.
		slLogfile.Printf("bSaveObject: Unable to create object section for :%s:\n", pod_node->estrObjectName.strData());

		// No!  Return an error;
		return false;
	}
	
	// Attempt to add a geometry section to the file.
	if (!bSaveGeometry(pod_node, go_object.sehGeometry))
	{
		slLogfile.Printf("bSaveObject: Unable to construct geometry section for :%s:\n", pod_node->estrObjectName.strData());

		// No!  Return an error.
		return false;
	}

	// Yes!  Then attempt to write this information to the file image.  Were we succesful?
	if (!bSaveMapping(pod_node, go_object.sehMapping))
	{
		slLogfile.Printf("bSaveObject: Unable to construct mapping section for :%s:\n", pod_node->estrObjectName.strData());

		// Return an error.
		return false;
	}

	//
	// Add code to write physics, AI, sound, trigger, design daemon and special information.
	//

	// Attempt to write the class info to the image.
	if (!bWriteToSection(pod_node, seh_handle, &go_object.syhObjectName, sizeof(TSymbolHandle),
		"bSaveObject: Unable to write object symbol handle."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &go_object.sehGeometry, sizeof(TSectionHandle),
		"bSaveObject: Unable to write geometry section handle."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &go_object.sehMapping, sizeof(TSectionHandle),
		"bSaveObject: Unable to write mapping section handle."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &go_object.sehPhysics, sizeof(TSectionHandle),
		"bSaveObject: Unable to write physics section handle."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &go_object.sehAI, sizeof(TSectionHandle),
		"bSaveObject: Unable to write AI section handle."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &go_object.sehSound, sizeof(TSectionHandle),
		"bSaveObject: Unable to write sound section handle."))
		return false;

	if (!bWriteToSection(pod_node, seh_handle, &go_object.sehSpecial, sizeof(TSectionHandle),
		"bSaveObject: Unable to write special section handle."))
		return false;


	// Return a successful result.
	return true;
}


bool CGroffExport::bSaveRegion(CObjectDefList* podl_scene, TSectionHandle& seh_region)
{
	// Allocate the region section and fill it in as we go along.
	CGroffRegion gr_region(podl_scene->uObjectCount());

	// Were we successful?
	if (gr_region.agocObjectList == 0)
	{

		// Something went wrong so flush the image and return an error.
		slLogfile.Printf("bSaveRegion: Unable to build region with %d objects.\n", podl_scene->uObjectCount()); 

		// No!  Return an error.
		return false;
	}

	// Create a section for the region descriptor.
	seh_region = fioFile.sehCreate(".region", gREGION);

	// Were we successful?
	if (seh_region == 0)
	{
		// No!  Return an error;
		return false;
	}

	// Setup a pointer to the object at the from of the list.
	CObjectDef* pod_node = podl_scene->podHead;	

	// Process the scene list one object at a time.
	uint u_object_index = 0;
	while (pod_node != 0)
	{	
		// Setup the positional information.
		CGroffObjectConfig& goc_config = gr_region.agocObjectList[u_object_index];

		// Convert the object list format to representation format.
		goc_config.fScale = pod_node->fGetScale();
		
		// Convert the position.
		Point3 p3_pos = pod_node->p3GetPosition();
		fvector3 fv3_pos;
		fv3_pos.X = p3_pos[0];
		fv3_pos.Y = p3_pos[1];
		fv3_pos.Z = p3_pos[2];
		goc_config.fv3Position = fv3_pos;

		// Convert the rotation.
		Point3 p3_rot = pod_node->p3GetRotation();
		fvector3 fv3_rot;
		fv3_rot.X = p3_rot[0];
		fv3_rot.Y = p3_rot[1];
		fv3_rot.Z = p3_rot[2];
		goc_config.fv3Rotation = fv3_rot;

		// Attempt to add the objects to the scene.
		if (!bSaveObject(pod_node, goc_config.sehObject, goc_config.syhObjectName))
		{

			// Something went wrong so flush the image and return an error.
			slLogfile.Printf("bAddRegion: Unable to add object :%s: to region.\n", pod_node->estrObjectName.strData());

			// Return an error.
			return false;
		}

		// Advance to the next node in the list.
		pod_node = pod_node->podNext;

		// Increment the index pointer.
		u_object_index++;
	};

	// Attempt to write the object to the image.
	if (!bWriteToSection(pod_node, seh_region, &gr_region.uObjectCount, sizeof(uint),
		"bSaveRegion: Unable to write object count."))
		return false;

	if (!bWriteToSection(pod_node, seh_region, gr_region.agocObjectList, sizeof(CGroffObjectConfig) * gr_region.uObjectCount,
		"bSaveRegion: Unable to write object handles."))
		return false;

	// Return a successful result.
	return true;
}


bool CGroffExport::bSaveHeader()
{
	// Write out the file header.
	CGroffHeader gh_header;

	// Attempt to create the section header.
	TSectionHandle seh_handle = fioFile.sehCreate(".header", gHEADER);

	// Were we successful?
	if (seh_handle == 0)
	{
		// Something went wrong so flush the image and return an error.
		slLogfile.Printf("bExportScene: Unable to create file header section.\n");
		// No!  Return an error.
		return false;
	}

	// Write out the file header.
	if (!bWriteToSection(0, seh_handle, &gh_header, sizeof(CGroffHeader), "bAddHeader: Unable to write file header."))
	{
		// Return an error.
		return false;
	}

	// Return a successful result.
	return true;
}


bool CGroffExport::bSaveScene(CObjectDefList* podl_scene, const char* str_export_filename)
{
	// Determine whether the file should be active or not.
	if (guiInterface.bGenerateLogfiles())
	{
		// Open the log file.
		char str_logfile[256];

		// Construct the proper path for the logfile.
		guiInterface.BuildPath(str_logfile, guiInterface.strGetLogfileDirPath(), "GroffGen.log");
		slLogfile.Open(str_logfile);

		// Activate the logfile.
		slLogfile.Enable();
	}
	else
	{
		// Deactivate the logfile.
		slLogfile.Disable();
	}

	// Place a startup message in the logfile.
	slLogfile.Printf("***** Generating GROFF Image: %s *****\n", str_export_filename);

	// Open the Groff output file.  Were we successful?
	if (!fioFile.bOpen(str_export_filename, eWrite))
	{
		// We were not able to open the file.  Return an error.
		return false;
	}

	// Attempt to generate the object file header.
	if (!bSaveHeader())
	{
		// Return an error.
		return false;
	}

	TSectionHandle sh_region;

	// Attempt to generate a region file.  Were we successful?
	if (!bSaveRegion(podl_scene, sh_region))
	{
		// No!  Return an error.
		return false;
	}

	// Write the image to disk.
	fioFile.bWriteImage();

	// Close the export file.
	fioFile.bClose();

	// Report that the file export is complete.
	slLogfile.Printf("\n***** GROFF File generation complete. *****\n");

	// Close the logfile.
	slLogfile.Close();

	// Return a successful result
	return true;
}
