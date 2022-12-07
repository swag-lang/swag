#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Report.h"

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

    CompilerMessage msg      = {0};
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
    if (node->flags & AST_VALUE_COMPUTED && !(node->flags & AST_NO_BYTECODE))
        return true;

    if (node->kind == AstNodeKind::TypeExpression || node->kind == AstNodeKind::TypeLambda)
        return context->report({node, Err(Err0012)});

    if (node->flags & AST_FROM_GENERIC_REPLACE)
        return context->report({node, Err(Err0012)});

    if (node->resolvedSymbolName)
    {
        Utf8 name = SymTable::getNakedKindName(node->resolvedSymbolName->kind);
        Utf8 hint;

        // Reference to a static struct member
        if (node->resolvedSymbolOverload && node->resolvedSymbolOverload->flags & OVERLOAD_VAR_STRUCT)
        {
            name = "the struct member";
            hint = Fmt(Hnt(Hnt0003), node->resolvedSymbolOverload->symbol->ownerTable->scope->name.c_str());
        }

        Diagnostic  diag{node, Fmt(Err(Err0013), name.c_str(), node->resolvedSymbolName->name.c_str()), hint};
        Diagnostic* note = nullptr;

        // Missing self ?
        if (node->childs.size() <= 1 && node->ownerStructScope && node->ownerStructScope == context->node->ownerStructScope && node->ownerFct)
        {
            if (node->ownerStructScope->symTable.find(node->resolvedSymbolName->name))
            {
                note = new Diagnostic{Nte(Nte0005), DiagnosticLevel::Note};
            }
        }

        return context->report(diag, note);
    }

    return true;
}

bool SemanticJob::checkIsConcreteOrType(SemanticContext* context, AstNode* node)
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

    return checkIsConcrete(context, node);
}

bool SemanticJob::resolveTypeLambdaClosure(SemanticContext* context)
{
    auto node          = CastAst<AstTypeLambda>(context->node, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
    auto typeInfo      = allocType<TypeInfoFuncAttr>(TypeInfoKind::Lambda);
    typeInfo->declNode = node;

    if (node->specFlags & AST_SPEC_TYPELAMBDA_CANTHROW)
        typeInfo->flags |= TYPEINFO_CAN_THROW;

    if (node->returnType)
    {
        typeInfo->returnType = node->returnType->typeInfo;
        if (typeInfo->returnType->flags & TYPEINFO_GENERIC)
            typeInfo->flags |= TYPEINFO_GENERIC;
    }
    else
    {
        typeInfo->returnType = g_TypeMgr->typeInfoVoid;
    }

    if (node->parameters)
    {
        int index = 0;
        for (auto param : node->parameters->childs)
        {
            auto typeParam      = g_TypeMgr->makeParam();
            typeParam->typeInfo = param->typeInfo;
            typeParam->declNode = param;

            if (typeParam->typeInfo->flags & TYPEINFO_GENERIC)
                typeInfo->flags |= TYPEINFO_GENERIC;

            // Variadic must be the last one
            if (typeParam->typeInfo->kind == TypeInfoKind::Variadic)
            {
                typeInfo->flags |= TYPEINFO_VARIADIC;
                if (index != node->parameters->childs.size() - 1)
                    return context->report({param, Err(Err0734)});
            }
            else if (typeParam->typeInfo->kind == TypeInfoKind::TypedVariadic)
            {
                typeInfo->flags |= TYPEINFO_TYPED_VARIADIC;
                if (index != node->parameters->childs.size() - 1)
                    return context->report({param, Err(Err0734)});
            }
            else if (typeParam->typeInfo->kind == TypeInfoKind::CVariadic)
            {
                typeInfo->flags |= TYPEINFO_C_VARIADIC;
                if (index != node->parameters->childs.size() - 1)
                    return context->report({param, Err(Err0734)});
            }

            typeInfo->parameters.push_back(typeParam);
            index++;
        }
    }

    typeInfo->computeName();

    if (node->kind == AstNodeKind::TypeClosure)
    {
        typeInfo->sizeOf = SWAG_LIMIT_CLOSURE_SIZEOF;
        typeInfo->flags |= TYPEINFO_RETURN_BY_COPY | TYPEINFO_CLOSURE;
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
    if (node->typeInfo->flags & TYPEINFO_RETURN_BY_COPY ||
        node->typeInfo->kind == TypeInfoKind::Pointer ||
        node->typeInfo->kind == TypeInfoKind::Slice)
    {
        if (node->typeFlags & TYPEFLAG_FORCE_CONST)
            node->typeFlags |= TYPEFLAG_IS_CONST;
        if (node->typeFlags & TYPEFLAG_IS_CONST)
            node->typeInfo = TypeManager::makeConst(node->typeInfo);
    }
}

bool SemanticJob::resolveType(SemanticContext* context)
{
    auto typeNode = CastAst<AstTypeExpression>(context->node, AstNodeKind::TypeExpression);

    // Array with predefined dimensions, we evaluate all dimensions as const
    if (typeNode->arrayDim && typeNode->arrayDim != UINT8_MAX)
    {
        for (int i = typeNode->arrayDim - 1; i >= 0; i--)
        {
            auto child = typeNode->childs[i];

            // If generic, do not evaluate. No type for now
            if (child->kind == AstNodeKind::IdentifierRef &&
                !(child->flags & AST_CONST_EXPR) &&
                typeNode->ownerStructScope &&
                typeNode->ownerStructScope->owner->typeInfo->flags & TYPEINFO_GENERIC)
            {
                typeNode->typeInfo = g_TypeMgr->typeInfoUndefined;
                return true;
            }

            SWAG_CHECK(evaluateConstExpression(context, child));
            if (context->result == ContextResult::Pending)
                return true;
            child->flags |= AST_NO_BYTECODE;
        }
    }

    // Already solved
    if ((typeNode->flags & AST_FROM_GENERIC) && typeNode->typeInfo && !typeNode->typeInfo->isNative(NativeTypeKind::Undefined))
    {
        forceConstType(context, typeNode);
        return true;
    }

    if ((typeNode->semFlags & AST_SEM_TYPE_SOLVED) && typeNode->typeInfo && !typeNode->typeInfo->isNative(NativeTypeKind::Undefined))
    {
        forceConstType(context, typeNode);
        return true;
    }

    // Code
    if (typeNode->typeFlags & TYPEFLAG_IS_CODE)
    {
        auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
        SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, Err(Err0736)}));
        typeNode->typeInfo = g_TypeMgr->typeInfoCode;
        return true;
    }

    // cvarargs
    if (typeNode->typeFromLiteral && typeNode->typeFromLiteral->flags & TYPEINFO_C_VARIADIC)
    {
        auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
        SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, Err(Err0735)}));
        typeNode->typeInfo = g_TypeMgr->typeInfoCVariadic;
        return true;
    }

    if (typeNode->identifier)
    {
        typeNode->typeInfo = typeNode->identifier->typeInfo;
        typeNode->inheritOrFlag(typeNode->identifier, AST_IS_GENERIC);
    }
    else
    {
        if (!typeNode->typeFromLiteral)
            typeNode->typeFromLiteral = TypeManager::literalTypeToType(typeNode->token);
        typeNode->typeInfo = typeNode->typeFromLiteral;

        // Typed variadic ?
        if (typeNode->typeInfo->kind == TypeInfoKind::Variadic && !typeNode->childs.empty())
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
        typeNode->typeInfo               = allocType<TypeInfoGeneric>();
        typeNode->typeInfo->name         = typeNode->resolvedSymbolName->name;
        typeNode->typeInfo               = typeNode->typeInfo;
    }

    // Otherwise, this is strange, we should have a type
    SWAG_VERIFY(typeNode->typeInfo, Report::internalError(context->node, "resolveType, null type !"));

    // If type comes from an identifier, be sure it's a type
    if (typeNode->identifier)
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
                    Diagnostic diag{child->sourceFile, child->token, Fmt(Err(Err0017), child->token.ctext(), SymTable::getArticleKindName(symName->kind))};
                    Diagnostic note{symOver->node, Fmt(Nte(Nte0029), symName->name.c_str()), DiagnosticLevel::Note};
                    if (typeNode->ptrCount && symName->kind == SymbolKind::Variable)
                    {
                        if (symOver->typeInfo->kind == TypeInfoKind::Pointer)
                        {
                            Diagnostic note1{Fmt(Hlp(Hlp0005), symName->name.c_str(), symName->name.c_str()), DiagnosticLevel::Help};
                            diag.hint = Hnt(Hnt0024);
                            return context->report(diag, &note1, &note);
                        }
                        else
                        {
                            diag.hint = Hnt(Hnt0024);
                            return context->report(diag, &note);
                        }
                    }

                    return context->report(diag, &note);
                }
            }
        }
    }

    // In fact, this is a pointer
    if (typeNode->ptrCount)
    {
        auto             firstType  = typeNode->typeInfo;
        TypeInfoPointer* ptrPointer = nullptr;

        for (int i = 0; i < typeNode->ptrCount; i++)
        {
            bool isConst      = false;
            bool isArithmetic = false;

            if (typeNode->ptrFlags[i] & AstTypeExpression::PTR_CONST)
                isConst = true;
            else if (typeNode->typeFlags & TYPEFLAG_IS_CONST && i == 0)
                isConst = true;
            if (typeNode->ptrFlags[i] & AstTypeExpression::PTR_ARITMETIC)
                isArithmetic = true;

            auto ptrFlags = (firstType->flags & TYPEINFO_GENERIC);
            if (typeNode->typeFlags & TYPEFLAG_IS_SELF)
                ptrFlags |= TYPEINFO_SELF;
            if (typeNode->typeFlags & TYPEFLAG_USING)
                ptrFlags |= TYPEINFO_HAS_USING;
            if (ptrFlags & TYPEINFO_GENERIC)
                typeNode->flags |= AST_IS_GENERIC;

            auto ptrPointer1 = g_TypeMgr->makePointerTo(firstType, isConst, isArithmetic, ptrFlags);

            if (ptrPointer)
            {
                // Be sure we have a unique instance of the type, not a shared predefined one
                if (ptrPointer->flags & TYPEINFO_SHARED)
                {
                    auto newPtr = (TypeInfoPointer*) ptrPointer->clone();
                    if (typeNode->typeInfo == ptrPointer)
                        typeNode->typeInfo = newPtr;
                    ptrPointer = newPtr;
                }

                ptrPointer->pointedType = ptrPointer1;
                ptrPointer->forceComputeName();
            }

            ptrPointer = ptrPointer1;
            if (i == 0)
                typeNode->typeInfo = ptrPointer;
        }
    }

    // A struct function parameter is const
    else if (!(typeNode->typeFlags & TYPEFLAG_IS_REF))
    {
        forceConstType(context, typeNode);
    }

    // In fact, this is an array
    if (typeNode->arrayDim)
    {
        // Array of slice
        if (typeNode->typeFlags & TYPEFLAG_IS_SLICE)
        {
            auto ptrSlice         = allocType<TypeInfoSlice>();
            ptrSlice->pointedType = typeNode->typeInfo;
            if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
                ptrSlice->flags |= TYPEINFO_CONST;
            ptrSlice->flags |= (ptrSlice->pointedType->flags & TYPEINFO_GENERIC);
            typeNode->typeInfo = ptrSlice;
            ptrSlice->computeName();
            if (typeNode->typeFlags & TYPEFLAG_IS_CONST_SLICE)
                ptrSlice->setConst();
        }

        // Array without a specified size
        if (typeNode->arrayDim == UINT8_MAX)
        {
            auto ptrArray         = allocType<TypeInfoArray>();
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
        }
        else
        {
            auto rawType    = typeNode->typeInfo;
            auto totalCount = 1;
            for (int i = typeNode->arrayDim - 1; i >= 0; i--)
            {
                auto child = typeNode->childs[i];

                uint32_t count        = 0;
                bool     genericCount = false;

                if (!(child->flags & AST_VALUE_COMPUTED) && child->resolvedSymbolOverload && (child->resolvedSymbolOverload->flags & OVERLOAD_GENERIC))
                {
                    genericCount = true;
                }
                else
                {
                    SWAG_CHECK(checkIsConstExpr(context, child->flags & AST_VALUE_COMPUTED, child, Err(Err0021)));
                    count = (uint32_t) child->computedValue->reg.u32;
                }

                auto childType = TypeManager::concreteType(child->typeInfo);
                SWAG_VERIFY(childType->isNativeInteger(), context->report({child, Fmt(Err(Err0022), child->typeInfo->getDisplayNameC())}));
                SWAG_CHECK(context->checkSizeOverflow("array", count * rawType->sizeOf, SWAG_LIMIT_ARRAY_SIZE));
                SWAG_VERIFY(!child->isConstant0(), context->report({child, Err(Err0023)}));

                auto ptrArray   = allocType<TypeInfoArray>();
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
        }
    }

    // In fact, this is a slice
    else if (typeNode->typeFlags & TYPEFLAG_IS_SLICE)
    {
        auto ptrSlice         = allocType<TypeInfoSlice>();
        ptrSlice->pointedType = typeNode->typeInfo;
        if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
            ptrSlice->flags |= TYPEINFO_CONST;
        ptrSlice->flags |= (ptrSlice->pointedType->flags & TYPEINFO_GENERIC);
        typeNode->typeInfo = ptrSlice;
        ptrSlice->computeName();
    }

    // In fact this is a reference
    if (typeNode->typeFlags & TYPEFLAG_IS_REF)
    {
        auto typeRef = g_TypeMgr->makePointerTo(typeNode->typeInfo, typeNode->typeFlags & TYPEFLAG_IS_CONST, false, TYPEINFO_POINTER_REF);
        typeRef->flags |= (typeRef->pointedType->flags & TYPEINFO_GENERIC);
        typeNode->typeInfo = typeRef;
        typeRef->computeName();
    }

    typeNode->allocateComputedValue();
    typeNode->computedValue->reg.pointer = (uint8_t*) typeNode->typeInfo;
    if (!(typeNode->flags & AST_HAS_STRUCT_PARAMETERS))
        typeNode->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR | AST_NO_BYTECODE | AST_VALUE_IS_TYPEINFO;

    // Is this a const pointer to a typeinfo ?
    if (typeNode->typeInfo->isPointerToTypeInfo())
    {
        SWAG_VERIFY(typeNode->typeInfo->isConst(), context->report({typeNode, Err(Err0024), Hnt(Hnt0053)}));
    }

    return true;
}

bool SemanticJob::checkPublicAlias(SemanticContext* context, AstNode* node)
{
    auto back = node->childs.back();

    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        if (node->ownerScope->isGlobalOrImpl())
        {
            auto overload = back->resolvedSymbolOverload;
            if (overload && !(overload->node->attributeFlags & ATTRIBUTE_PUBLIC) && !overload->node->sourceFile->isGenerated)
            {
                Diagnostic diag{back, Fmt(Err(Err0025), back->token.ctext())};
                Diagnostic note{overload->node, Fmt(Nte(Nte0029), node->resolvedSymbolName->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }

            node->ownerScope->addPublicNode(node);
        }
    }

    return true;
}

bool SemanticJob::resolveAlias(SemanticContext* context)
{
    auto node = context->node;
    auto back = node->childs.back();

    // Be sure symbol is there...
    if (!node->resolvedSymbolName)
        node->resolvedSymbolName = node->ownerScope->symTable.registerSymbolName(context, node, SymbolKind::Alias);

    // alias x = @typeof
    auto overload = back->resolvedSymbolOverload;
    if (!overload && back->kind == AstNodeKind::IdentifierRef)
    {
        back = back->childs.back();
        if (back->token.id == TokenId::IntrinsicTypeOf)
        {
            node->resolvedSymbolName->kind = SymbolKind::TypeAlias;
            SWAG_CHECK(resolveTypeAlias(context));
            return true;
        }
    }

    SWAG_VERIFY(back->kind != AstNodeKind::ArrayPointerIndex, context->report({back, Err(Err0819)}));
    SWAG_VERIFY(overload, context->report({back, Err(Err0027)}));
    auto symbol       = overload->symbol;
    auto typeResolved = overload->typeInfo;

    if (typeResolved->kind == TypeInfoKind::Struct || typeResolved->kind == TypeInfoKind::Interface)
    {
        node->resolvedSymbolName->kind = SymbolKind::TypeAlias;
        SWAG_CHECK(resolveTypeAliasBefore(context));
        SWAG_CHECK(resolveTypeAlias(context));
        return true;
    }

    // Collect all attributes for the variable
    SWAG_CHECK(collectAttributes(context, node, nullptr));
    SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_STRICT), context->report({node, Err(Err0028)}));

    node->flags |= AST_NO_BYTECODE;

    // Constraints with alias on a variable
    if (symbol->kind == SymbolKind::Variable)
    {
        // alias x = struct.x is not possible
        if (back->kind == AstNodeKind::IdentifierRef)
        {
            int cptVar = 0;
            for (auto& c : back->childs)
            {
                if (c->resolvedSymbolName && c->resolvedSymbolName->kind == SymbolKind::Variable)
                {
                    SWAG_VERIFY(cptVar == 0, context->report({back, Err(Err0029)}));
                    cptVar++;
                }
            }
        }
    }

    switch (symbol->kind)
    {
    case SymbolKind::Namespace:
    case SymbolKind::Enum:
    case SymbolKind::Function:
    case SymbolKind::Variable:
    case SymbolKind::TypeAlias:
        break;
    default:
        return context->report({back, Fmt(Err(Err0030), SymTable::getArticleKindName(symbol->kind))});
    }

    SWAG_ASSERT(overload);
    SWAG_CHECK(node->ownerScope->symTable.registerUsingAliasOverload(context, node, node->resolvedSymbolName, overload));
    SWAG_CHECK(checkPublicAlias(context, node));
    return true;
}

bool SemanticJob::resolveTypeAliasBefore(SemanticContext* context)
{
    auto node = context->node;

    // Collect all attributes for the variable
    SWAG_CHECK(collectAttributes(context, node, nullptr));

    auto typeInfo      = allocType<TypeInfoAlias>();
    typeInfo->declNode = node;
    typeInfo->name     = node->token.text;
    if (node->attributeFlags & ATTRIBUTE_STRICT)
        typeInfo->flags |= TYPEINFO_STRICT;
    node->typeInfo = typeInfo;

    uint32_t symbolFlags = OVERLOAD_INCOMPLETE;
    if (node->typeInfo->flags & TYPEINFO_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;

    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::TypeAlias, nullptr, symbolFlags);
    SWAG_CHECK(node->resolvedSymbolOverload);
    SWAG_CHECK(checkPublicAlias(context, node));
    return true;
}

bool SemanticJob::resolveTypeAlias(SemanticContext* context)
{
    auto node         = context->node;
    auto typeInfo     = CastTypeInfo<TypeInfoAlias>(node->typeInfo, TypeInfoKind::Alias);
    typeInfo->rawType = node->childs.front()->typeInfo;
    typeInfo->sizeOf  = typeInfo->rawType->sizeOf;
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_RETURN_BY_COPY);
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_GENERIC);
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_CONST);
    typeInfo->computeName();
    uint32_t symbolFlags = node->resolvedSymbolOverload->flags & ~OVERLOAD_INCOMPLETE;
    if (typeInfo->flags & TYPEINFO_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;
    SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::TypeAlias, nullptr, symbolFlags));
    return true;
}

bool SemanticJob::resolveExplicitBitCast(SemanticContext* context)
{
    auto node     = CastAst<AstCast>(context->node, AstNodeKind::BitCast);
    auto typeNode = node->childs[0];
    auto exprNode = node->childs[1];

    SWAG_CHECK(checkIsConcrete(context, exprNode));

    auto typeInfo     = TypeManager::concreteType(typeNode->typeInfo);
    auto exprTypeInfo = TypeManager::concreteType(exprNode->typeInfo);

    if (!(typeInfo->flags & (TYPEINFO_INTEGER | TYPEINFO_FLOAT)) &&
        (!typeInfo->isNative(NativeTypeKind::Rune)))
        return context->report({typeNode, Fmt(Err(Err0031), typeInfo->getDisplayNameC())});

    if (!(exprTypeInfo->flags & (TYPEINFO_INTEGER | TYPEINFO_FLOAT)) &&
        (!exprTypeInfo->isNative(NativeTypeKind::Rune)) &&
        (exprTypeInfo->kind != TypeInfoKind::Pointer))
        return context->report({exprNode, Fmt(Err(Err0032), exprTypeInfo->getDisplayNameC())});

    SWAG_VERIFY(typeInfo->sizeOf <= exprTypeInfo->sizeOf, context->report({exprNode, Fmt(Err(Err0033), typeInfo->getDisplayNameC(), exprTypeInfo->getDisplayNameC())}));

    node->typeInfo = typeNode->typeInfo;
    node->setPassThrough();
    node->inheritOrFlag(exprNode, AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_R_VALUE | AST_L_VALUE | AST_SIDE_EFFECTS);
    node->inheritComputedValue(exprNode);
    node->resolvedSymbolName     = exprNode->resolvedSymbolName;
    node->resolvedSymbolOverload = exprNode->resolvedSymbolOverload;

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
    if (typeNode->typeInfo->kind == TypeInfoKind::Interface && exprTypeInfo->kind == TypeInfoKind::Struct)
    {
        context->job->waitAllStructInterfaces(exprTypeInfo);
        if (context->result == ContextResult::Pending)
            return true;
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, nullptr, exprNode, CASTFLAG_EXPLICIT | CASTFLAG_ACCEPT_PENDING));
    if (context->result == ContextResult::Pending)
        return true;

    node->typeInfo       = typeNode->typeInfo;
    node->toCastTypeInfo = typeNode->typeInfo;

    node->byteCodeFct = ByteCodeGenJob::emitExplicitCast;
    node->inheritOrFlag(exprNode, AST_CONST_EXPR | AST_VALUE_IS_TYPEINFO | AST_VALUE_COMPUTED | AST_R_VALUE | AST_L_VALUE | AST_SIDE_EFFECTS | AST_OPAFFECT_CAST);
    node->inheritComputedValue(exprNode);
    node->resolvedSymbolName     = exprNode->resolvedSymbolName;
    node->resolvedSymbolOverload = exprNode->resolvedSymbolOverload;

    // In case case has triggered a special function call, need to get it
    // (usage of opAffect)
    if (exprNode->extension && exprNode->extension->misc && exprNode->extension->misc->resolvedUserOpSymbolOverload)
    {
        node->allocateExtension(ExtensionKind::Resolve);
        node->extension->misc->resolvedUserOpSymbolOverload = exprNode->extension->misc->resolvedUserOpSymbolOverload;
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
    node->inheritOrFlag(exprNode, AST_CONST_EXPR | AST_VALUE_IS_TYPEINFO | AST_VALUE_COMPUTED | AST_SIDE_EFFECTS);
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
    auto& typeTable  = module->typeTable;

    node->allocateComputedValue();
    node->computedValue->reg.pointer    = (uint8_t*) typeInfo;
    node->computedValue->storageSegment = getConstantSegFromContext(node);
    SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, typeInfo, node->computedValue->storageSegment, &node->computedValue->storageOffset, MAKE_CONCRETE_SHOULD_WAIT | flags, resultTypeInfo));
    if (context->result != ContextResult::Done)
        return true;
    node->setFlagsValueIsComputed();
    node->flags |= AST_VALUE_IS_TYPEINFO;
    return true;
}