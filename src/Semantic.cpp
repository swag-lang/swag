#include "pch.h"
#include "Semantic.h"
#include "ThreadManager.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "Timer.h"

void Semantic::release()
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
}

bool Semantic::setUnRef(AstNode* node)
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

TypeInfo* Semantic::getConcreteTypeUnRef(AstNode* node, uint32_t concreteFlags)
{
    auto typeInfo = TypeManager::concreteType(node->typeInfo, concreteFlags);
    if (!typeInfo->isPointerRef())
        return typeInfo;

    if (setUnRef(node))
    {
        typeInfo = TypeManager::concretePtrRef(typeInfo);
        typeInfo = TypeManager::concreteType(typeInfo, concreteFlags);
    }

    return typeInfo;
}

AstIdentifier* Semantic::createTmpId(SemanticContext* context, AstNode* node, const Utf8& name)
{
    auto sem = context->sem;

    if (!sem->tmpIdRef)
    {
        sem->tmpIdRef = Ast::newIdentifierRef(context->sourceFile, name, nullptr, nullptr);
        sem->tmpIdRef->childs.back()->flags |= AST_SILENT_CHECK;
        sem->tmpIdRef->flags |= AST_SILENT_CHECK;
    }

    sem->tmpIdRef->parent = node;
    auto id               = CastAst<AstIdentifier>(sem->tmpIdRef->childs.back(), AstNodeKind::Identifier);
    id->sourceFile        = context->sourceFile;
    id->token.text        = node->token.text;
    id->inheritOwners(node);
    id->inheritTokenLocation(node);
    return id;
}

bool Semantic::valueEqualsTo(const ComputedValue* value, AstNode* node)
{
    node->allocateComputedValue();
    return valueEqualsTo(value, node->computedValue, node->typeInfo, node->flags);
}

bool Semantic::valueEqualsTo(const ComputedValue* value1, const ComputedValue* value2, TypeInfo* typeInfo, uint64_t flags)
{
    if (!value1 || !value2)
        return true;

    // Types
    if (flags & AST_VALUE_IS_GEN_TYPEINFO)
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

bool Semantic::isCompilerContext(AstNode* node)
{
    if (node->flags & AST_NO_BACKEND)
        return true;
    if (node->attributeFlags & ATTRIBUTE_COMPILER)
        return true;
    if (node->ownerFct && node->ownerFct->attributeFlags & ATTRIBUTE_COMPILER)
        return true;
    return false;
}

DataSegment* Semantic::getConstantSegFromContext(AstNode* node, bool forceCompiler)
{
    auto module = node->sourceFile->module;
    if (forceCompiler || isCompilerContext(node))
        return &module->compilerSegment;
    return &module->constantSegment;
}

void Semantic::enterState(AstNode* node)
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
