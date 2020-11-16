/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of "DialogObject.h."
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogObject.cpp                                              $
 * 
 * 30    7/20/98 10:12p Rwyatt
 * Remove description text in final mode
 * 
 * 29    7/03/98 10:46p Agrant
 * expand description buffer
 * 
 * 28    98/05/18 19:43 Speter
 * Changed GetMaterialType to smatGetMaterialType.
 * 
 * 27    5/01/98 4:42p Rwyatt
 * Added mass to the dialog
 * 
 * 26    4/24/98 5:44p Rwyatt
 * Added sound material hash value to dialog
 * 
 * 25    3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 24    3/13/98 4:36p Agrant
 * added a distance stat to the object properties dlg
 * 
 * 23    1/26/98 11:27a Agrant
 * Show box sizes in world units, rather than normalized units.
 * 
 * 22    12/14/97 11:32a Agrant
 * Description field in object props dlg
 * 
 * 21    97/10/30 15:14 Speter
 * CPhysicsInfo::pbvGetBoundVol() now returns const CBoundVol*.
 * 
 * 20    97/09/29 16:33 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.  
 * 
 * 19    97/09/26 16:47 Speter
 * Separated PhysicsInfo.hpp into additional InfoBox.hpp and InfoSkeleton.hpp.
 * 
 * 18    97/07/09 16:37 Speter
 * Removed all CPointIterator stuff.
 * 
 * 17    6/02/97 4:50p Rwyatt
 * Will noshow the sound Properties for an object if ID is in the current sound database. If
 * the ID is not in the current databse then the ID in hex form is shown.
 * 
 * 16    5/29/97 4:09p Rwyatt
 * Sound materials are now hashed into binary form s cannot be displayed. I need to search in
 * the material database for the hashed number and retrieve its textual name.
 * 
 * 15    97/05/21 18:11 Speter
 * Dialog now shows both Face and Triangle count.
 * 
 * 14    97/05/13 13:44 Speter
 * Changed iNumTriangles() to iNumPolygons().
 * 
 * 13    97-05-09 13:47 Speter
 * Converted typeid().name() to strTypeName() calls.
 * 
 * 12    97-05-05 11:52 Speter
 * Changed display of all class info to use typeid().name() function.
 * 
 * 11    97-05-01 18:28 Speter
 * Added, then commented out, some test code.
 * 
 * 10    4/16/97 2:04p Agrant
 * Defaults for bounding box display for non-box physics infos.
 * 
 * 9     97-04-14 12:11 Speter
 * Changed access of removed CBoundVolBox::v3Pos to v3GetOrigin().
 * Commented out CBoundVolCompound.
 * 
 * 8     97/03/19 14:53 Speter
 * Added #Points to object dialog.
 * 
 * 7     3/11/97 3:19p Blee
 * Revamped trigger system.
 * 
 * 6     97/01/17 17:32 Speter
 * Updated for moved iNumPoints() and iNumVertices() functions.
 * 
 * 5     97/01/07 12:10 Speter
 * Made all CRenderTypes use rptr<>.
 * 
 * 4     96/12/15 18:12 Speter
 * Updated for new iterator, added i_num_points variable.
 * 
 * 3     12/13/96 6:06p Pkeet
 * Cleaned things up a bit.
 * 
 *********************************************************************************************/


//
// Includes.
//

#include "StdAfx.h"
#include "DialogObject.h"

#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/EntityDBase/FilterIterator.hpp"
#include "GUIPipeLine.hpp"
#include "LightProperties.hpp"
#include "GUITools.hpp"
#include "GUIApp.h"

#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/EntityDBase/Animate.hpp"
#include "Lib/EntityDBase/Animal.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib/Renderer/RenderType.hpp"
#include "Lib/GeomDBase/Shape.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/Trigger/Trigger.hpp"
#include "Lib/Std/StringEx.hpp"
#include "Lib/Audio/Material.hpp"
#include "Lib/Physics/InfoBox.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//*********************************************************************************************
CDialogObject::CDialogObject(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogObject::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogObject)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pinsObject = 0;
}

//*********************************************************************************************
void CDialogObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogObject)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogObject, CDialog)
	//{{AFX_MSG_MAP(CDialogObject)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************************
void CDialogObject::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	DisplayText();
}

//*****************************************************************************************
void CDialogObject::SetInstance(CInstance* pins)
{
	Assert(pins);

	pinsObject = pins;
}

//*********************************************************************************************
//
void CDialogObject::DisplayText
(
)
//
// Sets the text in static controls to reflect object parameters.
//
//**************************************
{
	Assert(pinsObject);

	// Display the name of the object.
	SetDlgItemText(IDC_STATIC_NAME, pinsObject->strGetInstanceName() );

	// Display type information about the object.
	DisplayType();

	//
	// Gather information about the object.
	//

	// Default values.
	uint u_num_faces    = 0;	// Number of faces in the mesh.
	uint u_num_tris		= 0;
	uint u_num_vertices = 0;	// Number of vertices in the mesh.
	uint u_num_points   = 0;	// Number of points in the mesh.

	// Get mesh information.
	rptr_const<CRenderType> prdt = pinsObject->prdtGetRenderInfo();
	if (prdt)
	{
		rptr_const<CShape> psh = rptr_const_dynamic_cast(CShape, prdt);

		if (psh)
		{
			u_num_faces		= psh->iNumPolygons();
			u_num_tris		= psh->iNumTriangles();
			u_num_vertices	= psh->iNumVertices();
			u_num_points	= psh->iNumPoints();
		}
	}

	//
	// Display information about the object.
	//
	SetDlgItemInt(IDC_STATIC_FACES,		u_num_faces);
	SetDlgItemInt(IDC_STATIC_TRIANGLES,	u_num_tris);
	SetDlgItemInt(IDC_STATIC_VERTICES,	u_num_vertices);
	SetDlgItemInt(IDC_STATIC_POINTS,	u_num_points);

#if VER_TEST
	char pc_buffer[1024];
	pinsObject->iGetDescription(pc_buffer, 1024);
	SetDlgItemText(IDC_STATIC_DESCRIPTION,	pc_buffer);
#else
	SetDlgItemText(IDC_STATIC_DESCRIPTION,	"No description");
#endif

	//
	// Get and display the physics information associated with the object.
	//
	const CPhysicsInfo* pphi = pinsObject->pphiGetPhysicsInfo();
	if (pphi)
		DisplayPhysicsInfo(pphi);

	// Display presence info.
	DisplayPresenceInfo(pinsObject->pr3Presence());
}

//*********************************************************************************************
//
void CDialogObject::DisplayType
(
)
//
// Sets the text in static controls to reflect object parameters.
//
//**************************************
{
	Assert(pinsObject);

	// Display type name data string.
	SetDlgItemText(IDC_STATIC_TYPE, strTypeName(*pinsObject));
}

//*********************************************************************************************
//
void CDialogObject::DisplayPhysicsBoundingVolume
(
	const CBoundVol* pbv
)
//
// Displays the physics bounding volume information about the object.
//
//**************************************
{
	Assert(pbv);
	SetDlgItemText(IDC_STATIC_PHYSICS_BV, strTypeName(*pbv));
}

//*********************************************************************************************
//
void CDialogObject::DisplayPhysicsBoundingBox
(
	const CBoundVolBox* pbvb
)
//
// Displays the physics bounding box information about the object.
//
//**************************************
{
	CVector3<> v3_dim(0,0,0);	// Dimensions.
	CVector3<> v3_pos(0,0,0);	// Position.

	// Change values from the default if it is a box bounding volume.
	if (pbvb)
	{
		v3_pos = CVector3<>(0, 0, 0);
		v3_dim = (*pbvb)[0];
		v3_dim = v3_dim * 2.0f;
	}




	// Write dimensions.
	SetDlgItemFloat(this, IDC_PHYSICS_BV_DIM_X, v3_dim.tX * pinsObject->fGetScale());
	SetDlgItemFloat(this, IDC_PHYSICS_BV_DIM_Y, v3_dim.tY * pinsObject->fGetScale());
	SetDlgItemFloat(this, IDC_PHYSICS_BV_DIM_Z, v3_dim.tZ * pinsObject->fGetScale());

	// Write positions.
	SetDlgItemFloat(this, IDC_PHYSICS_BV_POS_X, v3_pos.tX);
	SetDlgItemFloat(this, IDC_PHYSICS_BV_POS_Y, v3_pos.tY);
	SetDlgItemFloat(this, IDC_PHYSICS_BV_POS_Z, v3_pos.tZ);
}

//*********************************************************************************************
//
void CDialogObject::DisplayPhysicsInfo
(
	const CPhysicsInfo* pphi
)
//
// Displays the physics information about the object.
//
//**************************************
{
	Assert(pphi);

	char str_flags[256];

	// Display the object type.
	SetDlgItemText(IDC_STATIC_PHYSICS_TYPE, strTypeName(*pphi));

	// Display information about the bounding volume.
	const CBoundVol* pbv = pphi->pbvGetBoundVol();
	if (pbv)
	{
		DisplayPhysicsBoundingVolume(pbv);
		DisplayPhysicsBoundingBox(pbv->pbvbCast());
	}

	// Display the sound material type.
	TSoundMaterial tmat = pphi->smatGetMaterialType();

	wsprintf(str_flags,"%x", tmat);
	// the sound material is in binary form so we need to search the material database..
	SetDlgItemText(IDC_PHYSICS_SOUND_MATERIAL,str_flags);

	*str_flags = 0;
	// Display the physics flags.
	CSet<EPhysicsFlags> epf = ((CPhysicsInfo*)pphi)->epfPhysicsFlags();

	if (epf[epfTANGIBLE])
		strcat(str_flags, "Tangible ");

	if (epf[epfMOVEABLE])
		strcat(str_flags, "Moveable ");
	
	SetDlgItemText(IDC_PHYSICS_FLAGS, str_flags);

	// Display physics box information.
	DisplayPhysicsInfoBox(pphi);
}

//*********************************************************************************************
//
void CDialogObject::DisplayPhysicsInfoBox
(
	const CPhysicsInfo* pphi
)
//
// Displays the physics box information about the object.
//
//**************************************
{
	float f_density    = 0.0f;	// The density of the object, what units?
	float f_friction   = 0.0f;	// The frictive quality of the object, 0 to 1.
	float f_elasticity = 0.0f;	// The elastic quality of the object (for collisions) 0 to 1.
	float f_mass	   = 0.0f;	// Mass of the object

	if (pphi)
	{
		const CPhysicsInfoBox* pphib = pphi->ppibCast();

		if (pphib)
		{
			f_density    = pphib->fDensity;
			f_friction   = pphib->fFriction;
			f_elasticity = pphib->fElasticity;
		}

		f_mass = pphi->fMass(pinsObject);
	}

	// Display the information.
	SetDlgItemFloat(this, IDC_PHYSICS_BOX_DENSITY,    f_density);
	SetDlgItemFloat(this, IDC_PHYSICS_BOX_FRICTION,   f_friction);
	SetDlgItemFloat(this, IDC_PHYSICS_BOX_ELASTICITY, f_elasticity);
	SetDlgItemFloat(this, IDC_PHYSICS_BOX_MASS,		  f_mass);
}

//*********************************************************************************************
//
void CDialogObject::DisplayPresenceInfo
(
	const CPresence3<>& pr3
)
//
// Displays the presence information about the object.
//
//**************************************
{
	SetDlgItemFloat(this, IDC_PRES_POS_X, pr3.v3Pos.tX);
	SetDlgItemFloat(this, IDC_PRES_POS_Y, pr3.v3Pos.tY);
	SetDlgItemFloat(this, IDC_PRES_POS_Z, pr3.v3Pos.tZ);
	SetDlgItemFloat(this, IDC_PRES_SCALE, pr3.rScale);
	SetDlgItemFloat(this, IDC_PRES_DISTANCE, (pr3.v3Pos - pcamGetCamera()->v3Pos()).tLen());
}
