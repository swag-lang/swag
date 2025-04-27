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
                case ByteCodeOp::GetParam8:
                case ByteCodeOp::GetParam16:
                case ByteCodeOp::GetParam32:
                case ByteCodeOp::GetParam64:
                case ByteCodeOp::GetParam64x2:
                case ByteCodeOp::GetIncParam64:
                case ByteCodeOp::MakeStackPointer:
                case ByteCodeOp::MakeLambda:
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
                if (ipn->op == ByteCodeOp::Ret || ipn->op == ByteCodeOp::CopyRAtoRRRet)
                    break;
                if (ipn->hasFlag(BCI_START_STMT))
                    break;
                if (ipn->op == ByteCodeOp::Nop)
                    break;
                if (ByteCode::haveSameRead(ip, ipn))
                    break;

                // We do not want something like:
                // PushRAParam R0
                // R0 =?
                // PushRAParam R0
                //
                // This could happen because of optimization passes.
                //
                // When generating backend, R0 will have the same value in both push, which
                // cannot be the case.
                //
                // So we never move an instruction INSIDE a PushParam block

                if (ipn->isPushParam() && !startParamBlock)
                    startParamBlock = ipn;

                if ((ip->hasWriteRegInA() && ipn->hasRefToReg(ip->a.u32)) ||
                    (ip->hasWriteRegInB() && ipn->hasRefToReg(ip->b.u32)) ||
                    (ip->hasWriteRegInC() && ipn->hasRefToReg(ip->c.u32)) ||
                    (ip->hasWriteRegInD() && ipn->hasRefToReg(ip->d.u32)))
                {
                    context->mapInstInst[ip] = startParamBlock ? startParamBlock : ipn;
                    break;
                }

                if ((ip->hasReadRegInA() && ipn->hasWriteRefToReg(ip->a.u32)) ||
                    (ip->hasReadRegInB() && ipn->hasWriteRefToReg(ip->b.u32)) ||
                    (ip->hasReadRegInC() && ipn->hasWriteRefToReg(ip->c.u32)) ||
                    (ip->hasReadRegInD() && ipn->hasWriteRefToReg(ip->d.u32)))
                {
                    break;
                }

                if (ipn->isJump())
                    break;

                // After the call, this is the end of the param block, so we can move an instruction
                // here again, as long as nothing in the param block and the call references the
                // register
                if (ipn->isCall())
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

                            cpt0 += ip->hasReadRegInA() || ip->hasWriteRegInA() ? 1 : 0;
                            cpt0 += ip->hasReadRegInB() || ip->hasWriteRegInB() ? 1 : 0;
                            cpt0 += ip->hasReadRegInC() || ip->hasWriteRegInC() ? 1 : 0;
                            cpt0 += ip->hasReadRegInD() || ip->hasWriteRegInD() ? 1 : 0;

                            cpt1 += ip[1].hasReadRegInA() || ip[1].hasWriteRegInA() ? 1 : 0;
                            cpt1 += ip[1].hasReadRegInB() || ip[1].hasWriteRegInB() ? 1 : 0;
                            cpt1 += ip[1].hasReadRegInC() || ip[1].hasWriteRegInC() ? 1 : 0;
                            cpt1 += ip[1].hasReadRegInD() || ip[1].hasWriteRegInD() ? 1 : 0;

                            if (cpt1 <= cpt0)
                                continue;
                            break;
                        }
                    }
                }
            }

            while (ip + 1 != ipn)
            {
                ByteCode::swapInstructions(ip, ip + 1);
                ip++;
            }

            restart = true;
            context->setDirtyPass();
            break;
        }
    }

    return true;
}
