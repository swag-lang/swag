#include "pch.h"
#include "SemanticJob.h"
#include "SymTable.h"
#include "ThreadManager.h"
#include "Ast.h"
#include "AstNode.h"
#include "SourceFile.h"

thread_local Pool<SemanticJob> g_Pool_semanticJob;

bool SemanticJob::internalError(SemanticContext* context, const char* msg, AstNode* node)
{
    if (!node)
        node = context->node;
    context->report({node, node->token, format("internal compiler error during semantic (%s)", msg)});
    return false;
}

bool SemanticJob::checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_VERIFY(typeInfo->kind == TypeInfoKind::Native, notAllowed(context, node, typeInfo));
    return true;
}

bool SemanticJob::notAllowed(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    return context->report({node, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(typeInfo), typeInfo->name.c_str())});
}

SemanticJob* SemanticJob::newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* rootNode, bool run)
{
    auto job          = g_Pool_semanticJob.alloc();
    job->sourceFile   = sourceFile;
    job->module       = sourceFile->module;
    job->dependentJob = dependentJob;
    job->nodes.push_back(rootNode);
    if (run)
        g_ThreadMgr.addJob(job);
    return job;
}

bool SemanticJob::error(SemanticContext* context, const Utf8& msg)
{
    context->report({context->node, context->node->token, msg});
    return false;
}

void SemanticJob::enterState(AstNode* node)
{
    if (node->semanticState == AstNodeResolveState::Enter)
        return;

    node->semanticState = AstNodeResolveState::Enter;
    switch (node->kind)
    {
    case AstNodeKind::IdentifierRef:
    {
        AstIdentifierRef* idRef       = static_cast<AstIdentifierRef*>(node);
        idRef->startScope             = nullptr;
        idRef->resolvedSymbolName     = nullptr;
        idRef->resolvedSymbolOverload = nullptr;
        idRef->previousResolvedNode   = nullptr;
        break;
    }
    }
}

JobResult SemanticJob::execute()
{
    scoped_lock lkExecute(executeMutex);

    if (!originalNode)
        originalNode = nodes.front();

    auto firstNode     = nodes.front();
    baseContext        = &context;
    context.baseJob    = this;
    context.job        = this;
    context.sourceFile = sourceFile;
    context.result     = ContextResult::Done;

    while (!nodes.empty())
    {
        auto node    = nodes.back();
        context.node = node;

        // Already done by the #compiler pass
        if (!(flags & JOB_COMPILER_PASS) && node->flags & AST_DONE_COMPILER_PASS)
        {
            nodes.pop_back();
            continue;
        }

        switch (node->semanticState)
        {
        case AstNodeResolveState::Enter:

            // Some nodes need to spawn a new semantic job
            //if (node != firstNode && !(node->flags & AST_NO_SEMANTIC))
            if (node != firstNode)
            {
                if (firstNode->kind == AstNodeKind::Impl ||
                    firstNode->kind == AstNodeKind::File ||
                    (firstNode->kind == AstNodeKind::CompilerIf && node->ownerScope->isGlobal()))
                {
                    switch (node->kind)
                    {
                    case AstNodeKind::FuncDecl:
                    {
                        // A sub function can be waiting for the owner function to be resolved.
                        // We inform the parent function that we have seen the sub function, and that
                        // the attributes context is now fine for it. That way, the parent function can
                        // trigger the resolve of the sub function by just removing AST_NO_SEMANTIC or by hand.
                        scoped_lock lk(node->mutex);
                        node->doneFlags |= AST_DONE_FILE_JOB_PASS;

                        if (!(node->flags & AST_NO_SEMANTIC))
                        {
                            auto job          = g_Pool_semanticJob.alloc();
                            job->sourceFile   = sourceFile;
                            job->module       = module;
                            job->dependentJob = dependentJob;
                            job->nodes.push_back(node);
                            g_ThreadMgr.addJob(job);
                        }

                        nodes.pop_back();
                        continue;
                    }

                    case AstNodeKind::VarDecl:
                    case AstNodeKind::LetDecl:
                    case AstNodeKind::ConstDecl:
                    case AstNodeKind::Alias:
                    case AstNodeKind::EnumDecl:
                    case AstNodeKind::StructDecl:
                    case AstNodeKind::InterfaceDecl:
                    case AstNodeKind::CompilerAssert:
                    case AstNodeKind::CompilerPrint:
                    case AstNodeKind::CompilerRun:
                    case AstNodeKind::CompilerBake:
                    case AstNodeKind::AttrDecl:
                    case AstNodeKind::CompilerIf:
                    case AstNodeKind::Impl:
                    {
                        if (!(node->flags & AST_NO_SEMANTIC))
                        {
                            auto job          = g_Pool_semanticJob.alloc();
                            job->sourceFile   = sourceFile;
                            job->module       = module;
                            job->dependentJob = dependentJob;
                            job->nodes.push_back(node);
                            g_ThreadMgr.addJob(job);
                        }

                        nodes.pop_back();
                        continue;
                    }
                    }
                }
            }

            node->semanticState = AstNodeResolveState::ProcessingChilds;
            context.result      = ContextResult::Done;

            if (node->semanticBeforeFct && !node->semanticBeforeFct(&context))
                return JobResult::ReleaseJob;

            if (node->parentAttributes)
                node->attributeFlags |= node->parentAttributes->attributeFlags;

            if (!node->childs.empty() && !(node->flags & AST_NO_SEMANTIC))
            {
                if (node->flags & AST_REVERSE_SEMANTIC)
                {
                    for (int i = 0; i < (int) node->childs.size(); i++)
                    {
                        auto child = node->childs[i];
                        if (child->flags & AST_NO_SEMANTIC)
                            continue;

                        // Top to bottom inheritance
                        if (node->kind == AstNodeKind::Statement)
                            child->parentAttributes = node->parentAttributes;
                        child->attributeFlags |= node->attributeFlags;

                        enterState(child);
                        nodes.push_back(child);
                    }
                }
                else
                {
                    for (int i = (int) node->childs.size() - 1; i >= 0; i--)
                    {
                        auto child = node->childs[i];

                        // If the child has the AST_NO_SEMANTIC flag, do not push it.
                        // Special case for function in the root file, because we need to deal with AST_DONE_FILE_JOB_PASS
                        if ((child->flags & AST_NO_SEMANTIC) &&
                            (originalNode->kind != AstNodeKind::File || child->kind != AstNodeKind::FuncDecl))
                            continue;

                        // Top to bottom inheritance
                        if (node->kind == AstNodeKind::Statement)
                            child->parentAttributes = node->parentAttributes;
                        child->attributeFlags |= node->attributeFlags;

                        enterState(child);
                        nodes.push_back(child);
                    }
                }
            }
            break;

        case AstNodeResolveState::ProcessingChilds:
            if (node->semanticFct && !(node->flags & AST_NO_SEMANTIC))
            {
                if (!node->semanticFct(&context))
                    return JobResult::ReleaseJob;
                if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                if (context.result == ContextResult::NewChilds)
                    continue;
            }

            node->semanticState = AstNodeResolveState::PostChilds;

        case AstNodeResolveState::PostChilds:
            if (node->semanticAfterFct && !(node->flags & AST_NO_SEMANTIC))
            {
                if (!node->semanticAfterFct(&context))
                    return JobResult::ReleaseJob;
                if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                if (context.result == ContextResult::NewChilds)
                    continue;
            }

            if (flags & JOB_COMPILER_PASS)
                node->flags |= AST_DONE_COMPILER_PASS;
            nodes.pop_back();
            break;
        }
    }

    for (auto p : pendingJobNodes)
    {
        auto job          = g_Pool_semanticJob.alloc();
        job->sourceFile   = sourceFile;
        job->module       = module;
        job->dependentJob = dependentJob;
        job->nodes.push_back(p);
        g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}
