#pragma once
#ifdef _WIN32
#ifdef SWAG_HAS_PROFILE
#include "Utf8.h"
#include "cvmarkersobj.h"

#define MAX_PROFILE_STACK 8
extern Concurrency::diagnostic::marker_series* g_Profile[MAX_PROFILE_STACK];
extern thread_local int                        g_ProfileStackIndex;

struct Profile
{
    Profile(int categ, const Utf8& name)
    {
        SWAG_ASSERT(g_ProfileStackIndex < MAX_PROFILE_STACK);
        CvEnterSpanExA(*(PCV_MARKERSERIES*) g_Profile[g_ProfileStackIndex++], CvImportanceNormal, categ, &_Span, name.buffer);
    }

    ~Profile()
    {
        g_ProfileStackIndex--;
        CvLeaveSpan(_Span);
    }

    PCV_SPAN _Span;
};

#define SWAG_PROFILE(__categ, __name) Profile __prf(__categ, __name)

#else
#define SWAG_PROFILE(__categ, __name)
#endif
#endif