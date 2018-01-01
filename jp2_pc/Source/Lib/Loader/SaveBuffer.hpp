/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Simple utility functions for saving and loading data from buffers.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/SaveBuffer.hpp                                            $
 * 
 * 3     98.08.14 7:15p Mmouni
 * Disabled specialized save/load bool templates in VC4.2
 * 
 * 2     98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 1     98/02/18 21:12 Speter
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_LOADER_SAVEBUFFER_HPP
#define HEADER_GUIAPP_LOADER_SAVEBUFFER_HPP

//**********************************************************************************************
//
// Global functions.
//

	//******************************************************************************************
	template<class TYPE>  
	inline char* pcSaveT(char* pc, const TYPE& t)
	// Saves t into buffer pc
	{
		*((TYPE*)pc) = t;
		return pc + sizeof(TYPE);
	}
  
	//******************************************************************************************
	template<class TYPE>  
	inline const char* pcLoadT(const char* pc, TYPE* pt)
	// Loads t into buffer pc
	{
		*pt = *((TYPE*)pc);
		return pc + sizeof(TYPE);
	}


	//******************************************************************************************
	template<class TYPE>  
	inline void SaveT(char** ppc, const TYPE& t)
	// Saves t into buffer pc.
	{
		*((TYPE*)*ppc) = t;
		*ppc += sizeof(TYPE);
	}
  
	//******************************************************************************************
	template<class TYPE>  
	inline void LoadT(const char** ppc, TYPE* pt)
	// Loads t into buffer pc.
	{
		*pt = *((TYPE*)*ppc);
		*ppc += sizeof(TYPE);
	}

#if _MSC_VER >= 1100
	//******************************************************************************************
	inline char* pcSaveT(char* pc, const bool& t)
	// Saves t into buffer pc
	{
		*((int*)pc) = t;
		return pc + sizeof(int);
	}
  
	//******************************************************************************************
	inline const char* pcLoadT(const char* pc, bool* pt)
	// Loads t into buffer pc
	{
		*pt = *((int*)pc) != 0;
		return pc + sizeof(int);
	}

	//******************************************************************************************
	inline void SaveT(char** ppc, const bool& t)
	// Saves t into buffer pc.
	{
		*((int*)*ppc) = t;
		*ppc += sizeof(int);
	}
  
	//******************************************************************************************
	inline void LoadT(const char** ppc, bool* pt)
	// Loads t into buffer pc.
	{
		*pt = *((int*)*ppc) != 0;
		*ppc += sizeof(int);
	}
#endif

/*
	//******************************************************************************************
	template<class TYPE>  
	inline void SaveT(char** ppc, const TYPE* pt, int i_num)
	// Saves an array of t into buffer pc.
	{
		for (int i = 0; i < i_num; i++)
			SaveT(ppc, pt[i]);
	}
  
	//******************************************************************************************
	template<class TYPE>  
	inline LoadT(const char** ppc, TYPE* pt, int i_num)
	// Loads an array of t into buffer pc.
	{
		for (int i = 0; i < i_num; i++)
			LoadT(ppc, &pt[i]);
	}
*/

#endif
