#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"

namespace
{
    // If a CopyRBRA is followed by the same CopyRBRA, and between them there's no write to RA or RB,
    // then the second CopyRBRA is useless
    void optimizePassDupCopyRBRAOp(ByteCodeOptContext* context, ByteCodeOp op)
    {
        auto& mapCopyRA = context->mapRegInstA;
        auto& mapCopyRB = context->mapRegInstB;
        mapCopyRA.clear();
        mapCopyRB.clear();

        for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
        {
            if (ip->hasFlag(BCI_START_STMT))
            {
                mapCopyRA.clear();
                mapCopyRB.clear();
            }

            const auto flags = ByteCode::opFlags(ip->op);

            if (ip->op == ByteCodeOp::CopyRBtoRA64)
            {
                // CopyRBRA followed by one single PushRAParam, take the original register
                if (ip[1].op == ByteCodeOp::PushRAParam &&
                    ip[0].a.u32 == ip[1].a.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    ip[1].a.u32 = ip[0].b.u32;
                    context->setDirtyPass();
                }

                // 2 CopyRBRA followed by one single PushRAParam2, take the original registers
                if (ip[1].op == ByteCodeOp::CopyRBtoRA64 &&
                    ip[2].op == ByteCodeOp::PushRAParam2 &&
                    ip[0].a.u32 == ip[2].b.u32 &&
                    ip[1].a.u32 == ip[2].a.u32 &&
                    !ip[0].hasFlag(BCI_START_STMT) &&
                    !ip[1].hasFlag(BCI_START_STMT) &&
                    !ip[2].hasFlag(BCI_START_STMT))
                {
                    ip[2].a.u32 = ip[1].b.u32;
                    ip[2].b.u32 = ip[0].b.u32;
                    context->setDirtyPass();
                }
            }

            if (ip->op == op)
            {
                // CopyRBRA X, X
                if (ip->a.u32 == ip->b.u32)
                {
                    ByteCodeOptimizer::setNop(context, ip);
                }

                // CopyRBRA X, Y followed by CopyRBRA Y, X
                if (ip[1].op == op &&
                    ip->a.u32 == ip[1].b.u32 &&
                    ip->b.u32 == ip[1].a.u32)
                {
                    ByteCodeOptimizer::setNop(context, ip + 1);
                }

                const auto it = mapCopyRA.find(ip->a.u32);
                if (it)
                {
                    const auto it1 = mapCopyRB.find(ip->b.u32);
                    if (it1 && *it1 == *it)
                        ByteCodeOptimizer::setNop(context, ip);
                }

                mapCopyRB.remove(ip->a.u32);
                mapCopyRA.set(ip->a.u32, ip);
                mapCopyRB.set(ip->b.u32, ip);
                continue;
            }

            if (!mapCopyRA.count() && !mapCopyRB.count())
                continue;

            // If we use a register that comes from a CopyRBRA, then use the initial register instead.
            // That way, the Copy can become dead store and removed later.
            // Specific instructions only, as ByteCodeOp::CopyRBtoRA32 will clear the high 32 bit, if the following instruction
            // deal with 64 bits, this could be a pb not to do that clean by just using the original 32 bits register.
            if (op == ByteCodeOp::CopyRBtoRA32)
            {
                switch (ip->op)
                {
                    case ByteCodeOp::JumpIfZero32:
                    case ByteCodeOp::JumpIfNotZero32:
                        if (ip->hasReadRegInA() && !ip->hasWriteRegInA())
                        {
                            const auto it = mapCopyRA.find(ip->a.u32);
                            if (it)
                            {
                                const auto it1 = mapCopyRB.find((*it)->b.u32);
                                if (it1 && *it == *it1)
                                {
                                    ip->a.u32 = (*it)->b.u32;
                                    context->setDirtyPass();
                                }
                            }
                        }
                        break;

                    case ByteCodeOp::SetAtPointer32:
                    case ByteCodeOp::SetAtStackPointer32:
                    case ByteCodeOp::AffectOpPlusEqS32:
                        if (ip->hasReadRegInB() && !ip->hasWriteRegInB())
                        {
                            const auto it = mapCopyRA.find(ip->b.u32);
                            if (it)
                            {
                                const auto it1 = mapCopyRB.find((*it)->b.u32);
                                if (it1 && *it == *it1)
                                {
                                    ip->b.u32 = (*it)->b.u32;
                                    context->setDirtyPass();
                                }
                            }
                        }
                        break;

                    default:
                        break;
                }
            }
            else if (op == ByteCodeOp::CopyRBtoRA64)
            {
                // If we use a register that comes from a CopyRBRA, then use the initial
                // register instead (that way, the Copy can become dead store and removed later)
                // Do it only for CopyRBtoRA64, as other copy have an implicit cast
                // *NOT* for PushRAParam, because the register numbers are important in case of variadic parameters.
                // *NOT* for CopySP, because the register numbers are important in case of variadic parameters.
                if ((ip->op != ByteCodeOp::PushRAParam || !ip->hasFlag(BCI_VARIADIC)) &&
                    (ip->op != ByteCodeOp::PushRAParam2 || !ip->hasFlag(BCI_VARIADIC)) &&
                    (ip->op != ByteCodeOp::PushRAParam3 || !ip->hasFlag(BCI_VARIADIC)) &&
                    (ip->op != ByteCodeOp::PushRAParam4 || !ip->hasFlag(BCI_VARIADIC)) &&
                    ip->op != ByteCodeOp::CopySP)
                {
                    if (ip->hasReadRegInA() && !ip->hasWriteRegInA())
                    {
                        const auto it = mapCopyRA.find(ip->a.u32);
                        if (it)
                        {
                            const auto it1 = mapCopyRB.find((*it)->b.u32);
                            if (it1 && *it == *it1)
                            {
                                ip->a.u32 = (*it)->b.u32;
                                context->setDirtyPass();
                            }
                        }
                    }

                    if (ip->hasReadRegInB() && !ip->hasWriteRegInB())
                    {
                        const auto it = mapCopyRA.find(ip->b.u32);
                        if (it)
                        {
                            const auto it1 = mapCopyRB.find((*it)->b.u32);
                            if (it1 && *it == *it1)
                            {
                                ip->b.u32 = (*it)->b.u32;
                                context->setDirtyPass();
                            }
                        }
                    }

                    if (ip->hasReadRegInC() && !ip->hasWriteRegInC())
                    {
                        const auto it = mapCopyRA.find(ip->c.u32);
                        if (it)
                        {
                            const auto it1 = mapCopyRB.find((*it)->b.u32);
                            if (it1 && *it == *it1)
                            {
                                ip->c.u32 = (*it)->b.u32;
                                context->setDirtyPass();
                            }
                        }
                    }

                    if (ip->hasReadRegInD() && !ip->hasWriteRegInD())
                    {
                        const auto it = mapCopyRA.find(ip->d.u32);
                        if (it)
                        {
                            const auto it1 = mapCopyRB.find((*it)->b.u32);
                            if (it1 && *it == *it1)
                            {
                                ip->d.u32 = (*it)->b.u32;
                                context->setDirtyPass();
                            }
                        }
                    }
                }
            }

            // Reset CopyRARB Map
            if (ip->isJump())
            {
                mapCopyRA.clear();
                mapCopyRB.clear();
            }

            if (flags.has(OPF_WRITE_A) && !ip->hasFlag(BCI_IMM_A))
            {
                const auto it = mapCopyRA.find(ip->a.u32);
                if (it)
                {
                    const auto it1 = mapCopyRB.find((*it)->b.u32);
                    if (it1 && *it == *it1)
                        mapCopyRB.remove((*it)->b.u32);
                    mapCopyRA.remove(ip->a.u32);
                }

                mapCopyRB.remove(ip->a.u32);
            }

            if (flags.has(OPF_WRITE_B) && !ip->hasFlag(BCI_IMM_B))
            {
                const auto it = mapCopyRA.find(ip->b.u32);
                if (it)
                {
                    const auto it1 = mapCopyRB.find((*it)->b.u32);
                    if (it1 && *it == *it1)
                        mapCopyRB.remove((*it)->b.u32);
                    mapCopyRA.remove(ip->b.u32);
                }

                mapCopyRB.remove(ip->b.u32);
            }

            if (flags.has(OPF_WRITE_C) && !ip->hasFlag(BCI_IMM_C))
            {
                const auto it = mapCopyRA.find(ip->c.u32);
                if (it)
                {
                    const auto it1 = mapCopyRB.find((*it)->b.u32);
                    if (it1 && *it == *it1)
                        mapCopyRB.remove((*it)->b.u32);
                    mapCopyRA.remove(ip->c.u32);
                }

                mapCopyRB.remove(ip->c.u32);
            }

            if (flags.has(OPF_WRITE_D) && !ip->hasFlag(BCI_IMM_D))
            {
                const auto it = mapCopyRA.find(ip->d.u32);
                if (it)
                {
                    const auto it1 = mapCopyRB.find((*it)->b.u32);
                    if (it1 && *it == *it1)
                        mapCopyRB.remove((*it)->b.u32);
                    mapCopyRA.remove(ip->d.u32);
                }

                mapCopyRB.remove(ip->d.u32);
            }
        }
    }
}

bool ByteCodeOptimizer::optimizePassDupCopyRBRA(ByteCodeOptContext* context)
{
    // See setContextFlags if you add one instruction
    if (!(context->contextBcFlags & OCF_HAS_COPY_RBRA))
        return true;

    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA8);
    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA16);
    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA32);
    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA64);
    return true;
}

namespace
{
    void optimizePassDupSetRAOp(ByteCodeOptContext* context, ByteCodeOp op)
    {
        auto& mapRA = context->mapRegInstA;
        mapRA.clear();

        for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
        {
            if (ip->hasFlag(BCI_START_STMT) || ip->isJump())
                mapRA.clear();

            if (ip->op == op)
            {
                const auto it = mapRA.find(ip->a.u32);
                if (it)
                {
                    if ((*it)->b.u64 == ip->b.u64 &&
                        (*it)->c.u64 == ip->c.u64 &&
                        (*it)->d.u64 == ip->d.u64 &&
                        (*it)->flags == ip->flags)
                    {
                        ByteCodeOptimizer::setNop(context, ip);
                        mapRA.remove(ip->a.u32);
                        continue;
                    }
                }

                mapRA.set(ip->a.u32, ip);
            }
            else if (mapRA.count())
            {
                if (ip->hasWriteRegInA())
                    mapRA.remove(ip->a.u32);
                if (ip->hasWriteRegInB())
                    mapRA.remove(ip->b.u32);
                if (ip->hasWriteRegInC())
                    mapRA.remove(ip->c.u32);
                if (ip->hasWriteRegInD())
                    mapRA.remove(ip->d.u32);
            }
        }
    }
}

// Remove duplicated pure instructions (set RA to a constant)
bool ByteCodeOptimizer::optimizePassDupSetRA(ByteCodeOptContext* context)
{
    // See setContextFlags if you add one instruction
    if (!(context->contextBcFlags & OCF_HAS_DUP_COPY))
        return true;

    optimizePassDupSetRAOp(context, ByteCodeOp::CopyRRtoRA);

    optimizePassDupSetRAOp(context, ByteCodeOp::GetParam64);
    optimizePassDupSetRAOp(context, ByteCodeOp::GetIncParam64);
    optimizePassDupSetRAOp(context, ByteCodeOp::MakeStackPointer);
    optimizePassDupSetRAOp(context, ByteCodeOp::MakeCompilerSegPointer);
    optimizePassDupSetRAOp(context, ByteCodeOp::MakeConstantSegPointer);
    optimizePassDupSetRAOp(context, ByteCodeOp::MakeBssSegPointer);
    optimizePassDupSetRAOp(context, ByteCodeOp::MakeMutableSegPointer);
    optimizePassDupSetRAOp(context, ByteCodeOp::MakeLambda);
    optimizePassDupSetRAOp(context, ByteCodeOp::ClearRA);
    optimizePassDupSetRAOp(context, ByteCodeOp::SetImmediate32);
    optimizePassDupSetRAOp(context, ByteCodeOp::SetImmediate64);

    return true;
}

// Remove duplicated pure instructions (set RA to a constant)
bool ByteCodeOptimizer::optimizePassDupInstruction(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (!ip->hasOpFlag(OPF_REG_ONLY | OPF_REG_READ))
            continue;
        if (ip->countWriteRegs() > 1)
            continue;

        uint32_t reg;
        if (ip->hasWriteRegInA())
            reg = ip->a.u32;
        else if (ip->hasWriteRegInB())
            reg = ip->b.u32;
        else if (ip->hasWriteRegInC())
            reg = ip->c.u32;
        else
            reg = ip->d.u32;

        if (ip->hasReadRefToReg(reg))
            continue;

        for (auto ipScan = ip + 1; ipScan->op != ByteCodeOp::End; ipScan++)
        {
            if (!ipScan->hasOpFlag(OPF_REG_ONLY | OPF_REG_READ | OPF_JUMP))
                break;
            if (ipScan->hasFlag(BCI_START_STMT))
                break;

            if (ip->hasReadRegInA() && ipScan->hasWriteRefToReg(ip->a.u32))
                break;
            if (ip->hasReadRegInB() && ipScan->hasWriteRefToReg(ip->b.u32))
                break;
            if (ip->hasReadRegInC() && ipScan->hasWriteRefToReg(ip->c.u32))
                break;
            if (ip->hasReadRegInD() && ipScan->hasWriteRefToReg(ip->d.u32))
                break;

            const bool sameInst = ByteCode::haveSameRead(ip, ipScan);
            if (!sameInst && ipScan->hasWriteRefToReg(reg))
                break;
            if (!sameInst)
                continue;

            // Exact same instructions, it's useless, as we only have "pure" instructions between the 2
            // (i.e. only instructions that read something, or modify a register)
            if ((ipScan->hasWriteRegInA() && ipScan->a.u32 == reg) ||
                (ipScan->hasWriteRegInB() && ipScan->b.u32 == reg) ||
                (ipScan->hasWriteRegInC() && ipScan->c.u32 == reg) ||
                (ipScan->hasWriteRegInD() && ipScan->d.u32 == reg))
            {
                setNop(context, ipScan);
                break;
            }

            uint32_t writeReg;
            if (ipScan->hasWriteRegInA())
                writeReg = ipScan->a.u32;
            else if (ipScan->hasWriteRegInB())
                writeReg = ipScan->b.u32;
            else if (ipScan->hasWriteRegInC())
                writeReg = ipScan->c.u32;
            else
                writeReg = ipScan->d.u32;

            ipScan += 1;
            while (ipScan->op != ByteCodeOp::End)
            {
                if (!ipScan->hasOpFlag(OPF_REG_ONLY | OPF_REG_READ | OPF_JUMP))
                    break;
                if (ipScan->hasFlag(BCI_START_STMT))
                    break;

                if (ipScan->hasReadRefToRegA(writeReg) && !ipScan->hasWriteRefToRegA(writeReg))
                {
                    ipScan->a.u32 = reg;
                    context->setDirtyPass();
                }

                if (ipScan->hasReadRefToRegB(writeReg) && !ipScan->hasWriteRefToRegB(writeReg))
                {
                    ipScan->b.u32 = reg;
                    context->setDirtyPass();
                }

                if (ipScan->hasReadRefToRegC(writeReg) && !ipScan->hasWriteRefToRegC(writeReg))
                {
                    ipScan->c.u32 = reg;
                    context->setDirtyPass();
                }

                if (ipScan->hasReadRefToRegD(writeReg) && !ipScan->hasWriteRefToRegD(writeReg))
                {
                    ipScan->d.u32 = reg;
                    context->setDirtyPass();
                }

                if (ipScan->hasWriteRefToReg(writeReg))
                    break;
                if (ipScan->hasWriteRefToReg(reg))
                    break;

                ipScan += 1;
            }

            break;
        }
    }

    return true;
}
