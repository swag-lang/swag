#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "Syntax/AstNode.h"

// If an instruction can have an immediate form, then transform it if the corresponding
// register is a constant.
bool ByteCodeOptimizer::optimizePassImmediate(ByteCodeOptContext* context)
{
    auto& regs = context->mapRegInstA;
    regs.clear();

    auto& regsRW = context->mapRegU64;
    regsRW.clear();

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        // Do not care about jump destination from safety blocks
        if (ip->hasFlag(BCI_START_STMT_N))
            regs.clear();

        auto flags = ByteCode::opFlags(ip->op);

        bool optPostWrite = false;
        switch (ip->op)
        {
            case ByteCodeOp::ClearRA:
                regsRW[ip->a.u32] = 0;
                regs[ip->a.u32]   = ip;
                ip->removeFlag(BCI_IMM_C);
                ip->removeFlag(BCI_IMM_D);
                break;

            case ByteCodeOp::ClearRAx2:
                regsRW[ip->a.u32] = 0;
                regsRW[ip->b.u32] = 0;
                regs[ip->a.u32]   = ip;
                regs[ip->b.u32]   = ip;
                ip->removeFlag(BCI_IMM_C);
                ip->removeFlag(BCI_IMM_D);
                break;

            case ByteCodeOp::ClearRAx3:
                regsRW[ip->a.u32] = 0;
                regsRW[ip->b.u32] = 0;
                regsRW[ip->c.u32] = 0;
                regs[ip->a.u32]   = ip;
                regs[ip->b.u32]   = ip;
                regs[ip->c.u32]   = ip;
                ip->removeFlag(BCI_IMM_C);
                ip->removeFlag(BCI_IMM_D);
                break;

            case ByteCodeOp::ClearRAx4:
                regsRW[ip->a.u32] = 0;
                regsRW[ip->b.u32] = 0;
                regsRW[ip->c.u32] = 0;
                regsRW[ip->d.u32] = 0;
                regs[ip->a.u32]   = ip;
                regs[ip->b.u32]   = ip;
                regs[ip->c.u32]   = ip;
                regs[ip->d.u32]   = ip;
                ip->removeFlag(BCI_IMM_C);
                ip->removeFlag(BCI_IMM_D);
                break;

            case ByteCodeOp::SetImmediate32:
                regsRW[ip->a.u32] = ip->b.u32;
                regs[ip->a.u32]   = ip;
                ip->removeFlag(BCI_IMM_C);
                ip->removeFlag(BCI_IMM_D);
                break;

            case ByteCodeOp::SetImmediate64:
                regsRW[ip->a.u32] = ip->b.u64;
                regs[ip->a.u32]   = ip;
                ip->removeFlag(BCI_IMM_C);
                ip->removeFlag(BCI_IMM_D);
                break;

            default:
                if (ip->hasFlag(BCI_START_STMT))
                    regs.clear();
                if (regs.empty())
                    break;

                optPostWrite = true;

                // Read/write to A, and A is a constant, we store the current value in B. The constant folding pass can take care of that
                // depending on the instruction
                if (!ip->hasFlag(BCI_IMM_B) && flags.has(OPF_READ_A) && flags.has(OPF_WRITE_A) && regs.contains(ip->a.u32) &&
                    !flags.has(OPF_READ_B) && !flags.has(OPF_READ_VAL32_B) && !flags.has(OPF_READ_VAL64_B))
                {
                    context->setDirtyPass();
                    ip->addFlag(BCI_IMM_B);
                    regs.erase(ip->a.u32);
                    ip->b.u64 = regsRW[ip->a.u32];
                    break;
                }

                if (!ip->hasFlag(BCI_IMM_B) && flags.has(OPF_WRITE_A) && flags.has(OPF_READ_B) && regs.contains(ip->a.u32) &&
                    ip->a.u32 == ip->b.u32)
                {
                    context->setDirtyPass();
                    ip->addFlag(BCI_IMM_B);
                    regs.erase(ip->a.u32);
                    ip->b.u64 = regsRW[ip->a.u32];
                    break;
                }

                // Read/write to A, and A is a constant, we store the current value in C. The constant folding pass can take care of that
                // depending on the instruction
                if (!ip->hasFlag(BCI_IMM_C) && flags.has(OPF_READ_A) && flags.has(OPF_WRITE_A) && regs.contains(ip->a.u32) &&
                    !flags.has(OPF_READ_C) && !flags.has(OPF_READ_VAL32_C) && !flags.has(OPF_READ_VAL64_C))
                {
                    context->setDirtyPass();
                    ip->addFlag(BCI_IMM_C);
                    regs.erase(ip->a.u32);
                    ip->c.u64 = regsRW[ip->a.u32];
                    break;
                }

                // Operators can read from A and write to C, with A == C.
                // In that case we want the optim to take place on A if it's immediate, so do not cancel it for C.
                if (flags.has(OPF_READ_A) && flags.has(OPF_WRITE_C) && !ip->hasFlag(BCI_IMM_A) && regs.contains(ip->a.u32))
                    break;

                if (flags.has(OPF_WRITE_A))
                    regs.erase(ip->a.u32);
                if (flags.has(OPF_WRITE_B))
                    regs.erase(ip->b.u32);
                if (flags.has(OPF_WRITE_C))
                    regs.erase(ip->c.u32);
                if (flags.has(OPF_WRITE_D))
                    regs.erase(ip->d.u32);
                break;
        }

        if (regs.empty())
            continue;

        if (!ip->hasFlag(BCI_IMM_A) && flags.has(OPF_READ_A) && regs.contains(ip->a.u32))
        {
            if (flags.has(OPF_IMM_A))
            {
                context->setDirtyPass();
                ip->addFlag(BCI_IMM_A);
                regs.erase(ip->a.u32);
                ip->a.u64 = regsRW[ip->a.u32];
            }
        }

        if (!ip->hasFlag(BCI_IMM_B) && flags.has(OPF_READ_B) && regs.contains(ip->b.u32))
        {
            if (flags.has(OPF_IMM_B))
            {
                context->setDirtyPass();
                ip->addFlag(BCI_IMM_B);
                regs.erase(ip->b.u32);
                ip->b.u64 = regsRW[ip->b.u32];
            }
            else if (ip->op == ByteCodeOp::CopyRBtoRA64)
            {
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                regs.erase(ip->b.u32);
                ip->b.u64 = regsRW[ip->b.u32];
                flags     = ByteCode::opFlags(ip->op);
            }
        }

        if (!ip->hasFlag(BCI_IMM_C) && flags.has(OPF_READ_C) && regs.contains(ip->c.u32))
        {
            if (flags.has(OPF_IMM_C))
            {
                context->setDirtyPass();
                ip->addFlag(BCI_IMM_C);
                regs.erase(ip->c.u32);
                ip->c.u64 = regsRW[ip->c.u32];
            }
        }

        if (!ip->hasFlag(BCI_IMM_D) && flags.has(OPF_READ_D) && regs.contains(ip->d.u32))
        {
            if (flags.has(OPF_IMM_D))
            {
                context->setDirtyPass();
                ip->addFlag(BCI_IMM_D);
                regs.erase(ip->d.u32);
                ip->d.u64 = regsRW[ip->d.u32];
            }
        }

        if (optPostWrite)
        {
            if (flags.has(OPF_WRITE_A))
                regs.erase(ip->a.u32);
            if (flags.has(OPF_WRITE_B))
                regs.erase(ip->b.u32);
            if (flags.has(OPF_WRITE_C))
                regs.erase(ip->c.u32);
            if (flags.has(OPF_WRITE_D))
                regs.erase(ip->d.u32);
        }

        if (flags.has(OPF_READ_A) && !ip->hasFlag(BCI_IMM_A))
            regs.erase(ip->a.u32);
        if (flags.has(OPF_READ_B) && !ip->hasFlag(BCI_IMM_B))
            regs.erase(ip->b.u32);
        if (flags.has(OPF_READ_C) && !ip->hasFlag(BCI_IMM_C))
            regs.erase(ip->c.u32);
        if (flags.has(OPF_READ_D) && !ip->hasFlag(BCI_IMM_D))
            regs.erase(ip->d.u32);
    }

    return true;
}

bool ByteCodeOptimizer::optimizePassImmediate2(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->op == ByteCodeOp::SetImmediate32 || ip->op == ByteCodeOp::SetImmediate64 || ip->op == ByteCodeOp::ClearRA)
        {
            auto ipn = ip + 1;
            while (ipn->op != ByteCodeOp::End)
            {
                if (ipn->hasReadRefToReg(ip->a.u32))
                {
                    bool bad           = false;
                    bool hasJumpInside = false;
                    for (const auto it : context->jumps)
                    {
                        if (it->op == ByteCodeOp::Nop)
                            continue;

                        const auto dest = it + it->b.s32;

                        // If we have a jump from outside to inside, bad
                        if ((it <= ip || it >= ipn) && (dest >= ip && dest <= ipn))
                        {
                            bad = true;
                            break;
                        }

                        // If we have a jump from inside to outside, bad
                        if ((it >= ip && it <= ipn) && (dest <= ip || dest >= ipn))
                        {
                            bad = true;
                            break;
                        }

                        // But we need to have a jump from inside to inside, otherwise normal optim will be enough
                        if (it >= ip && it <= ipn)
                        {
                            hasJumpInside = true;
                        }
                    }

                    if (bad || !hasJumpInside)
                        break;

                    auto flags = ByteCode::opFlags(ipn->op);
                    bool canA  = false;
                    bool canB  = false;
                    bool canC  = false;
                    bool canD  = false;

                    if (ipn->hasReadRefToRegA(ip->a.u32) && flags.has(OPF_IMM_A))
                        canA = true;
                    if (ipn->hasReadRefToRegB(ip->a.u32) && flags.has(OPF_IMM_B))
                        canB = true;
                    if (ipn->hasReadRefToRegC(ip->a.u32) && flags.has(OPF_IMM_C))
                        canC = true;
                    if (ipn->hasReadRefToRegD(ip->a.u32) && flags.has(OPF_IMM_D))
                        canD = true;

                    // To be sure...
                    if (canA + canB + canC + canD > 1)
                        break;

                    if (canA)
                    {
                        ipn->flags.add(BCI_IMM_A);
                        if (ip->op == ByteCodeOp::ClearRA)
                            ipn->a.u64 = 0;
                        else
                            ipn->a.u64 = ip->b.u64;
                        context->setDirtyPass();
                    }
                    else if (canB)
                    {
                        ipn->flags.add(BCI_IMM_B);
                        if (ip->op == ByteCodeOp::ClearRA)
                            ipn->b.u64 = 0;
                        else
                            ipn->b.u64 = ip->b.u64;
                        context->setDirtyPass();
                    }
                    else if (canC)
                    {
                        ipn->flags.add(BCI_IMM_C);
                        if (ip->op == ByteCodeOp::ClearRA)
                            ipn->c.u64 = 0;
                        else
                            ipn->c.u64 = ip->b.u64;
                        context->setDirtyPass();
                    }
                    else if (canD)
                    {
                        ipn->flags.add(BCI_IMM_D);
                        if (ip->op == ByteCodeOp::ClearRA)
                            ipn->d.u64 = 0;
                        else
                            ipn->d.u64 = ip->b.u64;
                        context->setDirtyPass();
                    }

                    break;
                }

                if (ipn->hasRefToReg(ip->a.u32))
                    break;

                ipn++;
            }
        }
    }

    return true;
}
