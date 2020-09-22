#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"

void ByteCodeOptimizer::optimizePassStack(ByteCodeOptContext* context)
{
    map<uint32_t, pair<uint64_t, ByteCodeInstruction*>> mapCst;
    map<uint32_t, uint32_t>                             mapCstReg;

    uint32_t lastReg    = 0xFFFFFFFF;
    uint32_t lastOffset = 0xFFFFFFFF;

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || isJump(ip))
        {
            mapCst.clear();
            lastReg    = 0xFFFFFFFF;
            lastOffset = 0xFFFFFFFF;
        }

        // If MakeStackPointer is followed by an increment of the pointer, we can just
        // make the increment now and remove the increment instruction
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::IncPointer32 &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].c.u32 &&
            ip[1].flags & BCI_IMM_B)
        {
            auto it = mapCst.find(ip->b.u32);
            if (it != mapCst.end())
                mapCst.erase(it);

            ip->b.u32 += ip[1].b.u32;
            setNop(context, &ip[1]);

            it = mapCst.find(ip->b.u32);
            if (it != mapCst.end())
                mapCst.erase(it);

            if (lastReg == ip->a.u32)
            {
                lastReg    = 0xFFFFFFFF;
                lastOffset = 0xFFFFFFFF;
            }
        }

        // Testing if a stack pointer is not null is irrelevant. This can happen often because of
        // safety checks, when dereferencing a struct on the stack
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::TestNotZero64 &&
            ip[0].a.u32 == ip[1].b.u32)
        {
            context->passHasDoneSomething = true;
            ip[1].op                      = ByteCodeOp::SetImmediate32;
            ip[1].b.u32                   = 1;
        }

        // If we have a MakeStackPointer identical to a previous one, and no read of the register between them,
        // then se second MakeStackPointer is useless
        auto flags = g_ByteCodeOpFlags[(int) ip->op];
        if (ip[0].op == ByteCodeOp::MakeStackPointer)
        {
            if (lastReg == ip->a.u32 && lastOffset == ip->b.u32)
            {
                setNop(context, ip);
            }
            else
            {
                lastReg    = ip->a.u32;
                lastOffset = ip->b.u32;
            }
        }
        else if ((flags & OPFLAG_WRITE_A && ip->a.u32 == lastReg && !(ip->flags & BCI_IMM_A)) ||
                 (flags & OPFLAG_WRITE_B && ip->b.u32 == lastReg && !(ip->flags & BCI_IMM_B)) ||
                 (flags & OPFLAG_WRITE_C && ip->c.u32 == lastReg && !(ip->flags & BCI_IMM_C)) ||
                 (flags & OPFLAG_WRITE_D && ip->d.u32 == lastReg && !(ip->flags & BCI_IMM_D)))
        {
            lastReg    = 0xFFFFFFFF;
            lastOffset = 0xFFFFFFFF;
        }

        // Try to detect when we store a constant on the stack, followed by a get of the value. The get
        // is then useless, as we can change it to an immediate constant copy
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            (ip[1].op == ByteCodeOp::SetAtPointer8 || ip[1].op == ByteCodeOp::SetAtPointer16 || ip[1].op == ByteCodeOp::SetAtPointer32 || ip[1].op == ByteCodeOp::SetAtPointer64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].flags & BCI_IMM_B)
        {
            mapCstReg[ip->a.u32] = ip->b.u32;
            mapCst[ip->b.u32]    = {ip[1].b.u64, ip + 1};
            ip++;
        }
        else if (ip->op == ByteCodeOp::SetZeroStack8 ||
                 ip->op == ByteCodeOp::SetZeroStack16 ||
                 ip->op == ByteCodeOp::SetZeroStack32 ||
                 ip->op == ByteCodeOp::SetZeroStack64)

        {
            mapCst[ip->a.u32] = {0, ip};
        }
        else if (ip->op == ByteCodeOp::GetFromStack64)
        {
            auto it = mapCst.find(ip->b.u32);
            if (it != mapCst.end())
            {
                context->passHasDoneSomething = true;
                it->second.second             = nullptr;
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.u64                     = it->second.first;
            }
        }
        else if (flags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
        {
            auto it = mapCstReg.find(ip->a.u32);
            if (it != mapCstReg.end())
            {
                auto it1 = mapCst.find(it->second);
                if (it1 != mapCst.end())
                {
                    mapCstReg.erase(it);
                    mapCst.erase(it1);
                }
            }
        }
        else if (ip->op == ByteCodeOp::MakeStackPointer)
        {
            auto it = mapCst.find(ip->b.u32);
            if (it != mapCst.end())
                mapCst.erase(it);
            auto it1 = mapCstReg.find(ip->a.u32);
            if (it1 != mapCstReg.end())
                mapCstReg.erase(it1);
        }
        else if (ip->op == ByteCodeOp::SetZeroStackX)
        {
            auto it = mapCst.find(ip->a.u32);
            if (it != mapCst.end())
                mapCst.erase(it);
        }
    }
}
