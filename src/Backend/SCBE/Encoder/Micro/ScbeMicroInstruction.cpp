#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"

bool ScbeMicroInstruction::isJump() const
{
    return op == ScbeMicroOp::JumpM || op == ScbeMicroOp::JumpCI || op == ScbeMicroOp::JumpTable || op == ScbeMicroOp::JumpCC;
}

bool ScbeMicroInstruction::isCall() const
{
    return op == ScbeMicroOp::CallExtern || op == ScbeMicroOp::CallIndirect || op == ScbeMicroOp::CallLocal;
}

bool ScbeMicroInstruction::isRet() const
{
    return op == ScbeMicroOp::Ret || op == ScbeMicroOp::Leave;
}