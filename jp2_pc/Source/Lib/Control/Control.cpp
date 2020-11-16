/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Control.hpp.
 *
 * ToDO:
 *
 * Get as much info from the system about the input devices, so the devices read through
 * direct input act like they do when read through the system... I mean things like if the
 * the buttons have been reversed etc.
 *
 * Finish off joystick reader, the current version does not read the buttons.
 *
 * DirectX 5.0 can read joysticks through the directInput API, ie without using joyGetPosEx.
 * This supprt needs adding if NT gets DirectX 5.0, if it does not then this code should be
 * left alone
 *
 ***********************************************************************************************
 *
 * $Log: /JP2_PC/Source/Lib/Control/Control.cpp $
 * 
 * 58    9/16/98 8:51a Shernd
 * New key commands
 * 
 * 57    9/09/98 7:46p Shernd
 * Yet another default keymapping change
 * 
 * 56    9/09/98 10:21a Shernd
 * For trespasser don't execute the ShowCursor statements. 
 * 
 * 55    9/03/98 8:12p Shernd
 * New default key assignments
 * 
 * 54    8/31/98 4:44p Shernd
 * updated for invert mouse
 * 
 * 53    98/08/28 19:49 Speter
 * Added uCMD_THROW.
 * 
 * 52    8/27/98 1:26p Sblack
 * 
 * 51    98/08/24 22:03 Speter
 * Finally, got rid of mouse jump when starting sim.
 * 
 * 50    8/23/98 12:24a Shernd
 * Remove the Punch key
 * 
 * 49    8/19/98 7:11p Shernd
 * Removed the Teleport Cmd and added a Replay Last Voice over cmd
 * 
 * 48    98/08/13 14:42 Speter
 * Added stow and teleport keys (saved in replays).
 * 
 * 47    8/13/98 1:47p Mlange
 * The step and system messages now require registration of their
 * recipients.
 * 
 * 46    8/13/98 9:43a Shernd
 * Defined the number of control items better.
 * 
 * 45    8/10/98 2:35p Shernd
 * Brought the defintions of km_DefaultKeyMapping to the header
 * 
 * 44    8/05/98 10:28p Sblack
 * 
 **********************************************************************************************/

#include "common.hpp"
#include <Lib/W95/WinInclude.hpp>
#include "Control.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgControl.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/Replay.hpp"
#include "Lib/Sys/ConIO.hpp"

#include <mmsystem.h>

#include "Lib/Sys/DWSizeStruct.hpp"

#ifdef USE_DIRECTINPUT
#include <DirectX/DInput.h>
#endif //#ifdef USE_DIRECTINPUT

extern bool         bInvertMouse;
extern bool         bIsTrespasser;

 CInputDeemone* gpInputDeemone = 0;		// Control input.
 
//*********************************************************************************************
// CInputDeemone
// The control entity.  Processes step and system messages.
//
// This knows nothing about any controllers, it simple calls in to the CInput class which 
// knows about them. The reader functions within this class all return a standard form, as many
// can be added as is required.
//
// That class should contain everything required for access to the device, nothing should be in
// this class. This is case for DirectInput and Win32.
//
CInputDeemone::CInputDeemone(myHWND hwnd,myHANDLE hinst,EControlMethod ecm_controls)
	: inp_Input(hwnd,hinst)
{
	b_Capture = false;			// Don't capture until explicitly turned on.
	ecm_Control=ecm_controls;

	SetInstanceName("Input Daemon");

	// Register this entity with the message types it needs to receive.
	  CMessageStep::RegisterRecipient(this);
	CMessageSystem::RegisterRecipient(this);
}


//*********************************************************************************************
//
CInputDeemone::~CInputDeemone()
{
	CMessageSystem::UnregisterRecipient(this);
	  CMessageStep::UnregisterRecipient(this);
}


//*********************************************************************************************
// Turns on/off input capture.
void CInputDeemone::Capture(bool b_state)
{
	if (b_Capture != b_state)			// We only want to do something if state changes.
	{
		b_Capture = b_state;
		inp_Input.Capture(b_state);
	}
}


//*********************************************************************************************
// Process step message: we want to get the current state of keyboard and
// mouse since the last step message.
void CInputDeemone::Process(const CMessageStep& msg_step)
{
	if (crpReplay.bLoadActive())
	{
		// Get the Control message from the replay file.
		crpReplay.PlayMessage(ectReplayChunkControl);
	}
	else if (b_Capture)
	{
		// Directly generate a Control message.
		SInput tin;

		// which control method are we using.....
		switch (ecm_Control)
		{
		case ecm_DefaultControls:
			tin=inp_Input.tinReadDefaultControls();
			break;

		case ecm_Joystick:
			tin=inp_Input.tinReadStandardJoystickControls();
			break;
		}

		tin.fElapsedTime = msg_step.sStep;

		//
		// For now, since the player needs to act whenever a key is down,
		// we send a message when either any key/button is down, or there has been a transition.
		// Perhaps instead the player should simply act every frame, and examine the current key/button state.
		// 

		// Always send the message, even if nothing happens.  That way, the player will stop moving.
		CMessageControl msg(tin);
		msg.Dispatch();
	}
}


//*********************************************************************************************
// Process system message: we respond to start and stop simulation messages,
//  turning on/off control respectively.
// TODO: Respond to system focus messages, turning on/off control.
void CInputDeemone::Process(const CMessageSystem& msg_system)
{
	switch (msg_system.escCode)
	{
		case escSTART_SIM:
			Capture(true);

			// Call the reader once to cancel any mouse movement outside the sim.
			inp_Input.tinReadDefaultControls();
			break;

		case escSTOP_SIM:
			Capture(false);
			break;
	}
}
	


//*********************************************************************************************
// CInput
//
//*********************************************************************************************

// This Key map is copied into the CInput class by the constructor of that class. This global
// table is only here until the constructor has a more suitable place to get the data from.
// It could get the key preferences from a init file or from the registry.
#ifdef USE_DIRECTINPUT

SKeyMapping km_DefaultKeyMapping[KEYMAP_COUNT] =
{
	// DirectInput always returns individual keys so for a generic shift key we need to process 
	// both shift keys. Under Win32 (non DirectInput) on 95 this is difficult and maybe 
	// impossible with certainkeyboard drivers.

	// NOTE: Different key constants defined in DInput.h
	{DIK_LSHIFT,	uCMD_SHIFT},
	{DIK_RSHIFT,	uCMD_SHIFT},
	{DIK_SPACE,	    uCMD_FIRE},

	{DIK_Q,			uCMD_JUMP},
	{DIK_A,			uCMD_CROUCH},
	{DIK_E,			uCMD_STOW},
	{DIK_F,			uCMD_THROW},
    {DIK_R,         uCMD_REPLAYVO},

	// these are not actual actions, they get converted into floating point values before CInput
	// returns the values. They are simply in this table for efficiency reasons.
	{DIK_W,			uBITKEY_RUN},
	{DIK_S,			uBITKEY_WALK},
	{DIK_X,			uBITKEY_BACKUP},
	{DIK_A			uBITKEY_LEFT},
	{DIK_D,			uBITKEY_RIGHT},
};


#else

// The default key mappings for non direct input. Win32 can read the mouse buttons as keys
// so there is no need for use to read the mouse buttons seperate.
// The key codes are VK codes as returned by GetKeyboardState, Win32 will require extra code
// to properly handle the extended keys.
SKeyMapping km_DefaultKeyMapping[KEYMAP_COUNT] =
{
	{VK_LBUTTON,	uCMD_HAND},
	{VK_RBUTTON,	uCMD_GRAB},

	{VK_SHIFT,	    uCMD_SHIFT},
	{VK_CONTROL,	uCMD_CONTROL},
	{VK_SPACE,	    uCMD_USE},

	{'Q',		    uCMD_JUMP},
	{'Z',			uCMD_CROUCH},
	{'E',			uCMD_STOW},
	{'F',			uCMD_THROW},
    {'R',           uCMD_REPLAYVO},     // Replays the last Voice Over

	// see above DIK codes
	{'W',			uBITKEY_RUN},
	{'S',			uBITKEY_WALK},
	{'X',			uBITKEY_BACKUP},
	{'A',			uBITKEY_LEFT},
	{'D',			uBITKEY_RIGHT},
};


#endif //#ifdef USE_DIRECTINPUT



//*********************************************************************************************
// NOTE: cannot get these in the class at the moment because of a messed up header file
//
#ifdef USE_DIRECTINPUT
static LPDIRECTINPUT			lpDI;
static LPDIRECTINPUTDEVICE		lpDIMouse=NULL;
static LPDIRECTINPUTDEVICE		lpDIKeyboard=NULL;
static DIMOUSESTATE				dim_MouseState={0};
#endif



//*********************************************************************************************
//
CInput::CInput(myHWND h,myHANDLE i)
	: hwnd(h), hinst(i)
{
	u4LastButtons = 0;

#ifdef USE_DIRECTINPUT

    HRESULT hr;

    hr = DirectInputCreate((HINSTANCE)hinst,DIRECTINPUT_VERSION,&lpDI,NULL);
	if (hr!=DI_OK)
	{
		lpDI=NULL;
		Assert(0);		// directInput failed.....
		return;
	}

	if (!bOpenMouseDevice(hwnd))
	{
		Assert(0);		// failed to create a mouse device for directinput
		return;
	}

	if (!bOpenKeyDevice(hwnd))
	{
		Assert(0);		// failed to create a keyboard device for directinput
		return;
	}

	// directInput does not flip the mouse buttons around for left handed people
	// so we have to do it. This system is totally flexible so we can fully
	// map the mouse buttons to any others.

	if (GetSystemMetrics(SM_SWAPBUTTON))
	{
		// mouse buttons are swapped...
		au4_mousemap[0]=1;
		au4_mousemap[1]=0;
		au4_mousemap[2]=2;
		au4_mousemap[3]=3;
	}
	else
	{
		// mouse buttons are normal.
		au4_mousemap[0]=0;
		au4_mousemap[1]=1;
		au4_mousemap[2]=2;
		au4_mousemap[3]=3;
	}

#else
	// center the mouse
	RECT rect;
	POINT point;
	GetClientRect((HWND)hwnd, &rect);
	point.x = rect.right/2;
	point.y = rect.bottom/2;
	ClientToScreen((HWND)hwnd, &point);
	i_Xcenter = point.x;
	i_Ycenter = point.y;

	SetCursorPos(i_Xcenter, i_Ycenter);

	tinReadDefaultControls();
#endif

}



//*********************************************************************************************
//
CInput::~CInput()
{
#ifdef USE_DIRECTINPUT

	// remove the mouse if we were using it...
	CloseMouseDevice();
	CloseKeyDevice();

	if (lpDI)
	{
		lpDI->Release();
		lpDI=NULL;
	}

#else
#endif
}



//*********************************************************************************************
// We always use win32 for the joystick because NT has no support for DX5.0 which is required
// for joysticks. joyGetPosEx is an IOCTL call into the driver so it is not slow.
// To do: normalise the values so that turn amounts are in radians.
//
SInput& CInput::tinReadStandardJoystickControls()
{
	CDDSize<JOYINFOEX> ji_stick;

	// Clear keys to off.
	tin_Input.u4ButtonState = 0;
	tin_Input.u4ButtonHit = 0;

	ji_stick.dwFlags=JOY_RETURNX|JOY_RETURNY|JOY_RETURNBUTTONS|JOY_USEDEADZONE|JOY_RETURNCENTERED;
	joyGetPosEx(JOYSTICKID1,&ji_stick);

	tin_Input.v2Rotate.tX = ((float)ji_stick.dwXpos-32768.0F)*0.002;
	tin_Input.v2Rotate.tY = ((float)ji_stick.dwYpos-32768.0F)*0.002;

	tin_Input.v2Move.tY = tin_Input.v2Move.tX = 0;

	return tin_Input;
}


// Constant to convert mickeys to radians.
static const float fMICKEY_SCALE = 1.0 / 200.0;

//*********************************************************************************************
//
SInput& CInput::tinReadDefaultControls()
{
#ifdef USE_DIRECTINPUT

	uint8			au1_key_buf[256];

	// Get Keyboard and mouse state from windows
	GetKeyState(au1_key_buf);

	// Clear keys to off.
	tin_Input.u4ButtonState = 0;
	tin_Input.u4ButtonHit = 0;

	// Walk through the key map and see if any of them are pressed or have been hit.
	for (uint u_count = 0; u_count < KEYMAP_COUNT; u_count++)
	{
		if (au1_key_buf[ km_DefaultKeyMapping[u_count].u4_KeyCode ] & 0x80)
			tin_Input.u4ButtonState |= km_DefaultKeyMapping[u_count].u4_Action;
		if (au1_key_buf[ km_DefaultKeyMapping[u_count].u4_KeyCode ] & 1)
			tin_Input.u4ButtonHit |= km_DefaultKeyMapping[u_count].u4_Action;
	}

	//
	// Done the keyboard, now read the mouse and mouse buttons
	//

	// Get the state of the mouse into the class structure
	GetMouseState();
	
	// Reverse the Y coord, as we want it going UP.
	tin_Input.v2Rotate.tX = (float)dim_MouseState.lX * fMICKEY_SCALE;
	tin_Input.v2Rotate.tY = -(float)dim_MouseState.lY * fMICKEY_SCALE;

	// DirectInput does not read the mouse buttons as keys like Win32 does
	// so we now need to read the mouse buttons....

	// left button is button 0
	if (dim_MouseState.rgbButtons[ au4_mousemap[0] ] & 0x80)
		tin_Input.u4ButtonState |= uCMD_HAND;
	if (dim_MouseState.rgbButtons[ au4_mousemap[0] ] & 1)
		tin_Input.u4ButtonHit |= uCMD_HAND;

	// right button is button 1 on a 2 button mouse, but on a 3 button mouse
	// it could be button 2
	if (dim_MouseState.rgbButtons[ au4_mousemap[1] ] & 0x80)
		tin_Input.u4ButtonState |= uCMD_GRAB;
	if (dim_MouseState.rgbButtons[ au4_mousemap[1] ] & 1)
		tin_Input.u4ButtonHit |= uCMD_GRAB;

	// this could be button 1..I'm not sure and nor is the documentation..
	if (dim_MouseState.rgbButtons[ au4_mousemap[2] ] & 0x80)
		// If the real middle button is pressed (if one is present), then 
		// press both mouse buttons.
		tin_Input.u4ButtonState |= uCMD_HAND | uCMD_GRAB;
	if (dim_MouseState.rgbButtons[ au4_mousemap[2] ] & 1)
		// If the real middle button is pressed (if one is present), then 
		// press both mouse buttons.
		tin_Input.u4ButtonButton |= uCMD_HAND | uCMD_GRAB;

#else		// NON DirectInput

	// Clear keys to off.
	tin_Input.u4ButtonState = 0;
	tin_Input.u4ButtonHit = 0;

	// Walk through the key map and see if any of them are pressed.
	for (uint u_count = 0; u_count < KEYMAP_COUNT; u_count++)
	{
		uint32 u4_action = km_DefaultKeyMapping[u_count].u4_Action;
		uint16 u2_state = ::GetAsyncKeyState(km_DefaultKeyMapping[u_count].u4_KeyCode);

		if (u2_state & 0x8000)
			// Is currently pressed.
			tin_Input.u4ButtonState |= u4_action;

		if (!(u4LastButtons & u4_action) && u2_state)
		{
			//
			// If previous state was up, and any bit of current state set, it was pressed
			// since last call. Set both State and Hit flags.
			// GetAsyncKeyState() is supposed to set this flag only if the button has been pressed
			// since last call, but in reality sets it more often; so we must manually compare
			// against previous state.
			//
			tin_Input.u4ButtonHit   |= u4_action;
			tin_Input.u4ButtonState |= u4_action;
		}
	}

	//conStd.Print("%X-%X ", tin_Input.u4ButtonHit, tin_Input.u4ButtonState);

	u4LastButtons = tin_Input.u4ButtonState;

	// Mouse position is relative to last step: x,y describes signed movement,
	//  negative being left,down and positive being right,up.  The units are
	//  undetermined yet but will have a scale factor.

	POINT   point;
    float   fYMouse;

	GetCursorPos(&point);
	tin_Input.v2Rotate.tX = (point.x - i_Xcenter) * fMICKEY_SCALE;

    fYMouse = -(point.y - i_Ycenter) * fMICKEY_SCALE;
    if (bInvertMouse)
    {
	    fYMouse *= -1.0f;
    }

	tin_Input.v2Rotate.tY = fYMouse;

	// Reset mouse position to center of window, we do this to get a relative
	//  position at each step.  We get the client rectangle each time just in
	//  case the user has moved/resized the window.
	// TODO: take out for DirectInput since it will give us relative direction.
	
	RECT rect;
	GetClientRect((HWND)hwnd, &rect);
	point.x = rect.right/2;
	point.y = rect.bottom/2;
	ClientToScreen((HWND)hwnd, &point);
	i_Xcenter = point.x;
	i_Ycenter = point.y;

	SetCursorPos(i_Xcenter, i_Ycenter);
#endif //#ifdef USE_DIRECTINPUT

	// no matter which of the input systems we used we still need to process
	// the bit keys. 

	// The priority of the move keys is run then walk then backup
	if (tin_Input.u4ButtonState & uBITKEY_RUN)
	{
		tin_Input.v2Move.tY = 1.0f;
	}
	else if (tin_Input.u4ButtonState & uBITKEY_WALK)
	{
		tin_Input.v2Move.tY = 0.5f;
	}
	else if (tin_Input.u4ButtonState & uBITKEY_BACKUP)
	{
		tin_Input.v2Move.tY = -.4f;//-0.3f;
	}
	else
	{
		tin_Input.v2Move.tY = 0.0f;
	}

	// Handle strafing simultaneously, but only if only one is pressed.
	// Strafe speed is walk speed.
	tin_Input.v2Move.tX = 0.0f;
	if (tin_Input.u4ButtonState & uBITKEY_LEFT)
	{
		if (!(tin_Input.u4ButtonState & uBITKEY_RIGHT))
			tin_Input.v2Move.tX = -0.6f;
	}
	else if (tin_Input.u4ButtonState & uBITKEY_RIGHT)
	{
		tin_Input.v2Move.tX = 0.6f;
	}

//	if (tin_Input.u4ButtonState & uCMD_SHIFT)
//		tin_Input.v2Move *= 1.5f;

/*	float f_wasted;
	CVector2<> v2_rotate;
	bool b_wasted;
	if (bReadJoystickSimple(v2_rotate.tX, v2_rotate.tY, f_wasted, b_wasted, b_wasted))
	{
		tin_Input.v2Rotate = v2_rotate;
	}*/

	// clear the bit key values from the button state
	tin_Input.u4ButtonState &= ~(uBITKEY_RUN|uBITKEY_WALK|uBITKEY_BACKUP);
	tin_Input.u4ButtonHit   &= ~(uBITKEY_RUN|uBITKEY_WALK|uBITKEY_BACKUP);

	return tin_Input;
}



//*********************************************************************************************
// Set capture state for input.
void CInput::Capture(bool b_state)
{
#ifdef USE_DIRECTINPUT

	HRESULT		hr;

	if (b_state)
	{
		hr = lpDIMouse->Acquire();
		if (hr!=DI_OK)
		{
			Assert(0);				// cannot acquire mouse
			return;
		}

		hr = lpDIKeyboard->Acquire();
		if (hr!=DI_OK)
		{
			Assert(0);				// cannot acquire the keyboard
			return;
		}

		ShowCursor(FALSE);
	}
	else
	{
		ShowCursor(TRUE);
		lpDIMouse->Unacquire();
		lpDIKeyboard->Unacquire();
	}


#else	// NON DirectInput


    if (!bIsTrespasser)
    {
	    if (b_state)
	    {
		    ShowCursor(FALSE);
        }
	    else
	    {
		    ShowCursor(TRUE);
        }
	}
#endif //#ifdef USE_DIRECTINPUT
}



//*********************************************************************************************
// THESE FUNCTIONS ARE ONLY PRESENT IN THE DIRECTINPUT BUILD
//
#ifdef USE_DIRECTINPUT


//*********************************************************************************************
//
bool CInput::bOpenMouseDevice(myHWND hwnd)
{
	HRESULT	hr;

    //
    // Obtain an interface to the system mouse device.
    //
	if (!lpDI)
	{
		// there is no DIInput Interface....
		return(FALSE);
	}

	//
	// Create the device of the specifed type
	//
    hr = lpDI->CreateDevice(GUID_SysMouse,&lpDIMouse,NULL);
    if (hr!=DI_OK) 
	{
        return FALSE;
    }

    //
    // Set the data format to "mouse format".
    //
    hr = lpDIMouse->SetDataFormat(&c_dfDIMouse);

    if (hr!=DI_OK) 
	{
		CloseMouseDevice();
        return FALSE;
    }

    //
	// Set the cooperativity level.
    //
	hr = lpDIMouse->SetCooperativeLevel((HWND)hwnd,DISCL_EXCLUSIVE | DISCL_FOREGROUND);

    if (hr!=DI_OK) 
	{
		CloseMouseDevice();
        return FALSE;
    }

	return (TRUE);
}



//*********************************************************************************************
//
bool CInput::bOpenKeyDevice(myHWND hwnd)
{
	HRESULT	hr;

    //
    // Obtain an interface to the system keyboard device.
    //
	if (!lpDI)
	{
		Assert(0);
		// there is no DIInput Interface....
		return(FALSE);
	}

	//
	// Create the device of the specifed type
	//
    hr = lpDI->CreateDevice(GUID_SysKeyboard,&lpDIKeyboard,NULL);
    if (hr!=DI_OK) 
	{
        return FALSE;
    }

    //
    // Set the data format to "mouse format".
    //
    hr = lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    if (hr!=DI_OK) 
	{
		CloseKeyDevice(); 
        return FALSE;
    }

    //
	// Set the cooperativity level of the keyboard
	// we cannot get the keyboard in exclusive mode, this is quite good because
	// it means the message pump still works with things like F8 etc etc
    //
	hr = lpDIKeyboard->SetCooperativeLevel((HWND)hwnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    if (hr!=DI_OK) 
	{
		CloseKeyDevice();
        return FALSE;
    }

	return (TRUE);
}



//*********************************************************************************************
//
void CInput::CloseMouseDevice()
{
	if (lpDIMouse)
	{
		lpDIMouse->Unacquire();
		lpDIMouse->Release();
		lpDIMouse=NULL;
	}
}



//*********************************************************************************************
//
void CInput::CloseKeyDevice()
{
	if (lpDIKeyboard)
	{
		lpDIKeyboard->Unacquire();
		lpDIKeyboard->Release();
		lpDIKeyboard=NULL;
	}
}



//*********************************************************************************************
//
void CInput::GetMouseState()
{
	HRESULT	hr;

	Assert(lpDIMouse!=NULL);

	hr=lpDIMouse->GetDeviceState(sizeof(DIMOUSESTATE),(void*)&dim_MouseState);
	// GetDeviceState returned OK
	if (hr==DI_OK)
		return;

	// something is wrong..Have we lost the mouse device??
	if ((hr==DIERR_INPUTLOST) || (hr==DIERR_NOTACQUIRED))
	{
		// unacquire, then re-acquire to keep things simple..
		lpDIMouse->Unacquire();
		hr = lpDIMouse->Acquire();
		memset(&dim_MouseState, 0, sizeof(DIMOUSESTATE) );

		if (hr == DI_OK)
		{
			lpDIMouse->GetDeviceState(sizeof(DIMOUSESTATE),(void*)&dim_MouseState);
		}
	}

	return;
}



//*********************************************************************************************
//
void CInput::GetKeyState(uint8* pu1_table)
{
	HRESULT	hr;

	Assert(lpDIKeyboard!=NULL);

	hr=lpDIKeyboard->GetDeviceState(256,(void*)pu1_table);
	// GetDeviceState returned OK
	if (hr==DI_OK)
		return;

	// something is wrong..Have we lost the key device??
	if ((hr==DIERR_INPUTLOST) || (hr==DIERR_NOTACQUIRED))
	{
		// unacquire, then re-acquire to keep things simple..
		lpDIKeyboard->Unacquire();
		hr = lpDIKeyboard->Acquire();
		memset(pu1_table, 0, 256 );
		if (hr == DI_OK)
		{
			lpDIKeyboard->GetDeviceState(256,(void*)pu1_table);
		}
	}

	return;
}

#endif //#ifdef USE_DIRECTINPUT

//
// Global function implementation.
//

//*********************************************************************************************
bool bReadJoystickSimple(float& rf_x, float& rf_y, float& rf_z, bool& rb_trigger, bool& rb_thumb)
{
	rf_x = 0.0f;
	rf_y = 0.0f;
	rf_z = 0.0f;
	rb_trigger = false;
	rb_thumb   = false;
	CDDSize<JOYINFOEX> ji;

	ji.dwFlags=JOY_RETURNX|JOY_RETURNY|JOY_RETURNR|JOY_RETURNBUTTONS|JOY_USEDEADZONE|JOY_RETURNCENTERED;
	if (joyGetPosEx(0, &ji)!=JOYERR_NOERROR)
		return false;

	// Put the values into the correct ranges.
	rf_x = *((uint16*)&ji.dwXpos);
	rf_y = *((uint16*)&ji.dwYpos);
	rf_z = *((uint16*)&ji.dwRpos);
	rf_x /= 32768.0f;
	rf_y /= 32768.0f;
	rf_z /= 32768.0f;
	rf_x -= 1.0f;
	rf_y -= 1.0f;
	rf_z -= 1.0f;
	rb_trigger = ji.dwButtons & 1;
	rb_thumb   = ji.dwButtons & 2;

	return true;
}

