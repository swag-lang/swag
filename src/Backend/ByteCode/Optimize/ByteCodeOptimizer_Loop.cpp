#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "Wmf/Module.h"

namespace
{
    ByteCodeInstruction* findLoop(ByteCodeOptContext* context, ByteCodeInstruction* ip, bool& hasJumps)
    {
        // Find a negative jump
        if (!ByteCode::isJump(ip) || ip->b.s32 > 0)
            return nullptr;

        context->vecReg.clear();

        const auto ipStart = ip + ip->b.s32 + 1;
        auto       ipScan  = ipStart;

        // Zap non jump instructions
        while (!ByteCode::isJump(ipScan))
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
            if (ByteCode::isJump(ipScan) && ipScan != ipExitJump && ipScan != ip)
            {
                hasJumps        = true;
                const auto test = ipScan + ipScan->b.s32 + 1;
                if (test < ipStart || test > ip + 1)
                    break;
            }

            // A jump dyn inside, just dismiss
            if (ByteCode::isJumpDyn(ipScan))
                break;

            if (ByteCode::hasWriteRegInA(ipScan))
            {
                context->vecReg.expand_clear(ipScan->a.u32 + 1);
                context->vecReg[ipScan->a.u32]++;
            }
            if (ByteCode::hasWriteRegInB(ipScan))
            {
                context->vecReg.expand_clear(ipScan->b.u32 + 1);
                context->vecReg[ipScan->b.u32]++;
            }
            if (ByteCode::hasWriteRegInC(ipScan))
            {
                context->vecReg.expand_clear(ipScan->c.u32 + 1);
                context->vecReg[ipScan->c.u32]++;
            }
            if (ByteCode::hasWriteRegInD(ipScan))
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
            if (ByteCode::isJumpDyn(it))
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
                //case ByteCodeOp::SetImmediate32:
                //case ByteCodeOp::SetImmediate64:
                    context->vecInst.push_back(ipScan);
                    break;

                default:
                    if (ipScan->hasOpFlag(OPF_REG_ONLY))
                    {
                        if ((!ByteCode::hasReadRegInA(ipScan) || ipScan->a.u32 >= context->vecReg.size() || context->vecReg[ipScan->a.u32] == 0) &&
                            (!ByteCode::hasReadRegInB(ipScan) || ipScan->b.u32 >= context->vecReg.size() || context->vecReg[ipScan->b.u32] == 0) &&
                            (!ByteCode::hasReadRegInC(ipScan) || ipScan->c.u32 >= context->vecReg.size() || context->vecReg[ipScan->c.u32] == 0) &&
                            (!ByteCode::hasReadRegInD(ipScan) || ipScan->d.u32 >= context->vecReg.size() || context->vecReg[ipScan->d.u32] == 0))
                        {
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

            SWAG_ASSERT(ByteCode::hasWriteRegInA(cstOp));
            SWAG_ASSERT(!ByteCode::hasWriteRegInB(cstOp));
            SWAG_ASSERT(!ByteCode::hasWriteRegInC(cstOp));
            SWAG_ASSERT(!ByteCode::hasWriteRegInD(cstOp));

            uint32_t newReg = 0;
            for (const auto& it1 : context->vecInstCopy)
            {
                if (it1.op == cstOp->op &&
                    (it1.b.u64 == cstOp->b.u64 || !ByteCode::hasSomethingInB(cstOp)) &&
                    (it1.c.u64 == cstOp->c.u64 || !ByteCode::hasSomethingInC(cstOp)) &&
                    (it1.d.u64 == cstOp->d.u64 || !ByteCode::hasSomethingInD(cstOp)))
                {
                    newReg = it1.a.u32;
                    break;
                }
            }

            if (newReg)
            {
                SET_OP(cstOp, ByteCodeOp::CopyRBtoRA64);
                cstOp->b.u32 = newReg;
                continue;
            }

            // If the register is used more than once, then we allocate a new one and make a copy at the previous place.
            // The copy will have a chance to be removed, and if not, the loop will just have one copy instead of the original instruction.
            if ((cstOp->a.u32 < context->vecReg.size() && context->vecReg[cstOp->a.u32] > 1) || hasJumps)
            {
                if (!insertNopBefore(context, ipStart))
                    break;

                shift += 1;
                newReg = context->bc->maxReservedRegisterRC;
                context->bc->maxReservedRegisterRC++;

                cstOp          = it + shift;
                ipStart->op    = cstOp->op;
                ipStart->a.u32 = newReg;
                ipStart->b     = cstOp->b;
                ipStart->c     = cstOp->c;
                ipStart->d     = cstOp->d;

                context->vecInstCopy.push_back(*ipStart);

                SET_OP(cstOp, ByteCodeOp::CopyRBtoRA64);
                cstOp->b.u32 = newReg;

                ipScan = cstOp + 1;
                while (ipScan != ip + shift)
                {
                    if (ByteCode::hasWriteRefToReg(ipScan, cstOp->a.u32))
                        break;
                    if (ByteCode::isRet(ipScan))
                        break;
                    if (ByteCode::isJump(ipScan) && context->vecReg[cstOp->a.u32] > 1)
                        break;
                    if (ipScan->hasFlag(BCI_START_STMT) && context->vecReg[cstOp->a.u32] > 1)
                        break;

                    if (ByteCode::hasReadRefToRegA(ipScan, cstOp->a.u32))
                        ipScan->a.u32 = newReg;
                    if (ByteCode::hasReadRefToRegB(ipScan, cstOp->a.u32))
                        ipScan->b.u32 = newReg;
                    if (ByteCode::hasReadRefToRegC(ipScan, cstOp->a.u32))
                        ipScan->c.u32 = newReg;
                    if (ByteCode::hasReadRefToRegD(ipScan, cstOp->a.u32))
                        ipScan->d.u32 = newReg;

                    ipScan += 1;
                }
            }

            // If the register is written only once in the loop, then we can just move the instruction outside of the loop.
            else
            {
                if (!insertNopBefore(context, ipStart))
                    break;

                shift += 1;
                cstOp       = it + shift;
                ipStart->op = cstOp->op;
                ipStart->a  = cstOp->a;
                ipStart->b  = cstOp->b;
                ipStart->c  = cstOp->c;
                ipStart->d  = cstOp->d;

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
