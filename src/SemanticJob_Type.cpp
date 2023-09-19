#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Naming.h"

bool SemanticJob::checkTypeIsNative(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo, AstNode* left, AstNode* right)
{
    if (leftTypeInfo->isNative() && rightTypeInfo->isNative())
        return true;
    auto node = context->node;

    if (!leftTypeInfo->isNative())
    {
        Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0005), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
        diag.hint = Nte(Nte1061);
        diag.addRange(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }
    else
    {
        Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0005), node->token.ctext(), rightTypeInfo->getDisplayNameC())};
        diag.hint = Nte(Nte1061);
        diag.addRange(right, Diagnostic::isType(rightTypeInfo));
        return context->report(diag);
    }
}

bool SemanticJob::checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_VERIFY(typeInfo->isNative(), notAllowedError(context, node, typeInfo));
    return true;
}

bool SemanticJob::sendCompilerMsgTypeDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto node       = context->node;

    // Filter what we send
    if (module->kind == ModuleKind::BootStrap || module->kind == ModuleKind::Runtime)
        return true;
    if (sourceFile->imported)
        return true;
    if (!node->ownerScope->isGlobalOrImpl())
        return true;
    if (node->flags & (AST_IS_GENERIC | AST_FROM_GENERIC))
        return true;
    if (node->typeInfo->flags & (TYPEINFO_STRUCT_IS_TUPLE | TYPEINFO_GENERIC))
        return true;

    CompilerMessage msg;
    msg.concrete.kind        = CompilerMsgKind::SemTypes;
    msg.concrete.name.buffer = node->token.text.buffer;
    msg.concrete.name.count  = node->token.text.length();
    msg.typeInfo             = node->typeInfo;
    msg.node                 = node;
    SWAG_CHECK(module->postCompilerMessage(context, msg));

    if (node->attributeFlags & ATTRIBUTE_GEN)
    {
        msg.concrete.kind = CompilerMsgKind::AttributeGen;
        SWAG_CHECK(module->postCompilerMessage(context, msg));
    }

    return true;
}

bool SemanticJob::checkIsConcrete(SemanticContext* context, AstNode* node)
{
    if (!node)
        return true;
    if (node->flags & AST_R_VALUE)
        return true;
    if (node->hasComputedValue() && !(node->flags & AST_NO_BYTECODE))
        return true;

    if (node->kind == AstNodeKind::TypeExpression || node->kind == AstNodeKind::TypeLambda)
        return context->report({node, Err(Err0012)});
    if (node->flags & AST_FROM_GENERIC_REPLACE)
        return context->report({node, Err(Err0012)});

    if (!node->resolvedSymbolName)
        return true;

    // If this is an identifierref, then we need to be check concrete from left to right,
    // to raise an error on the first problem, and not only the result
    if (node->kind == AstNodeKind::IdentifierRef)
    {
        for (auto c : node->childs)
        {
            if (c == node)
                break;
            SWAG_CHECK(checkIsConcrete(context, c));
        }
    }

    // Reference to a static struct member
    if (node->resolvedSymbolOverload && node->resolvedSymbolOverload->flags & OVERLOAD_VAR_STRUCT)
    {
        Diagnostic  diag{node, Fmt(Err(Err0003), node->resolvedSymbolOverload->symbol->ownerTable->scope->name.c_str())};
        Diagnostic* note = nullptr;

        // Missing self ?
        if (node->childs.size() <= 1 &&
            node->ownerStructScope &&
            node->ownerStructScope == context->node->ownerStructScope &&
            node->ownerFct)
        {
            if (node->ownerStructScope->symTable.find(node->resolvedSymbolName->name))
            {
                auto nodeFct = CastAst<AstFuncDecl>(node->ownerFct, AstNodeKind::FuncDecl);
                auto typeFct = CastTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
                if (typeFct->parameters.size() == 0 || !(typeFct->parameters[0]->typeInfo->isSelf()))
                    note = Diagnostic::note(node->ownerFct, node->ownerFct->token, Nte(Nte0129));
                else if (typeFct->parameters.size() && typeFct->parameters[0]->typeInfo->isSelf() && !(typeFct->parameters[0]->typeInfo->flags & TYPEINFO_HAS_USING))
                    note = Diagnostic::note(nodeFct->parameters->childs.front(), Nte(Nte0128));
                else
                    note = Diagnostic::note(Nte(Nte0102));
            }
        }

        return context->report(diag, note);
    }

    Diagnostic  diag{node, Fmt(Err(Err0013), Naming::kindName(node->resolvedSymbolName->kind).c_str(), node->resolvedSymbolName->name.c_str())};
    Diagnostic* note = nullptr;

    // struct.field
    if (node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Struct)
        note = Diagnostic::note(Fmt(Nte(Nte1013), node->resolvedSymbolName->name.c_str(), node->resolvedSymbolName->name.c_str()));

    return context->report(diag, note);
}

bool SemanticJob::checkIsConcreteOrType(SemanticContext* context, AstNode* node, bool typeOnly)
{
    if (node->flags & AST_R_VALUE)
        return true;

    if (node->kind == AstNodeKind::TypeExpression ||
        node->kind == AstNodeKind::TypeLambda ||
        (node->kind == AstNodeKind::IdentifierRef && (node->flags & AST_FROM_GENERIC_REPLACE)) ||
        (node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Struct) ||
        (node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::TypeAlias) ||
        (node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Interface) ||
        (node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Attribute) ||
        (node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Namespace) ||
        (node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Enum))
    {
        TypeInfo* result = nullptr;
        SWAG_CHECK(resolveTypeAsExpression(context, node, &result));
        if (context->result != ContextResult::Done)
            return true;
        node->typeInfo = result;
        node->flags &= ~AST_NO_BYTECODE;
        return true;
    }

    if (typeOnly)
        return true;
    return checkIsConcrete(context, node);
}

bool SemanticJob::resolveTypeLambdaClosure(SemanticContext* context)
{
    auto node          = CastAst<AstTypeLambda>(context->node, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
    auto typeInfo      = makeType<TypeInfoFuncAttr>(TypeInfoKind::LambdaClosure);
    typeInfo->declNode = node;

    if (node->specFlags & AstTypeLambda::SPECFLAG_CAN_THROW)
        typeInfo->flags |= TYPEINFO_CAN_THROW;

    if (node->returnType)
    {
        typeInfo->returnType = node->returnType->typeInfo;
        if (typeInfo->returnType->isGeneric())
            typeInfo->flags |= TYPEINFO_GENERIC;
    }
    else
    {
        typeInfo->returnType = g_TypeMgr->typeInfoVoid;
    }

    if (node->parameters)
    {
        size_t index = 0;
        for (auto param : node->parameters->childs)
        {
            auto typeParam      = g_TypeMgr->makeParam();
            typeParam->typeInfo = param->typeInfo;
            typeParam->declNode = param;

            if (param->hasExtMisc() && param->extMisc()->isNamed)
                typeParam->name = param->extMisc()->isNamed->token.text;

            if (typeParam->typeInfo->isGeneric())
                typeInfo->flags |= TYPEINFO_GENERIC;

            // Variadic must be the last one
            if (typeParam->typeInfo->isVariadic())
            {
                typeInfo->flags |= TYPEINFO_VARIADIC;
                if (index != node->parameters->childs.size() - 1)
                    return context->report({param, Err(Err0734)});
            }
            else if (typeParam->typeInfo->isTypedVariadic())
            {
                typeInfo->flags |= TYPEINFO_TYPED_VARIADIC;
                if (index != node->parameters->childs.size() - 1)
                    return context->report({param, Err(Err0734)});
            }
            else if (typeParam->typeInfo->isCVariadic())
            {
                typeInfo->flags |= TYPEINFO_C_VARIADIC;
                if (index != node->parameters->childs.size() - 1)
                    return context->report({param, Err(Err0734)});
            }

            typeInfo->parameters.push_back(typeParam);
            index++;
        }

        if (node->parameters->flags & AST_IN_TYPE_VAR_DECLARATION)
            SWAG_CHECK(setupFuncDeclParams(context, typeInfo, node, node->parameters, false));
    }

    typeInfo->computeName();

    if (node->kind == AstNodeKind::TypeClosure)
    {
        typeInfo->sizeOf = SWAG_LIMIT_CLOSURE_SIZEOF;
        typeInfo->flags |= TYPEINFO_CLOSURE;
    }
    else
    {
        typeInfo->sizeOf = sizeof(void*);
    }

    node->typeInfo = typeInfo;

    return true;
}

void SemanticJob::forceConstType(SemanticContext* context, AstTypeExpression* node)
{
    auto concrete = TypeManager::concreteType(node->typeInfo, CONCRETE_FORCEALIAS);
    if (concrete->isStruct() ||
        concrete->isArray() ||
        concrete->isClosure() ||
        concrete->isPointer() ||
        concrete->isSlice())
    {
        if (node->typeFlags & TYPEFLAG_FORCE_CONST)
            node->typeFlags |= TYPEFLAG_IS_CONST;
        if (node->typeFlags & TYPEFLAG_IS_CONST)
            node->typeInfo = g_TypeMgr->makeConst(node->typeInfo);
    }
}

bool SemanticJob::resolveType(SemanticContext* context)
{
    auto typeNode = CastAst<AstTypeExpression>(context->node, AstNodeKind::TypeExpression);

    // Array with predefined dimensions, we evaluate all dimensions as const
    // Do it first because of potential pending
    if (typeNode->arrayDim && typeNode->arrayDim != UINT8_MAX)
    {
        for (int i = typeNode->arrayDim - 1; i >= 0; i--)
        {
            auto child = typeNode->childs[i];
            SWAG_CHECK(evaluateConstExpression(context, child));
            if (context->result == ContextResult::Pending)
                return true;
            if (child->hasComputedValue())
                child->flags |= AST_NO_BYTECODE;
        }
    }

    // Already solved
    if ((typeNode->flags & AST_FROM_GENERIC) && typeNode->typeInfo && !typeNode->typeInfo->isNative(NativeTypeKind::Undefined))
    {
        // Count is generic, need to reevaluate
        if (typeNode->typeInfo->flags & TYPEINFO_GENERIC_COUNT)
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

    if ((typeNode->semFlags & SEMFLAG_TYPE_SOLVED) && typeNode->typeInfo && !typeNode->typeInfo->isNative(NativeTypeKind::Undefined))
    {
        forceConstType(context, typeNode);
        return true;
    }

    // Code
    if (typeNode->typeFlags & TYPEFLAG_IS_CODE)
    {
        auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
        SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, Fmt(Err(Err0696), "code")}));
        typeNode->typeInfo = g_TypeMgr->typeInfoCode;
        return true;
    }

    // cvarargs
    if (typeNode->typeFromLiteral && typeNode->typeFromLiteral->flags & TYPEINFO_C_VARIADIC)
    {
        auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
        SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, Fmt(Err(Err0696), "cvarargs")}));
        typeNode->typeInfo = g_TypeMgr->typeInfoCVariadic;
        return true;
    }

    if (typeNode->identifier)
    {
        typeNode->typeInfo = typeNode->identifier->typeInfo;
        typeNode->inheritOrFlag(typeNode->identifier, AST_IS_GENERIC);
    }
    else if (typeNode->typeFlags & TYPEFLAG_IS_SUB_TYPE)
    {
        typeNode->typeInfo = typeNode->childs.back()->typeInfo;
    }
    else
    {
        if (!typeNode->typeFromLiteral)
            typeNode->typeFromLiteral = TypeManager::literalTypeToType(typeNode->literalType);
        typeNode->typeInfo = typeNode->typeFromLiteral;

        // Typed variadic ?
        if (typeNode->typeInfo->isVariadic() && !typeNode->childs.empty())
        {
            auto typeVariadic     = (TypeInfoVariadic*) typeNode->typeInfo->clone();
            typeVariadic->kind    = TypeInfoKind::TypedVariadic;
            typeVariadic->rawType = typeNode->childs.front()->typeInfo;
            typeVariadic->flags |= (typeVariadic->rawType->flags & TYPEINFO_GENERIC);
            typeVariadic->forceComputeName();
            typeNode->typeFromLiteral = typeVariadic;
            typeNode->typeInfo        = typeVariadic;
        }
    }

    // This is a generic type, not yet known
    if (!typeNode->typeInfo && typeNode->identifier && typeNode->identifier->resolvedSymbolOverload->flags & OVERLOAD_GENERIC)
    {
        typeNode->resolvedSymbolName     = typeNode->identifier->resolvedSymbolName;
        typeNode->resolvedSymbolOverload = typeNode->identifier->resolvedSymbolOverload;
        typeNode->typeInfo               = makeType<TypeInfoGeneric>();
        typeNode->typeInfo->name         = typeNode->resolvedSymbolName->name;
        typeNode->typeInfo               = typeNode->typeInfo;
    }

    // Otherwise, this is strange, we should have a type
    SWAG_VERIFY(typeNode->typeInfo, Report::internalError(context->node, "resolveType, null type !"));

    // If type comes from an identifier, be sure it's a type
    if (typeNode->identifier && !typeNode->identifier->childs.empty())
    {
        auto child = typeNode->identifier->childs.back();
        if (!child->typeInfo || !child->typeInfo->isNative(NativeTypeKind::Undefined))
        {
            if (child->resolvedSymbolName)
            {
                auto symName = child->resolvedSymbolName;
                auto symOver = child->resolvedSymbolOverload;
                if (symName->kind != SymbolKind::Enum &&
                    symName->kind != SymbolKind::TypeAlias &&
                    symName->kind != SymbolKind::GenericType &&
                    symName->kind != SymbolKind::Struct &&
                    symName->kind != SymbolKind::Interface)
                {
                    Diagnostic  diag{child->sourceFile, child->token, Fmt(Err(Err0017), child->token.ctext(), Naming::aKindName(symName->kind).c_str())};
                    Diagnostic* note = Diagnostic::hereIs(symOver);
                    if ((typeNode->typeFlags & TYPEFLAG_IS_PTR) && symName->kind == SymbolKind::Variable)
                    {
                        if (symOver->typeInfo->isPointer())
                        {
                            diag.hint  = Nte(Nte1024);
                            auto note1 = Diagnostic::note(Fmt(Nte(Nte0105), symName->name.c_str(), symName->name.c_str()));
                            return context->report(diag, note1, note);
                        }
                        else
                        {
                            diag.hint = Nte(Nte1024);
                            return context->report(diag, note);
                        }
                    }

                    return context->report(diag, note);
                }
            }
        }
    }

    // In fact, this is a pointer
    if (typeNode->typeFlags & TYPEFLAG_IS_PTR)
    {
        auto ptrFlags = (typeNode->typeInfo->flags & TYPEINFO_GENERIC);
        if (typeNode->typeFlags & TYPEFLAG_IS_SELF)
            ptrFlags |= TYPEINFO_SELF;
        if (typeNode->typeFlags & TYPEFLAG_HAS_USING)
            ptrFlags |= TYPEINFO_HAS_USING;
        if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
            ptrFlags |= TYPEINFO_CONST;
        if (typeNode->typeFlags & TYPEFLAG_IS_PTR_ARITHMETIC)
            ptrFlags |= TYPEINFO_POINTER_ARITHMETIC;
        if (ptrFlags & TYPEINFO_GENERIC)
            typeNode->flags |= AST_IS_GENERIC;
        typeNode->typeInfo = g_TypeMgr->makePointerTo(typeNode->typeInfo, ptrFlags);
        return true;
    }

    if (!(typeNode->typeFlags & TYPEFLAG_IS_REF))
        forceConstType(context, typeNode);

    // In fact, this is a slice
    if (typeNode->typeFlags & TYPEFLAG_IS_SLICE)
    {
        auto ptrSlice         = makeType<TypeInfoSlice>();
        ptrSlice->pointedType = typeNode->typeInfo;
        if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
            ptrSlice->flags |= TYPEINFO_CONST;
        ptrSlice->flags |= (ptrSlice->pointedType->flags & TYPEINFO_GENERIC);
        typeNode->typeInfo = ptrSlice;
        ptrSlice->computeName();
        return true;
    }

    // In fact this is a reference
    if (typeNode->typeFlags & TYPEFLAG_IS_REF)
    {
        auto ptrFlags = TYPEINFO_POINTER_REF;
        if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
            ptrFlags |= TYPEINFO_CONST;
        ptrFlags |= (typeNode->typeInfo->flags & TYPEINFO_GENERIC);

        if (typeNode->typeFlags & TYPEFLAG_IS_MOVE_REF)
        {
            auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
            SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, Fmt(Err(Err0696), "moveref")}));
            ptrFlags |= TYPEINFO_POINTER_MOVE_REF;
        }

        auto typeRef       = g_TypeMgr->makePointerTo(typeNode->typeInfo, ptrFlags);
        typeNode->typeInfo = typeRef;
        return true;
    }

    // In fact, this is an array
    if (typeNode->typeFlags & TYPEFLAG_IS_ARRAY)
    {
        SWAG_ASSERT(typeNode->arrayDim);

        // Array without a specified size
        if (typeNode->arrayDim == UINT8_MAX)
        {
            auto ptrArray         = makeType<TypeInfoArray>();
            ptrArray->count       = UINT32_MAX;
            ptrArray->totalCount  = UINT32_MAX;
            ptrArray->pointedType = typeNode->typeInfo;
            ptrArray->finalType   = typeNode->typeInfo;
            if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
                ptrArray->flags |= TYPEINFO_CONST;
            ptrArray->flags |= (ptrArray->finalType->flags & TYPEINFO_GENERIC);
            ptrArray->sizeOf = 0;
            ptrArray->computeName();
            typeNode->typeInfo = ptrArray;
            return true;
        }

        auto rawType = typeNode->typeInfo;
        SWAG_VERIFY(!rawType->isVoid(), context->report({typeNode->childs.back(), Err(Err0148)}));

        auto totalCount = 1;
        for (int i = typeNode->arrayDim - 1; i >= 0; i--)
        {
            auto child = typeNode->childs[i];

            uint32_t count        = 0;
            bool     genericCount = false;

            if (child->kind == AstNodeKind::IdentifierRef &&
                !(child->flags & AST_CONST_EXPR) &&
                typeNode->ownerStructScope &&
                typeNode->ownerStructScope->owner->typeInfo->isGeneric())
            {
                genericCount = true;
            }
            else if (!child->hasComputedValue() &&
                     child->resolvedSymbolOverload &&
                     (child->resolvedSymbolOverload->flags & OVERLOAD_GENERIC))
            {
                genericCount = true;
            }
            else
            {
                SWAG_CHECK(checkIsConstExpr(context, child->hasComputedValue(), child, Err(Err0021)));
                count = (uint32_t) child->computedValue->reg.u32;
            }

            auto childType = TypeManager::concreteType(child->typeInfo);
            SWAG_VERIFY(childType->isNativeInteger(), context->report({child, Fmt(Err(Err0022), child->typeInfo->getDisplayNameC())}));
            SWAG_CHECK(context->checkSizeOverflow("array", count * rawType->sizeOf, SWAG_LIMIT_ARRAY_SIZE));
            SWAG_VERIFY(!child->isConstant0(), context->report({child, Err(Err0023)}));

            auto ptrArray   = makeType<TypeInfoArray>();
            ptrArray->count = count;
            totalCount *= ptrArray->count;
            SWAG_CHECK(context->checkSizeOverflow("array", totalCount * rawType->sizeOf, SWAG_LIMIT_ARRAY_SIZE));
            ptrArray->totalCount  = totalCount;
            ptrArray->pointedType = typeNode->typeInfo;
            ptrArray->finalType   = rawType;
            ptrArray->sizeOf      = ptrArray->count * ptrArray->pointedType->sizeOf;
            if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
                ptrArray->flags |= TYPEINFO_CONST;
            ptrArray->flags |= (ptrArray->finalType->flags & TYPEINFO_GENERIC);

            if (genericCount)
            {
                ptrArray->flags |= TYPEINFO_GENERIC | TYPEINFO_GENERIC_COUNT;
                ptrArray->sizeNode = child;
            }

            ptrArray->computeName();
            typeNode->typeInfo = ptrArray;
        }

        return true;
    }

    // Be sure we do not have a useless user 'const'
    auto typeC = TypeManager::concreteType(typeNode->typeInfo);
    if (typeNode->typeFlags & TYPEFLAG_HAS_LOC_CONST &&
        !typeC->isPointer() &&
        !typeC->isArray() &&
        !typeC->isStruct())
    {
        Diagnostic diag{typeNode->sourceFile, typeNode->locConst, Fmt(Err(Err0250), typeNode->typeInfo->getDisplayNameC())};
        diag.hint = Nte(Nte1026);
        return context->report(diag);
    }

    return true;
}

bool SemanticJob::resolveTypeAliasBefore(SemanticContext* context)
{
    auto node = context->node;

    // Collect all attributes for the variable
    SWAG_CHECK(collectAttributes(context, node, nullptr));

    auto typeInfo      = makeType<TypeInfoAlias>();
    typeInfo->declNode = node;
    typeInfo->name     = node->token.text;
    if (node->attributeFlags & ATTRIBUTE_STRICT)
        typeInfo->flags |= TYPEINFO_STRICT;
    node->typeInfo = typeInfo;

    uint32_t symbolFlags = OVERLOAD_INCOMPLETE;
    if (node->typeInfo->isGeneric())
        symbolFlags |= OVERLOAD_GENERIC;

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::TypeAlias;
    toAdd.flags    = symbolFlags;

    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    SWAG_CHECK(node->resolvedSymbolOverload);
    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        node->ownerScope->addPublicNode(node);
    return true;
}

bool SemanticJob::resolveTypeAlias(SemanticContext* context)
{
    auto node         = context->node;
    auto typeInfo     = CastTypeInfo<TypeInfoAlias>(node->typeInfo, TypeInfoKind::Alias);
    typeInfo->rawType = node->childs.front()->typeInfo;
    typeInfo->sizeOf  = typeInfo->rawType->sizeOf;
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_GENERIC);
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_CONST);
    typeInfo->computeName();
    uint32_t symbolFlags = node->resolvedSymbolOverload->flags & ~OVERLOAD_INCOMPLETE;
    if (typeInfo->isGeneric())
        symbolFlags |= OVERLOAD_GENERIC;

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::TypeAlias;
    toAdd.flags    = symbolFlags;
    SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd));
    return true;
}

bool SemanticJob::resolveExplicitBitCast(SemanticContext* context)
{
    auto node     = CastAst<AstCast>(context->node, AstNodeKind::Cast);
    auto typeNode = node->childs[0];
    auto exprNode = node->childs[1];

    SWAG_CHECK(checkIsConcrete(context, exprNode));

    auto typeInfo     = TypeManager::concreteType(typeNode->typeInfo);
    auto exprTypeInfo = TypeManager::concreteType(exprNode->typeInfo);

    if (!typeInfo->isNativeInteger() &&
        !typeInfo->isNativeFloat() &&
        !typeInfo->isRune())
    {
        Diagnostic diag{typeNode, Fmt(Err(Err0031), typeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    if (!exprTypeInfo->isNativeInteger() &&
        !exprTypeInfo->isNativeFloat() &&
        !exprTypeInfo->isRune() &&
        !exprTypeInfo->isPointer())
    {
        Diagnostic diag{exprNode, Fmt(Err(Err0032), exprTypeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    if (typeInfo->sizeOf > exprTypeInfo->sizeOf)
    {
        Diagnostic diag{exprNode, Fmt(Err(Err0033), exprTypeInfo->getDisplayNameC(), typeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    node->typeInfo    = typeNode->typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitPassThrough;
    node->inheritOrFlag(exprNode, AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_R_VALUE | AST_L_VALUE | AST_SIDE_EFFECTS);
    node->inheritComputedValue(exprNode);
    node->resolvedSymbolName     = exprNode->resolvedSymbolName;
    node->resolvedSymbolOverload = exprNode->resolvedSymbolOverload;

    if (node->hasComputedValue() && node->typeInfo->isNative())
    {
        switch (node->typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            node->computedValue->reg.s64 = node->computedValue->reg.s8;
            break;
        case NativeTypeKind::S16:
            node->computedValue->reg.s64 = node->computedValue->reg.s16;
            break;
        case NativeTypeKind::S32:
            node->computedValue->reg.s64 = node->computedValue->reg.s32;
            break;
        case NativeTypeKind::U8:
            node->computedValue->reg.u64 = node->computedValue->reg.u8;
            break;
        case NativeTypeKind::U16:
            node->computedValue->reg.u64 = node->computedValue->reg.u16;
            break;
        case NativeTypeKind::U32:
            node->computedValue->reg.u64 = node->computedValue->reg.u32;
            break;
        default:
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveExplicitCast(SemanticContext* context)
{
    auto node     = CastAst<AstCast>(context->node, AstNodeKind::Cast);
    auto typeNode = node->childs[0];
    auto exprNode = node->childs[1];

    SWAG_CHECK(checkIsConcrete(context, exprNode));

    // When we cast from a structure to an interface, we need to be sure that every interfaces are
    // registered in the structure type, otherwise the cast can fail depending on the compile order
    auto exprTypeInfo = TypeManager::concretePtrRef(exprNode->typeInfo);
    if (typeNode->typeInfo->isInterface() && exprTypeInfo->isStruct())
    {
        context->job->waitAllStructInterfaces(exprTypeInfo);
        if (context->result == ContextResult::Pending)
            return true;
    }

    uint32_t castFlags = CASTFLAG_EXPLICIT | CASTFLAG_ACCEPT_PENDING;
    if (node->specFlags & AstCast::SPECFLAG_UNCONST)
        castFlags |= CASTFLAG_FORCE_UNCONST;
    if (node->specFlags & AstCast::SPECFLAG_OVERFLOW)
        castFlags |= CASTFLAG_CAN_OVERFLOW;
    SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, nullptr, exprNode, castFlags));
    if (context->result == ContextResult::Pending)
        return true;

    node->typeInfo       = typeNode->typeInfo;
    node->toCastTypeInfo = typeNode->typeInfo;

    node->byteCodeFct = ByteCodeGenJob::emitExplicitCast;
    node->inheritOrFlag(exprNode, AST_CONST_EXPR | AST_VALUE_IS_GENTYPEINFO | AST_VALUE_COMPUTED | AST_R_VALUE | AST_L_VALUE | AST_SIDE_EFFECTS | AST_OPAFFECT_CAST);
    node->inheritComputedValue(exprNode);
    node->resolvedSymbolName     = exprNode->resolvedSymbolName;
    node->resolvedSymbolOverload = exprNode->resolvedSymbolOverload;

    // In case case has triggered a special function call, need to get it
    // (usage of opAffect)
    if (exprNode->hasExtMisc() && exprNode->extMisc()->resolvedUserOpSymbolOverload)
    {
        node->allocateExtension(ExtensionKind::Misc);
        node->extMisc()->resolvedUserOpSymbolOverload = exprNode->extMisc()->resolvedUserOpSymbolOverload;
    }

    // Revert the implicit cast informations
    // Requested type will be stored in typeInfo of node, and previous type will be stored in typeInfo of exprNode
    // (we cannot use castedTypeInfo from node, because an explicit cast result could be casted itself with an implicit cast)
    if (exprNode->castedTypeInfo)
    {
        if (!(node->flags & (AST_VALUE_COMPUTED | AST_OPAFFECT_CAST)))
        {
            exprNode->typeInfo       = exprNode->castedTypeInfo;
            exprNode->castedTypeInfo = nullptr;
        }

        // In case of a computed value, we need to remember the type we come from to make
        // last minutes cast in emitLiteral
        else
            node->castedTypeInfo = exprNode->castedTypeInfo;
    }

    return true;
}

bool SemanticJob::resolveExplicitAutoCast(SemanticContext* context)
{
    auto node      = CastAst<AstCast>(context->node, AstNodeKind::AutoCast);
    auto exprNode  = node->childs[0];
    auto cloneType = exprNode->typeInfo->clone();
    cloneType->flags |= TYPEINFO_AUTO_CAST;
    node->typeInfo = cloneType;

    node->byteCodeFct = ByteCodeGenJob::emitExplicitAutoCast;
    node->inheritOrFlag(exprNode, AST_CONST_EXPR | AST_VALUE_IS_GENTYPEINFO | AST_VALUE_COMPUTED | AST_SIDE_EFFECTS);
    node->inheritComputedValue(exprNode);
    return true;
}

bool SemanticJob::resolveTypeList(SemanticContext* context)
{
    auto node = context->node;
    if (node->childs.size() == 1)
        node->typeInfo = node->childs.front()->typeInfo;
    return true;
}

bool SemanticJob::resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo** resultTypeInfo, uint32_t flags)
{
    SWAG_CHECK(resolveTypeAsExpression(context, node, node->typeInfo, resultTypeInfo, flags));
    return true;
}

bool SemanticJob::resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, TypeInfo** resultTypeInfo, uint32_t flags)
{
    auto  sourceFile = context->sourceFile;
    auto  module     = sourceFile->module;
    auto& typeGen    = module->typeGen;

    node->allocateComputedValue();
    node->computedValue->reg.pointer    = (uint8_t*) typeInfo;
    node->computedValue->storageSegment = getConstantSegFromContext(node);
    SWAG_CHECK(typeGen.genExportedTypeInfo(context, typeInfo, node->computedValue->storageSegment, &node->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT | flags, resultTypeInfo));
    if (context->result != ContextResult::Done)
        return true;
    node->setFlagsValueIsComputed();
    node->flags |= AST_VALUE_IS_GENTYPEINFO;
    return true;
}