/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of GUIPipeLine.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GUIPipeline.cpp                                                $
 * 
 * 103   9/14/98 4:45p Mlange
 * Now unselects objects outside of dragging rectangle. Fixed broken 'avoid boundary boxes'
 * logic in dragging rectangle select function.
 * 
 * 102   98/09/12 16:39 Speter
 * Now selects animals in preference to bio-boxes.
 * 
 * 101   98.09.12 12:18a Mmouni
 * Changed shape query to render type query.
 * 
 * 100   9/08/98 3:15p Mlange
 * Implemented rectangle select.
 * 
 * 99    98/09/07 1:48 Speter
 * Replaced Settle() function with ActivateSettle(), which queues objects for activation if
 * necessary.
 * 
 **********************************************************************************************/


//
// Includes.
//
#include "stdafx.h"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "GUIAppDlg.h"
#include "GUIPipeLine.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/Query/QPhysics.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/Renderer/PipeLine.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/View/LineDraw.hpp"
#include "Shell/WinRenderTools.hpp"
#include "GUITools.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"
#include "Lib/Physics/PhysicsSystem.hpp"
#include "Lib/Physics/Magnet.hpp"
#include "Lib/GeomDBase/RayCast.hpp"
#include "Lib/Renderer/RenderCacheInterface.hpp"
#include "Lib/Sys/FastHeap.hpp"
#include "Lib/Std/LocalArray.hpp"
#include <float.h>
#include <math.h>
#include <algo.h>


extern CGUIAppDlg* pappdlgMainWindow;


//
// Constants.
//

// Distance square from a mouse position to an object's centre on screen to cause selection.
int iClickDistSqr = 36;

//*********************************************************************************************
bool bGet2DCentreFromShape(const CCamera& cam, const CTransform3<>& tf3_shape_to_world,
	                       int& i_x, int& i_y)
{
	// Transform origin of the shape to camera space and clip.
	CVector3<> v3_cam = tf3_shape_to_world.v3Pos * cam.tf3ToNormalisedCamera();
	if (cam.pbvcamClipVolume()->esfSideOf(v3_cam) != esfINSIDE)
		return false;

	CVector3<> v3_projected = cam.ProjectPoint(v3_cam);

	i_x = iPosFloatCast(v3_projected.tX);
	i_y = iPosFloatCast(v3_projected.tY);
	return true;
}


//
// Class implementations.
//

//*********************************************************************************************
//
// CCamera implementation.
//

//*********************************************************************************************
void CPipeLine::Init()
{
	//
	// Build the light list so that the first element on the list is always
	// the ambient light.
	//

	// Now done in WDBase::Init()
//	wWorld.Add(new CInstance(rptr_cast(CRenderType, rptr_new CLightAmbient(0.2f))));

}

//*********************************************************************************************
rptr<CLightAmbient> CPipeLine::pltaGetAmbientLight()
{
	//
	// Iterate through the world database and find the first ambient light.
	// The ambient light is always the first one in the list.
	//
	CWDbQueryLights wqlt;

	// Return the light.
	rptr<CLight> plt = ptCastRenderType<CLight>(wqlt.tGet()->prdtGetRenderInfo());
	return rptr_static_cast(CLightAmbient, plt);
}

//*********************************************************************************************
void CPipeLine::AddObject(CInstance* pins)
{
	AddInView(pins);

	// Select the added shape.
	Select(pins);
}

//*********************************************************************************************
void CPipeLine::Paint()
{
	gmlGameLoop.Paint();
}

/*
//*********************************************************************************************
bool CPipeLine::bGetCentreofSelectedObject(int& i_x, int& i_y)
{
	if (pinsSelectedShape == 0)
		return false;

	ptr<CCamera> pcam = CWDbQueryActiveCamera(wWorld).tGet();

	bGet2DCentreFromShape(*pcam, pinsSelectedShape->pr3Presence(), i_x, i_y);

	return true;
}
*/

//*********************************************************************************************
bool CPipeLine::bSelected(CPartition* ppart)
{
	TSelectedList::iterator itppart = find(lsppartSelected.begin(), lsppartSelected.end(), ppart);
	return itppart != lsppartSelected.end();
}

//*********************************************************************************************
int CPipeLine::iSelectedCount() const
{
	return lsppartSelected.size();
}

//*********************************************************************************************
CPartition* CPipeLine::ppartLastSelected(int i_index)
{
	// Get the asked-for index, if any.
	forall (pipeMain.lsppartSelected, TSelectedList, itppart)
	{
		if (i_index-- == 0)
			return *itppart;
	}

	// No such beast.
	return 0;
}

//*********************************************************************************************
void CPipeLine::Select(CPartition* ppart, bool b_augment)
{
	// Add objects to the front of the list, for easy extraction of most recent.

	if (b_augment)
	{
		// Toggle selection of object.
		if (ppart)
		{
			if (bSelected(ppart))
				lsppartSelected.remove(ppart);
			else
				lsppartSelected.push_front(ppart);
		}
	}
	else
	{
		// If already selected, do nothing.  (This allows mouse to move selected objects).
		CInstance* pins = ptCast<CInstance>(ppart);

		if (!bSelected(ppart))
		{
			// Make this the sole selected object.
			erase_all(lsppartSelected);

			if (pins)
			{
				// Auto-select other instances that are attached by magnets.
				list<CMagnetPair*> lspmp;
				NMagnetSystem::GetAttachedMagnets(pins, &lspmp);

				forall (lspmp, list<CMagnetPair*>, itpmp)
				{
					if ((*itpmp)->pinsSlave)
					{
						lsppartSelected.remove((*itpmp)->pinsSlave);
						lsppartSelected.push_front((*itpmp)->pinsSlave);
					}
				}
			}
		}

		// Whether selected or not, move to front of list.
		if (ppart)
		{
			// Move to front of list.
			lsppartSelected.remove(ppart);
			lsppartSelected.push_front(ppart);

			// Also, make physically active.
			if (pins)
				pphSystem->ActivateSettle(pins);
		}
	}
}

//*****************************************************************************************
void CPipeLine::Select(const CPoint& pnt_a, const CPoint& pnt_b)
{
	int i_min_x = Min(pnt_a.x, pnt_b.x);
	int i_max_x = Max(pnt_a.x, pnt_b.x);
	int i_min_y = Min(pnt_a.y, pnt_b.y);
	int i_max_y = Max(pnt_a.y, pnt_b.y);

	//
	// Iterate through the list of shapes.
	//
	ptr<CCamera> pcam = CWDbQueryActiveCamera(wWorld).tGet();

	for (CWDbQueryRenderTypesAndMagnets wqrtm; wqrtm.bIsNotEnd(); wqrtm++)
	{
		CPartition* ppart_curr = wqrtm.tGet().ppart;

		// Skip boundary boxes.
		if (ptCast<CBoundaryBox>(ppart_curr))
			continue;

		int i_x_s, i_y_s;

		bool b_within_rect = bGet2DCentreFromShape(*pcam, ppart_curr->pr3Presence(), i_x_s, i_y_s) &&
		                     bWithin(i_x_s, i_min_x, i_max_x) && bWithin(i_y_s, i_min_y, i_max_y);

		if (b_within_rect)
		{
			if (!bSelected(ppart_curr))
				Select(ppart_curr, true);
		}
		else
		{
			if (bSelected(ppart_curr))
				lsppartSelected.remove(ppart_curr);
		}
	}
}


//*********************************************************************************************
bool CPipeLine::bSelect(int i_x, int i_y, bool b_augment)
{
	//
	// Iterate through the list of shapes.
	//
	CPartition* ppart_selected = 0;

	ptr<CCamera> pcam = CWDbQueryActiveCamera(wWorld).tGet();

	for (CWDbQueryRenderTypesAndMagnets wqrtm; wqrtm.bIsNotEnd(); wqrtm++)
	{
		int i_x_s, i_y_s;

		if (bGet2DCentreFromShape(*pcam, wqrtm.tGet().ppart->pr3Presence(), i_x_s, i_y_s))
		{
			i_x_s -= i_x;
			i_y_s -= i_y;
			if (i_x_s * i_x_s + i_y_s * i_y_s <= iClickDistSqr)
			{
				ppart_selected = wqrtm.tGet().ppart;
				if (!ptCast<CBoundaryBox>(ppart_selected))
					// If it wasn't a bio boundary box, finish.
					// Otherwise, continue to see if something else is selected in the same position.
					break;
			}
		}
	}

	Select(ppart_selected, b_augment);	

	return ppart_selected != 0;
}

//*********************************************************************************************
void CPipeLine::UnselectAll()
{
	lsppartSelected.erase(lsppartSelected.begin(), lsppartSelected.end());
}

// Colours used for marking objects.
static CColour clrSPHERE_INSIDE		(0.0, 1.0, 0.0);		// Green.
static CColour clrSPHERE_INTERSECT	(1.0, 1.0, 0.0);		// Yellow.
static CColour clrSPHERE_OUTSIDE	(1.0, 0.0, 0.0);		// Red.

static CColour clrEDGE_FLAT			(0.0, 1.0, 1.0);		// Cyan.
static CColour clrEDGE_CURVED		(1.0, 0.0, 1.0);		// Magenta.

static CColour clrNORMAL_FLAT		(0.0, 0.0, 1.0);		// Blue.
static CColour clrNORMAL_CURVED		(1.0, 0.0, 0.5);		// Purple.
bool bDrawPartitions = false;

//*********************************************************************************************
void CPipeLine::MarkObjects(rptr<CRasterWin> pras, 
	bool b_draw_crosshairs, bool b_draw_bounds, bool b_draw_wire, bool b_draw_pins)
{
	CCycleTimer ctmr;

	ptr<CCamera> pcam = CWDbQueryActiveCamera(wWorld).tGet();

	//
	// Iterate through the list of shapes to render.
	//
	if (b_draw_crosshairs)
	{
		for (CWDbQueryRenderTypes wqrt(pcam); wqrt.bIsNotEnd(); wqrt++)
		{
			int i_x, i_y;

			if (bGet2DCentreFromShape(*pcam, wqrt->ppart->pr3Presence(), i_x, i_y))
			{
				if (!(pappdlgMainWindow->bCrosshairRadius &&				// don't draw if....  using radius
					  (wqrt->ppart->v3Pos() - pcam->v3Pos()).tLenSqr() // and really far away
						> pappdlgMainWindow->rCrosshairRadius * pappdlgMainWindow->rCrosshairRadius))  
				{
					if (pappdlgMainWindow->bCrosshairVegetation || // draw if not filtering vegetation
						((CInstance*)wqrt->ppart)->strGetInstanceName()[0] != 'V')  // or if name doesn't start with 'V'
					{
						DrawCrosshair(rptr_cast(CRaster, pras), i_x, i_y, false);
					}
				}
			}
		}
	}

	forall (pipeMain.lsppartSelected, TSelectedList, itppart)
	{
		// Draw crosshairs for selected objects!
		if (b_draw_crosshairs)
		{
			int i_x, i_y;
			if (bGet2DCentreFromShape(*pcam, (*itppart)->pr3Presence(), i_x, i_y))
			{
				DrawCrosshair(rptr_cast(CRaster, pras), i_x, i_y, true);
			}
		}
	}

	if (b_draw_wire || b_draw_pins)
	{
		forall (pipeMain.lsppartSelected, TSelectedList, itppart)
		{
			CInstance* pins = ptCast<CInstance>(*itppart);
			if (!pins)
				continue;

			// Here come vertex normals on selected shapes only, drawn with a mini-renderer.
			rptr_const<CShape> psh = ptCastRenderType<CShape>(pins->prdtGetRenderInfo());
			if (psh)
			{
				psh = psh->pshGetProperShape
				(
					(*itppart)->fEstimateScreenSize(*pcam)
				);

				CDraw draw(pras);

				// Build the shape to camera space transformation.
				CTransform3<> tf3_shape_camera = (*itppart)->pr3Presence() * pcam->tf3ToNormalisedCamera();

				// Obtain the position of the camera in local object space.
				CVector3<> v3_cam_obj_pos = pcam->v3Pos() * ~(*itppart)->pr3Presence();

				// Length of normals to draw (in object space).
				TReal r_normal_len = 1.0 / sqrt(float(psh->iNumPolygons()));

				// We have a shape.  Iterate through polygons and vertices.
				aptr<CShape::CPolyIterator> ppi = psh->pPolyIterator(pins, 0); 

				// Create an array of transformed points/outcodes for each unique shape point.
				CLArray(SClipPoint, paclpt_points, psh->iNumPoints());
				ppi->TransformPoints(tf3_shape_camera, *pcam, paclpt_points, true);

				for (CShape::CPolyIterator& pi = *ppi; pi.bNext(); )
				{
					// Access only front-facing polygons.
					if (pi.plPlane().rDistance(v3_cam_obj_pos) > 0)
					{
						for (uint u_v = 0; u_v < pi.iNumVertices(); u_v++)
						{
							if (b_draw_wire)
							{
								uint u_v2 = (u_v + 1) % pi.iNumVertices();

								if (Fuzzy(pi.d3Normal(u_v)) == pi.d3Normal(u_v2))
									draw.Colour(clrEDGE_FLAT);
								else
									draw.Colour(clrEDGE_CURVED);

								// Draw polygon outlines.
								draw.Line
								(
									pcam->ProjectPoint(paclpt_points[pi.iShapePoint(u_v)].v3Point),
									pcam->ProjectPoint(paclpt_points[pi.iShapePoint(u_v2)].v3Point)
								);
							}
							if (b_draw_pins)
							{
								if (Fuzzy(pi.d3Normal(u_v)) == pi.d3Normal())
									draw.Colour(clrNORMAL_FLAT);
								else
									draw.Colour(clrNORMAL_CURVED);

								// Get 2 points to draw in object space.
								// Set length of line based on object nominal size of 1.
								CVector3<> v3_1 = pi.v3Point(u_v);
								CVector3<> v3_2 = (pi.v3Point(u_v) + pi.d3Normal(u_v) * r_normal_len);

								// Project the points, then wack'em on the screen.
								draw.Line
								(
									pcam->ProjectPoint(v3_1 * tf3_shape_camera),
									pcam->ProjectPoint(v3_2 * tf3_shape_camera)
								);
							}
						}
					}
				}
			}
		}
	}

	if (b_draw_bounds)
	{
		CDraw draw(pras);

		// For each selected object, draw bounding volume of it and all children.
		forall (pipeMain.lsppartSelected, TSelectedList, itppart)
		{
			CWDbQueryAllChildren wqac(*itppart);

			if (bDrawPartitions)
			{
				// Add ancestor partitions to list.
				CPartition* ppart_parent = *itppart;
				while (ppart_parent = ppart_parent->ppartGetParent())
				{
					// Add ppart to list. Give it the INTERSECT flag so it shows in a different colour.
					SPartitionListElement ple = {ppart_parent, esfINTERSECT};
					wqac.push_back(ple);
				}
			}

			for (; wqac.bIsNotEnd(); wqac++)
			{
				if (!wqac->ppart->bNoSpatialInfo())
				{
					switch (wqac->esfView)
					{
						case esfINSIDE:
							draw.Colour(clrSPHERE_INSIDE);
							break;
						case esfINTERSECT:
							draw.Colour(clrSPHERE_INTERSECT);
							break;
						case esfOUTSIDE:
							draw.Colour(clrSPHERE_OUTSIDE);
							break;
					}

					CInstance* pins;
					if (pins = ptCast<CInstance>(wqac->ppart))
					{
						// Hack for finding detail level.
						rptr_const<CShape> psh = ptCastRenderType<CShape>(pins->prdtGetRenderInfo());
						if (psh)
						{
							rptr_const<CShape> psh_shape = psh->pshGetProperShape
							(
								pins->fEstimateScreenSize(*pcam)
							);
							if (psh_shape == psh)
								;
							else if (psh_shape == psh->pshCoarser)
								draw.Colour(CColour(0.0, 0.5, 0.5));
							else
								draw.Colour(CColour(0.0, 0.0, 0.1));

							//
							// Draw the crosshairs.
							//

							// Get the points defining the bounding volume.
							CPArray< CVector3<> > pav3 = psh_shape->pav3GetWrap();
							CLArray(CVector3<>, pav3_proj, pav3.uLen);

							// Build the shape to camera space transformation.
							CTransform3<> tf3_shape_cam = pins->pr3Presence() * pcam->tf3ToNormalisedCamera();

							// Project the points.
							pcam->ProjectPointsPerspective(pav3, pav3_proj, tf3_shape_cam);

							// Draw the crosshairs.
							for (uint u = 0; u < pav3_proj.uLen; ++u)
							{
								int i_x = int(pav3_proj[u].tX);
								int i_y = int(pav3_proj[u].tY);
							
								draw.Unlock();
								DrawCrosshair(rptr_cast(CRaster, pras), i_x, i_y, 1);
								draw.Lock();
							}
						}
					}

					// Draw the bounding volume.
					const CBoundVolBox* pbvb;
					if (pbvb = wqac->ppart->pbvBoundingVol()->pbvbCast())
					{
						draw.Box3D(pbvb->tf3Box(wqac->ppart->pr3Presence() * pcam->tf3ToHomogeneousScreen()));
					}
					else
					{
						CVolSphere vs(wqac->ppart->pbvBoundingVol()->fMaxExtent(), &wqac->ppart->pr3Presence());

						// Transform origin of the shape to camera space and clip.
						CVector3<> v3_cam = vs.v3Pos * pcam->tf3ToNormalisedCamera();
						if (v3_cam.tY <= 0)
							continue;

						// Find radius offset in world space.
						CVector3<> v3_radius = CVector3<>(vs.rRadius, 0, vs.rRadius) * pcam->pr3Presence().r3Rot;
						CVector3<> v3_a = pcam->ProjectPoint((vs.v3Pos - v3_radius) * pcam->tf3ToNormalisedCamera());
						CVector3<> v3_b = pcam->ProjectPoint((vs.v3Pos + v3_radius) * pcam->tf3ToNormalisedCamera());

						if (v3_a.tX > -8000 && v3_b.tX < 8000 &&
							v3_a.tY > -8000 && v3_b.tY < 8000)
							::Ellipse(draw.hdcScreen, v3_a.tX, v3_a.tY, v3_b.tX, v3_b.tY);
					}
				}
			}
		}

		extern CDArray< CVector2<> > pav2Convex;

		if (pav2Convex.uLen)
		{
			float f_x = (pav2Convex[0].tX * 5.0f + 80.0f);
			float f_y = (-pav2Convex[0].tY * 5.0f + 120.0f);
			SetMinMax(f_x, 0.0f, 500.0f);
			SetMinMax(f_y, 0.0f, 500.0f);
			draw.MoveTo(f_x, f_y);

			// Draw the crosshairs.
			for (uint u = 1; u < pav2Convex.uLen; ++u)
			{
				float f_x0 = (pav2Convex[u].tX * 5.0f + 80.0f);
				float f_y0 = (-pav2Convex[u].tY * 5.0f + 120.0f);
				SetMinMax(f_x0, 0.0f, 500.0f);
				SetMinMax(f_y0, 0.0f, 500.0f);
				draw.LineTo(f_x0, f_y0);
			}
			draw.LineTo(f_x, f_y);
		}
	}

	proProfile.psEndFrame.Add(ctmr());
}

//*********************************************************************************************
CInstance* CPipeLine::pinsGetLight(CInstance* pins)
{
	//
	// Find a CEntityLight attached to this instance.
	//
	for (CWDbQueryLights wqlt; wqlt.bIsNotEnd(); wqlt++)
	{
		CEntityAttached* petat;
		if (petat = dynamic_cast<CEntityAttached*>(*wqlt))
		{
			if (petat->pinsAttached() == pins)
				return petat;
		}
	}

	return 0;
}

//*********************************************************************************************
int CPipeLine::iDeleteSelected()
{
	forall (pipeMain.lsppartSelected, TSelectedList, itppart)
	{
		CInstance* pins = ptCast<CInstance>(*itppart);
		if (!pins)
			continue;

		//
		// If a light is attached to the shape, remove the light from the light list.
		//
		CInstance* pins_light = pinsGetLight(pins);

		if (pins_light)
		{
			// Remove the light from the world.
			wWorld.Remove(pins_light);

			// Free the memory associated with the light.
			delete pins_light;
		}

		// Remove object from the world database.
		wWorld.Remove(pins);

		// Free the memory associated with the shape.
		delete pins;
	}

	int i = lsppartSelected.size();

	// Clear the list.
	erase_all(lsppartSelected);

	return i;
}


//
// Function implementations.
//

//*********************************************************************************************
CCamera* pcamGetCamera()
{
	CCamera* pcam = CWDbQueryActiveCamera().tGet();
	Assert(pcam);
	return pcam;
}

//*********************************************************************************************
void AddInView(CInstance* pins)
{
	// Push the object onto the world database.
	wWorld.Add(pins);
}


//
// Externally defined variables.
//
CPipeLine  pipeMain;
bool       bStaticHandles = false;


//
//	CWorld overrides.
//

//*********************************************************************************************
bool CWorld::bSelected(CInstance* pins)
{
	return pipeMain.bSelected(pins);
}

//*********************************************************************************************
int CWorld::iSelectedCount() const
{
	return pipeMain.iSelectedCount();
}

//*********************************************************************************************
CInstance* CWorld::pinsLastSelected
(
	int i_index						// Which object to get from the list.
									// 0 means last selected, 1 means next last, etc.
)
{
	return ptCast<CInstance>(pipeMain.ppartLastSelected(i_index));
}

//*********************************************************************************************
void CWorld::Select
(
	CInstance* pins,				// Object to select.
	bool b_augment					// Adds it to list; otherwise, selects exclusively.
)
{
	pipeMain.Select(pins, b_augment);
}

//*****************************************************************************************
bool CWorld::bSelect
(
	int i_x, int i_y,					// Position of mouse cursor on screen.
	bool b_augment				// As above.
)
{
	return pipeMain.bSelect(i_x, i_y, b_augment);		
}

//*****************************************************************************************
int iDeleteSelected
(
)
{
	return pipeMain.iDeleteSelected();	
}
