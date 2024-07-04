#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"

void ByteCodeOptimizer::reduceStackJumps(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
#define OPT_J(__t1, __t2)                 \
    if (ip[1].op == __t1 &&               \
        ip[1].a.u32 == ip[0].a.u32 &&     \
        !ip[1].flags.has(BCI_IMM_A) &&    \
        !ip[1].hasFlag(BCI_START_STMT))   \
    {                                     \
        SET_OP(ip + 1, __t2);             \
        ip[1].a.u64 = ip[0].b.u64;        \
        break;                            \
    }                                     \
    if (ip[2].op == __t1 &&               \
        ip[2].a.u32 == ip[0].a.u32 &&     \
        !ip[2].flags.has(BCI_IMM_A) &&    \
        !ip[1].hasFlag(BCI_START_STMT) && \
        !ip[2].hasFlag(BCI_START_STMT))   \
    {                                     \
        SET_OP(ip + 2, __t2);             \
        ip[2].a.u64 = ip[0].b.u64;        \
        break;                            \
    }

    switch (ip->op)
    {
        case ByteCodeOp::GetFromStack32:
            OPT_J(ByteCodeOp::JumpIfEqual32, ByteCodeOp::JumpIfStackEqual32);
            OPT_J(ByteCodeOp::JumpIfNotEqual32, ByteCodeOp::JumpIfStackNotEqual32);
            OPT_J(ByteCodeOp::JumpIfZero32, ByteCodeOp::JumpIfStackZero32);
            OPT_J(ByteCodeOp::JumpIfNotZero32, ByteCodeOp::JumpIfStackNotZero32);
            break;
        case ByteCodeOp::GetFromStack64:
            OPT_J(ByteCodeOp::JumpIfEqual64, ByteCodeOp::JumpIfStackEqual64);
            OPT_J(ByteCodeOp::JumpIfNotEqual64, ByteCodeOp::JumpIfStackNotEqual64);
            OPT_J(ByteCodeOp::JumpIfZero64, ByteCodeOp::JumpIfStackZero64);
            OPT_J(ByteCodeOp::JumpIfNotZero64, ByteCodeOp::JumpIfStackNotZero64);
            break;
    }
}

void ByteCodeOptimizer::reduceX2(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::GetParam64:
            if (ip[1].op == ByteCodeOp::GetParam64 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam64x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::CopyRBtoRA64:
            if (ip[1].op == ByteCodeOp::CopyRBtoRA64 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::ClearRA:
            if (ip[1].op == ByteCodeOp::ClearRA &&
                ip[2].op == ByteCodeOp::ClearRA &&
                ip[3].op == ByteCodeOp::ClearRA &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                !ip[3].hasFlag(BCI_START_STMT))
            {
                ip[0].b.u32 = ip[1].a.u32;
                ip[0].c.u32 = ip[2].a.u32;
                ip[0].d.u32 = ip[3].a.u32;
                SET_OP(ip, ByteCodeOp::ClearRAx4);
                setNop(context, ip + 1);
                setNop(context, ip + 2);
                setNop(context, ip + 3);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearRA &&
                ip[2].op == ByteCodeOp::ClearRA &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                ip[0].b.u32 = ip[1].a.u32;
                ip[0].c.u32 = ip[2].a.u32;
                SET_OP(ip, ByteCodeOp::ClearRAx3);
                setNop(context, ip + 1);
                setNop(context, ip + 2);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearRA &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[0].b.u32 = ip[1].a.u32;
                SET_OP(ip, ByteCodeOp::ClearRAx2);
                setNop(context, ip + 1);
                break;
            }

            break;

        case ByteCodeOp::MakeStackPointer:
            if (ip[1].op == ByteCodeOp::MakeStackPointer &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[0].c.u32 = ip[1].a.u32;
                ip[0].d.u32 = ip[1].b.u32;
                SET_OP(ip, ByteCodeOp::MakeStackPointerx2);
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::SetAtStackPointer8:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer8x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                if (ip[1].hasFlag(BCI_IMM_B))
                    ip[0].addFlag(BCI_IMM_D);
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::SetAtStackPointer16:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer16x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                if (ip[1].hasFlag(BCI_IMM_B))
                    ip[0].addFlag(BCI_IMM_D);
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::SetAtStackPointer32:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer32x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                if (ip[1].hasFlag(BCI_IMM_B))
                    ip[0].addFlag(BCI_IMM_D);
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::SetAtStackPointer64:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer64x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                if (ip[1].hasFlag(BCI_IMM_B))
                    ip[0].addFlag(BCI_IMM_D);
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack8:
            if (ip[1].op == ByteCodeOp::GetFromStack8 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack8x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack16:
            if (ip[1].op == ByteCodeOp::GetFromStack16 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack16x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack32:
            if (ip[1].op == ByteCodeOp::GetFromStack32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack32x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack64:
            if (ip[1].op == ByteCodeOp::GetFromStack64 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack64x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;
    }
}

void ByteCodeOptimizer::reduceInvCopy(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (!ip->hasFlag(BCI_NOT_PURE) &&
        !ByteCode::hasWriteRegInA(ip) &&
        !ByteCode::hasWriteRegInB(ip) &&
        ByteCode::hasWriteRegInC(ip) &&
        !ByteCode::hasWriteRegInD(ip) &&
        ip[0].c.u32 == ip[1].b.u32 &&
        !ip[1].hasFlag(BCI_START_STMT))
    {
        if (ip[1].op == ByteCodeOp::CopyRBtoRA32 ||
            ip[1].op == ByteCodeOp::CopyRBtoRA64)
        {
            ip->addFlag(BCI_NOT_PURE);
            ip->c.u32 = ip[1].a.u32;
            std::swap(ip[1].a, ip[1].b);
            context->setDirtyPass();
        }
    }
}

bool ByteCodeOptimizer::optimizePassReduceX2(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        reduceStackJumps(context, ip);
        reduceX2(context, ip);
        reduceInvCopy(context, ip);
    }

    return true;
}
