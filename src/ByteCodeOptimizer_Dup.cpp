#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "SourceFile.h"
#include "ByteCode.h"

// If a CopyRBRA is followed by the same CopyRBRA, and between them there's no write to RA or RB,
// then the second CopyRBRA is useless
void ByteCodeOptimizer::optimizePassDupCopyRBRAOp(ByteCodeOptContext* context, ByteCodeOp op)
{
    auto& mapCopyRA = context->mapRegInstA;
    auto& mapCopyRB = context->mapRegInstB;
    mapCopyRA.clear();
    mapCopyRB.clear();

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT)
        {
            mapCopyRA.clear();
            mapCopyRB.clear();
        }

        auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;

        if (ip->op == ByteCodeOp::CopyRBtoRA64)
        {
            // CopyRBRA followed by one single pushraparam, take the original register
            if (ip[1].op == ByteCodeOp::PushRAParam &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                ip[1].a.u32                   = ip[0].b.u32;
                context->passHasDoneSomething = true;
            }

            // 2 CopyRBRA followed by one single pushraparam2, take the original registers
            if (ip[1].op == ByteCodeOp::CopyRBtoRA64 &&
                ip[2].op == ByteCodeOp::PushRAParam2 &&
                ip[0].a.u32 == ip[2].b.u32 &&
                ip[1].a.u32 == ip[2].a.u32 &&
                !(ip[0].flags & BCI_START_STMT) &&
                !(ip[1].flags & BCI_START_STMT) &&
                !(ip[2].flags & BCI_START_STMT))
            {
                ip[2].a.u32                   = ip[1].b.u32;
                ip[2].b.u32                   = ip[0].b.u32;
                context->passHasDoneSomething = true;
            }
        }

        if (ip->op == op)
        {
            // CopyRBRA X, X
            if (ip->a.u32 == ip->b.u32)
            {
                setNop(context, ip);
            }

            // CopyRBRA X, Y followed by CopyRBRA Y, X
            if (ip[1].op == op &&
                ip->a.u32 == ip[1].b.u32 &&
                ip->b.u32 == ip[1].a.u32)
            {
                setNop(context, ip + 1);
            }

            auto it = mapCopyRA.find(ip->a.u32);
            if (it)
            {
                auto it1 = mapCopyRB.find(ip->b.u32);
                if (it1 && *it1 == *it)
                    setNop(context, ip);
            }

            mapCopyRB.remove(ip->a.u32);
            mapCopyRA.set(ip->a.u32, ip);
            mapCopyRB.set(ip->b.u32, ip);
            continue;
        }

        if (!mapCopyRA.count && !mapCopyRB.count)
            continue;

        // If we use a register that comes from a CopyRBRA, then use the initial register instead.
        // That way, the Copy can become deadstore and removed later.
        // Specific instructions only, as ByteCodeOp::CopyRBtoRA32 will clear the high 32 bit, if the following instruction
        // deal with 64 bits, this could be a pb not to do that clean by just using the original 32 bits register.
        if (op == ByteCodeOp::CopyRBtoRA32)
        {
            switch (ip->op)
            {
            case ByteCodeOp::JumpIfZero32:
            case ByteCodeOp::JumpIfNotZero32:
                if (ByteCode::hasReadRegInA(ip) && !ByteCode::hasWriteRegInA(ip))
                {
                    auto it = mapCopyRA.find(ip->a.u32);
                    if (it)
                    {
                        auto it1 = mapCopyRB.find((*it)->b.u32);
                        if (it1 && *it == *it1)
                        {
                            ip->a.u32 = (*it)->b.u32;
                            context->passHasDoneSomething = true;
                        }
                    }
                }
                break;

            case ByteCodeOp::SetAtPointer32:
            case ByteCodeOp::SetAtStackPointer32:
            case ByteCodeOp::AffectOpPlusEqS32:
                if (ByteCode::hasReadRegInB(ip) && !ByteCode::hasWriteRegInB(ip))
                {
                    auto it = mapCopyRA.find(ip->b.u32);
                    if (it)
                    {
                        auto it1 = mapCopyRB.find((*it)->b.u32);
                        if (it1 && *it == *it1)
                        {
                            ip->b.u32 = (*it)->b.u32;
                            context->passHasDoneSomething = true;
                        }
                    }
                }
                break;
            }
        }
        else if (op == ByteCodeOp::CopyRBtoRA64)
        {
            // If we use a register that comes from a CopyRBRA, then use the initial
            // register instead (that way, the Copy can become deadstore and removed later)
            // Do it only for CopyRBtoRA64, as other copy have an implicit cast
            // *NOT* for PushRAParam, because the register numbers are important in case of variadic parameters.
            // *NOT* for CopySP, because the register numbers are important in case of variadic parameters.
            if ((ip->op != ByteCodeOp::PushRAParam || !(ip->flags & BCI_VARIADIC)) &&
                (ip->op != ByteCodeOp::PushRAParam2 || !(ip->flags & BCI_VARIADIC)) &&
                (ip->op != ByteCodeOp::PushRAParam3 || !(ip->flags & BCI_VARIADIC)) &&
                (ip->op != ByteCodeOp::PushRAParam4 || !(ip->flags & BCI_VARIADIC)) &&
                ip->op != ByteCodeOp::CopySP)
            {
                if (ByteCode::hasReadRegInA(ip) && !ByteCode::hasWriteRegInA(ip))
                {
                    auto it = mapCopyRA.find(ip->a.u32);
                    if (it)
                    {
                        auto it1 = mapCopyRB.find((*it)->b.u32);
                        if (it1 && *it == *it1)
                        {
                            ip->a.u32                     = (*it)->b.u32;
                            context->passHasDoneSomething = true;
                        }
                    }
                }

                if (ByteCode::hasReadRegInB(ip) && !ByteCode::hasWriteRegInB(ip))
                {
                    auto it = mapCopyRA.find(ip->b.u32);
                    if (it)
                    {
                        auto it1 = mapCopyRB.find((*it)->b.u32);
                        if (it1 && *it == *it1)
                        {
                            ip->b.u32                     = (*it)->b.u32;
                            context->passHasDoneSomething = true;
                        }
                    }
                }

                if (ByteCode::hasReadRegInC(ip) && !ByteCode::hasWriteRegInC(ip))
                {
                    auto it = mapCopyRA.find(ip->c.u32);
                    if (it)
                    {
                        auto it1 = mapCopyRB.find((*it)->b.u32);
                        if (it1 && *it == *it1)
                        {
                            ip->c.u32                     = (*it)->b.u32;
                            context->passHasDoneSomething = true;
                        }
                    }
                }

                if (ByteCode::hasReadRegInD(ip) && !ByteCode::hasWriteRegInD(ip))
                {
                    auto it = mapCopyRA.find(ip->d.u32);
                    if (it)
                    {
                        auto it1 = mapCopyRB.find((*it)->b.u32);
                        if (it1 && *it == *it1)
                        {
                            ip->d.u32                     = (*it)->b.u32;
                            context->passHasDoneSomething = true;
                        }
                    }
                }
            }
        }

        // Reset CopyRARB Map
        if (ByteCode::isJump(ip))
        {
            mapCopyRA.clear();
            mapCopyRB.clear();
        }

        if ((flags & OPFLAG_WRITE_A) && !(ip->flags & BCI_IMM_A))
        {
            auto it = mapCopyRA.find(ip->a.u32);
            if (it)
            {
                auto it1 = mapCopyRB.find((*it)->b.u32);
                if (it1 && *it == *it1)
                    mapCopyRB.remove((*it)->b.u32);
                mapCopyRA.remove(ip->a.u32);
            }

            mapCopyRB.remove(ip->a.u32);
        }

        if ((flags & OPFLAG_WRITE_B) && !(ip->flags & BCI_IMM_B))
        {
            auto it = mapCopyRA.find(ip->b.u32);
            if (it)
            {
                auto it1 = mapCopyRB.find((*it)->b.u32);
                if (it1 && *it == *it1)
                    mapCopyRB.remove((*it)->b.u32);
                mapCopyRA.remove(ip->b.u32);
            }

            mapCopyRB.remove(ip->b.u32);
        }

        if ((flags & OPFLAG_WRITE_C) && !(ip->flags & BCI_IMM_C))
        {
            auto it = mapCopyRA.find(ip->c.u32);
            if (it)
            {
                auto it1 = mapCopyRB.find((*it)->b.u32);
                if (it1 && *it == *it1)
                    mapCopyRB.remove((*it)->b.u32);
                mapCopyRA.remove(ip->c.u32);
            }

            mapCopyRB.remove(ip->c.u32);
        }

        if ((flags & OPFLAG_WRITE_D) && !(ip->flags & BCI_IMM_D))
        {
            auto it = mapCopyRA.find(ip->d.u32);
            if (it)
            {
                auto it1 = mapCopyRB.find((*it)->b.u32);
                if (it1 && *it == *it1)
                    mapCopyRB.remove((*it)->b.u32);
                mapCopyRA.remove(ip->d.u32);
            }

            mapCopyRB.remove(ip->d.u32);
        }
    }
}

bool ByteCodeOptimizer::optimizePassDupCopyRBRA(ByteCodeOptContext* context)
{
    // See setContextFlags if you add one instruction
    if (!(context->contextBcFlags & OCF_HAS_COPYRBRA))
        return true;

    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA8);
    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA16);
    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA32);
    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA64);
    return true;
}

void ByteCodeOptimizer::optimizePassDupCopyOp(ByteCodeOptContext* context, ByteCodeOp op)
{
    auto& mapRA = context->mapRegInstA;
    mapRA.clear();

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || ByteCode::isJump(ip))
            mapRA.clear();

        if (ip->op == op)
        {
            auto it = mapRA.find(ip->a.u32);
            if (it)
            {
                if ((*it)->b.u64 == ip->b.u64 &&
                    (*it)->c.u64 == ip->c.u64 &&
                    (*it)->d.u64 == ip->d.u64 &&
                    (*it)->flags == ip->flags)
                {
                    setNop(context, ip);
                    mapRA.remove(ip->a.u32);
                    continue;
                }
            }

            mapRA.set(ip->a.u32, ip);
        }
        else if (mapRA.count)
        {
            auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;
            if ((flags & OPFLAG_WRITE_A) && !(ip->flags & BCI_IMM_A))
                mapRA.remove(ip->a.u32);
            if ((flags & OPFLAG_WRITE_B) && !(ip->flags & BCI_IMM_B))
                mapRA.remove(ip->b.u32);
            if ((flags & OPFLAG_WRITE_C) && !(ip->flags & BCI_IMM_C))
                mapRA.remove(ip->c.u32);
            if ((flags & OPFLAG_WRITE_D) && !(ip->flags & BCI_IMM_D))
                mapRA.remove(ip->d.u32);
        }
    }
}

// Remove duplicated pure instructions (set RA to a constant)
bool ByteCodeOptimizer::optimizePassDupCopy(ByteCodeOptContext* context)
{
    // See setContextFlags if you add one instruction
    if (!(context->contextBcFlags & OCF_HAS_DUPCOPY))
        return true;

    optimizePassDupCopyOp(context, ByteCodeOp::CopyRRtoRC);
    optimizePassDupCopyOp(context, ByteCodeOp::MakeBssSegPointer);
    optimizePassDupCopyOp(context, ByteCodeOp::MakeConstantSegPointer);
    optimizePassDupCopyOp(context, ByteCodeOp::MakeMutableSegPointer);
    optimizePassDupCopyOp(context, ByteCodeOp::GetParam64);
    optimizePassDupCopyOp(context, ByteCodeOp::GetIncParam64);
    optimizePassDupCopyOp(context, ByteCodeOp::SetImmediate32);
    optimizePassDupCopyOp(context, ByteCodeOp::SetImmediate64);

    return true;
}
