#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "AstNode.h"
#include "SourceFile.h"
#include "ByteCode.h"

void ByteCodeOptimizer::optimizePassLea(ByteCodeOptContext* context)
{
    /*struct toRep
    {
        ByteCodeInstruction* ip;
        uint32_t             replaceReg;
    };

    map<uint32_t, toRep> segType;
    map<uint32_t, toRep> segBss;
    map<uint32_t, toRep> segData;
    map<uint32_t, toRep> segCst;
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ByteCode::isJump(ip) || ip->flags & BCI_START_STMT)
        {
            segType.clear();
        }

        if (ip->flags & BCI_CSTDONE)
            continue;

        if (ip->op == ByteCodeOp::MakeMutableSegPointer)
        {
            ip->flags |= BCI_CSTDONE;

            auto it = segData.find(ip->b.u32);
            if (it == segData.end())
                segData[ip->b.u32] = {ip, UINT32_MAX};
            else
            {
                if (it->second.replaceReg == UINT32_MAX)
                {
                    it->second.replaceReg = context->bc->maxReservedRegisterRC++;
                    replaceRegister(context, it->second.ip, it->second.ip->a.u32, it->second.replaceReg);
                }

                replaceRegister(context, ip, ip->a.u32, it->second.replaceReg);
                setNop(context, ip);
            }
        }

        if (ip->op == ByteCodeOp::MakeBssSegPointer)
        {
            ip->flags |= BCI_CSTDONE;

            auto it = segBss.find(ip->b.u32);
            if (it == segBss.end())
                segBss[ip->b.u32] = {ip, UINT32_MAX};
            else
            {
                if (it->second.replaceReg == UINT32_MAX)
                {
                    it->second.replaceReg = context->bc->maxReservedRegisterRC++;
                    replaceRegister(context, it->second.ip, it->second.ip->a.u32, it->second.replaceReg);
                }

                replaceRegister(context, ip, ip->a.u32, it->second.replaceReg);
                setNop(context, ip);
            }
        }

        if (ip->op == ByteCodeOp::MakeTypeSegPointer)
        {
            ip->flags |= BCI_CSTDONE;

            auto it = segType.find(ip->b.u32);
            if (it == segType.end())
                segType[ip->b.u32] = {ip, UINT32_MAX};
            else
            {
                if (it->second.replaceReg == UINT32_MAX)
                {
                    it->second.replaceReg = context->bc->maxReservedRegisterRC++;
                    replaceRegister(context, it->second.ip, it->second.ip->a.u32, it->second.replaceReg);
                }

                replaceRegister(context, ip, ip->a.u32, it->second.replaceReg);
                setNop(context, ip);
            }
        }

        if (ip->op == ByteCodeOp::MakeConstantSegPointer)
        {
            ip->flags |= BCI_CSTDONE;

            auto it = segCst.find(ip->b.u32);
            if (it == segCst.end())
                segCst[ip->b.u32] = {ip, UINT32_MAX};
            else
            {
                if (it->second.replaceReg == UINT32_MAX)
                {
                    it->second.replaceReg = context->bc->maxReservedRegisterRC++;
                    replaceRegister(context, it->second.ip, it->second.ip->a.u32, it->second.replaceReg);
                }

                replaceRegister(context, ip, ip->a.u32, it->second.replaceReg);
                setNop(context, ip);
            }
        }
    }*/
}