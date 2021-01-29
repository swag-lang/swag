#include "pch.h"
#include "Workspace.h"
#include "ModuleBuildJob.h"
#include "CommandLineParser.h"

bool Workspace::watchCommand()
{
    setup();

    if (g_CommandLine.verbose)
        g_Log.verbose(format("=> watching workspace '%s'", workspacePath.string().c_str()));

    CommandLineParser cmdParser;
    cmdParser.setup(&g_CommandLine);
    auto cmdLine = cmdParser.buildString(false);

    OS::watch([&](const string& moduleName) {
        uint32_t errors = 0;
        OS::doProcess(nullptr, format("swag.exe test %s", cmdLine.c_str()), g_Workspace.workspacePath.string(), false, errors);
    });

    return true;
}