/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		CRenderCacheLRU
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderCacheLRUItem.hpp                                   $
 * 
 * 3     5/12/98 6:10p Pkeet
 * Added code to freeze caches.
 * 
 * 2     4/10/98 4:10p Pkeet
 * Fixed an assert.
 * 
 * 1     4/09/98 8:22p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_RENDERCACHELRUITEM_HPP
#define HEADER_LIB_RENDERER_RENDERCACHELRUITEM_HPP

//
// Includes.
//
#include "Lib/Sys/LRU.hpp"


//
// Forward declarations.
//

//*********************************************************************************************
//
class CRenderCacheLRU : public CLRUItem
//
// Element of the cache schedule list.
//
// Prefix: lrit
//
//**************************************
{
	CRenderCache* prencRenderCache;
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	// Initializing constructor.
	CRenderCacheLRU
	(
		CRenderCache* prenc
	)
		: prencRenderCache(prenc), CLRUItem(prenc->iEuthansiaAge)
	{
		Assert(prencRenderCache);
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	virtual void Delete()
	{
		if (rcsRenderCacheSettings.bFreezeCaches)
			return;
		Assert(prencRenderCache->ppartShape);
		prencRenderCache->ppartShape->DeleteRenderCache();
	}

};

#endif // HEADER_LIB_RENDERER_RENDERCACHELRUITEM_HPP