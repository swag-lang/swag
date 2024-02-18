#include "pch.h"
#include "ByteCode.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Crc32.h"
#include "Log.h"
#include "Module.h"
#include "TypeManager.h"

#undef BYTECODE_OP
#define BYTECODE_OP(__op, __flags, __dis) {__flags, (uint32_t) strlen(#__op), #__op, __dis},
ByteCodeOpDesc g_ByteCodeOpDesc[] = {
#include "ByteCodeOpList.h"
};

void ByteCode::release()
{
    const auto s = Allocator::alignSize(maxInstructions * sizeof(ByteCodeInstruction));
    Allocator::free(out, s);
    out = nullptr;
}

ByteCode::Location ByteCode::getLocation(const ByteCode* bc, const ByteCodeInstruction* ip, bool getInline)
{
    SWAG_ASSERT(bc && ip);
    if (!ip->node || !ip->node->ownerScope || !bc->sourceFile)
        return {};

    Location loc;

    loc.file = ip && ip->node && ip->node->sourceFile ? ip->node->sourceFile : bc->sourceFile;
    if (loc.file && loc.file->fileForSourceLocation)
        loc.file = loc.file->fileForSourceLocation;
    loc.location = ip->location;

    if (!getInline)
    {
        if (ip->node->ownerInline && !ip->node->hasAstFlag(AST_IN_MIXIN) && ip->node->ownerInline->ownerFct == ip->node->ownerFct)
        {
            auto n = ip->node;
            while (n->ownerInline && !n->hasAstFlag(AST_IN_MIXIN) && n->ownerInline->ownerFct == n->ownerFct)
                n = n->ownerInline;
            loc.file     = n->sourceFile;
            loc.location = &n->token.startLocation;
        }
    }

    return loc;
}

Utf8 ByteCode::getPrintRefName()
{
    Utf8 str = Log::colorToVTS(LogColor::Name);
    str += getPrintName();

    const auto type = getCallType();
    if (type)
    {
        str += " ";
        str += Log::colorToVTS(LogColor::Type);
        str += type->getDisplayName();
    }

    if (!out)
        return str;

    const auto loc = getLocation(this, out);
    if (loc.file || loc.location)
        str += " ";

    if (loc.file)
    {
        str += Log::colorToVTS(LogColor::Location);
        str += loc.file->path.string().c_str();
    }

    if (loc.location)
    {
        str += Log::colorToVTS(LogColor::Location);
        str += FMT(":%d", loc.location->line - 1);
    }

    return str;
}

Utf8 ByteCode::getPrintName()
{
    if (out && node)
        return node->getScopedName();
    return name;
}

Utf8 ByteCode::getPrintFileName() const
{
    if (sourceFile)
        return sourceFile->name;
    return "";
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
    // If this is an intrinsic that can be called by the compiler itself, it should not
    // have overloads, and the name will be the name alone (without the node address which is
    // used to differentiate overloads)
    if (node && node->sourceFile && node->sourceFile->hasFlag(FILE_IS_RUNTIME_FILE) && !node->ownerStructScope)
    {
        if (node->resolvedSymbolName && node->resolvedSymbolName->cptOverloadsInit == 1)
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

    callName += FMT("_%lX", reinterpret_cast<uint64_t>(this));
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

        if (out[0].op == ByteCodeOp::SetBP &&
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

    default:
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

    if (funcNode->sourceFile->hasFlag(FILE_IS_BOOTSTRAP_FILE) || funcNode->sourceFile->hasFlag(FILE_IS_RUNTIME_FILE))
        return true;
    if (funcNode->hasAttribute(ATTRIBUTE_PUBLIC | ATTRIBUTE_MAIN_FUNC | ATTRIBUTE_INIT_FUNC | ATTRIBUTE_DROP_FUNC | ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_TEST_FUNC))
        return true;
    if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_PATCH))
        return true;

    if (!isUsed)
        return false;

    return true;
}

bool ByteCode::areSame(ByteCodeInstruction*       start0,
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

        const uint32_t flags0 = ip0->flags & ~(BCI_JUMP_DEST | BCI_START_STMT);
        const uint32_t flags1 = ip1->flags & ~(BCI_JUMP_DEST | BCI_START_STMT);
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
        if (specialCall && (ip0->op == ByteCodeOp::LocalCall ||
                            ip0->op == ByteCodeOp::LocalCallPop ||
                            ip0->op == ByteCodeOp::LocalCallPopParam ||
                            ip0->op == ByteCodeOp::LocalCallPopRC))
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
    const uint32_t flags = ip->flags & ~(BCI_JUMP_DEST | BCI_START_STMT);
    oldCrc               = Crc32::compute2(reinterpret_cast<const uint8_t*>(&ip->op), oldCrc);
    oldCrc               = Crc32::compute2(reinterpret_cast<const uint8_t*>(&flags), oldCrc);

    if (hasSomethingInC(ip))
        oldCrc = Crc32::compute8(reinterpret_cast<const uint8_t*>(&ip->c.pointer), oldCrc);
    if (hasSomethingInD(ip))
        oldCrc = Crc32::compute8(reinterpret_cast<const uint8_t*>(&ip->d.pointer), oldCrc);

    // Special call. We add the alias if it exists instead of the called bytecode
    if (specialCall && (ip->op == ByteCodeOp::LocalCall ||
                        ip->op == ByteCodeOp::LocalCallPop ||
                        ip->op == ByteCodeOp::LocalCallPopParam ||
                        ip->op == ByteCodeOp::LocalCallPopRC))
    {
        const auto bc = reinterpret_cast<ByteCode*>(ip->a.pointer);
        oldCrc        = Crc32::compute8(reinterpret_cast<const uint8_t*>(&bc), oldCrc);
    }
    else if (hasSomethingInA(ip))
        oldCrc = Crc32::compute8(reinterpret_cast<const uint8_t*>(&ip->a.pointer), oldCrc);

    // For a jump, we compute the crc to go the destination (if two jump nodes
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

void ByteCode::makeRoomForInstructions(uint32_t room)
{
    SWAG_RACE_CONDITION_WRITE(raceCond);

    if (numInstructions + room < maxInstructions)
        return;

    const auto oldSize = static_cast<int>(maxInstructions * sizeof(ByteCodeInstruction));
    maxInstructions    = max(numInstructions + room * 2, maxInstructions * 2);

    // Evaluate the first number of instructions for a given function.
    // We take the number of ast nodes in the function as a metric.
    // This is to mitigate the number of re-allocations, without wasting too much memory.
    if (!maxInstructions && node && node->kind == AstNodeKind::FuncDecl)
    {
        const auto funcDecl = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        // 0.8f is kind of magical, based on various measures.
        maxInstructions = static_cast<int>(static_cast<float>(funcDecl->nodeCounts) * 0.8f);
    }

    maxInstructions            = max(maxInstructions, 8);
    const auto newInstructions = static_cast<ByteCodeInstruction*>(Allocator::alloc(maxInstructions * sizeof(ByteCodeInstruction)));
    std::copy_n(out, numInstructions, newInstructions);
    Allocator::free(out, oldSize);

#ifdef SWAG_STATS
    g_Stats.memInstructions -= oldSize;
    g_Stats.memInstructions += maxInstructions * sizeof(ByteCodeInstruction);
#endif

    out = newInstructions;
}

uint32_t ByteCode::getSetZeroAtPointerSize(const ByteCodeInstruction* inst, uint32_t& offset)
{
    switch (inst->op)
    {
    case ByteCodeOp::SetZeroAtPointer8:
        offset = inst->b.u32;
        return 1;
    case ByteCodeOp::SetZeroAtPointer16:
        offset = inst->b.u32;
        return 2;
    case ByteCodeOp::SetZeroAtPointer32:
        offset = inst->b.u32;
        return 4;
    case ByteCodeOp::SetZeroAtPointer64:
        offset = inst->b.u32;
        return 8;
    case ByteCodeOp::SetZeroAtPointerX:
        offset = inst->c.u32;
        return inst->b.u32;
    default:
        break;
    }

    return 0;
}

uint32_t ByteCode::getSetZeroStackSize(const ByteCodeInstruction* inst, uint32_t& offset)
{
    switch (inst->op)
    {
    case ByteCodeOp::SetZeroStack8:
        offset = inst->a.u32;
        return 1;
    case ByteCodeOp::SetZeroStack16:
        offset = inst->a.u32;
        return 2;
    case ByteCodeOp::SetZeroStack32:
        offset = inst->a.u32;
        return 4;
    case ByteCodeOp::SetZeroStack64:
        offset = inst->a.u32;
        return 8;
    case ByteCodeOp::SetZeroStackX:
        offset = inst->a.u32;
        return inst->b.u32;
    default:
        break;
    }

    return 0;
}
