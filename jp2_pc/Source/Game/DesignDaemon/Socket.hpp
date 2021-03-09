/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		CSocket class
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/Socket.hpp                                          $
 * 
 * 2     8/26/98 3:17a Agrant
 * more socket implementation- untested
 * 
 * 1     8/25/98 11:11p Agrant
 * initial
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_DESIGNDAEMON_SOCKET_HPP
#define HEADER_GAME_DESIGNDAEMON_SOCKET_HPP

#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/Loader/Loader.hpp"

class CMagnet;

//
// Class definitions.
//

//*********************************************************************************************
//
class CSocket : public CInstance
//
// A socket class that can attach arbitrarily to other sockets of different types.
//
// Prefix: sock
//
//**************************************
{
public:
	int			iGender;		// Can only mate with objects of a different gender.
	TReal		rRadius;		// Must be within rRadius to mate.
	CVector3<>	v3Connector;	// When mating, match up the connector points (local space).
	const CMagnet*	pmagMagnet;		// The magnet to use to join with another socket.  (Higher gender wins)

	//*****************************************************************************************
	//
	// Constructors.
	//
	CSocket();

	CSocket
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);


	
	
	
	//*************************************************************************************
	//
	void Mate
	(
		CSocket* psock
	);
	//
	//	Teleports into position and mates with psock.  Moves this.
	//
	//******************************

	
	//
	//	Overrides.
	//

	//*****************************************************************************************
	virtual bool bUse(bool b_repeat) override;

	//*****************************************************************************************
	virtual void PickedUp() override;

	//*****************************************************************************************
	virtual CInstance* pinsCopy() const override;

	//*****************************************************************************************
	virtual void Cast(CSocket** psock) override
	{
		*psock = this;
	}


#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};



#endif // HEADER_GAME_DESIGNDAEMON_SOCKET_HPP
