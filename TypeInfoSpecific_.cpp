#include "pch.h"
#include "TypeManager.h"
#include "Allocator.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "Ast.h"
#include "Module.h"
#include "Generic.h"

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

void TypeInfoReference::computePreName(Utf8& preName)
{
    preName.clear();
    if (flags & TYPEINFO_CONST)
        preName = "const ";
    preName += "&";
}

void TypeInfoReference::computeScopedName()
{
    unique_lock lk(mutex);
    if (!scopedName.empty())
        return;

    computePreName(scopedName);
    pointedType->computeScopedName();
    scopedName += pointedType->scopedName;
}

void TypeInfoReference::computeScopedNameExport()
{
    unique_lock lk(mutex);
    if (!scopedNameExport.empty())
        return;

    computePreName(scopedNameExport);
    pointedType->computeScopedNameExport();
    scopedNameExport += pointedType->scopedNameExport;
}

void TypeInfoReference::computeName()
{
    unique_lock lk(mutex);
    if (!name.empty())
        return;

    computePreName(name);
    pointedType->computeName();
    name += pointedType->name;
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
    newType->finalType   = finalType;
    newType->pointedType = pointedType;
    newType->ptrCount    = ptrCount;
    newType->copyFrom(this);
    return newType;
}

void TypeInfoPointer::computePointedType()
{
    if (ptrCount == 1)
    {
        pointedType = finalType;
    }
    else
    {
        auto result = (TypeInfoPointer*) clone();
        result->ptrCount--;
        result->forceComputeName();
        result->computePointedType();
        pointedType = result;
    }
}

void TypeInfoPointer::computePreName(Utf8& preName)
{
    preName.clear();
    if (flags & TYPEINFO_CONST)
        preName = "const ";
    for (uint32_t i = 0; i < ptrCount; i++)
        preName += "*";
}

void TypeInfoPointer::computeScopedName()
{
    unique_lock lk(mutex);
    if (!scopedName.empty())
        return;

    if (!finalType) // "null"
        scopedName = name;
    else
    {
        computePreName(scopedName);
        finalType->computeScopedName();
        scopedName += finalType->scopedName;
    }
}

void TypeInfoPointer::computeScopedNameExport()
{
    unique_lock lk(mutex);
    if (!scopedNameExport.empty())
        return;

    if (!finalType) // "null"
        scopedNameExport = name;
    else
    {
        computePreName(scopedNameExport);
        finalType->computeScopedNameExport();
        scopedNameExport += finalType->scopedNameExport;
    }
}

void TypeInfoPointer::computeName()
{
    unique_lock lk(mutex);
    if (!name.empty())
        return;

    computePreName(name);
    finalType->computeName();
    name += finalType->name;
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
    if (ptrCount != other->ptrCount)
        return false;

    // Anonymous pointers
    if ((isSameFlags & ISSAME_CAST) && other->finalType == g_TypeMgr.typeInfoVoid)
        return true;

    return finalType->isSame(other->finalType, isSameFlags);
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

void TypeInfoArray::computePreName(Utf8& preName)
{
    preName.clear();
    if (flags & TYPEINFO_CONST)
        preName = "const ";

    if (count == UINT32_MAX)
    {
        preName += "[] ";
    }
    else
    {
        preName += format("[%d", count);
        auto pType = pointedType;
        while (pType->kind == TypeInfoKind::Array)
        {
            auto subType = CastTypeInfo<TypeInfoArray>(pType, TypeInfoKind::Array);
            preName += format(",%d", subType->count);
            pType = subType->pointedType;
        }

        preName += "] ";
    }
}

void TypeInfoArray::computeScopedName()
{
    unique_lock lk(mutex);
    if (!scopedName.empty())
        return;

    computePreName(scopedName);
    pointedType->computeScopedName();
    scopedName += pointedType->scopedName;
}

void TypeInfoArray::computeScopedNameExport()
{
    unique_lock lk(mutex);
    if (!scopedNameExport.empty())
        return;

    computePreName(scopedNameExport);
    pointedType->computeScopedNameExport();
    scopedNameExport += pointedType->scopedNameExport;
}

void TypeInfoArray::computeName()
{
    unique_lock lk(mutex);
    if (!name.empty())
        return;

    computePreName(name);
    pointedType->computeName();
    name += pointedType->name;
}

void TypeInfoSlice::computePreName(Utf8& preName)
{
    preName.clear();
    if (flags & TYPEINFO_CONST)
        preName = "const ";
    preName += "[..] ";
}

void TypeInfoSlice::computeName()
{
    unique_lock lk(mutex);
    if (!name.empty())
        return;

    computePreName(name);
    pointedType->computeName();
    name += pointedType->name;
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
        {
            structName += typeParam->namedParam;
            structName += "_";
        }

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

void TypeInfoVariadic::computeName()
{
    unique_lock lk(mutex);
    if (!name.empty())
        return;

    if (rawType)
    {
        rawType->computeName();
        name += rawType->name;
    }

    name += "...";
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

void TypeInfoFuncAttr::computeName()
{
    unique_lock lk(mutex);
    if (!name.empty())
        return;

    name.clear();
    if (genericParameters.size())
    {
        name += "'(";
        for (int i = 0; i < genericParameters.size(); i++)
        {
            if (i)
                name += ", ";
            name += genericParameters[i]->typeInfo ? genericParameters[i]->typeInfo->name : genericParameters[i]->name;
        }

        name += ")";
    }

    name += "(";
    for (int i = 0; i < parameters.size(); i++)
    {
        if (i)
            name += ", ";
        name += parameters[i]->typeInfo->name;
    }

    name += ")";
    if (returnType && !returnType->isNative(NativeTypeKind::Void))
        name += format("->%s", returnType->name.c_str());
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
            auto myFunc    = CastAst<AstFuncDecl>(declNode, AstNodeKind::FuncDecl);
            auto otherFunc = CastAst<AstFuncDecl>(other->declNode, AstNodeKind::FuncDecl);
            if (myFunc->replaceTypes.size() != otherFunc->replaceTypes.size())
                return false;
            for (auto r : myFunc->replaceTypes)
            {
                auto it = otherFunc->replaceTypes.find(r.first);
                if (it == otherFunc->replaceTypes.end())
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

TypeInfo* TypeInfoStruct::clone()
{
    auto newType               = allocType<TypeInfoStruct>();
    newType->scope             = scope;
    newType->opInit            = opInit;
    newType->opUserPostCopyFct = opUserPostCopyFct;
    newType->opPostCopy        = opPostCopy;
    newType->opUserPostMoveFct = opUserPostMoveFct;
    newType->opPostMove        = opPostMove;
    newType->opUserDropFct     = opUserDropFct;
    newType->opDrop            = opDrop;
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
    bool sameName = structName == other->structName;
    if (!isTuple && !sameName)
        return false;

    // Compare generic parameters
    auto numGenParams = genericParameters.size();
    if (numGenParams != other->genericParameters.size())
        return false;
    for (int i = 0; i < numGenParams; i++)
    {
        if (isSameFlags & ISSAME_CAST)
        {
            if (other->genericParameters[i]->typeInfo->kind == TypeInfoKind::Generic)
                continue;
        }

        if (!genericParameters[i]->typeInfo->isSame(other->genericParameters[i]->typeInfo, isSameFlags))
            return false;
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

void TypeInfoStruct::computeScopedName()
{
    unique_lock lk(mutex);
    if (!scopedName.empty())
        return;

    getScopedName(scopedName, false);
    scopedName += structName;

    if (!genericParameters.empty())
    {
        scopedName += "'(";
        for (int i = 0; i < genericParameters.size(); i++)
        {
            if (i)
                scopedName += ", ";
            genericParameters[i]->typeInfo->computeScopedName();
            scopedName += genericParameters[i]->typeInfo->scopedName;
        }

        scopedName += ")";
    }
}

void TypeInfoStruct::computeScopedNameExport()
{
    unique_lock lk(mutex);
    if (!scopedNameExport.empty())
        return;

    // Exporting a tuple. Need to use the tuple syntax
    if (flags & TYPEINFO_STRUCT_IS_TUPLE)
    {
        scopedNameExport += "{";
        for (int i = 0; i < fields.size(); i++)
        {
            auto p = fields[i];
            if (i)
                scopedNameExport += ", ";
            p->typeInfo->computeScopedNameExport();
            if (!p->namedParam.empty() && !(p->flags & TYPEINFO_AUTO_NAME))
            {
                scopedNameExport += p->namedParam;
                scopedNameExport += ": ";
            }

            scopedNameExport += p->typeInfo->scopedNameExport;
        }

        scopedNameExport += "}";
        return;
    }

    getScopedName(scopedNameExport, true);
    scopedNameExport += structName;

    if (!genericParameters.empty())
    {
        scopedNameExport += "'";
        if (genericParameters.size() > 1)
            scopedNameExport += "(";
        for (int i = 0; i < genericParameters.size(); i++)
        {
            if (i)
                scopedNameExport += ", ";
            genericParameters[i]->typeInfo->computeScopedNameExport();
            scopedNameExport += genericParameters[i]->typeInfo->scopedNameExport;
        }

        if (genericParameters.size() > 1)
            scopedNameExport += ")";
    }
}

void TypeInfoStruct::computeName()
{
    unique_lock lk(mutex);
    if (!name.empty())
        return;

    name = structName;
    if (genericParameters.size() > 0)
    {
        name += "'(";
        for (int i = 0; i < genericParameters.size(); i++)
        {
            if (i)
                name += ", ";
            name += genericParameters[i]->typeInfo ? genericParameters[i]->typeInfo->name : genericParameters[i]->name;
        }

        name += ")";
    }
}