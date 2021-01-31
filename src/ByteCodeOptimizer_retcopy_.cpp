#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "TypeInfo.h"

static void optimRetCopy(ByteCodeOptContext* context, ByteCodeInstruction* ipOrg, ByteCodeInstruction* ip)
{
    // If the second MakeStackPointer is the same as the first one (the parameter), then the
    // return copy is totally useless, se we must not simulate a drop, because there's in fact
    // only one variable, and not one variable copied to the other
    // This happens when inlining a function that returns a struct
    bool sameStackOffset = ipOrg->b.u32 == ip->b.u32;

    // Change the original stack pointer offset to reference the variable instead of the temporary
    // copy
    auto orgOffset = ipOrg->b.u32;
    ipOrg->b.u32   = ip->b.u32;

    // Is there a corresponding drop in the scope ?
    bool hasDrop = false;
    if (!sameStackOffset)
    {
        for (auto& toDrop : ipOrg->node->ownerScope->symTable.structVarsToDrop)
        {
            if (!toDrop.typeStruct)
                continue;
            if (toDrop.storageOffset == orgOffset && toDrop.typeStruct->opDrop)
            {
                hasDrop = true;
                break;
            }
        }
    }

    // Remove the MakeStackPointer
    // For now we replace it with a reinit of the variable, because of opDrop that can be called
    // on it.
    if (hasDrop)
    {
        ip->op    = ByteCodeOp::SetZeroStackX;
        ip->a.u32 = orgOffset;
        ip->b.u64 = ip[1].c.u32; // Copy the size from the following memcpy
    }
    else
    {
        ByteCodeOptimizer::setNop(context, ip);
    }

    // Remove the memcpy
    ByteCodeOptimizer::setNop(context, ip + 1);
    // We need to remove every instructions related to the post move
    ip += 2;
    while (ip->flags & BCI_POST_COPYMOVE)
        ByteCodeOptimizer::setNop(context, ip++);
}

// When a function returns something by copy, this is first moved to a temporary place on the stack of the caller,
// then this is moved back to the variable of the caller if there's an affectation.
// This pass will remove the unnecessary copy to the temporary variable, and will just pass the right callstack address
// to the callee.
// Pattern detected is :
//
// MakeStackPointer X
// (IncPointer64)
// CopyRCtoRT X
// ...
// Call
// ...
// MakeStackPointer Y
// MemCpy X to Y
// ... post move stuff
void ByteCodeOptimizer::optimizePassRetCopyLocal(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        bool startOk = false;
        if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::CopyRCtoRT && ip->a.u32 == ip[1].b.u32)
            startOk = true;
        if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::IncPointer64 && ip[2].op == ByteCodeOp::CopyRCtoRT && ip->a.u32 == ip[2].b.u32)
            startOk = true;

        // Detect pushing pointer to the stack for a return value
        if (startOk)
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
            if (ip->op == ByteCodeOp::MakeStackPointer && isMemCpy(ip + 1) && ip[1].b.u32 == ipOrg->a.u32)
                optimRetCopy(context, ipOrg, ip);
            else
                ip = ipOrg;
        }
    }
}

// Same, but we make the detection before and after a function that has been inlined
void ByteCodeOptimizer::optimizePassRetCopyInline(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        bool startOk = false;
        if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].node->ownerInline != ip[0].node->ownerInline)
            startOk = true;
        if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::IncPointer64 && ip[2].node->ownerInline != ip[0].node->ownerInline)
            startOk = true;

        // Detect pushing pointer to the stack for a return value
        if (startOk)
        {
            auto ipOrg = ip;

            // Go to the end of the inline block
            ip++;
            while (ip->op != ByteCodeOp::End && ip->node->ownerInline != ipOrg->node->ownerInline)
                ip++;
            if (ip->op == ByteCodeOp::PopRR)
                ip++;

            // This will copy the result in the real variable
            if (ip->op == ByteCodeOp::MakeStackPointer && isMemCpy(ip + 1) && ip[1].b.u32 == ipOrg->a.u32)
                optimRetCopy(context, ipOrg, ip);
            else
                ip = ipOrg;
        }
    }
}

void ByteCodeOptimizer::optimizePassRetCopyGlobal(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        bool startOk = false;
        if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::CopyRCtoRT && ip->a.u32 == ip[1].b.u32)
            startOk = true;
        if (ip->op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::IncPointer64 &&
            ip[2].op == ByteCodeOp::CopyRCtoRT &&
            ip->a.u32 == ip[2].b.u32 && ip[1].a.u32 == ip[1].c.u32)
            startOk = true;

        // Detect pushing pointer to the stack for a return value
        if (startOk)
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
            if (isMemCpy(ip) && ip->b.u32 == ipOrg->a.u32)
            {
                // Make CopyRCtoRT point to the MemCpy destination register
                if (ipOrg[1].op == ByteCodeOp::CopyRCtoRT)
                    ipOrg[1].b.u64 = ip->a.u32;
                else
                {
                    SWAG_ASSERT(ipOrg[1].op == ByteCodeOp::IncPointer64);
                    ipOrg[1].a.u32 = ip->a.u32;
                    ipOrg[1].c.u64 = ip->a.u32;

                    SWAG_ASSERT(ipOrg[2].op == ByteCodeOp::CopyRCtoRT);
                    ipOrg[2].b.u64 = ip->a.u32;
                }

                // Remove the original MakeStackPointer
                setNop(context, ipOrg);

                // Remove the memcpy
                ByteCodeOptimizer::setNop(context, ip);
                // We need to remove every instructions related to the post move
                ip += 1;
                while (ip->flags & BCI_POST_COPYMOVE)
                    ByteCodeOptimizer::setNop(context, ip++);
            }
            else
            {
                ip = ipOrg;
            }
        }
    }
}