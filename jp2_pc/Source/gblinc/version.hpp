#pragma once

/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		Version string.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * 1     98.03.25 6:48p Mmouni
 * Created.
 * 
 **********************************************************************************************/

#include "versionGit.hpp"

#define prep_iconv_str(s)  # s
#define prep_conv_str(s)   prep_iconv_str(s)

#define BUILD_MAJOR     1
#define BUILD_MINOR     0
#define BUILD_NUM       116

#define STR_BUILD_MAJOR prep_conv_str(BUILD_MAJOR)
#define STR_BUILD_MINOR prep_conv_str(BUILD_MINOR)
#define STR_BUILD_NUM   prep_conv_str(BUILD_NUM)

#define BUILD_VERSION       STR_BUILD_MAJOR "." STR_BUILD_MINOR "." STR_BUILD_NUM
#define BUILD_VERSION_HASH  BUILD_VERSION "." GIT_HASH_SHORT
