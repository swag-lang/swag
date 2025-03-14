#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"

// Remove duplicated pure instructions (set RA to a constant)
bool ByteCodeOptimizer::optimizePassParam(ByteCodeOptContext* context)
{
    // See setContextFlags if you add one instruction
    if (!(context->contextBcFlags & OCF_HAS_PARAM))
        return true;

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->op == ByteCodeOp::CopyRRtoRA)
        {
            auto ipScan = ip + 1;
            while (!ipScan->isRet() && !ipScan->hasFlag(BCI_START_STMT) && !ipScan->isJump())
            {
                if (ipScan->op == ByteCodeOp::CopyRRtoRA && ipScan->a.u32 == ip->a.u32)
                {
                    if (ipScan->b.u64 >= ip->b.u64)
                    {
                        SET_OP(ipScan, ByteCodeOp::IncPointer64);
                        ipScan->addFlag(BCI_IMM_B);
                        ipScan->b.u64 -= ip->b.u64;
                        ipScan->c.u32 = ipScan->a.u32;
                        return true;
                    }
                }

                if (ipScan->hasWriteRefToReg(ip->a.u32))
                    break;

                ipScan++;
            }
        }
        else if (ip->op == ByteCodeOp::GetParam64)
        {
            auto ipScan = ip + 1;
            while (!ipScan->isRet() && !ipScan->hasFlag(BCI_START_STMT) && !ipScan->isJump())
            {
                if (ipScan->op == ByteCodeOp::GetIncParam64 &&
                    ipScan->a.u32 == ip->a.u32 &&
                    ipScan->b.u64 == ip->b.u64)
                {
                    SET_OP(ipScan, ByteCodeOp::IncPointer64);
                    ipScan->addFlag(BCI_IMM_B);
                    ipScan->b.u64 = ipScan->d.u64;
                    ipScan->c.u32 = ipScan->a.u32;
                    return true;
                }

                if (ipScan->hasWriteRefToReg(ip->a.u32))
                    break;

                ipScan++;
            }
        }
        else if (ip->op == ByteCodeOp::GetIncParam64)
        {
            auto ipScan = ip + 1;
            while (!ipScan->isRet() && !ipScan->hasFlag(BCI_START_STMT) && !ipScan->isJump())
            {
                if (ipScan->op == ByteCodeOp::GetIncParam64 &&
                    ipScan->a.u32 == ip->a.u32 &&
                    ipScan->b.u64 == ip->b.u64)
                {
                    if (ipScan->d.u64 >= ip->d.u64)
                    {
                        SET_OP(ipScan, ByteCodeOp::IncPointer64);
                        ipScan->addFlag(BCI_IMM_B);
                        ipScan->b.u64 = ipScan->d.u64 - ip->d.u64;
                        ipScan->c.u32 = ipScan->a.u32;
                        return true;
                    }
                }

                if (ipScan->hasWriteRefToReg(ip->a.u32))
                    break;

                ipScan++;
            }
        }
    }

    return true;
}
