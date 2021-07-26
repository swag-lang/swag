#include "pch.h"
#include "Global.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "Backend.h"

Global g_Global;

void Global::setup()
{
    g_TypeMgr.setup();
    g_LangSpec.setup();
    Backend::setup();
}

