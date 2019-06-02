#include "pch.h"
#include "Global.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "ConcatBucket.h"
#include "ByteCode.h"
#include "Log.h"

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
    context->bc = val;
    context->bc->out.rewind();
    context->ep = context->bc->out.currentSP;
    return context->ep;
}

uint8_t* ByteCodeRun::runIntrinsicPrintF64(ByteCodeRunContext* context)
{
    auto val = context->popF64();
    g_Log.lock();
    g_Log.print(to_string(val));
    g_Log.unlock();
    return context->ep;
}

uint8_t* ByteCodeRun::runIntrinsicPrintS64(ByteCodeRunContext* context)
{
    auto val = context->popS64();
    g_Log.lock();
    g_Log.print(to_string(val));
    g_Log.unlock();
    return context->ep;
}

uint8_t* ByteCodeRun::runIntrinsicPrintChar(ByteCodeRunContext* context)
{
    auto val = context->popU32();
    g_Log.lock();
    Utf8 msg;
	msg += (char32_t) val;
    g_Log.print(msg);
    g_Log.unlock();
    return context->ep;
}
