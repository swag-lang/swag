#include "pch.h"
#include "Ast.h"
#include "TypeManager.h"
#include "AstFlags.h"

static const ExportedTypeInfo* concreteAlias(const ExportedTypeInfo* type1)
{
    if (type1->kind != TypeInfoKind::Alias || (type1->flags & (uint16_t) ExportedTypeInfoFlags::Strict))
        return type1;
    auto typeAlias = (const ExportedTypeInfoAlias*) type1;
    return concreteAlias((ExportedTypeInfo*) typeAlias->rawType);
}

bool TypeManager::compareConcreteType(const ExportedTypeInfo* type1, const ExportedTypeInfo* type2)
{
    SWAG_ASSERT(type1 && type2);
    if (type1 == type2)
        return true;

    type1 = concreteAlias(type1);
    type2 = concreteAlias(type2);

    if ((type1->kind != type2->kind) || (type1->sizeOf != type2->sizeOf) || (type1->flags != type2->flags))
        return false;
    if (type1->fullName.count != type2->fullName.count)
        return false;
    return !memcmp(type1->fullName.buffer, type2->fullName.buffer, type1->fullName.count);
}

TypeInfo* TypeManager::concreteType(TypeInfo* typeInfo, uint32_t flags)
{
    if (!typeInfo)
        return typeInfo;

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Native:
        return typeInfo;

    case TypeInfoKind::FuncAttr:
        if (flags & CONCRETE_FUNC)
        {
            auto returnType = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr)->returnType;
            if (!returnType)
                return g_TypeMgr->typeInfoVoid;
            return concreteType(returnType, flags);
        }
        break;

    case TypeInfoKind::Enum:
        if (flags & CONCRETE_ENUM)
            return concreteType(CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum)->rawType, flags);
        break;

    case TypeInfoKind::Alias:
        if (flags & (CONCRETE_ALIAS | CONCRETE_FORCEALIAS))
        {
            auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
            if (typeAlias->isStrict() && !(flags & CONCRETE_FORCEALIAS))
                return typeAlias;
            return concreteType(typeAlias->rawType, flags);
        }
        break;

    case TypeInfoKind::Generic:
        if (flags & CONCRETE_GENERIC)
        {
            auto typeGeneric = CastTypeInfo<TypeInfoGeneric>(typeInfo, TypeInfoKind::Generic);
            if (!typeGeneric->rawType)
                return typeGeneric;
            return concreteType(typeGeneric->rawType, flags);
        }
        break;

    default:
        break;
    }

    return typeInfo;
}

TypeInfo* TypeManager::concretePtrRefType(TypeInfo* typeInfo, uint32_t flags)
{
    if (!typeInfo)
        return nullptr;
    typeInfo = concretePtrRef(typeInfo);
    typeInfo = concreteType(typeInfo, flags);
    typeInfo = concretePtrRef(typeInfo);
    return typeInfo;
}

TypeInfo* TypeManager::concretePtrRef(TypeInfo* typeInfo)
{
    if (!typeInfo)
        return nullptr;
    if (typeInfo->flags & TYPEINFO_POINTER_REF)
        return CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer)->pointedType;
    return typeInfo;
}

TypeInfo* TypeManager::concretePtrRefCond(TypeInfo* typeInfo, AstNode* node)
{
    if (node->semFlags & SEMFLAG_FROM_REF)
        return concretePtrRef(typeInfo);
    return typeInfo;
}
