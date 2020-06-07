#pragma once

#include "Lib/Std/UTypes.hpp"

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

const uint32 uBITKEY_RUN = 0x01000000;
const uint32 uBITKEY_WALK = 0x02000000;
const uint32 uBITKEY_BACKUP = 0x04000000;
const uint32 uBITKEY_LEFT = 0x08000000;
const uint32 uBITKEY_RIGHT = 0x10000000;


//*********************************************************************************************
// Flags that indicate key and mouse button states.

// DO NOT USE THE TOP BYTE ON THESE CONSTANTS. THESE BITS ARE USED FOR INTERNAL KEY PRESSES
// WITHIN CINPUT.....
// Key states are logical: they don't describe the actual keys 
// but rather the action indicated by the player.
const uint uCMD_JUMP = 0x00000001;
const uint uCMD_CROUCH = 0x00000002;
const uint uCMD_PUNCH = 0x00000004;

const uint uCMD_CONTROL = 0x00000008;
const uint uCMD_SHIFT = 0x00000010;
const uint uCMD_USE = 0x00000020;

const uint uCMD_HAND = 0x00000040;
const uint uCMD_GRAB = 0x00000080;
const uint uCMD_STOW = 0x00000100;
const uint uCMD_REPLAYVO = 0x00000200;
const uint uCMD_THROW = 0x00000400;
