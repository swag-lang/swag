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
    }

    if (to->kind == TypeInfoKind::Alias)
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

TypeInfo* TypeInfoNameAlias::clone()
{
    auto newType = allocType<TypeInfoNameAlias>();
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoNameAlias::isSame(TypeInfo* to, uint32_t isSameFlags)
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

bool TypeInfoAlias::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    if (to->kind == TypeInfoKind::Alias)
    {
        auto other = static_cast<TypeInfoAlias*>(to);
        return rawType->isSame(other->rawType, isSameFlags);
    }

    SWAG_ASSERT(flags & TYPEINFO_FAKE_ALIAS);
    return rawType->isSame(to, isSameFlags);
}

TypeInfo* TypeInfoParam::clone()
{
    auto newType        = allocType<TypeInfoParam>();
    newType->namedParam = namedParam;
    newType->typeInfo   = typeInfo;
    newType->index      = index;
    newType->offset     = offset;
    newType->attributes = attributes;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoParam::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto other = static_cast<TypeInfoParam*>(to);
    return typeInfo->isSame(other->typeInfo, isSameFlags);
}

TypeInfo* TypeInfoReference::clone()
{
    auto newType          = allocType<TypeInfoReference>();
    newType->pointedType  = pointedType;
    newType->originalType = originalType;
    newType->copyFrom(this);
    return newType;
}

void TypeInfoReference::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    if (flags & TYPEINFO_CONST)
        resName += "const ";
    resName += "&";
    resName += pointedType->computeWhateverName(nameType);
}

bool TypeInfoReference::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    auto other = static_cast<TypeInfoReference*>(to);
    return pointedType->isSame(other->pointedType, isSameFlags);
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
    if (flags & TYPEINFO_CONST)
        resName += "const ";
    resName += "*";
    if (relative)
        resName += format("~%u ", relative);

    if (!pointedType) // "null"
        return;

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
    }

    if (isSameFlags & ISSAME_CAST)
    {
        if (this == g_TypeMgr.typeInfoNull && to->kind == TypeInfoKind::Lambda)
            return true;
    }

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    if (isSameFlags & ISSAME_CAST)
    {
        if (this == g_TypeMgr.typeInfoNull)
            return true;
        if (to == g_TypeMgr.typeInfoNull)
            return true;
    }

    auto other = static_cast<TypeInfoPointer*>(to);

    // Anonymous pointers
    if ((isSameFlags & ISSAME_CAST) && other->pointedType == g_TypeMgr.typeInfoVoid)
        return true;

    return pointedType->isSame(other->pointedType, isSameFlags);
}

TypeInfo* TypeInfoArray::clone()
{
    auto newType         = allocType<TypeInfoArray>();
    newType->pointedType = pointedType;
    newType->finalType   = finalType;
    newType->count       = count;
    newType->totalCount  = totalCount;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoArray::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    auto other = static_cast<TypeInfoArray*>(to);
    if (count != other->count)
        return false;
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
        resName += format("[%d", count);
        auto pType = pointedType;
        while (pType->kind == TypeInfoKind::Array)
        {
            auto subType = CastTypeInfo<TypeInfoArray>(pType, TypeInfoKind::Array);
            resName += format(",%d", subType->count);
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
    resName += "[..]";
    if (relative)
        resName += format("~%u", relative);
    resName += " ";

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

Utf8 TypeInfoList::computeTupleName(JobContext* context)
{
    Utf8 structName = context->sourceFile->scopePrivate->name + "_tuple_";

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
    auto newType = allocType<TypeInfoList>();

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
        if (to->kind == TypeInfoKind::Struct && kind == TypeInfoKind::TypeListTuple)
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
    if (to->kind == kind)
        return name == to->name;
    if (isSameFlags & ISSAME_EXACT)
        return name == to->name;
    return true;
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
    auto newType                  = allocType<TypeInfoFuncAttr>();
    newType->firstDefaultValueIdx = firstDefaultValueIdx;
    newType->returnType           = returnType;
    newType->stackSize            = stackSize;
    newType->attributes           = attributes;
    newType->attributeUsage       = attributeUsage;

    for (int i = 0; i < genericParameters.size(); i++)
    {
        auto param = static_cast<TypeInfoParam*>(genericParameters[i]->clone());
        newType->genericParameters.push_back(param);
    }

    for (int i = 0; i < parameters.size(); i++)
    {
        auto param = static_cast<TypeInfoParam*>(parameters[i]->clone());
        newType->parameters.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

static void computeNameGenericParameters(VectorNative<TypeInfoParam*>& genericParameters, Utf8& resName, uint32_t nameType)
{
    if (genericParameters.empty())
        return;

    resName += "'(";
    for (int i = 0; i < genericParameters.size(); i++)
    {
        if (i)
            resName += ", ";

        auto genParam = genericParameters[i];
        if (genParam->flags & TYPEINFO_DEFINED_VALUE)
        {
            SWAG_ASSERT(genParam->typeInfo);
            auto str = Ast::literalToString(genParam->typeInfo, genParam->value.text, genParam->value.reg);
            resName += str;
        }
        else if (genParam->typeInfo)
            resName += genParam->typeInfo->computeWhateverName(nameType);
        else
            resName += genParam->name;
    }

    resName += ")";
}

void TypeInfoFuncAttr::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    if (nameType != COMPUTE_NAME)
    {
        getScopedName(resName);
        SWAG_ASSERT(declNode);
        resName += declNode->token.text;
    }

    computeNameGenericParameters(genericParameters, resName, nameType);

    // Parameters
    resName += "(";
    for (int i = 0; i < parameters.size(); i++)
    {
        if (i)
            resName += ", ";
        resName += parameters[i]->typeInfo->computeWhateverName(nameType);
    }

    resName += ")";

    // Return type
    if (returnType && !returnType->isNative(NativeTypeKind::Void))
    {
        resName += "->";
        resName += returnType->computeWhateverName(nameType);
    }
    else
    {
        resName += "->void";
    }

    if (flags & TYPEINFO_CAN_THROW)
        resName += " throw";
}

bool TypeInfoFuncAttr::isSame(TypeInfoFuncAttr* other, uint32_t isSameFlags)
{
    if (parameters.size() != other->parameters.size())
        return false;

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
        if (!returnType && other->returnType && !other->returnType->isNative(NativeTypeKind::Void))
            return false;
        if (returnType && !returnType->isNative(NativeTypeKind::Void) && !other->returnType)
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

    for (int i = 0; i < parameters.size(); i++)
    {
        if (parameters[i]->typeInfo->isNative(NativeTypeKind::Undefined))
            continue;
        if (other->parameters[i]->typeInfo->isNative(NativeTypeKind::Undefined))
            continue;
        auto type1 = TypeManager::concreteReference(parameters[i]->typeInfo);
        auto type2 = TypeManager::concreteReference(other->parameters[i]->typeInfo);
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
        if (kind == TypeInfoKind::Lambda && to == g_TypeMgr.typeInfoNull)
            return true;
    }

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    auto other = static_cast<TypeInfoFuncAttr*>(to);
    SWAG_ASSERT(to->kind == TypeInfoKind::FuncAttr || to->kind == TypeInfoKind::Lambda);
    if (!isSame(other, isSameFlags))
        return false;

    if ((isSameFlags & ISSAME_EXACT) || (to->kind == TypeInfoKind::Lambda))
    {
        if (returnType && returnType != g_TypeMgr.typeInfoVoid && !other->returnType)
            return false;
        if (!returnType && other->returnType && other->returnType != g_TypeMgr.typeInfoVoid)
            return false;
        if (returnType && other->returnType && !returnType->isNative(NativeTypeKind::Undefined) && !returnType->isSame(other->returnType, isSameFlags))
            return false;
    }

    return true;
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

        auto typeParam = TypeManager::concreteReferenceType(parameters[argNo]->typeInfo);
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
    return TypeManager::concreteReferenceType(parameters[argNo]->typeInfo);
}

TypeInfo* TypeInfoStruct::clone()
{
    auto newType               = allocType<TypeInfoStruct>();
    newType->scope             = scope;
    newType->opInit            = opInit;
    newType->opUserInitFct     = opUserInitFct;
    newType->opDrop            = opDrop;
    newType->opUserDropFct     = opUserDropFct;
    newType->opReloc           = opReloc;
    newType->opUserRelocFct    = opUserRelocFct;
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
        auto param = static_cast<TypeInfoParam*>(genericParameters[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->genericParameters.push_back(param);
    }

    size = (int) fields.size();
    newType->fields.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = static_cast<TypeInfoParam*>(fields[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->fields.push_back(param);
    }

    size = (int) consts.size();
    newType->consts.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = static_cast<TypeInfoParam*>(consts[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->consts.push_back(param);
    }

    size = (int) methods.size();
    newType->methods.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = static_cast<TypeInfoParam*>(methods[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
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
        if (kind == TypeInfoKind::Interface && to->kind == TypeInfoKind::Struct)
        {
            auto other = CastTypeInfo<TypeInfoStruct>(to, to->kind);
            if (other->hasInterface(this))
                return true;
        }
    }

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    if ((flags & TYPEINFO_STRUCT_IS_TUPLE) != (to->flags & TYPEINFO_STRUCT_IS_TUPLE))
        return false;

    auto other = CastTypeInfo<TypeInfoStruct>(to, to->kind);

    int childCount = (int) fields.size();
    if (childCount != other->fields.size())
        return false;

    // Do not compare names for tuples
    bool isTuple  = flags & TYPEINFO_STRUCT_IS_TUPLE;
    bool sameName = declNode->ownerScope == to->declNode->ownerScope && structName == other->structName;
    if (!isTuple && !sameName)
        return false;

    // Compare generic parameters
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
                return false;
        }
        else if (!myGenParam->typeInfo->isSame(otherGenParam->typeInfo, isSameFlags))
        {
            return false;
        }
    }

    // Compare field by field
    if (!(isSameFlags & ISSAME_CAST) && !isTuple)
    {
        if ((flags & TYPEINFO_GENERIC) != (other->flags & TYPEINFO_GENERIC))
            return false;
        if (scope != other->scope)
            return false;
        for (int i = 0; i < childCount; i++)
        {
            if (!fields[i]->isSame(other->fields[i], isSameFlags))
                return false;
        }
    }
    else if (isTuple && !sameName)
    {
        if ((flags & TYPEINFO_GENERIC) != (other->flags & TYPEINFO_GENERIC))
            return false;
        for (int i = 0; i < childCount; i++)
        {
            if (!fields[i]->isSame(other->fields[i], isSameFlags | ISSAME_EXACT))
                return false;
        }
    }

    return true;
}

bool TypeInfoStruct::canRawCopy()
{
    return !opPostCopy && !opPostMove && !opReloc;
}

Utf8 TypeInfoStruct::getDisplayName()
{
    if (flags & TYPEINFO_STRUCT_IS_TUPLE)
        return "tuple";
    if (declNode && declNode->kind == AstNodeKind::InterfaceDecl)
        return format("interface %s", name.c_str());
    if (declNode && declNode->kind == AstNodeKind::TypeSet)
        return format("typeset %s", name.c_str());
    if (declNode && declNode->kind == AstNodeKind::StructDecl && ((AstStruct*) declNode)->structFlags & STRUCTFLAG_UNION)
        return format("union %s", name.c_str());
    return format("struct %s", name.c_str());
}

void TypeInfoStruct::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    // For a tuple, we use the tuple syntax when this is an export
    if ((nameType == COMPUTE_SCOPED_NAME_EXPORT) && (flags & TYPEINFO_STRUCT_IS_TUPLE))
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

    if (nameType != COMPUTE_NAME)
        getScopedName(resName);
    resName += structName;

    computeNameGenericParameters(genericParameters, resName, nameType);
}
