/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *
 * Notes:
 *
 * To Do:
 *		Change Assert's into Assert's.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/Socket.cpp                                          $
 * 
 * 8     9/09/98 12:13p Rwycko
 * reversed connector offset
 * 
 * 7     98/09/08 14:30 Speter
 * Do not deact/re-act around magneting; works better.
 * 
 * 6     9/08/98 12:08p Rwycko
 * if a socket radius query finds another socket, use that socket even if we are "too far" away
 * by a center-to-center measure
 * 
 * 5     9/08/98 11:32a Rwycko
 * don't ever swing a socket
 * 
 * 4     98/08/31 18:58 Speter
 * New pmpFindMagnet signature.
 * 
 * 3     8/26/98 11:37p Agrant
 * full socket implementation  (i hope)
 * 
 * 2     8/26/98 3:17a Agrant
 * more socket implementation- untested
 * 
 * 1     8/25/98 11:10p Agrant
 * initial
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Socket.hpp"

#include "Lib/EntityDBase/Query/QPhysics.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Physics/Magnet.hpp"
#include "Lib/Physics/InfoBox.hpp"
#include "Player.hpp"

	//*****************************************************************************************
	CSocket::CSocket()
	{
		// Register this entity with the message types it needs to receive.
	}

		
	//*****************************************************************************************
	CSocket::CSocket
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CInstance(pgon, pload, h_object, pvtable, pinfo)
	{
		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_object)
		{
			bFILL_INT(iGender, esGender);
			bFILL_FLOAT(rRadius, esRadius);

			pmagMagnet = CMagnet::pmagFindShared(pgon->hAttributeHandle, __pov, pvtable, pload);
		}
		END_OBJECT_HANDLE;
		END_TEXT_PROCESSING;

		// Get the pivot point from our physics box, and scale by our size.
		const CPhysicsInfoBox* pphib = pphiGetPhysicsInfo()->ppibCast();
		AlwaysAssert(pphib);
		v3Connector = - pphib->v3Pivot;

		// Must be tangible to mate.
		AlwaysAssert(pphiGetPhysicsInfo()->bIsTangible());
	}


	//*****************************************************************************************
	bool CSocket::bUse(bool b_repeat)
	{
		if (b_repeat)
			return true;

		// Query for sockets within the radius.
		CPresence3<> pr3;
		pr3.v3Pos = v3Pos();
		CWDbQueryPhysicsBoxFast wqph(CBoundVolSphere(rRadius), pr3);

		CSocket* psock_best = 0;
		TReal	 r_best_dist_sqr = 1000.0f;

		// Select the closest socket.
		foreach (wqph)
		{
			CInstance* pins_near = *wqph;
			CSocket* psock = ptCast<CSocket>(pins_near);
			if (psock)
			{
				if (psock->iGender == iGender)
					continue;

				const CMagnetPair* pmp = NMagnetSystem::pmpFindMagnet(this, psock, set0);
				if (pmp)
				{
					// Already socketed!
					// Remove the specified magnet pair.
					gpPlayer->Drop(CVector3<>(0,0,0));
					NMagnetSystem::RemoveMagnetPair(this, psock);
					return true;
				}

				pmp = NMagnetSystem::pmpFindMagnet(psock, this, set0);
				if (pmp)
				{
					// Already socketed!
					// Remove the specified magnet pair.
					gpPlayer->Drop(CVector3<>(0,0,0));
					NMagnetSystem::RemoveMagnetPair(psock, this);
					return true;
				}


				TReal r_dist_sqr = (v3Pos() - psock->v3Pos()).tLenSqr();
				if (r_dist_sqr < r_best_dist_sqr)// && r_dist_sqr < psock->rRadius * psock->rRadius)
				{
					// Found a new best.
					psock_best = psock;
					r_best_dist_sqr = r_dist_sqr;
				}
			}
		}

		if (psock_best)
		{
			// Who should move?
   			if (psock_best->pphiGetPhysicsInfo()->bIsMovable())
			{
				psock_best->Mate(this);
			}
			else if (pphiGetPhysicsInfo()->bIsMovable())
			{
				Mate(psock_best);
			}
			else
			{
				// Neither socket is moveable.  Do nothing.
				return true;
			}
		}

		return true;
	}

	//*****************************************************************************************
	void CSocket::PickedUp()
	{
		CInstance::PickedUp();
	}

	//*****************************************************************************************
	void CSocket::Mate(CSocket* psock)
	{
		// Teleport me to the right spot, and then magnet me to the target.
		AlwaysAssert(psock);

		//  First, deactivate us both.
//		PhysicsDeactivate();
//		psock->PhysicsDeactivate();

		// Make sure we aren't holding it.
		// Drop anything if we are magnetting it.
		CVector3<> v3_drop_velocity(0,0,0);
		if (this == gpPlayer->pinsHeldObject())
		{
			gpPlayer->Drop(v3_drop_velocity);
		}
		else if (psock == gpPlayer->pinsHeldObject())
		{
			gpPlayer->Drop(v3_drop_velocity);
		}

		//
		// Teleport me to him.
		//


		// Construct a presence for the target teleport destination.
		CPresence3<> pr3_dest = pr3Presence();
		pr3_dest.r3Rot = psock->r3Rot();

		pr3_dest.v3Pos = psock->v3Pos() +
						(psock->v3Connector * psock->fGetScale()) * psock->r3Rot() -
						(v3Connector * fGetScale()) * psock->r3Rot();

		Move(pr3_dest.p3Placement());
	
		//
		//  Magnet me to him.
		//

		NMagnetSystem::AddMagnetPair(this, psock, pmagMagnet);

		//  Reactivate.
//		PhysicsActivate();
//		psock->PhysicsActivate();
	}

	//*****************************************************************************************
	CInstance* CSocket::pinsCopy() const
	{
		// Must return an identical copy!

		// Make sure that the info is not unique.  Not sure what to do when it is unique.
		AlwaysAssert(!pinfoProperties->setUnique[CInfo::eitTHIS]);

		CSocket* psock = new CSocket();

		*psock = *this;

		// And instance pickup magnets or similar add-ons.
		psock->CopyExternalData(this);

		return psock;
	}


#if VER_TEST
	//*****************************************************************************************
	int CSocket::iGetDescription(char *buffer, int i_buffer_length)
	{
		CInstance::iGetDescription(buffer, i_buffer_length);

		char my_buffer[256];
		sprintf(my_buffer, "Gender\t%d\nRadius\t%f\n", iGender, rRadius);
		strcat(buffer, my_buffer);

		AlwaysAssert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif
