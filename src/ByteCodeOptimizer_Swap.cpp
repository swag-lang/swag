#include "pch.h"
#include "ByteCodeOptimizer.h"

bool ByteCodeOptimizer::optimizePassSwap(ByteCodeOptContext* context)
{
    context->mapInstInst.clear();
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
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
        case ByteCodeOp::MakeConstantSegPointer:
            break;
        case ByteCodeOp::IncPointer64:
            if (!(ip->flags & BCI_IMM_B))
                continue;
            break;
        default:
            continue;
        }

        auto ipn = ip + 1;
        while (true)
        {
            if (ipn->op == ByteCodeOp::Ret || ipn->op == ByteCodeOp::CopyRCtoRRRet)
                break;
            if (ipn->flags & BCI_START_STMT)
                break;
            if (ip->node->ownerInline != ipn->node->ownerInline)
                break;
            if (ipn->op == ByteCodeOp::Nop)
                break;

            // Do NOT move after a parameter, because we do not want the same register to be stored in
            // different parameters
            if (ipn->op == ByteCodeOp::PushRAParam ||
                ipn->op == ByteCodeOp::PushRAParam2 ||
                ipn->op == ByteCodeOp::PushRAParam3 ||
                ipn->op == ByteCodeOp::PushRAParam4 ||
                ipn->op == ByteCodeOp::PushRAParamCond ||
                ipn->op == ByteCodeOp::PushRVParam)
                break;

            if ((ByteCode::hasWriteRegInA(ip) && ByteCode::hasRefToReg(ipn, ip->a.u32)) ||
                (ByteCode::hasWriteRegInB(ip) && ByteCode::hasRefToReg(ipn, ip->b.u32)) ||
                (ByteCode::hasWriteRegInC(ip) && ByteCode::hasRefToReg(ipn, ip->c.u32)) ||
                (ByteCode::hasWriteRegInD(ip) && ByteCode::hasRefToReg(ipn, ip->d.u32)))
            {
                context->mapInstInst[ip] = ipn;
                break;
            }

            if ((ByteCode::hasReadRegInA(ip) && ByteCode::hasWriteRefToReg(ipn, ip->a.u32)) ||
                (ByteCode::hasReadRegInB(ip) && ByteCode::hasWriteRefToReg(ipn, ip->b.u32)) ||
                (ByteCode::hasReadRegInC(ip) && ByteCode::hasWriteRefToReg(ipn, ip->c.u32)) ||
                (ByteCode::hasReadRegInD(ip) && ByteCode::hasWriteRefToReg(ipn, ip->d.u32)))
            {
                break;
            }

            if (ByteCode::isJump(ipn))
                break;
            ipn++;
        }
    }

    for (auto it : context->mapInstInst)
    {
        auto ip  = it.first;
        auto ipn = it.second;

        if (ipn == ip + 1)
            continue;

        auto it1 = context->mapInstInst.find(ip + 1);
        if (it1 != context->mapInstInst.end())
        {
            if (it1->second > it.second)
                continue;

            if (it1->second == it.second)
            {
                if (ip->op != ByteCodeOp::MakeStackPointer)
                    continue;
                if (ip[1].op != ByteCodeOp::GetFromStack8 &&
                    ip[1].op != ByteCodeOp::GetFromStack16 &&
                    ip[1].op != ByteCodeOp::GetFromStack32 &&
                    ip[1].op != ByteCodeOp::GetFromStack64 &&
                    ip[1].op != ByteCodeOp::GetParam8 &&
                    ip[1].op != ByteCodeOp::GetParam16 &&
                    ip[1].op != ByteCodeOp::GetParam32 &&
                    ip[1].op != ByteCodeOp::GetParam64)
                    continue;
            }
        }

        swap(ip[0], ip[1]);

        if (ip[1].flags & BCI_START_STMT)
        {
            ip[0].flags |= ip[1].flags & BCI_START_STMT;
            ip[1].flags &= ~BCI_START_STMT;
        }

        context->passHasDoneSomething = true;
        break;
    }

    return true;
}