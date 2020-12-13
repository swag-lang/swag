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
#include "ThreadManager.h"
#include "Profile.h"
#include "ByteCodeOptimizer.h"
#include "Context.h"

thread_local Pool<ModuleBuildJob> g_Pool_moduleBuildJob;

bool ModuleBuildJob::addDependency(ModuleDependency* dep)
{
    auto depModule = dep->module;
    if (dep->importDone)
        return true;

    // Some dependencies can have been added by this stage
    if (!depModule)
    {
        depModule = g_Workspace.getModuleByName(dep->name);
        depModule->allocateBackend();
        if (!depModule)
            return module->error(format("unknown module dependency '%s'", dep->name.c_str()));
        dep->module = depModule;
    }

    dep->importDone = true;

    VectorNative<SourceFile*> files;
    SWAG_ASSERT(depModule->backend);

    auto publicPath = depModule->path + "/";
    publicPath += SWAG_PUBLIC_FOLDER;
    if (!fs::exists(publicPath))
        return true;

    // Add all public files from the dependency module
    OS::visitFiles(publicPath.c_str(), [&](const char* filename) {
        auto file  = g_Allocator.alloc<SourceFile>();
        file->name = filename;
        file->path = publicPath + "/";
        file->path += filename;
        file->imported       = depModule;
        file->forceNamespace = dep->forceNamespace;
        files.push_back(file);
    });

    // One syntax job per dependency file
    for (auto one : files)
    {
        module->addFile(one);

        auto syntaxJob          = g_Pool_syntaxJob.alloc();
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
        if (module != g_Workspace.bootstrapModule && module != g_Workspace.runtimeModule)
        {
            module->constantSegment.initFrom(&g_Workspace.runtimeModule->constantSegment);
            module->mutableSegment.initFrom(&g_Workspace.runtimeModule->mutableSegment);
            module->bssSegment.initFrom(&g_Workspace.runtimeModule->bssSegment);
            module->typeSegment.initFrom(&g_Workspace.runtimeModule->typeSegment);
        }

        pass = ModuleBuildPass::Publish;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Publish)
    {
        pass = ModuleBuildPass::Dependencies;
        if (g_CommandLine.output && !module->path.empty() && module->kind != ModuleKind::Test)
        {
            publishFilesToPublic();
            publishFilesToTarget();
            if (!jobsToAdd.empty())
                return JobResult::KeepJobAlive;
        }
    }

    // Wait for dependencies to be build
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Dependencies)
    {
        for (auto& dep : module->moduleDependencies)
        {
            auto depModule = g_Workspace.getModuleByName(dep->name);
            if (!depModule)
            {
                module->error(format("unknown module dependency '%s'", dep->name.c_str()));
                return JobResult::ReleaseJob;
            }

            dep->module = depModule;

            if (depModule->numErrors)
                return JobResult::ReleaseJob;

            unique_lock lk1(depModule->mutexDependency);
            if ((depModule->hasBeenBuilt & BUILDRES_EXPORT) == 0)
            {
                depModule->dependentJobs.add(this);
                return JobResult::KeepJobAlive;
            }
        }

        pass = ModuleBuildPass::IncludeSwg;
    }

    // We add the all export files that corresponds
    // to each module we want to import
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::IncludeSwg)
    {
        // Determine now if we need to recompile
        module->allocateBackend();
        module->backend->setMustCompile();

        // If we do not need to compile, then exit, we're done with that module
        if (!module->backend->mustCompile && !g_CommandLine.test && (!g_CommandLine.run || !g_CommandLine.script))
        {
            timerSemanticModule.start();
            pass = ModuleBuildPass::WaitForDependencies;
        }
        else
        {
            for (auto& dep : module->moduleDependencies)
            {
                if (!addDependency(dep))
                    return JobResult::ReleaseJob;
            }

            // Sync with all jobs
            if (!jobsToAdd.empty())
                return JobResult::KeepJobAlive;

            pass = ModuleBuildPass::SemanticCompiler;
        }
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::SemanticCompiler)
    {
        pass = ModuleBuildPass::SemanticModule;

        // Cannot send compiler messages while we are resolving #compiler functions
        module->runContext.canSendCompilerMessages = false;

        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerSemanticCompiler.start();
            if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassBegin, "SemanticCompiler", module->name);
        }

        if (!module->filesForCompilerPass.empty())
        {
            for (auto itfile : module->filesForCompilerPass)
            {
                auto semanticJob          = g_Pool_semanticJob.alloc();
                semanticJob->sourceFile   = itfile;
                semanticJob->module       = module;
                semanticJob->dependentJob = this;
                semanticJob->nodes.push_back(itfile->astRoot);
                semanticJob->flags |= JOB_COMPILER_PASS;
                jobsToAdd.push_back(semanticJob);
            }

            return JobResult::KeepJobAlive;
        }
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::SemanticModule)
    {
        pass = ModuleBuildPass::AfterSemantic;

        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerSemanticCompiler.stop();
            if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
            {
                g_Log.verbosePass(LogPassType::PassEnd, "SemanticCompiler", module->name, timerSemanticCompiler.elapsed);
                g_Log.verbosePass(LogPassType::PassBegin, "SemanticModule", module->name);
            }
            timerSemanticModule.start();
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // We can then send compiler messages again
        module->runContext.canSendCompilerMessages = true;
        module->sendCompilerMessage(CompilerMsgKind::PassBeforeSemantic, this);
        auto semanticJob          = g_Pool_moduleSemanticJob.alloc();
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

        pass = ModuleBuildPass::WaitForDependencies;
        module->sendCompilerMessage(CompilerMsgKind::PassAfterSemantic, this);

        // This is a dummy job, in case the user code does not trigger new jobs during the
        // CompilerMsgKind::PassAfterSemantic pass
        auto semanticJob          = g_Pool_moduleSemanticJob.alloc();
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

        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerSemanticModule.stop();
            if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassEnd, "SemanticModule", module->name, timerSemanticModule.elapsed);
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
            if (!module->WaitForDependenciesDone(this))
                return JobResult::KeepJobAlive;
        }

        pass = ModuleBuildPass::OptimizeBc;

        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassBegin, "OptimizeBc", module->name);
            timerOptimizeBc.start();
        }
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::OptimizeBc)
    {
        bool done = false;
        if (!ByteCodeOptimizer::optimize(this, module, done))
            return JobResult::ReleaseJob;
        if (!done)
            return JobResult::KeepJobAlive;

        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerOptimizeBc.stop();
            if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassEnd, "OptimizeBc", module->name, timerOptimizeBc.elapsed);
        }

        module->printBC();

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
            timerRun.start();

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

        SWAG_PROFILE(PRF_GFCT, format("run bc %s", module->name.c_str()));

        module->sendCompilerMessage(CompilerMsgKind::PassBeforeRun, this);

        // Push a copy of the default context, in case the user code changes it (or push a new one)
        PushSwagContext cxt;

        // #init functions are only executed in script mode, if the module has a #main
        bool callInitDrop = !module->byteCodeInitFunc.empty() && g_CommandLine.script && module->byteCodeMainFunc;
        // OR in a test module, during testing
        callInitDrop |= g_CommandLine.test && g_CommandLine.runByteCodeTests;
        if (callInitDrop)
        {
            if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::Info, "Exec", format("%s (%d #init)", module->name.c_str(), module->byteCodeInitFunc.size()));

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
            if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::Info, "Exec", format("%s (%d #run)", module->name.c_str(), module->byteCodeRunFunc.size()));

            // A #run pass cannot modify a bss variable
            module->bssCannotChange = true;

            for (auto func : module->byteCodeRunFunc)
            {
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
                if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
                    g_Log.verbosePass(LogPassType::Info, "Test", format("%s (%d #test)", module->name.c_str(), module->byteCodeTestFunc.size()));

                // Modified global variables during test will be restored after
                module->saveBssValues     = true;
                module->saveMutableValues = true;

                for (auto func : module->byteCodeTestFunc)
                {
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
        if (module->byteCodeMainFunc && g_CommandLine.script)
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
            if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::Info, "Exec", format("%s (%d #drop)", module->name.c_str(), module->byteCodeDropFunc.size()));

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
                if (!file->testErrors)
                    continue;
                if (g_CommandLine.testFilter.empty() || strstr(file->name, g_CommandLine.testFilter.c_str()))
                {
                    auto nb             = file->testErrors;
                    file->testErrors = 0;
                    file->report({file, format("missing unittest errors: %d (%d raised)", nb, file->numErrors)});
                }
            }
        }

        // TIming...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerRun.stop();
            if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassBegin, "Output", module->name);
            timerOutput.start();
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // Do not run native tests or command in script mode, it's already done in bytecode
        if (g_CommandLine.script)
            pass = ModuleBuildPass::Done;
        else
            pass = ModuleBuildPass::RunNative;

        if (module->mustOutputSomething())
        {
            if (g_CommandLine.output)
            {
                module->sendCompilerMessage(CompilerMsgKind::PassBeforeOutput, this);
                auto outputJob          = g_Pool_moduleOutputJob.alloc();
                outputJob->module       = module;
                outputJob->dependentJob = this;
                jobsToAdd.push_back(outputJob);
                return JobResult::KeepJobAlive;
            }
            else if (module->backend->mustCompile)
            {
                OS::touchFile(module->backend->bufferSwg.path);
            }
        }
    }

    // Run pass
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::RunNative)
    {
        pass = ModuleBuildPass::Done;

        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerOutput.stop();
            if (g_CommandLine.verbose && !module->hasTtestErrors && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassEnd, "Output", module->name, timerOutput.elapsed);
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // Run test executable
        if (module->mustGenerateTestExe() && g_CommandLine.runBackendTests)
        {
            auto job                            = g_Pool_moduleRunJob.alloc();
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
            auto job                         = g_Pool_moduleRunJob.alloc();
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
    if (module->publicSourceFiles.empty())
        return;

    string publicPath = module->path + "/";
    publicPath += SWAG_PUBLIC_FOLDER;

    if (!fs::exists(publicPath))
    {
        error_code errorCode;
        if (!fs::create_directories(publicPath.c_str(), errorCode))
        {
            module->error(format("cannot create public directory '%s'", publicPath.c_str()));
            return;
        }
    }

    // We need the public folder to be in sync with the current state of the code.
    // That means that every files in the public folder that is no more #public must
    // be removed (and every old file that does not exist anymore)
    if (module->kind != ModuleKind::Dependency)
    {
        set<Utf8> publicFiles;
        for (auto one : module->publicSourceFiles)
        {
            auto name = one->name;
            name.makeUpper();
            publicFiles.insert(name);
        }

        OS::visitFiles(publicPath.c_str(), [&](const char* filename) {
            // Keep the generated file untouched !
            module->allocateBackend();
            if (module->backend->bufferSwg.name == filename)
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
    }

    // Add all #public files
    for (auto one : module->publicSourceFiles)
    {
        auto job          = g_Pool_copyFileJob.alloc();
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
        auto job          = g_Pool_copyFileJob.alloc();
        job->module       = module;
        job->sourcePath   = publishPath + "/" + cFileName;
        job->destPath     = g_Workspace.targetPath.string() + "/" + cFileName;
        job->dependentJob = this;
        jobsToAdd.push_back(job);
    });

    // Everything in a sub folder named 'os-arch' will be copied only if this matches the current os and arch
    auto osArchPath = publishPath;
    osArchPath += "/";
    osArchPath += g_Workspace.GetOsName();
    osArchPath += "-";
    osArchPath += g_Workspace.GetArchName();
    if (fs::exists(osArchPath))
    {
        OS::visitFiles(osArchPath.c_str(), [&](const char* cFileName) {
            auto job          = g_Pool_copyFileJob.alloc();
            job->module       = module;
            job->sourcePath   = osArchPath + "/" + cFileName;
            job->destPath     = g_Workspace.targetPath.string() + "/" + cFileName;
            job->dependentJob = this;
            jobsToAdd.push_back(job);
        });
    }
}
