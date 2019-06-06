#include "pch.h"
#include "Global.h"
#include "TypeManager.h"
#include "LanguageSpec.h"

Global      g_Global;

void Global::setup()
{
    g_TypeMgr.setup();
    g_LangSpec.setup();
}

Utf8 format(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    size_t len = vsnprintf(nullptr, 0, format, args);
    va_end(args);
    vector<char> vec(len + 1);
    va_start(args, format);
    vsnprintf(&vec[0], len + 1, format, args);
    va_end(args);
    return &vec[0];
}