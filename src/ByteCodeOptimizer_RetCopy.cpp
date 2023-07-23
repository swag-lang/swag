#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "TypeInfo.h"
#include "TypeManager.h"

static void removeOpDrop(ByteCodeOptContext* context, ByteCodeInstruction* ipOrg, ByteCodeInstruction* ip, uint32_t orgOffset)
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
            else if (ipe[1].op == ByteCodeOp::PushRAParam &&
                     (ipe[2].op == ByteCodeOp::LocalCallPop || ipe[2].op == ByteCodeOp::ForeignCallPop))
            {
                if (ip->node->ownerScope->isSameOrParentOf(ipe->node->ownerScope))
                {
                    ByteCodeOptimizer::setNop(context, ipe);
                    ByteCodeOptimizer::setNop(context, ipe + 1);
                    ByteCodeOptimizer::setNop(context, ipe + 2);
                }
            }
            else if (ipe[1].op == ByteCodeOp::LocalCallPopParam)
            {
                if (ip->node->ownerScope->isSameOrParentOf(ipe->node->ownerScope))
                {
                    ByteCodeOptimizer::setNop(context, ipe);
                    ByteCodeOptimizer::setNop(context, ipe + 1);
                }
            }
        }

        ipe++;
    }
}

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
        SWAG_ASSERT(ipOrg->op != ByteCodeOp::CopyRRtoRC);
        ipOrg->b.u32 = ip->b.u32;
    }
    else
    {
        SWAG_ASSERT(ip->op == ByteCodeOp::CopyRRtoRC);
        SET_OP(ipOrg, ByteCodeOp::CopyRRtoRC);
        ipOrg->b.u64 = ip->b.u64;
    }

    // Is there a corresponding drop in the scope ?
    bool hasDrop = !sameStackOffset && ipOrg->node->ownerScope->symTable.structVarsToDrop.count > 0;

    // Remove opDrop to the old variable that is no more affected.
    // Make a nop, and detect all drops of that variable to remove them also.
    // Not sure this will work in all situations
    if (hasDrop)
        removeOpDrop(context, ipOrg, ip, orgOffset);

    // Remove the MakeStackPointer
    // NO ! The register can be necessary for some code after, especially if this is a CopyRRtoRC
    // ByteCodeOptimizer::setNop(context, ip);

    // Remove the memcpy
    ByteCodeOptimizer::setNop(context, ip + 1);

    // We need to remove every instructions related to the post move
    ip += 2;
    while (ip->flags & BCI_POST_COPYMOVE)
        ByteCodeOptimizer::setNop(context, ip++);
}

void ByteCodeOptimizer::registerParamsReg(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::PushRAParam:
        context->vecReg.push_back(ip->a.u32);
        break;
    case ByteCodeOp::PushRAParam2:
        context->vecReg.push_back(ip->a.u32);
        context->vecReg.push_back(ip->b.u32);
        break;
    case ByteCodeOp::PushRAParam3:
        context->vecReg.push_back(ip->a.u32);
        context->vecReg.push_back(ip->b.u32);
        context->vecReg.push_back(ip->c.u32);
        break;
    case ByteCodeOp::PushRAParam4:
        context->vecReg.push_back(ip->a.u32);
        context->vecReg.push_back(ip->b.u32);
        context->vecReg.push_back(ip->c.u32);
        context->vecReg.push_back(ip->d.u32);
        break;

    default:
        break;
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
    if (!(context->contextBcFlags & OCF_HAS_COPY_RCRT))
        return true;

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        bool startOk = false;

        if (ip->op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::CopyRCtoRT &&
            ip->a.u32 == ip[1].a.u32)
            startOk = true;

        if (ip->op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::IncPointer64 &&
            ip[2].op == ByteCodeOp::CopyRCtoRT &&
            ip->a.u32 == ip[2].a.u32)
            startOk = true;

        // Detect pushing pointer to the stack for a return value
        if (startOk)
        {
            auto ipOrg = ip;

            // Find the following call
            context->vecReg.clear();
            while (ip->op != ByteCodeOp::End &&
                   ip->op != ByteCodeOp::LocalCall &&
                   ip->op != ByteCodeOp::LocalCallPop &&
                   ip->op != ByteCodeOp::LocalCallPopParam &&
                   ip->op != ByteCodeOp::LocalCallPopRC &&
                   ip->op != ByteCodeOp::ForeignCall &&
                   ip->op != ByteCodeOp::ForeignCallPop &&
                   ip->op != ByteCodeOp::LambdaCall &&
                   ip->op != ByteCodeOp::LambdaCallPop)
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

void ByteCodeOptimizer::registerMakeAddr(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
    case ByteCodeOp::MakeStackPointer:
    case ByteCodeOp::MakeBssSegPointer:
    case ByteCodeOp::MakeMutableSegPointer:
    case ByteCodeOp::MakeCompilerSegPointer:
        context->mapRegReg.set(ip->a.u32, ip->b.u32);
        break;
    case ByteCodeOp::InternalGetTlsPtr:
        context->mapRegReg.set(ip->a.u32, UINT32_MAX); // Disable optim whatever
        break;

    case ByteCodeOp::GetParam64:

        // If the next instruction changes the value, then we can do the optim (this is in fact a GetIncParam64)
        if (ip[1].op == ByteCodeOp::IncPointer64 && ip[1].a.u32 == ip[1].c.u32 && ip[0].a.u32 == ip[1].a.u32)
            break;

        // If parameter is a simple native, then this is a copy, so this is safe
        if (context->bc->typeInfoFunc)
        {
            auto param = context->bc->typeInfoFunc->registerIdxToType(ip->c.u32);
            if (param->isNativeIntegerOrRune() || param->isNativeFloat() || param->isBool())
                break;
        }

        context->mapRegReg.set(ip->a.u32, UINT32_MAX); // Disable optim whatever
        break;

    default:
    {
        if (ByteCode::hasWriteRegInA(ip))
            context->mapRegReg.remove(ip->a.u32);
        if (ByteCode::hasWriteRegInB(ip))
            context->mapRegReg.remove(ip->b.u32);
        if (ByteCode::hasWriteRegInC(ip))
            context->mapRegReg.remove(ip->c.u32);
        if (ByteCode::hasWriteRegInD(ip))
            context->mapRegReg.remove(ip->d.u32);
        break;
    }
    }
}

bool ByteCodeOptimizer::optimizePassRetCopyGlobal(ByteCodeOptContext* context)
{
    if (!(context->contextBcFlags & OCF_HAS_COPY_RCRT))
        return true;

    context->mapRegReg.clear();
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        bool startOk = false;
        registerMakeAddr(context, ip);

        if (ip->op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::CopyRCtoRT &&
            ip->a.u32 == ip[1].a.u32)
            startOk = true;

        if (ip->op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::IncPointer64 &&
            ip[2].op == ByteCodeOp::CopyRCtoRT &&
            ip->a.u32 == ip[2].a.u32 &&
            ip[1].a.u32 == ip[1].c.u32)
            startOk = true;

        // Detect pushing pointer to the stack for a return value
        if (startOk)
        {
            auto ipOrg     = ip;
            auto orgOffset = ipOrg->b.u32;

            // Find the following call
            context->vecReg.clear();
            while (ip->op != ByteCodeOp::End &&
                   ip->op != ByteCodeOp::LocalCall &&
                   ip->op != ByteCodeOp::LocalCallPop &&
                   ip->op != ByteCodeOp::LocalCallPopParam &&
                   ip->op != ByteCodeOp::LocalCallPopRC &&
                   ip->op != ByteCodeOp::ForeignCall &&
                   ip->op != ByteCodeOp::ForeignCallPop &&
                   ip->op != ByteCodeOp::LambdaCall &&
                   ip->op != ByteCodeOp::LambdaCallPop)
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
                auto it = context->mapRegReg.find(ip->a.u32);
                if (it)
                {
                    for (auto it1 : context->vecReg)
                    {
                        auto it2 = context->mapRegReg.find(it1);
                        if (!it2)
                            continue;
                        if ((*it2) == UINT32_MAX || (*it) == UINT32_MAX || *it2 == *it)
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

                    // Is there a corresponding drop in the scope ?
                    bool hasDrop = ipOrg->node->ownerScope->symTable.structVarsToDrop.count > 0;

                    // Remove opDrop to the old variable that is no more affected.
                    // Make a nop, and detect all drops of that variable to remove them also.
                    // Not sure this will work in all situations
                    if (hasDrop)
                        removeOpDrop(context, ipOrg, ip, orgOffset);

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

            context->mapRegReg.clear();
        }
    }

    return true;
}

// Same, but we make the detection before and after a function that has been inlined
bool ByteCodeOptimizer::optimizePassRetCopyInline(ByteCodeOptContext* context)
{
    if (!(context->contextBcFlags & OCF_HAS_INLINE))
        return true;

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        bool       startOk     = false;
        AstInline* inlineBlock = nullptr;

        if (ip->op == ByteCodeOp::MakeStackPointer &&
            ip[1].node->ownerInline != ip[0].node->ownerInline)
        {
            inlineBlock = ip[1].node->ownerInline;
            startOk     = true;
        }

        if (ip->op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::IncPointer64 &&
            ip[2].node->ownerInline != ip[0].node->ownerInline)
        {
            inlineBlock = ip[2].node->ownerInline;
            startOk     = true;
        }

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
            bool canOptim = false;
            if (ip->op == ByteCodeOp::MakeStackPointer && ByteCode::isMemCpy(ip + 1) && ip[1].b.u32 == ipOrg->a.u32)
                canOptim = true;

            // For now, disable optimization for a struct.
            if (canOptim && inlineBlock && TypeManager::concreteType(inlineBlock->func->typeInfo)->isStruct())
                canOptim = false;

            if (canOptim)
                optimRetCopy(context, ipOrg, ip);
            else
                ip = ipOrg;
        }
    }

    return true;
}

// Optimize the return value when this is a struct
// If we affect the result to a local variable, then remove on unecessary copy
bool ByteCodeOptimizer::optimizePassRetCopyStructVal(ByteCodeOptContext* context)
{
    if (!(context->contextBcFlags & OCF_HAS_COPY_RTRC))
        return true;

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip[0].op == ByteCodeOp::CopyRTtoRC &&
            ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
            ip[2].op == ByteCodeOp::IncSPPostCall &&
            ip[3].op == ByteCodeOp::CopyStack64 &&
            ip[0].a.u32 == ip[1].b.u32 &&
            ip[1].a.u32 == ip[3].b.u32)
        {
            ip[1].a.u32 = ip[3].a.u32;
            setNop(context, ip + 3);
        }
        else if (ip[0].op == ByteCodeOp::CopyRTtoRC &&
                 ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
                 ip[2].op == ByteCodeOp::CopyStack64 &&
                 ip[0].a.u32 == ip[1].b.u32 &&
                 ip[1].a.u32 == ip[2].b.u32)
        {
            ip[1].a.u32 = ip[2].a.u32;
            setNop(context, ip + 2);
        }
    }

    return true;
}