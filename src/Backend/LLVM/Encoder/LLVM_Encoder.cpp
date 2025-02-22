#include "pch.h"
#include "Backend/LLVM/Encoder/LLVM_Encoder.h"
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"

void LLVM_Encoder::init(const BuildParameters& buildParameters)
{
    BackendEncoder::init(buildParameters);
}
