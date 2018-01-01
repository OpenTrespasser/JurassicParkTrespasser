/***********************************************************************************************
 * Copyright (c) DreamWorks Interactive 1996
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/PVA.cpp                                                   $
 * 
 * 18    9/27/98 11:41p Agrant
 * removed dead code
 * 
 * 17    6/24/98 4:52p Agrant
 * Fix origin for double joint meashes
 * 
 * 16    6/12/98 3:15p Kmckis
 * Alpha female trex assignments
 * 
 * 15    6/11/98 10:29p Kmckis
 * Added Brachi PVA's - and modified the trike PVA's a bit.
 * 
 * 14    98/06/07 18:57 Speter
 * Added v3JointOrigin(). New PVAs from Kyle.
 * 
 * 13    6/05/98 6:58a Kmckis
 * added PVA 3001 (Male TRex)
 * 
 * 12    5/25/98 12:23p Kmckis
 * New Anne Joint Assignments - changed PVA assignments to be individual functions rather than
 * all PVA's in one function.
 * 
 * 11    5/22/98 1:30p Kmckis
 * More Raptor and Trike Assignments
 * 
 * 10    5/16/98 5:22p Kmckis
 * tits
 * 
 * 9     5/11/98 8:43p Kmckis
 * New Anne assignments.  Initial Trike assignments.
 * 
 * 8     3/19/98 12:32p Kmckis
 * more vert assignments
 * 
 * 7     3/18/98 8:26p Kmckis
 * new vertex assignments for anne's new hand orientation
 * 
 * 6     1/14/98 8:11p Kmckis
 * T-Rex vertex assignments
 * 
 * 5     1/07/98 2:34p Agrant
 * untested PVA text prop method
 * 
 * 4     12/22/97 6:06p Kmckis
 * PVA 2000 assignments
 * 
 * 3     12/20/97 11:52p Agrant
 * wacky bug
 * 
 ***********************************************************************************************/

#include "Common.hpp"
#include "Lib/Transform/Transform.hpp"
#include "Lib/Sys/DebugConsole.hpp"




//**********************************************************************************************
//
void JoinJointsAndPoints
(
	CPArray<uint>       pau_joint_links,
	int i_PVA								// index of the PVA set to use.
)
//
// For each vertex, find the joint to which it is linked.
//
//**************************************
// 
// PVA Assignments Key:
//
// PVA 1: 
// PVA 2: 
// PVA 3: 
// PVA 4: Latest Raptor (Dancing Raptor)
// PVA 5: New Anne Stuff (lots of hand modifications incl. opening/closing)
// PVA 6: Raptor with Jaw assignments
//
// PVA 1000: Anne
// 
// PVA 2000: Velociraptor
//
// PVA 3000: T-Rex
//
// PVA 4000: Triceratops
//
// PVA 5000: Allosaurus
//
// PVA 6000: Para
//
// PVA 7000: Stegosaur
//
// PVA 8000: Compy
//
// PVA 9000: Brachiosaur
//
// PVA 10000: Baby Triceratops
//
// PVA 11000: Baby T-Rex
//
//**************************************
{
	// Now always using data specified PVA!
	AlwaysAssert(i_PVA < 0);
	int i = 0;
	switch (i_PVA)
	{
	case -2:
		// Already taken care of by text props!
		break;
	case -1:
		// Dummy PVA assignments.
		for (i = pau_joint_links.uLen - 1; i >= 0; --i)
		{
			pau_joint_links[i] = 0;
		}
		break;

	default:
		dout << "PVA index not found: " << i_PVA << '\n';
		AlwaysAssert(false);
	}
}

//**********************************************************************************************
CVector3<> v3JointOrigin(int i_PVA, CPArray< CTransform3<> > patf3_joints, int i_last_normal_joint)
{
	if (i_PVA < 0)
	{
		// All text prop PVAs must use the final joint as the origin.
		return patf3_joints[i_last_normal_joint].v3Pos;
	}
	else switch (i_PVA / 1000)
	{
		case 0:
		case 1:
			// Anne. Use joint 0.
			return patf3_joints[0].v3Pos;
		case 2:
		case 3:
		case 5:
		case 6:
		case 8:
		case 11:
		// Bipeds. It's between the hips (12 and 16).
			return (patf3_joints[12].v3Pos + patf3_joints[16].v3Pos) * 0.5;
		case 4:
		case 7:
		case 9:
		case 10:
		// Quad. It's between all 4 hips.
			return (patf3_joints[12].v3Pos + patf3_joints[16].v3Pos +
					patf3_joints[16].v3Pos + patf3_joints[20].v3Pos) * 0.25;
		default:
			AlwaysAssert(0);
			return v3Zero;
	}
}
