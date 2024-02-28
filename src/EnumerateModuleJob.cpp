#include "pch.h"
#include "EnumerateModuleJob.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LoadSourceFileJob.h"
#include "Module.h"
#include "ModuleDepManager.h"
#include "Os.h"
#include "Report.h"
#include "SyntaxJob.h"
#include "ThreadManager.h"
#include "Workspace.h"

SourceFile* EnumerateModuleJob::addFileToModule(Module*              theModule,
                                                Vector<SourceFile*>& allFiles,
                                                const Path&          dirName,
                                                const Utf8&          fileName,
                                                uint64_t             writeTime,
                                                const SourceFile*    prePass,
                                                Module*              imported,
                                                bool                 markDown)
{
    const auto file = Allocator::alloc<SourceFile>();
    file->addFlag(theModule->is(ModuleKind::Test) ? FILE_FROM_TESTS : 0);
    file->name     = fileName;
    file->imported = imported;

    file->addFlag(markDown ? FILE_MARK_DOWN : 0);
    if (markDown)
        file->buildPass = BuildPass::Syntax;

    if (prePass)
    {
        file->path              = dirName;
        file->buffer            = prePass->buffer;
        file->bufferSize        = prePass->bufferSize;
        file->offsetStartBuffer = prePass->offsetStartBuffer;
        file->allocBufferSize   = prePass->allocBufferSize;
    }
    else
    {
        Path pathFile = dirName;
        pathFile.append(fileName);
        file->path = pathFile;
    }

    file->writeTime = writeTime;

    // If we have only one core, then we will sort files in alphabetical order to always
    // treat them in a reliable order. That way, --randomize and --seed can work.
    if (g_ThreadMgr.numWorkers == 1 || g_CommandLine.scriptCommand)
        allFiles.push_back(file);
    else
    {
        // In async mode, syntax jobs are treated in a special way, as in fact they could be run at any time
        // before the module job reaches the ModuleBuildPass::Syntax
        // So each time a thread does not have a job to run, it will try a syntax job instead of doing nothing.
        // When a module reaches the ModuleBuildPass::Syntax, it will cancel all pending special syntax jobs,
        // and change them to "normal" ones.
        // If we do not have to rebuild a module, we will do some syntax jobs anyway, but this is fine as
        // they are done during "idle" time.
        theModule->addFile(file);

        if (!markDown)
        {
            const auto syntaxJob  = Allocator::alloc<SyntaxJob>();
            syntaxJob->sourceFile = file;
            syntaxJob->module     = theModule;
            syntaxJob->addFlag(JOB_IS_OPT);
            syntaxJob->jobGroup = &theModule->syntaxJobGroup;
            theModule->syntaxJobGroup.addJob(syntaxJob);
        }
    }

    return file;
}

bool EnumerateModuleJob::dealWithFileToLoads(Module* theModule)
{
    Vector<SourceFile*> allFiles;

    // Treat #load
    for (const auto n : theModule->compilerLoads)
    {
        Path orgFilePath = n->token.text;

        // Is this a simple file ?
        auto            filePath = orgFilePath;
        std::error_code err;
        if (!std::filesystem::exists(filePath, err))
        {
            filePath = theModule->path;
            filePath.append(orgFilePath);
            filePath             = std::filesystem::absolute(filePath);
            const auto filePath1 = std::filesystem::canonical(filePath, err);
            if (!err)
                filePath = filePath1;
            if (!std::filesystem::exists(filePath, err))
            {
                Report::report({n->token.sourceFile, n->token, formErr(Err0709, n->token.c_str())});
                return false;
            }
        }

        auto       fileName  = filePath.filename();
        const auto writeTime = OS::getFileWriteTime(filePath);
        addFileToModule(theModule, allFiles, filePath.parent_path(), fileName, writeTime, nullptr, nullptr, true);
    }

    // Sort files, and register them in a constant order
    if (!allFiles.empty())
    {
        std::ranges::sort(allFiles, [](const SourceFile* a, const SourceFile* b) { return strcmp(a->name, b->name) < 0; });
        for (const auto file : allFiles)
            theModule->addFile(file);
    }

    return true;
}

void EnumerateModuleJob::enumerateFilesInModule(const Path& basePath, Module* theModule)
{
    Vector<SourceFile*> allFiles;

    auto path = basePath;
    path      = ModuleDepManager::getAliasPath(path);
    path.append(SWAG_SRC_FOLDER);

    if (!dealWithFileToLoads(theModule))
        return;

    // Is the list of files already computed ?
    const auto it = g_Workspace->mapFirstPassModulesNames.find(path);
    if (it != g_Workspace->mapFirstPassModulesNames.end())
    {
        for (const auto f : it->second->files)
        {
            if (theModule->isNot(ModuleKind::Test) || g_CommandLine.testFilter.empty() || f->name.containsNoCase(g_CommandLine.testFilter))
            {
                const auto pz = strrchr(f->name, '.');
                if (pz && !_strcmpi(pz, ".swg"))
                {
                    addFileToModule(theModule, allFiles, f->path, f->name, f->writeTime, f, nullptr, false);
                }
                else if (pz && !_strcmpi(pz, ".md"))
                {
                    addFileToModule(theModule, allFiles, f->path, f->name, f->writeTime, f, nullptr, true);
                }
                else
                {
                    theModule->moreRecentSourceFile = max(theModule->moreRecentSourceFile, f->writeTime);
                }
            }
        }

        return;
    }

    // Scan source folder
    Vector<Path> directories;
    directories.push_back(path);

    Path tmp1;
    Path modulePath;
    while (!directories.empty())
    {
        Path tmp = std::move(directories.back());
        directories.pop_back();

        OS::visitFilesFolders(tmp, [&](uint64_t writeTime, const char* cFileName, bool isFolder) {
            if (isFolder)
            {
                tmp1 = tmp;
                tmp1.append(cFileName);
                directories.emplace_back(std::move(tmp1));
            }
            else
            {
                const Utf8 fileN = cFileName;
                if (theModule->isNot(ModuleKind::Test) || g_CommandLine.testFilter.empty() || fileN.containsNoCase(g_CommandLine.testFilter))
                {
                    const auto pz = strrchr(cFileName, '.');
                    if (pz && !_strcmpi(pz, ".swg"))
                    {
                        addFileToModule(theModule, allFiles, tmp, cFileName, writeTime, nullptr, nullptr, false);
                    }
                    else if (g_CommandLine.genDoc && pz && !_strcmpi(pz, ".md"))
                    {
                        addFileToModule(theModule, allFiles, tmp, cFileName, writeTime, nullptr, nullptr, true);
                    }

                    // Even if this is not a .swg file, as this is in the src directory, the file time contribute
                    // to the rebuild detection (in case file is #load by another for example)
                    else
                    {
                        theModule->moreRecentSourceFile = max(theModule->moreRecentSourceFile, writeTime);
                    }
                }
            }
        });
    }

    // Add the config file, second pass
    if (g_ModuleCfgMgr->getCfgModule(theModule->name))
    {
        auto cfgFile = theModule->path;
        cfgFile      = ModuleDepManager::getAliasPath(cfgFile);
        cfgFile.append(SWAG_CFG_FILE);
        const auto writeTime = OS::getFileWriteTime(cfgFile);
        const auto file      = addFileToModule(theModule, allFiles, cfgFile.parent_path(), SWAG_CFG_FILE, writeTime, nullptr, nullptr, false);
        file->addFlag(FILE_IS_CFG_FILE);
    }

    // Sort files, and register them in a constant order
    if (!allFiles.empty())
    {
        std::ranges::sort(allFiles, [](const SourceFile* a, const SourceFile* b) { return strcmp(a->name, b->name) < 0; });
        for (const auto file : allFiles)
            theModule->addFile(file);
    }
}

void EnumerateModuleJob::loadFilesInModules(const Path& basePath)
{
    // Scan source folder
    OS::visitFolders(basePath, [&](const char* fileName) {
        auto path = basePath;
        path.append(fileName);
        path = ModuleDepManager::getAliasPath(path);
        path.append(SWAG_SRC_FOLDER);

        const auto module                           = Allocator::alloc<Module>();
        g_Workspace->mapFirstPassModulesNames[path] = module;

        // Scan source folder
        Vector<Path> directories;
        directories.push_back(path);

        Path tmp1;
        while (!directories.empty())
        {
            Path tmp = std::move(directories.back());
            directories.pop_back();

            OS::visitFilesFolders(tmp, [&](uint64_t writeTime, const char* subFileName, bool isFolder) {
                if (isFolder)
                {
                    tmp1 = tmp;
                    tmp1.append(subFileName);
                    directories.emplace_back(std::move(tmp1));
                }
                else
                {
                    const auto file = Allocator::alloc<SourceFile>();
                    file->module    = module;
                    file->name      = subFileName;
                    Path pathFile   = tmp;
                    pathFile.append(subFileName);
                    file->path      = pathFile;
                    file->writeTime = writeTime;
                    module->files.push_back(file);

                    const auto pz = strrchr(subFileName, '.');
                    if (pz && !_strcmpi(pz, ".swg"))
                    {
                        const auto readFileJob  = Allocator::alloc<LoadSourceFileJob>();
                        readFileJob->flags      = JOB_IS_OPT;
                        readFileJob->sourceFile = file;
                        g_ThreadMgr.addJob(readFileJob);
                    }
                }
            });
        }
    });
}

Module* EnumerateModuleJob::addModule(const Path& path)
{
    Utf8       moduleName;
    Path       moduleFolder;
    ModuleKind kind;

    // Get infos about module, depending on where it is located
    Workspace::computeModuleName(path, moduleName, moduleFolder, kind);

    // Create theModule
    const auto theModule = g_Workspace->createOrUseModule(moduleName, moduleFolder, kind);

    // Parse all files in the source tree
    enumerateFilesInModule(path, theModule);
    return theModule;
}

void EnumerateModuleJob::enumerateModules(const Path& path)
{
    Vector<std::string> allModules;

    // Scan source folder
    OS::visitFolders(path, [&](const char* cFolderName) {
        // If we have only one core, then we will sort modules in alphabetical order to always
        // treat them in a reliable order. That way, --randomize and --seed can work.
        if (g_ThreadMgr.numWorkers == 1)
            allModules.push_back(cFolderName);
        else
        {
            auto toAdd = path;
            toAdd.append(cFolderName);
            addModule(toAdd);
        }
    });

    // Sort modules, and register them in a constant order
    if (!allModules.empty())
    {
        std::ranges::sort(allModules);
        for (const auto& m : allModules)
        {
            auto toAdd = path;
            toAdd.append(m);
            addModule(toAdd);
        }
    }
}

JobResult EnumerateModuleJob::execute()
{
    // Just scan the files, and load them, as optional jobs, during the build setup stage
    if (readFileMode)
    {
        SWAG_ASSERT(!g_CommandLine.scriptCommand);
        loadFilesInModules(g_Workspace->dependenciesPath);
        loadFilesInModules(g_Workspace->modulesPath);
        if (g_CommandLine.test || g_CommandLine.genDoc)
            loadFilesInModules(g_Workspace->testsPath);
        return JobResult::ReleaseJob;
    }

    // Modules in the workspace
    if (!g_CommandLine.scriptCommand)
    {
        enumerateModules(g_Workspace->dependenciesPath);
        enumerateModules(g_Workspace->modulesPath);
        if (g_CommandLine.test || g_CommandLine.genDoc)
            enumerateModules(g_Workspace->testsPath);
    }
    else
    {
        enumerateModules(g_Workspace->dependenciesPath);

        // If we are in script mode, then we add one single module with the script file
        const auto parentFolder = Path(g_CommandLine.scriptName.c_str()).parent_path();
        const auto file         = Allocator::alloc<SourceFile>();
        file->name              = Path(g_CommandLine.scriptName).filename().replace_extension();
        const auto scriptModule = g_Workspace->createOrUseModule(file->name, parentFolder, ModuleKind::Script);
        file->path              = g_CommandLine.scriptName;
        file->module            = scriptModule;
        file->addFlag(FILE_IS_SCRIPT_FILE);
        scriptModule->isScriptFile = true;
        scriptModule->addFile(file);
        g_Workspace->runModule = scriptModule;

        if (!dealWithFileToLoads(scriptModule))
            return JobResult::ReleaseJob;
    }

    // Deal with embedded modules
    for (const auto m : g_Workspace->modules)
    {
        if (!m->buildCfg.embeddedImports)
            continue;

        Vector<SourceFile*>             allFiles;
        VectorNative<ModuleDependency*> toKeep;
        for (uint32_t i = m->moduleDependencies.size() - 1; i != UINT32_MAX; i--)
        {
            auto dep = m->moduleDependencies[i];
            auto mod = g_Workspace->getModuleByName(dep->name);
            if (!mod)
            {
                toKeep.push_back(dep);
                continue;
            }

            m->moduleEmbedded.push_back(mod);

            for (const auto f : mod->files)
            {
                if (f->hasFlag(FILE_IS_CFG_FILE))
                    continue;
                const auto newFile = addFileToModule(m, allFiles, f->path.parent_path(), f->name, f->writeTime, nullptr, mod, false);
                newFile->addFlag(FILE_IS_EMBEDDED);
                newFile->globalUsingEmbedded = mod->buildParameters.globalUsing;
            }

            // Add the dependencies of the embedded module too
            for (const auto otherDep : mod->moduleDependencies)
            {
                const auto cpt = m->moduleDependencies.size();

                if (!m->addDependency(otherDep->node, otherDep->tokenLocation, otherDep->tokenVersion))
                    return JobResult::ReleaseJob;

                if (m->moduleDependencies.size() > cpt)
                {
                    const auto cfgModule = g_ModuleCfgMgr->getCfgModule(mod->name);

                    // :GetCfgFileParams
                    for (const auto& fl : cfgModule->buildParameters.foreignLibs)
                        m->buildParameters.foreignLibs.insert(fl);

                    i++;
                }
            }
        }

        // Sort files, and register them in a constant order
        if (!allFiles.empty())
        {
            std::ranges::sort(allFiles, [](const SourceFile* a, const SourceFile* b) { return strcmp(a->name, b->name) < 0; });
            for (const auto file : allFiles)
                m->addFile(file);
        }

        m->moduleDependencies.clear();
        m->moduleDependencies.append(toKeep);
    }

    return JobResult::ReleaseJob;
}
