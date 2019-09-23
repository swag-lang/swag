#include "pch.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "SymTable.h"
#include "Scope.h"

bool SemanticJob::resolveEnum(SemanticContext* context)
{
    auto enumNode = context->node;
    auto typeInfo = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    SWAG_CHECK(enumNode->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, enumNode, typeInfo, SymbolKind::Enum));

    return true;
}

bool SemanticJob::resolveEnumType(SemanticContext* context)
{
    auto typeNode = context->node;
    auto enumNode = context->node->parent;

    auto rawTypeInfo  = typeNode->childs.empty() ? g_TypeMgr.typeInfoS32 : typeNode->childs[0]->typeInfo;
    auto typeInfo     = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    typeInfo->rawType = rawTypeInfo;
    typeInfo->sizeOf  = rawTypeInfo->sizeOf;

    return true;
}

bool SemanticJob::resolveEnumValue(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto valNode    = context->node;
    auto enumNode   = valNode->parent;
    auto typeEnum   = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    auto assignNode = valNode->childs.empty() ? nullptr : valNode->childs[0];

    auto rawType = CastTypeInfo<TypeInfoNative>(typeEnum->rawType, TypeInfoKind::Native);
    if (assignNode)
    {
        SWAG_VERIFY(assignNode->flags & AST_VALUE_COMPUTED, context->errorContext.report({sourceFile, valNode->token, "expression cannot be evaluated at compile time"}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, rawType, assignNode, CASTFLAG_CONCRETE_ENUM));
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
            return context->errorContext.report({sourceFile, valNode->token, format("enum value '%s' of type '%s' must be initialized", valNode->name.c_str(), rawType->name.c_str())});
        }
    }

    valNode->typeInfo = typeEnum;
    SWAG_CHECK(typeEnum->scope->symTable->addSymbolTypeInfo(context->sourceFile, valNode, valNode->typeInfo, SymbolKind::EnumValue, &enumNode->computedValue));

    // Store each value in the enum type
    auto typeParam = g_Pool_typeInfoParam.alloc();
    typeParam->flags |= TYPEINFO_DEFINED_VALUE;
    typeParam->namedParam = valNode->name;
    typeParam->typeInfo   = rawType;
    typeParam->value      = enumNode->computedValue;
    typeParam->index      = (uint32_t) typeEnum->values.size();
    typeEnum->values.push_back(typeParam);

    // Compute next value
    switch (rawType->nativeType)
    {
    case NativeTypeKind::U8:
        if (enumNode->computedValue.reg.u8 == UINT8_MAX)
            return context->errorContext.report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u8'", valNode->name.c_str())});
        enumNode->computedValue.reg.u8++;
        break;
    case NativeTypeKind::U16:
        if (enumNode->computedValue.reg.u16 == UINT16_MAX)
            return context->errorContext.report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u16'", valNode->name.c_str())});
        enumNode->computedValue.reg.u16++;
        break;
    case NativeTypeKind::U32:
        if (enumNode->computedValue.reg.u32 == UINT32_MAX)
            return context->errorContext.report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u32'", valNode->name.c_str())});
        enumNode->computedValue.reg.u32++;
        break;
    case NativeTypeKind::U64:
        if (enumNode->computedValue.reg.u64 == UINT64_MAX)
            return context->errorContext.report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u64'", valNode->name.c_str())});
        enumNode->computedValue.reg.u64++;
        break;
    case NativeTypeKind::S8:
        if (enumNode->computedValue.reg.s8 <= INT8_MIN || enumNode->computedValue.reg.s8 >= INT8_MAX)
            return context->errorContext.report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's8'", valNode->name.c_str())});
        enumNode->computedValue.reg.s8++;
        break;
    case NativeTypeKind::S16:
        if (enumNode->computedValue.reg.s16 <= INT16_MIN || enumNode->computedValue.reg.s16 >= INT16_MAX)
            return context->errorContext.report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's16'", valNode->name.c_str())});
        enumNode->computedValue.reg.s16++;
        break;
    case NativeTypeKind::S32:
        if (enumNode->computedValue.reg.s32 <= INT32_MIN || enumNode->computedValue.reg.s32 >= INT32_MAX)
            return context->errorContext.report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's32'", valNode->name.c_str())});
        enumNode->computedValue.reg.s32++;
        break;
    case NativeTypeKind::S64:
        if (enumNode->computedValue.reg.s64 <= INT64_MIN || enumNode->computedValue.reg.s64 >= INT64_MAX)
            return context->errorContext.report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's64'", valNode->name.c_str())});
        enumNode->computedValue.reg.s64++;
        break;
    }

    return true;
}