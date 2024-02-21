#include "pch.h"
#include "Semantic.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Module.h"
#include "TypeManager.h"

void Semantic::start(SemanticContext* context, SourceFile* sourceFile, AstNode* originalNode)
{
    context->sourceFile = sourceFile;
    context->canSpawn   = originalNode->kind == AstNodeKind::Impl ||
                        originalNode->kind == AstNodeKind::File ||
                        originalNode->kind == AstNodeKind::StatementNoScope ||
                        originalNode->kind == AstNodeKind::AttrUse ||
                        originalNode->kind == AstNodeKind::CompilerIf;

    // Sub functions attributes inheritance
    if (originalNode->kind == AstNodeKind::FuncDecl && originalNode->ownerFct)
        inheritAttributesFromOwnerFunc(originalNode);

    // In configuration pass1, we only treat the #dependencies block
    if (context->sourceFile->module->kind == ModuleKind::Config && originalNode->kind == AstNodeKind::File)
    {
        for (const auto c : originalNode->children)
        {
            if (c->kind != AstNodeKind::CompilerDependencies)
            {
                c->addAstFlag(AST_NO_SEMANTIC); // :FirstPassCfgNoSem
                c->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
            }
        }
    }
}

bool Semantic::setUnRef(AstNode* node)
{
    if (node->kind == AstNodeKind::KeepRef)
        return false;
    if (node->kind == AstNodeKind::Cast)
        return false;

    node->addSemFlag(SEMFLAG_FROM_REF);

    switch (node->kind)
    {
        case AstNodeKind::IdentifierRef:
        case AstNodeKind::NoDrop:
        case AstNodeKind::Move:
        case AstNodeKind::FuncCallParam:
            setUnRef(node->children.back());
            break;
        default:
            break;
    }

    return true;
}

TypeInfo* Semantic::getConcreteTypeUnRef(AstNode* node, ToConcreteFlags concreteFlags)
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
    if (!context->tmpIdRef)
    {
        context->tmpIdRef = Ast::newIdentifierRef(name, nullptr, nullptr, context->sourceFile);
        context->tmpIdRef->children.back()->addAstFlag(AST_SILENT_CHECK);
        context->tmpIdRef->addAstFlag(AST_SILENT_CHECK);
    }

    context->tmpIdRef->parent = node;
    const auto id             = castAst<AstIdentifier>(context->tmpIdRef->children.back(), AstNodeKind::Identifier);
    id->token.sourceFile      = context->sourceFile;
    id->token.text            = node->token.text;
    id->inheritOwners(node);
    id->inheritTokenLocation(node->token);
    return id;
}

bool Semantic::valueEqualsTo(const ComputedValue* value, AstNode* node)
{
    node->allocateComputedValue();
    return valueEqualsTo(value, node->computedValue(), node->typeInfo, node->flags);
}

bool Semantic::valueEqualsTo(const ComputedValue* value1, const ComputedValue* value2, const TypeInfo* typeInfo, AstNodeFlags flags)
{
    if (!value1 || !value2)
        return true;

    // Types
    if (flags.has(AST_VALUE_IS_GEN_TYPEINFO))
    {
        if (!value1)
            return false;
        if (value1->reg.u64 == value2->reg.u64)
            return true;

        const auto typeInfo1 = reinterpret_cast<TypeInfo*>(value1->reg.pointer);
        const auto typeInfo2 = reinterpret_cast<TypeInfo*>(value2->reg.pointer);
        if (!typeInfo1 || !typeInfo2)
            return false;

        if (typeInfo1->isSame(typeInfo2, CAST_FLAG_EXACT))
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

        const void* addr1 = value1->getStorageAddr();
        const void* addr2 = value2->getStorageAddr();
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

bool Semantic::isCompilerContext(const AstNode* node)
{
    if (node->hasAstFlag(AST_NO_BACKEND))
        return true;
    if (node->hasAttribute(ATTRIBUTE_COMPILER))
        return true;
    if (node->ownerFct && node->ownerFct->hasAttribute(ATTRIBUTE_COMPILER))
        return true;
    return false;
}

DataSegment* Semantic::getConstantSegFromContext(const AstNode* node, bool forceCompiler)
{
    const auto module = node->token.sourceFile->module;
    if (forceCompiler || isCompilerContext(node))
        return &module->compilerSegment;
    return &module->constantSegment;
}

bool Semantic::setState(SemanticContext* context, AstNode* node, AstNodeResolveState state)
{
    if (node->semanticState == state)
        return true;
    node->semanticState = state;

    switch (state)
    {
        case AstNodeResolveState::Enter:
            if (node->kind == AstNodeKind::IdentifierRef)
            {
                const auto idRef  = castAst<AstIdentifierRef>(node);
                idRef->startScope = nullptr;
                idRef->setResolvedSymbol(nullptr, nullptr);
                idRef->previousResolvedNode = nullptr;
                break;
            }
            break;

        case AstNodeResolveState::PostChildren:
            if (node->kind == AstNodeKind::FuncDecl ||
                node->kind == AstNodeKind::StructDecl ||
                node->kind == AstNodeKind::EnumDecl ||
                node->kind == AstNodeKind::TypeAlias ||
                node->kind == AstNodeKind::ConstDecl)
            {
                SWAG_CHECK(checkAccess(context, node));
            }
            break;

        default:
            break;
    }

    return true;
}
