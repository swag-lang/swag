#include "pch.h"
#include "Os.h"
#include "Context.h"
#include "Workspace.h"
#include "ByteCodeRun.h"
#include "ByteCode.h"
#include "AstNode.h"
#include "Global.h"

swag_tls_id_t        g_tlsContextIdByteCode = 0;
swag_context_t       g_defaultContextByteCode;
swag_process_infos_t g_processInfos = {0};

static void byteCodeRun(void* byteCodePtr, ...)
{
    ByteCode*         bc       = (ByteCode*) undoByteCodeLambda(byteCodePtr);
    TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(bc->node->typeInfo, TypeInfoKind::FuncAttr);

    vector<Register*> returnRegisters;
    vector<Register*> paramRegisters;
    va_list           valist;

    // Todo, unsupported yet
    SWAG_ASSERT(!typeFunc->numReturnRegisters());

    va_start(valist, byteCodePtr);
    for (int i = 0; i < typeFunc->numReturnRegisters(); i++)
    {
        auto r = va_arg(valist, Register*);
        returnRegisters.push_back(r);
    }

    ByteCodeRunContext runContext;
    auto               node = bc->node;
    runContext.setup(node->sourceFile, node, g_Workspace.runContext.numRegistersRR, g_Workspace.runContext.stackSize);

    // Parameters
    for (int i = 0; i < typeFunc->numParamsRegisters(); i++)
    {
        auto r = va_arg(valist, Register*);
        paramRegisters.push_back(r);
    }

    while (!paramRegisters.empty())
    {
        auto r = paramRegisters.back();
        paramRegisters.pop_back();
        runContext.push(r->pointer);
    }

    // Dummy 24 bytes on the stack necessary before a call
    runContext.push(nullptr);
    runContext.push(nullptr);
    runContext.push(nullptr);

    // Run !
    runContext.bp = runContext.sp;
    bc->enterByteCode(&runContext);
    g_Run.run(&runContext);
}

void initDefaultContext()
{
    g_tlsContextIdByteCode = OS::tlsAlloc();
    OS::tlsSetValue(g_tlsContextIdByteCode, &g_defaultContextByteCode);

    g_processInfos.arguments.addr  = g_CommandLine.userArgumentsSlice.first;
    g_processInfos.arguments.count = (uint64_t) g_CommandLine.userArgumentsSlice.second;
    g_processInfos.contextTlsId    = g_tlsContextIdByteCode;
    g_processInfos.defaultContext  = &g_defaultContextByteCode;
    g_processInfos.byteCodeRun     = byteCodeRun;
}