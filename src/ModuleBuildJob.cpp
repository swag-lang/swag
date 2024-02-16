#include "pch.h"
#include "ModuleBuildJob.h"
#include "Backend.h"
#include "ByteCodeOptimizer.h"
#include "Context.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "FileJob.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "ModuleGenDocJob.h"
#include "ModuleOutputJob.h"
#include "ModuleRunJob.h"
#include "ModuleSemanticJob.h"
#include "Report.h"
#include "SyntaxJob.h"
#include "ThreadManager.h"
#include "Workspace.h"

void ModuleBuildJob::publishFilesToPublic(const Module* moduleToPublish)
{
    if (module->exportSourceFiles.empty())
        return;

    const auto publicPath = moduleToPublish->publicPath;
    if (publicPath.empty())
        return;

    // We need the public folder to be in sync with the current state of the code.
    // That means that every files in the public folder that is no more '#global export' must
    // be removed (and every old file that does not exist anymore)
    SetUtf8 publicFiles;
    for (const auto one : moduleToPublish->exportSourceFiles)
    {
        auto name = one->name;
        name.makeUpper();
        publicFiles.insert(name);
    }

    OS::visitFiles(publicPath.string().c_str(),
                   [&](const char* filename)
                   {
                       // Keep the generated file untouched !
                       if (moduleToPublish->backend->exportFileName == filename)
                           return;

                       // If this is still a #public file, then do nothing. The job will erase it
                       // if the one from the source code is more recent
                       Utf8 pubName = filename;
                       pubName.makeUpper();
                       if (publicFiles.contains(pubName))
                           return;

                       // Otherwise, remove it !
                       auto path = publicPath;
                       path.append(filename);
                       error_code err;
                       filesystem::remove(path, err);
                   });

    // Add all #public files
    for (const auto one : moduleToPublish->exportSourceFiles)
    {
        const auto job  = Allocator::alloc<CopyFileJob>();
        job->module     = module;
        job->sourcePath = one->path;
        job->destPath   = publicPath;
        job->destPath.append(one->name.c_str());
        job->dependentJob = this;
        jobsToAdd.push_back(job);
    }
}

void ModuleBuildJob::publishFilesToTarget(const Module* moduleToPublish)
{
    Path publishPath = moduleToPublish->path;
    publishPath.append(SWAG_PUBLISH_FOLDER);
    error_code err;
    if (!exists(publishPath, err))
        return;

    // Everything at the root of the /publish folder will be copied "as is" in the output directory, whatever the
    // current target is
    OS::visitFiles(publishPath.string().c_str(),
                   [&](const char* cFileName)
                   {
                       const auto job  = Allocator::alloc<CopyFileJob>();
                       job->module     = module;
                       job->sourcePath = publishPath;
                       job->sourcePath.append(cFileName);
                       job->destPath = g_Workspace->targetPath;
                       job->destPath.append(cFileName);
                       job->dependentJob = this;
                       jobsToAdd.push_back(job);
                   });

    // Everything in a sub folder named 'os-arch' will be copied only if this matches the current os and arch
    auto osArchPath = publishPath;
    osArchPath.append(Backend::getOsName(g_CommandLine.target));
    osArchPath += "-";
    osArchPath += Backend::getArchName(g_CommandLine.target);
    if (exists(osArchPath, err))
    {
        OS::visitFiles(osArchPath.string().c_str(),
                       [&](const char* cFileName)
                       {
                           const auto job  = Allocator::alloc<CopyFileJob>();
                           job->module     = module;
                           job->sourcePath = osArchPath;
                           job->sourcePath.append(cFileName);
                           job->destPath = g_Workspace->targetPath;
                           job->destPath.append(cFileName);
                           job->dependentJob = this;
                           jobsToAdd.push_back(job);
                       });
    }
}

void ModuleBuildJob::publishFilesToPublic()
{
    publishFilesToPublic(module);
    for (const auto m : module->moduleEmbedded)
    {
        if (!m->addedToBuild)
            publishFilesToPublic(m);
    }
}

void ModuleBuildJob::publishFilesToTarget()
{
    publishFilesToTarget(module);
    for (const auto m : module->moduleEmbedded)
    {
        if (!m->addedToBuild)
            publishFilesToTarget(m);
    }
}

bool ModuleBuildJob::loadDependency(ModuleDependency* dep)
{
    if (dep->importDone)
        return true;

    // Some dependencies can have been added by this stage
    auto depModule = dep->module;
    if (!depModule)
    {
        depModule = g_Workspace->getModuleByName(dep->name);
        SWAG_ASSERT(depModule);
        dep->module = depModule;
    }

    dep->importDone = true;
    depModule->allocateBackend();
    return loadDependency(depModule);
}

bool ModuleBuildJob::loadDependency(Module* depModule)
{
    // Add all public files from the dependency module
    VectorNative<SourceFile*> files;
    const auto                publicPath = depModule->publicPath;
    error_code                err;
    if (filesystem::exists(publicPath.c_str(), err))
    {
        OS::visitFiles(publicPath.string().c_str(),
                       [&](const char* filename)
                       {
                           const auto pz = strrchr(filename, '.');
                           if (pz && !_strcmpi(pz, ".swg"))
                           {
                               const auto file = Allocator::alloc<SourceFile>();
                               file->name      = filename;
                               file->path      = publicPath;
                               file->path.append(filename);
                               file->imported = depModule;
                               files.push_back(file);
                           }
                       });
    }

    // One syntax job per dependency file
    for (const auto one : files)
    {
        module->addFile(one);

        const auto syntaxJob    = Allocator::alloc<SyntaxJob>();
        syntaxJob->sourceFile   = one;
        syntaxJob->module       = module;
        syntaxJob->dependentJob = this;
        jobsToAdd.push_back(syntaxJob);
    }

    return true;
}

void ModuleBuildJob::checkMissingErrors() const
{
    // During unit testing, be sure we don't have un-triggered errors
    if (g_CommandLine.test && g_CommandLine.runByteCodeTests)
    {
        for (auto file : module->files)
        {
            if (file->shouldHaveError && !file->numErrors)
            {
                if (g_CommandLine.testFilter.empty() || file->name.containsNoCase(g_CommandLine.testFilter))
                {
                    file->shouldHaveError = false;
                    Report::report({file, file->tokenHasError, Err(Err0582)});
                }
            }

            if (file->shouldHaveWarning && !file->numWarnings)
            {
                if (g_CommandLine.testFilter.empty() || file->name.containsNoCase(g_CommandLine.testFilter))
                {
                    file->shouldHaveWarning = false;
                    Report::report({file, file->tokenHasWarning, Err(Err0583)});
                }
            }
        }
    }
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
        module->logPass(pass);
        module->logStage("ModuleBuildPass::Init\n");
        if (module->kind != ModuleKind::BootStrap && module->kind != ModuleKind::Runtime)
            module->initFrom(g_Workspace->runtimeModule);
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
        module->logPass(pass);
        module->logStage("ModuleBuildPass::Dependencies\n");
        module->allocateBackend();
        module->backend->setupExportFile();

        for (auto& dep : module->moduleDependencies)
        {
            auto depModule = g_Workspace->getModuleByName(dep->name);
            if (!depModule)
            {
                Report::error(module, FMT(Err(Err0707), dep->name.c_str()));
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

        // If we have an embedded module, and that module is also compiled as a normal module,
        // then we need to wait for it to have published its files
        for (auto& depModule : module->moduleEmbedded)
        {
            if (!depModule->addedToBuild)
                continue;
            ScopedLock lk1(depModule->mutexDependency);
            if ((depModule->hasBeenBuilt & BUILDRES_PUBLISH) == 0)
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
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::Syntax\n");

        // Determine now if we need to recompile
        module->backend->setMustCompile();

        // If we do not need to compile, then exit, we're done with that module
        bool mustBuild = true;
        if (!module->backend->mustCompile && !g_CommandLine.test)
        {
            // Force the build if we will run that module in bytecode mode
            if (!g_CommandLine.scriptMode || g_Workspace->runModule != module)
                mustBuild = false;
        }

        if (!mustBuild)
        {
            module->syntaxJobGroup.waitAndClear();
            pass = ModuleBuildPass::WaitForDependencies;
        }
        else
        {
            pass = ModuleBuildPass::Publish;
            module->syntaxJobGroup.complete(this);

            // When we are synchronized, do it now, as syntaxGroup is not relevant
            if (g_ThreadMgr.numWorkers == 1 || g_CommandLine.scriptCommand)
            {
                for (auto file : module->files)
                {
                    if (file->markDown)
                        continue;

                    auto syntaxJob          = Allocator::alloc<SyntaxJob>();
                    syntaxJob->sourceFile   = file;
                    syntaxJob->module       = module;
                    syntaxJob->dependentJob = this;
                    jobsToAdd.push_back(syntaxJob);
                }
            }

            if (!jobsToAdd.empty())
                return JobResult::KeepJobAlive;
        }
    }

    // Publish exported files
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Publish)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::Publish\n");
        if (g_CommandLine.buildPass <= BuildPass::Syntax)
            return JobResult::ReleaseJob;

        // Need to run all error modules
        // Each file with a #global test error on top is now in a dedicated module, so we need to
        // run them in case the error has not been triggered during the syntax pass
        {
            for (auto errorMd : module->errorModules)
            {
                auto job       = Allocator::alloc<ModuleBuildJob>();
                job->module    = errorMd;
                job->fromError = true;
                errorMd->inheritCfgFrom(module);
                g_ThreadMgr.addJob(job);
            }
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
        module->setHasBeenBuilt(BUILDRES_PUBLISH);

        module->logPass(pass);
        module->logStage("ModuleBuildPass::IncludeSwg\n");

        for (auto& dep : module->moduleDependencies)
        {
            if (!loadDependency(dep))
                return JobResult::ReleaseJob;
        }

        if (!jobsToAdd.empty())
            return JobResult::KeepJobAlive;

        pass = ModuleBuildPass::SemanticModule;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::SemanticModule)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::SemanticModule\n");

        module->buildModulesSlice();
        pass = ModuleBuildPass::BeforeCompilerMessagesPass0;
        addFlag(JOB_PENDING_PLACE_HOLDER); // We can now be relaunched if necessary

        auto semanticJob          = Allocator::alloc<ModuleSemanticJob>();
        semanticJob->module       = module;
        semanticJob->dependentJob = this;
        jobsToAdd.push_back(semanticJob);
        return JobResult::KeepJobAlive;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::BeforeCompilerMessagesPass0)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::BeforeCompilerMessagesPass0\n");

        pass = ModuleBuildPass::CompilerMessagesPass0;

        if (!module->prepareCompilerMessages(&context, 0))
            return JobResult::ReleaseJob;
        if (!jobsToAdd.empty())
            return JobResult::KeepJobAlive;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::CompilerMessagesPass0)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::CompilerMessagesPass0\n");

        if (!module->flushCompilerMessages(&context, 0, this))
            return JobResult::ReleaseJob;
        if (context.result != ContextResult::Done)
            return JobResult::KeepJobAlive;

        pass = ModuleBuildPass::BeforeCompilerMessagesPass1;

        // This is a dummy job, in case the user code does not trigger new jobs during the message pass
        auto semanticJob          = Allocator::alloc<ModuleSemanticJob>();
        semanticJob->module       = nullptr;
        semanticJob->dependentJob = this;
        jobsToAdd.push_back(semanticJob);

        return JobResult::KeepJobAlive;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::BeforeCompilerMessagesPass1)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::BeforeCompilerMessagesPass1\n");

        pass = ModuleBuildPass::AfterSemantic;

        if (!module->prepareCompilerMessages(&context, 1))
            return JobResult::ReleaseJob;
        if (!jobsToAdd.empty())
            return JobResult::KeepJobAlive;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::AfterSemantic)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::AfterSemantic\n");

        // This is too late for meta programming of 'impl' blocks...
        module->acceptsCompileImpl = false;

        if (!module->flushCompilerMessages(&context, 1, this))
            return JobResult::ReleaseJob;
        if (context.result != ContextResult::Done)
            return JobResult::KeepJobAlive;

        pass = ModuleBuildPass::OptimizeBc;
        removeFlag(JOB_PENDING_PLACE_HOLDER); // We can't be relaunched anymore

        module->sendCompilerMessage(CompilerMsgKind::PassAfterSemantic, this);

        // This is a dummy job, in case the user code does not trigger new jobs during the message pass
        auto semanticJob          = Allocator::alloc<ModuleSemanticJob>();
        semanticJob->module       = nullptr;
        semanticJob->dependentJob = this;
        jobsToAdd.push_back(semanticJob);

        return JobResult::KeepJobAlive;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::OptimizeBc)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::OptimizeBc\n");

        bool done = false;
        if (!ByteCodeOptimizer::optimize(this, module, done))
            return JobResult::ReleaseJob;
        if (!done)
            return JobResult::KeepJobAlive;

        module->printBC();

        pass = ModuleBuildPass::WaitForDependencies;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::WaitForDependencies)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::WaitForDependencies\n");

        if (module->kind != ModuleKind::Config)
        {
            if (!module->waitForDependenciesDone(this))
                return JobResult::KeepJobAlive;

            // Now we can patch all methods pointers in type definitions
            module->mutableSegment.doPatchMethods(&context);
            module->tlsSegment.doPatchMethods(&context);
            module->constantSegment.doPatchMethods(&context);

            // We can also build the tables for the current module
            module->buildTypesSlice();
            module->buildGlobalVarsToDropSlice();
        }

        pass = ModuleBuildPass::FlushGenFiles;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::FlushGenFiles)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::FlushGenFiles\n");

        module->flushGenFiles();

        if (!module->hasBytecodeToRun())
            pass = ModuleBuildPass::Output;
        else if (module->kind == ModuleKind::Script && !g_CommandLine.scriptRun)
            pass = ModuleBuildPass::Done;
        else
            pass = ModuleBuildPass::RunByteCode;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::RunByteCode)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logPass(pass);
        module->logStage("ModuleBuildPass::RunByteCode\n");

        module->initProcessInfos();
        module->sendCompilerMessage(CompilerMsgKind::PassBeforeRunByteCode, this);

        // Push a copy of the default context, in case the user code changes it (or push a new one)
        // :PushDefaultCxt
        PushSwagContext cxt;

        // Setup runtime
        auto setupFct = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name_priv_setupRuntime);
        SWAG_ASSERT(setupFct);

        module->logStage(FMT("__setupRuntime %s\n", setupFct->node->sourceFile->name.c_str()));
        ExecuteNodeParams execParams;
        auto              runtimeFlags = Backend::getRuntimeFlags(module);
        runtimeFlags |= static_cast<uint64_t>(SwagRuntimeFlags::FromCompiler);
        execParams.callParams.push_back(runtimeFlags);
        module->executeNode(setupFct->node->sourceFile, setupFct->node, baseContext, &execParams);

        if (module->criticalErrors)
            return JobResult::ReleaseJob;

        // This is too late for meta programming...
        module->acceptsCompileString = false;

        // #init functions are only executed in script mode
        bool callInitDrop = !module->byteCodeInitFunc.empty() && g_CommandLine.scriptMode;

        // OR in a test module, during testing
        callInitDrop |= g_CommandLine.test && g_CommandLine.runByteCodeTests && module->kind == ModuleKind::Test;

        if (callInitDrop)
        {
            for (auto func : module->byteCodeInitFunc)
            {
                module->logStage(FMT("#init %s\n", func->node->sourceFile->name.c_str()));
                module->executeNode(func->node->sourceFile, func->node, baseContext);
                if (module->criticalErrors)
                    return JobResult::ReleaseJob;
            }

            module->callPreMain();

            for (auto func : module->byteCodePreMainFunc)
            {
                module->logStage(FMT("#premain %s\n", func->node->sourceFile->name.c_str()));
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
            if (!g_CommandLine.scriptMode)
                module->bssCannotChange = true;

            for (auto func : module->byteCodeRunFunc)
            {
#ifdef SWAG_STATS
                ++g_Stats.runFunctions;
#endif
                module->logStage(FMT("#run %s\n", func->node->sourceFile->name.c_str()));
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
#ifdef SWAG_STATS
                    ++g_Stats.testFunctions;
#endif
                    module->logStage(FMT("#test %s\n", func->node->sourceFile->name.c_str()));
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
        if (g_CommandLine.scriptMode)
        {
            // We should have a #main to execute
            if (!module->byteCodeMainFunc)
            {
                if (module->kind == ModuleKind::Script)
                {
                    Report::error(module, Err(Err0523));
                    return JobResult::ReleaseJob;
                }
            }
            else
            {
                module->logStage(FMT("#main %s\n", module->byteCodeMainFunc->node->sourceFile->name.c_str()));
                ExecuteNodeParams params;
                params.breakOnStart = g_CommandLine.dbgMain;
                module->executeNode(module->byteCodeMainFunc->node->sourceFile, module->byteCodeMainFunc->node, baseContext, &params);
                if (module->criticalErrors)
                    return JobResult::ReleaseJob;
            }
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // #drop functions
        if (callInitDrop)
        {
            for (auto func : module->byteCodeDropFunc)
            {
                module->logStage(FMT("#drop %s\n", func->node->sourceFile->name.c_str()));
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
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logStage("ModuleBuildPass::Output\n");

        if (module->kind == ModuleKind::Config || module->kind == ModuleKind::Script)
            pass = ModuleBuildPass::Done;
        else
        {
            // Do not run native tests or command in script mode, it's already done in bytecode
            if (g_CommandLine.scriptMode)
                pass = ModuleBuildPass::Done;
            else if (g_CommandLine.genDoc)
                pass = ModuleBuildPass::GenerateDoc;
            else
                pass = ModuleBuildPass::RunNative;

            // Generate backend
            if (module->mustOutputSomething())
            {
                module->logPass(ModuleBuildPass::Output);
                module->sendCompilerMessage(CompilerMsgKind::PassBeforeOutput, this);
                auto outputJob          = Allocator::alloc<ModuleOutputJob>();
                outputJob->module       = module;
                outputJob->dependentJob = this;
                jobsToAdd.push_back(outputJob);
                return JobResult::KeepJobAlive;
            }
        }
    }

    // Generate doc pass
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::GenerateDoc)
    {
        checkMissingErrors();
        if (module->numErrors)
            return JobResult::ReleaseJob;

        module->logStage("ModuleBuildPass::GenerateDoc\n");
        pass = ModuleBuildPass::Done;

        // Special job to generate the Swag runtime documentation only one time for all modules
        {
            static Mutex mtx;
            static bool  swagDocDone = false;
            ScopedLock   lk(mtx);
            if (!swagDocDone)
            {
                swagDocDone             = true;
                auto outputJob          = Allocator::alloc<ModuleGenDocJob>();
                outputJob->module       = g_Workspace->runtimeModule;
                outputJob->dependentJob = this;
                outputJob->docKind      = BuildCfgDocKind::Api;
                jobsToAdd.push_back(outputJob);
            }
        }

        if (module->buildCfg.genDoc.kind != BuildCfgDocKind::None)
        {
            module->logPass(ModuleBuildPass::GenerateDoc);
            auto outputJob          = Allocator::alloc<ModuleGenDocJob>();
            outputJob->module       = module;
            outputJob->dependentJob = this;
            outputJob->docKind      = module->buildCfg.genDoc.kind;
            jobsToAdd.push_back(outputJob);
        }

        if (!jobsToAdd.empty())
            return JobResult::KeepJobAlive;
    }

    // Run pass
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::RunNative)
    {
        checkMissingErrors();

        if (module->numErrors)
            return JobResult::ReleaseJob;

        module->logStage("ModuleBuildPass::RunNative\n");
        pass = ModuleBuildPass::Done;

        // Run test executable
        if (module->mustGenerateTestExe() &&
            g_CommandLine.runBackendTests)
        {
            module->logPass(ModuleBuildPass::RunNative);
            auto job                         = Allocator::alloc<ModuleRunJob>();
            job->module                      = module;
            job->buildParameters             = module->buildParameters;
            job->buildParameters.compileType = Test;
            jobsToAdd.push_back(job);
        }

        // Run command
        if (g_CommandLine.run &&
            module->kind != ModuleKind::Test &&
            module->buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Executable)
        {
            module->logPass(ModuleBuildPass::RunNative);
            auto job                         = Allocator::alloc<ModuleRunJob>();
            job->module                      = module;
            job->buildParameters             = module->buildParameters;
            job->buildParameters.compileType = Normal;
            jobsToAdd.push_back(job);
        }
    }

    module->sendCompilerMessage(CompilerMsgKind::PassAllDone, this);
    module->setHasBeenBuilt(BUILDRES_FULL);

    return JobResult::ReleaseJob;
}

void ModuleBuildJob::release()
{
    if (module->kind != ModuleKind::Config)
    {
        module->release();
    }
}
