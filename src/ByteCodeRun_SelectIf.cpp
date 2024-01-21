#include "pch.h"
#include "Diagnostic.h"
#include "ByteCode.h"
#include "Module.h"
#include "ErrorIds.h"
#include "ByteCodeGen.h"
#include "TypeInfo.h"

bool ByteCodeRun::getVariadicSI(ByteCodeRunContext* context, ByteCodeInstruction* ip, Register* regPtr, Register* regCount)
{
    auto paramIdx   = ip->c.u32;
    auto callParams = context->callerContext->validIfParameters;

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

    if (child->typeInfo->isListArray())
    {
        auto typeList = CastTypeInfo<TypeInfoList>(child->typeInfo, TypeInfoKind::TypeListArray);
        if (regCount)
            regCount->u64 = typeList->subTypes.size();
        return true;
    }

    if (child->typeInfo->isArray())
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(child->typeInfo, TypeInfoKind::Array);
        if (regCount)
            regCount->u64 = typeArray->totalCount;
        return true;
    }

    return false;
}

void* ByteCodeRun::executeLocationSI(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    uint32_t paramIdx   = ip->c.u32;
    auto     callParams = context->callerContext->validIfParameters;
    if (!callParams)
        return nullptr;
    if (paramIdx >= callParams->childs.size())
        return nullptr;

    auto         child          = callParams->childs[paramIdx];
    uint32_t     storageOffset  = UINT32_MAX;
    DataSegment* storageSegment = nullptr;
    ByteCodeGen::computeSourceLocation(context->callerContext, child, &storageOffset, &storageSegment, true);
    return storageSegment->address(storageOffset);
}

bool ByteCodeRun::executeIsConstExprSI(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto solved = (SymbolOverload*) ip->d.pointer;

    // Variadic
    /////////////////////////////////////////
    if (solved->typeInfo->isVariadic() || solved->typeInfo->isTypedVariadic())
        return getVariadicSI(context, ip, nullptr, nullptr);

    uint32_t paramIdx   = ip->c.u32;
    auto     callParams = context->callerContext->validIfParameters;
    if (!callParams)
        return true;
    if (paramIdx >= callParams->childs.size())
        return true;

    auto child = callParams->childs[paramIdx];

    // Slice
    /////////////////////////////////////////
    if (solved->typeInfo->isSlice())
    {
        if (child->typeInfo->isArray())
            return true;
        if (child->typeInfo->isListArray())
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
        callInternalCompilerError(context, ip, Fmt(Err(Err0031), solved->symbol->name.c_str()));
        return;
    }

    auto paramIdx    = ip->c.u32;
    auto callParams  = context->callerContext->validIfParameters;
    auto registersRC = context->curRegistersRC;

    if (!callParams || paramIdx >= callParams->childs.size())
    {
        registersRC[ip->a.u32].pointer = nullptr;
        registersRC[ip->b.u32].u64     = 0;
        return;
    }

    // Variadic
    /////////////////////////////////////////
    if (solved->typeInfo->isVariadic() || solved->typeInfo->isTypedVariadic())
    {
        if (!getVariadicSI(context, ip, &registersRC[ip->a.u32], &registersRC[ip->b.u32]))
        {
            callInternalCompilerError(context, ip, Fmt(Err(Err0039), solved->typeInfo->getDisplayNameC()));
            return;
        }

        return;
    }

    auto child = callParams->childs[paramIdx];

    // Slice
    /////////////////////////////////////////
    if (solved->typeInfo->isSlice())
    {
        if (child->typeInfo->isArray())
        {
            auto typeArray                 = CastTypeInfo<TypeInfoArray>(child->typeInfo, TypeInfoKind::Array);
            registersRC[ip->a.u32].pointer = nullptr;
            registersRC[ip->b.u32].u64     = typeArray->totalCount;
            return;
        }

        if (child->typeInfo->isListArray())
        {
            auto typeList                  = CastTypeInfo<TypeInfoList>(child->typeInfo, TypeInfoKind::TypeListArray);
            registersRC[ip->a.u32].pointer = nullptr;
            registersRC[ip->b.u32].u64     = typeList->subTypes.size();
            return;
        }

        callInternalCompilerError(context, ip, Fmt(Err(Err0039), solved->typeInfo->getDisplayNameC()));
        return;
    }

    // Native
    /////////////////////////////////////////
    SWAG_ASSERT(paramIdx < callParams->childs.size());
    if (solved->typeInfo->isNative())
    {
        switch (solved->typeInfo->nativeType)
        {
        case NativeTypeKind::String:
            registersRC[ip->a.u32].pointer = (uint8_t*) child->computedValue->text.buffer;
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
        case NativeTypeKind::Rune:
        case NativeTypeKind::Bool:
        case NativeTypeKind::F32:
        case NativeTypeKind::F64:
            registersRC[ip->a.u32].u64 = child->computedValue->reg.u64;
            return;

        default:
            break;
        }
    }

    callInternalCompilerError(context, ip, Fmt(Err(Err0039), solved->typeInfo->getDisplayNameC()));
}
