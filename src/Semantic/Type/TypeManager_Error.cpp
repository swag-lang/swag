#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Naming.h"
#include "Wmf/Module.h"

bool TypeManager::isOverflowEnabled(const SemanticContext* context, const AstNode* fromNode, CastFlags castFlags)
{
    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_CAN_OVERFLOW))
        return true;
    if (castFlags.has(CAST_FLAG_COERCE))
        return false;
    if (fromNode && context->sourceFile && context->sourceFile->module && !context->sourceFile->module->mustEmitSafetyOverflow(fromNode, SafetyContext::Compiler))
        return true;
    return false;
}

bool TypeManager::safetyErrorOutOfRange(SemanticContext* context, AstNode* fromNode, const TypeInfo* fromType, TypeInfo* toType, bool isNeg)
{
    if (isNeg)
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::F32:
                return context->report({fromNode, formErr(Saf0022, fromNode->computedValue()->reg.f32, toType->getDisplayNameC())});
            case NativeTypeKind::F64:
                return context->report({fromNode, formErr(Saf0022, fromNode->computedValue()->reg.f64, toType->getDisplayNameC())});
            default:
                return context->report({fromNode, formErr(Saf0020, fromNode->computedValue()->reg.s64, toType->getDisplayNameC())});
        }
    }

    if (fromNode->is(AstNodeKind::Literal) && fromNode->token.text.length() > 2)
    {
        if (std::tolower(fromNode->token.text[1]) == 'x' || std::tolower(fromNode->token.text[1]) == 'b')
        {
            return context->report({fromNode, formErr(Saf0023, fromNode->token.cstr(), fromNode->computedValue()->reg.u64, toType->getDisplayNameC())});
        }
    }

    switch (fromType->nativeType)
    {
        case NativeTypeKind::F32:
            return context->report({fromNode, formErr(Saf0026, fromNode->computedValue()->reg.f32, toType->getDisplayNameC())});
        case NativeTypeKind::F64:
            return context->report({fromNode, formErr(Saf0026, fromNode->computedValue()->reg.f64, toType->getDisplayNameC())});
        default:
            if (fromType->isNativeIntegerSigned())
                return context->report({fromNode, formErr(Saf0025, fromNode->computedValue()->reg.s64, toType->getDisplayNameC())});
            return context->report({fromNode, formErr(Saf0024, fromNode->computedValue()->reg.u64, toType->getDisplayNameC())});
    }
}

bool TypeManager::safetyComputedValue(SemanticContext* context, const TypeInfo* toType, const TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (!fromNode || !fromNode->hasFlagComputedValue())
        return true;
    if (castFlags.has(CAST_FLAG_JUST_CHECK))
        return true;
    if (!castFlags.has(CAST_FLAG_EXPLICIT))
        return true;
    if (!fromNode->token.sourceFile->module->mustEmitSafety(fromNode, SafetyContext::Compiler, SafetyWhat::Overflow))
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

void TypeManager::getCastErrorMsg(Utf8& msg, Utf8& note, Vector<Utf8>& remarks, TypeInfo* toType, TypeInfo* fromType, CastFlags castFlags, CastErrorType castError, Vector<const Diagnostic*>& notes)
{
    msg.clear();
    note.clear();
    if (!toType || !fromType)
        return;

    if (castError == CastErrorType::SafetyCastAny)
    {
        msg = formErr(Saf0001, toType->getDisplayNameC());
    }
    else if (castError == CastErrorType::Const)
    {
        msg = formErr(Err0578, fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (castError == CastErrorType::SliceArray)
    {
        const auto to   = castTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);
        const auto from = castTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        note            = form("the array has [[%d]] elements of type [[%s]], which does not match a slice of type [[%s]]", from->totalCount, from->finalType->getDisplayNameC(), to->pointedType->getDisplayNameC());
    }
    else if (toType->isTypeValue() && !fromType->isTypeValue())
    {
        msg = toErr(Err0628);
    }
    else if (!toType->isPointerRef() && toType->isPointer() && fromType->isNativeInteger())
    {
        msg = formErr(Err0601, fromType->getDisplayNameC());
        if (!fromType->isNative(NativeTypeKind::U64))
            note = "only [[u64]] may be cast to a pointer";
    }
    else if (toType->isPointerArithmetic() && !fromType->isPointerArithmetic() && fromType->isPointer())
    {
        msg = formErr(Err0600, fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (toType->isInterface() && (fromType->isStruct() || fromType->isPointerTo(TypeInfoKind::Struct)))
    {
        if (fromType->isPointerTo(TypeInfoKind::Struct))
            fromType = castTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer)->pointedType;

        msg = formErr(Err0202, fromType->getDisplayNameC(), toType->getDisplayNameC());
        notes.push_back(Diagnostic::note(form("the [[%s]] does not implement the [[%s]]", fromType->getDisplayNameC(), toType->getDisplayNameC())));
    }
    else if (toType->isPointerTo(TypeInfoKind::Struct) && fromType->isInterface())
    {
        msg    = formErr(Err0202, fromType->getDisplayNameC(), toType->getDisplayNameC());
        toType = castTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer)->pointedType;
        notes.push_back(Diagnostic::note(form("the [[%s]] does not implement the [[%s]]", toType->getDisplayNameC(), fromType->getDisplayNameC())));
    }
    else if (toType->isStruct() && fromType->isInterface())
    {
        note = Diagnostic::isType(fromType);
        msg  = toErr(Err0529);
    }
    else if (fromType->isPointerToTypeInfo() && !toType->isPointerToTypeInfo())
    {
        note = form("this is a type value, also known as [[typeinfo]], or [[%s]]", fromType->getDisplayNameC());
        msg  = formErr(Err0528, toType->getDisplayNameC());
    }
    else if (fromType->isClosure() && toType->isLambda())
    {
        note = "a lambda can be converted to a closure, but not the other way around";
        msg  = toErr(Err0527);
    }
    else if (toType->isLambdaClosure() && fromType->isLambdaClosure())
    {
        const auto fromTypeFunc = castTypeInfo<TypeInfoFuncAttr>(fromType, TypeInfoKind::LambdaClosure);
        if (fromTypeFunc->firstDefaultValueIdx != UINT32_MAX)
        {
            msg = toErr(Err0388);
        }
        else
        {
            msg                   = formErr(Err0530, fromType->getDisplayNameC(), toType->getDisplayNameC());
            const auto toTypeFunc = castTypeInfo<TypeInfoFuncAttr>(toType, TypeInfoKind::LambdaClosure);

            BadSignatureInfos bi;
            fromTypeFunc->isSame(toTypeFunc, castFlags | CAST_FLAG_EXACT, bi);
            switch (bi.matchResult)
            {
                case MatchResult::NotEnoughArguments:
                    remarks.push_back("too many parameters");
                    break;
                case MatchResult::TooManyArguments:
                    remarks.push_back("not enough parameters");
                    break;
                case MatchResult::MismatchThrow:
                    remarks.push_back("mismatch throw");
                    break;
                case MatchResult::MissingReturnType:
                    remarks.push_back("invalid return type");
                    break;
                case MatchResult::NoReturnType:
                    remarks.push_back("missing return type");
                    break;
                case MatchResult::MismatchReturnType:
                    remarks.push_back("mismatch return type");
                    break;
                case MatchResult::BadSignature:
                    remarks.push_back(form("mismatch signature (%s)", Naming::niceParameterRank(bi.badSignatureNum1 + 1).cstr()));
                    break;
            }
        }
    }
    else if (!fromType->isPointer() && toType->isPointerRef())
    {
        const auto toPtrRef = castTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);
        if (fromType->isSame(toPtrRef->pointedType, CAST_FLAG_CAST))
            note = "hint: take the address with [['&']] to create a reference";
    }
    else if (toType->isTuple() && fromType->isTuple())
    {
        Utf8 toName, fromName;
        toType->computeWhateverName(toName, ComputeNameKind::DisplayName);
        fromType->computeWhateverName(fromName, ComputeNameKind::DisplayName);
        remarks.push_back(form("the source type is [[%s]]", fromName.cstr()));
        remarks.push_back(form("the requested type is [[%s]]", toName.cstr()));

        msg = toErr(Err0526);
    }
}

bool TypeManager::castError(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags, CastErrorType castErrorType)
{
    // Last minute change: convert 'fromType' (struct) to 'toType' with an opCast
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
        getCastErrorMsg(msg, hint, remarks, toType, fromType, castFlags, castErrorType, notes);
        SWAG_ASSERT(fromNode);

        if (msg.empty())
            msg = formErr(Err0530, fromType->getDisplayNameC(), toType->getDisplayNameC());
        if (!hint.empty())
            notes.push_back(Diagnostic::note(fromNode, hint));

        // Is there an explicit cast possible?
        if (!castFlags.has(CAST_FLAG_EXPLICIT) || castFlags.has(CAST_FLAG_COERCE))
        {
            if (!toType->isTypeValue() && !fromType->isTypeValue())
            {
                if (makeCompatibles(context, toType, fromType, nullptr, nullptr, CAST_FLAG_EXPLICIT | CAST_FLAG_JUST_CHECK))
                    notes.push_back(Diagnostic::note(fromNode, form("hint: add an explicit [[cast(%s)]] if necessary", toType->getDisplayNameC())));
            }
        }

        Diagnostic err{fromNode, msg};
        err.remarks = remarks;

        // Add a note in case we affect to an identifier.
        if (context->node->is(AstNodeKind::AffectOp) ||
            context->node->is(AstNodeKind::BinaryOp) ||
            context->node->is(AstNodeKind::NullConditionalExpression) ||
            context->node->is(AstNodeKind::FactorOp))
        {
            const auto left = context->node->firstChild();
            notes.push_back(Diagnostic::isType(left));
        }

        return context->report(err, notes);
    }

    return false;
}
