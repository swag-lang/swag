#include "pch.h"

#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"

TypeInfo* TypeInfoNative::clone()
{
    const auto newType = makeType<TypeInfoNative>();
    newType->copyFrom(this);
    newType->valueInteger = valueInteger;
    return newType;
}

bool TypeInfoNative::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;

    if (castFlags.has(CAST_FLAG_CAST))
    {
        if (to->isKindGeneric())
            return true;
        if (nativeType == NativeTypeKind::Undefined)
            return true;
    }

    if (to->isAlias())
        to = TypeManager::concreteType(to, CONCRETE_ALIAS);

    if (to->kind != TypeInfoKind::Native)
        return false;

    const auto other = castTypeInfo<TypeInfoNative>(to, to->kind);
    if (nativeType != other->nativeType)
        return false;

    return true;
}

void TypeInfoNative::computeWhateverName(Utf8& resName, ComputeNameKind nameKind)
{
    if (isNullable())
        resName += "nl ";
    if (isConst())
        resName += "const ";
    switch (nativeType)
    {
        case NativeTypeKind::S8:
            resName += "s8";
            break;
        case NativeTypeKind::S16:
            resName += "s16";
            break;
        case NativeTypeKind::S32:
            resName += "s32";
            break;
        case NativeTypeKind::S64:
            resName += "s64";
            break;
        case NativeTypeKind::U8:
            resName += "u8";
            break;
        case NativeTypeKind::U16:
            resName += "u16";
            break;
        case NativeTypeKind::U32:
            resName += "u32";
            break;
        case NativeTypeKind::U64:
            resName += "u64";
            break;
        case NativeTypeKind::Bool:
            resName += "bool";
            break;
        case NativeTypeKind::String:
            resName += "string";
            break;
        case NativeTypeKind::CString:
            resName += "cstring";
            break;
        case NativeTypeKind::Any:
            resName += "any";
            break;
        case NativeTypeKind::F32:
            resName += "f32";
            break;
        case NativeTypeKind::F64:
            resName += "f64";
            break;
        case NativeTypeKind::Rune:
            resName += "rune";
            break;
        case NativeTypeKind::Void:
            resName += "void";
            break;
        case NativeTypeKind::Undefined:
            resName += "?";
            break;
    }
}

Utf8 TypeInfoNamespace::getDisplayName()
{
    Utf8 str;
    computeWhateverName(str, ComputeNameKind::DisplayName);
    return form("namespace %s", str.cstr());
}

TypeInfo* TypeInfoNamespace::clone()
{
    const auto newType = makeType<TypeInfoNamespace>();
    newType->scope     = scope;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoCode::clone()
{
    const auto newType = makeType<TypeInfoCode>();
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoCode::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, castFlags))
        return false;
    if (castFlags.has(CAST_FLAG_CAST))
        return true;
    return false;
}

TypeInfo* TypeInfoAlias::clone()
{
    const auto newType = makeType<TypeInfoAlias>();
    newType->rawType   = rawType;
    newType->copyFrom(this);
    return newType;
}

void TypeInfoAlias::computeWhateverName(Utf8& resName, ComputeNameKind nameKind)
{
    if (nameKind == ComputeNameKind::DisplayName && flags.has(TYPEINFO_CONST_ALIAS))
    {
        if (isConst())
            resName += "const ";
        resName += rawType->computeWhateverName(nameKind);
    }
    else
    {
        TypeInfo::computeWhateverName(resName, nameKind);
    }
}

Utf8 TypeInfoAlias::getDisplayName()
{
    Utf8 res;
    
    if (isNullable())
        res += "nl ";
    if (isConst())
        res += "const ";
    res += rawType->getDisplayName();
    return res;
}

bool TypeInfoAlias::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;

    if (!TypeInfo::isSame(to, castFlags) || name != to->name)
    {
        const auto me = TypeManager::concreteType(this, CONCRETE_ALIAS);
        if (me != this)
            return me->isSame(to, castFlags);
        return false;
    }

    if (to->isAlias())
    {
        const auto other = castTypeInfo<TypeInfoAlias>(to, to->kind);
        return rawType->isSame(other->rawType, castFlags);
    }

    SWAG_ASSERT(flags.has(TYPEINFO_CONST_ALIAS));
    return rawType->isSame(to, castFlags);
}

TypeInfo* TypeInfoPointer::clone()
{
    const auto newType   = makeType<TypeInfoPointer>();
    newType->pointedType = pointedType;
    newType->copyFrom(this);
    return newType;
}

Utf8 TypeInfoPointer::getDisplayName()
{
    if (hasFlag(TYPEINFO_POINTER_AUTO_REF))
        return pointedType->getDisplayName();

    Utf8 str;
    computeWhateverName(str, ComputeNameKind::DisplayName);
    return str;
}

void TypeInfoPointer::computeWhateverName(Utf8& resName, ComputeNameKind nameKind)
{
    if (hasFlag(TYPEINFO_C_STRING))
    {
        resName = "cstring";
        return;
    }

    if (!pointedType)
    {
        resName = "null";
        return;
    }

    if (!hasFlag(TYPEINFO_POINTER_AUTO_REF) || (nameKind != ComputeNameKind::DisplayName && nameKind != ComputeNameKind::ScopedNameExport))
    {
        if (isNullable())
            resName += "nl ";
        if (isConst())
            resName += "const ";
        if (isPointerRef() && hasFlag(TYPEINFO_POINTER_MOVE_REF))
            resName += "&&";
        else if (hasFlag(TYPEINFO_POINTER_ACCEPT_MOVE_REF))
            resName += "&&";
        else if (isPointerRef())
            resName += "&";
        else if (hasFlag(TYPEINFO_POINTER_ARITHMETIC))
            resName += "^";
        else
            resName += "*";
    }

    resName += pointedType->computeWhateverName(nameKind);
}

bool TypeInfoPointer::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;

    to = TypeManager::concreteType(to);
    if (castFlags.has(CAST_FLAG_CAST))
    {
        if (to->isKindGeneric() && !hasFlag(TYPEINFO_POINTER_MOVE_REF))
            return true;
        if (isPointerNull() && to->isLambdaClosure())
            return true;
    }

    if (!TypeInfo::isSame(to, castFlags))
        return false;

    if (castFlags.has(CAST_FLAG_CAST))
    {
        if (isPointerNull() && !to->isPointerRef())
            return true;
        if (to->isPointerNull() && !isPointerRef())
            return true;
    }

    const auto other = castTypeInfo<TypeInfoPointer>(to, to->kind);

    // Anonymous pointers
    if (castFlags.has(CAST_FLAG_CAST))
    {
        if (other->pointedType->isVoid() && !castFlags.has(CAST_FLAG_FOR_GENERIC))
            return true;
        if (to->hasFlag(TYPEINFO_POINTER_ARITHMETIC) && !hasFlag(TYPEINFO_POINTER_ARITHMETIC))
            return false;
        if (to->isPointerRef() || isPointerRef())
            return false;
    }

    return pointedType->isSame(other->pointedType, castFlags);
}

TypeInfo* TypeInfoArray::clone()
{
    const auto newType   = makeType<TypeInfoArray>();
    newType->pointedType = pointedType;
    newType->finalType   = finalType;
    newType->count       = count;
    newType->totalCount  = totalCount;
    newType->sizeNode    = sizeNode;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoArray::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;
    if (to->isKindGeneric())
        return true;
    if (!TypeInfo::isSame(to, castFlags))
        return false;

    const auto other = castTypeInfo<TypeInfoArray>(to, to->kind);
    if (isGeneric() == other->isGeneric())
    {
        if (count != other->count)
            return false;
    }

    if (!pointedType->isSame(other->pointedType, castFlags))
        return false;

    SWAG_ASSERT(!sizeOf || !other->sizeOf || sizeOf == other->sizeOf);
    return true;
}

void TypeInfoArray::computeWhateverName(Utf8& resName, ComputeNameKind nameKind)
{
    if (flags.has(TYPEINFO_CONST))
        resName += "const ";

    if (count == UINT32_MAX)
    {
        resName += "[] ";
    }
    else
    {
        if (count == 0)
            resName += form("[?", count);
        else
            resName += form("[%d", count);
        auto pType = pointedType;
        while (pType->isArray() && pType != finalType)
        {
            const auto subType = castTypeInfo<TypeInfoArray>(pType, TypeInfoKind::Array);
            if (subType->count == 0)
                resName += form(",?", subType->count);
            else
                resName += form(",%d", subType->count);
            pType = subType->pointedType;
        }

        resName += "]";
        if (!finalType->isArray())
            resName += " ";
    }

    resName += finalType->computeWhateverName(nameKind);
}

void TypeInfoSlice::computeWhateverName(Utf8& resName, ComputeNameKind nameKind)
{
    if (isNullable())
        resName += "nl ";
    if (isConst())
        resName += "const ";
    resName += "[..] ";
    resName += pointedType->computeWhateverName(nameKind);
}

TypeInfo* TypeInfoSlice::clone()
{
    const auto newType   = makeType<TypeInfoSlice>();
    newType->pointedType = pointedType;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoSlice::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, castFlags))
        return false;
    const auto castFrom = castTypeInfo<TypeInfoSlice>(to, to->kind);
    return pointedType->isSame(castFrom->pointedType, castFlags);
}

void TypeInfoList::computeWhateverName(Utf8& resName, ComputeNameKind nameKind)
{
    if (kind == TypeInfoKind::TypeListArray)
    {
        if (isConst())
            resName += "const ";
        resName += form("[%u] ", subTypes.size());
        if (!subTypes.empty())
        {
            subTypes[0]->typeInfo->computeWhateverName(nameKind);
            resName += subTypes[0]->typeInfo->name;
        }
    }
    else
    {
        resName = TypeInfoStruct::computeTupleDisplayName(subTypes, nameKind);
    }
}

TypeInfo* TypeInfoList::clone()
{
    const auto newType = makeType<TypeInfoList>(kind);

    const uint32_t size = subTypes.size();
    newType->subTypes.reserve(size);
    for (uint32_t i = 0; i < size; i++)
    {
        auto param = subTypes[i];
        param      = param->clone();
        newType->subTypes.push_back(param);
    }

    newType->scope = scope;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoList::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;

    // Can cast from type list tuple to struct
    // The real check will be done later
    if (castFlags.has(CAST_FLAG_CAST))
    {
        if (to->isStruct() && kind == TypeInfoKind::TypeListTuple)
            return true;
    }

    if (!TypeInfo::isSame(to, castFlags))
        return false;
    const auto other = castTypeInfo<TypeInfoList>(to, to->kind);
    if (subTypes.size() != other->subTypes.size())
        return false;

    for (uint32_t i = 0; i < subTypes.size(); i++)
    {
        if (!subTypes[i]->isSame(other->subTypes[i], castFlags))
            return false;
    }

    if (castFlags.has(CAST_FLAG_EXACT))
    {
        if (scope != other->scope)
            return false;
    }

    return true;
}

TypeInfo* TypeInfoVariadic::clone()
{
    const auto newType = makeType<TypeInfoVariadic>();
    newType->copyFrom(this);
    newType->rawType = rawType;
    return newType;
}

void TypeInfoVariadic::computeWhateverName(Utf8& resName, ComputeNameKind nameKind)
{
    if (kind == TypeInfoKind::CVariadic)
    {
        resName = "cvarargs";
        return;
    }

    if (rawType)
        resName += rawType->computeWhateverName(nameKind);
    resName += "...";
}

bool TypeInfoVariadic::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (!TypeInfo::isSame(to, castFlags))
        return false;
    const auto other = castTypeInfo<TypeInfoVariadic>(to, to->kind);
    if (rawType && !other->rawType)
        return false;
    if (!rawType && other->rawType)
        return false;
    if (rawType == other->rawType)
        return true;
    return rawType->isSame(other->rawType, castFlags);
}

TypeInfo* TypeInfoGeneric::clone()
{
    const auto newType = makeType<TypeInfoGeneric>();
    newType->copyFrom(this);
    newType->rawType = rawType;
    return newType;
}

bool TypeInfoGeneric::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;
    if (!castFlags.has(CAST_FLAG_EXACT) && !to->isKindGeneric())
        return true;
    if (to->kind == kind)
        return name == to->name;
    if (castFlags.has(CAST_FLAG_EXACT))
        return name == to->name;
    return true;
}

Utf8 TypeInfoEnum::getDisplayName()
{
    Utf8 str;
    computeWhateverName(str, ComputeNameKind::DisplayName);
    return form("enum %s", str.cstr());
}

bool TypeInfoEnum::contains(const Utf8& valueName)
{
    for (const auto p : values)
    {
        if (p->name == valueName)
            return true;
    }

    return false;
}

void TypeInfoEnum::collectEnums(VectorNative<TypeInfoEnum*>& collect)
{
    collect.clear();
    collect.push_back(this);
    if (!flags.has(TYPEINFO_ENUM_HAS_USING))
        return;

    for (const auto value : values)
    {
        if (value->typeInfo->isEnum())
        {
            auto subType = castTypeInfo<TypeInfoEnum>(value->typeInfo, TypeInfoKind::Enum);
            collect.push_back_once(subType);
        }
    }
}

TypeInfo* TypeInfoEnum::clone()
{
    const auto newType  = makeType<TypeInfoEnum>();
    newType->scope      = scope;
    newType->rawType    = rawType;
    newType->attributes = attributes;

    for (const auto& value : values)
    {
        auto param = value->clone();
        newType->values.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

bool TypeInfoEnum::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;

    if (castFlags.has(CAST_FLAG_CAST))
    {
        if (to->isKindGeneric())
            return true;
    }

    if (!TypeInfo::isSame(to, castFlags))
        return false;

    if (name != to->name)
        return false;

    const auto other = castTypeInfo<TypeInfoEnum>(to, to->kind);
    if (!rawType->isSame(other->rawType, castFlags))
        return false;
    if (values.size() != other->values.size())
        return false;

    if (!castFlags.has(CAST_FLAG_CAST))
    {
        const auto childSize = values.size();
        if (childSize != other->values.size())
            return false;
        for (uint32_t i = 0; i < childSize; i++)
        {
            if (!values[i]->isSame(other->values[i], castFlags))
                return false;
        }
    }

    return true;
}

TypeInfo* TypeInfoFuncAttr::clone()
{
    ScopedLock lk(mutex);

    const auto newType            = makeType<TypeInfoFuncAttr>();
    newType->firstDefaultValueIdx = firstDefaultValueIdx;
    newType->returnType           = returnType;
    newType->attributes           = attributes;
    newType->attributeUsage       = attributeUsage;
    newType->callConv             = callConv;
    newType->replaceTypes         = replaceTypes;

    for (const auto& genericParameter : genericParameters)
    {
        auto param = genericParameter->clone();
        newType->genericParameters.push_back(param);
    }

    for (const auto& parameter : parameters)
    {
        auto param = parameter->clone();
        newType->parameters.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

namespace
{
    void computeNameGenericParameters(VectorNative<TypeInfoParam*>& genericParameters, Utf8& resName, ComputeNameKind nameKind)
    {
        if (genericParameters.empty())
            return;

        if (nameKind != ComputeNameKind::DisplayName || genericParameters.size() > 1)
            resName += "'(";
        else
            resName += "'";

        for (uint32_t i = 0; i < genericParameters.size(); i++)
        {
            if (i)
                resName += ", ";

            const auto genParam = genericParameters[i];
            if (genParam->flags.has(TYPEINFOPARAM_DEFINED_VALUE))
            {
                SWAG_ASSERT(genParam->typeInfo);
                SWAG_ASSERT(genParam->value);
                const auto str = Ast::literalToString(genParam->typeInfo, *genParam->value);
                if (genParam->typeInfo->isString())
                    resName += "\"";
                resName += str;
                if (genParam->typeInfo->isString())
                    resName += "\"";
            }
            else if (genParam->flags.has(TYPEINFOPARAM_GENERIC_CONSTANT))
                resName += genParam->name;
            else if (genParam->typeInfo)
                resName += genParam->typeInfo->computeWhateverName(nameKind);
            else
                resName += genParam->name;
        }

        if (nameKind != ComputeNameKind::DisplayName || genericParameters.size() > 1)
            resName += ")";
    }
}

void TypeInfoFuncAttr::computeWhateverName(Utf8& resName, ComputeNameKind nameKind)
{
    bool addedName = false;
    if (kind != TypeInfoKind::LambdaClosure)
    {
        if (nameKind == ComputeNameKind::ScopedName || nameKind == ComputeNameKind::ScopedNameExport)
        {
            computeScopedName(resName);
            SWAG_ASSERT(declNode);
            resName += declNode->token.text;
            addedName = true;
        }
    }

    if (!addedName)
    {
        if (flags.has(TYPEINFO_FUNC_IS_ATTR))
            resName += "attr";
        else
            resName += "func";
    }

    computeNameGenericParameters(genericParameters, resName, nameKind);

    // Parameters
    resName += "(";
    bool first = true;
    for (uint32_t i = 0; i < parameters.size(); i++)
    {
        // First parameter of a closure is generated, so do not count it in the name
        if (isClosure() && !i)
            continue;

        if (!first)
            resName += ", ";
        first = false;
        resName += parameters[i]->typeInfo->computeWhateverName(nameKind);
    }

    resName += ")";

    // Return type
    if (returnType && !returnType->isVoid())
    {
        resName += "->";
        resName += returnType->computeWhateverName(nameKind);
    }

    if (hasFlag(TYPEINFO_CAN_THROW))
        resName += " throw";
}

bool TypeInfoFuncAttr::isSame(const TypeInfoFuncAttr* other, CastFlags castFlags) const
{
    BadSignatureInfos bi;
    return isSame(other, castFlags, bi);
}

bool TypeInfoFuncAttr::isSame(const TypeInfoFuncAttr* other, CastFlags castFlags, BadSignatureInfos& bi) const
{
    // Cannot convert a closure to a lambda
    if (isClosure() && !other->isClosure())
        return false;

    // Can convert a lambda to a closure, but do not take care of closure first parameter
    // as it is generated
    auto countMyParams = parameters.size();
    if (!isClosure() && other->isClosure())
        countMyParams += 1;

    if (other->parameters.size() > countMyParams)
    {
        bi.matchResult = MatchResult::TooManyArguments;
        return false;
    }

    if (other->parameters.size() < countMyParams)
    {
        bi.matchResult = MatchResult::NotEnoughArguments;
        return false;
    }

    if (genericParameters.size() != other->genericParameters.size())
    {
        // We want func's32(s32) to match func(T) when func(T) is generic
        // This is necessary for lambdas. Perhaps that test is not enough !
        if (flags.has(TYPEINFO_GENERIC) == other->flags.has(TYPEINFO_GENERIC) &&
            !flags.has(TYPEINFO_UNDEFINED) &&
            !other->hasFlag(TYPEINFO_UNDEFINED))
            return false;
    }

    if (flags.has(TYPEINFO_CAN_THROW) != other->flags.has(TYPEINFO_CAN_THROW))
    {
        bi.matchResult = MatchResult::MismatchThrow;
        return false;
    }

    // Default values should be the same
    if (isLambdaClosure() && other->isLambdaClosure() && firstDefaultValueIdx != UINT32_MAX)
        return false;

    if (castFlags.has(CAST_FLAG_EXACT))
    {
        if ((!returnType || returnType->isVoid()) && other->returnType && !other->returnType->isVoid())
        {
            bi.matchResult = MatchResult::NoReturnType;
            return false;
        }

        if (returnType && !returnType->isVoid() && (!other->returnType || other->returnType->isVoid()))
        {
            bi.matchResult = MatchResult::MissingReturnType;
            return false;
        }

        if (returnType &&
            other->returnType &&
            !returnType->isUndefined() &&
            !returnType->isSame(other->returnType, castFlags))
        {
            bi.matchResult = MatchResult::MismatchReturnType;
            return false;
        }

        // If the two functions are generics, compare the types that have been used to instantiate the function.
        // If the types do not match, then the two functions are not the same.
        if (declNode && declNode->is(AstNodeKind::FuncDecl) && other->declNode && other->declNode->is(AstNodeKind::FuncDecl))
        {
            const auto myFunc        = castAst<AstFuncDecl>(declNode, AstNodeKind::FuncDecl);
            const auto typeMyFunc    = castTypeInfo<TypeInfoFuncAttr>(myFunc->typeInfo, TypeInfoKind::FuncAttr);
            const auto otherFunc     = castAst<AstFuncDecl>(other->declNode, AstNodeKind::FuncDecl);
            const auto typeOtherFunc = castTypeInfo<TypeInfoFuncAttr>(otherFunc->typeInfo, TypeInfoKind::FuncAttr);
            if (typeMyFunc->replaceTypes.size() != typeOtherFunc->replaceTypes.size())
                return false;
            for (const auto& r : typeMyFunc->replaceTypes)
            {
                auto it = typeOtherFunc->replaceTypes.find(r.first);
                if (it == typeOtherFunc->replaceTypes.end())
                    return false;
                if (r.second.typeInfoReplace != it->second.typeInfoReplace)
                    return false;
            }
        }
    }

    if (genericParameters.size() == other->genericParameters.size())
    {
        for (uint32_t i = 0; i < genericParameters.size(); i++)
        {
            if (!genericParameters[i]->typeInfo->isSame(other->genericParameters[i]->typeInfo, castFlags))
                return false;
            if (genericParameters[i]->value != other->genericParameters[i]->value)
                return false;
        }
    }

    // Compare capture argument only if not converting a lambda to a closure
    if (isClosure() && other->isClosure())
    {
        if (capture.size() != other->capture.size())
            return false;
        for (uint32_t i = 0; i < capture.size(); i++)
        {
            if (capture[i]->typeInfo->isUndefined())
                continue;
            if (other->capture[i]->typeInfo->isUndefined())
                continue;
            const auto type1 = TypeManager::concretePtrRef(capture[i]->typeInfo);
            const auto type2 = TypeManager::concretePtrRef(other->capture[i]->typeInfo);
            if (!type1->isSame(type2, castFlags))
                return false;
        }
    }

    int firstParam = 0;
    if (!isClosure() && other->isClosure())
        firstParam = 1;

    for (uint32_t i = 0; i < parameters.size(); i++)
    {
        auto type1 = parameters[i]->typeInfo;
        auto type2 = other->parameters[i + firstParam]->typeInfo;

        if (type1->isUndefined() || type2->isUndefined())
            continue;

        if (type1->hasFlag(TYPEINFO_POINTER_MOVE_REF) != type2->hasFlag(TYPEINFO_POINTER_MOVE_REF))
        {
            bi.matchResult      = MatchResult::BadSignature;
            bi.badSignatureNum1 = i;
            bi.badSignatureNum2 = i + firstParam;
            return false;
        }

        if (type1->isAutoConstPointerRef() != type2->isAutoConstPointerRef() &&
            type1->isPointerRef() &&
            type2->isPointerRef())
        {
            bi.matchResult      = MatchResult::BadSignature;
            bi.badSignatureNum1 = i;
            bi.badSignatureNum2 = i + firstParam;
            return false;
        }

        type1 = TypeManager::concretePtrRef(type1);
        type2 = TypeManager::concretePtrRef(type2);
        if (!type1->isSame(type2, castFlags))
        {
            bi.matchResult      = MatchResult::BadSignature;
            bi.badSignatureNum1 = i;
            bi.badSignatureNum2 = i + firstParam;
            return false;
        }
    }

    return true;
}

bool TypeInfoFuncAttr::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;

    if (castFlags.has(CAST_FLAG_CAST))
    {
        if (kind == TypeInfoKind::LambdaClosure && to->isPointerNull())
            return true;
    }

    if (!TypeInfo::isSame(to, castFlags))
        return false;

    const auto other = castTypeInfo<TypeInfoFuncAttr>(to, to->kind);
    SWAG_ASSERT(to->isFuncAttr() || to->isLambdaClosure());
    if (!isSame(other, castFlags))
        return false;

    if (castFlags.has(CAST_FLAG_EXACT) || to->isLambdaClosure())
    {
        if (returnType && !returnType->isVoid() && !other->returnType)
            return false;
        if (!returnType && other->returnType && !other->returnType->isVoid())
            return false;
        if (returnType && other->returnType && !returnType->isUndefined() && !returnType->isSame(other->returnType, castFlags))
            return false;
    }

    return true;
}

bool TypeInfoFuncAttr::isFctVariadic() const
{
    if (parameters.empty())
        return false;
    const auto typeParam = parameters.back()->typeInfo;
    if (typeParam->isVariadic() || typeParam->isTypedVariadic())
        return true;
    return false;
}

bool TypeInfoFuncAttr::isFctCVariadic() const
{
    if (parameters.empty())
        return false;
    const auto typeParam = parameters.back()->typeInfo;
    if (typeParam->isCVariadic())
        return true;
    return false;
}

TypeInfo* TypeInfoFuncAttr::concreteReturnType() const
{
    if (!returnType)
        return g_TypeMgr->typeInfoVoid;
    return TypeManager::concreteType(returnType);
}

// argIdx is the argument index of a function, starting after the return arguments
uint32_t TypeInfoFuncAttr::registerIdxToParamIdx(uint32_t argIdx)
{
    if (hasFlag(TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        if (argIdx < 2)
            return parameters.size() - 1;
        argIdx -= 2;
    }

    uint32_t argNo = 0;
    while (true)
    {
        if (argNo >= parameters.size())
        {
            SWAG_ASSERT(flags.has(TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC | TYPEINFO_C_VARIADIC));
            return parameters.size() - 1;
        }

        const auto typeParam = TypeManager::concreteType(parameters[argNo]->typeInfo);
        const auto n         = typeParam->numRegisters();
        if (argIdx < n)
            return argNo;
        argIdx -= n;
        argNo++;
    }
}

TypeInfo* TypeInfoFuncAttr::registerIdxToType(uint32_t argIdx)
{
    const auto argNo = registerIdxToParamIdx(argIdx);
    if (argNo >= parameters.size())
        return nullptr;
    return TypeManager::concreteType(parameters[argNo]->typeInfo);
}

uint32_t TypeInfoFuncAttr::numParamsRegisters()
{
    uint32_t total = 0;
    for (const auto param : parameters)
    {
        const auto typeParam = TypeManager::concreteType(param->typeInfo);
        total += typeParam->numRegisters();
    }

    return total;
}

uint32_t TypeInfoFuncAttr::numReturnRegisters() const
{
    return returnType ? returnType->numRegisters() : 0;
}

uint32_t TypeInfoFuncAttr::numTotalRegisters()
{
    return numReturnRegisters() + numParamsRegisters();
}

const CallConv& TypeInfoFuncAttr::getCallConv() const
{
    return g_CallConv[callConv];
}

namespace
{
    void flatten(VectorNative<TypeInfoParam*>& result, TypeInfoParam* param)
    {
        if (!param->flags.has(TYPEINFOPARAM_HAS_USING) || !param->typeInfo->isStruct())
        {
            result.push_back(param);
            return;
        }

        const auto typeStruct = castTypeInfo<TypeInfoStruct>(param->typeInfo, TypeInfoKind::Struct);
        for (const auto p : typeStruct->fields)
            flatten(result, p);
    }
}

TypeInfoParam* TypeInfoStruct::findChildByNameNoLock(const Utf8& childName) const
{
    for (const auto child : fields)
    {
        if (child->name == childName)
            return child;
    }

    return nullptr;
}

TypeInfoParam* TypeInfoStruct::hasInterface(const TypeInfoStruct* itf) const
{
    SharedLock lk(mutex);
    return hasInterfaceNoLock(itf);
}

TypeInfoParam* TypeInfoStruct::hasInterfaceNoLock(const TypeInfoStruct* itf) const
{
    for (const auto child : interfaces)
    {
        if (child->typeInfo->isSame(itf, CAST_FLAG_CAST))
            return child;
    }

    return nullptr;
}

void TypeInfoStruct::flattenUsingFields()
{
    ScopedLock lk(mutexCache);
    SWAG_RACE_CONDITION_READ(raceFields);

    if (!flattenFields.empty())
        return;
    if (fields.empty())
        return;

    flattenFields.reserve(fields.size());
    for (const auto p : fields)
        flatten(flattenFields, p);
}

void TypeInfoStruct::collectUsingFields(VectorNative<std::pair<TypeInfoParam*, uint32_t>>& result, uint32_t offset)
{
    if (!flags.has(TYPEINFO_HAS_USING))
        return;

    ScopedLock lk(mutexCache);
    SWAG_RACE_CONDITION_READ(raceFields);

    for (const auto p : fields)
    {
        if (!p->flags.has(TYPEINFOPARAM_HAS_USING))
            continue;

        const auto ptrStruct = p->typeInfo->getStructOrPointedStruct();
        if (ptrStruct)
        {
            ptrStruct->collectUsingFields(result, offset + p->offset);
            result.push_back({p, offset + p->offset});
        }
    }
}

TypeInfo* TypeInfoStruct::clone()
{
    ScopedLock lk(mutex);

    const auto newType         = makeType<TypeInfoStruct>();
    newType->scope             = scope;
    newType->opInit            = opInit;
    newType->opUserInitFct     = opUserInitFct;
    newType->opDrop            = opDrop;
    newType->opUserDropFct     = opUserDropFct;
    newType->opPostCopy        = opPostCopy;
    newType->opUserPostCopyFct = opUserPostCopyFct;
    newType->opPostMove        = opPostMove;
    newType->opUserPostMoveFct = opUserPostMoveFct;
    newType->attributes        = attributes;
    newType->itable            = itable;
    newType->alignOf           = alignOf;
    newType->structName        = structName;
    newType->replaceTypes      = replaceTypes;

    uint32_t size = genericParameters.size();
    newType->genericParameters.reserve(size);
    for (uint32_t i = 0; i < size; i++)
    {
        auto param = genericParameters[i];
        param      = param->clone();
        newType->genericParameters.push_back(param);
    }

    size = fields.size();
    newType->fields.reserve(size);
    for (uint32_t i = 0; i < size; i++)
    {
        auto param = fields[i];
        param      = param->clone();
        newType->fields.push_back(param);
    }

    size = constDecl.size();
    newType->constDecl.reserve(size);
    for (uint32_t i = 0; i < size; i++)
    {
        auto param = constDecl[i];
        param      = param->clone();
        newType->constDecl.push_back(param);
    }

    size = methods.size();
    newType->methods.reserve(size);
    for (uint32_t i = 0; i < size; i++)
    {
        auto param = methods[i];
        param      = param->clone();
        newType->methods.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

bool TypeInfoStruct::isSame(const TypeInfo* to, CastFlags castFlags) const
{
    if (this == to)
        return true;

    if (to->hasFlag(TYPEINFO_CONST_ALIAS))
        to = castTypeInfo<TypeInfoAlias>(to, to->kind);

    if (castFlags.has(CAST_FLAG_CAST))
    {
        if (to->isKindGeneric())
            return true;
    }

    if (castFlags.has(CAST_FLAG_INTERFACE))
    {
        if (kind == TypeInfoKind::Interface && to->isStruct())
        {
            const auto other = castTypeInfo<TypeInfoStruct>(to, to->kind);
            if (other->hasInterface(this))
                return true;
        }
    }

    if (!TypeInfo::isSame(to, castFlags))
        return false;

    // A tuple can only match a tuple, and a struct another struct
    if (!castFlags.has(CAST_FLAG_CAST) ||
        castFlags.has(CAST_FLAG_FOR_GENERIC) ||
        castFlags.has(CAST_FLAG_EXACT_TUPLE_STRUCT))
    {
        if (isTuple() != to->isTuple())
            return false;
    }

    const bool hasTuple = isTuple() || to->isTuple();
    const auto other    = castTypeInfo<TypeInfoStruct>(to, to->kind);

    // Do not compare names if one is a tuple
    const bool sameName = declNode->ownerScope == to->declNode->ownerScope && structName == other->structName;
    if (!hasTuple && !sameName)
        return false;

    // Compare generic parameters
    if (!hasFlag(TYPEINFO_GENERATED_TUPLE) && !other->hasFlag(TYPEINFO_GENERATED_TUPLE))
    {
        if (hasFlag(TYPEINFO_GENERIC | TYPEINFO_FROM_GENERIC) && other->hasFlag(TYPEINFO_GENERIC | TYPEINFO_FROM_GENERIC))
        {
            const auto numGenParams = genericParameters.size();
            if (numGenParams != other->genericParameters.size())
                return false;
            for (uint32_t i = 0; i < numGenParams; i++)
            {
                const auto myGenParam    = genericParameters[i];
                const auto otherGenParam = other->genericParameters[i];
                if (castFlags.has(CAST_FLAG_CAST))
                {
                    if (otherGenParam->typeInfo->isKindGeneric())
                        continue;
                }

                if (myGenParam->flags.has(TYPEINFOPARAM_DEFINED_VALUE) || otherGenParam->flags.has(TYPEINFOPARAM_DEFINED_VALUE))
                {
                    SemanticContext cxt;
                    const auto      castCom = castFlags.with(CAST_FLAG_JUST_CHECK | CAST_FLAG_COMMUTATIVE);
                    if (!TypeManager::makeCompatibles(&cxt, otherGenParam->typeInfo, myGenParam->typeInfo, nullptr, nullptr, castCom) &&
                        !TypeManager::makeCompatibles(&cxt, myGenParam->typeInfo, otherGenParam->typeInfo, nullptr, nullptr, castCom))
                        return false;

                    if (!Semantic::valueEqualsTo(myGenParam->value, otherGenParam->value, myGenParam->typeInfo, 0))
                        return false;
                }
                else if (!myGenParam->typeInfo->isSame(otherGenParam->typeInfo, castFlags))
                {
                    return false;
                }
            }
        }
    }

    // Compare field by field
    bool compareFields = false;
    if (!hasTuple && !castFlags.has(CAST_FLAG_CAST))
    {
        if (isGeneric() != other->isGeneric())
            return false;
        if (scope != other->scope)
            return false;

        compareFields = true;
    }
    else if (hasTuple && !sameName)
    {
        if (!hasFlag(TYPEINFO_GENERATED_TUPLE) && !other->hasFlag(TYPEINFO_GENERATED_TUPLE))
        {
            if (isGeneric() != other->isGeneric())
                return false;
        }

        compareFields = true;
    }

    if (compareFields)
    {
        const size_t childCount = fields.size();
        if (childCount != other->fields.size())
            return false;

        for (uint32_t i = 0; i < childCount; i++)
        {
            // Compare field type
            if (!fields[i]->isSame(other->fields[i], castFlags))
                return false;

            // But this is ok to affect between tuple and struct even if they do not have the same fields names
            bool compareNames = false;
            if (castFlags.has(CAST_FLAG_EXACT))
                compareNames = true;
            else if (!castFlags.has(CAST_FLAG_FOR_AFFECT) && !castFlags.has(CAST_FLAG_FOR_COMPARE))
                compareNames = true;
            else if (castFlags.has(CAST_FLAG_EXACT_TUPLE_STRUCT))
                compareNames = true;
            if (compareNames)
            {
                if (fields[i]->name != other->fields[i]->name)
                    return false;
            }
        }
    }

    return true;
}

bool TypeInfoStruct::canRawCopy() const
{
    return !opPostCopy && !opUserPostCopyFct && !opPostMove && !opUserPostMoveFct;
}

bool TypeInfoStruct::isPlainOldData() const
{
    return canRawCopy() && !opDrop && !opUserDropFct;
}

Utf8 TypeInfoStruct::getDisplayName()
{
    if (declNode && declNode->is(AstNodeKind::InterfaceDecl))
        return form("interface %s", name.cstr());
    if (declNode && declNode->is(AstNodeKind::StructDecl) && declNode->hasSpecFlag(AstStruct::SPEC_FLAG_UNION))
        return form("union %s", name.cstr());

    Utf8 str;
    computeWhateverName(str, ComputeNameKind::DisplayName);
    if (isTuple())
        return form("%s", str.cstr());
    return form("struct %s", str.cstr());
}

Utf8 TypeInfoStruct::computeTupleDisplayName(const VectorNative<TypeInfoParam*>& fields, ComputeNameKind nameKind)
{
    Utf8 resName = "{";
    bool first   = true;
    for (const auto param : fields)
    {
        if (!first)
            resName += ",";
        first = false;

        if (nameKind == ComputeNameKind::DisplayName)
        {
            if (resName.length() > 20)
            {
                resName += "...";
                break;
            }
        }

        if (!param->name.empty() && !param->flags.has(TYPEINFOPARAM_AUTO_NAME))
        {
            resName += param->name;
            resName += ":";
        }

        resName += param->typeInfo->computeWhateverName(nameKind);
    }

    resName += "}";
    return resName;
}

void TypeInfoStruct::computeWhateverName(Utf8& resName, ComputeNameKind nameKind)
{
    if (isTuple() && nameKind != ComputeNameKind::Name)
    {
        resName = computeTupleDisplayName(fields, nameKind);
        return;
    }

    if (nameKind != ComputeNameKind::Name && nameKind != ComputeNameKind::DisplayName)
        computeScopedName(resName);

    resName += structName;

    computeNameGenericParameters(genericParameters, resName, nameKind);
}
