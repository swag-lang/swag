#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "Wmf/Module.h"

// We detect loops, and then we try to move constant instructions outside of the loop.
// For example a GetParam64 does not need to be done at each iteration, it could be done once before the loop.
bool ByteCodeOptimizer::optimizePassLoop(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        // Find a negative jump
        if (!ByteCode::isJump(ip) || ip->b.s32 > 0)
            continue;

        const auto ipStart = ip + ip->b.s32 + 1;
        auto       ipScan  = ipStart;

        // Zap non jump instructions
        while (!ByteCode::isJump(ipScan))
            ipScan++;
        if (ipScan == ip)
            continue;

        // We should have a jump to the instruction right after the original negative jump, or
        // this is not a loop
        const auto ipEnd = ipScan + ipScan->b.s32 + 1;
        if (ipEnd != ip + 1)
            continue;

        const auto ipExitJump = ipScan;
        ipScan                = ipStart;

        context->vecInst.clear();
        context->vecInstCopy.clear();
        uint32_t countReg[RegisterList::MAX_REGISTERS] = {};

        bool hasJumps = false;
        while (ipScan != ip)
        {
            // Test an inside jump that will escape the loop
            if (ByteCode::isJump(ipScan) && ipScan != ipExitJump && ipScan != ip)
            {
                hasJumps        = true;
                const auto test = ipScan + ipScan->b.s32 + 1;
                if (test < ipStart || test > ip)
                    break;
            }

            // A jump dyn inside, just dismiss
            if (ByteCode::isJumpDyn(ipScan))
                break;

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
                    context->vecInst.push_back(ipScan);
                    break;
                default:
                    break;
            }

            if (ByteCode::hasWriteRegInA(ipScan))
                countReg[ipScan->a.u32]++;
            if (ByteCode::hasWriteRegInB(ipScan))
                countReg[ipScan->b.u32]++;
            if (ByteCode::hasWriteRegInC(ipScan))
                countReg[ipScan->c.u32]++;
            if (ByteCode::hasWriteRegInD(ipScan))
                countReg[ipScan->d.u32]++;

            ipScan++;
        }

        // Stop before the end of the loop, bad one...
        if (ipScan != ip)
            continue;

        // Nothing to do
        if (context->vecInst.empty())
            continue;

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
                        {
                            ipScan = nullptr;
                            break;
                        }
                    }
                }
            }
            else
            {
                const auto ipJump = it + it->b.s32 + 1;
                if (ipJump >= ipStart && ipJump <= ipScan)
                {
                    if (it < ipStart || it > ipScan)
                    {
                        ipScan = nullptr;
                        break;
                    }
                }
            }
        }

        if (!ipScan)
            continue;

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
            if (countReg[cstOp->a.u32] > 1 || hasJumps)
            {
                if (context->bc->maxReservedRegisterRC == RegisterList::MAX_REGISTERS)
                    break;
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
        }

        if (shift)
            break;
    }

    return true;
}
