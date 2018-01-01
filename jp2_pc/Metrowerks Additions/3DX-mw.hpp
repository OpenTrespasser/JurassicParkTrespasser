/***************************************************************************
 * CONFIDENTIAL 
 * Copyright (c) 1997 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *      
 * This software is unpublished and contains the trade secrets and 
 * confidential proprietary information of AMD. Unless otherwise
 * provided in the software Agreement associated herewith, it is 
 * licensed in confidence "AS IS" and is not to be reproduced in
 * whole or part by any means except for backup. Use, duplication,
 * or disclosure by the Government is subject to the restrictions
 * in paragraph(b)(3)(B) of the Rights in Technical Data and 
 * Computer Software clause in DFAR 52.227-7013(a)(Oct. 1988).
 * Software owned by Advanced Micro Devices Inc., One AMD Place
 * P.O. Box 3453, Sunnyvale, CA 94088-3453
 ***************************************************************************
 *      
 * 3dx.h
 *      
 * Written by: Duke Cho
 * Date:       July 08, 1997
 * Version:    1.02.000
 *****************************************************************************
 * Modification History
 *
 * Mod No  Date		Who      Description 
 * 00		7/14/97  DC       Fixed the spelling error in one of instruction macro
 * 01		7/21/97  DC       Added femms macro
 * 02		7/25/97  MJ       Removed the space in macro name PF2ID
 * 03		6/27/97  HL       Add global variables for x87 translator
 * 04		8/19/97  DC       Removed the function prototype           
 * 05		8/20/97  DC       Fixed the VC++4.2 emit bug
 * 06		8/29/97  DC       Fixed the VC++4.2 and VC++5.X emit bug 
 * 07		9/08/97  DC       Added four more macros
 *
 * MACRO FORMAT
 * ============
 * This file contains inline assembly macros that
 * generate Cedar instructions in binary format.
 * Therefore, C or C++ programmer can use Cedar instructions
 * without any penalty in their C or C++ source code.
 * 
 * The macro's name and format conventions are as follow:
 *
 *
 * 	1. First argument of macro is a destination and
 * 	   second argument is a source operand.
 * 		ex) _asm PFCMPEQ (m3, m4)
 *      				  |    |
 *   	 		         dst  src
 *
 *	2. The destination operand can be m0 to m7 only. 
 *         The source operand can be any one of the register
 *         m0 to m7 or _eax, _ecx, _edx, _ebx, _esi, or _edi 
 *         that contains effective address.
 *    		ex) _asm PFRCP    (M7, M6)
 *		ex) _asm PFRCPIT2 (m0, m4)
 *		ex) _asm PFMUL    (m3, _edi)
 *	
 *  3. The prefetch(w) takes one src operand _eax, _ecx, _edx,
 *     _ebx, _esi, or _edi that contains effective address.
 *      	ex) _asm PREFETCH (_edi)
 *  			    
 * EXAMPLE
 * =======
 * Following program doesn't do anything but it shows you
 * how to use inline assembly Cedar instructions in C.
 * Note that this will only work in flat memory model which
 * segment registers cs, ds, ss and es point to the same 
 * linear address space total less than 4GB.
 * 
 * Used Microsoft VC++ 5.0
 *
 * #include <stdio.h>                    
 * #include "3dx.h"                   
 *                                       
 * void main ()                          
 * {                                     
 *	float x = (float)1.25;            
 * 	float y = (float)1.25;            
 * 	float z, zz;                      
 *
 *	_asm {                            
 * 		movd mm1, x
 * 		movd mm2, y                  
 * 		pfmul (m1, m2)               
 * 		movd z, mm1
 *		emms
 * 	}                                 
 *                                       
 * 	printf ("value of z = %f\n", z);  
 *        
 * 	// 
 *	// Demonstration of using the memory instead of
 *	// multimedia register
 *	//
 * 	_asm {        
 * 		movd mm3, x
 *	   	lea esi, y   // load effective address of y
 * 		pfmul (m3, _esi)
 * 		movd zz, mm3                  
 * 		emms
 * 	}
 *
 *  	printf ("value of zz = %f\n", zz);
 *  }                                     
 ***************************************************************************/

#define	M0   mm0
#define	M1   mm1
#define	M2   mm2
#define	M3   mm3
#define	M4   mm4
#define	M5   mm5
#define	M6   mm6
#define	M7   mm7

#define	m0   mm0
#define	m1   mm1
#define	m2   mm2
#define	m3   mm3
#define	m4   mm4
#define	m5   mm5
#define	m6   mm6
#define	m7   mm7

#define _EAX eax
#define _ECX ecx
#define _EDX edx
#define _EBX ebx
#define _ESI esi
#define _EDI edi

#define _eax eax
#define _ecx ecx
#define _edx edx
#define _ebx ebx
#define _esi esi
#define _edi edi

#define PF2ID(dst, src)			pf2id dst, src
#define PF2IW(dst, src)			pf2iw dst, src
#define PFACC(dst, src)			pfacc dst, src
#define PFADD(dst, src)			pfadd dst, src
#define PFCMPEQ(dst, src)		pfcmpeq	dst, src
#define PFCMPGE(dst, src)		pfcmpge dst, src
#define PFCMPGT(dst, src)		pfcmpgt dst, src
#define PFMAX(dst, src)			pfmax	dst, src
#define PFMIN(dst, src)			pfmin	dst, src
#define PFMUL(dst, src)			pfmul	dst, src
#define PFRCP(dst, src)			pfrcp	dst, src
#define PFRCPIT1(dst, src)		pfrcpit1 dst, src
#define PFRCPIT2(dst, src)		pfrcpit2 dst, src
#define PFRSQRT(dst, src)		pfrsqrt dst, src
#define PFRSQIT1(dst, src)		pfrsqit1 dst, src
#define PFSUB(dst, src)			pfsub dst, src
#define PFSUBR(dst, src)		pfsubr dst, src
#define PI2FD(dst, src)			pi2fd	dst, src
#define PI2FW(dst, src)			pi2fw	dst, src
#define FEMMS 					femms
#define PAVGUSB(dst, src)		pavgusb dst, src
#define PMULHRW(dst, src)		pmulhrw dst, src
#define PREFETCH(src)			prefetch src
#define PREFETCHW(src)			prefetchw src
#define PSWAPW(dst, src)		pswapw dst, src

//
// Exactly same as above except macro names are all 
// lower case latter.
//
#define pf2id(dst, src)			pf2id dst, src
#define pf2iw(dst, src)			pf2iw dst, src
#define pfacc(dst, src)			pfacc dst, src
#define pfadd(dst, src)			pfadd dst, src
#define pfcmpeq(dst, src)		pfcmpeq dst, src
#define pfcmpge(dst, src)		pfcmpge dst, src
#define pfcmpgt(dst, src)		pfcmpgt dst, src
#define pfmax(dst, src)			pfmax dst, src
#define pfmin(dst, src)			pfmin dst, src
#define pfmul(dst, src)			pfmul dst, src
#define pfrcp(dst, src)			pfrcp dst, src
#define pfrcpit1(dst, src)		pfrcpit1 dst, src
#define pfrcpit2(dst, src)		pfrcpit2 dst, src
#define pfrsqrt(dst, src)		pfrsqrt dst, src
#define pfrsqit1(dst, src)		pfrsqit1 dst, src
#define pfsub(dst, src)			pfsub dst, src
#define pfsubr(dst, src)		pfsubr dst, src
#define pi2fd(dst, src)			pi2fd dst, src
#define pi2fw(dst, src)			pi2fw dst, src
//#define femms 					
#define pavgusb(dst, src)		pavgusb dst, src
#define pmulhrw(dst, src)		pmulhrw dst, src
#define prefetch(src)			prefetch src
#define prefetchw(src)			prefetchw src
#define pswapw(dst, src)		pswapw dst, src

