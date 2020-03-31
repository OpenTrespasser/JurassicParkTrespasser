/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Classes and functions for loading exported terrain data.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/TerrainLoad.hpp                                         $
 * 
 * 15    7/21/98 8:00p Mlange
 * Can now refine the exported terrain triangulation to the limit of the minimum terrain texture
 * node size.
 * 
 * 14    4/23/98 4:35p Mlange
 * Wavelet quad tree data queries can now specify their frequence cutoff as a ratio of a node's
 * size.
 * 
 * 13    3/30/98 12:47p Mlange
 * Now enforces a minimum resolution of the terrain data.
 * 
 * 12    1/16/98 3:58p Mlange
 * Moved save triangulation code to separate, new function.
 * 
 * 11    12/05/97 1:39p Mlange
 * The load terrain function now no longer converts a .trr  to a .wtd file. There is a new
 * convert terrain function for this. Added some error checking.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_TERRAINLOAD_HPP
#define HEADER_LIB_GEOMDBASE_TERRAINLOAD_HPP


#include <fstream>
#include <float.h>
#include <string>

#include "Lib/Transform/TransLinear.hpp"
#include "Lib/Transform/Vector.hpp"



class CConsoleBuffer;

namespace NMultiResolution
{
	class CTransformedDataHeader;
};

//**********************************************************************************************
//
NMultiResolution::CTransformedDataHeader* ptdhLoadTerrainData
(
	const std::string& str_terrain_name	// Filename of terrain data file, without extension.
);
//
// Load a terrain data file.
//
// Returns:
//		A newly allocated transformed data header class representing the terrain data. The
//		calling function is responsible for deleting it when done. This function returns
//		a null pointer if no .wtd file could be found.
//
//**************************************


//**********************************************************************************************
//
void ConvertTerrainData
(
	const std::string& str_terrain_name,	// Filename of terrain data file, without extension.
	int i_quant_bits,				// Number of bits to use to quantise terrain height values.
	CConsoleBuffer& rcon_text_out
);
//
// Create a .wtd terrain data file from an exported terrain .trr file.
//
// Notes:
//		This function will create a matching .wtd file for a .trr file if none exists yet. If
//		a .wtd file exists, it will be overwritten WITHOUT prompting the user first.
//
//		Creation of a .wtd file will cause progress and file information to be output to the
//		given console. The console output is written to a session file.
//
//		This function will open a new session file for the console unless that console has an
//		open session file already. The filename of the new session file is derived from the given
//		terrain data file, appended with 'Info.txt'. For example, the session file for a 'Plains'
//		terrain file is: 'PlainsInfo.txt'. This function leaves the session file open so that
//		other function may append to it.
//
//**************************************


//**********************************************************************************************
//
void SaveTerrainTriangulation
(
	const std::string& str_terrain_name,	// Filename of terrain data file, without extension.
	TReal r_freq_highpass,			// High pass wavelet coeficient filter, in meters or as ratio.
	bool b_freq_as_ratio,			// Whether cutoff is represented as a ratio of a node's size.
	bool b_conform,
	CConsoleBuffer& rcon_text_out
);
//
// Create a .tri triangulated terrain data file from a .wtd file.
//
// Notes:
//		If a .tri file exists, it will be overwritten WITHOUT prompting the user first.
//
//		Creation of a .tri file will cause progress and file information to be output to the
//		given console. The console output is written to a session file.
//
//		This function will open a new session file for the console unless that console has an
//		open session file already. The filename of the new session file is derived from the given
//		terrain data file, appended with 'TriInfo.txt'.
//
//**************************************


//**********************************************************************************************
//
struct SExportDataInfo
//
// Describes the exported terrain data file.
//
// Prefix: edi
//
//**************************************
{
	int iNumDataPoints;			// Number of data points in file.

	TReal rHighestRes;			// Highest XY resolution (in meters) found in terrain data.

	CVector2<> v2Min;			// Minimum (bottom left) extents of world data.

	TReal rMinHeight;			// Minimum and maximum height values in data.
	TReal rMaxHeight;

	int iNumQuantisationBits;	// Number of bits to use to quantise terrain height values.

	int iMaxIndices;			// The maximum number of indices along the X or Y axis required to uniquely identify
								// each data point, rounded to the next power of two. Note that the last row and
								// column of the data points are ignored in calculating this number.

	SExportDataInfo()
		: iNumDataPoints(0), v2Min(FLT_MAX, FLT_MAX), rMinHeight(FLT_MAX), rMaxHeight(-FLT_MAX)
	{
	}
};


//**********************************************************************************************
//
class CTerrainExportedData
//
// Class for parsing an exported terrain data file.
//
// Prefix: ted
//
// Notes:
//		The exported terrain data file simply consist of list of consecutive X Y Z coordinates
//		in world space. The coordinates are delimited by white space.
//
//		The data points in the file may be in any order. The data points are parsed in the order
//		they appear in the file, starting at the first data point. When a point is found at an
//		identical XY location as a previous point, the Z coordinate of the previous point is
//		overwritten with the Z coordinate of the current point.
//
//		The data file must contain a data point at the minimum XY location (bottom left corner)
//		of its world extent. The XY world extent of the terrain data is forced square and rounded
//		to the next power of two of its corresponding grid index.
//
//		To simply interaction with 3D Max, the last row and column of the data point set are
//		ignored.
//
//**************************************
{
	enum EError
	// Terrain conversion error types.
	// Prefix: eerr
	{
		eerrCANNOT_OPEN_FILE,
		eerrNO_DATA_IN_FILE,
		eerrOLD_TERRAIN_FORMAT,
		eerrNEG_DATA_VALUE,
		eerrNO_LOWER_LEFT_POINT,
		eerrRESOLUTION_TOO_HIGH,
		eerrREAD_PAST_END,
		eerrERROR_READING_FILE,
		eerrSNAPPED_TO_GRID,
		eerrFORCED_EXTENT_POWER_TWO,
		eerrUNKNOWN,
		eerrEND,
	};

	static const char* const astrErrorStrings[];

	bool abErrorDialogDone[eerrEND];		// Flags indicating dialog has been raised for specific error.

	std::ifstream streamFile;

	SExportDataInfo ediInfo;

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Open the given .trr file. Info on progress while parsing file is output to the given console.
	CTerrainExportedData(const char* str_filename, int i_quant_bits, CConsoleBuffer& rcon_text_out);

	~CTerrainExportedData();


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	const SExportDataInfo& ediGetInfo() const
	//
	// Returns:
	//		Description of the exported terrain data file.
	//
	//**************************************
	{
		return ediInfo;
	}


	//******************************************************************************************
	//
	void Reset();
	//
	// Resets (seeks) the interal file pointer to the beginning of the exported terrain data
	// file.
	//
	//**************************************


	//******************************************************************************************
	//
	CVector3<> v3Read();
	//
	// Returns:
	//		The data point at the current file pointer.
	//
	// Notes:
	//		Advances the internal file pointer to the next data point.
	//
	//**************************************


private:
	//******************************************************************************************
	//
	void InitInfo
	(
		int i_quant_bits,
		CConsoleBuffer& rcon_text_out
	);
	//
	// Initialises the 'ediInfo' class member.
	//
	//**************************************


	//******************************************************************************************
	//
	double dRead();
	//
	// Returns:
	//		The single coordinate at the current file pointer, or FLT_MAX if there was some
	//		kind of error.
	//
	// Notes:
	//		Advances the internal file pointer to the next coordinate value..
	//
	//**************************************


	//******************************************************************************************
	//
	void Error
	(
		EError eerr
	);
	//
	// Raise error in conversion process.
	//
	//**************************************
};




#endif
