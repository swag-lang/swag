#include "pch.h"
#include "SemanticJob.h"
#include "Global.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "Scope.h"

bool SemanticJob::resolveEnumType(SemanticContext* context)
{
    auto typeNode = context->node;
    auto enumNode = context->node->parent;

    // Enum raw type
    auto rawTypeInfo = typeNode->childs.empty() ? g_TypeMgr.typeInfoS32 : typeNode->childs[0]->typeInfo;

    // Register symbol with its type
    auto typeInfo     = CastTypeInfo<TypeInfoEnum>(enumNode->typeInfo, TypeInfoKind::Enum);
    typeInfo->rawType = rawTypeInfo;
    SWAG_CHECK(enumNode->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, enumNode, typeInfo, SymbolKind::Enum));

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
        SWAG_VERIFY(assignNode->flags & AST_VALUE_COMPUTED, sourceFile->report({sourceFile, valNode->token, "expression cannot be evaluated at compile time"}));
        SWAG_CHECK(TypeManager::makeCompatibles(sourceFile, rawType, assignNode, CASTFLAG_FLATTEN));
        enumNode->computedValue = assignNode->computedValue;
    }
    else
    {
        switch (rawType->nativeType)
        {
        case NativeType::Bool:
        case NativeType::Char:
        case NativeType::String:
        case NativeType::F32:
        case NativeType::F64:
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' of type '%s' must be initialized", valNode->name.c_str(), rawType->name.c_str())});
        }
    }

    SWAG_CHECK(typeEnum->scope->symTable->addSymbolTypeInfo(context->sourceFile, valNode, typeEnum, SymbolKind::EnumValue, &enumNode->computedValue));

    // Compute next value
    switch (rawType->nativeType)
    {
    case NativeType::U8:
        if (enumNode->computedValue.reg.u8 == UINT8_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u8'", valNode->name.c_str())});
        enumNode->computedValue.reg.u8++;
        break;
    case NativeType::U16:
        if (enumNode->computedValue.reg.u16 == UINT16_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u16'", valNode->name.c_str())});
        enumNode->computedValue.reg.u16++;
        break;
    case NativeType::U32:
        if (enumNode->computedValue.reg.u32 == UINT32_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u32'", valNode->name.c_str())});
        enumNode->computedValue.reg.u32++;
        break;
    case NativeType::U64:
        if (enumNode->computedValue.reg.u64 == UINT64_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 'u64'", valNode->name.c_str())});
        enumNode->computedValue.reg.u64++;
        break;
    case NativeType::S8:
        if (enumNode->computedValue.reg.s8 <= INT8_MIN || enumNode->computedValue.reg.s8 >= INT8_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's8'", valNode->name.c_str())});
        enumNode->computedValue.reg.s8++;
        break;
    case NativeType::S16:
        if (enumNode->computedValue.reg.s16 <= INT16_MIN || enumNode->computedValue.reg.s16 >= INT16_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's16'", valNode->name.c_str())});
        enumNode->computedValue.reg.s16++;
        break;
    case NativeType::S32:
        if (enumNode->computedValue.reg.s32 <= INT32_MIN || enumNode->computedValue.reg.s32 >= INT32_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's32'", valNode->name.c_str())});
        enumNode->computedValue.reg.s32++;
        break;
    case NativeType::S64:
        if (enumNode->computedValue.reg.s64 <= INT64_MIN || enumNode->computedValue.reg.s64 >= INT64_MAX)
            return sourceFile->report({sourceFile, valNode->token, format("enum value '%s' is out of range of 's64'", valNode->name.c_str())});
        enumNode->computedValue.reg.s64++;
        break;
    }

    return true;
}