#include "pch.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "SymTable.h"

bool SemanticJob::resolveMakePointer(SemanticContext* context)
{
    auto node       = context->node;
    auto child      = node->childs.front();
    auto typeInfo   = child->typeInfo;
    auto sourceFile = context->sourceFile;

    SWAG_VERIFY(child->flags & AST_L_VALUE, sourceFile->report({sourceFile, child, "cannot take address of expression"}));
    if (child->kind != AstNodeKind::IdentifierRef && child->kind != AstNodeKind::ArrayPointerIndex)
        return sourceFile->report({sourceFile, child, "invalid address expression"});

    // Lambda
    if (child->resolvedSymbolName->kind == SymbolKind::Function)
    {
        auto lambdaType    = child->typeInfo->clone();
        lambdaType->kind   = TypeInfoKind::Lambda;
        lambdaType->sizeOf = sizeof(void*);
        node->typeInfo     = g_TypeMgr.registerType(lambdaType);
        node->byteCodeFct  = &ByteCodeGenJob::emitMakeLambda;
    }

    // Expression
    else
    {
        node->byteCodeFct = &ByteCodeGenJob::emitMakePointer;

        auto ptrType      = g_Pool_typeInfoPointer.alloc();
        ptrType->ptrCount = 1;

        // If this is an array, then this is legit, the pointer will address the first
        // element : need to find it's type
        while (typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            typeInfo       = typeArray->pointedType;
        }

        ptrType->pointedType = typeInfo;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*" + typeInfo->name;

        // Type is constant if we take address of a readonly variable
        if (child->resolvedSymbolOverload && child->resolvedSymbolOverload->flags & OVERLOAD_CONST)
            ptrType->setConst();

        node->typeInfo = g_TypeMgr.registerType(ptrType);
    }

    return true;
}

bool SemanticJob::resolveArrayPointerIndex(SemanticContext* context)
{
    auto node = context->node;

    if (node->flags & AST_TAKE_ADDRESS)
    {
        SWAG_CHECK(resolveArrayPointerRef(context));
    }
    else
    {
        SWAG_CHECK(resolveArrayPointerDeRef(context));
    }

    // If this is not the last child of the IdentifierRef, then this is a reference, and
    // we must take the address and no dereference that identifier
    if (node->parent->kind == AstNodeKind::IdentifierRef)
    {
        auto parent = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);
        if (node != parent->childs.back())
            node->flags |= AST_TAKE_ADDRESS;
		parent->previousResolvedNode = node;
    }

    return true;
}

bool SemanticJob::resolveArrayPointerRef(SemanticContext* context)
{
    auto arrayNode                    = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    arrayNode->resolvedSymbolName     = arrayNode->array->resolvedSymbolName;
    arrayNode->resolvedSymbolOverload = arrayNode->array->resolvedSymbolOverload;
    arrayNode->inheritAndFlag(arrayNode->array, AST_L_VALUE);

    auto arrayType  = arrayNode->array->typeInfo;
    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(!arrayType->isConst(), sourceFile->report({sourceFile, arrayNode->array, format("type '%s' is constant and cannot be referenced", arrayType->name.c_str())}));

    if (!(arrayNode->access->typeInfo->flags & TYPEINFO_INTEGER))
        return sourceFile->report({sourceFile, arrayNode->array, format("access type should be integer, not '%s'", arrayNode->access->typeInfo->name.c_str())});

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
        break;
    }
    case TypeInfoKind::Slice:
    {
        auto typePtr           = CastTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = &ByteCodeGenJob::emitSliceRef;
        break;
    }
    default:
    {
        return sourceFile->report({sourceFile, arrayNode->array, format("cannot dereference type '%s'", arrayType->name.c_str())});
    }
    }

    return true;
}

bool SemanticJob::resolveArrayPointerDeRef(SemanticContext* context)
{
    auto sourceFile        = context->sourceFile;
    auto arrayNode         = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    auto arrayType         = g_TypeMgr.concreteType(arrayNode->array->typeInfo);
    arrayNode->byteCodeFct = &ByteCodeGenJob::emitPointerDeRef;

    if (!(arrayNode->access->typeInfo->flags & TYPEINFO_INTEGER))
        return sourceFile->report({sourceFile, arrayNode->array, format("access type should be integer, not '%s'", arrayNode->access->typeInfo->name.c_str())});

    switch (arrayType->kind)
    {
    case TypeInfoKind::Native:
        if (arrayType->nativeType == NativeType::String)
            arrayNode->typeInfo = g_TypeMgr.typeInfoU8;
        break;

    case TypeInfoKind::Pointer:
    {
        auto typePtr = static_cast<TypeInfoPointer*>(arrayType);
        if (typePtr->ptrCount == 1)
        {
            arrayNode->typeInfo = typePtr->pointedType;
        }
        else
        {
            auto newType = static_cast<TypeInfoPointer*>(typePtr->clone());
            newType->ptrCount--;
            arrayNode->typeInfo = g_TypeMgr.registerType(newType);
        }
        break;
    }

    case TypeInfoKind::Array:
    {
        auto typePtr        = static_cast<TypeInfoArray*>(arrayType);
        arrayNode->typeInfo = typePtr->pointedType;
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typePtr        = static_cast<TypeInfoSlice*>(arrayType);
        arrayNode->typeInfo = typePtr->pointedType;
        break;
    }

    case TypeInfoKind::Variadic:
        arrayNode->typeInfo = g_TypeMgr.typeInfoVariadicValue;
        break;

    default:
        return sourceFile->report({sourceFile, arrayNode->array, format("%s type '%s' cannot be referenced like a pointer", TypeInfo::getNakedKindName(arrayType), arrayType->name.c_str())});
    }

    return true;
}
