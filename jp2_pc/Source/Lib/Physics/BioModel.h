//	BioModel.h, a list of interesting stuff...
//	==========================================

#define MAX_BIOMODELS	16		//2*NUM_PELVISES
#define FIELD_DIMENSION	30
#define MAX_JOINTS		10

#ifdef __MWERKS__
// The MW build uses templated stream classes
 namespace std {
  template<class charT, class traits> class basic_ostream;
  template<class charT> struct char_traits;
  typedef basic_ostream<char,char_traits<char> > ostream; 
 };
#else
 class ostream;
#endif

#pragma warning( disable : 4244 )
#pragma warning( disable : 4101 )

//**********************************************************************************************
class CBioModel
{
public:
	float	Field[FIELD_DIMENSION][3][3];
	float	FieldInputs[FIELD_DIMENSION][3];
	int8	Tensor[FIELD_DIMENSION][FIELD_DIMENSION];
	float	Rho[FIELD_DIMENSION][FIELD_DIMENSION];
	float	Offset[3];
	float	Psi[FIELD_DIMENSION][5];
	float	Control[6];
	int		Dof;
	int		Pelvis;

	// Standard constructor.
	CBioModel( int pelvis, float length, float base, float taper, float joints, float offset[3] );

	void Integrate(float delta_t);

	void SetInput( float position[3], float roll, float intensity, int pelvis_number );

	void GenerateFrames(
					    float center[MAX_JOINTS][3],
					    float X[MAX_JOINTS][3],
					    float Y[MAX_JOINTS][3],
					    float Z[MAX_JOINTS][3] );

	void DumpState(ostream& os);

private:
	void UpdateControl();

	void apply_bc_and_locate_field_tracers( int coord );

};

// Array of biomodel pointers; dynamically allocated.
extern aptr<CBioModel>	BioModels[MAX_BIOMODELS];

void DumpBioState(ostream& os);

//	 Clear, integrates all ACTIVE Fields...
void integrate_field( float delta_t );


