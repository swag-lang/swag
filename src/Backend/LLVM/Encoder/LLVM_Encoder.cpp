#include "pch.h"
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"
#include "Backend/LLVM/Encoder/LLVM_Encoder.h"

void LLVMEncoder::init(const BuildParameters& buildParameters)
{
    BackendEncoder::init(buildParameters);
}