// ReSharper disable CppInconsistentNaming
#pragma once

struct SCBE_Micro;
struct SCBE_MicroInstruction;
enum class SCBE_MicroOp : uint8_t;

struct SCBE_Optimizer
{
    void ignore(SCBE_MicroInstruction* inst);
    void setOp(SCBE_MicroInstruction* inst, SCBE_MicroOp op);

    void optimize(const SCBE_Micro& out);

    bool passHasDoneSomething = false;
};
