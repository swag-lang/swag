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

SCBE_MicroInstruction* SCBE_Optimizer::zap(SCBE_MicroInstruction* inst)
{
    while (inst->op == SCBE_MicroOp::Nop || inst->op == SCBE_MicroOp::Label || inst->op == SCBE_MicroOp::Debug || inst->op == SCBE_MicroOp::Ignore)
        inst++;
    return inst;
}

void SCBE_Optimizer::passReduce(const SCBE_Micro& out)
{
    auto inst = reinterpret_cast<SCBE_MicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != SCBE_MicroOp::End)
    {
        const auto next = zap(inst + 1);

        switch (inst[0].op)
        {
            case SCBE_MicroOp::StoreMR:
                if (next->op == SCBE_MicroOp::LoadRM &&
                    !next->flags.has(MIF_JUMP_DEST) &&
                    inst[0].opBitsA == next->opBitsA &&
                    inst[0].regA == next->regB &&
                    inst[0].valueA == next->valueA)
                {
                    if (inst[0].regB == next->regA)
                    {
                        ignore(next);
                    }
                    else if (inst[0].opBitsA == OpBits::B64)
                    {
                        setOp(next, SCBE_MicroOp::LoadRR);
                        next->regB = inst[0].regB;
                    }
                    break;
                }

                if (inst[0].regA == CPUReg::RSP &&
                    next->op == SCBE_MicroOp::Leave)
                {
                    ignore(inst);
                    break;
                }
                break;
        }

        inst = zap(next);
    }
}

void SCBE_Optimizer::passStoreMR(const SCBE_Micro& out)
{
}

void SCBE_Optimizer::optimize(const SCBE_Micro& out)
{
    if (out.optLevel == BuildCfgBackendOptim::O0)
        return;

    passHasDoneSomething = true;
    while (passHasDoneSomething)
    {
        passHasDoneSomething = false;
        passReduce(out);
        passStoreMR(out);
    }
}
