#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"

void ByteCodeOptimizer::optimizePassStack(ByteCodeOptContext* context)
{
    auto& mapCst = context->mapCst;
    mapCst.clear();
    auto& mapCstReg = context->mapU32U32;
    mapCstReg.clear();

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || isJump(ip))
            mapCst.clear();

        // If MakeStackPointer is followed by an increment of the pointer, we can just
        // make the increment now and remove the increment instruction
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::IncPointer64 &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].c.u32 &&
            ip[1].flags & BCI_IMM_B)
        {
            auto it = mapCst.find(ip->b.u64);
            if (it != mapCst.end())
                mapCst.erase(it);

            ip->b.u64 += ip[1].b.u64;
            setNop(context, &ip[1]);

            it = mapCst.find(ip->b.u64);
            if (it != mapCst.end())
                mapCst.erase(it);
        }

        auto flags = g_ByteCodeOpFlags[(int) ip->op];

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
