#include "pch.h"
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"
#include "Backend/LLVM/Encoder/LlvmEncoder.h"

void LlvmEncoder::init(const BuildParameters& buildParameters)
{
    BackendEncoder::init(buildParameters);
}
