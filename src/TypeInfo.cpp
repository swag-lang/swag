#include "pch.h"
#include "TypeInfo.h"
#include "AstNode.h"
#include "TypeManager.h"
#include "Mutex.h"
#include "RaceCondition.h"

void TypeInfo::clearName()
{
    ScopedLock lk(mutex);
    SWAG_RACE_CONDITION_WRITE(raceName);
    name.clear();
    displayName.clear();
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
    ScopedLock lk1(name.mutex);

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

const char* TypeInfo::getDisplayNameC()
{
    auto res = getDisplayName();
    return _strdup(res.c_str()); // Leak and slow, but only for messages
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
        if (name.empty())
        {
            computeWhateverName(str, nameType);
            SWAG_RACE_CONDITION_WRITE(raceName);
            name = move(str);
        }

        return name;

    case COMPUTE_DISPLAY_NAME:
        if (displayName.empty())
        {
            computeWhateverName(str, nameType);
            SWAG_RACE_CONDITION_WRITE(raceName);
            displayName = move(str);
        }

        return displayName;

    case COMPUTE_SCOPED_NAME:
        if (scopedName.empty())
        {
            computeWhateverName(str, nameType);
            SWAG_RACE_CONDITION_WRITE(raceName);
            scopedName = move(str);
        }

        return scopedName;

    case COMPUTE_SCOPED_NAME_EXPORT:
        if (scopedNameExport.empty())
        {
            computeWhateverName(str, nameType);
            SWAG_RACE_CONDITION_WRITE(raceName);
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
    case TypeInfoKind::FuncAttr:
        return "a function";
    case TypeInfoKind::Lambda:
        if (typeInfo->isClosure())
            return "a closure";
        return "a lambda";
    case TypeInfoKind::TypeListTuple:
        return "a tuple";
    case TypeInfoKind::TypeListArray:
        return "an array";
    case TypeInfoKind::Variadic:
    case TypeInfoKind::TypedVariadic:
    case TypeInfoKind::CVariadic:
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
    case TypeInfoKind::FuncAttr:
        return "function";
    case TypeInfoKind::Lambda:
        if (typeInfo->isClosure())
            return "closure";
        return "lambda";
    case TypeInfoKind::TypeListTuple:
        return "tuple";
    case TypeInfoKind::TypeListArray:
        return "array";
    case TypeInfoKind::Variadic:
    case TypeInfoKind::TypedVariadic:
    case TypeInfoKind::CVariadic:
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

TypeInfoStruct* TypeInfo::getStructOrPointedStruct()
{
    if (kind == TypeInfoKind::Struct)
        return CastTypeInfo<TypeInfoStruct>(this, TypeInfoKind::Struct);

    if (kind == TypeInfoKind::Pointer)
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
        if (typePointer->pointedType->kind == TypeInfoKind::Struct)
            CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
    }

    return nullptr;
}

bool TypeInfo::isPointerTo(NativeTypeKind pointerKind)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = (TypeInfoPointer*) this;
    if (!ptr->pointedType)
        return false;
    auto c = TypeManager::concreteType(ptr->pointedType, CONCRETE_ALIAS);
    if (!c->isNative(pointerKind))
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

bool TypeInfo::isCString()
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    return flags & TYPEINFO_C_STRING;
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

bool TypeInfo::isArrayOfEnum()
{
    if (kind != TypeInfoKind::Array)
        return false;
    auto ptr = (TypeInfoArray*) this;
    return ptr->finalType->kind == TypeInfoKind::Enum;
}

bool TypeInfo::isClosure()
{
    return flags & TYPEINFO_CLOSURE;
}

bool TypeInfo::isLambda()
{
    return (kind == TypeInfoKind::Lambda) && !(isClosure());
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
        if ((isClosure()) != (from->isClosure()))
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
    name         = from->name;
    declNode     = from->declNode;
    kind         = from->kind;
    nativeType   = from->nativeType;
    flags        = from->flags & ~TYPEINFO_SHARED;
    sizeOf       = from->sizeOf;
    promotedFrom = from->promotedFrom;
}

void TypeInfo::removeGenericFlag()
{
    if (flags & TYPEINFO_GENERIC)
    {
        flags &= ~TYPEINFO_GENERIC;
        flags |= TYPEINFO_FROM_GENERIC;
    }
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
    newType->attributes = attributes;
    newType->typeInfo   = typeInfo;
    newType->declNode   = declNode;
    newType->index      = index;
    newType->offset     = offset;
    newType->flags      = flags;

    if (value)
    {
        newType->allocateComputedValue();
        *newType->value = *value;
    }

    return newType;
}

bool TypeInfoParam::isSame(TypeInfoParam* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    return typeInfo->isSame(to->typeInfo, isSameFlags);
}

void TypeInfoParam::allocateComputedValue()
{
    if (value)
        return;
    value = g_Allocator.alloc<ComputedValue>();
}