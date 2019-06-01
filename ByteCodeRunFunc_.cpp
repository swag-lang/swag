#include "pch.h"
#include "Global.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "ConcatBucket.h"
#include "ByteCode.h"

uint8_t* ByteCodeRun::runRet(ByteCodeRunContext* context)
{
    context->epbc--;
    context->bc = context->stack_bc[context->epbc];
    context->ep = context->stack_ep[context->epbc];
    return context->ep;
}

uint8_t* ByteCodeRun::runLocalFuncCall(ByteCodeRunContext* context)
{
    ByteCode* val = *(ByteCode**) context->ep;
    context->bc->out.seek(sizeof(void*));

    context->stack_bc[context->epbc] = context->bc;
    context->stack_ep[context->epbc] = context->bc->out.currentSP;
    context->epbc++;
    context->bc                      = val;
    context->bc->out.rewind();
    context->ep                      = context->bc->out.currentSP;
    return context->ep;
}
