#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"
#include "AstNode.h"

// This pass tries to reuse registers in factor expressions
// a = getparam(0)
// b = getparam(0)
// c = a * b => c = a * a
bool ByteCodeOptimizer::optimizePassAliasFactor(ByteCodeOptContext* context)
{
    context->map6432.clear();
    context->mapRegReg.clear();

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;

        if (ByteCode::isJump(ip) || ip->flags & BCI_START_STMT)
        {
            context->map6432.clear();
            context->mapRegReg.clear();
        }

        uint32_t curReg = 0;
        uint64_t encode = 0;

        if (ip->op == ByteCodeOp::GetFromStack8 ||
            ip->op == ByteCodeOp::GetFromStack16 ||
            ip->op == ByteCodeOp::GetFromStack32 ||
            ip->op == ByteCodeOp::GetFromStack64 ||
            ip->op == ByteCodeOp::GetParam8 ||
            ip->op == ByteCodeOp::GetParam16 ||
            ip->op == ByteCodeOp::GetParam32 ||
            ip->op == ByteCodeOp::GetParam64)
        {
            curReg = ip->a.u32;
            encode = ((uint64_t) ip->op << 32) | (ip->b.u32);
        }
        else if (!(flags & OPFLAG_IS_FACTOR))
        {
            context->map6432.clear();
            context->mapRegReg.clear();
        }

        if (encode)
        {
            auto it = context->map6432.find(encode);

            // Store the register assigned to the given opcode if not done
            if (it == context->map6432.end())
            {
                // If the same register was already associated with another opcode,
                for (auto& it1 : context->map6432)
                {
                    if (it1.second == curReg)
                    {
                        // Remove the old opcode
                        context->map6432.erase(it1.first);

                        // Remove all aliases to that register
                        for (int i = 0; context->mapRegReg.count != 0 && i < RegisterList::MAX_REGISTERS; i++)
                        {
                            if (context->mapRegReg.here[i] and context->mapRegReg.val[i] == curReg)
                            {
                                context->mapRegReg.remove(i);
                            }
                        }
                        break;
                    }
                }

                context->map6432[encode] = curReg;
            }

            // If we already have the same opcode assigned to another register, then this can be an alias
            else if (it->second != curReg)
                context->mapRegReg.set(curReg, it->second);
        }
        else
        {
            // If we have an alias for a reag register, we change the register to it
            if (flags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
            {
                auto ptr = context->mapRegReg.find(ip->a.u32);
                if (ptr)
                {
                    ip->a.u32                           = *ptr;
                    context->allPassesHaveDoneSomething = true;
                }
            }

            if (flags & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B))
            {
                auto ptr = context->mapRegReg.find(ip->b.u32);
                if (ptr)
                {
                    ip->b.u32                           = *ptr;
                    context->allPassesHaveDoneSomething = true;
                }
            }

            if (flags & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C))
            {
                auto ptr = context->mapRegReg.find(ip->c.u32);
                if (ptr)
                {
                    ip->c.u32                           = *ptr;
                    context->allPassesHaveDoneSomething = true;
                }
            }

            if (flags & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D))
            {
                auto ptr = context->mapRegReg.find(ip->d.u32);
                if (ptr)
                {
                    ip->d.u32                           = *ptr;
                    context->allPassesHaveDoneSomething = true;
                }
            }

            // If we write to a register that has an associated opcode, we remove the opcode
            // We remove the fact that the register has an alias
            if (flags & OPFLAG_WRITE_A && !(ip->flags & BCI_IMM_A))
            {
                for (auto& it : context->map6432)
                {
                    if (it.second == ip->a.u32)
                    {
                        context->map6432.erase(it.first);
                        break;
                    }
                }

                context->mapRegReg.remove(ip->a.u32);
            }

            if (flags & OPFLAG_WRITE_B && !(ip->flags & BCI_IMM_B))
            {
                for (auto& it : context->map6432)
                {
                    if (it.second == ip->b.u32)
                    {
                        context->map6432.erase(it.first);
                        break;
                    }
                }

                context->mapRegReg.remove(ip->b.u32);
            }

            if (flags & OPFLAG_WRITE_C && !(ip->flags & BCI_IMM_C))
            {
                for (auto& it : context->map6432)
                {
                    if (it.second == ip->c.u32)
                    {
                        context->map6432.erase(it.first);
                        break;
                    }
                }

                context->mapRegReg.remove(ip->c.u32);
            }

            if (flags & OPFLAG_WRITE_D && !(ip->flags & BCI_IMM_D))
            {
                for (auto& it : context->map6432)
                {
                    if (it.second == ip->d.u32)
                    {
                        context->map6432.erase(it.first);
                        break;
                    }
                }

                context->mapRegReg.remove(ip->d.u32);
            }
        }
    }

    return true;
}

bool ByteCodeOptimizer::optimizePassAlias(ByteCodeOptContext* context)
{
    SWAG_CHECK(optimizePassAliasFactor(context));
    return true;
}