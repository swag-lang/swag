#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"

bool ByteCodeOptimizer::optimizePassNullPointer(ByteCodeOptContext* context)
{
    parseTree(context, 0, context->tree[0].start, 0x00000001, [](ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt) {
        auto ip = parseCxt.curIp;

        // Testing if a stack pointer is not null is irrelevant. This can happen often because of
        // safety checks, when dereferencing a struct on the stack
        if ((ip[0].op == ByteCodeOp::MakeStackPointer || ip[0].op == ByteCodeOp::CopyRRtoRC) &&
            ip[1].op == ByteCodeOp::JumpIfNotZero64 &&
            !(ip[1].flags & BCI_IMM_A) &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            SET_OP(ip + 1, ByteCodeOp::Jump);
            context->passHasDoneSomething = true;
            return;
        }

        if (ip[0].op == ByteCodeOp::CopyRTtoRC &&
            ip[1].op == ByteCodeOp::IncSPPostCall &&
            ip[2].op == ByteCodeOp::JumpIfNotZero64 &&
            !(ip[2].flags & BCI_IMM_A) &&
            ip[0].a.u32 == ip[2].a.u32)
        {
            SET_OP(ip + 2, ByteCodeOp::Jump);
            context->passHasDoneSomething = true;
            return;
        }

        // Testing if a pointer is not null is irrelevant if previous instruction has incremented the pointer.
        if (ip[0].op == ByteCodeOp::IncPointer64 &&
            ip[1].op == ByteCodeOp::JumpIfNotZero64 &&
            !(ip[1].flags & BCI_IMM_A) &&
            ip[0].c.u32 == ip[1].a.u32 &&
            (ip[0].flags & BCI_IMM_B) &&
            ip[0].b.u32)
        {
            SET_OP(ip + 1, ByteCodeOp::Jump);
            context->passHasDoneSomething = true;
            return;
        }

        // [Safety] no need to test if a parameter is not null multiple times
        uint32_t paramNPIdx = UINT32_MAX;
        uint32_t paramNPReg = UINT32_MAX;
        if ((ip[1].flags & BCI_SAFETY) &&
            ip[0].op == ByteCodeOp::GetFromStackParam64 &&
            ip[1].op == ByteCodeOp::JumpIfNotZero64 &&
            !(ip[1].flags & BCI_IMM_A) &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            paramNPReg = ip[0].a.u32;
            paramNPIdx = ip[0].c.u32;
        }
        else if (ip[0].op == ByteCodeOp::GetFromStackParam64 &&
                 ip[1].op == ByteCodeOp::IncPointer64 &&
                 ip[0].a.u32 == ip[1].a.u32 &&
                 ip[1].a.u32 == ip[1].c.u32)
        {
            paramNPReg = ip[0].a.u32;
            paramNPIdx = ip[0].c.u32;
        }
        else if ((ip[0].flags & BCI_SAFETY) &&
                 ip[0].op == ByteCodeOp::JumpIfNotZero64)
        {
            paramNPReg = ip[0].a.u32;
        }

        if (paramNPIdx == UINT32_MAX && paramNPReg == UINT32_MAX)
            return;

        parseTree(context, parseCxt.curNode, parseCxt.curIp, 0x00000002, [&](ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt1) {
            auto ip1 = parseCxt1.curIp;
            if (ip1 == ip || ip1 == ip + 1)
                return;

            auto flags1 = g_ByteCodeOpDesc[(int) ip1->op].flags;

            // Incrmenting the tested pointer is fine. The safety check after on the same pointer could be removed...
            if (ip1->op == ByteCodeOp::IncPointer64 && ip1->a.u32 == paramNPReg && ip1->c.u32 == paramNPReg)
                paramNPReg = paramNPReg;
            else
            {
                if ((flags1 & OPFLAG_WRITE_A) && !(ip1->flags & BCI_IMM_A) && ip1->a.u32 == paramNPReg)
                    paramNPReg = UINT32_MAX;
                if ((flags1 & OPFLAG_WRITE_B) && !(ip1->flags & BCI_IMM_B) && ip1->b.u32 == paramNPReg)
                    paramNPReg = UINT32_MAX;
                if ((flags1 & OPFLAG_WRITE_C) && !(ip1->flags & BCI_IMM_C) && ip1->c.u32 == paramNPReg)
                    paramNPReg = UINT32_MAX;
                if ((flags1 & OPFLAG_WRITE_D) && !(ip1->flags & BCI_IMM_D) && ip1->d.u32 == paramNPReg)
                    paramNPReg = UINT32_MAX;
            }

            // Check not null done multiple times with the same register
            if ((ip1[0].flags & BCI_SAFETY) &&
                ip1[0].op == ByteCodeOp::JumpIfNotZero64 &&
                !(ip1[0].flags & BCI_IMM_A) &&
                ip1[0].a.u32 == paramNPReg)
            {
                SET_OP(ip1, ByteCodeOp::Jump);
                context->passHasDoneSomething = true;
            }

            // Check not null done multiple times with the same parameter
            if ((ip1[1].flags & BCI_SAFETY) &&
                ip1[0].op == ByteCodeOp::GetFromStackParam64 &&
                ip1[1].op == ByteCodeOp::JumpIfNotZero64 &&
                !(ip1[1].flags & BCI_IMM_A) &&
                ip1[0].a.u32 == ip1[1].a.u32 &&
                ip1[0].c.u32 == paramNPIdx)
            {
                SET_OP(ip1 + 1, ByteCodeOp::Jump);
                context->passHasDoneSomething = true;
            }
        });
    });

    return true;
}
