#include "pch.h"
#include "TypeInfo.h"
#include "AstNode.h"
#include "TypeManager.h"

void TypeInfo::clearName()
{
    scoped_lock lk(mutex);
    name.clear();
    displayName.clear();
}

void TypeInfo::forceComputeName()
{
    clearName();
    computeWhateverName(COMPUTE_NAME);
    computeWhateverName(COMPUTE_DISPLAY_NAME);
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
    scoped_lock lk(mutex);
    computeWhateverNameNoLock(COMPUTE_NAME);
    SWAG_ASSERT(!name.empty());
    return name;
}

Utf8 TypeInfo::getDisplayName()
{
    return displayName.empty() ? name : displayName;
}

const Utf8& TypeInfo::computeWhateverName(uint32_t nameType)
{
    scoped_lock lk(mutex);
    return computeWhateverNameNoLock(nameType);
}

const Utf8& TypeInfo::computeWhateverNameNoLock(uint32_t nameType)
{
    Utf8 str;
    switch (nameType)
    {
    case COMPUTE_NAME:
        if (name.empty())
        {
            computeWhateverName(str, nameType);
            name = move(str);
        }

        return name;

    case COMPUTE_DISPLAY_NAME:
        if (displayName.empty())
        {
            computeWhateverName(str, nameType);
            displayName = move(str);
        }

        return displayName;

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
        if (kind == TypeInfoKind::Native && nativeType == NativeTypeKind::String && relative)
        {
            resName = "string";
            resName += format("~%u", relative * 8);
        }
        else
            resName = name;
        break;

    case COMPUTE_DISPLAY_NAME:
        if (displayName.empty())
            displayName = name;
        resName = displayName;
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
