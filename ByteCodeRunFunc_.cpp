#include "pch.h"
#include "Global.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "ConcatBucket.h"
#include "ByteCode.h"

uint8_t* ByteCodeRun::runRet(ByteCodeRunContext* context)
{
    return context->ep;
}

uint8_t* ByteCodeRun::runLocalFuncCall(ByteCodeRunContext* context)
{
    ByteCode* val = *(ByteCode**) context->ep;
    return context->bc->out.seek(sizeof(void*));
}
