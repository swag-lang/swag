#include "pch.h"
#include "Backend/LLVM/Encoder/LlvmEncoder.h"
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"

void LlvmEncoder::init(const BuildParameters& buildParameters)
{
    BackendEncoder::init(buildParameters);
}
