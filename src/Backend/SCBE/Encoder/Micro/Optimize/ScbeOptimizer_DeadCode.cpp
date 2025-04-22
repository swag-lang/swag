#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"

void ScbeOptimizer::optimizePassDeadCode(const ScbeMicro& out)
{
    for (auto inst = out.getFirstInstruction(); inst->op != ScbeMicroOp::End; inst = ScbeMicro::getNextInstruction(inst))
        inst->flags.remove(MIF_REACHED);

    out.getFirstInstruction()->flags.add(MIF_REACHED);
    const auto valid = exploreAfter(out, out.getFirstInstruction(), [](const ScbeMicro& outIn, const ScbeExploreContext& cxtIn) {
        cxtIn.curInst->flags.add(MIF_REACHED);
        return ScbeExploreReturn::Continue;
    });

    if (!valid)
        return;

    for (auto inst = out.getFirstInstruction(); inst->op != ScbeMicroOp::End; inst = ScbeMicro::getNextInstruction(inst))
    {
        if (!inst->flags.has(MIF_REACHED) &&
            inst->op != ScbeMicroOp::Enter &&
            inst->op != ScbeMicroOp::Leave &&
            inst->op != ScbeMicroOp::Label &&
            inst->op != ScbeMicroOp::PatchJump)
        {
            ignore(out, inst);
        }
    }
}
