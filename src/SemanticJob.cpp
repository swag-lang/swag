#include "pch.h"
#include "SemanticJob.h"
#include "SymTable.h"
#include "ThreadManager.h"
#include "Ast.h"
#include "AstNode.h"
#include "SourceFile.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Timer.h"
#include "Allocator.h"

bool SemanticJob::setUnRef(AstNode* node)
{
    if (node->kind == AstNodeKind::KeepRef)
        return false;

    if (node->kind == AstNodeKind::IdentifierRef)
        node->childs.back()->semFlags |= AST_SEM_FROM_REF;
    else
        node->semFlags |= AST_SEM_FROM_REF;

    return true;
}

AstIdentifier* SemanticJob::createTmpId(SemanticContext* context, AstNode* node, const Utf8& name)
{
    auto job = context->job;

    if (!job->tmpIdRef)
    {
        job->tmpIdRef = Ast::newIdentifierRef(context->sourceFile, name, nullptr, nullptr);
        job->tmpIdRef->childs.back()->flags |= AST_SILENT_CHECK;
        job->tmpIdRef->flags |= AST_SILENT_CHECK;
    }

    job->tmpIdRef->parent = node;
    auto id               = CastAst<AstIdentifier>(job->tmpIdRef->childs.back(), AstNodeKind::Identifier);
    id->sourceFile        = context->sourceFile;
    id->token.text        = node->token.text;
    id->inheritOwners(node);
    id->inheritTokenLocation(node);
    return id;
}

bool SemanticJob::valueEqualsTo(const ComputedValue* value, AstNode* node)
{
    node->allocateComputedValue();
    return valueEqualsTo(value, node->computedValue, node->typeInfo, node->flags);
}

bool SemanticJob::valueEqualsTo(const ComputedValue* value1, const ComputedValue* value2, TypeInfo* typeInfo, uint64_t flags)
{
    if (!value1 || !value2)
        return true;

    // Types
    if (flags & AST_VALUE_IS_TYPEINFO)
    {
        if (!value1)
            return false;
        if (value1->reg.u64 == value2->reg.u64)
            return true;

        auto typeInfo1 = (TypeInfo*) value1->reg.u64;
        auto typeInfo2 = (TypeInfo*) value2->reg.u64;
        if (!typeInfo1 || !typeInfo2)
            return false;

        if (typeInfo1->isSame(typeInfo2, ISSAME_EXACT))
            return true;
    }

    if (typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        if (!value1)
            return false;
        if (value1->storageSegment != value2->storageSegment)
            return false;
        if (value1->storageOffset == UINT32_MAX && value2->storageOffset != UINT32_MAX)
            return false;
        if (value1->storageOffset != UINT32_MAX && value2->storageOffset == UINT32_MAX)
            return false;
        if (value1->storageOffset == value2->storageOffset)
            return true;

        void* addr1 = value1->storageSegment->address(value1->storageOffset);
        void* addr2 = value2->storageSegment->address(value2->storageOffset);
        return memcmp(addr1, addr2, typeInfo->sizeOf) == 0;
    }

    if (typeInfo->isNativeIntegerOrRune())
        return value1->reg.u64 == value2->reg.u64;
    if (typeInfo->isNative(NativeTypeKind::F32))
        return value1->reg.f32 == value2->reg.f32;
    if (typeInfo->isNative(NativeTypeKind::F64))
        return value1->reg.f32 == value2->reg.f32;
    if (typeInfo->isNative(NativeTypeKind::String))
        return value1->text == value2->text;

    return *value1 == *value2;
}

bool SemanticJob::checkIsConstExpr(JobContext* context, bool test, AstNode* expression, const char* errMsg)
{
    if (test)
        return true;

    if (expression->hasSpecialFuncCall())
    {
        Diagnostic diag{expression, Fmt(Err(Err0281), expression->typeInfo->getDisplayNameC())};
        diag.hint = Fmt(Hnt(Hnt0047), expression->extension->resolvedUserOpSymbolOverload->symbol->name.c_str());
        return context->report(diag, computeNonConstExprNote(expression));
    }

    Diagnostic diag{expression, errMsg ? errMsg : Err(Err0798)};
    return context->report(diag, computeNonConstExprNote(expression));
}

bool SemanticJob::checkIsConstExpr(JobContext* context, AstNode* expression)
{
    return checkIsConstExpr(context, expression->flags & AST_CONST_EXPR, expression);
}

bool SemanticJob::checkTypeIsNative(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo)
{
    if (leftTypeInfo->kind == TypeInfoKind::Native && rightTypeInfo->kind == TypeInfoKind::Native)
        return true;
    auto node = context->node;
    return context->report(node, Fmt(Err(Err0504), node->token.ctext(), leftTypeInfo->getDisplayNameC(), rightTypeInfo->getDisplayNameC()));
}

bool SemanticJob::checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_VERIFY(typeInfo->kind == TypeInfoKind::Native, notAllowed(context, node, typeInfo));
    return true;
}

bool SemanticJob::notAllowed(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg)
{
    Utf8 text = Fmt(Err(Err0005), node->token.ctext(), TypeInfo::getNakedKindName(typeInfo), typeInfo->getDisplayNameC());
    if (msg)
    {
        text += " ";
        text += msg;
    }

    return context->report(node, text);
}

bool SemanticJob::error(SemanticContext* context, const Utf8& msg)
{
    context->report(context->node, msg);
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
    Timer timer(&g_Stats.semanticTime);

    ScopedLock lkExecute(executeMutex);
    if (sourceFile->module->numErrors)
        return JobResult::ReleaseJob;

    if (!originalNode)
    {
        originalNode = nodes.front();
        SWAG_ASSERT(originalNode);

        canSpawn = originalNode->kind == AstNodeKind::Impl ||
                   originalNode->kind == AstNodeKind::File ||
                   originalNode->kind == AstNodeKind::StatementNoScope ||
                   originalNode->kind == AstNodeKind::AttrUse ||
                   originalNode->kind == AstNodeKind::CompilerIf;

        // Sub functions attributes inheritance
        if (originalNode->kind == AstNodeKind::FuncDecl && originalNode->ownerFct)
            inheritAttributesFromOwnerFunc(originalNode);

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
    context.job        = this;
    context.sourceFile = sourceFile;
    context.result     = ContextResult::Done;

    while (!nodes.empty())
    {
        auto node = nodes.back();
        SWAG_CHECK_BLOCK(node);

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
                    ScopedLock lk(node->mutex);

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

                context.result = ContextResult::Done;
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
                context.result = ContextResult::Done;
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
