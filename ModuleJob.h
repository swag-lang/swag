#pragma once
#include "Job.h"
#include "Module.h"

struct ModuleJob : public Job
{
    Module* module = nullptr;

    bool execute()
    {
        return module->semantic();
    }
};
