#include "pch.h"
#include "TypeManager.h"
#include "Ast.h"
#include "SemanticJob.h"

TypeInfo* TypeInfoNative::clone()
{
    auto newType = allocType<TypeInfoNative>();
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoNative::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    if (isSameFlags & ISSAME_CAST)
    {
        if (to->kind == TypeInfoKind::Generic)
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
    auto newType   = allocType<TypeInfoNamespace>();
    newType->scope = scope;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoCode::clone()
{
    auto newType = allocType<TypeInfoCode>();
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoCode::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    if (isSameFlags & ISSAME_CAST)
        return true;
    return false;
}

TypeInfo* TypeInfoAlias::clone()
{
    auto newType     = allocType<TypeInfoAlias>();
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

bool TypeInfoAlias::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    if (!TypeInfo::isSame(to, isSameFlags) || name != to->name)
    {
        auto me = TypeManager::concreteType(this, CONCRETE_ALIAS);
        if (me != this)
            return me->isSame(to, isSameFlags);
        return false;
    }

    if (to->isAlias())
    {
        auto other = static_cast<TypeInfoAlias*>(to);
        return rawType->isSame(other->rawType, isSameFlags);
    }

    SWAG_ASSERT(flags & TYPEINFO_FAKE_ALIAS);
    return rawType->isSame(to, isSameFlags);
}

TypeInfo* TypeInfoPointer::clone()
{
    auto newType         = allocType<TypeInfoPointer>();
    newType->pointedType = pointedType;
    newType->copyFrom(this);
    return newType;
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

    if (!(flags & TYPEINFO_POINTER_AUTO_REF))
    {
        if (flags & TYPEINFO_CONST)
            resName += "const ";
        if (flags & TYPEINFO_POINTER_REF)
            resName += "ref ";
        else if (flags & TYPEINFO_POINTER_ARITHMETIC)
            resName += "^";
        else
            resName += "*";
    }

    resName += pointedType->computeWhateverName(nameType);
}

bool TypeInfoPointer::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    to = TypeManager::concreteType(to);
    if (isSameFlags & ISSAME_CAST)
    {
        if (to->kind == TypeInfoKind::Generic)
            return true;
        if (this == g_TypeMgr->typeInfoNull && to->isLambdaClosure())
            return true;
    }

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    if (isSameFlags & ISSAME_CAST)
    {
        if (this == g_TypeMgr->typeInfoNull)
            return true;
        if (to == g_TypeMgr->typeInfoNull)
            return true;
    }

    auto other = static_cast<TypeInfoPointer*>(to);

    // Anonymous pointers
    if (isSameFlags & ISSAME_CAST)
    {
        if (other->pointedType == g_TypeMgr->typeInfoVoid && !(isSameFlags & ISSAME_FOR_GENERIC))
            return true;
        if ((to->flags & TYPEINFO_POINTER_ARITHMETIC) && !(flags & TYPEINFO_POINTER_ARITHMETIC))
            return false;
        if ((to->flags & TYPEINFO_POINTER_REF) || (flags & TYPEINFO_POINTER_REF))
            return false;
    }

    return pointedType->isSame(other->pointedType, isSameFlags);
}

TypeInfo* TypeInfoArray::clone()
{
    auto newType         = allocType<TypeInfoArray>();
    newType->pointedType = pointedType;
    newType->finalType   = finalType;
    newType->count       = count;
    newType->totalCount  = totalCount;
    newType->sizeNode    = sizeNode;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoArray::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (to->kind == TypeInfoKind::Generic)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    auto other = static_cast<TypeInfoArray*>(to);
    if ((flags & TYPEINFO_GENERIC) == (other->flags & TYPEINFO_GENERIC))
    {
        if (count != other->count)
            return false;
    }

    if (!pointedType->isSame(other->pointedType, isSameFlags))
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
        while (pType->isArray())
        {
            auto subType = CastTypeInfo<TypeInfoArray>(pType, TypeInfoKind::Array);
            if (subType->count == 0)
                resName += Fmt(",?", subType->count);
            else
                resName += Fmt(",%d", subType->count);
            pType = subType->pointedType;
        }

        resName += "] ";
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
    auto newType         = allocType<TypeInfoSlice>();
    newType->pointedType = pointedType;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoSlice::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto castedFrom = static_cast<TypeInfoSlice*>(to);
    return pointedType->isSame(castedFrom->pointedType, isSameFlags);
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
    else if (nameType == COMPUTE_DISPLAY_NAME)
    {
        resName = "tuple";
    }
}

Utf8 TypeInfoList::computeTupleName(JobContext* context)
{
    Utf8 structName = context->sourceFile->scopeFile->name + "_tuple_";

    int numChilds = (int) subTypes.size();
    for (int idx = 0; idx < numChilds; idx++)
    {
        auto typeParam = subTypes[idx];
        auto childType = typeParam->typeInfo;
        if (!typeParam->namedParam.empty())
            structName += typeParam->namedParam;
        structName += childType->name;
    }

    Ast::normalizeIdentifierName(structName);
    return structName;
}

TypeInfo* TypeInfoList::clone()
{
    auto newType = allocType<TypeInfoList>(kind);

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

bool TypeInfoList::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    // Can cast from typelist tuple to struct
    // The real check will be done later
    if (isSameFlags & ISSAME_CAST)
    {
        if (to->isStruct() && kind == TypeInfoKind::TypeListTuple)
            return true;
    }

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto other = static_cast<TypeInfoList*>(to);
    if (subTypes.size() != other->subTypes.size())
        return false;

    for (int i = 0; i < subTypes.size(); i++)
    {
        if (!subTypes[i]->isSame(other->subTypes[i], isSameFlags))
            return false;
    }

    if (isSameFlags & ISSAME_EXACT)
    {
        if (scope != other->scope)
            return false;
    }

    return true;
}

TypeInfo* TypeInfoVariadic::clone()
{
    auto newType = allocType<TypeInfoVariadic>();
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

bool TypeInfoVariadic::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto other = static_cast<TypeInfoVariadic*>(to);
    if (rawType && !other->rawType)
        return false;
    if (!rawType && other->rawType)
        return false;
    if (rawType == other->rawType)
        return true;
    return rawType->isSame(other->rawType, isSameFlags);
}

TypeInfo* TypeInfoGeneric::clone()
{
    auto newType = allocType<TypeInfoGeneric>();
    newType->copyFrom(this);
    newType->rawType = rawType;
    return newType;
}

bool TypeInfoGeneric::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!(isSameFlags & ISSAME_EXACT) && to->kind != TypeInfoKind::Generic)
        return true;
    if (to->kind == kind)
        return name == to->name;
    if (isSameFlags & ISSAME_EXACT)
        return name == to->name;
    return true;
}

bool TypeInfoEnum::contains(const Utf8& valueName)
{
    for (auto p : values)
    {
        if (p->namedParam == valueName)
            return true;
    }

    return false;
}

TypeInfo* TypeInfoEnum::clone()
{
    auto newType        = allocType<TypeInfoEnum>();
    newType->scope      = scope;
    newType->rawType    = rawType;
    newType->attributes = attributes;

    for (int i = 0; i < values.size(); i++)
    {
        auto param = static_cast<TypeInfoParam*>(values[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->values.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

bool TypeInfoEnum::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (isSameFlags & ISSAME_CAST)
    {
        if (to->kind == TypeInfoKind::Generic)
            return true;
    }

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto other = static_cast<TypeInfoEnum*>(to);
    if (!rawType->isSame(other->rawType, isSameFlags))
        return false;
    if (values.size() != other->values.size())
        return false;

    if (!(isSameFlags & ISSAME_CAST))
    {
        int childSize = (int) values.size();
        if (childSize != other->values.size())
            return false;
        for (int i = 0; i < childSize; i++)
        {
            if (!values[i]->isSame(other->values[i], isSameFlags))
                return false;
        }
    }

    return true;
}

TypeInfo* TypeInfoFuncAttr::clone()
{
    ScopedLock lk(mutex);

    auto newType                  = allocType<TypeInfoFuncAttr>();
    newType->firstDefaultValueIdx = firstDefaultValueIdx;
    newType->returnType           = returnType;
    newType->stackSize            = stackSize;
    newType->attributes           = attributes;
    newType->attributeUsage       = attributeUsage;
    newType->callConv             = callConv;

    for (int i = 0; i < genericParameters.size(); i++)
    {
        auto param = genericParameters[i]->clone();
        newType->genericParameters.push_back(param);
    }

    for (int i = 0; i < parameters.size(); i++)
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

    for (int i = 0; i < genericParameters.size(); i++)
    {
        if (i)
            resName += ", ";

        auto genParam = genericParameters[i];
        if (genParam->flags & TYPEINFO_DEFINED_VALUE)
        {
            SWAG_ASSERT(genParam->typeInfo);
            SWAG_ASSERT(genParam->value);
            auto str = Ast::literalToString(genParam->typeInfo, *genParam->value);
            resName += str;
        }
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
    for (int i = 0; i < parameters.size(); i++)
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

bool TypeInfoFuncAttr::isSame(TypeInfoFuncAttr* other, uint32_t isSameFlags)
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
        if (parameters.size() != other->parameters.size())
            return false;
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
        return false;

    if (isSameFlags & ISSAME_EXACT)
    {
        if (!returnType && other->returnType && !other->returnType->isVoid())
            return false;
        if (returnType && !returnType->isVoid() && !other->returnType)
            return false;
        if (returnType && other->returnType && !returnType->isNative(NativeTypeKind::Undefined) && !returnType->isSame(other->returnType, isSameFlags))
            return false;

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
                if (r.second != it->second)
                    return false;
            }
        }
    }

    if (genericParameters.size() == other->genericParameters.size())
    {
        for (int i = 0; i < genericParameters.size(); i++)
        {
            if (!genericParameters[i]->typeInfo->isSame(other->genericParameters[i]->typeInfo, isSameFlags))
                return false;
            if (!(genericParameters[i]->value == other->genericParameters[i]->value))
                return false;
        }
    }

    // Compare capture argument only if not converting a lambda to a closure
    if ((isClosure()) && (other->isClosure()))
    {
        if (capture.size() != other->capture.size())
            return false;
        for (int i = 0; i < capture.size(); i++)
        {
            if (capture[i]->typeInfo->isNative(NativeTypeKind::Undefined))
                continue;
            if (other->capture[i]->typeInfo->isNative(NativeTypeKind::Undefined))
                continue;
            auto type1 = TypeManager::concretePtrRef(capture[i]->typeInfo);
            auto type2 = TypeManager::concretePtrRef(other->capture[i]->typeInfo);
            if (!type1->isSame(type2, isSameFlags))
                return false;
        }
    }

    int firstParam = 0;
    if (!(isClosure()) && (other->isClosure()))
        firstParam = 1;

    for (int i = 0; i < parameters.size(); i++)
    {
        if (parameters[i]->typeInfo->isNative(NativeTypeKind::Undefined))
            continue;
        if (other->parameters[i + firstParam]->typeInfo->isNative(NativeTypeKind::Undefined))
            continue;
        auto type1 = TypeManager::concretePtrRef(parameters[i]->typeInfo);
        auto type2 = TypeManager::concretePtrRef(other->parameters[i + firstParam]->typeInfo);
        if (!type1->isSame(type2, isSameFlags))
            return false;
    }

    return true;
}

bool TypeInfoFuncAttr::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    if (isSameFlags & ISSAME_CAST)
    {
        if (kind == TypeInfoKind::LambdaClosure && to == g_TypeMgr->typeInfoNull)
            return true;
    }

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    auto other = static_cast<TypeInfoFuncAttr*>(to);
    SWAG_ASSERT(to->isFuncAttr() || to->isLambdaClosure());
    if (!isSame(other, isSameFlags))
        return false;

    if ((isSameFlags & ISSAME_EXACT) || (to->isLambdaClosure()))
    {
        if (returnType && returnType != g_TypeMgr->typeInfoVoid && !other->returnType)
            return false;
        if (!returnType && other->returnType && other->returnType != g_TypeMgr->typeInfoVoid)
            return false;
        if (returnType && other->returnType && !returnType->isNative(NativeTypeKind::Undefined) && !returnType->isSame(other->returnType, isSameFlags))
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

bool TypeInfoFuncAttr::returnByCopy()
{
    if (!returnType || returnType == g_TypeMgr->typeInfoVoid)
        return false;

    auto type = concreteReturnType();
    if (type->isSlice() ||
        type->isInterface() ||
        type->isAny() ||
        type->isString() ||
        type->flags & TYPEINFO_RETURN_BY_COPY)
    {
        return true;
    }

    return false;
}

bool TypeInfoFuncAttr::returnByValue()
{
    if (!returnType || returnType == g_TypeMgr->typeInfoVoid)
        return false;
    return !returnByCopy();
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

    int argNo = 0;
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
            return argNo;
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

TypeInfo* TypeInfoStruct::clone()
{
    ScopedLock lk(mutex);

    auto newType               = allocType<TypeInfoStruct>();
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

bool TypeInfoStruct::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    if (to->flags & TYPEINFO_FAKE_ALIAS)
        to = static_cast<TypeInfoAlias*>(to)->rawType;

    if (isSameFlags & ISSAME_CAST)
    {
        if (to->kind == TypeInfoKind::Generic)
            return true;
    }

    if (isSameFlags & ISSAME_INTERFACE)
    {
        if (kind == TypeInfoKind::Interface && to->isStruct())
        {
            auto other = CastTypeInfo<TypeInfoStruct>(to, to->kind);
            if (other->hasInterface(this))
                return true;
        }
    }

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    if (isTuple() != to->isTuple())
        return false;

    auto other = CastTypeInfo<TypeInfoStruct>(to, to->kind);

    // Do not compare names for tuples
    bool isTuple  = flags & TYPEINFO_STRUCT_IS_TUPLE;
    bool sameName = declNode->ownerScope == to->declNode->ownerScope && structName == other->structName;
    if (!isTuple && !sameName)
        return false;

    // Compare generic parameters
    if (!(flags & TYPEINFO_GENERATED_TUPLE) && !(other->flags & TYPEINFO_GENERATED_TUPLE))
    {
        auto numGenParams = genericParameters.size();
        if (numGenParams != other->genericParameters.size())
            return false;
        for (int i = 0; i < numGenParams; i++)
        {
            auto myGenParam    = genericParameters[i];
            auto otherGenParam = other->genericParameters[i];
            if (isSameFlags & ISSAME_CAST)
            {
                if (otherGenParam->typeInfo->kind == TypeInfoKind::Generic)
                    continue;
            }

            if (myGenParam->flags & TYPEINFO_DEFINED_VALUE || otherGenParam->flags & TYPEINFO_DEFINED_VALUE)
            {
                SemanticContext cxt;
                if (!TypeManager::makeCompatibles(&cxt, otherGenParam->typeInfo, myGenParam->typeInfo, nullptr, nullptr, CASTFLAG_NO_ERROR | CASTFLAG_JUST_CHECK | CASTFLAG_COMMUTATIVE))
                    if (!TypeManager::makeCompatibles(&cxt, myGenParam->typeInfo, otherGenParam->typeInfo, nullptr, nullptr, CASTFLAG_NO_ERROR | CASTFLAG_JUST_CHECK | CASTFLAG_COMMUTATIVE))
                        return false;
            }
            else if (!myGenParam->typeInfo->isSame(otherGenParam->typeInfo, isSameFlags))
            {
                return false;
            }
        }
    }

    // Compare field by field
    if (!(isSameFlags & ISSAME_CAST) && !isTuple)
    {
        if ((flags & TYPEINFO_GENERIC) != (other->flags & TYPEINFO_GENERIC))
            return false;
        if (scope != other->scope)
            return false;
        int childCount = (int) fields.size();
        if (childCount != other->fields.size())
            return false;

        for (int i = 0; i < childCount; i++)
        {
            if (!fields[i]->isSame(other->fields[i], isSameFlags))
                return false;
        }
    }
    else if (isTuple && !sameName)
    {
        if (!(flags & TYPEINFO_GENERATED_TUPLE) && !(other->flags & TYPEINFO_GENERATED_TUPLE))
        {
            if ((flags & TYPEINFO_GENERIC) != (other->flags & TYPEINFO_GENERIC))
                return false;
        }

        int childCount = (int) fields.size();
        if (childCount != other->fields.size())
            return false;

        // Two tuple types will match if their content is equal, including specific user names
        // {x: s32} := {y: s32}
        for (int i = 0; i < childCount; i++)
        {
            if (!fields[i]->isSame(other->fields[i], isSameFlags))
                return false;

            // But this is ok to affect one tuple to another even if they do not have the same fields names
            if (!(isSameFlags & ISSAME_FOR_AFFECT))
            {
                if (fields[i]->namedParam != other->fields[i]->namedParam)
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
    return canRawCopy() && !opDrop & !opUserDropFct;
}

const char* TypeInfoStruct::getDisplayNameC()
{
    auto res = getDisplayName();
    return _strdup(res.c_str()); // Leak and slow, but only for messages
}

Utf8 TypeInfoStruct::getDisplayName()
{
    if (flags & TYPEINFO_STRUCT_IS_TUPLE)
        return "tuple";
    if (declNode && declNode->kind == AstNodeKind::InterfaceDecl)
        return Fmt("interface %s", name.c_str());
    if (declNode && declNode->kind == AstNodeKind::StructDecl && ((AstStruct*) declNode)->structFlags & STRUCTFLAG_UNION)
        return Fmt("union %s", name.c_str());

    Utf8 str;
    computeWhateverName(str, COMPUTE_DISPLAY_NAME);
    return Fmt("struct %s", str.c_str());
}

void TypeInfoStruct::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    // For a tuple, we use the tuple syntax when this is an export
    if (((nameType == COMPUTE_SCOPED_NAME_EXPORT) || nameType == COMPUTE_DISPLAY_NAME) && (flags & TYPEINFO_STRUCT_IS_TUPLE))
    {
        resName += "{";
        for (int i = 0; i < fields.size(); i++)
        {
            auto p = fields[i];
            if (i)
                resName += ", ";
            if (!p->namedParam.empty() && !(p->flags & TYPEINFO_AUTO_NAME))
            {
                resName += p->namedParam;
                resName += ": ";
            }

            resName += p->typeInfo->computeWhateverName(nameType);
        }

        resName += "}";
        return;
    }

    if (nameType != COMPUTE_NAME && nameType != COMPUTE_DISPLAY_NAME)
        getScopedName(resName);

    if (!(flags & TYPEINFO_STRUCT_IS_TUPLE) || nameType != COMPUTE_DISPLAY_NAME)
        resName += structName;

    computeNameGenericParameters(genericParameters, resName, nameType);
}
