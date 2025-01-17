#include "pch.h"

#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"

void ByteCodeOptimizer::reduceCall(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::PushRAParam:
            if (ip[1].op == ByteCodeOp::ForeignCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ForeignCallPopParam);
                ip[1].c.u32 = 0;
                ip[1].d.u32 = ip[0].a.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::LocalCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::LocalCallPopParam);
                ip[1].c.u32 = 0;
                ip[1].d.u32 = ip[0].a.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::LambdaCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::LambdaCallPopParam);
                ip[1].c.u32 = 0;
                ip[1].d.u32 = ip[0].a.u32;
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::PushRAParam2:

            if (ip[1].op == ByteCodeOp::ForeignCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ForeignCallPop0Param2);
                ip[1].c.u32 = ip[0].a.u32;
                ip[1].d.u32 = ip[0].b.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::LocalCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::LocalCallPop0Param2);
                ip[1].c.u32 = ip[0].a.u32;
                ip[1].d.u32 = ip[0].b.u32;
                setNop(context, ip);
                break;
            }

            break;
    }
}

namespace
{
    void reduceStackJumpsStackOp(ByteCodeOptContext* context, ByteCodeInstruction* ip, ByteCodeOp op0, ByteCodeOp op1, bool checkOrder = false)
    {
        if (checkOrder &&
            ip[1].op == op0 &&
            ip[1].c.u32 == ip[0].a.u32 &&
            !ip[1].flags.has(BCI_IMM_A) &&
            !ip[1].flags.has(BCI_IMM_C))
        {
            std::swap(ip[1].a, ip[1].c);
            context->setDirtyPass();
            return;
        }

        if (ip[1].op == op0 &&
            ip[1].a.u32 == ip[0].a.u32 &&
            !ip[1].flags.has(BCI_IMM_A) &&
            !ip[1].hasFlag(BCI_START_STMT))
        {
            SET_OP(ip + 1, op1);
            ip[1].a.u64 = ip[0].b.u64;
        }
    }

    void reduceStackJumpsDeRefOp(ByteCodeOptContext* context, ByteCodeInstruction* ip, ByteCodeOp op0, ByteCodeOp op1)
    {
        if (ip[1].op == op0 &&
            ip[1].c.u32 == ip[0].a.u32 &&
            !ip[1].flags.has(BCI_IMM_A) &&
            !ip[1].flags.has(BCI_IMM_C))
        {
            std::swap(ip[1].a, ip[1].c);
            context->setDirtyPass();
            return;
        }

        if (ip[1].op == op0 &&
            ip[0].a.u32 != ip[0].b.u32 &&
            ip[1].a.u32 == ip[0].a.u32 &&
            !ip[1].flags.has(BCI_IMM_A) &&
            !ip[1].hasFlag(BCI_START_STMT))
        {
            SET_OP(ip + 1, op1);
            ip[1].a.u64 = ip[0].b.u64;
            ip[1].d.u64 = ip[0].c.u64;
        }
    }
}

void ByteCodeOptimizer::reduceStackJumps(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::GetFromStack8:
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfFalse, ByteCodeOp::JumpIfStackFalse);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfTrue, ByteCodeOp::JumpIfStackTrue);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfZero8, ByteCodeOp::JumpIfStackZero8);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfNotZero8, ByteCodeOp::JumpIfStackNotZero8);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfEqual8, ByteCodeOp::JumpIfStackEqual8, true);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfNotEqual8, ByteCodeOp::JumpIfStackNotEqual8, true);
            break;

        case ByteCodeOp::GetFromStack16:
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfZero16, ByteCodeOp::JumpIfStackZero16);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfNotZero16, ByteCodeOp::JumpIfStackNotZero16);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfEqual16, ByteCodeOp::JumpIfStackEqual16, true);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfNotEqual16, ByteCodeOp::JumpIfStackNotEqual16, true);
            break;

        case ByteCodeOp::GetFromStack32:
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfZero32, ByteCodeOp::JumpIfStackZero32);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfNotZero32, ByteCodeOp::JumpIfStackNotZero32);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfEqual32, ByteCodeOp::JumpIfStackEqual32, true);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfNotEqual32, ByteCodeOp::JumpIfStackNotEqual32, true);
            break;

        case ByteCodeOp::GetFromStack64:
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfZero64, ByteCodeOp::JumpIfStackZero64);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfNotZero64, ByteCodeOp::JumpIfStackNotZero64);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfEqual64, ByteCodeOp::JumpIfStackEqual64, true);
            reduceStackJumpsStackOp(context, ip, ByteCodeOp::JumpIfNotEqual64, ByteCodeOp::JumpIfStackNotEqual64, true);
            break;

        case ByteCodeOp::DeRef64:
            reduceStackJumpsDeRefOp(context, ip, ByteCodeOp::JumpIfEqual64, ByteCodeOp::JumpIfDeRefEqual64);
            reduceStackJumpsDeRefOp(context, ip, ByteCodeOp::JumpIfNotEqual64, ByteCodeOp::JumpIfDeRefNotEqual64);
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
    if (!ip->dynFlags.has(BCID_INV_CPY) &&
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
            ip->dynFlags.add(BCID_INV_CPY);
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
        reduceCall(context, ip);
        reduceStackJumps(context, ip);
        reduceX2(context, ip);
        reduceInvCopy(context, ip);
    }

    return true;
}
