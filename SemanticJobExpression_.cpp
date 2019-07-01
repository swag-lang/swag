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
    auto node      = context->node;
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
    node->typeInfo = g_TypeMgr.registerType(typeInfo);
    return true;
}

bool SemanticJob::resolveIntrinsicProp(SemanticContext* context)
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
        if (expr->typeInfo->isNative(NativeType::String))
        {
            if (expr->flags & AST_VALUE_COMPUTED)
            {
                node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
                node->computedValue.reg.u64 = expr->computedValue.text.length();
            }
            else
            {
                node->byteCodeFct = &ByteCodeGenJob::emitCountProperty;
            }
        }
        else
        {
            return sourceFile->report({sourceFile, expr, "'count' property can't be applied to expression"});
        }

        node->typeInfo = g_TypeMgr.typeInfoU32;
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
        else
        {
            return sourceFile->report({sourceFile, expr, "'data' property can't be applied to expression"});
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

    SWAG_VERIFY(child->kind == AstNodeKind::IdentifierRef, sourceFile->report({sourceFile, child, "invalid address expression"}));
    node->byteCodeFct = &ByteCodeGenJob::emitMakePointer;

    auto ptrType         = g_Pool_typeInfoPointer.alloc();
    ptrType->ptrCount    = 1;
    ptrType->pointedType = typeInfo;
    ptrType->sizeOf      = sizeof(void*);
    ptrType->name        = "*" + typeInfo->name;
    node->typeInfo       = g_TypeMgr.registerType(ptrType);

    return true;
}

bool SemanticJob::resolveArrayOrPointerRef(SemanticContext* context)
{
    auto arrayNode                    = CastAst<AstPointerDeRef>(context->node, AstNodeKind::PointerDeRef);
    arrayNode->resolvedSymbolName     = arrayNode->array->resolvedSymbolName;
    arrayNode->resolvedSymbolOverload = arrayNode->array->resolvedSymbolOverload;
    if (arrayNode->array->typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typePtr           = CastTypeInfo<TypeInfoPointer>(arrayNode->array->typeInfo, TypeInfoKind::Pointer);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = &ByteCodeGenJob::emitPointerRef;
    }
    else
    {
        auto typePtr           = CastTypeInfo<TypeInfoArray>(arrayNode->array->typeInfo, TypeInfoKind::Array);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = &ByteCodeGenJob::emitArrayRef;
    }

    return true;
}

bool SemanticJob::resolveArrayPointerDeRef(SemanticContext* context)
{
    auto sourceFile        = context->sourceFile;
    auto arrayNode         = CastAst<AstPointerDeRef>(context->node, AstNodeKind::PointerDeRef);
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
    else
    {
        return sourceFile->report({sourceFile, arrayNode->array, format("type '%s' can't be referenced like a pointer", arrayType->name.c_str())});
    }

    return true;
}