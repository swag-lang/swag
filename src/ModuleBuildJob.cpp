#include "pch.h"
#include "ModuleBuildJob.h"
#include "Workspace.h"
#include "ModuleSemanticJob.h"
#include "ModuleOutputJob.h"
#include "Backend.h"
#include "CopyFileJob.h"
#include "SemanticJob.h"
#include "Module.h"
#include "ModuleRunJob.h"
#include "ByteCodeOptimizer.h"
#include "Context.h"
#include "ModuleManager.h"
#include "ThreadManager.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "LoadSourceFileJob.h"
#include "ScopedLock.h"

bool ModuleBuildJob::loadDependency(ModuleDependency* dep)
{
    auto depModule = dep->module;
    if (dep->importDone)
        return true;

    // Some dependencies can have been added by this stage
    if (!depModule)
    {
        depModule = g_Workspace.getModuleByName(dep->name);
        SWAG_ASSERT(depModule);
        depModule->allocateBackend();
        dep->module = depModule;
    }

    dep->importDone = true;

    VectorNative<SourceFile*> files;
    SWAG_ASSERT(depModule->backend);

    // Add all public files from the dependency module
    auto publicPath = depModule->publicPath;
    if (fs::exists(publicPath.c_str()))
    {
        OS::visitFiles(publicPath.c_str(), [&](const char* filename) {
            auto pz = strrchr(filename, '.');
            if (pz && !_strcmpi(pz, ".swg"))
            {
                auto file  = g_Allocator.alloc<SourceFile>();
                file->name = filename;
                file->path = publicPath + "/";
                file->path += filename;
                file->path     = Utf8::normalizePath(file->path);
                file->imported = depModule;
                files.push_back(file);
            }
        });
    }

    // One syntax job per dependency file
    for (auto one : files)
    {
        module->addFile(one);

        auto syntaxJob          = g_Allocator.alloc<SyntaxJob>();
        syntaxJob->sourceFile   = one;
        syntaxJob->module       = module;
        syntaxJob->dependentJob = this;
        jobsToAdd.push_back(syntaxJob);
    }

    return true;
}

JobResult ModuleBuildJob::execute()
{
    JobContext context;
    context.baseJob = this;
    baseContext     = &context;

    // Init module
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Init)
    {
        if (module->kind != ModuleKind::BootStrap && module->kind != ModuleKind::Runtime)
            module->initFrom(g_Workspace.runtimeModule);
        if (fromError)
            pass = ModuleBuildPass::IncludeSwg;
        else if (module->kind == ModuleKind::Config)
            pass = ModuleBuildPass::SemanticModule;
        else
            pass = ModuleBuildPass::Dependencies;
    }

    // Wait for dependencies to be build
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Dependencies)
    {
        module->allocateBackend();
        module->backend->setupExportFile();

        for (auto& dep : module->moduleDependencies)
        {
            auto depModule = g_Workspace.getModuleByName(dep->name);
            if (!depModule)
            {
                module->error(Utf8::format(Msg0499, dep->name.c_str()));
                return JobResult::ReleaseJob;
            }

            dep->module = depModule;

            if (depModule->numErrors)
                return JobResult::ReleaseJob;

            // If this module is not done, wait for it
            ScopedLock lk1(depModule->mutexDependency);
            if ((depModule->hasBeenBuilt & BUILDRES_EXPORT) == 0)
            {
                depModule->dependentJobs.add(this);
                return JobResult::KeepJobAlive;
            }
        }

        pass = ModuleBuildPass::Syntax;
    }

    // Syntax pass
    // By doing it after the dependency pass, we can avoid parsing all
    // the files if we do not have to recompile because none of our
    // dependencies have changed (and us neither of course)
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Syntax)
    {
        // Determine now if we need to recompile
        module->backend->setMustCompile();

        // If we do not need to compile, then exit, we're done with that module
        bool mustBuild = true;
        if (!module->backend->mustCompile && !g_CommandLine.test)
        {
            mustBuild = false;

            // Force the build if we will run that module in bytecode mode
            if (g_CommandLine.scriptMode && g_Workspace.runModule == module)
                mustBuild = true;
        }

        if (!mustBuild)
        {
            pass = ModuleBuildPass::WaitForDependencies;
        }
        else
        {
            pass = ModuleBuildPass::Publish;

            for (auto file : module->files)
            {
                auto syntaxJob          = g_Allocator.alloc<SyntaxJob>();
                syntaxJob->sourceFile   = file;
                syntaxJob->module       = module;
                syntaxJob->dependentJob = this;
                if (!file->buffer)
                {
                    auto loadFileJob        = g_Allocator.alloc<LoadSourceFileJob>();
                    loadFileJob->sourceFile = file;
                    loadFileJob->jobsToAdd.push_back(syntaxJob);
                    loadFileJob->dependentJob = this;
                    jobsToAdd.push_back(loadFileJob);
                }
                else
                    jobsToAdd.push_back(syntaxJob);
            }

            if (!jobsToAdd.empty())
                return JobResult::KeepJobAlive;
        }
    }

    // Publish exported files
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Publish)
    {
        if (g_CommandLine.buildPass <= BuildPass::Syntax)
            return JobResult::ReleaseJob;

        // Need to run all error modules
        // Each file with a #global testerror on top is now in a dedicated module, so we need to
        // run them in case the error has not been triggered during the syntax pass
        for (auto errorMd : module->errorModules)
        {
            auto job       = g_Allocator.alloc<ModuleBuildJob>();
            job->module    = errorMd;
            job->fromError = true;
            g_ThreadMgr.addJob(job);
        }

        pass = ModuleBuildPass::IncludeSwg;
        if (g_CommandLine.output && !module->path.empty() && module->kind != ModuleKind::Test)
        {
            publishFilesToPublic();
            publishFilesToTarget();
            if (!jobsToAdd.empty())
                return JobResult::KeepJobAlive;
        }
    }

    // We add all public files that corresponds
    // to each module we want to import
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::IncludeSwg)
    {
        for (auto& dep : module->moduleDependencies)
        {
            if (!loadDependency(dep))
                return JobResult::ReleaseJob;
        }

        // Sync with all jobs
        if (!jobsToAdd.empty())
            return JobResult::KeepJobAlive;

        pass = ModuleBuildPass::SemanticModule;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::SemanticModule)
    {
        pass = ModuleBuildPass::AfterSemantic;
        if (module->numErrors)
            return JobResult::ReleaseJob;

        auto semanticJob          = g_Allocator.alloc<ModuleSemanticJob>();
        semanticJob->module       = module;
        semanticJob->dependentJob = this;
        jobsToAdd.push_back(semanticJob);
        return JobResult::KeepJobAlive;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::AfterSemantic)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;

        // Send all compiler messages
        if (!module->flushCompilerMessages(&context))
            return JobResult::ReleaseJob;
        if (context.result != ContextResult::Done)
            return JobResult::KeepJobAlive;

        if (module->kind == ModuleKind::Config)
            pass = ModuleBuildPass::OptimizeBc;
        else
            pass = ModuleBuildPass::WaitForDependencies;

        module->sendCompilerMessage(CompilerMsgKind::PassAfterSemantic, this);

        // This is a dummy job, in case the user code does not trigger new jobs during the
        // CompilerMsgKind::PassAfterSemantic pass
        auto semanticJob          = g_Allocator.alloc<ModuleSemanticJob>();
        semanticJob->module       = nullptr;
        semanticJob->dependentJob = this;
        jobsToAdd.push_back(semanticJob);

        return JobResult::KeepJobAlive;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::WaitForDependencies)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;

        // Close generated file
        if (module->handleGeneratedFile)
        {
            fclose(module->handleGeneratedFile);
            module->handleGeneratedFile = nullptr;
        }

        pass = ModuleBuildPass::WaitForDependenciesEffective;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::WaitForDependenciesEffective)
    {
        // If we will not run some stuff, then no need to wait for dependencies, because we do not
        // have to load the dlls
        if (module->hasBytecodeToRun())
        {
            if (!module->waitForDependenciesDone(this))
                return JobResult::KeepJobAlive;
        }

        pass = ModuleBuildPass::OptimizeBc;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::OptimizeBc)
    {
        bool done = false;
        if (!ByteCodeOptimizer::optimize(this, module, done))
            return JobResult::ReleaseJob;
        if (!done)
            return JobResult::KeepJobAlive;

        module->printBC();

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // Now we can patch all methods pointers in type definitions
        module->mutableSegment.doPatchMethods(&context);
        module->tlsSegment.doPatchMethods(&context);
        module->constantSegment.doPatchMethods(&context);

        if (!module->hasBytecodeToRun())
            pass = ModuleBuildPass::Output;
        else
            pass = ModuleBuildPass::RunByteCode;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::RunByteCode)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;

        module->sendCompilerMessage(CompilerMsgKind::PassBeforeRunByteCode, this);

        // Push a copy of the default context, in case the user code changes it (or push a new one)
        PushSwagContext cxt;

        // Setup runtime
        auto setupFct = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name__setupRuntime);
        SWAG_ASSERT(setupFct);
        module->executeNode(setupFct->node->sourceFile, setupFct->node, baseContext);
        if (module->criticalErrors)
            return JobResult::ReleaseJob;

        // #init functions are only executed in script mode, if the module has a #main
        bool callInitDrop = !module->byteCodeInitFunc.empty() && g_CommandLine.scriptMode && module->byteCodeMainFunc;
        // OR in a test module, during testing
        callInitDrop |= g_CommandLine.test && g_CommandLine.runByteCodeTests;
        if (callInitDrop)
        {
            for (auto func : module->byteCodeInitFunc)
            {
                module->executeNode(func->node->sourceFile, func->node, baseContext);
                if (module->criticalErrors)
                    return JobResult::ReleaseJob;
            }
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // #run functions are always executed
        if (!module->byteCodeRunFunc.empty())
        {
            // A #run pass cannot modify a bss variable
            module->bssCannotChange = true;

            for (auto func : module->byteCodeRunFunc)
            {
                if (g_CommandLine.stats)
                    g_Stats.runFunctions++;
                module->executeNode(func->node->sourceFile, func->node, baseContext);
                if (module->criticalErrors)
                    return JobResult::ReleaseJob;
            }

            module->bssCannotChange = false;
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // #test functions are only executed in test mode
        if (g_CommandLine.test && g_CommandLine.runByteCodeTests)
        {
            if (!module->byteCodeTestFunc.empty())
            {
                // Modified global variables during test will be restored after
                module->saveBssValues     = true;
                module->saveMutableValues = true;

                for (auto func : module->byteCodeTestFunc)
                {
                    if (g_CommandLine.stats)
                        g_Stats.testFunctions++;
                    module->executeNode(func->node->sourceFile, func->node, baseContext);
                    if (module->criticalErrors)
                        return JobResult::ReleaseJob;
                }

                module->bssSegment.restoreAllValues();
                module->mutableSegment.restoreAllValues();
                module->saveBssValues     = false;
                module->saveMutableValues = false;
            }
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // #main function, in script mode
        if (module->byteCodeMainFunc && g_CommandLine.scriptMode)
        {
            module->executeNode(module->byteCodeMainFunc->node->sourceFile, module->byteCodeMainFunc->node, baseContext);
            if (module->criticalErrors)
                return JobResult::ReleaseJob;
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // #drop functions
        if (callInitDrop)
        {
            for (auto func : module->byteCodeDropFunc)
            {
                module->executeNode(func->node->sourceFile, func->node, baseContext);
                if (module->criticalErrors)
                    return JobResult::ReleaseJob;
            }
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        pass = ModuleBuildPass::Output;
    }

    // Output pass
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Output)
    {
        // During unit testing, be sure we don't have untriggered errors
        if (g_CommandLine.test && g_CommandLine.runByteCodeTests)
        {
            for (auto file : module->files)
            {
                if (file->numTestErrors)
                {
                    if (g_CommandLine.testFilter.empty() || strstr(file->name, g_CommandLine.testFilter.c_str()))
                    {
                        auto nb             = file->numTestErrors.load();
                        file->numTestErrors = 0;
                        file->report({file, Utf8::format(Msg0500, nb, file->numErrors)});
                    }
                }

                if (file->numTestWarnings)
                {
                    if (g_CommandLine.testFilter.empty() || strstr(file->name, g_CommandLine.testFilter.c_str()))
                    {
                        auto nb               = file->numTestWarnings.load();
                        file->numTestWarnings = 0;
                        file->report({file, Utf8::format(Msg0501, nb, file->numWarnings)});
                    }
                }
            }
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        if (module->kind == ModuleKind::Config || module->kind == ModuleKind::Script)
            pass = ModuleBuildPass::Done;
        else
        {
            // Do not run native tests or command in script mode, it's already done in bytecode
            if (g_CommandLine.scriptMode)
                pass = ModuleBuildPass::Done;
            else
                pass = ModuleBuildPass::RunNative;

            if (module->mustOutputSomething())
            {
                if (g_CommandLine.output)
                {
                    module->sendCompilerMessage(CompilerMsgKind::PassBeforeOutput, this);
                    auto outputJob          = g_Allocator.alloc<ModuleOutputJob>();
                    outputJob->module       = module;
                    outputJob->dependentJob = this;
                    jobsToAdd.push_back(outputJob);
                    return JobResult::KeepJobAlive;
                }
                else if (module->backend->mustCompile)
                {
                    OS::touchFile(module->backend->exportFilePath);
                }
            }
        }
    }

    // Run pass
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::RunNative)
    {
        pass = ModuleBuildPass::Done;
        if (module->numErrors)
            return JobResult::ReleaseJob;

        // Run test executable
        if (module->mustGenerateTestExe() && g_CommandLine.runBackendTests)
        {
            auto job                            = g_Allocator.alloc<ModuleRunJob>();
            job->module                         = module;
            job->dependentJob                   = this;
            job->buildParameters                = module->buildParameters;
            job->buildParameters.outputFileName = module->name;
            job->buildParameters.compileType    = BackendCompileType::Test;
            jobsToAdd.push_back(job);
        }

        // Run command
        if (g_CommandLine.run && module->kind != ModuleKind::Test)
        {
            auto job                         = g_Allocator.alloc<ModuleRunJob>();
            job->module                      = module;
            job->dependentJob                = this;
            job->buildParameters             = module->buildParameters;
            job->buildParameters.compileType = BackendCompileType::Normal;
            jobsToAdd.push_back(job);
        }
    }

    module->release();
    module->sendCompilerMessage(CompilerMsgKind::PassAllDone, this);
    module->setHasBeenBuilt(BUILDRES_FULL);

    return JobResult::ReleaseJob;
}

void ModuleBuildJob::publishFilesToPublic()
{
    if (module->exportSourceFiles.empty())
        return;

    string publicPath = module->publicPath.c_str();
    if (publicPath.empty())
        return;

    // We need the public folder to be in sync with the current state of the code.
    // That means that every files in the public folder that is no more '#global export' must
    // be removed (and every old file that does not exist anymore)
    set<Utf8> publicFiles;
    for (auto one : module->exportSourceFiles)
    {
        auto name = one->name;
        name.makeUpper();
        publicFiles.insert(name);
    }

    OS::visitFiles(publicPath.c_str(), [&](const char* filename) {
        // Keep the generated file untouched !
        if (module->backend->exportFileName == filename)
            return;

        // If this is still a #public file, then do nothing. The job will erase it
        // if the one from the source code is more recent
        Utf8 pubName = filename;
        pubName.makeUpper();
        if (publicFiles.find(pubName) != publicFiles.end())
            return;

        // Otherwise, remove it !
        auto path = publicPath + "/";
        path += filename;
        error_code errorCode;
        fs::remove(path, errorCode);
    });

    // Add all #public files
    for (auto one : module->exportSourceFiles)
    {
        auto job          = g_Allocator.alloc<CopyFileJob>();
        job->module       = module;
        job->sourcePath   = one->path;
        job->destPath     = publicPath + "/" + one->name;
        job->dependentJob = this;
        jobsToAdd.push_back(job);
    }
}

void ModuleBuildJob::publishFilesToTarget()
{
    string publishPath = module->path + "/";
    publishPath += SWAG_PUBLISH_FOLDER;
    if (!fs::exists(publishPath))
        return;

    // Everything at the root of the /publish folder will be copied "as is" in the output directory, whatever the
    // current target is
    OS::visitFiles(publishPath.c_str(), [&](const char* cFileName) {
        auto job          = g_Allocator.alloc<CopyFileJob>();
        job->module       = module;
        job->sourcePath   = publishPath + "/" + cFileName;
        job->destPath     = g_Workspace.targetPath.string() + "/" + cFileName;
        job->dependentJob = this;
        jobsToAdd.push_back(job);
    });

    // Everything in a sub folder named 'os-arch' will be copied only if this matches the current os and arch
    auto osArchPath = publishPath;
    osArchPath += "/";
    osArchPath += Backend::GetOsName();
    osArchPath += "-";
    osArchPath += Backend::GetArchName();
    if (fs::exists(osArchPath))
    {
        OS::visitFiles(osArchPath.c_str(), [&](const char* cFileName) {
            auto job          = g_Allocator.alloc<CopyFileJob>();
            job->module       = module;
            job->sourcePath   = osArchPath + "/" + cFileName;
            job->destPath     = g_Workspace.targetPath.string() + "/" + cFileName;
            job->dependentJob = this;
            jobsToAdd.push_back(job);
        });
    }
}
