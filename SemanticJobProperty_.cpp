#include "pch.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "TypeManager.h"
#include "Workspace.h"

bool SemanticJob::resolveCountProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
            node->computedValue.reg.u64 = node->computedValue.text.length();
			node->typeInfo = g_TypeMgr.typeInfoU32;
        }
        else
        {
            node->byteCodeFct = &ByteCodeGenJob::emitCountOfProperty;
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
        node->byteCodeFct = &ByteCodeGenJob::emitCountOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::TypeList)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeList);
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        node->computedValue.reg.u64 = (uint32_t) typeList->childs.size();
		node->typeInfo = g_TypeMgr.typeInfoU32;
    }
    else if (typeInfo->kind == TypeInfoKind::Variadic || typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        node->byteCodeFct = &ByteCodeGenJob::emitCountOfProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opCount", nullptr, node, nullptr));
        if (context->result == SemanticResult::Pending)
            return true;
		node->typeInfo = g_TypeMgr.typeInfoU32;
    }
    else
    {
        return false;
    }

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
        SWAG_VERIFY(expr->typeInfo, context->errorContext.report({sourceFile, expr, "expression cannot be evaluated at compile time"}));
        node->computedValue.reg.u64 = expr->typeInfo->sizeOf;
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        node->typeInfo = g_TypeMgr.typeInfoU32;
        break;

    case Property::TypeOf:
        SWAG_VERIFY(expr->typeInfo, context->errorContext.report({sourceFile, expr, "expression cannot be evaluated at compile time"}));
        expr->flags |= AST_NO_BYTECODE;
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, expr->typeInfo, &node->typeInfo, &node->computedValue.reg.u32));
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_VALUE_IS_TYPEINFO;
        return true;

    case Property::KindOf:
        SWAG_VERIFY(expr->typeInfo, context->errorContext.report({sourceFile, expr, "expression cannot be evaluated at compile time"}));
        SWAG_VERIFY(expr->typeInfo->isNative(NativeTypeKind::Any), context->errorContext.report({sourceFile, expr, "expression is not of type 'any'"}));
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, expr->typeInfo, &node->typeInfo, &node->computedValue.reg.u32));
        node->byteCodeFct = &ByteCodeGenJob::emitKindOfProperty;
        return true;

    case Property::CountOf:
        SWAG_CHECK(checkIsConcrete(context, expr));
        node->inheritComputedValue(expr);
        if (!resolveCountProperty(context, node, expr->typeInfo))
            return context->errorContext.report({sourceFile, node->expression, format("'count' property cannot be applied to expression of type '%s'", node->expression->typeInfo->name.c_str())});
        node->typeInfo = g_TypeMgr.typeInfoU32;
        break;

    case Property::DataOf:
        SWAG_CHECK(checkIsConcrete(context, expr));
        if (expr->typeInfo->isNative(NativeTypeKind::String))
        {
            auto ptrType         = g_Pool_typeInfoPointer.alloc();
            ptrType->ptrCount    = 1;
            ptrType->finalType = g_TypeMgr.typeInfoU8;
            ptrType->sizeOf      = sizeof(void*);
            ptrType->name        = "*u8";
            node->typeInfo       = typeTable.registerType(ptrType);
            node->byteCodeFct    = &ByteCodeGenJob::emitDataOfProperty;
        }
        else if (expr->typeInfo->kind == TypeInfoKind::Slice)
        {
            auto ptrSlice        = CastTypeInfo<TypeInfoSlice>(expr->typeInfo, TypeInfoKind::Slice);
            auto ptrType         = g_Pool_typeInfoPointer.alloc();
            ptrType->ptrCount    = 1;
            ptrType->finalType = ptrSlice->pointedType;
            ptrType->sizeOf      = sizeof(void*);
            ptrType->name        = "*" + ptrSlice->pointedType->name;
            node->typeInfo       = typeTable.registerType(ptrType);
            node->byteCodeFct    = &ByteCodeGenJob::emitDataOfProperty;
        }
        else if (expr->typeInfo->kind == TypeInfoKind::Array)
        {
            auto ptrArray        = CastTypeInfo<TypeInfoArray>(expr->typeInfo, TypeInfoKind::Array);
            auto ptrType         = g_Pool_typeInfoPointer.alloc();
            ptrType->ptrCount    = 1;
            ptrType->finalType = ptrArray->pointedType;
            ptrType->sizeOf      = sizeof(void*);
            ptrType->name        = "*" + ptrArray->pointedType->name;
            node->typeInfo       = typeTable.registerType(ptrType);
            node->byteCodeFct    = &ByteCodeGenJob::emitDataOfProperty;
        }
        else if (expr->typeInfo->isNative(NativeTypeKind::Any))
        {
            auto ptrType         = g_Pool_typeInfoPointer.alloc();
            ptrType->ptrCount    = 1;
            ptrType->finalType = g_TypeMgr.typeInfoVoid;
            ptrType->sizeOf      = sizeof(void*);
            ptrType->name        = "*" + g_TypeMgr.typeInfoVoid->name;
            node->typeInfo       = typeTable.registerType(ptrType);
            node->byteCodeFct    = &ByteCodeGenJob::emitDataOfProperty;
        }
        else
        {
            return context->errorContext.report({sourceFile, expr, format("'data' property cannot be applied to expression of type '%s'", expr->typeInfo->name.c_str())});
        }

        break;
    }

    return true;
}
