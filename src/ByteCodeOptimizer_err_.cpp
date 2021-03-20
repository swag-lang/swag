#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"

void ByteCodeOptimizer::optimizePassErr(ByteCodeOptContext* context)
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

                if (ipScan[0].op == ByteCodeOp::IntrinsicSetErr ||
                    ipScan[0].op == ByteCodeOp::LambdaCall ||
                    ipScan[0].op == ByteCodeOp::LocalCall ||
                    ipScan[0].op == ByteCodeOp::ForeignCall)
                    hasSetErr = true;
                ipScan++;
            }

            if (!hasSetErr)
            {
                setNop(context, ip);
                setNop(context, ipEnd);
            }
        }
    }
}
