/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Camera and pipeline objects for the GUI.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GUIPipeLine.hpp                                               $
 * 
 * 34    9/08/98 3:15p Mlange
 * Implemented rectangle select.
 * 
 * 33    98/03/06 10:55 Speter
 * Moved bones display to CPhysicsSystem.
 * 
 * 32    2/05/98 12:11p Agrant
 * added the UnselectAll function.
 * 
 * 31    98/02/04 15:00 Speter
 * Changed lspinsSelected to lsppartSelected, to allow selection of magnets and other
 * partitions.
 * 
 * 30    9/04/97 8:18p Pkeet
 * Added the static handles flag.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUINTERFACE_GUIPIPELINE_HPP
#define HEADER_GUINTERFACE_GUIPIPELINE_HPP

#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/GeomDBase/Shape.hpp"


//
// Class definitions.
//

typedef std::list<CPartition*> TSelectedList;

//*********************************************************************************************
//
class CPipeLine
//
// The GUI world database, light database and pipeline.
//
// Prefix: pipe
//
//**************************************
{
public:

	TSelectedList	lsppartSelected;		// List of shapes currently selected.

public:

	//*****************************************************************************************
	//
	void Init
	(
	);
	//
	// Initialize the GUI pipeline and databases for first time use. Adds an ambient light.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Paint
	(
	);
	//
	// Render the current scene.
	//
	//**************************************

	//*****************************************************************************************
	//
	void MarkObjects
	(
		rptr<CRasterWin> pras,				// Screen raster.
		bool b_draw_crosshairs,				// Whether to draw cross-hairs at centre.
		bool b_draw_spheres,				// Whether to draw bounding spheres.
		bool b_draw_wire,					// Whether to draw wireframe.
		bool b_draw_pins					// Whether to draw vertex normals.
	);
	//
	// Draw requested markings on object.
	//
	//**************************************

	//*********************************************************************************************
	//
	bool bSelected
	(
		CPartition* ppart
	);
	//
	// Returns:
	//		Whether ppart is currently selected.
	//
	//**********************************

	//*********************************************************************************************
	//
	int iSelectedCount() const;
	//
	// Returns:
	//		How many objects selected.
	//
	//**********************************

	//*********************************************************************************************
	CPartition* ppartLastSelected
	(
		int i_index = 0					// Which object to get from the list.
										// 0 means last selected, 1 means next last, etc.
	);
	//
	// Returns:
	//		The asked-for instance, or 0 if it existsn't.
	//
	//**********************************

	//*********************************************************************************************
	//
	void Select
	(
		CPartition* ppart,				// Object to select.
		bool b_augment = false			// Adds it to list; otherwise, selects exclusively.
	);
	//
	// Makes ppart selected.
	//
	//**********************************

	//*****************************************************************************************
	//
	void Select
	(
		const CPoint& pnt_a,
		const CPoint& pnt_b
	);
	//
	// Select a shape based on a dragging rectangle. Always adds selected objects to the list.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bSelect
	(
		int i_x, int i_y,					// Position of mouse cursor on screen.
		bool b_augment = false				// As above.
	);
	//
	// Select a shape based on the position of the mouse.
	//
	// Returns 'true' if a shape is found near the mouse cursor.
	//
	//**************************************

	//*********************************************************************************************
	//
	void UnselectAll
	(
	);
	//
	// Unselects all selected objects.
	//
	//**********************************

	//*****************************************************************************************
	//
	int iDeleteSelected
	(
	);
	//
	// Removes the selected shapes from the world.
	//
	// Returns:
	//		How many removed.
	//
	//**************************************

	//*****************************************************************************************
	//
	rptr<CLightAmbient> pltaGetAmbientLight
	(
	);
	//
	// Returns a pointer to the ambient light in the scene.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddObject
	(
		CInstance* pins	// Shape to add.
	);
	//
	// Adds a shape to the root presence.
	//
	//**************************************

	//*****************************************************************************************
	//
	CInstance* pinsGetLight
	(
		CInstance* pins
	);
	//
	// Returns a pointer to any light associated with a shape. Returns zero if no light can be
	// found.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bGetCentreofSelectedObject
	(
		int& i_x,	// Horizontal position of object's centre on screen.
		int& i_y	// Vertical position of object's centre on screen.
	);
	//
	// Places the screen coordinates of the selected object in i_x and i_y.
	//
	// Returns 'true' if an object is selected.
	//
	//**************************************

};


//
// Global functions.
//

//*********************************************************************************************
//
CCamera* pcamGetCamera
(
);
//
// Returns a pointer to the primary camera.
//
//**************************************

//*********************************************************************************************
//
void AddInView
(
	CInstance *pins
);
//
// Adds the instance to the world database.
//
//**************************************


//
// External variables.
//

// The main graphics pipeline.
extern CPipeLine  pipeMain;

// Flag to draw partitions.
extern bool bDrawPartitions;

// Flag to draw static handles.
extern bool bStaticHandles;


#endif
