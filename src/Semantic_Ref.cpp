#include "pch.h"
#include "Semantic.h"
#include "ByteCodeGen.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Report.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "Diagnostic.h"

bool Semantic::boundCheck(SemanticContext* context, TypeInfo* forType, AstNode* arrayNode, AstNode* arrayAccess, uint64_t maxCount)
{
    if (!arrayAccess->hasComputedValue())
        return true;

    if (maxCount == 0)
    {
        if (forType->isSlice())
        {
            Diagnostic diag{arrayAccess, Err(Err0599)};
            diag.addRange(arrayNode, Nte(Nte0173));
            return context->report(diag);
        }

        if (forType->isString())
        {
            Diagnostic diag{arrayAccess, Err(Err0599)};
            diag.addRange(arrayNode, Nte(Nte0174));
            return context->report(diag);
        }
    }

    auto idx = arrayAccess->computedValue->reg.u64;
    if (idx >= maxCount)
        return context->report({arrayAccess, Fmt(Err(Err0136), idx, maxCount - 1)});
    return true;
}

bool Semantic::checkCanMakeFuncPointer(SemanticContext* context, AstFuncDecl* funcNode, AstNode* node)
{
    Utf8 msg;
    Utf8 msg1;

    if (funcNode->attributeFlags & ATTRIBUTE_MACRO)
    {
        msg  = Err(Err0183);
        msg1 = Nte(Nte0121);
    }
    else if (funcNode->attributeFlags & ATTRIBUTE_MIXIN)
    {
        msg  = Err(Err0184);
        msg1 = Nte(Nte0122);
    }
    else if (funcNode->attributeFlags & ATTRIBUTE_INLINE)
    {
        msg  = Err(Err0186);
        msg1 = Nte(Nte0120);
    }

    if (!msg.empty())
    {
        Diagnostic diag{node, msg};
        auto       note  = Diagnostic::hereIs(funcNode);
        auto       note1 = Diagnostic::note(msg1);
        return context->report(diag, note, note1);
    }

    return true;
}

bool Semantic::checkCanTakeAddress(SemanticContext* context, AstNode* node)
{
    SWAG_ASSERT(node->kind == AstNodeKind::IdentifierRef || node->kind == AstNodeKind::ArrayPointerIndex);
    if (!(node->flags & AST_L_VALUE))
    {
        if (node->resolvedSymbolName->kind != SymbolKind::Variable)
        {
            Diagnostic diag{node, Fmt(Err(Err0179), Naming::aKindName(node->resolvedSymbolName->kind).c_str())};
            return context->report(diag);
        }

        Diagnostic diag{node, Fmt(Err(Err0180), node->typeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    return true;
}

bool Semantic::resolveMakePointerLambda(SemanticContext* context)
{
    auto     node  = CastAst<AstMakePointer>(context->node, AstNodeKind::MakePointerLambda, AstNodeKind::MakePointer);
    AstNode* child = nullptr;

    // When this is a closure, we have /capture params/ref to the function/capture block/ref to the capture block
    if (node->lambda && node->lambda->captureParameters)
        child = node->childs[1];
    else
        child = node->childs.front();

    SWAG_CHECK(checkCanTakeAddress(context, child));
    SWAG_CHECK(checkIsConcrete(context, child));
    node->flags |= AST_R_VALUE;
    node->resolvedSymbolName     = child->resolvedSymbolName;
    node->resolvedSymbolOverload = child->resolvedSymbolOverload;

    auto funcNode = node->resolvedSymbolOverload->node;
    SWAG_CHECK(checkCanMakeFuncPointer(context, (AstFuncDecl*) funcNode, child));

    auto lambdaType  = child->typeInfo->clone();
    lambdaType->kind = TypeInfoKind::LambdaClosure;
    if (child->typeInfo->isClosure())
        lambdaType->sizeOf = SWAG_LIMIT_CLOSURE_SIZEOF;
    else
        lambdaType->sizeOf = sizeof(void*);
    node->typeInfo    = lambdaType;
    node->byteCodeFct = ByteCodeGen::emitMakeLambda;

    // :CaptureBlock
    // Block capture
    if (node->lambda && node->lambda->captureParameters)
    {
        auto       typeBlock = CastTypeInfo<TypeInfoStruct>(node->childs.back()->typeInfo, TypeInfoKind::Struct);
        const auto MaxSize   = SWAG_LIMIT_CLOSURE_SIZEOF - 2 * sizeof(void*);
        SWAG_VERIFY(typeBlock->sizeOf <= MaxSize, context->report({node->lambda->captureParameters, Fmt(Err(Err0026), typeBlock->sizeOf, MaxSize)}));
    }

    return true;
}

bool Semantic::resolveMakePointer(SemanticContext* context)
{
    auto node     = CastAst<AstMakePointer>(context->node, AstNodeKind::MakePointer);
    auto child    = node->childs.front();
    auto typeInfo = child->typeInfo;

    if (!child->resolvedSymbolName)
    {
        Diagnostic diag{node, node->token, Err(Err0187)};
        diag.addRange(child, Diagnostic::isType(typeInfo));
        return context->report(diag);
    }

    if (child->resolvedSymbolOverload)
    {
        ScopedLock lk(child->resolvedSymbolName->mutex);
        child->resolvedSymbolOverload->flags |= OVERLOAD_HAS_MAKE_POINTER;
    }

    if (child->resolvedSymbolOverload && child->resolvedSymbolOverload->flags & OVERLOAD_IS_LET)
    {
        if (child->kind != AstNodeKind::IdentifierRef || child->childs.back()->kind != AstNodeKind::ArrayPointerIndex)
        {
            Diagnostic diag{node, node->token, Err(Err0185)};
            return context->report(diag, Diagnostic::hereIs(child->resolvedSymbolOverload->node));
        }
    }

    if (child->resolvedSymbolName->kind == SymbolKind::Function)
    {
        // For a function, if no parameters, then this is for a lambda
        auto back = child->childs.back();
        if (back->kind != AstNodeKind::FuncCall)
        {
            if (back->kind != AstNodeKind::Identifier)
                return resolveMakePointerLambda(context);

            auto idBack = CastAst<AstIdentifier>(back, AstNodeKind::Identifier);
            if (!idBack->callParameters)
                return resolveMakePointerLambda(context);
        }

        // Otherwise we want to take the address of the return value
        // Only for a reference
        typeInfo = TypeManager::concreteType(typeInfo);
        if (!typeInfo->isPointerRef())
        {
            if (typeInfo->isVoid())
            {
                Diagnostic diag{node, node->token, Fmt(Err(Err0178), typeInfo->getDisplayNameC())};
                return context->report(diag, Diagnostic::hereIs(child->resolvedSymbolOverload));
            }
            else
            {
                Diagnostic diag{node, node->token, Fmt(Err(Err0182), typeInfo->getDisplayNameC())};
                auto       note = Diagnostic::note(Fmt(Nte(Nte0100), Naming::aKindName(typeInfo).c_str()));
                return context->report(diag, Diagnostic::hereIs(child->resolvedSymbolOverload), note);
            }
        }
    }

    SWAG_CHECK(checkCanTakeAddress(context, child));
    SWAG_CHECK(checkIsConcrete(context, child));
    node->flags |= AST_R_VALUE;
    node->resolvedSymbolName     = child->resolvedSymbolName;
    node->resolvedSymbolOverload = child->resolvedSymbolOverload;
    node->byteCodeFct            = ByteCodeGen::emitMakePointer;
    node->inheritComputedValue(child);

    // A new pointer
    bool     forceConst = false;
    uint64_t ptrFlags   = 0;

    // Transform a reference pointer to a pointer to the pointed value
    if (typeInfo->isPointerRef())
    {
        forceConst = typeInfo->isConst();
        typeInfo   = TypeManager::concretePtrRef(typeInfo);
        child->semFlags |= SEMFLAG_FORCE_NO_TAKE_ADDRESS;
        child->childs.back()->semFlags |= SEMFLAG_FORCE_NO_TAKE_ADDRESS;
        node->byteCodeFct = ByteCodeGen::emitPassThrough;
    }

    // If this is an array, then this is legit, the pointer will address the first
    // element : need to find it's type
    else if (typeInfo->isArray())
    {
        while (typeInfo->isArray())
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            typeInfo       = typeArray->pointedType;
        }

        ptrFlags |= TYPEINFO_POINTER_ARITHMETIC;
    }

    // :PointerArithmetic
    // Taking the address of an array element is ok for pointer arithmetic
    if (child->kind == AstNodeKind::IdentifierRef)
    {
        auto last = child->childs.back();
        if (last->kind == AstNodeKind::ArrayPointerIndex)
            ptrFlags |= TYPEINFO_POINTER_ARITHMETIC;
    }

    // Taking the address of a const is const
    if (child->flags & AST_IS_CONST || forceConst)
    {
        ptrFlags |= TYPEINFO_CONST;
    }

    // Type is constant if we take address of a readonly variable
    else if (child->resolvedSymbolOverload && !child->typeInfo->isPointerRef())
    {
        auto typeResolved = child->resolvedSymbolOverload->typeInfo->getConcreteAlias();

        if ((child->resolvedSymbolOverload->flags & OVERLOAD_CONST_ASSIGN) &&
            !typeResolved->isPointerArithmetic() &&
            !typeResolved->isArray() &&
            !typeResolved->isSlice())
        {
            ptrFlags |= TYPEINFO_CONST;
        }
        else if (typeResolved->isConst() && typeResolved->isSlice())
        {
            ptrFlags |= TYPEINFO_CONST;
        }
        else if (node->flags & AST_IS_CONST)
        {
            ptrFlags |= TYPEINFO_CONST;
        }
    }

    // Transform pointer to a reference
    if (node->specFlags & AstMakePointer::SPECFLAG_TOREF)
    {
        ptrFlags |= TYPEINFO_POINTER_REF;
    }

    auto ptrType   = g_TypeMgr->makePointerTo(typeInfo, ptrFlags);
    node->typeInfo = ptrType;
    return true;
}

bool Semantic::resolveArrayPointerSlicingUpperBound(SemanticContext* context)
{
    auto arrayNode = context->node;
    auto slicing   = CastAst<AstArrayPointerSlicing>(context->node->parent, AstNodeKind::ArrayPointerSlicing);
    auto upperNode = slicing->upperBound;

    context->node = upperNode;
    SWAG_CHECK(resolveIntrinsicCountOf(context, upperNode, arrayNode));
    context->node = arrayNode;
    YIELD();

    if (upperNode->hasComputedValue())
    {
        upperNode->computedValue->reg.u64 -= 1;
        upperNode->byteCodeFct = ByteCodeGen::emitLiteral;
    }
    else
    {
        upperNode->byteCodeFct = ByteCodeGen::emitMakeArrayPointerSlicingUpperBound;
    }

    return true;
}

bool Semantic::resolveArrayPointerSlicing(SemanticContext* context)
{
    auto node         = CastAst<AstArrayPointerSlicing>(context->node, AstNodeKind::ArrayPointerSlicing);
    auto typeVar      = getConcreteTypeUnRef(node->array, CONCRETE_ALL);
    typeVar           = TypeManager::concreteType(typeVar);
    uint64_t maxBound = 0;

    {
        PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, [node]()
                          { return Fmt(Nte(Nte0135), node->lowerBound->typeInfo->getDisplayNameC()); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, node->lowerBound, CASTFLAG_TRY_COERCE));
    }
    {
        PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, [node]()
                          { return Fmt(Nte(Nte0135), node->lowerBound->typeInfo->getDisplayNameC()); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, node->upperBound, CASTFLAG_TRY_COERCE));
    }

    // Exclude upper bound if constant
    if (node->upperBound->hasComputedValue() &&
        node->specFlags & AstArrayPointerSlicing::SPECFLAG_EXCLUDE_UP &&
        !(node->upperBound->semFlags & SEMFLAG_ASSIGN_COMPUTED))
    {
        if (!node->upperBound->computedValue->reg.u64)
        {
            Diagnostic diag{node->upperBound, Err(Err0612)};
            return context->report(diag);
        }

        node->upperBound->semFlags |= SEMFLAG_ASSIGN_COMPUTED;
        node->upperBound->computedValue->reg.u64 -= 1;
    }

    // Slicing of an array
    if (typeVar->isArray())
    {
        auto typeInfoArray = CastTypeInfo<TypeInfoArray>(node->array->typeInfo, TypeInfoKind::Array);
        if (typeInfoArray->totalCount != typeInfoArray->count)
        {
            Diagnostic diag{node, node->token, Err(Err0376)};
            diag.addRange(node->array, Diagnostic::isType(typeInfoArray));
            return context->report(diag);
        }

        auto ptrSlice         = makeType<TypeInfoSlice>();
        ptrSlice->pointedType = typeInfoArray->finalType;
        if (typeInfoArray->isConst())
            ptrSlice->flags |= TYPEINFO_CONST;
        ptrSlice->computeName();
        node->typeInfo = ptrSlice;
        maxBound       = typeInfoArray->count - 1;
    }

    // Slicing of a string
    else if (typeVar->isString())
    {
        node->typeInfo = typeVar;
        if (node->array->hasComputedValue())
            maxBound = node->array->computedValue->text.length() - 1;
    }

    // Slicing of a pointer
    else if (typeVar->isPointer())
    {
        // :PointerArithmetic
        if (!typeVar->isPointerArithmetic())
        {
            Diagnostic diag{node, node->token, Err(Err0361)};
            diag.addRange(node->array, Diagnostic::isType(typeVar));
            return context->report(diag);
        }

        auto typeInfoPointer  = CastTypeInfo<TypeInfoPointer>(node->array->typeInfo, TypeInfoKind::Pointer);
        auto ptrSlice         = makeType<TypeInfoSlice>();
        ptrSlice->pointedType = typeInfoPointer->pointedType;
        if (typeInfoPointer->isConst())
            ptrSlice->flags |= TYPEINFO_CONST;
        ptrSlice->computeName();
        node->typeInfo = ptrSlice;
    }

    // Slicing of a... slice
    else if (typeVar->isSlice())
    {
        node->typeInfo = typeVar;
    }

    // Slicing of a struct with a special function
    else if (typeVar->isStruct())
    {
        // Flatten all operator parameters : self, then all indices
        node->structFlatParams.clear();
        node->structFlatParams.push_back(node->lowerBound);
        node->structFlatParams.push_back(node->upperBound);

        // Self in first position
        node->structFlatParams.push_front(node->array);

        // Resolve call
        auto        typeInfo = node->array->typeInfo;
        SymbolName* symbol   = nullptr;
        SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opSlice, node->array, &symbol));
        YIELD();
        if (!symbol)
        {
            Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0374), node->array->token.ctext(), typeInfo->getDisplayNameC())};
            diag.hint = Fmt(Nte(Nte0144), g_LangSpec->name_opSlice.c_str());
            diag.addRange(node->array, Diagnostic::isType(typeInfo));
            return context->report(diag);
        }

        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opSlice, nullptr, nullptr, node->array, node->structFlatParams));
        YIELD();
    }
    else
    {
        return context->report({node->array, Fmt(Err(Err0375), node->array->typeInfo->getDisplayNameC())});
    }

    // startBound <= endBound
    if (node->lowerBound->hasComputedValue() && node->upperBound->hasComputedValue())
    {
        if (node->lowerBound->computedValue->reg.u64 > node->upperBound->computedValue->reg.u64)
        {
            Diagnostic diag{node->lowerBound, Fmt(Err(Err0611), node->lowerBound->computedValue->reg.u64, node->upperBound->computedValue->reg.u64)};
            diag.addRange(node->upperBound, Nte(Nte0172));
            return context->report(diag);
        }
    }

    // endBound < maxBound
    if (maxBound && node->upperBound->hasComputedValue())
    {
        if (node->upperBound->computedValue->reg.u64 > maxBound)
        {
            Diagnostic diag{node->upperBound, Fmt(Err(Err0610), node->upperBound->computedValue->reg.u64, maxBound)};
            return context->report(diag);
        }
    }

    node->byteCodeFct = ByteCodeGen::emitMakeArrayPointerSlicing;
    return true;
}

bool Semantic::resolveMoveRef(SemanticContext* context)
{
    auto node  = context->node;
    auto front = node->childs.front();
    node->inheritAndFlag1(AST_CONST_EXPR);

    auto typeInfo = TypeManager::concreteType(front->typeInfo);

    if (front->hasComputedValue())
    {
        Diagnostic diag(node, node->token, Err(Err0464));
        return context->report(diag);
    }

    if (!typeInfo->isPointer() && !typeInfo->isPointerRef())
    {
        Diagnostic diag(node, node->token, Fmt(Err(Err0466), front->typeInfo->getDisplayNameC()));
        diag.addRange(front, Diagnostic::isType(front->typeInfo));
        return context->report(diag);
    }

    if (typeInfo->isConst())
    {
        Diagnostic diag(node, node->token, Err(Err0465));
        diag.addRange(front, Diagnostic::isType(front->typeInfo));
        return context->report(diag);
    }

    typeInfo = typeInfo->clone();
    typeInfo->flags |= TYPEINFO_POINTER_REF | TYPEINFO_POINTER_ACCEPT_MOVE_REF;
    typeInfo->forceComputeName();

    node->typeInfo    = typeInfo;
    node->byteCodeFct = ByteCodeGen::emitPassThrough;
    return true;
}

bool Semantic::resolveKeepRef(SemanticContext* context)
{
    auto node  = context->node;
    auto front = node->childs.front();
    node->inheritAndFlag1(AST_CONST_EXPR);

    auto typeInfo = TypeManager::concreteType(front->typeInfo);
    if (!typeInfo->isPointerRef() && !typeInfo->isPointer())
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0365), typeInfo->getDisplayNameC())};

        if (front->kind == AstNodeKind::IdentifierRef && front->childs.front()->kind == AstNodeKind::ArrayPointerIndex)
        {
            diag.addRange(front, Nte(Nte0026));
            return context->report(diag);
        }
        else if (front->kind == AstNodeKind::IdentifierRef)
        {
            diag.hint = Nte(Nte0129);
            auto note = Diagnostic::note(front, Fmt(Nte(Nte0195), front->token.ctext()));
            return context->report(diag, note);
        }
        else
        {
            diag.addRange(front, Diagnostic::isType(typeInfo));
            diag.hint = Nte(Nte0129);
            return context->report(diag);
        }
    }

    if (!typeInfo->isPointerRef())
    {
        typeInfo = typeInfo->clone();
        typeInfo->flags |= TYPEINFO_POINTER_REF;
        if (node->flags & AST_IS_CONST)
            typeInfo->setConst();
        typeInfo->forceComputeName();
    }
    else if (node->flags & AST_IS_CONST)
    {
        typeInfo = g_TypeMgr->makeConst(typeInfo);
    }

    node->typeInfo    = typeInfo;
    node->byteCodeFct = ByteCodeGen::emitPassThrough;
    return true;
}

bool Semantic::resolveArrayPointerIndex(SemanticContext* context)
{
    auto node = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (node->forceTakeAddress())
        SWAG_CHECK(resolveArrayPointerRef(context));
    else
        SWAG_CHECK(resolveArrayPointerDeRef(context));
    YIELD();

    if (!(node->specFlags & AstArrayPointerIndex::SPECFLAG_IS_DREF))
        node->inheritAndFlag1(AST_CONST_EXPR);

    // If this is not the last child of the IdentifierRef, then this is a reference, and
    // we must take the address and not dereference that identifier
    if (node->parent->kind == AstNodeKind::IdentifierRef)
    {
        auto parent = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);
        if (node != parent->childs.back())
        {
            // The last ArrayPointerIndex in a list [0, 0, 0] must dereference
            if (node->childs[0]->kind != AstNodeKind::ArrayPointerIndex)
                node->semFlags |= SEMFLAG_FORCE_TAKE_ADDRESS;

            // In order to resolve what's next, we need to fill the startScope of the identifier ref
            auto typeReturn = node->array->typeInfo;

            // There's a 'opIndex' function
            if (node->hasExtMisc() && node->extMisc()->resolvedUserOpSymbolOverload)
                typeReturn = TypeManager::concretePtrRefType(node->extMisc()->resolvedUserOpSymbolOverload->typeInfo);

            // Get the pointed type if we have a pointer
            if (typeReturn->isPointer())
            {
                auto typePointer = CastTypeInfo<TypeInfoPointer>(typeReturn, TypeInfoKind::Pointer);
                typeReturn       = typePointer->pointedType;
            }

            // And if this is a struct or an interface, we fill the startScope
            if (typeReturn->isStruct() || typeReturn->isInterface())
            {
                auto typeStruct    = CastTypeInfo<TypeInfoStruct>(typeReturn, TypeInfoKind::Struct, TypeInfoKind::Interface);
                parent->startScope = typeStruct->scope;
            }
        }

        parent->previousResolvedNode = node;
    }

    return true;
}

bool Semantic::resolveArrayPointerRef(SemanticContext* context)
{
    auto arrayNode                    = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    arrayNode->resolvedSymbolName     = arrayNode->array->resolvedSymbolName;
    arrayNode->resolvedSymbolOverload = arrayNode->array->resolvedSymbolOverload;
    arrayNode->inheritOrFlag(arrayNode->array, AST_L_VALUE);

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));
    arrayNode->flags |= AST_R_VALUE;

    auto baseType  = arrayNode->array->typeInfo;
    auto arrayType = TypeManager::concretePtrRefType(baseType, CONCRETE_FORCEALIAS | CONCRETE_FUNC);

    // When we are building a pointer, this is fine to be const, because in fact we do no generate an address to modify the content
    // (or it will be done later on a pointer, and it will be const too)
    if (arrayNode->parent->parent->kind != AstNodeKind::MakePointer)
    {
        if (baseType->isConst())
        {
            Diagnostic diag{arrayNode->parent->parent, arrayNode->parent->parent->token, Fmt(Err(Err0104), baseType->getDisplayNameC())};
            diag.addRange(arrayNode->array->token, Diagnostic::isType(baseType));
            return context->report(diag);
        }
    }
    else
    {
        // If array is const, inform the make pointer that it need to make a const pointer
        if (baseType->isConst())
            arrayNode->parent->parent->flags |= AST_IS_CONST;
    }

    auto accessType = TypeManager::concreteType(arrayNode->access->typeInfo);
    if (!(accessType->isNativeInteger()) && !(accessType->flags & TYPEINFO_ENUM_INDEX))
    {
        Diagnostic diag{arrayNode->access, Fmt(Err(Err0740), arrayNode->access->typeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    switch (arrayType->kind)
    {
    case TypeInfoKind::Pointer:
    {
        if (!arrayType->isPointerArithmetic() && !(arrayNode->specFlags & AstArrayPointerIndex::SPECFLAG_IS_DREF))
        {
            Diagnostic diag{arrayNode->array, Fmt(Err(Err0256), arrayNode->resolvedSymbolName->name.c_str(), arrayType->getDisplayNameC())};
            return context->report(diag);
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr = CastTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);

        if (typePtr->pointedType->isVoid())
        {
            Diagnostic diag{arrayNode->access, Err(Err0421)};
            diag.addRange(arrayNode->array, Diagnostic::isType(typePtr));
            return context->report(diag);
        }

        arrayNode->typeInfo = typePtr->pointedType;
        arrayNode->flags |= AST_ARRAY_POINTER_REF;
        arrayNode->array->flags |= AST_ARRAY_POINTER_REF;
        arrayNode->byteCodeFct = ByteCodeGen::emitPointerRef;
        break;
    }

    case TypeInfoKind::Native:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        if (arrayType->nativeType == NativeTypeKind::String)
        {
            arrayNode->typeInfo    = g_TypeMgr->typeInfoU8;
            arrayNode->byteCodeFct = ByteCodeGen::emitStringRef;
            arrayNode->flags |= AST_IS_CONST;
        }
        else
        {
            Diagnostic diag{arrayNode->array, Fmt(Err(Err0260), arrayType->getDisplayNameC())};
            if (arrayNode->specFlags & AstArrayPointerIndex::SPECFLAG_IS_DREF)
                diag.addRange(arrayNode->token.startLocation, arrayNode->token.endLocation, Nte(Nte0112));
            return context->report(diag);
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typeArray         = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);
        arrayNode->typeInfo    = typeArray->pointedType;
        arrayNode->byteCodeFct = ByteCodeGen::emitArrayRef;

        // Try to dereference as a constant if we can
        uint32_t     storageOffset  = UINT32_MAX;
        DataSegment* storageSegment = nullptr;
        SWAG_CHECK(getConstantArrayPtr(context, &storageOffset, &storageSegment));
        if (storageSegment)
        {
            arrayNode->setFlagsValueIsComputed();
            arrayNode->computedValue->storageOffset  = storageOffset;
            arrayNode->computedValue->storageSegment = storageSegment;
        }
        else
        {
            SWAG_CHECK(boundCheck(context, arrayType, arrayNode->array, arrayNode->access, typeArray->count));
        }
        break;
    }

    case TypeInfoKind::Slice:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr           = CastTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = ByteCodeGen::emitSliceRef;
        break;
    }

    case TypeInfoKind::Struct:
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));

        if (arrayType->isTuple())
        {
            if (arrayNode->specFlags & AstArrayPointerIndex::SPECFLAG_IS_DREF)
            {
                Diagnostic diag{arrayNode->access, Err(Err0382)};
                diag.addRange(arrayNode->token.startLocation, arrayNode->token.endLocation, Nte(Nte0112));
                return context->report(diag);
            }
            else
            {
                Diagnostic diag{arrayNode->access, Err(Err0382)};
                diag.addRange(arrayNode->array, Diagnostic::isType(arrayType));
                return context->report(diag);
            }
        }

        arrayNode->typeInfo = arrayType;

        // In fact we are taking the address of an operator [] call result
        if (arrayNode->parent->parent->kind == AstNodeKind::MakePointer)
        {
            SWAG_CHECK(resolveArrayPointerDeRef(context));
            YIELD();
            arrayNode->typeInfo = TypeManager::concreteType(arrayNode->typeInfo);
        }

        // Only the top level ArrayPointerIndex node will deal with the call
        if (arrayNode->parent->kind != AstNodeKind::ArrayPointerIndex)
        {
            // Flatten all indices. self and value will be set before the call later
            // Can be already done, so do not overwrite
            // :StructFlatParamsDone
            if (arrayNode->structFlatParams.empty())
            {
                arrayNode->structFlatParams.push_back(arrayNode->access);

                AstNode* child = arrayNode->array;
                while (child->kind == AstNodeKind::ArrayPointerIndex)
                {
                    auto arrayChild = CastAst<AstArrayPointerIndex>(child, AstNodeKind::ArrayPointerIndex);
                    arrayNode->structFlatParams.push_front(arrayChild->access);
                    child = arrayChild->array;
                }
            }
        }
        break;

    default:
    {
        return context->report({arrayNode->array, Fmt(Err(Err0260), arrayType->getDisplayNameC())});
    }
    }

    return true;
}

bool Semantic::getConstantArrayPtr(SemanticContext* context, uint32_t* storageOffset, DataSegment** storageSegment)
{
    auto arrayNode = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto arrayType = TypeManager::concreteType(arrayNode->array->typeInfo);
    auto typePtr   = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);

    if (!arrayNode->typeInfo->isArray() && arrayNode->access->hasComputedValue())
    {
        bool     isConstAccess = true;
        uint64_t offsetAccess  = arrayNode->access->computedValue->reg.u64 * typePtr->finalType->sizeOf;
        SWAG_CHECK(boundCheck(context, arrayType, arrayNode->array, arrayNode->access, typePtr->count));

        // Deal with array of array
        auto subArray = arrayNode;
        while (isConstAccess && subArray->array->kind == AstNodeKind::ArrayPointerIndex)
        {
            subArray      = CastAst<AstArrayPointerIndex>(subArray->array, AstNodeKind::ArrayPointerIndex);
            isConstAccess = isConstAccess && subArray->access->hasComputedValue();
            if (isConstAccess)
            {
                if (subArray->array->typeInfo->kind != TypeInfoKind::Array)
                    isConstAccess = false;
                else
                {
                    auto subTypePtr = CastTypeInfo<TypeInfoArray>(subArray->array->typeInfo, TypeInfoKind::Array);
                    SWAG_CHECK(boundCheck(context, subArray->array->typeInfo, subArray->array, subArray->access, subTypePtr->count));
                    offsetAccess += subArray->access->computedValue->reg.u64 * subTypePtr->pointedType->sizeOf;
                    typePtr = subTypePtr;
                }
            }
        }

        if (isConstAccess)
        {
            auto overload = subArray->array->resolvedSymbolOverload;
            if (overload && (overload->flags & OVERLOAD_COMPUTED_VALUE))
            {
                SWAG_ASSERT(overload->computedValue.storageOffset != UINT32_MAX);
                SWAG_ASSERT(overload->computedValue.storageSegment);
                *storageOffset  = overload->computedValue.storageOffset + (uint32_t) offsetAccess;
                *storageSegment = overload->computedValue.storageSegment;
                return true;
            }

            if (subArray->array->hasComputedValue())
            {
                SWAG_ASSERT(subArray->array->computedValue);
                SWAG_ASSERT(subArray->array->computedValue->storageOffset != UINT32_MAX);
                SWAG_ASSERT(subArray->array->computedValue->storageSegment);
                *storageOffset  = subArray->array->computedValue->storageOffset + (uint32_t) offsetAccess;
                *storageSegment = subArray->array->computedValue->storageSegment;
                return true;
            }
        }
    }

    return true;
}

bool Semantic::resolveArrayPointerDeRef(SemanticContext* context)
{
    auto arrayNode         = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto arrayAccess       = arrayNode->access;
    auto arrayType         = getConcreteTypeUnRef(arrayNode->array, CONCRETE_ALL);
    arrayNode->byteCodeFct = ByteCodeGen::emitPointerDeRef;

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));

    if (arrayType->isTuple())
    {
        if (arrayNode->specFlags & AstArrayPointerIndex::SPECFLAG_IS_DREF)
        {
            Diagnostic diag{arrayNode->access, Err(Err0382)};
            diag.addRange(arrayNode->token.startLocation, arrayNode->token.endLocation, Nte(Nte0112));
            return context->report(diag);
        }
        else
        {
            Diagnostic diag{arrayNode->access, Err(Err0382)};
            diag.addRange(arrayNode->array, Diagnostic::isType(arrayType));
            return context->report(diag);
        }
    }

    arrayNode->flags |= AST_R_VALUE;

    auto accessType = getConcreteTypeUnRef(arrayNode->access, CONCRETE_ALL);
    if (!(accessType->isNativeInteger()) && !(accessType->flags & TYPEINFO_ENUM_INDEX))
    {
        Diagnostic diag{arrayNode->access, Fmt(Err(Err0740), arrayNode->access->typeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    // Do not set resolvedSymbolOverload !
    arrayNode->resolvedSymbolName = arrayNode->array->resolvedSymbolName;

    // Can we dereference at compile time ?
    if (arrayType->isString())
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        if (arrayNode->access->hasComputedValue())
        {
            if (arrayNode->array->resolvedSymbolOverload && (arrayNode->array->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
            {
                arrayNode->setFlagsValueIsComputed();
                auto& text = arrayNode->array->resolvedSymbolOverload->computedValue.text;
                SWAG_CHECK(boundCheck(context, arrayType, arrayNode->array, arrayNode->access, text.length()));
                auto idx                         = arrayAccess->computedValue->reg.u32;
                arrayNode->computedValue->reg.u8 = text[idx];
            }
        }

        arrayNode->typeInfo = g_TypeMgr->typeInfoU8;
        return true;
    }

    switch (arrayType->kind)
    {
    case TypeInfoKind::Pointer:
    {
        if (!arrayType->isPointerArithmetic() && !(arrayNode->specFlags & AstArrayPointerIndex::SPECFLAG_IS_DREF))
        {
            Diagnostic diag{arrayNode->access, Fmt(Err(Err0256), arrayNode->resolvedSymbolName->name.c_str(), arrayType->getDisplayNameC())};
            diag.addRange(arrayNode->array, Diagnostic::isType(arrayType));
            return context->report(diag, Diagnostic::note(Nte(Nte0103)));
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr = CastTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);

        if (typePtr->pointedType->isVoid())
        {
            Diagnostic diag{arrayNode->access, Err(Err0421)};
            diag.addRange(arrayNode->array, Diagnostic::isType(typePtr));
            return context->report(diag);
        }

        arrayNode->typeInfo = typePtr->pointedType;
        setupIdentifierRef(context, arrayNode);

        // Try to dereference as a constant if we can
        if (arrayNode->array->hasComputedValue() && arrayNode->access->hasComputedValue())
        {
            auto storageSegment = arrayNode->array->computedValue->storageSegment;
            if (!storageSegment)
            {
                Diagnostic diag{arrayNode, Err(Err0599)};
                return context->report(diag);
            }

            auto storageOffset = arrayNode->array->computedValue->storageOffset;
            SWAG_ASSERT(storageOffset != UINT32_MAX);

            auto offset = arrayNode->access->computedValue->reg.u32 * typePtr->pointedType->sizeOf;
            auto ptr    = storageSegment->address(storageOffset + offset);
            if (!derefConstantValue(context, arrayNode, typePtr->pointedType, storageSegment, ptr))
            {
                if (typePtr->pointedType->isStruct())
                {
                    arrayNode->setFlagsValueIsComputed();
                    arrayNode->computedValue->storageSegment = storageSegment;
                    arrayNode->computedValue->storageOffset  = storageOffset;
                    arrayNode->typeInfo                      = typePtr->pointedType;
                }
            }
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr        = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);
        arrayNode->typeInfo = typePtr->pointedType;
        setupIdentifierRef(context, arrayNode);

        // Try to dereference as a constant if we can
        uint32_t     storageOffset  = UINT32_MAX;
        DataSegment* storageSegment = nullptr;
        SWAG_CHECK(getConstantArrayPtr(context, &storageOffset, &storageSegment));
        if (storageSegment)
        {
            auto ptr = storageSegment->address(storageOffset);
            if (!derefConstantValue(context, arrayNode, typePtr->finalType, storageSegment, ptr))
            {
                if (typePtr->finalType->isStruct())
                {
                    arrayNode->setFlagsValueIsComputed();
                    arrayNode->computedValue->storageSegment = storageSegment;
                    arrayNode->computedValue->storageOffset  = storageOffset;
                    arrayNode->typeInfo                      = typePtr->finalType;
                }
            }
        }

        break;
    }

    case TypeInfoKind::Slice:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typeSlice      = CastTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
        arrayNode->typeInfo = typeSlice->pointedType;
        setupIdentifierRef(context, arrayNode);

        // Try to dereference as a constant if we can
        if (arrayNode->access->hasComputedValue())
        {
            if (arrayNode->array->resolvedSymbolOverload && (arrayNode->array->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
            {
                auto& computedValue = arrayNode->array->resolvedSymbolOverload->computedValue;
                auto  slice         = (SwagSlice*) computedValue.getStorageAddr();
                SWAG_CHECK(boundCheck(context, arrayType, arrayNode->array, arrayNode->access, slice->count));

                auto ptr = (uint8_t*) slice->buffer;
                ptr += arrayNode->access->computedValue->reg.u64 * typeSlice->pointedType->sizeOf;
                derefConstantValue(context, arrayNode, typeSlice->pointedType, computedValue.storageSegment, ptr);
            }
        }

        break;
    }

    case TypeInfoKind::Variadic:
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        arrayNode->typeInfo = g_TypeMgr->typeInfoAny;
        break;

    case TypeInfoKind::TypedVariadic:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typeVariadic   = CastTypeInfo<TypeInfoVariadic>(arrayType, TypeInfoKind::TypedVariadic);
        arrayNode->typeInfo = typeVariadic->rawType;
        setupIdentifierRef(context, arrayNode);
        break;
    }

    case TypeInfoKind::Struct:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));

        // Only the top level ArrayPointerIndex node (for a given serial) will deal with the call
        if (arrayNode->parent->kind == AstNodeKind::ArrayPointerIndex &&
            (arrayNode->parent->specFlags & AstArrayPointerIndex::SPECFLAG_SERIAL) == (arrayNode->specFlags & AstArrayPointerIndex::SPECFLAG_SERIAL))
        {
            arrayNode->typeInfo = arrayType;
            break;
        }

        // Flatten all operator parameters : self, then all indices
        // :StructFlatParamsDone
        arrayNode->structFlatParams.clear();
        arrayNode->structFlatParams.push_back(arrayNode->access);

        uint16_t serial = arrayNode->specFlags & AstArrayPointerIndex::SPECFLAG_SERIAL;
        AstNode* child  = arrayNode->array;
        while (child->kind == AstNodeKind::ArrayPointerIndex &&
               (child->specFlags & AstArrayPointerIndex::SPECFLAG_SERIAL) == serial)
        {
            auto arrayChild = CastAst<AstArrayPointerIndex>(child, AstNodeKind::ArrayPointerIndex);
            arrayNode->structFlatParams.push_front(arrayChild->access);
            child = arrayChild->array;
        }

        // Self in first position
        arrayNode->structFlatParams.push_front(arrayNode->array);

        // Resolve call
        SymbolName* symbol = nullptr;
        SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opIndex, arrayNode->array, &symbol));
        if (!symbol)
        {
            YIELD();

            if (arrayNode->array->token.text.empty())
            {
                Diagnostic diag{arrayNode->access, Fmt(Err(Err0259), arrayType->getDisplayNameC())};
                diag.hint = Fmt(Nte(Nte0144), g_LangSpec->name_opIndex.c_str());
                diag.addRange(arrayNode->array, Diagnostic::isType(arrayType));
                return context->report(diag);
            }
            else
            {
                Diagnostic diag{arrayNode->access, Fmt(Err(Err0258), arrayNode->array->token.ctext(), arrayType->getDisplayNameC())};
                diag.hint = Fmt(Nte(Nte0144), g_LangSpec->name_opIndex.c_str());
                diag.addRange(arrayNode->array, Diagnostic::isType(arrayType));
                return context->report(diag);
            }
        }

        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndex, nullptr, nullptr, arrayNode->array, arrayNode->structFlatParams));
        break;
    }

    default:
    {
        Diagnostic diag{arrayNode->array, Fmt(Err(Err0260), arrayNode->array->typeInfo->getDisplayNameC())};
        if (arrayNode->specFlags & AstArrayPointerIndex::SPECFLAG_IS_DREF)
            diag.addRange(arrayNode->token.startLocation, arrayNode->token.endLocation, Nte(Nte0112));
        return context->report(diag);
    }
    }

    return true;
}

bool Semantic::checkInitDropCount(SemanticContext* context, AstNode* node, AstNode* expression, AstNode* count)
{
    if (!count)
        return true;

    auto countTypeInfo = TypeManager::concreteType(count->typeInfo);
    SWAG_VERIFY(countTypeInfo->isNativeInteger(), context->report({count, Fmt(Err(Err0194), node->token.ctext(), countTypeInfo->getDisplayNameC())}));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, count, CASTFLAG_TRY_COERCE));

    if (!count->hasComputedValue() || count->computedValue->reg.u64 > 1)
    {
        if (!expression->typeInfo->isPointerArithmetic())
        {
            if (count->hasComputedValue())
            {
                Diagnostic diag{expression, Fmt(Err(Err0197), node->token.ctext(), expression->typeInfo->getDisplayNameC())};
                diag.addRange(count, Fmt(Nte(Nte0127), count->computedValue->reg.u64));
                return context->report(diag);
            }
            else
            {
                Diagnostic diag{expression, Fmt(Err(Err0198), node->token.ctext(), expression->typeInfo->getDisplayNameC())};
                diag.addRange(count, Nte(Nte0128));
                return context->report(diag);
            }
        }
    }

    return true;
}

bool Semantic::resolveInit(SemanticContext* context)
{
    auto node               = CastAst<AstInit>(context->node, AstNodeKind::Init);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    if (!node->count)
    {
        expressionTypeInfo = getConcreteTypeUnRef(node->expression, CONCRETE_ALIAS);
        SWAG_VERIFY(node->expression->kind == AstNodeKind::IdentifierRef, context->report({node->expression, Fmt(Err(Err0199), node->token.ctext())}));
        SWAG_VERIFY(node->expression->resolvedSymbolOverload, context->report({node->expression, Fmt(Err(Err0199), node->token.ctext())}));
        SWAG_VERIFY(!expressionTypeInfo->isConst(), context->report({node->expression, Fmt(Err(Err0057), node->token.ctext(), expressionTypeInfo->getDisplayNameC())}));
        auto back = node->expression->childs.back();
        back->semFlags |= SEMFLAG_FORCE_TAKE_ADDRESS;
        back->resolvedSymbolOverload->flags |= OVERLOAD_HAS_MAKE_POINTER;
    }
    else
    {
        SWAG_VERIFY(expressionTypeInfo->isPointer(), context->report({node->expression, Fmt(Err(Err0196), node->token.ctext(), expressionTypeInfo->getDisplayNameC())}));
        SWAG_VERIFY(!node->expression->typeInfo->isConst(), context->report({node->expression, Fmt(Err(Err0056), node->token.ctext(), expressionTypeInfo->getDisplayNameC())}));
        SWAG_CHECK(checkInitDropCount(context, node, node->expression, node->count));
    }

    if (node->parameters)
    {
        TypeInfo* pointedType = nullptr;
        if (node->count)
        {
            auto typeinfoPointer = CastTypeInfo<TypeInfoPointer>(expressionTypeInfo, TypeInfoKind::Pointer);
            pointedType          = typeinfoPointer->pointedType;
        }
        else
        {
            pointedType = expressionTypeInfo;
            if (pointedType->isArray())
                pointedType = ((TypeInfoArray*) pointedType)->finalType;
        }

        if (pointedType->isNative() || pointedType->isPointer())
        {
            SWAG_VERIFY(node->parameters->childs.size() == 1, context->report({node->parameters, Fmt(Err(Err0637), pointedType->getDisplayNameC())}));
            auto child = node->parameters->childs.front();
            SWAG_CHECK(TypeManager::makeCompatibles(context, pointedType, child->typeInfo, nullptr, child));
        }
        else if (pointedType->isStruct())
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(pointedType, TypeInfoKind::Struct);

            SymbolMatchContext symMatchContext;
            symMatchContext.reset();
            for (auto child : node->parameters->childs)
                symMatchContext.parameters.push_back(child);

            auto& listTryMatch = context->cacheListTryMatch;
            while (true)
            {
                context->clearTryMatch();
                auto symbol = typeStruct->declNode->resolvedSymbolName;

                {
                    SharedLock lk(symbol->mutex);
                    for (auto overload : symbol->overloads)
                    {
                        auto t               = context->getTryMatch();
                        t->symMatchContext   = symMatchContext;
                        t->overload          = overload;
                        t->genericParameters = nullptr;
                        t->callParameters    = node->parameters;
                        t->dependentVar      = nullptr;
                        t->cptOverloads      = (uint32_t) symbol->overloads.size();
                        listTryMatch.push_back(t);
                    }
                }

                SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, node));
                if (context->result == ContextResult::Pending)
                    return true;
                if (context->result != ContextResult::NewChilds)
                    break;
                context->result = ContextResult::Done;
            }
        }
        else
        {
            return Report::internalError(context->node, "resolveInit, invalid type");
        }
    }

    node->byteCodeFct = ByteCodeGen::emitInit;
    return true;
}

bool Semantic::resolveDropCopyMove(SemanticContext* context)
{
    auto node               = CastAst<AstDropCopyMove>(context->node, AstNodeKind::Drop, AstNodeKind::PostCopy, AstNodeKind::PostMove);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->isPointer(), context->report({node->expression, Fmt(Err(Err0196), node->token.ctext(), expressionTypeInfo->getDisplayNameC())}));
    SWAG_VERIFY(!node->expression->typeInfo->isConst(), context->report({node->expression, Fmt(Err(Err0056), node->token.ctext(), expressionTypeInfo->getDisplayNameC())}));
    SWAG_CHECK(checkInitDropCount(context, node, node->expression, node->count));

    // Be sure struct if not marked as nocopy
    if (node->kind == AstNodeKind::PostCopy)
    {
        auto ptrType     = CastTypeInfo<TypeInfoPointer>(expressionTypeInfo, TypeInfoKind::Pointer);
        auto pointedType = TypeManager::concreteType(ptrType->pointedType);
        if (pointedType->flags & TYPEINFO_STRUCT_NO_COPY)
        {
            return context->report({node->expression, Fmt(Err(Err0101), pointedType->getDisplayNameC())});
        }
    }

    node->byteCodeFct = ByteCodeGen::emitDropCopyMove;
    return true;
}
