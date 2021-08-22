#pragma once

#undef SWAG_ERROR
#define SWAG_ERROR(__n, __msg) __n,
enum g_V
{
#include "ErrorList.h"
};

extern const char* g_E[];
