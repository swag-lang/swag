#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/SCBE_Optimizer.h"
#include "Backend/SCBE/Encoder/Micro/SCBE_Micro.h"
#include "Main/Statistics.h"

void SCBE_Optimizer::ignore(SCBE_MicroInstruction* inst)
{
#ifdef SWAG_STATS
    g_Stats.totalOptimScbe += 1;
#endif
    inst->op             = SCBE_MicroOp::Ignore;
    passHasDoneSomething = true;
}

void SCBE_Optimizer::setOp(SCBE_MicroInstruction* inst, SCBE_MicroOp op)
{
    inst->op             = op;
    passHasDoneSomething = true;
}

void SCBE_Optimizer::optimize(const SCBE_Micro& out)
{
    if (out.optLevel == BuildCfgBackendOptim::O0)
        return;

    passHasDoneSomething = true;
    while (passHasDoneSomething)
    {
        passHasDoneSomething = false;

        const auto num  = out.concat.totalCount() / sizeof(SCBE_MicroInstruction);
        auto       inst = reinterpret_cast<SCBE_MicroInstruction*>(out.concat.firstBucket->data);
        for (uint32_t i = 0; i < num; i++, inst++)
        {
            if (inst->op == SCBE_MicroOp::Nop || inst->op == SCBE_MicroOp::Label || inst->op == SCBE_MicroOp::Debug || inst->op == SCBE_MicroOp::Ignore)
                continue;

            auto next = inst + 1;
            while (next->op == SCBE_MicroOp::Nop || next->op == SCBE_MicroOp::Label || next->op == SCBE_MicroOp::Debug || next->op == SCBE_MicroOp::Ignore)
                next++;

            if (inst[0].op == SCBE_MicroOp::StoreMR &&
                next->op == SCBE_MicroOp::LoadRM &&
                !next->flags.has(MIF_JUMP_DEST) &&
                inst[0].opBitsA == next->opBitsA &&
                inst[0].regA == next->regB &&
                inst[0].regB == next->regA &&
                inst[0].valueA == next->valueA)
            {
                ignore(next);
            }

            if (inst[0].op == SCBE_MicroOp::StoreMR &&
                next->op == SCBE_MicroOp::LoadRM &&
                !next->flags.has(MIF_JUMP_DEST) &&
                inst[0].opBitsA == next->opBitsA &&
                inst[0].regA == next->regB &&
                inst[0].valueA == next->valueA)
            {
                if (inst[0].opBitsA == OpBits::B64)
                {
                    setOp(next, SCBE_MicroOp::LoadRR);
                    next->regB = inst[0].regB;
                }
            }

            if (inst[0].op == SCBE_MicroOp::StoreMR &&
                inst[0].regA == CPUReg::RSP &&
                next->op == SCBE_MicroOp::Leave)
            {
                ignore(inst);
            }
        }
    }
}
