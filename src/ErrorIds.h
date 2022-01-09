#pragma once

#undef SWAG_ERROR
#define SWAG_ERROR(__n, __msg) __n,
enum g_V
{
#include "ErrorList_Err.h"
#include "ErrorList_Note.h"
#include "ErrorList_Hint.h"
};

extern const char* g_E[];
