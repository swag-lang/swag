#include "pch.h"
#include "Global.h"
#include "ByteCodeGen.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "ConcatBucket.h"

uint8_t* ByteCodeRun::runBinOpPlusS32(ByteCodeRunContext* context)
{
    auto val1 = context->popS32();
    auto val2 = context->popS32();
    context->push(val2 + val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpPlusS64(ByteCodeRunContext* context)
{
    auto val1 = context->popS64();
    auto val2 = context->popS64();
    context->push(val2 + val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpPlusU32(ByteCodeRunContext* context)
{
    auto val1 = context->popU32();
    auto val2 = context->popU32();
    context->push(val2 + val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpPlusU64(ByteCodeRunContext* context)
{
    auto val1 = context->popU64();
    auto val2 = context->popU64();
    context->push(val2 + val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpPlusF32(ByteCodeRunContext* context)
{
    auto val1 = context->popF32();
    auto val2 = context->popF32();
    context->push(val2 + val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpPlusF64(ByteCodeRunContext* context)
{
    auto val1 = context->popF64();
    auto val2 = context->popF64();
    context->push(val2 + val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMinusS32(ByteCodeRunContext* context)
{
    auto val1 = context->popS32();
    auto val2 = context->popS32();
    context->push(val2 - val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMinusS64(ByteCodeRunContext* context)
{
    auto val1 = context->popS64();
    auto val2 = context->popS64();
    context->push(val2 - val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMinusU32(ByteCodeRunContext* context)
{
    auto val1 = context->popU32();
    auto val2 = context->popU32();
    context->push(val2 - val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMinusU64(ByteCodeRunContext* context)
{
    auto val1 = context->popU64();
    auto val2 = context->popU64();
    context->push(val2 - val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMinusF32(ByteCodeRunContext* context)
{
    auto val1 = context->popF32();
    auto val2 = context->popF32();
    context->push(val2 - val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMinusF64(ByteCodeRunContext* context)
{
    auto val1 = context->popF64();
    auto val2 = context->popF64();
    context->push(val2 - val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMulS32(ByteCodeRunContext* context)
{
    auto val1 = context->popS32();
    auto val2 = context->popS32();
    context->push(val2 * val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMulS64(ByteCodeRunContext* context)
{
    auto val1 = context->popS64();
    auto val2 = context->popS64();
    context->push(val2 * val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMulU32(ByteCodeRunContext* context)
{
    auto val1 = context->popU32();
    auto val2 = context->popU32();
    context->push(val2 * val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMulU64(ByteCodeRunContext* context)
{
    auto val1 = context->popU64();
    auto val2 = context->popU64();
    context->push(val2 * val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMulF32(ByteCodeRunContext* context)
{
    auto val1 = context->popF32();
    auto val2 = context->popF32();
    context->push(val2 * val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpMulF64(ByteCodeRunContext* context)
{
    auto val1 = context->popF64();
    auto val2 = context->popF64();
    context->push(val2 * val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpDivF32(ByteCodeRunContext* context)
{
    auto val1 = context->popF32();
    auto val2 = context->popF32();
    if (val1 == 0.0f)
    {
        context->error("division by zero");
        return context->ep;
    }

    context->push(val2 / val1);
    return context->ep;
}

uint8_t* ByteCodeRun::runBinOpDivF64(ByteCodeRunContext* context)
{
    auto val1 = context->popF64();
    auto val2 = context->popF64();
    if (val1 == 0.0f)
    {
        context->error("division by zero");
        return context->ep;
    }

    context->push(val2 / val1);
    return context->ep;
}
