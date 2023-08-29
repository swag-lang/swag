#pragma once
#include "Job.h"
#include "Vector.h"
struct Path;
struct Module;
struct SourceFile;

struct EnumerateModuleJob : public Job
{
    JobResult execute() override;

    SourceFile* addFileToModule(Module* theModule, Vector<SourceFile*>& allFiles, const Path& dirName, const Utf8& fileName, uint64_t writeTime, SourceFile* prePass, Module* imported, bool markDown);
    bool        dealWithIncludes(Module* theModule);
    void        enumerateFilesInModule(const Path& basePath, Module* module);
    void        loadFilesInModules(const Path& path);
    Module*     addModule(const Path& path);
    void        enumerateModules(const Path& path);

    bool readFileMode = false;
};
