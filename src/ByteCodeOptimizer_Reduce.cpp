#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"
#include "AstNode.h"

void ByteCodeOptimizer::reduceErr(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    // Useless pop/push
    if (ip[0].op == ByteCodeOp::InternalPushErr &&
        ip[1].op == ByteCodeOp::InternalPopErr)
    {
        setNop(context, ip);
        setNop(context, ip + 1);
    }

    // GetErr/Jump just after
    if (ip[0].op == ByteCodeOp::IntrinsicGetErr &&
        ip[1].op == ByteCodeOp::JumpIfZero64 &&
        ip[1].a.u32 == ip[0].b.u32 &&
        ip[0].flags & BCI_TRYCATCH &&
        ip[1].flags & BCI_TRYCATCH &&
        ip[1].b.s32 == 0)
    {
        setNop(context, ip);
        setNop(context, ip + 1);
    }

    // GetErr/Jump on another GetErr/Jump, make a shortcut
    if (ip[0].op == ByteCodeOp::IntrinsicGetErr && ip[1].op == ByteCodeOp::JumpIfZero64)
    {
        auto ipNext = &ip[1] + ip[1].b.s32 + 1;
        if (ipNext[0].op == ByteCodeOp::IntrinsicGetErr && ipNext[1].op == ByteCodeOp::JumpIfZero64)
        {
            ip[1].b.s32 += ipNext[1].b.s32 + 2;
            context->passHasDoneSomething = true;
        }
    }

    // If there's not SetErr between push and pop, then remove them
    if (ip[0].op == ByteCodeOp::InternalPushErr)
    {
        auto                 ipScan    = ip + 1;
        auto                 cpt       = 1;
        bool                 hasSetErr = false;
        ByteCodeInstruction* ipEnd     = nullptr;
        while (cpt)
        {
            if (ipScan[0].op == ByteCodeOp::InternalPushErr)
                cpt++;
            if (ipScan[0].op == ByteCodeOp::InternalPopErr)
            {
                cpt--;
                if (!cpt)
                    ipEnd = ipScan;
            }

            if (ipScan[0].op == ByteCodeOp::End ||
                ipScan[0].op == ByteCodeOp::IntrinsicSetErr ||
                ipScan[0].op == ByteCodeOp::LambdaCall ||
                ipScan[0].op == ByteCodeOp::LocalCall ||
                ipScan[0].op == ByteCodeOp::ForeignCall)
            {
                hasSetErr = true;
                break;
            }

            ipScan++;
        }

        if (!hasSetErr)
        {
            setNop(context, ip);
            setNop(context, ipEnd);
        }
    }
}

void ByteCodeOptimizer::reduceEmptyFct(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (context->bc->isDoingNothing() && !context->bc->isEmpty)
    {
        context->bc->isEmpty          = true;
        context->passHasDoneSomething = true;
    }

    if (ip->op == ByteCodeOp::LocalCall)
    {
        auto destBC = (ByteCode*) ip->a.pointer;
        if (destBC->isEmpty.load() != true)
            return;

        // We eliminate the call to the function that does nothing
        setNop(context, ip);
        if (ip[1].op == ByteCodeOp::IncSPPostCall)
            setNop(context, ip + 1);

        // Then we can eliminate some instructions related to the function call parameters
        auto backIp = ip;
        if (!(backIp->flags & BCI_START_STMT))
        {
            while (backIp != context->bc->out &&
                   backIp->op != ByteCodeOp::LocalCall &&
                   backIp->op != ByteCodeOp::ForeignCall &&
                   backIp->op != ByteCodeOp::LambdaCall)
            {
                if (backIp->op == ByteCodeOp::PushRVParam ||
                    backIp->op == ByteCodeOp::PushRAParam ||
                    backIp->op == ByteCodeOp::PushRAParam2 ||
                    backIp->op == ByteCodeOp::PushRAParam3 ||
                    backIp->op == ByteCodeOp::PushRAParam4 ||
                    backIp->op == ByteCodeOp::CopySPVaargs ||
                    backIp->op == ByteCodeOp::CopySP)
                {
                    setNop(context, backIp);
                }

                if (backIp->flags & BCI_START_STMT)
                    break;
                backIp--;
            }
        }
    }
}

void ByteCodeOptimizer::reduceMemcpy(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    // Copy a constant value (from segment) to the stack
    if (ip->op == ByteCodeOp::MakeConstantSegPointer &&
        ip[1].op == ByteCodeOp::MakeStackPointer &&
        ByteCode::isMemCpy(ip + 2) &&
        ip->a.u32 == ip[2].b.u32 &&
        ip[1].a.u32 == ip[2].a.u32)
    {
        switch (ip[2].op)
        {
        case ByteCodeOp::MemCpy8:
        {
            auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer8);
            ip[1].a.u64 = ip[1].b.u64;
            ip[1].b.u64 = *(uint8_t*) ptr;
            ip[1].flags |= BCI_IMM_B;
            setNop(context, ip + 2);
            break;
        }
        case ByteCodeOp::MemCpy16:
        {
            auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer16);
            ip[1].a.u64 = ip[1].b.u64;
            ip[1].b.u64 = *(uint16_t*) ptr;
            ip[1].flags |= BCI_IMM_B;
            setNop(context, ip + 2);
            break;
        }
        case ByteCodeOp::MemCpy32:
        {
            auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer32);
            ip[1].a.u64 = ip[1].b.u64;
            ip[1].b.u64 = *(uint32_t*) ptr;
            ip[1].flags |= BCI_IMM_B;
            setNop(context, ip + 2);
            break;
        }
        case ByteCodeOp::MemCpy64:
        {
            auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer64);
            ip[1].a.u64 = ip[1].b.u64;
            ip[1].b.u64 = *(uint64_t*) ptr;
            ip[1].flags |= BCI_IMM_B;
            setNop(context, ip + 2);
            break;
        }
        }
    }
}

void ByteCodeOptimizer::reduceAppend(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    auto opFlags = g_ByteCodeOpDesc[(int) ip->op].flags;

    // Multiple InternalGetTlsPtr, just copy registers, as InternalGetTlsPtr has a function call cost
    if (ip[0].op == ByteCodeOp::InternalGetTlsPtr &&
        ip[1].op == ByteCodeOp::InternalGetTlsPtr &&
        ip[2].op != ByteCodeOp::InternalGetTlsPtr &&
        ip[2].op != ByteCodeOp::IncPointer64 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA64);
        ip[1].b.u32 = ip[0].a.u32;
    }

    // A = something followed by B = A
    // make B = something, this gives the opportunity to remove one of them
    if (ip[1].op == ByteCodeOp::CopyRBtoRA64 &&
        (opFlags & OPFLAG_WRITE_A) &&
        !(opFlags & OPFLAG_READ_A) &&
        (!(opFlags & OPFLAG_READ_B) || ip->flags & BCI_IMM_B || ip->b.u32 != ip->a.u32) &&
        (!(opFlags & OPFLAG_READ_C) || ip->flags & BCI_IMM_C || ip->c.u32 != ip->a.u32) &&
        (!(opFlags & OPFLAG_READ_D) || ip->flags & BCI_IMM_D || ip->d.u32 != ip->a.u32) &&
        ip->a.u32 == ip[1].b.u32 &&
        !(ip[0].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        // List can be extended, as long as the instruction does not have side effects when called twice
        switch (ip->op)
        {
        case ByteCodeOp::CopyRBtoRA64:
        case ByteCodeOp::CopyRTtoRC:
        case ByteCodeOp::MakeStackPointer:
        case ByteCodeOp::GetFromStack8:
        case ByteCodeOp::GetFromStack16:
        case ByteCodeOp::GetFromStack32:
        case ByteCodeOp::GetFromStack64:
        case ByteCodeOp::GetFromStackParam64:
        case ByteCodeOp::NegBool:
        case ByteCodeOp::CastBool8:
        case ByteCodeOp::CastBool16:
        case ByteCodeOp::CastBool32:
        case ByteCodeOp::CastBool64:
        case ByteCodeOp::MakeConstantSegPointer:
        case ByteCodeOp::MakeMutableSegPointer:
        case ByteCodeOp::MakeBssSegPointer:
        case ByteCodeOp::MakeCompilerSegPointer:
        case ByteCodeOp::MakeLambda:
        case ByteCodeOp::DeRef8:
        case ByteCodeOp::DeRef16:
        case ByteCodeOp::DeRef32:
        case ByteCodeOp::DeRef64:
        {
            auto s                        = ip[1].a.u32;
            ip[1]                         = ip[0];
            ip[1].a.u32                   = s;
            context->passHasDoneSomething = true;
            break;
        }
        }
    }
}

void ByteCodeOptimizer::reduceSwap(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->op == ByteCodeOp::Ret || ip[1].op == ByteCodeOp::Ret || ip[2].op == ByteCodeOp::Ret)
        return;
    if (ip->op == ByteCodeOp::Nop || ip[1].op == ByteCodeOp::Nop || ip[2].op == ByteCodeOp::Nop)
        return;
    if (ByteCode::isJump(ip) || ByteCode::isJump(ip + 1) || ByteCode::isJump(ip + 2))
        return;
    if (ip->flags & BCI_START_STMT || ip[1].flags & BCI_START_STMT || ip[2].flags & BCI_START_STMT)
        return;

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

void ByteCodeOptimizer::reduceStack(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if ((ip[0].op == ByteCodeOp::GetFromStackParam64) &&
        (ip[1].op == ByteCodeOp::ClearMaskU32) &&
        (ip[1].b.u32 == 0xFF) &&
        (ip[0].a.u32 == ip[1].a.u32) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip, ByteCodeOp::GetFromStackParam8);
        setNop(context, ip + 1);
    }

    if ((ip[0].op == ByteCodeOp::GetFromStackParam64) &&
        (ip[1].op == ByteCodeOp::ClearMaskU32) &&
        (ip[1].b.u32 == 0xFFFF) &&
        (ip[0].a.u32 == ip[1].a.u32) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip, ByteCodeOp::GetFromStackParam16);
        setNop(context, ip + 1);
    }

    if ((ip[0].op == ByteCodeOp::GetFromStackParam64) &&
        (ip[1].op == ByteCodeOp::ClearMaskU64) &&
        (ip[1].b.u32 == 0xFFFFFFFF) &&
        (ip[0].a.u32 == ip[1].a.u32) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip, ByteCodeOp::GetFromStackParam32);
        setNop(context, ip + 1);
    }

    if ((ip[0].op == ByteCodeOp::MakeStackPointer) &&
        (ip[1].op == ByteCodeOp::CopyRCtoRT) &&
        (ip[0].a.u32 == ip[1].a.u32) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::MakeStackPointerRT);
        ip[1].a.u32 = ip[0].b.u32;
    }

    // Clear stack followed by a setstack
    if ((ip[0].op == ByteCodeOp::SetZeroStack32) &&
        (ip[1].op == ByteCodeOp::SetAtStackPointer32) &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip);
    }

    if ((ip[0].op == ByteCodeOp::SetZeroStack64) &&
        (ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip);
    }

    // CopyRBToRa, by convention, clear the remaining bits. So no need for a cast just after
    if ((ip[0].op == ByteCodeOp::CopyRBtoRA8) &&
        (ip[1].op == ByteCodeOp::ClearMaskU32 || ip[1].op == ByteCodeOp::ClearMaskU64) &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip + 1);
    }

    if ((ip[0].op == ByteCodeOp::CopyRBtoRA16) &&
        (ip[1].op == ByteCodeOp::ClearMaskU32) &&
        ip[1].b.u32 >= 0xFFFF &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip + 1);
    }

    if ((ip[0].op == ByteCodeOp::CopyRBtoRA16) &&
        (ip[1].op == ByteCodeOp::ClearMaskU64) &&
        ip[1].b.u64 >= 0xFFFF &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip + 1);
    }

    if ((ip[0].op == ByteCodeOp::CopyRBtoRA32) &&
        (ip[1].op == ByteCodeOp::ClearMaskU64) &&
        ip[1].b.u64 == 0xFFFFFFFF &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip + 1);
    }

    // DeRef, by convention, clear the remaining bits. So no need for a cast just after
    if (ip[0].op == ByteCodeOp::DeRef8 &&
        (ip[1].op == ByteCodeOp::ClearMaskU32 || ip[1].op == ByteCodeOp::ClearMaskU64) &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::DeRef16 &&
        (ip[1].op == ByteCodeOp::ClearMaskU32) &&
        (ip[1].b.u32 >= 0xFFFF) &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::DeRef16 &&
        (ip[1].op == ByteCodeOp::ClearMaskU64) &&
        (ip[1].b.u64 >= 0xFFFF) &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::DeRef32 &&
        ip[1].op == ByteCodeOp::ClearMaskU64 &&
        ip[1].b.u64 >= 0xFFFFFFFF &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip + 1);
    }

    // Copy64 followed by cast
    if (ip[0].op == ByteCodeOp::CopyRBtoRA64 &&
        ip[1].op == ByteCodeOp::ClearMaskU64 &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        switch (ip[1].b.u64)
        {
        case 0xFF:
            SET_OP(ip, ByteCodeOp::CopyRBtoRA8);
            setNop(context, ip + 1);
            break;
        case 0xFFFF:
            SET_OP(ip, ByteCodeOp::CopyRBtoRA16);
            setNop(context, ip + 1);
            break;
        case 0xFFFFFFFF:
            SET_OP(ip, ByteCodeOp::CopyRBtoRA32);
            setNop(context, ip + 1);
            break;
        }
    }

    if (ip[0].op == ByteCodeOp::CopyRBtoRA64 &&
        ip[1].op == ByteCodeOp::ClearMaskU32 &&
        ip[0].a.u32 == ip[1].a.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        switch (ip[1].b.u64)
        {
        case 0xFF:
            SET_OP(ip, ByteCodeOp::CopyRBtoRA8);
            setNop(context, ip + 1);
            break;
        case 0xFFFF:
            SET_OP(ip, ByteCodeOp::CopyRBtoRA16);
            setNop(context, ip + 1);
            break;
        }
    }

    // Indirect assign to register. Make it direct
    if (ip[0].op == ByteCodeOp::ClearRA &&
        ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
        ip[2].op == ByteCodeOp::SetAtPointer64 &&
        !(ip[1].flags & BCI_START_STMT) &&
        !(ip[2].flags & BCI_START_STMT) &&
        ip[2].c.u32 == 0 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        ip[1].a.u32 == ip[2].a.u32)
    {
        if (ip[2].flags & BCI_IMM_B)
        {
            SET_OP(ip + 2, ByteCodeOp::SetImmediate64);
        }
        else
        {
            SET_OP(ip + 2, ByteCodeOp::CopyRBtoRA64);
        }

        ip[2].a.u32 = ip[0].a.u32;
        setNop(context, ip);
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::ClearRA &&
        ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
        ip[2].op == ByteCodeOp::SetAtPointer32 &&
        !(ip[1].flags & BCI_START_STMT) &&
        !(ip[2].flags & BCI_START_STMT) &&
        ip[2].c.u32 == 0 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        ip[1].a.u32 == ip[2].a.u32)
    {
        if (ip[2].flags & BCI_IMM_B)
        {
            ip[2].b.u64 = ip[2].b.u32;
            SET_OP(ip + 2, ByteCodeOp::SetImmediate64);
        }
        else
        {
            SET_OP(ip + 2, ByteCodeOp::CopyRBtoRA32);
        }

        ip[2].a.u32 = ip[0].a.u32;
        setNop(context, ip);
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::ClearRA &&
        ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
        ip[2].op == ByteCodeOp::SetAtPointer16 &&
        !(ip[1].flags & BCI_START_STMT) &&
        !(ip[2].flags & BCI_START_STMT) &&
        ip[2].c.u32 == 0 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        ip[1].a.u32 == ip[2].a.u32)
    {
        if (ip[2].flags & BCI_IMM_B)
        {
            ip[2].b.u64 = ip[2].b.u16;
            SET_OP(ip + 2, ByteCodeOp::SetImmediate64);
        }
        else
        {
            SET_OP(ip + 2, ByteCodeOp::CopyRBtoRA16);
        }

        ip[2].a.u32 = ip[0].a.u32;
        setNop(context, ip);
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::ClearRA &&
        ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
        ip[2].op == ByteCodeOp::SetAtPointer8 &&
        !(ip[1].flags & BCI_START_STMT) &&
        !(ip[2].flags & BCI_START_STMT) &&
        ip[2].c.u32 == 0 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        ip[1].a.u32 == ip[2].a.u32)
    {
        if (ip[2].flags & BCI_IMM_B)
        {
            ip[2].b.u64 = ip[2].b.u8;
            SET_OP(ip + 2, ByteCodeOp::SetImmediate64);
        }
        else
        {
            SET_OP(ip + 2, ByteCodeOp::CopyRBtoRA8);
        }

        ip[2].a.u32 = ip[0].a.u32;
        setNop(context, ip);
        setNop(context, ip + 1);
    }

    // Replace GetFromStack with SetImmediate
    if (ip[0].op == ByteCodeOp::SetAtStackPointer8 &&
        ip[1].op == ByteCodeOp::GetFromStack8 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        ip[0].flags & BCI_IMM_B &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
        ip[1].b.u64 = ip[0].b.u8;
        ip[1].flags &= ~BCI_IMM_A;
        ip[1].flags |= BCI_IMM_B;
    }

    if (ip[0].op == ByteCodeOp::SetAtStackPointer16 &&
        ip[1].op == ByteCodeOp::GetFromStack16 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        ip[0].flags & BCI_IMM_B &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
        ip[1].b.u64 = ip[0].b.u16;
        ip[1].flags &= ~BCI_IMM_A;
        ip[1].flags |= BCI_IMM_B;
    }

    if (ip[0].op == ByteCodeOp::SetAtStackPointer32 &&
        ip[1].op == ByteCodeOp::GetFromStack32 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        ip[0].flags & BCI_IMM_B &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
        ip[1].b.u64 = ip[0].b.u32;
        ip[1].flags &= ~BCI_IMM_A;
        ip[1].flags |= BCI_IMM_B;
    }

    if (ip[0].op == ByteCodeOp::SetAtStackPointer64 &&
        ip[1].op == ByteCodeOp::GetFromStack64 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        ip[0].flags & BCI_IMM_B &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::SetImmediate64);
        ip[1].b.u64 = ip[0].b.u64;
        ip[1].flags &= ~BCI_IMM_A;
        ip[1].flags |= BCI_IMM_B;
    }

    // MakeStackPointer followed by SetAtPointer, replace with SetAtStackPointer, but
    // leave the MakeStackPointer which will be removed later (?) if no more used
    if (ip[0].op == ByteCodeOp::MakeStackPointer &&
        ip[1].op == ByteCodeOp::SetAtPointer8 &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer8);
        ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
    }

    if (ip[0].op == ByteCodeOp::MakeStackPointer &&
        ip[1].op == ByteCodeOp::SetAtPointer16 &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer16);
        ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
    }

    if (ip[0].op == ByteCodeOp::MakeStackPointer &&
        ip[1].op == ByteCodeOp::SetAtPointer32 &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer32);
        ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
    }

    if (ip[0].op == ByteCodeOp::MakeStackPointer &&
        ip[1].op == ByteCodeOp::SetAtPointer64 &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer64);
        ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
    }

    // MakeStackPointr followed by deref is equivalent to GetFromStack
    if (ip[0].op == ByteCodeOp::MakeStackPointer &&
        ip[1].op == ByteCodeOp::DeRef8 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::GetFromStack8);
        ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
    }

    if (ip[0].op == ByteCodeOp::MakeStackPointer &&
        ip[1].op == ByteCodeOp::DeRef16 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::GetFromStack16);
        ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
    }

    if (ip[0].op == ByteCodeOp::MakeStackPointer &&
        ip[1].op == ByteCodeOp::DeRef32 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::GetFromStack32);
        ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
    }

    if (ip[0].op == ByteCodeOp::MakeStackPointer &&
        ip[1].op == ByteCodeOp::DeRef64 &&
        ip[0].a.u32 == ip[1].b.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::GetFromStack64);
        ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
    }

    // Set/Get from stack
    if (ip[0].op == ByteCodeOp::SetAtStackPointer8 &&
        ip[1].op == ByteCodeOp::GetFromStack8 &&
        !(ip->flags & BCI_IMM_B) &&
        ip->b.u32 == ip[1].a.u32 &&
        ip->a.u32 == ip[1].b.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::ClearMaskU64);
        ip[1].b.u64 = 0xFF;
    }

    if (ip[0].op == ByteCodeOp::SetAtStackPointer16 &&
        ip[1].op == ByteCodeOp::GetFromStack16 &&
        !(ip->flags & BCI_IMM_B) &&
        ip->b.u32 == ip[1].a.u32 &&
        ip->a.u32 == ip[1].b.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::ClearMaskU64);
        ip[1].b.u64 = 0xFFFF;
    }

    if (ip[0].op == ByteCodeOp::SetAtStackPointer32 &&
        ip[1].op == ByteCodeOp::GetFromStack32 &&
        !(ip->flags & BCI_IMM_B) &&
        ip->b.u32 == ip[1].a.u32 &&
        ip->a.u32 == ip[1].b.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::ClearMaskU64);
        ip[1].b.u64 = 0xFFFFFFFF;
    }

    if (ip[0].op == ByteCodeOp::SetAtStackPointer64 &&
        ip[1].op == ByteCodeOp::GetFromStack64 &&
        !(ip->flags & BCI_IMM_B) &&
        ip->b.u32 == ip[1].a.u32 &&
        ip->a.u32 == ip[1].b.u32 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        setNop(context, ip + 1);
    }
}

void ByteCodeOptimizer::reduceIncPtr(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip[0].op == ByteCodeOp::IncPointer64 &&
        ip[0].flags & BCI_IMM_B &&
        ip[0].b.s64 > 0 && // Offset cannot be negative, so zap if incpointer is negative
        !(ip[1].flags & BCI_START_STMT))
    {
        // followed by DeRefStringSlice, set constant to deref
        if (ip[1].op == ByteCodeOp::DeRefStringSlice &&
            (ip->b.s64 + ip[1].c.s64 >= 0) &&
            (ip->b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
            ip->a.u32 == ip->c.u32 &&
            ip->c.u32 == ip[1].a.u32)
        {
            ip[1].c.s64 += ip->b.s64;
            setNop(context, ip);
        }

        // followed by SetZeroAtPointerX
        if (ip[1].op == ByteCodeOp::SetZeroAtPointerX &&
            (ip->b.s64 + ip[1].c.s64 >= 0) &&
            (ip->b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
            ip[0].c.u32 == ip[1].a.u32)
        {
            ip[1].a.u32 = ip[0].a.u32;
            ip[1].c.s64 += ip[0].b.s64;
            setNop(context, ip);
        }

        // Make DeRef with an offset
        if (ip[1].op == ByteCodeOp::DeRef8 &&
            (ip[0].b.s64 + ip[1].c.s64 >= 0) &&
            (ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
            ip[0].c.u32 == ip[1].b.u32 &&
            ip[1].a.u32 == ip[1].b.u32)
        {
            ip[1].b.u32 = ip[0].a.u32;
            ip[1].c.s64 += ip[0].b.s64;
            setNop(context, ip);
        }

        if (ip[1].op == ByteCodeOp::DeRef16 &&
            (ip[0].b.s64 + ip[1].c.s64 >= 0) &&
            (ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
            ip[0].c.u32 == ip[1].b.u32 &&
            ip[1].a.u32 == ip[1].b.u32)
        {
            ip[1].b.u32 = ip[0].a.u32;
            ip[1].c.s64 += ip[0].b.s64;
            setNop(context, ip);
        }

        if (ip[1].op == ByteCodeOp::DeRef32 &&
            (ip[0].b.s64 + ip[1].c.s64 >= 0) &&
            (ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
            ip[0].c.u32 == ip[1].b.u32 &&
            ip[1].a.u32 == ip[1].b.u32)
        {
            ip[1].b.u32 = ip[0].a.u32;
            ip[1].c.s64 += ip[0].b.s64;
            setNop(context, ip);
        }

        if (ip[1].op == ByteCodeOp::DeRef64 &&
            (ip[0].b.s64 + ip[1].c.s64 >= 0) &&
            (ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
            ip[0].c.u32 == ip[1].b.u32 &&
            ip[1].a.u32 == ip[1].b.u32)
        {
            ip[1].b.u32 = ip[0].a.u32;
            ip[1].c.s64 += ip[0].b.s64;
            setNop(context, ip);
        }

        // IncPointer with src != dst, followed by one SetAtPointer
        // No need to to a nop, the optimizer will remove instruction if unused
        // (safer that the version below)
        if ((ip[1].op == ByteCodeOp::SetAtPointer8 ||
             ip[1].op == ByteCodeOp::SetAtPointer16 ||
             ip[1].op == ByteCodeOp::SetAtPointer32 ||
             ip[1].op == ByteCodeOp::SetAtPointer64) &&
            ip[0].a.u32 != ip[0].c.u32 &&
            ip[0].c.u32 == ip[1].a.u32 &&
            (ip[0].b.s64 + ip[1].c.s64 >= 0) &&
            (ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF))
        {
            ip[1].a.u32 = ip[0].a.u32;
            ip[1].c.s64 += ip[0].b.s64;
            context->passHasDoneSomething = true;
        }

        // followed by one or two SetAtPointer
        // Is this safe ?
        if ((ip[1].op == ByteCodeOp::SetAtPointer8 ||
             ip[1].op == ByteCodeOp::SetAtPointer16 ||
             ip[1].op == ByteCodeOp::SetAtPointer32 ||
             ip[1].op == ByteCodeOp::SetAtPointer64) &&
            ip[0].a.u32 == ip[0].c.u32 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            ip[1].c.s64 += ip[0].b.s64;
            if (ip[2].op == ip[1].op && ip[0].a.u32 == ip[2].a.u32)
            {
                SWAG_ASSERT(!(ip[2].flags & BCI_START_STMT));
                ip[2].c.s64 += ip[0].b.s64;
            }
            setNop(context, ip);
        }

        if ((ip[1].op == ByteCodeOp::SetZeroAtPointer8 ||
             ip[1].op == ByteCodeOp::SetZeroAtPointer16 ||
             ip[1].op == ByteCodeOp::SetZeroAtPointer32 ||
             ip[1].op == ByteCodeOp::SetZeroAtPointer64) &&
            ip[0].a.u32 == ip[0].c.u32 &&
            ip[0].a.u32 == ip[1].a.u32 &&
            (ip[0].b.s64 + ip[1].b.s64 >= 0) &&
            (ip[0].b.s64 + ip[1].b.s64 <= 0x7FFFFFFF))

        {
            ip[1].b.s64 += ip[0].b.s64;
            setNop(context, ip);
        }
    }

    // Occurs when setting a string.
    if (ip[0].op == ByteCodeOp::MakeStackPointer &&
        ip[2].op == ByteCodeOp::IncPointer64 &&
        ip[3].op == ByteCodeOp::SetAtPointer64 &&
        ip[0].a.u32 == ip[2].a.u32 &&
        ip[2].c.u32 == ip[3].a.u32 &&
        ip[2].flags & BCI_IMM_B &&
        ip[2].b.s64 > 0)
    {
        SET_OP(ip + 3, ByteCodeOp::SetAtStackPointer64);
        ip[3].a.u32 = ip[0].b.u32 + ip[2].b.u32;
    }

    // MakeStackPointer Reg, ImmB
    // IncPointer64     Reg, Reg, ImmB
    // We can change the offset of the MakeStackPointer and remove the IncPointer
    if (ip[0].op == ByteCodeOp::MakeStackPointer &&
        ip[1].op == ByteCodeOp::IncPointer64 &&
        ip[1].c.u32 == ip[0].a.u32 &&
        ip[1].c.u32 == ip[1].a.u32 &&
        ip[1].flags & BCI_IMM_B &&
        ip[1].b.s64 > 0)
    {
        ip[0].b.u32 += ip[1].b.u32;
        setNop(context, ip + 1);
    }
}

void ByteCodeOptimizer::reduceNoOp(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    // Remove unecessary DebugNop
    if (ip->op == ByteCodeOp::DebugNop && ip->location == ip[1].location)
        setNop(context, ip);

    // Useless pop/push
    if (ip[0].op == ByteCodeOp::InternalPushErr &&
        ip[1].op == ByteCodeOp::InternalPopErr)
    {
        setNop(context, ip);
        setNop(context, ip + 1);
    }

    // Useless multi return
    if (ip[0].op == ByteCodeOp::Ret &&
        ip[1].op == ByteCodeOp::Ret)
    {
        setNop(context, ip);
    }

    // Duplicated safety
    if (ip[0].op == ByteCodeOp::JumpIfNotZero64 &&
        ip[1].op == ByteCodeOp::InternalPanic &&
        ip[2].op == ByteCodeOp::JumpIfNotZero64 &&
        ip[3].op == ByteCodeOp::InternalPanic &&
        ip[0].a.u32 == ip[2].a.u32 &&
        ip[0].b.s32 == 1 &&
        ip[2].b.s32 == 1)
    {
        setNop(context, ip);
        setNop(context, ip + 1);
    }

    // GetFromStack8/16/32 clear the other bits by convention, so no need to
    // have a ClearMask after
    if (ip[0].op == ByteCodeOp::GetFromStack8 &&
        ip[1].op == ByteCodeOp::ClearMaskU64 &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::GetFromStack16 &&
        ip[1].op == ByteCodeOp::ClearMaskU64 &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::GetFromStack8 &&
        ip[1].op == ByteCodeOp::ClearMaskU32 &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::GetFromStack16 &&
        ip[1].op == ByteCodeOp::ClearMaskU32 &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::GetFromStack32 &&
        ip[1].op == ByteCodeOp::ClearMaskU64 &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        setNop(context, ip + 1);
    }

    // Remove operators which do nothing
    if ((ip->flags & BCI_IMM_B) &&
        !(ip->flags & BCI_IMM_A) &&
        !(ip->flags & BCI_IMM_C) &&
        ip->a.u32 == ip->c.u32)
    {
        switch (ip->op)
        {
        case ByteCodeOp::BinOpPlusS32:
        case ByteCodeOp::BinOpMinusS32:
        case ByteCodeOp::BinOpPlusU32:
        case ByteCodeOp::BinOpMinusU32:
            if (ip->b.u32 == 0)
                setNop(context, ip);
            break;

        case ByteCodeOp::BinOpPlusS64:
        case ByteCodeOp::BinOpPlusU64:
        case ByteCodeOp::BinOpMinusS64:
        case ByteCodeOp::BinOpMinusU64:
        case ByteCodeOp::IncPointer64:
        case ByteCodeOp::DecPointer64:
            if (ip->b.u64 == 0)
                setNop(context, ip);
            break;
        }
    }

    if ((ip->flags & BCI_IMM_B) &&
        !(ip->flags & BCI_IMM_A) &&
        !(ip->flags & BCI_IMM_C))
    {
        switch (ip->op)
        {
        case ByteCodeOp::BinOpMulU32:
        case ByteCodeOp::BinOpMulU64:
        case ByteCodeOp::BinOpMulS32:
        case ByteCodeOp::BinOpMulS64:
            if (ip->b.u32 == 1)
            {
                if (ip->a.u32 != ip->c.u32)
                {
                    SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                    auto s    = ip->a.u32;
                    ip->a.u32 = ip->c.u32;
                    ip->b.u32 = s;
                    ip->flags &= ~BCI_IMM_B;
                }
            }
            break;
        }
    }
}

void ByteCodeOptimizer::reduceSetAt(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    // Reduce SetZeroStack
    if (!(ip[1].flags & BCI_START_STMT))
    {
        uint32_t offset0, offset1;
        auto     size0 = ByteCode::getSetZeroStackSize(ip, offset0);
        auto     size1 = ByteCode::getSetZeroStackSize(ip + 1, offset1);
        if (size0 && size1 && offset0 + size0 == offset1)
        {
            auto totalSize = size0 + size1;
            switch (totalSize)
            {
            case 2:
                SET_OP(ip, ByteCodeOp::SetZeroStack16);
                break;
            case 4:
                SET_OP(ip, ByteCodeOp::SetZeroStack32);
                break;
            case 8:
                SET_OP(ip, ByteCodeOp::SetZeroStack64);
                break;
            default:
                SET_OP(ip, ByteCodeOp::SetZeroStackX);
                ip->a.u32 = offset0;
                SWAG_ASSERT(totalSize <= 0x7FFFFFFF);
                ip->b.u32 = totalSize;
                break;
            }

            setNop(context, ip + 1);
        }
    }
    // Reduce SetZeroAtPointer
    if (!(ip[1].flags & BCI_START_STMT))
    {
        uint32_t offset0, offset1;
        auto     size0 = ByteCode::getSetZeroAtPointerSize(ip, offset0);
        auto     size1 = ByteCode::getSetZeroAtPointerSize(ip + 1, offset1);
        if (size0 && size1 && offset0 + size0 == offset1)
        {
            auto totalSize = size0 + size1;
            switch (totalSize)
            {
            case 2:
                SET_OP(ip, ByteCodeOp::SetZeroAtPointer16);
                break;
            case 4:
                SET_OP(ip, ByteCodeOp::SetZeroAtPointer32);
                break;
            case 8:
                SET_OP(ip, ByteCodeOp::SetZeroAtPointer64);
                break;
            default:
                SET_OP(ip, ByteCodeOp::SetZeroAtPointerX);
                ip->c.s64 = offset0;
                SWAG_ASSERT(totalSize <= 0x7FFFFFFF);
                ip->b.s64 = totalSize;
                break;
            }

            setNop(context, ip + 1);
        }
    }

    // Reduce SetAtStackPointer
    if (ip->op == ByteCodeOp::SetAtStackPointer8 &&
        ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
        ip->a.u32 + sizeof(uint8_t) == ip[1].a.u32 &&
        !(ip[0].flags & BCI_START_STMT) &&
        !(ip[1].flags & BCI_START_STMT) &&
        (ip->flags & BCI_IMM_B) &&
        (ip[1].flags & BCI_IMM_B))
    {
        SET_OP(ip, ByteCodeOp::SetAtStackPointer16);
        ip->b.u16 |= ip[1].b.u8 << 8;
        setNop(context, ip + 1);
    }

    if (ip->op == ByteCodeOp::SetAtStackPointer16 &&
        ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
        ip->a.u32 + sizeof(uint16_t) == ip[1].a.u32 &&
        !(ip[0].flags & BCI_START_STMT) &&
        !(ip[1].flags & BCI_START_STMT) &&
        (ip->flags & BCI_IMM_B) &&
        (ip[1].flags & BCI_IMM_B))
    {
        SET_OP(ip, ByteCodeOp::SetAtStackPointer32);
        ip->b.u32 |= ip[1].b.u16 << 16;
        setNop(context, ip + 1);
    }

    if (ip->op == ByteCodeOp::SetAtStackPointer32 &&
        ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
        ip->a.u32 + sizeof(uint32_t) == ip[1].a.u32 &&
        !(ip[0].flags & BCI_START_STMT) &&
        !(ip[1].flags & BCI_START_STMT) &&
        (ip->flags & BCI_IMM_B) &&
        (ip[1].flags & BCI_IMM_B))
    {
        SET_OP(ip, ByteCodeOp::SetAtStackPointer64);
        ip->b.u64 |= (uint64_t) ip[1].b.u32 << 32;
        setNop(context, ip + 1);
    }
}

void ByteCodeOptimizer::reduceX2(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    // SetAtStackPointer x2
    if (ip[0].op == ByteCodeOp::SetAtStackPointer8 &&
        ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
        !(ip[0].flags & BCI_START_STMT) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip, ByteCodeOp::SetAtStackPointer8x2);
        ip[0].c.u64 = ip[1].a.u64;
        ip[0].d.u64 = ip[1].b.u64;
        if (ip[1].flags & BCI_IMM_B)
            ip[0].flags |= BCI_IMM_D;
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::SetAtStackPointer16 &&
        ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
        !(ip[0].flags & BCI_START_STMT) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip, ByteCodeOp::SetAtStackPointer16x2);
        ip[0].c.u64 = ip[1].a.u64;
        ip[0].d.u64 = ip[1].b.u64;
        if (ip[1].flags & BCI_IMM_B)
            ip[0].flags |= BCI_IMM_D;
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::SetAtStackPointer32 &&
        ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
        !(ip[0].flags & BCI_START_STMT) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip, ByteCodeOp::SetAtStackPointer32x2);
        ip[0].c.u64 = ip[1].a.u64;
        ip[0].d.u64 = ip[1].b.u64;
        if (ip[1].flags & BCI_IMM_B)
            ip[0].flags |= BCI_IMM_D;
        setNop(context, ip + 1);
    }

    if (ip[0].op == ByteCodeOp::SetAtStackPointer64 &&
        ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
        !(ip[0].flags & BCI_START_STMT) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip, ByteCodeOp::SetAtStackPointer64x2);
        ip[0].c.u64 = ip[1].a.u64;
        ip[0].d.u64 = ip[1].b.u64;
        if (ip[1].flags & BCI_IMM_B)
            ip[0].flags |= BCI_IMM_D;
        setNop(context, ip + 1);
    }

    // GetFromStack64x2
    if (ip[0].op == ByteCodeOp::GetFromStack64 &&
        ip[1].op == ByteCodeOp::GetFromStack64 &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip, ByteCodeOp::GetFromStack64x2);
        ip[0].c.u64 = ip[1].a.u64;
        ip[0].d.u64 = ip[1].b.u64;
        setNop(context, ip + 1);
    }
}

void ByteCodeOptimizer::reduceCmpJump(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    // NegBool followed by jump bool
    if (ip[0].op == ByteCodeOp::NegBool &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        !(ip[1].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_JUMP_DEST) &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfTrue);
        ip[1].a.u32 = ip->b.u32;
    }

    if (ip[0].op == ByteCodeOp::NegBool &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        !(ip[1].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_JUMP_DEST) &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfFalse);
        ip[1].a.u32 = ip->b.u32;
    }

    // Cast bool followed by a jump. Change the jump
    if (ip->op == ByteCodeOp::CastBool8 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip->a.u32 == ip[1].a.u32 &&
        !(ip[0].flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero8);
        ip[1].a.u32 = ip->b.u32;
    }

    if (ip->op == ByteCodeOp::CastBool16 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip->a.u32 == ip[1].a.u32 &&
        !(ip[0].flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero16);
        ip[1].a.u32 = ip->b.u32;
    }

    if (ip->op == ByteCodeOp::CastBool32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip->a.u32 == ip[1].a.u32 &&
        !(ip[0].flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
        ip[1].a.u32 = ip->b.u32;
    }

    if (ip->op == ByteCodeOp::CastBool64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip->a.u32 == ip[1].a.u32 &&
        !(ip[0].flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero64);
        ip[1].a.u32 = ip->b.u32;
    }

    //
    if (ip->op == ByteCodeOp::CastBool8 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip->a.u32 == ip[1].a.u32 &&
        !(ip[0].flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
        ip[1].a.u32 = ip->b.u32;
    }

    if (ip->op == ByteCodeOp::CastBool16 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip->a.u32 == ip[1].a.u32 &&
        !(ip[0].flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
        ip[1].a.u32 = ip->b.u32;
    }

    if (ip->op == ByteCodeOp::CastBool32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip->a.u32 == ip[1].a.u32 &&
        !(ip[0].flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
        ip[1].a.u32 = ip->b.u32;
    }

    if (ip->op == ByteCodeOp::CastBool64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip->a.u32 == ip[1].a.u32 &&
        !(ip[0].flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_IMM_A) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
        ip[1].a.u32 = ip->b.u32;
    }

    // Compare to == 0
    if (ip->flags & BCI_IMM_B && ip->a.u32 != ip->c.u32)
    {
        if (ip->op == ByteCodeOp::CompareOpEqual8 &&
            ip->b.u8 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero8);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual8 &&
            ip->b.u8 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual16 &&
            ip->b.u16 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero16);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual16 &&
            ip->b.u16 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual32 &&
            ip->b.u32 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual32 &&
            ip->b.u32 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual64 &&
            ip->b.u64 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero64);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual64 &&
            ip->b.u64 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual8 &&
            ip->b.u8 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfZero8 &&
            ip[1].a.u32 == ip->c.u32)
        {
            setNop(context, ip);
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual16 &&
            ip->b.u8 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfZero16 &&
            ip[1].a.u32 == ip->c.u32)
        {
            setNop(context, ip);
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual32 &&
            ip->b.u8 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfZero32 &&
            ip[1].a.u32 == ip->c.u32)
        {
            setNop(context, ip);
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpEqual64 &&
            ip->b.u64 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfNotZero8 &&
            ip[1].a.u32 == ip->c.u32)
        {
            setNop(context, ip);
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
            ip[1].a.u32 = ip->a.u32;
        }

        // Compare to != 0
        if (ip->op == ByteCodeOp::CompareOpNotEqual8 &&
            ip->b.u8 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpNotEqual8 &&
            ip->b.u8 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero8);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpNotEqual16 &&
            ip->b.u16 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpNotEqual16 &&
            ip->b.u16 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero16);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpNotEqual32 &&
            ip->b.u32 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpNotEqual32 &&
            ip->b.u32 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpNotEqual64 &&
            ip->b.u64 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
            ip[1].a.u32 = ip->a.u32;
        }

        if (ip->op == ByteCodeOp::CompareOpNotEqual64 &&
            ip->b.u64 == 0 &&
            ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero64);
            ip[1].a.u32 = ip->a.u32;
        }
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpEqual8 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual8);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpEqual16 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual16);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpEqual32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpEqual64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpEqual8 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfEqual8);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpEqual16 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfEqual16);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpEqual32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfEqual32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpEqual64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfEqual64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpNotEqual8 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfEqual8);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpNotEqual16 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfEqual16);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpNotEqual32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfEqual32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpNotEqual64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfEqual64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpNotEqual8 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual8);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpNotEqual16 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual16);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpNotEqual32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpNotEqual64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpLowerU32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerU64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerS32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerS64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerF32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerF32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerF64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerF64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpGreaterEqU32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterEqU64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterEqS32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterEqS64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterEqF32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerF32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterEqF64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerF64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpLowerU32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerU64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerS32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerS64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerF32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqF32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerF64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqF64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpGreaterEqU32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterEqU64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterEqS32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterEqS64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterEqF32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqF32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterEqF64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqF64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpGreaterU32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterU64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterS32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterS64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterF32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterF32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterF64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterF64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpLowerEqU32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerEqU64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerEqS32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerEqS64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerEqF32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterF32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerEqF64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterF64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpLowerEqU32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerEqU64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerEqS32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerEqS64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerEqF32 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqF32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpLowerEqF64 &&
        ip[1].op == ByteCodeOp::JumpIfTrue &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqF64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    // Mix compare and jump
    if (ip->op == ByteCodeOp::CompareOpGreaterU32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterU64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterS32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterS64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterF32 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqF32);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }

    if (ip->op == ByteCodeOp::CompareOpGreaterF64 &&
        ip[1].op == ByteCodeOp::JumpIfFalse &&
        ip[1].a.u32 == ip->c.u32 &&
        ip->a.u32 != ip->c.u32 &&
        (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
        !(ip[1].flags & BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqF64);
        ip[1].a.u64 = ip->a.u64;
        ip[1].c.u64 = ip->b.u64;
        ip[1].flags |= ip->flags & BCI_IMM_A;
        ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
    }
}

bool ByteCodeOptimizer::optimizePassReduce(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        reduceErr(context, ip);
        reduceEmptyFct(context, ip);
        reduceAppend(context, ip);
        reduceMemcpy(context, ip);
        reduceStack(context, ip);
        reduceIncPtr(context, ip);
        reduceSetAt(context, ip);
        reduceNoOp(context, ip);
        reduceCmpJump(context, ip);
        reduceX2(context, ip);
        reduceSwap(context, ip);
    }

    return true;
}