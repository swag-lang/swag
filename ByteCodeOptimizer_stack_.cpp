#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"

void ByteCodeOptimizer::optimizePassStack(ByteCodeOptContext* context)
{
    map<uint32_t, pair<uint64_t, ByteCodeInstruction*>> mapCst;

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || isJump(ip))
            mapCst.clear();

        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            (ip[1].op == ByteCodeOp::SetAtPointer8 || ip[1].op == ByteCodeOp::SetAtPointer16 || ip[1].op == ByteCodeOp::SetAtPointer32 || ip[1].op == ByteCodeOp::SetAtPointer64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].flags & BCI_IMM_B)
        {
            mapCst[ip[0].b.u32] = {ip[1].b.u64, ip + 1};
        }
        else if (ip->op == ByteCodeOp::SetZeroStack8 ||
                 ip->op == ByteCodeOp::SetZeroStack16 ||
                 ip->op == ByteCodeOp::SetZeroStack32 ||
                 ip->op == ByteCodeOp::SetZeroStack64)

        {
            mapCst[ip->a.u32] = {0, ip};
        }
        else if (ip->op == ByteCodeOp::MakeStackPointer)
        {
            auto it = mapCst.find(ip->b.u32);
            if (it != mapCst.end())
                mapCst.erase(it);
        }
        else if (ip->op == ByteCodeOp::SetZeroStackX)
        {
            auto it = mapCst.find(ip->a.u32);
            if (it != mapCst.end())
                mapCst.erase(it);
        }

        if (ip[0].op == ByteCodeOp::GetFromStack64)
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

        // MakeStackPointer followed by an increment of the pointer, we can just
        // make the increment now and remove the instruction
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::IncPointer32 &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].c.u32 &&
            ip[1].flags & BCI_IMM_B)
        {
            auto it = mapCst.find(ip->b.u32);
            if (it != mapCst.end())
                mapCst.erase(it);

            ip[0].b.u32 += ip[1].b.u32;
            setNop(context, &ip[1]);
        }

        // Testing if a stack pointer is not null is irrelevant.
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::TestNotZero64 &&
            ip[0].a.u32 == ip[1].b.u32)
        {
            context->passHasDoneSomething = true;
            ip[1].op                      = ByteCodeOp::SetImmediate32;
            ip[1].b.u32                   = 1;
        }
    }
}
