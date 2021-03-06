#include "pch.h"
#include "TypeInfo.h"
#include "AstNode.h"
#include "Scope.h"
#include "Module.h"
#include "TypeManager.h"

void TypeInfo::computeName()
{
}

void TypeInfo::forceComputeName()
{
    name.clear();
    computeName();
}

void TypeInfo::getScopedName(Utf8& newName)
{
    if (declNode && declNode->ownerScope)
    {
        if (declNode->ownerScope->kind != ScopeKind::Function)
        {
            newName += declNode->ownerScope->getFullName();
            if (!newName.empty())
                newName += ".";
        }
    }
}

const Utf8& TypeInfo::computeName(int mode)
{
    switch (mode)
    {
    case COMPUTE_NAME_FLAT:
        computeName();
        return name;
    case COMPUTE_NAME_SCOPED:
        computeScopedName();
        return scopedName;
    default:
        SWAG_ASSERT(false);
        return name;
    }
}

void TypeInfo::computeScopedName()
{
    scoped_lock lk(mutex);
    if (!scopedName.empty())
        return;
    getScopedName(scopedName);
    scopedName += name;
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
    case TypeInfoKind::TypeSet:
        return "a typeset";
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
    case TypeInfoKind::TypeSet:
        return "typeset";
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

bool TypeInfo::isArrayOfRelative()
{
    if (kind != TypeInfoKind::Array)
        return false;
    auto ptr = (TypeInfoArray*) this;
    return ptr->finalType->isRelative();
}
