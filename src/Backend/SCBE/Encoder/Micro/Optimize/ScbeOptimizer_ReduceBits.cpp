#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Wmf/SourceFile.h"
#pragma optimize("", off)

void ScbeOptimizer::optimizePassReduceBits(const ScbeMicro& out)
{
    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->op == ScbeMicroOp::LoadZeroExtRR &&
            inst->regA == inst->regB)
        {
            bool       keep  = false;
            bool       extra = false;
            const auto valid = exploreAfter(out, inst, [&keep, &extra](const ScbeMicro& outIn, const ScbeExploreContext& cxtIn) {
                const auto readRegs = outIn.cpu->getReadRegisters(cxtIn.curInst);
                if (readRegs.contains(cxtIn.startInst->regA))
                {
                    const auto opBits = cxtIn.curInst->getOpBitsReadReg();
                    if (opBits != OpBits::Zero && BackendEncoder::getNumBits(opBits) > BackendEncoder::getNumBits(cxtIn.startInst->opBitsA))
                        extra = true;
                    if (opBits != OpBits::Zero && BackendEncoder::getNumBits(opBits) < BackendEncoder::getNumBits(cxtIn.startInst->opBitsB))
                        extra = true;

                    if (opBits == cxtIn.startInst->opBitsA)
                    {
                        keep = true;
                        return ScbeExploreReturn::Stop;
                    }
                }

                const auto regs = outIn.cpu->getWriteRegisters(cxtIn.curInst);
                if (regs.contains(cxtIn.startInst->regA))
                    return ScbeExploreReturn::Break;

                return ScbeExploreReturn::Continue;
            });

            if (valid && !keep && !extra)
            {
                if (!cxt.hasReachedEndOnce || !out.isFuncReturnRegister(inst->regA))
                {
                    ignore(out, inst);
                }
            }
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}
