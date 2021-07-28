#pragma once
#include "Os.h"
#include "Runtime.h"
#include "ByteCodeRunContext.h"
union Register;
struct ByteCode;
struct Module;

extern uint64_t                        g_TlsThreadLocalId;
extern uint64_t                        g_TlsContextId;
extern SwagContext                     g_DefaultContext;
extern SwagProcessInfos                g_ProcessInfos;
extern thread_local ByteCodeRunContext g_RunContext;

void     initDefaultContext();
uint64_t getDefaultContextFlags(Module* module);
void*    makeCallback(void* lambda);

struct PushSwagContext
{
    PushSwagContext()
    {
        copy = g_DefaultContext;
        OS::tlsSetValue(g_TlsContextId, &copy);
    }

    ~PushSwagContext()
    {
        OS::tlsSetValue(g_TlsContextId, &g_DefaultContext);
    }

    SwagContext copy;
};