#include "pch.h"
#include "TypeInfo.h"
#include "AstNode.h"
#include "TypeManager.h"
#include "Scope.h"

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
    computeWhateverNameNoLock(COMPUTE_NAME);
    SWAG_ASSERT(!name.empty());
    return name;
}

Utf8 TypeInfo::getTypeName(bool forceNoScope)
{
    SharedLock lk(mutex);
    if (forceNoScope)
        return name;
    return scopedName;
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
            name = std::move(str);
        }

        return name;

    case COMPUTE_DISPLAY_NAME:
        if (displayName.empty())
        {
            computeWhateverName(str, nameType);
            SWAG_RACE_CONDITION_WRITE(raceName);
            displayName = std::move(str);
        }

        return displayName;

    case COMPUTE_SCOPED_NAME:
        if (scopedName.empty())
        {
            computeWhateverName(str, nameType);
            SWAG_RACE_CONDITION_WRITE(raceName);
            scopedName = std::move(str);
        }

        return scopedName;

    case COMPUTE_SCOPED_NAME_EXPORT:
        if (scopedNameExport.empty())
        {
            computeWhateverName(str, nameType);
            SWAG_RACE_CONDITION_WRITE(raceName);
            scopedNameExport = std::move(str);
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

bool TypeInfo::isPointerVoid()
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr   = CastTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    auto unref = ptr->pointedType->getConcreteAlias();
    if (!unref->isNative())
        return false;
    if (unref->nativeType != NativeTypeKind::Void)
        return false;
    return true;
}

bool TypeInfo::isPointerConstVoid()
{
    return isPointerVoid() && isConst();
}

TypeInfo* TypeInfo::getConstAlias()
{
    if (!(flags & TYPEINFO_CONST_ALIAS))
        return this;
    return ((TypeInfoAlias*) this)->rawType;
}

TypeInfo* TypeInfo::getConcreteAlias()
{
    return TypeManager::concreteType(this, CONCRETE_FORCEALIAS);
}

TypeInfo* TypeInfo::getFinalType()
{
    auto retType = this;
    while (retType)
    {
        if (retType->isPointer())
        {
            retType = CastTypeInfo<TypeInfoPointer>(retType, TypeInfoKind::Pointer)->pointedType;
            continue;
        }

        if (retType->isSlice())
        {
            retType = CastTypeInfo<TypeInfoSlice>(retType, TypeInfoKind::Slice)->pointedType;
            continue;
        }

        if (retType->isArray())
        {
            retType = CastTypeInfo<TypeInfoArray>(retType, TypeInfoKind::Array)->finalType;
            continue;
        }

        return retType;
    }

    return this;
}

TypeInfoStruct* TypeInfo::getStructOrPointedStruct()
{
    auto self = getConcreteAlias();
    if (self->kind == TypeInfoKind::Struct)
        return CastTypeInfo<TypeInfoStruct>(self, TypeInfoKind::Struct);

    if (self->kind == TypeInfoKind::Pointer)
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(self, TypeInfoKind::Pointer);
        auto pointed     = typePointer->pointedType->getConcreteAlias();
        if (pointed->isStruct())
            return CastTypeInfo<TypeInfoStruct>(pointed, TypeInfoKind::Struct);
    }

    return nullptr;
}

bool TypeInfo::isPointerTo(NativeTypeKind pointerKind)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = CastTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    if (!ptr->pointedType)
        return false;
    return ptr->pointedType->getConcreteAlias()->isNative(pointerKind);
}

bool TypeInfo::isPointerTo(TypeInfoKind pointerKind)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = CastTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    if (!ptr->pointedType)
        return false;
    return ptr->pointedType->getConcreteAlias()->kind == pointerKind;
}

bool TypeInfo::isPointerTo(TypeInfo* finalType)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = CastTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    if (!ptr->pointedType)
        return false;
    return ptr->pointedType->getConcreteAlias() == finalType;
}

bool TypeInfo::isPointerToTypeInfo()
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = CastTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    if (!ptr->pointedType)
        return false;
    return ptr->pointedType->getConcreteAlias()->flags & TYPEINFO_STRUCT_TYPEINFO;
}

bool TypeInfo::isPointerNull()
{
    return this == g_TypeMgr->typeInfoNull;
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
    auto ptr = CastTypeInfo<TypeInfoArray>(this, TypeInfoKind::Array);
    return ptr->finalType->getConcreteAlias()->isStruct();
}

bool TypeInfo::isArrayOfEnum()
{
    if (kind != TypeInfoKind::Array)
        return false;
    auto ptr = CastTypeInfo<TypeInfoArray>(this, TypeInfoKind::Array);
    return ptr->finalType->getConcreteAlias()->isEnum();
}

bool TypeInfo::isMethod()
{
    if (kind != TypeInfoKind::FuncAttr)
        return false;
    auto ptr = CastTypeInfo<TypeInfoFuncAttr>(this, kind);
    if (ptr->parameters.size() == 0)
        return false;
    auto param = ptr->parameters[0];
    if (!param->typeInfo->isPointer())
        return false;
    if (!(param->typeInfo->isSelf()))
        return false;
    if (!(param->typeInfo->flags & TYPEINFO_HAS_USING))
        return false;
    return true;
}

bool TypeInfo::isSame(TypeInfo* from, uint64_t castFlags)
{
    if (this == from)
        return true;

    if (kind != from->kind)
        return false;

    if (castFlags & CASTFLAG_EXACT)
    {
        if ((flags & TYPEINFO_CONST) != (from->flags & TYPEINFO_CONST))
            return false;
        if ((flags & TYPEINFO_GENERIC) != (from->flags & TYPEINFO_GENERIC))
            return false;
        if ((flags & TYPEINFO_AUTO_CAST) != (from->flags & TYPEINFO_AUTO_CAST))
            return false;
        if ((flags & TYPEINFO_POINTER_MOVE_REF) != (from->flags & TYPEINFO_POINTER_MOVE_REF))
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
    flags      = from->flags;
    sizeOf     = from->sizeOf;
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

bool TypeInfoParam::isSame(TypeInfoParam* to, uint64_t castFlags)
{
    if (this == to)
        return true;
    return typeInfo->isSame(to->typeInfo, castFlags);
}

void TypeInfoParam::allocateComputedValue()
{
    if (value)
        return;
    value = Allocator::alloc<ComputedValue>();
}