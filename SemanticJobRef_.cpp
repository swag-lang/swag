#include "pch.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "SymTable.h"
#include "Module.h"
#include "TypeManager.h"

bool SemanticJob::resolveMakePointer(SemanticContext* context)
{
    auto node     = context->node;
    auto child    = node->childs.front();
    auto typeInfo = child->typeInfo;

    SWAG_VERIFY(child->flags & AST_L_VALUE, context->report({child, "cannot take address of expression"}));
    if (child->kind != AstNodeKind::IdentifierRef && child->kind != AstNodeKind::ArrayPointerIndex)
        return context->report({child, "invalid address expression"});

    SWAG_CHECK(checkIsConcrete(context, child));
    node->flags |= AST_R_VALUE;

    // Lambda
    if (child->resolvedSymbolName->kind == SymbolKind::Function)
    {
		SWAG_VERIFY(!(child->resolvedSymbolOverload->node->attributeFlags & ATTRIBUTE_INLINE), context->report({ child, "cannot take address of an inline function" }));

        auto lambdaType    = child->typeInfo->clone();
        lambdaType->kind   = TypeInfoKind::Lambda;
        lambdaType->sizeOf = sizeof(void*);
        node->typeInfo     = lambdaType;
        node->byteCodeFct  = ByteCodeGenJob::emitMakeLambda;
    }

    // Expression
    else
    {
        node->byteCodeFct = ByteCodeGenJob::emitMakePointer;

        auto ptrType      = g_Pool_typeInfoPointer.alloc();
        ptrType->ptrCount = 1;

        // If this is an array, then this is legit, the pointer will address the first
        // element : need to find it's type
        while (typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            typeInfo       = typeArray->pointedType;
        }

        ptrType->finalType   = typeInfo;
        ptrType->pointedType = typeInfo;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*" + typeInfo->name;

        // Type is constant if we take address of a readonly variable
        if (child->resolvedSymbolOverload && child->resolvedSymbolOverload->flags & OVERLOAD_CONST_ASSIGN)
            ptrType->setConst();

        node->typeInfo = ptrType;
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

    if (context->result == ContextResult::Pending)
        return true;

    // If this is not the last child of the IdentifierRef, then this is a reference, and
    // we must take the address and not dereference that identifier
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
    arrayNode->inheritOrFlag(arrayNode->array, AST_L_VALUE);

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));
    arrayNode->flags |= AST_R_VALUE;

    auto arrayType = arrayNode->array->typeInfo;
    SWAG_VERIFY(!arrayType->isConst(), context->report({arrayNode->access, format("type '%s' is immutable and cannot be changed", arrayType->name.c_str())}));

    auto accessType = TypeManager::concreteType(arrayNode->access->typeInfo);
    if (!(accessType->flags & TYPEINFO_INTEGER))
        return context->report({arrayNode->array, format("access type should be integer, not '%s'", arrayNode->access->typeInfo->name.c_str())});

    switch (arrayType->kind)
    {
    case TypeInfoKind::Pointer:
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);
        SWAG_VERIFY(typePtr->ptrCount != 1 || typePtr->finalType != g_TypeMgr.typeInfoVoid, context->report({arrayNode, "cannot dereference a 'void' pointer"}));
        arrayNode->typeInfo    = typePtr->finalType;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitPointerRef;
        break;
    }
    case TypeInfoKind::Array:
    {
        auto typePtr           = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitArrayRef;
        break;
    }
    case TypeInfoKind::Slice:
    {
        auto typePtr           = CastTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitSliceRef;
        break;
    }
    case TypeInfoKind::Struct:
        // Only the top level ArrayPointerIndex node will deal with the call
        if (arrayNode->parent->kind == AstNodeKind::ArrayPointerIndex)
        {
            arrayNode->typeInfo = arrayType;
        }
        else
        {
            // Flatten all indices. self and value will be set before the call later
            arrayNode->typeInfo = arrayType;
            arrayNode->structFlatParams.clear();
            arrayNode->structFlatParams.push_back(arrayNode->access);

            AstNode* child = arrayNode->array;
            while (child->kind == AstNodeKind::ArrayPointerIndex)
            {
                auto arrayChild = CastAst<AstPointerDeRef>(child, AstNodeKind::ArrayPointerIndex);
                arrayNode->structFlatParams.insert(arrayNode->structFlatParams.begin(), arrayChild->access);
                child = arrayChild->array;
            }
        }
        break;

    default:
    {
        return context->report({arrayNode->array, format("cannot dereference type '%s'", arrayType->name.c_str())});
    }
    }

    return true;
}

bool SemanticJob::resolveArrayPointerDeRef(SemanticContext* context)
{
    auto arrayNode         = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    auto arrayType         = TypeManager::concreteType(arrayNode->array->typeInfo);
    arrayNode->byteCodeFct = ByteCodeGenJob::emitPointerDeRef;

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));
    arrayNode->flags |= AST_R_VALUE;

    if (!(arrayNode->access->typeInfo->flags & TYPEINFO_INTEGER))
        return context->report({arrayNode->array, format("access type should be integer, not '%s'", arrayNode->access->typeInfo->name.c_str())});

    arrayNode->resolvedSymbolName = arrayNode->array->resolvedSymbolName;

    switch (arrayType->kind)
    {
    case TypeInfoKind::Native:
        if (arrayType->nativeType == NativeTypeKind::String)
            arrayNode->typeInfo = g_TypeMgr.typeInfoU8;
        break;

    case TypeInfoKind::Pointer:
    {
        auto typePtr = static_cast<TypeInfoPointer*>(arrayType);
        SWAG_VERIFY(typePtr->ptrCount != 1 || typePtr->finalType != g_TypeMgr.typeInfoVoid, context->report({arrayNode, "cannot dereference a 'void' pointer"}));
        if (typePtr->ptrCount == 1)
        {
            arrayNode->typeInfo = typePtr->finalType;
        }
        else
        {
            auto newType = static_cast<TypeInfoPointer*>(typePtr->clone());
            newType->ptrCount--;
            newType->computeName();
            arrayNode->typeInfo = newType;
        }

        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);
        break;
    }

    case TypeInfoKind::Array:
    {
        auto typePtr        = static_cast<TypeInfoArray*>(arrayType);
        arrayNode->typeInfo = typePtr->pointedType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typePtr        = static_cast<TypeInfoSlice*>(arrayType);
        arrayNode->typeInfo = typePtr->pointedType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);
        break;
    }

    case TypeInfoKind::Variadic:
        arrayNode->typeInfo = g_TypeMgr.typeInfoAny;
        break;

    case TypeInfoKind::TypedVariadic:
    {
        auto typeVariadic   = static_cast<TypeInfoVariadic*>(arrayType);
        arrayNode->typeInfo = typeVariadic->rawType;
        break;
    }

    case TypeInfoKind::Struct:
        // Only the top level ArrayPointerIndex node will deal with the call
        if (arrayNode->parent->kind == AstNodeKind::ArrayPointerIndex)
        {
            arrayNode->typeInfo = arrayType;
        }
        else
        {
            // Flatten all operator parameters : self, then all indices
            arrayNode->structFlatParams.clear();
            arrayNode->structFlatParams.push_back(arrayNode->access);

            AstNode* child = arrayNode->array;
            while (child->kind == AstNodeKind::ArrayPointerIndex)
            {
                auto arrayChild = CastAst<AstPointerDeRef>(child, AstNodeKind::ArrayPointerIndex);
                arrayNode->structFlatParams.insert(arrayNode->structFlatParams.begin(), arrayChild->access);
                child = arrayChild->array;
            }

            // Self in first position
            arrayNode->structFlatParams.insert(arrayNode->structFlatParams.begin(), arrayNode->array);

            // Resolve call
            SWAG_CHECK(resolveUserOp(context, "opIndex", nullptr, nullptr, arrayNode->array, arrayNode->structFlatParams));
        }
        break;

    default:
        return context->report({arrayNode->array, format("%s type '%s' cannot be referenced like a pointer", TypeInfo::getNakedKindName(arrayType), arrayType->name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveInit(SemanticContext* context)
{
    auto node               = CastAst<AstInit>(context->node, AstNodeKind::Init);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->kind == TypeInfoKind::Pointer, context->report({node->expression, format("'init' first parameter should be a pointer, but is '%s'", expressionTypeInfo->name.c_str())}));

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->flags & TYPEINFO_INTEGER, context->report({node->count, format("'init' count parameter should be an integer, but is '%s'", countTypeInfo->name.c_str())}));
        SWAG_VERIFY(countTypeInfo->sizeOf <= 4, context->report({node->count, "'init' count parameter should be 32 bits"}));
    }

    if (node->parameters)
    {
        auto typeinfoPointer = CastTypeInfo<TypeInfoPointer>(expressionTypeInfo, TypeInfoKind::Pointer);
        auto pointedType     = typeinfoPointer->pointedType;

        if (pointedType->kind == TypeInfoKind::Native || pointedType->kind == TypeInfoKind::Pointer)
        {
            SWAG_VERIFY(node->parameters->childs.size() == 1, context->report({node->count, format("too many initialization parameters for type '%s'", pointedType->name.c_str())}));
            auto child = node->parameters->childs.front();
            SWAG_CHECK(TypeManager::makeCompatibles(context, pointedType, child->typeInfo, nullptr, child));
        }
        else if (pointedType->kind == TypeInfoKind::Struct)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(pointedType, TypeInfoKind::Struct);
            auto job        = context->job;
            job->cacheDependentSymbols.clear();
            job->cacheDependentSymbols.push_back(typeStruct->structNode->resolvedSymbolName);
            job->symMatch.reset();
            for (auto child : node->parameters->childs)
                job->symMatch.parameters.push_back(child);
            SWAG_CHECK(matchIdentifierParameters(context, nullptr, node->parameters, node));
            if (context->result == ContextResult::Pending)
                return true;
        }
        else
        {
            return internalError(context, "resolveInit, invalid type");
        }
    }

    node->byteCodeFct = ByteCodeGenJob::emitInit;

    return true;
}

bool SemanticJob::resolveDrop(SemanticContext* context)
{
    auto node               = CastAst<AstDrop>(context->node, AstNodeKind::Drop);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->kind == TypeInfoKind::Pointer, context->report({node->expression, format("'drop' first parameter should be a pointer, but is '%s'", expressionTypeInfo->name.c_str())}));

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->flags & TYPEINFO_INTEGER, context->report({node->count, format("'drop' count parameter should be an integer, but is '%s'", countTypeInfo->name.c_str())}));
        SWAG_VERIFY(countTypeInfo->sizeOf <= 4, context->report({node->count, "'drop' count parameter should be 32 bits"}));
    }

    node->byteCodeFct = ByteCodeGenJob::emitDrop;

    return true;
}