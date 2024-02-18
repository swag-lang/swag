#include "pch.h"
#include "SemanticJob.h"
#include "AstFlags.h"
#include "Module.h"
#include "Scope.h"
#include "ThreadManager.h"
#ifdef SWAG_STATS
#include "Timer.h"
#endif

void SemanticJob::release()
{
    context.release();
    Allocator::free<SemanticJob>(this);
}

SemanticJob* SemanticJob::newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* rootNode, bool run)
{
    const auto job    = Allocator::alloc<SemanticJob>();
    job->sourceFile   = sourceFile;
    job->module       = sourceFile->module;
    job->dependentJob = dependentJob;
    job->nodes.push_back(rootNode);
    if (run)
        g_ThreadMgr.addJob(job);
    return job;
}

bool SemanticJob::spawnJob()
{
    const auto node = context.node;

    // Some nodes need to spawn a new semantic job
    if (context.canSpawn && node != originalNode)
    {
        switch (node->kind)
        {
        case AstNodeKind::AttrUse:
            if (!node->ownerScope->isGlobalOrImpl() || node->hasSpecFlag(AstAttrUse::SPEC_FLAG_GLOBAL))
                break;
            [[fallthrough]];

        case AstNodeKind::FuncDecl:
        case AstNodeKind::StructDecl:
        case AstNodeKind::InterfaceDecl:
        case AstNodeKind::VarDecl:
        case AstNodeKind::ConstDecl:
        case AstNodeKind::TypeAlias:
        case AstNodeKind::NameAlias:
        case AstNodeKind::EnumDecl:
        case AstNodeKind::CompilerAssert:
        case AstNodeKind::CompilerPrint:
        case AstNodeKind::CompilerError:
        case AstNodeKind::CompilerWarning:
        case AstNodeKind::CompilerRun:
        case AstNodeKind::AttrDecl:
        case AstNodeKind::CompilerIf:
        case AstNodeKind::Impl:
        {
            // A sub thing can be waiting for the owner function to be resolved.
            // We inform the parent function that we have seen the sub thing, and that
            // the attributes context is now fine for it. That way, the parent function can
            // trigger the resolve of the sub things by just removing AST_NO_SEMANTIC or by hand.
            ScopedLock lk(node->mutex);

            // Flag AST_NO_SEMANTIC must be tested with the node locked, as it can be changed in
            // registerFuncSymbol by another thread
            if (!node->hasAstFlag(AST_NO_SEMANTIC) && !node->hasSemFlag(SEMFLAG_FILE_JOB_PASS))
            {
                SWAG_ASSERT(sourceFile->module == module);
                const auto job           = newJob(dependentJob, sourceFile, node, false);
                job->context.errCxtSteps = context.errCxtSteps;
                g_ThreadMgr.addJob(job);
            }

            node->addSemFlag(SEMFLAG_FILE_JOB_PASS);
            nodes.pop_back();
            return true;
        }

        default:
            break;
        }
    }

    return false;
}

JobResult SemanticJob::execute()
{
    ScopedLock lkExecute(executeMutex);
    if (sourceFile->module->numErrors)
        return JobResult::ReleaseJob;

#ifdef SWAG_STATS
    Timer timer(&g_Stats.semanticTime);
#endif

    if (!originalNode)
    {
        baseContext     = &context;
        context.baseJob = this;

        originalNode = nodes.front();
        SWAG_ASSERT(originalNode);
        Semantic::start(&context, sourceFile, originalNode);
    }

    while (!nodes.empty())
    {
        const auto node = nodes.back();
        context.node    = node;
        context.result  = ContextResult::Done;

        // To be sure that a bytecode job is not running on those nodes !
        SWAG_ASSERT(node->bytecodeState == AstNodeResolveState::Enter ||
            node->bytecodeState == AstNodeResolveState::PostChildren ||
            node->hasAstFlag(AST_COMPUTED_VALUE | AST_CONST_EXPR));

        // Some attribute flags must propagate from parent to children, whatever
        Semantic::inheritAttributesFromParent(node);

        switch (node->semanticState)
        {
        case AstNodeResolveState::Enter:
        {
            if (spawnJob())
                continue;

            if (node->hasExtSemantic() && node->extSemantic()->semanticBeforeFct)
            {
                context.result = ContextResult::Done;
                if (!node->extSemantic()->semanticBeforeFct(&context))
                    return JobResult::ReleaseJob;
                SWAG_ASSERT(context.result != ContextResult::Pending);
                SWAG_ASSERT(context.result != ContextResult::NewChildren);
            }

            if (node->hasAstFlag(AST_NO_SEMANTIC))
            {
                if (!Semantic::setState(&context, node, AstNodeResolveState::PostChildren))
                    return JobResult::ReleaseJob;
                continue;
            }

            if (!Semantic::setState(&context, node, AstNodeResolveState::ProcessingChildren))
                return JobResult::ReleaseJob;

            context.tmpNodes = node->children;
            if (node->hasAstFlag(AST_REVERSE_SEMANTIC))
                context.tmpNodes.reverse();
            for (int i = static_cast<int>(context.tmpNodes.count) - 1; i >= 0; i--)
            {
                auto child = context.tmpNodes[i];

                // If the child has the AST_NO_SEMANTIC flag, do not push it.
                // Special case for sub declarations, because we need to deal with SEMFLAG_FILE_JOB_PASS
                if (child->hasAstFlag(AST_NO_SEMANTIC) && !child->hasAstFlag(AST_SUB_DECL))
                    continue;
                if (child->hasSemFlag(SEMFLAG_ONCE) && child->semanticState != AstNodeResolveState::Enter)
                    continue;

                if (!Semantic::setState(&context, child, AstNodeResolveState::Enter))
                    return JobResult::ReleaseJob;
                nodes.push_back(child);
            }
        }
        break;

        case AstNodeResolveState::ProcessingChildren:

            if (node->hasAstFlag(AST_NO_SEMANTIC))
            {
                if (!Semantic::setState(&context, node, AstNodeResolveState::PostChildren))
                    return JobResult::ReleaseJob;
                continue;
            }

            if (node->semanticFct)
            {
                context.result = ContextResult::Done;
                if (!node->semanticFct(&context))
                    return JobResult::ReleaseJob;
            }

            if (context.result == ContextResult::Pending)
                return JobResult::KeepJobAlive;
            if (context.result == ContextResult::NewChildren)
                continue;

            if (!Semantic::setState(&context, node, AstNodeResolveState::PostChildren))
                return JobResult::ReleaseJob;
            SWAG_ASSERT(context.result != ContextResult::Pending);
            SWAG_ASSERT(context.result != ContextResult::NewChildren);
            [[fallthrough]];

        case AstNodeResolveState::PostChildren:
            if (node->hasExtSemantic() && node->extSemantic()->semanticAfterFct)
            {
                context.result = ContextResult::Done;
                if (!node->extSemantic()->semanticAfterFct(&context))
                    return JobResult::ReleaseJob;
                if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                if (context.result == ContextResult::NewChildren)
                    continue;
            }

            nodes.pop_back();
            break;

        default:
            break;
        }
    }

    return JobResult::ReleaseJob;
}
