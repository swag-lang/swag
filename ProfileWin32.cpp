#include "pch.h"
#ifdef _WIN32
#ifdef SWAG_HAS_PROFILE
#include "ProfileWin32.h"
thread_local int                        g_ProfileStackIndex = 0;
Concurrency::diagnostic::marker_series* g_Profile[MAX_PROFILE_STACK];
#endif
#endif