#include "pch.h"
#include "ByteCode.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "Crc32.h"

#undef BYTECODE_OP
#define BYTECODE_OP(__op, __flags, __dis) {__flags, (uint32_t) strlen(#__op), #__op, __dis},
ByteCodeOpDesc g_ByteCodeOpDesc[] = {
#include "ByteCodeOpList.h"
};

void ByteCode::release()
{
    auto s = Allocator::alignSize(maxInstructions * sizeof(ByteCodeInstruction));
    Allocator::free(out, s);
    out = nullptr;
}

ByteCode::Location ByteCode::getLocation(ByteCode* bc, ByteCodeInstruction* ip, bool getInline)
{
    SWAG_ASSERT(bc && ip);
    if (!ip->node || !ip->node->ownerScope || !bc->sourceFile)
        return {};

    ByteCode::Location loc;

    loc.file = ip && ip->node && ip->node->sourceFile ? ip->node->sourceFile : bc->sourceFile;
    if (loc.file && loc.file->fileForSourceLocation)
        loc.file = loc.file->fileForSourceLocation;
    loc.location = ip->location;

    bool zapInline = !bc->sourceFile->module->buildCfg.byteCodeDebugInline && !getInline;
    if (zapInline)
    {
        if (ip->node->ownerInline && !(ip->node->flags & AST_IN_MIXIN) && ip->node->ownerInline->ownerFct == ip->node->ownerFct)
        {
            auto n = ip->node;
            while (n->ownerInline && !(n->flags & AST_IN_MIXIN) && (n->ownerInline->ownerFct == n->ownerFct))
                n = n->ownerInline;
            loc.file     = n->sourceFile;
            loc.location = &n->token.startLocation;
        }
    }

    return loc;
}

Utf8 ByteCode::getCallNameFromDecl()
{
    if (node)
    {
        auto funcNode = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        return funcNode->getCallName();
    }

    return getCallName();
}

Utf8 ByteCode::getCallName()
{
    if (alias)
        return alias->getCallName();

    ScopedLock lk(mutexCallName);
    if (!callName.empty())
        return callName;

    // If this is an intrinsic that can be called by the compiler itself, it should not
    // have overloads, and the name will be the name alone (without the node address which is
    // used to differentiate overloads)
    if (node && node->sourceFile->isRuntimeFile && !node->ownerStructScope)
    {
        if (node->resolvedSymbolName && node->resolvedSymbolName->cptOverloadsInit == 1)
        {
            callName = name;
            return callName;
        }
    }

    if (name.empty())
    {
        ScopedLock lock(node->mutex);
        callName = node->getScopedName();
    }
    else
        callName = name;

    callName += Fmt("_%lX", (uint64_t) this);
    return callName;
}

TypeInfoFuncAttr* ByteCode::getCallType()
{
    if (isCompilerGenerated)
        return g_TypeMgr->typeInfoOpCall;
    if (alias)
        return alias->getCallType();
    if (node && node->typeInfo->isFuncAttr())
        return CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    return typeInfoFunc;
}

void ByteCode::markLabels()
{
    uint32_t count = numJumps;
    auto     ip    = out;
    for (uint32_t i = 0; i < numInstructions && count; i++, ip++)
    {
        if (ByteCode::isJump(ip))
        {
            ip[ip->b.s32 + 1].flags |= BCI_JUMP_DEST;
            ip[1].flags |= BCI_JUMP_DEST;
            count--;
        }
        else if (ByteCode::isJumpDyn(ip))
        {
            int32_t* table = (int32_t*) sourceFile->module->compilerSegment.address(ip->d.u32);
            for (uint32_t idx = 0; idx < ip->c.u32; idx++)
            {
                ip[table[idx] + 1].flags |= BCI_JUMP_DEST;
            }

            count--;
        }
    }
}

bool ByteCode::isDoingNothing()
{
    if (!getCallType()->returnType->isVoid())
        return false;

    if (numInstructions == 2)
    {
        if (out[0].op == ByteCodeOp::Ret)
            return true;
    }

    if (numInstructions == 3)
    {
        if (out[0].op == ByteCodeOp::GetParam64 &&
            out[1].op == ByteCodeOp::Ret)
            return true;
    }

    if (numInstructions == 3)
    {
        if (out[0].op == ByteCodeOp::DecSPBP &&
            out[1].op == ByteCodeOp::Ret)
        {
            return true;
        }
    }

    if (numInstructions == 3)
    {
        if (out[0].op == ByteCodeOp::SetBP &&
            out[1].op == ByteCodeOp::Ret)
        {
            return true;
        }
    }

    if (numInstructions == 4)
    {
        if (out[0].op == ByteCodeOp::ClearRA &&
            out[1].op == ByteCodeOp::InternalSetErr &&
            out[2].op == ByteCodeOp::Ret)
        {
            return true;
        }
    }

    return false;
}

void* ByteCode::doByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    u |= SWAG_LAMBDA_BC_MARKER;
    return (void*) u;
}

void* ByteCode::undoByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    SWAG_ASSERT(u & SWAG_LAMBDA_BC_MARKER);
    u ^= SWAG_LAMBDA_BC_MARKER;
    return (void*) u;
}

bool ByteCode::isByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    return u & SWAG_LAMBDA_BC_MARKER;
}

bool ByteCode::canEmit()
{
    if (!node)
        return true;

    auto funcNode = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);

    // Do we need to generate that function ?
    if (funcNode->attributeFlags & ATTRIBUTE_COMPILER)
        return false;
    if ((funcNode->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
        return false;
    if (funcNode->attributeFlags & ATTRIBUTE_FOREIGN)
        return false;
    if (!funcNode->content && !funcNode->isSpecialFunctionGenerated())
        return false;

    if (funcNode->sourceFile->isBootstrapFile || funcNode->sourceFile->isRuntimeFile)
        return true;
    if (funcNode->attributeFlags & (ATTRIBUTE_PUBLIC | ATTRIBUTE_MAIN_FUNC | ATTRIBUTE_INIT_FUNC | ATTRIBUTE_DROP_FUNC | ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_TEST_FUNC))
        return true;
    if (funcNode->specFlags & AstFuncDecl::SPECFLAG_PATCH)
        return true;

    if (!isUsed)
        return false;

    return true;
}

bool ByteCode::areSame(ByteCodeInstruction* start0, ByteCodeInstruction* end0, ByteCodeInstruction* start1, ByteCodeInstruction* end1, bool specialJump, bool specialCall)
{
    bool                 same = end0 - start0 == end1 - start1;
    ByteCodeInstruction* ip0  = start0;
    ByteCodeInstruction* ip1  = start1;
    while (same && ip0 != end0)
    {
        if (ip0->op != ip1->op)
        {
            same = false;
            break;
        }

        uint32_t flags0 = ip0->flags & ~(BCI_JUMP_DEST | BCI_START_STMT);
        uint32_t flags1 = ip1->flags & ~(BCI_JUMP_DEST | BCI_START_STMT);
        if (flags0 != flags1)
        {
            same = false;
            break;
        }

        if (ByteCode::hasSomethingInC(ip0) && ip0->c.u64 != ip1->c.u64)
        {
            same = false;
            break;
        }

        if (ByteCode::hasSomethingInD(ip0) && ip0->d.u64 != ip1->d.u64)
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
            ByteCode* bc0 = (ByteCode*) ip0->a.u64;
            ByteCode* bc1 = (ByteCode*) ip1->a.u64;
            if (bc0 && bc0->alias)
                bc0 = bc0->alias;
            if (bc1 && bc1->alias)
                bc1 = bc1->alias;
            if (bc0 != bc1)
                same = false;
        }
        else if (ByteCode::hasSomethingInA(ip0) && ip0->a.u64 != ip1->a.u64)
        {
            same = false;
            break;
        }

        // Compare if the 2 jump destinations are the same
        if (specialJump && ip0->op == ByteCodeOp::Jump)
        {
            auto destIp0 = ip0 + ip0->b.s32 + 1;
            auto destIp1 = ip1 + ip1->b.s32 + 1;
            if (destIp0 != destIp1)
                same = false;
        }
        else if (ByteCode::hasSomethingInB(ip0) && ip0->b.u64 != ip1->b.u64)
        {
            same = false;
            break;
        }

        ip0++;
        ip1++;
    }

    return same;
}

uint32_t ByteCode::computeCrc(ByteCodeInstruction* ip, uint32_t oldCrc, bool specialJump, bool specialCall)
{
    oldCrc = Crc32::compute2((const uint8_t*) &ip->op, oldCrc);

    uint32_t flags = ip->flags & ~(BCI_JUMP_DEST | BCI_START_STMT);
    oldCrc         = Crc32::compute2((const uint8_t*) &flags, oldCrc);

    if (ByteCode::hasSomethingInC(ip))
        oldCrc = Crc32::compute8((const uint8_t*) &ip->c.u64, oldCrc);
    if (ByteCode::hasSomethingInD(ip))
        oldCrc = Crc32::compute8((const uint8_t*) &ip->d.u64, oldCrc);

    // Special call. We add the alias if it exitsts instead of the called bytecode
    if (specialCall && (ip->op == ByteCodeOp::LocalCall ||
                        ip->op == ByteCodeOp::LocalCallPop ||
                        ip->op == ByteCodeOp::LocalCallPopParam ||
                        ip->op == ByteCodeOp::LocalCallPopRC))
    {
        ByteCode* bc = (ByteCode*) ip->a.u64;
        if (bc && bc->alias)
            oldCrc = Crc32::compute8((const uint8_t*) &bc->alias, oldCrc);
        else
            oldCrc = Crc32::compute8((const uint8_t*) &bc, oldCrc);
    }
    else if (ByteCode::hasSomethingInA(ip))
        oldCrc = Crc32::compute8((const uint8_t*) &ip->a.u64, oldCrc);

    // For a jump, we compute the crc to go the the destination (if two jump nodes
    // are going to the same instruction, then we consider they are equal)
    if (specialJump && ip->op == ByteCodeOp::Jump)
    {
        auto destIp = ip + ip->b.s32 + 1;
        auto destN  = destIp - out;
        oldCrc      = Crc32::compute8((const uint8_t*) &destN, oldCrc);
    }
    else if (ByteCode::hasSomethingInB(ip))
        oldCrc = Crc32::compute8((const uint8_t*) &ip->b.u64, oldCrc);

    return oldCrc;
}