#pragma once
#include "Job.h"

struct EnumerateModuleJob : public Job
{
    JobResult execute() override;

    void    enumerateFilesInModule(const fs::path& path, Module* module);
    Module* addModule(const fs::path& path);
    void    enumerateModules(const fs::path& path);
};
