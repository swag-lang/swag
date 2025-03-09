#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"

bool ScbeMicroInstruction::isJump() const
{
    return op == ScbeMicroOp::JumpM || op == ScbeMicroOp::JumpCI || op == ScbeMicroOp::JumpTable || op == ScbeMicroOp::JumpCC;
}
bool ScbeMicroInstruction::isCall() const
{
    return op == ScbeMicroOp::CallExtern || op == ScbeMicroOp::CallIndirect || op == ScbeMicroOp::CallLocal;
}
