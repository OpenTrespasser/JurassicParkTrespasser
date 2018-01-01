/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Mode information and documentation for AI Test App interface
 *
 * Bugs:
 *
 * To do:
 *		Implement mouse cursor changes to show UI mode.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/AI/DebugArray.hpp                                                $
 * 
 * 1     2/09/97 8:30p Agrant
 * initial rev
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_AI_DEBUGSTL_HPP
#define HEADER_TEST_AI_DEBUGSTL_HPP


template <class VECTYPE, int iMAX_SIZE>
class debug_array {
public:

	VECTYPE tArray[iMAX_SIZE];	// The shadow array.


	debug_array(const vector<VECTYPE>* v)
	{
		int i = v->size();
		if (iMAX_SIZE < i)
			i = iMAX_SIZE;

		for (i--; i >= 0; i--)
		{
			tArray[i] = (*v)[i];
		}
	}

	debug_array(CGraph<VECTYPE, TReal>* g)
	{
		int i = g->iNumNodes();
		if (iMAX_SIZE < i)
			i = iMAX_SIZE;

		for (i--; i >= 0; i--)
		{
			tArray[i] = g->nNode(i);
		}
	}
	

};






#endif



