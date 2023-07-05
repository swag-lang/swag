#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "Math.h"

void ByteCodeOptimizer::reduceX2(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::MakeStackPointer:
        if (ip[1].op == ByteCodeOp::MakeStackPointer &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].c.u32 = ip[1].a.u32;
            ip[0].d.u32 = ip[1].b.u32;
            SET_OP(ip, ByteCodeOp::MakeStackPointer_x2);
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::ClearRA:
        if (ip[1].op == ByteCodeOp::ClearRA &&
            ip[2].op == ByteCodeOp::ClearRA &&
            ip[3].op == ByteCodeOp::ClearRA &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT) &&
            !(ip[3].flags & BCI_START_STMT))
        {
            ip[0].b.u32 = ip[1].a.u32;
            ip[0].c.u32 = ip[2].a.u32;
            ip[0].d.u32 = ip[3].a.u32;
            SET_OP(ip, ByteCodeOp::ClearRA_x4);
            setNop(context, ip + 1);
            setNop(context, ip + 2);
            setNop(context, ip + 3);
            break;
        }

        if (ip[1].op == ByteCodeOp::ClearRA &&
            ip[2].op == ByteCodeOp::ClearRA &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT))
        {
            ip[0].b.u32 = ip[1].a.u32;
            ip[0].c.u32 = ip[2].a.u32;
            SET_OP(ip, ByteCodeOp::ClearRA_x3);
            setNop(context, ip + 1);
            setNop(context, ip + 2);
            break;
        }

        if (ip[1].op == ByteCodeOp::ClearRA &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].b.u32 = ip[1].a.u32;
            SET_OP(ip, ByteCodeOp::ClearRA_x2);
            setNop(context, ip + 1);
            break;
        }

        break;

        // SetAtStackPointer x2
    case ByteCodeOp::SetAtStackPointer8:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer8_x2);
            ip[0].c.u64 = ip[1].a.u64;
            ip[0].d.u64 = ip[1].b.u64;
            if (ip[1].flags & BCI_IMM_B)
                ip[0].flags |= BCI_IMM_D;
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::SetAtStackPointer16:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer16_x2);
            ip[0].c.u64 = ip[1].a.u64;
            ip[0].d.u64 = ip[1].b.u64;
            if (ip[1].flags & BCI_IMM_B)
                ip[0].flags |= BCI_IMM_D;
            setNop(context, ip + 1);
        }

    case ByteCodeOp::SetAtStackPointer32:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer32_x2);
            ip[0].c.u64 = ip[1].a.u64;
            ip[0].d.u64 = ip[1].b.u64;
            if (ip[1].flags & BCI_IMM_B)
                ip[0].flags |= BCI_IMM_D;
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::SetAtStackPointer64:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer64_x2);
            ip[0].c.u64 = ip[1].a.u64;
            ip[0].d.u64 = ip[1].b.u64;
            if (ip[1].flags & BCI_IMM_B)
                ip[0].flags |= BCI_IMM_D;
            setNop(context, ip + 1);
            break;
        }
        break;

        // GetFromStack64_x2
        /*case ByteCodeOp::GetFromStack64:
            if (ip[1].op == ByteCodeOp::GetFromStack64 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack64_x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;*/

    default:
        break;
    }
}

bool ByteCodeOptimizer::optimizePassReduceX2(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        reduceX2(context, ip);
    }

    return true;
}
