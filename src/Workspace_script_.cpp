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

    // Add one module with the script file
    auto module = g_Allocator.alloc<Module>();
    auto file   = g_Allocator.alloc<SourceFile>();
    file->name  = fs::path(g_CommandLine.scriptName).filename().string();

    module->kind = ModuleKind::Script;
    module->setup(file->name, "");
    g_Workspace.modules.push_back(module);

    file->path   = g_CommandLine.scriptName;
    file->module = module;
    module->addFile(file);

    g_CommandLine.run           = true;
    g_CommandLine.scriptMode    = true;
    g_CommandLine.scriptCommand = true;
    g_Workspace.build();
}
