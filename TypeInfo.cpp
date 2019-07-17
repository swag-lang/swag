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
    int  cptResolved  = 0;
    bool badSignature = false;

    // One boolean per used parameter
    context.doneParameters.clear();
    context.doneParameters.resize(parameters.size(), false);

    // First we solve unnamed parameters
    int  numParams          = (int) context.parameters.size();
    bool hasNamedParameters = false;
    for (int i = 0; i < numParams; i++)
    {
        auto callParameter = context.parameters[i];
        auto param         = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallOneParam);
        if (!param->namedParam.empty())
        {
            hasNamedParameters = true;
            break;
        }

        if (i >= parameters.size())
        {
            context.result = MatchResult::TooManyParameters;
            return;
        }

        auto symbolParameter = parameters[i];
        auto typeInfo        = TypeManager::concreteType(callParameter->typeInfo, MakeConcrete::FlagFunc);
        bool same            = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
        if (!same)
        {
            context.badSignatureParameterIdx  = i;
            context.badSignatureRequestedType = symbolParameter->typeInfo;
            context.badSignatureGivenType     = typeInfo;
            badSignature                      = true;
        }

        context.doneParameters[cptResolved] = true;
        param->resolvedParameter            = symbolParameter;
        param->index                        = cptResolved++;
    }

    // Named parameters
    if (hasNamedParameters)
    {
        auto callParameter = context.parameters[0];
        callParameter->parent->flags |= AST_MUST_SORT_CHILDS;

        auto startResolved = cptResolved;
        for (int i = startResolved; i < numParams; i++)
        {
            callParameter = context.parameters[i];
            auto param    = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallOneParam);
            if (param->namedParam.empty())
            {
                // After the last named parameters, we must have the first default value, or nothing
                if (i < firstDefaultValueIdx)
                {
                    context.badSignatureParameterIdx = i;
                    context.result                   = InvalidNamedParameter;
                    return;
                }

                break;
            }

            for (int j = startResolved; j < parameters.size(); j++)
            {
                auto symbolParameter = parameters[j];
                if (parameters[j]->name == param->namedParam)
                {
                    if (context.doneParameters[j])
                    {
                        context.badSignatureParameterIdx = i;
                        context.result                   = DuplicatedNamedParameter;
                        return;
                    }

                    param->resolvedParameter  = symbolParameter;
                    param->index              = j;
                    context.doneParameters[j] = true;
                    cptResolved++;
                    break;
                }
            }

            if (!param->resolvedParameter)
            {
                context.badSignatureParameterIdx = i;
                context.result                   = InvalidNamedParameter;
                return;
            }
        }
    }

    // No need to sort childs if there's no named parameters, as the order is already correct
    else if (context.parameters.size())
    {
        auto callParameter = context.parameters[0];
        callParameter->parent->flags |= AST_MUST_SORT_CHILDS;
    }

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

TypeInfo* TypeInfoNative::clone()
{
    auto newType = g_Pool_typeInfoNative.alloc();
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoNamespace::clone()
{
    auto newType = g_Pool_typeInfoNamespace.alloc();

    newType->scope = this->scope;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoEnum::clone()
{
    auto newType = g_Pool_typeInfoEnum.alloc();

    newType->scope   = this->scope;
    newType->rawType = this->rawType;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoEnumValue::clone()
{
    auto newType = g_Pool_typeInfoEnumValue.alloc();

    newType->scope     = this->scope;
    newType->enumOwner = this->enumOwner;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoFuncAttrParam::clone()
{
    auto newType = g_Pool_typeInfoFuncAttrParam.alloc();

    newType->namedParam = this->namedParam;
    newType->typeInfo   = this->typeInfo;
    newType->index      = this->index;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoFuncAttr::clone()
{
    auto newType = g_Pool_typeInfoFuncAttr.alloc();

    newType->firstDefaultValueIdx = this->firstDefaultValueIdx;
    newType->parameters           = this->parameters;
    newType->returnType           = this->returnType;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoPointer::clone()
{
    auto newType = g_Pool_typeInfoPointer.alloc();

    newType->pointedType = this->pointedType;
    newType->ptrCount    = this->ptrCount;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoArray::clone()
{
    auto newType = g_Pool_typeInfoArray.alloc();

    newType->pointedType = this->pointedType;
    newType->count       = this->count;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoSlice::clone()
{
    auto newType = g_Pool_typeInfoSlice.alloc();

    newType->pointedType = this->pointedType;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoList::clone()
{
    auto newType = g_Pool_typeInfoList.alloc();

    newType->childs = this->childs;
    newType->copyFrom(this);
    return newType;
}