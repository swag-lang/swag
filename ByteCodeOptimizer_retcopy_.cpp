#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "AstNode.h"

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
            if (ip->op == ByteCodeOp::PopRR)
                ip++;

            // This will copy the result in the real variable
            if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::MemCpy && ip[1].b.u32 == ipOrg->a.u32)
            {
                // Change the original stack pointer offset to reference the variable instead of the temporary
                // copy
                auto orgOffset = ipOrg->b.u32;
                ipOrg->b.u32   = ip->b.u32;

                // Is there a corresponding drop in the scope ?
                bool hasDrop = false;
                for (auto& toDrop : ipOrg->node->ownerScope->symTable.structVarsToDrop)
                {
                    if (toDrop.storageOffset == orgOffset && toDrop.typeStruct)
                    {
                        hasDrop = true;
                        break;
                    }
                }

                // Remove the MakeStackPointer
                // For now we replace it with a reinit of the variable, because of opDrop that can be called
                // on it.
                if (hasDrop)
                {
                    ip->op    = ByteCodeOp::SetZeroStackX;
                    ip->a.u32 = orgOffset;
                    ip->b.u32 = ip[1].c.u32; // Copy the size from the following memcpy
                }
                else
                {
                    setNop(context, ip);
                }

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
