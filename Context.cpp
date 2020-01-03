#include "pch.h"
#include "Os.h"
#include "Context.h"
#include "Workspace.h"
#include "ByteCodeRun.h"
#include "ByteCode.h"
#include "AstNode.h"
#include "Global.h"

swag_tls_id_t        g_tlsContextIdByteCode = 0;
swag_tls_id_t        g_tlsContextIdBackend  = 0;
Context              g_defaultContextByteCode;
swag_context_t       g_defaultContextBackend;
swag_process_infos_t g_processInfos = {0};

static void defaultAllocator(Register* r0, Register* r1)
{
    Context* context = (Context*) OS::tlsGetValue(g_tlsContextIdByteCode);
    SWAG_ASSERT(context->allocator.itable);
    auto bc = (ByteCode*) undoByteCodeLambda(((void**) context->allocator.itable)[0]);

    ByteCodeRunContext runContext;
    auto               node = bc->node;
    runContext.setup(node->sourceFile, node, g_Workspace.runContext.numRegistersRR, g_Workspace.runContext.stackSize);

    // Push AllocatorRequest pointer
    runContext.push(r1->pointer);

    // Push the 'self' from the interface
    runContext.push(r0->pointer);

    // Dummy 24 bytes on the stack necessary before a call
    runContext.push(nullptr);
    runContext.push(nullptr);
    runContext.push(nullptr);

    runContext.bp = runContext.sp;
    bc->enterByteCode(&runContext);
    g_Run.run(&runContext);
}

void initDefaultContext()
{
    g_tlsContextIdByteCode = OS::tlsAlloc();
    OS::tlsSetValue(g_tlsContextIdByteCode, &g_defaultContextByteCode);
    g_tlsContextIdBackend = OS::tlsAlloc();
    OS::tlsSetValue(g_tlsContextIdBackend, &g_defaultContextBackend);

    g_processInfos.arguments.addr            = g_CommandLine.userArgumentsSlice.first;
    g_processInfos.arguments.count           = (uint64_t) g_CommandLine.userArgumentsSlice.second;
    g_processInfos.contextTlsId              = g_tlsContextIdBackend;
    g_processInfos.defaultContext            = &g_defaultContextBackend;
    g_defaultContextBackend.allocator.data   = nullptr;
    static auto itable                       = &defaultAllocator;
    g_defaultContextBackend.allocator.itable = &itable;
}