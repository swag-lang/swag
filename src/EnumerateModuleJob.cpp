#include "pch.h"
#include "EnumerateModuleJob.h"
#include "Workspace.h"
#include "Os.h"
#include "Module.h"
#include "ModuleDepManager.h"
#include "LoadSourceFileJob.h"
#include "ThreadManager.h"
#include "SyntaxJob.h"
#include "Diagnostic.h"
#include "Report.h"

SourceFile* EnumerateModuleJob::addFileToModule(Module* theModule, Vector<SourceFile*>& allFiles, const Path& dirName, const Utf8& fileName, uint64_t writeTime, SourceFile* prePass, Module* imported)
{
    auto file       = Allocator::alloc<SourceFile>();
    file->fromTests = theModule->kind == ModuleKind::Test;
    file->name      = fileName;
    file->imported  = imported;

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
        pathFile.append(fileName.c_str());
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
        auto syntaxJob        = Allocator::alloc<SyntaxJob>();
        syntaxJob->sourceFile = file;
        syntaxJob->module     = theModule;
        syntaxJob->flags |= JOB_IS_OPT;
        syntaxJob->jobGroup = &theModule->syntaxGroup;
        theModule->syntaxGroup.addJob(syntaxJob);
    }

    return file;
}

bool EnumerateModuleJob::dealWithIncludes(Module* theModule)
{
    Vector<SourceFile*> allFiles;

    // Treat includes
    for (auto n : theModule->includes)
    {
        Path orgFilePath = n->token.ctext();

        // Is this a simple file ?
        auto       filePath = orgFilePath;
        error_code err;
        if (!filesystem::exists(filePath, err))
        {
            filePath = theModule->path;
            filePath.append(orgFilePath.string());
            filePath       = filesystem::absolute(filePath);
            auto filePath1 = filesystem::canonical(filePath, err);
            if (!err)
                filePath = filePath1;
            if (!filesystem::exists(filePath, err))
            {
                Diagnostic diag{n->sourceFile, n->token, Fmt(Err(Err0304), n->token.ctext())};
                Report::report(diag);
                return false;
            }
        }

        auto fileName  = filePath.filename().string();
        auto writeTime = OS::getFileWriteTime(filePath.string().c_str());
        addFileToModule(theModule, allFiles, filePath.parent_path(), fileName, writeTime);
    }

    // Sort files, and register them in a constant order
    if (!allFiles.empty())
    {
        sort(allFiles.begin(), allFiles.end(), [](SourceFile* a, SourceFile* b)
             { return strcmp(a->name.c_str(), b->name.c_str()) == -1; });
        for (auto file : allFiles)
            theModule->addFile(file);
    }

    return true;
}

void EnumerateModuleJob::enumerateFilesInModule(const Path& basePath, Module* theModule)
{
    Vector<SourceFile*> allFiles;

    auto path = basePath;
    path      = g_ModuleCfgMgr->getAliasPath(path);
    path.append(SWAG_SRC_FOLDER);

    if (!dealWithIncludes(theModule))
        return;

    // Is the list of files already computed ?
    auto it = g_Workspace->mapFirstPassModulesNames.find(path);
    if (it != g_Workspace->mapFirstPassModulesNames.end())
    {
        for (auto f : it->second->files)
        {
            if (theModule->kind != ModuleKind::Test || g_CommandLine.testFilter.empty() || strstr(f->name, g_CommandLine.testFilter.c_str()))
            {
                auto pz = strrchr(f->name, '.');
                if (pz && !_strcmpi(pz, ".swg"))
                {
                    addFileToModule(theModule, allFiles, f->path, f->name.c_str(), f->writeTime, f);
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

    Path tmp, tmp1;
    Path modulePath;
    while (directories.size())
    {
        tmp = std::move(directories.back());
        directories.pop_back();

        OS::visitFilesFolders(tmp.string().c_str(),
                              [&](uint64_t writeTime, const char* cFileName, bool isFolder)
                              {
                                  if (isFolder)
                                  {
                                      tmp1 = tmp;
                                      tmp1.append(cFileName);
                                      directories.emplace_back(std::move(tmp1));
                                  }
                                  else
                                  {
                                      if (theModule->kind != ModuleKind::Test || g_CommandLine.testFilter.empty() || strstr(cFileName, g_CommandLine.testFilter.c_str()))
                                      {
                                          auto pz = strrchr(cFileName, '.');
                                          if (pz && !_strcmpi(pz, ".swg"))
                                          {
                                              addFileToModule(theModule, allFiles, tmp, cFileName, writeTime);
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
    auto cfgModule = g_ModuleCfgMgr->getCfgModule(theModule->name);
    if (cfgModule)
    {
        auto cfgFile = theModule->path;
        cfgFile      = g_ModuleCfgMgr->getAliasPath(cfgFile);
        cfgFile.append(SWAG_CFG_FILE);
        auto writeTime  = OS::getFileWriteTime(cfgFile.string().c_str());
        auto file       = addFileToModule(theModule, allFiles, cfgFile.parent_path(), SWAG_CFG_FILE, writeTime);
        file->isCfgFile = true;
    }

    // Sort files, and register them in a constant order
    if (!allFiles.empty())
    {
        sort(allFiles.begin(), allFiles.end(), [](SourceFile* a, SourceFile* b)
             { return strcmp(a->name.c_str(), b->name.c_str()) == -1; });
        for (auto file : allFiles)
            theModule->addFile(file);
    }
}

void EnumerateModuleJob::loadFilesInModules(const Path& basePath)
{
    // Scan source folder
    OS::visitFolders(basePath.string().c_str(),
                     [&](const char* cFileName)
                     {
                         auto path = basePath;
                         path.append(cFileName);
                         path = g_ModuleCfgMgr->getAliasPath(path);
                         path.append(SWAG_SRC_FOLDER);

                         auto module                                 = Allocator::alloc<Module>();
                         g_Workspace->mapFirstPassModulesNames[path] = module;

                         // Scan source folder
                         Vector<Path> directories;
                         directories.push_back(path);

                         Path tmp, tmp1;
                         while (directories.size())
                         {
                             tmp = std::move(directories.back());
                             directories.pop_back();

                             OS::visitFilesFolders(tmp.string().c_str(),
                                                   [&](uint64_t writeTime, const char* cFileName, bool isFolder)
                                                   {
                                                       if (isFolder)
                                                       {
                                                           tmp1 = tmp;
                                                           tmp1.append(cFileName);
                                                           directories.emplace_back(std::move(tmp1));
                                                       }
                                                       else
                                                       {
                                                           auto file     = Allocator::alloc<SourceFile>();
                                                           file->module  = module;
                                                           file->name    = cFileName;
                                                           Path pathFile = tmp;
                                                           pathFile.append(cFileName);
                                                           file->path      = pathFile;
                                                           file->writeTime = writeTime;
                                                           module->files.push_back(file);

                                                           auto pz = strrchr(cFileName, '.');
                                                           if (pz && !_strcmpi(pz, ".swg"))
                                                           {
                                                               auto readFileJob        = Allocator::alloc<LoadSourceFileJob>();
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
    g_Workspace->computeModuleName(path, moduleName, moduleFolder, kind);

    // Create theModule
    auto theModule = g_Workspace->createOrUseModule(moduleName, moduleFolder, kind);

    // Parse all files in the source tree
    enumerateFilesInModule(path, theModule);
    return theModule;
}

void EnumerateModuleJob::enumerateModules(const Path& path)
{
    Vector<string> allModules;

    // Scan source folder
    OS::visitFolders(path.string().c_str(),
                     [&](const char* cFolderName)
                     {
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
        sort(allModules.begin(), allModules.end());
        for (auto m : allModules)
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
        if (g_CommandLine.test)
            loadFilesInModules(g_Workspace->testsPath);
        return JobResult::ReleaseJob;
    }

    // Modules in the workspace
    if (!g_CommandLine.scriptCommand)
    {
        enumerateModules(g_Workspace->dependenciesPath);
        enumerateModules(g_Workspace->modulesPath);
        if (g_CommandLine.test)
            enumerateModules(g_Workspace->testsPath);
    }
    else
    {
        enumerateModules(g_Workspace->dependenciesPath);

        // If we are in script mode, then we add one single module with the script file
        auto parentFolder          = Path(g_CommandLine.scriptName.c_str()).parent_path();
        auto file                  = Allocator::alloc<SourceFile>();
        file->name                 = Path(g_CommandLine.scriptName).filename().replace_extension().string();
        auto scriptModule          = g_Workspace->createOrUseModule(file->name, parentFolder, ModuleKind::Script);
        file->path                 = g_CommandLine.scriptName;
        file->module               = scriptModule;
        file->isScriptFile         = true;
        scriptModule->isScriptFile = true;
        scriptModule->addFile(file);
        g_Workspace->runModule = scriptModule;

        if (!dealWithIncludes(scriptModule))
            return JobResult::ReleaseJob;
    }

    // Deal with embedded modules
    for (auto m : g_Workspace->modules)
    {
        if (!m->buildCfg.embeddedImports)
            continue;

        Vector<SourceFile*>             allFiles;
        VectorNative<ModuleDependency*> toKeep;
        for (int i = (int) m->moduleDependencies.size() - 1; i >= 0; i--)
        {
            auto dep = m->moduleDependencies[i];
            auto mod = g_Workspace->getModuleByName(dep->name);
            if (!mod)
            {
                toKeep.push_back(dep);
                continue;
            }

            m->moduleEmbedded.push_back(mod);

            for (auto f : mod->files)
            {
                if (f->isCfgFile)
                    continue;
                auto newFile                  = addFileToModule(m, allFiles, f->path.parent_path(), f->name, f->writeTime, nullptr, mod);
                newFile->isEmbedded           = true;
                newFile->globalUsingsEmbedded = mod->buildParameters.globalUsings;
            }

            // Add the dependencies of the embedded module too
            for (auto otherDep : mod->moduleDependencies)
            {
                auto cpt = m->moduleDependencies.size();

                if (!m->addDependency(otherDep->node, otherDep->tokenLocation, otherDep->tokenVersion))
                    return JobResult::ReleaseJob;

                if (m->moduleDependencies.size() > cpt)
                {
                    auto cfgModule = g_ModuleCfgMgr->getCfgModule(mod->name);

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
            sort(allFiles.begin(), allFiles.end(), [](SourceFile* a, SourceFile* b)
                 { return strcmp(a->name.c_str(), b->name.c_str()) == -1; });
            for (auto file : allFiles)
                m->addFile(file);
        }

        m->moduleDependencies.clear();
        m->moduleDependencies.append(toKeep);
    }

    return JobResult::ReleaseJob;
}
