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
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MovementPrediction.cpp                                $
 * 
 * 6     10/01/98 12:28a Asouth
 * Delete[] what had been new[]
 * 
 * 5     97/08/05 3:04p Pkeet
 * Fixed bug.
 * 
 * 4     97/08/05 3:03p Pkeet
 * Fixed bug.
 * 
 * 3     97/08/05 11:48a Pkeet
 * Fixed bug in movement prediction.
 * 
 * 2     97/08/04 6:31p Pkeet
 * Disabled the call to 'UpdatePrediction' in the TestPrediction function.
 * 
 * 1     97/07/31 1:37p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/Transform/Transform.hpp"
#include "Lib/Transform/Presence.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "MovementPrediction.hpp"


//
// Function prototypes.
//

//*********************************************************************************************
//
void Roll
(
	      CPresence3<>* apr3,
	const CPresence3<>& pr3,
	                int i_num
);
//
// Shifts all the elements in the given array up by one.
//
//**************************************


//
// Class implementations.
//

//*********************************************************************************************
//
// Implementation of class CPredictMovement
//

	//*****************************************************************************************
	//
	// CPredictMovement constructor and destructor.
	//

	//*****************************************************************************************
	CPredictMovement::CPredictMovement(const CPresence3<>& pr3_track, int i_std_lookahead,
		                               int i_track)
		: iNumTrack(i_track), iStandardLookahead(i_std_lookahead),
		  pr3StandardPrediction(pr3_track)
	{
		Assert(iNumTrack > 0);

		apr3Previous = new CPresence3<>[iNumTrack];
		ClearTracking();
	}

	//*****************************************************************************************
	CPredictMovement::~CPredictMovement()
	{
		delete[] apr3Previous;
	}

	//*****************************************************************************************
	//
	// CPredictMovement member functions.
	//

	//*****************************************************************************************
	void CPredictMovement::UpdatePrediction(const CPresence3<>& pr3)
	{
		// If there is insufficient data for prediction, just add the current presence.
		if (iPresencesAdded < iNumTrack)
		{
			apr3Previous[iNumTrack - iPresencesAdded - 1] = pr3;
			++iPresencesAdded;
		}
		else
		{
			Roll(apr3Previous, pr3, iNumTrack);
		}

		// Make the standard prediction.
		pr3StandardPrediction = pr3GetPrediction(iStandardLookahead);
	}
	
	//*****************************************************************************************
	void CPredictMovement::ClearTracking()
	{
		iPresencesAdded = 0;
	}

	//*****************************************************************************************
	CPresence3<> CPredictMovement::pr3GetPrediction(int i_lookahead) const
	{
		Assert(i_lookahead > 0);

		// If there is insufficient data for prediction, just use the current presence.
		if (iPresencesAdded < iNumTrack)
			return pr3StandardPrediction;

		//
		// The current implementation will use only the three most recent data points to
		// project points into the future, and will not attempt to predict orientation.
		// The three points involved in the prediction correspond to the first three entries
		// in the tracking array:
		//
		//		P2		apr3Previous[0]		(Current position)
		//		P1		apr3Previous[1]		(Current position)
		//		P0		apr3Previous[2]		(Current position)
		//
		// The array has been arranged this way to ensure compatibility if this function
		// should be extended; if P2 was aprPrevious[2] the tracker would have to ensure
		// the most current value was at aprPrevious[2].
		//
		// The prediction is made by combining the first and second derivates, and solving
		// the Taylor expansion:
		//
		//                  ,     ,, 2        ,,, 3
		//		Pnew = P + P t + P  t    ( + P   t )
		//                       -----   -----------
		//                         2     (    6    )
		//
		// Note that currently t is assumed to be a constant interval of 1, and therefore
		// can be ignored.
		//
		//		P   = P2
		//
		//		 ,    P1 - P0
		//		P   = ------- = P1 - P0
		//		      t1 - t0
		//
		//		       P2 - P1   P1 - P0
		//		       ------- - -------
		//		 ,,    t2 - t1   t1 - t0
		//		P   = -------------------
		//		         (t2 - t0) / 2
		//
		// which simplifies to:
		//
		//		 ,,
		//		P   = P2 - 2P1 + P0 (given that delta 't' always equals one)
		//
		CPresence3<> pr3_predicted = apr3Previous[0];
		float f_t = float(i_lookahead);

		// Calculate derivatives.
		CVector3<> v3_p_prime = apr3Previous[0].v3Pos - apr3Previous[1].v3Pos;
		CVector3<> v3_p_double_prime = apr3Previous[0].v3Pos - apr3Previous[1].v3Pos * 2.0f +
			                           apr3Previous[2].v3Pos;

		// Calculate the new position.
		CVector3<> v3_predicted = apr3Previous[0].v3Pos + v3_p_prime * f_t +
			                      v3_p_double_prime * (f_t * f_t * 0.5f);

		// Add values to the returned presence.
		pr3_predicted.v3Pos = v3_predicted;

		// Return the predicted presence.
		return pr3_predicted;
	}

	//*****************************************************************************************
	CPresence3<> CPredictMovement::pr3GetStandardPrediction() const
	{
		return pr3StandardPrediction;
	}


//
// Function implementations.
//

//*********************************************************************************************
void Roll(CPresence3<>* apr3, const CPresence3<>& pr3, int i_num)
{
	// Shift the buffer.
	for (int i = i_num - 1; i > 0; --i)
		apr3[i] = apr3[i - 1];

	// Add the new value.
	apr3[0] = pr3;
}

//*********************************************************************************************
void PrintPos(CConsoleEx& rcon, CVector3<> v3)
{
	rcon.Print("%1.2f, %1.2f, %1.2f", v3.tX, v3.tY, v3.tZ);
}

//
// For test only.
//
const int iTestTrack = 8;
CPresence3<> apr3TestTrack[iTestTrack];

//*********************************************************************************************
void PrintPredictions(int i_to = 4)
{
	conPredictMovement.Print("\n");
	for (int i = 0; i < i_to; ++i)
	{
		conPredictMovement.Print("Predicted (%ld) x, y, z: ", i);
		PrintPos(conPredictMovement, apr3TestTrack[i].v3Pos);
		conPredictMovement.Print("\n");
	}
}

//*********************************************************************************************
void TestPrediction(CPredictMovement* ppm)
{
	Assert(ppm);

	// Settings.
	conPredictMovement.Print("Loaded: %ld, Tracking: %ld\n\n", ppm->iPresencesAdded,
		                     ppm->iNumTrack);

	// Update.
	//ppm->UpdatePrediction();
	Roll(apr3TestTrack, ppm->pr3GetStandardPrediction(), iTestTrack);

	// Inform.
	conPredictMovement.Print("Current x, y, z  : ");
	PrintPos(conPredictMovement, ppm->apr3Previous[0].v3Pos);
	conPredictMovement.Print("\n");
	conPredictMovement.Print("Predicted x, y, z: ");
	PrintPos(conPredictMovement, apr3TestTrack[ppm->iStandardLookahead].v3Pos);
	conPredictMovement.Print("\n");

	// Error.
	conPredictMovement.Print("\n");
	CVector3<> v3_diff = ppm->apr3Previous[0].v3Pos -
		                 apr3TestTrack[ppm->iStandardLookahead].v3Pos;
	conPredictMovement.Print("Error            : %1.2f\n", v3_diff.tLen());

	// Additional.
	PrintPredictions();
}
