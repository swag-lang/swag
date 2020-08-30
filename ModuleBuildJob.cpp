#include "pch.h"
#include "ModuleBuildJob.h"
#include "Workspace.h"
#include "ModuleSemanticJob.h"
#include "ModuleOutputJob.h"
#include "ModuleManager.h"
#include "Os.h"
#include "ByteCode.h"
#include "Backend.h"
#include "IOJob.h"
#include "SemanticJob.h"
#include "Module.h"
#include "ModuleRunJob.h"
#include "ThreadManager.h"
#include "Profile.h"

thread_local Pool<ModuleBuildJob> g_Pool_moduleBuildJob;

JobResult ModuleBuildJob::execute()
{
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

    // We add the "???.swg" export file that corresponds
    // to each module we want to import
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::IncludeSwg)
    {
        // Determine now if we need to recompile
        module->allocateBackend();
        module->backend->setMustCompile();

        // If we do not need to compile, then exit, we're done with that module
        if (!module->backend->mustCompile && !g_CommandLine.generateDoc && !g_CommandLine.test && (!g_CommandLine.run || !g_CommandLine.script))
        {
            timerSemanticModule.start();
            pass = ModuleBuildPass::WaitForDependencies;
        }
        else
        {
            for (auto& dep : module->moduleDependencies)
            {
                auto depModule = dep->module;
                if (dep->importDone)
                    continue;

                // Some dependencies can have been added by the this stage
                if (!depModule)
                {
                    depModule = g_Workspace.getModuleByName(dep->name);
                    depModule->allocateBackend();
                    if (!depModule)
                    {
                        module->error(format("unknown module dependency '%s'", dep->name.c_str()));
                        return JobResult::ReleaseJob;
                    }

                    dep->module = depModule;
                }

                auto node       = dep->node;
                dep->importDone = true;

                // Now the .swg export file should be in the cache
                if (depModule->backend && !depModule->backend->timeExportFile)
                {
                    depModule->backend->setupExportFile();
                    if (!depModule->backend->timeExportFile)
                    {
                        node->sourceFile->report({node, format("cannot find module export file for '%s'", dep->name.c_str())});
                        continue;
                    }
                }

                // Then do syntax on it
                auto syntaxJob          = g_Pool_syntaxJob.alloc();
                auto file               = g_Allocator.alloc<SourceFile>();
                syntaxJob->sourceFile   = file;
                syntaxJob->module       = module;
                syntaxJob->dependentJob = this;
                file->name              = depModule->backend->bufferSwg.name;
                file->path              = depModule->backend->bufferSwg.path;
                file->generated         = true;
                dep->generated          = depModule->backend->exportFileGenerated;
                module->addFile(file);
                jobsToAdd.push_back(syntaxJob);
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
        module->canSendCompilerMessages = false;
        pass                            = ModuleBuildPass::Publish;

        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerSemanticCompiler.start();
            if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
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
                semanticJob->flags |= JOB_COMPILER_PASS;
                jobsToAdd.push_back(semanticJob);

                for (auto itfunc : itfile->compilerPassFunctions)
                    semanticJob->nodes.push_back(itfunc);
                for (auto itusing : itfile->compilerPassUsing)
                    semanticJob->nodes.push_back(itusing);
            }

            return JobResult::KeepJobAlive;
        }
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Publish)
    {
        pass = ModuleBuildPass::SemanticModule;
        if (g_CommandLine.output && !module->path.empty() && !module->fromTestsFolder)
        {
            // Everything in a /publish folder will be copied "as is" in the output directory
            string publishPath = module->path + "/publish";
            if (fs::exists(publishPath))
            {
                OS::visitFiles(publishPath.c_str(), [&](const char* cFileName) {
                    auto job          = g_Pool_ioJob.alloc();
                    job->module       = module;
                    job->sourcePath   = publishPath + "/" + cFileName;
                    job->destPath     = g_Workspace.targetPath.string() + "/" + cFileName;
                    job->dependentJob = this;
                    job->type         = IOJobType::CopyFileJob;
                    jobsToAdd.push_back(job);
                });
            }
        }
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::SemanticModule)
    {
        // Cannot send compiler messages while we are resolving #compiler functions
        module->canSendCompilerMessages = true;
        pass                            = ModuleBuildPass::WaitForDependencies;

        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerSemanticCompiler.stop();
            if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
            {
                g_Log.verbosePass(LogPassType::PassEnd, "SemanticCompiler", module->name, timerSemanticCompiler.elapsed.count());
                g_Log.verbosePass(LogPassType::PassBegin, "SemanticModule", module->name);
            }
            timerSemanticModule.start();
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        module->sendCompilerMessage(CompilerMsgKind::PassBeforeSemantic);
        auto semanticJob          = g_Pool_moduleSemanticJob.alloc();
        semanticJob->module       = module;
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
            if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassEnd, "SemanticModule", module->name, timerSemanticModule.elapsed.count());
        }

        // If we will not run some stuff, then no need to wait for dependencies, because we do not
        // have to load the dlls
        if (!module->hasBytecodeToRun())
        {
            // Timing...
            if (g_CommandLine.stats || g_CommandLine.verbose)
                timerRun.start();
            pass = ModuleBuildPass::Output;
        }
        else
        {
            pass = ModuleBuildPass::WaitForDependenciesEffective;
        }
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::WaitForDependenciesEffective)
    {
        if (!module->WaitForDependenciesDone(this))
            return JobResult::KeepJobAlive;
        pass = ModuleBuildPass::RunByteCode;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::RunByteCode)
    {
        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
            timerRun.start();

        if (module->numErrors)
            return JobResult::ReleaseJob;

        if (module->hasBytecodeToRun())
        {
            SWAG_PROFILE(PRF_GFCT, format("run bc %s%s", module->name.c_str(), module->buildParameters.postFix.c_str()));

            module->sendCompilerMessage(CompilerMsgKind::PassBeforeRun);

            // #init functions are only executed in script mode, if the module has a #main
            if (!module->byteCodeInitFunc.empty() && g_CommandLine.script && module->byteCodeMainFunc)
            {
                if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                    g_Log.verbosePass(LogPassType::Info, "Exec", format("%s (%d #init)", module->name.c_str(), module->byteCodeInitFunc.size()));

                for (auto func : module->byteCodeInitFunc)
                {
                    module->executeNode(func->node->sourceFile, func->node, baseContext);
                }
            }

            if (module->numErrors)
                return JobResult::ReleaseJob;

            // #run functions are always executed
            if (!module->byteCodeRunFunc.empty())
            {
                if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                    g_Log.verbosePass(LogPassType::Info, "Exec", format("%s (%d #run)", module->name.c_str(), module->byteCodeRunFunc.size()));

                // A #run pass cannot modify a bss variable
                module->bssCannotChange = true;

                for (auto func : module->byteCodeRunFunc)
                {
                    g_Stats.runFunctions++;
                    module->executeNode(func->node->sourceFile, func->node, baseContext);
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
                    if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                        g_Log.verbosePass(LogPassType::Info, "Test", format("%s (%d #test)", module->name.c_str(), module->byteCodeTestFunc.size()));

                    // Modified global variables during test will be restored after
                    module->saveBssValues     = true;
                    module->saveMutableValues = true;

                    for (auto func : module->byteCodeTestFunc)
                    {
                        g_Stats.testFunctions++;
                        module->executeNode(func->node->sourceFile, func->node, baseContext);
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
            }

            if (module->numErrors)
                return JobResult::ReleaseJob;

            // #drop functions are only executed in script mode, if the module has a #main
            if (!module->byteCodeDropFunc.empty() && g_CommandLine.script && module->byteCodeMainFunc)
            {
                if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                    g_Log.verbosePass(LogPassType::Info, "Exec", format("%s (%d #drop)", module->name.c_str(), module->byteCodeDropFunc.size()));

                for (auto func : module->byteCodeDropFunc)
                {
                    module->executeNode(func->node->sourceFile, func->node, baseContext);
                }
            }
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        // During unit testing, be sure we don't have untriggered errors
        if (g_CommandLine.test && g_CommandLine.runByteCodeTests)
        {
            for (auto file : module->files)
            {
                if (!file->unittestError)
                    continue;
                if (g_CommandLine.testFilter.empty() || strstr(file->name, g_CommandLine.testFilter.c_str()))
                {
                    auto nb             = file->unittestError;
                    file->unittestError = 0;
                    file->report({file, format("missing unittest errors: %d (%d raised)", nb, file->numErrors)});
                }
            }
        }

        pass = ModuleBuildPass::Output;
    }

    // Output pass
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Output)
    {
        // TIming...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerRun.stop();
            if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
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

        // Do not generate an executable that have been run in script mode
        bool mustOutput = true;
        if (module->byteCodeMainFunc && g_CommandLine.script)
            mustOutput = false;
        else if (module->name.empty()) // bootstrap
            mustOutput = false;
        else if (module->buildPass < BuildPass::Backend)
            mustOutput = false;
        else if (module->files.empty())
            mustOutput = false;
        else if (module->hasUnittestError) // module must have unittest errors, so not output
            mustOutput = false;
        else if (module->fromTestsFolder && !g_CommandLine.outputTest)
            mustOutput = false;

        if (mustOutput)
        {
            if (g_CommandLine.output || g_CommandLine.generateDoc)
            {
                module->sendCompilerMessage(CompilerMsgKind::PassBeforeOutput);
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

    if (pass == ModuleBuildPass::RunNative)
    {
        pass = ModuleBuildPass::Done;

        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerOutput.stop();
            if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassEnd, "Output", module->name, timerOutput.elapsed.count());
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
            if (!module->fromTestsFolder)
                job->buildParameters.postFix = ".test";
            g_ThreadMgr.addJob(job);
        }

        // Run command
        if (g_CommandLine.run && !module->fromTestsFolder)
        {
            auto job                         = g_Pool_moduleRunJob.alloc();
            job->module                      = module;
            job->dependentJob                = this;
            job->buildParameters             = module->buildParameters;
            job->buildParameters.compileType = BackendCompileType::Normal;
            g_ThreadMgr.addJob(job);
        }
    }

    // This will wake up dependencies
    module->constantSegment.release();
    module->constantSegmentCompiler.release();
    module->mutableSegment.release();

    module->sendCompilerMessage(CompilerMsgKind::PassAllDone);
    module->setHasBeenBuilt(BUILDRES_FULL);

    return JobResult::ReleaseJob;
}
