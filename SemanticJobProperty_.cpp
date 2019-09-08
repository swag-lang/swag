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
        }
        else
        {
            node->byteCodeFct = &ByteCodeGenJob::emitCountProperty;
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        auto typeArray              = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->computedValue.reg.u64 = typeArray->count;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        node->byteCodeFct = &ByteCodeGenJob::emitCountProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::TypeList)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeList);
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        node->computedValue.reg.u64 = (uint32_t) typeList->childs.size();
    }
    else if (typeInfo->kind == TypeInfoKind::Variadic)
    {
        node->byteCodeFct = &ByteCodeGenJob::emitCountProperty;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opCount", nullptr, node, nullptr));
        if (context->result == SemanticResult::Pending)
            return true;
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
        SWAG_VERIFY(expr->typeInfo, context->errorContext.report({sourceFile, expr, "expression cannot be evaluated at compile time"}));
        node->computedValue.reg.u64 = expr->typeInfo->sizeOf;
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        node->typeInfo = g_TypeMgr.typeInfoU32;
        break;

    case Property::TypeOf:
    {
        SWAG_VERIFY(expr->typeInfo, context->errorContext.report({sourceFile, expr, "expression cannot be evaluated at compile time"}));
        auto&       swagScope = sourceFile->module->workspace->swagScope;
        scoped_lock lock(swagScope.mutex);
        if (!swagScope.fullySolved)
        {
            swagScope.dependentJobs.push_back(context->job);
            context->result = SemanticResult::Pending;
            return true;
        }

        typeTable.makeConcreteTypeInfo(context, expr->typeInfo, &node->typeInfo, &node->computedValue.reg.u32);
        node->byteCodeFct = &ByteCodeGenJob::emitTypeOfProperty;
        return true;
    }

    case Property::CountOf:
        node->inheritComputedValue(node->expression);
        if (!resolveCountProperty(context, node, node->expression->typeInfo))
            return context->errorContext.report({sourceFile, node->expression, format("'count' property cannot be applied to expression of type '%s'", node->expression->typeInfo->name.c_str())});
        break;

    case Property::DataOf:
        if (expr->typeInfo->isNative(NativeTypeKind::String))
        {
            auto ptrType         = g_Pool_typeInfoPointer.alloc();
            ptrType->ptrCount    = 1;
            ptrType->pointedType = g_TypeMgr.typeInfoU8;
            ptrType->sizeOf      = sizeof(void*);
            ptrType->name        = "*u8";
            node->typeInfo       = typeTable.registerType(ptrType);
            node->byteCodeFct    = &ByteCodeGenJob::emitDataProperty;
        }
        else if (expr->typeInfo->kind == TypeInfoKind::Slice)
        {
            auto ptrSlice        = CastTypeInfo<TypeInfoSlice>(expr->typeInfo, TypeInfoKind::Slice);
            auto ptrType         = g_Pool_typeInfoPointer.alloc();
            ptrType->ptrCount    = 1;
            ptrType->pointedType = ptrSlice->pointedType;
            ptrType->sizeOf      = sizeof(void*);
            ptrType->name        = "*" + ptrSlice->pointedType->name;
            node->typeInfo       = typeTable.registerType(ptrType);
            node->byteCodeFct    = &ByteCodeGenJob::emitDataProperty;
        }
        else if (expr->typeInfo->kind == TypeInfoKind::Array)
        {
            auto ptrArray        = CastTypeInfo<TypeInfoArray>(expr->typeInfo, TypeInfoKind::Array);
            auto ptrType         = g_Pool_typeInfoPointer.alloc();
            ptrType->ptrCount    = 1;
            ptrType->pointedType = ptrArray->pointedType;
            ptrType->sizeOf      = sizeof(void*);
            ptrType->name        = "*" + ptrArray->pointedType->name;
            node->typeInfo       = typeTable.registerType(ptrType);
            node->byteCodeFct    = &ByteCodeGenJob::emitDataProperty;
        }
        else
        {
            return context->errorContext.report({sourceFile, expr, format("'data' property cannot be applied to expression of type '%s'", expr->typeInfo->name.c_str())});
        }

        break;
    }

    return true;
}
