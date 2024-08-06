#pragma once

#undef SWAG_ERROR
#define SWAG_ERROR(__enum) __enum

enum ErrorID : uint64_t
{
#include "ErrorList.h"

    MaxErrors,
};

extern Utf8 g_E[MaxErrors];

extern void initErrors();

extern Utf8 formErr(ErrorID idx, ...);
extern Utf8 formNte(ErrorID idx, ...);
extern Utf8 toErr(ErrorID idx);
extern Utf8 toNte(ErrorID idx);
