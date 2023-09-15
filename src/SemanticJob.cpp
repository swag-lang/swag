#include "pch.h"
#include "SemanticJob.h"
#include "ThreadManager.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "Timer.h"

void SemanticJob::release()
{
    clearTryMatch();
    clearGenericMatch();
    clearMatch();

    for (auto p : cacheFreeTryMatch)
        Allocator::free<OneTryMatch>(p);
    for (auto p : cacheFreeGenericMatches)
        Allocator::free<OneGenericMatch>(p);
    for (auto p : cacheFreeMatches)
        Allocator::free<OneMatch>(p);
    tmpConcat.release();
    if (tmpIdRef)
        tmpIdRef->release();

    Allocator::free<SemanticJob>(this);
}

bool SemanticJob::setUnRef(AstNode* node)
{
    if (node->kind == AstNodeKind::KeepRef)
        return false;
    if (node->kind == AstNodeKind::Cast)
        return false;

    node->semFlags |= SEMFLAG_FROM_REF;

    switch (node->kind)
    {
    case AstNodeKind::IdentifierRef:
    case AstNodeKind::NoDrop:
    case AstNodeKind::Move:
    case AstNodeKind::FuncCallParam:
        setUnRef(node->childs.back());
        break;
    default:
        break;
    }

    return true;
}

TypeInfo* SemanticJob::getConcreteTypeUnRef(AstNode* node, uint32_t concreteFlags)
{
    auto typeInfo = TypeManager::concreteType(node->typeInfo, concreteFlags);
    if (!typeInfo->isPointerRef())
        return typeInfo;
    if (setUnRef(node))
        return TypeManager::concretePtrRef(typeInfo);
    return typeInfo;
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
    if (flags & AST_VALUE_IS_GENTYPEINFO)
    {
        if (!value1)
            return false;
        if (value1->reg.u64 == value2->reg.u64)
            return true;

        auto typeInfo1 = (TypeInfo*) value1->reg.u64;
        auto typeInfo2 = (TypeInfo*) value2->reg.u64;
        if (!typeInfo1 || !typeInfo2)
            return false;

        if (typeInfo1->isSame(typeInfo2, CASTFLAG_EXACT))
            return true;
    }

    if (typeInfo->isListArray())
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

        void* addr1 = value1->getStorageAddr();
        void* addr2 = value2->getStorageAddr();
        return memcmp(addr1, addr2, typeInfo->sizeOf) == 0;
    }

    if (typeInfo->isNativeIntegerOrRune())
        return value1->reg.u64 == value2->reg.u64;
    if (typeInfo->isNative(NativeTypeKind::F32))
        return value1->reg.f32 == value2->reg.f32;
    if (typeInfo->isNative(NativeTypeKind::F64))
        return value1->reg.f32 == value2->reg.f32;
    if (typeInfo->isString())
        return value1->text == value2->text;

    return *value1 == *value2;
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
    default:
        break;
    }
}

JobResult SemanticJob::execute()
{
#ifdef SWAG_STATS
    Timer timer(&g_Stats.semanticTime);
#endif

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
        auto node     = nodes.back();
        context.node  = node;
        bool canDoSem = !(node->flags & AST_NO_SEMANTIC);

        // To be sure that a bytecode job is not running on those nodes !
        SWAG_ASSERT(node->bytecodeState == AstNodeResolveState::Enter ||
                    node->bytecodeState == AstNodeResolveState::PostChilds ||
                    (node->flags & (AST_VALUE_COMPUTED | AST_CONST_EXPR)));

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
                    if (!(node->flags & AST_NO_SEMANTIC) && !(node->semFlags & SEMFLAG_FILE_JOB_PASS))
                    {
                        auto job          = Allocator::alloc<SemanticJob>();
                        job->sourceFile   = sourceFile;
                        job->module       = module;
                        job->dependentJob = dependentJob;
                        job->nodes.push_back(node);
                        job->context.errCxtSteps.insert(job->context.errCxtSteps.end(), context.errCxtSteps.begin(), context.errCxtSteps.end());
                        g_ThreadMgr.addJob(job);
                    }

                    node->semFlags |= SEMFLAG_FILE_JOB_PASS;
                    nodes.pop_back();
                    continue;
                }

                case AstNodeKind::AttrUse:
                    if (!node->ownerScope->isGlobalOrImpl() || ((AstAttrUse*) node)->specFlags & AstAttrUse::SPECFLAG_GLOBAL)
                        break;
                    if (canDoSem)
                    {
                        auto job          = Allocator::alloc<SemanticJob>();
                        job->sourceFile   = sourceFile;
                        job->module       = module;
                        job->dependentJob = dependentJob;
                        job->nodes.push_back(node);
                        g_ThreadMgr.addJob(job);
                    }

                    nodes.pop_back();
                    continue;

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
                    if (canDoSem)
                    {
                        auto job          = Allocator::alloc<SemanticJob>();
                        job->sourceFile   = sourceFile;
                        job->module       = module;
                        job->dependentJob = dependentJob;
                        job->nodes.push_back(node);
                        g_ThreadMgr.addJob(job);
                    }

                    nodes.pop_back();
                    continue;
                default:
                    break;
                }
            }

            node->semanticState = AstNodeResolveState::ProcessingChilds;
            context.result      = ContextResult::Done;

            if (node->hasExtSemantic() &&
                node->extSemantic()->semanticBeforeFct &&
                !node->extSemantic()->semanticBeforeFct(&context))
            {
                return JobResult::ReleaseJob;
            }

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
                    if ((child->semFlags & SEMFLAG_ONCE) && child->semanticState != AstNodeResolveState::Enter)
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
                    // Special case for sub declarations, because we need to deal with SEMFLAG_FILE_JOB_PASS
                    if ((child->flags & AST_NO_SEMANTIC) && !(child->flags & AST_SUB_DECL))
                        continue;
                    if ((child->semFlags & SEMFLAG_ONCE) && child->semanticState != AstNodeResolveState::Enter)
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
                    return JobResult::ReleaseJob;
                else if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                else if (context.result == ContextResult::NewChilds)
                    continue;
            }

            node->semanticState = AstNodeResolveState::PostChilds;

        case AstNodeResolveState::PostChilds:

            inheritAccess(node);
            if (!checkAccess(&context, node))
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
