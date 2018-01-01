//	BioFunctions.h
//	--------------

#include "BioModel.h"

//	 Called in CPhysicsSystem constructor...
void InitBiomodels( void );

//	 Clear, integrates all ACTIVE Fields...
void integrate_field( float delta_t );

//	 Makes a model...
void GenerateBioFrames( int model,
					    float center[MAX_JOINTS][3],
					    float X[MAX_JOINTS][3],
					    float Y[MAX_JOINTS][3],
					    float Z[MAX_JOINTS][3] );

//	 Generates frames from a biomodel...
void MakeBioTruss( int model, float length, float base, float taper, float joints, float offset[3] );

//	 Here we pass a control input to an individual point in the Field...
void SetBioInput( int model, float position[3], float roll, float intensity, int pelvis_number );
