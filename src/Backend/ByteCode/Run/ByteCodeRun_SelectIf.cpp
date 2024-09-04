#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/ErrorIds.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/AstFlags.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"

bool ByteCodeRun::getVariadicSI(const ByteCodeRunContext* context, const ByteCodeInstruction* ip, Register* regPtr, Register* regCount)
{
    const auto paramIdx   = ip->c.u32;
    const auto callParams = context->callerContext->whereParameters;

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
    const auto numParamsCall = callParams->childCount();
    const auto numParamsFunc = ip->c.u32;
    const auto count         = numParamsCall - numParamsFunc;
    if (regCount)
        regCount->u64 = numParamsCall - numParamsFunc;
    if (count != 1)
        return true;

    // Try to deal with @spread
    auto child = callParams->children[paramIdx];
    if (!child->typeInfo->hasFlag(TYPEINFO_SPREAD))
        return true;

    child = child->firstChild();
    SWAG_ASSERT(child->is(AstNodeKind::IdentifierRef));
    child = child->firstChild();
    SWAG_ASSERT(child->is(AstNodeKind::IntrinsicProp));
    child = child->firstChild();

    if (child->typeInfo->isListArray())
    {
        const auto typeList = castTypeInfo<TypeInfoList>(child->typeInfo, TypeInfoKind::TypeListArray);
        if (regCount)
            regCount->u64 = typeList->subTypes.size();
        return true;
    }

    if (child->typeInfo->isArray())
    {
        const auto typeArray = castTypeInfo<TypeInfoArray>(child->typeInfo, TypeInfoKind::Array);
        if (regCount)
            regCount->u64 = typeArray->totalCount;
        return true;
    }

    return false;
}

void* ByteCodeRun::executeLocationSI(const ByteCodeRunContext* context, const ByteCodeInstruction* ip)
{
    const uint32_t paramIdx   = ip->c.u32;
    const auto     callParams = context->callerContext->whereParameters;
    if (!callParams)
        return nullptr;
    if (paramIdx >= callParams->childCount())
        return nullptr;

    const auto   child          = callParams->children[paramIdx];
    uint32_t     storageOffset  = UINT32_MAX;
    DataSegment* storageSegment = nullptr;
    ByteCodeGen::computeSourceLocation(context->callerContext, child, &storageOffset, &storageSegment, true);
    return storageSegment->address(storageOffset);
}

bool ByteCodeRun::executeIsConstExprSI(const ByteCodeRunContext* context, const ByteCodeInstruction* ip)
{
    const auto solved = reinterpret_cast<SymbolOverload*>(ip->d.pointer);

    // Variadic
    /////////////////////////////////////////
    if (solved->typeInfo->isVariadic() || solved->typeInfo->isTypedVariadic())
        return getVariadicSI(context, ip, nullptr, nullptr);

    const uint32_t paramIdx   = ip->c.u32;
    const auto     callParams = context->callerContext->whereParameters;
    if (!callParams)
        return true;
    if (paramIdx >= callParams->childCount())
        return true;

    const auto child = callParams->children[paramIdx];

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
    if (child->hasAstFlag(AST_COMPUTED_VALUE | AST_CONST_EXPR))
        return true;

    return false;
}

void ByteCodeRun::executeGetFromStackSI(ByteCodeRunContext* context, const ByteCodeInstruction* ip)
{
    // Is this constexpr ?
    const auto solved = reinterpret_cast<SymbolOverload*>(ip->d.pointer);
    if (!executeIsConstExprSI(context, ip))
    {
        callInternalCompilerError(context, ip, formErr(Err0040, solved->symbol->name.cstr(), solved->typeInfo->getDisplayNameC()));
        return;
    }

    const auto paramIdx    = ip->c.u32;
    const auto callParams  = context->callerContext->whereParameters;
    const auto registersRC = context->curRegistersRC;

    if (!callParams || paramIdx >= callParams->childCount())
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
            callInternalCompilerError(context, ip, formErr(Err0040, solved->symbol->name.cstr(), solved->typeInfo->getDisplayNameC()));
            return;
        }

        return;
    }

    const auto child = callParams->children[paramIdx];

    // Slice
    /////////////////////////////////////////
    if (solved->typeInfo->isSlice())
    {
        if (child->typeInfo->isArray())
        {
            const auto typeArray           = castTypeInfo<TypeInfoArray>(child->typeInfo, TypeInfoKind::Array);
            registersRC[ip->a.u32].pointer = nullptr;
            registersRC[ip->b.u32].u64     = typeArray->totalCount;
            return;
        }

        if (child->typeInfo->isListArray())
        {
            const auto typeList            = castTypeInfo<TypeInfoList>(child->typeInfo, TypeInfoKind::TypeListArray);
            registersRC[ip->a.u32].pointer = nullptr;
            registersRC[ip->b.u32].u64     = typeList->subTypes.size();
            return;
        }

        callInternalCompilerError(context, ip, formErr(Err0040, solved->symbol->name.cstr(), solved->typeInfo->getDisplayNameC()));
        return;
    }

    // Native
    /////////////////////////////////////////
    SWAG_ASSERT(paramIdx < callParams->childCount());
    if (solved->typeInfo->isNative())
    {
        switch (solved->typeInfo->nativeType)
        {
            case NativeTypeKind::String:
                registersRC[ip->a.u32].pointer = reinterpret_cast<uint8_t*>(child->computedValue()->text.buffer);
                registersRC[ip->b.u32].u64     = child->computedValue()->text.length();
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
                registersRC[ip->a.u32].u64 = child->computedValue()->reg.u64;
                return;

            default:
                break;
        }
    }

    callInternalCompilerError(context, ip, formErr(Err0040, solved->symbol->name.cstr(), solved->typeInfo->getDisplayNameC()));
}
