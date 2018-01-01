/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company and Microsoft
 * Corporation make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 */

// Added a pragma to disable the bool reserved warnign message in VC++ 4.1.
#pragma warning(disable: 4237)

#if _MSC_VER < 1020
#ifndef bool
typedef int bool;
#endif
#else
#include <Yvals.h>
#endif

#if _MSC_VER < 1100
#define true  1
#define false 0
#endif
