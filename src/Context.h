#pragma once
#include "ByteCodeRunContext.h"
#include "Os.h"
#include "Runtime.h"
union Register;
struct ByteCode;
struct Module;
struct TypeInfoStruct;

extern uint64_t                         g_TlsThreadLocalId;
extern uint64_t                         g_TlsContextId;
extern void*                            g_SystemAllocatorTable;
extern void*                            g_DebugAllocatorTable;
extern SwagContext                      g_DefaultContext;
extern SwagProcessInfos                 g_ProcessInfos;
extern thread_local ByteCodeRunContext  g_RunContextVal;
extern thread_local ByteCodeRunContext* g_RunContext;

void     initDefaultContext();
uint64_t getDefaultContextFlags(Module* module);
FuncCB   makeCallback(void* lambda);

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