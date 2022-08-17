#include "pch.h"
#include "EnumerateModuleJob.h"
#include "Workspace.h"
#include "Os.h"
#include "Module.h"
#include "ModuleCfgManager.h"
#include "LoadSourceFileJob.h"
#include "ThreadManager.h"
#include "SyntaxJob.h"
#include "AstNode.h"
#include "Diagnostic.h"
#include "ErrorIds.h"

SourceFile* EnumerateModuleJob::addFileToModule(Module* theModule, vector<SourceFile*>& allFiles, string dirName, string fileName, uint64_t writeTime, SourceFile* prePass, Module* imported)
{
    auto file       = g_Allocator.alloc<SourceFile>();
    file->fromTests = theModule->kind == ModuleKind::Test;
    file->name      = fileName;
    file->imported  = imported;

    if (prePass)
    {
        file->path            = dirName;
        file->buffer          = prePass->buffer;
        file->curBuffer       = prePass->curBuffer;
        file->endBuffer       = prePass->endBuffer;
        file->bufferSize      = prePass->bufferSize;
        file->allocBufferSize = prePass->allocBufferSize;
    }
    else
    {
        fs::path pathFile = dirName.c_str();
        pathFile.append(fileName);
        file->path = Utf8::normalizePath(pathFile);
    }

    file->writeTime = writeTime;

    // If we have only one core, then we will sort files in alphabetical order to always
    // treat them in a reliable order. That way, --randomize and --seed can work.
    if (g_CommandLine->numCores == 1 || g_CommandLine->scriptCommand)
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
        auto syntaxJob        = g_Allocator.alloc<SyntaxJob>();
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
    vector<SourceFile*> allFiles;

    // Treat includes
    for (auto n : theModule->includes)
    {
        fs::path orgFilePath = n->token.text.c_str();

        // Is this a simple file ?
        auto filePath = orgFilePath;
        if (!fs::exists(filePath))
        {
            filePath = theModule->path;
            filePath.append(orgFilePath.string());
            filePath = fs::absolute(filePath).string();
            error_code errorCode;
            auto       filePath1 = fs::canonical(filePath, errorCode).string();
            if (!errorCode)
                filePath = filePath1;
            if (!fs::exists(filePath))
            {
                Diagnostic diag{n->sourceFile, n->token, Fmt(Err(Err0304), n->token.text.c_str())};
                n->sourceFile->report(diag);
                return false;
            }
        }

        auto fileName  = filePath.filename().string();
        auto writeTime = OS::getFileWriteTime(filePath.string().c_str());
        addFileToModule(theModule, allFiles, fs::path(filePath.c_str()).parent_path().string(), fileName, writeTime);
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

void EnumerateModuleJob::enumerateFilesInModule(const fs::path& basePath, Module* theModule)
{
    vector<SourceFile*> allFiles;

    auto path = basePath;
    path      = g_ModuleCfgMgr->getAliasPath(path).string();
    path += "/";
    path += SWAG_SRC_FOLDER;
    path += "/";

    if (!dealWithIncludes(theModule))
        return;

    // Is the list of files already computed ?
    auto it = g_Workspace->mapFirstPassModulesNames.find(path.string());
    if (it != g_Workspace->mapFirstPassModulesNames.end())
    {
        for (auto f : it->second->files)
        {
            if (theModule->kind != ModuleKind::Test || g_CommandLine->testFilter.empty() || strstr(f->name, g_CommandLine->testFilter.c_str()))
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
    vector<string> directories;
    directories.push_back(path.string());

    string   tmp, tmp1;
    fs::path modulePath;
    while (directories.size())
    {
        tmp = move(directories.back());
        directories.pop_back();

        OS::visitFilesFolders(tmp.c_str(),
                              [&](uint64_t writeTime, const char* cFileName, bool isFolder)
                              {
                                  if (isFolder)
                                  {
                                      tmp1 = tmp + "/" + cFileName;
                                      directories.emplace_back(move(tmp1));
                                  }
                                  else
                                  {
                                      if (theModule->kind != ModuleKind::Test || g_CommandLine->testFilter.empty() || strstr(cFileName, g_CommandLine->testFilter.c_str()))
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
        cfgFile      = g_ModuleCfgMgr->getAliasPath(cfgFile).string();
        cfgFile += "/";
        cfgFile += SWAG_CFG_FILE;
        auto writeTime  = OS::getFileWriteTime(cfgFile.c_str());
        auto file       = addFileToModule(theModule, allFiles, fs::path(cfgFile).parent_path().string(), SWAG_CFG_FILE, writeTime);
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

void EnumerateModuleJob::loadFilesInModules(const fs::path& basePath)
{
    // Scan source folder
    OS::visitFolders(basePath.string().c_str(),
                     [&](const char* cFileName)
                     {
                         string path = basePath.string() + cFileName;
                         path        = g_ModuleCfgMgr->getAliasPath(path).string();
                         path += "/";
                         path += SWAG_SRC_FOLDER;
                         path += "/";

                         auto module                                 = g_Allocator.alloc<Module>();
                         g_Workspace->mapFirstPassModulesNames[path] = module;

                         // Scan source folder
                         vector<string> directories;
                         directories.push_back(path);

                         string tmp, tmp1;
                         while (directories.size())
                         {
                             tmp = move(directories.back());
                             directories.pop_back();

                             OS::visitFilesFolders(tmp.c_str(),
                                                   [&](uint64_t writeTime, const char* cFileName, bool isFolder)
                                                   {
                                                       if (isFolder)
                                                       {
                                                           tmp1 = tmp + "/" + cFileName;
                                                           directories.emplace_back(move(tmp1));
                                                       }
                                                       else
                                                       {
                                                           auto file         = g_Allocator.alloc<SourceFile>();
                                                           file->module      = module;
                                                           file->name        = cFileName;
                                                           fs::path pathFile = tmp.c_str();
                                                           pathFile.append(cFileName);
                                                           file->path      = Utf8::normalizePath(pathFile);
                                                           file->writeTime = writeTime;
                                                           module->files.push_back(file);

                                                           auto pz = strrchr(cFileName, '.');
                                                           if (pz && !_strcmpi(pz, ".swg"))
                                                           {
                                                               auto readFileJob        = g_Allocator.alloc<LoadSourceFileJob>();
                                                               readFileJob->flags      = JOB_IS_OPT;
                                                               readFileJob->sourceFile = file;
                                                               g_ThreadMgr.addJob(readFileJob);
                                                           }
                                                       }
                                                   });
                         }
                     });
}

Module* EnumerateModuleJob::addModule(const fs::path& path)
{
    Utf8       moduleName, moduleFolder;
    ModuleKind kind;

    // Get infos about module, depending on where it is located
    g_Workspace->computeModuleName(path, moduleName, moduleFolder, kind);

    // Create theModule
    auto theModule = g_Workspace->createOrUseModule(moduleName, moduleFolder, kind);

    // Parse all files in the source tree
    enumerateFilesInModule(path, theModule);
    return theModule;
}

void EnumerateModuleJob::enumerateModules(const fs::path& path)
{
    vector<string> allModules;

    // Scan source folder
    OS::visitFolders(path.string().c_str(),
                     [&](const char* cFolderName)
                     {
                         // If we have only one core, then we will sort modules in alphabetical order to always
                         // treat them in a reliable order. That way, --randomize and --seed can work.
                         if (g_CommandLine->numCores == 1)
                             allModules.push_back(cFolderName);
                         else
                             addModule(path.string() + cFolderName);
                     });

    // Sort modules, and register them in a constant order
    if (!allModules.empty())
    {
        sort(allModules.begin(), allModules.end());
        for (auto m : allModules)
        {
            addModule(path.string() + m);
        }
    }
}

JobResult EnumerateModuleJob::execute()
{
    // Just scan the files, and load them, as optional jobs, during the build setup stage
    if (readFileMode)
    {
        SWAG_ASSERT(!g_CommandLine->scriptCommand);
        loadFilesInModules(g_Workspace->dependenciesPath);
        loadFilesInModules(g_Workspace->modulesPath);
        loadFilesInModules(g_Workspace->examplesPath);
        if (g_CommandLine->test)
            loadFilesInModules(g_Workspace->testsPath);
        return JobResult::ReleaseJob;
    }

    // Modules in the workspace
    if (!g_CommandLine->scriptCommand)
    {
        enumerateModules(g_Workspace->dependenciesPath);
        enumerateModules(g_Workspace->modulesPath);
        enumerateModules(g_Workspace->examplesPath);
        if (g_CommandLine->test)
            enumerateModules(g_Workspace->testsPath);
    }
    else
    {
        enumerateModules(g_Workspace->dependenciesPath);

        // If we are in script mode, then we add one single module with the script file
        auto parentFolder          = fs::path(g_CommandLine->scriptName.c_str()).parent_path().string();
        auto file                  = g_Allocator.alloc<SourceFile>();
        file->name                 = fs::path(g_CommandLine->scriptName).filename().string();
        auto scriptModule          = g_Workspace->createOrUseModule(file->name, parentFolder, ModuleKind::Script);
        file->path                 = g_CommandLine->scriptName;
        file->module               = scriptModule;
        file->isScriptFile         = true;
        scriptModule->isScriptFile = true;
        scriptModule->addFile(file);
        g_Workspace->runModule = scriptModule;

        if (!dealWithIncludes(scriptModule))
            return JobResult::ReleaseJob;
    }

    // Deal with embbeded modules
    for (auto m : g_Workspace->modules)
    {
        if (!m->buildCfg.embbedImports)
            continue;

        vector<SourceFile*>             allFiles;
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

            m->moduleEmbbeded.push_back(mod);

            for (auto f : mod->files)
            {
                if (f->isCfgFile)
                    continue;
                auto newFile                = addFileToModule(m, allFiles, fs::path(f->path).parent_path().string(), f->name.c_str(), f->writeTime, nullptr, mod);
                newFile->isEmbbeded         = true;
                newFile->globalUsingsEmbbed = mod->buildParameters.globalUsings;
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
