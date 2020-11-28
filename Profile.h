#pragma once
#ifdef SWAG_HAS_PROFILE

#define PRF_GFCT 0
#define PRF_RUN 1
#define PRF_LINK 2
#define PRF_LOAD 3
#define PRF_SAVE 4
#define PRF_OUT 5
#define PRF_OPT 5

#ifdef _WIN32
#include "ProfileWin32.h"
#endif

#else
#define SWAG_PROFILE(__categ, __name)
#endif
