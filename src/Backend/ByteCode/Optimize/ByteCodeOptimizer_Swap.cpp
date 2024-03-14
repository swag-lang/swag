#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"

bool ByteCodeOptimizer::optimizePassSwap(ByteCodeOptContext* context)
{
    bool restart = true;
    while (restart)
    {
        restart = false;
        context->mapInstInst.clear();
        for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
        {
            switch (ip->op)
            {
                case ByteCodeOp::MakeStackPointer:
                case ByteCodeOp::GetParam8:
                case ByteCodeOp::GetParam16:
                case ByteCodeOp::GetParam32:
                case ByteCodeOp::GetParam64:
                case ByteCodeOp::GetIncParam64:
                case ByteCodeOp::SetImmediate32:
                case ByteCodeOp::SetImmediate64:
                case ByteCodeOp::MakeConstantSegPointer:
                    break;

                case ByteCodeOp::IncPointer64:
                case ByteCodeOp::DecPointer64:
                    if (!ip->hasFlag(BCI_IMM_B))
                        continue;
                    break;
                default:
                    continue;
            }

            auto                 ipn             = ip + 1;
            ByteCodeInstruction* startParamBlock = nullptr;
            while (true)
            {
                if (ipn->op == ByteCodeOp::Ret || ipn->op == ByteCodeOp::CopyRBtoRRRet)
                    break;
                if (ipn->hasFlag(BCI_START_STMT))
                    break;
                if (ipn->op == ByteCodeOp::Nop)
                    break;

                // We do not want something like:
                // PushRAParam R0
                // R0 = ?
                // PushRAParam R0
                //
                // This could happen because of optimization passes.
                //
                // When generating backend, R0 will have the same value in both push, which
                // should not be the case.
                //
                // So we never move an instruction INSIDE a PushParam block

                if (ByteCode::isPushParam(ipn) && !startParamBlock)
                    startParamBlock = ipn;

                if ((ByteCode::hasWriteRegInA(ip) && ByteCode::hasRefToReg(ipn, ip->a.u32)) ||
                    (ByteCode::hasWriteRegInB(ip) && ByteCode::hasRefToReg(ipn, ip->b.u32)) ||
                    (ByteCode::hasWriteRegInC(ip) && ByteCode::hasRefToReg(ipn, ip->c.u32)) ||
                    (ByteCode::hasWriteRegInD(ip) && ByteCode::hasRefToReg(ipn, ip->d.u32)))
                {
                    context->mapInstInst[ip] = startParamBlock ? startParamBlock : ipn;
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

                // After the call, this is the end of the param block, so we can move an instruction
                // here again, as long as nothing in the param block and the call references the
                // register
                if (ByteCode::isCall(ipn))
                    startParamBlock = nullptr;

                ipn++;
            }
        }

        for (const auto it : context->mapInstInst)
        {
            auto       ip  = it.first;
            const auto ipn = it.second;

            if (ipn == ip + 1)
                continue;

            auto it1 = context->mapInstInst.find(ip + 1);
            if (it1 != context->mapInstInst.end())
            {
                if (it1->second > it.second)
                    continue;

                if (it1->second == it.second)
                {
                    switch (ip->op)
                    {
                        case ByteCodeOp::MakeStackPointer:
                            if (ip[1].op != ByteCodeOp::GetFromStack8 &&
                                ip[1].op != ByteCodeOp::GetFromStack16 &&
                                ip[1].op != ByteCodeOp::GetFromStack32 &&
                                ip[1].op != ByteCodeOp::GetFromStack64 &&
                                ip[1].op != ByteCodeOp::GetParam8 &&
                                ip[1].op != ByteCodeOp::GetParam16 &&
                                ip[1].op != ByteCodeOp::GetParam32 &&
                                ip[1].op != ByteCodeOp::GetParam64)
                                continue;
                            break;
                        default:
                        {
                            int cpt0 = 0;
                            int cpt1 = 0;

                            cpt0 += ByteCode::hasReadRegInA(ip) || ByteCode::hasWriteRegInA(ip) ? 1 : 0;
                            cpt0 += ByteCode::hasReadRegInB(ip) || ByteCode::hasWriteRegInB(ip) ? 1 : 0;
                            cpt0 += ByteCode::hasReadRegInC(ip) || ByteCode::hasWriteRegInC(ip) ? 1 : 0;
                            cpt0 += ByteCode::hasReadRegInD(ip) || ByteCode::hasWriteRegInD(ip) ? 1 : 0;

                            cpt1 += ByteCode::hasReadRegInA(ip + 1) || ByteCode::hasWriteRegInA(ip + 1) ? 1 : 0;
                            cpt1 += ByteCode::hasReadRegInB(ip + 1) || ByteCode::hasWriteRegInB(ip + 1) ? 1 : 0;
                            cpt1 += ByteCode::hasReadRegInC(ip + 1) || ByteCode::hasWriteRegInC(ip + 1) ? 1 : 0;
                            cpt1 += ByteCode::hasReadRegInD(ip + 1) || ByteCode::hasWriteRegInD(ip + 1) ? 1 : 0;

                            if (cpt1 <= cpt0)
                                continue;
                            break;
                        }
                    }
                }
            }

            while (ip + 1 != ipn)
            {
                std::swap(ip[0].op, ip[1].op);
                std::swap(ip[0].flags, ip[1].flags);
                std::swap(ip[0].a, ip[1].a);
                std::swap(ip[0].b, ip[1].b);
                std::swap(ip[0].c, ip[1].c);
                std::swap(ip[0].d, ip[1].d);
                std::swap(ip[0].node, ip[1].node);
                std::swap(ip[0].location, ip[1].location);

                if (ip[1].hasFlag(BCI_START_STMT))
                {
                    ip[0].inheritFlag(ip + 1, BCI_START_STMT);
                    ip[1].removeFlag(BCI_START_STMT);
                }

                ip++;
            }

            restart = true;
            context->setDirtyPass();
            break;
        }
    }

    return true;
}
