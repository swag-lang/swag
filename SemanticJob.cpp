#include "pch.h"
#include "SemanticJob.h"
#include "SymTable.h"
#include "ThreadManager.h"
#include "Ast.h"
#include "AstNode.h"
#include "SourceFile.h"
#include "DiagnosticInfos.h"

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
    {
        SWAG_ASSERT(nodes.size() == 1);
        originalNode = nodes.front();
    }

#ifdef SWAG_HAS_ASSERT
    PushDiagnosticInfos di;
    if (g_CommandLine.debug)
    {
        g_diagnosticInfos.last().message    = "SemanticJob";
        g_diagnosticInfos.last().sourceFile = sourceFile;
    }
#endif

    auto firstNode     = nodes.front();
    baseContext        = &context;
    context.job        = this;
    context.sourceFile = sourceFile;
    context.result     = ContextResult::Done;

    while (!nodes.empty())
    {
        auto node    = nodes.back();
        context.node = node;

#ifdef SWAG_HAS_ASSERT
        if (g_CommandLine.debug)
        {
            g_diagnosticInfos.last().node = node;
        }
#endif

        switch (node->semanticState)
        {
        case AstNodeResolveState::Enter:

            // Some nodes need to spawn a new semantic job
            if (node != firstNode && !(node->flags & AST_NO_SEMANTIC))
            {
                if (firstNode->kind == AstNodeKind::Impl ||
                    firstNode->kind == AstNodeKind::File ||
                    (firstNode->kind == AstNodeKind::CompilerIf && node->ownerScope->isGlobal()))
                {
                    switch (node->kind)
                    {
                    case AstNodeKind::FuncDecl:
                    case AstNodeKind::VarDecl:
                    case AstNodeKind::LetDecl:
                    case AstNodeKind::ConstDecl:
                    case AstNodeKind::TypeAlias:
                    case AstNodeKind::EnumDecl:
                    case AstNodeKind::StructDecl:
                    case AstNodeKind::InterfaceDecl:
                    case AstNodeKind::CompilerAssert:
                    case AstNodeKind::CompilerPrint:
                    case AstNodeKind::CompilerRun:
                    case AstNodeKind::AttrDecl:
                    case AstNodeKind::CompilerIf:
                    case AstNodeKind::Impl:
                    {
                        auto job          = g_Pool_semanticJob.alloc();
                        job->sourceFile   = sourceFile;
                        job->module       = module;
                        job->dependentJob = dependentJob;
                        job->nodes.push_back(node);
                        nodes.pop_back();
                        g_ThreadMgr.addJob(job);
                        continue;
                    }
                    break;
                    }
                }
            }

            node->semanticState = AstNodeResolveState::ProcessingChilds;
            context.result      = ContextResult::Done;

            if (node->semanticBeforeFct && !node->semanticBeforeFct(&context))
                return JobResult::ReleaseJob;

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

                        enterState(child);
                        nodes.push_back(child);
                    }
                }
                else
                {
                    for (int i = (int) node->childs.size() - 1; i >= 0; i--)
                    {
                        auto child = node->childs[i];
                        if (child->flags & AST_NO_SEMANTIC)
                            continue;

                        // Top to bottom inheritance
                        if (node->kind == AstNodeKind::Statement)
                            child->parentAttributes = node->parentAttributes;

                        enterState(child);
                        nodes.push_back(child);
                    }
                }
            }
            break;

        case AstNodeResolveState::ProcessingChilds:
            if (node->semanticFct)
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
            if (node->semanticAfterFct)
            {
                if (!node->semanticAfterFct(&context))
                    return JobResult::ReleaseJob;
                if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                if (context.result == ContextResult::NewChilds)
                    continue;
            }

            nodes.pop_back();
            break;
        }
    }

    return JobResult::ReleaseJob;
}
