#include "pch.h"
#include "Ast.h"
#include "Semantic.h"
#include "TypeManager.h"

TypeInfo* TypeInfoNative::clone()
{
    auto newType = makeType<TypeInfoNative>();
    newType->copyFrom(this);
    newType->valueInteger = valueInteger;
    return newType;
}

bool TypeInfoNative::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;

    if (castFlags & CASTFLAG_CAST)
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

    auto other = static_cast<TypeInfoNative*>(to);
    if (nativeType != other->nativeType)
        return false;

    return true;
}

TypeInfo* TypeInfoNamespace::clone()
{
    auto newType   = makeType<TypeInfoNamespace>();
    newType->scope = scope;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoCode::clone()
{
    auto newType = makeType<TypeInfoCode>();
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoCode::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, castFlags))
        return false;
    if (castFlags & CASTFLAG_CAST)
        return true;
    return false;
}

TypeInfo* TypeInfoAlias::clone()
{
    auto newType     = makeType<TypeInfoAlias>();
    newType->rawType = rawType;
    newType->copyFrom(this);
    return newType;
}

void TypeInfoAlias::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    if (nameType == COMPUTE_DISPLAY_NAME && flags & TYPEINFO_FAKE_ALIAS)
    {
        if (flags & TYPEINFO_CONST)
            resName += "const ";
        resName += rawType->computeWhateverName(nameType);
    }
    else
    {
        TypeInfo::computeWhateverName(resName, nameType);
    }
}

bool TypeInfoAlias::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;

    if (!TypeInfo::isSame(to, castFlags) || name != to->name)
    {
        auto me = TypeManager::concreteType(this, CONCRETE_ALIAS);
        if (me != this)
            return me->isSame(to, castFlags);
        return false;
    }

    if (to->isAlias())
    {
        auto other = static_cast<TypeInfoAlias*>(to);
        return rawType->isSame(other->rawType, castFlags);
    }

    SWAG_ASSERT(flags & TYPEINFO_FAKE_ALIAS);
    return rawType->isSame(to, castFlags);
}

TypeInfo* TypeInfoPointer::clone()
{
    auto newType         = makeType<TypeInfoPointer>();
    newType->pointedType = pointedType;
    newType->copyFrom(this);
    return newType;
}

Utf8 TypeInfoPointer::getDisplayName()
{
    if (flags & TYPEINFO_POINTER_AUTO_REF)
        return pointedType->getDisplayName();

    Utf8 str;
    computeWhateverName(str, COMPUTE_DISPLAY_NAME);
    return str;
}

void TypeInfoPointer::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    if (flags & TYPEINFO_C_STRING)
    {
        resName = "cstring";
        return;
    }

    if (!pointedType)
    {
        resName = "null";
        return;
    }

    if (!(flags & TYPEINFO_POINTER_AUTO_REF) || (nameType != COMPUTE_DISPLAY_NAME && nameType != COMPUTE_SCOPED_NAME_EXPORT))
    {
        if (flags & TYPEINFO_CONST)
            resName += "const ";
        if (flags & TYPEINFO_POINTER_REF && (flags & TYPEINFO_POINTER_MOVE_REF))
            resName += "&&";
        else if (flags & TYPEINFO_POINTER_ACCEPT_MOVE_REF)
            resName += "&&";
        else if (flags & TYPEINFO_POINTER_REF)
            resName += "&";
        else if (flags & TYPEINFO_POINTER_ARITHMETIC)
            resName += "^";
        else
            resName += "*";
    }

    resName += pointedType->computeWhateverName(nameType);
}

bool TypeInfoPointer::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;

    to = TypeManager::concreteType(to);
    if (castFlags & CASTFLAG_CAST)
    {
        if (to->isKindGeneric() && !(flags & TYPEINFO_POINTER_MOVE_REF))
            return true;
        if (this->isPointerNull() && to->isLambdaClosure())
            return true;
    }

    if (!TypeInfo::isSame(to, castFlags))
        return false;

    if (castFlags & CASTFLAG_CAST)
    {
        if (this->isPointerNull())
            return true;
        if (to->isPointerNull())
            return true;
    }

    auto other = static_cast<TypeInfoPointer*>(to);

    // Anonymous pointers
    if (castFlags & CASTFLAG_CAST)
    {
        if (other->pointedType->isVoid() && !(castFlags & CASTFLAG_FOR_GENERIC))
            return true;
        if ((to->flags & TYPEINFO_POINTER_ARITHMETIC) && !(flags & TYPEINFO_POINTER_ARITHMETIC))
            return false;
        if ((to->flags & TYPEINFO_POINTER_REF) || (flags & TYPEINFO_POINTER_REF))
            return false;
    }

    return pointedType->isSame(other->pointedType, castFlags);
}

TypeInfo* TypeInfoArray::clone()
{
    auto newType         = makeType<TypeInfoArray>();
    newType->pointedType = pointedType;
    newType->finalType   = finalType;
    newType->count       = count;
    newType->totalCount  = totalCount;
    newType->sizeNode    = sizeNode;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoArray::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;
    if (to->isKindGeneric())
        return true;
    if (!TypeInfo::isSame(to, castFlags))
        return false;

    auto other = static_cast<TypeInfoArray*>(to);
    if ((flags & TYPEINFO_GENERIC) == (other->flags & TYPEINFO_GENERIC))
    {
        if (count != other->count)
            return false;
    }

    if (!pointedType->isSame(other->pointedType, castFlags))
        return false;

    SWAG_ASSERT(!sizeOf || !other->sizeOf || sizeOf == other->sizeOf);
    return true;
}

void TypeInfoArray::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    if (flags & TYPEINFO_CONST)
        resName += "const ";

    if (count == UINT32_MAX)
    {
        resName += "[] ";
    }
    else
    {
        if (count == 0)
            resName += Fmt("[?", count);
        else
            resName += Fmt("[%d", count);
        auto pType = pointedType;
        while (pType->isArray() && pType != finalType)
        {
            auto subType = CastTypeInfo<TypeInfoArray>(pType, TypeInfoKind::Array);
            if (subType->count == 0)
                resName += Fmt(",?", subType->count);
            else
                resName += Fmt(",%d", subType->count);
            pType = subType->pointedType;
        }

        resName += "]";
        if (!finalType->isArray())
            resName += " ";
    }

    resName += finalType->computeWhateverName(nameType);
}

void TypeInfoSlice::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    if (flags & TYPEINFO_CONST)
        resName += "const ";
    resName += "[..] ";
    resName += pointedType->computeWhateverName(nameType);
}

TypeInfo* TypeInfoSlice::clone()
{
    auto newType         = makeType<TypeInfoSlice>();
    newType->pointedType = pointedType;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoSlice::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, castFlags))
        return false;
    auto castedFrom = static_cast<TypeInfoSlice*>(to);
    return pointedType->isSame(castedFrom->pointedType, castFlags);
}

void TypeInfoList::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    if (kind == TypeInfoKind::TypeListArray)
    {
        if (flags & TYPEINFO_CONST)
            resName += "const ";
        resName += Fmt("[%u] ", subTypes.size());
        if (!subTypes.empty())
        {
            subTypes[0]->typeInfo->computeWhateverName(nameType);
            resName += subTypes[0]->typeInfo->name;
        }
    }
    else
    {
        resName = TypeInfoStruct::computeTupleDisplayName(subTypes, nameType);
    }
}

TypeInfo* TypeInfoList::clone()
{
    auto newType = makeType<TypeInfoList>(kind);

    int size = (int) subTypes.size();
    newType->subTypes.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = static_cast<TypeInfoParam*>(subTypes[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->subTypes.push_back(param);
    }

    newType->scope = scope;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoList::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;

    // Can cast from typelist tuple to struct
    // The real check will be done later
    if (castFlags & CASTFLAG_CAST)
    {
        if (to->isStruct() && kind == TypeInfoKind::TypeListTuple)
            return true;
    }

    if (!TypeInfo::isSame(to, castFlags))
        return false;
    auto other = static_cast<TypeInfoList*>(to);
    if (subTypes.size() != other->subTypes.size())
        return false;

    for (size_t i = 0; i < subTypes.size(); i++)
    {
        if (!subTypes[i]->isSame(other->subTypes[i], castFlags))
            return false;
    }

    if (castFlags & CASTFLAG_EXACT)
    {
        if (scope != other->scope)
            return false;
    }

    return true;
}

TypeInfo* TypeInfoVariadic::clone()
{
    auto newType = makeType<TypeInfoVariadic>();
    newType->copyFrom(this);
    newType->rawType = rawType;
    return newType;
}

void TypeInfoVariadic::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    if (kind == TypeInfoKind::CVariadic)
    {
        resName = "cvarargs";
        return;
    }

    if (rawType)
        resName += rawType->computeWhateverName(nameType);
    resName += "...";
}

bool TypeInfoVariadic::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (!TypeInfo::isSame(to, castFlags))
        return false;
    auto other = static_cast<TypeInfoVariadic*>(to);
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
    auto newType = makeType<TypeInfoGeneric>();
    newType->copyFrom(this);
    newType->rawType = rawType;
    return newType;
}

bool TypeInfoGeneric::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;
    if (!(castFlags & CASTFLAG_EXACT) && !to->isKindGeneric())
        return true;
    if (to->kind == kind)
        return name == to->name;
    if (castFlags & CASTFLAG_EXACT)
        return name == to->name;
    return true;
}

Utf8 TypeInfoEnum::getDisplayName()
{
    Utf8 str;
    computeWhateverName(str, COMPUTE_DISPLAY_NAME);
    return Fmt("enum %s", str.c_str());
}

bool TypeInfoEnum::contains(const Utf8& valueName)
{
    for (auto p : values)
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
    if (!(flags & TYPEINFO_ENUM_HAS_USING))
        return;

    for (auto value : values)
    {
        if (value->typeInfo->isEnum())
        {
            auto subType = CastTypeInfo<TypeInfoEnum>(value->typeInfo, TypeInfoKind::Enum);
            collect.push_back_once(subType);
        }
    }
}

TypeInfo* TypeInfoEnum::clone()
{
    auto newType        = makeType<TypeInfoEnum>();
    newType->scope      = scope;
    newType->rawType    = rawType;
    newType->attributes = attributes;

    for (size_t i = 0; i < values.size(); i++)
    {
        auto param = static_cast<TypeInfoParam*>(values[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->values.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

bool TypeInfoEnum::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;

    if (castFlags & CASTFLAG_CAST)
    {
        if (to->isKindGeneric())
            return true;
    }

    if (!TypeInfo::isSame(to, castFlags))
        return false;

    if (name != to->name)
        return false;

    auto other = static_cast<TypeInfoEnum*>(to);
    if (!rawType->isSame(other->rawType, castFlags))
        return false;
    if (values.size() != other->values.size())
        return false;

    if (!(castFlags & CASTFLAG_CAST))
    {
        auto childSize = values.size();
        if (childSize != other->values.size())
            return false;
        for (size_t i = 0; i < childSize; i++)
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

    auto newType                  = makeType<TypeInfoFuncAttr>();
    newType->firstDefaultValueIdx = firstDefaultValueIdx;
    newType->returnType           = returnType;
    newType->stackSize            = stackSize;
    newType->attributes           = attributes;
    newType->attributeUsage       = attributeUsage;
    newType->callConv             = callConv;
    newType->replaceTypes         = replaceTypes;

    for (size_t i = 0; i < genericParameters.size(); i++)
    {
        auto param = genericParameters[i]->clone();
        newType->genericParameters.push_back(param);
    }

    for (size_t i = 0; i < parameters.size(); i++)
    {
        auto param = parameters[i]->clone();
        newType->parameters.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

static void computeNameGenericParameters(VectorNative<TypeInfoParam*>& genericParameters, Utf8& resName, uint32_t nameType)
{
    if (genericParameters.empty())
        return;

    if (nameType != COMPUTE_DISPLAY_NAME || genericParameters.size() > 1)
        resName += "'(";
    else
        resName += "'";

    for (size_t i = 0; i < genericParameters.size(); i++)
    {
        if (i)
            resName += ", ";

        auto genParam = genericParameters[i];
        if (genParam->flags & TYPEINFOPARAM_DEFINED_VALUE)
        {
            SWAG_ASSERT(genParam->typeInfo);
            SWAG_ASSERT(genParam->value);
            auto str = Ast::literalToString(genParam->typeInfo, *genParam->value);
            if (genParam->typeInfo->isString())
                resName += "\"";
            resName += str;
            if (genParam->typeInfo->isString())
                resName += "\"";
        }
        else if (genParam->flags & TYPEINFOPARAM_GENERIC_CONSTANT)
            resName += genParam->name;
        else if (genParam->typeInfo)
            resName += genParam->typeInfo->computeWhateverName(nameType);
        else
            resName += genParam->name;
    }

    if (nameType != COMPUTE_DISPLAY_NAME || genericParameters.size() > 1)
        resName += ")";
}

void TypeInfoFuncAttr::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    bool addedName = false;
    if (kind != TypeInfoKind::LambdaClosure)
    {
        if (nameType == COMPUTE_SCOPED_NAME || nameType == COMPUTE_SCOPED_NAME_EXPORT)
        {
            getScopedName(resName);
            SWAG_ASSERT(declNode);
            resName += declNode->token.text;
            addedName = true;
        }
    }

    if (!addedName)
    {
        if (flags & TYPEINFO_FUNC_IS_ATTR)
            resName += "attr";
        else if (isClosure())
            resName += "closure";
        else
            resName += "func";
    }

    computeNameGenericParameters(genericParameters, resName, nameType);

    // Parameters
    resName += "(";
    bool first = true;
    for (size_t i = 0; i < parameters.size(); i++)
    {
        // First parameter of a closure is generated, so do not count it in the name
        if (isClosure() && !i)
            continue;

        if (!first)
            resName += ", ";
        first = false;
        resName += parameters[i]->typeInfo->computeWhateverName(nameType);
    }

    resName += ")";

    // Return type
    if (returnType && !returnType->isVoid())
    {
        resName += "->";
        resName += returnType->computeWhateverName(nameType);
    }

    if (flags & TYPEINFO_CAN_THROW)
        resName += " throw";
}

bool TypeInfoFuncAttr::isSame(TypeInfoFuncAttr* other, uint64_t castFlags)
{
    BadSignatureInfos bi;
    return isSame(other, castFlags, bi);
}

bool TypeInfoFuncAttr::isSame(TypeInfoFuncAttr* other, uint64_t castFlags, BadSignatureInfos& bi)
{
    // Cannot convert a closure to a lambda
    if (isClosure() && !other->isClosure())
        return false;

    // Can convert a lambda to a closure, but do not take care of closure first parameter
    // as it is generated
    if (!isClosure() && other->isClosure())
    {
        if (parameters.size() + 1 != other->parameters.size())
            return false;
    }
    else
    {
        if (other->parameters.size() > parameters.size())
        {
            bi.matchResult = MatchResult::TooManyParameters;
            return false;
        }

        if (other->parameters.size() < parameters.size())
        {
            bi.matchResult = MatchResult::NotEnoughParameters;
            return false;
        }
    }

    if (genericParameters.size() != other->genericParameters.size())
    {
        // We want func's32(s32) to match func(T) when func(T) is generic
        // This is necessary for lambdas. Perhaps that test is not enough !
        if ((flags & TYPEINFO_GENERIC) == (other->flags & TYPEINFO_GENERIC) &&
            !(flags & TYPEINFO_UNDEFINED) &&
            !(other->flags & TYPEINFO_UNDEFINED))
            return false;
    }

    if ((flags & TYPEINFO_CAN_THROW) != (other->flags & TYPEINFO_CAN_THROW))
    {
        bi.matchResult = MatchResult::MismatchThrow;
        return false;
    }

    // Default values should be the same
    if (isLambdaClosure() && other->isLambdaClosure() && firstDefaultValueIdx != UINT32_MAX)
        return false;

    if (castFlags & CASTFLAG_EXACT)
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
        // If the types does not match, then the two functions are not the same.
        if (declNode && declNode->kind == AstNodeKind::FuncDecl && other->declNode && other->declNode->kind == AstNodeKind::FuncDecl)
        {
            auto myFunc        = CastAst<AstFuncDecl>(declNode, AstNodeKind::FuncDecl);
            auto typeMyFunc    = CastTypeInfo<TypeInfoFuncAttr>(myFunc->typeInfo, TypeInfoKind::FuncAttr);
            auto otherFunc     = CastAst<AstFuncDecl>(other->declNode, AstNodeKind::FuncDecl);
            auto typeOtherFunc = CastTypeInfo<TypeInfoFuncAttr>(otherFunc->typeInfo, TypeInfoKind::FuncAttr);
            if (typeMyFunc->replaceTypes.size() != typeOtherFunc->replaceTypes.size())
                return false;
            for (auto r : typeMyFunc->replaceTypes)
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
        for (size_t i = 0; i < genericParameters.size(); i++)
        {
            if (!genericParameters[i]->typeInfo->isSame(other->genericParameters[i]->typeInfo, castFlags))
                return false;
            if (!(genericParameters[i]->value == other->genericParameters[i]->value))
                return false;
        }
    }

    // Compare capture argument only if not converting a lambda to a closure
    if (isClosure() && other->isClosure())
    {
        if (capture.size() != other->capture.size())
            return false;
        for (size_t i = 0; i < capture.size(); i++)
        {
            if (capture[i]->typeInfo->isUndefined())
                continue;
            if (other->capture[i]->typeInfo->isUndefined())
                continue;
            auto type1 = TypeManager::concretePtrRef(capture[i]->typeInfo);
            auto type2 = TypeManager::concretePtrRef(other->capture[i]->typeInfo);
            if (!type1->isSame(type2, castFlags))
                return false;
        }
    }

    int firstParam = 0;
    if (!isClosure() && other->isClosure())
        firstParam = 1;

    for (size_t i = 0; i < parameters.size(); i++)
    {
        auto type1 = parameters[i]->typeInfo;
        auto type2 = other->parameters[i + firstParam]->typeInfo;

        if (type1->isUndefined() || type2->isUndefined())
            continue;

        if ((type1->flags & TYPEINFO_POINTER_MOVE_REF) != (type2->flags & TYPEINFO_POINTER_MOVE_REF))
        {
            bi.matchResult      = MatchResult::BadSignature;
            bi.badSignatureNum1 = (int) i;
            bi.badSignatureNum2 = (int) i + firstParam;
            return false;
        }

        if (type1->isAutoConstPointerRef() != type2->isAutoConstPointerRef() &&
            type1->isPointerRef() &&
            type2->isPointerRef())
        {
            bi.matchResult      = MatchResult::BadSignature;
            bi.badSignatureNum1 = (int) i;
            bi.badSignatureNum2 = (int) i + firstParam;
            return false;
        }

        type1 = TypeManager::concretePtrRef(type1);
        type2 = TypeManager::concretePtrRef(type2);
        if (!type1->isSame(type2, castFlags))
        {
            bi.matchResult      = MatchResult::BadSignature;
            bi.badSignatureNum1 = (int) i;
            bi.badSignatureNum2 = (int) i + firstParam;
            return false;
        }
    }

    return true;
}

bool TypeInfoFuncAttr::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;

    if (castFlags & CASTFLAG_CAST)
    {
        if (kind == TypeInfoKind::LambdaClosure && to->isPointerNull())
            return true;
    }

    if (!TypeInfo::isSame(to, castFlags))
        return false;

    auto other = static_cast<TypeInfoFuncAttr*>(to);
    SWAG_ASSERT(to->isFuncAttr() || to->isLambdaClosure());
    if (!isSame(other, castFlags))
        return false;

    if ((castFlags & CASTFLAG_EXACT) || (to->isLambdaClosure()))
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

bool TypeInfoFuncAttr::isVariadic()
{
    if (parameters.empty())
        return false;
    auto typeParam = ((TypeInfoParam*) parameters.back())->typeInfo;
    if (typeParam->isVariadic() || typeParam->isTypedVariadic())
        return true;
    return false;
}

bool TypeInfoFuncAttr::isCVariadic()
{
    if (parameters.empty())
        return false;
    auto typeParam = ((TypeInfoParam*) parameters.back())->typeInfo;
    if (typeParam->isCVariadic())
        return true;
    return false;
}

TypeInfo* TypeInfoFuncAttr::concreteReturnType()
{
    if (!returnType)
        return g_TypeMgr->typeInfoVoid;
    return TypeManager::concreteType(returnType);
}

// argIdx is the argument index of a function, starting after the return arguments
uint32_t TypeInfoFuncAttr::registerIdxToParamIdx(int argIdx)
{
    if (flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        if (argIdx < 2)
            return (uint32_t) parameters.size() - 1;
        argIdx -= 2;
    }

    size_t argNo = 0;
    while (true)
    {
        if (argNo >= parameters.size())
        {
            SWAG_ASSERT(flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC));
            return (uint32_t) parameters.size() - 1;
        }

        auto typeParam = TypeManager::concreteType(parameters[argNo]->typeInfo);
        auto n         = typeParam->numRegisters();
        if (argIdx < n)
            return (uint32_t) argNo;
        argIdx -= n;
        argNo++;
    }

    return UINT32_MAX;
}

TypeInfo* TypeInfoFuncAttr::registerIdxToType(int argIdx)
{
    auto argNo = registerIdxToParamIdx(argIdx);
    if (argNo >= parameters.size())
        return nullptr;
    return TypeManager::concreteType(parameters[argNo]->typeInfo);
}

int TypeInfoFuncAttr::numParamsRegisters()
{
    int total = 0;
    for (auto param : parameters)
    {
        auto typeParam = TypeManager::concreteType(param->typeInfo);
        total += typeParam->numRegisters();
    }

    return total;
}

int TypeInfoFuncAttr::numReturnRegisters()
{
    return returnType ? returnType->numRegisters() : 0;
}

int TypeInfoFuncAttr::numTotalRegisters()
{
    return numReturnRegisters() + numParamsRegisters();
}

const CallConv& TypeInfoFuncAttr::getCallConv()
{
    return g_CallConv[callConv];
}

static void flatten(VectorNative<TypeInfoParam*>& result, TypeInfoParam* param)
{
    if (!(param->flags & TYPEINFOPARAM_HAS_USING) || param->typeInfo->kind != TypeInfoKind::Struct)
    {
        result.push_back(param);
        return;
    }

    auto typeStruct = CastTypeInfo<TypeInfoStruct>(param->typeInfo, TypeInfoKind::Struct);
    for (auto p : typeStruct->fields)
        flatten(result, p);
}

TypeInfoParam* TypeInfoStruct::findChildByNameNoLock(const Utf8& childName)
{
    for (auto child : fields)
    {
        if (child->name == childName)
            return child;
    }

    return nullptr;
}

TypeInfoParam* TypeInfoStruct::hasInterface(TypeInfoStruct* itf)
{
    SharedLock lk(mutex);
    return hasInterfaceNoLock(itf);
}

TypeInfoParam* TypeInfoStruct::hasInterfaceNoLock(TypeInfoStruct* itf)
{
    for (auto child : interfaces)
    {
        if (child->typeInfo->isSame(itf, CASTFLAG_CAST))
            return child;
    }

    return nullptr;
}

void TypeInfoStruct::flattenUsingFields()
{
    ScopedLock lk(mutexCache);
    if (flattenFields.size())
        return;
    flattenFields.reserve((uint32_t) fields.size());
    for (auto p : fields)
        flatten(flattenFields, p);
}

TypeInfo* TypeInfoStruct::clone()
{
    ScopedLock lk(mutex);

    auto newType               = makeType<TypeInfoStruct>();
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

    int size = (int) genericParameters.size();
    newType->genericParameters.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = genericParameters[i];
        param      = param->clone();
        newType->genericParameters.push_back(param);
    }

    size = (int) fields.size();
    newType->fields.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = fields[i];
        param      = param->clone();
        newType->fields.push_back(param);
    }

    size = (int) consts.size();
    newType->consts.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = consts[i];
        param      = param->clone();
        newType->consts.push_back(param);
    }

    size = (int) methods.size();
    newType->methods.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = methods[i];
        param      = param->clone();
        newType->methods.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

bool TypeInfoStruct::isSame(TypeInfo* to, uint64_t castFlags)
{
    if (this == to)
        return true;

    if (to->flags & TYPEINFO_FAKE_ALIAS)
        to = static_cast<TypeInfoAlias*>(to)->rawType;

    if (castFlags & CASTFLAG_CAST)
    {
        if (to->isKindGeneric())
            return true;
    }

    if (castFlags & CASTFLAG_INTERFACE)
    {
        if (kind == TypeInfoKind::Interface && to->isStruct())
        {
            auto other = CastTypeInfo<TypeInfoStruct>(to, to->kind);
            if (other->hasInterface(this))
                return true;
        }
    }

    if (!TypeInfo::isSame(to, castFlags))
        return false;

    // A tuple can only match a tuple, and a struct a struct
    if (!(castFlags & CASTFLAG_CAST) || (castFlags & CASTFLAG_FOR_GENERIC))
    {
        if (isTuple() != to->isTuple())
            return false;
    }

    bool hasTuple = isTuple() || to->isTuple();
    auto other    = CastTypeInfo<TypeInfoStruct>(to, to->kind);

    // Do not compare names if one is a tuple
    bool sameName = declNode->ownerScope == to->declNode->ownerScope && structName == other->structName;
    if (!hasTuple && !sameName)
        return false;

    // Compare generic parameters
    if (!(flags & TYPEINFO_GENERATED_TUPLE) && !(other->flags & TYPEINFO_GENERATED_TUPLE))
    {
        if ((flags & (TYPEINFO_GENERIC | TYPEINFO_FROM_GENERIC)) && (other->flags & (TYPEINFO_GENERIC | TYPEINFO_FROM_GENERIC)))
        {
            auto numGenParams = genericParameters.size();
            if (numGenParams != other->genericParameters.size())
                return false;
            for (size_t i = 0; i < numGenParams; i++)
            {
                auto myGenParam    = genericParameters[i];
                auto otherGenParam = other->genericParameters[i];
                if (castFlags & CASTFLAG_CAST)
                {
                    if (otherGenParam->typeInfo->isKindGeneric())
                        continue;
                }

                if (myGenParam->flags & TYPEINFOPARAM_DEFINED_VALUE || otherGenParam->flags & TYPEINFOPARAM_DEFINED_VALUE)
                {
                    SemanticContext cxt;
                    if (!TypeManager::makeCompatibles(&cxt, otherGenParam->typeInfo, myGenParam->typeInfo, nullptr, nullptr, castFlags | CASTFLAG_JUST_CHECK | CASTFLAG_COMMUTATIVE) &&
                        !TypeManager::makeCompatibles(&cxt, myGenParam->typeInfo, otherGenParam->typeInfo, nullptr, nullptr, castFlags | CASTFLAG_JUST_CHECK | CASTFLAG_COMMUTATIVE))
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
    if (!hasTuple && !(castFlags & CASTFLAG_CAST))
    {
        if ((flags & TYPEINFO_GENERIC) != (other->flags & TYPEINFO_GENERIC))
            return false;
        if (scope != other->scope)
            return false;

        compareFields = true;
    }
    else if (hasTuple && !sameName)
    {
        if (!(flags & TYPEINFO_GENERATED_TUPLE) && !(other->flags & TYPEINFO_GENERATED_TUPLE))
        {
            if ((flags & TYPEINFO_GENERIC) != (other->flags & TYPEINFO_GENERIC))
                return false;
        }

        compareFields = true;
    }

    if (compareFields)
    {
        size_t childCount = fields.size();
        if (childCount != other->fields.size())
            return false;

        for (size_t i = 0; i < childCount; i++)
        {
            // Compare field type
            if (!fields[i]->isSame(other->fields[i], castFlags))
                return false;

            // But this is ok to affect between tuple and struct even if they do not have the same fields names
            if (!(castFlags & CASTFLAG_FOR_AFFECT) || (castFlags & CASTFLAG_EXACT))
            {
                if (fields[i]->name != other->fields[i]->name)
                    return false;
            }
        }
    }

    return true;
}

bool TypeInfoStruct::canRawCopy()
{
    return !opPostCopy && !opUserPostCopyFct && !opPostMove && !opUserPostMoveFct;
}

bool TypeInfoStruct::isPlainOldData()
{
    return canRawCopy() && !opDrop && !opUserDropFct;
}

Utf8 TypeInfoStruct::getDisplayName()
{
    if (declNode && declNode->kind == AstNodeKind::InterfaceDecl)
        return Fmt("interface %s", name.c_str());
    if (declNode && declNode->kind == AstNodeKind::StructDecl && ((AstStruct*) declNode)->specFlags & AstStruct::SPECFLAG_UNION)
        return Fmt("union %s", name.c_str());

    Utf8 str;
    computeWhateverName(str, COMPUTE_DISPLAY_NAME);
    if (isTuple())
        return Fmt("%s", str.c_str());
    else
        return Fmt("struct %s", str.c_str());
}

Utf8 TypeInfoStruct::computeTupleDisplayName(const VectorNative<TypeInfoParam*>& fields, uint32_t nameType)
{
    Utf8 resName = "{";
    for (auto param : fields)
    {
        if (param != fields.front())
            resName += ",";

        if (nameType != COMPUTE_DISPLAY_NAME)
        {
            if (!param->name.empty() && !(param->flags & TYPEINFOPARAM_AUTO_NAME))
            {
                resName += param->name;
                resName += ": ";
            }
        }

        resName += param->typeInfo->computeWhateverName(nameType);

        if (nameType == COMPUTE_DISPLAY_NAME)
        {
            if (resName.length() > 20)
            {
                resName += "...";
                break;
            }
        }
    }

    resName += "}";
    return resName;
}

void TypeInfoStruct::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    if (isTuple())
    {
        resName = TypeInfoStruct::computeTupleDisplayName(fields, nameType);
        return;
    }

    if (nameType != COMPUTE_NAME && nameType != COMPUTE_DISPLAY_NAME)
        getScopedName(resName);
    resName += structName;
    computeNameGenericParameters(genericParameters, resName, nameType);
}
