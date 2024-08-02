#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/SourceFile.h"
#include "Wmf/Workspace.h"

bool Semantic::resolveEnum(SemanticContext* context)
{
    const auto node     = castAst<AstEnum>(context->node, AstNodeKind::EnumDecl);
    const auto typeInfo = castTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);

    // An enum can be in a function, so do not generate bytecode for it !
    node->addAstFlag(AST_NO_BYTECODE);

    // Be sure we have only one enum node
    const auto symbolName = node->resolvedSymbolName();
    if (symbolName && symbolName->nodes.size() > 1)
    {
        Diagnostic err({node, node->getTokenName(), formErr(Err0077, symbolName->name.c_str())});
        for (const auto p : symbolName->nodes)
        {
            if (p != node)
            {
                const auto note   = Diagnostic::note(p, p->getTokenName(), toNte(Nte0071));
                note->canBeMerged = false;
                err.addNote(note);
                break;
            }
        }

        return context->report(err);
    }

    typeInfo->declNode = node;
    node->scope->owner = node;

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = typeInfo;
    toAdd.kind     = SymbolKind::Enum;

    node->setResolvedSymbolOverload(node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd));
    SWAG_CHECK(node->resolvedSymbolOverload());

    // Be sure we don't have duplicated values
    if (node->hasAttribute(ATTRIBUTE_NO_DUPLICATE))
    {
        const auto rawType = TypeManager::concreteType(typeInfo->rawType);
        if (!rawType->isNative() && !rawType->isString())
        {
            const Diagnostic err{node->firstChild(), formErr(Err0264, rawType->getDisplayNameC())};
            return context->report(err);
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
                        Diagnostic err{one->declNode, one->declNode->token, formErr(Err0067, one->name.c_str())};
                        err.addNote(it->second, it->second->getTokenName(), toNte(Nte0071));
                        const auto val = Ast::literalToString(rawType, *one->value);
                        err.addNote(formNte(Nte0116, val.c_str()));
                        return context->report(err);
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
                        Diagnostic err{one->declNode, one->declNode->token, formErr(Err0067, one->name.c_str())};
                        err.addNote(it->second, it->second->getTokenName(), toNte(Nte0071));
                        const auto val = Ast::literalToString(rawType, *one->value);
                        err.addNote(formNte(Nte0116, val.c_str()));
                        return context->report(err);
                    }

                    val64[one->value->reg.u64] = one->declNode;
                }
            }
        }
    }

    // Check public
    if (node->hasAttribute(ATTRIBUTE_PUBLIC) && !node->hasAstFlag(AST_FROM_GENERIC))
        node->ownerScope->addPublicNode(node);

    // We are parsing the swag module
    if (node->token.sourceFile->hasFlag(FILE_BOOTSTRAP))
        g_Workspace->swagScope.registerType(node->typeInfo);

    return true;
}

bool Semantic::resolveEnumType(SemanticContext* context)
{
    auto       typeNode = context->node;
    const auto enumNode = context->node->parent;

    const auto typeInfo = castTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    SWAG_CHECK(collectAttributes(context, enumNode, &typeInfo->attributes));

    // Hardcoded swag enums
    if (context->sourceFile->hasFlag(FILE_BOOTSTRAP))
    {
        if (enumNode->token.is(g_LangSpec->name_AttributeUsage))
            enumNode->addAttribute(ATTRIBUTE_ENUM_FLAGS);
    }

    // By default, raw type is s32, except for flags and index
    TypeInfo* rawTypeInfo = enumNode->hasAttribute(ATTRIBUTE_ENUM_FLAGS | ATTRIBUTE_ENUM_INDEX) ? g_TypeMgr->typeInfoU32 : g_TypeMgr->typeInfoS32;
    if (!typeNode->children.empty())
        rawTypeInfo = typeNode->firstChild()->typeInfo;

    typeInfo->rawType = rawTypeInfo;
    typeInfo->sizeOf  = rawTypeInfo->sizeOf;

    if (enumNode->hasAttribute(ATTRIBUTE_ENUM_FLAGS))
    {
        typeInfo->addFlag(TYPEINFO_ENUM_FLAGS);
        const auto concreteType = TypeManager::concreteType(rawTypeInfo);
        if (!concreteType->isNativeInteger() || concreteType->isNativeIntegerSigned())
            return context->report({typeNode->firstChild(), formErr(Err0262, rawTypeInfo->getDisplayNameC())});
    }

    if (enumNode->hasAttribute(ATTRIBUTE_ENUM_INDEX))
    {
        typeInfo->addFlag(TYPEINFO_ENUM_INDEX);
        const auto concreteType = TypeManager::concreteType(rawTypeInfo);
        if (!concreteType->isNativeInteger())
            return context->report({typeNode->firstChild(), formErr(Err0263, rawTypeInfo->getDisplayNameC())});
    }

    if (enumNode->hasAttribute(ATTRIBUTE_INCOMPLETE))
        typeInfo->addFlag(TYPEINFO_INCOMPLETE);

    rawTypeInfo = TypeManager::concreteType(rawTypeInfo, CONCRETE_FORCE_ALIAS);
    switch (rawTypeInfo->kind)
    {
        case TypeInfoKind::Generic:
            return true;

        case TypeInfoKind::Array:
        {
            const auto typeArray = castTypeInfo<TypeInfoArray>(rawTypeInfo, TypeInfoKind::Array);
            if (typeArray->count == UINT32_MAX)
            {
                const auto       front = typeNode->firstChild();
                const Diagnostic err{front, formErr(Err0266, rawTypeInfo->getDisplayNameC())};
                return context->report(err);
            }

            if (!rawTypeInfo->isConst())
            {
                const auto front = typeNode->firstChild();
                Diagnostic err{front, formErr(Err0265, rawTypeInfo->getDisplayNameC())};
                err.addNote(formNte(Nte0171, rawTypeInfo->getDisplayNameC()));
                return context->report(err);
            }

            return true;
        }

        case TypeInfoKind::Slice:
        {
            const auto       front = typeNode->firstChild();
            const Diagnostic err{front, formErr(Err0267, rawTypeInfo->getDisplayNameC(), rawTypeInfo->getDisplayNameC())};
            SWAG_VERIFY(rawTypeInfo->isConst(), context->report(err));
            return true;
        }

        case TypeInfoKind::Native:
            if (rawTypeInfo->nativeType == NativeTypeKind::Any)
                return context->report({typeNode->firstChild(), formErr(Err0268, rawTypeInfo->getDisplayNameC())});
            return true;
    }

    if (rawTypeInfo->isCString())
        return context->report({typeNode->firstChild(), toErr(Err0269)});

    if (!typeNode->children.empty())
        typeNode = typeNode->firstChild();
    return context->report({typeNode, formErr(Err0268, rawTypeInfo->getDisplayNameC())});
}

bool Semantic::resolveSubEnumValue(SemanticContext* context)
{
    const auto node  = context->node;
    const auto child = node->firstChild();

    node->typeInfo = child->typeInfo;
    node->setResolvedSymbol(child->resolvedSymbolName(), child->resolvedSymbolOverload());

    // Be sure the identifier is an enum
    if (!node->typeInfo->isEnum())
    {
        const Diagnostic err{node, formErr(Err0256, node->typeInfo->getDisplayNameC())};
        return context->report(err, Diagnostic::hereIs(node->resolvedSymbolOverload()));
    }

    const auto enumNode = castAst<AstEnum>(node->findParent(AstNodeKind::EnumDecl), AstNodeKind::EnumDecl);
    const auto typeEnum = castTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    if (typeEnum->rawType->isGeneric())
        return true;

    // Be sure enum types are the same
    const auto typeSubEnum         = castTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
    const auto concreteTypeSubEnum = TypeManager::concreteType(typeSubEnum->rawType, CONCRETE_ALIAS);
    const auto concreteTypeEnum    = TypeManager::concreteType(typeEnum->rawType, CONCRETE_ALIAS);
    if (!concreteTypeSubEnum->isSame(concreteTypeEnum, CAST_FLAG_EXACT))
    {
        const Diagnostic err{node, formErr(Err0421, concreteTypeEnum->getDisplayNameC(), concreteTypeSubEnum->getDisplayNameC())};
        const auto       note  = Diagnostic::hereIs(node->resolvedSymbolOverload());
        const auto       note1 = Diagnostic::hereIs(enumNode->type);
        return context->report(err, note, note1);
    }

    // Add a symbol in the enum scope
    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::Enum;
    node->setResolvedSymbolOverload(typeEnum->scope->symTable.addSymbolTypeInfo(context, toAdd));
    SWAG_CHECK(node->resolvedSymbolOverload());

    // Store the value in the enum type
    const auto typeParam = TypeManager::makeParam();
    typeParam->name      = node->token.text;
    typeParam->typeInfo  = typeSubEnum;
    typeParam->index     = typeEnum->values.size();
    typeParam->declNode  = node;
    SWAG_CHECK(collectAttributes(context, node, &typeParam->attributes));
    typeEnum->values.push_back(typeParam);
    typeEnum->addFlag(TYPEINFO_ENUM_HAS_USING);

    return true;
}

bool Semantic::resolveEnumValue(SemanticContext* context)
{
    auto       valNode  = castAst<AstEnumValue>(context->node, AstNodeKind::EnumValue);
    const auto enumNode = valNode->findParent(AstNodeKind::EnumDecl);
    SWAG_ASSERT(enumNode);
    const auto typeEnum = castTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    if (typeEnum->rawType->isGeneric())
        return true;

    const auto   assignNode     = valNode->children.empty() ? nullptr : valNode->firstChild();
    const auto   rawTypeInfo    = TypeManager::concreteType(typeEnum->rawType, CONCRETE_FORCE_ALIAS);
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
            SWAG_CHECK(TypeManager::makeCompatibles(context, rawTypeInfo, nullptr, assignNode, CAST_FLAG_CONCRETE_ENUM));
            SWAG_ASSERT(!assignNode->hasFlagComputedValue());

            assignNode->setFlagsValueIsComputed();
            storageSegment = getConstantSegFromContext(assignNode);
            SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffset, assignNode->computedValue(), assignNode->typeInfo, assignNode));
            enumNode->computedValue()->storageOffset  = storageOffset;
            enumNode->computedValue()->storageSegment = storageSegment;
        }
        else if (rawTypeInfo->isSlice())
        {
            SWAG_CHECK(checkIsConstExpr(context, assignNode));
            SWAG_CHECK(TypeManager::makeCompatibles(context, rawTypeInfo, nullptr, assignNode, CAST_FLAG_CONCRETE_ENUM));

            const auto assignType = TypeManager::concreteType(assignNode->typeInfo);
            assignNode->setFlagsValueIsComputed();
            storageSegment = getConstantSegFromContext(assignNode);
            SWAG_CHECK(collectConstantSlice(context, assignNode, assignType, storageSegment, storageOffset));
            enumNode->computedValue()->storageOffset  = storageOffset;
            enumNode->computedValue()->storageSegment = storageSegment;
        }
        else
        {
            SWAG_CHECK(checkIsConstExpr(context, assignNode->hasFlagComputedValue(), assignNode));
            SWAG_CHECK(TypeManager::makeCompatibles(context, rawTypeInfo, nullptr, assignNode, CAST_FLAG_CONCRETE_ENUM));
            enumNode->extSemantic()->computedValue = assignNode->computedValue();
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
                        return context->report({valNode, formErr(Err0556, valNode->token.c_str(), rawTypeInfo->getDisplayNameC())});
                    default:
                        break;
                }
                break;

            case TypeInfoKind::Slice:
                return context->report({valNode, formErr(Err0556, valNode->token.c_str(), rawTypeInfo->getDisplayNameC())});

            default:
                break;
        }
    }

    if (rawTypeInfo->isNative())
    {
        const auto rawType = castTypeInfo<TypeInfoNative>(rawTypeInfo, TypeInfoKind::Native);

        // First child is enumType
        const AstNode* firstEnumValue = nullptr;
        for (uint32_t fev = 1; fev < enumNode->childCount(); fev++)
        {
            firstEnumValue = enumNode->children[fev];
            if (firstEnumValue->is(AstNodeKind::EnumValue) && !firstEnumValue->hasSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING))
                break;
        }

        SWAG_ASSERT(firstEnumValue);

        // Compute automatic value from previous
        const bool isFlags = enumNode->hasAttribute(ATTRIBUTE_ENUM_FLAGS);
        if (!assignNode && (isFlags || valNode != firstEnumValue))
        {
            // Compute next value
            switch (rawType->nativeType)
            {
                case NativeTypeKind::U8:
                    if (enumNode->computedValue()->reg.u8 == UINT8_MAX)
                        return context->report({valNode, formErr(Err0601, valNode->token.c_str(), rawType->getDisplayNameC())});
                    if (isFlags && enumNode->computedValue()->reg.u8)
                    {
                        const auto n = enumNode->computedValue()->reg.u8;
                        SWAG_VERIFY((n & n - 1) == 0, context->report({valNode, formErr(Err0540, valNode->token.c_str())}));
                        enumNode->computedValue()->reg.u8 <<= 1;
                    }
                    else
                        enumNode->computedValue()->reg.u8++;
                    break;
                case NativeTypeKind::U16:
                    if (enumNode->computedValue()->reg.u16 == UINT16_MAX)
                        return context->report({valNode, formErr(Err0601, valNode->token.c_str(), rawType->getDisplayNameC())});
                    if (isFlags && enumNode->computedValue()->reg.u16)
                    {
                        const auto n = enumNode->computedValue()->reg.u16;
                        SWAG_VERIFY((n & n - 1) == 0, context->report({valNode, formErr(Err0540, valNode->token.c_str())}));
                        enumNode->computedValue()->reg.u16 <<= 1;
                    }
                    else
                        enumNode->computedValue()->reg.u16++;
                    break;
                case NativeTypeKind::U32:
                    if (enumNode->computedValue()->reg.u32 == UINT32_MAX)
                        return context->report({valNode, formErr(Err0601, valNode->token.c_str(), rawType->getDisplayNameC())});
                    if (isFlags && enumNode->computedValue()->reg.u32)
                    {
                        const auto n = enumNode->computedValue()->reg.u32;
                        SWAG_VERIFY((n & n - 1) == 0, context->report({valNode, formErr(Err0540, valNode->token.c_str())}));
                        enumNode->computedValue()->reg.u32 <<= 1;
                    }
                    else
                        enumNode->computedValue()->reg.u32++;
                    break;
                case NativeTypeKind::U64:
                    if (enumNode->computedValue()->reg.u64 == UINT64_MAX)
                        return context->report({valNode, formErr(Err0601, valNode->token.c_str(), rawType->getDisplayNameC())});
                    if (isFlags && enumNode->computedValue()->reg.u64)
                    {
                        const auto n = enumNode->computedValue()->reg.u64;
                        SWAG_VERIFY((n & n - 1) == 0, context->report({valNode, formErr(Err0540, valNode->token.c_str())}));
                        enumNode->computedValue()->reg.u64 <<= 1;
                    }
                    else
                        enumNode->computedValue()->reg.u64++;
                    break;

                case NativeTypeKind::S8:
                    if (enumNode->computedValue()->reg.s8 <= INT8_MIN || enumNode->computedValue()->reg.s8 >= INT8_MAX)
                        return context->report({valNode, formErr(Err0601, valNode->token.c_str(), rawType->getDisplayNameC())});
                    enumNode->computedValue()->reg.s8++;
                    break;
                case NativeTypeKind::S16:
                    if (enumNode->computedValue()->reg.s16 <= INT16_MIN || enumNode->computedValue()->reg.s16 >= INT16_MAX)
                        return context->report({valNode, formErr(Err0601, valNode->token.c_str(), rawType->getDisplayNameC())});
                    enumNode->computedValue()->reg.s16++;
                    break;
                case NativeTypeKind::S32:
                    if (enumNode->computedValue()->reg.s32 <= INT32_MIN || enumNode->computedValue()->reg.s32 >= INT32_MAX)
                        return context->report({valNode, formErr(Err0601, valNode->token.c_str(), rawType->getDisplayNameC())});
                    enumNode->computedValue()->reg.s32++;
                    break;
                case NativeTypeKind::S64:
                    if (enumNode->computedValue()->reg.s64 <= INT64_MIN || enumNode->computedValue()->reg.s64 >= INT64_MAX)
                        return context->report({valNode, formErr(Err0601, valNode->token.c_str(), rawType->getDisplayNameC())});
                    enumNode->computedValue()->reg.s64++;
                    break;
                default:
                    break;
            }
        }
    }

    valNode->typeInfo = typeEnum;

    // Add a symbol in the enum scope
    AddSymbolTypeInfo toAdd;
    toAdd.node           = valNode;
    toAdd.typeInfo       = valNode->typeInfo;
    toAdd.kind           = SymbolKind::EnumValue;
    toAdd.computedValue  = enumNode->computedValue();
    toAdd.storageOffset  = storageOffset;
    toAdd.storageSegment = storageSegment;
    valNode->setResolvedSymbolOverload(typeEnum->scope->symTable.addSymbolTypeInfo(context, toAdd));
    SWAG_CHECK(valNode->resolvedSymbolOverload());

    // Store the value in the enum type
    const auto typeParam = TypeManager::makeParam();
    typeParam->flags.add(TYPEINFOPARAM_DEFINED_VALUE);
    typeParam->name     = valNode->token.text;
    typeParam->typeInfo = rawTypeInfo;
    typeParam->allocateComputedValue();
    *typeParam->value   = *enumNode->computedValue();
    typeParam->index    = typeEnum->values.size();
    typeParam->declNode = valNode;
    SWAG_CHECK(collectAttributes(context, valNode, &typeParam->attributes));
    valNode->attributes = typeParam->attributes;
    typeEnum->values.push_back(typeParam);

    return true;
}
