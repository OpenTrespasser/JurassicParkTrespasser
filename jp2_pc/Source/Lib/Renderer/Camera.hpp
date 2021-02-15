/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The camera class for defining the view on the 3d scene.
 *
 * Bugs:
 *		When the viewport is moved so that some of it is off the screen, the viewport is
 *		not resized accordingly.  This causes rendering off the screen, which crashes.
 *
 * To do:
 *		Consider truncating the viewport's coordinates to integers first, so that there is at
 *		least half a pixel's worth of room before rounding errors in the clipping produce
 *		visible artifacts. See also the todo list in the Clip3D.hpp header file.
 *
 *		Include 'P' in the equation for the field of view. Add comments describing the
 *		derivation of the equations for field of view to scale factor. Optimise the
 *		implementation of these. Add assert checking for too large field of view.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Camera.hpp                                               $
 * 
 * 70    98.06.25 8:46p Mmouni
 * Added stuff to support render quality setting.
 * 
 * 69    5/25/98 12:10a Pkeet
 * Added a parent camera pointer and access member functions to it.
 * 
 * 68    98/02/10 13:15 Speter
 * Replaced changed CSArray with CPArray in WorldExtents.
 * 
 * 67    12/17/97 5:52p Gfavor
 * Removed multiply by fInvZAdjust in ProjectPointParallel.
 * 
 * 66    97/11/15 10:52p Pkeet
 * Added the 'fViewWidthRatio' data member to the camera properties.
 * 
 * 65    11/10/97 6:07p Agrant
 * Save/Load functions
 * 
 * 64    10/27/97 5:27p Mlange
 * Added CCamera::vsBoundingSphere() and CCamera::WorldExtents().
 * 
 * 63    97/10/02 12:19 Speter
 * ProjectPointParallel now multiplies screen Z by fInvZAdjust (allows for valid range of Z).
 * 
 * 62    9/12/97 5:13p Mlange
 * Added support for simplified point projections through the tlr2GetProjectPlaneToScreen() and
 * rGetProjectPlaneDist() functions.
 * 
 * 61    8/28/97 4:10p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 61    97/08/22 11:21 Speter
 * Added bClipNearFar flag to SProperties.
 * 
 * 60    97/08/11 12:23 Speter
 * Made CTransLinear use the * operator, like other transforms.
 * 
 * 59    97/08/08 15:29 Speter
 * fAspectRatio now resides in CCamera::SProperties, and refers to physical aspect ratio.  Added
 * tf3ToHomogeneousScreen() function.
 * 
 * 58    97/08/04 6:18p Pkeet
 * Added the 'pr3VPresence' member functions.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_CAMERA_HPP
#define HEADER_LIB_RENDERER_CAMERA_HPP

#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/Renderer/RenderType.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Lib/Transform/Transform.hpp"
#include "Lib/View/Viewport.hpp"
#include "Lib/Math/FastTrig.hpp"


//
// Forward declarations.
//
class CBoundVol;
class CBoundVolCamera;
class CVolSphere;
class CRenderPolygon;
class CPipelineHeap;
class CPredictMovement;


//
// Class definitions.
//

//**********************************************************************************************
//
class CCamera : public CEntityAttached
//
// Defines the view on the 3d world.
//
// Prefix: cam
//
//**************************************
{
public:

	//******************************************************************************************
	//
	struct SProperties
	//
	// Structure for holding the camera's properties.
	//
	// Prefix: camprop
	//
	//**************************************
	{
		CViewport vpViewport;		// Viewport to map the 3d view to. The centre of the viewport defines
									// the centre of projection.

		bool bPerspective;			// Whether this is a perspective-projecting camera.
									// Otherwise, it's parallel projecting.

		TReal rNearClipPlaneDist;	// Distance of the near clipping plane from the camera. 
									// Must be a value >0.

		TReal rFarClipPlaneDist;	// Distance of the far clipping plane from the camera.  
									// Must be a value >rNearClipPlaneDist.

		TReal rDesiredFarClipPlaneDist;		// Distance of the far clipping plane from the camera.  
											// Not adjusted for the global quality settigs.

		bool bClipNearFar;			// Actually clip objects with near and far planes.
									// (Otherwise, they just determine scale).

		TReal rViewWidth;			// Half the width of the camera volume at distance = 1.0.
									// For bPerspective = false, this is the width of the view.
									// For bPerspective = true, this is the ratio of width to depth,
									// or the tangent of half the view angle.
									// This value can be set directly, or by the SetAngleOfView()
									// function, which sets it based on the current rFarClipPlaneDist,
									// for perspective cameras.
		float fAspectRatio;			// Ratio of width to height in physical view rectangle.
									// Defaults to standard monitor aspect ratio of 4/3.
		float fZoomFactor;			// The amount of zoom. This relates directly to increase in size of the
									// view on the scene. For example, a zoom factor of 2.0 doubles the size
									// of each shape that is in view. A zoom factor of less than one (but
									// greater than zero) is allowed.
		float fViewWidthRatio;		// Set to vpViewport.scWidth / rViewWidth if bPerspective
									// is set, otherwise set to 1.


		//**************************************************************************************
		//
		// Constructor.
		//

		SProperties();

		//**************************************************************************************
		//
		// Member functions.
		//

		//**************************************************************************************
		CAngle angGetAngleOfView() const;
			
		//**************************************************************************************
		void SetAngleOfView
		(
			CAngle ang
		);

		//**************************************************************************************
		//
		void SetFarClipFromDesired
		(
		);
		//
		// Set the far clip distance from the desired far clip distance and the 
		// global quality settings.
		//
		//**************************************
	};


private:
	SProperties campropCurrent;		// The current properties of this camera.

	ptr<CBoundVol> pbvVolume;		// The bounding volume of the camera.
	ptr<CBoundVolCamera> pbvcamClipVolume_;
									// Object used to clip polygons.

	CTransform3<> tf3Normalise;		// The normalising transform that converts from camera space to the canonical view volume.
	CTransLinear2<> tlr2Project;	// Linear transformation used to convert norm coords to screen space.

	TReal rNormNearClipDist;		// Distance to the near clipping plane after the view normalisation has been applied.
	TReal rNormFarClipDist;			// Distance to the far clipping plane after the view normalisation has been applied.

	TReal rProjectPlaneDist;		// The distance of the projection plane from the camera.

	float fInvZAdjust;				// The ratio of the near to far clipping planes.
	CPredictMovement* ppmPrediction;// Object to predict movement.
	const CCamera*    pcamParent;	// Pointer to the parent camera.

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CCamera();

	// Initialiser constructor.
	CCamera(const CPresence3<>& pr3, const SProperties& camprop);

	// Destructor.
	~CCamera();


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetProperties
	(
		const CCamera::SProperties& camprop		// New properties for this camera.
	);
	//
	// Update the properties of this camera.
	//
	//**************************************

	//******************************************************************************************
	//
	const CCamera* pcamGetParent
	(
	) const
	//
	// Returns the parent camera of this camera.
	//
	//**************************************
	{
		Assert(pcamParent);
		return pcamParent;
	}

	//******************************************************************************************
	//
	void SetParentCamera
	(
		const CCamera* pcam
	)
	//
	// Sets the parent camera of this camera.
	//
	//**************************************
	{
		Assert(pcam);
		pcamParent = pcam;
	}

	//*****************************************************************************************
	//
	virtual bool bCanHaveChildren
	(
	) override;
	//
	// Returns 'false.'
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void UpdatePrediction
	(
	);
	//
	// Updates the prediction object. If no prediction object is currently created, this
	// function creates one.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual CPredictMovement* ppmGetPrediction
	(
	);
	//
	// Returns a pointer to the prediction object. If no prediction object is currently
	// created, this function creates one.
	//
	//**************************

	//*****************************************************************************************
	//
	CPresence3<> pr3GetStandardPrediction
	(
	);
	//
	// Returns a presence giving the predicted position and orientation using the default
	// lookahead value.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void ClearPrediction
	(
	);
	//
	// Clears the prediction object's tracking.
	//
	//**************************

	//******************************************************************************************
	//
	const CCamera::SProperties& campropGetProperties
	(
	) const
	//
	// Obtain the current properties of the camera.
	//
	// Returns:
	//		The current properties of the camera.
	//
	//**************************************
	{
		return campropCurrent;
	}

	//******************************************************************************************
	//
	CTransform3<> tf3ToNormalisedCamera
	(
	) const;
	//
	// Returns:
	//		The transform from world space to normalised camera space.
	//
	// Notes:
	//		To construct a shape-to-normalised camera transform, concatenate the inverse of the
	//		shape's placement with this transform.
	//
	//**************************************


	//******************************************************************************************
	//
	CTransform3<> tf3ToHomogeneousScreen
	(
	) const;
	//
	// Returns:
	//		The transform from world space to screen space, without the perspective divide.
	//
	// Notes:
	//		This transform can be used to go from world to screen space in one go, when the
	//		intermediate normalised camera space is not needed.  For perspective cameras, the
	//		division by Z is still needed.
	//
	// Example:
	//		CVector3<> v3_screen = v2_world * cam.tf3ToHomogeneousScreen();
	//		if (cam.campropGetProperties().bPerspective)
	//		{
	//			v3_screen.tZ = 1.0 / v3_screen.tZ;
	//			v3_screen.tX *= v3_screen.tZ;
	//			v3_screen.tY *= v3_screen.tZ;
	//		}
	//
	//**************************************


	//******************************************************************************************
	//
	CTransLinear2<> tlr2GetProjectPlaneToScreen
	(
	) const
	//
	// Returns:
	//		The mapping from projection plane to screen space coordinates.
	//
	// Notes:
	//		Together with the rGetProjectPlaneDist() function this can be used to implement a
	//		simplified point projection.
	//
	//		Example (assuming the points are in world space units but have been translated and
	//		rotated relative to the camera's world position and orientation) :
	//
	//		Yscreen = (Zworld * D * Sy) / Yworld + Ty
	//
	//		Where D  = rGetProjectPlaneDist()
	//		      Sy = tlr2GetProjectPlaneToScreen().tlrY.tScale
	//		      Ty = tlr2GetProjectPlaneToScreen().tlrY.tOffset
	//
	//**************************************
	{
		return tlr2Project;
	}


	//******************************************************************************************
	//
	TReal rGetProjectPlaneDist
	(
	) const
	//
	// Returns:
	//		The distance of the projection plane from the camera as a scale factor applied to
	//		world space X and Z.
	//
	// Notes:
	//		Valid for perspective cameras only!
	//
	//		The derivation of this scale factor assumes that the centre of projection is at the
	//		centre of the viewport and ignores the aspect ratio of the display.
	//
	//		See also: tlr2ProjectPlaneToScreen().
	//
	//**************************************
	{
		Assert(campropCurrent.bPerspective);

		return rProjectPlaneDist;
	}


	//******************************************************************************************
	//
	CVector3<> ProjectPoint
	(
		const CVector3<>& v3_cam		// A normalised camera-space point to project to screen space.
	) const
	//
	// Returns:
	//		The point, projected, scaled, and axis-transformed to screen space.
	//
	// Cross-references:
	//		Calls either ProjectPointPerspective or ProjectPointParallel (q.v).
	//
	//**************************************
	{
		if (campropCurrent.bPerspective)
			return ProjectPointPerspective(v3_cam);
		else
			return ProjectPointParallel(v3_cam);
	}

	//******************************************************************************************
	//
	CVector3<> UnprojectPoint
	(
		const CVector3<>& v3_screen			// A screen-space point: 
											// x and y are screen pixel values.
											// z is the normalised inverse distance from the camera.
	) const;
	//
	// Returns:
	//		The camera-space point, with axes swapped as described in ProjectPoint.
	//
	//**************************************

	//******************************************************************************************
	//
	void ProjectVertices
	(
		CPArray<SRenderVertex> parv_vertices	// Array of vertices to transform in place.
	) const;
	//
	// Project an array of vertices from normalised camera space to 2d screen space.
	// Alter the coordinates in place, and set the fInvZ field as well.
	//
	// Runtime requirements:
	//		The input points must have been clipped to lie within the view fustrum.
	//
	// Notes:
	//		The resulting vertices are in screen space, with 0,0 in the top left and y
	//		increasing down the screen. Also, the vertices are translated by .5 in the x and y
	//		directions so that the drawing primitives can simply truncate (instead of round) to
	//		the centre of an integer pixel.
	//
	//		The recipocal Z component of the resulting vertex ranges from 1 to
	//		rNearClipPlaneDist / rFarClipPlaneDist.
	//
	//**************************************

	//******************************************************************************************
	//
	void ProjectPointsPerspective
	(
		CPArray< CVector3<> > pav3_from,	// Array of points to transform.
		CPArray< CVector3<> > pav3_to,		// Storage for transformed and projected points.
		const CTransform3<>&  t3f			// Shape to camera transform.
	) const;
	//
	// Project an array of vertices from object space to 2d screen space.
	//
	// Notes:
	//		This function differs from other project vertex functions because it stores the
	//		new x, y and z values in a second array provided by the user.
	//
	//**************************************

	//******************************************************************************************
	//
	void ProjectPointsPerspectiveFast
	(
		CPArray< CVector3<> > pav3_from,	// Array of points to transform.
		CPArray< CVector3<> > pav3_to,		// Storage for transformed and projected points.
		const CTransform3<>&  t3f			// Shape to camera transform.
	) const;
	//
	// Project an array of vertices from object space to 2d screen space.
	//
	// Notes:
	//		Quicker, but less accurate that the regular function.
	//
	//**************************************

	//******************************************************************************************
	//
	const CBoundVolCamera* pbvcamClipVolume() const
	//
	//**********************************
	{
		return pbvcamClipVolume_;
	}


	//******************************************************************************************
	//
	void WorldExtents
	(
		CPArray< CVector3<> > pa_extents		// Array to initialise.  
												// Must have size() of (at least) 8.
	) const;
	//
	// Initialises the given array with eight points defining the world space extents of the
	// camera volume.
	//
	// Notes:
	//		Array indices [0..3] define the position of the far clipping plane.
	//		Array indices [4..7] define the position of the near clipping plane.
	//
	//		The points are ordered counterclockwise around the view vector, starting at the
	//		bottom left.
	//
	//**********************************



	//******************************************************************************************
	//
	CVolSphere vsBoundingSphere() const;
	//
	// Returns:
	//		A close fitting sphere to the world space camera volume.
	//
	//**********************************

	//******************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual void Cast(CCamera** ppcam) override
	{
		*ppcam = this;
	}

	//*****************************************************************************************
	virtual const CBoundVol* pbvBoundingVol() const override
	// Returns the bounding volume for the camera's partition.
	{
		return pbvVolume;
	}

	//*****************************************************************************************
	virtual bool bIncludeInBuildPart() const override
	{
		return false;
	}

	//*****************************************************************************************
	//
	CPresence3<> pr3VPresence
	(
		bool b_use_prediction = false	// Flag indicates if prediction should be used.
	);
	//
	// Returns a reference to the partition's presence.
	//
	//**************************

	//*****************************************************************************************
	//
	CPresence3<> pr3VPresence
	(
		bool b_use_prediction = false	// Flag indicates if prediction should be used.
	) const;
	//
	// Returns a reference to the partition's presence.
	//
	//**************************

protected:

	//******************************************************************************************
	//
	CVector3<> ProjectPointPerspective
	(
		const CVector3<>& v3_cam		// A camera-space point to project to screen space.
	) const
	//
	// Returns:
	//		The point, projected, scaled, and axis-transformed to screen space.
	//
	// Notes:
	//
	//		The axes in each system differ as follows:
	//					Camera	Screen
	//		right		x		x
	//		down		z		y
	//		in			y		z
	//
	//		The returned point has the following values:
	//			x	= Screen x
	//			y	= Screen y
	//			z	= Raster 1/z
	//
	//**************************************
	{
		float f_inv_y = 1.0 / v3_cam.tY;

		return CVector3<>
		(
			(v3_cam.tX * f_inv_y) * tlr2Project.tlrX,
			(v3_cam.tZ * f_inv_y) * tlr2Project.tlrY,
			f_inv_y * fInvZAdjust
		);
	}

	//******************************************************************************************
	//
	CVector3<> ProjectPointParallel
	(
		const CVector3<>& v3_cam		// A camera-space point to project to screen space.
	) const
	//
	// Returns:
	//		The point, scaled and axis-transformed to screen space.
	//
	// Similar to ProjectPointPerspective, but without the perspective divide.
	// Also, the resulting Z is proportional to input Y, and the adjust of Z by 
	// fInvZAdjust is unnecessary.
	//
	//**************************************
	{
		return CVector3<>
		(
			(v3_cam.tX) * tlr2Project.tlrX,
			(v3_cam.tZ) * tlr2Project.tlrY,
			v3_cam.tY
		);
	}

	//
	//	Overrides.
	//

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const override;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer) override;

private:

	// Disable copy and assign (use prdtCopy() below?).
	CCamera(const CCamera&);

	CCamera& operator =(const CCamera&);

	//*****************************************************************************************
	virtual void CreatePrediction();

};


#endif

