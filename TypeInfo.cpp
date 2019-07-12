#include "pch.h"
#include "SourceFile.h"
#include "TypeInfo.h"
#include "SymTable.h"
#include "ThreadManager.h"
#include "Global.h"
#include "TypeManager.h"
#include "Ast.h"

Pool<TypeInfoFuncAttr>      g_Pool_typeInfoFuncAttr;
Pool<TypeInfoNamespace>     g_Pool_typeInfoNamespace;
Pool<TypeInfoEnum>          g_Pool_typeInfoEnum;
Pool<TypeInfoEnumValue>     g_Pool_typeInfoEnumValue;
Pool<TypeInfoFuncAttrParam> g_Pool_typeInfoFuncAttrParam;
Pool<TypeInfoPointer>       g_Pool_typeInfoPointer;
Pool<TypeInfoArray>         g_Pool_typeInfoArray;
Pool<TypeInfoSlice>         g_Pool_typeInfoSlice;
Pool<TypeInfoList>          g_Pool_typeInfoList;
Pool<TypeInfoNative>        g_Pool_typeInfoNative;

bool TypeInfoFuncAttr::isSame(TypeInfoFuncAttr* other)
{
    if (parameters.size() != other->parameters.size())
        return false;
    for (int i = 0; i < parameters.size(); i++)
    {
        if (!parameters[i]->typeInfo->isSame(other->parameters[i]->typeInfo))
            return false;
    }

    return true;
}

bool TypeInfoFuncAttr::isSame(TypeInfo* from)
{
    if (!TypeInfo::isSame(from))
        return false;
    auto fromFunc = CastTypeInfo<TypeInfoFuncAttr>(from, TypeInfoKind::FuncAttr);
    return isSame(fromFunc);
}

void TypeInfoFuncAttr::match(SymbolMatchContext& context)
{
    int  firstUserNamed = 0;
    int  cptResolved    = 0;
    bool badSignature   = false;

    // First we solve unnamed parameters
    int numParams = (int) context.parameters.size();
    for (int i = 0; i < numParams; i++)
    {
        auto param = CastAst<AstFuncCallParam>(context.parameters[i], AstNodeKind::FuncCallParam);
        if (!param->name.empty())
        {
            firstUserNamed = i;
            break;
        }

        if (i >= parameters.size())
        {
            context.result = MatchResult::TooManyParameters;
            return;
        }

        auto typeInfo = TypeManager::concreteType(context.parameters[i]->typeInfo);
        bool same     = TypeManager::makeCompatibles(nullptr, parameters[i]->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
        if (!same)
        {
            context.badSignatureParameterIdx  = i;
            context.badSignatureRequestedType = parameters[i]->typeInfo;
            context.badSignatureGivenType     = typeInfo;
            badSignature                      = true;
        }

        param->resolvedParameter = parameters[i];
        cptResolved++;
    }

    // Named parameters

    // Not enough parameters
    int firstDefault = firstDefaultValueIdx;
    if (firstDefault == -1)
        firstDefault = (int) parameters.size();
    if (cptResolved < firstDefault)
    {
        context.result = MatchResult::NotEnoughParameters;
        return;
    }

    context.result = badSignature ? MatchResult::BadSignature : MatchResult::Ok;
}

const char* TypeInfo::getNakedName(TypeInfo* typeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Namespace:
        return "namespace";
    case TypeInfoKind::Enum:
        return "enum";
    case TypeInfoKind::EnumValue:
        return "enum value";
    case TypeInfoKind::Array:
        return "array";
    case TypeInfoKind::Pointer:
        return "pointer";
    }

    return "type";
}

TypeInfo* TypeInfoNative::clone(TypeInfo* from)
{
    auto newType = g_Pool_typeInfoNative.alloc();
    newType->copyFrom(from);
    return newType;
}

TypeInfo* TypeInfoNamespace::clone(TypeInfo* from)
{
    auto newType = g_Pool_typeInfoNamespace.alloc();
    auto other   = static_cast<TypeInfoNamespace*>(from);

    newType->scope = other->scope;
    newType->copyFrom(from);
    return newType;
}

TypeInfo* TypeInfoEnum::clone(TypeInfo* from)
{
    auto newType = g_Pool_typeInfoEnum.alloc();
    auto other   = static_cast<TypeInfoEnum*>(from);

    newType->scope   = other->scope;
    newType->rawType = other->rawType;
    newType->copyFrom(from);
    return newType;
}

TypeInfo* TypeInfoEnumValue::clone(TypeInfo* from)
{
    auto newType = g_Pool_typeInfoEnumValue.alloc();
    auto other   = static_cast<TypeInfoEnumValue*>(from);

    newType->scope     = other->scope;
    newType->enumOwner = other->enumOwner;
    newType->copyFrom(from);
    return newType;
}

TypeInfo* TypeInfoFuncAttrParam::clone(TypeInfo* from)
{
    auto newType = g_Pool_typeInfoFuncAttrParam.alloc();
    auto other   = static_cast<TypeInfoFuncAttrParam*>(from);

    newType->namedParam = other->namedParam;
    newType->typeInfo   = other->typeInfo;
    newType->index      = other->index;
    newType->copyFrom(from);
    return newType;
}

TypeInfo* TypeInfoFuncAttr::clone(TypeInfo* from)
{
    auto newType = g_Pool_typeInfoFuncAttr.alloc();
    auto other   = static_cast<TypeInfoFuncAttr*>(from);

    newType->firstDefaultValueIdx = other->firstDefaultValueIdx;
    newType->parameters           = other->parameters;
    newType->returnType           = other->returnType;
    newType->copyFrom(from);
    return newType;
}

TypeInfo* TypeInfoPointer::clone(TypeInfo* from)
{
    auto newType = g_Pool_typeInfoPointer.alloc();
    auto other   = static_cast<TypeInfoPointer*>(from);

    newType->pointedType = other->pointedType;
    newType->ptrCount    = other->ptrCount;
    newType->copyFrom(from);
    return newType;
}

TypeInfo* TypeInfoArray::clone(TypeInfo* from)
{
    auto newType = g_Pool_typeInfoArray.alloc();
    auto other   = static_cast<TypeInfoArray*>(from);

    newType->pointedType = other->pointedType;
    newType->count       = other->count;
    newType->copyFrom(from);
    return newType;
}

TypeInfo* TypeInfoSlice::clone(TypeInfo* from)
{
    auto newType = g_Pool_typeInfoSlice.alloc();
    auto other   = static_cast<TypeInfoSlice*>(from);

    newType->pointedType = other->pointedType;
    newType->copyFrom(from);
    return newType;
}

TypeInfo* TypeInfoList::clone(TypeInfo* from)
{
    auto newType = g_Pool_typeInfoList.alloc();
    auto other   = static_cast<TypeInfoList*>(from);

    newType->childs = other->childs;
    newType->copyFrom(from);
    return newType;
}