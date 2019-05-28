#include "pch.h"
#include "Global.h"
#include "ByteCodeGen.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "ConcatBucket.h"

uint8_t* ByteCodeRun::runPushS32(ByteCodeRunContext* context)
{
    int32_t val = *(int32_t*) context->ep;
	context->push(val);
    return context->bc.out.seek(4);
}
