#include "pch.h"
#include "TypeManager.h"
#include "SourceFile.h"
#include "Module.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool TypeManager::errorOutOfRange(SemanticContext* context, AstNode* fromNode, TypeInfo* fromType, TypeInfo* toType, bool isNeg)
{
    if (isNeg)
        return context->report(fromNode, Fmt(Err(Err0180), fromNode->computedValue->reg.s64, toType->getDisplayNameC()));

    if (fromNode->kind == AstNodeKind::Literal && fromNode->token.text.length() > 2)
    {
        if (std::tolower(fromNode->token.text[1]) == 'x' || std::tolower(fromNode->token.text[1]) == 'b')
            return context->report(fromNode, Fmt(Err(Err0183), fromNode->token.ctext(), fromNode->computedValue->reg.u64, toType->getDisplayNameC()));
    }

    return context->report(fromNode, Fmt(Err(Err0181), fromNode->computedValue->reg.u64, toType->getDisplayNameC()));
}

bool TypeManager::safetyComputedValue(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (!fromNode || !(fromNode->flags & AST_VALUE_COMPUTED))
        return true;
    if (castFlags & (CASTFLAG_NO_ERROR | CASTFLAG_JUST_CHECK))
        return true;
    if (!fromNode->sourceFile->module->mustEmitSafetyOF(fromNode))
        return true;
    if (!(castFlags & CASTFLAG_EXPLICIT))
        return true;

    auto msg  = ByteCodeGenJob::safetyMsg(SafetyMsg::CastTruncated, toType, fromType);
    auto msg1 = ByteCodeGenJob::safetyMsg(SafetyMsg::CastNeg, toType, fromType);

    // Negative value to unsigned type
    if (fromType->isNativeIntegerSigned() && toType->isNativeIntegerUnsignedOrRune() && fromNode->computedValue->reg.s64 < 0)
        return context->report(fromNode ? fromNode : context->node, msg1);

    switch (toType->nativeType)
    {
    case NativeTypeKind::U8:
        if (fromNode->computedValue->reg.u64 > UINT8_MAX)
            return context->report(fromNode ? fromNode : context->node, msg);
        break;
    case NativeTypeKind::U16:
        if (fromNode->computedValue->reg.u64 > UINT16_MAX)
            return context->report(fromNode ? fromNode : context->node, msg);
        break;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        if (fromNode->computedValue->reg.u64 > UINT32_MAX)
            return context->report(fromNode ? fromNode : context->node, msg);
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        if (fromType->isNativeIntegerSigned())
        {
            if (fromNode->computedValue->reg.u64 > INT64_MAX)
                return context->report(fromNode ? fromNode : context->node, msg);
        }
        break;

    case NativeTypeKind::S8:
        if (fromNode->computedValue->reg.s64 < INT8_MIN || fromNode->computedValue->reg.s64 > INT8_MAX)
            return context->report(fromNode ? fromNode : context->node, msg);
        break;
    case NativeTypeKind::S16:
        if (fromNode->computedValue->reg.s64 < INT16_MIN || fromNode->computedValue->reg.s64 > INT16_MAX)
            return context->report(fromNode ? fromNode : context->node, msg);
        break;
    case NativeTypeKind::S32:
        if (fromNode->computedValue->reg.s64 < INT32_MIN || fromNode->computedValue->reg.s64 > INT32_MAX)
            return context->report(fromNode ? fromNode : context->node, msg);
        break;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        if (!fromType->isNativeIntegerSigned())
        {
            if (fromNode->computedValue->reg.u64 > INT64_MAX)
                return context->report(fromNode ? fromNode : context->node, msg);
        }
        break;
    }

    return true;
}

bool TypeManager::tryOpAffect(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    auto structType = toType;
    if (castFlags & CASTFLAG_UFCS && structType->isPointerTo(TypeInfoKind::Struct))
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType   = typePtr->pointedType;
    }

    if (structType->kind == TypeInfoKind::Struct && (castFlags & (CASTFLAG_EXPLICIT | CASTFLAG_AUTO_OPCAST)))
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(structType, TypeInfoKind::Struct);
        if (!typeStruct->declNode)
            return false;

        auto structNode = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
        Utf8 nameAffect;
        if ((fromNode && fromNode->semFlags & AST_SEM_LITERAL_SUFFIX) || castFlags & CASTFLAG_LITERAL_SUFFIX)
            nameAffect = g_LangSpec->name_opAffectSuffix;
        else
            nameAffect = g_LangSpec->name_opAffect;

        auto symbol = structNode->scope->symTable.find(nameAffect);

        // Instantiated opAffect, in a generic struct, will be in the scope of the original struct, not the intantiated one
        if (!symbol && typeStruct->fromGeneric)
        {
            structNode = CastAst<AstStruct>(typeStruct->fromGeneric->declNode, AstNodeKind::StructDecl);
            symbol     = structNode->scope->symTable.find(nameAffect);
        }

        if (!symbol)
            return false;

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
        VectorNative<SymbolOverload*> toAffect;
        for (auto over : symbol->overloads)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (!(typeFunc->declNode->attributeFlags & ATTRIBUTE_IMPLICIT) && !(castFlags & CASTFLAG_EXPLICIT))
                continue;
            if (typeFunc->parameters.size() <= 1)
                continue;
            if (makeCompatibles(context, typeFunc->parameters[1]->typeInfo, fromType, nullptr, nullptr, CASTFLAG_NO_LAST_MINUTE | CASTFLAG_TRY_COERCE | CASTFLAG_NO_ERROR | CASTFLAG_JUST_CHECK))
                toAffect.push_back(over);
        }

        if (toAffect.empty())
            return false;

        // :opAffectParam
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->flags |= AST_OPAFFECT_CAST;
            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toType;
            fromNode->allocateExtension();
            fromNode->extension->resolvedUserOpSymbolOverload = toAffect[0];
        }

        return true;
    }

    return false;
}

bool TypeManager::tryOpCast(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    auto structType = fromType;
    if (castFlags & CASTFLAG_UFCS && structType->isPointerTo(TypeInfoKind::Struct))
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType   = typePtr->pointedType;
    }

    if (structType->kind == TypeInfoKind::Struct && (castFlags & (CASTFLAG_EXPLICIT | CASTFLAG_AUTO_OPCAST)))
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(structType, TypeInfoKind::Struct);
        if (!typeStruct->declNode)
            return false;

        auto structNode = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
        auto symbol     = structNode->scope->symTable.find(g_LangSpec->name_opCast);

        // Instantiated opCast, in a generic struct, will be in the scope of the original struct, not the intantiated one
        if (!symbol && typeStruct->fromGeneric)
        {
            structNode = CastAst<AstStruct>(typeStruct->fromGeneric->declNode, AstNodeKind::StructDecl);
            symbol     = structNode->scope->symTable.find(g_LangSpec->name_opCast);
        }

        if (!symbol)
            return false;

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
        VectorNative<SymbolOverload*> toCast;
        for (auto over : symbol->overloads)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc->flags & TYPEINFO_GENERIC || typeFunc->returnType->flags & TYPEINFO_GENERIC)
                continue;
            if (!(typeFunc->declNode->attributeFlags & ATTRIBUTE_IMPLICIT) && !(castFlags & CASTFLAG_EXPLICIT))
                continue;
            if (typeFunc->returnType->isSame(toType, ISSAME_EXACT))
                toCast.push_back(over);
        }

        if (toCast.empty())
            return false;

        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toType;
            fromNode->allocateExtension();
            fromNode->extension->resolvedUserOpSymbolOverload = toCast[0];
            fromNode->semFlags |= AST_SEM_USER_CAST;
        }

        return true;
    }

    return false;
}

void TypeManager::getCastErrorMsg(Utf8& msg, Utf8& hint, TypeInfo* toType, TypeInfo* fromType, uint32_t castFlags)
{
    msg.clear();
    hint.clear();
    if (!toType || !fromType)
        return;

    if (toType->kind == TypeInfoKind::Interface && ((fromType->kind == TypeInfoKind::Struct) || (fromType->isPointerTo(TypeInfoKind::Struct))))
    {
        auto fromTypeCpy = fromType;
        if (fromTypeCpy->isPointerTo(TypeInfoKind::Struct))
        {
            hint        = Hint::isType(fromTypeCpy);
            fromTypeCpy = CastTypeInfo<TypeInfoPointer>(fromTypeCpy, TypeInfoKind::Pointer)->pointedType;
        }

        msg = Fmt(Err(Err0176), fromTypeCpy->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (!toType->isPointerRef() && toType->kind == TypeInfoKind::Pointer && (fromType->isNativeIntegerOrRune() || fromType->isNativeFloat() || fromType->isNative(NativeTypeKind::Bool)))
    {
        hint = Hnt(Hnt0005);
        msg  = Fmt(Err(Err0907), fromType->getDisplayNameC());
    }
    else if (castFlags & CASTFLAG_CONST_ERR)
    {
        hint = Hnt(Hnt0022);
        msg  = Fmt(Err(Err0418), fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (fromType->isPointerToTypeInfo() && !toType->isPointerToTypeInfo())
    {
        hint = Hnt(Hnt0040);
        msg  = Fmt(Err(Err0177), fromType->getDisplayNameC(), toType->getDisplayNameC());
    }
    else if (fromType->isClosure() && toType->isLambda())
    {
        msg = Fmt(Err(Err0178));
    }
}

bool TypeManager::castError(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
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

    if (!(castFlags & CASTFLAG_NO_ERROR))
    {
        // More specific message
        Utf8 hint, msg;
        getCastErrorMsg(msg, hint, toType, fromType, castFlags);

        SWAG_ASSERT(fromNode);
        if (fromNode != context->node)
            context->errorContextStack.push_back({context->node, JobContext::ErrorContextType::Node});

        // Is there an explicit cast possible ?
        if (!(castFlags & CASTFLAG_EXPLICIT))
        {
            if (TypeManager::makeCompatibles(context, toType, fromType, nullptr, nullptr, CASTFLAG_EXPLICIT | CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            {
                PushErrHint errh(Fmt(Hnt(Hnt0032), fromType->getDisplayNameC(), toType->getDisplayNameC()));
                Diagnostic  diag{fromNode, Fmt(Err(Err0175), fromType->getDisplayNameC(), toType->getDisplayNameC())};
                return context->report(diag);
            }
        }

        // A specific error message ?
        if (!msg.empty())
        {
            PushErrHint errh(hint);
            return context->report(hint, {fromNode, msg});
        }

        // General cast error
        return context->report(fromNode, Fmt(Err(Err0177), fromType->getDisplayNameC(), toType->getDisplayNameC()));
    }

    return false;
}

bool TypeManager::castToNativeBool(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType == g_TypeMgr->typeInfoBool)
        return true;

    // Automatic cast to a bool is done only if requested, on specific nodes (like if or while expressions)
    if (!(castFlags & CASTFLAG_AUTO_BOOL) && !(castFlags & CASTFLAG_EXPLICIT))
        return castError(context, g_TypeMgr->typeInfoBool, fromType, fromNode, castFlags);

    fromType = TypeManager::concreteType(fromType);

    if (fromType->kind == TypeInfoKind::Pointer ||
        fromType->kind == TypeInfoKind::Lambda ||
        fromType->kind == TypeInfoKind::Interface ||
        fromType->kind == TypeInfoKind::Slice)
    {
        if (!(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->typeInfo       = g_TypeMgr->typeInfoBool;
            fromNode->castedTypeInfo = fromType;
            return true;
        }
    }

    if (fromType->kind == TypeInfoKind::Native)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->flags & AST_VALUE_COMPUTED)
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
                    if (fromNode->flags & AST_VALUE_COMPUTED)
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
                    if (fromNode->flags & AST_VALUE_COMPUTED)
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
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->flags & AST_VALUE_COMPUTED)
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
                    if (fromNode->flags & AST_VALUE_COMPUTED)
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
                    if (fromNode->flags & AST_VALUE_COMPUTED)
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
                    if (fromNode->flags & AST_VALUE_COMPUTED)
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
        }
    }

    return castError(context, g_TypeMgr->typeInfoBool, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeRune(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::Rune)
        return true;

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr->typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f32;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f64;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::String:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
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
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::String:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
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
        }

        if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
        {
            if (!fromNode)
            {
                auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                if (value < 0)
                    return false;
            }

            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoRune, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U8)
        return true;

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8, true);
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.u64 > UINT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8);
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoU8;
                return true;
            }
            else if ((fromType->flags & TYPEINFO_UNTYPED_INTEGER) || (fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
            {
                if (!fromNode)
                {
                    auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                    if (value > UINT8_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    // Try with promoted type
    if (fromType->promotedFrom && fromType->promotedFrom->isNative(NativeTypeKind::U8) && !(castFlags & CASTFLAG_EXPLICIT))
        castFlags |= CASTFLAG_EXPLICIT | CASTFLAG_FROM_PROMOTE;

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && (castFlags & CASTFLAG_FROM_PROMOTE))
            {
                fromNode->castedTypeInfo = fromType;
                fromNode->typeInfo       = g_TypeMgr->typeInfoU8;
            }
            else if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr->typeInfoU8;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f32;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU8;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f64;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU8;
                }
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoU8, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U16)
        return true;

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16, true);
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.u64 > UINT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16);
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoU16;
                return true;
            }
            else if ((fromType->flags & TYPEINFO_UNTYPED_INTEGER) || (fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
            {
                if (!fromNode)
                {
                    auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                    if (value > UINT16_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    // Try with promoted type
    if (fromType->promotedFrom && fromType->promotedFrom->isNative(NativeTypeKind::U16) && !(castFlags & CASTFLAG_EXPLICIT))
        castFlags |= CASTFLAG_EXPLICIT | CASTFLAG_FROM_PROMOTE;

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && (castFlags & CASTFLAG_FROM_PROMOTE))
            {
                fromNode->castedTypeInfo = fromType;
                fromNode->typeInfo       = g_TypeMgr->typeInfoU16;
            }
            else if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr->typeInfoU16;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f32;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU16;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f64;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU16;
                }
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoU16, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U32)
        return true;

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32, true);
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.u64 > UINT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32);
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoU32;
                return true;
            }
            else if ((fromType->flags & TYPEINFO_UNTYPED_INTEGER) || (fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
            {
                return true;
            }

            break;
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
        case NativeTypeKind::Int:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr->typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.f32;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint32_t) fromNode->computedValue->reg.f64;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU32;
                }
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoU32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U64 || fromType->nativeType == NativeTypeKind::UInt)
        return true;

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU64, true);
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoU64;
                return true;
            }
            else if ((fromType->flags & TYPEINFO_UNTYPED_INTEGER) || (fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
            {
                return true;
            }

            break;
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
        case NativeTypeKind::Int:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr->typeInfoU64;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f32;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU64;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f64;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoU64;
                }
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoU64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeUInt(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U64 || fromType->nativeType == NativeTypeKind::UInt)
        return true;

    if (fromType->kind == TypeInfoKind::Pointer || fromType->kind == TypeInfoKind::Lambda)
    {
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
                fromNode->typeInfo = g_TypeMgr->typeInfoUInt;
            return true;
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
        case NativeTypeKind::Int:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoUInt, true);
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoUInt;
                return true;
            }
            else if ((fromType->flags & TYPEINFO_UNTYPED_INTEGER) || (fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
            {
                return true;
            }

            break;
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
        case NativeTypeKind::Int:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr->typeInfoUInt;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f32;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoUInt;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.u64 = (uint64_t) fromNode->computedValue->reg.f64;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoUInt;
                }
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoUInt, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S8)
        return true;

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
        {
            if (!(fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
                break;
            auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
            if (value <= UINT8_MAX)
                return true;
            break;
        }

        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.s64 < INT8_MIN || fromNode->computedValue->reg.s64 > INT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS8;
                return true;
            }
            else if ((fromType->flags & TYPEINFO_UNTYPED_INTEGER) || (fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
            {
                if (!fromNode)
                {
                    auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                    if (value < INT8_MIN || value > INT8_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    // Try with promoted type
    if (fromType->promotedFrom && fromType->promotedFrom->isNative(NativeTypeKind::S8) && !(castFlags & CASTFLAG_EXPLICIT))
        castFlags |= CASTFLAG_EXPLICIT | CASTFLAG_FROM_PROMOTE;

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && (castFlags & CASTFLAG_FROM_PROMOTE))
            {
                fromNode->castedTypeInfo = fromType;
                fromNode->typeInfo       = g_TypeMgr->typeInfoS8;
            }
            else if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS8;
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f32;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoS8;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f64;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoS8;
                }
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoS8, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S16)
        return true;

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
        {
            if (!(fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
                break;
            auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
            if (value <= UINT16_MAX)
                return true;
            break;
        }

        case NativeTypeKind::S8:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.s64 < INT16_MIN || fromNode->computedValue->reg.s64 > INT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS16;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                    if (value < INT16_MIN || value > INT16_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    // Try with promoted type
    if (fromType->promotedFrom && fromType->promotedFrom->isNative(NativeTypeKind::S16) && !(castFlags & CASTFLAG_EXPLICIT))
        castFlags |= CASTFLAG_EXPLICIT | CASTFLAG_FROM_PROMOTE;

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Rune:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && (castFlags & CASTFLAG_FROM_PROMOTE))
            {
                fromNode->castedTypeInfo = fromType;
                fromNode->typeInfo       = g_TypeMgr->typeInfoS16;
            }
            else if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS16;
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f32;
                    fromNode->typeInfo               = g_TypeMgr->typeInfoS16;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f64;
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS16;
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoS16, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S32)
        return true;

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
            if (fromType->flags & TYPEINFO_UNTYPED_BINHEXA)
                return true;

        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.s64 < INT32_MIN || fromNode->computedValue->reg.s64 > INT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS32;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                return true;
            }

            break;
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
        case NativeTypeKind::Int:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS32;
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f32;
                fromNode->typeInfo               = g_TypeMgr->typeInfoS32;
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f64;
                fromNode->typeInfo               = g_TypeMgr->typeInfoS32;
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoS32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S64 || fromType->nativeType == NativeTypeKind::Int)
        return true;

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
            if (fromType->flags & TYPEINFO_UNTYPED_BINHEXA)
                return true;

        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.s64 < INT64_MIN || fromNode->computedValue->reg.s64 > INT64_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS64;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                return true;
            }

            break;
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
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoS64;
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f32;
                fromNode->typeInfo               = g_TypeMgr->typeInfoS64;
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f64;
                fromNode->typeInfo               = g_TypeMgr->typeInfoS64;
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoS64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeInt(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S64 || fromType->nativeType == NativeTypeKind::Int)
        return true;

    if (!(castFlags & CASTFLAG_EXPLICIT) || (castFlags & CASTFLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue->reg.s64 < INT64_MIN || fromNode->computedValue->reg.s64 > INT64_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        errorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoInt);
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoInt;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                return true;
            }

            break;
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
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr->typeInfoInt;
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f32;
                fromNode->typeInfo               = g_TypeMgr->typeInfoInt;
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue->reg.s64 = (int64_t) fromNode->computedValue->reg.f64;
                fromNode->typeInfo               = g_TypeMgr->typeInfoInt;
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr->typeInfoInt, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeF32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::F32)
        return true;

    switch (fromType->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    {
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.s64;
                fromNode->typeInfo               = g_TypeMgr->typeInfoF32;
            }
            return true;
        }
        else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
        {
            if (!fromNode)
            {
                auto    value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                float   tmpF  = (float) value;
                int64_t tmpI  = (int64_t) tmpF;
                if (tmpI != value)
                    return false;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }

    case NativeTypeKind::Bool:
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->allocateComputedValue();
                fromNode->computedValue->reg.f32 = fromNode->computedValue->reg.b ? 1.0f : 0.0f;
                fromNode->typeInfo               = g_TypeMgr->typeInfoF32;
            }
            return true;
        }
        break;

    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
    {
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.u64;
                fromNode->typeInfo               = g_TypeMgr->typeInfoF32;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }

    case NativeTypeKind::F64:
    {
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue->reg.f32 = (float) fromNode->computedValue->reg.f64;
                fromNode->typeInfo               = g_TypeMgr->typeInfoF32;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }
    }

    return castError(context, g_TypeMgr->typeInfoF32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeF64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::F64)
        return true;

    switch (fromType->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    {
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.s64;
                fromNode->typeInfo               = g_TypeMgr->typeInfoF64;
            }
            return true;
        }
        else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
        {
            if (!fromNode)
            {
                auto    value = CastTypeInfo<TypeInfoNative>(fromType, fromType->kind)->valueInteger;
                double  tmpF  = (double) value;
                int64_t tmpI  = (int64_t) tmpF;
                if (tmpI != value)
                    return false;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }

    case NativeTypeKind::Bool:
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->allocateComputedValue();
                fromNode->computedValue->reg.f64 = fromNode->computedValue->reg.b ? 1.0f : 0.0f;
                fromNode->typeInfo               = g_TypeMgr->typeInfoF64;
            }
            return true;
        }
        break;

    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
    {
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->allocateComputedValue();
                fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.u64;
                fromNode->typeInfo               = g_TypeMgr->typeInfoF64;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }

    case NativeTypeKind::F32:
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->allocateComputedValue();
                fromNode->computedValue->reg.f64 = (double) fromNode->computedValue->reg.f32;
                fromNode->typeInfo               = g_TypeMgr->typeInfoF64;
            }

            return true;
        }
        else if (fromType->flags & TYPEINFO_UNTYPED_FLOAT)
            return true;
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }

    return castError(context, g_TypeMgr->typeInfoF64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags)
{
    // Pick the best order
    if (castFlags & CASTFLAG_COMMUTATIVE)
    {
        if ((toType->flags & TYPEINFO_UNTYPED_INTEGER) && !(fromType->flags & TYPEINFO_UNTYPED_INTEGER))
        {
            swap(toType, fromType);
            swap(toNode, fromNode);
        }
        else if ((toType->flags & TYPEINFO_UNTYPED_FLOAT) && !(fromType->flags & TYPEINFO_UNTYPED_FLOAT))
        {
            swap(toType, fromType);
            swap(toNode, fromNode);
        }
    }

    return castToNative(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
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
    case NativeTypeKind::UInt:
        SWAG_CHECK(castToNativeUInt(context, fromType, fromNode, castFlags));
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
    case NativeTypeKind::Int:
        SWAG_CHECK(castToNativeInt(context, fromType, fromNode, castFlags));
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
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK) && !(fromNode->flags & AST_VALUE_COMPUTED))
        {
            fromNode->typeInfo = toType;
            if (!fromNode->castedTypeInfo)
                fromNode->castedTypeInfo = fromType;
        }
    }

    return true;
}

bool TypeManager::castExpressionList(SemanticContext* context, TypeInfoList* fromTypeList, TypeInfo* toType, AstNode* fromNode, uint32_t castFlags)
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
    if (toType->kind == TypeInfoKind::TypeListTuple || toType->kind == TypeInfoKind::TypeListArray)
        toTypeList = CastTypeInfo<TypeInfoList>(toType, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);

    for (int i = 0; i < fromSize; i++)
    {
        auto child     = fromNode ? fromNode->childs[i] : nullptr;
        auto convertTo = toTypeList ? toTypeList->subTypes[i]->typeInfo : toType;

        // Expression list inside another expression list (like a struct inside an array)
        if (child && child->kind == AstNodeKind::ExpressionList && convertTo->kind == TypeInfoKind::Struct)
        {
            auto toTypeStruct = CastTypeInfo<TypeInfoStruct>(convertTo, TypeInfoKind::Struct);
            bool hasChanged   = false;

            if (toTypeStruct->fields.size() > child->childs.size())
                return context->report(child, Fmt(Err(Err0196), toTypeStruct->name.c_str(), toTypeStruct->fields.size(), child->childs.size()));
            if (toTypeStruct->fields.size() < child->childs.size())
                return context->report(child, Fmt(Err(Err0197), toTypeStruct->name.c_str(), toTypeStruct->fields.size(), child->childs.size()));

            auto count = toTypeStruct->fields.size();
            for (int j = 0; j < count; j++)
            {
                auto childJ  = child->childs[j];
                auto oldType = childJ->typeInfo;
                SWAG_CHECK(TypeManager::makeCompatibles(context, toTypeStruct->fields[j]->typeInfo, childJ->typeInfo, nullptr, childJ, castFlags | CASTFLAG_TRY_COERCE));
                if (childJ->typeInfo != oldType)
                    hasChanged = true;

                // Collect array to slice : will need special treatment when collecting constants
                if (childJ->typeInfo->kind == TypeInfoKind::TypeListArray && toTypeStruct->fields[j]->typeInfo->kind == TypeInfoKind::Slice)
                {
                    childJ->allocateExtension();
                    childJ->extension->collectTypeInfo = toTypeStruct->fields[j]->typeInfo;
                }

                // We use castOffset to store the offset between one field and one other, in order to collect later at
                // the right position
                childJ->allocateExtension();
                childJ->extension->castOffset = 0;
                if (j)
                {
                    child->childs[j - 1]->extension->castOffset = toTypeStruct->fields[j]->offset - toTypeStruct->fields[j - 1]->offset;
                    if (child->childs[j - 1]->extension->castOffset != child->childs[j - 1]->typeInfo->sizeOf)
                        hasChanged = true;
                }

                // For the last field, padding will be the difference between the field offset and the struct size
                // (because struct sizeof is aligned too, and padding can be added at the end)
                if (j == count - 1)
                {
                    childJ->extension->castOffset = toTypeStruct->sizeOf - toTypeStruct->fields[j]->offset;
                    if (childJ->extension->castOffset != childJ->typeInfo->sizeOf)
                        hasChanged = true;
                }
            }

            if (child->typeInfo->sizeOf != toTypeStruct->sizeOf)
                hasChanged = true;

            if (hasChanged)
                SemanticJob::computeExpressionListTupleType(context, child);
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, convertTo, fromTypeList->subTypes[i]->typeInfo, nullptr, child, castFlags | CASTFLAG_TRY_COERCE));
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
        for (int i = 0; i < fromTypeList->subTypes.size(); i++)
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

bool TypeManager::castToString(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType == g_TypeMgr->typeInfoNull)
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            if (fromNode->flags & AST_VALUE_COMPUTED)
                fromNode->computedValue->text.reset();
            fromNode->typeInfo       = toType;
            fromNode->castedTypeInfo = g_TypeMgr->typeInfoNull;
        }

        return true;
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // [..] u8 to string, this is possible !
        if (fromType->kind == TypeInfoKind::Slice)
        {
            auto fromTypeSlice = CastTypeInfo<TypeInfoSlice>(fromType, TypeInfoKind::Slice);
            if (fromTypeSlice->pointedType == g_TypeMgr->typeInfoU8)
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
        if (fromType->kind == TypeInfoKind::Array)
        {
            auto fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
            if (fromTypeArray->pointedType == g_TypeMgr->typeInfoU8)
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

bool TypeManager::castToFromAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags)
{
    if (toType->isNative(NativeTypeKind::Any))
    {
        if (fromType == g_TypeMgr->typeInfoNull)
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
                    toNode->allocateExtension();
                    toNode->extension->stackOffset = toNode->ownerScope->startStackSize;
                    toNode->ownerScope->startStackSize += toType->numRegisters() * sizeof(Register);
                    SemanticJob::setOwnerMaxStackSize(toNode, toNode->ownerScope->startStackSize);
                }

                toNode->castedTypeInfo = toType;
                toNode->typeInfo       = fromType;
                auto  module           = context->sourceFile->module;
                auto& typeTable        = module->typeTable;

                // :AnyTypeSegment
                toNode->allocateExtension();
                toNode->extension->anyTypeSegment = SemanticJob::getConstantSegFromContext(toNode);
                SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, fromType, toNode->extension->anyTypeSegment, &toNode->extension->anyTypeOffset));
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
                fromNode->allocateExtension();
                fromNode->extension->stackOffset = fromNode->ownerScope->startStackSize;
                fromNode->ownerScope->startStackSize += fromType->numRegisters() * sizeof(Register);
                SemanticJob::setOwnerMaxStackSize(fromNode, fromNode->ownerScope->startStackSize);
            }

            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toType;
            auto  module             = context->sourceFile->module;
            auto& typeTable          = module->typeTable;

            // :AnyTypeSegment
            fromNode->allocateExtension();
            fromNode->extension->anyTypeSegment = SemanticJob::getConstantSegFromContext(fromNode);
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, fromNode->castedTypeInfo, fromNode->extension->anyTypeSegment, &fromNode->extension->anyTypeOffset));
        }
    }
    else if (fromType->isNative(NativeTypeKind::Any))
    {
        if (!(castFlags & CASTFLAG_EXPLICIT))
        {
            // Ambigous. Do we check for a bool, or do we check for null
            if (toType->isNative(NativeTypeKind::Bool))
                return castError(context, toType, fromType, fromNode, castFlags);

            // To convert a simple any to something more complexe, need an explicit cast
            if (toType->kind == TypeInfoKind::Slice ||
                toType->kind == TypeInfoKind::Array ||
                toType->kind == TypeInfoKind::Pointer)
                return castError(context, toType, fromType, fromNode, castFlags);
        }

        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            // When casting something complexe to any, we will copy the value to the stack to be sure
            // that the memory layout is correct, without relying on registers being contiguous, and not being reallocated (by an optimize pass).
            // See ByteCodeGenJob::emitCastToNativeAny
            if (fromNode->ownerFct && fromType->numRegisters() > 1)
            {
                fromNode->allocateExtension();
                fromNode->extension->stackOffset = fromNode->ownerScope->startStackSize;
                fromNode->ownerScope->startStackSize += fromType->numRegisters() * sizeof(Register);
                SemanticJob::setOwnerMaxStackSize(fromNode, fromNode->ownerScope->startStackSize);
            }

            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toType;
            auto  module             = context->sourceFile->module;
            auto& typeTable          = module->typeTable;

            // :AnyTypeSegment
            fromNode->allocateExtension();
            fromNode->extension->anyTypeSegment = SemanticJob::getConstantSegFromContext(fromNode);
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, toType, fromNode->extension->anyTypeSegment, &fromNode->extension->anyTypeOffset));
        }
    }

    return true;
}

bool TypeManager::castStructToStruct(SemanticContext* context, TypeInfoStruct* toStruct, TypeInfoStruct* fromStruct, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags, bool& ok)
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
        if (testStruct->isSame(toStruct, ISSAME_CAST))
        {
            ok = true;
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                if (toType->flags & TYPEINFO_SELF)
                {
                    if (it.offset)
                    {
                        fromNode->allocateExtension();
                        fromNode->extension->castOffset = it.offset;
                        fromNode->castedTypeInfo        = fromNode->typeInfo;
                        fromNode->typeInfo              = toType;
                    }

                    continue;
                }

                // We will have to dereference the pointer to get the real thing
                if (it.field && it.field->typeInfo->kind == TypeInfoKind::Pointer)
                    fromNode->semFlags |= AST_SEM_DEREF_USING;
                fromNode->semFlags |= AST_SEM_USING;

                fromNode->allocateExtension();
                fromNode->extension->castOffset = it.offset;
                fromNode->castedTypeInfo        = fromNode->typeInfo;
                fromNode->typeInfo              = toType;
                continue;
            }
        }

        // No using ! We're done
        if ((castFlags & CASTFLAG_FOR_GENERIC) && !(it.typeStruct->flags & TYPEINFO_STRUCT_TYPEINFO))
            return true;

        auto structNode = CastAst<AstStruct>(it.typeStruct->declNode, AstNodeKind::StructDecl);
        if (!(structNode->specFlags & AST_SPEC_STRUCTDECL_HAS_USING))
            continue;

        context->job->waitOverloadCompleted(it.typeStruct->declNode->resolvedSymbolOverload);
        if (context->result != ContextResult::Done)
            return true;

        TypeInfoParam*  foundField  = nullptr;
        TypeInfoStruct* foundStruct = nullptr;
        for (auto field : it.typeStruct->fields)
        {
            if (!(field->flags & TYPEINFO_HAS_USING))
                continue;

            TypeInfoStruct* typeStruct = nullptr;
            if (field->typeInfo->kind == TypeInfoKind::Struct)
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
                    if (foundTypeStruct0 && foundTypeStruct1 && foundTypeStruct0->isSame(foundTypeStruct1, ISSAME_CAST))
                    {
                        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                        {
                            Diagnostic diag{fromNode, Fmt(Err(Err0200), fromType->getDisplayNameC(), toType->getDisplayNameC(), fromStruct->getDisplayNameC(), toStruct->getDisplayNameC())};
                            Diagnostic note1{foundField->declNode, Nte(Nte0015), DiagnosticLevel::Note};
                            Diagnostic note2{field->declNode, Nte(Nte0016), DiagnosticLevel::Note};
                            return context->report(diag, &note1, &note2);
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

    stack.push_back({fromTypeStruct, 0, nullptr});
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
                    Diagnostic note1{it.field->declNode, Nte(Nte0006), DiagnosticLevel::Note};
                    Diagnostic note2{foundField->declNode, Nte(Nte0062), DiagnosticLevel::Note};
                    return context->report(diag, &note1, &note2);
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
        if (!(structNode->specFlags & AST_SPEC_STRUCTDECL_HAS_USING))
            continue;

        context->job->waitOverloadCompleted(it.typeStruct->declNode->resolvedSymbolOverload);
        if (context->result != ContextResult::Done)
            return true;

        for (auto field : it.typeStruct->fields)
        {
            if (!(field->flags & TYPEINFO_HAS_USING))
                continue;
            if (field->typeInfo->kind != TypeInfoKind::Struct)
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
            accessName += field->namedParam;

            stack.push_back({typeStruct, it.offset + field->offset, field, accessName});
        }
    }

    return true;
}

bool TypeManager::castToInterface(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FOR_GENERIC)
        return castError(context, toType, fromType, fromNode, castFlags);

    auto toTypeItf = CastTypeInfo<TypeInfoStruct>(toType, TypeInfoKind::Interface);

    if (fromType == g_TypeMgr->typeInfoNull)
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            // We will copy the value to the stack to be sure/ that the memory layout is correct, without relying on
            // registers being contiguous, and not being reallocated (by an optimize pass).
            // This is the same problem when casting to 'any'.
            // See ByteCodeGenJob::emitCastToNativeAny
            if (fromNode->ownerFct)
            {
                fromNode->allocateExtension();
                fromNode->extension->stackOffset = fromNode->ownerScope->startStackSize;
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
    if (fromType->kind == TypeInfoKind::Struct || fromType->isPointerTo(TypeInfoKind::Struct))
    {
        context->job->waitAllStructInterfaces(fromType);
        if (context->result != ContextResult::Done)
        {
            SWAG_ASSERT(castFlags & CASTFLAG_ACCEPT_PENDING);
            return true;
        }

        auto typeStruct = fromType;
        if (fromType->kind == TypeInfoKind::Pointer)
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
                fromNode->allocateExtension();
                fromNode->extension->castOffset = itfRef.fieldOffset;
                fromNode->extension->castItf    = itfRef.itf;
                fromNode->castedTypeInfo        = fromType;
                fromNode->typeInfo              = toTypeItf;
            }

            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToReference(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    auto toTypeReference = CastTypeInfo<TypeInfoReference>(toType, TypeInfoKind::Reference);

    // Same referenced type
    if (toTypeReference->pointedType->isSame(fromType, ISSAME_CAST))
        return true;

    // Structure to interface reference
    if (fromType->kind == TypeInfoKind::Struct && toTypeReference->pointedType->kind == TypeInfoKind::Interface)
    {
        auto toTypeItf      = CastTypeInfo<TypeInfoStruct>(toTypeReference->pointedType, TypeInfoKind::Interface);
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

    // Struct to struct
    if (fromType->kind == TypeInfoKind::Struct && toTypeReference->pointedType->kind == TypeInfoKind::Struct)
    {
        auto fromStruct = CastTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        auto toStruct   = CastTypeInfo<TypeInfoStruct>(toTypeReference->pointedType, TypeInfoKind::Struct);
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
    if (fromType->isPointerTo(TypeInfoKind::Struct) && toTypeReference->pointedType->kind == TypeInfoKind::Struct)
    {
        auto fromPtr    = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        auto fromStruct = CastTypeInfo<TypeInfoStruct>(fromPtr->pointedType, TypeInfoKind::Struct);
        auto toStruct   = CastTypeInfo<TypeInfoStruct>(toTypeReference->pointedType, TypeInfoKind::Struct);
        bool ok         = false;
        SWAG_CHECK(castStructToStruct(context, toStruct, fromStruct, toType, fromType, fromNode, castFlags, ok));
        if (ok || context->result == ContextResult::Pending)
        {
            if (fromStruct != toStruct)
                context->castFlagsResult |= CASTFLAG_RESULT_STRUCT_CONVERT;
            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToPointerRef(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    auto toTypePointer = CastTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);

    if (fromType->kind == TypeInfoKind::Pointer)
    {
        // Convert from pointer to ref : only if authorized
        if (!(fromType->flags & TYPEINFO_POINTER_REF) && !(castFlags & CASTFLAG_EXPLICIT) && !(castFlags & CASTFLAG_PTR_REF))
            return castError(context, toType, fromType, fromNode, castFlags);

        // Compare pointed types
        auto fromTypePointer = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        if (toTypePointer->pointedType->isSame(fromTypePointer->pointedType, ISSAME_CAST))
            return true;
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToPointer(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    // To "cstring"
    if (toType->isCString())
    {
        if (fromType->isNative(NativeTypeKind::String))
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
    if (fromType->kind == TypeInfoKind::Pointer && toTypePointer->pointedType->kind == TypeInfoKind::Struct)
    {
        auto fromTypePointer = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        if (fromTypePointer->pointedType->kind == TypeInfoKind::Struct)
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
    if ((castFlags & CASTFLAG_UFCS) && fromType->kind == TypeInfoKind::Struct && toTypePointer->pointedType->kind == TypeInfoKind::Struct)
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
    if (fromType->kind == TypeInfoKind::Lambda)
    {
        if ((castFlags & CASTFLAG_EXPLICIT) && toType->isPointerVoid())
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->typeInfo = g_TypeMgr->typeInfoConstPointers[(int) NativeTypeKind::Void];
            }

            return true;
        }
    }

    // Pointer to pointer
    if (fromType->kind == TypeInfoKind::Pointer)
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

        // Cannot cast from non arithmetic to arithmetic
        if ((toType->flags & TYPEINFO_POINTER_ARITHMETIC) && !(fromType->flags & TYPEINFO_POINTER_ARITHMETIC))
        {
            if (castFlags & CASTFLAG_FOR_AFFECT)
                return castError(context, toType, fromType, fromNode, castFlags);
            return true;
        }
    }

    // Array to pointer of the same type
    if (fromType->kind == TypeInfoKind::Array)
    {
        auto fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        if ((!(castFlags & CASTFLAG_NO_IMPLICIT) && toTypePointer->pointedType->isNative(NativeTypeKind::Void)) ||
            (!(castFlags & CASTFLAG_NO_IMPLICIT) && toTypePointer->pointedType->isSame(fromTypeArray->pointedType, ISSAME_CAST)) ||
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
    if (fromType->kind == TypeInfoKind::Slice)
    {
        auto fromTypeSlice = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Slice);
        if ((!(castFlags & CASTFLAG_NO_IMPLICIT) && toTypePointer->pointedType->isNative(NativeTypeKind::Void)) ||
            (!(castFlags & CASTFLAG_NO_IMPLICIT) && toTypePointer->pointedType->isSame(fromTypeSlice->pointedType, ISSAME_CAST)) ||
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
    if (fromType->kind == TypeInfoKind::Struct || fromType->kind == TypeInfoKind::Interface)
    {
        if ((castFlags & CASTFLAG_EXPLICIT) || (toTypePointer->flags & TYPEINFO_SELF) || toTypePointer->isConst() || (castFlags & CASTFLAG_UFCS))
        {
            // to *void or *structure
            if (toTypePointer->pointedType->isNative(NativeTypeKind::Void) ||
                toTypePointer->pointedType->isSame(fromType, ISSAME_CAST))
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK) && !(toTypePointer->flags & TYPEINFO_SELF))
                {
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = toType;
                }

                return true;
            }
        }
    }

    // Interface back to struct pointer
    if (fromType->kind == TypeInfoKind::Interface && toTypePointer->isPointerTo(TypeInfoKind::Struct))
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

    // Struct/Interface reference to pointer
    if (fromType->kind == TypeInfoKind::Reference)
    {
        auto typeRef = TypeManager::concreteReference(fromType);
        if (typeRef->kind == TypeInfoKind::Struct || typeRef->kind == TypeInfoKind::Interface)
        {
            if ((castFlags & CASTFLAG_EXPLICIT) || (toTypePointer->flags & TYPEINFO_SELF) || toTypePointer->isConst())
            {
                // to *void or *structure
                if (toTypePointer->pointedType->isNative(NativeTypeKind::Void) ||
                    toTypePointer->pointedType->isSame(typeRef, ISSAME_CAST))
                {
                    if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK) && !(toTypePointer->flags & TYPEINFO_SELF))
                    {
                        fromNode->castedTypeInfo = fromNode->typeInfo;
                        fromNode->typeInfo       = toType;
                    }

                    return true;
                }
            }
        }
    }

    // Struct pointer to interface pointer
    if (fromType->isPointerTo(TypeInfoKind::Struct) && toType->isPointerTo(TypeInfoKind::Interface))
        return castError(context, toType, fromType, fromNode, castFlags);

    // String to const *u8
    if (fromType->isNative(NativeTypeKind::String))
    {
        if (toType == g_TypeMgr->typeInfoNull)
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
    if (fromType->isNative(NativeTypeKind::UInt))
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

bool TypeManager::castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    TypeInfoArray* toTypeArray = CastTypeInfo<TypeInfoArray>(toType, TypeInfoKind::Array);
    if (fromType->kind == TypeInfoKind::TypeListArray)
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeListArray);
        auto          fromSize     = fromTypeList->subTypes.size();
        if (toTypeArray->count != fromSize)
        {
            if (!(castFlags & CASTFLAG_NO_ERROR))
            {
                if (toTypeArray->count > fromTypeList->subTypes.size())
                    context->report(fromNode, Fmt(Err(Err0203), toTypeArray->count, fromTypeList->subTypes.size()));
                else
                    context->report(fromNode, Fmt(Err(Err0204), toTypeArray->count, fromTypeList->subTypes.size()));
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

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        if (fromType->kind == TypeInfoKind::Pointer)
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

bool TypeManager::castToLambda(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    TypeInfoFuncAttr* toTypeLambda = CastTypeInfo<TypeInfoFuncAttr>(toType, TypeInfoKind::Lambda);
    if (castFlags & CASTFLAG_EXPLICIT)
    {
        if (fromType == g_TypeMgr->typeInfoNull || fromType->isPointerConstVoid())
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

bool TypeManager::castToClosure(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    TypeInfoFuncAttr* toTypeLambda = CastTypeInfo<TypeInfoFuncAttr>(toType, TypeInfoKind::Lambda);
    if (castFlags & CASTFLAG_EXPLICIT)
    {
        if (fromType == g_TypeMgr->typeInfoNull)
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

bool TypeManager::castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    TypeInfoSlice* toTypeSlice = CastTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);

    if (fromType->kind == TypeInfoKind::TypeListArray)
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeListArray);
        SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeSlice->pointedType, fromNode, castFlags));
        return true;
    }
    else if (fromType->kind == TypeInfoKind::Array)
    {
        TypeInfoArray* fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        if ((!(castFlags & CASTFLAG_NO_IMPLICIT) && toTypeSlice->pointedType->isSame(fromTypeArray->pointedType, ISSAME_CAST)) ||
            (castFlags & CASTFLAG_EXPLICIT))
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toTypeSlice;
            }

            return true;
        }
    }
    else if (fromType->isNative(NativeTypeKind::String))
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
    else if (fromType == g_TypeMgr->typeInfoNull)
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
    else if (fromType->kind == TypeInfoKind::Slice)
    {
        auto fromTypeSlice = CastTypeInfo<TypeInfoSlice>(fromType, TypeInfoKind::Slice);
        if (fromTypeSlice->pointedType->kind == TypeInfoKind::Native)
        {
            int s = toTypeSlice->pointedType->sizeOf;
            int d = fromTypeSlice->pointedType->sizeOf;
            if ((d / s) * s == d)
            {
                if (castFlags & CASTFLAG_EXPLICIT)
                    return true;
            }
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

void TypeManager::promote3264(AstNode* left, AstNode* right)
{
    promoteOne(left, right, true);
    promoteOne(right, left, true);
}

void TypeManager::promote816(AstNode* left, AstNode* right)
{
    promoteOne(left, right, false);
    promoteOne(right, left, false);
}

TypeInfo* TypeManager::promoteUntyped(TypeInfo* typeInfo)
{
    if (typeInfo->flags & TYPEINFO_UNTYPED_INTEGER)
        return g_TypeMgr->typeInfoS32;
    if (typeInfo->flags & TYPEINFO_UNTYPED_FLOAT)
        return g_TypeMgr->typeInfoF32;
    if (typeInfo->flags & TYPEINFO_UNTYPED_BINHEXA)
        return g_TypeMgr->typeInfoU32;
    return typeInfo;
}

void TypeManager::promoteUntypedInteger(AstNode* left, AstNode* right)
{
    TypeInfo* leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    TypeInfo* rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    SWAG_ASSERT(leftTypeInfo->flags & TYPEINFO_UNTYPED_INTEGER);
    auto leftNative = CastTypeInfo<TypeInfoNative>(leftTypeInfo, TypeInfoKind::Native);
    if (rightTypeInfo->isNativeInteger())
    {
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
            else if (leftNative->valueInteger <= UINT32_MAX)
                left->typeInfo = g_TypeMgr->typeInfoU32;
            else
                left->typeInfo = g_TypeMgr->typeInfoU64;
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
}

bool TypeManager::promoteOne(SemanticContext* context, AstNode* right)
{
    TypeInfo* rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    if (rightTypeInfo->kind != TypeInfoKind::Native)
        return true;
    switch (rightTypeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
        SWAG_CHECK(makeCompatibles(context, g_TypeMgr->typeInfoS32, nullptr, right, CASTFLAG_TRY_COERCE));
        break;
    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
        SWAG_CHECK(makeCompatibles(context, g_TypeMgr->typeInfoU32, nullptr, right, CASTFLAG_TRY_COERCE));
        break;
    }

    return true;
}

void TypeManager::promoteOne(AstNode* left, AstNode* right, bool is3264)
{
    TypeInfo* leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    TypeInfo* rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    if ((leftTypeInfo->kind != TypeInfoKind::Native) || (rightTypeInfo->kind != TypeInfoKind::Native))
        return;

    if ((leftTypeInfo->flags & TYPEINFO_UNTYPED_INTEGER) && !(rightTypeInfo->flags & TYPEINFO_UNTYPED_INTEGER))
    {
        promoteUntypedInteger(left, right);
        leftTypeInfo = left->typeInfo;
    }

    if (!(leftTypeInfo->flags & TYPEINFO_UNTYPED_INTEGER) && (rightTypeInfo->flags & TYPEINFO_UNTYPED_INTEGER))
    {
        promoteUntypedInteger(right, left);
        rightTypeInfo = right->typeInfo;
    }

    // This types do not have a promotion
    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::Bool:
    case NativeTypeKind::Rune:
    case NativeTypeKind::String:
        return;
    }

    TypeInfo* newLeftTypeInfo = nullptr;
    if (is3264)
        newLeftTypeInfo = (TypeInfo*) g_TypeMgr->promoteMatrix3264[(int) leftTypeInfo->nativeType][(int) rightTypeInfo->nativeType];
    else
        newLeftTypeInfo = (TypeInfo*) g_TypeMgr->promoteMatrix816[(int) leftTypeInfo->nativeType][(int) rightTypeInfo->nativeType];
    if (newLeftTypeInfo == nullptr)
        newLeftTypeInfo = leftTypeInfo;

    if (newLeftTypeInfo == leftTypeInfo)
        return;

    if (!(left->flags & AST_VALUE_COMPUTED) || !is3264)
    {
        left->typeInfo       = newLeftTypeInfo;
        left->castedTypeInfo = leftTypeInfo;
        return;
    }

    left->typeInfo = newLeftTypeInfo;
    auto newLeft   = newLeftTypeInfo->nativeType;
    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        if (newLeft == NativeTypeKind::U32)
            left->computedValue->reg.u64 = left->computedValue->reg.u8;
        else if (newLeft == NativeTypeKind::U64)
            left->computedValue->reg.u64 = left->computedValue->reg.u8;
        else if (newLeft == NativeTypeKind::UInt)
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
        else if (newLeft == NativeTypeKind::UInt)
            left->computedValue->reg.u64 = left->computedValue->reg.u16;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue->reg.f32 = (float) left->computedValue->reg.u16;
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.u16;
        break;
    case NativeTypeKind::U32:
        if (newLeft == NativeTypeKind::U64)
            left->computedValue->reg.u64 = left->computedValue->reg.u32;
        else if (newLeft == NativeTypeKind::UInt)
            left->computedValue->reg.u64 = left->computedValue->reg.u32;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue->reg.f32 = (float) left->computedValue->reg.u32;
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.u32;
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.u64;
        break;
    case NativeTypeKind::S8:
        if (newLeft == NativeTypeKind::S32)
            left->computedValue->reg.s64 = left->computedValue->reg.s8;
        else if (newLeft == NativeTypeKind::S64)
            left->computedValue->reg.s64 = left->computedValue->reg.s8;
        else if (newLeft == NativeTypeKind::Int)
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
        else if (newLeft == NativeTypeKind::Int)
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
    case NativeTypeKind::Int:
        if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = (double) left->computedValue->reg.s64;
        break;
    case NativeTypeKind::F32:
        if (newLeft == NativeTypeKind::F64)
            left->computedValue->reg.f64 = left->computedValue->reg.f32;
        break;
    }
}

bool TypeManager::convertLiteralTupleToStructVar(SemanticContext* context, TypeInfo* toType, AstNode* fromNode, bool fromType)
{
    if (fromNode->doneFlags & AST_DONE_STRUCT_CONVERT)
        return true;
    fromNode->doneFlags |= AST_DONE_STRUCT_CONVERT;

    auto sourceFile = context->sourceFile;
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(toType, TypeInfoKind::Struct);

    if (fromNode->kind == AstNodeKind::FuncCallParam)
    {
        fromNode->setPassThrough();
        fromNode = fromNode->childs.front();
        if (fromNode->kind != AstNodeKind::ExpressionList)
            return true;
    }

    // Parent of the identifier that will reference the new variable
    auto parentForRef = fromNode;
    if (fromNode->parent->kind == AstNodeKind::FuncCallParam)
        parentForRef = fromNode->parent;

    // Declare a variable
    auto varNode = Ast::newVarDecl(sourceFile, Fmt("__6tmp_%d", g_UniqueID.fetch_add(1)), fromNode->parent);

    // The variable will be inserted after its reference (below), so we need to inverse the order of evaluation.
    // Seems a little bit like a hack. Not sure this will always work.
    // :ReverseLiteralStruct
    fromNode->parent->flags |= AST_REVERSE_SEMANTIC;

    varNode->inheritTokenLocation(fromNode);
    varNode->flags |= AST_GENERATED;

    auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
    typeNode->inheritTokenLocation(fromNode);
    typeNode->flags |= AST_HAS_STRUCT_PARAMETERS;
    varNode->type = typeNode;

    SWAG_ASSERT(typeStruct->declNode);
    varNode->addAlternativeScope(typeStruct->declNode->ownerScope);
    typeNode->identifier = Ast::newIdentifierRef(sourceFile, typeStruct->declNode->token.text, typeNode);

    typeNode->identifier->flags |= AST_GENERATED;
    typeNode->identifier->inheritTokenLocation(fromNode);

    auto back = typeNode->identifier->childs.back();
    back->flags &= ~AST_NO_BYTECODE;
    back->flags |= AST_IN_TYPE_VAR_DECLARATION;

    // If this is in a return expression, then force the identifier type to be retval
    if (fromNode->inSimpleReturn())
        typeNode->typeFlags |= TYPEFLAG_RETVAL;

    // And make a reference to that variable
    auto identifierRef = Ast::newIdentifierRef(sourceFile, varNode->token.text, parentForRef);
    identifierRef->flags |= AST_R_VALUE | AST_TRANSIENT | AST_GENERATED;

    // Make parameters
    auto identifier = CastAst<AstIdentifier>(typeNode->identifier->childs.back(), AstNodeKind::Identifier);
    identifier->inheritTokenLocation(fromNode);
    identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);
    identifier->callParameters->flags |= AST_CALL_FOR_STRUCT;

    int countParams = (int) fromNode->childs.size();
    if (parentForRef == fromNode)
        countParams--;
    for (int i = 0; i < countParams; i++)
    {
        auto         oneChild = fromNode->childs[i];
        auto         oneParam = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
        CloneContext cloneContext;
        cloneContext.parent = oneParam;
        oneParam->inheritTokenLocation(oneChild);
        oneChild->clone(cloneContext);
        oneChild->flags |= AST_NO_BYTECODE | AST_NO_SEMANTIC;
        if (oneChild->kind == AstNodeKind::Identifier)
            oneChild->specFlags |= AST_SPEC_IDENTIFIER_NO_INLINE;
        if (oneChild->flags & AST_IS_NAMED)
            oneParam->namedParam = oneChild->token.text;

        // If this is for a return, remember it, in order to make a move or a copy
        if ((typeStruct->flags & TYPEINFO_STRUCT_IS_TUPLE) && fromNode->parent->kind == AstNodeKind::Return)
            oneParam->autoTupleReturn = CastAst<AstReturn>(fromNode->parent, AstNodeKind::Return);
    }

    // For a tuple initialization, every parameters must be covered
    if (!fromType && (typeStruct->flags & TYPEINFO_STRUCT_IS_TUPLE))
    {
        int maxCount = (int) typeStruct->fields.size();
        if (countParams > maxCount)
            return context->report(fromNode->childs[maxCount], Fmt(Err(Err0195), maxCount, countParams));
        if (countParams < maxCount)
            return context->report(fromNode->childs.back(), Fmt(Err(Err0205), maxCount, countParams));
    }

    fromNode->typeInfo = toType;
    fromNode->setPassThrough();

    // Add the 2 nodes to the semantic
    if (fromType)
    {
        fromNode->parent->typeInfo = toType;
        context->job->nodes.push_back(identifierRef);
        context->job->nodes.push_back(varNode);
    }
    else
    {
        auto b = context->job->nodes.back();
        context->job->nodes.pop_back();
        context->job->nodes.push_back(identifierRef);
        context->job->nodes.push_back(varNode);
        context->job->nodes.push_back(b);
    }

    return true;
}

bool TypeManager::convertLiteralTupleToStructType(SemanticContext* context, TypeInfoStruct* toType, AstNode* fromNode)
{
    auto sourceFile = context->sourceFile;
    auto typeList   = CastTypeInfo<TypeInfoList>(fromNode->typeInfo, TypeInfoKind::TypeListTuple);

    // :SubDeclParent
    auto newParent = fromNode->parent;
    while (newParent != sourceFile->astRoot && !(newParent->flags & AST_GLOBAL_NODE) && (newParent->kind != AstNodeKind::Namespace))
    {
        newParent = newParent->parent;
        SWAG_ASSERT(newParent);
    }

    auto structNode = Ast::newStructDecl(sourceFile, fromNode, nullptr);
    structNode->flags |= AST_PRIVATE | AST_GENERATED;
    structNode->doneFlags |= AST_DONE_FILE_JOB_PASS;
    Ast::removeFromParent(structNode);
    Ast::addChildBack(newParent, structNode);
    structNode->originalParent = newParent;
    structNode->allocateExtension();
    structNode->extension->semanticBeforeFct = SemanticJob::preResolveGeneratedStruct;

    auto contentNode    = Ast::newNode(sourceFile, AstNodeKind::TupleContent, structNode, nullptr);
    structNode->content = contentNode;
    contentNode->allocateExtension();
    contentNode->extension->semanticBeforeFct = SemanticJob::preResolveStructContent;

    Utf8 name = sourceFile->scopeFile->name + "_tuple_";
    name += Fmt("%d", g_UniqueID.fetch_add(1));
    structNode->token.text = move(name);

    // Add struct type and scope
    Scope* rootScope;
    if (sourceFile->fromTests)
        rootScope = sourceFile->scopeFile;
    else
        rootScope = newParent->ownerScope;
    structNode->allocateExtension();
    structNode->addAlternativeScope(fromNode->parent->ownerScope);
    structNode->extension->alternativeNode = newParent;

    auto newScope     = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, rootScope, true);
    structNode->scope = newScope;

    // Create type
    auto typeInfo        = allocType<TypeInfoStruct>();
    structNode->typeInfo = typeInfo;
    typeInfo->flags |= TYPEINFO_STRUCT_IS_TUPLE | TYPEINFO_GENERATED_TUPLE;
    typeInfo->declNode   = structNode;
    typeInfo->name       = structNode->token.text;
    typeInfo->structName = structNode->token.text;
    typeInfo->scope      = newScope;

    int countParams = (int) typeList->subTypes.size();
    int maxCount    = (int) toType->fields.size();
    if (countParams > maxCount)
        return context->report(fromNode->childs.front()->childs[maxCount], Fmt(Err(Err0195), maxCount, countParams));
    if (countParams < maxCount)
        return context->report(fromNode->childs.front()->childs.back(), Fmt(Err(Err0205), maxCount, countParams));

    // Each field of the toType struct must have a type given by the tuple.
    // But as that tuple can have named parameters, we need to find the correct type depending
    // on the name (if specified).
    {
        int i = 0;
        for (auto p : toType->fields)
        {
            auto p1        = typeList->subTypes[i];
            auto typeField = p1->typeInfo;
            Utf8 nameVar   = p->namedParam;
            for (int j = 0; j < typeList->subTypes.size(); j++)
            {
                if (nameVar == typeList->subTypes[j]->namedParam)
                {
                    p1        = typeList->subTypes[j];
                    typeField = p1->typeInfo;
                    break;
                }
            }

            if (nameVar.empty())
                nameVar = Fmt("item%d", i);
            i++;

            if (typeField->kind == TypeInfoKind::TypeListArray)
                typeField = TypeManager::convertTypeListToArray(context, (TypeInfoList*) typeField, false);
            if (typeField->kind == TypeInfoKind::TypeListTuple)
                return context->report(fromNode, Err(Err0119));

            // This is used for generic automatic deduction. We can use typeInfo->genericParameters, or we would
            // have to construct a struct AST with generic parameters too, and this is not possible as the struct
            // is not generic in all cases (generic types used in the struct can come from the function for example).
            if (p->typeInfo->flags & TYPEINFO_GENERIC)
                typeInfo->deducedGenericParameters.push_back(typeField);

            auto varNode  = Ast::newVarDecl(sourceFile, nameVar, contentNode);
            auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
            varNode->flags |= AST_GENERATED | AST_STRUCT_MEMBER;
            varNode->type                  = typeNode;
            varNode->ownerScope            = newScope;
            structNode->resolvedSymbolName = newScope->symTable.registerSymbolNameNoLock(context, structNode, SymbolKind::Variable);

            typeNode->typeInfo = typeField;
            typeNode->flags |= AST_NO_SEMANTIC;
        }
    }

    SWAG_CHECK(convertLiteralTupleToStructVar(context, typeInfo, fromNode, true));

    context->job->nodes.push_back(structNode);
    context->result = ContextResult::NewChilds1;

    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags)
{
    SWAG_CHECK(makeCompatibles(context, leftNode->typeInfo, leftNode, rightNode, castFlags));
    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags)
{
    // convert {...} expression list to a structure : this will create a variable, with parameters
    SWAG_ASSERT(fromNode->typeInfo);
    auto fromType = concreteType(fromNode->typeInfo, CONCRETE_ALIAS);
    if (fromType->kind == TypeInfoKind::TypeListTuple)
    {
        bool convert = false;
        if (toType->kind == TypeInfoKind::Struct)
            convert = true;
        if (toType->kind == TypeInfoKind::Reference && toType->isConst())
        {
            auto ptrRef = CastTypeInfo<TypeInfoReference>(toType, TypeInfoKind::Reference);
            if (ptrRef->pointedType->kind == TypeInfoKind::Struct)
            {
                toType  = ptrRef->pointedType;
                convert = true;
            }
        }
        if (toType->isPointerRef() && toType->isConst())
        {
            auto ptrRef = CastTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);
            if (ptrRef->pointedType->kind == TypeInfoKind::Struct)
            {
                toType  = ptrRef->pointedType;
                convert = true;
            }
        }

        if (fromNode->parent->kind == AstNodeKind::FuncDeclParam)
            convert = false;

        if (convert)
        {
            SWAG_CHECK(convertLiteralTupleToStructVar(context, toType, fromNode));
            return true;
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
        if (fromNode->typeInfo->kind == TypeInfoKind::TypeListTuple || fromNode->typeInfo->kind == TypeInfoKind::TypeListArray)
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
                if (exprList && !(exprList->doneFlags & AST_DONE_EXPRLIST_CST))
                {
                    exprList->doneFlags |= AST_DONE_EXPRLIST_CST;

                    // Test sizeof because @{} is legit to initialize a struct (for default values in function arguments)
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

bool TypeManager::makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags)
{
    // convert {...} expression list to a structure : this will create a variable, with parameters
    auto realFromType = concreteType(fromType, CONCRETE_ALIAS);
    auto realToType   = concreteType(toType, CONCRETE_ALIAS);
    SWAG_ASSERT(realFromType && realToType);
    if (realFromType->kind == TypeInfoKind::TypeListTuple)
    {
        if (realToType->kind == TypeInfoKind::Struct)
            return true;

        if (realToType->kind == TypeInfoKind::Reference && realToType->isConst())
        {
            auto ptrRef = CastTypeInfo<TypeInfoReference>(realToType, TypeInfoKind::Reference);
            if (ptrRef->pointedType->kind == TypeInfoKind::Struct)
                return true;
        }

        if (realToType->isPointerRef() && realToType->isConst())
        {
            auto ptrRef = CastTypeInfo<TypeInfoPointer>(realToType, TypeInfoKind::Pointer);
            if (ptrRef->pointedType->kind == TypeInfoKind::Struct)
                return true;
        }
    }

    SWAG_ASSERT(toType && fromType);

    if (fromType->flags & TYPEINFO_AUTO_CAST)
        castFlags |= CASTFLAG_EXPLICIT;
    if (toType->flags & TYPEINFO_GENERIC)
        castFlags |= CASTFLAG_NO_IMPLICIT;
    if (toType->flags & TYPEINFO_FROM_GENERIC)
        castFlags |= CASTFLAG_NO_IMPLICIT;

    if (toType->kind == TypeInfoKind::FuncAttr)
        toType = TypeManager::concreteType(toType, CONCRETE_FUNC);
    if (fromType->kind == TypeInfoKind::FuncAttr)
        fromType = TypeManager::concreteType(fromType, CONCRETE_FUNC);
    if (toType->kind != TypeInfoKind::Lambda && fromType->kind == TypeInfoKind::Lambda)
        fromType = TypeManager::concreteType(fromType, CONCRETE_FUNC);

    // Cast from a reference to the type of the reference
    if (!(castFlags & CASTFLAG_STRICT) || (fromType->kind == TypeInfoKind::Reference && toType->kind == TypeInfoKind::Reference))
    {
        if (fromType->kind == TypeInfoKind::Reference && toType->kind != TypeInfoKind::Pointer)
            fromType = concreteReference(fromType);
        if (toType->kind == TypeInfoKind::Reference && fromType->kind != TypeInfoKind::Struct && !fromType->isPointerTo(TypeInfoKind::Struct))
            toType = concreteReference(toType);
    }

    // Transform typealias to related type
    if (toType->kind == TypeInfoKind::Alias)
        toType = TypeManager::concreteType(toType, CONCRETE_ALIAS | (castFlags & CASTFLAG_EXPLICIT ? CONCRETE_FORCEALIAS : 0));
    if (fromType->kind == TypeInfoKind::Alias)
        fromType = TypeManager::concreteType(fromType, CONCRETE_ALIAS | (castFlags & CASTFLAG_EXPLICIT ? CONCRETE_FORCEALIAS : 0));

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

    // Transform typealias to related type again, because an enum can have a type alias as an underlying type
    if (toType->kind == TypeInfoKind::Alias)
        toType = TypeManager::concreteType(toType, CONCRETE_ALIAS | (castFlags & CASTFLAG_EXPLICIT ? CONCRETE_FORCEALIAS : 0));
    if (fromType->kind == TypeInfoKind::Alias)
        fromType = TypeManager::concreteType(fromType, CONCRETE_ALIAS | (castFlags & CASTFLAG_EXPLICIT ? CONCRETE_FORCEALIAS : 0));

    if (fromType == toType)
        return true;

    // Everything can be casted to or from type 'any'
    if (toType->isNative(NativeTypeKind::Any) || fromType->isNative(NativeTypeKind::Any))
        return castToFromAny(context, toType, fromType, toNode, fromNode, castFlags);

    // Variadic
    if (fromType->kind == TypeInfoKind::TypedVariadic)
        fromType = ((TypeInfoVariadic*) fromType)->rawType;
    if (toType->kind == TypeInfoKind::TypedVariadic)
        toType = ((TypeInfoVariadic*) toType)->rawType;

    bool result = false;

    // From a reference
    if (fromType->kind == TypeInfoKind::Reference)
    {
        auto fromTypeRef = TypeManager::concreteReference(fromType);
        if (fromTypeRef == toType)
            result = true;
        else if (fromTypeRef->isSame(toType, ISSAME_CAST))
            result = true;
    }

    if (fromType->isPointerRef())
    {
        auto fromTypeRef = TypeManager::concretePtrRef(fromType);
        if (fromTypeRef == toType)
        {
            castFlags |= CASTFLAG_FORCE_UNCONST;
            result = true;
        }
        else if (fromTypeRef->isSame(toType, ISSAME_CAST))
        {
            castFlags |= CASTFLAG_FORCE_UNCONST;
            result = true;
        }
    }

    if (!result)
    {
        auto isSameFlags = ISSAME_CAST;
        if (castFlags & CASTFLAG_FOR_AFFECT)
            isSameFlags |= ISSAME_FOR_AFFECT;
        if (castFlags & CASTFLAG_FOR_GENERIC)
            isSameFlags |= ISSAME_FOR_GENERIC;
        if (fromType->isSame(toType, isSameFlags))
            result = true;
    }

    // Always match against a generic
    if (!result)
    {
        if (toType->kind == TypeInfoKind::Generic)
            result = true;
    }

    if (!result)
    {
        switch (toType->kind)
        {
            // Cast to reference
        case TypeInfoKind::Reference:
            SWAG_CHECK(castToReference(context, toType, fromType, fromNode, castFlags));
            break;

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
        case TypeInfoKind::Lambda:
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
    if (toType->kind != TypeInfoKind::Generic && toType->kind != TypeInfoKind::Lambda && !(castFlags & CASTFLAG_FORCE_UNCONST))
    {
        if (!(castFlags & CASTFLAG_PARAMS) || toType->kind != TypeInfoKind::Struct)
        {
            bool diff = false;
            if (castFlags & CASTFLAG_FOR_GENERIC)
                diff = toType->isConst() != fromType->isConst();
            else
                diff = !toType->isConst() && fromType->isConst();
            if (diff)
            {
                if (!toType->isNative(NativeTypeKind::String) &&
                    (toType != g_TypeMgr->typeInfoNull) &&
                    (!toType->isNative(NativeTypeKind::Bool) || !(castFlags & CASTFLAG_AUTO_BOOL)) &&
                    (!toType->isNative(NativeTypeKind::UInt) || fromType->kind != TypeInfoKind::Pointer))
                {
                    if (!(castFlags & CASTFLAG_UNCONST))
                        return castError(context, toType, fromType, fromNode, castFlags | CASTFLAG_CONST_ERR);

                    // We can affect a const to an unconst if type is by copy, and we are in an affectation
                    if (!(fromType->flags & TYPEINFO_RETURN_BY_COPY) && !(toType->flags & TYPEINFO_RETURN_BY_COPY))
                        return castError(context, toType, fromType, fromNode, castFlags | CASTFLAG_CONST_ERR);
                }
            }
        }
    }

    return true;
}

static const ConcreteTypeInfo* concreteAlias(const ConcreteTypeInfo* type1)
{
    if (type1->kind != TypeInfoKind::Alias || (type1->flags & (uint16_t) TypeInfoFlags::Strict))
        return type1;
    auto typeAlias = (const ConcreteTypeInfoAlias*) type1;
    return concreteAlias((ConcreteTypeInfo*) typeAlias->rawType);
}

bool TypeManager::compareConcreteType(const ConcreteTypeInfo* type1, const ConcreteTypeInfo* type2)
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

TypeInfo* TypeManager::asPointerArithmetic(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Pointer)
        return typeInfo;
    typeInfo = typeInfo->clone();
    typeInfo->flags |= TYPEINFO_POINTER_ARITHMETIC;
    return typeInfo;
}

void TypeManager::convertStructParamToRef(AstNode* node, TypeInfo* typeInfo)
{
    SWAG_ASSERT(node->kind == AstNodeKind::FuncDeclParam);

    // A struct/interface is forced to be a const reference
    if (!(node->typeInfo->flags & TYPEINFO_GENERIC))
    {
        if (typeInfo->kind == TypeInfoKind::Struct)
        {
            // If this has been transformed to an alias cause of const, take the original
            // type to make the reference
            if (typeInfo->flags & TYPEINFO_FAKE_ALIAS)
                typeInfo = ((TypeInfoAlias*) typeInfo)->rawType;

            auto typeRef         = allocType<TypeInfoReference>();
            typeRef->flags       = typeInfo->flags | TYPEINFO_CONST;
            typeRef->pointedType = typeInfo;
            typeRef->computeName();
            node->typeInfo = typeRef;
        }
    }
}