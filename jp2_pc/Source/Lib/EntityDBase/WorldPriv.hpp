/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		STL declarations for use by the world database.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/WorldPriv.hpp                                         $
 * 
 * 1     3/16/98 12:49p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_WORLDDPRIV_HPP
#define HEADER_LIB_ENTITYDBASE_WORLDDPRIV_HPP


#include <list.h>
#include <map.h>


class TMapIntPPart : public map<uint32, CPartition*, less<uint32> >
{
};

class TListConstChar : public list<const char*>
{
};

class TListInstance : public list<CInstance*>
{
};


#endif
