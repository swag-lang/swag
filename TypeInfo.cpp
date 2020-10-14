#include "pch.h"
#include "TypeInfo.h"
#include "AstNode.h"

void TypeInfo::computeName()
{
}

void TypeInfo::forceComputeName()
{
    nakedName.clear();
    computeName();
}

void TypeInfo::computeScopedNameNoLock()
{
    if (!scopedName.empty())
        return;

    Utf8 newName;
    newName = preName;

    if (declNode && declNode->ownerScope)
    {
        if (declNode->ownerScope->kind != ScopeKind::Function)
        {
            newName += declNode->ownerScope->getFullName();
            if (!newName.empty())
                newName += ".";
        }
    }

    SWAG_ASSERT(!nakedName.empty());

    // Function types are scoped with the name, because two functions of the exact same type
    // (parameters and return value) should have a different concrete type info, because of attributes
    if (declNode && declNode->kind == AstNodeKind::FuncDecl)
        newName += declNode->name;

    newName += nakedName;
    scopedName = newName;
}

void TypeInfo::computeScopedName()
{
    {
        shared_lock lk(mutexScopeName);
        if (!scopedName.empty())
            return;
    }

    {
        scoped_lock lk(mutexScopeName);
        if (!scopedName.empty())
            return;
        computeScopedNameNoLock();
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

bool TypeInfo::isPointer1()
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = (TypeInfoPointer*) this;
    if (ptr->ptrCount != 1)
        return false;
    return true;
}

bool TypeInfo::isPointerTo(TypeInfoKind pointerKind)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = (TypeInfoPointer*) this;
    if (ptr->ptrCount != 1)
        return false;
    if (ptr->finalType->kind != pointerKind)
        return false;
    return true;
}

bool TypeInfo::isPointerToTypeInfo()
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = (TypeInfoPointer*) this;
    if (ptr->ptrCount != 1)
        return false;
    return ptr->finalType->flags & TYPEINFO_STRUCT_TYPEINFO;
}

bool TypeInfo::isInitializerList()
{
    if (kind != TypeInfoKind::TypeListTuple)
        return false;
    return true;
}