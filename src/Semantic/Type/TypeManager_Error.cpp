#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Wmf/Module.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"

bool TypeManager::isOverflowEnabled(const SemanticContext* context, const AstNode* fromNode, CastFlags castFlags)
{
    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_CAN_OVERFLOW))
        return true;
    if (castFlags.has(CAST_FLAG_COERCE))
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
                return context->report({fromNode, formErr(Saf0021, fromNode->computedValue()->reg.f32, toType->getDisplayNameC())});
            case NativeTypeKind::F64:
                return context->report({fromNode, formErr(Saf0021, fromNode->computedValue()->reg.f64, toType->getDisplayNameC())});
            default:
                return context->report({fromNode, formErr(Saf0019, fromNode->computedValue()->reg.s64, toType->getDisplayNameC())});
        }
    }

    if (fromNode->is(AstNodeKind::Literal) && fromNode->token.text.length() > 2)
    {
        if (std::tolower(fromNode->token.text[1]) == 'x' || std::tolower(fromNode->token.text[1]) == 'b')
        {
            return context->report({fromNode, formErr(Saf0022, fromNode->token.c_str(), fromNode->computedValue()->reg.u64, toType->getDisplayNameC())});
        }
    }

    switch (fromType->nativeType)
    {
        case NativeTypeKind::F32:
            return context->report({fromNode, formErr(Saf0025, fromNode->computedValue()->reg.f32, toType->getDisplayNameC())});
        case NativeTypeKind::F64:
            return context->report({fromNode, formErr(Saf0025, fromNode->computedValue()->reg.f64, toType->getDisplayNameC())});
        default:
            if (fromType->isNativeIntegerSigned())
                return context->report({fromNode, formErr(Saf0024, fromNode->computedValue()->reg.s64, toType->getDisplayNameC())});
            return context->report({fromNode, formErr(Saf0023, fromNode->computedValue()->reg.u64, toType->getDisplayNameC())});
    }
}

bool TypeManager::safetyComputedValue(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (!fromNode || !fromNode->hasFlagComputedValue())
        return true;
    if (castFlags.has(CAST_FLAG_JUST_CHECK))
        return true;
    if (!castFlags.has(CAST_FLAG_EXPLICIT))
        return true;
    if (!fromNode->token.sourceFile->module->mustEmitSafety(fromNode, SAFETY_OVERFLOW))
        return true;

    auto msg  = ByteCodeGen::safetyMsg(SafetyMsg::CastTruncated, toType, fromType);
    auto msg1 = ByteCodeGen::safetyMsg(SafetyMsg::CastNeg, toType, fromType);

    // Negative value to unsigned type
    if (fromType->isNativeIntegerSigned() && toType->isNativeIntegerUnsignedOrRune() && fromNode->computedValue()->reg.s64 < 0)
        return context->report({fromNode ? fromNode : context->node, msg1});

    switch (toType->nativeType)
    {
        case NativeTypeKind::U8:
            if (fromNode->computedValue()->reg.u64 > UINT8_MAX)
                return context->report({fromNode ? fromNode : context->node, msg});
            break;
        case NativeTypeKind::U16:
            if (fromNode->computedValue()->reg.u64 > UINT16_MAX)
                return context->report({fromNode ? fromNode : context->node, msg});
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            if (fromNode->computedValue()->reg.u64 > UINT32_MAX)
                return context->report({fromNode ? fromNode : context->node, msg});
            break;
        case NativeTypeKind::U64:
            if (fromType->isNativeIntegerSigned())
            {
                if (fromNode->computedValue()->reg.u64 > INT64_MAX)
                    return context->report({fromNode ? fromNode : context->node, msg});
            }
            break;

        case NativeTypeKind::S8:
            if (fromNode->computedValue()->reg.s64 < INT8_MIN || fromNode->computedValue()->reg.s64 > INT8_MAX)
                return context->report({fromNode ? fromNode : context->node, msg});
            break;
        case NativeTypeKind::S16:
            if (fromNode->computedValue()->reg.s64 < INT16_MIN || fromNode->computedValue()->reg.s64 > INT16_MAX)
                return context->report({fromNode ? fromNode : context->node, msg});
            break;
        case NativeTypeKind::S32:
            if (fromNode->computedValue()->reg.s64 < INT32_MIN || fromNode->computedValue()->reg.s64 > INT32_MAX)
                return context->report({fromNode ? fromNode : context->node, msg});
            break;
        case NativeTypeKind::S64:
            if (!fromType->isNativeIntegerSigned())
            {
                if (fromNode->computedValue()->reg.u64 > INT64_MAX)
                    return context->report({fromNode ? fromNode : context->node, msg});
            }
            break;
    }

    return true;
}

void TypeManager::getCastErrorMsg(Utf8&         msg,
                                  Utf8&         hint,
                                  Vector<Utf8>& remarks,
                                  TypeInfo*     toType,
                                  TypeInfo*     fromType,
                                  AstNode* /*fromNode*/,
                                  CastFlags /*castFlags*/,
                                  CastErrorType castError)
{
    msg.clear();
    hint.clear();
    if (!toType || !fromType)
        return;

    if (castError == CastErrorType::SafetyCastAny)
    {
        msg = formErr(Saf0001, toType->getDisplayNameC());
    }
    else if (castError == CastErrorType::Const)
    {
        msg = formErr(Err0054, fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (castError == CastErrorType::SliceArray)
    {
        const auto to   = castTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);
        const auto from = castTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        hint            = formNte(Nte0113, from->totalCount, from->finalType->getDisplayNameC(), to->pointedType->getDisplayNameC());
    }
    else if (toType->isPointerArithmetic() && !fromType->isPointerArithmetic())
    {
        msg = formErr(Err0646, fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (toType->isInterface() && (fromType->isStruct() || fromType->isPointerTo(TypeInfoKind::Struct)))
    {
        if (fromType->isPointerTo(TypeInfoKind::Struct))
        {
            hint     = Diagnostic::isType(fromType);
            fromType = castTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer)->pointedType;
        }

        msg = formErr(Err0314, fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (!toType->isPointerRef() && toType->isPointer() && fromType->isNativeInteger())
    {
        msg = formErr(Err0644, fromType->getDisplayNameC());
    }
    else if (fromType->isPointerToTypeInfo() && !toType->isPointerToTypeInfo())
    {
        hint = formNte(Nte0161, fromType->getDisplayNameC());
        msg  = formErr(Err0645, toType->getDisplayNameC());
    }
    else if (fromType->isClosure() && toType->isLambda())
    {
        msg = toErr(Err0647);
    }
    else if (toType->isLambdaClosure() && fromType->isLambdaClosure())
    {
        const auto fromTypeFunc = castTypeInfo<TypeInfoFuncAttr>(fromType, TypeInfoKind::LambdaClosure);
        if (fromTypeFunc->firstDefaultValueIdx != UINT32_MAX)
            msg = toErr(Err0251);
    }
    else if (!fromType->isPointer() && toType->isPointerRef())
    {
        const auto toPtrRef = castTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);
        if (fromType->isSame(toPtrRef->pointedType, CAST_FLAG_CAST))
            hint = toNte(Nte0196);
    }
    else if (toType->isTuple() && fromType->isTuple())
    {
        Utf8 toName, fromName;
        toType->computeWhateverName(toName, COMPUTE_DISPLAY_NAME);
        fromType->computeWhateverName(fromName, COMPUTE_DISPLAY_NAME);
        remarks.push_back(form("source type is %s", fromName.c_str()));
        remarks.push_back(form("requested type is %s", toName.c_str()));

        msg = toErr(Err0640);
    }
}

bool TypeManager::castError(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags, CastErrorType castErrorType)
{
    // Last minute change : convert 'fromType' (struct) to 'toType' with an opCast
    if (!castFlags.has(CAST_FLAG_NO_LAST_MINUTE))
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

    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
    {
        // More specific message
        Utf8                      hint, msg;
        Vector<const Diagnostic*> notes;
        Vector<Utf8>              remarks;
        getCastErrorMsg(msg, hint, remarks, toType, fromType, fromNode, castFlags, castErrorType);
        SWAG_ASSERT(fromNode);

        if (msg.empty())
            msg = formErr(Err0643, fromType->getDisplayNameC(), toType->getDisplayNameC());
        if (!hint.empty())
            notes.push_back(Diagnostic::note(fromNode, hint));

        // Is there an explicit cast possible ?
        if (!castFlags.has(CAST_FLAG_EXPLICIT) || castFlags.has(CAST_FLAG_COERCE))
        {
            if (makeCompatibles(context, toType, fromType, nullptr, nullptr, CAST_FLAG_EXPLICIT | CAST_FLAG_JUST_CHECK))
                notes.push_back(Diagnostic::note(fromNode, formNte(Nte0030, toType->getDisplayNameC())));
        }

        Diagnostic err{fromNode, msg};
        err.remarks = remarks;

        // Add a note in case we affect to an identifier.
        if (context->node->is(AstNodeKind::AffectOp))
        {
            const auto left = context->node->firstChild();
            if (left->is(AstNodeKind::IdentifierRef))
            {
                const auto* note = Diagnostic::note(left->lastChild(), Diagnostic::isType(left->lastChild()));
                notes.push_back(note);
            }
        }

        return context->report(err, notes);
    }

    return false;
}
