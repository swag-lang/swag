#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Scope.h"
#include "Semantic.h"
#include "SourceFile.h"
#include "TypeManager.h"
#include "Workspace.h"

bool Semantic::resolveEnum(SemanticContext* context)
{
    const auto node     = CastAst<AstEnum>(context->node, AstNodeKind::EnumDecl);
    const auto typeInfo = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);

    // An enum can be in a function, so do not generate bytecode for it !
    node->flags |= AST_NO_BYTECODE;

    // Be sure we have only one enum node
    if (node->resolvedSymbolName && node->resolvedSymbolName->nodes.size() > 1)
    {
        const Diagnostic diag({node, node->getTokenName(), Fmt(Err(Err0080), node->resolvedSymbolName->name.c_str())});
        Diagnostic*      note = nullptr;
        for (const auto p : node->resolvedSymbolName->nodes)
        {
            if (p != node)
            {
                note              = Diagnostic::note(p, p->getTokenName(), Nte(Nte0071));
                note->canBeMerged = false;
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
        const auto rawType = TypeManager::concreteType(typeInfo->rawType);
        if (!rawType->isNative() && !rawType->isString())
        {
            const Diagnostic diag{node->childs.front(), Fmt(Err(Err0269), rawType->getDisplayNameC())};
            return context->report(diag);
        }

        VectorNative<TypeInfoEnum*> collect;
        typeInfo->collectEnums(collect);
        MapUtf8<AstNode*>       valText;
        Map<uint64_t, AstNode*> val64;
        for (const auto typeEnum : collect)
        {
            if (rawType->isString())
            {
                for (const auto one : typeEnum->values)
                {
                    if (!one->value)
                        continue;
                    auto it = valText.find(one->value->text);
                    if (it != valText.end())
                    {
                        const Diagnostic diag{one->declNode, one->declNode->token, Fmt(Err(Err0069), one->name.c_str())};
                        const auto       note  = Diagnostic::note(it->second, it->second->getTokenName(), Nte(Nte0071));
                        const auto       val   = Ast::literalToString(rawType, *one->value);
                        const auto       note1 = Diagnostic::note(Fmt(Nte(Nte0116), val.c_str()));
                        return context->report(diag, note, note1);
                    }

                    valText[one->value->text] = one->declNode;
                }
            }
            else if (rawType->isNative())
            {
                for (const auto one : typeEnum->values)
                {
                    if (!one->value)
                        continue;
                    auto it = val64.find(one->value->reg.u64);
                    if (it != val64.end())
                    {
                        const Diagnostic diag{one->declNode, one->declNode->token, Fmt(Err(Err0069), one->name.c_str())};
                        const auto       note  = Diagnostic::note(it->second, it->second->getTokenName(), Nte(Nte0071));
                        const auto       val   = Ast::literalToString(rawType, *one->value);
                        const auto       note1 = Diagnostic::note(Fmt(Nte(Nte0116), val.c_str()));
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
    auto       typeNode = context->node;
    const auto enumNode = context->node->parent;

    const auto typeInfo = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
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
        const auto concreteType = TypeManager::concreteType(rawTypeInfo);
        if (!(concreteType->isNativeInteger()) || concreteType->isNativeIntegerSigned())
            return context->report({typeNode->childs.front(), Fmt(Err(Err0267), rawTypeInfo->getDisplayNameC())});
    }

    if (enumNode->attributeFlags & ATTRIBUTE_ENUM_INDEX)
    {
        typeInfo->flags |= TYPEINFO_ENUM_INDEX;
        const auto concreteType = TypeManager::concreteType(rawTypeInfo);
        if (!(concreteType->isNativeInteger()))
            return context->report({typeNode->childs.front(), Fmt(Err(Err0268), rawTypeInfo->getDisplayNameC())});
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
        const auto typeArray = CastTypeInfo<TypeInfoArray>(rawTypeInfo, TypeInfoKind::Array);
        if (typeArray->count == UINT32_MAX)
        {
            const auto       front = typeNode->childs.front();
            const Diagnostic diag{front, Fmt(Err(Err0271), rawTypeInfo->getDisplayNameC())};
            return context->report(diag);
        }

        if (!rawTypeInfo->isConst())
        {
            const auto       front = typeNode->childs.front();
            const Diagnostic diag{front, Fmt(Err(Err0270), rawTypeInfo->getDisplayNameC())};
            const auto       note = Diagnostic::note(Fmt(Nte(Nte0171), rawTypeInfo->getDisplayNameC()));
            return context->report(diag, note);
        }

        return true;
    }

    case TypeInfoKind::Slice:
    {
        const auto       front = typeNode->childs.front();
        const Diagnostic diag{front, Fmt(Err(Err0272), rawTypeInfo->getDisplayNameC(), rawTypeInfo->getDisplayNameC())};
        SWAG_VERIFY(rawTypeInfo->isConst(), context->report(diag));
        return true;
    }

    case TypeInfoKind::Native:
        if (rawTypeInfo->nativeType == NativeTypeKind::Any)
            return context->report({typeNode->childs.front(), Fmt(Err(Err0273), rawTypeInfo->getDisplayNameC())});
        return true;

    default:
        break;
    }

    if (rawTypeInfo->isCString())
        return context->report({typeNode->childs.front(), Err(Err0274)});

    if (!typeNode->childs.empty())
        typeNode = typeNode->childs.front();
    return context->report({typeNode, Fmt(Err(Err0273), rawTypeInfo->getDisplayNameC())});
}

bool Semantic::resolveSubEnumValue(SemanticContext* context)
{
    const auto node  = context->node;
    const auto child = node->childs.front();

    node->typeInfo               = child->typeInfo;
    node->resolvedSymbolName     = child->resolvedSymbolName;
    node->resolvedSymbolOverload = child->resolvedSymbolOverload;

    // Be sure the identifier is an enum
    if (!node->typeInfo->isEnum())
    {
        const Diagnostic diag{node, Fmt(Err(Err0261), node->typeInfo->getDisplayNameC())};
        return context->report(diag, Diagnostic::hereIs(node->resolvedSymbolOverload));
    }

    const auto enumNode = CastAst<AstEnum>(node->findParent(AstNodeKind::EnumDecl), AstNodeKind::EnumDecl);
    const auto typeEnum = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    if (typeEnum->rawType->isGeneric())
        return true;

    // Be sure enum types are the same
    const auto typeSubEnum         = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
    const auto concreteTypeSubEnum = TypeManager::concreteType(typeSubEnum->rawType, CONCRETE_ALIAS);
    const auto concreteTypeEnum    = TypeManager::concreteType(typeEnum->rawType, CONCRETE_ALIAS);
    if (!concreteTypeSubEnum->isSame(concreteTypeEnum, CASTFLAG_EXACT))
    {
        const Diagnostic diag{node, Fmt(Err(Err0429), concreteTypeEnum->getDisplayNameC(), concreteTypeSubEnum->getDisplayNameC())};
        const auto       note  = Diagnostic::hereIs(node->resolvedSymbolOverload);
        const auto       note1 = Diagnostic::hereIs(enumNode->type);
        return context->report(diag, note, note1);
    }

    // Add a symbol in the enum scope
    AddSymbolTypeInfo toAdd;
    toAdd.node                   = node;
    toAdd.typeInfo               = node->typeInfo;
    toAdd.kind                   = SymbolKind::Enum;
    node->resolvedSymbolOverload = typeEnum->scope->symTable.addSymbolTypeInfo(context, toAdd);
    SWAG_CHECK(node->resolvedSymbolOverload);

    // Store the value in the enum type
    const auto typeParam = TypeManager::makeParam();
    typeParam->name      = node->token.text;
    typeParam->typeInfo  = typeSubEnum;
    typeParam->index     = (uint32_t) typeEnum->values.size();
    typeParam->declNode  = node;
    SWAG_CHECK(collectAttributes(context, node, &typeParam->attributes));
    typeEnum->values.push_back(typeParam);
    typeEnum->flags |= TYPEINFO_ENUM_HAS_USING;

    return true;
}

bool Semantic::resolveEnumValue(SemanticContext* context)
{
    auto       valNode  = CastAst<AstEnumValue>(context->node, AstNodeKind::EnumValue);
    const auto enumNode = valNode->findParent(AstNodeKind::EnumDecl);
    SWAG_ASSERT(enumNode);
    const auto typeEnum = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    if (typeEnum->rawType->isGeneric())
        return true;

    const auto   assignNode     = valNode->childs.empty() ? nullptr : valNode->childs[0];
    const auto   rawTypeInfo    = TypeManager::concreteType(typeEnum->rawType, CONCRETE_FORCEALIAS);
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

            const auto assignType = TypeManager::concreteType(assignNode->typeInfo);
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
                return context->report({valNode, Fmt(Err(Err0567), valNode->token.ctext(), rawTypeInfo->getDisplayNameC())});
            default:
                break;
            }
            break;

        case TypeInfoKind::Slice:
            return context->report({valNode, Fmt(Err(Err0567), valNode->token.ctext(), rawTypeInfo->getDisplayNameC())});

        default:
            break;
        }
    }

    if (rawTypeInfo->isNative())
    {
        const auto rawType = CastTypeInfo<TypeInfoNative>(rawTypeInfo, TypeInfoKind::Native);

        // First child is enumType
        const AstNode* firstEnumValue = nullptr;
        for (size_t fev = 1; fev < enumNode->childs.size(); fev++)
        {
            firstEnumValue = enumNode->childs[fev];
            if (firstEnumValue->kind == AstNodeKind::EnumValue && !(firstEnumValue->specFlags & AstEnumValue::SPECFLAG_HAS_USING))
                break;
        }

        SWAG_ASSERT(firstEnumValue);

        // Compute automatic value from previous
        const bool isFlags = (enumNode->attributeFlags & ATTRIBUTE_ENUM_FLAGS);
        if (!assignNode && (isFlags || (valNode != firstEnumValue)))
        {
            // Compute next value
            switch (rawType->nativeType)
            {
            case NativeTypeKind::U8:
                if (enumNode->computedValue->reg.u8 == UINT8_MAX)
                    return context->report({valNode, Fmt(Err(Err0608), valNode->token.ctext(), rawType->getDisplayNameC())});
                if (isFlags && enumNode->computedValue->reg.u8)
                {
                    const auto n = enumNode->computedValue->reg.u8;
                    SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, Fmt(Err(Err0551), valNode->token.ctext())}));
                    enumNode->computedValue->reg.u8 <<= 1;
                }
                else
                    enumNode->computedValue->reg.u8++;
                break;
            case NativeTypeKind::U16:
                if (enumNode->computedValue->reg.u16 == UINT16_MAX)
                    return context->report({valNode, Fmt(Err(Err0608), valNode->token.ctext(), rawType->getDisplayNameC())});
                if (isFlags && enumNode->computedValue->reg.u16)
                {
                    const auto n = enumNode->computedValue->reg.u16;
                    SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, Fmt(Err(Err0551), valNode->token.ctext())}));
                    enumNode->computedValue->reg.u16 <<= 1;
                }
                else
                    enumNode->computedValue->reg.u16++;
                break;
            case NativeTypeKind::U32:
                if (enumNode->computedValue->reg.u32 == UINT32_MAX)
                    return context->report({valNode, Fmt(Err(Err0608), valNode->token.ctext(), rawType->getDisplayNameC())});
                if (isFlags && enumNode->computedValue->reg.u32)
                {
                    const auto n = enumNode->computedValue->reg.u32;
                    SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, Fmt(Err(Err0551), valNode->token.ctext())}));
                    enumNode->computedValue->reg.u32 <<= 1;
                }
                else
                    enumNode->computedValue->reg.u32++;
                break;
            case NativeTypeKind::U64:
                if (enumNode->computedValue->reg.u64 == UINT64_MAX)
                    return context->report({valNode, Fmt(Err(Err0608), valNode->token.ctext(), rawType->getDisplayNameC())});
                if (isFlags && enumNode->computedValue->reg.u64)
                {
                    const auto n = enumNode->computedValue->reg.u64;
                    SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, Fmt(Err(Err0551), valNode->token.ctext())}));
                    enumNode->computedValue->reg.u64 <<= 1;
                }
                else
                    enumNode->computedValue->reg.u64++;
                break;

            case NativeTypeKind::S8:
                if (enumNode->computedValue->reg.s8 <= INT8_MIN || enumNode->computedValue->reg.s8 >= INT8_MAX)
                    return context->report({valNode, Fmt(Err(Err0608), valNode->token.ctext(), rawType->getDisplayNameC())});
                enumNode->computedValue->reg.s8++;
                break;
            case NativeTypeKind::S16:
                if (enumNode->computedValue->reg.s16 <= INT16_MIN || enumNode->computedValue->reg.s16 >= INT16_MAX)
                    return context->report({valNode, Fmt(Err(Err0608), valNode->token.ctext(), rawType->getDisplayNameC())});
                enumNode->computedValue->reg.s16++;
                break;
            case NativeTypeKind::S32:
                if (enumNode->computedValue->reg.s32 <= INT32_MIN || enumNode->computedValue->reg.s32 >= INT32_MAX)
                    return context->report({valNode, Fmt(Err(Err0608), valNode->token.ctext(), rawType->getDisplayNameC())});
                enumNode->computedValue->reg.s32++;
                break;
            case NativeTypeKind::S64:
                if (enumNode->computedValue->reg.s64 <= INT64_MIN || enumNode->computedValue->reg.s64 >= INT64_MAX)
                    return context->report({valNode, Fmt(Err(Err0608), valNode->token.ctext(), rawType->getDisplayNameC())});
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
    const auto typeParam = TypeManager::makeParam();
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
