/**********************************************************************
 *<
	FILE: jagtypes.h

	DESCRIPTION:  Typedefs for general jaguar types.

	CREATED BY: Rolf Berteig

	HISTORY: created 19 November 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __JAGTYPES__
#define __JAGTYPES__

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef uchar UBYTE;
typedef unsigned short USHORT;
typedef unsigned short UWORD;
														 
struct Color24 { 
	uchar r,g,b;
	};

struct Color48 { 
	UWORD r,g,b;
	};

struct Color64 { 
	UWORD r,g,b,a;
	};

/* Time:
 */
typedef int TimeValue;

#define TIME_TICKSPERSEC	4800

#define TicksToSec( ticks ) ((float)(ticks)/(float)TIME_TICKSPERSEC)
#define SecToTicks( secs ) ((TimeValue)(secs*TIME_TICKSPERSEC))
#define TicksSecToTime( ticks, secs ) ( (TimeValue)(ticks)+SecToTicks(secs) )
#define TimeToTicksSec( time, ticks, secs ) { (ticks) = (time)%TIME_TICKSPERSEC; (secs) = (time)/TIME_TICKSPERSEC ; } 

#define TIME_PosInfinity	TimeValue(0x7fffffff)
#define TIME_NegInfinity 	TimeValue(0x80000000)


//-----------------------------------------------------
// Class_ID
//-----------------------------------------------------
class Class_ID {
	ULONG a,b;
	public:
		Class_ID() { a = b = 0xffffffff; }
		Class_ID(const Class_ID& cid) { a = cid.a; b = cid.b;	}
		Class_ID(ulong aa, ulong bb) { a = aa; b = bb; }
		ULONG PartA() { return a; }
		ULONG PartB() { return b; }
		int operator==(const Class_ID& cid) const { return (a==cid.a&&b==cid.b); }
		int operator!=(const Class_ID& cid) const { return (a!=cid.a||b!=cid.b); }
		Class_ID& operator=(const Class_ID& cid)  { a=cid.a; b = cid.b; return (*this); }
	};

// SuperClass ID
typedef ulong SClass_ID;  



#endif // __JAGTYPES__

