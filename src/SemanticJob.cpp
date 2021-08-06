#include "pch.h"
#include "SemanticJob.h"
#include "SymTable.h"
#include "ThreadManager.h"
#include "Ast.h"
#include "AstNode.h"
#include "SourceFile.h"
#include "Module.h"
#include "ErrorIds.h"

bool SemanticJob::checkTypeIsNative(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo)
{
    if (leftTypeInfo->kind == TypeInfoKind::Native && rightTypeInfo->kind == TypeInfoKind::Native)
        return true;
    auto node = context->node;
    return context->report({node, node->token, Utf8::format(Msg0504, node->token.text.c_str(), leftTypeInfo->getDisplayName().c_str(), rightTypeInfo->getDisplayName().c_str())});
}

bool SemanticJob::checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_VERIFY(typeInfo->kind == TypeInfoKind::Native, notAllowed(context, node, typeInfo));
    return true;
}

bool SemanticJob::notAllowed(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    return context->report({node, node->token, Utf8::format(Msg0005, node->token.text.c_str(), TypeInfo::getNakedKindName(typeInfo), typeInfo->getDisplayName().c_str())});
}

bool SemanticJob::error(SemanticContext* context, const Utf8& msg)
{
    context->report({context->node, context->node->token, msg});
    return false;
}

AstNode* SemanticJob::backToSemError()
{
    for (int i = (int) nodes.size() - 1; i >= 0; i--)
    {
        auto node = nodes[i];
        if (node->kind == AstNodeKind::CompilerSemError)
        {
            while (nodes.size() != i + 1)
                nodes.pop_back();
            return node;
        }
    }

    return nullptr;
}

SemanticJob* SemanticJob::newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* rootNode, bool run)
{
    auto job          = g_Allocator.alloc<SemanticJob>();
    job->sourceFile   = sourceFile;
    job->module       = sourceFile->module;
    job->dependentJob = dependentJob;
    job->nodes.push_back(rootNode);
    if (run)
        g_ThreadMgr.addJob(job);
    return job;
}

bool SemanticJob::isCompilerContext(AstNode* node)
{
    if (node->flags & AST_NO_BACKEND)
        return true;
    if (node->attributeFlags & ATTRIBUTE_COMPILER)
        return true;
    if (node->ownerFct && node->ownerFct->attributeFlags & ATTRIBUTE_COMPILER)
        return true;
    return false;
}

DataSegment* SemanticJob::getConstantSegFromContext(AstNode* node, bool forceCompiler)
{
    auto module = node->sourceFile->module;
    if (forceCompiler || isCompilerContext(node))
        return &module->compilerSegment;
    return &module->constantSegment;
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
    if (sourceFile->module->numErrors)
        return JobResult::ReleaseJob;

    if (!originalNode)
    {
        originalNode = nodes.front();

        canSpawn = originalNode->kind == AstNodeKind::Impl ||
                   originalNode->kind == AstNodeKind::File ||
                   originalNode->kind == AstNodeKind::StatementNoScope ||
                   originalNode->kind == AstNodeKind::AttrUse ||
                   originalNode->kind == AstNodeKind::CompilerIf;

        // Sub functions attributes inheritance
        if (originalNode->kind == AstNodeKind::FuncDecl && originalNode->ownerFct)
            inheritAttributesFromMainFunc(originalNode);

        // In configuration pass1, we only treat the #dependencies block
        if (sourceFile->module->kind == ModuleKind::Config && originalNode->kind == AstNodeKind::File)
        {
            for (auto c : originalNode->childs)
            {
                if (c->kind != AstNodeKind::CompilerDependencies)
                {
                    c->flags |= AST_NO_SEMANTIC;
                    c->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
                }
            }
        }
    }

    baseContext        = &context;
    context.baseJob    = this;
    context.job        = this;
    context.sourceFile = sourceFile;
    context.result     = ContextResult::Done;

    while (!nodes.empty())
    {
        auto node     = nodes.back();
        context.node  = node;
        bool canDoSem = !(node->flags & AST_NO_SEMANTIC);

        // To be sure that a bytecode job is not running on those nodes !
        SWAG_ASSERT(node->bytecodeState == AstNodeResolveState::Enter || (node->flags & (AST_VALUE_COMPUTED | AST_CONST_EXPR)));

        // Some attribute flags must propagate from parent to childs, whatever
        inheritAttributesFromParent(node);

        switch (node->semanticState)
        {
        case AstNodeResolveState::Enter:
        {
            // Some nodes need to spawn a new semantic job
            if (canSpawn && node != originalNode)
            {
                switch (node->kind)
                {
                case AstNodeKind::FuncDecl:
                case AstNodeKind::StructDecl:
                case AstNodeKind::InterfaceDecl:
                {
                    // A sub thing can be waiting for the owner function to be resolved.
                    // We inform the parent function that we have seen the sub thing, and that
                    // the attributes context is now fine for it. That way, the parent function can
                    // trigger the resolve of the sub things by just removing AST_NO_SEMANTIC or by hand.
                    scoped_lock lk(node->mutex);

                    // Do NOT use canDoSem here, because we need to test the flag with the node locked, as it can be changed
                    // in registerFuncSymbol by another thread
                    if (!(node->flags & AST_NO_SEMANTIC) && !(node->doneFlags & AST_DONE_FILE_JOB_PASS))
                    {
                        auto job          = g_Allocator.alloc<SemanticJob>();
                        job->sourceFile   = sourceFile;
                        job->module       = module;
                        job->dependentJob = dependentJob;
                        job->nodes.push_back(node);
                        g_ThreadMgr.addJob(job);
                    }

                    node->doneFlags |= AST_DONE_FILE_JOB_PASS;
                    nodes.pop_back();
                    continue;
                }

                case AstNodeKind::AttrUse:
                    if (!node->ownerScope->isGlobalOrImpl() || ((AstAttrUse*) node)->specFlags & AST_SPEC_ATTRUSE_GLOBAL)
                        break;

                case AstNodeKind::VarDecl:
                case AstNodeKind::ConstDecl:
                case AstNodeKind::Alias:
                case AstNodeKind::EnumDecl:
                case AstNodeKind::CompilerAssert:
                case AstNodeKind::CompilerSemError:
                case AstNodeKind::CompilerPrint:
                case AstNodeKind::CompilerRun:
                case AstNodeKind::AttrDecl:
                case AstNodeKind::CompilerIf:
                case AstNodeKind::Impl:
                {
                    if (canDoSem)
                    {
                        auto job          = g_Allocator.alloc<SemanticJob>();
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

            node->semanticState = AstNodeResolveState::ProcessingChilds;
            context.result      = ContextResult::Done;

            if (node->extension && node->extension->semanticBeforeFct && !node->extension->semanticBeforeFct(&context))
                return JobResult::ReleaseJob;

            if (!canDoSem)
            {
                nodes.pop_back();
                break;
            }

            auto countChilds = (int) node->childs.size();
            if (node->flags & AST_REVERSE_SEMANTIC)
            {
                for (int i = 0; i < countChilds; i++)
                {
                    auto child = node->childs[i];
                    if (child->flags & AST_NO_SEMANTIC)
                        continue;
                    if ((child->semFlags & AST_SEM_ONCE) && child->semanticState != AstNodeResolveState::Enter)
                        continue;

                    enterState(child);
                    nodes.push_back(child);
                }
            }
            else
            {
                for (int i = countChilds - 1; i >= 0; i--)
                {
                    auto child = node->childs[i];

                    // If the child has the AST_NO_SEMANTIC flag, do not push it.
                    // Special case for sub declarations, because we need to deal with AST_DONE_FILE_JOB_PASS
                    if ((child->flags & AST_NO_SEMANTIC) && !(child->flags & AST_SUB_DECL))
                        continue;
                    if ((child->semFlags & AST_SEM_ONCE) && child->semanticState != AstNodeResolveState::Enter)
                        continue;

                    enterState(child);
                    nodes.push_back(child);
                }
            }
        }
        break;

        case AstNodeResolveState::ProcessingChilds:
            if (node->semanticFct)
            {
                // Can have been changed after the state change
                // Example: FuncDeclType can change the AST_NO_SEMANTIC flag of the function itself in case
                // of Swag.compileif
                if (!canDoSem)
                {
                    nodes.pop_back();
                    break;
                }

                if (!node->semanticFct(&context))
                {
                    node = backToSemError();
                    if (!node)
                        return JobResult::ReleaseJob;
                }
                else if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                else if (context.result == ContextResult::NewChilds)
                    continue;
            }

            node->semanticState = AstNodeResolveState::PostChilds;

        case AstNodeResolveState::PostChilds:
            if (node->extension && node->extension->semanticAfterFct)
            {
                if (!node->extension->semanticAfterFct(&context))
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
