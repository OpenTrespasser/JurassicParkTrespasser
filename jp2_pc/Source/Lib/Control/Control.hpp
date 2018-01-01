/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	Control classes.
 *
 * Bugs:
 *
 * To do:
 * get DInput.h to include without messing up the rest of the build
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Control/Control.hpp                                              $
 * 
 * 24    98/08/28 19:49 Speter
 * Added uCMD_THROW.
 * 
 * 23    8/23/98 12:24a Shernd
 * Remove the Punch key
 * 
 * 22    8/19/98 7:11p Shernd
 * Removed the Teleport Cmd and added a Replay Last Voice over cmd
 * 
 * 21    98/08/13 14:42 Speter
 * Added stow and teleport keys (saved in replays).
 * 
 * 20    8/13/98 9:43a Shernd
 * Defined the number of control items better.
 * 
 * 19    8/10/98 2:35p Shernd
 * Brought the defintions of km_DefaultKeyMapping to the header
 * 
 * 18    98/08/05 20:41 Speter
 * Added uCMD_CONTROL. Removed uCMD_KICK.
 * 
 * 17    98/04/05 20:41 Speter
 * Removed redundant and problematic uCMD_DROP.
 * 
 * 16    98/03/31 17:05 Speter
 * Added strafe support, and recognition of key hits. Changed symbols to be more
 * coding-standard compliant.
 * 
 * 15    2/05/98 12:05p Agrant
 * Declare the global input daemon
 * 
 * 14    97/10/24 15:57 Speter
 * Replaced mouse_*button codes with logical symbols key_hand and key_grab.
 * 
 * 13    97/10/15 2:09a Pkeet
 * Added the fire key.
 * 
 * 12    8/20/97 12:49a Pkeet
 * Added the 'bReadJoystickSimple' function.
 * 
 * 11    7/31/97 3:49p Rwyatt
 * Disabled DirectInput because it causes trouble with the VC debugger under NT
 * 
 * 10    7/24/97 1:03p Rwyatt
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_CONTROL_CONTROL_HPP
#define HEADER_LIB_CONTROL_CONTROL_HPP

//#define USE_DIRECTINPUT

#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/EntityDBase/Subsystem.hpp"



//*********************************************************************************************
// Opaque types because including Windows.h gives lots of grief, these will do for the small
// amount they are used, rememeber to cast them when they are used...
//
typedef void* myHWND;
typedef void* myHANDLE;



//*********************************************************************************************
// current control types
//
enum EControlMethod
// prefix: ecm
{
	ecm_DefaultControls,
	ecm_Joystick
};



//*********************************************************************************************
struct SKeyMapping
// prefic: km
// Structure that defines a key mapping.
// The key code can be either a windows VK code or a DIK
{
	uint32	u4_KeyCode;
	uint32	u4_Action;
};

#ifdef USE_DIRECTINPUT
#define KEYMAP_COUNT       13
#else
#define KEYMAP_COUNT       15
#endif

extern SKeyMapping km_DefaultKeyMapping[];

const uint32 uBITKEY_RUN		= 0x01000000;
const uint32 uBITKEY_WALK		= 0x02000000;
const uint32 uBITKEY_BACKUP		= 0x04000000;
const uint32 uBITKEY_LEFT		= 0x08000000;
const uint32 uBITKEY_RIGHT		= 0x10000000;


//*********************************************************************************************
// Flags that indicate key and mouse button states.

// DO NOT USE THE TOP BYTE ON THESE CONSTANTS. THESE BITS ARE USED FOR INTERNAL KEY PRESSES
// WITHIN CINPUT.....
// Key states are logical: they don't describe the actual keys 
// but rather the action indicated by the player.
const uint uCMD_JUMP		= 0x00000001;
const uint uCMD_CROUCH		= 0x00000002;
const uint uCMD_PUNCH		= 0x00000004;

const uint uCMD_CONTROL		= 0x00000008;
const uint uCMD_SHIFT		= 0x00000010;
const uint uCMD_USE			= 0x00000020;

const uint uCMD_HAND		= 0x00000040;
const uint uCMD_GRAB		= 0x00000080;
const uint uCMD_STOW		= 0x00000100;
const uint uCMD_REPLAYVO	= 0x00000200;
const uint uCMD_THROW		= 0x00000400;


//*********************************************************************************************
// this is the structure that the games uses to receive its input
//
struct SInput
// Prefix: tin
{
	float		fElapsedTime;		// Time since the last input message.
	CVector2<>	v2Rotate;			// Amount to rotate in radians.  X is horizontal, Y vertical.
	CVector2<>	v2Move;				// Amount to move, in m/s.
	uint32		u4ButtonState;		// Current state of commands (on if button down).
	uint32		u4ButtonHit;		// Whether commands have been hit since last frame.

	// Utility functions.
	bool bPressed(uint u_key) const
	{
		return u4ButtonState & u_key;
	}

	bool bHit(uint u_key) const
	{
		return u4ButtonHit & u_key;
	}
};



//*********************************************************************************************
//
class CInput
// Prefix: inp
//**************************************
{
public:
	CInput(myHWND hwnd,myHANDLE hinst);
	~CInput();

	void Capture(bool state);	// Capture input - on/off switch.

	// functions to return the controls in the standard form of a Tinput struct
	// these functions may use win32 or DirectInput to do their work.
	SInput& tinReadDefaultControls();
	SInput& tinReadStandardJoystickControls();

private:

#ifdef USE_DIRECTINPUT
	//
	// functions to open and read devices with DirectInput 3.0
	//
	bool bOpenMouseDevice(myHWND hwnd);
	void CloseMouseDevice();
	bool bOpenKeyDevice(myHWND hwnd);
	void CloseKeyDevice();
	void GetMouseState();
	void GetKeyState(uint8* pu1_key_table);
#endif

	myHWND		hwnd;				// HWND for mouse and window actions in non DirectInput
	myHANDLE	hinst;				// instance handle for DirectInput

	SInput		tin_Input;			// Describes current input.
	uint32		u4LastButtons;		// Remembers last button state, as Windows won't do it correctly.
	
#ifdef USE_DIRECTINPUT
	uint32		au4_mousemap[4];	// used to map the mouse buttons in DirectInput
#else
	// if we are not using directinput then we need to center the mouse..
	int			i_Xcenter;
	int			i_Ycenter;			// Center of window in screen coords
#endif //#ifdef USE_DIRECTINPUT

};





//*********************************************************************************************
// This class knows nothing about control methods, this class will just call the correct method
// of the above CInput class.
//
class CInputDeemone : public CSubsystem
// Prefix: co
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CInputDeemone(
				myHWND hwnd,				// window handle to link the controls to
				myHANDLE hinst,				// handle of the application
				EControlMethod ecm_controls	// initial control method
				);
	~CInputDeemone();

	//
	//	Message that we need to process
	//
	void Process(const CMessageStep& msg_step);
	void Process(const CMessageSystem& msg_system);

	// start and stop input capture, initial state is not captured
	void Capture(bool b_state = true);

	// get the current control method
	EControlMethod ecmGetControlMethod()
	{
		return ecm_Control;
	}

	// set the current control method
	void SetControlMethod(EControlMethod ecm_control_method)
	{
		ecm_Control=ecm_control_method;
	}

private:
	CInput			inp_Input;		// Class to read the various different controllers
	bool			b_Capture;		// Are we capturing input?
	EControlMethod	ecm_Control;	// the current control method
};


//
// Global functions.
//

//*********************************************************************************************
//
bool bReadJoystickSimple
(
	float& rf_x,		// X input value.
	float& rf_y,		// Y input value.
	float& rf_z,		// Z input value.
	bool&  rb_trigger,	// Trigger joystick button.
	bool&  rb_thumb		// Thumb joystick button.
);
//
// Reads values from a joystick and returns them as values in the range -1.0 to 1.0.
//
// Notes:
//		This function is intended for demo/debug purposes only as it functions without the
//		problem of supporting direct input.
//
//**************************************

extern CInputDeemone* gpInputDeemone;		// Control input.

#endif
