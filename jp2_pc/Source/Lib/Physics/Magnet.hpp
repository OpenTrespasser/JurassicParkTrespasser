/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CMagnetPair
 *
 * Bugs:
 *
 * To do:
 *		Implement proper instancing.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/Magnet.hpp                                                $
 * 
 * 41    10/02/98 3:24p Mlange
 * Added GetAttachedMagnetsSlaves() function.
 * 
 * 40    9/05/98 5:18p Agrant
 * save memory in final mode-  magnets do not descend from CPartition in final
 * 
 * 39    98/09/04 21:58 Speter
 * Added sender to Move().
 * 
 * 38    98/08/31 19:02 Speter
 * Changed signatures for RemoveMagnetPair and pmpFindMagnet...now take actual flag set, and if
 * not set0, any flag in set will match.
 * 
 * 37    98/08/28 19:53 Speter
 * bIsIntangible() now returns whether an object is intangified by a magnet.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_MAGNET_HPP
#define HEADER_LIB_PHYSICS_MAGNET_HPP

#include <list>
#include "Lib/EntityDBase/PhysicsInfo.hpp"

#define VER_PARTITION_MAGNETS VER_TEST

enum EMagnetFlag		// Used for flags in CMagnet
// Prefix: emf
{
	emfBREAKABLE,		// The magnet can break.
	emfXFREE,			// The master can rotate about these local axes.
	emfYFREE,
	emfZFREE,
	emfHAND_HOLD,		// Specifies the hand placement when picking up/holding the master object.
						// This magnet should be attached to a single object.
	emfSHOULDER_HOLD,	// Specifies the placement of the shoulder relative to the object when holding it.
						// This implicitly specifies the hand orientation relative to the shoulder.
						// The arm can still swivel about the shoulder.
	emfHINGE,			// Magneted to world with free axes.
	emfLIMITS,			// Hinge magnet has angular limits specified.

	emfFREEZE,			// A world magnet which has no free axes.

	emfXTFREE,			// The master can translate on these world axes.
	emfYTFREE,
	emfZTFREE,

	emfSLIDE,			// World magnet with one or more translational axes free.
	emfINTANGIBLE		// Makes slave object intangible; may be applied to any other magnet.
};

//**********************************************************************************************
//
class CMagnet
//
// Prefix: mag
//
// Class holding magnet info.  Does not specify a connection between any objects (see CMagnetPair).
// Shared wherever possible.
//
//**************************************
{
public:
	//
	//  Member variables.
	//

	CSet<EMagnetFlag> setFlags;		// Holds flags for the magnet.

	float fBreakStrength;			// The breaking point of the magnet, if applicable.
	uint8 u1SubstituteMesh;			//  Only applies if we have a pickup magnet.
									//  Zero means use the default hand.

	//
	// For hinged magnets only: rotation parameters.
	// Currently, same settings used for all DOF.
	//
	float		fRestore;			// Restoring force. Arbitrary units, 0 to 10.
	float		fFriction;			// Rotational damping. Arbitrary units, 0 to 10.
	float		fDrive;				// Drive (velocity?) for hinge.
	float		fAngleMin,			// Angular rotation limits, relative to object's initial position.
				fAngleMax;			// Active only if emfLIMITS flag specified.

	//*****************************************************************************************
	//
	// Constructors and other creation functions.
	//

	//*****************************************************************************************
	CMagnet
	(
		CSet<EMagnetFlag> setemf = set0,
		float f_break = 0,
		float f_restore = 0,
		float f_friction = 0,
		float f_drive = 0,
		float f_ang_min = 0,
		float f_ang_max = 0
	);

	//*****************************************************************************************
	//
	static const CMagnet* pmagFindShared
	(
		const CMagnet& mag 
	);
	// Obtain a CMagnet class that is identical to the argument.
	//
	//**************************

	//*****************************************************************************************
	//
	static const CMagnet* pmagFindShared
	(
		const ::CHandle&	h, 
		CObjectValue*	poval_cmagnet, 
		CValueTable*	pvt, 
		CLoadWorld*		pload
	);
	//
	// Obtain a CMagnet class that has the requested data.
	//
	//**************************

			
	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	bool operator< (const CMagnet& mag) const
	{
		return memcmp(this, &mag, sizeof(CMagnet)) < 0;
	}


	//*****************************************************************************************
	//
	static CInstance* pinsLoadMasterMagnet
	(
		CGroffObjectName* pgon,
		CLoadWorld*	pload,
		const ::CHandle& h,
		CValueTable* pvt
	);
	//
	//**************************

	//*****************************************************************************************
	//
	static CInstance* pinsLoadSlaveMagnet
	(
		CGroffObjectName* pgon,
		CLoadWorld*	pload,
		const ::CHandle& h,
		CValueTable* pvt
	);
	//
	//**************************

	//*****************************************************************************************
	//
	CDir3<> d3Axis() const;
	//
	// Returns:
	//		The hinge axis of this magnet (d3ZAxis if not hinged).
	//
	//**************************

	//*****************************************************************************************
	//
	CVector3<> d3Min() const;
	//
	// Returns:
	//		The direction (in magnet space) of the minimum X axis rotation position.
	//
	//**************************

	//*****************************************************************************************
	CVector3<> d3Max() const;
	//
	// Returns:
	//		The direction (in magnet space) of the maximum X axis rotation position.
	//
	//**************************

};

//**********************************************************************************************
//
class CMagnetPair
#if VER_PARTITION_MAGNETS
					: public CPartition
#endif
//
// Prefix: mp
//
// Specifies an attachment between two instances via a magnet.  By current magnet functionality,
// such attachments can be static, or rotate about 1 or more axes.  There is no displacement.
//
// Unique for every given magnet pair.
//
//**************************************
{
public:
	//
	//  Member variables.
	//

	CInstance*      pinsMaster;		// The master instance.  Must always be present.
	CInstance*		pinsSlave;		// The slave instance.  May be 0, if master is magneted to the
									// world, or a pickup magnet, etc.
	const CMagnet*	pmagData;		// The magnet data.

	CRotate3<>		r3Orig;			// The original master rotation, which hinge limits are relative to.
	CPresence3<>	pr3MagMaster;	// The magnet presence, relative to master instance.
									// Doesn't need the scale info, but the transforms are easier to figure out this way.

public:

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CMagnetPair
	(
		CInstance* pins_master, 
		CInstance* pins_slave, 
		const CMagnet* pmag, 
		const CPresence3<>& pr3_mag_master = pr3Null,
		const CRotate3<>& r3_hinge_rel = r3Null
	);

	virtual ~CMagnetPair();

	//*****************************************************************************************
	//
	//	Member functions.
	//

	//*****************************************************************************************
	//
	CSet<EMagnetFlag> setemfFlags() const;
	//
	// Returns:
	//		The flags for this magnet, plus extra flags if this is a world magnet.
	//
	//**********************************

	//*****************************************************************************************
	//
	const CPresence3<>& pr3RelMaster() const
	//
	// Returns:
	//		The magnet presence, relative to the master object.
	//
	//**********************************
	{
		return pr3MagMaster;
	}

	//*****************************************************************************************
	//
	const CRotate3<>& r3HingeRel() const
	//
	// Returns:
	//		The rotation which hinge limits are relative to.
	//
	//**********************************
	{
		return r3Orig;
	}

	//*****************************************************************************************
	//
	CVector3<> d3Zero() const;
	//
	// Returns:
	//		The direction (in world space) of the zero-point X axis rotation position.
	//
	//**************************

	//*****************************************************************************************
	//
	CVector3<> d3Min() const;
	//
	// Returns:
	//		The direction (in world space) of the minimum X axis rotation position.
	//
	//**************************

	//*****************************************************************************************
	CVector3<> d3Max() const;
	//
	// Returns:
	//		The direction (in world space) of the maximum X axis rotation position.
	//
	//**************************

	//*****************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual const char* strPartType() const
	{ 
		return "CMagnetPair"; 
	}

	//*****************************************************************************************
	virtual const CBoundVol* pbvBoundingVol() const;

	//*****************************************************************************************
	virtual CPresence3<> pr3Presence() const;

	//*****************************************************************************************
	virtual void SetPresence(const CPresence3<> &pr3);

	//*****************************************************************************************
	virtual CPlacement3<> p3Placement() const;

	//*****************************************************************************************
	virtual void SetPlacement(const CPlacement3<>& p3);

	//*****************************************************************************************
	virtual CVector3<> v3Pos() const;
	
	//*****************************************************************************************
	virtual void SetPos(const CVector3<>& v3_pos);

	//*****************************************************************************************
	virtual CRotate3<> r3Rot() const;
	
	//*****************************************************************************************
	virtual void SetRot(const CRotate3<>& r3_rot);

	//*****************************************************************************************
	virtual float fGetScale() const;

	//*****************************************************************************************
	virtual void SetScale(float f_new_scale);

	//*****************************************************************************************
	virtual void Move(const CPlacement3<>& p3_new, CEntity* pet_sender = 0);

	//*****************************************************************************************
	virtual int iSizeOf() const;

	//*****************************************************************************************
	virtual char* pcSave(char* pc_buffer) const;  // See instance.hpp

	//*****************************************************************************************
	virtual const char* pcLoad(const char* pc_buffer);  // See instance.hpp

	class CPriv;
};

//**********************************************************************************************
//
namespace NMagnetSystem
//
// Notes:
//		Handles the global magneting database.
//
//**************************************
{
	//******************************************************************************************
	//
	// Functions.
	//

	//*****************************************************************************************
	//
	void AddMagnetPair
	(
		CInstance*				pins_master,	// The instance to which the master magnet is attached.
		CInstance*				pins_slave,		// The instance to which the slave magnet is attached.
		const CMagnet*			pmag,			// The actual data for the magnet pair.
		const CPresence3<>&		pr3_mag_master = pr3Null,
												// The presence of the magnet, relative to master.
		const CRotate3<>&		r3_hinge_rel = r3Null
												// The rotation that hinge limits are relative to.
	);
	//
	// Creates and adds a magnet pair.
	//
	//**************************

	//*****************************************************************************************
	//
	void RemoveMagnetPair
	(
		CInstance*			pins_master,	// The master object.
		CInstance*			pins_slave,		// The slave object, or 0 if magneted to world.
		CSet<EMagnetFlag>	setemf = set0	// The expected flags; if not set0, one must match.
	);
	//
	// Removes and deletes any magnet pairs matching the objects and flags, 
	// and updates physics system accordingly.
	//
	//**************************

	//*****************************************************************************************
	//
	void RemoveMagnets
	(
		CInstance* pins,				// The instance whose magnets to remove.
		bool b_remove_masters = true,	// Whether to remove its master magnets.
		bool b_remove_slaves = true		// Whether to remove its slave magnets.
	);
	//
	// Removes all magnets attached to pins, and updates physics system accordingly.
	//
	//**********************************

	//*****************************************************************************************
	//
	void RemoveAllMagnets
	(
	);
	//
	// Removes all magnets.
	//
	//**********************************

	//*****************************************************************************************
	//
	void SetFrozen
	(
		CInstance* pins,		// Instance to freeze/unfreeze.
		bool b_frozen
	);
	//
	// Freezes or unfreezes the instance utilising the magnet system;
	// if frozen, instance acts immovable.
	//
	//**************************

	//*****************************************************************************************
	//
	bool bIsFrozen
	(
		const CInstance* pins	// Instance to check.
	);
	//
	// Returns:
	//		Whether this instance is temporarily "frozen" by a static world magnet.
	//
	//**************************

	//*****************************************************************************************
	//
	bool bIsIntangible
	(
		const CInstance* pins	// Instance to check.
	);
	//
	// Returns:
	//		Whether this instance is temporarily made intangible by some magnet.
	//
	//**************************

	//*****************************************************************************************
	//
	float fAngleRotated
	(
		const CInstance* pins	// Instance to check.
	);
	//
	// Returns:
	//		If this object is hinged to the world, the angle (in radians) it has rotated 
	//		from its initial position; else 0.
	//
	//**************************

	//*****************************************************************************************
	//
	CInstance* pinsFindMaster
	(
		CInstance* pins,		// Object to check for masters.
		CInstance* pins_super_master = 0
								// Ultimate master to stop before.
	);
	//
	// Returns:
	//		The ultimate master instance of pins, up to but not including pins_super_master,
	//		if given.
	//
	//**************************

	//*****************************************************************************************
	//
	void GetAttachedMagnets
	(
		CInstance* pins,			// An instance potentially in a magnet group.
		std::list<CMagnetPair*>* plsmp	// Returns list of magnet pairs attaching this instance
									// to others.
	);
	//
	// Finds the set of all magnets attached to pins.
	//
	// The first entry is a dummy magnet pair containing the master instance of the group
	// as pinsSlave, and 0 for pinsMaster.
	// 
	// Every other entry indicates a magnet attachment.  pinsMaster exists for each entry.
	// If pinsSlave exists, pinsMaster is magneted to it; otherwise, to the world, or a 
	// special magnet.
	//
	// The set of unique instances in a magnet group consists of all the non-zero pinsSlaves
	// in the magnet list.
	//
	//**************************

	//*****************************************************************************************
	//
	void GetAttachedMagnetsSlaves
	(
		CInstance* pins,				// An instance potentially in a magnet group.
		CDArray<CMagnetPair*>* pdamp	// Returns list of magnet pairs attaching this instance
										// to others.
	);
	//
	// Finds the set of all slave magnets attached to pins.
	//
	// Every entry indicates a magnet attachment.  pinsMaster exists for each entry.
	// If pinsSlave exists, pinsMaster is magneted to it; otherwise, to the world, or a 
	// special magnet.
	//
	// The set of unique instances in a magnet group consists of all the non-zero pinsSlaves
	// in the magnet list.
	//
	//**************************

	//*****************************************************************************************
	//
	const CMagnetPair* pmpFindMagnet
	(
		const CInstance*	pins_master,		// The master instance.
		const CInstance*	pins_slave,			// The slave instance, or 0 if magneted to world.
		CSet<EMagnetFlag>	setemf = set0		// The expected flags; if not set0, one must match.
	);
	//
	// Returns:
	//		The first magnet pair attaching master to slave with the specified flags.
	//		or 0 if none.
	//	
	//	Notes:
	//		Pass set0 in for setemf if you don't care about the flags.
	//
	//**********************************

	//*****************************************************************************************
	//
	inline const CMagnetPair* pmpFindMagnet
	(
		const CInstance* pins,				// Instance to check.
		CSet<EMagnetFlag> setemf			// The expected flags; if not set0, one must match.
	)
	//
	// Returns:
	//		The first magnet pair attached to pins which has any of setemf attributes,
	//		or 0 if none.
	//
	//**********************************
	{
		return pmpFindMagnet(pins, 0, setemf);
	}

	//*****************************************************************************************
	//
	void QueryMagnets
	(
		TPartitionList& rlsple,			// List of TPartitionElements to add magnets to.
		const CPartition* ppart_query	// Partition volume to query inside of.
	);
	//
	// Adds all CMagnetPairs that are within ppart_query to the rlsple list.
	//
	//**********************************

	//*****************************************************************************************
	//
	void DrawMagnets
	(
		CInstance*		pins,		// The instance whose magnets we draw.
		CDraw&			draw,		// The drawing class.
		CCamera&		cam			// The view camera.
	);
	//
	// Removes and deletes a magnet pair.
	//
	//**************************

	//*****************************************************************************************
	//
	char* pcSave(char *  pc_buffer);
	//
	// Saves the magnet tables.
	//
	//**************************

	//*****************************************************************************************
	//
	const char* pcLoad(const char* pc_buffer);
	//
	// Loads the magnet tables.
	//
	//**************************
};

#endif