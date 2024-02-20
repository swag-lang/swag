#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Naming.h"
#include "Report.h"
#include "Semantic.h"
#include "TypeManager.h"

bool Semantic::boundCheck(SemanticContext* context, const TypeInfo* forType, AstNode* arrayNode, AstNode* arrayAccess, uint64_t maxCount)
{
    if (!arrayAccess->hasFlagComputedValue())
        return true;

    if (maxCount == 0)
    {
        if (forType->isSlice())
        {
            Diagnostic err{arrayAccess, Err(Err0599)};
            err.addNote(arrayNode, Nte(Nte0173));
            return context->report(err);
        }

        if (forType->isString())
        {
            Diagnostic err{arrayAccess, Err(Err0599)};
            err.addNote(arrayNode, Nte(Nte0174));
            return context->report(err);
        }
    }

    const auto idx = arrayAccess->computedValue()->reg.u64;
    if (idx >= maxCount)
        return context->report({arrayAccess, FMT(Err(Err0136), idx, maxCount - 1)});
    return true;
}

bool Semantic::checkCanMakeFuncPointer(SemanticContext* context, AstFuncDecl* funcNode, AstNode* node)
{
    Utf8 msg;
    Utf8 msg1;

    if (funcNode->hasAttribute(ATTRIBUTE_MACRO))
    {
        msg  = Err(Err0183);
        msg1 = Nte(Nte0121);
    }
    else if (funcNode->hasAttribute(ATTRIBUTE_MIXIN))
    {
        msg  = Err(Err0184);
        msg1 = Nte(Nte0122);
    }
    else if (funcNode->hasAttribute(ATTRIBUTE_INLINE))
    {
        msg  = Err(Err0186);
        msg1 = Nte(Nte0120);
    }

    if (!msg.empty())
    {
        const Diagnostic err{node, msg};
        const auto       note  = Diagnostic::hereIs(funcNode);
        const auto       note1 = Diagnostic::note(msg1);
        return context->report(err, note, note1);
    }

    return true;
}

bool Semantic::checkCanTakeAddress(SemanticContext* context, AstNode* node)
{
    SWAG_ASSERT(node->kind == AstNodeKind::IdentifierRef || node->kind == AstNodeKind::ArrayPointerIndex);
    if (!node->hasAstFlag(AST_L_VALUE))
    {
        if (node->resolvedSymbolName->kind != SymbolKind::Variable)
        {
            const Diagnostic err{node, FMT(Err(Err0179), Naming::aKindName(node->resolvedSymbolName->kind).c_str())};
            return context->report(err);
        }

        const Diagnostic err{node, FMT(Err(Err0180), node->typeInfo->getDisplayNameC())};
        return context->report(err);
    }

    return true;
}

bool Semantic::resolveMakePointerLambda(SemanticContext* context)
{
    const auto node  = castAst<AstMakePointer>(context->node, AstNodeKind::MakePointerLambda, AstNodeKind::MakePointer);
    AstNode*   child = nullptr;

    // When this is a closure, we have /capture params/ref to the function/capture block/ref to the capture block
    if (node->lambda && node->lambda->captureParameters)
        child = node->children[1];
    else
        child = node->children.front();

    SWAG_CHECK(checkCanTakeAddress(context, child));
    SWAG_CHECK(checkIsConcrete(context, child));
    node->addAstFlag(AST_R_VALUE);
    node->resolvedSymbolName     = child->resolvedSymbolName;
    node->resolvedSymbolOverload = child->resolvedSymbolOverload;

    const auto funcNode = node->resolvedSymbolOverload->node;
    SWAG_CHECK(checkCanMakeFuncPointer(context, castAst<AstFuncDecl>(funcNode), child));

    const auto lambdaType = child->typeInfo->clone();
    lambdaType->kind      = TypeInfoKind::LambdaClosure;
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
        const auto     typeBlock = castTypeInfo<TypeInfoStruct>(node->children.back()->typeInfo, TypeInfoKind::Struct);
        constexpr auto MaxSize   = SWAG_LIMIT_CLOSURE_SIZEOF - 2 * sizeof(void*);
        SWAG_VERIFY(typeBlock->sizeOf <= MaxSize, context->report({node->lambda->captureParameters, FMT(Err(Err0026), typeBlock->sizeOf, MaxSize)}));
    }

    return true;
}

bool Semantic::resolveMakePointer(SemanticContext* context)
{
    const auto node     = castAst<AstMakePointer>(context->node, AstNodeKind::MakePointer);
    const auto child    = node->children.front();
    auto       typeInfo = child->typeInfo;

    if (!child->resolvedSymbolName)
    {
        Diagnostic err{node, node->token, Err(Err0187)};
        err.addNote(child, Diagnostic::isType(typeInfo));
        return context->report(err);
    }

    if (child->resolvedSymbolOverload)
    {
        ScopedLock lk(child->resolvedSymbolName->mutex);
        child->resolvedSymbolOverload->flags.add(OVERLOAD_HAS_MAKE_POINTER);
    }

    if (child->resolvedSymbolOverload && child->resolvedSymbolOverload->hasFlag(OVERLOAD_IS_LET))
    {
        if (child->kind != AstNodeKind::IdentifierRef || child->children.back()->kind != AstNodeKind::ArrayPointerIndex)
        {
            const Diagnostic err{node, node->token, Err(Err0185)};
            return context->report(err, Diagnostic::hereIs(child->resolvedSymbolOverload->node));
        }
    }

    if (child->resolvedSymbolName->kind == SymbolKind::Function)
    {
        // For a function, if no parameters, then this is for a lambda
        const auto back = child->children.back();
        if (back->kind != AstNodeKind::FuncCall)
        {
            if (back->kind != AstNodeKind::Identifier)
                return resolveMakePointerLambda(context);

            const auto idBack = castAst<AstIdentifier>(back, AstNodeKind::Identifier);
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
                const Diagnostic err{node, node->token, FMT(Err(Err0178), typeInfo->getDisplayNameC())};
                return context->report(err, Diagnostic::hereIs(child->resolvedSymbolOverload));
            }

            const Diagnostic err{node, node->token, FMT(Err(Err0182), typeInfo->getDisplayNameC())};
            const auto       note = Diagnostic::note(FMT(Nte(Nte0100), Naming::aKindName(typeInfo).c_str()));
            return context->report(err, Diagnostic::hereIs(child->resolvedSymbolOverload), note);
        }
    }

    SWAG_CHECK(checkCanTakeAddress(context, child));
    SWAG_CHECK(checkIsConcrete(context, child));
    node->addAstFlag(AST_R_VALUE);
    node->resolvedSymbolName     = child->resolvedSymbolName;
    node->resolvedSymbolOverload = child->resolvedSymbolOverload;
    node->byteCodeFct            = ByteCodeGen::emitMakePointer;
    node->inheritComputedValue(child);

    // A new pointer
    bool          forceConst = false;
    TypeInfoFlags ptrFlags   = 0;

    // Transform a reference pointer to a pointer to the pointed value
    if (typeInfo->isPointerRef())
    {
        forceConst = typeInfo->isConst();
        typeInfo   = TypeManager::concretePtrRef(typeInfo);
        child->addSemFlag(SEMFLAG_FORCE_NO_TAKE_ADDRESS);
        child->children.back()->addSemFlag(SEMFLAG_FORCE_NO_TAKE_ADDRESS);
        node->byteCodeFct = ByteCodeGen::emitPassThrough;
    }

    // If this is an array, then this is legit, the pointer will address the first
    // element : need to find it's type
    else if (typeInfo->isArray())
    {
        while (typeInfo->isArray())
        {
            const auto typeArray = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            typeInfo             = typeArray->pointedType;
        }

        ptrFlags.add(TYPEINFO_POINTER_ARITHMETIC);
    }

    // :PointerArithmetic
    // Taking the address of an array element is ok for pointer arithmetic
    if (child->kind == AstNodeKind::IdentifierRef)
    {
        const auto last = child->children.back();
        if (last->kind == AstNodeKind::ArrayPointerIndex)
            ptrFlags.add(TYPEINFO_POINTER_ARITHMETIC);
    }

    // Taking the address of a const is const
    if (child->hasAstFlag(AST_IS_CONST) || forceConst)
    {
        ptrFlags.add(TYPEINFO_CONST);
    }

    // Type is constant if we take address of a readonly variable
    else if (child->resolvedSymbolOverload && !child->typeInfo->isPointerRef())
    {
        const auto typeResolved = child->resolvedSymbolOverload->typeInfo->getConcreteAlias();

        if (child->resolvedSymbolOverload->hasFlag(OVERLOAD_CONST_ASSIGN) &&
            !typeResolved->isPointerArithmetic() &&
            !typeResolved->isArray() &&
            !typeResolved->isSlice())
        {
            ptrFlags.add(TYPEINFO_CONST);
        }
        else if (typeResolved->isConst() && typeResolved->isSlice())
        {
            ptrFlags.add(TYPEINFO_CONST);
        }
        else if (node->hasAstFlag(AST_IS_CONST))
        {
            ptrFlags.add(TYPEINFO_CONST);
        }
    }

    // Transform pointer to a reference
    if (node->hasSpecFlag(AstMakePointer::SPEC_FLAG_TO_REF))
    {
        ptrFlags.add(TYPEINFO_POINTER_REF);
    }

    const auto ptrType = g_TypeMgr->makePointerTo(typeInfo, ptrFlags);
    node->typeInfo     = ptrType;
    return true;
}

bool Semantic::resolveArrayPointerSlicingUpperBound(SemanticContext* context)
{
    const auto arrayNode = context->node;
    const auto slicing   = castAst<AstArrayPointerSlicing>(context->node->parent, AstNodeKind::ArrayPointerSlicing);
    const auto upperNode = slicing->upperBound;

    context->node = upperNode;
    SWAG_CHECK(resolveIntrinsicCountOf(context, upperNode, arrayNode));
    context->node = arrayNode;
    YIELD();

    if (upperNode->hasFlagComputedValue())
    {
        upperNode->computedValue()->reg.u64 -= 1;
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
    auto node         = castAst<AstArrayPointerSlicing>(context->node, AstNodeKind::ArrayPointerSlicing);
    auto typeVar      = getConcreteTypeUnRef(node->array, CONCRETE_ALL);
    typeVar           = TypeManager::concreteType(typeVar);
    uint64_t maxBound = 0;

    {
        PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, [node]
        {
            return FMT(Nte(Nte0135), node->lowerBound->typeInfo->getDisplayNameC());
        });
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, node->lowerBound, CAST_FLAG_TRY_COERCE));
    }
    {
        PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, [node]
        {
            return FMT(Nte(Nte0135), node->lowerBound->typeInfo->getDisplayNameC());
        });
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, node->upperBound, CAST_FLAG_TRY_COERCE));
    }

    // Exclude upper bound if constant
    if (node->upperBound->hasFlagComputedValue() &&
        node->hasSpecFlag(AstArrayPointerSlicing::SPEC_FLAG_EXCLUDE_UP) &&
        !node->upperBound->hasSemFlag(SEMFLAG_ASSIGN_COMPUTED))
    {
        if (!node->upperBound->computedValue()->reg.u64)
        {
            const Diagnostic err{node->upperBound, Err(Err0612)};
            return context->report(err);
        }

        node->upperBound->addSemFlag(SEMFLAG_ASSIGN_COMPUTED);
        node->upperBound->computedValue()->reg.u64 -= 1;
    }

    // Slicing of an array
    if (typeVar->isArray())
    {
        const auto typeInfoArray = castTypeInfo<TypeInfoArray>(node->array->typeInfo, TypeInfoKind::Array);
        if (typeInfoArray->totalCount != typeInfoArray->count)
        {
            Diagnostic err{node, node->token, Err(Err0376)};
            err.addNote(node->array, Diagnostic::isType(typeInfoArray));
            return context->report(err);
        }

        const auto ptrSlice   = makeType<TypeInfoSlice>();
        ptrSlice->pointedType = typeInfoArray->finalType;
        if (typeInfoArray->isConst())
            ptrSlice->addFlag(TYPEINFO_CONST);
        ptrSlice->computeName();
        node->typeInfo = ptrSlice;
        maxBound       = typeInfoArray->count - 1;
    }

    // Slicing of a string
    else if (typeVar->isString())
    {
        node->typeInfo = typeVar;
        if (node->array->hasFlagComputedValue())
            maxBound = node->array->computedValue()->text.length() - 1;
    }

    // Slicing of a pointer
    else if (typeVar->isPointer())
    {
        // :PointerArithmetic
        if (!typeVar->isPointerArithmetic())
        {
            Diagnostic err{node, node->token, Err(Err0361)};
            err.addNote(node->array, Diagnostic::isType(typeVar));
            return context->report(err);
        }

        const auto typeInfoPointer = castTypeInfo<TypeInfoPointer>(node->array->typeInfo, TypeInfoKind::Pointer);
        const auto ptrSlice        = makeType<TypeInfoSlice>();
        ptrSlice->pointedType      = typeInfoPointer->pointedType;
        if (typeInfoPointer->isConst())
            ptrSlice->addFlag(TYPEINFO_CONST);
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
        // Flatten all operator parameters : self, then all indexes
        node->structFlatParams.clear();
        node->structFlatParams.push_back(node->lowerBound);
        node->structFlatParams.push_back(node->upperBound);

        // Self in first position
        node->structFlatParams.push_front(node->array);

        // Resolve call
        const auto  typeInfo = node->array->typeInfo;
        SymbolName* symbol   = nullptr;
        SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opSlice, node->array, &symbol));
        YIELD();
        if (!symbol)
        {
            Diagnostic err{node->sourceFile, node->token, FMT(Err(Err0374), node->array->token.c_str(), typeInfo->getDisplayNameC())};
            err.hint = FMT(Nte(Nte0144), g_LangSpec->name_opSlice.c_str());
            err.addNote(node->array, Diagnostic::isType(typeInfo));
            return context->report(err);
        }

        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opSlice, nullptr, nullptr, node->array, node->structFlatParams));
        YIELD();
    }
    else
    {
        return context->report({node->array, FMT(Err(Err0375), node->array->typeInfo->getDisplayNameC())});
    }

    // startBound <= endBound
    if (node->lowerBound->hasFlagComputedValue() && node->upperBound->hasFlagComputedValue())
    {
        if (node->lowerBound->computedValue()->reg.u64 > node->upperBound->computedValue()->reg.u64)
        {
            Diagnostic err{node->lowerBound, FMT(Err(Err0611), node->lowerBound->computedValue()->reg.u64, node->upperBound->computedValue()->reg.u64)};
            err.addNote(node->upperBound, Nte(Nte0172));
            return context->report(err);
        }
    }

    // endBound < maxBound
    if (maxBound && node->upperBound->hasFlagComputedValue())
    {
        if (node->upperBound->computedValue()->reg.u64 > maxBound)
        {
            const Diagnostic err{node->upperBound, FMT(Err(Err0610), node->upperBound->computedValue()->reg.u64, maxBound)};
            return context->report(err);
        }
    }

    node->byteCodeFct = ByteCodeGen::emitMakeArrayPointerSlicing;
    return true;
}

bool Semantic::resolveMoveRef(SemanticContext* context)
{
    const auto node  = context->node;
    const auto front = node->children.front();
    node->inheritAstFlagsAnd(AST_CONST_EXPR);

    auto typeInfo = TypeManager::concreteType(front->typeInfo);

    if (front->hasFlagComputedValue())
    {
        const Diagnostic err(node, node->token, Err(Err0464));
        return context->report(err);
    }

    if (!typeInfo->isPointer() && !typeInfo->isPointerRef())
    {
        Diagnostic err(node, node->token, FMT(Err(Err0466), front->typeInfo->getDisplayNameC()));
        err.addNote(front, Diagnostic::isType(front->typeInfo));
        return context->report(err);
    }

    if (typeInfo->isConst())
    {
        Diagnostic err(node, node->token, Err(Err0465));
        err.addNote(front, Diagnostic::isType(front->typeInfo));
        return context->report(err);
    }

    typeInfo = typeInfo->clone();
    typeInfo->addFlag(TYPEINFO_POINTER_REF | TYPEINFO_POINTER_ACCEPT_MOVE_REF);
    typeInfo->forceComputeName();

    node->typeInfo    = typeInfo;
    node->byteCodeFct = ByteCodeGen::emitPassThrough;
    return true;
}

bool Semantic::resolveKeepRef(SemanticContext* context)
{
    const auto node  = context->node;
    const auto front = node->children.front();
    node->inheritAstFlagsAnd(AST_CONST_EXPR);

    auto typeInfo = TypeManager::concreteType(front->typeInfo);
    if (!typeInfo->isPointerRef() && !typeInfo->isPointer())
    {
        Diagnostic err{node, node->token, FMT(Err(Err0365), typeInfo->getDisplayNameC())};

        if (front->kind == AstNodeKind::IdentifierRef && front->children.front()->kind == AstNodeKind::ArrayPointerIndex)
        {
            err.addNote(front, Nte(Nte0026));
            return context->report(err);
        }

        if (front->kind == AstNodeKind::IdentifierRef)
        {
            err.hint        = Nte(Nte0129);
            const auto note = Diagnostic::note(front, FMT(Nte(Nte0195), front->token.c_str()));
            return context->report(err, note);
        }

        err.addNote(front, Diagnostic::isType(typeInfo));
        err.hint = Nte(Nte0129);
        return context->report(err);
    }

    if (!typeInfo->isPointerRef())
    {
        typeInfo = typeInfo->clone();
        typeInfo->addFlag(TYPEINFO_POINTER_REF);
        if (node->hasAstFlag(AST_IS_CONST))
            typeInfo->setConst();
        typeInfo->forceComputeName();
    }
    else if (node->hasAstFlag(AST_IS_CONST))
    {
        typeInfo = g_TypeMgr->makeConst(typeInfo);
    }

    node->typeInfo    = typeInfo;
    node->byteCodeFct = ByteCodeGen::emitPassThrough;
    return true;
}

bool Semantic::resolveArrayPointerIndex(SemanticContext* context)
{
    const auto node = castAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (node->isForceTakeAddress())
        SWAG_CHECK(resolveArrayPointerRef(context));
    else
        SWAG_CHECK(resolveArrayPointerDeRef(context));
    YIELD();

    if (!node->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_IS_DEREF))
        node->inheritAstFlagsAnd(AST_CONST_EXPR);

    // If this is not the last child of the IdentifierRef, then this is a reference, and
    // we must take the address and not dereference that identifier
    if (node->parent->kind == AstNodeKind::IdentifierRef)
    {
        const auto parent = castAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);
        if (node != parent->children.back())
        {
            // The last ArrayPointerIndex in a list [0, 0, 0] must dereference
            if (node->children[0]->kind != AstNodeKind::ArrayPointerIndex)
                node->addSemFlag(SEMFLAG_FORCE_TAKE_ADDRESS);

            // In order to resolve what's next, we need to fill the startScope of the identifier ref
            auto typeReturn = node->array->typeInfo;

            // There's a 'opIndex' function
            if (node->hasExtMisc() && node->extMisc()->resolvedUserOpSymbolOverload)
                typeReturn = TypeManager::concreteType(node->extMisc()->resolvedUserOpSymbolOverload->typeInfo);

            // Unref
            if (typeReturn->isPointerRef() && !node->hasSemFlag(SEMFLAG_FROM_REF))
            {
                const auto returnType = castTypeInfo<TypeInfoPointer>(typeReturn, TypeInfoKind::Pointer)->pointedType;
                if (returnType->isPointer())
                    node->addSemFlag(SEMFLAG_FROM_PTR_REF);
            }

            typeReturn = TypeManager::concretePtrRefType(typeReturn);

            // Get the pointed type if we have a pointer
            if (typeReturn->isPointer())
            {
                const auto typePointer = castTypeInfo<TypeInfoPointer>(typeReturn, TypeInfoKind::Pointer);
                typeReturn             = typePointer->pointedType;
            }

            // And if this is a struct or an interface, we fill the startScope
            if (typeReturn->isStruct() || typeReturn->isInterface())
            {
                const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeReturn, TypeInfoKind::Struct, TypeInfoKind::Interface);
                parent->startScope    = typeStruct->scope;
            }
        }

        parent->previousResolvedNode = node;
    }

    return true;
}

bool Semantic::resolveArrayPointerRef(SemanticContext* context)
{
    const auto arrayNode              = castAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    arrayNode->resolvedSymbolName     = arrayNode->array->resolvedSymbolName;
    arrayNode->resolvedSymbolOverload = arrayNode->array->resolvedSymbolOverload;
    arrayNode->inheritAstFlagsOr(arrayNode->array, AST_L_VALUE);

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));
    arrayNode->addAstFlag(AST_R_VALUE);

    const auto baseType  = arrayNode->array->typeInfo;
    const auto arrayType = TypeManager::concretePtrRefType(baseType, CONCRETE_FORCE_ALIAS | CONCRETE_FUNC);

    // When we are building a pointer, this is fine to be const, because in fact we do no generate an address to modify the content
    // (or it will be done later on a pointer, and it will be const too)
    if (arrayNode->parent->parent->kind != AstNodeKind::MakePointer)
    {
        if (baseType->isConst())
        {
            Diagnostic err{arrayNode->parent->parent, arrayNode->parent->parent->token, FMT(Err(Err0104), baseType->getDisplayNameC())};
            err.addNote(arrayNode->array->token, Diagnostic::isType(baseType));
            return context->report(err);
        }
    }
    else
    {
        // If array is const, inform the make pointer that it need to make a const pointer
        if (baseType->isConst())
            arrayNode->parent->parent->addAstFlag(AST_IS_CONST);
    }

    const auto accessType = TypeManager::concreteType(arrayNode->access->typeInfo);
    if (!arrayType->isStruct())
    {
        if (!accessType->isNativeInteger() && !accessType->hasFlag(TYPEINFO_ENUM_INDEX))
        {
            const Diagnostic err{arrayNode->access, FMT(Err(Err0740), arrayNode->access->typeInfo->getDisplayNameC())};
            return context->report(err);
        }
    }

    switch (arrayType->kind)
    {
        case TypeInfoKind::Pointer:
        {
            if (!arrayType->isPointerArithmetic() && !arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_IS_DEREF))
            {
                const Diagnostic err{arrayNode->array, FMT(Err(Err0256), arrayNode->resolvedSymbolName->name.c_str(), arrayType->getDisplayNameC())};
                return context->report(err);
            }

            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CAST_FLAG_TRY_COERCE | CAST_FLAG_INDEX));
            const auto typePtr = castTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);

            if (typePtr->pointedType->isVoid())
            {
                Diagnostic err{arrayNode->access, Err(Err0421)};
                err.addNote(arrayNode->array, Diagnostic::isType(typePtr));
                return context->report(err);
            }

            arrayNode->typeInfo = typePtr->pointedType;
            arrayNode->addAstFlag(AST_ARRAY_POINTER_REF);
            arrayNode->array->addAstFlag(AST_ARRAY_POINTER_REF);
            arrayNode->byteCodeFct = ByteCodeGen::emitPointerRef;
            break;
        }

        case TypeInfoKind::Native:
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CAST_FLAG_TRY_COERCE | CAST_FLAG_INDEX));
            if (arrayType->nativeType == NativeTypeKind::String)
            {
                arrayNode->typeInfo    = g_TypeMgr->typeInfoU8;
                arrayNode->byteCodeFct = ByteCodeGen::emitStringRef;
                arrayNode->addAstFlag(AST_IS_CONST);
            }
            else
            {
                Diagnostic err{arrayNode->array, FMT(Err(Err0260), arrayType->getDisplayNameC())};
                if (arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_IS_DEREF))
                    err.addNote(arrayNode->token.startLocation, arrayNode->token.endLocation, Nte(Nte0112));
                return context->report(err);
            }

            break;
        }

        case TypeInfoKind::Array:
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CAST_FLAG_TRY_COERCE | CAST_FLAG_INDEX));
            const auto typeArray   = castTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);
            arrayNode->typeInfo    = typeArray->pointedType;
            arrayNode->byteCodeFct = ByteCodeGen::emitArrayRef;

            // Try to dereference as a constant if we can
            uint32_t     storageOffset  = UINT32_MAX;
            DataSegment* storageSegment = nullptr;
            SWAG_CHECK(getConstantArrayPtr(context, &storageOffset, &storageSegment));
            if (storageSegment)
            {
                arrayNode->setFlagsValueIsComputed();
                arrayNode->computedValue()->storageOffset  = storageOffset;
                arrayNode->computedValue()->storageSegment = storageSegment;
            }
            else
            {
                SWAG_CHECK(boundCheck(context, arrayType, arrayNode->array, arrayNode->access, typeArray->count));
            }
            break;
        }

        case TypeInfoKind::Slice:
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CAST_FLAG_TRY_COERCE | CAST_FLAG_INDEX));
            const auto typePtr     = castTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
            arrayNode->typeInfo    = typePtr->pointedType;
            arrayNode->byteCodeFct = ByteCodeGen::emitSliceRef;
            break;
        }

        case TypeInfoKind::Struct:
            arrayNode->access->typeInfo = getConcreteTypeUnRef(arrayNode->access, CONCRETE_FUNC | CONCRETE_ALIAS);

            if (arrayType->isTuple())
            {
                if (arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_IS_DEREF))
                {
                    Diagnostic err{arrayNode->access, Err(Err0382)};
                    err.addNote(arrayNode->token.startLocation, arrayNode->token.endLocation, Nte(Nte0112));
                    return context->report(err);
                }

                Diagnostic err{arrayNode->access, Err(Err0382)};
                err.addNote(arrayNode->array, Diagnostic::isType(arrayType));
                return context->report(err);
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
                // Flatten all indexes. self and value will be set before the call later
                // Can be already done, so do not overwrite
                // :StructFlatParamsDone
                if (arrayNode->structFlatParams.empty())
                {
                    arrayNode->structFlatParams.push_back(arrayNode->access);

                    AstNode* child = arrayNode->array;
                    while (child->kind == AstNodeKind::ArrayPointerIndex)
                    {
                        const auto arrayChild = castAst<AstArrayPointerIndex>(child, AstNodeKind::ArrayPointerIndex);
                        arrayNode->structFlatParams.push_front(arrayChild->access);
                        child = arrayChild->array;
                    }
                }
            }
            break;

        default:
        {
            return context->report({arrayNode->array, FMT(Err(Err0260), arrayType->getDisplayNameC())});
        }
    }

    return true;
}

bool Semantic::getConstantArrayPtr(SemanticContext* context, uint32_t* storageOffset, DataSegment** storageSegment)
{
    const auto arrayNode = castAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    const auto arrayType = TypeManager::concreteType(arrayNode->array->typeInfo);
    const auto typePtr   = castTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);

    if (!arrayNode->typeInfo->isArray() && arrayNode->access->hasFlagComputedValue())
    {
        bool     isConstAccess = true;
        uint64_t offsetAccess  = arrayNode->access->computedValue()->reg.u64 * typePtr->finalType->sizeOf;
        SWAG_CHECK(boundCheck(context, arrayType, arrayNode->array, arrayNode->access, typePtr->count));

        // Deal with array of array
        auto subArray = arrayNode;
        while (isConstAccess && subArray->array->kind == AstNodeKind::ArrayPointerIndex)
        {
            subArray      = castAst<AstArrayPointerIndex>(subArray->array, AstNodeKind::ArrayPointerIndex);
            isConstAccess = isConstAccess && subArray->access->hasFlagComputedValue();
            if (isConstAccess)
            {
                if (subArray->array->typeInfo->kind != TypeInfoKind::Array)
                    isConstAccess = false;
                else
                {
                    const auto subTypePtr = castTypeInfo<TypeInfoArray>(subArray->array->typeInfo, TypeInfoKind::Array);
                    SWAG_CHECK(boundCheck(context, subArray->array->typeInfo, subArray->array, subArray->access, subTypePtr->count));
                    offsetAccess += subArray->access->computedValue()->reg.u64 * subTypePtr->pointedType->sizeOf;
                }
            }
        }

        if (isConstAccess)
        {
            const auto overload = subArray->array->resolvedSymbolOverload;
            if (overload && overload->hasFlag(OVERLOAD_COMPUTED_VALUE))
            {
                SWAG_ASSERT(overload->computedValue.storageOffset != UINT32_MAX);
                SWAG_ASSERT(overload->computedValue.storageSegment);
                *storageOffset  = overload->computedValue.storageOffset + static_cast<uint32_t>(offsetAccess);
                *storageSegment = overload->computedValue.storageSegment;
                return true;
            }

            if (subArray->array->hasFlagComputedValue())
            {
                SWAG_ASSERT(subArray->array->hasFlagComputedValue());
                SWAG_ASSERT(subArray->array->computedValue()->storageOffset != UINT32_MAX);
                SWAG_ASSERT(subArray->array->computedValue()->storageSegment);
                *storageOffset  = subArray->array->computedValue()->storageOffset + static_cast<uint32_t>(offsetAccess);
                *storageSegment = subArray->array->computedValue()->storageSegment;
                return true;
            }
        }
    }

    return true;
}

bool Semantic::resolveArrayPointerDeRef(SemanticContext* context)
{
    const auto arrayNode   = castAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    const auto arrayAccess = arrayNode->access;
    const auto arrayType   = getConcreteTypeUnRef(arrayNode->array, CONCRETE_ALL);
    arrayNode->byteCodeFct = ByteCodeGen::emitPointerDeRef;

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));

    if (arrayType->isTuple())
    {
        if (arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_IS_DEREF))
        {
            Diagnostic err{arrayNode->access, Err(Err0382)};
            err.addNote(arrayNode->token.startLocation, arrayNode->token.endLocation, Nte(Nte0112));
            return context->report(err);
        }

        Diagnostic err{arrayNode->access, Err(Err0382)};
        err.addNote(arrayNode->array, Diagnostic::isType(arrayType));
        return context->report(err);
    }

    arrayNode->addAstFlag(AST_R_VALUE);

    const auto accessType = getConcreteTypeUnRef(arrayNode->access, CONCRETE_ALL);
    if (!arrayType->isStruct())
    {
        if (!accessType->isNativeInteger() && !accessType->hasFlag(TYPEINFO_ENUM_INDEX))
        {
            const Diagnostic err{arrayNode->access, FMT(Err(Err0740), arrayNode->access->typeInfo->getDisplayNameC())};
            return context->report(err);
        }
    }

    // Do not set resolvedSymbolOverload !
    arrayNode->resolvedSymbolName = arrayNode->array->resolvedSymbolName;

    // Can we dereference at compile time ?
    if (arrayType->isString())
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CAST_FLAG_TRY_COERCE | CAST_FLAG_INDEX));
        if (arrayNode->access->hasFlagComputedValue())
        {
            if (arrayNode->array->resolvedSymbolOverload && arrayNode->array->resolvedSymbolOverload->hasFlag(OVERLOAD_COMPUTED_VALUE))
            {
                arrayNode->setFlagsValueIsComputed();
                const auto& text = arrayNode->array->resolvedSymbolOverload->computedValue.text;
                SWAG_CHECK(boundCheck(context, arrayType, arrayNode->array, arrayNode->access, text.length()));
                const auto idx                     = arrayAccess->computedValue()->reg.u32;
                arrayNode->computedValue()->reg.u8 = text[idx];
            }
        }

        arrayNode->typeInfo = g_TypeMgr->typeInfoU8;
        return true;
    }

    switch (arrayType->kind)
    {
        case TypeInfoKind::Pointer:
        {
            if (!arrayType->isPointerArithmetic() && !arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_IS_DEREF))
            {
                Diagnostic err{arrayNode->access, FMT(Err(Err0256), arrayNode->resolvedSymbolName->name.c_str(), arrayType->getDisplayNameC())};
                err.addNote(arrayNode->array, Diagnostic::isType(arrayType));
                return context->report(err, Diagnostic::note(Nte(Nte0103)));
            }

            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CAST_FLAG_TRY_COERCE | CAST_FLAG_INDEX));
            const auto typePtr = castTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);

            if (typePtr->pointedType->isVoid())
            {
                Diagnostic err{arrayNode->access, Err(Err0421)};
                err.addNote(arrayNode->array, Diagnostic::isType(typePtr));
                return context->report(err);
            }

            arrayNode->typeInfo = typePtr->pointedType;
            setupIdentifierRef(context, arrayNode);

            // Try to dereference as a constant if we can
            if (arrayNode->array->hasFlagComputedValue() && arrayNode->access->hasFlagComputedValue())
            {
                const auto storageSegment = arrayNode->array->computedValue()->storageSegment;
                if (!storageSegment)
                {
                    const Diagnostic err{arrayNode, Err(Err0599)};
                    return context->report(err);
                }

                const auto storageOffset = arrayNode->array->computedValue()->storageOffset;
                SWAG_ASSERT(storageOffset != UINT32_MAX);

                const auto offset = arrayNode->access->computedValue()->reg.u32 * typePtr->pointedType->sizeOf;
                const auto ptr    = storageSegment->address(storageOffset + offset);
                if (!derefConstantValue(context, arrayNode, typePtr->pointedType, storageSegment, ptr))
                {
                    if (typePtr->pointedType->isStruct())
                    {
                        arrayNode->setFlagsValueIsComputed();
                        arrayNode->computedValue()->storageSegment = storageSegment;
                        arrayNode->computedValue()->storageOffset  = storageOffset;
                        arrayNode->typeInfo                        = typePtr->pointedType;
                    }
                }
            }

            break;
        }

        case TypeInfoKind::Array:
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CAST_FLAG_TRY_COERCE | CAST_FLAG_INDEX));
            const auto typePtr  = castTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);
            arrayNode->typeInfo = typePtr->pointedType;
            setupIdentifierRef(context, arrayNode);

            // Try to dereference as a constant if we can
            uint32_t     storageOffset  = UINT32_MAX;
            DataSegment* storageSegment = nullptr;
            SWAG_CHECK(getConstantArrayPtr(context, &storageOffset, &storageSegment));
            if (storageSegment)
            {
                const auto ptr = storageSegment->address(storageOffset);
                if (!derefConstantValue(context, arrayNode, typePtr->finalType, storageSegment, ptr))
                {
                    if (typePtr->finalType->isStruct())
                    {
                        arrayNode->setFlagsValueIsComputed();
                        arrayNode->computedValue()->storageSegment = storageSegment;
                        arrayNode->computedValue()->storageOffset  = storageOffset;
                        arrayNode->typeInfo                        = typePtr->finalType;
                    }
                }
            }

            break;
        }

        case TypeInfoKind::Slice:
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CAST_FLAG_TRY_COERCE | CAST_FLAG_INDEX));
            const auto typeSlice = castTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
            arrayNode->typeInfo  = typeSlice->pointedType;
            setupIdentifierRef(context, arrayNode);

            // Try to dereference as a constant if we can
            if (arrayNode->access->hasFlagComputedValue())
            {
                if (arrayNode->array->resolvedSymbolOverload && arrayNode->array->resolvedSymbolOverload->hasFlag(OVERLOAD_COMPUTED_VALUE))
                {
                    const auto& computedValue = arrayNode->array->resolvedSymbolOverload->computedValue;
                    const auto  slice         = static_cast<SwagSlice*>(computedValue.getStorageAddr());
                    SWAG_CHECK(boundCheck(context, arrayType, arrayNode->array, arrayNode->access, slice->count));

                    auto ptr = static_cast<uint8_t*>(slice->buffer);
                    ptr += arrayNode->access->computedValue()->reg.u64 * typeSlice->pointedType->sizeOf;
                    derefConstantValue(context, arrayNode, typeSlice->pointedType, computedValue.storageSegment, ptr);
                }
            }

            break;
        }

        case TypeInfoKind::Variadic:
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CAST_FLAG_TRY_COERCE | CAST_FLAG_INDEX));
            arrayNode->typeInfo = g_TypeMgr->typeInfoAny;
            break;

        case TypeInfoKind::TypedVariadic:
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, arrayNode->access, CAST_FLAG_TRY_COERCE | CAST_FLAG_INDEX));
            const auto typeVariadic = castTypeInfo<TypeInfoVariadic>(arrayType, TypeInfoKind::TypedVariadic);
            arrayNode->typeInfo     = typeVariadic->rawType;
            setupIdentifierRef(context, arrayNode);
            break;
        }

        case TypeInfoKind::Struct:
        {
            arrayNode->access->typeInfo = getConcreteTypeUnRef(arrayNode->access, CONCRETE_FUNC | CONCRETE_ALIAS);

            // Only the top level ArrayPointerIndex node (for a given serial) will deal with the call
            if (arrayNode->parent->kind == AstNodeKind::ArrayPointerIndex &&
                arrayNode->parent->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_SERIAL) == arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_SERIAL))
            {
                arrayNode->typeInfo = arrayType;
                break;
            }

            // Flatten all operator parameters : self, then all indexes
            // :StructFlatParamsDone
            arrayNode->structFlatParams.clear();
            arrayNode->structFlatParams.push_back(arrayNode->access);

            const auto serial = arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_SERIAL);
            AstNode*   child  = arrayNode->array;
            while (child->kind == AstNodeKind::ArrayPointerIndex &&
                   child->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_SERIAL) == serial)
            {
                const auto arrayChild = castAst<AstArrayPointerIndex>(child, AstNodeKind::ArrayPointerIndex);
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
                    Diagnostic err{arrayNode->access, FMT(Err(Err0259), arrayType->getDisplayNameC())};
                    err.hint = FMT(Nte(Nte0144), g_LangSpec->name_opIndex.c_str());
                    err.addNote(arrayNode->array, Diagnostic::isType(arrayType));
                    return context->report(err);
                }

                Diagnostic err{arrayNode->access, FMT(Err(Err0258), arrayNode->array->token.c_str(), arrayType->getDisplayNameC())};
                err.hint = FMT(Nte(Nte0144), g_LangSpec->name_opIndex.c_str());
                err.addNote(arrayNode->array, Diagnostic::isType(arrayType));
                return context->report(err);
            }

            SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndex, nullptr, nullptr, arrayNode->array, arrayNode->structFlatParams));
            break;
        }

        default:
        {
            Diagnostic err{arrayNode->array, FMT(Err(Err0260), arrayNode->array->typeInfo->getDisplayNameC())};
            if (arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_IS_DEREF))
                err.addNote(arrayNode->token.startLocation, arrayNode->token.endLocation, Nte(Nte0112));
            return context->report(err);
        }
    }

    return true;
}

bool Semantic::checkInitDropCount(SemanticContext* context, const AstNode* node, AstNode* expression, AstNode* count)
{
    if (!count)
        return true;

    const auto countTypeInfo = TypeManager::concreteType(count->typeInfo);
    SWAG_VERIFY(countTypeInfo->isNativeInteger(), context->report({count, FMT(Err(Err0194), node->token.c_str(), countTypeInfo->getDisplayNameC())}));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, nullptr, count, CAST_FLAG_TRY_COERCE));

    if (!count->hasFlagComputedValue() || count->computedValue()->reg.u64 > 1)
    {
        if (!expression->typeInfo->isPointerArithmetic())
        {
            if (count->hasFlagComputedValue())
            {
                Diagnostic err{expression, FMT(Err(Err0197), node->token.c_str(), expression->typeInfo->getDisplayNameC())};
                err.addNote(count, FMT(Nte(Nte0127), count->computedValue()->reg.u64));
                return context->report(err);
            }

            Diagnostic err{expression, FMT(Err(Err0198), node->token.c_str(), expression->typeInfo->getDisplayNameC())};
            err.addNote(count, Nte(Nte0128));
            return context->report(err);
        }
    }

    return true;
}

bool Semantic::resolveInit(SemanticContext* context)
{
    const auto node               = castAst<AstInit>(context->node, AstNodeKind::Init);
    auto       expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    if (!node->count)
    {
        expressionTypeInfo = getConcreteTypeUnRef(node->expression, CONCRETE_ALIAS);
        SWAG_VERIFY(node->expression->kind == AstNodeKind::IdentifierRef, context->report({node->expression, FMT(Err(Err0199), node->token.c_str())}));
        SWAG_VERIFY(node->expression->resolvedSymbolOverload, context->report({node->expression, FMT(Err(Err0199), node->token.c_str())}));
        SWAG_VERIFY(!expressionTypeInfo->isConst(), context->report({node->expression, FMT(Err(Err0057), node->token.c_str(), expressionTypeInfo->getDisplayNameC())}));
        const auto back = node->expression->children.back();
        back->addSemFlag(SEMFLAG_FORCE_TAKE_ADDRESS);
        back->resolvedSymbolOverload->flags.add(OVERLOAD_HAS_MAKE_POINTER);
    }
    else
    {
        SWAG_VERIFY(expressionTypeInfo->isPointer(), context->report({node->expression, FMT(Err(Err0196), node->token.c_str(), expressionTypeInfo->getDisplayNameC())}));
        SWAG_VERIFY(!node->expression->typeInfo->isConst(), context->report({node->expression, FMT(Err(Err0056), node->token.c_str(), expressionTypeInfo->getDisplayNameC())}));
        SWAG_CHECK(checkInitDropCount(context, node, node->expression, node->count));
    }

    if (node->parameters)
    {
        TypeInfo* pointedType = nullptr;
        if (node->count)
        {
            const auto typeinfoPointer = castTypeInfo<TypeInfoPointer>(expressionTypeInfo, TypeInfoKind::Pointer);
            pointedType                = typeinfoPointer->pointedType;
        }
        else
        {
            pointedType = expressionTypeInfo;
            if (pointedType->isArray())
                pointedType = castTypeInfo<TypeInfoArray>(pointedType)->finalType;
        }

        if (pointedType->isNative() || pointedType->isPointer())
        {
            SWAG_VERIFY(node->parameters->children.size() == 1, context->report({node->parameters, FMT(Err(Err0637), pointedType->getDisplayNameC())}));
            const auto child = node->parameters->children.front();
            SWAG_CHECK(TypeManager::makeCompatibles(context, pointedType, child->typeInfo, nullptr, child));
        }
        else if (pointedType->isStruct())
        {
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(pointedType, TypeInfoKind::Struct);

            SymbolMatchContext symMatchContext;
            symMatchContext.reset();
            for (auto child : node->parameters->children)
                symMatchContext.parameters.push_back(child);

            auto& listTryMatch = context->cacheListTryMatch;
            while (true)
            {
                context->clearTryMatch();
                const auto symbol = typeStruct->declNode->resolvedSymbolName;

                {
                    SharedLock lk(symbol->mutex);
                    for (const auto overload : symbol->overloads)
                    {
                        auto t               = context->getTryMatch();
                        t->symMatchContext   = symMatchContext;
                        t->overload          = overload;
                        t->genericParameters = nullptr;
                        t->callParameters    = node->parameters;
                        t->dependentVar      = nullptr;
                        t->cptOverloads      = symbol->overloads.size();
                        listTryMatch.push_back(t);
                    }
                }

                SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, node));
                if (context->result == ContextResult::Pending)
                    return true;
                if (context->result != ContextResult::NewChildren)
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
    const auto node               = castAst<AstDropCopyMove>(context->node, AstNodeKind::Drop, AstNodeKind::PostCopy, AstNodeKind::PostMove);
    const auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->isPointer(), context->report({node->expression, FMT(Err(Err0196), node->token.c_str(), expressionTypeInfo->getDisplayNameC())}));
    SWAG_VERIFY(!node->expression->typeInfo->isConst(), context->report({node->expression, FMT(Err(Err0056), node->token.c_str(), expressionTypeInfo->getDisplayNameC())}));
    SWAG_CHECK(checkInitDropCount(context, node, node->expression, node->count));

    // Be sure struct if not marked as nocopy
    if (node->kind == AstNodeKind::PostCopy)
    {
        const auto ptrType     = castTypeInfo<TypeInfoPointer>(expressionTypeInfo, TypeInfoKind::Pointer);
        const auto pointedType = TypeManager::concreteType(ptrType->pointedType);
        if (pointedType->hasFlag(TYPEINFO_STRUCT_NO_COPY))
        {
            return context->report({node->expression, FMT(Err(Err0101), pointedType->getDisplayNameC())});
        }
    }

    node->byteCodeFct = ByteCodeGen::emitDropCopyMove;
    return true;
}
