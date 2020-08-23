#pragma once
#ifdef _WIN32
#ifdef SWAG_HAS_PROFILE
#include "cvmarkersobj.h"

#define SWAG_PROFILE(___name)               \
    Utf8                   __name(___name); \
    VectorNative<char16_t> __uni16;         \
    __name.toUni16(__uni16);                \
    __uni16.push_back(0);                   \
    Concurrency::diagnostic::span s(g_Profiler, 2, (LPCTSTR) __uni16.buffer);

extern Concurrency::diagnostic::marker_series g_Profiler;
#else
#define SWAG_PROFILE(__name)
#endif
#endif