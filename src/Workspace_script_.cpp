#include "pch.h"
#include "Workspace.h"
#include "OS.h"
#include "Module.h"
#include "ErrorIds.h"

void Workspace::scriptCommand()
{
    if (!fs::exists(g_CommandLine.scriptName))
    {
        g_Log.error(format(Msg0166, g_CommandLine.scriptName.c_str()));
        exit(-1);
    }

    g_CommandLine.run           = true;
    g_CommandLine.scriptMode    = true;
    g_CommandLine.scriptCommand = true;
    g_Workspace.build();
}
