#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"

void ScbeOptimizer::optimizePassMakeVolatile(const ScbeMicro& out)
{
    for (const auto vol : unusedVolatileInteger)
    {
        for (const auto perm : out.cc->nonVolatileRegistersInteger)
        {
            if (!unusedNonVolatileInteger.contains(perm))
            {
                if (regToReg(out, vol, perm))
                    return;
            }
        }
    }

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->hasWriteRegA() &&
            out.cc->nonVolatileRegistersInteger.contains(inst->regA))
        {
            const auto readRegs = out.cpu->getReadRegisters(inst);
            if (!readRegs.contains(inst->regA))
            {
                auto vol = out.cc->volatileRegistersIntegerSet;
                if (out.isFuncReturnRegister(vol.first()))
                    vol.erase(vol.first());

                auto     next = ScbeMicro::getNextInstruction(inst);
                uint32_t used = 0;
                while (true)
                {
                    if (next->isJump() || next->isJumpDest())
                        break;
                    const auto regs = out.cpu->getReadWriteRegisters(next);
                    for (const auto r : regs)
                        vol.erase(r);
                    if (vol.empty())
                        break;
                    if (regs.contains(inst->regA))
                        used++;
                    if (next->op == ScbeMicroOp::Leave)
                    {
                        regToReg(out, vol.first(), inst->regA, static_cast<uint32_t>(inst - out.getFirstInstruction()), static_cast<uint32_t>(next - out.getFirstInstruction()));
                        break;
                    }

                    next = ScbeMicro::getNextInstruction(next);
                }
            }
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}
