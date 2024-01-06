#include "pch.h"
#include "SemanticJob.h"
#include "ThreadManager.h"
#include "Module.h"
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
    auto job          = Allocator::alloc<SemanticJob>();
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
    auto node = context.node;

    // Some nodes need to spawn a new semantic job
    if (context.canSpawn && node != originalNode)
    {
        switch (node->kind)
        {
        case AstNodeKind::AttrUse:
            if (!node->ownerScope->isGlobalOrImpl() || ((AstAttrUse*) node)->specFlags & AstAttrUse::SPECFLAG_GLOBAL)
                break;

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
            if (!(node->flags & AST_NO_SEMANTIC) && !(node->semFlags & SEMFLAG_FILE_JOB_PASS))
            {
                SWAG_ASSERT(sourceFile->module == module);
                auto job                 = newJob(dependentJob, sourceFile, node, false);
                job->context.errCxtSteps = context.errCxtSteps;
                g_ThreadMgr.addJob(job);
            }

            node->semFlags |= SEMFLAG_FILE_JOB_PASS;
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
        originalNode = nodes.front();
        SWAG_ASSERT(originalNode);

        context.canSpawn = originalNode->kind == AstNodeKind::Impl ||
                           originalNode->kind == AstNodeKind::File ||
                           originalNode->kind == AstNodeKind::StatementNoScope ||
                           originalNode->kind == AstNodeKind::AttrUse ||
                           originalNode->kind == AstNodeKind::CompilerIf;

        // Sub functions attributes inheritance
        if (originalNode->kind == AstNodeKind::FuncDecl && originalNode->ownerFct)
            Semantic::inheritAttributesFromOwnerFunc(originalNode);

        // In configuration pass1, we only treat the #dependencies block
        if (sourceFile->module->kind == ModuleKind::Config && originalNode->kind == AstNodeKind::File)
        {
            for (auto c : originalNode->childs)
            {
                if (c->kind != AstNodeKind::CompilerDependencies)
                {
                    c->flags |= AST_NO_SEMANTIC; // :FirstPassCfgNoSem
                    c->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
                }
            }
        }
    }

    baseContext        = &context;
    context.baseJob    = this;
    context.sourceFile = sourceFile;
    context.result     = ContextResult::Done;

    while (!nodes.empty())
    {
        auto node    = nodes.back();
        context.node = node;

        // To be sure that a bytecode job is not running on those nodes !
        SWAG_ASSERT(node->bytecodeState == AstNodeResolveState::Enter ||
                    node->bytecodeState == AstNodeResolveState::PostChilds ||
                    (node->flags & (AST_VALUE_COMPUTED | AST_CONST_EXPR)));

        // Some attribute flags must propagate from parent to childs, whatever
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
                SWAG_ASSERT(context.result != ContextResult::NewChilds);
            }

            if (node->flags & AST_NO_SEMANTIC)
            {
                node->semanticState = AstNodeResolveState::PostChilds;
                continue;
            }

            node->semanticState = AstNodeResolveState::ProcessingChilds;

            int start = (int) node->childs.count - 1;
            int end   = -1;
            int inc   = -1;
            if (node->flags & AST_REVERSE_SEMANTIC)
            {
                start = 0;
                end   = (int) node->childs.count;
                inc   = 1;
            }

            for (int i = start; i != end; i += inc)
            {
                auto child = node->childs[i];

                // If the child has the AST_NO_SEMANTIC flag, do not push it.
                // Special case for sub declarations, because we need to deal with SEMFLAG_FILE_JOB_PASS
                if ((child->flags & AST_NO_SEMANTIC) && !(child->flags & AST_SUB_DECL))
                    continue;
                if ((child->semFlags & SEMFLAG_ONCE) && child->semanticState != AstNodeResolveState::Enter)
                    continue;

                Semantic::enterState(child);
                nodes.push_back(child);
            }
        }
        break;

        case AstNodeResolveState::ProcessingChilds:

            if (node->flags & AST_NO_SEMANTIC)
            {
                node->semanticState = AstNodeResolveState::PostChilds;
                continue;
            }

            if (node->semanticFct)
            {
                context.result = ContextResult::Done;
                if (!node->semanticFct(&context))
                    return JobResult::ReleaseJob;
                else if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                else if (context.result == ContextResult::NewChilds)
                    continue;
            }

            node->semanticState = AstNodeResolveState::PostChilds;

        case AstNodeResolveState::PostChilds:

            Semantic::inheritAccess(node);
            if (!Semantic::checkAccess(&context, node))
                return JobResult::ReleaseJob;

            if (node->hasExtSemantic() && node->extSemantic()->semanticAfterFct)
            {
                context.result = ContextResult::Done;
                if (!node->extSemantic()->semanticAfterFct(&context))
                    return JobResult::ReleaseJob;
                if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                if (context.result == ContextResult::NewChilds)
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
