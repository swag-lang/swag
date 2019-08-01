#include "pch.h"
#include "TypeInfo.h"
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
Pool<TypeInfoVariadic>      g_Pool_typeInfoVariadic;
Pool<TypeInfoStruct>        g_Pool_typeInfoStruct;

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
    auto fromFunc = static_cast<TypeInfoFuncAttr*>(from);
    assert(from->kind == TypeInfoKind::FuncAttr || from->kind == TypeInfoKind::Lambda);
    return isSame(fromFunc);
}

bool TypeInfoFuncAttr::isSameExact(TypeInfo* from)
{
    if (!isSame(from))
        return false;
    auto fromFunc = static_cast<TypeInfoFuncAttr*>(from);

    if (returnType && returnType != g_TypeMgr.typeInfoVoid && !fromFunc->returnType)
        return false;
    if (!returnType && fromFunc->returnType && fromFunc->returnType != g_TypeMgr.typeInfoVoid)
        return false;
    if (returnType && fromFunc->returnType && !returnType->isSame(fromFunc->returnType))
        return false;
    return true;
}

void TypeInfoFuncAttr::match(SymbolMatchContext& context)
{
    int  cptResolved  = 0;
    bool badSignature = false;

    // For a lambda
    if (context.forLambda)
    {
        context.result = MatchResult::Ok;
        return;
    }

    // One boolean per used parameter
    context.doneParameters.clear();
    context.doneParameters.resize(parameters.size(), false);

    // First we solve unnamed parameters
    int  numParams          = (int) context.parameters.size();
    bool hasNamedParameters = false;
    for (int i = 0; i < numParams; i++)
    {
        auto callParameter = context.parameters[i];
        auto param         = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallParam);
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
        if (symbolParameter->typeInfo == g_TypeMgr.typeInfoVariadic)
        {
            context.result = badSignature ? MatchResult::BadSignature : MatchResult::Ok;
            return;
        }

        auto typeInfo = TypeManager::concreteType(callParameter->typeInfo, MakeConcrete::FlagFunc);
        bool same     = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
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
            auto param    = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallParam);
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

                    auto typeInfo = TypeManager::concreteType(callParameter->typeInfo, MakeConcrete::FlagFunc);
                    bool same     = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
                    if (!same)
                    {
                        context.badSignatureParameterIdx  = i;
                        context.badSignatureRequestedType = symbolParameter->typeInfo;
                        context.badSignatureGivenType     = typeInfo;
                        badSignature                      = true;
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

TypeInfo* TypeInfoNative::clone()
{
    auto newType = g_Pool_typeInfoNative.alloc();
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoNamespace::clone()
{
    auto newType   = g_Pool_typeInfoNamespace.alloc();
    newType->scope = scope;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoEnum::clone()
{
    auto newType     = g_Pool_typeInfoEnum.alloc();
    newType->scope   = scope;
    newType->rawType = rawType;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoEnumValue::clone()
{
    auto newType       = g_Pool_typeInfoEnumValue.alloc();
    newType->scope     = scope;
    newType->enumOwner = enumOwner;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoFuncAttrParam::clone()
{
    auto newType        = g_Pool_typeInfoFuncAttrParam.alloc();
    newType->namedParam = namedParam;
    newType->typeInfo   = typeInfo;
    newType->index      = index;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoFuncAttr::clone()
{
    auto newType                  = g_Pool_typeInfoFuncAttr.alloc();
    newType->firstDefaultValueIdx = firstDefaultValueIdx;
    newType->parameters           = parameters;
    newType->returnType           = returnType;
    newType->copyFrom(this);
    return newType;
}

void TypeInfoFuncAttr::computeName()
{
    name = format("(");

    for (int i = 0; i < parameters.size(); i++)
    {
        if (i)
            name += ", ";
        name += parameters[i]->typeInfo->name;
    }

    name += ")";
    if (returnType)
        name += format("->%s", returnType->name.c_str());
}

TypeInfo* TypeInfoPointer::clone()
{
    auto newType         = g_Pool_typeInfoPointer.alloc();
    newType->pointedType = pointedType;
    newType->ptrCount    = ptrCount;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoArray::clone()
{
    auto newType         = g_Pool_typeInfoArray.alloc();
    newType->pointedType = pointedType;
    newType->count       = count;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoSlice::clone()
{
    auto newType         = g_Pool_typeInfoSlice.alloc();
    newType->pointedType = pointedType;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoList::clone()
{
    auto newType      = g_Pool_typeInfoList.alloc();
    newType->childs   = childs;
    newType->scope    = scope;
    newType->listKind = listKind;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoVariadic::clone()
{
    auto newType = g_Pool_typeInfoVariadic.alloc();
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoStruct::clone()
{
    auto newType         = g_Pool_typeInfoStruct.alloc();
    newType->scope       = scope;
    newType->childs      = childs;
    newType->structNode  = structNode;
    newType->copyFrom(this);
    return newType;
}

const char* TypeInfo::getKindName(TypeInfo* typeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Namespace:
        return "a namespace";
    case TypeInfoKind::Enum:
        return "an enum";
    case TypeInfoKind::EnumValue:
        return "an enum value";
    case TypeInfoKind::Array:
        return "an array";
    case TypeInfoKind::Pointer:
        return "a pointer";
    case TypeInfoKind::FuncAttr:
        return "a function";
    case TypeInfoKind::TypeList:
        return "a tuple";
    case TypeInfoKind::Variadic:
        return "a variadic";
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
    case TypeInfoKind::EnumValue:
        return "enum value";
    case TypeInfoKind::Array:
        return "array";
    case TypeInfoKind::Pointer:
        return "pointer";
    case TypeInfoKind::FuncAttr:
        return "function";
    case TypeInfoKind::TypeList:
        return "tuple";
    case TypeInfoKind::Variadic:
        return "variadic";
    }

    return "<type>";
}