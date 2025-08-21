#include "pch.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Scope.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"

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
    computeWhateverName(ComputeNameKind::Name);
}

void TypeInfo::computeScopedName(Utf8& newName) const
{
    if (declNode && declNode->ownerScope && declNode->ownerScope->isNot(ScopeKind::Function))
    {
        newName += declNode->ownerScope->getFullName();
        if (!newName.empty())
            newName += ".";
    }
}

Utf8 TypeInfo::getName()
{
    ScopedLock lk(mutex);
    computeWhateverNameNoLock(ComputeNameKind::Name);
    SWAG_ASSERT(!name.empty());
    return name;
}

Utf8 TypeInfo::getScopedName()
{
    ScopedLock lk(mutex);
    computeWhateverNameNoLock(ComputeNameKind::ScopedName);
    return scopedName;
}

Utf8 TypeInfo::getDisplayName()
{
    Utf8 str;
    computeWhateverName(str, ComputeNameKind::DisplayName);
    return str.empty() ? name : str;
}

const char* TypeInfo::getDisplayNameC()
{
    const auto res = getDisplayName();
    return _strdup(res.cstr()); // Leak and slow, but only for messages
}

const Utf8& TypeInfo::computeWhateverName(ComputeNameKind nameKind)
{
    ScopedLock lk(mutex);
    return computeWhateverNameNoLock(nameKind);
}

const Utf8& TypeInfo::computeWhateverNameNoLock(ComputeNameKind nameKind)
{
    Utf8 str;
    switch (nameKind)
    {
        case ComputeNameKind::Name:
            if (name.empty())
            {
                computeWhateverName(str, nameKind);
                SWAG_RACE_CONDITION_WRITE(raceName);
                name = std::move(str);
            }
            return name;

        case ComputeNameKind::DisplayName:
            if (displayName.empty())
            {
                computeWhateverName(str, nameKind);
                SWAG_RACE_CONDITION_WRITE(raceName);
                displayName = std::move(str);
            }
            return displayName;

        case ComputeNameKind::ScopedName:
            if (scopedName.empty())
            {
                computeWhateverName(str, nameKind);
                SWAG_RACE_CONDITION_WRITE(raceName);
                scopedName = std::move(str);
            }
            return scopedName;

        case ComputeNameKind::ScopedNameExport:
            if (scopedNameExport.empty())
            {
                computeWhateverName(str, nameKind);
                SWAG_RACE_CONDITION_WRITE(raceName);
                scopedNameExport = std::move(str);
            }
            return scopedNameExport;
    }

    SWAG_ASSERT(false);
    return name;
}

void TypeInfo::computeWhateverName(Utf8& resName, ComputeNameKind nameKind)
{
    switch (nameKind)
    {
        case ComputeNameKind::Name:
        case ComputeNameKind::DisplayName:
            resName = name;
            break;

        case ComputeNameKind::ScopedName:
        case ComputeNameKind::ScopedNameExport:
            computeScopedName(resName);
            resName += name;
            break;
    }
}

bool TypeInfo::isPointerVoid()
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    const auto ptr   = castTypeInfo<TypeInfoPointer>(this, TypeInfoKind::Pointer);
    const auto unRef = ptr->pointedType->getConcreteAlias();
    if (!unRef->isNative())
        return false;
    if (unRef->nativeType != NativeTypeKind::Void)
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
    return castTypeInfo<TypeInfoAlias>(this)->rawType;
}

const TypeInfo* TypeInfo::getConstAlias() const
{
    if (!hasFlag(TYPEINFO_CONST_ALIAS))
        return this;
    return castTypeInfo<TypeInfoAlias>(this)->rawType;
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
    const auto me = getConcreteAlias();
    if (me->kind == TypeInfoKind::Struct)
        return castTypeInfo<TypeInfoStruct>(me, TypeInfoKind::Struct);

    if (me->kind == TypeInfoKind::Pointer)
    {
        const auto typePointer = castTypeInfo<TypeInfoPointer>(me, TypeInfoKind::Pointer);
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

bool TypeInfo::isNullable() const
{
    if (flags.has(TYPEINFO_NULLABLE))
        return true;
    const auto in = TypeManager::concreteType(this);
    if (in->flags.has(TYPEINFO_NULLABLE))
        return true;
    return false;
}

bool TypeInfo::couldBeNull() const
{
    if (isString() || isAny() || isInterface() || isSlice() || isCString() || isLambdaClosure())
        return true;
    if (isPointer() && !isConstPointerRef() && !isAutoConstPointerRef() && !isPointerRef())
        return true;
    return false;
}

bool TypeInfo::isSame(const TypeInfo* from, CastFlags castFlags) const
{
    if (this == from)
        return true;

    if (kind != from->kind)
        return false;

    if (castFlags.has(CAST_FLAG_EXACT))
    {
        if (isConst() != from->isConst())
            return false;
        if (isGeneric() != from->isGeneric())
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
    if (isConst())
        return;
    addFlag(TYPEINFO_CONST);
    name = "const " + name;
}

uint32_t TypeInfo::numRegisters() const
{
    if (sizeOf == 0)
        return 0;
    const uint32_t result = std::max(sizeOf, static_cast<uint32_t>(sizeof(void*))) / sizeof(void*);
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
    if (isGeneric())
    {
        removeFlag(TYPEINFO_GENERIC);
        addFlag(TYPEINFO_FROM_GENERIC);
    }
}

uint32_t TypeInfoParam::numRegisters() const
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

bool TypeInfoParam::isSame(const TypeInfoParam* to, CastFlags castFlags) const
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
