#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"

void ScbeOptimizer::optimizePassReduceBits(const ScbeMicro& out)
{
    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->op == ScbeMicroOp::LoadZeroExtRR ||
            inst->op == ScbeMicroOp::LoadZeroExtRM)
        {
            bool       keep  = false;
            const auto valid = exploreAfter(out, inst, [&keep](const ScbeMicro& outIn, const ScbeExploreContext& cxtIn) {
                const auto readRegs = outIn.cpu->getReadRegisters(cxtIn.curInst);
                if (readRegs.contains(cxtIn.startInst->regA))
                {
                    const auto opBits = cxtIn.curInst->getOpBitsReadReg();
                    if (BackendEncoder::getNumBits(opBits) > BackendEncoder::getNumBits(cxtIn.startInst->opBitsB))
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

            if (valid && !keep)
            {
                if (!cxt.hasReachedEndOnce || !out.isFuncReturnRegister(inst->regA))
                {
                    switch (inst->op)
                    {
                        case ScbeMicroOp::LoadZeroExtRR:
                            if (inst->regA == inst->regB)
                                ignore(out, inst);
                            break;
                        case ScbeMicroOp::LoadZeroExtRM:
                            if (out.cpu->encodeLoadRegMem(inst->regA, inst->regB, inst->valueA, inst->opBitsB, EMIT_CanEncode) == CpuEncodeResult::Zero)
                            {
                                setOp(out, inst, ScbeMicroOp::LoadRM);
                                inst->opBitsA = inst->opBitsB;
                            }
                            break;
                    }
                }
            }
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}
