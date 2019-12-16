#include "pch.h"
#include "ModuleBuildJob.h"
#include "Workspace.h"
#include "SyntaxJob.h"
#include "AstNode.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "ModuleSemanticJob.h"
#include "ModuleOutputJob.h"
#include "Diagnostic.h"
#include "ByteCodeModuleManager.h"
#include "Os.h"
#include "ByteCode.h"
#include "Stats.h"

thread_local Pool<ModuleBuildJob> g_Pool_moduleBuildJob;

JobResult ModuleBuildJob::execute()
{
    // Wait for dependencies to be build
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Dependencies)
    {
        for (auto dep : module->moduleDependencies)
        {
            shared_lock lk(g_Workspace.mutexModules);
            auto        it = g_Workspace.mapModulesNames.find(dep.first);
            SWAG_ASSERT(it != g_Workspace.mapModulesNames.end());

            if (it->second->numErrors)
                return JobResult::ReleaseJob;

            auto        depModule = it->second;
            unique_lock lk1(depModule->mutexDependency);
            if ((depModule->hasBeenBuilt & BUILDRES_EXPORT) == 0)
            {
                depModule->dependentJobs.add(this);
                return JobResult::KeepJobAlive;
            }
        }

        pass = ModuleBuildPass::IncludeSwg;
    }

    // We add the "?.swg" file corresponding to the
    // module we want to import
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::IncludeSwg)
    {
        pass = ModuleBuildPass::LoadDependencies;

        for (auto& dep : module->moduleDependencies)
        {
            auto node = dep.second.node;

            // Now the .swg export file should be in the cache
            bool generated = true;
            auto path      = g_Workspace.targetPath.string() + "\\" + node->name + ".generated.swg";
            if (!fs::exists(path))
            {
                generated = false;
                path      = g_Workspace.targetPath.string() + "\\" + node->name + ".swg";
            }

            if (!fs::exists(path))
            {
                node->sourceFile->report({node, format("cannot find module export file '%s'", path.c_str())});
                continue;
            }

            // Then do syntax on it
            auto syntaxJob          = g_Pool_syntaxJob.alloc();
            auto file               = g_Pool_sourceFile.alloc();
            syntaxJob->sourceFile   = file;
            syntaxJob->module       = module;
            syntaxJob->dependentJob = this;
            file->path              = move(path);
            file->generated         = true;
            dep.second.generated    = generated;
            module->addFile(file);
            jobsToAdd.push_back(syntaxJob);
        }

        // Sync with all jobs
        if (!jobsToAdd.empty())
            return JobResult::KeepJobAlive;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::LoadDependencies)
    {
        for (auto dep : module->moduleDependencies)
        {
            auto it = g_Workspace.mapModulesNames.find(dep.first);
            SWAG_ASSERT(it != g_Workspace.mapModulesNames.end());

            if (it->second->numErrors)
                return JobResult::ReleaseJob;

            auto        depModule = it->second;
            shared_lock lk(depModule->mutexDependency);
            if (depModule->hasBeenBuilt != BUILDRES_FULL)
            {
                depModule->dependentJobs.add(this);
                return JobResult::KeepJobAlive;
            }
        }

        pass = ModuleBuildPass::BuildBuildSwg;
        for (const auto& dep : module->moduleDependencies)
        {
            if (!g_ModuleMgr.loadModule(dep.first))
            {
                module->error(format("fail to load module '%s' => %s", dep.first.c_str(), OS::getLastErrorAsString().c_str()));
                return JobResult::ReleaseJob;
            }
        }
    }

    // Semantic pass on 'build.swg' file
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::BuildBuildSwg)
    {
        pass = ModuleBuildPass::Publish;

        auto semanticJob           = g_Pool_moduleSemanticJob.alloc();
        semanticJob->module        = module;
        semanticJob->dependentJob  = this;
        semanticJob->buildFileMode = true;
        jobsToAdd.push_back(semanticJob);
        return JobResult::KeepJobAlive;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Publish)
    {
        if (g_CommandLine.backendOutput)
            g_Workspace.publishModule(module);
        pass = ModuleBuildPass::Semantic;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Semantic)
    {
        pass = ModuleBuildPass::Run;

        auto semanticJob           = g_Pool_moduleSemanticJob.alloc();
        semanticJob->module        = module;
        semanticJob->dependentJob  = this;
        semanticJob->buildFileMode = false;
        jobsToAdd.push_back(semanticJob);
        return JobResult::KeepJobAlive;
    }

    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Run)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;

        checkPendingJobs();
        if ((g_CommandLine.test && g_CommandLine.runByteCodeTests && !module->byteCodeTestFunc.empty()) || !module->byteCodeRunFunc.empty())
        {
            // INIT
            if (!module->byteCodeInitFunc.empty())
            {
                if (!module->numErrors)
                {
                    if (g_CommandLine.verboseBuildPass)
                        g_Log.verbose(format("   module '%s', bytecode execution of %d #init function(s)", module->name.c_str(), module->byteCodeInitFunc.size()));

                    for (auto func : module->byteCodeInitFunc)
                    {
                        module->executeNode(func->node->sourceFile, func->node);
                    }
                }
            }

            // #TEST
            if (g_CommandLine.test && g_CommandLine.runByteCodeTests && !module->byteCodeTestFunc.empty())
            {
                if (!module->numErrors)
                {
                    if (g_CommandLine.verboseBuildPass)
                        g_Log.verbose(format("   module '%s', bytecode execution of %d #test function(s)", module->name.c_str(), module->byteCodeTestFunc.size()));

                    for (auto func : module->byteCodeTestFunc)
                    {
                        g_Stats.testFunctions++;
                        module->executeNode(func->node->sourceFile, func->node);
                    }
                }
            }

            // #RUN
            if (!module->byteCodeRunFunc.empty())
            {
                if (!module->numErrors)
                {
                    if (g_CommandLine.verboseBuildPass)
                        g_Log.verbose(format("   module '%s', bytecode execution of %d #run function(s)", module->name.c_str(), module->byteCodeRunFunc.size()));

                    for (auto func : module->byteCodeRunFunc)
                    {
                        g_Stats.testFunctions++;
                        module->executeNode(func->node->sourceFile, func->node);
                    }
                }
            }

            // DROP
            if (!module->byteCodeDropFunc.empty())
            {
                if (!module->numErrors)
                {
                    if (g_CommandLine.verboseBuildPass)
                        g_Log.verbose(format("   module '%s', bytecode execution of %d #drop function(s)", module->name.c_str(), module->byteCodeDropFunc.size()));

                    for (auto func : module->byteCodeDropFunc)
                    {
                        module->executeNode(func->node->sourceFile, func->node);
                    }
                }
            }
        }

        // During unit testing, be sure we don't have
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

    // Output pass on all modules
    //////////////////////////////////////////////////
    if (pass == ModuleBuildPass::Output)
    {
        pass = ModuleBuildPass::End;
        if (g_CommandLine.backendOutput || g_CommandLine.generateDoc)
        {
            if (!module->numErrors && !module->name.empty() && (module->buildPass >= BuildPass::Backend) && module->files.size())
            {
                auto outputJob          = g_Pool_moduleOutputJob.alloc();
                outputJob->module       = module;
                outputJob->dependentJob = this;
                jobsToAdd.push_back(outputJob);
                return JobResult::KeepJobAlive;
            }
        }
    }

    module->setHasBeenBuilt(BUILDRES_FULL);

    return JobResult::ReleaseJob;
}

void ModuleBuildJob::checkPendingJobs()
{
    unique_lock lk(g_ThreadMgr.mutexAdd);
    if (g_ThreadMgr.waitingJobs.empty())
        return;

    for (auto pendingJob : g_ThreadMgr.waitingJobs)
    {
        if (pendingJob->dependentJob != this)
            continue;

        auto firstNode = pendingJob->originalNode;
        if (firstNode)
        {
            if (!(firstNode->flags & AST_GENERATED))
            {
                AstNode* node = nullptr;
                if (!pendingJob->nodes.empty())
                    node = pendingJob->nodes.back();
                else if (!pendingJob->dependentNodes.empty())
                    node = pendingJob->dependentNodes.back();
                if (node)
                {
                    if (!pendingJob->sourceFile->numErrors)
                    {
                        if (pendingJob->waitingSymbolSolved && !firstNode->name.empty())
                            pendingJob->sourceFile->report({node, node->token, format("cannot resolve %s '%s' because identifier '%s' has not been solved (do you have a cycle ?)", AstNode::getNakedKindName(firstNode).c_str(), firstNode->name.c_str(), pendingJob->waitingSymbolSolved->fullName.c_str())});
                        else if (pendingJob->waitingSymbolSolved)
                            pendingJob->sourceFile->report({node, node->token, format("cannot resolve %s because identifier '%s' has not been solved (do you have a cycle ?)", AstNode::getNakedKindName(firstNode).c_str(), pendingJob->waitingSymbolSolved->fullName.c_str())});
                        else
                            pendingJob->sourceFile->report({firstNode, firstNode->token, format("cannot resolve %s '%s'", AstNode::getNakedKindName(firstNode).c_str(), firstNode->name.c_str())});
                        pendingJob->sourceFile->numErrors = 0;
                    }
                }
            }
        }
    }
}
