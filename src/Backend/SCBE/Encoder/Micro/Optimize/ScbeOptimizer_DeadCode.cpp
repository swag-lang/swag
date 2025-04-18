#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/SourceFile.h"
#pragma optimize("", off)

void ScbeOptimizer::optimizePassDeadCode(const ScbeMicro& out)
{
    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        inst->flags.remove(MIF_REACHED);
        inst = ScbeMicro::getNextInstruction(inst);
    }

    cxt.startInst = out.getFirstInstruction();
    cxt.startInst->flags.add(MIF_REACHED);
    const auto valid = explore(cxt, out, [](const ScbeMicro& outIn, const ScbeExploreContext& cxtIn) {
        cxtIn.curInst->flags.add(MIF_REACHED);
        return ScbeExploreReturn::Continue;
    });

    if (!valid)
        return;

    inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (!inst->flags.has(MIF_REACHED) &&
            inst->op != ScbeMicroOp::Enter &&
            inst->op != ScbeMicroOp::Leave &&
            inst->op != ScbeMicroOp::Label &&
            inst->op != ScbeMicroOp::PatchJump)
        {
            ignore(out, inst);
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}
