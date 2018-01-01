/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of D3D.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/D3d/D3D.cpp                                              $
 * 
 * 9     12/20/96 6:11p Mlange
 * Temporarily disabled Direct3D. Textures are now no longer forced to a power of 2.
 * 
 * 8     96/11/27 19:32 Speter
 * Updated for change to CCom: ptCom member changed to ptPtr.
 * 
 * 7     10/31/96 6:41p Agrant
 * changed the _NTSDK ifndef to NO_D3D.
 * To disable D3D, just define NO_D3D in the D3D library build settings.
 * 
 * 6     96/08/29 12:47 Speter
 * Disable querying IDirect3D interface under NT, due to unknown problem.
 * 
 * 5     96/08/19 16:17 Speter
 * Now actually handle lack of Direct3D support, rather than pretending to, as in previous
 * check-in.
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

#define NO_D3D

#include "Common.hpp"
#include "D3D.hpp"

//**********************************************************************************************
//
// CDirect3D implementation.
//

	//******************************************************************************************
	//
	// class CError implementation.
	//

		//**************************************************************************************
		void CDirect3D::CError::operator =(int i_err)
		{
			if (i_err == 0)
				return;

			switch (i_err)
			{
				case D3DERR_BADMAJORVERSION:
					TerminalError(ERROR_D3D, true, "Bad major version");
				case D3DERR_BADMINORVERSION:
					TerminalError(ERROR_D3D, true, "Bad minor version");
				case D3DERR_EXECUTE_CLIPPED_FAILED:
					TerminalError(ERROR_D3D, true, "Execute clipped failed");
				case D3DERR_EXECUTE_CREATE_FAILED:
					TerminalError(ERROR_D3D, true, "Execute create failed");
				case D3DERR_EXECUTE_DESTROY_FAILED:
					TerminalError(ERROR_D3D, true, "Execute destroy failed");
				case D3DERR_EXECUTE_FAILED:
					TerminalError(ERROR_D3D, true, "Execute failed");
				case D3DERR_EXECUTE_LOCK_FAILED:
					TerminalError(ERROR_D3D, true, "Execute lock failed");
				case D3DERR_EXECUTE_LOCKED:
					TerminalError(ERROR_D3D, true, "Execute locked");
				case D3DERR_EXECUTE_NOT_LOCKED:
					TerminalError(ERROR_D3D, true, "Execute not locked");
				case D3DERR_EXECUTE_UNLOCK_FAILED:
					TerminalError(ERROR_D3D, true, "Execute unlock failed");

				case D3DERR_LIGHT_SET_FAILED:
					TerminalError(ERROR_D3D, true, "Light set failed");

				case D3DERR_MATERIAL_CREATE_FAILED:
					TerminalError(ERROR_D3D, true, "Material create failed");
				case D3DERR_MATERIAL_DESTROY_FAILED:
					TerminalError(ERROR_D3D, true, "Material destroy failed");
				case D3DERR_MATERIAL_GETDATA_FAILED:
					TerminalError(ERROR_D3D, true, "Material get data failed");
				case D3DERR_MATERIAL_SETDATA_FAILED:
					TerminalError(ERROR_D3D, true, "Material set data failed");

				case D3DERR_MATRIX_CREATE_FAILED:
					TerminalError(ERROR_D3D, true, "Material create failed");
				case D3DERR_MATRIX_DESTROY_FAILED:
					TerminalError(ERROR_D3D, true, "Material destroy failed");
				case D3DERR_MATRIX_GETDATA_FAILED:
					TerminalError(ERROR_D3D, true, "Matrix get data failed");
				case D3DERR_MATRIX_SETDATA_FAILED:
					TerminalError(ERROR_D3D, true, "Matrix set data failed");

				case D3DERR_SCENE_BEGIN_FAILED:
					TerminalError(ERROR_D3D, true, "Scene begin failed");
				case D3DERR_SCENE_END_FAILED:
					TerminalError(ERROR_D3D, true, "Scene end failed");
				case D3DERR_SCENE_IN_SCENE:
					TerminalError(ERROR_D3D, true, "Currently in scene");
				case D3DERR_SCENE_NOT_IN_SCENE:
					TerminalError(ERROR_D3D, true, "Not currently in scene");

				case D3DERR_SETVIEWPORTDATA_FAILED:
					TerminalError(ERROR_D3D, true, "Viewport set data failed");

				case D3DERR_TEXTURE_CREATE_FAILED:
					TerminalError(ERROR_D3D, true, "Texture create failed");
				case D3DERR_TEXTURE_DESTROY_FAILED:
					TerminalError(ERROR_D3D, true, "Texture destroy failed");
				case D3DERR_TEXTURE_GETSURF_FAILED:
					TerminalError(ERROR_D3D, true, "Texture get surface failed");
				case D3DERR_TEXTURE_LOAD_FAILED:
					TerminalError(ERROR_D3D, true, "Texture load failed");
				case D3DERR_TEXTURE_LOCK_FAILED:
					TerminalError(ERROR_D3D, true, "Texture lock failed");
				case D3DERR_TEXTURE_LOCKED:
					TerminalError(ERROR_D3D, true, "Texture locked");
				case D3DERR_TEXTURE_NO_SUPPORT:
					TerminalError(ERROR_D3D, true, "Texture not supported");
				case D3DERR_TEXTURE_NOT_LOCKED:
					TerminalError(ERROR_D3D, true, "Texture not locked");
				case D3DERR_TEXTURE_SWAP_FAILED:
					TerminalError(ERROR_D3D, true, "Texture swap failed");
				case D3DERR_TEXTURE_UNLOCK_FAILED:
					TerminalError(ERROR_D3D, true, "Texture unlock failed");

				default:
					DirectDraw::err = i_err;
			}
		}


	static HRESULT PASCAL EnumDevicesCallback(
		GUID* pguid, char* str_desc, char* str_name, 
		D3DDEVICEDESC* pd3ddevcap_hw, D3DDEVICEDESC* pd3ddevcap_sw,
		void* p)
	{
		CDirect3D* pd3d = (CDirect3D*)p;
		new(pd3d->sadevdList) CDirect3D::SDeviceDesc(pguid, str_name, str_desc, pd3ddevcap_hw, pd3ddevcap_sw);
		return D3DENUMRET_OK;
	}

	CDirect3D::CDirect3D()
	{
		CInitDD	init_dd;

		// Ensure only pD3D is ever initialised.
		Assert(!pD3D);

#ifndef NO_D3D
		// Create the COM object from the DirectDraw COM object.
		if (DirectDraw::pdd->QueryInterface(IID_IDirect3D, (void**)&ptPtr) != 0
		 || ptPtr == 0)
		{
			// There is no Direct3D support on this system.
			// The rest of the program will have to check for this.
			bValid = false;
			return;
		}

		bValid = true;

		// Get a list of drivers.
		ptPtr->EnumDevices(EnumDevicesCallback, this);
#else
		bValid = false;
#endif
	}

// The sole instance.
CDirect3D* pD3D;

//******************************************************************************************
//
// CD3DExBuf implementation.
//

	//******************************************************************************************
	void CD3DExBuf::Allocate(CCom<IDirect3DDevice> d3d_dev, uint u_size)
	{
		Assert(D3D.bValid);

		// Ensure we haven't already allocated it.
		Assert(!ptPtr);

		// Create an execute buffer of the desired size.
		CDDSize<D3DEXECUTEBUFFERDESC>	d3d_exbuf_desc;

		d3d_exbuf_desc.dwFlags = D3DDEB_BUFSIZE;
		d3d_exbuf_desc.dwBufferSize = u_size;
		D3D.err = d3d_dev->CreateExecuteBuffer(&d3d_exbuf_desc, &ptPtr, NULL);

		// Set the array data (we have a max size, but no pointer yet).
		Set(0, u_size);
		D3DDevice = d3d_dev;
	}

	//******************************************************************************************
	void CD3DExBuf::Reset()
	{
		// Create an execute buffer of the desired size.
		CDDSize<D3DEXECUTEBUFFERDESC>	d3d_exbuf_desc;

		// Lock the execute buffer so it can be filled.
		D3D.err = (*this)->Lock(&d3d_exbuf_desc);

		// Clear it out for good measure.
		memset(d3d_exbuf_desc.lpData, 0, d3d_exbuf_desc.dwBufferSize);

		// Set the array data with the pointer.
		atArray = (char*) d3d_exbuf_desc.lpData;
		uLen = 0;
	}
	
	//******************************************************************************************
	void CD3DExBuf::Finish(uint	u_vertices)
	{
		if (uLen == 0)
		{
			// We have an empty buffer, so merely unlock and return.
			D3D.err = ptPtr->Unlock();
			return;
		}

		// Add the OP_EXIT command.
		*this << SD3DInstruction(D3DOP_EXIT);

		// Prepare for Execute command.
		// Unlock the buffer, as we're done filling it.
		D3D.err = ptPtr->Unlock();

		// Set the stats for it.
		CDDSize<D3DEXECUTEDATA>		d3d_exdata;

		d3d_exdata.dwVertexOffset		= 0;
		d3d_exdata.dwVertexCount		= u_vertices;
		d3d_exdata.dwInstructionOffset	= u_vertices * sizeof(D3DTLVERTEX);

		// uLen is the total amount of data in the buffer, so the instruction length
		// is uLen minus the amount of data for the vertices.
		d3d_exdata.dwInstructionLength	= uLen - d3d_exdata.dwInstructionOffset;

		D3D.err = ptPtr->SetExecuteData(&d3d_exdata);
	}

	//******************************************************************************************
	void CD3DExBuf::Execute(IDirect3DViewport* pd3d_viewport, uint u_flags)
	{
		if (uLen == 0)
			return;
		D3D.err = D3DDevice->Execute(*this, pd3d_viewport, u_flags);

		// Check out the results.
		CDDSize<D3DEXECUTEDATA> d3d_exdata;
		(*this)->GetExecuteData(&d3d_exdata);
	}

