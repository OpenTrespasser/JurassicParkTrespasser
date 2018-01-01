/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Physics test code.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Physics/PhysicsTest.hpp                                          $
 * 
 * 1     8/14/96 3:11p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_PHYSICS_PHYSICSTEST_HPP
#define HEADER_TEST_PHYSICS_PHYSICSTEST_HPP


//*********************************************************************************************
//
void Create
(
);
//
// Call once after application initialization.
//
//**************************************

//*********************************************************************************************
//
void Step
(
);
//
// Called when nothing else is going on. Simulation code should be placed here.
//
//**************************************

//*********************************************************************************************
//
void Paint
(
);
//
// Called when the application draws to screen. Line drawing code should be place here.
//
//**************************************

//*********************************************************************************************
//
void Destroy
(
);
//
// Call once before the application is destroyed.
//
//**************************************


#endif