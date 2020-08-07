#pragma once

#include <type_traits>
#include <cstring>

template<class T> class CDDSize : public T
//	
//	Handy template class for all those DD structs that require a dwSize field to be set properly.
//
//	Example: instead of saying:
//
//		DDSURFACEDESC2	sd;
//		memset(&sd, 0, sizeof(sd));
//		sd.dwSize = sizeof(sd);
//
//	Say:
//
//		CDDSize<DDSURFACEDESC2>	sd;
//
//**************************************
{
public:
	// Constructor just sets everything to 0, then sets the correct dwSize field.
	CDDSize()
	{
		InitStruct(*this);
		
		static_assert(sizeof(*this) == sizeof(T)); //Ensure compatibility of types
		static_assert(std::is_trivially_copyable_v<CDDSize<T>>);
	}

	static void InitStruct(T& structdata)
	{
		std::memset(&structdata, 0, sizeof(structdata));
		structdata.dwSize = sizeof(structdata);

		static_assert(std::is_trivially_copyable_v<T>); //Ensure memset is harmless
	}
};
