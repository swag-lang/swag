#include "pch.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "TypeManager.h"
#include "Workspace.h"

bool SemanticJob::resolveDataOfProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = g_TypeMgr.typeInfoU8;
        ptrType->pointedType = g_TypeMgr.typeInfoU8;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*u8";
        ptrType->setConst();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        auto ptrSlice        = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = ptrSlice->pointedType;
        ptrType->pointedType = ptrSlice->pointedType;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*" + ptrSlice->pointedType->name;
        if (ptrSlice->isConst())
            ptrType->setConst();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto ptrArray        = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = ptrArray->pointedType;
        ptrType->pointedType = ptrArray->pointedType;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*" + ptrArray->pointedType->name;
        if (ptrArray->isConst())
            ptrType->setConst();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
    }
    else if (typeInfo->isNative(NativeTypeKind::Any))
    {
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = g_TypeMgr.typeInfoVoid;
        ptrType->pointedType = g_TypeMgr.typeInfoVoid;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*" + g_TypeMgr.typeInfoVoid->name;
        node->typeInfo       = ptrType;
        node->byteCodeFct    = ByteCodeGenJob::emitDataOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, "opData", nullptr, nullptr, node, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = g_TypeMgr.typeInfoPVoid;
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
    }
    else
    {
        return false;
    }

    return true;
}

bool SemanticJob::resolveCountOfProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
            node->computedValue.reg.u64 = node->computedValue.text.length();
            node->typeInfo              = g_TypeMgr.typeInfoU32;
        }
        else
        {
            node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        auto typeArray              = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->computedValue.reg.u64 = typeArray->count;
        node->typeInfo              = g_TypeMgr.typeInfoU32;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::TypeList)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeList);
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        node->computedValue.reg.u64 = (uint32_t) typeList->childs.size();
        node->typeInfo              = g_TypeMgr.typeInfoU32;
    }
    else if (typeInfo->kind == TypeInfoKind::Variadic || typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, "opCount", nullptr, nullptr, node, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = g_TypeMgr.typeInfoU32;
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
    }
    else
    {
        return false;
    }

    node->typeInfo = g_TypeMgr.typeInfoU32;
    return true;
}

bool SemanticJob::resolveIntrinsicProperty(SemanticContext* context)
{
    auto  node       = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;
    auto  expr       = node->expression;

    switch (node->prop)
    {
    case Property::SizeOf:
        SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
        node->computedValue.reg.u64 = expr->typeInfo->sizeOf;
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        node->typeInfo = g_TypeMgr.typeInfoU32;
        break;

    case Property::TypeOf:
        SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
        expr->flags |= AST_NO_BYTECODE;
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, expr->typeInfo, &node->typeInfo, &node->computedValue.reg.u32));
        typeTable.waitForTypeTableJobs(context->job);
        if (context->result != ContextResult::Done)
            return true;
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_VALUE_IS_TYPEINFO;
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;

    case Property::KindOf:
        SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
        SWAG_VERIFY(expr->typeInfo->isNative(NativeTypeKind::Any), context->report({expr, "expression is not of type 'any'"}));
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, expr->typeInfo, &node->typeInfo, &node->computedValue.reg.u32));
        typeTable.waitForTypeTableJobs(context->job);
        if (context->result != ContextResult::Done)
            return true;
        node->byteCodeFct = ByteCodeGenJob::emitKindOfProperty;
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;

    case Property::CountOf:
        if (expr->typeInfo->kind == TypeInfoKind::Enum)
        {
            auto typeEnum               = CastTypeInfo<TypeInfoEnum>(expr->typeInfo, TypeInfoKind::Enum);
            node->computedValue.reg.u32 = (uint32_t) typeEnum->values.size();
            node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
            node->typeInfo = g_TypeMgr.typeInfoU32;
            break;
        }

        SWAG_CHECK(checkIsConcrete(context, expr));
        node->inheritComputedValue(expr);
        if (!resolveCountOfProperty(context, node, expr->typeInfo))
            return context->report({node->expression, format("'countof' property cannot be applied to expression of type '%s'", node->expression->typeInfo->name.c_str())});
        break;

    case Property::DataOf:
        SWAG_CHECK(checkIsConcrete(context, expr));
        if (!resolveDataOfProperty(context, node, expr->typeInfo))
            return context->report({expr, format("'dataof' property cannot be applied to expression of type '%s'", expr->typeInfo->name.c_str())});
        break;
    }

    return true;
}
