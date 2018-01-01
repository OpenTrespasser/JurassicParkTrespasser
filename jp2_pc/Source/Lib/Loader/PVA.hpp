/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		PVA functions.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/PVA.hpp                                                    $
 * 
 * 2     6/24/98 4:52p Agrant
 * Fix origin for double joint meashes
 * 
 * 1     98/06/07 19:10 Speter
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_LOADER_PVA_HPP
#define HEADER_LIB_LOADER_PVA_HPP

//**********************************************************************************************
//
void JoinJointsAndPoints
(
	CPArray<uint>       pau_joint_links,	// Joint links to fill.
	int i_PVA								// Index of the PVA set to use.
);
//
// For each vertex, find the joint to which it is linked.
//
//**************************************

//**********************************************************************************************
//
CVector3<> v3JointOrigin
(
	int i_PVA,								// Index of the PVA set to use.
	CPArray< CTransform3<> > patf3_joints,	// Joint array.
	int i_last_normal_joint					// Last joint except for double joints
);
//
// Returns:
//		The object-space position to use as the origin.
//
//**************************************

#endif
