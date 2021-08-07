#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "TypeInfo.h"

static void optimRetCopy(ByteCodeOptContext* context, ByteCodeInstruction* ipOrg, ByteCodeInstruction* ip)
{
    bool sameStackOffset = false;

    SWAG_ASSERT(ipOrg->op == ByteCodeOp::MakeStackPointer);
    auto orgOffset = ipOrg->b.u32;

    // If the second MakeStackPointer is the same as the first one (the parameter), then the
    // return copy is totally useless, se we must not simulate a drop, because there's in fact
    // only one variable, and not one variable copied to the other
    // This happens when inlining a function that returns a struct
    if (ip->op == ByteCodeOp::MakeStackPointer)
    {
        sameStackOffset = ipOrg->b.u32 == ip->b.u32;

        // Change the original stack pointer offset to reference the variable instead of the temporary
        // copy
        ipOrg->b.u32 = ip->b.u32;
    }
    else
    {
        SWAG_ASSERT(ip->op == ByteCodeOp::CopyRRtoRC);
        SET_OP(ipOrg, ByteCodeOp::CopyRRtoRC);
    }

    // Is there a corresponding drop in the scope ?
    bool hasDrop = false;
    if (!sameStackOffset)
    {
        for (auto& toDrop : ipOrg->node->ownerScope->symTable.structVarsToDrop)
        {
            if (!toDrop.typeStruct)
                continue;
            if (toDrop.storageOffset == orgOffset && (toDrop.typeStruct->opDrop || toDrop.typeStruct->opUserDropFct))
            {
                hasDrop = true;
                break;
            }
        }
    }

    // Remove opDrop to the old variable that is no more affected.
    // Make a nop, and detect all drops of that variable to remove them also.
    // Not sure this will work in all situations
    if (hasDrop)
    {
        // We start before the function call, because we can have opDrop in try/catch blocks
        auto ipe = ipOrg + 1;
        while (ipe->op != ByteCodeOp::End)
        {
            if (ipe[0].op == ByteCodeOp::MakeStackPointer && ipe[0].b.u32 == orgOffset)
            {
                if (ipe[1].op == ByteCodeOp::PushRAParam &&
                    (ipe[2].op == ByteCodeOp::LocalCall || ipe[2].op == ByteCodeOp::ForeignCall) &&
                    ipe[3].op == ByteCodeOp::IncSPPostCall)
                {
                    if (ip->node->ownerScope->isSameOrParentOf(ipe->node->ownerScope))
                    {
                        ByteCodeOptimizer::setNop(context, ipe);
                        ByteCodeOptimizer::setNop(context, ipe + 1);
                        ByteCodeOptimizer::setNop(context, ipe + 2);
                        ByteCodeOptimizer::setNop(context, ipe + 3);
                    }
                }
            }

            ipe++;
        }
    }

    // Remove the MakeStackPointer
    // NO ! The register can be necessary for some code after, especially if this is a CopyRRtoRC
    //ByteCodeOptimizer::setNop(context, ip);

    // Remove the memcpy
    ByteCodeOptimizer::setNop(context, ip + 1);

    // We need to remove every instructions related to the post move
    ip += 2;
    while (ip->flags & BCI_POST_COPYMOVE)
        ByteCodeOptimizer::setNop(context, ip++);
}

void ByteCodeOptimizer::registerParamsReg(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->op == ByteCodeOp::PushRAParam)
        context->paramsReg.push_back(ip->a.u32);
    else if (ip->op == ByteCodeOp::PushRAParam2)
    {
        context->paramsReg.push_back(ip->a.u32);
        context->paramsReg.push_back(ip->b.u32);
    }
    else if (ip->op == ByteCodeOp::PushRAParam3)
    {
        context->paramsReg.push_back(ip->a.u32);
        context->paramsReg.push_back(ip->b.u32);
        context->paramsReg.push_back(ip->c.u32);
    }
    else if (ip->op == ByteCodeOp::PushRAParam4)
    {
        context->paramsReg.push_back(ip->a.u32);
        context->paramsReg.push_back(ip->b.u32);
        context->paramsReg.push_back(ip->c.u32);
        context->paramsReg.push_back(ip->d.u32);
    }
}

void ByteCodeOptimizer::registerMakeAddr(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->op == ByteCodeOp::MakeStackPointer ||
        ip->op == ByteCodeOp::MakeBssSegPointer ||
        ip->op == ByteCodeOp::MakeMutableSegPointer ||
        ip->op == ByteCodeOp::MakeCompilerSegPointer)
    {
        context->mapU32U32[ip->a.u32] = ip->b.u32;
    }
    else if (ip->op == ByteCodeOp::InternalGetTlsPtr)
    {
        context->mapU32U32[ip->a.u32] = UINT32_MAX; // Disable optim whatever
    }
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
bool ByteCodeOptimizer::optimizePassRetCopyLocal(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        bool startOk = false;

        if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::CopyRCtoRT && ip->a.u32 == ip[1].a.u32)
            startOk = true;
        if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::IncPointer64 && ip[2].op == ByteCodeOp::CopyRCtoRT && ip->a.u32 == ip[2].a.u32)
            startOk = true;

        // Detect pushing pointer to the stack for a return value
        if (startOk)
        {
            auto ipOrg = ip;

            // Find the following call
            context->paramsReg.clear();
            while (ip->op != ByteCodeOp::End && ip->op != ByteCodeOp::LocalCall && ip->op != ByteCodeOp::ForeignCall && ip->op != ByteCodeOp::LambdaCall)
                ip++;

            if (ip->op != ByteCodeOp::End)
                ip++;
            if (ip->op == ByteCodeOp::IncSPPostCall)
                ip++;
            if (ip->op == ByteCodeOp::PopRR)
                ip++;
            while (ip->flags & BCI_TRYCATCH)
                ip++;

            // This will copy the result in the real variable
            if (ByteCode::isMemCpy(ip + 1) && ip[1].b.u32 == ipOrg->a.u32)
            {
                if (ip->op == ByteCodeOp::MakeStackPointer)
                    optimRetCopy(context, ipOrg, ip);
                else if (ip->op == ByteCodeOp::CopyRRtoRC && ip[1].a.u32 == ip[0].a.u32)
                    optimRetCopy(context, ipOrg, ip);
            }
            else
                ip = ipOrg;
        }
    }

    return true;
}

bool ByteCodeOptimizer::optimizePassRetCopyGlobal(ByteCodeOptContext* context)
{
    context->mapU32U32.clear();
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        bool startOk = false;
        registerMakeAddr(context, ip);

        if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::CopyRCtoRT && ip->a.u32 == ip[1].a.u32)
            startOk = true;
        if (ip->op == ByteCodeOp::MakeStackPointer && ip[1].op == ByteCodeOp::IncPointer64 && ip[2].op == ByteCodeOp::CopyRCtoRT && ip->a.u32 == ip[2].a.u32 && ip[1].a.u32 == ip[1].c.u32)
            startOk = true;

        // Detect pushing pointer to the stack for a return value
        if (startOk)
        {
            auto ipOrg = ip;

            // Find the following call
            context->paramsReg.clear();
            while (ip->op != ByteCodeOp::End && ip->op != ByteCodeOp::LocalCall && ip->op != ByteCodeOp::ForeignCall && ip->op != ByteCodeOp::LambdaCall)
            {
                registerParamsReg(context, ip);
                ip++;
            }

            if (ip->op != ByteCodeOp::End)
                ip++;
            if (ip->op == ByteCodeOp::IncSPPostCall)
                ip++;
            if (ip->op == ByteCodeOp::PopRR)
                ip++;
            while (ip->flags & BCI_TRYCATCH)
                ip++;

            // This will copy the result in the real variable
            if (ByteCode::isMemCpy(ip) && ip->b.u32 == ipOrg->a.u32)
            {
                // Pointer aliasing. Do not make the optimization if the wanted result is also a parameter
                // to the call. For example a = toto(a, b).
                bool ok = true;
                auto it = context->mapU32U32.find(ip->a.u32);
                if (it != context->mapU32U32.end())
                {
                    for (auto it1 : context->paramsReg)
                    {
                        auto it2 = context->mapU32U32.find(it1);
                        if (it2 == context->mapU32U32.end())
                            continue;
                        if (it2->second == UINT32_MAX || it->second == UINT32_MAX || it2->second == it->second)
                        {
                            ok = false;
                            ip = ipOrg;
                            break;
                        }
                    }
                }

                if (ok)
                {
                    // Make CopyRCtoRT point to the MemCpy destination register
                    if (ipOrg[1].op == ByteCodeOp::CopyRCtoRT)
                        ipOrg[1].a.u32 = ip->a.u32;
                    else
                    {
                        SWAG_ASSERT(ipOrg[1].op == ByteCodeOp::IncPointer64);
                        ipOrg[1].a.u32 = ip->a.u32;
                        ipOrg[1].c.u64 = ip->a.u32;

                        SWAG_ASSERT(ipOrg[2].op == ByteCodeOp::CopyRCtoRT);
                        ipOrg[2].a.u32 = ip->a.u32;
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
            }
            else
            {
                ip = ipOrg;
            }

            context->mapU32U32.clear();
        }
    }

    return true;
}

// Same, but we make the detection before and after a function that has been inlined
bool ByteCodeOptimizer::optimizePassRetCopyInline(ByteCodeOptContext* context)
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
            while (ip->flags & BCI_TRYCATCH)
                ip++;

            // This will copy the result in the real variable
            /*if (ByteCode::isMemCpy(ip) && ip->b.u32 == ipOrg->a.u32 && ipOrg->a.u32 == ipOrg[1].node->ownerInline->resultRegisterRC[0])
            {
                SET_OP(ipOrg, ByteCodeOp::CopyRBtoRA64);
                ipOrg->b.u32 = ip->a.u32;
                setNop(context, ip);
            }
            else */
            if (ip->op == ByteCodeOp::MakeStackPointer && ByteCode::isMemCpy(ip + 1) && ip[1].b.u32 == ipOrg->a.u32)
                optimRetCopy(context, ipOrg, ip);
            else
                ip = ipOrg;
        }
    }

    return true;
}