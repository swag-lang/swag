#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "Semantic/Scope.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/AstNode.h"

namespace
{
    void removeOpDrop(ByteCodeOptContext* context, ByteCodeInstruction* ipOrg, const ByteCodeInstruction* ip, uint32_t orgOffset)
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

    void optimRetCopy(ByteCodeOptContext* context, ByteCodeInstruction* ipOrg, ByteCodeInstruction* ip)
    {
        bool sameStackOffset = false;

        SWAG_ASSERT(ipOrg->op == ByteCodeOp::MakeStackPointer);
        const auto orgOffset = ipOrg->b.u32;

        // If the second MakeStackPointer is the same as the first one (the parameter), then the
        // return copy is totally useless, se we must not simulate a drop, because there's in fact
        // only one variable, and not one variable copied to the other
        // This happens when inlining a function that returns a struct
        if (ip->op == ByteCodeOp::MakeStackPointer)
        {
            sameStackOffset = ipOrg->b.u32 == ip->b.u32;

            // Change the original stack pointer offset to reference the variable instead of the temporary
            // copy
            SWAG_ASSERT(ipOrg->op != ByteCodeOp::CopyRRtoRA);
            ipOrg->b.u32 = ip->b.u32;
        }
        else
        {
            SWAG_ASSERT(ip->op == ByteCodeOp::CopyRRtoRA);
            SET_OP(ipOrg, ByteCodeOp::CopyRRtoRA);
            ipOrg->b.u64 = ip->b.u64;
        }

        // Is there a corresponding drop in the scope ?
        const bool hasDrop = !sameStackOffset && ipOrg->node->ownerScope->symTable.structVarsToDrop.count > 0;

        // Remove opDrop to the old variable that is no more affected.
        // Make a nop, and detect all drops of that variable to remove them also.
        // Not sure this will work in all situations
        if (hasDrop)
            removeOpDrop(context, ipOrg, ip, orgOffset);

        // Remove the MakeStackPointer
        // NO ! The register can be necessary for some code after, especially if this is a CopyRRtoRA
        // ByteCodeOptimizer::setNop(context, ip);

        // Remove the memcpy
        ByteCodeOptimizer::setNop(context, ip + 1);

        // We need to remove every instructions related to the post move
        ip += 2;
        while (ip->hasFlag(BCI_POST_COPY_MOVE))
            ByteCodeOptimizer::setNop(context, ip++);
    }
}

void ByteCodeOptimizer::registerParamsReg(ByteCodeOptContext* context, const ByteCodeInstruction* ip)
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
// CopyRAtoRT X
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
            ip[1].op == ByteCodeOp::CopyRAtoRT &&
            ip->a.u32 == ip[1].a.u32)
            startOk = true;

        if (ip->op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::IncPointer64 &&
            ip[2].op == ByteCodeOp::CopyRAtoRT &&
            ip->a.u32 == ip[2].a.u32)
            startOk = true;

        // Detect pushing pointer to the stack for a return value
        if (startOk)
        {
            const auto ipOrg = ip;

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
            while (ip->hasFlag(BCI_TRY_CATCH))
                ip++;

            // This will copy the result in the real variable
            if (ByteCode::isMemCpy(ip + 1) && ip[1].b.u32 == ipOrg->a.u32)
            {
                if (ip->op == ByteCodeOp::MakeStackPointer)
                    optimRetCopy(context, ipOrg, ip);
                else if (ip->op == ByteCodeOp::CopyRRtoRA && ip[1].a.u32 == ip[0].a.u32)
                    optimRetCopy(context, ipOrg, ip);
            }
            else
                ip = ipOrg;
        }
    }

    return true;
}

void ByteCodeOptimizer::registerMakeAddr(ByteCodeOptContext* context, const ByteCodeInstruction* ip)
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
                const auto param = context->bc->typeInfoFunc->registerIdxToType(ip->c.u32);
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
            ip[1].op == ByteCodeOp::CopyRAtoRT &&
            ip->a.u32 == ip[1].a.u32)
            startOk = true;

        if (ip->op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::IncPointer64 &&
            ip[2].op == ByteCodeOp::CopyRAtoRT &&
            ip->a.u32 == ip[2].a.u32 &&
            ip[1].a.u32 == ip[1].c.u32)
            startOk = true;

        // Detect pushing pointer to the stack for a return value
        if (startOk)
        {
            const auto ipOrg     = ip;
            const auto orgOffset = ipOrg->b.u32;

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
            while (ip->hasFlag(BCI_TRY_CATCH))
                ip++;

            // This will copy the result in the real variable
            if (ByteCode::isMemCpy(ip) && ip->b.u32 == ipOrg->a.u32)
            {
                // Pointer aliasing. Do not make the optimization if the wanted result is also a parameter
                // to the call. For example a = toto(a, b).
                bool       ok = true;
                const auto it = context->mapRegReg.find(ip->a.u32);
                if (it)
                {
                    for (const auto it1 : context->vecReg)
                    {
                        const auto it2 = context->mapRegReg.find(it1);
                        if (!it2)
                            continue;
                        if (*it2 == UINT32_MAX || *it == UINT32_MAX || *it2 == *it)
                        {
                            ok = false;
                            ip = ipOrg;
                            break;
                        }
                    }
                }

                if (ok)
                {
                    // Make CopyRAtoRT point to the MemCpy destination register
                    if (ipOrg[1].op == ByteCodeOp::CopyRAtoRT)
                        ipOrg[1].a.u32 = ip->a.u32;
                    else
                    {
                        SWAG_ASSERT(ipOrg[1].op == ByteCodeOp::IncPointer64);
                        ipOrg[1].a.u32 = ip->a.u32;
                        ipOrg[1].c.u64 = ip->a.u32;

                        SWAG_ASSERT(ipOrg[2].op == ByteCodeOp::CopyRAtoRT);
                        ipOrg[2].a.u32 = ip->a.u32;
                    }

                    // Is there a corresponding drop in the scope ?
                    const bool hasDrop = ipOrg->node->ownerScope->symTable.structVarsToDrop.count > 0;

                    // Remove opDrop to the old variable that is no more affected.
                    // Make a nop, and detect all drops of that variable to remove them also.
                    // Not sure this will work in all situations
                    if (hasDrop)
                        removeOpDrop(context, ipOrg, ip, orgOffset);

                    // Remove the original MakeStackPointer
                    setNop(context, ipOrg);

                    // Remove the memcpy
                    setNop(context, ip);
                    // We need to remove every instructions related to the post move
                    ip += 1;
                    while (ip->hasFlag(BCI_POST_COPY_MOVE))
                        setNop(context, ip++);
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

// Optimize the return value when this is a struct
// If we affect the result to a local variable, then remove on unnecessary copy
bool ByteCodeOptimizer::optimizePassRetCopyStructVal(ByteCodeOptContext* context)
{
    if (!(context->contextBcFlags & OCF_HAS_COPY_RTRC))
        return true;

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip[0].op != ByteCodeOp::LocalCall &&
            ip[0].op != ByteCodeOp::ForeignCall &&
            ip[0].op != ByteCodeOp::LambdaCall)
        {
            continue;
        }

        if (ip[1].op == ByteCodeOp::CopyRTtoRA &&
            ip[2].op == ByteCodeOp::SetAtStackPointer64 &&
            ip[3].op == ByteCodeOp::IncSPPostCall &&
            ip[4].op == ByteCodeOp::CopyStack64 &&
            ip[1].a.u32 == ip[2].b.u32 &&
            ip[2].a.u32 == ip[4].b.u32)
        {
            ip[2].a.u32 = ip[4].a.u32;
            setNop(context, ip + 4);
        }
        else if (ip[1].op == ByteCodeOp::CopyRTtoRA &&
                 ip[2].op == ByteCodeOp::SetAtStackPointer64 &&
                 ip[3].op == ByteCodeOp::CopyStack64 &&
                 ip[1].a.u32 == ip[2].b.u32 &&
                 ip[2].a.u32 == ip[3].b.u32)
        {
            ip[2].a.u32 = ip[3].a.u32;
            setNop(context, ip + 3);
        }
    }

    return true;
}
