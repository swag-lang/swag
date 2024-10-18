#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Wmf/Module.h"

bool Semantic::checkTypeIsNative(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo, const AstNode* left, const AstNode* right)
{
    if (leftTypeInfo->isNative() && rightTypeInfo->isNative())
        return true;
    const auto node = context->node;

    if (!leftTypeInfo->isNative())
    {
        Diagnostic err{node->token.sourceFile, node->token, formErr(Err0597, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
        err.addNote(left, Diagnostic::isType(leftTypeInfo));
        return context->report(err);
    }

    Diagnostic err{node->token.sourceFile, node->token, formErr(Err0598, node->token.cstr(), rightTypeInfo->getDisplayNameC())};
    err.addNote(right, Diagnostic::isType(rightTypeInfo));
    return context->report(err);
}

bool Semantic::checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_VERIFY(typeInfo->isNative(), SemanticError::notAllowedError(context, node, typeInfo));
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::sendCompilerMsgTypeDecl(SemanticContext* context)
{
    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;
    const auto node       = context->node;

    // Filter what we send
    if (module->is(ModuleKind::BootStrap) || module->is(ModuleKind::Runtime))
        return true;
    if (sourceFile->imported)
        return true;
    if (!node->ownerScope->isGlobalOrImpl())
        return true;
    if (node->hasAstFlag(AST_GENERIC | AST_FROM_GENERIC))
        return true;
    if (node->typeInfo->isTuple() || node->typeInfo->isGeneric())
        return true;

    CompilerMessage msg;
    msg.concrete.kind        = CompilerMsgKind::SemTypes;
    msg.concrete.name.buffer = node->token.text.buffer;
    msg.concrete.name.count  = node->token.text.length();
    msg.typeInfo             = node->typeInfo;
    msg.node                 = node;
    SWAG_CHECK(module->postCompilerMessage(msg));

    if (node->hasAttribute(ATTRIBUTE_GEN))
    {
        msg.concrete.kind = CompilerMsgKind::AttributeGen;
        SWAG_CHECK(module->postCompilerMessage(msg));
    }

    return true;
}

bool Semantic::checkIsConcrete(SemanticContext* context, AstNode* node)
{
    if (!node)
        return true;
    if (node->hasAstFlag(AST_R_VALUE))
        return true;
    if (node->hasFlagComputedValue() && !node->hasAstFlag(AST_NO_BYTECODE))
        return true;

    if (node->is(AstNodeKind::TypeExpression) || node->is(AstNodeKind::TypeLambda))
        return context->report({node, toErr(Err0185)});
    if (node->hasAstFlag(AST_FROM_GENERIC_REPLACE))
        return context->report({node, toErr(Err0185)});

    if (!node->resolvedSymbolName())
        return true;

    // If this is an identifier ref, then we need to check concrete from left to right,
    // to raise an error on the first problem, and not only the result
    if (node->is(AstNodeKind::IdentifierRef))
    {
        for (const auto c : node->children)
        {
            if (c == node)
                break;
            SWAG_CHECK(checkIsConcrete(context, c));
        }
    }

    // Reference to a static struct member
    const auto overload = node->resolvedSymbolOverload();
    if (overload && overload->hasFlag(OVERLOAD_VAR_STRUCT))
    {
        Diagnostic err{node, formErr(Err0484, overload->symbol->name.cstr(), overload->symbol->ownerTable->scope->name.cstr())};

        // Missing self ?
        if (node->childCount() <= 1 &&
            node->ownerStructScope &&
            node->ownerStructScope == context->node->ownerStructScope &&
            node->ownerFct)
        {
            if (node->ownerStructScope->symTable.find(node->resolvedSymbolName()->name))
            {
                const auto nodeFct = castAst<AstFuncDecl>(node->ownerFct, AstNodeKind::FuncDecl);
                const auto typeFct = castTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
                if (typeFct->parameters.empty() || !typeFct->parameters[0]->typeInfo->isSelf())
                    err.addNote(node->ownerFct, node->ownerFct->token, toNte(Nte0058));
                else if (!typeFct->parameters.empty() && typeFct->parameters[0]->typeInfo->isSelf() && !typeFct->parameters[0]->typeInfo->hasFlag(TYPEINFO_HAS_USING))
                    err.addNote(nodeFct->parameters->firstChild(), toNte(Nte0029));
                else
                    err.addNote(toNte(Nte0071));
            }
        }

        return context->report(err);
    }

    Diagnostic err{node, formErr(Err0485, Naming::kindName(node->resolvedSymbolName()->kind).cstr(), node->resolvedSymbolName()->name.cstr())};

    // struct.field
    const auto symbolName = node->resolvedSymbolName();
    if (symbolName && symbolName->is(SymbolKind::Struct))
        err.addNote(formNte(Nte0085, symbolName->name.cstr(), symbolName->name.cstr()));

    return context->report(err);
}

bool Semantic::checkIsConcreteOrType(SemanticContext* context, AstNode* node, bool typeOnly)
{
    if (node->hasAstFlag(AST_R_VALUE))
        return true;

    const auto symbolName = node->resolvedSymbolName();
    if (node->is(AstNodeKind::TypeExpression) ||
        node->is(AstNodeKind::TypeLambda) ||
        (node->is(AstNodeKind::IdentifierRef) && node->hasAstFlag(AST_FROM_GENERIC_REPLACE)) ||
        (symbolName && symbolName->is(SymbolKind::Struct)) ||
        (symbolName && symbolName->is(SymbolKind::TypeAlias)) ||
        (symbolName && symbolName->is(SymbolKind::Interface)) ||
        (symbolName && symbolName->is(SymbolKind::Attribute)) ||
        (symbolName && symbolName->is(SymbolKind::Namespace)) ||
        (symbolName && symbolName->is(SymbolKind::Enum)))
    {
        TypeInfo* result = nullptr;
        SWAG_CHECK(resolveTypeAsExpression(context, node, &result));
        YIELD();
        node->typeInfo = result;
        node->removeAstFlag(AST_NO_BYTECODE);
        return true;
    }

    if (typeOnly)
        return true;
    return checkIsConcrete(context, node);
}

bool Semantic::resolveTypeLambdaClosure(SemanticContext* context)
{
    const auto node     = castAst<AstTypeExpression>(context->node, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
    const auto typeInfo = makeType<TypeInfoFuncAttr>(TypeInfoKind::LambdaClosure);
    typeInfo->declNode  = node;

    if (node->hasSpecFlag(AstTypeExpression::SPEC_FLAG_CAN_THROW))
        typeInfo->addFlag(TYPEINFO_CAN_THROW);

    if (node->returnType)
    {
        typeInfo->returnType = node->returnType->typeInfo;
        if (typeInfo->returnType->isGeneric())
            typeInfo->addFlag(TYPEINFO_GENERIC);
    }
    else
    {
        typeInfo->returnType = g_TypeMgr->typeInfoVoid;
    }

    if (node->parameters)
    {
        size_t index = 0;
        for (auto param : node->parameters->children)
        {
            auto typeParam      = TypeManager::makeParam();
            typeParam->typeInfo = param->typeInfo;
            typeParam->declNode = param;

            const auto isNamed = param->extraPointer<AstNode>(ExtraPointerKind::IsNamed);
            if (isNamed)
                typeParam->name = isNamed->token.text;

            if (typeParam->typeInfo->isGeneric())
                typeInfo->addFlag(TYPEINFO_GENERIC);

            // Variadic must be the last one
            if (typeParam->typeInfo->isVariadic())
            {
                typeInfo->addFlag(TYPEINFO_VARIADIC);
                if (index != node->parameters->childCount() - 1)
                    return context->report({param, toErr(Err0412)});
            }
            else if (typeParam->typeInfo->isTypedVariadic())
            {
                typeInfo->addFlag(TYPEINFO_TYPED_VARIADIC);
                if (index != node->parameters->childCount() - 1)
                    return context->report({param, toErr(Err0412)});
            }
            else if (typeParam->typeInfo->isCVariadic())
            {
                typeInfo->addFlag(TYPEINFO_C_VARIADIC);
                if (index != node->parameters->childCount() - 1)
                    return context->report({param, toErr(Err0412)});
            }

            typeInfo->parameters.push_back(typeParam);
            index++;
        }

        if (node->parameters->hasAstFlag(AST_IN_TYPE_VAR_DECLARATION))
            SWAG_CHECK(setupFuncDeclParams(context, typeInfo, node, node->parameters, false));
    }

    typeInfo->computeName();

    if (node->is(AstNodeKind::TypeClosure))
    {
        typeInfo->sizeOf = SWAG_LIMIT_CLOSURE_SIZEOF;
        typeInfo->addFlag(TYPEINFO_CLOSURE);
    }
    else
    {
        typeInfo->sizeOf = sizeof(void*);
    }

    node->typeInfo = typeInfo;

    if (node->typeFlags.has(TYPE_FLAG_NULLABLE))
        node->typeInfo = g_TypeMgr->makeNullable(node->typeInfo);

    return true;
}

void Semantic::forceConstType(SemanticContext*, AstTypeExpression* node)
{
    const auto concrete = node->typeInfo->getConcreteAlias();
    if (concrete->isStruct() ||
        concrete->isArray() ||
        concrete->isClosure() ||
        concrete->isPointer() ||
        concrete->isSlice())
    {
        if (node->typeFlags.has(TYPE_FLAG_FORCE_CONST))
            node->typeFlags.add(TYPE_FLAG_IS_CONST);
        if (node->typeFlags.has(TYPE_FLAG_IS_CONST))
            node->typeInfo = g_TypeMgr->makeConst(node->typeInfo);
    }
}

bool Semantic::resolveType(SemanticContext* context)
{
    const auto typeNode = castAst<AstTypeExpression>(context->node, AstNodeKind::TypeExpression);
    SWAG_CHECK(resolveType(context, typeNode));
    YIELD();

    if (typeNode->typeFlags.has(TYPE_FLAG_NULLABLE))
    {
        const auto concrete = typeNode->typeInfo->getConcreteAlias();
        SWAG_VERIFY(concrete->couldBeNull(), context->report({typeNode, formErr(Err0219, typeNode->typeInfo->getDisplayNameC())}));
        typeNode->typeInfo = g_TypeMgr->makeNullable(typeNode->typeInfo);
    }

    return true;
}

bool Semantic::resolveType(SemanticContext* context, AstTypeExpression* typeNode)
{
    // Array with predefined dimensions, we evaluate all dimensions as const
    // Do it first because of potential pending
    if (typeNode->arrayDim && typeNode->arrayDim != UINT8_MAX)
    {
        for (int i = typeNode->arrayDim - 1; i >= 0; i--)
        {
            const auto child = typeNode->children[i];
            SWAG_CHECK(evaluateConstExpression(context, child));
            YIELD();
            if (child->hasFlagComputedValue())
                child->addAstFlag(AST_NO_BYTECODE);
        }
    }

    // Already solved
    if (typeNode->hasAstFlag(AST_FROM_GENERIC) &&
        typeNode->typeInfo &&
        !typeNode->typeInfo->isUndefined())
    {
        // Count is generic, need to reevaluate
        if (typeNode->typeInfo->hasFlag(TYPEINFO_GENERIC_COUNT))
        {
            SWAG_ASSERT(typeNode->typeInfo->isArray());
            typeNode->typeInfo = nullptr;
        }
        else
        {
            forceConstType(context, typeNode);
            return true;
        }
    }

    if (typeNode->hasSemFlag(SEMFLAG_TYPE_SOLVED) &&
        typeNode->typeInfo &&
        !typeNode->typeInfo->isUndefined())
    {
        forceConstType(context, typeNode);
        return true;
    }

    // Code
    if (typeNode->typeFlags.has(TYPE_FLAG_IS_CODE))
    {
        const auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
        SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, formErr(Err0409, "code")}));
        typeNode->typeInfo = g_TypeMgr->typeInfoCode;
        return true;
    }

    // cvarargs
    if (typeNode->typeFromLiteral && typeNode->typeFromLiteral->hasFlag(TYPEINFO_C_VARIADIC))
    {
        const auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
        SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, formErr(Err0409, "cvarargs")}));
        typeNode->typeInfo = g_TypeMgr->typeInfoCVariadic;
        return true;
    }

    if (typeNode->identifier)
    {
        typeNode->typeInfo = typeNode->identifier->typeInfo;
        typeNode->inheritAstFlagsOr(typeNode->identifier, AST_GENERIC);
    }
    else if (typeNode->typeFlags.has(TYPE_FLAG_IS_SUB_TYPE))
    {
        typeNode->typeInfo = typeNode->lastChild()->typeInfo;
    }
    else
    {
        if (!typeNode->typeFromLiteral)
            typeNode->typeFromLiteral = TypeManager::literalTypeToType(typeNode->literalType);
        typeNode->typeInfo = typeNode->typeFromLiteral;

        // Typed variadic ?
        if (typeNode->typeInfo->isTypedVariadic())
        {
            SWAG_ASSERT(!typeNode->children.empty());
            const auto typeVariadic = castTypeInfo<TypeInfoVariadic>(typeNode->typeInfo->clone());
            typeVariadic->rawType   = typeNode->firstChild()->typeInfo;
            typeVariadic->addFlag(typeVariadic->rawType->flags.mask(TYPEINFO_GENERIC));
            typeVariadic->forceComputeName();
            typeNode->typeFromLiteral = typeVariadic;
            typeNode->typeInfo        = typeVariadic;
        }
    }

    // This is a generic type, not yet known
    if (!typeNode->typeInfo && typeNode->identifier && typeNode->identifier->resolvedSymbolOverload()->hasFlag(OVERLOAD_GENERIC))
    {
        typeNode->setResolvedSymbol(typeNode->identifier->resolvedSymbolName(), typeNode->identifier->resolvedSymbolOverload());
        const auto typeGeneric = makeType<TypeInfoGeneric>();
        typeGeneric->name      = typeNode->resolvedSymbolName()->name;
        typeGeneric->declNode  = typeNode;
        typeNode->typeInfo     = typeGeneric;
    }

    // Otherwise, this is strange, we should have a type
    SWAG_VERIFY(typeNode->typeInfo, Report::internalError(context->node, "resolveType, null type !"));

    // If type comes from an identifier, be sure it's a type
    if (typeNode->identifier && !typeNode->identifier->children.empty())
    {
        const auto child = typeNode->identifier->lastChild();
        if (!child->typeInfo || !child->typeInfo->isUndefined())
        {
            const auto symName = child->resolvedSymbolName();
            if (symName)
            {
                const auto symOver = child->resolvedSymbolOverload();
                if (symName->isNot(SymbolKind::Enum) &&
                    symName->isNot(SymbolKind::TypeAlias) &&
                    symName->isNot(SymbolKind::GenericType) &&
                    symName->isNot(SymbolKind::Struct) &&
                    symName->isNot(SymbolKind::Interface))
                {
                    Diagnostic err{child->token.sourceFile, child->token, formErr(Err0263, child->token.cstr(), Naming::aKindName(symName->kind).cstr())};
                    err.addNote(Diagnostic::hereIs(symOver));
                    return context->report(err);
                }
            }
        }
    }

    // In fact, this is a pointer
    if (typeNode->typeFlags.has(TYPE_FLAG_IS_PTR))
    {
        auto ptrFlags = typeNode->typeInfo->flags.mask(TYPEINFO_GENERIC);
        if (typeNode->typeFlags.has(TYPE_FLAG_IS_SELF))
            ptrFlags.add(TYPEINFO_IS_SELF);
        if (typeNode->typeFlags.has(TYPE_FLAG_HAS_USING))
            ptrFlags.add(TYPEINFO_HAS_USING);
        if (typeNode->typeFlags.has(TYPE_FLAG_IS_CONST))
            ptrFlags.add(TYPEINFO_CONST);
        if (typeNode->typeFlags.has(TYPE_FLAG_IS_PTR_ARITHMETIC))
            ptrFlags.add(TYPEINFO_POINTER_ARITHMETIC);
        if (ptrFlags.has(TYPEINFO_GENERIC))
            typeNode->addAstFlag(AST_GENERIC);
        typeNode->typeInfo = g_TypeMgr->makePointerTo(typeNode->typeInfo, ptrFlags);
        return true;
    }

    if (!(typeNode->typeFlags.has(TYPE_FLAG_IS_REF)))
        forceConstType(context, typeNode);

    // In fact, this is a slice
    if (typeNode->typeFlags.has(TYPE_FLAG_IS_SLICE))
    {
        const auto ptrSlice   = makeType<TypeInfoSlice>();
        ptrSlice->pointedType = typeNode->typeInfo;
        if (typeNode->typeFlags.has(TYPE_FLAG_IS_CONST))
            ptrSlice->addFlag(TYPEINFO_CONST);
        ptrSlice->flags.add(ptrSlice->pointedType->flags.mask(TYPEINFO_GENERIC));
        typeNode->typeInfo = ptrSlice;
        ptrSlice->computeName();
        return true;
    }

    // In fact this is a reference
    if (typeNode->typeFlags.has(TYPE_FLAG_IS_REF))
    {
        TypeInfoFlags ptrFlags = TYPEINFO_POINTER_REF;
        if (typeNode->typeFlags.has(TYPE_FLAG_IS_CONST))
            ptrFlags.add(TYPEINFO_CONST);
        ptrFlags.add(typeNode->typeInfo->flags.mask(TYPEINFO_GENERIC));

        if (typeNode->typeFlags.has(TYPE_FLAG_IS_MOVE_REF))
        {
            const auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
            SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, toErr(Err0402)}));
            ptrFlags.add(TYPEINFO_POINTER_MOVE_REF);
        }

        const auto typeRef = g_TypeMgr->makePointerTo(typeNode->typeInfo, ptrFlags);
        typeNode->typeInfo = typeRef;
        return true;
    }

    // In fact, this is an array
    if (typeNode->typeFlags.has(TYPE_FLAG_IS_ARRAY))
    {
        SWAG_ASSERT(typeNode->arrayDim);

        // Array without a specified size
        if (typeNode->arrayDim == UINT8_MAX)
        {
            const auto ptrArray   = makeType<TypeInfoArray>();
            ptrArray->count       = UINT32_MAX;
            ptrArray->totalCount  = UINT32_MAX;
            ptrArray->pointedType = typeNode->typeInfo;
            ptrArray->finalType   = typeNode->typeInfo;
            if (typeNode->typeFlags.has(TYPE_FLAG_IS_CONST))
                ptrArray->flags.add(TYPEINFO_CONST);
            ptrArray->addFlag(ptrArray->finalType->flags.mask(TYPEINFO_GENERIC));
            ptrArray->sizeOf = 0;
            ptrArray->computeName();
            typeNode->typeInfo = ptrArray;
            return true;
        }

        const auto rawType = typeNode->typeInfo;
        SWAG_VERIFY(!rawType->isVoid(), context->report({typeNode->lastChild(), toErr(Err0566)}));

        uint32_t totalCount = 1;
        for (int i = typeNode->arrayDim - 1; i >= 0; i--)
        {
            auto child = typeNode->children[i];

            uint32_t count        = 0;
            bool     genericCount = false;

            if (child->is(AstNodeKind::IdentifierRef) &&
                !child->hasAstFlag(AST_CONST_EXPR) &&
                typeNode->ownerStructScope &&
                typeNode->ownerStructScope->owner->typeInfo->isGeneric())
            {
                genericCount = true;
            }
            else if (!child->hasFlagComputedValue() &&
                     child->resolvedSymbolOverload() &&
                     child->resolvedSymbolOverload()->hasFlag(OVERLOAD_GENERIC))
            {
                genericCount = true;
            }
            else
            {
                SWAG_CHECK(checkIsConstExpr(context, child->hasFlagComputedValue(), child, toErr(Err0038)));
                count = child->computedValue()->reg.u32;
            }

            const auto childType = TypeManager::concreteType(child->typeInfo);
            SWAG_VERIFY(childType->isNativeInteger(), context->report({child, formErr(Err0567, child->typeInfo->getDisplayNameC())}));
            SWAG_CHECK(context->checkSizeOverflow("array", static_cast<uint64_t>(count) * rawType->sizeOf, SWAG_LIMIT_ARRAY_SIZE));
            SWAG_VERIFY(!child->isConstant0(), context->report({child, toErr(Err0161)}));

            const auto ptrArray = makeType<TypeInfoArray>();
            ptrArray->count     = count;
            totalCount *= ptrArray->count;
            SWAG_CHECK(context->checkSizeOverflow("array", static_cast<uint64_t>(totalCount) * rawType->sizeOf, SWAG_LIMIT_ARRAY_SIZE));
            ptrArray->totalCount  = totalCount;
            ptrArray->pointedType = typeNode->typeInfo;
            ptrArray->finalType   = rawType;
            ptrArray->sizeOf      = ptrArray->count * ptrArray->pointedType->sizeOf;
            if (typeNode->typeFlags.has(TYPE_FLAG_IS_CONST))
                ptrArray->addFlag(TYPEINFO_CONST);
            ptrArray->addFlag(ptrArray->finalType->flags.mask(TYPEINFO_GENERIC));

            if (genericCount)
            {
                ptrArray->addFlag(TYPEINFO_GENERIC | TYPEINFO_GENERIC_COUNT);
                ptrArray->sizeNode = child;
            }

            ptrArray->computeName();
            typeNode->typeInfo = ptrArray;
        }

        return true;
    }

    // Be sure we do not have a useless user 'const'
    const auto typeC = TypeManager::concreteType(typeNode->typeInfo);
    if (typeNode->typeFlags.has(TYPE_FLAG_HAS_LOC_CONST) &&
        !typeC->isPointer() &&
        !typeC->isArray() &&
        !typeC->isStruct())
    {
        Diagnostic err{typeNode->token.sourceFile, typeNode->locConst, formErr(Err0254, typeNode->typeInfo->getDisplayNameC())};
        err.addNote(typeNode->firstChild(), Diagnostic::isType(typeNode));
        return context->report(err);
    }

    return true;
}

bool Semantic::resolveAliasAfterValue(SemanticContext* context)
{
    const auto node = context->node->parent;

    if (node->firstChild()->is(AstNodeKind::IdentifierRef))
    {
        const auto idRef    = castAst<AstIdentifierRef>(node->firstChild(), AstNodeKind::IdentifierRef);
        const auto resolved = idRef->lastChild()->resolvedSymbolName();

        if (idRef->lastChild()->is(AstNodeKind::Identifier))
        {
            if (resolved->is(SymbolKind::Function) || resolved->is(SymbolKind::Namespace))
                node->addSpecFlag(AstAlias::SPEC_FLAG_NAME_ALIAS);
        }

        if (resolved)
        {
            if (resolved->is(SymbolKind::TypeAlias))
                node->removeSpecFlag(AstAlias::SPEC_FLAG_NAME_ALIAS);
            else if (resolved->is(SymbolKind::Variable) || resolved->is(SymbolKind::NameAlias))
                node->addSpecFlag(AstAlias::SPEC_FLAG_NAME_ALIAS);
        }
    }
    else if (node->firstChild()->isNot(AstNodeKind::TypeExpression) &&
             node->firstChild()->isNot(AstNodeKind::TypeClosure) &&
             node->firstChild()->isNot(AstNodeKind::TypeLambda))
    {
        node->addSpecFlag(AstAlias::SPEC_FLAG_NAME_ALIAS);
    }

    if (node->hasSpecFlag(AstAlias::SPEC_FLAG_NAME_ALIAS))
        return true;

    // Collect all attributes for the variable
    SWAG_CHECK(collectAttributes(context, node, nullptr));

    const auto typeInfo = makeType<TypeInfoAlias>();
    typeInfo->declNode  = node;
    typeInfo->name      = node->token.text;
    typeInfo->aliasName = typeInfo->name;
    if (node->hasAttribute(ATTRIBUTE_STRICT))
        typeInfo->addFlag(TYPEINFO_STRICT);
    node->typeInfo = typeInfo;

    OverloadFlags overFlags = OVERLOAD_INCOMPLETE;
    if (node->typeInfo->isGeneric())
        overFlags.add(OVERLOAD_GENERIC);

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::TypeAlias;
    toAdd.flags    = overFlags;

    node->setResolvedSymbolOverload(node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd));
    SWAG_CHECK(node->resolvedSymbolOverload());
    if (node->hasAttribute(ATTRIBUTE_PUBLIC))
        node->ownerScope->addPublicNode(node);
    return true;
}

bool Semantic::resolveAlias(SemanticContext* context)
{
    const auto node = castAst<AstAlias>(context->node, AstNodeKind::TypeAlias);

    if (node->hasSpecFlag(AstAlias::SPEC_FLAG_NAME_ALIAS))
        return resolveNameAlias(context);

    const auto typeInfo = castTypeInfo<TypeInfoAlias>(node->typeInfo, TypeInfoKind::Alias);
    typeInfo->rawType   = node->firstChild()->typeInfo;
    typeInfo->sizeOf    = typeInfo->rawType->sizeOf;
    typeInfo->addFlag(typeInfo->rawType->flags.mask(TYPEINFO_GENERIC));
    typeInfo->addFlag(typeInfo->rawType->flags.mask(TYPEINFO_CONST));
    typeInfo->computeName();
    SWAG_ASSERT(node->resolvedSymbolOverload());
    OverloadFlags overFlags = node->resolvedSymbolOverload()->flags.maskInvert(OVERLOAD_INCOMPLETE);
    if (typeInfo->isGeneric())
        overFlags.add(OVERLOAD_GENERIC);

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::TypeAlias;
    toAdd.flags    = overFlags;
    SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd));
    return true;
}

bool Semantic::resolveExplicitBitCast(SemanticContext* context)
{
    const auto node     = castAst<AstCast>(context->node, AstNodeKind::Cast);
    const auto typeNode = node->firstChild();
    const auto exprNode = node->secondChild();

    SWAG_CHECK(checkIsConcrete(context, exprNode));

    const auto typeInfo     = TypeManager::concreteType(typeNode->typeInfo);
    const auto exprTypeInfo = TypeManager::concreteType(exprNode->typeInfo);

    if (!typeInfo->isNativeInteger() &&
        !typeInfo->isNativeFloat() &&
        !typeInfo->isRune())
    {
        const Diagnostic err{typeNode, formErr(Err0166, typeInfo->getDisplayNameC())};
        return context->report(err);
    }

    if (!exprTypeInfo->isNativeInteger() &&
        !exprTypeInfo->isNativeFloat() &&
        !exprTypeInfo->isRune() &&
        !exprTypeInfo->isPointer())
    {
        const Diagnostic err{exprNode, formErr(Err0164, exprTypeInfo->getDisplayNameC())};
        return context->report(err);
    }

    if (typeInfo->sizeOf > exprTypeInfo->sizeOf)
    {
        const Diagnostic err{exprNode, formErr(Err0165, exprTypeInfo->getDisplayNameC(), typeInfo->getDisplayNameC())};
        return context->report(err);
    }

    node->typeInfo    = typeNode->typeInfo;
    node->byteCodeFct = ByteCodeGen::emitPassThrough;
    node->inheritAstFlagsOr(exprNode, AST_CONST_EXPR | AST_R_VALUE | AST_L_VALUE | AST_SIDE_EFFECTS);
    node->inheritComputedValue(exprNode);
    node->setResolvedSymbol(exprNode->resolvedSymbolName(), exprNode->resolvedSymbolOverload());

    if (node->hasFlagComputedValue() && node->typeInfo->isNative())
    {
        switch (node->typeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                node->computedValue()->reg.s64 = node->computedValue()->reg.s8;
                break;
            case NativeTypeKind::S16:
                node->computedValue()->reg.s64 = node->computedValue()->reg.s16;
                break;
            case NativeTypeKind::S32:
                node->computedValue()->reg.s64 = node->computedValue()->reg.s32;
                break;
            case NativeTypeKind::U8:
                node->computedValue()->reg.u64 = node->computedValue()->reg.u8;
                break;
            case NativeTypeKind::U16:
                node->computedValue()->reg.u64 = node->computedValue()->reg.u16;
                break;
            case NativeTypeKind::U32:
                node->computedValue()->reg.u64 = node->computedValue()->reg.u32;
                break;
            default:
                break;
        }
    }

    return true;
}

bool Semantic::resolveExplicitCast(SemanticContext* context)
{
    const auto node     = castAst<AstCast>(context->node, AstNodeKind::Cast);
    const auto typeNode = node->firstChild();
    const auto exprNode = node->secondChild();

    SWAG_CHECK(checkIsConcrete(context, exprNode));
    exprNode->typeInfo      = getConcreteTypeUnRef(exprNode, CONCRETE_FUNC | CONCRETE_ALIAS);
    const auto exprTypeInfo = exprNode->typeInfo;

    // When we cast from a structure to an interface, we need to be sure that every interfaces are
    // registered in the structure type, otherwise the cast can fail depending on the compile order
    if (typeNode->typeInfo->isInterface())
    {
        if (exprTypeInfo->isStruct() || exprTypeInfo->isPointerTo(TypeInfoKind::Struct))
        {
            waitAllStructInterfaces(context->baseJob, exprTypeInfo);
            YIELD();
        }
    }

    // Same when casting back an interface to a pointer to struct
    if (typeNode->typeInfo->isStruct() || typeNode->typeInfo->isPointerTo(TypeInfoKind::Struct))
    {
        if (exprTypeInfo->isInterface())
        {
            waitAllStructInterfaces(context->baseJob, typeNode->typeInfo);
            YIELD();

            auto typeStruct = typeNode->typeInfo;
            if (typeNode->typeInfo->isPointer())
                typeStruct = castTypeInfo<TypeInfoPointer>(typeNode->typeInfo, TypeInfoKind::Pointer)->pointedType;
            waitTypeCompleted(context->baseJob, typeStruct);
            YIELD();

            if (typeNode->typeInfo->isStruct())
                SWAG_CHECK(exprNode->addAnyType(context, typeNode->typeInfo));
            else
            {
                const auto ptrType = castTypeInfo<TypeInfoPointer>(typeNode->typeInfo, TypeInfoKind::Pointer);
                SWAG_CHECK(exprNode->addAnyType(context, ptrType->pointedType));
            }

            YIELD();
        }
    }

    // A pattern matching will only cast to a value. So if we want to cast an interface to a struct, then
    // in fact we want to cast an interface to a pointer to a struct
    auto castTo = typeNode->typeInfo;
    if (castTo->isStruct() && node->hasSpecFlag(AstCast::SPEC_FLAG_PATTERN_MATCH) && exprTypeInfo->isInterface())
        castTo = g_TypeMgr->makePointerTo(castTo);

    CastFlags castFlags = CAST_FLAG_EXPLICIT | CAST_FLAG_ACCEPT_PENDING;
    if (node->hasSpecFlag(AstCast::SPEC_FLAG_UN_CONST))
        castFlags.add(CAST_FLAG_FORCE_UN_CONST);
    if (node->hasSpecFlag(AstCast::SPEC_FLAG_OVERFLOW))
        castFlags.add(CAST_FLAG_CAN_OVERFLOW);
    SWAG_CHECK(TypeManager::makeCompatibles(context, castTo, nullptr, exprNode, castFlags));
    YIELD();

    node->typeInfo       = castTo;
    node->toCastTypeInfo = castTo;

    node->byteCodeFct = ByteCodeGen::emitExplicitCast;
    node->inheritAstFlagsOr(exprNode, AST_CONST_EXPR | AST_VALUE_GEN_TYPEINFO | AST_R_VALUE | AST_L_VALUE | AST_SIDE_EFFECTS | AST_OP_AFFECT_CAST);
    node->inheritComputedValue(exprNode);
    node->setResolvedSymbol(exprNode->resolvedSymbolName(), exprNode->resolvedSymbolOverload());

    // In case has triggered a special function call, need to get it (usage of opAffect)
    const auto userOp = exprNode->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
    if (userOp)
        node->addExtraPointer(ExtraPointerKind::UserOp, userOp);

    // Revert the implicit cast information
    // Requested type will be stored in typeInfo of node, and previous type will be stored in typeInfo of exprNode
    // (we cannot use typeInfoCast from node, because an explicit cast result could be cast itself with an implicit cast)
    if (exprNode->typeInfoCast)
    {
        if (!node->hasAstFlag(AST_COMPUTED_VALUE | AST_OP_AFFECT_CAST))
        {
            exprNode->typeInfo     = exprNode->typeInfoCast;
            exprNode->typeInfoCast = nullptr;
        }

        // In case of a computed value, we need to remember the type we come from to make
        // last minutes cast in emitLiteral
        else
            node->typeInfoCast = exprNode->typeInfoCast;
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveExplicitAutoCast(SemanticContext* context)
{
    const auto node     = castAst<AstCast>(context->node, AstNodeKind::AutoCast);
    const auto exprNode = node->firstChild();

    exprNode->typeInfo   = getConcreteTypeUnRef(exprNode, CONCRETE_FUNC | CONCRETE_ALIAS);
    const auto cloneType = exprNode->typeInfo->clone();
    cloneType->addFlag(TYPEINFO_AUTO_CAST);
    node->typeInfo = cloneType;

    node->byteCodeFct = ByteCodeGen::emitExplicitAutoCast;
    node->inheritAstFlagsOr(exprNode, AST_CONST_EXPR | AST_VALUE_GEN_TYPEINFO | AST_SIDE_EFFECTS);
    node->inheritComputedValue(exprNode);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveTypeList(SemanticContext* context)
{
    const auto node = context->node;
    if (node->childCount() == 1)
        node->typeInfo = node->firstChild()->typeInfo;
    return true;
}

bool Semantic::resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo** resultTypeInfo, GenExportFlags genFlags)
{
    SWAG_CHECK(resolveTypeAsExpression(context, node, node->typeInfo, resultTypeInfo, genFlags));
    return true;
}

bool Semantic::resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, TypeInfo** resultTypeInfo, GenExportFlags genFlags)
{
    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;
    auto&      typeGen    = module->typeGen;

    node->allocateComputedValue();
    node->computedValue()->reg.pointer    = reinterpret_cast<uint8_t*>(typeInfo);
    node->computedValue()->storageSegment = getConstantSegFromContext(node);
    SWAG_CHECK(typeGen.genExportedTypeInfo(context, typeInfo, node->computedValue()->storageSegment, &node->computedValue()->storageOffset, genFlags.with(GEN_EXPORTED_TYPE_SHOULD_WAIT), resultTypeInfo));
    YIELD();
    node->setFlagsValueIsComputed();
    node->addAstFlag(AST_VALUE_GEN_TYPEINFO);
    return true;
}

bool Semantic::resolveCastAsIs(SemanticContext* context)
{
    const auto node     = context->node;
    const auto valNode  = node->firstChild();
    const auto typeNode = node->secondChild();

    SWAG_VERIFY(valNode->typeInfo->isInterface(), context->report({valNode, formErr(Err0261, "is", valNode->typeInfo->getDisplayNameC())}));
    SWAG_VERIFY(typeNode->typeInfo->isStruct(), context->report({typeNode, formErr(Err0260, "is", typeNode->typeInfo->getDisplayNameC())}));

    SWAG_CHECK(checkIsConcrete(context, valNode));
    SWAG_CHECK(checkIsConcreteOrType(context, typeNode, true));
    YIELD();

    return true;
}

bool Semantic::resolveCastAs(SemanticContext* context)
{
    const auto node = context->node;
    if (!node->hasSemFlag(SEMFLAG_CAST1))
    {
        TypeInfoFlags ptrFlags = 0;
        if (node->firstChild()->hasAstFlag(AST_CONST_EXPR))
            ptrFlags.add(TYPEINFO_CONST);
        node->typeInfo = g_TypeMgr->makePointerTo(node->secondChild()->typeInfo, ptrFlags);
        node->addSemFlag(SEMFLAG_CAST1);
    }

    SWAG_CHECK(resolveCastAsIs(context));
    YIELD();

    node->byteCodeFct = ByteCodeGen::emitCastAs;
    return true;
}

bool Semantic::resolveCastIs(SemanticContext* context)
{
    SWAG_CHECK(resolveCastAsIs(context));
    YIELD();

    const auto node   = context->node;
    node->byteCodeFct = ByteCodeGen::emitCastIs;
    node->typeInfo    = g_TypeMgr->typeInfoBool;
    return true;
}
