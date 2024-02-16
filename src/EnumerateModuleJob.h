#pragma once
#include "Job.h"

struct Module;
struct Path;
struct SourceFile;

struct EnumerateModuleJob final : Job
{
	JobResult execute() override;

	static SourceFile* addFileToModule(Module* theModule, Vector<SourceFile*>& allFiles, const Path& dirName, const Utf8& fileName, uint64_t writeTime, const SourceFile* prePass, Module* imported, bool markDown);
	static bool        dealWithFileToLoads(Module* theModule);
	static void        enumerateFilesInModule(const Path& basePath, Module* theModule);
	static void        loadFilesInModules(const Path& basePath);
	static Module*     addModule(const Path& path);
	static void        enumerateModules(const Path& path);

	bool readFileMode = false;
};
