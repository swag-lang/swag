#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"
#include "AstNode.h"
#include "Math.h"

void ByteCodeOptimizer::reduceMath(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->flags & BCI_IMM_A)
        return;
    if (!(ip->flags & BCI_IMM_B))
        return;

    switch (ip->op)
    {
    case ByteCodeOp::BinOpDivU32:
        if (ip->b.u32 == 1)
        {
            SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
            ip->b.u32 = ip->a.u32;
            ip->a.u32 = ip->c.u32;
            ip->flags &= ~BCI_IMM_B;
            break;
        }

        if (isPowerOfTwo(ip->b.u32))
        {
            SET_OP(ip, ByteCodeOp::BinOpShiftRightU32);
            ip->b.u32 = (uint32_t) log2(ip->b.u32);
            break;
        }
        break;

    case ByteCodeOp::BinOpDivU64:
        if (ip->b.u64 == 1)
        {
            SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
            ip->b.u32 = ip->a.u32;
            ip->a.u32 = ip->c.u32;
            ip->flags &= ~BCI_IMM_B;
            break;
        }

        if (isPowerOfTwo(ip->b.u64))
        {
            SET_OP(ip, ByteCodeOp::BinOpShiftRightU64);
            ip->b.u64 = (uint64_t) log2(ip->b.u64);
            break;
        }
        break;

    case ByteCodeOp::BinOpDivS32:
        if (ip->b.u32 == 1)
        {
            SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
            ip->b.u32 = ip->a.u32;
            ip->a.u32 = ip->c.u32;
            ip->flags &= ~BCI_IMM_B;
            break;
        }

        if (isPowerOfTwo(ip->b.u32))
        {
            SET_OP(ip, ByteCodeOp::BinOpShiftRightS32);
            ip->b.u32 = (uint32_t) log2(ip->b.u32);
            break;
        }
        break;

    case ByteCodeOp::BinOpDivS64:
        if (ip->b.u64 == 1)
        {
            SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
            ip->b.u32 = ip->a.u32;
            ip->a.u32 = ip->c.u32;
            ip->flags &= ~BCI_IMM_B;
            break;
        }

        if (isPowerOfTwo(ip->b.u64))
        {
            SET_OP(ip, ByteCodeOp::BinOpShiftRightS64);
            ip->b.u64 = (uint64_t) log2(ip->b.u64);
            break;
        }
        break;

    case ByteCodeOp::BinOpModuloU32:
        if (isPowerOfTwo(ip->b.u32))
        {
            SET_OP(ip, ByteCodeOp::BinOpBitmaskAnd32);
            ip->b.u32 -= 1;
            break;
        }
        break;

    case ByteCodeOp::BinOpModuloU64:
        if (isPowerOfTwo(ip->b.u32))
        {
            SET_OP(ip, ByteCodeOp::BinOpBitmaskAnd64);
            ip->b.u64 -= 1;
            break;
        }
        break;

    case ByteCodeOp::BinOpMulU32_Safe:
        if (ip->b.u32 == 1)
        {
            SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
            ip->b.u32 = ip->a.u32;
            ip->a.u32 = ip->c.u32;
            ip->flags &= ~BCI_IMM_B;
            break;
        }

        if (isPowerOfTwo(ip->b.u32))
        {
            SET_OP(ip, ByteCodeOp::BinOpShiftLeftU32);
            ip->b.u32 = (uint32_t) log2(ip->b.u32);
            break;
        }
        break;

    case ByteCodeOp::BinOpMulU64_Safe:
        if (ip->b.u64 == 1)
        {
            SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
            ip->b.u32 = ip->a.u32;
            ip->a.u32 = ip->c.u32;
            ip->flags &= ~BCI_IMM_B;
            break;
        }

        if (isPowerOfTwo(ip->b.u32))
        {
            SET_OP(ip, ByteCodeOp::BinOpShiftLeftU64);
            ip->b.u64 = (uint32_t) log2(ip->b.u64);
            break;
        }
        break;

    case ByteCodeOp::BinOpMulS32_Safe:
        if (ip->b.u32 == 1)
        {
            SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
            ip->b.u32 = ip->a.u32;
            ip->a.u32 = ip->c.u32;
            ip->flags &= ~BCI_IMM_B;
            break;
        }

        if (isPowerOfTwo(ip->b.u32))
        {
            SET_OP(ip, ByteCodeOp::BinOpShiftLeftS32);
            ip->b.u32 = (uint32_t) log2(ip->b.u32);
            break;
        }
        break;

    case ByteCodeOp::BinOpMulS64_Safe:
        if (ip->b.u64 == 1)
        {
            SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
            ip->b.u32 = ip->a.u32;
            ip->a.u32 = ip->c.u32;
            ip->flags &= ~BCI_IMM_B;
            break;
        }

        if (isPowerOfTwo(ip->b.u32))
        {
            SET_OP(ip, ByteCodeOp::BinOpShiftLeftS64);
            ip->b.u64 = (uint32_t) log2(ip->b.u64);
            break;
        }
        break;
    }
}

void ByteCodeOptimizer::reduceFactor(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip[0].op)
    {
    case ByteCodeOp::CopyRBtoRA64:
        if (ip[1].op == ByteCodeOp::Mul64byVB64 &&
            !(ip[1].flags & BCI_START_STMT) &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip);
            SET_OP(ip + 1, ByteCodeOp::BinOpMulU64_Safe);
            ip[1].a.u32 = ip[0].b.u32;
            ip[1].c.u32 = ip[0].a.u32;
            ip[1].flags |= BCI_IMM_B;
            break;
        }
        break;
    }
}

void ByteCodeOptimizer::reduceErr(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip[0].op)
    {
    case ByteCodeOp::InternalClearErr:
    {
        auto ipScan = ip + 1;
        while (!ByteCode::isRet(ipScan))
        {
            if (ipScan->op == ByteCodeOp::InternalHasErr ||
                ipScan->op == ByteCodeOp::IntrinsicGetErr ||
                ByteCode::isJump(ipScan) ||
                (ipScan->flags & BCI_START_STMT))
                break;

            // An InternalClearErr followed by another one
            if (ipScan->op == ByteCodeOp::InternalClearErr)
            {
                setNop(context, ip);
                break;
            }

            // An InternalClearErr followed by a function call which will clear the error also
            if (ipScan->op == ByteCodeOp::LocalCall ||
                ipScan->op == ByteCodeOp::LocalCallPop ||
                ipScan->op == ByteCodeOp::LocalCallPopRC ||
                ipScan->op == ByteCodeOp::LambdaCall ||
                ipScan->op == ByteCodeOp::LambdaCallPop ||
                ipScan->op == ByteCodeOp::ForeignCall ||
                ipScan->op == ByteCodeOp::ForeignCallPop)
            {
                TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ipScan->b.pointer;
                if (typeFuncBC->flags & TYPEINFO_CAN_THROW)
                {
                    setNop(context, ip);
                    break;
                }
            }

            ipScan++;
        }
    }
    break;

    case ByteCodeOp::InternalHasErr:
        // Has err followed by ret
        if (ByteCode::isRet(ip + 1))
        {
            setNop(context, ip);
            break;
        }

        // GetErr/Jump just after
        if (ip[1].op == ByteCodeOp::JumpIfZero32 &&
            ip[1].a.u32 == ip[0].a.u32 &&
            ip[0].flags & BCI_TRYCATCH &&
            ip[1].flags & BCI_TRYCATCH &&
            ip[1].b.s32 == 0)
        {
            setNop(context, ip);
            setNop(context, ip + 1);
            break;
        }

        // Useless InternalHasErr
        if (ip[1].op == ByteCodeOp::JumpIfZero32 &&
            ip[2].op == ByteCodeOp::MakeConstantSegPointer &&
            ip[3].op == ByteCodeOp::InternalStackTrace &&
            ip[4].op == ByteCodeOp::InternalHasErr &&
            ip[5].op == ByteCodeOp::JumpIfZero32 &&
            !(ip[4].flags & BCI_START_STMT))
        {
            setNop(context, ip + 4);
            setNop(context, ip + 5);
            break;
        }

        // Useless InternalHasErr
        if (ip[1].op == ByteCodeOp::JumpIfZero32 &&
            ip[2].op == ByteCodeOp::InternalHasErr &&
            ip[3].op == ByteCodeOp::JumpIfZero32 &&
            !(ip[2].flags & BCI_START_STMT))
        {
            setNop(context, ip + 2);
            setNop(context, ip + 3);
            break;
        }

        // GetErr/Jump on another GetErr/Jump, make a shortcut
        if (ip[1].op == ByteCodeOp::JumpIfZero32)
        {
            auto ipNext = &ip[1] + ip[1].b.s32 + 1;
            if (ipNext[0].op == ByteCodeOp::InternalHasErr && ipNext[1].op == ByteCodeOp::JumpIfZero32)
            {
                ip[1].b.s32 += ipNext[1].b.s32 + 2;
                context->passHasDoneSomething = true;
                break;
            }
        }

        // InternalHasErr followed by return
        if (ip[1].op == ByteCodeOp::JumpIfZero32 &&
            ip[2].op == ByteCodeOp::Ret &&
            ip[1].b.s32 == 1 &&
            !(ip[2].flags & BCI_START_STMT) &&
            context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret)
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
            ip[1].b.s32 = (int32_t) (&context->bc->out[context->bc->numInstructions - 2] - (ip + 1) - 1);
            setNop(context, ip + 2);
            break;
        }

        break;

    case ByteCodeOp::InternalPushErr:
        // Useless pop/push
        if (ip[1].op == ByteCodeOp::InternalPopErr)
        {
            setNop(context, ip);
            setNop(context, ip + 1);
            break;
        }

        // If there's not SetErr between push and pop, then remove them
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
                    ipScan[0].op == ByteCodeOp::InternalSetErr ||
                    ipScan[0].op == ByteCodeOp::LambdaCall ||
                    ipScan[0].op == ByteCodeOp::LambdaCallPop ||
                    ipScan[0].op == ByteCodeOp::LocalCall ||
                    ipScan[0].op == ByteCodeOp::LocalCallPop ||
                    ipScan[0].op == ByteCodeOp::LocalCallPopRC ||
                    ipScan[0].op == ByteCodeOp::ForeignCall ||
                    ipScan[0].op == ByteCodeOp::ForeignCallPop)
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

        break;

    case ByteCodeOp::MakeConstantSegPointer:
        // Duplicated stack trace
        if (ip[1].op == ByteCodeOp::InternalStackTrace &&
            ip[2].op == ByteCodeOp::MakeConstantSegPointer &&
            ip[3].op == ByteCodeOp::InternalStackTrace &&
            !(ip[2].flags & BCI_START_STMT))
        {
            setNop(context, ip + 2);
            setNop(context, ip + 3);
        }
        break;
    }
}

void ByteCodeOptimizer::reduceCallEmptyFct(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->op == ByteCodeOp::LocalCall || ip->op == ByteCodeOp::LocalCallPop || ip->op == ByteCodeOp::LocalCallPopRC)
    {
        auto destBC = (ByteCode*) ip->a.pointer;
        if (destBC->isEmpty.load() != true)
            return;

        // We eliminate the call to the function that does nothing
        setNop(context, ip);
        if (ip[1].op == ByteCodeOp::IncSPPostCallCond)
            setNop(context, ip + 1);
        if (ip[1].op == ByteCodeOp::IncSPPostCall)
            setNop(context, ip + 1);
        if (ip[2].op == ByteCodeOp::IncSPPostCall)
            setNop(context, ip + 2);

        // Then we can eliminate some instructions related to the function call parameters
        auto backIp = ip;
        if (!(backIp->flags & BCI_START_STMT))
        {
            while (backIp != context->bc->out &&
                   backIp->op != ByteCodeOp::LocalCall &&
                   backIp->op != ByteCodeOp::LocalCallPop &&
                   backIp->op != ByteCodeOp::LocalCallPopRC &&
                   backIp->op != ByteCodeOp::ForeignCall &&
                   backIp->op != ByteCodeOp::ForeignCallPop &&
                   backIp->op != ByteCodeOp::LambdaCall &&
                   backIp->op != ByteCodeOp::LambdaCallPop)
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
    if (ip->op == ByteCodeOp::IntrinsicMemCpy && ip->flags & BCI_IMM_C)
    {
        switch (ip->c.u32)
        {
        case 1:
            SET_OP(ip, ByteCodeOp::MemCpy8);
            break;
        case 2:
            SET_OP(ip, ByteCodeOp::MemCpy16);
            break;
        case 4:
            SET_OP(ip, ByteCodeOp::MemCpy32);
            break;
        case 8:
            SET_OP(ip, ByteCodeOp::MemCpy64);
            break;
        }
    }

    else if (ip[0].op == ByteCodeOp::MakeStackPointer &&
             ip[1].op == ByteCodeOp::MakeStackPointer &&
             ip[2].a.u32 == ip[1].a.u32 &&
             ip[2].b.u32 == ip[0].a.u32 &&
             !(ip[1].flags & BCI_START_STMT) &&
             !(ip[2].flags & BCI_START_STMT))
    {
        switch (ip[2].op)
        {
        case ByteCodeOp::MemCpy8:
            SET_OP(ip + 2, ByteCodeOp::CopyStack8);
            ip[2].a.u32 = ip[1].b.u32;
            ip[2].b.u32 = ip[0].b.u32;
            break;

        case ByteCodeOp::MemCpy16:
            SET_OP(ip + 2, ByteCodeOp::CopyStack16);
            ip[2].a.u32 = ip[1].b.u32;
            ip[2].b.u32 = ip[0].b.u32;
            break;

        case ByteCodeOp::MemCpy32:
            SET_OP(ip + 2, ByteCodeOp::CopyStack32);
            ip[2].a.u32 = ip[1].b.u32;
            ip[2].b.u32 = ip[0].b.u32;
            break;

        case ByteCodeOp::MemCpy64:
            SET_OP(ip + 2, ByteCodeOp::CopyStack64);
            ip[2].a.u32 = ip[1].b.u32;
            ip[2].b.u32 = ip[0].b.u32;
            break;
        }
    }

    // Copy a constant value (from segment) to the stack
    else if (ip->op == ByteCodeOp::MakeConstantSegPointer &&
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
            SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer8);
            ip[2].a.u64 = ip[1].b.u64;
            ip[2].b.u64 = *(uint8_t*) ptr;
            ip[2].flags |= BCI_IMM_B;
            break;
        }
        case ByteCodeOp::MemCpy16:
        {
            auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
            SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer16);
            ip[2].a.u64 = ip[1].b.u64;
            ip[2].b.u64 = *(uint16_t*) ptr;
            ip[2].flags |= BCI_IMM_B;
            break;
        }
        case ByteCodeOp::MemCpy32:
        {
            auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
            SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer32);
            ip[2].a.u64 = ip[1].b.u64;
            ip[2].b.u64 = *(uint32_t*) ptr;
            ip[2].flags |= BCI_IMM_B;
            break;
        }
        case ByteCodeOp::MemCpy64:
        {
            auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
            SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer64);
            ip[2].a.u64 = ip[1].b.u64;
            ip[2].b.u64 = *(uint64_t*) ptr;
            ip[2].flags |= BCI_IMM_B;
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
        case ByteCodeOp::GetParam64:
        case ByteCodeOp::NegBool:
        case ByteCodeOp::CastBool8:
        case ByteCodeOp::CastBool16:
        case ByteCodeOp::CastBool32:
        case ByteCodeOp::CastBool64:
        case ByteCodeOp::MakeConstantSegPointer:
        case ByteCodeOp::MakeMutableSegPointer:
        case ByteCodeOp::MakeBssSegPointer:
        case ByteCodeOp::MakeCompilerSegPointer:
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

static bool isSwapBlocked(ByteCodeInstruction* ip)
{
    uint32_t a = UINT32_MAX;
    uint32_t c = UINT32_MAX;

    if (ip[0].flags & BCI_START_STMT)
        return true;

    switch (ip->op)
    {
    case ByteCodeOp::MakeStackPointer:
    case ByteCodeOp::GetFromStack8:
    case ByteCodeOp::GetFromStack16:
    case ByteCodeOp::GetFromStack32:
    case ByteCodeOp::GetFromStack64:
    case ByteCodeOp::GetParam64:
    case ByteCodeOp::SetImmediate32:
    case ByteCodeOp::SetImmediate64:
        a = ip->a.u32;
        break;
    case ByteCodeOp::IncPointer64:
        if (!(ip->flags & BCI_IMM_B))
            return false;
        a = ip->a.u32;
        c = ip->c.u32;
        break;
    default:
        return false;
    }

    if (ip[1].op == ip[0].op)
        return true;
    if (a != UINT32_MAX && ByteCode::hasRefToReg(ip + 1, a))
        return true;
    if (c != UINT32_MAX && ByteCode::hasRefToReg(ip + 1, c))
        return true;
    if (ip[1].op == ByteCodeOp::Ret || ip[1].op == ByteCodeOp::CopyRCtoRRRet)
        return true;
    if (ByteCode::isJump(ip + 1))
        return true;
    if (ip[1].flags & BCI_START_STMT)
        return true;
    if (ip->node->ownerInline != ip[1].node->ownerInline)
        return true;

    return false;
}

static bool canSwap(ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::MakeStackPointer:
        if (isSwapBlocked(ip))
            return false;
        if (ip[1].op != ByteCodeOp::GetFromStack8 &&
            ip[1].op != ByteCodeOp::GetFromStack16 &&
            ip[1].op != ByteCodeOp::GetFromStack32 &&
            ip[1].op != ByteCodeOp::GetFromStack64 &&
            isSwapBlocked(ip + 1))
            return false;
        return true;

    case ByteCodeOp::GetFromStack8:
    case ByteCodeOp::GetFromStack16:
    case ByteCodeOp::GetFromStack32:
    case ByteCodeOp::GetFromStack64:
    case ByteCodeOp::GetParam64:
    case ByteCodeOp::SetImmediate32:
    case ByteCodeOp::SetImmediate64:
        if (isSwapBlocked(ip))
            return false;
        if (isSwapBlocked(ip + 1))
            return false;
        return true;

    case ByteCodeOp::IncPointer64:
        if (!(ip->flags & BCI_IMM_B))
            return false;
        if (isSwapBlocked(ip))
            return false;
        if (isSwapBlocked(ip + 1))
            return false;
        return true;
    }

    return false;
}

void ByteCodeOptimizer::reduceSwap(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (canSwap(ip))
    {
        swap(ip[0], ip[1]);
        if (canSwap(ip))
        {
            swap(ip[0], ip[1]);
            return;
        }

        context->passHasDoneSomething = true;
    }
}

void ByteCodeOptimizer::reduceFunc(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::DecSPBP:
        if (ip[1].op == ByteCodeOp::CopyRCtoRRRet && ip[2].op == ByteCodeOp::End)
        {
            setNop(context, ip);
            ip[1].a.u64 = 0;
            break;
        }
        break;

    case ByteCodeOp::CopyRCtoRR:
        if (ip[1].op == ByteCodeOp::Ret)
        {
            ip[0].b.u64 = ip[0].a.u64;
            ip[0].a.u64 = ip[1].a.u64;
            ip[0].flags |= ip[0].flags & BCI_IMM_A ? BCI_IMM_B : 0;
            ip[0].flags &= ~BCI_IMM_A;
            SET_OP(ip, ByteCodeOp::CopyRCtoRRRet);
            if (!(ip[1].flags & BCI_START_STMT))
                setNop(context, ip + 1);
            break;
        }
        break;

    // Swap CopyRTtoRC and IncSPPostCall to put IncSPPostCall right next to the call, which
    // give the opportunity to optimize the call pop
    case ByteCodeOp::CopyRTtoRC:
        if (ip[1].op == ByteCodeOp::IncSPPostCall &&
            !(ip[1].flags & BCI_START_STMT))
        {
            swap(ip[0], ip[1]);
            context->passHasDoneSomething = true;
            break;
        }
        break;

    case ByteCodeOp::LocalCallPop:
        if (ip[1].op == ByteCodeOp::CopyRTtoRC &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::LocalCallPopRC);
            ip->d.u32 = ip[1].a.u32;
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::LocalCall:
        if (ip[1].op == ByteCodeOp::IncSPPostCall &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::LocalCallPop);
            ip->c.u32 = ip[1].a.u32;
            setNop(context, ip + 1);
            break;
        }
        break;
    case ByteCodeOp::ForeignCall:
        if (ip[1].op == ByteCodeOp::IncSPPostCall &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::ForeignCallPop);
            ip->c.u32 = ip[1].a.u32;
            setNop(context, ip + 1);
            break;
        }
        break;
    case ByteCodeOp::LambdaCall:
        if (ip[1].op == ByteCodeOp::IncSPPostCall &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::LambdaCallPop);
            ip->c.u32 = ip[1].a.u32;
            setNop(context, ip + 1);
            break;
        }
        break;
    }
}

void ByteCodeOptimizer::reduceStack(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::DecSPBP:
        if ((ip[1].op == ByteCodeOp::Ret) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            ip[1].a.u32 = 0;
            break;
        }
        break;

    case ByteCodeOp::SetBP:
        if ((ip[1].op == ByteCodeOp::Ret) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }
        break;

    case ByteCodeOp::CopyStack8:
        if (ip->a.u32 == ip->b.u32)
        {
            setNop(context, ip);
            break;
        }

        if (ip[1].op == ByteCodeOp::CopyStack8 &&
            !(ip[1].flags & BCI_START_STMT) &&
            ip[1].a.u32 == ip[0].a.u32 + 1 &&
            ip[1].b.u32 == ip[0].b.u32 + 1)
        {
            SET_OP(ip, ByteCodeOp::CopyStack16);
            setNop(context, ip + 1);
            break;
        }

        break;

    case ByteCodeOp::CopyStack16:
        if (ip->a.u32 == ip->b.u32)
        {
            setNop(context, ip);
            break;
        }

        if (ip[1].op == ByteCodeOp::CopyStack16 &&
            !(ip[1].flags & BCI_START_STMT) &&
            ip[1].a.u32 == ip[0].a.u32 + 2 &&
            ip[1].b.u32 == ip[0].b.u32 + 2)
        {
            SET_OP(ip, ByteCodeOp::CopyStack32);
            setNop(context, ip + 1);
            break;
        }

        break;

    case ByteCodeOp::CopyStack32:
        if (ip->a.u32 == ip->b.u32)
        {
            setNop(context, ip);
            break;
        }

        if (ip[1].op == ByteCodeOp::CopyStack32 &&
            !(ip[1].flags & BCI_START_STMT) &&
            ip[1].a.u32 == ip[0].a.u32 + 4 &&
            ip[1].b.u32 == ip[0].b.u32 + 4)
        {
            SET_OP(ip, ByteCodeOp::CopyStack64);
            setNop(context, ip + 1);
            break;
        }

        break;

    case ByteCodeOp::CopyStack64:
        if (ip->a.u32 == ip->b.u32)
        {
            setNop(context, ip);
            break;
        }

        break;

    case ByteCodeOp::GetFromStack8:
        if ((ip[1].op == ByteCodeOp::SetAtStackPointer8) &&
            ip[1].b.u32 == ip[0].a.u32 &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_IMM_B))
        {
            SET_OP(ip + 1, ByteCodeOp::CopyStack8);
            ip[1].b.u64 = ip[0].b.u64;
            break;
        }
        break;

    case ByteCodeOp::GetFromStack16:
        if ((ip[1].op == ByteCodeOp::SetAtStackPointer16) &&
            ip[1].b.u32 == ip[0].a.u32 &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_IMM_B))
        {
            SET_OP(ip + 1, ByteCodeOp::CopyStack16);
            ip[1].b.u64 = ip[0].b.u64;
            break;
        }
        break;

    case ByteCodeOp::GetFromStack32:
        if ((ip[1].op == ByteCodeOp::SetAtStackPointer32) &&
            ip[1].b.u32 == ip[0].a.u32 &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_IMM_B))
        {
            SET_OP(ip + 1, ByteCodeOp::CopyStack32);
            ip[1].b.u64 = ip[0].b.u64;
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU32) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].b.u32 == 0xFF &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetFromStack8);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU32) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].b.u32 == 0xFFFF &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetFromStack16);
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::GetIncFromStack64:
        if ((ip[1].op == ByteCodeOp::IncPointer64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].c.u32 &&
            ip[1].flags & BCI_IMM_B &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip->c.s64 += ip[1].b.s64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef8) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef8);
            ip->c.u64 += ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef16) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef16);
            ip->c.u64 += ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef32) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef32);
            ip->c.u64 += ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef64);
            ip->c.u64 += ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::GetFromStack64:
        if ((ip[1].op == ByteCodeOp::DeRef8) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef8);
            ip->c.u64 = ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef16) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef16);
            ip->c.u64 = ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef32) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef32);
            ip->c.u64 = ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef64);
            ip->c.u64 = ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::IncPointer64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].c.u32 &&
            ip[1].flags & BCI_IMM_B &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncFromStack64);
            ip->c.u64 = ip[1].b.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
            ip[1].b.u32 == ip[0].a.u32 &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_IMM_B))
        {
            SET_OP(ip + 1, ByteCodeOp::CopyStack64);
            ip[1].b.u64 = ip[0].b.u64;
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].b.u64 == 0xFF &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetFromStack8);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].b.u64 == 0xFFFF &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetFromStack16);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].b.u64 == 0xFFFFFFFF &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetFromStack32);
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::MakeStackPointer:
        if ((ip[1].op == ByteCodeOp::SetZeroAtPointer8) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetZeroStack8);
            ip[1].a.u32 = ip[1].b.u32 + ip->b.u32;
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetZeroAtPointer16) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetZeroStack16);
            ip[1].a.u32 = ip[1].b.u32 + ip->b.u32;
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetZeroAtPointer32) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetZeroStack32);
            ip[1].a.u32 = ip[1].b.u32 + ip->b.u32;
            break;
        }

        if ((ip[1].op == ByteCodeOp::Add64byVB64) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].b.u32 += ip[1].b.u32;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetZeroAtPointer64) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetZeroStack64);
            ip[1].a.u32 = ip[1].b.u32 + ip->b.u32;
            break;
        }

        if ((ip[1].op == ByteCodeOp::CopyRCtoRT) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::MakeStackPointerRT);
            ip[1].a.u64 = ip[0].b.u32 + ip[1].b.u64;
            break;
        }

        // MakeStackPointer followed by SetAtPointer, replace with SetAtStackPointer, but
        // leave the MakeStackPointer which will be removed later (?) if no more used
        if (ip[1].op == ByteCodeOp::SetAtPointer8 &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer8);
            ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::SetAtPointer16 &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer16);
            ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::SetAtPointer32 &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer32);
            ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::SetAtPointer64 &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer64);
            ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::DeRef8 &&
            ip[0].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::GetFromStack8);
            ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::DeRef16 &&
            ip[0].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::GetFromStack16);
            ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::DeRef32 &&
            ip[0].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::GetFromStack32);
            ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::DeRef64 &&
            ip[0].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::GetFromStack64);
            ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
            break;
        }

        break;

    case ByteCodeOp::GetIncParam64:
        if ((ip[1].op == ByteCodeOp::IncPointer64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].c.u32 &&
            ip[1].flags & BCI_IMM_B &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip->d.s64 += ip[1].b.s64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef8) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64DeRef8);
            ip->d.u64 += ip[1].c.s64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef16) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64DeRef16);
            ip->d.u64 += ip[1].c.s64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef32) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64DeRef32);
            ip->d.u64 += ip[1].c.s64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64DeRef64);
            ip->d.u64 += ip[1].c.s64;
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::GetParam64:
        if ((ip[1].op == ByteCodeOp::ClearMaskU32) &&
            (ip[1].b.u32 == 0xFF) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetParam8);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU32) &&
            (ip[1].b.u32 == 0xFFFF) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetParam16);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU64) &&
            (ip[1].b.u64 == 0xFF) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetParam8);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU64) &&
            (ip[1].b.u64 == 0xFFFF) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetParam16);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU64) &&
            (ip[1].b.u64 == 0xFFFFFFFF) &&
            (ip[0].a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetParam32);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::IncPointer64) &&
            ip[2].op != ByteCodeOp::SetZeroAtPointer8 &&
            ip[2].op != ByteCodeOp::SetZeroAtPointer16 &&
            ip[2].op != ByteCodeOp::SetZeroAtPointer32 &&
            ip[2].op != ByteCodeOp::SetZeroAtPointer64 &&
            ip[2].op != ByteCodeOp::SetZeroAtPointerX &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].c.u32 &&
            ip[1].flags & BCI_IMM_B &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64);
            ip->d.u64 = ip[1].b.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef8) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            ip[1].c.s64 == 0 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetParam64DeRef8);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef16) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            ip[1].c.s64 == 0 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetParam64DeRef16);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef32) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            ip[1].c.s64 == 0 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetParam64DeRef32);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            ip[1].c.s64 == 0 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetParam64DeRef64);
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::MakeStackPointer) &&
            (ip[2].op == ByteCodeOp::IncPointer64) &&
            ip[0].a.u32 == ip[2].a.u32 &&
            ip[0].a.u32 != ip[1].a.u32 &&
            ip[2].flags & BCI_IMM_B &&
            ip[2].a.u32 == ip[2].c.u32 &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64);
            ip->d.u64 = ip[2].b.u64;
            setNop(context, ip + 2);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack64) &&
            (ip[2].op == ByteCodeOp::IncPointer64) &&
            ip[0].a.u32 == ip[2].a.u32 &&
            ip[0].a.u32 != ip[1].a.u32 &&
            ip[2].flags & BCI_IMM_B &&
            ip[2].a.u32 == ip[2].c.u32 &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64);
            ip->d.u64 = ip[2].b.u64;
            setNop(context, ip + 2);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef8) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64DeRef8);
            ip->d.u64 = ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef16) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64DeRef16);
            ip->d.u64 = ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef32) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64DeRef32);
            ip->d.u64 = ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::DeRef64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            ip[1].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::GetIncParam64DeRef64);
            ip->d.u64 = ip[1].c.u64;
            setNop(context, ip + 1);
            break;
        }

        break;

    case ByteCodeOp::SetZeroStack8:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
            ip[0].a.u32 + 1 == ip[1].a.u32 &&
            (ip[1].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[1].a.u32 -= 1;
            ip[1].b.u64 <<= 8;
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer16);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetAtStackPointer8 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer16 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetZeroStack8 ||
             ip[1].op == ByteCodeOp::SetZeroStack16 ||
             ip[1].op == ByteCodeOp::SetZeroStack32 ||
             ip[1].op == ByteCodeOp::SetZeroStack64) &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        break;

    case ByteCodeOp::SetZeroStack16:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
            ip[0].a.u32 + 2 == ip[1].a.u32 &&
            (ip[1].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[1].a.u32 -= 2;
            ip[1].b.u64 <<= 16;
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer32);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetAtStackPointer16 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetZeroStack16 ||
             ip[1].op == ByteCodeOp::SetZeroStack32 ||
             ip[1].op == ByteCodeOp::SetZeroStack64) &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        break;

    case ByteCodeOp::SetZeroStack32:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
            ip[0].a.u32 + 4 == ip[1].a.u32 &&
            (ip[1].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[1].a.u32 -= 4;
            ip[1].b.u64 <<= 32;
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer64);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack8) &&
            ip[1].b.u32 >= ip[0].a.u32 &&
            ip[1].b.u32 < ip[0].a.u32 + 4 - 1 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack16) &&
            ip[1].b.u32 >= ip[0].a.u32 &&
            ip[1].b.u32 < ip[0].a.u32 + 4 - 2 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack32) &&
            ip[1].b.u32 == ip[0].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetZeroStack32 ||
             ip[1].op == ByteCodeOp::SetZeroStack64) &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        break;

    case ByteCodeOp::SetZeroStack64:
        if ((ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack8) &&
            ip[1].b.u32 >= ip[0].a.u32 &&
            ip[1].b.u32 < ip[0].a.u32 + 8 - 1 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack16) &&
            ip[1].b.u32 >= ip[0].a.u32 &&
            ip[1].b.u32 < ip[0].a.u32 + 8 - 2 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack32) &&
            ip[1].b.u32 >= ip[0].a.u32 &&
            ip[1].b.u32 < ip[0].a.u32 + 8 - 4 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack64) &&
            ip[1].b.u32 == ip[0].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        if (ip[1].op == ByteCodeOp::SetZeroStack64 &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        break;

    case ByteCodeOp::SetZeroStackX:
        if ((ip[1].op == ByteCodeOp::GetFromStack8) &&
            ip[1].b.u32 >= ip[0].a.u32 &&
            ip[1].b.u32 < ip[0].a.u32 + ip[0].b.u32 - 1 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack16) &&
            ip[1].b.u32 >= ip[0].a.u32 &&
            ip[1].b.u32 < ip[0].a.u32 + ip[0].b.u32 - 2 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack32) &&
            ip[1].b.u32 >= ip[0].a.u32 &&
            ip[1].b.u32 < ip[0].a.u32 + ip[0].b.u32 - 4 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        if ((ip[1].op == ByteCodeOp::GetFromStack64) &&
            ip[1].b.u32 >= ip[0].a.u32 &&
            ip[1].b.u32 < ip[0].a.u32 + ip[0].b.u32 - 8 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearRA);
            break;
        }

        break;

    case ByteCodeOp::CopyRBtoRA8:
        if ((ip[1].op == ByteCodeOp::ClearMaskU32 || ip[1].op == ByteCodeOp::ClearMaskU64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::CopyRBtoRA16:
        if ((ip[1].op == ByteCodeOp::ClearMaskU32) &&
            ip[1].b.u32 >= 0xFFFF &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU64) &&
            ip[1].b.u64 >= 0xFFFF &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::CopyRBtoRA32:
        if ((ip[1].op == ByteCodeOp::ClearMaskU64) &&
            ip[1].b.u64 == 0xFFFFFFFF &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip + 1);
            break;
        }
        break;

        // DeRef, by convention, clear the remaining bits. So no need for a cast just after
    case ByteCodeOp::DeRef8:
        if ((ip[1].op == ByteCodeOp::ClearMaskU32 || ip[1].op == ByteCodeOp::ClearMaskU64) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::DeRef16:
        if ((ip[1].op == ByteCodeOp::ClearMaskU32) &&
            (ip[1].b.u32 >= 0xFFFF) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::ClearMaskU64) &&
            (ip[1].b.u64 >= 0xFFFF) &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::DeRef32:
        if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
            ip[1].b.u64 >= 0xFFFFFFFF &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip + 1);
            break;
        }
        break;

        // Copy64 followed by cast
    case ByteCodeOp::CopyRBtoRA64:
        if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
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
            break;
        }

        else if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
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
            break;
        }
        break;

    case ByteCodeOp::SetImmediate32:
        if (ip[0].b.u32 == 0)
        {
            SET_OP(ip, ByteCodeOp::ClearRA);
            break;
        }

        break;

    case ByteCodeOp::SetImmediate64:
        if (ip[0].b.u64 == 0)
        {
            SET_OP(ip, ByteCodeOp::ClearRA);
            break;
        }

        break;

        // Indirect assign to register. Make it direct
    case ByteCodeOp::ClearRA:
        if (ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
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
            break;
        }
        break;

    case ByteCodeOp::SetAtStackPointer8:
        if (ip[0].flags & BCI_IMM_B && ip[0].b.u8 == 0)
        {
            SET_OP(ip, ByteCodeOp::SetZeroStack8);
            break;
        }

        if (ip[1].op == ByteCodeOp::CopyStack8 &&
            ip[0].flags & BCI_IMM_B &&
            ip[0].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer8);
            ip[1].b.u64 = ip[0].b.u8;
            ip[1].flags |= BCI_IMM_B;
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack8 &&
            ip[0].a.u32 == ip[1].b.u32 &&
            ip[0].flags & BCI_IMM_B &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
            ip[1].b.u64 = ip[0].b.u8;
            ip[1].flags &= ~BCI_IMM_A;
            ip[1].flags |= BCI_IMM_B;
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack8 &&
            !(ip->flags & BCI_IMM_B) &&
            ip->b.u32 == ip[1].a.u32 &&
            ip->a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearMaskU64);
            ip[1].b.u64 = 0xFF;
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetAtStackPointer8 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer16 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
            (ip[1].flags & BCI_IMM_B) &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetZeroStack8 ||
             ip[1].op == ByteCodeOp::SetZeroStack16 ||
             ip[1].op == ByteCodeOp::SetZeroStack32 ||
             ip[1].op == ByteCodeOp::SetZeroStack64) &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack8 &&
            ip->a.u32 == ip[1].b.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA8);
            ip[1].b.u32 = ip[0].b.u32;
            break;
        }

        break;

    case ByteCodeOp::SetAtStackPointer16:
        if (ip[0].flags & BCI_IMM_B && ip[0].b.u16 == 0)
        {
            SET_OP(ip, ByteCodeOp::SetZeroStack16);
            break;
        }

        if (ip[1].op == ByteCodeOp::CopyStack16 &&
            ip[0].flags & BCI_IMM_B &&
            ip[0].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer16);
            ip[1].b.u64 = ip[0].b.u16;
            ip[1].flags |= BCI_IMM_B;
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack16 &&
            ip[0].a.u32 == ip[1].b.u32 &&
            ip[0].flags & BCI_IMM_B &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
            ip[1].b.u64 = ip[0].b.u16;
            ip[1].flags &= ~BCI_IMM_A;
            ip[1].flags |= BCI_IMM_B;
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack16 &&
            !(ip->flags & BCI_IMM_B) &&
            ip->b.u32 == ip[1].a.u32 &&
            ip->a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearMaskU64);
            ip[1].b.u64 = 0xFFFF;
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetAtStackPointer16 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
            (ip[1].flags & BCI_IMM_B) &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetZeroStack16 ||
             ip[1].op == ByteCodeOp::SetZeroStack32 ||
             ip[1].op == ByteCodeOp::SetZeroStack64) &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack16 &&
            ip->a.u32 == ip[1].b.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA16);
            ip[1].b.u32 = ip[0].b.u32;
            break;
        }
        break;

    case ByteCodeOp::SetAtStackPointer32:
        if (ip[0].flags & BCI_IMM_B && ip[0].b.u32 == 0)
        {
            SET_OP(ip, ByteCodeOp::SetZeroStack32);
            break;
        }

        if (ip[1].op == ByteCodeOp::CopyStack32 &&
            ip[0].flags & BCI_IMM_B &&
            ip[0].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer32);
            ip[1].b.u64 = ip[0].b.u32;
            ip[1].flags |= BCI_IMM_B;
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack32 &&
            ip[0].a.u32 == ip[1].b.u32 &&
            ip[0].flags & BCI_IMM_B &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
            ip[1].b.u64 = ip[0].b.u32;
            ip[1].flags &= ~BCI_IMM_A;
            ip[1].flags |= BCI_IMM_B;
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack32 &&
            !(ip->flags & BCI_IMM_B) &&
            ip->b.u32 == ip[1].a.u32 &&
            ip->a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::ClearMaskU64);
            ip[1].b.u64 = 0xFFFFFFFF;
            break;
        }

        if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
            (ip->flags & BCI_IMM_B) &&
            (ip[1].flags & BCI_IMM_B) &&
            (ip->a.u32 == ip[1].a.u32 - 4) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer64);
            ip[0].b.u64 |= ((uint64_t) ip[1].b.u32) << 32;
            setNop(context, ip + 1);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
             ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
            (ip[1].flags & BCI_IMM_B) &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if ((ip[1].op == ByteCodeOp::SetZeroStack32 ||
             ip[1].op == ByteCodeOp::SetZeroStack64) &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack32 &&
            ip->a.u32 == ip[1].b.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA32);
            ip[1].b.u32 = ip[0].b.u32;
            break;
        }

        break;

    case ByteCodeOp::SetAtStackPointer64:
        if (ip[0].flags & BCI_IMM_B && ip[0].b.u64 == 0)
        {
            SET_OP(ip, ByteCodeOp::SetZeroStack64);
            break;
        }

        if (ip[1].op == ByteCodeOp::CopyStack64 &&
            ip[0].flags & BCI_IMM_B &&
            ip[0].a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer64);
            ip[1].b.u64 = ip[0].b.u64;
            ip[1].flags |= BCI_IMM_B;
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack32 &&
            ip[0].a.u32 == ip[1].b.u32 &&
            ip[0].flags & BCI_IMM_B &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
            ip[1].b.u64 = ip[0].b.u32;
            ip[1].flags |= BCI_IMM_B;
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack32 &&
            ip[0].a.u32 + 4 == ip[1].b.u32 &&
            ip[0].flags & BCI_IMM_B &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
            ip[1].b.u64 = ip[0].b.u64 >> 32;
            ip[1].flags |= BCI_IMM_B;
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack64 &&
            ip[0].a.u32 == ip[1].b.u32 &&
            ip[0].flags & BCI_IMM_B &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::SetImmediate64);
            ip[1].b.u64 = ip[0].b.u64;
            ip[1].flags |= BCI_IMM_B;
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack64 &&
            !(ip->flags & BCI_IMM_B) &&
            ip->b.u32 == ip[1].a.u32 &&
            ip->a.u32 == ip[1].b.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip + 1);
            break;
        }

        if (ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
            (ip[1].flags & BCI_IMM_B) &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if (ip[1].op == ByteCodeOp::SetZeroStack64 &&
            (ip->a.u32 == ip[1].a.u32) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            break;
        }

        if (ip[1].op == ByteCodeOp::GetFromStack64 &&
            ip->a.u32 == ip[1].b.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA64);
            ip[1].b.u32 = ip[0].b.u32;
            break;
        }

        break;
    }

    // Init stack just before a return
    if (ip[1].op == ByteCodeOp::Ret)
    {
        switch (ip[0].op)
        {
        case ByteCodeOp::SetAtStackPointer8:
        case ByteCodeOp::SetAtStackPointer16:
        case ByteCodeOp::SetAtStackPointer32:
        case ByteCodeOp::SetAtStackPointer64:
        case ByteCodeOp::SetAtStackPointer8x2:
        case ByteCodeOp::SetAtStackPointer16x2:
        case ByteCodeOp::SetAtStackPointer32x2:
        case ByteCodeOp::SetAtStackPointer64x2:
        case ByteCodeOp::SetZeroStack8:
        case ByteCodeOp::SetZeroStack16:
        case ByteCodeOp::SetZeroStack32:
        case ByteCodeOp::SetZeroStack64:
        case ByteCodeOp::SetZeroStackX:
            setNop(context, ip);
            break;
        }
    }
}

void ByteCodeOptimizer::reduceIncPtr(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip[0].op)
    {
    case ByteCodeOp::Mul64byVB64:
        if (ip[1].op == ByteCodeOp::IncPointer64 &&
            !(ip[1].flags & BCI_IMM_B) &&
            ip[1].b.u32 == ip[0].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::IncMulPointer64);
            ip[1].d.u64 = ip->b.u64;
            setNop(context, ip);
            break;
        }
        break;

    case ByteCodeOp::IncPointer64:
        if (ip[0].flags & BCI_IMM_B &&
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
                break;
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
                break;
            }

            // Make DeRef with an offset
            if (ip[1].op == ByteCodeOp::DeRef8 &&
                (ip[0].b.s64 + ip[1].c.s64 >= 0) &&
                (ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
                ip[0].c.u32 == ip[1].b.u32)
            {
                if (ip[1].a.u32 == ip[1].b.u32)
                {
                    ip[1].b.u32 = ip[0].a.u32;
                    ip[1].c.s64 += ip[0].b.s64;
                    setNop(context, ip);
                }
                else
                {
                    ip[1].b.u32 = ip[0].a.u32;
                    ip[1].c.s64 += ip[0].b.s64;
                    swap(ip[0], ip[1]);
                }
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef16 &&
                (ip[0].b.s64 + ip[1].c.s64 >= 0) &&
                (ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
                ip[0].c.u32 == ip[1].b.u32)
            {
                if (ip[1].a.u32 == ip[1].b.u32)
                {
                    ip[1].b.u32 = ip[0].a.u32;
                    ip[1].c.s64 += ip[0].b.s64;
                    setNop(context, ip);
                }
                else
                {
                    ip[1].b.u32 = ip[0].a.u32;
                    ip[1].c.s64 += ip[0].b.s64;
                    swap(ip[0], ip[1]);
                }
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef32 &&
                (ip[0].b.s64 + ip[1].c.s64 >= 0) &&
                (ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
                ip[0].c.u32 == ip[1].b.u32)
            {
                if (ip[1].a.u32 == ip[1].b.u32)
                {
                    ip[1].b.u32 = ip[0].a.u32;
                    ip[1].c.s64 += ip[0].b.s64;
                    setNop(context, ip);
                }
                else
                {
                    ip[1].b.u32 = ip[0].a.u32;
                    ip[1].c.s64 += ip[0].b.s64;
                    swap(ip[0], ip[1]);
                }
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef64 &&
                (ip[0].b.s64 + ip[1].c.s64 >= 0) &&
                (ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
                ip[0].c.u32 == ip[1].b.u32)
            {
                if (ip[1].a.u32 == ip[1].b.u32)
                {
                    ip[1].b.u32 = ip[0].a.u32;
                    ip[1].c.s64 += ip[0].b.s64;
                    setNop(context, ip);
                }
                else
                {
                    ip[1].b.u32 = ip[0].a.u32;
                    ip[1].c.s64 += ip[0].b.s64;
                    swap(ip[0], ip[1]);
                }
                break;
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
                break;
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
                break;
            }

            // Inc Pointer follower by SetAtPointer.
            // Encode the offset in SetAtPointer, but do not remove the IncPointer, as the
            // register can be used later. So we just swap the 2 instructions, and the
            // optimizer will remove the inc pointer if no more necessary
            if ((ip[1].op == ByteCodeOp::SetAtPointer8 ||
                 ip[1].op == ByteCodeOp::SetAtPointer16 ||
                 ip[1].op == ByteCodeOp::SetAtPointer32 ||
                 ip[1].op == ByteCodeOp::SetAtPointer64) &&
                ip[0].a.u32 == ip[0].c.u32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                (ip[0].b.s64 + ip[1].c.s64 >= 0) &&
                (ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF) &&
                !(ip[0].flags & BCI_START_STMT))

            {
                ip[1].c.s64 += ip[0].b.s64;
                swap(ip[0], ip[1]);
                context->passHasDoneSomething = true;
                break;
            }
        }
        break;

    case ByteCodeOp::MakeStackPointer:
        // Occurs when setting a string.
        if (ip[2].op == ByteCodeOp::IncPointer64 &&
            ip[3].op == ByteCodeOp::SetAtPointer64 &&
            ip[0].a.u32 == ip[2].a.u32 &&
            ip[2].c.u32 == ip[3].a.u32 &&
            ip[2].flags & BCI_IMM_B &&
            ip[2].b.s64 > 0)
        {
            SET_OP(ip + 3, ByteCodeOp::SetAtStackPointer64);
            ip[3].a.u32 = ip[0].b.u32 + ip[2].b.u32;
            break;
        }

        // MakeStackPointer Reg, ImmB
        // IncPointer64     Reg, Reg, ImmB
        // We can change the offset of the MakeStackPointer and remove the IncPointer
        if (ip[1].op == ByteCodeOp::IncPointer64 &&
            ip[1].c.u32 == ip[0].a.u32 &&
            ip[1].c.u32 == ip[1].a.u32 &&
            ip[1].flags & BCI_IMM_B &&
            ip[1].b.s64 > 0)
        {
            ip[0].b.u32 += ip[1].b.u32;
            setNop(context, ip + 1);
            break;
        }

        break;

    case ByteCodeOp::CopyRRtoRC:
        // Store offset of CopyRRtoRC directly in the instruction
        if (ip[1].op == ByteCodeOp::IncPointer64 &&
            ip[1].a.u32 == ip[1].c.u32 &&
            ip[0].a.u32 == ip[1].a.u32 &&
            (ip[1].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].b.u64 += ip[1].b.u64;
            setNop(context, ip + 1);
            break;
        }
        break;
    }
}

void ByteCodeOptimizer::reduceCast(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::ClearMaskU64:
        if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
            ip[1].a.u32 == ip[0].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].b.u64 = min(ip[0].b.u64, ip[1].b.u64);
            setNop(context, ip + 1);
            break;
        }

        break;

        // GetFromStack8/16/32 clear the other bits by convention, so no need to
        // have a ClearMask after
    case ByteCodeOp::GetFromStack8:
        if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
            break;
        }

        if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::GetFromStack16:
        if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
            break;
        }

        if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::GetFromStack32:
        if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
            break;
        }
        break;
    }
}

void ByteCodeOptimizer::reduceNoOp(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::Add64byVB64:
        if (ip->b.u64 == 0)
        {
            setNop(context, ip);
            break;
        }
        break;

    case ByteCodeOp::Mul64byVB64:
        if (ip->b.u64 == 1)
        {
            setNop(context, ip);
            break;
        }
        break;

    case ByteCodeOp::DebugNop:
        if (ip[1].op == ByteCodeOp::DebugNop)
        {
            setNop(context, ip + 1);
            break;
        }
        break;

        // Useless pop/push
    case ByteCodeOp::InternalPushErr:
        if (ip[1].op == ByteCodeOp::InternalPopErr)
        {
            setNop(context, ip);
            setNop(context, ip + 1);
            break;
        }
        break;

        // Useless multi return
    case ByteCodeOp::Ret:
        if (ip[1].op == ByteCodeOp::Ret)
        {
            setNop(context, ip);
            break;
        }
        break;

        // Duplicated safety
    case ByteCodeOp::JumpIfNotZero64:
        if (ip[1].op == ByteCodeOp::InternalPanic &&
            ip[2].op == ByteCodeOp::JumpIfNotZero64 &&
            ip[3].op == ByteCodeOp::InternalPanic &&
            ip[0].a.u32 == ip[2].a.u32 &&
            ip[0].b.s32 == 1 &&
            ip[2].b.s32 == 1)
        {
            setNop(context, ip);
            setNop(context, ip + 1);
            break;
        }
        break;
    }

    // Remove operators which do nothing
    if (ip->flags & BCI_IMM_B)
    {
        switch (ip->op)
        {
        case ByteCodeOp::IncPointer64:
            if (ip->b.u64 == 0)
            {
                if (ip->a.u32 == ip->c.u32)
                    setNop(context, ip);
                else
                {
                    SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                    ip->b.u64 = ip->a.u64;
                    ip->a.u64 = ip->c.u64;
                    ip->flags &= ~BCI_IMM_B;
                }
            }
            break;

        case ByteCodeOp::AffectOpDivEqS8:
        case ByteCodeOp::AffectOpMulEqS8:
            if (ip->b.u8 == 1)
                setNop(context, ip);
            break;
        case ByteCodeOp::AffectOpDivEqS16:
        case ByteCodeOp::AffectOpMulEqS16:
            if (ip->b.u16 == 1)
                setNop(context, ip);
            break;
        case ByteCodeOp::AffectOpDivEqS32:
        case ByteCodeOp::AffectOpMulEqS32:
            if (ip->b.u32 == 1)
                setNop(context, ip);
            break;
        case ByteCodeOp::AffectOpDivEqS64:
        case ByteCodeOp::AffectOpMulEqS64:
            if (ip->b.u64 == 1)
                setNop(context, ip);
            break;
        case ByteCodeOp::AffectOpPlusEqS8:
        case ByteCodeOp::AffectOpPlusEqS8_Safe:
        case ByteCodeOp::AffectOpPlusEqU8:
        case ByteCodeOp::AffectOpPlusEqU8_Safe:
        case ByteCodeOp::AffectOpMinusEqS8:
        case ByteCodeOp::AffectOpMinusEqS8_Safe:
        case ByteCodeOp::AffectOpMinusEqU8:
        case ByteCodeOp::AffectOpMinusEqU8_Safe:
            if (ip->b.u8 == 0)
                setNop(context, ip);
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
        case ByteCodeOp::AffectOpPlusEqS16_Safe:
        case ByteCodeOp::AffectOpPlusEqU16:
        case ByteCodeOp::AffectOpPlusEqU16_Safe:
        case ByteCodeOp::AffectOpMinusEqS16:
        case ByteCodeOp::AffectOpMinusEqS16_Safe:
        case ByteCodeOp::AffectOpMinusEqU16:
        case ByteCodeOp::AffectOpMinusEqU16_Safe:
            if (ip->b.u16 == 0)
                setNop(context, ip);
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
        case ByteCodeOp::AffectOpPlusEqS32_Safe:
        case ByteCodeOp::AffectOpPlusEqU32:
        case ByteCodeOp::AffectOpPlusEqU32_Safe:
        case ByteCodeOp::AffectOpMinusEqS32:
        case ByteCodeOp::AffectOpMinusEqS32_Safe:
        case ByteCodeOp::AffectOpMinusEqU32:
        case ByteCodeOp::AffectOpMinusEqU32_Safe:
            if (ip->b.u32 == 0)
                setNop(context, ip);
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
        case ByteCodeOp::AffectOpPlusEqS64_Safe:
        case ByteCodeOp::AffectOpPlusEqU64:
        case ByteCodeOp::AffectOpPlusEqU64_Safe:
        case ByteCodeOp::AffectOpMinusEqS64:
        case ByteCodeOp::AffectOpMinusEqS64_Safe:
        case ByteCodeOp::AffectOpMinusEqU64:
        case ByteCodeOp::AffectOpMinusEqU64_Safe:
            if (ip->b.u64 == 0)
                setNop(context, ip);
            break;

        case ByteCodeOp::AffectOpShiftLeftEqU8:
        case ByteCodeOp::AffectOpShiftLeftEqU16:
        case ByteCodeOp::AffectOpShiftLeftEqU32:
        case ByteCodeOp::AffectOpShiftLeftEqU64:
        case ByteCodeOp::AffectOpShiftLeftEqS8:
        case ByteCodeOp::AffectOpShiftLeftEqS16:
        case ByteCodeOp::AffectOpShiftLeftEqS32:
        case ByteCodeOp::AffectOpShiftLeftEqS64:
        case ByteCodeOp::AffectOpShiftRightEqU8:
        case ByteCodeOp::AffectOpShiftRightEqU16:
        case ByteCodeOp::AffectOpShiftRightEqU32:
        case ByteCodeOp::AffectOpShiftRightEqU64:
        case ByteCodeOp::AffectOpShiftRightEqS8:
        case ByteCodeOp::AffectOpShiftRightEqS16:
        case ByteCodeOp::AffectOpShiftRightEqS32:
        case ByteCodeOp::AffectOpShiftRightEqS64:
            if (ip->b.u32 == 0)
                setNop(context, ip);
            break;
        }
    }

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
        case ByteCodeOp::BinOpPlusS32_Safe:
        case ByteCodeOp::BinOpMinusS32_Safe:
        case ByteCodeOp::BinOpPlusU32_Safe:
        case ByteCodeOp::BinOpMinusU32_Safe:
            if (ip->b.u32 == 0)
                setNop(context, ip);
            break;

        case ByteCodeOp::BinOpPlusS64:
        case ByteCodeOp::BinOpPlusU64:
        case ByteCodeOp::BinOpMinusS64:
        case ByteCodeOp::BinOpMinusU64:
        case ByteCodeOp::BinOpPlusS64_Safe:
        case ByteCodeOp::BinOpPlusU64_Safe:
        case ByteCodeOp::BinOpMinusS64_Safe:
        case ByteCodeOp::BinOpMinusU64_Safe:
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
        case ByteCodeOp::BinOpMulU32_Safe:
        case ByteCodeOp::BinOpMulU64_Safe:
        case ByteCodeOp::BinOpMulS32_Safe:
        case ByteCodeOp::BinOpMulS64_Safe:
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
    if (!(ip[1].flags & BCI_START_STMT))
    {
        // Reduce SetZeroStack
        uint32_t offset0, offset1;
        auto     size0 = ByteCode::getSetZeroStackSize(ip, offset0);
        if (size0)
        {
            auto size1 = ByteCode::getSetZeroStackSize(ip + 1, offset1);
            if (size1 && offset0 + size0 == offset1)
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
        size0 = ByteCode::getSetZeroAtPointerSize(ip, offset0);
        if (size0)
        {
            auto size1 = ByteCode::getSetZeroAtPointerSize(ip + 1, offset1);
            if (size1 && offset0 + size0 == offset1)
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
    }

    switch (ip->op)
    {
    case ByteCodeOp::SetAtPointer8:
        if (ip[1].op == ByteCodeOp::SetAtPointer8 &&
            ip->c.u32 + sizeof(uint8_t) == ip[1].c.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            (ip->flags & BCI_IMM_B) &&
            (ip[1].flags & BCI_IMM_B))
        {
            SET_OP(ip, ByteCodeOp::SetAtPointer16);
            ip->b.u64 |= (uint64_t) ip[1].b.u32 << 8;
            setNop(context, ip + 1);
            break;
        }

        break;

    case ByteCodeOp::SetAtPointer16:
        if (ip[1].op == ByteCodeOp::SetAtPointer16 &&
            ip->c.u32 + sizeof(uint16_t) == ip[1].c.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            (ip->flags & BCI_IMM_B) &&
            (ip[1].flags & BCI_IMM_B))
        {
            SET_OP(ip, ByteCodeOp::SetAtPointer32);
            ip->b.u64 |= (uint64_t) ip[1].b.u32 << 16;
            setNop(context, ip + 1);
            break;
        }

        break;

    case ByteCodeOp::SetAtPointer32:
        if (ip[1].op == ByteCodeOp::SetAtPointer32 &&
            ip->c.u32 + sizeof(uint32_t) == ip[1].c.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            (ip->flags & BCI_IMM_B) &&
            (ip[1].flags & BCI_IMM_B))
        {
            SET_OP(ip, ByteCodeOp::SetAtPointer64);
            ip->b.u64 |= (uint64_t) ip[1].b.u32 << 32;
            setNop(context, ip + 1);
            break;
        }

        break;

    case ByteCodeOp::SetAtPointer64:
        if ((ip->flags & BCI_IMM_B) && ip->b.u64 == 0)
        {
            SET_OP(ip, ByteCodeOp::SetZeroAtPointer64);
            ip->b.u64 = ip->c.u32;
            break;
        }

        break;

    case ByteCodeOp::SetAtStackPointer8:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
            ip->a.u32 + sizeof(uint8_t) == ip[1].a.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            (ip->flags & BCI_IMM_B) &&
            (ip[1].flags & BCI_IMM_B))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer16);
            ip->b.u16 |= ip[1].b.u8 << 8;
            setNop(context, ip + 1);
            break;
        }

        break;

    case ByteCodeOp::SetAtStackPointer16:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
            ip->a.u32 + sizeof(uint16_t) == ip[1].a.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            (ip->flags & BCI_IMM_B) &&
            (ip[1].flags & BCI_IMM_B))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer32);
            ip->b.u32 |= ip[1].b.u16 << 16;
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::SetAtStackPointer32:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
            ip->a.u32 + sizeof(uint32_t) == ip[1].a.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            (ip->flags & BCI_IMM_B) &&
            (ip[1].flags & BCI_IMM_B))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer64);
            ip->b.u64 |= (uint64_t) ip[1].b.u32 << 32;
            setNop(context, ip + 1);
            break;
        }

        break;
    }
}

void ByteCodeOptimizer::reduceX2(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::MakeStackPointer:
        if (ip[1].op == ByteCodeOp::MakeStackPointer &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].c.u32 = ip[1].a.u32;
            ip[0].d.u32 = ip[1].b.u32;
            SET_OP(ip, ByteCodeOp::MakeStackPointer2);
            setNop(context, ip + 1);
            break;
        }
        break;

    case ByteCodeOp::ClearRA:
        if (ip[1].op == ByteCodeOp::ClearRA &&
            ip[2].op == ByteCodeOp::ClearRA &&
            ip[3].op == ByteCodeOp::ClearRA &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT) &&
            !(ip[3].flags & BCI_START_STMT))
        {
            ip[0].b.u32 = ip[1].a.u32;
            ip[0].c.u32 = ip[2].a.u32;
            ip[0].d.u32 = ip[3].a.u32;
            SET_OP(ip, ByteCodeOp::ClearRA4);
            setNop(context, ip + 1);
            setNop(context, ip + 2);
            setNop(context, ip + 3);
            break;
        }

        if (ip[1].op == ByteCodeOp::ClearRA &&
            ip[2].op == ByteCodeOp::ClearRA &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT))
        {
            ip[0].b.u32 = ip[1].a.u32;
            ip[0].c.u32 = ip[2].a.u32;
            SET_OP(ip, ByteCodeOp::ClearRA3);
            setNop(context, ip + 1);
            setNop(context, ip + 2);
            break;
        }

        if (ip[1].op == ByteCodeOp::ClearRA &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].b.u32 = ip[1].a.u32;
            SET_OP(ip, ByteCodeOp::ClearRA2);
            setNop(context, ip + 1);
            break;
        }

        break;

        // SetAtStackPointer x2
    case ByteCodeOp::SetAtStackPointer8:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer8x2);
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

    case ByteCodeOp::SetAtStackPointer32:
        if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer32x2);
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
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip, ByteCodeOp::SetAtStackPointer64x2);
            ip[0].c.u64 = ip[1].a.u64;
            ip[0].d.u64 = ip[1].b.u64;
            if (ip[1].flags & BCI_IMM_B)
                ip[0].flags |= BCI_IMM_D;
            setNop(context, ip + 1);
            break;
        }
        break;

        // GetFromStack64x2
        /*case ByteCodeOp::GetFromStack64:
            if (ip[1].op == ByteCodeOp::GetFromStack64 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack64x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;*/
    }
}

void ByteCodeOptimizer::reduceCmpJump(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    // Instruction followed by jump followed by the exact same instruction, and no stmt start
    // Remove the clone
    if (ByteCode::isJump(ip + 1) &&
        ip[1].op != ByteCodeOp::Jump &&
        !(ip[1].flags & BCI_START_STMT) &&
        !(ip[2].flags & BCI_START_STMT) &&
        ip[0].op == ip[2].op &&
        ip[0].flags == ip[2].flags &&
        ip[0].a.u64 == ip[2].a.u64 &&
        ip[0].b.u64 == ip[2].b.u64 &&
        ip[0].c.u64 == ip[2].c.u64 &&
        ip[0].d.u64 == ip[2].d.u64)
    {
        setNop(context, ip + 2);
    }

    switch (ip->op)
    {
    case ByteCodeOp::ClearMaskU32:
        if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
            ip[2].op == ByteCodeOp::JumpIfNotEqual32 &&
            ip[0].b.u32 == 0xFF &&
            ip[1].b.u32 == 0xFF &&
            ip[2].a.u32 == ip[0].a.u32 &&
            ip[2].c.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            setNop(context, ip + 1);
            SET_OP(ip + 2, ByteCodeOp::JumpIfNotEqual8);
            break;
        }

        if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
            ip[2].op == ByteCodeOp::JumpIfNotEqual32 &&
            ip[0].b.u32 == 0xFFFF &&
            ip[1].b.u32 == 0xFFFF &&
            ip[2].a.u32 == ip[0].a.u32 &&
            ip[2].c.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            setNop(context, ip + 1);
            SET_OP(ip + 2, ByteCodeOp::JumpIfNotEqual16);
            break;
        }

        if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
            ip[2].op == ByteCodeOp::JumpIfEqual32 &&
            ip[0].b.u32 == 0xFF &&
            ip[1].b.u32 == 0xFF &&
            ip[2].a.u32 == ip[0].a.u32 &&
            ip[2].c.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            setNop(context, ip + 1);
            SET_OP(ip + 2, ByteCodeOp::JumpIfEqual8);
            break;
        }

        if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
            ip[2].op == ByteCodeOp::JumpIfEqual32 &&
            ip[0].b.u32 == 0xFFFF &&
            ip[1].b.u32 == 0xFFFF &&
            ip[2].a.u32 == ip[0].a.u32 &&
            ip[2].c.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT))
        {
            setNop(context, ip);
            setNop(context, ip + 1);
            SET_OP(ip + 2, ByteCodeOp::JumpIfEqual16);
            break;
        }
        break;

    case ByteCodeOp::IncrementRA64:
        if (ip[1].op == ByteCodeOp::JumpIfEqual64 &&
            !(ip[0].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT) &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip);
            SET_OP(ip + 1, ByteCodeOp::IncJumpIfEqual64);
            break;
        }
        break;

    // NegBool followed by jump bool
    case ByteCodeOp::NegBool:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT) &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfTrue);
            ip[1].a.u32 = ip->b.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT) &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfFalse);
            ip[1].a.u32 = ip->b.u32;
            break;
        }
        break;

    // Cast bool followed by a jump. Change the jump
    case ByteCodeOp::CastBool8:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip->a.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero8);
            ip[1].a.u32 = ip->b.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip->a.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
            ip[1].a.u32 = ip->b.u32;
            break;
        }

        break;

    case ByteCodeOp::CastBool16:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip->a.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero16);
            ip[1].a.u32 = ip->b.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip->a.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
            ip[1].a.u32 = ip->b.u32;
            break;
        }

        break;

    case ByteCodeOp::CastBool32:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip->a.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
            ip[1].a.u32 = ip->b.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip->a.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
            ip[1].a.u32 = ip->b.u32;
            break;
        }

        break;

    case ByteCodeOp::CastBool64:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip->a.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero64);
            ip[1].a.u32 = ip->b.u32;
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip->a.u32 == ip[1].a.u32 &&
            !(ip[0].flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_IMM_A) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
            ip[1].a.u32 = ip->b.u32;
            break;
        }

        break;

        // Mix compare and jump
    case ByteCodeOp::CompareOpEqual8:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpEqual16:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpEqual32:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpEqual64:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpEqualF32:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            ip->a.u32 != ip->c.u32 &&
            (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqualF32);
            ip[1].a.u64 = ip->a.u64;
            ip[1].c.u64 = ip->b.u64;
            ip[1].flags |= ip->flags & BCI_IMM_A;
            ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            ip->a.u32 != ip->c.u32 &&
            (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfEqualF32);
            ip[1].a.u64 = ip->a.u64;
            ip[1].c.u64 = ip->b.u64;
            ip[1].flags |= ip->flags & BCI_IMM_A;
            ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
            break;
        }

        break;

    case ByteCodeOp::CompareOpEqualF64:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            ip->a.u32 != ip->c.u32 &&
            (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqualF64);
            ip[1].a.u64 = ip->a.u64;
            ip[1].c.u64 = ip->b.u64;
            ip[1].flags |= ip->flags & BCI_IMM_A;
            ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            ip->a.u32 != ip->c.u32 &&
            (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfEqualF64);
            ip[1].a.u64 = ip->a.u64;
            ip[1].c.u64 = ip->b.u64;
            ip[1].flags |= ip->flags & BCI_IMM_A;
            ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
            break;
        }

        break;

        // Mix compare and jump
    case ByteCodeOp::CompareOpNotEqual8:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpNotEqual16:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpNotEqual32:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpNotEqual64:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpNotEqualF32:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            ip->a.u32 != ip->c.u32 &&
            (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfEqualF32);
            ip[1].a.u64 = ip->a.u64;
            ip[1].c.u64 = ip->b.u64;
            ip[1].flags |= ip->flags & BCI_IMM_A;
            ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            ip->a.u32 != ip->c.u32 &&
            (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqualF32);
            ip[1].a.u64 = ip->a.u64;
            ip[1].c.u64 = ip->b.u64;
            ip[1].flags |= ip->flags & BCI_IMM_A;
            ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
            break;
        }

        break;

    case ByteCodeOp::CompareOpNotEqualF64:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
            ip[1].a.u32 == ip->c.u32 &&
            ip->a.u32 != ip->c.u32 &&
            (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfEqualF64);
            ip[1].a.u64 = ip->a.u64;
            ip[1].c.u64 = ip->b.u64;
            ip[1].flags |= ip->flags & BCI_IMM_A;
            ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
            ip[1].a.u32 == ip->c.u32 &&
            ip->a.u32 != ip->c.u32 &&
            (ip->b.u32 != ip->c.u32 || ip->flags & BCI_IMM_B) &&
            !(ip[1].flags & BCI_START_STMT))
        {
            SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqualF64);
            ip[1].a.u64 = ip->a.u64;
            ip[1].c.u64 = ip->b.u64;
            ip[1].flags |= ip->flags & BCI_IMM_A;
            ip[1].flags |= ip->flags & BCI_IMM_B ? BCI_IMM_C : 0;
            break;
        }

        break;

        // Mix compare and jump
    case ByteCodeOp::CompareOpLowerU32:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        // Mix compare and jump
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpLowerU64:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpLowerS32:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpLowerS64:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpLowerF32:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpLowerF64:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

        // Mix compare and jump
    case ByteCodeOp::CompareOpGreaterEqU32:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        // Mix compare and jump
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpGreaterEqU64:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpGreaterEqS32:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpGreaterEqS64:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpGreaterEqF32:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpGreaterEqF64:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

        // Mix compare and jump
    case ByteCodeOp::CompareOpGreaterU32:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpGreaterU64:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpGreaterS32:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpGreaterS64:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpGreaterF32:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpGreaterF64:
        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        break;

        // Mix compare and jump
    case ByteCodeOp::CompareOpLowerEqU32:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpLowerEqU64:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpLowerEqS32:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpLowerEqS64:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpLowerEqF32:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;

    case ByteCodeOp::CompareOpLowerEqF64:
        if (ip[1].op == ByteCodeOp::JumpIfFalse &&
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
            break;
        }

        if (ip[1].op == ByteCodeOp::JumpIfTrue &&
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
            break;
        }

        break;
    }

    // Compare to == 0
    if (ip->flags & BCI_IMM_B && ip->a.u32 != ip->c.u32)
    {
        switch (ip->op)
        {
        case ByteCodeOp::CompareOpEqual8:
            if (ip->b.u8 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero8);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u8 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u8 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfZero8 &&
                ip[1].a.u32 == ip->c.u32)
            {
                setNop(context, ip);
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            break;

        case ByteCodeOp::CompareOpEqual16:
            if (ip->b.u16 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero16);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u16 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u8 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfZero16 &&
                ip[1].a.u32 == ip->c.u32)
            {
                setNop(context, ip);
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            break;

        case ByteCodeOp::CompareOpEqual32:
            if (ip->b.u32 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u32 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u8 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfZero32 &&
                ip[1].a.u32 == ip->c.u32)
            {
                setNop(context, ip);
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            break;

        case ByteCodeOp::CompareOpEqual64:
            if (ip->b.u64 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero64);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u64 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u64 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfNotZero8 &&
                ip[1].a.u32 == ip->c.u32)
            {
                setNop(context, ip);
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
                ip[1].a.u32 = ip->a.u32;
            }

            break;

        // Compare to != 0
        case ByteCodeOp::CompareOpNotEqual8:
            if (ip->b.u8 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u8 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero8);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            break;

        case ByteCodeOp::CompareOpNotEqual16:
            if (ip->b.u16 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u16 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero16);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            break;

        case ByteCodeOp::CompareOpNotEqual32:
            if (ip->b.u32 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u32 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            break;

        case ByteCodeOp::CompareOpNotEqual64:
            if (ip->b.u64 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            if (ip->b.u64 == 0 &&
                ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero64);
                ip[1].a.u32 = ip->a.u32;
                break;
            }

            break;
        }
    }
}

void ByteCodeOptimizer::reduceLateStack(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::SetAtStackPointer8:
    case ByteCodeOp::SetAtStackPointer16:
    case ByteCodeOp::SetAtStackPointer32:
    case ByteCodeOp::SetAtStackPointer64:
    case ByteCodeOp::SetAtStackPointer8x2:
    case ByteCodeOp::SetAtStackPointer16x2:
    case ByteCodeOp::SetAtStackPointer32x2:
    case ByteCodeOp::SetAtStackPointer64x2:
    case ByteCodeOp::SetZeroStack8:
    case ByteCodeOp::SetZeroStack16:
    case ByteCodeOp::SetZeroStack32:
    case ByteCodeOp::SetZeroStack64:
    case ByteCodeOp::CopyStack8:
    case ByteCodeOp::CopyStack16:
    case ByteCodeOp::CopyStack32:
    case ByteCodeOp::CopyStack64:
        if (ip[1].op == ByteCodeOp::CopyRCtoRRRet || ip[1].op == ByteCodeOp::Ret)
        {
            setNop(context, ip);
            break;
        }
        break;
    }
}

void ByteCodeOptimizer::reduceStackOp(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::MakeStackPointer:
        if (ip[0].a.u32 == ip[1].a.u32)
        {
            switch (ip[1].op)
            {
            case ByteCodeOp::AffectOpPlusEqS8_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS8_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqS16_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS16_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqS32_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS32_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqS64_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS64_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU8_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU8_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU16_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU16_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU32_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU32_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU64_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU64_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqF32:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqF32_S);
                break;
            case ByteCodeOp::AffectOpPlusEqF64:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqF64_S);
                break;

            case ByteCodeOp::AffectOpMinusEqS8_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS8_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqS16_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS16_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqS32_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS32_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqS64_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS64_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU8_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU8_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU16_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU16_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU32_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU32_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU64_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU64_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqF32:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqF32_S);
                break;
            case ByteCodeOp::AffectOpMinusEqF64:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqF64_S);
                break;

            case ByteCodeOp::AffectOpMulEqS8_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS8_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqS16_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS16_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqS32_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS32_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqS64_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS64_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU8_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU8_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU16_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU16_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU32_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU32_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU64_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU64_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqF32:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqF32_S);
                break;
            case ByteCodeOp::AffectOpMulEqF64:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqF64_S);
                break;
            }
        }
        break;

    case ByteCodeOp::GetFromStack8:
        if (ip[0].a.u32 == ip[1].b.u32)
        {
            switch (ip[1].op)
            {
            case ByteCodeOp::AffectOpPlusEqS8_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS8_SSSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU8_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU8_SSSafe);
                break;

            case ByteCodeOp::AffectOpMinusEqS8_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS8_SSSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU8_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU8_SSSafe);
                break;

            case ByteCodeOp::AffectOpMulEqS8_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS8_SSSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU8_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU8_SSSafe);
                break;
            }
        }
        break;

    case ByteCodeOp::GetFromStack16:
        if (ip[0].a.u32 == ip[1].b.u32)
        {
            switch (ip[1].op)
            {
            case ByteCodeOp::AffectOpPlusEqS16_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS16_SSSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU16_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU16_SSSafe);
                break;

            case ByteCodeOp::AffectOpMinusEqS16_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS16_SSSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU16_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU16_SSSafe);
                break;

            case ByteCodeOp::AffectOpMulEqS16_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS16_SSSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU16_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU16_SSSafe);
                break;
            }
        }
        break;

    case ByteCodeOp::GetFromStack32:
        if (ip[0].a.u32 == ip[1].b.u32)
        {
            switch (ip[1].op)
            {
            case ByteCodeOp::AffectOpPlusEqS32_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS32_SSSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU32_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU32_SSSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqF32_S:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqF32_SS);
                break;

            case ByteCodeOp::AffectOpMinusEqS32_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS32_SSSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU32_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU32_SSSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqF32_S:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqF32_SS);
                break;

            case ByteCodeOp::AffectOpMulEqS32_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS32_SSSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU32_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU32_SSSafe);
                break;
            case ByteCodeOp::AffectOpMulEqF32_S:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqF32_SS);
                break;
            }
        }
        break;

    case ByteCodeOp::GetFromStack64:
        if (ip[0].a.u32 == ip[1].b.u32)
        {
            switch (ip[1].op)
            {
            case ByteCodeOp::AffectOpPlusEqS64_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS64_SSSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU64_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU64_SSSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqF64_S:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqF64_SS);
                break;

            case ByteCodeOp::AffectOpMinusEqS64_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS64_SSSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU64_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU64_SSSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqF64_S:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqF64_SS);
                break;

            case ByteCodeOp::AffectOpMulEqS64_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS64_SSSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU64_SSafe:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU64_SSSafe);
                break;
            case ByteCodeOp::AffectOpMulEqF64_S:
                ip[1].b.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqF64_SS);
                break;
            }
        }
        break;
    }
}

void ByteCodeOptimizer::reduceForceSafe(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->dynFlags & BCID_SAFETY_OF)
        return;

    switch (ip->op)
    {
    case ByteCodeOp::AffectOpPlusEqS8:
        SET_OP(ip, ByteCodeOp::AffectOpPlusEqS8_Safe);
        break;
    case ByteCodeOp::AffectOpPlusEqS16:
        SET_OP(ip, ByteCodeOp::AffectOpPlusEqS16_Safe);
        break;
    case ByteCodeOp::AffectOpPlusEqS32:
        SET_OP(ip, ByteCodeOp::AffectOpPlusEqS32_Safe);
        break;
    case ByteCodeOp::AffectOpPlusEqS64:
        SET_OP(ip, ByteCodeOp::AffectOpPlusEqS64_Safe);
        break;
    case ByteCodeOp::AffectOpPlusEqU8:
        SET_OP(ip, ByteCodeOp::AffectOpPlusEqU8_Safe);
        break;
    case ByteCodeOp::AffectOpPlusEqU16:
        SET_OP(ip, ByteCodeOp::AffectOpPlusEqU16_Safe);
        break;
    case ByteCodeOp::AffectOpPlusEqU32:
        SET_OP(ip, ByteCodeOp::AffectOpPlusEqU32_Safe);
        break;
    case ByteCodeOp::AffectOpPlusEqU64:
        SET_OP(ip, ByteCodeOp::AffectOpPlusEqU64_Safe);
        break;

    case ByteCodeOp::AffectOpMinusEqS8:
        SET_OP(ip, ByteCodeOp::AffectOpMinusEqS8_Safe);
        break;
    case ByteCodeOp::AffectOpMinusEqU8:
        SET_OP(ip, ByteCodeOp::AffectOpMinusEqU8_Safe);
        break;
    case ByteCodeOp::AffectOpMinusEqS16:
        SET_OP(ip, ByteCodeOp::AffectOpMinusEqS16_Safe);
        break;
    case ByteCodeOp::AffectOpMinusEqU16:
        SET_OP(ip, ByteCodeOp::AffectOpMinusEqU16_Safe);
        break;
    case ByteCodeOp::AffectOpMinusEqS32:
        SET_OP(ip, ByteCodeOp::AffectOpMinusEqS32_Safe);
        break;
    case ByteCodeOp::AffectOpMinusEqU32:
        SET_OP(ip, ByteCodeOp::AffectOpMinusEqU32_Safe);
        break;
    case ByteCodeOp::AffectOpMinusEqS64:
        SET_OP(ip, ByteCodeOp::AffectOpMinusEqS64_Safe);
        break;
    case ByteCodeOp::AffectOpMinusEqU64:
        SET_OP(ip, ByteCodeOp::AffectOpMinusEqU64_Safe);
        break;

    case ByteCodeOp::AffectOpMulEqS8:
        SET_OP(ip, ByteCodeOp::AffectOpMulEqS8_Safe);
        break;
    case ByteCodeOp::AffectOpMulEqU8:
        SET_OP(ip, ByteCodeOp::AffectOpMulEqU8_Safe);
        break;
    case ByteCodeOp::AffectOpMulEqS16:
        SET_OP(ip, ByteCodeOp::AffectOpMulEqS16_Safe);
        break;
    case ByteCodeOp::AffectOpMulEqU16:
        SET_OP(ip, ByteCodeOp::AffectOpMulEqU16_Safe);
        break;
    case ByteCodeOp::AffectOpMulEqS32:
        SET_OP(ip, ByteCodeOp::AffectOpMulEqS32_Safe);
        break;
    case ByteCodeOp::AffectOpMulEqU32:
        SET_OP(ip, ByteCodeOp::AffectOpMulEqU32_Safe);
        break;
    case ByteCodeOp::AffectOpMulEqS64:
        SET_OP(ip, ByteCodeOp::AffectOpMulEqS64_Safe);
        break;
    case ByteCodeOp::AffectOpMulEqU64:
        SET_OP(ip, ByteCodeOp::AffectOpMulEqU64_Safe);
        break;

    case ByteCodeOp::BinOpPlusS32:
        SET_OP(ip, ByteCodeOp::BinOpPlusS32_Safe);
        break;
    case ByteCodeOp::BinOpPlusU32:
        SET_OP(ip, ByteCodeOp::BinOpPlusU32_Safe);
        break;
    case ByteCodeOp::BinOpPlusS64:
        SET_OP(ip, ByteCodeOp::BinOpPlusS64_Safe);
        break;
    case ByteCodeOp::BinOpPlusU64:
        SET_OP(ip, ByteCodeOp::BinOpPlusU64_Safe);
        break;

    case ByteCodeOp::BinOpMinusS32:
        SET_OP(ip, ByteCodeOp::BinOpMinusS32_Safe);
        break;
    case ByteCodeOp::BinOpMinusU32:
        SET_OP(ip, ByteCodeOp::BinOpMinusU32_Safe);
        break;
    case ByteCodeOp::BinOpMinusS64:
        SET_OP(ip, ByteCodeOp::BinOpMinusS64_Safe);
        break;
    case ByteCodeOp::BinOpMinusU64:
        SET_OP(ip, ByteCodeOp::BinOpMinusU64_Safe);
        break;

    case ByteCodeOp::BinOpMulS32:
        SET_OP(ip, ByteCodeOp::BinOpMulS32_Safe);
        break;
    case ByteCodeOp::BinOpMulU32:
        SET_OP(ip, ByteCodeOp::BinOpMulU32_Safe);
        break;
    case ByteCodeOp::BinOpMulS64:
        SET_OP(ip, ByteCodeOp::BinOpMulS64_Safe);
        break;
    case ByteCodeOp::BinOpMulU64:
        SET_OP(ip, ByteCodeOp::BinOpMulU64_Safe);
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

bool ByteCodeOptimizer::optimizePassReduce(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        reduceErr(context, ip);
        reduceCallEmptyFct(context, ip);
        reduceMemcpy(context, ip);
        reduceFunc(context, ip);
        reduceStack(context, ip);
        reduceIncPtr(context, ip);
        reduceSetAt(context, ip);
        reduceCast(context, ip);
        reduceNoOp(context, ip);
        reduceCmpJump(context, ip);
        reduceSwap(context, ip);
        reduceAppend(context, ip);
        reduceForceSafe(context, ip);
        reduceStackOp(context, ip);
        reduceLateStack(context, ip);
        reduceFactor(context, ip);
        reduceMath(context, ip);
    }

    return true;
}