#include "pch.h"
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
    void reduceStackJumpsOp(ByteCodeOptContext* context, ByteCodeInstruction* ip, ByteCodeOp op0, ByteCodeOp op1)
    {
        bool isDeRef = false;
        bool isZero  = false;

        switch (op0)
        {
            case ByteCodeOp::JumpIfFalse:
            case ByteCodeOp::JumpIfTrue:
            case ByteCodeOp::JumpIfZero8:
            case ByteCodeOp::JumpIfNotZero8:
            case ByteCodeOp::JumpIfZero16:
            case ByteCodeOp::JumpIfNotZero16:
            case ByteCodeOp::JumpIfZero32:
            case ByteCodeOp::JumpIfNotZero32:
            case ByteCodeOp::JumpIfZero64:
            case ByteCodeOp::JumpIfNotZero64:
                isZero = true;
                break;
        }
        switch (op1)
        {
            case ByteCodeOp::JumpIfDeRefEqual8:
            case ByteCodeOp::JumpIfDeRefEqual16:
            case ByteCodeOp::JumpIfDeRefEqual32:
            case ByteCodeOp::JumpIfDeRefEqual64:
            case ByteCodeOp::JumpIfDeRefNotEqual8:
            case ByteCodeOp::JumpIfDeRefNotEqual16:
            case ByteCodeOp::JumpIfDeRefNotEqual32:
            case ByteCodeOp::JumpIfDeRefNotEqual64:
                isDeRef = true;
                break;
        }

        if (isDeRef && (ip[0].a.u32 == ip[0].b.u32))
            return;

        for (int i = 1; i <= 3; i++)
        {
            if (ip[i].op != op0)
                continue;
            if (ip[i].hasFlag(BCI_START_STMT))
                continue;

            // If the second instruction has a deref or a stack access on the second argument, then pass it to the first
            // argument to be able to do the optim in another pass
            if (!isZero &&
                ByteCode::hasOpFlag(op0, OPF_READ_C) &&
                ip[i].c.u32 == ip[0].a.u32 &&
                !ip[i].hasFlag(BCI_IMM_A | BCI_IMM_C) &&
                !ip[i].hasDynFlag(BCID_SWAP))
            {
                ip[i].addDynFlag(BCID_SWAP);
                std::swap(ip[i].a, ip[i].c);
                context->setDirtyPass();
                return;
            }

            if (ip[i].a.u32 == ip[0].a.u32 &&
                !ip[i].hasFlag(BCI_IMM_A))
            {
                for (int j = 1; j < i; j++)
                {
                    if (ip[j].hasFlag(BCI_START_STMT) ||
                        ByteCode::hasWriteRefToReg(ip + j, ip->a.u32) ||
                        (isDeRef && ByteCode::hasWriteRefToReg(ip + j, ip->b.u32)))
                        return;

                    if (!ip[j].hasOpFlag(OPF_REG_ONLY | OPF_REG_READ))
                        return;
                }

                SET_OP(ip + i, op1);
                ip[i].a.u64 = ip[0].b.u64;

                // Store the deref offset in D
                if (isDeRef)
                {
                    ip[i].d.u64 = ip[0].c.u64;
                }

                // If we are testing against zero, put an immediate 0 value in C
                if (isZero)
                {
                    ip[i].addFlag(BCI_IMM_C);
                    ip[i].c.u64 = 0;
                }
                break;
            }
        }
    }
}

void ByteCodeOptimizer::reduceStackJumps(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::GetFromStack8:
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfFalse, ByteCodeOp::JumpIfStackEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfTrue, ByteCodeOp::JumpIfStackNotEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero8, ByteCodeOp::JumpIfStackEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero8, ByteCodeOp::JumpIfStackNotEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual8, ByteCodeOp::JumpIfStackEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual8, ByteCodeOp::JumpIfStackNotEqual8);
            break;

        case ByteCodeOp::GetFromStack16:
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero16, ByteCodeOp::JumpIfStackEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero16, ByteCodeOp::JumpIfStackNotEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual16, ByteCodeOp::JumpIfStackEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual16, ByteCodeOp::JumpIfStackNotEqual16);
            break;

        case ByteCodeOp::GetFromStack32:
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero32, ByteCodeOp::JumpIfStackEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero32, ByteCodeOp::JumpIfStackNotEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual32, ByteCodeOp::JumpIfStackEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual32, ByteCodeOp::JumpIfStackNotEqual32);
            break;

        case ByteCodeOp::GetFromStack64:
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero64, ByteCodeOp::JumpIfStackEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero64, ByteCodeOp::JumpIfStackNotEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual64, ByteCodeOp::JumpIfStackEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual64, ByteCodeOp::JumpIfStackNotEqual64);
            break;

        case ByteCodeOp::DeRef8:
            if (ip->c.u64 >= 0xFFFFFFFF)
                break;
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfFalse, ByteCodeOp::JumpIfDeRefEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfTrue, ByteCodeOp::JumpIfDeRefNotEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero8, ByteCodeOp::JumpIfDeRefEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero8, ByteCodeOp::JumpIfDeRefNotEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual8, ByteCodeOp::JumpIfDeRefEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual8, ByteCodeOp::JumpIfDeRefNotEqual8);
            break;

        case ByteCodeOp::DeRef16:
            if (ip->c.u64 >= 0xFFFFFFFF)
                break;
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero16, ByteCodeOp::JumpIfDeRefEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero16, ByteCodeOp::JumpIfDeRefNotEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual16, ByteCodeOp::JumpIfDeRefEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual16, ByteCodeOp::JumpIfDeRefNotEqual16);
            break;

        case ByteCodeOp::DeRef32:
            if (ip->c.u64 >= 0xFFFFFFFF)
                break;
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero32, ByteCodeOp::JumpIfDeRefEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero32, ByteCodeOp::JumpIfDeRefNotEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual32, ByteCodeOp::JumpIfDeRefEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual32, ByteCodeOp::JumpIfDeRefNotEqual32);
            break;

        case ByteCodeOp::DeRef64:
            if (ip->c.u64 >= 0xFFFFFFFF)
                break;
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero64, ByteCodeOp::JumpIfDeRefEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero64, ByteCodeOp::JumpIfDeRefNotEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual64, ByteCodeOp::JumpIfDeRefEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual64, ByteCodeOp::JumpIfDeRefNotEqual64);
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
    if (!ip->dynFlags.has(BCID_SWAP) &&
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
            ip->dynFlags.add(BCID_SWAP);
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
