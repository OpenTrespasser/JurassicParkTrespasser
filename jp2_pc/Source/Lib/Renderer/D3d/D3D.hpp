/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Bugs:
 *		Querying the IDirect3D interface crashes on one of the NT machines (not on others).
 *		So for now, the _NTSDK flag is used to disable even querying IDirect3D.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/D3d/D3D.hpp                                              $
 * 
 * 9     98/02/10 12:59 Speter
 * Old CSArray changed to CMSArray.
 * 
 * 8     96/11/27 19:32 Speter
 * Updated for change to CCom: ptCom member changed to ptPtr.
 * 
 * 7     10/08/96 9:11p Agrant
 * Since bool is now a typedef, the hack around D3D.h is no longer needed.
 * 
 * 6     96/09/23 17:08 Speter
 * Replaced separate D3DDEVICEDESCs for hardware and software with a single one, either hardware
 * or software depending on underlying hardware.
 * 
 * 5     96/08/29 12:47 Speter
 * Disable querying IDirect3D interface under NT, due to unknown problem.
 * 
 * 4     96/08/19 16:04 Speter
 * Added bValid flag to CDirect3D class.  Now app can check whether Direct3D support exists.
 * 
 * 3     96/08/19 13:06 Speter
 * Changed name of initD3D to InitD3D.
 * Added CInitDD instance to Direct3D constructor.
 * 
 * 2     96/08/14 11:02 Speter
 * Changed type prefixes, changed pD3DMain to D3D.
 * Added CD3D::CError class for error reporting.
 * Added SD3DInstruction, and subtypes, for use in filling execute buffers.
 * Added CD3DMaterial class.
 * 
 * 1     96/08/09 10:54 Speter
 * First version, basic D3D functionality.
 * 
 **********************************************************************************************/

#ifndef HEADER_D3D_D3D_HPP
#define HEADER_D3D_D3D_HPP

#include <DirectX/D3D.h>
#undef min
#undef max

#include "Lib/W95/DD.hpp"

//
// Library type prefixes.
//
// GUID				: guid
// IDirect3D		: d3d
// D3DDEVICEDESC	: d3ddevcap
//

class CDirect3D: public CCom<IDirect3D>
// Prefix: d3d
{
public:
	bool	bValid;						// True if support for Direct3D exists.

	//**********************************************************************************************
	//
	class CError
	//
	// An error handling class for Direct3D errors.
	// Uses the assign operation to make error termination easy.
	// You only need the one that's declared below.
	//
	//**********************************
	{
	public:

		//**************************************************************************************
		//
		// Assignment operator.
		//

		//**************************************************************************************
		//
		void CError::operator =
		(
			int i_err					// An error code returned by a DirectDraw function.
		);
		//
		// Displays an error message corresponding to i_err if i_err != 0. 
		// Assign a DirectDraw return code to this object to invoke the error handling.
		//
		//******************************
	};

	// Include a CError variable.
	CError		err;

	struct SDeviceDesc
	// Prefix: devd
	{
		GUID			Guid;				// ID needed to instantiate.
		char*			strName;			// Short name.
		char*			strDesc;			// More descriptive name.
		D3DDEVICEDESC	d3ddevcap;			// Device caps (hardware or software).
		bool			bIsHW;				// Hardware acceleration active.

		SDeviceDesc()
		{
		}

		SDeviceDesc(GUID* pguid, char* str_name, char* str_desc, 
			D3DDEVICEDESC* pd3ddevcap_hw, D3DDEVICEDESC* pd3ddevcap_sw)
		{
			Guid		= *pguid;
			strName		= str_name;
			strDesc		= str_desc;
			bIsHW		= pd3ddevcap_hw->dcmColorModel;

			if (bIsHW)
				d3ddevcap	= *pd3ddevcap_hw;
			else
				d3ddevcap	= *pd3ddevcap_sw;

			// A driver's caps are valid if the dcmColorModel is a non-zero value.
		}
	};

	// Array of possible devices.
	CMSArray<SDeviceDesc, 5>	sadevdList;

	CDirect3D();
};

// The sole instance.
extern CDirect3D*	pD3D;
#define D3D			(*pD3D)

// A per-file instance.
static CInitPtr<CDirect3D>	InitD3D(pD3D);

//**********************************************************************************************
//
// D3D Instruction types, for use with CD3DExBuf.
//

struct SD3DInstruction: D3DINSTRUCTION
{
	SD3DInstruction(uint8 u1_opcode, uint8 u1_size = 0, uint16 u2_count = 0)
	{
		bOpcode	= u1_opcode;
		bSize	= u1_size;
		wCount	= u2_count;
	}
};

struct SD3DInsStates: SD3DInstruction
{
	SD3DInsStates(uint8 u1_opcode, uint16 u2_count)
		: SD3DInstruction(u1_opcode, sizeof(D3DSTATE), u2_count)
	{
		Assert(u1_opcode == D3DOP_STATELIGHT		||
			   u1_opcode == D3DOP_STATETRANSFORM	||
			   u1_opcode == D3DOP_STATERENDER	);
	}
};

struct SD3DState: D3DSTATE
{
	SD3DState(int i_type, int i_value)
	{
		drstRenderStateType	= (D3DRENDERSTATETYPE) i_type;
		dwArg[0]			= i_value;
	}
};

struct SD3DInsTriangles: SD3DInstruction
{
	SD3DInsTriangles(uint16 u2_count)
		: SD3DInstruction(D3DOP_TRIANGLE, sizeof(D3DTRIANGLE), u2_count)
	{
	}
};

struct SD3DTriangle: D3DTRIANGLE
{
	SD3DTriangle(int i_v1, int i_v2, int i_v3, uint u_flags = D3DTRIFLAG_EDGEENABLETRIANGLE)
	{
		wV1 = i_v1;
		wV2 = i_v2;
		wV3 = i_v3;
		wFlags = u_flags;
	}
};

struct SD3DInsProcessVertices: SD3DInstruction, D3DPROCESSVERTICES
// This structure incorporates the D3DOP_PROCESSVERTICES instruction.
{
	SD3DInsProcessVertices(uint u_op, uint u_start, uint u_count)
		: SD3DInstruction(D3DOP_PROCESSVERTICES, sizeof(D3DPROCESSVERTICES), 1)
	{
		dwFlags	= u_op;
		wStart	= u_start;
		wDest	= u_start;
		dwCount	= u_count;
	}
};

//**********************************************************************************************
//
class CD3DExBuf: public CCom<IDirect3DExecuteBuffer>, public CMArray<char>
//
//	Prefix: d3dexbuf
//
//	A class encapsulating the D3D Execute Buffer.  Contains convenience functions for
//	initialising, locking, executing, etc.  Also provides a CMArray interface for filling
//	the buffer, using the "new" or "<<" operators.
//
//**************************************
{
protected:
	CCom<IDirect3DDevice>	D3DDevice;

public:

	//******************************************************************************************
	void Allocate
	(
		CCom<IDirect3DDevice> d3d_dev,			// The device from which to allocate.
		uint u_size								// Desired size of buffer, in bytes.
	);
	//
	// Allocate the buffer.  Should be done only once.
	//
	// Notes:
	//		Before the buffer can be written, you must call Reset().
	//
	//**********************************

	//******************************************************************************************
	void Reset();
	//
	//	Reset the buffer to be empty, lock it so it can be written.
	//
	//	Notes:
	//		Call this before filling the buffer.
	//
	//**********************************
	
	//******************************************************************************************
	void Finish
	(
		uint	u_vertices = 0				// Number of vertices in buffer.
	);
	//
	//	Set appropriate data when buffer filled.
	//
	//	Notes:
	//		Call this when done filling the buffer.
	//		It is assumed that the vertices are placed *before* the instructions in the buffer.
	//
	//**********************************

	//******************************************************************************************
	void Execute
	(
		IDirect3DViewport*	pd3d_viewport,			// The viewport to render to.
		uint				u_flags					// Rendering flags (D3DEXECUTE_[UN]CLIPPED).
	);
	//
	//	Execute the instructions in the buffer.
	//
	//	Notes:
	//		This must be called between calls to D3DDevice->BeginScene() and EndScene().
	//
	//**********************************

	void Align
	(
		uint	u_multiple,		// Multiple on which to align.
		uint	u_mod = 0		// Modulus to add after alignment.
	)
	{
		Assert(u_multiple % 4 == 0);
		Assert(u_mod % 4 == 0);
		Assert(bPowerOfTwo(u_multiple));
		Assert(u_mod < u_multiple);

		while (( (uint32)(atArray+uLen) & (u_multiple-1)) != u_mod)
			// Insert empty operations.
			*this << SD3DInsTriangles(0);
	}

	void PutInsTriangles
	(
		uint16 u2_count
	)
	{
		// Align it first.
		Align(8, 4);
		*this << SD3DInsTriangles(u2_count);
	}
};


class CD3DMaterial: public CCom<IDirect3DMaterial>
{
public:
	D3DMATERIALHANDLE		d3dHandle;

	void Init(D3DMATERIAL& d3dmat_desc, IDirect3DDevice* pd3d_dev)
	{
		D3D.err = D3D->CreateMaterial(&ptPtr, 0);
		D3D.err = (*this)->SetMaterial(&d3dmat_desc);
		D3D.err = (*this)->GetHandle(pd3d_dev, &d3dHandle);
	}
};
	
#endif


