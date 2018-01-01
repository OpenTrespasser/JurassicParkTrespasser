/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		A movement prediction object.
 *
 * Bugs:
 *
 * To do:
 *		Add rotational prediction.
 *
 * Notes:
 *		The current implementation will only predict position, not rotational information. The
 *		inclusion of the entire presence instead of a CVector3<> is for completeness of the
 *		interface only.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MovementPrediction.hpp                                $
 * 
 * 2     97/08/05 11:48a Pkeet
 * Fixed bug in movement prediction.
 * 
 * 1     97/07/31 1:37p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_MOVEMENTPREDICTION_HPP
#define HEADER_LIB_ENTITYDBASE_MOVEMENTPREDICTION_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CPredictMovement
//
// A movement prediction object.
//
//**************************
{
	int          iNumTrack;			// Number of frames to track.
	CPresence3<> *apr3Previous;		// Old presences to track.
	int          iPresencesAdded;	// Number of presences added to the tracker.
	CPresence3<> pr3StandardPrediction;	// Prediction at set lookahead.
	int          iStandardLookahead;

public:

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//
	
	// Construct using basic parameters.
	CPredictMovement(const CPresence3<>& pr3_track, int i_std_lookahead = 4, int i_track = 4);

	// Destructor.
	~CPredictMovement();

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void UpdatePrediction
	(
		const CPresence3<>& pr3	// New presence.
	);
	//
	// Adds the new presence to the list of tracked presences.
	//
	//**************************

	//*****************************************************************************************
	//
	void ClearTracking
	(
	);
	//
	// Clears the list of tracked presences.
	//
	//**************************

	//*****************************************************************************************
	//
	CPresence3<> pr3GetPrediction
	(
		int i_lookahead = 4			// Number of frames to look ahead.
	) const;
	//
	// Returns a presence giving the predicted position and orientation.
	//
	//**************************

	//*****************************************************************************************
	//
	CPresence3<> pr3GetStandardPrediction
	(
	) const;
	//
	// Returns a presence giving the predicted position and orientation.
	//
	//**************************

	//*****************************************************************************************
	friend void TestPrediction(CPredictMovement* ppm);

};


//
// Global functions.
//

//*********************************************************************************************
//
void TestPrediction(CPredictMovement* ppm);
//
// Compiles stats on the success of movement prediction.
//
//**************************


#endif // HEADER_LIB_ENTITYDBASE_MOVEMENTPREDICTION_HPP
