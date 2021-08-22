#include "pch.h"

#undef SWAG_ERROR
#define SWAG_ERROR(__n, __msg) __msg,
const char* g_E[] = {
#include "ErrorList.h"
};
