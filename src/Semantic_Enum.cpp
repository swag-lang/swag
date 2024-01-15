#include "pch.h"
#include "Semantic.h"
#include "TypeManager.h"
#include "Ast.h"
#include "LanguageSpec.h"
#include "Workspace.h"
#include "Diagnostic.h"

bool Semantic::resolveEnum(SemanticContext* context)
{
    auto node     = CastAst<AstEnum>(context->node, AstNodeKind::EnumDecl);
    auto typeInfo = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);

    // An enum can be in a function, so do not generate bytecode for it !
    node->flags |= AST_NO_BYTECODE;

    // Be sure we have only one enum node
    if (node->resolvedSymbolName && node->resolvedSymbolName->nodes.size() > 1)
    {
        Diagnostic  diag({node, node->token, Fmt(Err(Err0820), node->resolvedSymbolName->name.c_str())});
        Diagnostic* note = nullptr;
        for (auto p : node->resolvedSymbolName->nodes)
        {
            if (p != node)
            {
                note                  = Diagnostic::note(p, p->getTokenName(), Nte(Nte0036));
                note->forceSourceFile = true;
                break;
            }
        }

        return context->report(diag, note);
    }

    typeInfo->declNode = node;
    node->scope->owner = node;

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = typeInfo;
    toAdd.kind     = SymbolKind::Enum;

    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    SWAG_CHECK(node->resolvedSymbolOverload);

    // Be sure we don't have duplicated values
    if (node->attributeFlags & ATTRIBUTE_NO_DUPLICATE)
    {
        auto rawType = TypeManager::concreteType(typeInfo->rawType);
        if (!rawType->isNative() && !rawType->isString())
        {
            Diagnostic diag{node->childs.front(), Fmt(Err(Err0675), rawType->getDisplayNameC())};
            return context->report(diag);
        }

        VectorNative<TypeInfoEnum*> collect;
        typeInfo->collectEnums(collect);
        MapUtf8<AstNode*>       valText;
        Map<uint64_t, AstNode*> val64;
        for (auto typeEnum : collect)
        {
            if (rawType->isString())
            {
                for (auto one : typeEnum->values)
                {
                    if (!one->value)
                        continue;
                    auto it = valText.find(one->value->text);
                    if (it != valText.end())
                    {
                        Diagnostic diag{one->declNode, one->declNode->token, Fmt(Err(Err0684), one->name.c_str())};
                        auto       note  = Diagnostic::note(it->second, it->second->getTokenName(), Nte(Nte0036));
                        auto       val   = Ast::literalToString(rawType, *one->value);
                        auto       note1 = Diagnostic::note(Fmt(Nte(Nte0014), val.c_str()));
                        return context->report(diag, note, note1);
                    }

                    valText[one->value->text] = one->declNode;
                }
            }
            else if (rawType->isNative())
            {
                for (auto one : typeEnum->values)
                {
                    if (!one->value)
                        continue;
                    auto it = val64.find(one->value->reg.u64);
                    if (it != val64.end())
                    {
                        Diagnostic diag{one->declNode, one->declNode->token, Fmt(Err(Err0684), one->name.c_str())};
                        auto       note  = Diagnostic::note(it->second, it->second->getTokenName(), Nte(Nte0036));
                        auto       val   = Ast::literalToString(rawType, *one->value);
                        auto       note1 = Diagnostic::note(Fmt(Nte(Nte0014), val.c_str()));
                        return context->report(diag, note, note1);
                    }

                    val64[one->value->reg.u64] = one->declNode;
                }
            }
        }
    }

    // Check public
    if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(node->flags & AST_FROM_GENERIC))
        node->ownerScope->addPublicNode(node);

    // We are parsing the swag module
    if (node->sourceFile->isBootstrapFile)
        g_Workspace->swagScope.registerType(node->typeInfo);

    return true;
}

bool Semantic::resolveEnumType(SemanticContext* context)
{
    auto typeNode = context->node;
    auto enumNode = context->node->parent;

    auto typeInfo = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    SWAG_CHECK(collectAttributes(context, enumNode, &typeInfo->attributes));

    // Hardcoded swag enums
    if (context->sourceFile->isBootstrapFile)
    {
        if (enumNode->token.text == g_LangSpec->name_AttributeUsage)
            enumNode->attributeFlags |= ATTRIBUTE_ENUM_FLAGS;
    }

    // By default, raw type is s32, except for flags and index
    TypeInfo* rawTypeInfo = (enumNode->attributeFlags & (ATTRIBUTE_ENUM_FLAGS | ATTRIBUTE_ENUM_INDEX)) ? g_TypeMgr->typeInfoU32 : g_TypeMgr->typeInfoS32;
    if (!typeNode->childs.empty())
        rawTypeInfo = typeNode->childs[0]->typeInfo;

    typeInfo->rawType = rawTypeInfo;
    typeInfo->sizeOf  = rawTypeInfo->sizeOf;

    if (enumNode->attributeFlags & ATTRIBUTE_ENUM_FLAGS)
    {
        typeInfo->flags |= TYPEINFO_ENUM_FLAGS;
        auto concreteType = TypeManager::concreteType(rawTypeInfo);
        if (!(concreteType->isNativeInteger()) || concreteType->isNativeIntegerSigned())
            return context->report({typeNode->childs.front(), Fmt(Err(Err0697), rawTypeInfo->getDisplayNameC())});
    }

    if (enumNode->attributeFlags & ATTRIBUTE_ENUM_INDEX)
    {
        typeInfo->flags |= TYPEINFO_ENUM_INDEX;
        auto concreteType = TypeManager::concreteType(rawTypeInfo);
        if (!(concreteType->isNativeInteger()))
            return context->report({typeNode->childs.front(), Fmt(Err(Err0698), rawTypeInfo->getDisplayNameC())});
    }

    if (enumNode->attributeFlags & ATTRIBUTE_INCOMPLETE)
        typeInfo->flags |= TYPEINFO_INCOMPLETE;

    rawTypeInfo = TypeManager::concreteType(rawTypeInfo, CONCRETE_FORCEALIAS);
    switch (rawTypeInfo->kind)
    {
    case TypeInfoKind::Generic:
        return true;

    case TypeInfoKind::Array:
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(rawTypeInfo, TypeInfoKind::Array);
        if (typeArray->count == UINT32_MAX)
        {
            auto       front = typeNode->childs.front();
            Diagnostic diag{front, Fmt(Err(Err0699), rawTypeInfo->getDisplayNameC())};
            return context->report(diag);
        }

        if (!rawTypeInfo->isConst())
        {
            auto       front = typeNode->childs.front();
            Diagnostic diag{front, Fmt(Err(Err0700), rawTypeInfo->getDisplayNameC())};
            auto       note = Diagnostic::note(Fmt(Nte(Nte1004), rawTypeInfo->getDisplayNameC()));
            return context->report(diag, note);
        }

        return true;
    }

    case TypeInfoKind::Slice:
    {
        auto       front = typeNode->childs.front();
        Diagnostic diag{front, Fmt(Err(Err0701), rawTypeInfo->getDisplayNameC(), rawTypeInfo->getDisplayNameC())};
        SWAG_VERIFY(rawTypeInfo->isConst(), context->report(diag));
        return true;
    }

    case TypeInfoKind::Native:
        if (rawTypeInfo->nativeType == NativeTypeKind::Any)
            return context->report({typeNode->childs.front(), Fmt(Err(Err0705), rawTypeInfo->getDisplayNameC())});
        return true;

    default:
        break;
    }

    if (rawTypeInfo->isCString())
        return context->report({typeNode->childs.front(), Err(Err0704)});

    if (!typeNode->childs.empty())
        typeNode = typeNode->childs.front();
    return context->report({typeNode, Fmt(Err(Err0705), rawTypeInfo->getDisplayNameC())});
}

bool Semantic::resolveSubEnumValue(SemanticContext* context)
{
    auto node  = context->node;
    auto child = node->childs.front();

    node->typeInfo               = child->typeInfo;
    node->resolvedSymbolName     = child->resolvedSymbolName;
    node->resolvedSymbolOverload = child->resolvedSymbolOverload;

    // Be sure the identifier is an enum
    if (!node->typeInfo->isEnum())
    {
        Diagnostic diag{node, Fmt(Err(Err0173), node->typeInfo->getDisplayNameC())};
        return context->report(diag, Diagnostic::hereIs(node));
    }

    auto enumNode = node->findParent(AstNodeKind::EnumDecl);
    SWAG_ASSERT(enumNode);
    auto typeEnum = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    if (typeEnum->rawType->isGeneric())
        return true;

    // Be sure enum types are the same
    auto typeSubEnum         = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
    auto concreteTypeSubEnum = TypeManager::concreteType(typeSubEnum->rawType, CONCRETE_ALIAS);
    auto concreteTypeEnum    = TypeManager::concreteType(typeEnum->rawType, CONCRETE_ALIAS);
    if (!concreteTypeSubEnum->isSame(concreteTypeEnum, CASTFLAG_EXACT))
    {
        Diagnostic diag{node, Fmt(Err(Err0549), concreteTypeEnum->getDisplayNameC(), concreteTypeSubEnum->getDisplayNameC())};
        return context->report(diag, Diagnostic::hereIs(node));
    }

    // Add a symbol in the enum scope
    AddSymbolTypeInfo toAdd;
    toAdd.node                   = node;
    toAdd.typeInfo               = node->typeInfo;
    toAdd.kind                   = SymbolKind::Enum;
    node->resolvedSymbolOverload = typeEnum->scope->symTable.addSymbolTypeInfo(context, toAdd);
    SWAG_CHECK(node->resolvedSymbolOverload);

    // Store the value in the enum type
    auto typeParam      = g_TypeMgr->makeParam();
    typeParam->name     = node->token.text;
    typeParam->typeInfo = typeSubEnum;
    typeParam->index    = (uint32_t) typeEnum->values.size();
    typeParam->declNode = node;
    SWAG_CHECK(collectAttributes(context, node, &typeParam->attributes));
    typeEnum->values.push_back(typeParam);
    typeEnum->flags |= TYPEINFO_ENUM_HAS_USING;

    return true;
}

bool Semantic::resolveEnumValue(SemanticContext* context)
{
    auto valNode  = CastAst<AstEnumValue>(context->node, AstNodeKind::EnumValue);
    auto enumNode = valNode->findParent(AstNodeKind::EnumDecl);
    SWAG_ASSERT(enumNode);
    auto typeEnum = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    if (typeEnum->rawType->isGeneric())
        return true;

    auto         assignNode     = valNode->childs.empty() ? nullptr : valNode->childs[0];
    auto         rawTypeInfo    = TypeManager::concreteType(typeEnum->rawType, CONCRETE_FORCEALIAS);
    uint32_t     storageOffset  = UINT32_MAX;
    DataSegment* storageSegment = nullptr;
    enumNode->allocateComputedValue();

    if (assignNode)
    {
        SWAG_CHECK(evaluateConstExpression(context, assignNode));
        YIELD();

        if (rawTypeInfo->isArray())
        {
            SWAG_CHECK(checkIsConstExpr(context, assignNode));
            SWAG_CHECK(TypeManager::makeCompatibles(context, rawTypeInfo, nullptr, assignNode, CASTFLAG_CONCRETE_ENUM));
            SWAG_ASSERT(!assignNode->hasComputedValue());

            assignNode->setFlagsValueIsComputed();
            storageSegment = getConstantSegFromContext(assignNode);
            SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffset, assignNode->computedValue, assignNode->typeInfo, assignNode));
            enumNode->computedValue->storageOffset  = storageOffset;
            enumNode->computedValue->storageSegment = storageSegment;
        }
        else if (rawTypeInfo->isSlice())
        {
            SWAG_CHECK(checkIsConstExpr(context, assignNode));
            SWAG_CHECK(TypeManager::makeCompatibles(context, rawTypeInfo, nullptr, assignNode, CASTFLAG_CONCRETE_ENUM));

            auto assignType = TypeManager::concreteType(assignNode->typeInfo);
            assignNode->setFlagsValueIsComputed();
            storageSegment = getConstantSegFromContext(assignNode);
            SWAG_CHECK(collectConstantSlice(context, assignNode, assignType, storageSegment, storageOffset));
            enumNode->computedValue->storageOffset  = storageOffset;
            enumNode->computedValue->storageSegment = storageSegment;
        }
        else
        {
            SWAG_CHECK(checkIsConstExpr(context, assignNode->hasComputedValue(), assignNode));
            SWAG_CHECK(TypeManager::makeCompatibles(context, rawTypeInfo, nullptr, assignNode, CASTFLAG_CONCRETE_ENUM));
            enumNode->computedValue = assignNode->computedValue;
        }
    }
    else
    {
        switch (rawTypeInfo->kind)
        {
        case TypeInfoKind::Native:
            switch (rawTypeInfo->nativeType)
            {
            case NativeTypeKind::Bool:
            case NativeTypeKind::Rune:
            case NativeTypeKind::String:
            case NativeTypeKind::F32:
            case NativeTypeKind::F64:
                return context->report({valNode, Fmt(Err(Err0706), valNode->token.ctext(), rawTypeInfo->getDisplayNameC())});
            default:
                break;
            }
            break;

        case TypeInfoKind::Slice:
            return context->report({valNode, Fmt(Err(Err0706), valNode->token.ctext(), rawTypeInfo->getDisplayNameC())});

        default:
            break;
        }
    }

    if (rawTypeInfo->isNative())
    {
        auto rawType = CastTypeInfo<TypeInfoNative>(rawTypeInfo, TypeInfoKind::Native);

        // First child is enumType
        AstNode* firstEnumValue = nullptr;
        for (size_t fev = 1; fev < enumNode->childs.size(); fev++)
        {
            firstEnumValue = enumNode->childs[fev];
            if (firstEnumValue->kind == AstNodeKind::EnumValue && !(firstEnumValue->specFlags & AstEnumValue::SPECFLAG_HAS_USING))
                break;
        }

        SWAG_ASSERT(firstEnumValue);

        // Compute automatic value from previous
        bool isFlags = (enumNode->attributeFlags & ATTRIBUTE_ENUM_FLAGS);
        if (!assignNode && (isFlags || (valNode != firstEnumValue)))
        {
            // Compute next value
            switch (rawType->nativeType)
            {
            case NativeTypeKind::U8:
                if (enumNode->computedValue->reg.u8 == UINT8_MAX)
                    return context->report({valNode, Fmt(Err(Err0708), valNode->token.ctext(), rawType->getDisplayNameC())});
                if (isFlags && enumNode->computedValue->reg.u8)
                {
                    auto n = enumNode->computedValue->reg.u8;
                    SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, Fmt(Err(Err0709), valNode->token.ctext())}));
                    enumNode->computedValue->reg.u8 <<= 1;
                }
                else
                    enumNode->computedValue->reg.u8++;
                break;
            case NativeTypeKind::U16:
                if (enumNode->computedValue->reg.u16 == UINT16_MAX)
                    return context->report({valNode, Fmt(Err(Err0708), valNode->token.ctext(), rawType->getDisplayNameC())});
                if (isFlags && enumNode->computedValue->reg.u16)
                {
                    auto n = enumNode->computedValue->reg.u16;
                    SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, Fmt(Err(Err0709), valNode->token.ctext())}));
                    enumNode->computedValue->reg.u16 <<= 1;
                }
                else
                    enumNode->computedValue->reg.u16++;
                break;
            case NativeTypeKind::U32:
                if (enumNode->computedValue->reg.u32 == UINT32_MAX)
                    return context->report({valNode, Fmt(Err(Err0708), valNode->token.ctext(), rawType->getDisplayNameC())});
                if (isFlags && enumNode->computedValue->reg.u32)
                {
                    auto n = enumNode->computedValue->reg.u32;
                    SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, Fmt(Err(Err0709), valNode->token.ctext())}));
                    enumNode->computedValue->reg.u32 <<= 1;
                }
                else
                    enumNode->computedValue->reg.u32++;
                break;
            case NativeTypeKind::U64:
                if (enumNode->computedValue->reg.u64 == UINT64_MAX)
                    return context->report({valNode, Fmt(Err(Err0708), valNode->token.ctext(), rawType->getDisplayNameC())});
                if (isFlags && enumNode->computedValue->reg.u64)
                {
                    auto n = enumNode->computedValue->reg.u64;
                    SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, Fmt(Err(Err0709), valNode->token.ctext())}));
                    enumNode->computedValue->reg.u64 <<= 1;
                }
                else
                    enumNode->computedValue->reg.u64++;
                break;

            case NativeTypeKind::S8:
                if (enumNode->computedValue->reg.s8 <= INT8_MIN || enumNode->computedValue->reg.s8 >= INT8_MAX)
                    return context->report({valNode, Fmt(Err(Err0708), valNode->token.ctext(), rawType->getDisplayNameC())});
                enumNode->computedValue->reg.s8++;
                break;
            case NativeTypeKind::S16:
                if (enumNode->computedValue->reg.s16 <= INT16_MIN || enumNode->computedValue->reg.s16 >= INT16_MAX)
                    return context->report({valNode, Fmt(Err(Err0708), valNode->token.ctext(), rawType->getDisplayNameC())});
                enumNode->computedValue->reg.s16++;
                break;
            case NativeTypeKind::S32:
                if (enumNode->computedValue->reg.s32 <= INT32_MIN || enumNode->computedValue->reg.s32 >= INT32_MAX)
                    return context->report({valNode, Fmt(Err(Err0708), valNode->token.ctext(), rawType->getDisplayNameC())});
                enumNode->computedValue->reg.s32++;
                break;
            case NativeTypeKind::S64:
                if (enumNode->computedValue->reg.s64 <= INT64_MIN || enumNode->computedValue->reg.s64 >= INT64_MAX)
                    return context->report({valNode, Fmt(Err(Err0708), valNode->token.ctext(), rawType->getDisplayNameC())});
                enumNode->computedValue->reg.s64++;
                break;
            default:
                break;
            }
        }
    }

    valNode->typeInfo = typeEnum;

    // Add a symbol in the enum scope
    AddSymbolTypeInfo toAdd;
    toAdd.node                      = valNode;
    toAdd.typeInfo                  = valNode->typeInfo;
    toAdd.kind                      = SymbolKind::EnumValue;
    toAdd.computedValue             = enumNode->computedValue;
    toAdd.storageOffset             = storageOffset;
    toAdd.storageSegment            = storageSegment;
    valNode->resolvedSymbolOverload = typeEnum->scope->symTable.addSymbolTypeInfo(context, toAdd);
    SWAG_CHECK(valNode->resolvedSymbolOverload);

    // Store the value in the enum type
    auto typeParam = g_TypeMgr->makeParam();
    typeParam->flags |= TYPEINFOPARAM_DEFINED_VALUE;
    typeParam->name     = valNode->token.text;
    typeParam->typeInfo = rawTypeInfo;
    typeParam->allocateComputedValue();
    *typeParam->value   = *enumNode->computedValue;
    typeParam->index    = (uint32_t) typeEnum->values.size();
    typeParam->declNode = valNode;
    SWAG_CHECK(collectAttributes(context, valNode, &typeParam->attributes));
    valNode->attributes = typeParam->attributes;
    typeEnum->values.push_back(typeParam);

    return true;
}