#include "pch.h"

#undef SWAG_ERROR
#define SWAG_ERROR(__n, __msg) __msg,
const char* g_E[] = {
#include "ErrorList_Err.h"
#include "ErrorList_Note.h"
#include "ErrorList_Hint.h"
#include "ErrorList_Help.h"
};
