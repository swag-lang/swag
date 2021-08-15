#include "pch.h"
#include "TypeInfo.h"
#include "AstNode.h"
#include "TypeManager.h"
#include "ScopedLock.h"

void TypeInfo::clearName()
{
    ScopedLock lk(mutex);
    name.clear();
}

void TypeInfo::forceComputeName()
{
    clearName();
    computeWhateverName(COMPUTE_NAME);
}

void TypeInfo::getScopedName(Utf8& newName)
{
    if (declNode && declNode->ownerScope && declNode->ownerScope->kind != ScopeKind::Function)
    {
        newName += declNode->ownerScope->getFullName();
        if (!newName.empty())
            newName += ".";
    }
}

Utf8 TypeInfo::getName()
{
    ScopedLock lk(mutex);
    computeWhateverNameNoLock(COMPUTE_NAME);
    SWAG_ASSERT(!name.empty());
    return name;
}

Utf8 TypeInfo::getDisplayName()
{
    Utf8 str;
    computeWhateverName(str, COMPUTE_DISPLAY_NAME);
    return str.empty() ? name : str;
}

const Utf8& TypeInfo::computeWhateverName(uint32_t nameType)
{
    ScopedLock lk(mutex);
    return computeWhateverNameNoLock(nameType);
}

const Utf8& TypeInfo::computeWhateverNameNoLock(uint32_t nameType)
{
    Utf8 str;
    switch (nameType)
    {
    case COMPUTE_NAME:
    case COMPUTE_DISPLAY_NAME:
        if (name.empty())
        {
            computeWhateverName(str, nameType);
            name = move(str);
        }

        return name;

    case COMPUTE_SCOPED_NAME:
        if (scopedName.empty())
        {
            computeWhateverName(str, nameType);
            scopedName = move(str);
        }

        return scopedName;

    case COMPUTE_SCOPED_NAME_EXPORT:
        if (scopedNameExport.empty())
        {
            computeWhateverName(str, nameType);
            scopedNameExport = move(str);
        }

        return scopedNameExport;
    }

    SWAG_ASSERT(false);
    return name;
}

void TypeInfo::computeWhateverName(Utf8& resName, uint32_t nameType)
{
    switch (nameType)
    {
    case COMPUTE_NAME:
    case COMPUTE_DISPLAY_NAME:
        resName = name;
        break;

    case COMPUTE_SCOPED_NAME:
    case COMPUTE_SCOPED_NAME_EXPORT:
        getScopedName(resName);
        resName += name;
        break;
    }
}

const char* TypeInfo::getArticleKindName(TypeInfo* typeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Namespace:
        return "a namespace";
    case TypeInfoKind::Enum:
        return "an enum";
    case TypeInfoKind::Array:
        return "an array";
    case TypeInfoKind::Slice:
        return "a slice";
    case TypeInfoKind::Pointer:
        return "a pointer";
    case TypeInfoKind::Reference:
        return "a reference";
    case TypeInfoKind::FuncAttr:
        return "a function";
    case TypeInfoKind::Lambda:
        return "a lambda";
    case TypeInfoKind::TypeListTuple:
        return "a tuple";
    case TypeInfoKind::TypeListArray:
        return "an array";
    case TypeInfoKind::Variadic:
    case TypeInfoKind::TypedVariadic:
        return "a variadic";
    case TypeInfoKind::Struct:
        return "a struct";
    case TypeInfoKind::Interface:
        return "an interface";
    case TypeInfoKind::Generic:
        return "a generic type";
    case TypeInfoKind::Alias:
        return "an alias type";
    case TypeInfoKind::Native:
        return "a type";
    }

    return "<type>";
}

const char* TypeInfo::getNakedKindName(TypeInfo* typeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Namespace:
        return "namespace";
    case TypeInfoKind::Enum:
        return "enum";
    case TypeInfoKind::Array:
        return "array";
    case TypeInfoKind::Slice:
        return "slice";
    case TypeInfoKind::Pointer:
        return "pointer";
    case TypeInfoKind::Reference:
        return "reference";
    case TypeInfoKind::FuncAttr:
        return "function";
    case TypeInfoKind::Lambda:
        return "lambda";
    case TypeInfoKind::TypeListTuple:
        return "tuple";
    case TypeInfoKind::TypeListArray:
        return "array";
    case TypeInfoKind::Variadic:
    case TypeInfoKind::TypedVariadic:
        return "variadic";
    case TypeInfoKind::Struct:
        return "struct";
    case TypeInfoKind::Interface:
        return "interface";
    case TypeInfoKind::Generic:
        return "generic type";
    case TypeInfoKind::Alias:
        return "alias";
    case TypeInfoKind::Native:
        return "type";
    }

    return "<type>";
}

bool TypeInfo::isPointerVoid()
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr   = (TypeInfoPointer*) this;
    auto unref = TypeManager::concreteType(ptr->pointedType, CONCRETE_ALIAS);
    if (unref->kind != TypeInfoKind::Native)
        return false;
    if (unref->nativeType != NativeTypeKind::Void)
        return false;
    return true;
}

bool TypeInfo::isPointerConstVoid()
{
    return isPointerVoid() && isConst();
}

bool TypeInfo::isPointerTo(NativeTypeKind pointerKind)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = (TypeInfoPointer*) this;
    if (!ptr->pointedType)
        return false;
    if (!ptr->pointedType->isNative(pointerKind))
        return false;
    return true;
}

bool TypeInfo::isPointerTo(TypeInfoKind pointerKind)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = (TypeInfoPointer*) this;
    if (!ptr->pointedType)
        return false;
    if (ptr->pointedType->kind != pointerKind)
        return false;
    return true;
}

bool TypeInfo::isPointerTo(TypeInfo* finalType)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = (TypeInfoPointer*) this;
    if (!ptr->pointedType)
        return false;
    if (ptr->pointedType != finalType)
        return false;
    return true;
}

bool TypeInfo::isPointerToTypeInfo()
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = (TypeInfoPointer*) this;
    if (!ptr->pointedType)
        return false;
    return ptr->pointedType->flags & TYPEINFO_STRUCT_TYPEINFO;
}

bool TypeInfo::isInitializerList()
{
    if (kind != TypeInfoKind::TypeListTuple)
        return false;
    return true;
}

bool TypeInfo::isArrayOfStruct()
{
    if (kind != TypeInfoKind::Array)
        return false;
    auto ptr = (TypeInfoArray*) this;
    return ptr->finalType->kind == TypeInfoKind::Struct;
}

bool TypeInfo::isMethod()
{
    if (kind != TypeInfoKind::FuncAttr)
        return false;
    auto ptr = (TypeInfoFuncAttr*) this;
    if (ptr->parameters.size() == 0)
        return false;
    auto param = ptr->parameters[0];
    if (param->typeInfo->kind != TypeInfoKind::Pointer)
        return false;
    if (!(param->typeInfo->flags & TYPEINFO_SELF))
        return false;
    if (!(param->typeInfo->flags & TYPEINFO_HAS_USING))
        return false;
    return true;
}

bool TypeInfo::isSame(TypeInfo* from, uint32_t isSameFlags)
{
    if (this == from)
        return true;

    if (kind != from->kind)
        return false;

    if (isSameFlags & ISSAME_EXACT)
    {
        if ((flags & TYPEINFO_CONST) != (from->flags & TYPEINFO_CONST))
            return false;
        if ((flags & TYPEINFO_GENERIC) != (from->flags & TYPEINFO_GENERIC))
            return false;
        if ((flags & TYPEINFO_AUTO_CAST) != (from->flags & TYPEINFO_AUTO_CAST))
            return false;
    }

    return true;
}

void TypeInfo::setConst()
{
    if (flags & TYPEINFO_CONST)
        return;
    flags |= TYPEINFO_CONST;
    name = "const " + name;
}

int TypeInfo::numRegisters()
{
    if (sizeOf == 0)
        return 0;
    if (flags & TYPEINFO_RETURN_BY_COPY)
        return 1;
    int result = max(sizeOf, sizeof(void*)) / sizeof(void*);
    SWAG_ASSERT(result <= 2);
    return result;
}

void TypeInfo::copyFrom(TypeInfo* from)
{
    name       = from->name;
    declNode   = from->declNode;
    kind       = from->kind;
    nativeType = from->nativeType;
    flags      = from->flags & ~TYPEINFO_SHARED;
    sizeOf     = from->sizeOf;
}

int TypeInfoParam::numRegisters()
{
    return typeInfo->numRegisters();
}

TypeInfoParam* TypeInfoParam::clone()
{
    auto newType        = g_TypeMgr->makeParam();
    newType->name       = name;
    newType->namedParam = namedParam;
    newType->value      = value;
    newType->attributes = attributes;
    newType->typeInfo   = typeInfo;
    newType->declNode   = declNode;
    newType->index      = index;
    newType->offset     = offset;
    newType->flags      = flags;
    return newType;
}

bool TypeInfoParam::isSame(TypeInfoParam* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    return typeInfo->isSame(to->typeInfo, isSameFlags);
}