/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		The base classes of the world database:
 *
 *			CInfo
 *			CInstance
 *
 *		Useful class that behaves like dynamic_cast for classes derived from CInstance:
 *
 *			ptCastInstance<>
 *
 * Bugs:
 *
 * To do:
 *		Fixed the mess of constructors in CInstance.
 *
 *		Make the CInstance cast identifier functions const. Update the ptCastInstance class
 *		accordingly. See Lib/Renderer/RenderType.hpp for an example.
 *
 *		Consider changing the name of CInstance. It is easily confused with the concept of
 *		instancing/reference counting.
 *
 *		Add support for instancing and reference counting.
 *
 *		Add functions for returning an unique property of CInfo to its shared state.
 *
 *		Improve CInstance::bIsMoving() to handle non-physics motion and be much faster.
 *
 * Notes:
 *		NB: Reader beware! Entries in the world database are also losely referred to as
 *		'objects'. Unfortunately, object is a rather overloaded term! E.g. it is also used to
 *		refer to an instance of a class, among many other things.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Instance.hpp                                         $
 * 
 * 92    98.09.12 12:18a Mmouni
 * Added override of CPartiton method to get RenderType.
 * 
 * 91    98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 90    98/09/02 21:19 Speter
 * Removed i_activity param from Activate().
 * 
 * 89    8/25/98 4:45p Rwyatt
 * Added a reset function for name map and heap
 * 
 * 88    8/10/98 5:23p Rwyatt
 * Removed iTotal and replaced it with a memory counter
 * 
 * 87    98/08/05 20:40 Speter
 * Changed Use() to bUse().
 * 
 * 86    7/29/98 3:04p Agrant
 * helper access functions
 * 
 * 85    98.07.24 1:40p Mmouni
 * Added PickedUp() function.
 * 
 * 84    7/20/98 10:14p Rwyatt
 * Removed iGetDescription in final mode
 * 
 * 83    7/14/98 4:09p Agrant
 * add a "get name by handle" function to CInstance
 * 
 * 82    98.07.09 6:12p Mmouni
 * Use() now takes the bRepeat parameter.
 * 
 * 81    7/03/98 3:46p Pkeet
 * Added the 'SetAlwaysFaceBV' member function.
 * 
 * 80    98/05/26 4:19 Speter
 * Damn.
 * 
 * 79    98/05/26 3:39 Speter
 * Added bCanHaveChildren() override.
 * 
 * 78    5/22/98 3:10a Rwyatt
 * New static member to give an instance name from a handle.
 * 
 * 77    4/24/98 3:35p Pkeet
 * Added the 'fDistanceFromGlobalCameraSqr' member function.
 * 
 * 76    4/21/98 8:27p Agrant
 * Added the CopyExternalData function to allow instancing of external data elements, like
 * pickup magnets.
 * 
 * 75    4/21/98 8:13p Pkeet
 * Removed the 'InitializeForCaching' and 'bContainsCacheableMeshes' member functions.
 * 
 * 74    3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 73    3/12/98 11:11p Agrant
 * Removed old pinsCreate
 * 
 * 72    3/11/98 5:03p Pkeet
 * Added default and copy constructors to 'CInfo.' Also added the 'ReleaseShared' static member
 * function.
 * 
 * 71    3/10/98 3:17p Agrant
 * Added pinsCopy, allowing better, faster instancing.
 * 
 * 70    98/02/18 10:34 Speter
 * Added get/set CPlacement functions to CPartition.  Removed CInstance::ppr3Presence() and
 * replaced with inline pr3GetPresence() and p3GetPlacement().
 * 
 * 69    98/02/12 11:50 Speter
 * Moved SetRenderInfo() etc. to .cpp file.
 * 
 * 68    2/11/98 3:47p Agrant
 * Made pinfoCopy const because, well, it is.
 * 
 * 67    2/06/98 7:20p Pkeet
 * Added the ' bIsBackdrop' member function. Added a the class 'CBackdrop' derived from
 * 'CInstance.'
 * 
 * 66    98/01/12 19:58 Speter
 * Removed obsolete SetPartitionProperties().
 * 
 * 65    12/14/97 11:34a Agrant
 * Moved Use implementation to .cpp
 * Added a iGetDescription proc to describe instances and descendants
 * 
 * 64    12/12/97 9:56p Agrant
 * Use function for instances, called when the player uses the instance.
 * 
 * 63    12/05/97 4:13p Agrant
 * Made the OnPrefetch() function const
 * 
 * 62    97/11/15 10:54p Pkeet
 * Added a 'const' to the 'bContainsMovingObject' member function.
 * 
 * 61    97/11/14 7:36p Pkeet
 * Added the 'bContainsCacheableMesh' member function. Changed the meaning of 'bCacheable.'
 * 
 *********************************************************************************************/

#ifndef HEADER_ENTITYDBASE_INTSTANCE_HPP
#define HEADER_ENTITYDBASE_INTSTANCE_HPP

#include <string>
#include <map>
#include "Lib/GeomDBase/Partition.hpp"
#include "Lib/Loader/Fetchable.hpp"
#include "Lib/Sys/FastHeap.hpp"

// The flag that causes "reset" to work.
// Turned off, as SaveScene now performs this function.
#define VER_RESET_POSITIONSxxxx


//*********************************************************************************************
//
// Forward declarations for CInfo.
//
class CRenderType;
class CPhysicsInfo;
class CAIInfo;

//*********************************************************************************************
//
class CInfo
//
// Describes the base properties of an entry in the world database.
//
// Prefix: info
//
// Notes:
//		Currently, the CInfo class describes three base properties of an entry in the world
//		database: its shape (for rendering), its physical definition and its 'meaning' to the
//		AI subsystem. Note that we make as few assumptions as possible about the definition of
//		each of these three data fields here. The various sub-systems that use the world
//		database will expand these definitions further.
//
//		Each of the data elements of the CInfo class can be shared or unique. For example,
//		the shape of an individual object ('object' meaning: entry into the world database) can
//		be shared between many other objects. There may be many trees in the world, but they
//		can all *look* the same. It follows then, that when a shared shape needs to be modified
//		in some way, a copy must be made to ensure that the change is only reflected in the
//		single object that requires it.
//
//		This class nicely hides the instancing mechanism used for obtaining the unique versions
//		of the base properties.
//
//		The first time we make a duplicate of a (single) property referenced by this class, we
//		*must* also make a copy of this class itself. This is because the instances of the
//		CInfo classes are shared between the objects in the world. Just like in the shape
//		example above, this change must not be reflected in all the objects that use it.
//
//**************************
{
public:
	enum EInfoType					
	// Prefix: eit
	{
		eitTHIS,					// References the CInfo itself.
		eitRENDERER,				// References the rendering info.
		eitPHYSICS,					// References the physcis info.
		eitAI						// References the AI info.
	};

	CSet<EInfoType> setUnique;		// Flags indicating which of the properties is unique and therefore writable.


	//
	// These data fields reference the various properties of an entry in the world database. These properties
	// are shared (see above) so must not be tampered with directly. Use the member accessor functions!
	//
private:

	rptr<CRenderType>	prdtRenderInfo;		// Definition for the renderer.
	CPhysicsInfo*		pphiPhysicsInfo;	// The physical properties.
	CAIInfo*			paiiAIInfo;			// The AI information.

public:
	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CInfo
	(
	);

	// Initialiser constructor.
	CInfo
	(
		bool b_is_unique,
		rptr<CRenderType>  prdt = rptr0,
		CPhysicsInfo* pphi = 0,
		CAIInfo*      paii = 0
	);

	// Copy constructor.
	CInfo
	(
		const CInfo& info
	);

	// Dummy copy constructor. Use the virtual copy function below instead.
	// Must be okay for STL to contain CInfos.
//	CInfo(const CInfo& info)
//	{
//		Assert(false);
//	}


	// Destructor.
	~CInfo();


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual CInfo* pinfoCopy
	(
	) const;
	//
	// Copies this.
	//
	// Returns:
	//		A new CInfo, which is therefore unique.
	//
	// Notes:
	//		Only the CInfo itself is unique! The data referenced by this class through its
	//		pointers is not copied/made unique by function.
	//
	//		A class derived from CInfo will probably need to overide this function.
	//
	//**************************

	//*****************************************************************************************
	//
	static CInfo* pinfoFindShared
	(
		rptr<CRenderType>  prdt,
		CPhysicsInfo* pphi,
		CAIInfo*      paii
	);
	//
	// Obtain a CInfo class that has the requested data.
	//
	// Notes:
	//		Gets a CInfo that looks like the one requested. This function will search for an
	//		existing CInfo with the same properties. If one does not exist, this function will
	//		create a new CInfo. Therefore, it may be either shared or be unique.
	//
	//**************************

	//*****************************************************************************************
	//
	static CInfo* pinfoFindShared
	(
		const CInfo* pinfo
	);
	//
	// Obtain a CInfo class that has the requested data.
	//
	// Notes:
	//		Gets a CInfo that looks like the one requested. This function will search for an
	//		existing CInfo with the same properties. If one does not exist, this function will
	//		create a new CInfo. Therefore, it may be either shared or be unique.
	//
	//**************************

	//*****************************************************************************************
	//
	rptr_const<CRenderType> prdtGetRenderInfo() const
	//
	// Gets the rendering properties in a non-writable form.
	//
	//**************************
	{
		return prdtRenderInfo;
	}

	//*****************************************************************************************
	//
	rptr<CRenderType> prdtGetRenderInfoWritable();
	//
	// Gets the shape property in a writable form.
	//
	// Notes:
	//		Creates a unique version of the rendering properties if it is not already unique.
	//
	//**************************
	

	//*****************************************************************************************
	//
	void SetRenderInfo
	(
		rptr<CRenderType> prdt
	);
	//
	// Sets the rendering properties and flags it as unique. Note that the CInfo class needs
	// to be unique itself.
	//
	//**************************



	//*****************************************************************************************
	//
	const CPhysicsInfo*	pphiGetPhysicsInfo() const
	//
	// Gets the physical properties in a non-writable form.
	//
	//**************************
	{
		Assert(pphiPhysicsInfo);
		return pphiPhysicsInfo;
	}

	//*****************************************************************************************
	//
	CPhysicsInfo* pphiGetPhysicsInfoWritable();
	//
	// Gets the physical properties in a writable form.
	//
	// Notes:
	//		Creates a unique version of the physical properties if it is not already unique.
	//
	//**************************

	//*****************************************************************************************
	//
	void SetPhysicsInfo
	(
		CPhysicsInfo* pphi
	);
	//
	// Sets the physics property and flags it as unique. Note that the CInfo class needs to be
	// unique itself.
	//
	//**************************

	//*****************************************************************************************
	//
	const CAIInfo* paiiGetAIInfo() const
	//
	// Gets the AI info in non-writable form.
	//
	//**************************
	{
		return paiiAIInfo;
	}

	//*****************************************************************************************
	//
	CAIInfo* paiiGetAIInfoWritable();
	//
	// Gets the AI info in a writable form.
	//
	// Notes:
	//		Creates a unique version of the AI info if it is not already unique.
	//
	//**************************

	//*****************************************************************************************
	//
	void SetAIInfo
	(
		CAIInfo* paii
	);
	//
	// Sets the AI property and flags it as unique. Note that the CInfo class needs to be
	// unique itself.
	//
	//**************************

	bool operator< 
	(
		const CInfo& info
	) const;

	//*****************************************************************************************
	//
	static void ReleaseShared
	(
	);
	//
	// Releases and destroys shared info objects.
	//
	//**************************

};




//*********************************************************************************************
//
// Forward declarations for CInstance.
//
struct	SInstanceData;
class	CGroffObjectName;
class	CLoadWorld;
class	CHandle;
class	CValueTable;


//*********************************************************************************************
//
typedef std::map<uint32, char*, std::less<uint32> >	TInstanceNameMap;

#define u4INSTANCE_NAMES_VIRTUALSIZE	(8*1024*1024)		// 8Mb for the instance names

//*********************************************************************************************
//
class CInstance : public CPartition, public CFetchable/*zero bytes*/
//
// Base class for entries in the world database.
//
// Prefix: ins
//
// Notes:
//		This is the base class for an entry in the world database. It contains the minimum
//		information required for an 'object' to exist in the world. It describes position,
//		orientation etc., along with the properties defined by CInfo.
//
//		As the world database base class, this class must also define all the 'identifier'
//		functions that are used to identify the exact type of an object in the world. This
//		mechanism is preferred to the dynamic_cast<> construct for performance reasons.
//
//		Note that this class contains a pointer to a CInfo (rather than the CInfo itself)
//		because, as is explained above, CInfo's are shared between the entries in the world
//		database. This class re-implements many of the member functions of CInfo, so to a user
//		of this class it appears as if CInfo is inherited from.
//
//**************************
{
public:
	//*****************************************************************************************
	//
	// Classes for initializing CInstance.
	//

	//*****************************************************************************************
	//
	struct SInit
	//
	// A base class for all types of CInstance classes.
	//
	// Prefix: initins
	//
	//**************************************
	{
	 	CPresence3<>		pr3Presence;		// Placement information.
		rptr<CRenderType>	prdtRenderInfo;		// Rendering information.
		CPhysicsInfo*		pphiPhysicsInfo;	// Physics information.
		CAIInfo*			paiiAIInfo;			// AI information.
		std::string			strName;			// Name of the instance.

		//*************************************************************************************
		//
		// Constructors.
		//

		// Constructor with default presence.
		SInit
		(
			rptr<CRenderType>	prdt = rptr0,		// Rendering information.
			CPhysicsInfo*       pphi = 0,			// Physics information.
			CAIInfo*            paii = 0,			// AI information.
			std::string			str_name = "InitCon"
		)
			: prdtRenderInfo(prdt), pphiPhysicsInfo(pphi), paiiAIInfo(paii), strName(str_name)
		{
		}

		// Constructor with full description.
		SInit
		(
	 		const CPresence3<>&	pr3,				// Placement/scale information.
			rptr<CRenderType>	prdt = rptr0,		// Rendering information.
			CPhysicsInfo*		pphi = 0,			// Physics information.
			CAIInfo*			paii = 0,			// AI information.
			std::string			str_name = "InitCon"
		)
			: pr3Presence(pr3), prdtRenderInfo(prdt), pphiPhysicsInfo(pphi), paiiAIInfo(paii), strName(str_name)
		{
		}
	};

	//string strName;			// A unique text name for debugging purposes.
	
protected:
	static CFastHeap*			pfhInstanceNames;	// Fast heap that contains all the instance names
	static TInstanceNameMap*	pnmNameMap;			// A map for locating a name from a unique ID
	static bool					bDeleted;			// Set to true when the instance names have been deleted

	CInfo* pinfoProperties;	// The properties of this world object. This can be shared with
							// other objects in the world.

	// Protected rather than private so that children can override SetPos and such
	uint         uHandle;	// A unique value for this object.
	CPresence3<> pr3Pres;	// Basic positional information.
	

public:
	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	CInstance();

	// Constructor using the init class.
	CInstance
	(
	 	const SInit& initins			// Initialization structure.
	);

	CInstance
	(
		CInfo* pinfo
	);


	CInstance
	(
		CPresence3<> pr3, 
		CInfo* pinfo
	);

	CInstance
	(
	 	CPresence3<>  pr3,
		rptr<CRenderType>	prdt = rptr0,
		CPhysicsInfo* pphi = 0,
		CAIInfo*      paii = 0
	);

	CInstance
	(
		rptr<CRenderType> prdt,
		CPhysicsInfo* pphi = 0,
		CAIInfo*      paii = 0
	);

	// The basic CInstance constructor for loading from a GROFF file.
	CInstance
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);
	
	
	//*****************************************************************************************
	static CInstance* pinsCreate
	(
//		const char* str_class,			// Name of actual class to instantiate.
//		const SInit& initins,			// Initialisation data.
//		const SInstanceData& insd		// Extra initialisation data (from GROFF).
		CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const ::CHandle&			ph_object,	// Handle to the object in the value table.
		CValueTable*			pvtable,	// The value table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);
	//
	// A constructor-like function which creates a new CInstance or descendent thereof,
	// depending on the str_class argument.
	//

	//*****************************************************************************************
	static CInstance* pinsCreate
	(
		const CInstance* pins_base
	);		// Makes a copy of pins_base
	
	//*****************************************************************************************
	virtual CInstance* pinsCopy
	(
	) const;		// Makes a copy of this

	// Destructor.
	virtual ~CInstance();
	
	//*****************************************************************************************
	//
	// Memory allocation functions.
	//

	//*****************************************************************************************
	//
	void* operator new
	(
		uint u_size_type	// Amount of memory to allocate in bytes.
	);
	//
	// Returns pointer to memory allocated from the next position in the heap.
	//
	//**************************************

	//*****************************************************************************************
	//
	void operator delete
	(
		void *pv_mem
	);
	//
	// Calls the destructor on the object.
	//
	//**************************************

	//*****************************************************************************************
	//
	static void ResetHeap
	(
	);
	//
	// Resets the fast heap used for CInstance memory allocations.
	//
	//**************************

	
	//*****************************************************************************************
	//
	static void ResetNameHeap
	(
	);
	//
	// Resets the fast heap used for CInstance names in debug builds.
	//
	//**************************


	//*****************************************************************************************
	//
	const CPresence3<>& pr3GetPresence() const
	//
	// Returns:
	//		A reference to the instance's contained presence.
	//
	// This function is distinguished from the virtual CPartition::pr3Presence() function.
	// It can be used with CInstance or derived classes for faster access.
	//
	//**********************************
	{
		return pr3Pres;
	}


	//*****************************************************************************************
	//
	const char* strGetInstanceName() const;
	//
	// Returns:
	//		The textual name of the instance
	//
	//**********************************

	//*****************************************************************************************
	//
	static const char* strGetInstanceName(uint32 u4_handle);
	//
	// Returns:
	//		The textual name of the instance with the specified handle
	//
	//**********************************

	//*****************************************************************************************
	//
	void SetInstanceName(const char* str);
	//
	//		Sets the text name of the instance.
	//
	// Returns:
	//		Nothing
	//
	//**********************************


	//*****************************************************************************************
	//
	static void DeleteInstanceNames();
	//
	// Deletes the fast heap that contains the instance names, this should never be called
	// before the load is finished.
	//
	//**********************************

	//*****************************************************************************************
	//
	static char* strInstanceNameFromHandle(uint32 u4_handle);
	//
	// Helper function that will return the name of an instance given a handle
	//
	//**********************************

	//*****************************************************************************************
	//
	const char* strGetUniqueName(char* str) const;
	//
	//
	//**********************************


	//*****************************************************************************************
	//
	const CPlacement3<>& p3GetPlacement() const
	//
	// Returns:
	//		A reference to the instance's contained placement.
	//
	// See above.
	//
	//**********************************
	{
		return pr3Pres;
	}

	//*****************************************************************************************
	virtual void CopyExternalData
	(
		const CInstance* pins
	);
	//
	// Copies any subsidiary data from pins into this.  Only grabs data not normally found within 
	//	the CInstance structure itself such as pickup magnets.
	//
	//**********************************

	//*****************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual float fDistanceFromGlobalCameraSqr() const;

	//*****************************************************************************************
	virtual CPresence3<> pr3Presence() const;

	//*****************************************************************************************
	virtual void SetPresence(const CPresence3<> &pr3);

	//*****************************************************************************************
	virtual CPlacement3<> p3Placement() const;

	//*****************************************************************************************
	virtual void SetPlacement(const CPlacement3<> &p3);

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
	virtual rptr_const<CRenderType> prdtGetRenderType() const;

	//*****************************************************************************************
	virtual rptr<CShape> pshGetShape() const;

	//*****************************************************************************************
	virtual rptr<CMesh> pmshGetMesh() const;

	//*****************************************************************************************
	virtual bool bContainsMovingObject() const;

	//*****************************************************************************************
	virtual void Cast(CInstance** ppins)
	{
		*ppins = this;
	}

	//*****************************************************************************************
	virtual const CBoundVol* pbvBoundingVol() const;


	//*****************************************************************************************
	//
	// Member functions.
	//

	//
	// Access functions for the presence information.  These functions provide encapsulation 
	// should the presence ever change to be an inherited or pointed-to object.
	//

	//*****************************************************************************************
	//
	const CInfo* pinfoGetInfo() const
	//
	// Gets the info.  Used for instancing at load time.
	//
	//**************************
	{
		Assert(pinfoProperties);
		return pinfoProperties;
	}

	//*****************************************************************************************
	//
	virtual void SetAlwaysFaceBV();
	//
	// Resets the bounding volume for objects that always face the camera.
	//
	//**************************

	//*****************************************************************************************
	//
	rptr_const<CRenderType> prdtGetRenderInfo() const
	//
	// Gets the rendering properties in a non-writable form.
	//
	//**************************
	{
		Assert(pinfoProperties);
		return pinfoProperties->prdtGetRenderInfo();
	}

	//*****************************************************************************************
	//
	rptr<CRenderType> prdtGetRenderInfoWritable()
	//
	// Gets the rendering properties in a writable form.
	//
	// Notes:
	//		Creates a unique version of the render data if it is not already unique.
	//
	//**************************
	{
		Assert(pinfoProperties);
		ForceUniqueInfo();
		return pinfoProperties->prdtGetRenderInfoWritable();
	}


	//*****************************************************************************************
	//
	void SetRenderInfo
	(
		rptr<CRenderType> prdt
	);
	//
	// Set the rendering properties and flags it as unique.
	//
	//**************************

	//*****************************************************************************************
	//
	const CPhysicsInfo*	pphiGetPhysicsInfo() const
	//
	// Gets the physical properties in a non-writable form.
	//
	//**************************
	{
		Assert(pinfoProperties);
		return pinfoProperties->pphiGetPhysicsInfo();
	}

	//*****************************************************************************************
	//
	CPhysicsInfo* pphiGetPhysicsInfoWritable()
	//
	// Gets the physical properties in a writable form.
	//
	// Notes:
	//		Creates a unique version of the physical properties if it is not already unique.
	//
	//**************************
	{
		Assert(pinfoProperties);
		ForceUniqueInfo();
		return pinfoProperties->pphiGetPhysicsInfoWritable();
	}

	//*****************************************************************************************
	//
	void SetPhysicsInfo
	(
		CPhysicsInfo* pphi
	);
	//
	// Sets the physics property and flags it as unique.
	//
	//**************************

	//*****************************************************************************************
	//
	const CAIInfo* paiiGetAIInfo() const
	//
	// Gets the AI info in non-writable form.
	//
	//**************************
	{
		Assert(pinfoProperties);
		return pinfoProperties->paiiGetAIInfo();
	}

	//*****************************************************************************************
	//
	CAIInfo* paiiGetAIInfoWritable()
	//
	// Gets the AI info in a writable form.
	//
	// Notes:
	//		Creates a unique version of the AI info if it is not already unique.
	//
	//**************************
	{
		Assert(pinfoProperties);
		ForceUniqueInfo();
		return pinfoProperties->paiiGetAIInfoWritable();
	}


	//*****************************************************************************************
	//
	void SetAIInfo
	(
		CAIInfo* paii
	);
	//
	// Sets the AI property and flags it as unique.
	//
	//**************************

	//
	// Convenience functions which access CInfo functionality.
	//

	//*****************************************************************************************
	//
	void PhysicsActivate();
	//
	// Calls the CPhysicsInfo's Activate().
	//
	//**********************************

	//*****************************************************************************************
	//
	void PhysicsDeactivate();
	//
	// Calls the CPhysicsInfo's Deactivate().
	//
	//**********************************


	//*****************************************************************************************
	//
	virtual bool bIsMoving
	(
	) const;
	//
	// Returns 'true' if the instance is moving or is likely to move.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bCanHaveChildren
	(
	);
	//
	// Returns 'true' if the instance is immovable.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual const char* strPartType
	(
	) const;
	//
	// Returns a partition type string.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bEmptyPart
	(
	) const
	//
	// Returns 'true' if the partition is empty, otherwise returns 'false.'
	//
	// Notes:
	//		Because an instance is an object, as a partition this node is never empty.
	//
	//**************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	virtual bool bIsInstance() const
	//
	// Returns true if this is an instance object.
	//
	//**************************
	{
		return true;
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual bool bUse
	(
		bool b_repeat = false		// Set to true if this is a repeat use (ie the button is held down).
	);
	//
	// Call when the object has been "used".
	//
	// Returns:
	//		Whether there was a non-default use action.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void PickedUp
	(
	);
	//
	// You were just picked up.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual int iGetTeam() const;
	//
	// To which team do you belong?  Zero if none.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual const CAnimate* paniGetOwner() const;
	//
	// To which animate do you belong?  Zero if none.
	//
	//**************************



#if VER_TEST
	//*****************************************************************************************
	//
	virtual int iGetDescription(char *buffer, int i_buffer_length);
	//
	//  A human-readable string describing the instance.
	//
	//	Returns:
	//		The number of bytes used of buffer.
	//
	//**************************
#endif

	//*****************************************************************************************
	//
	void Kill();
	//
	//  Start the delete process.
	//
	//	Notes:
	//		Inform all entities and then remove from world dbase.  Removes from dbase at end
	//		of frame, then deletes instance.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bIsBackdrop()
	//
	// Returns 'true' if the object is a backdrop.
	//
	// Notes:
	//		This function returns 'false' for all regular objects.
	//
	//**************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	// Overloads from CPartition.
	//

	//*****************************************************************************************
	//
	virtual bool bPreload(const CBoundVol* pbv, const CPresence3<>* pr3, bool b_is_contained = false);
	//
	// Preloads object into memory.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual uint32 u4GetUniqueHandle
	(
	) const;
	//
	// Returns a value uniquely representing the object.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void SetUniqueHandle
	(
		uint u_handle
	);
	//
	// Sets a value uniquely representing the object.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual char * pcSave
	(
		char *  pc_buffer
	) const;
	//
	// Saves the instance delta into buffer.
	//
	//	Returns:
	//		A pointer to the first unused byte in buffer.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual const char* pcLoad
	(
		const char *  pc_buffer
	);
	//
	// Sets the instance internal variables based on info in buffer.
	//
	//	Returns:
	//		A pointer to the first unused byte in buffer.
	//
	//**************************************

private:

	//*****************************************************************************************
	//
	void ForceUniqueInfo()
	//
	// Makes sure that the pInfo field points to a unique info structure.
	//
	//**************************
	{
		Assert(pinfoProperties);

		// If the info is not unique, make a unique copy.
		if (!pinfoProperties->setUnique[CInfo::eitTHIS])
			pinfoProperties = pinfoProperties->pinfoCopy();
	}

// Can we reset to the load position state?
#ifdef VER_RESET_POSITIONS
public:
	CPlacement3<> p3Reset;

	void Reset()
	{
		Move(p3Reset);
	}
#endif


	//
	//  Overrides.
	//
public:

	//*****************************************************************************************
	virtual void OnPrefetch(bool b_in_thread) const;

	//*****************************************************************************************
	virtual void OnFetch();

	//*****************************************************************************************
	virtual void OnUnfetch();

	//*****************************************************************************************
	virtual int iSize()
	{
		return sizeof(CInstance);
	}


};


//*********************************************************************************************
//
class CBackdrop: public CInstance
//
// A backdrop object.
//
// Prefix: bdrp
//
// Notes:
//		This object will always render regardless of its distance relative to the far clipping
//		plane.
//
//**************************
{
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	// Load constructor.
	CBackdrop
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	//*****************************************************************************************
	//
	// Member function.
	//

	//*****************************************************************************************
	virtual bool bIsBackdrop()
	{
		return true;
	}

};





//#ifndef HEADER_ENTITYDBASE_INTSTANCE_HPP
#endif
