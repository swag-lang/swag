#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"

bool ByteCodeOptimizer::optimizePassErr(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
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

    return true;
}
