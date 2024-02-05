#include "pch.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Semantic.h"
#include "TypeManager.h"

bool TypeManager::isOverflowEnabled(const SemanticContext* context, AstNode* fromNode, uint64_t castFlags)
{
    if ((castFlags & CASTFLAG_EXPLICIT) && (castFlags & CASTFLAG_CAN_OVERFLOW))
        return true;
    if (castFlags & CASTFLAG_COERCE)
        return false;
    if (fromNode && context->sourceFile && context->sourceFile->module && !context->sourceFile->module->mustEmitSafetyOverflow(fromNode, true))
        return true;
    return false;
}

bool TypeManager::errorOutOfRange(SemanticContext* context, AstNode* fromNode, const TypeInfo* fromType, TypeInfo* toType, bool isNeg)
{
    if (isNeg)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::F32:
            return context->report({fromNode, FMT(Err(Saf0021), fromNode->computedValue->reg.f32, toType->getDisplayNameC())});
        case NativeTypeKind::F64:
            return context->report({fromNode, FMT(Err(Saf0021), fromNode->computedValue->reg.f64, toType->getDisplayNameC())});
        default:
            return context->report({fromNode, FMT(Err(Saf0019), fromNode->computedValue->reg.s64, toType->getDisplayNameC())});
        }
    }

    if (fromNode->kind == AstNodeKind::Literal && fromNode->token.text.length() > 2)
    {
        if (std::tolower(fromNode->token.text[1]) == 'x' || std::tolower(fromNode->token.text[1]) == 'b')
        {
            return context->report({fromNode, FMT(Err(Saf0022), fromNode->token.ctext(), fromNode->computedValue->reg.u64, toType->getDisplayNameC())});
        }
    }

    switch (fromType->nativeType)
    {
    case NativeTypeKind::F32:
        return context->report({fromNode, FMT(Err(Saf0025), fromNode->computedValue->reg.f32, toType->getDisplayNameC())});
    case NativeTypeKind::F64:
        return context->report({fromNode, FMT(Err(Saf0025), fromNode->computedValue->reg.f64, toType->getDisplayNameC())});
    default:
        if (fromType->isNativeIntegerSigned())
            return context->report({fromNode, FMT(Err(Saf0024), fromNode->computedValue->reg.s64, toType->getDisplayNameC())});
        return context->report({fromNode, FMT(Err(Saf0023), fromNode->computedValue->reg.u64, toType->getDisplayNameC())});
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

    auto msg  = ByteCodeGen::safetyMsg(SafetyMsg::CastTruncated, toType, fromType);
    auto msg1 = ByteCodeGen::safetyMsg(SafetyMsg::CastNeg, toType, fromType);

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

void TypeManager::getCastErrorMsg(Utf8&         msg,
                                  Utf8&         hint,
                                  Vector<Utf8>& remarks,
                                  TypeInfo*     toType,
                                  TypeInfo*     fromType,
                                  AstNode*      fromNode,
                                  uint64_t      castFlags,
                                  CastErrorType castError,
                                  bool          forNote)
{
    msg.clear();
    hint.clear();
    if (!toType || !fromType)
        return;

    if (castError == CastErrorType::SafetyCastAny)
    {
        msg = FMT(Err(Saf0001), toType->getDisplayNameC());
    }
    else if (castError == CastErrorType::Const)
    {
        msg = FMT(ErrNte(Err0054, forNote), fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (castError == CastErrorType::SliceArray)
    {
        const auto to   = CastTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);
        const auto from = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        hint            = FMT(Nte(Nte0113), from->totalCount, from->finalType->getDisplayNameC(), to->pointedType->getDisplayNameC());
    }
    else if (toType->isPointerArithmetic() && !fromType->isPointerArithmetic())
    {
        msg = FMT(ErrNte(Err0646, forNote), fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (toType->isInterface() && ((fromType->isStruct()) || (fromType->isPointerTo(TypeInfoKind::Struct))))
    {
        if (fromType->isPointerTo(TypeInfoKind::Struct))
        {
            hint     = Diagnostic::isType(fromType);
            fromType = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer)->pointedType;
        }

        msg = FMT(ErrNte(Err0314, forNote), fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (!toType->isPointerRef() && toType->isPointer() && fromType->isNativeInteger())
    {
        msg = FMT(ErrNte(Err0644, forNote), fromType->getDisplayNameC());
    }
    else if (fromType->isPointerToTypeInfo() && !toType->isPointerToTypeInfo())
    {
        hint = FMT(Nte(Nte0161), fromType->getDisplayNameC());
        msg  = FMT(ErrNte(Err0645, forNote), toType->getDisplayNameC());
    }
    else if (fromType->isClosure() && toType->isLambda())
    {
        msg = FMT(ErrNte(Err0647, forNote));
    }
    else if (toType->isLambdaClosure() && fromType->isLambdaClosure())
    {
        const auto fromTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(fromType, TypeInfoKind::LambdaClosure);
        if (fromTypeFunc->firstDefaultValueIdx != UINT32_MAX)
            msg = FMT(ErrNte(Err0251, forNote));
    }
    else if (!fromType->isPointer() && toType->isPointerRef())
    {
        const auto toPtrRef = CastTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);
        if (fromType->isSame(toPtrRef->pointedType, CASTFLAG_CAST))
            hint = Nte(Nte0196);
    }
    else if (toType->isTuple() && fromType->isTuple())
    {
        Utf8 toName, fromName;
        toType->computeWhateverName(toName, COMPUTE_DISPLAY_NAME);
        fromType->computeWhateverName(fromName, COMPUTE_DISPLAY_NAME);
        remarks.push_back(FMT("source type is %s", fromName.c_str()));
        remarks.push_back(FMT("requested type is %s", toName.c_str()));

        msg = ErrNte(Err0640, forNote);
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
        Utf8                      hint, msg;
        Vector<const Diagnostic*> notes;
        Vector<Utf8>              remarks;
        getCastErrorMsg(msg, hint, remarks, toType, fromType, fromNode, castFlags, castErrorType);
        SWAG_ASSERT(fromNode);

        if (msg.empty())
            msg = FMT(Err(Err0643), fromType->getDisplayNameC(), toType->getDisplayNameC());
        if (!hint.empty())
            notes.push_back(Diagnostic::note(fromNode, hint));

        // Is there an explicit cast possible ?
        if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
        {
            if (TypeManager::makeCompatibles(context, toType, fromType, nullptr, nullptr, CASTFLAG_EXPLICIT | CASTFLAG_JUST_CHECK))
                notes.push_back(Diagnostic::note(fromNode, FMT(Nte(Nte0030), toType->getDisplayNameC())));
        }

        Diagnostic diag{fromNode, msg};
        diag.remarks = remarks;

        // Add a note in case we affect to an identifier.
        if (context->node->kind == AstNodeKind::AffectOp)
        {
            const auto left = context->node->childs.front();
            if (left->kind == AstNodeKind::IdentifierRef)
            {
                const auto* note = Diagnostic::note(left->childs.back(), Diagnostic::isType(left->childs.back()));
                notes.push_back(note);
            }
        }

        return context->report(diag, notes);
    }

    return false;
}
