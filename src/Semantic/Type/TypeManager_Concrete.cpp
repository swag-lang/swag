#include "pch.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

namespace
{
    const ExportedTypeInfo* concreteAlias(const ExportedTypeInfo* type1)
    {
        if (type1->kind != TypeInfoKind::Alias || type1->flags & static_cast<uint16_t>(ExportedTypeInfoFlags::Strict))
            return type1;
        const auto typeAlias = reinterpret_cast<const ExportedTypeInfoAlias*>(type1);
        return concreteAlias(typeAlias->rawType);
    }
}

bool TypeManager::compareConcreteType(const ExportedTypeInfo* type1, const ExportedTypeInfo* type2)
{
    SWAG_ASSERT(type1 && type2);
    if (type1 == type2)
        return true;

    type1 = concreteAlias(type1);
    type2 = concreteAlias(type2);

    if (type1->kind != type2->kind || type1->sizeOf != type2->sizeOf || type1->flags != type2->flags)
        return false;
    if (type1->fullName.count != type2->fullName.count)
        return false;
    return !memcmp(type1->fullName.buffer, type2->fullName.buffer, type1->fullName.count);
}

TypeInfo* TypeManager::concreteType(const TypeInfo* typeInfo, ToConcreteFlags flags)
{
    if (!typeInfo)
        return nullptr;

    switch (typeInfo->kind)
    {
        case TypeInfoKind::Native:
            return const_cast<TypeInfo*>(typeInfo);

        case TypeInfoKind::FuncAttr:
            if (flags.has(CONCRETE_FUNC))
            {
                const auto returnType = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr)->returnType;
                if (!returnType)
                    return g_TypeMgr->typeInfoVoid;
                return concreteType(returnType, flags);
            }
            break;

        case TypeInfoKind::Enum:
            if (flags.has(CONCRETE_ENUM))
                return concreteType(castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum)->rawType, flags);
            break;

        case TypeInfoKind::Alias:
            if (flags.has(CONCRETE_ALIAS | CONCRETE_FORCE_ALIAS))
            {
                const auto typeAlias = castTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
                if (typeAlias->isStrict() && !flags.has(CONCRETE_FORCE_ALIAS))
                    return const_cast<TypeInfo*>(static_cast<const TypeInfo*>(typeAlias));
                return concreteType(typeAlias->rawType, flags);
            }
            break;

        case TypeInfoKind::Generic:
            if (flags.has(CONCRETE_GENERIC))
            {
                const auto typeGeneric = castTypeInfo<TypeInfoGeneric>(typeInfo, TypeInfoKind::Generic);
                if (!typeGeneric->rawType)
                    return const_cast<TypeInfo*>(static_cast<const TypeInfo*>(typeGeneric));
                return concreteType(typeGeneric->rawType, flags);
            }
            break;
    }

    return const_cast<TypeInfo*>(typeInfo);
}

TypeInfo* TypeManager::concretePtrRefType(TypeInfo* typeInfo, ToConcreteFlags flags)
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
    if (typeInfo->isPointerRef())
        return castTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer)->pointedType;
    return typeInfo;
}

TypeInfo* TypeManager::concretePtrRefCond(TypeInfo* typeInfo, const AstNode* node)
{
    if (node->hasSemFlag(SEMFLAG_FROM_REF))
        return concretePtrRef(typeInfo);
    return typeInfo;
}
