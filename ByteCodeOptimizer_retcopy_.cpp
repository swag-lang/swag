#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"

// When a function returns something by copy, this is first moved to a temporary place on the stack of the caller,
// then this is moved back to the variable of the caller if there's an affectation.
// This pass will remove the unnecessary copy to the temporary variable, and will just pass the right callstack address
// to the callee.
// Pattern detected is :
//
// MakeStackPointer X
// CopyRCtoRT X
// ...
// Call
// ...
// MakeStackPointer Y
// MemCpy X to Y
// ... post move stuff
void ByteCodeOptimizer::optimizePassRetCopy(ByteCodeOptContext* context)
{
    if (context->bc->sourceFile->name == "compiler2073.swg")
        context = context;

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        // Detect pushing pointer to the stack for a return value
        if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::CopyRCtoRT && ip->a.u32 == ip[1].b.u32)
        {
            auto ipOrg = ip;

            // Find the following call
            while (ip->op != ByteCodeOp::End && ip->op != ByteCodeOp::LocalCall && ip->op != ByteCodeOp::ForeignCall && ip->op != ByteCodeOp::LambdaCall)
                ip++;
            if (ip->op != ByteCodeOp::End)
                ip++;
            if (ip->op == ByteCodeOp::IncSPPostCall)
                ip++;

            // This will copy the result in the real variable
            if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::MemCpy && ip[1].b.u32 == ipOrg->a.u32)
            {
                // Change the original stack pointer offset to reference the variable instead of the temporary
                // copy
                ipOrg->b.u32 = ip->b.u32;

                // Remove the MakeStackPointer
                setNop(context, ip);
                // Remove the memcpy
                setNop(context, ip + 1);
                // We need to remove every instructions related to the post move
                ip += 2;
                while (ip->flags & BCI_POST_MOVE)
                    setNop(context, ip++);
            }
            else
            {
                ip = ipOrg;
            }
        }
    }
}
