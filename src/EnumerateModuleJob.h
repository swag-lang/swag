#pragma once
#include "Job.h"

struct EnumerateModuleJob : public Job
{
    JobResult execute() override;

    void    addFileToModule(Module* theModule, vector<SourceFile*>& allFiles, string dirName, string fileName, uint64_t writeTime);
    void    enumerateFilesInModule(const fs::path& basePath, Module* module);
    Module* addModule(const fs::path& path);
    void    enumerateModules(const fs::path& path);
};
