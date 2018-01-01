#include "ddraw.h"

//#include "#AIGraph.cpp"

/*class Foo
{
	Foo(int i);
	int mi;
	Foo FAbs() const;
	bool TestAbs()
	  { return (FAbs() <= Foo(3)); };
//	bool Test2() {
//	  Foo ack(3);
//	  Foo fabs = FAbs();
//	  return (fabs <= ack);
//	}
	bool operator ==(Foo foo) const;
	bool operator <(Foo foo) const;
};
*/
/*
struct MyFile;
namespace std {
 struct MyFile {
   long parID;
   long dirID;
   char fname[12];
 };
}
using namespace std;

MyFile* gFilePtr;
*/


typedef long int32;
typedef unsigned long uint32;
typedef short int16;
typedef unsigned short uint16;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;

#define TypeMin(T)  T( T(1) << (sizeof(T) * 8 - 1) )
#define TypeMax(T)  (~T(0) ^ TypeMin(T))
#define Assert(x)

#define BitMask(i) ((long)(0x01 << i))
#define BitMaskRange(i,j) ((long)((0xffffffff >> (32-i+j)) << j))

uint32 u4FromFloat( float f );

#include "floatdef.hpp"


/*typedef short uint16;

void EnableExceptions( uint16 u2_flags );
inline void EnableExceptions( uint16 u2_flags )
{
	uint16 u2_old_state;
	uint16 u2_new_state;

	__asm 
	{
		// Clear exceptions.
		fnclex

		// Enable exceptions.
		fstcw	[u2_old_state]
		mov		ax,[u2_old_state]
		mov		bx,[u2_flags]
		not		bx
		and		ax,bx
		mov		[u2_new_state],ax
		fldcw	[u2_new_state]
	}
}
*/