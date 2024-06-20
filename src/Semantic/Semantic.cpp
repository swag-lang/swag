#include "pch.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Wmf/Module.h"

void Semantic::start(SemanticContext* context, SourceFile* sourceFile, AstNode* originalNode)
{
    context->sourceFile = sourceFile;
    context->canSpawn   = originalNode->is(AstNodeKind::Impl) ||
                        originalNode->is(AstNodeKind::File) ||
                        originalNode->is(AstNodeKind::StatementNoScope) ||
                        originalNode->is(AstNodeKind::AttrUse) ||
                        originalNode->is(AstNodeKind::CompilerIf);

    // Sub functions attributes inheritance
    if (originalNode->is(AstNodeKind::FuncDecl) && originalNode->ownerFct)
        inheritAttributesFromOwnerFunc(originalNode);

    // In configuration pass1, we only treat the #dependencies block
    if (context->sourceFile->module->is(ModuleKind::Config) && originalNode->is(AstNodeKind::File))
    {
        for (const auto c : originalNode->children)
        {
            if (c->isNot(AstNodeKind::CompilerDependencies))
            {
                c->addAstFlag(AST_NO_SEMANTIC); // :FirstPassCfgNoSem
                c->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
            }
        }
    }
}

bool Semantic::setUnRef(AstNode* node)
{
    if (node->is(AstNodeKind::KeepRef))
        return false;
    if (node->is(AstNodeKind::Cast))
        return false;

    node->addSemFlag(SEMFLAG_FROM_REF);

    switch (node->kind)
    {
        case AstNodeKind::IdentifierRef:
        case AstNodeKind::NoDrop:
        case AstNodeKind::Move:
        case AstNodeKind::FuncCallParam:
            setUnRef(node->lastChild());
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
        context->tmpIdRef                   = Ast::newIdentifierRef(name, nullptr, nullptr);
        context->tmpIdRef->token.sourceFile = context->sourceFile;
        context->tmpIdRef->lastChild()->addAstFlag(AST_SILENT_CHECK);
        context->tmpIdRef->addAstFlag(AST_SILENT_CHECK);
    }

    context->tmpIdRef->parent = node;
    const auto id             = castAst<AstIdentifier>(context->tmpIdRef->lastChild(), AstNodeKind::Identifier);
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

bool Semantic::valueEqualsTo(const ComputedValue* value1, const ComputedValue* value2, const TypeInfo* typeInfo, const AstNodeFlags& flags)
{
    if (!value1 || !value2)
        return true;

    // Types
    if (flags.has(AST_VALUE_GEN_TYPEINFO))
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
        return memcmp(value1->getStorageAddr(), value2->getStorageAddr(), typeInfo->sizeOf) == 0;
    }

    if (typeInfo->isNativeIntegerOrRune())
        return value1->reg.u64 == value2->reg.u64;
    if (typeInfo->isNative(NativeTypeKind::F32))
        return std::bit_cast<uint32_t>(value1->reg.f32) == std::bit_cast<uint32_t>(value2->reg.f32);
    if (typeInfo->isNative(NativeTypeKind::F64))
        return std::bit_cast<uint32_t>(value1->reg.f32) == std::bit_cast<uint32_t>(value2->reg.f32);
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
            if (node->is(AstNodeKind::IdentifierRef))
            {
                const auto idRef  = castAst<AstIdentifierRef>(node);
                idRef->startScope = nullptr;
                idRef->setResolvedSymbol(nullptr, nullptr);
                idRef->previousResolvedNode = nullptr;
                break;
            }
            break;

        case AstNodeResolveState::PostChildren:
            if (node->is(AstNodeKind::FuncDecl) ||
                node->is(AstNodeKind::StructDecl) ||
                node->is(AstNodeKind::EnumDecl) ||
                node->is(AstNodeKind::TypeAlias) ||
                node->is(AstNodeKind::ConstDecl))
            {
                SWAG_CHECK(checkAccess(context, node));
            }
            break;
    }

    return true;
}
