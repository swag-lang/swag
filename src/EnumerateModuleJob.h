#pragma once
#include "Job.h"

struct EnumerateModuleJob : public Job
{
    JobResult execute() override;

    SourceFile* addFileToModule(Module* theModule, Vector<SourceFile*>& allFiles, string dirName, string fileName, uint64_t writeTime, SourceFile* prePass = nullptr, Module* imported = nullptr);
    bool        dealWithIncludes(Module* theModule);
    void        enumerateFilesInModule(const fs::path& basePath, Module* module);
    void        loadFilesInModules(const fs::path& path);
    Module*     addModule(const fs::path& path);
    void        enumerateModules(const fs::path& path);

    bool readFileMode = false;
};
