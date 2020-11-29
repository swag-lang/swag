#include "pch.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "SymTable.h"
#include "Scope.h"
#include "Allocator.h"
#include "SourceFile.h"

bool SemanticJob::resolveEnum(SemanticContext* context)
{
    auto node     = context->node;
    auto typeInfo = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
    SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, node, typeInfo, SymbolKind::Enum));

    // Check public
    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        if (!(node->flags & AST_FROM_GENERIC))
        {
            node->ownerScope->addPublicEnum(node);
        }
    }

    return true;
}

bool SemanticJob::resolveEnumType(SemanticContext* context)
{
    auto typeNode = context->node;
    auto enumNode = context->node->parent;

    auto typeInfo = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    SWAG_CHECK(collectAttributes(context, enumNode, typeInfo->attributes));

    // Hardcoded swag enums
    if (context->sourceFile->isBootstrapFile)
    {
        if (enumNode->token.text == "AttributeUsage")
            enumNode->attributeFlags |= ATTRIBUTE_ENUM_FLAGS;
    }

    // By default, raw type is s32, except for flags and index
    TypeInfo* rawTypeInfo = (enumNode->attributeFlags & (ATTRIBUTE_ENUM_FLAGS | ATTRIBUTE_INDEX)) ? g_TypeMgr.typeInfoU32 : g_TypeMgr.typeInfoS32;
    if (!typeNode->childs.empty())
        rawTypeInfo = typeNode->childs[0]->typeInfo;

    typeInfo->rawType = rawTypeInfo;
    typeInfo->sizeOf  = rawTypeInfo->sizeOf;

    if (enumNode->attributeFlags & ATTRIBUTE_ENUM_FLAGS)
    {
        typeInfo->flags |= TYPEINFO_ENUM_FLAGS;

        auto concreteType = TypeManager::concreteType(rawTypeInfo);
        if (concreteType != g_TypeMgr.typeInfoU8 &&
            concreteType != g_TypeMgr.typeInfoU16 &&
            concreteType != g_TypeMgr.typeInfoU32 &&
            concreteType != g_TypeMgr.typeInfoU64)
        {
            return context->report({typeNode->childs[0], format("invalid type '%s' for enum flags (should be u8, u16, u32 or u64)", rawTypeInfo->name.c_str())});
        }
    }

    if (enumNode->attributeFlags & ATTRIBUTE_INDEX)
    {
        typeInfo->flags |= TYPEINFO_ENUM_INDEX;

        auto concreteType = TypeManager::concreteType(rawTypeInfo);
        if (concreteType != g_TypeMgr.typeInfoU8 &&
            concreteType != g_TypeMgr.typeInfoU16 &&
            concreteType != g_TypeMgr.typeInfoU32)
        {
            return context->report({typeNode->childs[0], format("invalid type '%s' for enum index (should be u8, u16 or u32)", rawTypeInfo->name.c_str())});
        }
    }

    return true;
}

bool SemanticJob::resolveEnumValue(SemanticContext* context)
{
    auto valNode  = context->node;
    auto enumNode = valNode->ownerMainNode;
    SWAG_ASSERT(enumNode && enumNode->kind == AstNodeKind::EnumDecl);

    auto typeEnum = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    if (typeEnum->rawType->flags & TYPEINFO_GENERIC)
        return true;

    auto assignNode = valNode->childs.empty() ? nullptr : valNode->childs[0];

    auto rawType = CastTypeInfo<TypeInfoNative>(typeEnum->rawType, TypeInfoKind::Native);
    if (assignNode)
    {
        SWAG_VERIFY(assignNode->flags & AST_VALUE_COMPUTED, context->report({valNode, valNode->token, "expression cannot be evaluated at compile time"}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, rawType, nullptr, assignNode, CASTFLAG_CONCRETE_ENUM));
        enumNode->computedValue = assignNode->computedValue;
    }
    else
    {
        switch (rawType->nativeType)
        {
        case NativeTypeKind::Bool:
        case NativeTypeKind::Char:
        case NativeTypeKind::String:
        case NativeTypeKind::F32:
        case NativeTypeKind::F64:
            return context->report({valNode, valNode->token, format("enum value '%s' of type '%s' must be initialized", valNode->token.text.c_str(), rawType->name.c_str())});
        }
    }

    // First child is enumType
    AstNode* firstEnumValue = nullptr;
    for (int fev = 1; fev < enumNode->childs.size(); fev++)
    {
        firstEnumValue = enumNode->childs[fev];
        if (firstEnumValue->kind == AstNodeKind::EnumValue)
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
            if (enumNode->computedValue.reg.u8 == UINT8_MAX)
                return context->report({valNode, valNode->token, format("enum value '%s' is out of range of 'u8'", valNode->token.text.c_str())});
            if (isFlags && enumNode->computedValue.reg.u8)
            {
                auto n = enumNode->computedValue.reg.u8;
                SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, valNode->token, format("cannot deduce flag value of '%s' because previous value is not power of two", valNode->token.text.c_str())}));
                enumNode->computedValue.reg.u8 <<= 1;
            }
            else
                enumNode->computedValue.reg.u8++;
            break;
        case NativeTypeKind::U16:
            if (enumNode->computedValue.reg.u16 == UINT16_MAX)
                return context->report({valNode, valNode->token, format("enum value '%s' is out of range of 'u16'", valNode->token.text.c_str())});
            if (isFlags && enumNode->computedValue.reg.u16)
            {
                auto n = enumNode->computedValue.reg.u16;
                SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, valNode->token, format("cannot deduce flag value of '%s' because previous value is not power of two", valNode->token.text.c_str())}));
                enumNode->computedValue.reg.u16 <<= 1;
            }
            else
                enumNode->computedValue.reg.u16++;
            break;
        case NativeTypeKind::U32:
            if (enumNode->computedValue.reg.u32 == UINT32_MAX)
                return context->report({valNode, valNode->token, format("enum value '%s' is out of range of 'u32'", valNode->token.text.c_str())});
            if (isFlags && enumNode->computedValue.reg.u32)
            {
                auto n = enumNode->computedValue.reg.u32;
                SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, valNode->token, format("cannot deduce flag value of '%s' because previous value is not power of two", valNode->token.text.c_str())}));
                enumNode->computedValue.reg.u32 <<= 1;
            }
            else
                enumNode->computedValue.reg.u32++;
            break;
        case NativeTypeKind::U64:
            if (enumNode->computedValue.reg.u64 == UINT64_MAX)
                return context->report({valNode, valNode->token, format("enum value '%s' is out of range of 'u64'", valNode->token.text.c_str())});
            if (isFlags && enumNode->computedValue.reg.u64)
            {
                auto n = enumNode->computedValue.reg.u64;
                SWAG_VERIFY((n & (n - 1)) == 0, context->report({valNode, valNode->token, format("cannot deduce flag value of '%s' because previous value is not power of two", valNode->token.text.c_str())}));
                enumNode->computedValue.reg.u64 <<= 1;
            }
            else
                enumNode->computedValue.reg.u64++;
            break;

        case NativeTypeKind::S8:
            if (enumNode->computedValue.reg.s8 <= INT8_MIN || enumNode->computedValue.reg.s8 >= INT8_MAX)
                return context->report({valNode, valNode->token, format("enum value '%s' is out of range of 's8'", valNode->token.text.c_str())});
            enumNode->computedValue.reg.s8++;
            break;
        case NativeTypeKind::S16:
            if (enumNode->computedValue.reg.s16 <= INT16_MIN || enumNode->computedValue.reg.s16 >= INT16_MAX)
                return context->report({valNode, valNode->token, format("enum value '%s' is out of range of 's16'", valNode->token.text.c_str())});
            enumNode->computedValue.reg.s16++;
            break;
        case NativeTypeKind::S32:
            if (enumNode->computedValue.reg.s32 <= INT32_MIN || enumNode->computedValue.reg.s32 >= INT32_MAX)
                return context->report({valNode, valNode->token, format("enum value '%s' is out of range of 's32'", valNode->token.text.c_str())});
            enumNode->computedValue.reg.s32++;
            break;
        case NativeTypeKind::S64:
            if (enumNode->computedValue.reg.s64 <= INT64_MIN || enumNode->computedValue.reg.s64 >= INT64_MAX)
                return context->report({valNode, valNode->token, format("enum value '%s' is out of range of 's64'", valNode->token.text.c_str())});
            enumNode->computedValue.reg.s64++;
            break;
        }
    }

    valNode->typeInfo = typeEnum;
    SWAG_CHECK(typeEnum->scope->symTable.addSymbolTypeInfo(context, valNode, valNode->typeInfo, SymbolKind::EnumValue, &enumNode->computedValue));

    // Store each value in the enum type
    auto typeParam = allocType<TypeInfoParam>();
    typeParam->flags |= TYPEINFO_DEFINED_VALUE;
    typeParam->namedParam = valNode->token.text;
    typeParam->typeInfo   = rawType;
    typeParam->value      = enumNode->computedValue;
    typeParam->index      = (uint32_t) typeEnum->values.size();
    typeParam->node       = valNode;
    SWAG_CHECK(collectAttributes(context, valNode, typeParam->attributes));
    typeEnum->values.push_back(typeParam);

    return true;
}