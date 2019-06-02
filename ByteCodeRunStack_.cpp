#include "pch.h"
#include "Global.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "ConcatBucket.h"
#include "ByteCode.h"

uint8_t* ByteCodeRun::runPushBool(ByteCodeRunContext* context)
{
    bool val = *(bool*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(bool));
}

uint8_t* ByteCodeRun::runPushS8(ByteCodeRunContext* context)
{
    int8_t val = *(int8_t*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(int8_t));
}

uint8_t* ByteCodeRun::runPushS16(ByteCodeRunContext* context)
{
    int16_t val = *(int16_t*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(int16_t));
}

uint8_t* ByteCodeRun::runPushS32(ByteCodeRunContext* context)
{
    int32_t val = *(int32_t*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(int32_t));
}

uint8_t* ByteCodeRun::runPushS64(ByteCodeRunContext* context)
{
    int64_t val = *(int64_t*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(int64_t));
}

uint8_t* ByteCodeRun::runPushU8(ByteCodeRunContext* context)
{
    uint8_t val = *(uint8_t*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(uint8_t));
}

uint8_t* ByteCodeRun::runPushU16(ByteCodeRunContext* context)
{
    uint16_t val = *(uint16_t*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(uint16_t));
}

uint8_t* ByteCodeRun::runPushU32(ByteCodeRunContext* context)
{
    uint32_t val = *(uint32_t*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(uint32_t));
}

uint8_t* ByteCodeRun::runPushU64(ByteCodeRunContext* context)
{
    uint64_t val = *(uint64_t*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(uint64_t));
}

uint8_t* ByteCodeRun::runPushF32(ByteCodeRunContext* context)
{
    float val = *(float*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(float));
}

uint8_t* ByteCodeRun::runPushF64(ByteCodeRunContext* context)
{
    double val = *(double*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(double));
}

uint8_t* ByteCodeRun::runPushString(ByteCodeRunContext* context)
{
    uint32_t val = *(uint32_t*) context->ep;
    context->push(val);
    return context->bc->out.seek(sizeof(uint32_t));
}
