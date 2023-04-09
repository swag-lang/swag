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
    auto ptr   = (TypeInfoPointer*) this;
    auto unref = TypeManager::concreteType(ptr->pointedType, CONCRETE_ALIAS);
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

TypeInfo* TypeInfo::getFakeAlias()
{
    if (!(flags & TYPEINFO_FAKE_ALIAS))
        return this;
    return ((TypeInfoAlias*) this)->rawType;
}

TypeInfoStruct* TypeInfo::getStructOrPointedStruct()
{
    if (kind == TypeInfoKind::Struct)
        return CastTypeInfo<TypeInfoStruct>(this, TypeInfoKind::Struct);

    if (kind == TypeInfoKind::Pointer)
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
        if (typePointer->pointedType->isStruct())
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
    auto ptr = (TypeInfoArray*) this;
    return ptr->finalType->isStruct();
}

bool TypeInfo::isArrayOfEnum()
{
    if (kind != TypeInfoKind::Array)
        return false;
    auto ptr = (TypeInfoArray*) this;
    return ptr->finalType->isEnum();
}

bool TypeInfo::isMethod()
{
    if (kind != TypeInfoKind::FuncAttr)
        return false;
    auto ptr = (TypeInfoFuncAttr*) this;
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

bool TypeInfo::isSame(TypeInfo* from, uint32_t castFlags)
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
    flags        = from->flags;
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

bool TypeInfoParam::isSame(TypeInfoParam* to, uint32_t castFlags)
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