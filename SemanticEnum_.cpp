#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Scope.h"
#include "TypeManager.h"
#include "ModuleSemanticJob.h"
#include "Diagnostic.h"

bool SemanticJob::resolveEnumType(SemanticContext* context)
{
    auto typeNode = context->node;
    auto enumNode = context->node->parent;

    // Enum raw type
    auto rawTypeInfo = typeNode->childs.empty() ? &g_TypeInfoS32 : typeNode->childs[0]->typeInfo;

    // Register symbol with its type
    auto typeInfo     = static_cast<TypeInfoEnum*>(enumNode->typeInfo);
    typeInfo->rawType = rawTypeInfo;
    SWAG_CHECK(enumNode->scope->symTable->addSymbolTypeInfo(context->sourceFile, enumNode->token, enumNode->name, enumNode->typeInfo, SymbolKind::Enum));

    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveEnumValue(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto valNode    = context->node;
    auto enumNode   = valNode->parent;
    auto typeEnum   = static_cast<TypeInfoEnum*>(enumNode->typeInfo);

    SWAG_CHECK(typeEnum->scope->symTable->addSymbolTypeInfo(context->sourceFile, valNode->token, valNode->name, typeEnum, SymbolKind::EnumValue, &enumNode->computedValue));

    auto rawType = static_cast<TypeInfoNamespace*>(typeEnum->rawType);
    switch (rawType->nativeType)
    {
    case NativeType::U8:
        if (enumNode->computedValue.variant.u8 == UINT8_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u8'", valNode->name.c_str())});
        enumNode->computedValue.variant.u8++;
        break;
    case NativeType::U16:
        if (enumNode->computedValue.variant.u16 == UINT16_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u16'", valNode->name.c_str())});
        enumNode->computedValue.variant.u16++;
        break;
    case NativeType::U32:
        if (enumNode->computedValue.variant.u32 == UINT32_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u32'", valNode->name.c_str())});
        enumNode->computedValue.variant.u32++;
        break;
    case NativeType::U64:
        if (enumNode->computedValue.variant.u64 == UINT64_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u64'", valNode->name.c_str())});
        enumNode->computedValue.variant.u64++;
        break;
    case NativeType::S8:
        if (enumNode->computedValue.variant.s8 <= INT8_MIN || enumNode->computedValue.variant.s8 >= INT8_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's8'", valNode->name.c_str())});
        enumNode->computedValue.variant.s8++;
        break;
    case NativeType::S16:
        if (enumNode->computedValue.variant.s16 <= INT16_MIN || enumNode->computedValue.variant.s16 >= INT16_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's16'", valNode->name.c_str())});
        enumNode->computedValue.variant.s16++;
        break;
    case NativeType::S32:
        if (enumNode->computedValue.variant.s32 <= INT32_MIN || enumNode->computedValue.variant.s32 >= INT32_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's32'", valNode->name.c_str())});
        enumNode->computedValue.variant.s32++;
        break;
    case NativeType::S64:
        if (enumNode->computedValue.variant.s64 <= INT64_MIN || enumNode->computedValue.variant.s64 >= INT64_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's64'", valNode->name.c_str())});
        enumNode->computedValue.variant.s64++;
        break;
    }

    context->result = SemanticResult::Done;
    return true;
}