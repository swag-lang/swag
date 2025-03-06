#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Core/Crc32.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Wmf/Module.h"

#undef BYTECODE_OP
#define BYTECODE_OP(__op, __flags, __dis) {__flags, static_cast<uint32_t>(strlen(#__op)), #__op, __dis},
ByteCodeOpDesc g_ByteCodeOpDesc[] = {
#include "ByteCodeOpList.h"

};

void ByteCode::release()
{
    const auto s = Allocator::alignSize(maxInstructions * sizeof(ByteCodeInstruction));
    Allocator::free(out, s);
    out = nullptr;
}

ByteCode::Location ByteCode::getLocation(const ByteCode* bc, const ByteCodeInstruction* ip, bool wantInline)
{
    SWAG_ASSERT(bc && ip);
    if (!ip->node || !ip->node->ownerScope || !bc->sourceFile)
        return {};

    Location loc;

    loc.node = ip->node;
    loc.file = ip->node->token.sourceFile ? ip->node->token.sourceFile : bc->sourceFile;
    if (loc.file && loc.file->fileForSourceLocation)
        loc.file = loc.file->fileForSourceLocation;
    loc.location = ip->location;

    if (!wantInline &&
        ip->node->hasOwnerInline() &&
        !ip->node->hasAstFlag(AST_IN_MIXIN) &&
        ip->node->ownerInline()->ownerFct == ip->node->ownerFct)
    {
        auto n = ip->node;
        while (n->hasOwnerInline() && !n->hasAstFlag(AST_IN_MIXIN) && n->ownerInline()->ownerFct == n->ownerFct)
            n = n->ownerInline();
        loc.node     = n;
        loc.file     = n->token.sourceFile;
        loc.location = &n->token.startLocation;
    }

    return loc;
}

Utf8 ByteCode::getCallNameFromDecl()
{
    if (node)
    {
        const auto funcNode = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        return funcNode->getCallName();
    }

    return getCallName();
}

Utf8 ByteCode::getCallName()
{
    // If this is an intrinsic that can be called by the compiler itself, it cannot
    // have overloads, and the name will be the name alone (without the node address which is
    // used to differentiate overloads)
    if (node && node->token.sourceFile && node->token.sourceFile->hasFlag(FILE_RUNTIME) && !node->ownerStructScope)
    {
        const auto symbolName = node->resolvedSymbolName();
        if (symbolName && symbolName->cptOverloadsInit == 1)
        {
            return name;
        }
    }

    ScopedLock lk(mutexCallName);
    if (!callName.empty())
        return callName;

    if (name.empty())
    {
        ScopedLock lock(node->mutex);
        callName = node->getScopedName();
    }
    else
        callName = name;

    callName += form("_%lX", this);
    return callName;
}

TypeInfoFuncAttr* ByteCode::getCallType() const
{
    if (isCompilerGenerated)
        return g_TypeMgr->typeInfoOpCall;
    if (out && node && node->typeInfo->isFuncAttr())
        return castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    return typeInfoFunc;
}

void ByteCode::markLabels() const
{
    uint32_t count = numJumps;
    auto     ip    = out;
    for (uint32_t i = 0; i < numInstructions && count; i++, ip++)
    {
        if (isJump(ip))
        {
            ip[ip->b.s32 + 1].addFlag(BCI_JUMP_DEST);
            ip[1].addFlag(BCI_JUMP_DEST);
            count--;
        }
        else if (isJumpDyn(ip))
        {
            const auto table = reinterpret_cast<int32_t*>(sourceFile->module->compilerSegment.address(ip->d.u32));
            for (uint32_t idx = 0; idx < ip->c.u32; idx++)
            {
                ip[table[idx] + 1].addFlag(BCI_JUMP_DEST);
            }

            count--;
        }
    }
}

bool ByteCode::isDoingNothing() const
{
    if (!getCallType()->returnType->isVoid())
        return false;

    switch (numInstructions)
    {
        case 2:
            if (out[0].op == ByteCodeOp::Ret)
                return true;
            break;

        case 3:
            if (out[0].op == ByteCodeOp::GetParam64 &&
                out[1].op == ByteCodeOp::Ret)
                return true;

            if (out[0].op == ByteCodeOp::DecSPBP &&
                out[1].op == ByteCodeOp::Ret)
            {
                return true;
            }
            break;

        case 4:
            if (out[0].op == ByteCodeOp::ClearRA &&
                out[1].op == ByteCodeOp::InternalSetErr &&
                out[2].op == ByteCodeOp::Ret)
            {
                return true;
            }
            break;
    }

    return false;
}

void* ByteCode::doByteCodeLambda(void* ptr)
{
    uint64_t u = reinterpret_cast<uint64_t>(ptr);
    u |= SWAG_LAMBDA_BC_MARKER;
    return reinterpret_cast<void*>(u);
}

void* ByteCode::undoByteCodeLambda(void* ptr)
{
    uint64_t u = reinterpret_cast<uint64_t>(ptr);
    SWAG_ASSERT(u & SWAG_LAMBDA_BC_MARKER);
    u ^= SWAG_LAMBDA_BC_MARKER;
    return reinterpret_cast<void*>(u);
}

bool ByteCode::isByteCodeLambda(void* ptr)
{
    const uint64_t u = reinterpret_cast<uint64_t>(ptr);
    return u & SWAG_LAMBDA_BC_MARKER;
}

bool ByteCode::canEmit() const
{
    if (!node)
        return true;

    const auto funcNode = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);

    // Do we need to generate that function ?
    if (funcNode->hasAttribute(ATTRIBUTE_COMPILER))
        return false;
    if (funcNode->hasAttribute(ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
        return false;
    if (funcNode->hasAttribute(ATTRIBUTE_FOREIGN))
        return false;
    if (!funcNode->content && !funcNode->isSpecialFunctionGenerated())
        return false;

    if (funcNode->token.sourceFile->hasFlag(FILE_BOOTSTRAP) || funcNode->token.sourceFile->hasFlag(FILE_RUNTIME))
        return true;
    if (funcNode->hasAttribute(ATTRIBUTE_PUBLIC | ATTRIBUTE_MAIN_FUNC | ATTRIBUTE_INIT_FUNC | ATTRIBUTE_DROP_FUNC | ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_TEST_FUNC))
        return true;
    if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_PATCH))
        return true;

    if (!isUsed)
        return false;

    return true;
}

bool ByteCode::haveSameRead(const ByteCodeInstruction* ip0, const ByteCodeInstruction* ip1)
{
    bool sameInst = true;
    if (ip1->op != ip0->op)
        sameInst = false;
    else if (!hasWriteRegInA(ip1) && (hasReadRegInA(ip0) != hasReadRegInA(ip1) || ip1->a.u64 != ip0->a.u64))
        sameInst = false;
    else if (!hasWriteRegInB(ip1) && (hasReadRegInB(ip0) != hasReadRegInB(ip1) || ip1->b.u64 != ip0->b.u64))
        sameInst = false;
    else if (!hasWriteRegInC(ip1) && (hasReadRegInC(ip0) != hasReadRegInC(ip1) || ip1->c.u64 != ip0->c.u64))
        sameInst = false;
    else if (!hasWriteRegInD(ip1) && (hasReadRegInD(ip0) != hasReadRegInD(ip1) || ip1->d.u64 != ip0->d.u64))
        sameInst = false;

    return sameInst;
}

bool ByteCode::areSame(const ByteCodeInstruction* start0,
                       const ByteCodeInstruction* end0,
                       const ByteCodeInstruction* start1,
                       const ByteCodeInstruction* end1,
                       bool                       specialJump,
                       bool                       specialCall)
{
    bool                       same = end0 - start0 == end1 - start1;
    const ByteCodeInstruction* ip0  = start0;
    const ByteCodeInstruction* ip1  = start1;
    while (same && ip0 != end0)
    {
        if (ip0->op != ip1->op)
        {
            same = false;
            break;
        }

        const InstructionFlags flags0 = ip0->flags.maskInvert(BCI_JUMP_DEST | BCI_START_STMT);
        const InstructionFlags flags1 = ip1->flags.maskInvert(BCI_JUMP_DEST | BCI_START_STMT);
        if (flags0 != flags1)
        {
            same = false;
            break;
        }

        if (hasSomethingInC(ip0) && ip0->c.u64 != ip1->c.u64)
        {
            same = false;
            break;
        }

        if (hasSomethingInD(ip0) && ip0->d.u64 != ip1->d.u64)
        {
            same = false;
            break;
        }

        // Compare if the 2 bytes codes or their alias are the same
        if (specialCall && isLocalCall(ip0))
        {
            const ByteCode* bc0 = reinterpret_cast<ByteCode*>(ip0->a.pointer);
            const ByteCode* bc1 = reinterpret_cast<ByteCode*>(ip1->a.pointer);
            if (bc0 != bc1)
                same = false;
        }
        else if (hasSomethingInA(ip0) && ip0->a.u64 != ip1->a.u64)
        {
            same = false;
            break;
        }

        // Compare if the 2 jump destinations are the same
        if (specialJump && ip0->op == ByteCodeOp::Jump)
        {
            const auto destIp0 = ip0 + ip0->b.s32 + 1;
            const auto destIp1 = ip1 + ip1->b.s32 + 1;
            if (destIp0 != destIp1)
                same = false;
        }
        else if (hasSomethingInB(ip0) && ip0->b.u64 != ip1->b.u64)
        {
            same = false;
            break;
        }

        ip0++;
        ip1++;
    }

    return same;
}

uint32_t ByteCode::computeCrc(const ByteCodeInstruction* ip, uint32_t oldCrc, bool specialJump, bool specialCall) const
{
    oldCrc = Crc32::compute2(reinterpret_cast<const uint8_t*>(&ip->op), oldCrc);

    if (hasSomethingInC(ip))
        oldCrc = Crc32::compute8(reinterpret_cast<const uint8_t*>(&ip->c.pointer), oldCrc);
    if (hasSomethingInD(ip))
        oldCrc = Crc32::compute8(reinterpret_cast<const uint8_t*>(&ip->d.pointer), oldCrc);

    // Special call. We add the alias if it exists instead of the called bytecode
    if (specialCall && isLocalCall(ip))
    {
        const auto bc = reinterpret_cast<ByteCode*>(ip->a.pointer);
        oldCrc        = Crc32::compute8(reinterpret_cast<const uint8_t*>(&bc), oldCrc);
    }
    else if (hasSomethingInA(ip))
        oldCrc = Crc32::compute8(reinterpret_cast<const uint8_t*>(&ip->a.pointer), oldCrc);

    // For a jump, we compute the crc to go to the destination (if two jump nodes
    // are going to the same instruction, then we consider they are equal)
    if (specialJump && ip->op == ByteCodeOp::Jump)
    {
        const auto destIp = ip + ip->b.s32 + 1;
        const auto destN  = destIp - out;
        oldCrc            = Crc32::compute8(reinterpret_cast<const uint8_t*>(&destN), oldCrc);
    }
    else if (hasSomethingInB(ip))
        oldCrc = Crc32::compute8(reinterpret_cast<const uint8_t*>(&ip->b.pointer), oldCrc);

    return oldCrc;
}

void ByteCode::swapInstructions(ByteCodeInstruction* ip0, ByteCodeInstruction* ip1)
{
    std::swap(ip0->op, ip1->op);
    std::swap(ip0->flags, ip1->flags);
    std::swap(ip0->a, ip1->a);
    std::swap(ip0->b, ip1->b);
    std::swap(ip0->c, ip1->c);
    std::swap(ip0->d, ip1->d);
    std::swap(ip0->node, ip1->node);
    std::swap(ip0->location, ip1->location);

    if (ip1->hasFlag(BCI_START_STMT))
    {
        ip0->inheritFlag(ip1, BCI_START_STMT);
        ip1->removeFlag(BCI_START_STMT);
    }
}

void ByteCode::makeRoomForInstructions(uint32_t room)
{
    SWAG_RACE_CONDITION_WRITE(raceCond);

    if (numInstructions + room < maxInstructions)
        return;

    const auto oldSize = static_cast<uint32_t>(maxInstructions * sizeof(ByteCodeInstruction));
    maxInstructions    = std::max(numInstructions + room * 2, maxInstructions * 2);

    // Evaluate the first number of instructions for a given function.
    // We take the number of AST nodes in the function as a metric.
    // This is to mitigate the number of re-allocations, without wasting too much memory.
    if (!maxInstructions && node && node->is(AstNodeKind::FuncDecl))
    {
        const auto funcDecl = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        // 0.8f is kind of magical, based on various measures.
        maxInstructions = static_cast<uint32_t>(static_cast<float>(funcDecl->nodeCounts) * 0.8f);
    }

    maxInstructions            = std::max(maxInstructions, static_cast<uint32_t>(8));
    const auto newInstructions = Allocator::allocN<ByteCodeInstruction>(maxInstructions);
    std::copy_n(out, numInstructions, newInstructions);
    Allocator::free(out, oldSize);

#ifdef SWAG_STATS
    g_Stats.memInstructions -= oldSize;
    g_Stats.memInstructions += maxInstructions * sizeof(ByteCodeInstruction);
#endif

    out = newInstructions;
}

uint32_t ByteCode::getSetZeroAtPointerSize(const ByteCodeInstruction* ip, uint32_t& offset)
{
    switch (ip->op)
    {
        case ByteCodeOp::SetZeroAtPointer8:
            offset = ip->b.u32;
            return 1;
        case ByteCodeOp::SetZeroAtPointer16:
            offset = ip->b.u32;
            return 2;
        case ByteCodeOp::SetZeroAtPointer32:
            offset = ip->b.u32;
            return 4;
        case ByteCodeOp::SetZeroAtPointer64:
            offset = ip->b.u32;
            return 8;
        case ByteCodeOp::SetZeroAtPointerX:
            offset = ip->c.u32;
            return ip->b.u32;
    }

    return 0;
}

uint32_t ByteCode::getSetZeroStackSize(const ByteCodeInstruction* ip, uint32_t& offset)
{
    switch (ip->op)
    {
        case ByteCodeOp::SetZeroStack8:
            offset = ip->a.u32;
            return 1;
        case ByteCodeOp::SetZeroStack16:
            offset = ip->a.u32;
            return 2;
        case ByteCodeOp::SetZeroStack32:
            offset = ip->a.u32;
            return 4;
        case ByteCodeOp::SetZeroStack64:
            offset = ip->a.u32;
            return 8;
        case ByteCodeOp::SetZeroStackX:
            offset = ip->a.u32;
            return ip->b.u32;
    }

    return 0;
}
