#include "pch.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "Global.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "LanguageSpec.h"
#include "Scope.h"

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node         = context->node;
    node->byteCodeFct = &ByteCodeGenJob::emitLiteral;
    node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    node->typeInfo           = node->token.literalType;
    node->computedValue.reg  = node->token.literalValue;
    node->computedValue.text = node->token.text;
    return true;
}

bool SemanticJob::resolveExpressionList(SemanticContext* context)
{
    auto node      = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);
    auto typeInfo  = g_Pool_typeInfoExpressionList.alloc();
    typeInfo->name = "<type list>";

    node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    for (auto child : node->childs)
    {
        typeInfo->childs.push_back(child->typeInfo);
        typeInfo->sizeOf += child->typeInfo->sizeOf;
        if (!(child->flags & AST_VALUE_COMPUTED))
            node->flags &= ~AST_VALUE_COMPUTED;
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
    }

    node->byteCodeFct = &ByteCodeGenJob::emitExpressionList;
    node->typeInfo    = g_TypeMgr.registerType(typeInfo);

    // Reserve
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct)
    {
        node->storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += node->typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

    return true;
}

bool SemanticJob::resolveCountProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    if (typeInfo->isNative(NativeType::String))
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
    else
    {
        return false;
    }

    node->typeInfo = g_TypeMgr.typeInfoU32;
    return true;
}

bool SemanticJob::resolveIntrinsicProperty(SemanticContext* context)
{
    auto node       = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto sourceFile = context->sourceFile;
    auto expr       = node->expression;

    switch (node->prop)
    {
    case Property::SizeOf:
        SWAG_VERIFY(expr->typeInfo, sourceFile->report({sourceFile, expr, "expression cannot be evaluated at compile time"}));
        node->computedValue.reg.u64 = expr->typeInfo->sizeOf;
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        node->typeInfo = g_TypeMgr.typeInfoU32;
        break;

    case Property::Count:
        node->inheritComputedValue(node->expression);
        if (!resolveCountProperty(context, node, node->expression->typeInfo))
            return sourceFile->report({sourceFile, node->expression, format("'count' property cannot be applied to expression of type '%s'", node->expression->typeInfo->name.c_str())});
        break;

    case Property::Data:
        if (expr->typeInfo->isNative(NativeType::String))
        {
            auto ptrType         = g_Pool_typeInfoPointer.alloc();
            ptrType->ptrCount    = 1;
            ptrType->pointedType = g_TypeMgr.typeInfoU8;
            ptrType->sizeOf      = sizeof(void*);
            ptrType->name        = "*u8";
            node->typeInfo       = g_TypeMgr.registerType(ptrType);
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
            node->typeInfo       = g_TypeMgr.registerType(ptrType);
            node->byteCodeFct    = &ByteCodeGenJob::emitDataProperty;
        }
        else
        {
            return sourceFile->report({sourceFile, expr, format("'data' property cannot be applied to expression of type '%s'", expr->typeInfo->name.c_str())});
        }

        break;
    }

    return true;
}

bool SemanticJob::resolveMakePointer(SemanticContext* context)
{
    auto node       = context->node;
    auto child      = node->childs.front();
    auto typeInfo   = child->typeInfo;
    auto sourceFile = context->sourceFile;

    SWAG_VERIFY(child->flags & AST_L_VALUE, sourceFile->report({sourceFile, child, "invalid address expression"}));

    if (child->kind == AstNodeKind::IdentifierRef || child->kind == AstNodeKind::ArrayPointerRef)
    {
        node->byteCodeFct = &ByteCodeGenJob::emitMakePointer;

        auto ptrType      = g_Pool_typeInfoPointer.alloc();
        ptrType->ptrCount = 1;

        // If this is an array, then this is legit, the pointer will pointer to the first
        // element : need to find it's type
        while (typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            typeInfo       = typeArray->pointedType;
        }

        ptrType->pointedType = typeInfo;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*" + typeInfo->name;
        node->typeInfo       = g_TypeMgr.registerType(ptrType);
    }
    else
    {
        return sourceFile->report({sourceFile, child, "invalid address expression"});
    }

    return true;
}

bool SemanticJob::resolveArrayOrPointerRef(SemanticContext* context)
{
    auto arrayNode                    = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerRef);
    arrayNode->resolvedSymbolName     = arrayNode->array->resolvedSymbolName;
    arrayNode->resolvedSymbolOverload = arrayNode->array->resolvedSymbolOverload;
    arrayNode->inheritAndFlag(arrayNode->array, AST_L_VALUE);

    auto arrayType = arrayNode->array->typeInfo;
    switch (arrayType->kind)
    {
    case TypeInfoKind::Pointer:
    {
        auto typePtr           = CastTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = &ByteCodeGenJob::emitPointerRef;
        break;
    }
    case TypeInfoKind::Array:
    {
        auto typePtr           = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = &ByteCodeGenJob::emitArrayRef;
        arrayNode->inheritAndFlag(arrayNode->array, AST_L_VALUE);
        break;
    }
    case TypeInfoKind::Slice:
    {
        auto typePtr           = CastTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = &ByteCodeGenJob::emitSliceRef;
        arrayNode->inheritAndFlag(arrayNode->array, AST_L_VALUE);
        break;
    }
    default:
    {
        auto sourceFile = context->sourceFile;
        return sourceFile->report({sourceFile, arrayNode->array, format("cannot dereference type '%s'", arrayType->name.c_str())});
    }
    }

    return true;
}

bool SemanticJob::resolveArrayPointerDeRef(SemanticContext* context)
{
    auto sourceFile        = context->sourceFile;
    auto arrayNode         = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerDeRef);
    auto arrayType         = arrayNode->array->typeInfo;
    arrayNode->byteCodeFct = &ByteCodeGenJob::emitPointerDeRef;

    if (arrayType->kind == TypeInfoKind::Native && arrayType->nativeType == NativeType::String)
    {
        arrayNode->typeInfo = g_TypeMgr.typeInfoU8;
    }
    else if (arrayType->kind == TypeInfoKind::Pointer)
    {
        auto typePtr = static_cast<TypeInfoPointer*>(arrayType);
        if (typePtr->ptrCount == 1)
        {
            arrayNode->typeInfo = typePtr->pointedType;
        }
        else
        {
            auto newType        = g_Pool_typeInfoPointer.alloc();
            newType->name       = typePtr->name;
            newType->ptrCount   = typePtr->ptrCount - 1;
            newType->sizeOf     = typePtr->sizeOf;
            arrayNode->typeInfo = g_TypeMgr.registerType(newType);
        }
    }
    else if (arrayType->kind == TypeInfoKind::Array)
    {
        auto typePtr        = static_cast<TypeInfoArray*>(arrayType);
        arrayNode->typeInfo = typePtr->pointedType;
    }
    else if (arrayType->kind == TypeInfoKind::Slice)
    {
        auto typePtr        = static_cast<TypeInfoSlice*>(arrayType);
        arrayNode->typeInfo = typePtr->pointedType;
    }
    else
    {
        return sourceFile->report({sourceFile, arrayNode->array, format("type '%s' cannot be referenced like a pointer", arrayType->name.c_str())});
    }

    return true;
}
