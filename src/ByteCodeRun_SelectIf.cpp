#include "pch.h"
#include "Diagnostic.h"
#include "ByteCode.h"
#include "Module.h"
#include "ErrorIds.h"

bool ByteCodeRun::getVariadicSI(ByteCodeRunContext* context, ByteCodeInstruction* ip, Register* regPtr, Register* regCount)
{
    auto paramIdx   = ip->c.u32;
    auto callParams = context->callerContext->selectIfParameters;

    // Nothing
    if (!callParams)
    {
        if (regCount)
            regCount->u64 = 0;
        return true;
    }

    if (regPtr)
        regPtr->pointer = nullptr;

    // Count
    auto numParamsCall = callParams->childs.size();
    auto numParamsFunc = ip->c.u32;
    auto count         = numParamsCall - numParamsFunc;
    if (regCount)
        regCount->u64 = numParamsCall - numParamsFunc;
    if (count != 1)
        return true;

    // Try to deal with @spread
    auto child = callParams->childs[paramIdx];
    if (!(child->typeInfo->flags & TYPEINFO_SPREAD))
        return true;

    child = child->childs.front();
    SWAG_ASSERT(child->kind == AstNodeKind::IdentifierRef);
    child = child->childs.front();
    SWAG_ASSERT(child->kind == AstNodeKind::IntrinsicProp);
    child = child->childs.front();

    if (child->typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(child->typeInfo, TypeInfoKind::TypeListArray);
        if (regCount)
            regCount->u64 = typeList->subTypes.size();
        return true;
    }

    if (child->typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(child->typeInfo, TypeInfoKind::Array);
        if (regCount)
            regCount->u64 = typeArray->totalCount;
        return true;
    }

    return false;
}

bool ByteCodeRun::executeIsConstExprSI(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto solved = (SymbolOverload*) ip->d.pointer;

    // Variadic
    /////////////////////////////////////////
    if (solved->typeInfo->kind == TypeInfoKind::Variadic || solved->typeInfo->kind == TypeInfoKind::TypedVariadic)
        return getVariadicSI(context, ip, nullptr, nullptr);

    uint32_t paramIdx   = ip->c.u32;
    auto     callParams = context->callerContext->selectIfParameters;
    if (!callParams)
        return true;

    SWAG_ASSERT(paramIdx < callParams->childs.size());
    auto child = callParams->childs[paramIdx];

    // Slice
    /////////////////////////////////////////
    if (solved->typeInfo->kind == TypeInfoKind::Slice)
    {
        if (child->typeInfo->kind == TypeInfoKind::Array)
            return true;
        if (child->typeInfo->kind == TypeInfoKind::TypeListArray)
            return true;
    }

    // The rest : flags
    /////////////////////////////////////////
    if (child->flags & (AST_VALUE_COMPUTED | AST_CONST_EXPR))
        return true;

    return false;
}

void ByteCodeRun::executeGetFromStackSI(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    // Is this constexpr ?
    auto solved = (SymbolOverload*) ip->d.pointer;
    if (!executeIsConstExprSI(context, ip))
    {
        context->hasError = true;
        context->errorMsg = Utf8::format(Msg0089, solved->symbol->name.c_str());
        return;
    }

    auto paramIdx    = ip->c.u32;
    auto callParams  = context->callerContext->selectIfParameters;
    auto registersRC = context->registersRC[context->curRC]->buffer;

    if (!callParams)
    {
        registersRC[ip->a.u32].pointer = nullptr;
        registersRC[ip->b.u32].u64     = 0;
        return;
    }

    // Variadic
    /////////////////////////////////////////
    if (solved->typeInfo->kind == TypeInfoKind::Variadic || solved->typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        if (!getVariadicSI(context, ip, &registersRC[ip->a.u32], &registersRC[ip->b.u32]))
        {
            context->hasError = true;
            context->errorMsg = Utf8::format(Msg0090, solved->typeInfo->getDisplayName().c_str());
        }

        return;
    }

    auto child = callParams->childs[paramIdx];

    // Slice
    /////////////////////////////////////////
    if (solved->typeInfo->kind == TypeInfoKind::Slice)
    {
        if (child->typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray                 = CastTypeInfo<TypeInfoArray>(child->typeInfo, TypeInfoKind::Array);
            registersRC[ip->a.u32].pointer = nullptr;
            registersRC[ip->b.u32].u64     = typeArray->totalCount;
            return;
        }

        if (child->typeInfo->kind == TypeInfoKind::TypeListArray)
        {
            auto typeList                  = CastTypeInfo<TypeInfoList>(child->typeInfo, TypeInfoKind::TypeListArray);
            registersRC[ip->a.u32].pointer = nullptr;
            registersRC[ip->b.u32].u64     = typeList->subTypes.size();
            return;
        }

        context->hasError = true;
        context->errorMsg = Utf8::format(Msg0090, solved->typeInfo->getDisplayName().c_str());
        return;
    }

    // Native
    /////////////////////////////////////////
    SWAG_ASSERT(paramIdx < callParams->childs.size());
    if (solved->typeInfo->kind == TypeInfoKind::Native)
    {
        switch (solved->typeInfo->nativeType)
        {
        case NativeTypeKind::String:
            registersRC[ip->a.u32].pointer = (uint8_t*) child->computedValue->text.c_str();
            registersRC[ip->b.u32].u64     = child->computedValue->text.length();
            return;

        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        case NativeTypeKind::Int:
        case NativeTypeKind::Rune:
        case NativeTypeKind::Bool:
        case NativeTypeKind::F32:
        case NativeTypeKind::F64:
            registersRC[ip->a.u32].u64 = child->computedValue->reg.u64;
            return;
        }
    }

    context->hasError = true;
    context->errorMsg = Utf8::format(Msg0102, solved->typeInfo->getDisplayName().c_str());
}
