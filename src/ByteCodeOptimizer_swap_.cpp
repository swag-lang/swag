
#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"

void ByteCodeOptimizer::optimizePassSwap(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->op == ByteCodeOp::Ret || ip[1].op == ByteCodeOp::Ret || ip[2].op == ByteCodeOp::Ret)
            continue;
        if (ip->op == ByteCodeOp::Nop || ip[1].op == ByteCodeOp::Nop || ip[2].op == ByteCodeOp::Nop)
            continue;
        if (ByteCode::isJump(ip) || ByteCode::isJump(ip + 1) || ByteCode::isJump(ip + 2))
            continue;
        if (ip->flags & BCI_START_STMT || ip[1].flags & BCI_START_STMT || ip[2].flags & BCI_START_STMT)
            continue;

        if (ip->op == ByteCodeOp::MakeStackPointer &&
            ip[1].op != ByteCodeOp::IncPointer64 &&
            ip[1].op != ip->op &&
            !hasRefToReg(ip + 1, ip->a.u32))
        {
            swap(ip[0], ip[1]);
            context->passHasDoneSomething = true;
        }

        if (ip->op == ByteCodeOp::IncPointer64 &&
            ip[1].op != ip->op &&
            ip[1].op != ByteCodeOp::MakeStackPointer &&
            ip->flags & BCI_IMM_B &&
            !hasRefToReg(ip + 1, ip->a.u32) &&
            !hasRefToReg(ip + 1, ip->c.u32))
        {
            swap(ip[0], ip[1]);
            context->passHasDoneSomething = true;
        }
    }
}