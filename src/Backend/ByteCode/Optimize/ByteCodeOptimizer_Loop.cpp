#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "Wmf/Module.h"

namespace
{
    ByteCodeInstruction* findLoop(ByteCodeOptContext* context, ByteCodeInstruction* ip, bool& hasJumps)
    {
        // Find a negative jump
        if (!ip->isJump() || ip->b.s32 > 0)
            return nullptr;

        context->vecReg.clear();

        const auto ipStart = ip + ip->b.s32 + 1;
        auto       ipScan  = ipStart;

        // Zap non jump instructions
        while (!ipScan->isJump())
            ipScan++;
        if (ipScan == ip)
            return nullptr;

        // We should have a jump to the instruction right after the original negative jump, or
        // this is not a loop
        const auto ipEnd = ipScan + ipScan->b.s32 + 1;
        if (ipEnd != ip + 1)
            return nullptr;

        const auto ipExitJump = ipScan;
        ipScan                = ipStart;

        while (ipScan != ip)
        {
            // Test an inside jump that will escape the loop
            if (ipScan->isJump() && ipScan != ipExitJump && ipScan != ip)
            {
                hasJumps        = true;
                const auto test = ipScan + ipScan->b.s32 + 1;
                if (test < ipStart)
                    break;
            }

            // A jump dyn inside, just dismiss
            if (ipScan->isJumpDyn())
                break;

            if (ipScan->hasWriteRegInA())
            {
                context->vecReg.expand_clear(ipScan->a.u32 + 1);
                context->vecReg[ipScan->a.u32]++;
            }
            if (ipScan->hasWriteRegInB())
            {
                context->vecReg.expand_clear(ipScan->b.u32 + 1);
                context->vecReg[ipScan->b.u32]++;
            }
            if (ipScan->hasWriteRegInC())
            {
                context->vecReg.expand_clear(ipScan->c.u32 + 1);
                context->vecReg[ipScan->c.u32]++;
            }
            if (ipScan->hasWriteRegInD())
            {
                context->vecReg.expand_clear(ipScan->d.u32 + 1);
                context->vecReg[ipScan->d.u32]++;
            }

            ipScan++;
        }

        // Stop before the end of the loop, bad one...
        if (ipScan != ip)
            return nullptr;

        // Be sure there's no external jump inside that loop
        for (const auto it : context->jumps)
        {
            if (it == ipStart || it == ipScan)
                continue;
            if (!ipScan)
                break;
            if (it->isJumpDyn())
            {
                const auto table = reinterpret_cast<int32_t*>(context->module->compilerSegment.address(it->d.u32));
                for (uint32_t i = 0; i < it->c.u32; i++)
                {
                    const auto ipJump = it + table[i] + 1;
                    if (ipJump >= ipStart && ipJump <= ipScan)
                    {
                        if (it < ipStart || it > ipScan)
                            return nullptr;
                    }
                }
            }
            else
            {
                const auto ipJump = it + it->b.s32 + 1;
                if (ipJump >= ipStart && ipJump <= ipScan)
                {
                    if (it < ipStart || it > ipScan)
                        return nullptr;
                }
            }
        }

        return ipStart;
    }

    void replaceReg(ByteCodeOptContext* context, ByteCodeInstruction* ipScan, const ByteCodeInstruction* ipEnd, uint32_t reg, uint32_t newReg)
    {
        while (ipScan != ipEnd)
        {
            if (ipScan->isRet())
                break;
            if (ipScan->isJump() && context->vecReg[reg] > 1)
                break;
            if (ipScan->hasFlag(BCI_START_STMT) && context->vecReg[reg] > 1)
                break;

            if (ipScan->hasReadRefToRegA(reg) && !ipScan->hasWriteRefToRegA(reg))
                ipScan->a.u32 = newReg;
            if (ipScan->hasReadRefToRegB(reg) && !ipScan->hasWriteRefToRegB(reg))
                ipScan->b.u32 = newReg;
            if (ipScan->hasReadRefToRegC(reg) && !ipScan->hasWriteRefToRegC(reg))
                ipScan->c.u32 = newReg;
            if (ipScan->hasReadRefToRegD(reg) && !ipScan->hasWriteRefToRegD(reg))
                ipScan->d.u32 = newReg;

            if (ipScan->hasWriteRefToReg(reg))
                break;
            ipScan += 1;
        }
    }
}

// We detect loops, and then we try to move constant instructions outside of the loop.
// For example a GetParam64 does not need to be done at each iteration, it could be done once before the loop.
bool ByteCodeOptimizer::optimizePassLoop(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        bool hasJumps = false;
        auto ipStart  = findLoop(context, ip, hasJumps);
        if (!ipStart)
            continue;

        context->vecInst.clear();

        auto ipScan = ipStart;
        while (ipScan != ip)
        {
            // Constant expression
            switch (ipScan->op)
            {
                case ByteCodeOp::GetParam8:
                case ByteCodeOp::GetParam16:
                case ByteCodeOp::GetParam32:
                case ByteCodeOp::GetParam64:
                case ByteCodeOp::GetIncParam64:
                case ByteCodeOp::MakeStackPointer:
                case ByteCodeOp::MakeCompilerSegPointer:
                case ByteCodeOp::MakeConstantSegPointer:
                case ByteCodeOp::MakeBssSegPointer:
                case ByteCodeOp::MakeMutableSegPointer:
                case ByteCodeOp::MakeLambda:
                case ByteCodeOp::ClearRA:
                case ByteCodeOp::SetImmediate32:
                case ByteCodeOp::SetImmediate64:
                case ByteCodeOp::CopyRBtoRA64:
                    context->vecInst.push_back(ipScan);
                    break;

                default:
                    if (ipScan->hasOpFlag(OPF_REG_ONLY) && ipScan->countWriteRegs() == 1 && ipScan->countReadRegs())
                    {
                        if ((!ipScan->hasReadRegInA() || ipScan->a.u32 >= context->vecReg.size() || context->vecReg[ipScan->a.u32] == 0) &&
                            (!ipScan->hasReadRegInB() || ipScan->b.u32 >= context->vecReg.size() || context->vecReg[ipScan->b.u32] == 0) &&
                            (!ipScan->hasReadRegInC() || ipScan->c.u32 >= context->vecReg.size() || context->vecReg[ipScan->c.u32] == 0) &&
                            (!ipScan->hasReadRegInD() || ipScan->d.u32 >= context->vecReg.size() || context->vecReg[ipScan->d.u32] == 0))
                        {
                            context->vecInst.push_back(ipScan);
                        }
                    }

                    break;
            }

            ipScan += 1;
        }

        // Nothing to do
        if (context->vecInst.empty())
            continue;

        context->vecInstCopy.clear();
        int shift = 0;
        for (const auto it : context->vecInst)
        {
            auto cstOp = it + shift;
            SWAG_ASSERT(cstOp->countWriteRegs() == 1);

            if (cstOp->op == ByteCodeOp::CopyRBtoRA64)
            {
                const auto reg = cstOp->a.u32;
                if (reg >= context->vecReg.size() || context->vecReg[reg] <= 1)
                {
                    const auto newReg = cstOp->b.u32;
                    if (newReg >= context->vecReg.size() || context->vecReg[newReg] <= 1)
                    {
                        auto ipT = cstOp + 1;
                        while (ipT != ip + shift)
                        {
                            if (ipT->hasReadRefToRegA(reg) && !ipT->hasWriteRefToRegA(reg))
                                ipT->a.u32 = newReg;
                            if (ipT->hasReadRefToRegB(reg) && !ipT->hasWriteRefToRegB(reg))
                                ipT->b.u32 = newReg;
                            if (ipT->hasReadRefToRegC(reg) && !ipT->hasWriteRefToRegC(reg))
                                ipT->c.u32 = newReg;
                            if (ipT->hasReadRefToRegD(reg) && !ipT->hasWriteRefToRegD(reg))
                                ipT->d.u32 = newReg;
                            if (ipScan->hasWriteRefToReg(newReg))
                                break;
                            ipT += 1;
                        }
                    }
                }

                continue;
            }

            // Is the same instruction already moved out of loop?
            // In that case we will just use the corresponding register
            uint32_t newReg = 0;
            for (const auto& it1 : context->vecInstCopy)
            {
                if (it1.op == cstOp->op &&
                    (it1.a.u64 == cstOp->a.u64 || !cstOp->hasSomethingInA() || cstOp->hasWriteRegInA()) &&
                    (it1.b.u64 == cstOp->b.u64 || !cstOp->hasSomethingInB() || cstOp->hasWriteRegInB()) &&
                    (it1.c.u64 == cstOp->c.u64 || !cstOp->hasSomethingInC() || cstOp->hasWriteRegInC()) &&
                    (it1.d.u64 == cstOp->d.u64 || !cstOp->hasSomethingInD() || cstOp->hasWriteRegInD()))
                {
                    if (it1.hasWriteRegInA())
                        newReg = it1.a.u32;
                    else if (it1.hasWriteRegInB())
                        newReg = it1.b.u32;
                    else if (it1.hasWriteRegInC())
                        newReg = it1.c.u32;
                    else
                        newReg = it1.d.u32;
                    break;
                }
            }

            uint32_t writeReg;
            if (cstOp->hasWriteRegInA())
                writeReg = cstOp->a.u32;
            else if (cstOp->hasWriteRegInB())
                writeReg = cstOp->b.u32;
            else if (cstOp->hasWriteRegInC())
                writeReg = cstOp->c.u32;
            else
                writeReg = cstOp->d.u32;

            // An instruction already exists, so replace the current one with the previous destination register
            if (newReg)
            {
                SET_OP(cstOp, ByteCodeOp::CopyRBtoRA64);
                cstOp->a.u64 = writeReg;
                cstOp->b.u64 = newReg;

                cstOp->removeFlag(BCI_IMM_A | BCI_IMM_B | BCI_IMM_C | BCI_IMM_D);
                replaceReg(context, cstOp + 1, ip, cstOp->a.u32, newReg);
                continue;
            }

            // If the register is used more than once, then we allocate a new one and make a copy at the previous place.
            // The copy will have a chance to be removed, and if not, the loop will just have one copy instead of the original instruction.
            if ((writeReg < context->vecReg.size() && context->vecReg[writeReg] > 1) || hasJumps)
            {
                if (!insertNopBefore(context, ipStart))
                    break;

                shift += 1;
                newReg = context->bc->maxReservedRegisterRC;
                context->bc->maxReservedRegisterRC++;

                cstOp          = it + shift;
                ipStart->op    = cstOp->op;
                ipStart->flags = cstOp->flags;
                ipStart->a     = cstOp->a;
                ipStart->b     = cstOp->b;
                ipStart->c     = cstOp->c;
                ipStart->d     = cstOp->d;

                SET_OP(cstOp, ByteCodeOp::CopyRBtoRA64);
                cstOp->a.u64 = writeReg;
                cstOp->b.u64 = newReg;
                cstOp->removeFlag(BCI_IMM_A | BCI_IMM_B | BCI_IMM_C | BCI_IMM_D);

                if (ipStart->hasWriteRegInA())
                    ipStart->a.u64 = newReg;
                else if (ipStart->hasWriteRegInB())
                    ipStart->b.u64 = newReg;
                else if (ipStart->hasWriteRegInC())
                    ipStart->c.u64 = newReg;
                else
                    ipStart->d.u64 = newReg;

                context->vecInstCopy.push_back(*ipStart);
                replaceReg(context, cstOp + 1, ip + shift, cstOp->a.u32, newReg);
            }

            // If the register is written only once in the loop, then we can just move the instruction outside of the loop.
            else
            {
                if (!insertNopBefore(context, ipStart))
                    break;

                shift += 1;
                cstOp          = it + shift;
                ipStart->op    = cstOp->op;
                ipStart->flags = cstOp->flags;
                ipStart->a     = cstOp->a;
                ipStart->b     = cstOp->b;
                ipStart->c     = cstOp->c;
                ipStart->d     = cstOp->d;

                context->vecInstCopy.push_back(*ipStart);
                setNop(context, cstOp);
            }

            ipStart += 1;
        }

        if (shift)
            break;
    }

    return true;
}
