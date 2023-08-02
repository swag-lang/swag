#include "pch.h"
#include "TypeManager.h"
#include "Module.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Naming.h"

bool TypeManager::canOverflow(SemanticContext* context, AstNode* fromNode, uint64_t castFlags)
{
    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_CAN_OVERFLOW))
        return true;
    if (castFlags & CASTFLAG_COERCE)
        return false;
    if (fromNode && context->sourceFile && context->sourceFile->module && !context->sourceFile->module->mustEmitSafetyOverflow(fromNode, true))
        return true;
    return false;
}

bool TypeManager::errorOutOfRange(SemanticContext* context, AstNode* fromNode, TypeInfo* fromType, TypeInfo* toType, bool isNeg)
{
    if (isNeg)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::F32:
            return context->report({fromNode, Fmt(Err(Saf0030), fromNode->computedValue->reg.f32, toType->getDisplayNameC())});
        case NativeTypeKind::F64:
            return context->report({fromNode, Fmt(Err(Saf0030), fromNode->computedValue->reg.f64, toType->getDisplayNameC())});
        default:
            return context->report({fromNode, Fmt(Err(Saf0032), fromNode->computedValue->reg.s64, toType->getDisplayNameC())});
        }
    }

    if (fromNode->kind == AstNodeKind::Literal && fromNode->token.text.length() > 2)
    {
        if (std::tolower(fromNode->token.text[1]) == 'x' || std::tolower(fromNode->token.text[1]) == 'b')
        {
            return context->report({fromNode, Fmt(Err(Saf0029), fromNode->token.ctext(), fromNode->computedValue->reg.u64, toType->getDisplayNameC())});
        }
    }

    switch (fromType->nativeType)
    {
    case NativeTypeKind::F32:
        return context->report({fromNode, Fmt(Err(Saf0031), fromNode->computedValue->reg.f32, toType->getDisplayNameC())});
    case NativeTypeKind::F64:
        return context->report({fromNode, Fmt(Err(Saf0031), fromNode->computedValue->reg.f64, toType->getDisplayNameC())});
    default:
        return context->report({fromNode, Fmt(Err(Saf0033), fromNode->computedValue->reg.u64, toType->getDisplayNameC())});
    }
}

bool TypeManager::safetyComputedValue(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (!fromNode || !fromNode->hasComputedValue())
        return true;
    if (castFlags & CASTFLAG_JUST_CHECK)
        return true;
    if (!(castFlags & CASTFLAG_EXPLICIT))
        return true;
    if (!fromNode->sourceFile->module->mustEmitSafety(fromNode, SAFETY_OVERFLOW))
        return true;

    auto msg  = ByteCodeGenJob::safetyMsg(SafetyMsg::CastTruncated, toType, fromType);
    auto msg1 = ByteCodeGenJob::safetyMsg(SafetyMsg::CastNeg, toType, fromType);

    // Negative value to unsigned type
    if (fromType->isNativeIntegerSigned() && toType->isNativeIntegerUnsignedOrRune() && fromNode->computedValue->reg.s64 < 0)
        return context->report({fromNode ? fromNode : context->node, msg1});

    switch (toType->nativeType)
    {
    case NativeTypeKind::U8:
        if (fromNode->computedValue->reg.u64 > UINT8_MAX)
            return context->report({fromNode ? fromNode : context->node, msg});
        break;
    case NativeTypeKind::U16:
        if (fromNode->computedValue->reg.u64 > UINT16_MAX)
            return context->report({fromNode ? fromNode : context->node, msg});
        break;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        if (fromNode->computedValue->reg.u64 > UINT32_MAX)
            return context->report({fromNode ? fromNode : context->node, msg});
        break;
    case NativeTypeKind::U64:
        if (fromType->isNativeIntegerSigned())
        {
            if (fromNode->computedValue->reg.u64 > INT64_MAX)
                return context->report({fromNode ? fromNode : context->node, msg});
        }
        break;

    case NativeTypeKind::S8:
        if (fromNode->computedValue->reg.s64 < INT8_MIN || fromNode->computedValue->reg.s64 > INT8_MAX)
            return context->report({fromNode ? fromNode : context->node, msg});
        break;
    case NativeTypeKind::S16:
        if (fromNode->computedValue->reg.s64 < INT16_MIN || fromNode->computedValue->reg.s64 > INT16_MAX)
            return context->report({fromNode ? fromNode : context->node, msg});
        break;
    case NativeTypeKind::S32:
        if (fromNode->computedValue->reg.s64 < INT32_MIN || fromNode->computedValue->reg.s64 > INT32_MAX)
            return context->report({fromNode ? fromNode : context->node, msg});
        break;
    case NativeTypeKind::S64:
        if (!fromType->isNativeIntegerSigned())
        {
            if (fromNode->computedValue->reg.u64 > INT64_MAX)
                return context->report({fromNode ? fromNode : context->node, msg});
        }
        break;
    default:
        break;
    }

    return true;
}

bool TypeManager::tryOpAffect(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    auto structType = toType;

    if (castFlags & CASTFLAG_UFCS && structType->isPointerTo(TypeInfoKind::Struct))
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType   = typePtr->pointedType;
    }

    bool isMoveRef = false;

    // Cast to a const reference, must take the pointed struct
    if (structType->isConstPointerRef() && castFlags & CASTFLAG_PARAMS)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType   = typePtr->pointedType;
        toType       = structType;
    }

    // We can also match a moveref with an opAffect
    else if (structType->isPointerMoveRef() && castFlags & CASTFLAG_PARAMS)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType   = typePtr->pointedType;
        toType       = structType;
        isMoveRef    = true;
    }

    if (!structType->isStruct() || !(castFlags & (CASTFLAG_EXPLICIT | CASTFLAG_AUTO_OPCAST)))
        return false;
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(structType, TypeInfoKind::Struct);
    if (!typeStruct->declNode)
        return false;

    auto        structNode = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    SymbolName* symbol;

    bool isSuffix = false;
    if ((fromNode && fromNode->semFlags & SEMFLAG_LITERAL_SUFFIX) || castFlags & CASTFLAG_LITERAL_SUFFIX)
        isSuffix = true;

    if (isSuffix)
        symbol = structNode->scope->symbolOpAffectSuffix;
    else
        symbol = structNode->scope->symbolOpAffect;

    // Instantiated opAffect, in a generic struct, will be in the scope of the original struct, not the intantiated one
    if (!symbol && typeStruct->fromGeneric)
    {
        structNode = CastAst<AstStruct>(typeStruct->fromGeneric->declNode, AstNodeKind::StructDecl);
        if (isSuffix)
            symbol = structNode->scope->symbolOpAffectSuffix;
        else
            symbol = structNode->scope->symbolOpAffect;
    }

    if (!symbol)
        return false;

    int idxMap = isSuffix ? 0 : 2;
    if (isMoveRef)
        idxMap++;

    // In the cache of possible matches
    {
        SharedLock lkOp(typeStruct->mutexCache);

        auto it = typeStruct->mapOpAffect[idxMap].find(fromType);
        if (it != typeStruct->mapOpAffect[idxMap].end())
        {
            if (!it->second)
                return false;

            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->flags |= AST_OPAFFECT_CAST;
                fromNode->castedTypeInfo = fromType;
                fromNode->typeInfo       = toType;
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->resolvedUserOpSymbolOverload = it->second;
            }

            context->castFlagsResult |= CASTFLAG_RESULT_AUTO_OPAFFECT;
            if (isMoveRef)
                context->castFlagsResult |= CASTFLAG_RESULT_AUTO_MOVE_OPAFFECT;
            return true;
        }
    }

    VectorNative<SymbolOverload*> toAffect;

    // Wait for all opAffect to be solved
    {
        ScopedLock ls(symbol->mutex);
        if (symbol->cptOverloads)
        {
            SWAG_ASSERT(context && context->job);
            SWAG_ASSERT(context->result == ContextResult::Done);
            context->job->waitSymbolNoLock(symbol);
            return true;
        }
    }

    // Resolve opAffect that match
    {
        SharedLock ls(symbol->mutex);
        for (auto over : symbol->overloads)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (!(typeFunc->declNode->attributeFlags & ATTRIBUTE_IMPLICIT) && !(castFlags & CASTFLAG_EXPLICIT))
                continue;
            if (typeFunc->parameters.size() <= 1)
                continue;
            if (makeCompatibles(context, typeFunc->parameters[1]->typeInfo, fromType, nullptr, nullptr, CASTFLAG_NO_LAST_MINUTE | CASTFLAG_TRY_COERCE | CASTFLAG_JUST_CHECK))
                toAffect.push_back(over);
        }
    }

    // Add in the cache of possible matches
    {
        ScopedLock lkOp(typeStruct->mutexCache);
        if (isSuffix)
            typeStruct->mapOpAffect[idxMap][fromType] = toAffect.empty() ? nullptr : toAffect[0];
        else
            typeStruct->mapOpAffect[idxMap][fromType] = toAffect.empty() ? nullptr : toAffect[0];
    }

    if (toAffect.empty())
        return false;

    // :opAffectParam
    if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
    {
        fromNode->flags |= AST_OPAFFECT_CAST;
        fromNode->castedTypeInfo = fromType;
        fromNode->typeInfo       = toType;
        fromNode->allocateExtension(ExtensionKind::Misc);
        fromNode->extMisc()->resolvedUserOpSymbolOverload = toAffect[0];
    }

    context->castFlagsResult |= CASTFLAG_RESULT_AUTO_OPAFFECT;
    if (isMoveRef)
        context->castFlagsResult |= CASTFLAG_RESULT_AUTO_MOVE_OPAFFECT;
    return true;
}

bool TypeManager::tryOpCast(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    toType   = TypeManager::concretePtrRef(toType);
    fromType = TypeManager::concretePtrRef(fromType);

    auto structType = fromType;
    if (castFlags & CASTFLAG_UFCS && structType->isPointerTo(TypeInfoKind::Struct))
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType   = typePtr->pointedType;
    }

    if (!structType->isStruct() || !(castFlags & (CASTFLAG_EXPLICIT | CASTFLAG_AUTO_OPCAST)))
        return false;
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(structType, TypeInfoKind::Struct);
    if (!typeStruct->declNode)
        return false;

    // In the cache of possible matches
    {
        SharedLock lkOp(typeStruct->mutexCache);
        auto       it = typeStruct->mapOpCast.find(toType);
        if (it != typeStruct->mapOpCast.end())
        {
            if (!it->second)
                return false;

            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromType;
                fromNode->typeInfo       = toType;
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->resolvedUserOpSymbolOverload = it->second;
                fromNode->semFlags |= SEMFLAG_USER_CAST;
            }

            context->castFlagsResult |= CASTFLAG_RESULT_AUTO_OPCAST;
            return true;
        }
    }

    auto        structNode = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    SymbolName* symbol     = structNode->scope->symbolOpCast;

    // Instantiated opCast, in a generic struct, will be in the scope of the original struct, not the intantiated one
    if (!symbol && typeStruct->fromGeneric)
    {
        structNode = CastAst<AstStruct>(typeStruct->fromGeneric->declNode, AstNodeKind::StructDecl);
        symbol     = structNode->scope->symbolOpCast;
    }

    if (!symbol)
        return false;

    VectorNative<SymbolOverload*> toCast;

    // Wait for all opCast to be solved
    {
        ScopedLock ls(symbol->mutex);
        if (symbol->cptOverloads)
        {
            SWAG_ASSERT(context && context->job);
            SWAG_ASSERT(context->result == ContextResult::Done);
            context->job->waitSymbolNoLock(symbol);
            return true;
        }
    }

    // Resolve opCast that match
    {
        SharedLock ls(symbol->mutex);
        for (auto over : symbol->overloads)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc->isGeneric() || typeFunc->returnType->isGeneric())
                continue;
            if (!(typeFunc->declNode->attributeFlags & ATTRIBUTE_IMPLICIT) && !(castFlags & CASTFLAG_EXPLICIT))
                continue;
            if (typeFunc->returnType->isSame(toType, CASTFLAG_EXACT))
                toCast.push_back(over);
        }
    }

    // Add in the cache of possible matches
    {
        ScopedLock lkOp(typeStruct->mutexCache);
        typeStruct->mapOpCast[toType] = toCast.empty() ? nullptr : toCast[0];
    }

    if (toCast.empty())
        return false;

    if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
    {
        fromNode->castedTypeInfo = fromType;
        fromNode->typeInfo       = toType;
        fromNode->allocateExtension(ExtensionKind::Misc);
        fromNode->extMisc()->resolvedUserOpSymbolOverload = toCast[0];
        fromNode->semFlags |= SEMFLAG_USER_CAST;
    }

    context->castFlagsResult |= CASTFLAG_RESULT_AUTO_OPCAST;
    return true;
}

void TypeManager::getCastErrorMsg(Utf8& msg, Utf8& hint, TypeInfo* toType, TypeInfo* fromType, uint64_t castFlags, CastErrorType castError, bool forNote)
{
    msg.clear();
    hint.clear();
    if (!toType || !fromType)
        return;

    if (castError == CastErrorType::SafetyCastAny)
    {
        msg = Fmt(Err(Saf0002), toType->getDisplayNameC());
    }
    else if (castError == CastErrorType::Const)
    {
        hint = Hnt(Hnt0022);
        msg  = Fmt(ErrNte(Err0418, forNote), fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (castError == CastErrorType::SliceArray)
    {
        auto to   = CastTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);
        auto from = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        hint      = Fmt(Hnt(Hnt0123), from->totalCount, from->finalType->getDisplayNameC(), to->pointedType->getDisplayNameC());
    }
    else if (toType->isPointerArithmetic() && !fromType->isPointerArithmetic())
    {
        msg = Fmt(ErrNte(Err0041, forNote), fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (toType->isInterface() && ((fromType->isStruct()) || (fromType->isPointerTo(TypeInfoKind::Struct))))
    {
        auto fromTypeCpy = fromType;
        if (fromTypeCpy->isPointerTo(TypeInfoKind::Struct))
        {
            hint        = Diagnostic::isType(fromTypeCpy);
            fromTypeCpy = CastTypeInfo<TypeInfoPointer>(fromTypeCpy, TypeInfoKind::Pointer)->pointedType;
        }

        msg = Fmt(ErrNte(Err0176, forNote), fromTypeCpy->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (!toType->isPointerRef() && toType->isPointer() && fromType->isNativeInteger())
    {
        msg  = Fmt(ErrNte(Err0907, forNote), fromType->getDisplayNameC());
    }
    else if (fromType->isPointerToTypeInfo() && !toType->isPointerToTypeInfo())
    {
        hint = Hnt(Hnt0040);
        msg  = Fmt(ErrNte(Err0177, forNote), fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (fromType->isClosure() && toType->isLambda())
    {
        msg = Fmt(ErrNte(Err0178, forNote));
    }
    else if (toType->isLambdaClosure() && fromType->isLambdaClosure())
    {
        auto fromTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(fromType, TypeInfoKind::LambdaClosure);
        if (fromTypeFunc->firstDefaultValueIdx != UINT32_MAX)
        {
            msg  = Fmt(ErrNte(Err0690, forNote));
            hint = Hnt(Hnt0100);
        }
    }
    else if (!fromType->isPointer() && toType->isPointerRef())
    {
        hint = Fmt(Hnt(Hnt0108), fromType->getDisplayNameC());
    }
}

bool TypeManager::castError(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags, CastErrorType castErrorType)
{
    // Last minute change : convert 'fromType' (struct) to 'toType' with an opCast
    if (!(castFlags & CASTFLAG_NO_LAST_MINUTE))
    {
        if (tryOpCast(context, toType, fromType, fromNode, castFlags))
            return true;
        if (tryOpAffect(context, toType, fromType, fromNode, castFlags))
            return true;
    }

    // Remember, for caller
    context->castErrorToType   = toType;
    context->castErrorFromType = fromType;
    context->castErrorFlags    = castFlags;
    context->castErrorType     = castErrorType;

    if (!(castFlags & CASTFLAG_JUST_CHECK))
    {
        // More specific message
        Utf8 hint, msg;
        getCastErrorMsg(msg, hint, toType, fromType, castFlags, castErrorType);

        SWAG_ASSERT(fromNode);

        // Is there an explicit cast possible ?
        if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
        {
            if (TypeManager::makeCompatibles(context, toType, fromType, nullptr, nullptr, CASTFLAG_EXPLICIT | CASTFLAG_JUST_CHECK))
            {
                Diagnostic diag{fromNode, Fmt(Err(Err0175), fromType->getDisplayNameC(), toType->getDisplayNameC())};
                diag.hint    = Fmt(Hnt(Hnt0032), fromType->getDisplayNameC(), toType->getDisplayNameC());
                diag.lowPrio = true;
                return context->report(diag);
            }
        }

        // A specific error message ?
        if (!msg.empty())
        {
            if (hint.empty())
                hint = Diagnostic::isType(fromType);
            Diagnostic diag{fromNode, msg, hint};
            return context->report(diag);
        }

        // General cast error
        Diagnostic diag{fromNode, Fmt(Err(Err0177), fromType->getDisplayNameC(), toType->getDisplayNameC())};
        diag.hint    = hint.empty() ? Diagnostic::isType(fromType) : hint;
        diag.lowPrio = true;
        return context->report(diag);
    }

    return false;
}

bool TypeManager::castToNativeBool(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType == g_TypeMgr->typeInfoBool)
        return true;

    // Automatic cast to a bool is done only if requested, on specific nodes (like if or while expressions)
    if (!(castFlags & CASTFLAG_AUTO_BOOL) && !(castFlags & CASTFLAG_EXPLICIT))
        return castError(context, g_TypeMgr->typeInfoBool, fromType, fromNode, castFlags);

    fromType = TypeManager::concreteType(fromType);

    if (fromType->isPointer() ||
        fromType->isLambdaClosure() ||
        fromType->isInterface() ||
        fromType->isSlice())
    {
        if (!(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->typeInfo       = g_TypeMgr->typeInfoBool;
            fromNode->castedTypeInfo = fromType;
            return true;
        }
    }

    if (fromType->isNative())
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->hasComputedValue())
                    {
                        fromNode->computedValue->reg.b = fromNode->computedValue->reg.u8;
                        fromNode->typeInfo             = g_TypeMgr->typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr->typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->hasComputedValue())
                    {
                        fromNode->computedValue->reg.b = fromNode->computedValue->reg.u16;
                        fromNode->typeInfo             = g_TypeMgr->typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr->typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->hasComputedValue())
                    {
                        fromNode->computedValue->reg.b = fromNode->computedValue->reg.u32;
                        fromNode->typeInfo             = g_TypeMgr->typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr->typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->hasComputedValue())
                    {
                        fromNode->computedValue->reg.b = fromNode->computedValue->reg.u64;
                        fromNode->typeInfo             = g_TypeMgr->typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr->typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->hasComputedValue())
                    {
                        fromNode->computedValue->reg.b = fromNode->computedValue->reg.f32 != 0;
                        fromNode->typeInfo             = g_TypeMgr->typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr->typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;
        case NativeTypeKind::F64:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->hasComputedValue())
                    {
                        fromNode->computedValue->reg.b = fromNode->computedValue->reg.f64 != 0;
                        fromNode->typeInfo             = g_TypeMgr->typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr->typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::String:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->hasComputedValue())
                    {
                        fromNode->computedValue->reg.b = true;
                        fromNode->typeInfo             = g_TypeMgr->typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr->typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;
        default:
            break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoBool, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeRune(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::Rune)
        return true;

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoRune, fromType, fromNode, castFlags);
        }
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr->typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f32;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f64;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::String:
            if (fromNode && fromNode->hasComputedValue())
            {
                uint32_t ch;
                if (fromNode->computedValue->text.toChar32(ch))
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                    {
                        fromNode->computedValue->reg.u64 = ch;
                        fromNode->typeInfo               = g_TypeMgr->typeInfoRune;
                    }

                    return true;
                }
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::String:
            if (fromNode && fromNode->hasComputedValue())
            {
                uint32_t ch;
                if (fromNode->computedValue->text.toChar32(ch))
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                    {
                        fromNode->computedValue->reg.u64 = ch;
                        fromNode->typeInfo               = g_TypeMgr->typeInfoRune;
                    }

                    return true;
                }
            }
            break;
        default:
            break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoRune, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U8)
        return true;

    if (!canOverflow(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8, true);
                    return false;
                }
            }
            else if (fromType->isUntypedInteger())
            {
                if (!fromNode)
                {
                    auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    auto value  = native->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.u64 > UINT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8);
                    return false;
                }
            }
            else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
            {
                if (!fromNode)
                {
                    auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    auto value  = (uint32_t) native->valueInteger;
                    if (value > UINT8_MAX)
                        return false;
                }
            }
            break;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f32 <= -SAFETY_ZERO_EPSILON)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8, true);
                    return false;
                }
                else if (fromNode->computedValue->reg.f32 >= (float) UINT8_MAX + 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8);
                    return false;
                }
            }
            break;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f64 <= -SAFETY_ZERO_EPSILON)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8, true);
                    return false;
                }
                else if (fromNode->computedValue->reg.f64 >= (double) UINT8_MAX + 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8);
                    return false;
                }
            }
            break;

        default:
            break;
        }
    }

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoU8;
                return true;
            }
            else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
            {
                return true;
            }
            break;

        default:
            break;
        }
    }

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoU8, fromType, fromNode, castFlags);
        }
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // clang-format off
        bool canChange = (fromNode && fromNode->hasComputedValue()) && !(castFlags & CASTFLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoU8;
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:  if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::S8:    if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.s8; return true;
        case NativeTypeKind::S16:   if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.s16; return true;
        case NativeTypeKind::S32:   if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.s32; return true;
        case NativeTypeKind::S64:   if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.s64; return true;
        case NativeTypeKind::Bool:  if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.b ? 1 : 0; return true;
        case NativeTypeKind::U16:   if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.u16; return true;
        case NativeTypeKind::U32:   if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::U64:   if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.u64; return true;
        case NativeTypeKind::F32:   if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.f32; return true;
        case NativeTypeKind::F64:   if (canChange) fromNode->computedValue->reg.u64 = (uint8_t) fromNode->computedValue->reg.f64; return true;
        default:                    break;
        }
        // clang-format on
    }

    return castError(context, g_TypeMgr->typeInfoU8, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U16)
        return true;

    if (!canOverflow(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16, true);
                    return false;
                }
            }
            else if (fromType->isUntypedInteger())
            {
                if (!fromNode)
                {
                    auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    auto value  = native->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.u64 > UINT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16);
                    return false;
                }
            }
            else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
            {
                if (!fromNode)
                {
                    auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    auto value  = (uint32_t) native->valueInteger;
                    if (value > UINT16_MAX)
                        return false;
                }
            }
            break;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f32 <= -SAFETY_ZERO_EPSILON)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16, true);
                    return false;
                }
                else if (fromNode->computedValue->reg.f32 >= (float) UINT16_MAX + 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16);
                    return false;
                }
            }
            break;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f64 <= -SAFETY_ZERO_EPSILON)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16, true);
                    return false;
                }
                else if (fromNode->computedValue->reg.f64 >= (double) UINT16_MAX + 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16);
                    return false;
                }
            }
            break;
        }
    }

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoU16;
                return true;
            }
            else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
            {
                return true;
            }
            break;

        default:
            break;
        }
    }

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::U8:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoU16, fromType, fromNode, castFlags);
        }
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // clang-format off
        bool canChange = (fromNode && fromNode->hasComputedValue()) && !(castFlags & CASTFLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoU16;
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:  if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::S8:    if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.s8; return true;
        case NativeTypeKind::S16:   if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.s16; return true;
        case NativeTypeKind::S32:   if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.s32; return true;
        case NativeTypeKind::S64:   if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.s64; return true;
        case NativeTypeKind::Bool:  if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.b ? 1 : 0; return true;
        case NativeTypeKind::U8:    if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.u8; return true;
        case NativeTypeKind::U32:   if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::U64:   if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.u64; return true;
        case NativeTypeKind::F32:   if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.f32; return true;
        case NativeTypeKind::F64:   if (canChange) fromNode->computedValue->reg.u64 = (uint16_t) fromNode->computedValue->reg.f64; return true;
        default:                    break;
        }
        // clang-format on
    }

    return castError(context, g_TypeMgr->typeInfoU16, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U32)
        return true;

    if (!canOverflow(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32, true);
                    return false;
                }
            }
            else if (fromType->isUntypedInteger())
            {
                if (!fromNode)
                {
                    auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    auto value  = native->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.u64 > UINT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32);
                    return false;
                }
            }
            break;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f32 <= -SAFETY_ZERO_EPSILON)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32, true);
                    return false;
                }
                else if (fromNode->computedValue->reg.f32 >= (float) UINT32_MAX + 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32);
                    return false;
                }
            }
            break;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f64 <= -SAFETY_ZERO_EPSILON)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32, true);
                    return false;
                }
                else if (fromNode->computedValue->reg.f64 >= (double) UINT32_MAX + 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32);
                    return false;
                }
            }
            break;

        default:
            break;
        }
    }

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoU32;
                return true;
            }
            else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
            {
                return true;
            }
            break;

        default:
            break;
        }
    }

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoU32, fromType, fromNode, castFlags);
        }
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // clang-format off
        bool canChange = (fromNode && fromNode->hasComputedValue()) && !(castFlags & CASTFLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoU32;
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:  if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::S8:    if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.s8; return true;
        case NativeTypeKind::S16:   if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.s16; return true;
        case NativeTypeKind::S32:   if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.s32; return true;
        case NativeTypeKind::S64:   if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.s64; return true;
        case NativeTypeKind::Bool:  if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.b ? 1 : 0; return true;
        case NativeTypeKind::U8:    if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.u8; return true;
        case NativeTypeKind::U16:   if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.u16; return true;
        case NativeTypeKind::U64:   if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.u64; return true;
        case NativeTypeKind::F32:   if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.f32; return true;
        case NativeTypeKind::F64:   if (canChange) fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.f64; return true;
        default:                    break;
        }
        // clang-format on
    }

    return castError(context, g_TypeMgr->typeInfoU32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U64)
        return true;

    if (fromType->isPointer() || fromType->isLambdaClosure())
    {
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
                fromNode->typeInfo = g_TypeMgr->typeInfoU64;
            return true;
        }
    }

    if (!canOverflow(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU64, true);
                    return false;
                }
            }
            else if (fromType->isUntypedInteger())
            {
                if (!fromNode)
                {
                    auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    auto value  = native->valueInteger;
                    if (value < 0)
                        return false;
                }
            }
            break;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f32 <= -SAFETY_ZERO_EPSILON)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU64, true);
                    return false;
                }
            }
            break;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f64 <= -SAFETY_ZERO_EPSILON)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU64, true);
                    return false;
                }
                else if (fromNode->computedValue->reg.f64 >= (double) UINT64_MAX + 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU64);
                    return false;
                }
            }
            break;

        default:
            break;
        }
    }

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoU64;
                return true;
            }
            else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
            {
                return true;
            }
            break;

        default:
            break;
        }
    }

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoU64, fromType, fromNode, castFlags);
        }
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // clang-format off
        bool canChange = (fromNode && fromNode->hasComputedValue()) && !(castFlags & CASTFLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoU64;
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:  if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::S8:    if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.s8; return true;
        case NativeTypeKind::S16:   if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.s16; return true;
        case NativeTypeKind::S32:   if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.s32; return true;
        case NativeTypeKind::S64:   if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.s64; return true;
        case NativeTypeKind::Bool:  if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.b ? 1 : 0; return true;
        case NativeTypeKind::U8:    if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.u8; return true;
        case NativeTypeKind::U16:   if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.u16; return true;
        case NativeTypeKind::U32:   if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::F32:   if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f32; return true;
        case NativeTypeKind::F64:   if (canChange) fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f64; return true;
        default:                    break;
        }
        // clang-format on
    }

    return castError(context, g_TypeMgr->typeInfoU64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S8)
        return true;

    if (!canOverflow(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (fromType->isUntypedBinHex())
            {
                auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                auto value  = native->valueInteger;
                if (value > UINT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                    return false;
                }
                break;
            }

            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.u64 > INT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                    return false;
                }
            }

        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.s64 < INT8_MIN || fromNode->computedValue->reg.s64 > INT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                    return false;
                }
            }
            else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
            {
                if (!fromNode)
                {
                    auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    auto value  = native->valueInteger;
                    if (value < INT8_MIN || value > INT8_MAX)
                        return false;
                }
            }
            break;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f32 <= (float) INT8_MIN - 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                    return false;
                }
                else if (fromNode->computedValue->reg.f32 >= (float) INT8_MAX + 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                    return false;
                }
            }
            break;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f64 <= (double) INT8_MIN - 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                    return false;
                }
                else if (fromNode->computedValue->reg.f64 >= (double) INT8_MAX + 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                    return false;
                }
            }
            break;

        default:
            break;
        }
    }

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
            if (fromType->isUntypedBinHex())
            {
                auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                auto value  = native->valueInteger;
                if (value <= UINT8_MAX)
                    return true;
            }
            break;

        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS8;
                return true;
            }
            else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
            {
                return true;
            }
            break;

        default:
            break;
        }
    }

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U8:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoS8, fromType, fromNode, castFlags);
        }
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // clang-format off
        bool canChange = (fromNode && fromNode->hasComputedValue()) && !(castFlags & CASTFLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoS8;
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:  if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::S16:   if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.s16; return true;
        case NativeTypeKind::S32:   if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.s32; return true;
        case NativeTypeKind::S64:   if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.s64; return true;
        case NativeTypeKind::Bool:  if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.b ? 1 : 0; return true;
        case NativeTypeKind::U8:    if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.u8; return true;
        case NativeTypeKind::U16:   if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.u16; return true;
        case NativeTypeKind::U32:   if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::U64:   if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.u64; return true;
        case NativeTypeKind::F32:   if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.f32; return true;
        case NativeTypeKind::F64:   if (canChange) fromNode->computedValue->reg.s64 = (int8_t) fromNode->computedValue->reg.f64; return true;
        default:                    break;
        }
        // clang-format on
    }

    return castError(context, g_TypeMgr->typeInfoS8, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S16)
        return true;

    if (!canOverflow(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (fromType->isUntypedBinHex())
            {
                auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                auto value  = native->valueInteger;
                if (value > UINT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                    return false;
                }
                break;
            }

            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.u64 > INT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                    return false;
                }
            }

        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.s64 < INT16_MIN || fromNode->computedValue->reg.s64 > INT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                    return false;
                }
            }
            else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
            {
                if (!fromNode)
                {
                    auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    auto value  = native->valueInteger;
                    if (value < INT16_MIN || value > INT16_MAX)
                        return false;
                }
            }
            break;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f32 <= (float) INT16_MIN - 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                    return false;
                }
                else if (fromNode->computedValue->reg.f32 >= (float) INT16_MAX + 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                    return false;
                }
            }
            break;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f64 <= (double) INT16_MIN - 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                    return false;
                }
                else if (fromNode->computedValue->reg.f64 >= (double) INT16_MAX + 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                    return false;
                }
            }
            break;

        default:
            break;
        }
    }

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
            if (fromType->isUntypedBinHex())
            {
                auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                auto value  = native->valueInteger;
                if (value <= UINT16_MAX)
                    return true;
            }
            break;

        case NativeTypeKind::S8:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS16;
                return true;
            }
            else if (fromType->isUntypedInteger())
            {
                return true;
            }

            break;

        default:
            break;
        }
    }

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoS16, fromType, fromNode, castFlags);
        }
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // clang-format off
        bool canChange = (fromNode && fromNode->hasComputedValue()) && !(castFlags & CASTFLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoS16;
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:  if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::S8:    if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.s8; return true;
        case NativeTypeKind::S32:   if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.s32; return true;
        case NativeTypeKind::S64:   if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.s64; return true;
        case NativeTypeKind::Bool:  if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.b ? 1 : 0; return true;
        case NativeTypeKind::U8:    if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.u8; return true;
        case NativeTypeKind::U16:   if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.u16; return true;
        case NativeTypeKind::U32:   if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::U64:   if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.u64; return true;
        case NativeTypeKind::F32:   if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.f32; return true;
        case NativeTypeKind::F64:   if (canChange) fromNode->computedValue->reg.s64 = (int16_t) fromNode->computedValue->reg.f64; return true;
        default:                    break;
        }
        // clang-format on
    }

    return castError(context, g_TypeMgr->typeInfoS16, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S32)
        return true;

    if (!canOverflow(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (fromType->isUntypedBinHex())
            {
                auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                auto value  = native->valueInteger;
                if (value > UINT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                    return false;
                }
                break;
            }

            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.u64 > INT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                    return false;
                }
            }

        case NativeTypeKind::S64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.s64 < INT32_MIN || fromNode->computedValue->reg.s64 > INT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                    return false;
                }
            }
            else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
            {
                if (!fromNode)
                {
                    auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    auto value  = native->valueInteger;
                    if (value < INT32_MIN || value > INT32_MAX)
                        return false;
                }
            }
            break;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f32 <= (float) INT32_MIN - 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                    return false;
                }
                else if (fromNode->computedValue->reg.f32 >= (float) INT32_MAX + 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                    return false;
                }
            }
            break;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f64 <= (double) INT32_MIN - 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                    return false;
                }
                else if (fromNode->computedValue->reg.f64 >= (double) INT32_MAX + 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                    return false;
                }
            }
            break;

        default:
            break;
        }
    }

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
            if (fromType->isUntypedBinHex())
                return true;

        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS32;
                return true;
            }
            else if (fromType->isUntypedInteger())
            {
                return true;
            }

            break;
        default:
            break;
        }
    }

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoS32, fromType, fromNode, castFlags);
        }
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // clang-format off
        bool canChange = (fromNode && fromNode->hasComputedValue()) && !(castFlags & CASTFLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoS32;
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:  if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::S8:    if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.s8; return true;
        case NativeTypeKind::S16:   if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.s16; return true;
        case NativeTypeKind::S64:   if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.s64; return true;
        case NativeTypeKind::Bool:  if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.b ? 1 : 0; return true;
        case NativeTypeKind::U8:    if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.u8; return true;
        case NativeTypeKind::U16:   if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.u16; return true;
        case NativeTypeKind::U32:   if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::U64:   if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.u64; return true;
        case NativeTypeKind::F32:   if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.f32; return true;
        case NativeTypeKind::F64:   if (canChange) fromNode->computedValue->reg.s64 = (int32_t) fromNode->computedValue->reg.f64; return true;
        default:                    break;
        }
        // clang-format on
    }

    return castError(context, g_TypeMgr->typeInfoS32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S64)
        return true;

    if (!canOverflow(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U64:
            if (fromType->isUntypedBinHex())
            {
                auto native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                auto value  = native->valueInteger;
                if (value > UINT64_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                    return false;
                }
                break;
            }

            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.u64 > INT64_MAX)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                    return false;
                }
            }
            break;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f32 <= (float) INT64_MIN - 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                    return false;
                }
                else if (fromNode->computedValue->reg.f32 >= (float) INT64_MAX + 0.5f)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                    return false;
                }
            }
            break;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (fromNode->computedValue->reg.f64 <= (double) INT64_MIN - 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                    return false;
                }
                else if (fromNode->computedValue->reg.f64 >= (double) INT64_MAX + 0.5)
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                    return false;
                }
            }
            break;

        default:
            break;
        }
    }

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
            if (fromType->isUntypedBinHex())
                return true;

        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
            if (fromNode && fromNode->hasComputedValue())
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS64;
                return true;
            }
            else if (fromType->isUntypedInteger())
            {
                return true;
            }

            break;
        default:
            break;
        }
    }

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoS64, fromType, fromNode, castFlags);
        }
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // clang-format off
        bool canChange = (fromNode && fromNode->hasComputedValue()) && !(castFlags & CASTFLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoS64;
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:  if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::S8:    if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.s8; return true;
        case NativeTypeKind::S16:   if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.s16; return true;
        case NativeTypeKind::S32:   if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.s32; return true;
        case NativeTypeKind::Bool:  if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.b ? 1 : 0; return true;
        case NativeTypeKind::U8:    if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.u8; return true;
        case NativeTypeKind::U16:   if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.u16; return true;
        case NativeTypeKind::U32:   if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::U64:   if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.u64; return true;
        case NativeTypeKind::F32:   if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f32; return true;
        case NativeTypeKind::F64:   if (canChange) fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f64; return true;
        default:                    break;
        }
        // clang-format on
    }

    return castError(context, g_TypeMgr->typeInfoS64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeF32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::F32)
        return true;

    if (!canOverflow(context, fromNode, castFlags))
    {
        if (!fromNode && fromType->isUntypedInteger())
        {
            auto    native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
            auto    value  = native->valueInteger;
            float   tmpF   = (float) value;
            int64_t tmpI   = (int64_t) tmpF;
            if (tmpI != value)
                return false;
        }
    }

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::F32:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoF32, fromType, fromNode, castFlags);
        }
    }

    if (fromType->isUntypedInteger() || fromType->isUntypedFloat())
        castFlags |= CASTFLAG_EXPLICIT;

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // clang-format off
        bool canChange = (fromNode && fromNode->hasComputedValue()) && !(castFlags & CASTFLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoF32;
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:  if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::S8:    if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.s8; return true;
        case NativeTypeKind::S16:   if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.s16; return true;
        case NativeTypeKind::S32:   if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.s32; return true;
        case NativeTypeKind::S64:   if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.s64; return true;
        case NativeTypeKind::Bool:  if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.b ? 1.0f : 0.0f; return true;
        case NativeTypeKind::U8:    if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.u8; return true;
        case NativeTypeKind::U16:   if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.u16; return true;
        case NativeTypeKind::U32:   if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::U64:   if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.u64; return true;
        case NativeTypeKind::F64:   if (canChange) fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.f64; return true;
        default:                    break;
        }
        // clang-format on
    }

    return castError(context, g_TypeMgr->typeInfoF32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeF64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::F64)
        return true;

    if (!canOverflow(context, fromNode, castFlags))
    {
        if (!fromNode && fromType->isUntypedInteger())
        {
            auto    native = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind);
            auto    value  = native->valueInteger;
            double  tmpF   = (double) value;
            int64_t tmpI   = (int64_t) tmpF;
            if (tmpI != value)
                return false;
        }
    }

    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::F32:
            break;
        default:
            return castError(context, g_TypeMgr->typeInfoF32, fromType, fromNode, castFlags);
        }
    }

    if (fromType->isUntypedInteger() || fromType->isUntypedFloat())
        castFlags |= CASTFLAG_EXPLICIT;

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // clang-format off
        bool canChange = (fromNode && fromNode->hasComputedValue()) && !(castFlags & CASTFLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoF64;
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:  if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::S8:    if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.s8; return true;
        case NativeTypeKind::S16:   if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.s16; return true;
        case NativeTypeKind::S32:   if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.s32; return true;
        case NativeTypeKind::S64:   if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.s64; return true;
        case NativeTypeKind::Bool:  if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.b ? 1.0 : 0.0; return true;
        case NativeTypeKind::U8:    if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.u8; return true;
        case NativeTypeKind::U16:   if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.u16; return true;
        case NativeTypeKind::U32:   if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.u32; return true;
        case NativeTypeKind::U64:   if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.u64; return true;
        case NativeTypeKind::F32:   if (canChange) fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.f32; return true;
        default:                    break;
        }
        // clang-format on
    }

    return castError(context, g_TypeMgr->typeInfoF64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint64_t castFlags)
{
    // Pick the best order
    if (castFlags & CASTFLAG_COMMUTATIVE)
    {
        if (toType->isUntypedInteger() && !fromType->isUntypedInteger())
        {
            swap(toType, fromType);
            swap(toNode, fromNode);
        }
        else if (toType->isUntypedFloat() && !fromType->isUntypedFloat())
        {
            swap(toType, fromType);
            swap(toNode, fromNode);
        }
    }

    return castToNative(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    // Automatic conversions if coerce mode is on (for expressions)
    if (castFlags & CASTFLAG_TRY_COERCE)
    {
        if (toType->sizeOf >= fromType->sizeOf)
        {
            auto leftIsInt    = toType->isNativeIntegerOrRune();
            auto rightIsInt   = fromType->isNativeIntegerOrRune();
            auto leftIsFloat  = toType->isNativeFloat();
            auto rightIsFloat = fromType->isNativeFloat();
            if ((leftIsInt && rightIsInt) || (leftIsFloat && rightIsFloat))
                castFlags |= CASTFLAG_EXPLICIT | CASTFLAG_COERCE;
            else if (leftIsFloat && fromType->isNativeInteger())
                castFlags |= CASTFLAG_EXPLICIT | CASTFLAG_COERCE;
        }
        else
        {
            if (toType->isNativeFloat() && fromType->isNativeInteger())
                castFlags |= CASTFLAG_EXPLICIT | CASTFLAG_COERCE;
        }
    }

    switch (toType->nativeType)
    {
    case NativeTypeKind::Bool:
        SWAG_CHECK(castToNativeBool(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::U8:
        SWAG_CHECK(castToNativeU8(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::U16:
        SWAG_CHECK(castToNativeU16(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::U32:
        SWAG_CHECK(castToNativeU32(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::Rune:
        SWAG_CHECK(castToNativeRune(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::U64:
        SWAG_CHECK(castToNativeU64(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::S8:
        SWAG_CHECK(castToNativeS8(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::S16:
        SWAG_CHECK(castToNativeS16(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::S32:
        SWAG_CHECK(castToNativeS32(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::S64:
        SWAG_CHECK(castToNativeS64(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::F32:
        SWAG_CHECK(castToNativeF32(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::F64:
        SWAG_CHECK(castToNativeF64(context, fromType, fromNode, castFlags));
        break;
    case NativeTypeKind::String:
        SWAG_CHECK(castToString(context, toType, fromType, fromNode, castFlags));
        break;
    default:
        return castError(context, toType, fromType, fromNode, castFlags);
    }

    SWAG_CHECK(safetyComputedValue(context, toType, fromType, fromNode, castFlags));

    // Automatic cast has been done
    if (castFlags & CASTFLAG_COERCE)
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK) && !fromNode->hasComputedValue())
        {
            fromNode->typeInfo = toType;
            if (!fromNode->castedTypeInfo)
                fromNode->castedTypeInfo = fromType;
        }
    }

    return true;
}

bool TypeManager::castExpressionList(SemanticContext* context, TypeInfoList* fromTypeList, TypeInfo* toType, AstNode* fromNode, uint64_t castFlags)
{
    auto fromSize = fromTypeList->subTypes.size();
    while (fromNode && fromNode->kind != AstNodeKind::ExpressionList)
        fromNode = fromNode->childs.front();
    SWAG_ASSERT(!fromNode || fromSize == fromNode->childs.size());

    // Need to recompute total size, as the size of each element can have been changed by the cast
    uint32_t newSizeof = 0;

    // Compute if expression is constexpr
    if (fromNode)
        fromNode->flags |= AST_CONST_EXPR;

    TypeInfoList* toTypeList = nullptr;
    if (toType->isListTuple() || toType->isListArray())
        toTypeList = CastTypeInfo<TypeInfoList>(toType, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);

    for (size_t i = 0; i < fromSize; i++)
    {
        auto child     = fromNode ? fromNode->childs[i] : nullptr;
        auto convertTo = toTypeList ? toTypeList->subTypes[i]->typeInfo : toType;

        // Expression list inside another expression list (like a struct inside an array)
        if (child && child->kind == AstNodeKind::ExpressionList && convertTo->isStruct())
        {
            auto exprNode          = CastAst<AstExpressionList>(child, AstNodeKind::ExpressionList);
            auto toTypeStruct      = CastTypeInfo<TypeInfoStruct>(convertTo, TypeInfoKind::Struct);
            exprNode->castToStruct = toTypeStruct;

            bool hasChanged = false;

            // Not enough fields
            if (toTypeStruct->fields.size() > child->childs.size())
                exprNode->castToStruct = toTypeStruct;

            // Too many fields
            else if (toTypeStruct->fields.size() < child->childs.size())
            {
                Diagnostic diag{child->childs[toTypeStruct->fields.count], Fmt(Err(Err0197), toTypeStruct->getDisplayNameC(), toTypeStruct->fields.size(), child->childs.size())};
                diag.hint = Hnt(Hnt0026);
                return context->report(diag);
            }

            SymbolMatchContext symContext;
            symContext.semContext = context;
            for (auto c : child->childs)
                symContext.parameters.push_back(c);
            toTypeStruct->match(symContext);
            switch (symContext.result)
            {
            case MatchResult::MissingNamedParameter:
            {
                auto       badParamIdx = symContext.badSignatureInfos.badSignatureParameterIdx;
                auto       failedParam = child->childs[badParamIdx];
                Diagnostic diag{failedParam, Fmt(Err(Err0006), Naming::niceArgumentRank(badParamIdx + 1).c_str()), Hnt(Hnt0031)};
                auto       otherParam = child->childs[badParamIdx - 1];
                if (otherParam->hasExtMisc() && otherParam->extMisc()->isNamed)
                    otherParam = otherParam->extMisc()->isNamed;
                diag.addRange(otherParam, Hnt(Hnt0030));
                return context->report(diag);
            }
            case MatchResult::DuplicatedNamedParameter:
            {
                auto       failedParam = child->childs[symContext.badSignatureInfos.badSignatureParameterIdx];
                Diagnostic diag{failedParam->extMisc()->isNamed, Fmt(Err(Err0011), failedParam->extMisc()->isNamed->token.ctext())};
                diag.hint       = Hnt(Hnt0009);
                auto otherParam = child->childs[symContext.badSignatureInfos.badSignatureNum1];
                if (otherParam->hasExtMisc() && otherParam->extMisc()->isNamed)
                    otherParam = otherParam->extMisc()->isNamed;
                diag.addRange(otherParam, Hnt(Hnt0059));
                return context->report(diag);
            }
            case MatchResult::InvalidNamedParameter:
            {
                auto       failedParam = child->childs[symContext.badSignatureInfos.badSignatureParameterIdx];
                Diagnostic diag{failedParam->extMisc()->isNamed, Fmt(Err(Err0008), failedParam->extMisc()->isNamed->token.ctext())};
                if (toTypeStruct->declNode && !(toTypeStruct->declNode->flags & AST_GENERATED) && toTypeStruct->declNode->resolvedSymbolOverload)
                    return context->report(diag, Diagnostic::hereIs(toTypeStruct->declNode->resolvedSymbolOverload));
                return context->report(diag);
            }
            default:
                break;
            }

            for (size_t j = 0; j < child->childs.size(); j++)
            {
                auto           childJ = child->childs[j];
                TypeInfoParam* fieldJ = symContext.solvedCallParameters[j];

                auto           oldType = childJ->typeInfo;
                PushErrCxtStep ec{context, childJ, ErrCxtStepKind::MsgPrio, [fieldJ, childJ]()
                                  { return Fmt(Err(Err0723), fieldJ->name.c_str(), fieldJ->typeInfo->getDisplayNameC(), childJ->typeInfo->getDisplayNameC()); }};
                SWAG_CHECK(TypeManager::makeCompatibles(context, fieldJ->typeInfo, childJ->typeInfo, nullptr, childJ, castFlags | CASTFLAG_TRY_COERCE));
                if (childJ->typeInfo != oldType)
                    hasChanged = true;

                // Collect array to slice : will need special treatment when collecting constants
                if (childJ->typeInfo->isListArray() && fieldJ->typeInfo->isSlice())
                {
                    childJ->allocateExtension(ExtensionKind::Misc);
                    childJ->extMisc()->collectTypeInfo = fieldJ->typeInfo;
                }

                // We use castOffset to store the offset to the field, in order to collect later at the right position
                // Note that offset is +1 to differentiate it from a "default" 0.
                childJ->allocateExtension(ExtensionKind::Misc);
                auto newOffset = (uint32_t) fieldJ->offset + 1;
                if (childJ->extMisc()->castOffset != newOffset)
                {
                    childJ->extMisc()->castOffset = newOffset;
                    hasChanged                    = true;
                }
            }

            if (child->typeInfo->sizeOf != toTypeStruct->sizeOf)
                hasChanged = true;

            if (hasChanged)
            {
                SemanticJob::computeExpressionListTupleType(context, child);
                SWAG_ASSERT(context->result == ContextResult::Done);
                child->typeInfo->sizeOf = toTypeStruct->sizeOf;
            }
        }

        if (fromNode)
        {
            PushErrCxtStep ec(context, fromNode->childs.front(), ErrCxtStepKind::Hint2, [convertTo]()
                              { return Fmt(Hnt(Hnt0062), convertTo->getDisplayNameC()); });
            SWAG_CHECK(TypeManager::makeCompatibles(context, convertTo, fromTypeList->subTypes[i]->typeInfo, nullptr, child, castFlags | CASTFLAG_TRY_COERCE));
        }
        else
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, convertTo, fromTypeList->subTypes[i]->typeInfo, nullptr, child, castFlags | CASTFLAG_TRY_COERCE));
        }

        if (child)
        {
            newSizeof += child->typeInfo->sizeOf;
            if (!(child->flags & AST_CONST_EXPR))
                fromNode->flags &= ~AST_CONST_EXPR;
        }
    }

    if (fromNode && (fromTypeList->sizeOf != newSizeof))
    {
        auto oldSizeof = fromTypeList->sizeOf;
        fromTypeList   = (TypeInfoList*) fromTypeList->clone();
        for (size_t i = 0; i < fromTypeList->subTypes.size(); i++)
            fromTypeList->subTypes[i]->typeInfo = fromNode->childs[i]->typeInfo;
        fromTypeList->sizeOf = newSizeof;
        fromNode->typeInfo   = fromTypeList;

        // :ExprListArrayStorage
        if (!(fromNode->flags & AST_CONST_EXPR) && fromNode->ownerScope && fromNode->ownerFct)
        {
            SWAG_ASSERT(fromNode->computedValue);
            fromNode->ownerScope->startStackSize -= oldSizeof;
            fromNode->ownerScope->startStackSize += newSizeof;
            SemanticJob::setOwnerMaxStackSize(fromNode, fromNode->ownerScope->startStackSize);
        }
    }

    return true;
}

bool TypeManager::castToString(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (fromType->isPointerNull())
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            if (fromNode->hasComputedValue())
                fromNode->computedValue->text.release();
            fromNode->typeInfo       = toType;
            fromNode->castedTypeInfo = g_TypeMgr->typeInfoNull;
        }

        return true;
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // [..] u8 to string, this is possible !
        if (fromType->isSlice())
        {
            auto fromTypeSlice = CastTypeInfo<TypeInfoSlice>(fromType, TypeInfoKind::Slice);
            auto concreteType  = TypeManager::concreteType(fromTypeSlice->pointedType, CONCRETE_ALIAS);
            if (concreteType->isNative(NativeTypeKind::U8))
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = g_TypeMgr->typeInfoString;
                }

                return true;
            }
        }

        // [] u8 to string, this is possible !
        if (fromType->isArray())
        {
            auto fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
            auto concreteType  = TypeManager::concreteType(fromTypeArray->pointedType, CONCRETE_ALIAS);
            if (concreteType->isNative(NativeTypeKind::U8))
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = g_TypeMgr->typeInfoString;
                }

                return true;
            }
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToFromAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint64_t castFlags)
{
    if (toType->isAny())
    {
        if (fromType->isPointerNull())
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->typeInfo       = toType;
                fromNode->castedTypeInfo = g_TypeMgr->typeInfoNull;
            }

            return true;
        }

        if (castFlags & CASTFLAG_COMMUTATIVE)
        {
            if (toNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                // When casting something complexe to any, we will copy the value to the stack to be sure
                // that the memory layout is correct, without relying on registers being contiguous, and not being reallocated (by an optimize pass).
                // See ByteCodeGenJob::emitCastToNativeAny
                if (toNode->ownerFct && toType->numRegisters() > 1)
                {
                    toNode->allocateExtension(ExtensionKind::Misc);
                    toNode->extMisc()->stackOffset = toNode->ownerScope->startStackSize;
                    toNode->ownerScope->startStackSize += toType->numRegisters() * sizeof(Register);
                    SemanticJob::setOwnerMaxStackSize(toNode, toNode->ownerScope->startStackSize);
                }

                toNode->castedTypeInfo = toType;
                toNode->typeInfo       = fromType;
                auto  module           = context->sourceFile->module;
                auto& typeGen          = module->typeGen;

                // :AnyTypeSegment
                toNode->allocateExtension(ExtensionKind::Misc);
                toNode->extMisc()->anyTypeSegment = SemanticJob::getConstantSegFromContext(toNode);
                SWAG_CHECK(typeGen.genExportedTypeInfo(context, fromType, toNode->extMisc()->anyTypeSegment, &toNode->extMisc()->anyTypeOffset));
            }

            return true;
        }

        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            // When casting something complexe to any, we will copy the value to the stack to be sure
            // that the memory layout is correct, without relying on registers being contiguous, and not being reallocated (by an optimize pass).
            // See ByteCodeGenJob::emitCastToNativeAny
            if (fromNode->ownerFct && fromType->numRegisters() > 1)
            {
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->stackOffset = fromNode->ownerScope->startStackSize;
                fromNode->ownerScope->startStackSize += fromType->numRegisters() * sizeof(Register);
                SemanticJob::setOwnerMaxStackSize(fromNode, fromNode->ownerScope->startStackSize);
            }

            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toType;
            auto  module             = context->sourceFile->module;
            auto& typeGen            = module->typeGen;

            // :AnyTypeSegment
            fromNode->allocateExtension(ExtensionKind::Misc);
            fromNode->extMisc()->anyTypeSegment = SemanticJob::getConstantSegFromContext(fromNode);
            SWAG_CHECK(typeGen.genExportedTypeInfo(context, fromNode->castedTypeInfo, fromNode->extMisc()->anyTypeSegment, &fromNode->extMisc()->anyTypeOffset));
        }
    }
    else if (fromType->isAny())
    {
        toType = TypeManager::concretePtrRef(toType);

        if (!(castFlags & CASTFLAG_EXPLICIT))
        {
            // Ambigous. Do we check for a bool, or do we check for null
            if (toType->isBool())
                return castError(context, toType, fromType, fromNode, castFlags);

            // To convert a simple any to something more complexe, need an explicit cast
            if (toType->isSlice() ||
                toType->isArray() ||
                toType->isPointer())
                return castError(context, toType, fromType, fromNode, castFlags);
        }

        // From a constant, need to check the type
        if (fromNode && fromNode->hasComputedValue())
        {
            ExportedAny* any         = (ExportedAny*) fromNode->computedValue->getStorageAddr();
            auto         newTypeInfo = context->sourceFile->module->typeGen.getRealType(fromNode->computedValue->storageSegment, (ExportedTypeInfo*) any->type);

            if (newTypeInfo && context->sourceFile->module->mustEmitSafety(fromNode, SAFETY_ANY, true))
            {
                if (!toType->isSame(newTypeInfo, CASTFLAG_EXACT))
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                        return castError(context, toType, newTypeInfo, fromNode, castFlags, CastErrorType::SafetyCastAny);
                    return false;
                }
            }

            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->typeInfo       = newTypeInfo;
                fromNode->castedTypeInfo = nullptr;
                SWAG_CHECK(SemanticJob::derefConstantValue(context, fromNode, fromNode->typeInfo, fromNode->computedValue->storageSegment, (uint8_t*) any->value));
            }

            return true;
        }

        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            // When casting something complexe to any, we will copy the value to the stack to be sure
            // that the memory layout is correct, without relying on registers being contiguous, and not being reallocated (by an optimize pass).
            // See ByteCodeGenJob::emitCastToNativeAny
            if (fromNode->ownerFct && fromType->numRegisters() > 1)
            {
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->stackOffset = fromNode->ownerScope->startStackSize;
                fromNode->ownerScope->startStackSize += fromType->numRegisters() * sizeof(Register);
                SemanticJob::setOwnerMaxStackSize(fromNode, fromNode->ownerScope->startStackSize);
            }

            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toType;
            auto  module             = context->sourceFile->module;
            auto& typeGen            = module->typeGen;

            // :AnyTypeSegment
            fromNode->allocateExtension(ExtensionKind::Misc);
            fromNode->extMisc()->anyTypeSegment = SemanticJob::getConstantSegFromContext(fromNode);
            SWAG_CHECK(typeGen.genExportedTypeInfo(context, toType, fromNode->extMisc()->anyTypeSegment, &fromNode->extMisc()->anyTypeOffset));
        }
    }

    return true;
}

bool TypeManager::castStructToStruct(SemanticContext* context, TypeInfoStruct* toStruct, TypeInfoStruct* fromStruct, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags, bool& ok)
{
    context->castStructStructFields.clear();
    auto& stack = context->castStructStructFields;

    stack.push_back({fromStruct, 0, nullptr});
    while (!stack.empty())
    {
        auto it = stack.back();
        stack.pop_back();

        auto testStruct = it.typeStruct->getStructOrPointedStruct();
        SWAG_ASSERT(testStruct);
        if (testStruct->isSame(toStruct, CASTFLAG_CAST))
        {
            ok = true;
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                if (toType->isSelf())
                {
                    if (it.offset)
                    {
                        fromNode->allocateExtension(ExtensionKind::Misc);
                        fromNode->extMisc()->castOffset = it.offset;
                        fromNode->castedTypeInfo        = fromNode->typeInfo;
                        fromNode->typeInfo              = toType;
                    }

                    continue;
                }

                // We will have to dereference the pointer to get the real thing
                if (it.field && it.field->typeInfo->isPointer())
                    fromNode->semFlags |= SEMFLAG_DEREF_USING;
                fromNode->semFlags |= SEMFLAG_USING;

                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->castOffset = it.offset;
                fromNode->castedTypeInfo        = fromNode->typeInfo;
                fromNode->typeInfo              = toType;
                continue;
            }
        }

        // No using ! We're done
        if ((castFlags & CASTFLAG_FOR_GENERIC) && !(it.typeStruct->flags & TYPEINFO_STRUCT_TYPEINFO))
            return true;

        auto structNode = CastAst<AstStruct>(it.typeStruct->declNode, AstNodeKind::StructDecl);
        if (!(structNode->specFlags & AstStruct::SPECFLAG_HAS_USING))
            continue;

        context->job->waitOverloadCompleted(it.typeStruct->declNode->resolvedSymbolOverload);
        if (context->result != ContextResult::Done)
            return true;

        TypeInfoParam*  foundField  = nullptr;
        TypeInfoStruct* foundStruct = nullptr;
        for (auto field : it.typeStruct->fields)
        {
            if (!(field->flags & TYPEINFOPARAM_HAS_USING))
                continue;

            TypeInfoStruct* typeStruct = nullptr;
            if (field->typeInfo->isStruct())
            {
                typeStruct = CastTypeInfo<TypeInfoStruct>(field->typeInfo, TypeInfoKind::Struct);
                if (typeStruct == it.typeStruct)
                    continue;
            }
            else if (field->typeInfo->isPointerTo(TypeInfoKind::Struct))
            {
                auto typePointer = CastTypeInfo<TypeInfoPointer>(field->typeInfo, TypeInfoKind::Pointer);
                typeStruct       = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            }

            if (typeStruct)
            {
                // Ambiguous ! Two fields with a 'using' on the same struct
                if (foundStruct)
                {
                    auto foundTypeStruct0 = foundStruct->getStructOrPointedStruct();
                    auto foundTypeStruct1 = typeStruct->getStructOrPointedStruct();
                    if (foundTypeStruct0 && foundTypeStruct1 && foundTypeStruct0->isSame(foundTypeStruct1, CASTFLAG_CAST))
                    {
                        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                        {
                            Diagnostic diag{fromNode, Fmt(Err(Err0200), fromType->getDisplayNameC(), toType->getDisplayNameC(), fromStruct->getDisplayNameC(), toStruct->getDisplayNameC())};
                            auto       note1 = Diagnostic::note(foundField->declNode, Nte(Nte0015));
                            auto       note2 = Diagnostic::note(field->declNode, Nte(Nte0016));
                            return context->report(diag, note1, note2);
                        }
                    }
                }

                foundField  = field;
                foundStruct = typeStruct;
                stack.push_back({typeStruct, it.offset + field->offset, field});
            }
        }
    }

    return true;
}

bool TypeManager::collectInterface(SemanticContext* context, TypeInfoStruct* fromTypeStruct, TypeInfoStruct* toTypeItf, InterfaceRef& ref, bool skipFirst)
{
    TypeInfoParam* foundField = nullptr;

    context->castCollectInterfaceField.clear();
    auto& stack = context->castCollectInterfaceField;

    stack.push_back({fromTypeStruct, 0, nullptr, nullptr});
    while (!stack.empty())
    {
        auto it = stack.back();
        stack.pop_back();

        if (!skipFirst)
        {
            auto foundItf = it.typeStruct->hasInterface(toTypeItf);
            if (foundItf)
            {
                if (foundField)
                {
                    Diagnostic diag{context->node, Fmt(Err(Err0034), fromTypeStruct->structName.c_str(), toTypeItf->name.c_str())};
                    auto       note1 = Diagnostic::note(it.field->declNode, Nte(Nte0006));
                    auto       note2 = Diagnostic::note(foundField->declNode, Nte(Nte0062));
                    return context->report(diag, note1, note2);
                }

                ref.itf         = foundItf;
                ref.fieldOffset = it.offset;
                ref.fieldRef    = it.fieldAccessName;
                foundField      = it.field;
                continue;
            }
        }

        skipFirst       = false;
        auto structNode = CastAst<AstStruct>(it.typeStruct->declNode, AstNodeKind::StructDecl);
        if (!(structNode->specFlags & AstStruct::SPECFLAG_HAS_USING))
            continue;

        context->job->waitOverloadCompleted(it.typeStruct->declNode->resolvedSymbolOverload);
        if (context->result != ContextResult::Done)
            return true;

        for (auto field : it.typeStruct->fields)
        {
            if (!(field->flags & TYPEINFOPARAM_HAS_USING))
                continue;
            if (!field->typeInfo->isStruct())
                continue;

            auto typeStruct = CastTypeInfo<TypeInfoStruct>(field->typeInfo, TypeInfoKind::Struct);
            if (typeStruct == it.typeStruct)
                continue;

            context->job->waitAllStructInterfaces(typeStruct);
            if (context->result != ContextResult::Done)
                return true;

            auto accessName = it.fieldAccessName;
            if (!accessName.empty())
                accessName += ".";
            accessName += field->name;

            stack.push_back({typeStruct, it.offset + field->offset, field, accessName});
        }
    }

    return true;
}

bool TypeManager::castToInterface(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    if (castFlags & CASTFLAG_FOR_GENERIC)
        return castError(context, toType, fromType, fromNode, castFlags);

    auto toTypeItf = CastTypeInfo<TypeInfoStruct>(toType, TypeInfoKind::Interface);

    if (fromType->isPointerNull())
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            // We will copy the value to the stack to be sure that the memory layout is correct, without relying on
            // registers being contiguous, and not being reallocated (by an optimize pass).
            // This is the same problem when casting to 'any'.
            // See ByteCodeGenJob::emitCastToNativeAny
            if (fromNode->ownerFct)
            {
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->stackOffset = fromNode->ownerScope->startStackSize;
                fromNode->ownerScope->startStackSize += 2 * sizeof(Register);
                SemanticJob::setOwnerMaxStackSize(fromNode, fromNode->ownerScope->startStackSize);
            }

            fromNode->castedTypeInfo = fromNode->typeInfo;
            fromNode->typeInfo       = toType;
        }

        return true;
    }

    // Struct to interface
    // We need to take care of "using" fields.
    if (fromType->isStruct() || fromType->isPointerTo(TypeInfoKind::Struct))
    {
        context->job->waitAllStructInterfaces(fromType);
        if (context->result != ContextResult::Done)
        {
            SWAG_ASSERT(castFlags & CASTFLAG_ACCEPT_PENDING);
            return true;
        }

        auto typeStruct = fromType;
        if (fromType->isPointer())
        {
            auto typePointer = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
            typeStruct       = typePointer->pointedType;
        }

        auto fromTypeStruct = CastTypeInfo<TypeInfoStruct>(typeStruct, TypeInfoKind::Struct);

        InterfaceRef itfRef;
        SWAG_CHECK(collectInterface(context, fromTypeStruct, toTypeItf, itfRef));
        if (context->result != ContextResult::Done)
        {
            SWAG_ASSERT(castFlags & CASTFLAG_ACCEPT_PENDING);
            return true;
        }

        if (itfRef.itf)
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->castOffset = itfRef.fieldOffset;
                fromNode->extMisc()->castItf    = itfRef.itf;
                fromNode->castedTypeInfo        = fromType;
                fromNode->typeInfo              = toTypeItf;
            }

            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToPointerRef(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    auto toTypePointer = CastTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);

    if (fromType->isPointer())
    {
        // Convert from pointer to ref : only if authorized
        if (!(fromType->flags & TYPEINFO_POINTER_REF) && !(castFlags & CASTFLAG_EXPLICIT) && !(castFlags & CASTFLAG_PTR_REF))
            return castError(context, toType, fromType, fromNode, castFlags);

        // Compare pointed types
        auto fromTypePointer = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        if (toTypePointer->pointedType->isSame(fromTypePointer->pointedType, castFlags | CASTFLAG_CAST))
            return true;
    }
    else if (toType->isConst())
    {
        // Compare pointed types
        if ((castFlags & CASTFLAG_PARAMS) && toTypePointer->pointedType->isSame(fromType, castFlags | CASTFLAG_CAST))
            return true;
    }

    // Structure to interface reference
    if (fromType->isStruct() && toTypePointer->pointedType->isInterface())
    {
        auto toTypeItf      = CastTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Interface);
        auto fromTypeStruct = CastTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        if (!fromTypeStruct->hasInterface(toTypeItf))
            return castError(context, toType, fromType, fromNode, castFlags);

        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toTypeItf;
        }

        return true;
    }

    // Struct to automatic const ref struct
    if (fromType->isStruct() && toTypePointer->pointedType->isStruct() && toTypePointer->isAutoConstPointerRef())
    {
        auto fromStruct = CastTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        auto toStruct   = CastTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Struct);
        bool ok         = false;
        SWAG_CHECK(castStructToStruct(context, toStruct, fromStruct, toType, fromType, fromNode, castFlags, ok));
        if (ok || context->result == ContextResult::Pending)
        {
            if (fromStruct != toStruct)
                context->castFlagsResult |= CASTFLAG_RESULT_STRUCT_CONVERT;
            return true;
        }
    }

    // Struct to struct pointer
    if (fromType->isPointerTo(TypeInfoKind::Struct) && toTypePointer->pointedType->isStruct())
    {
        auto fromPtr    = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        auto fromStruct = CastTypeInfo<TypeInfoStruct>(fromPtr->pointedType, TypeInfoKind::Struct);
        auto toStruct   = CastTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Struct);
        bool ok         = false;
        SWAG_CHECK(castStructToStruct(context, toStruct, fromStruct, toType, fromType, fromNode, castFlags, ok));
        if (ok || context->result == ContextResult::Pending)
        {
            if (fromStruct != toStruct)
                context->castFlagsResult |= CASTFLAG_RESULT_STRUCT_CONVERT;
            return true;
        }
    }

    // Struct to moveref can go to there
    if (fromType->isStruct() && toTypePointer->pointedType->isStruct() && toTypePointer->isPointerMoveRef())
    {
        if (fromType->isSame(toTypePointer->pointedType, castFlags | CASTFLAG_CAST))
        {
            context->castFlagsResult |= CASTFLAG_RESULT_GUESS_MOVE;
            return true;
        }
    }

    // Ufcs, accept type to ref type
    if (castFlags & CASTFLAG_UFCS)
    {
        if (toTypePointer->pointedType->isSame(fromType, castFlags | CASTFLAG_CAST))
        {
            context->castFlagsResult |= CASTFLAG_RESULT_FORCE_REF;
            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToPointer(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    // To "cstring"
    if (toType->isCString())
    {
        if (fromType->isString())
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromType;
                fromNode->typeInfo       = toType;
            }

            return true;
        }
    }

    auto toTypePointer = CastTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);

    // Pointer to struct to pointer to struct. Take care of using
    if (fromType->isPointer() && toTypePointer->pointedType->isStruct())
    {
        auto fromTypePointer = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        if (fromTypePointer->pointedType->isStruct())
        {
            auto fromStruct = CastTypeInfo<TypeInfoStruct>(fromTypePointer->pointedType, TypeInfoKind::Struct);
            auto toStruct   = CastTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Struct);
            bool ok         = false;
            SWAG_CHECK(castStructToStruct(context, toStruct, fromStruct, toType, fromType, fromNode, castFlags, ok));
            if (ok || context->result == ContextResult::Pending)
            {
                if (fromStruct != toStruct)
                    context->castFlagsResult |= CASTFLAG_RESULT_STRUCT_CONVERT;
                return true;
            }
        }
    }

    // Struct to pointer to struct
    // Accept only if this is ufcs, to simulate calling a method of a base 'class'
    if ((castFlags & CASTFLAG_UFCS) && fromType->isStruct() && toTypePointer->pointedType->isStruct())
    {
        auto fromStruct = CastTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        auto toStruct   = CastTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Struct);
        bool ok         = false;
        SWAG_CHECK(castStructToStruct(context, toStruct, fromStruct, toType, fromType, fromNode, castFlags, ok));
        if (ok || context->result == ContextResult::Pending)
        {
            if (fromStruct != toStruct)
                context->castFlagsResult |= CASTFLAG_RESULT_STRUCT_CONVERT;
            return true;
        }
    }

    // Lambda to *void
    if (fromType->isLambdaClosure())
    {
        if ((castFlags & CASTFLAG_EXPLICIT) && toType->isPointerVoid())
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                fromNode->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid, TYPEINFO_CONST);
            return true;
        }
    }

    // Pointer to pointer
    if (fromType->isPointer())
    {
        if (castFlags & CASTFLAG_EXPLICIT)
            return true;

        // Fine to compare pointers of TypeInfos, even if not of the same type
        if ((castFlags & CASTFLAG_COMPARE) && fromType->isPointerToTypeInfo() && toType->isPointerToTypeInfo())
            return true;

        // Pointer to *void or *void to pointer
        if (toType->isPointerVoid() || fromType->isPointerVoid())
        {
            if (castFlags & CASTFLAG_PARAMS)
            {
                if (toType->flags & (TYPEINFO_GENERIC | TYPEINFO_FROM_GENERIC))
                    return castError(context, toType, fromType, fromNode, castFlags);
            }

            if (castFlags & CASTFLAG_FOR_GENERIC)
            {
                return castError(context, toType, fromType, fromNode, castFlags);
            }

            return true;
        }

        // :PointerArithmetic
        // Cannot cast from non arithmetic to arithmetic
        if (toType->isPointerArithmetic() && !fromType->isPointerArithmetic())
        {
            // Fine to compare pointers for examples, but not to affect them.
            if (castFlags & CASTFLAG_FOR_AFFECT)
                return castError(context, toType, fromType, fromNode, castFlags);
            return true;
        }
    }

    // Array to pointer of the same type
    if (fromType->isArray())
    {
        auto fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        if ((!(castFlags & CASTFLAG_NO_IMPLICIT) && toTypePointer->pointedType->isVoid()) ||
            (!(castFlags & CASTFLAG_NO_IMPLICIT) && toTypePointer->pointedType->isSame(fromTypeArray->pointedType, CASTFLAG_CAST)) ||
            (castFlags & CASTFLAG_EXPLICIT))
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toType;
            }

            return true;
        }
    }

    // Slice to pointer of the same type
    if (fromType->isSlice())
    {
        auto fromTypeSlice = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Slice);
        if ((!(castFlags & CASTFLAG_NO_IMPLICIT) && toTypePointer->pointedType->isVoid()) ||
            (!(castFlags & CASTFLAG_NO_IMPLICIT) && toTypePointer->pointedType->isSame(fromTypeSlice->pointedType, CASTFLAG_CAST)) ||
            (castFlags & CASTFLAG_EXPLICIT))
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toType;
            }

            return true;
        }
    }

    // Struct/Interface to pointer
    if (fromType->isStruct() || fromType->isInterface())
    {
        if ((castFlags & CASTFLAG_EXPLICIT) || toTypePointer->isSelf() || toTypePointer->isConst() || (castFlags & CASTFLAG_UFCS))
        {
            // to *void or *structure
            if (toTypePointer->pointedType->isVoid() ||
                toTypePointer->pointedType->isSame(fromType, CASTFLAG_CAST))
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK) && !toTypePointer->isSelf())
                {
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = toType;
                }

                return true;
            }
        }
    }

    // Interface back to struct pointer
    if (fromType->isInterface() && toTypePointer->isPointerTo(TypeInfoKind::Struct))
    {
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            auto fromTypeItf  = CastTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Interface);
            auto toTypeStruct = CastTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Struct);
            if (!toTypeStruct->hasInterface(fromTypeItf))
                return castError(context, toType, fromType, fromNode, castFlags);

            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromType;
                fromNode->typeInfo       = toType;
            }

            return true;
        }
    }

    // Struct pointer to interface pointer
    if (fromType->isPointerTo(TypeInfoKind::Struct) && toType->isPointerTo(TypeInfoKind::Interface))
        return castError(context, toType, fromType, fromNode, castFlags);

    // String to const *u8
    if (fromType->isString())
    {
        if (toType->isPointerNull())
            return true;

        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (toTypePointer->pointedType->isNative(NativeTypeKind::U8) && toTypePointer->isConst())
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = toType;
                }

                return true;
            }
        }
    }

    // u64 to whatever pointer
    if (fromType->isNative(NativeTypeKind::U64))
    {
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toType;
            }

            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    TypeInfoArray* toTypeArray = CastTypeInfo<TypeInfoArray>(toType, TypeInfoKind::Array);
    if (fromType->isListArray())
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeListArray);
        auto          fromSize     = fromTypeList->subTypes.size();
        if (toTypeArray->count != fromSize)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                if (toTypeArray->count > fromTypeList->subTypes.size())
                    context->report({fromNode, Fmt(Err(Err0203), toTypeArray->count, fromTypeList->subTypes.size())});
                else
                    context->report({fromNode, Fmt(Err(Err0204), toTypeArray->count, fromTypeList->subTypes.size())});
            }

            return false;
        }

        SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeArray->pointedType, fromNode, castFlags));

        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromType->flags |= TYPEINFO_LISTARRAY_ARRAY;
        }

        return true;
    }

    if (castFlags & CASTFLAG_FOR_VAR_INIT)
    {
        if (toTypeArray->finalType->isSame(fromType, CASTFLAG_CAST))
        {
            if (toTypeArray->finalType->isString() ||
                toTypeArray->finalType->isNativeFloat() ||
                toTypeArray->finalType->isNativeIntegerOrRune() ||
                toTypeArray->finalType->isBool())
            {
                return true;
            }
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToLambda(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    TypeInfoFuncAttr* toTypeLambda = CastTypeInfo<TypeInfoFuncAttr>(toType, TypeInfoKind::LambdaClosure);
    if (castFlags & CASTFLAG_EXPLICIT)
    {
        if (fromType->isPointerNull() || fromType->isPointerConstVoid())
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toTypeLambda;
            }

            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToClosure(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    TypeInfoFuncAttr* toTypeLambda = CastTypeInfo<TypeInfoFuncAttr>(toType, TypeInfoKind::LambdaClosure);
    if (castFlags & CASTFLAG_EXPLICIT)
    {
        if (fromType->isPointerNull())
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toTypeLambda;
            }

            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    CastErrorType  castErrorType = CastErrorType::Zero;
    TypeInfoSlice* toTypeSlice   = CastTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);

    if (fromType->isListArray())
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeListArray);
        SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeSlice->pointedType, fromNode, castFlags));
        return true;
    }
    else if (fromType->isArray())
    {
        TypeInfoArray* fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        if ((!(castFlags & CASTFLAG_NO_IMPLICIT) && toTypeSlice->pointedType->isSame(fromTypeArray->pointedType, CASTFLAG_CAST)) ||
            (castFlags & CASTFLAG_EXPLICIT))
        {
            int s = toTypeSlice->pointedType->sizeOf;
            int d = fromTypeArray->sizeOf;
            SWAG_ASSERT(s != 0 || toTypeSlice->pointedType->isGeneric());

            bool match = s == 0 || (d / s) * s == d;
            if (match)
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = toTypeSlice;
                }

                return true;
            }
            else
                castErrorType = CastErrorType::SliceArray;
        }
    }
    else if (fromType->isString())
    {
        if (toTypeSlice->pointedType->isNative(NativeTypeKind::U8) && toTypeSlice->isConst())
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toTypeSlice;
            }

            return true;
        }
    }
    else if (fromType->isPointerNull())
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromNode->typeInfo;
            fromNode->typeInfo       = toTypeSlice;
        }

        return true;
    }
    else if (fromType->isPointerTo(g_TypeMgr->typeInfoVoid))
    {
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toTypeSlice;
            }

            return true;
        }
    }
    else if (fromType->isSlice())
    {
        auto fromTypeSlice = CastTypeInfo<TypeInfoSlice>(fromType, TypeInfoKind::Slice);
        if (castFlags & CASTFLAG_EXPLICIT || toTypeSlice->pointedType->isSame(fromTypeSlice->pointedType, CASTFLAG_CAST))
            return true;
    }

    return castError(context, toType, fromType, fromNode, castFlags, castErrorType);
}

TypeInfo* TypeManager::promoteUntyped(TypeInfo* typeInfo)
{
    if (typeInfo->isUntypedInteger())
        return g_TypeMgr->typeInfoS32;
    if (typeInfo->isUntypedFloat())
        return g_TypeMgr->typeInfoF32;
    if (typeInfo->isUntypedBinHex())
        return g_TypeMgr->typeInfoU32;
    return typeInfo;
}

void TypeManager::promoteUntypedInteger(AstNode* left, AstNode* right)
{
    TypeInfo* leftTypeInfo = TypeManager::concreteType(left->typeInfo);
    SWAG_ASSERT(leftTypeInfo->isUntypedInteger());

    TypeInfo* rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    if (!rightTypeInfo->isNativeInteger())
        return;

    auto leftNative = CastTypeInfo<TypeInfoNative>(leftTypeInfo, TypeInfoKind::Native);
    if (leftNative->valueInteger == 0)
    {
        left->typeInfo = rightTypeInfo;
    }
    else if ((leftNative->valueInteger > 0) && (rightTypeInfo->flags & TYPEINFO_UNSIGNED))
    {
        if (leftNative->valueInteger <= UINT8_MAX)
            left->typeInfo = g_TypeMgr->typeInfoU8;
        else if (leftNative->valueInteger <= UINT16_MAX)
            left->typeInfo = g_TypeMgr->typeInfoU16;
        else
            left->typeInfo = g_TypeMgr->typeInfoU32;
    }
    else
    {
        if (leftNative->valueInteger >= INT8_MIN && leftNative->valueInteger <= INT8_MAX)
            left->typeInfo = g_TypeMgr->typeInfoS8;
        else if (leftNative->valueInteger >= INT16_MIN && leftNative->valueInteger <= INT16_MAX)
            left->typeInfo = g_TypeMgr->typeInfoS16;
        else if (leftNative->valueInteger >= INT32_MIN && leftNative->valueInteger <= INT32_MAX)
            left->typeInfo = g_TypeMgr->typeInfoS32;
        else
            left->typeInfo = g_TypeMgr->typeInfoS64;
    }
}

bool TypeManager::promote32(SemanticContext* context, AstNode* left)
{
    TypeInfo* typeInfo = TypeManager::concreteType(left->typeInfo);
    if (!typeInfo->isNative())
        return true;

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        if (left->hasComputedValue())
            left->computedValue->reg.s64 = left->computedValue->reg.s8;
        SWAG_CHECK(makeCompatibles(context, g_TypeMgr->typeInfoS32, nullptr, left, CASTFLAG_TRY_COERCE));
        break;
    case NativeTypeKind::S16:
        if (left->hasComputedValue())
            left->computedValue->reg.s64 = left->computedValue->reg.s16;
        SWAG_CHECK(makeCompatibles(context, g_TypeMgr->typeInfoS32, nullptr, left, CASTFLAG_TRY_COERCE));
        break;
    case NativeTypeKind::U8:
        if (left->hasComputedValue())
            left->computedValue->reg.u64 = left->computedValue->reg.u8;
        SWAG_CHECK(makeCompatibles(context, g_TypeMgr->typeInfoU32, nullptr, left, CASTFLAG_TRY_COERCE));
        break;
    case NativeTypeKind::U16:
        if (left->hasComputedValue())
            left->computedValue->reg.u64 = left->computedValue->reg.u32;
        SWAG_CHECK(makeCompatibles(context, g_TypeMgr->typeInfoU32, nullptr, left, CASTFLAG_TRY_COERCE));
        break;
    default:
        break;
    }

    return true;
}

bool TypeManager::promote(SemanticContext* context, AstNode* left, AstNode* right)
{
    SWAG_CHECK(promoteLeft(context, left, right));
    SWAG_CHECK(promoteLeft(context, right, left));
    return true;
}

bool TypeManager::promoteLeft(SemanticContext* context, AstNode* left, AstNode* right)
{
    TypeInfo* leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    TypeInfo* rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    // Promotion only for native types
    if (!leftTypeInfo->isNativeInteger() && !leftTypeInfo->isNativeFloat())
        return true;
    if (!rightTypeInfo->isNativeInteger() && !rightTypeInfo->isNativeFloat())
        return true;

    if (leftTypeInfo->isUntypedInteger() && !rightTypeInfo->isUntypedInteger())
    {
        promoteUntypedInteger(left, right);
        leftTypeInfo = left->typeInfo;
    }

    if (!leftTypeInfo->isUntypedInteger() && rightTypeInfo->isUntypedInteger())
    {
        promoteUntypedInteger(right, left);
        rightTypeInfo = right->typeInfo;
    }

    auto newLeftTypeInfo = g_TypeMgr->promoteMatrix[(int) leftTypeInfo->nativeType][(int) rightTypeInfo->nativeType];
    SWAG_ASSERT(newLeftTypeInfo);
    if (newLeftTypeInfo == leftTypeInfo)
        return true;

    left->typeInfo = newLeftTypeInfo;
    if (!left->hasComputedValue())
    {
        left->castedTypeInfo = leftTypeInfo;
        return true;
    }

    auto newLeft = newLeftTypeInfo->nativeType;
    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        if (newLeft == NativeTypeKind::U32)
            left->computedValue->reg.u64 = left->computedValue->reg.u8;
        else if (newLeft == NativeTypeKind::U64)
            left->computedValue->reg.u64 = left->computedValue->reg.u8;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue->reg.f32 = (float) left->computedValue->reg.u8;
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.u8;
        break;
    case NativeTypeKind::U16:
        if (newLeft == NativeTypeKind::U32)
            left->computedValue->reg.u64 = left->computedValue->reg.u16;
        else if (newLeft == NativeTypeKind::U64)
            left->computedValue->reg.u64 = left->computedValue->reg.u16;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue->reg.f32 = (float) left->computedValue->reg.u16;
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.u16;
        break;
    case NativeTypeKind::U32:
        if (newLeft == NativeTypeKind::U64)
            left->computedValue->reg.u64 = left->computedValue->reg.u32;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue->reg.f32 = (float) left->computedValue->reg.u32;
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.u32;
        break;
    case NativeTypeKind::U64:
        if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.u64;
        break;
    case NativeTypeKind::S8:
        if (newLeft == NativeTypeKind::S32)
            left->computedValue->reg.s64 = left->computedValue->reg.s8;
        else if (newLeft == NativeTypeKind::S64)
            left->computedValue->reg.s64 = left->computedValue->reg.s8;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue->reg.f32 = (float) left->computedValue->reg.s8;
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.s8;
        break;
    case NativeTypeKind::S16:
        if (newLeft == NativeTypeKind::S32)
            left->computedValue->reg.s64 = left->computedValue->reg.s16;
        else if (newLeft == NativeTypeKind::S64)
            left->computedValue->reg.s64 = left->computedValue->reg.s16;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue->reg.f32 = (float) left->computedValue->reg.s16;
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.s16;
        break;
    case NativeTypeKind::S32:
        if (newLeft == NativeTypeKind::S64)
            left->computedValue->reg.s64 = left->computedValue->reg.s32;
        else if (newLeft == NativeTypeKind::S64)
            left->computedValue->reg.s64 = left->computedValue->reg.s32;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue->reg.f32 = (float) left->computedValue->reg.s32;
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.s32;
        break;
    case NativeTypeKind::S64:
        if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.s64;
        break;
    case NativeTypeKind::F32:
        if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = left->computedValue->reg.f32;
        break;
    default:
        break;
    }

    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, uint64_t castFlags)
{
    PushErrCxtStep ec(context, leftNode, ErrCxtStepKind::Hint2, [leftNode]()
                      { return Diagnostic::isType(leftNode->typeInfo); });
    SWAG_CHECK(makeCompatibles(context, leftNode->typeInfo, leftNode, rightNode, castFlags));
    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* toNode, AstNode* fromNode, uint64_t castFlags)
{
    // convert {...} expression list to a structure : this will create a variable, with parameters
    if (!(castFlags & CASTFLAG_NO_TUPLE_TO_STRUCT))
    {
        SWAG_ASSERT(fromNode->typeInfo);
        auto fromType = concreteType(fromNode->typeInfo, CONCRETE_ALIAS);
        if (fromType->isListTuple())
        {
            bool convert = false;
            if (toType->isStruct())
                convert = true;

            if (toType->isPointerRef() && toType->isConst())
            {
                auto ptrRef = CastTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);
                if (ptrRef->pointedType->isStruct())
                {
                    toType  = ptrRef->pointedType;
                    convert = true;
                }
            }

            if (fromNode->parent->kind == AstNodeKind::FuncDeclParam)
                convert = false;

            if (convert)
            {
                SWAG_CHECK(Ast::convertLiteralTupleToStructVar(context, toType, fromNode));
                return true;
            }
        }
    }

    SWAG_CHECK(makeCompatibles(context, toType, fromNode->typeInfo, toNode, fromNode, castFlags));
    if (!fromNode)
        return true;

    // Be sure to keep the TYPEINFO_SPREAD flag from the original type
    if (fromNode->castedTypeInfo && fromNode->castedTypeInfo->flags & TYPEINFO_SPREAD)
    {
        fromNode->typeInfo = fromNode->typeInfo->clone();
        fromNode->typeInfo->flags |= TYPEINFO_SPREAD;
    }

    // autocast
    if ((fromNode->typeInfo->flags & TYPEINFO_AUTO_CAST) && !fromNode->castedTypeInfo)
    {
        if (!(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromNode->typeInfo;
            fromNode->typeInfo       = toType;
        }
    }

    // Store constant expression in the constant segment
    if (!(castFlags & CASTFLAG_NO_COLLECT))
    {
        if (fromNode->typeInfo->isListTuple() || fromNode->typeInfo->isListArray())
        {
            while (fromNode && fromNode->kind != AstNodeKind::ExpressionList)
                fromNode = fromNode->childs.empty() ? nullptr : fromNode->childs.front();

            if (fromNode && (fromNode->flags & AST_CONST_EXPR))
            {
#ifdef SWAG_HAS_ASSERT
                TypeInfoList* typeList = CastTypeInfo<TypeInfoList>(fromNode->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
                SWAG_ASSERT(typeList->subTypes.size() == fromNode->childs.size());
#endif
                auto exprList = CastAst<AstExpressionList>(fromNode, AstNodeKind::ExpressionList);
                if (exprList && !(exprList->semFlags & SEMFLAG_EXPRLIST_CST))
                {
                    exprList->semFlags |= SEMFLAG_EXPRLIST_CST;

                    // Test sizeof because {} is legit to initialize a struct (for default values in function arguments)
                    if (fromNode->typeInfo->sizeOf)
                    {
                        auto constSegment = SemanticJob::getConstantSegFromContext(exprList);
                        exprList->allocateComputedValue();
                        exprList->computedValue->storageSegment = constSegment;
                        SWAG_CHECK(SemanticJob::reserveAndStoreToSegment(context, exprList->computedValue->storageSegment, exprList->computedValue->storageOffset, nullptr, fromNode->typeInfo, exprList));
                    }
                }
            }
        }
    }

    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint64_t castFlags)
{
    // convert {...} expression list to a structure : this will create a variable, with parameters
    auto realFromType = concreteType(fromType, CONCRETE_ALIAS);
    auto realToType   = concreteType(toType, CONCRETE_ALIAS);
    SWAG_ASSERT(realFromType && realToType);
    if (realFromType->isListTuple())
    {
        if (realToType->isStruct())
            return true;

        if (realToType->isConstPointerRef())
        {
            auto ptrRef = CastTypeInfo<TypeInfoPointer>(realToType, TypeInfoKind::Pointer);
            if (ptrRef->pointedType->isStruct())
                return true;
        }
    }

    SWAG_ASSERT(toType && fromType);

    if (fromType->flags & TYPEINFO_AUTO_CAST)
        castFlags |= CASTFLAG_EXPLICIT;
    if (toType->isGeneric())
        castFlags |= CASTFLAG_NO_IMPLICIT;
    if (toType->flags & TYPEINFO_FROM_GENERIC)
        castFlags |= CASTFLAG_NO_IMPLICIT;

    if (toType->isFuncAttr())
        toType = TypeManager::concreteType(toType, CONCRETE_FUNC);
    if (fromType->isFuncAttr())
        fromType = TypeManager::concreteType(fromType, CONCRETE_FUNC);
    if (!toType->isLambdaClosure() && fromType->isLambdaClosure())
        fromType = TypeManager::concreteType(fromType, CONCRETE_FUNC);

    // Transform typealias to related type
    if (toType->isAlias())
        toType = TypeManager::concreteType(toType, (castFlags & CASTFLAG_EXPLICIT) ? CONCRETE_FORCEALIAS : CONCRETE_ALIAS);
    if (fromType->isAlias())
        fromType = TypeManager::concreteType(fromType, (castFlags & CASTFLAG_EXPLICIT) ? CONCRETE_FORCEALIAS : CONCRETE_ALIAS);

    // Transform enum to underlying type
    if ((castFlags & CASTFLAG_CONCRETE_ENUM) || (castFlags & CASTFLAG_EXPLICIT) || toType->flags & TYPEINFO_INCOMPLETE || fromType->flags & TYPEINFO_INCOMPLETE)
    {
        toType   = TypeManager::concreteType(toType, CONCRETE_ENUM);
        fromType = TypeManager::concreteType(fromType, CONCRETE_ENUM);
    }

    if ((castFlags & CASTFLAG_INDEX) && (fromType->flags & TYPEINFO_ENUM_INDEX))
    {
        fromType = TypeManager::concreteType(fromType, CONCRETE_ENUM);
    }

    // Transform typealias to related type
    fromType = TypeManager::concreteType(fromType, (castFlags & CASTFLAG_EXPLICIT) ? CONCRETE_FORCEALIAS : CONCRETE_ALIAS);
    toType   = TypeManager::concreteType(toType, (castFlags & CASTFLAG_EXPLICIT) ? CONCRETE_FORCEALIAS : CONCRETE_ALIAS);

    if (fromType->isListTuple() || fromType->isListArray())
    {
        toType = TypeManager::concreteType(toType, CONCRETE_FORCEALIAS);
    }

    if (toNode && toNode->resolvedSymbolName && toNode->resolvedSymbolName->kind == SymbolKind::EnumValue)
    {
        toType   = TypeManager::concreteType(toType, CONCRETE_FORCEALIAS);
        fromType = TypeManager::concreteType(fromType, CONCRETE_FORCEALIAS);
    }

    if (fromNode && fromNode->resolvedSymbolName)
    {
        if (fromNode->resolvedSymbolName->kind == SymbolKind::EnumValue ||
            fromNode->resolvedSymbolName->kind == SymbolKind::Function)
        {
            toType   = TypeManager::concreteType(toType, CONCRETE_FORCEALIAS);
            fromType = TypeManager::concreteType(fromType, CONCRETE_FORCEALIAS);
        }
    }

    if (fromType == toType)
        return true;

    // Everything can be casted to or from type 'any'
    if (toType->isAny() || fromType->isAny())
        return castToFromAny(context, toType, fromType, toNode, fromNode, castFlags);

    // Variadic
    if (fromType->isTypedVariadic())
        fromType = ((TypeInfoVariadic*) fromType)->rawType;
    if (toType->isTypedVariadic())
        toType = ((TypeInfoVariadic*) toType)->rawType;

    bool result = false;

    // To/From a moveref
    if (!(fromType->flags & TYPEINFO_POINTER_ACCEPT_MOVE_REF) && (toType->flags & TYPEINFO_POINTER_MOVE_REF))
    {
        if ((castFlags & CASTFLAG_PARAMS) && !(castFlags & CASTFLAG_ACCEPT_MOVE_REF))
            return castError(context, toType, fromType, fromNode, castFlags);
    }
    else if ((fromType->flags & TYPEINFO_POINTER_ACCEPT_MOVE_REF) && !(toType->flags & TYPEINFO_POINTER_MOVE_REF))
    {
        return castError(context, toType, fromType, fromNode, castFlags);
    }

    // From a reference
    if (fromType->isPointerRef() ||
        (fromNode && fromNode->kind == AstNodeKind::KeepRef && fromType->isPointer()))
    {
        auto fromTypeRef = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer)->pointedType;
        if (fromTypeRef == toType)
        {
            castFlags |= CASTFLAG_FORCE_UNCONST;
            result = true;
        }
        else if (fromTypeRef->isSame(toType, castFlags | CASTFLAG_CAST))
        {
            castFlags |= CASTFLAG_FORCE_UNCONST;
            result = true;
        }
    }

    if (toNode && toNode->semFlags & SEMFLAG_FROM_REF && toType->isPointerRef())
        toType = concretePtrRef(toType);
    if (fromNode && fromNode->semFlags & SEMFLAG_FROM_REF && fromType->isPointerRef())
        fromType = concretePtrRef(fromType);

    // If not already ok, call 'same'
    if (!result && fromType->isSame(toType, castFlags | CASTFLAG_CAST))
        result = true;

    // Always match against a generic
    if (!result)
    {
        if (toType->isKindGeneric())
            result = true;
    }

    if (!result)
    {
        switch (toType->kind)
        {
            // Cast to pointer
        case TypeInfoKind::Pointer:
            if (toType->isPointerRef())
                SWAG_CHECK(castToPointerRef(context, toType, fromType, fromNode, castFlags));
            else
                SWAG_CHECK(castToPointer(context, toType, fromType, fromNode, castFlags));
            break;

            // Cast to native type
        case TypeInfoKind::Native:
            SWAG_CHECK(castToNative(context, toType, fromType, toNode, fromNode, castFlags));
            break;

            // Cast to array
        case TypeInfoKind::Array:
            SWAG_CHECK(castToArray(context, toType, fromType, fromNode, castFlags));
            break;

            // Cast to slice
        case TypeInfoKind::Slice:
            SWAG_CHECK(castToSlice(context, toType, fromType, fromNode, castFlags));
            break;

            // Cast to interface
        case TypeInfoKind::Interface:
            SWAG_CHECK(castToInterface(context, toType, fromType, fromNode, castFlags));
            break;

            // Cast to lambda
        case TypeInfoKind::LambdaClosure:
            if (toType->isClosure())
                SWAG_CHECK(castToClosure(context, toType, fromType, fromNode, castFlags));
            else
                SWAG_CHECK(castToLambda(context, toType, fromType, fromNode, castFlags));
            break;

        default:
            return castError(context, toType, fromType, fromNode, castFlags);
        }
    }

    // Const mismatch
    if (!toType->isKindGeneric() && !toType->isLambdaClosure() && !(castFlags & CASTFLAG_FORCE_UNCONST))
    {
        if (!(castFlags & CASTFLAG_PARAMS) || !toType->isStruct())
        {
            if ((!toType->isPointerNull()) &&
                (!toType->isString()) &&
                (!toType->isInterface()) &&
                (!toType->isBool() || !(castFlags & CASTFLAG_AUTO_BOOL)) &&
                (!toType->isNative(NativeTypeKind::U64) || !fromType->isPointer()))
            {
                bool toConst   = toType->isConst();
                bool fromConst = fromType->isConst();
                bool diff      = false;
                if (castFlags & CASTFLAG_FOR_GENERIC)
                    diff = toConst != fromConst;
                else
                    diff = !toConst && fromConst;
                if (diff)
                {
                    if (!(castFlags & CASTFLAG_UNCONST))
                        return castError(context, toType, fromType, fromNode, castFlags, CastErrorType::Const);

                    // We can affect a const to an unconst if type is by copy, and we are in an affectation
                    if (!fromType->isStruct() &&
                        !toType->isStruct() &&
                        !fromType->isArray() &&
                        !toType->isArray())
                    {
                        return castError(context, toType, fromType, fromNode, castFlags, CastErrorType::Const);
                    }
                }
            }
        }
    }

    return true;
}

static const ExportedTypeInfo* concreteAlias(const ExportedTypeInfo* type1)
{
    if (type1->kind != TypeInfoKind::Alias || (type1->flags & (uint16_t) ExportedTypeInfoFlags::Strict))
        return type1;
    auto typeAlias = (const ExportedTypeInfoAlias*) type1;
    return concreteAlias((ExportedTypeInfo*) typeAlias->rawType);
}

bool TypeManager::compareConcreteType(const ExportedTypeInfo* type1, const ExportedTypeInfo* type2)
{
    SWAG_ASSERT(type1 && type2);
    if (type1 == type2)
        return true;

    type1 = concreteAlias(type1);
    type2 = concreteAlias(type2);

    if ((type1->kind != type2->kind) || (type1->sizeOf != type2->sizeOf) || (type1->flags != type2->flags))
        return false;
    if (type1->fullName.count != type2->fullName.count)
        return false;
    return !memcmp(type1->fullName.buffer, type2->fullName.buffer, type1->fullName.count);
}

TypeInfo* TypeManager::concreteType(TypeInfo* typeInfo, uint32_t flags)
{
    if (!typeInfo)
        return typeInfo;

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Native:
        return typeInfo;

    case TypeInfoKind::FuncAttr:
        if (flags & CONCRETE_FUNC)
        {
            auto returnType = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr)->returnType;
            if (!returnType)
                return g_TypeMgr->typeInfoVoid;
            return concreteType(returnType, flags);
        }
        break;

    case TypeInfoKind::Enum:
        if (flags & CONCRETE_ENUM)
            return concreteType(CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum)->rawType, flags);
        break;

    case TypeInfoKind::Alias:
        if (flags & (CONCRETE_ALIAS | CONCRETE_FORCEALIAS))
        {
            auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
            if (typeAlias->isStrict() && !(flags & CONCRETE_FORCEALIAS))
                return typeAlias;
            return concreteType(typeAlias->rawType, flags);
        }
        break;

    case TypeInfoKind::Generic:
        if (flags & CONCRETE_GENERIC)
        {
            auto typeGeneric = CastTypeInfo<TypeInfoGeneric>(typeInfo, TypeInfoKind::Generic);
            if (!typeGeneric->rawType)
                return typeGeneric;
            return concreteType(typeGeneric->rawType, flags);
        }
        break;

    default:
        break;
    }

    return typeInfo;
}

TypeInfo* TypeManager::concretePtrRefType(TypeInfo* typeInfo, uint32_t flags)
{
    if (!typeInfo)
        return nullptr;
    typeInfo = concretePtrRef(typeInfo);
    typeInfo = concreteType(typeInfo, flags);
    typeInfo = concretePtrRef(typeInfo);
    return typeInfo;
}

TypeInfo* TypeManager::concretePtrRef(TypeInfo* typeInfo)
{
    if (!typeInfo)
        return nullptr;
    if (typeInfo->flags & TYPEINFO_POINTER_REF)
        return CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer)->pointedType;
    return typeInfo;
}

TypeInfo* TypeManager::concretePtrRefCond(TypeInfo* typeInfo, AstNode* node)
{
    if (node->semFlags & SEMFLAG_FROM_REF)
        return concretePtrRef(typeInfo);
    return typeInfo;
}
