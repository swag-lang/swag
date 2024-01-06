#pragma once

#undef SWAG_ERROR
#define SWAG_ERROR(__enum) __enum
enum ErrorID
{
#include "ErrorList.h"
    MAX_ERRORS,
};

extern Utf8 g_E[MAX_ERRORS];

extern void initErrors();
extern Utf8 Err(ErrorID idx);
extern Utf8 Nte(ErrorID idx);
extern Utf8 ErrNte(ErrorID idx, bool forNote);