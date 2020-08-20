#include "pch.h"
#include "ModuleBuildJob.h"
#include "Workspace.h"
#include "ModuleSemanticJob.h"
#include "ModuleOutputJob.h"
#include "ModuleLoadJob.h"
#include "ModuleManager.h"
#include "Os.h"
#include "ByteCode.h"
#include "Backend.h"
#include "CopyFileJob.h"
#include "SemanticJob.h"
#include "Module.h"
#include "ModuleRunJob.h"
#include "ThreadManager.h"

thread_local Pool<ModuleBuildJob> g_Pool_moduleBuildJob;

JobResult ModuleBuildJob::execute()
{
    // Wait for dependencies to be build
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Dependencies)
    {
        for (auto& dep : module->moduleDependencies)
        {
            auto depModule = g_Workspace.getModuleByName(dep.first);
            if (!depModule)
            {
                module->error(format("unknown module dependency '%s'", dep.first.c_str()));
                return JobResult::ReleaseJob;
            }

            dep.second.module = depModule;

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
            pass = ModuleBuildPass::RunByteCode;
        }
        else
        {
            for (auto& dep : module->moduleDependencies)
            {
                auto depModule = dep.second.module;
                if (dep.second.importDone)
                    continue;

                // Some dependencies can have been added by the this stage
                if (!depModule)
                {
                    depModule = g_Workspace.getModuleByName(dep.first);
                    depModule->allocateBackend();
                    if (!depModule)
                    {
                        module->error(format("unknown module dependency '%s'", dep.first.c_str()));
                        return JobResult::ReleaseJob;
                    }

                    dep.second.module = depModule;
                }

                auto node             = dep.second.node;
                dep.second.importDone = true;

                // Now the .swg export file should be in the cache
                if (depModule->backend && !depModule->backend->timeExportFile)
                {
                    depModule->backend->setupExportFile();
                    if (!depModule->backend->timeExportFile)
                    {
                        node->sourceFile->report({node, format("cannot find module export file for '%s'", dep.first.c_str())});
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
                dep.second.generated    = depModule->backend->exportFileGenerated;
                module->addFile(file);
                jobsToAdd.push_back(syntaxJob);

                // If the dependency is completely done, then we can load it during the syntax pass
                if (depModule->getHasBeenBuilt() == BUILDRES_FULL)
                {
                    auto loadJob          = g_Pool_moduleLoadJob.alloc();
                    loadJob->module       = depModule;
                    loadJob->dependentJob = this;
                    jobsToAdd.push_back(loadJob);
                }
            }

            // Sync with all jobs
            if (!jobsToAdd.empty())
                return JobResult::KeepJobAlive;

            pass = ModuleBuildPass::LoadDependencies;
        }
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::LoadDependencies)
    {
        for (auto& dep : module->moduleDependencies)
        {
            auto depModule = dep.second.module;
            SWAG_ASSERT(depModule);

            if (depModule->numErrors)
                return JobResult::ReleaseJob;

            unique_lock lk(depModule->mutexDependency);
            if (depModule->hasBeenBuilt != BUILDRES_FULL)
            {
                depModule->dependentJobs.add(this);
                return JobResult::KeepJobAlive;
            }
        }

        pass = ModuleBuildPass::Publish;
        for (const auto& dep : module->moduleDependencies)
        {
            if (!g_ModuleMgr.loadModule(dep.first, false, true))
            {
                module->error(format("failed to load dependency '%s' => %s", dep.first.c_str(), OS::getLastErrorAsString().c_str()));
                return JobResult::ReleaseJob;
            }
        }
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Publish)
    {
        pass = ModuleBuildPass::SemanticCompilerPass;
        if (g_CommandLine.output && !module->path.empty() && !module->fromTestsFolder)
        {
            // Everything in a /publih folder will be copied "as is" in the output directory
            string publishPath = module->path + "/publish";
            if (fs::exists(publishPath))
            {
                OS::visitFiles(publishPath.c_str(), [&](const char* cFileName) {
                    auto job          = g_Pool_copyFileJob.alloc();
                    job->module       = module;
                    job->sourcePath   = publishPath + "/" + cFileName;
                    job->destPath     = g_Workspace.targetPath.string() + "/" + cFileName;
                    job->dependentJob = this;
                    jobsToAdd.push_back(job);
                });
            }

            if (!jobsToAdd.empty())
                return JobResult::KeepJobAlive;
        }
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::SemanticCompilerPass)
    {
        module->canSendCompilerMessages = false;
        pass                            = ModuleBuildPass::SemanticModulePass;

        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerSemanticCompiler.start();
            if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                g_Log.verbose(format("## module %s semantic compiler pass begin", module->name.c_str()));
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
    if (pass == ModuleBuildPass::SemanticModulePass)
    {
        // Cannot send compiler messages while we are resolving #compiler functions
        module->canSendCompilerMessages = true;
        pass                            = ModuleBuildPass::RunByteCode;

        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerSemanticCompiler.stop();
            if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
            {
                g_Log.verbose(format(" # module %s semantic compiler pass end in %.3fs", module->name.c_str(), timerSemanticCompiler.elapsed.count()));
                g_Log.verbose(format("## module %s semantic module pass begin", module->name.c_str()));
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
    if (pass == ModuleBuildPass::RunByteCode)
    {
        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerSemanticModule.stop();
            if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                g_Log.verbose(format(" # module %s semantic module pass end in %.3fs", module->name.c_str(), timerSemanticModule.elapsed.count()));
            timerRun.start();
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        bool runByteCode = false;
        // If we have some #test functions, and we are in test mode
        if (g_CommandLine.test && g_CommandLine.runByteCodeTests && !module->byteCodeTestFunc.empty())
            runByteCode = true;
        // If we have #run functions
        else if (!module->byteCodeRunFunc.empty())
            runByteCode = true;
        // If we need to run in bytecode mode
        else if (g_CommandLine.run && g_CommandLine.script)
            runByteCode = true;

        if (runByteCode)
        {
            module->sendCompilerMessage(CompilerMsgKind::PassBeforeRun);

            // #init functions are only executed in script mode, if the module has a #main
            if (!module->byteCodeInitFunc.empty() && g_CommandLine.script && module->byteCodeMainFunc)
            {
                if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                    g_Log.verbose(format("   module %s, bytecode execution of %d #init function(s)", module->name.c_str(), module->byteCodeInitFunc.size()));

                for (auto func : module->byteCodeInitFunc)
                {
                    module->executeNode(func->node->sourceFile, func->node);
                }
            }

            if (module->numErrors)
                return JobResult::ReleaseJob;

            // #run functions are always executed
            if (!module->byteCodeRunFunc.empty())
            {
                if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                    g_Log.verbose(format("   module %s, bytecode execution of %d #run function(s)", module->name.c_str(), module->byteCodeRunFunc.size()));

                // A #run pass cannot modify a bss variable
                module->bssCannotChange = true;

                for (auto func : module->byteCodeRunFunc)
                {
                    g_Stats.runFunctions++;
                    module->executeNode(func->node->sourceFile, func->node);
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
                        g_Log.verbose(format("   module %s, bytecode execution of %d #test function(s)", module->name.c_str(), module->byteCodeTestFunc.size()));

                    // Modified global variables during test will be restored after
                    module->saveBssValues     = true;
                    module->saveMutableValues = true;

                    for (auto func : module->byteCodeTestFunc)
                    {
                        g_Stats.testFunctions++;
                        module->executeNode(func->node->sourceFile, func->node);
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
                module->executeNode(module->byteCodeMainFunc->node->sourceFile, module->byteCodeMainFunc->node);
            }

            if (module->numErrors)
                return JobResult::ReleaseJob;

            // #drop functions are only executed in script mode, if the module has a #main
            if (!module->byteCodeDropFunc.empty() && g_CommandLine.script && module->byteCodeMainFunc)
            {
                if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                    g_Log.verbose(format("   module %s, bytecode execution of %d #drop function(s)", module->name.c_str(), module->byteCodeDropFunc.size()));

                for (auto func : module->byteCodeDropFunc)
                {
                    module->executeNode(func->node->sourceFile, func->node);
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
                if (file->unittestError)
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
                g_Log.verbose(format("## module %s output pass begin", module->name.c_str()));
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
                g_Log.verbose(format(" # module %s output pass end in %.3fs", module->name.c_str(), timerOutput.elapsed.count()));
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
