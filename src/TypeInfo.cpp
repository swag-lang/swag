#include "pch.h"
#include "TypeInfo.h"
#include "AstNode.h"
#include "Scope.h"
#include "TypeManager.h"

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

void TypeInfo::computeScopedName(Utf8& newName) const
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
    const auto res = getDisplayName();
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

    default:
        break;
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
        computeScopedName(resName);
        resName += name;
        break;

    default:
        break;
    }
}

bool TypeInfo::isPointerVoid()
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    const auto ptr   = castTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    const auto unref = ptr->pointedType->getConcreteAlias();
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
    if (!hasFlag(TYPEINFO_CONST_ALIAS))
        return this;
    return ((TypeInfoAlias*) this)->rawType;
}

const TypeInfo* TypeInfo::getConstAlias() const
{
    if (!hasFlag(TYPEINFO_CONST_ALIAS))
        return this;
    return ((const TypeInfoAlias*) this)->rawType;
}

TypeInfo* TypeInfo::getConcreteAlias() const
{
    return TypeManager::concreteType(this, CONCRETE_FORCE_ALIAS);
}

TypeInfo* TypeInfo::getFinalType()
{
    auto retType = this;
    while (retType)
    {
        if (retType->isPointer())
        {
            retType = castTypeInfo<TypeInfoPointer>(retType, TypeInfoKind::Pointer)->pointedType;
            continue;
        }

        if (retType->isSlice())
        {
            retType = castTypeInfo<TypeInfoSlice>(retType, TypeInfoKind::Slice)->pointedType;
            continue;
        }

        if (retType->isArray())
        {
            retType = castTypeInfo<TypeInfoArray>(retType, TypeInfoKind::Array)->finalType;
            continue;
        }

        return retType;
    }

    return this;
}

TypeInfoStruct* TypeInfo::getStructOrPointedStruct() const
{
    const auto self = getConcreteAlias();
    if (self->kind == TypeInfoKind::Struct)
        return castTypeInfo<TypeInfoStruct>(self, TypeInfoKind::Struct);

    if (self->kind == TypeInfoKind::Pointer)
    {
        const auto typePointer = castTypeInfo<TypeInfoPointer>(self, TypeInfoKind::Pointer);
        const auto pointed     = typePointer->pointedType->getConcreteAlias();
        if (pointed->isStruct())
            return castTypeInfo<TypeInfoStruct>(pointed, TypeInfoKind::Struct);
    }

    return nullptr;
}

bool TypeInfo::isPointerTo(NativeTypeKind pointerKind)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    const auto ptr = castTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    if (!ptr->pointedType)
        return false;
    return ptr->pointedType->getConcreteAlias()->isNative(pointerKind);
}

bool TypeInfo::isPointerTo(TypeInfoKind pointerKind)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    const auto ptr = castTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    if (!ptr->pointedType)
        return false;
    return ptr->pointedType->getConcreteAlias()->kind == pointerKind;
}

bool TypeInfo::isPointerTo(const TypeInfo* finalType)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    const auto ptr = castTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    if (!ptr->pointedType)
        return false;
    return ptr->pointedType->getConcreteAlias() == finalType;
}

bool TypeInfo::isPointerToTypeInfo() const
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    const auto ptr = castTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    if (!ptr->pointedType)
        return false;
    return ptr->pointedType->getConcreteAlias()->hasFlag(TYPEINFO_STRUCT_TYPEINFO);
}

bool TypeInfo::isPointerNull() const
{
    return this == g_TypeMgr->typeInfoNull;
}

bool TypeInfo::isInitializerList() const
{
    if (kind != TypeInfoKind::TypeListTuple)
        return false;
    return true;
}

bool TypeInfo::isArrayOfStruct() const
{
    if (kind != TypeInfoKind::Array)
        return false;
    const auto ptr = castTypeInfo<TypeInfoArray>(this, TypeInfoKind::Array);
    return ptr->finalType->getConcreteAlias()->isStruct();
}

bool TypeInfo::isArrayOfEnum() const
{
    if (kind != TypeInfoKind::Array)
        return false;
    const auto ptr = castTypeInfo<TypeInfoArray>(this, TypeInfoKind::Array);
    return ptr->finalType->getConcreteAlias()->isEnum();
}

bool TypeInfo::isMethod() const
{
    if (kind != TypeInfoKind::FuncAttr)
        return false;
    const auto ptr = castTypeInfo<TypeInfoFuncAttr>(this, kind);
    if (ptr->parameters.empty())
        return false;
    const auto param = ptr->parameters[0];
    if (!param->typeInfo->isPointer())
        return false;
    if (!param->typeInfo->isSelf())
        return false;
    if (!param->typeInfo->hasFlag(TYPEINFO_HAS_USING))
        return false;
    return true;
}

bool TypeInfo::isSame(const TypeInfo* from, uint64_t castFlags) const
{
    if (this == from)
        return true;

    if (kind != from->kind)
        return false;

    if (castFlags & CASTFLAG_EXACT)
    {
        if (hasFlag(TYPEINFO_CONST) != from->hasFlag(TYPEINFO_CONST))
            return false;
        if (hasFlag(TYPEINFO_GENERIC) != from->hasFlag(TYPEINFO_GENERIC))
            return false;
        if (hasFlag(TYPEINFO_AUTO_CAST) != from->hasFlag(TYPEINFO_AUTO_CAST))
            return false;
        if (hasFlag(TYPEINFO_POINTER_MOVE_REF) != from->hasFlag(TYPEINFO_POINTER_MOVE_REF))
            return false;
        if (isClosure() != from->isClosure())
            return false;
    }

    return true;
}

void TypeInfo::setConst()
{
    if (hasFlag(TYPEINFO_CONST))
        return;
    addFlag(TYPEINFO_CONST);
    name = "const " + name;
}

uint32_t TypeInfo::numRegisters() const
{
    if (sizeOf == 0)
        return 0;
    const uint32_t result = max(sizeOf, sizeof(void*)) / sizeof(void*);
    SWAG_ASSERT(result <= 2);
    return result;
}

void TypeInfo::copyFrom(const TypeInfo* from)
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
    if (hasFlag(TYPEINFO_GENERIC))
    {
        removeFlag(TYPEINFO_GENERIC);
        addFlag(TYPEINFO_FROM_GENERIC);
    }
}

int TypeInfoParam::numRegisters() const
{
    return typeInfo->numRegisters();
}

TypeInfoParam* TypeInfoParam::clone() const
{
    const auto newType  = TypeManager::makeParam();
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

bool TypeInfoParam::isSame(const TypeInfoParam* to, uint64_t castFlags) const
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
