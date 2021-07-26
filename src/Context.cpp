#include "pch.h"
#include "Context.h"
#include "ByteCode.h"
#include "Module.h"
#include "Backend.h"
#include "ByteCodeStack.h"

uint64_t                        g_tlsContextId     = 0;
uint64_t                        g_tlsThreadLocalId = 0;
SwagContext                     g_defaultContext   = {0};
SwagProcessInfos                g_processInfos     = {0};
thread_local ByteCodeRunContext g_runContext;

static void byteCodeRun(bool forCallback, void* byteCodePtr, va_list valist)
{
    ByteCode*         bc       = (ByteCode*) ByteCode::undoByteCodeLambda(byteCodePtr);
    TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(bc->node->typeInfo, TypeInfoKind::FuncAttr);

    VectorNative<Register*> returnRegisters;
    VectorNative<Register*> paramRegisters;
    VectorNative<Register>  fakeRegisters;

    for (int i = 0; i < typeFunc->numReturnRegisters(); i++)
    {
        auto r = va_arg(valist, Register*);
        returnRegisters.push_back(r);
    }

    auto saveNode       = g_runContext.node;
    auto saveSourceFile = g_runContext.sourceFile;

    auto node   = bc->node;
    auto module = node->sourceFile->module;
    if (!g_runContext.stack)
        g_runContext.setup(node->sourceFile, node);
    else
    {
        g_runContext.sourceFile = node->sourceFile;
        g_runContext.node       = node;
    }

    // Parameters
    for (int i = 0; i < typeFunc->numParamsRegisters(); i++)
    {
        auto typeParam = typeFunc->registerIdxToType(i - typeFunc->numReturnRegisters());

        // We are called from native code, which must respect the passByValue calling convention when calling
        // a bytecode callback.
        // In that case, we store the value in a fakeRegister, because bytecode execution is all about
        // registers.
        if (!forCallback && Backend::passByValue(typeParam))
        {
            fakeRegisters.reserve(typeFunc->numParamsRegisters());
            fakeRegisters.count++;

            auto& r = fakeRegisters.back();
            r.u64   = va_arg(valist, uint64_t);
            paramRegisters.push_back(&fakeRegisters.back());
        }
        else
        {
            auto r = va_arg(valist, Register*);
            paramRegisters.push_back(r);
        }
    }

    auto saveSp      = g_runContext.sp;
    auto saveFirstRC = g_runContext.firstRC;

    while (!paramRegisters.empty())
    {
        auto r = paramRegisters.back();
        paramRegisters.pop_back();
        g_runContext.push(r->pointer);
    }

    // Simulate a LocalCall
    g_runContext.push(g_runContext.bp);
    g_runContext.push(g_runContext.bc);
    g_runContext.push(g_runContext.ip);
    g_runContext.bc      = bc;
    g_runContext.ip      = bc->out;
    g_runContext.bp      = g_runContext.sp;
    g_runContext.firstRC = g_runContext.curRC;
    g_runContext.bc->enterByteCode(&g_runContext);

    module->runner.run(&g_runContext);

    g_runContext.sp         = saveSp;
    g_runContext.node       = saveNode;
    g_runContext.sourceFile = saveSourceFile;
    g_runContext.firstRC    = saveFirstRC;

    // Get result
    for (int i = 0; i < returnRegisters.size(); i++)
    {
        auto r = returnRegisters[i];
        *r     = g_runContext.registersRR[i];
    }
}

static void byteCodeRun(void* byteCodePtr, ...)
{
    va_list valist;
    va_start(valist, byteCodePtr);
    g_byteCodeStack.push({nullptr, nullptr});
    byteCodeRun(false, byteCodePtr, valist);
    g_byteCodeStack.pop();
}

static void byteCodeRunCB(void* byteCodePtr, ...)
{
    va_list valist;
    va_start(valist, byteCodePtr);
    g_byteCodeStack.push({nullptr, nullptr});
    byteCodeRun(true, byteCodePtr, valist);
    g_byteCodeStack.pop();
}

// Callback stuff. This is tricky !
// The problem: we want an external library to be able to call a callback defined in swag.
// When swag code is native, no problem.
// The problem starts when that callback is bytecode, at compile time.
// We need a way to associate a native function that will be called by the external library to a bytecode.
//
// The only way i have found : make the native function generic (always the same parameters), and attach
// the address of that function to bytecode. One native callback for one specific bytecode.
// That's why we have here lots of native identical functions, which are dynamically associated to a bytecode
// when calling @mkcallback

static void* doCallback(void* cb, void* p1, void* p2, void* p3, void* p4, void* p5, void* p6);

struct Callback
{
    void* bytecode;
    void* cb;
};

#define DECL_CB(__idx)                                                                         \
    static void* __callback##__idx(void* p1, void* p2, void* p3, void* p4, void* p5, void* p6) \
    {                                                                                          \
        return doCallback(&__callback##__idx, p1, p2, p3, p4, p5, p6);                         \
    }
#define USE_CB(__idx)              \
    {                              \
        nullptr, __callback##__idx \
    }

// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
//
// If /OPT:ICF is enabled in the linker (COMDAT FOLDING), this will not work
// as the linker will make only one __callback function, and all pointers in
// g_callbackArr will be the same !!!

// clang-format off
DECL_CB(a); DECL_CB(b); DECL_CB(c); DECL_CB(d); DECL_CB(e); DECL_CB(f); DECL_CB(g); DECL_CB(h); DECL_CB(i);
DECL_CB(aa); DECL_CB(ab); DECL_CB(ac); DECL_CB(ad); DECL_CB(ae); DECL_CB(af); DECL_CB(ag); DECL_CB(ah); DECL_CB(ai);
DECL_CB(aaa); DECL_CB(aab); DECL_CB(aac); DECL_CB(aad); DECL_CB(aae); DECL_CB(aaf); DECL_CB(aag); DECL_CB(aah); DECL_CB(aai);

static Callback g_callbackArr[] = {
    USE_CB(a), USE_CB(b), USE_CB(c), USE_CB(d), USE_CB(e), USE_CB(f), USE_CB(g), USE_CB(h), USE_CB(i),
    USE_CB(aa), USE_CB(ab), USE_CB(ac), USE_CB(ad), USE_CB(ae), USE_CB(af), USE_CB(ag), USE_CB(ah), USE_CB(ai),
    USE_CB(aaa), USE_CB(aab), USE_CB(aac), USE_CB(aad), USE_CB(aae), USE_CB(aaf), USE_CB(aag), USE_CB(aah), USE_CB(aai),
};
// clang-format on

static mutex    g_makeCallbackMutex;
static uint32_t g_makeCallbackCount = 0;

// This is the actual callback that will be called by external libraries
static void* doCallback(void* cb, void* p1, void* p2, void* p3, void* p4, void* p5, void* p6)
{
    uint32_t cbIndex = UINT32_MAX;

    // Find the slot that corresponds to the native callback, because
    // we beed the bytecode pointer that matches 'cb'
    {
        unique_lock lk(g_makeCallbackMutex);
        for (uint32_t i = 0; i < g_makeCallbackCount; i++)
        {
            if (g_callbackArr[i].cb == cb)
            {
                cbIndex = i;
                break;
            }
        }
    }

    SWAG_ASSERT(cbIndex != UINT32_MAX);

    void*             result   = nullptr;
    ByteCode*         bc       = (ByteCode*) ByteCode::undoByteCodeLambda(g_callbackArr[cbIndex].bytecode);
    TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(bc->node->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(typeFunc->numReturnRegisters() <= 1);

    if (typeFunc->numReturnRegisters())
        byteCodeRunCB(g_callbackArr[cbIndex].bytecode, &result, &p1, &p2, &p3, &p4, &p5, &p6);
    else
        byteCodeRunCB(g_callbackArr[cbIndex].bytecode, &p1, &p2, &p3, &p4, &p5, &p6);

    return result;
}

// Runtime function called by user code
void* makeCallback(void* lambda)
{
    unique_lock lk(g_makeCallbackMutex);

    // Search if the lambda pointer has already been associated with a given callback
    for (uint32_t i = 0; i < g_makeCallbackCount; i++)
    {
        if (g_callbackArr[i].bytecode == lambda)
            return g_callbackArr[i].cb;
    }

    // No more room
    if (g_makeCallbackCount == sizeof(g_callbackArr) / sizeof(g_callbackArr[0]))
        return nullptr;

    // Use a new slot
    g_callbackArr[g_makeCallbackCount].bytecode = lambda;
    return g_callbackArr[g_makeCallbackCount++].cb;
}

void initDefaultContext()
{
    g_tlsContextId     = OS::tlsAlloc();
    g_tlsThreadLocalId = OS::tlsAlloc();
    OS::tlsSetValue(g_tlsContextId, &g_defaultContext);

    g_processInfos.arguments.buffer = g_CommandLine.userArgumentsSlice.first;
    g_processInfos.arguments.count  = (uint64_t) g_CommandLine.userArgumentsSlice.second;
    g_processInfos.contextTlsId     = g_tlsContextId;
    g_processInfos.defaultContext   = &g_defaultContext;
    g_processInfos.byteCodeRun      = byteCodeRun;
    g_processInfos.makeCallback     = makeCallback;
}

uint64_t getDefaultContextFlags(Module* module)
{
    uint64_t flags = 0;
    if (module->kind == ModuleKind::Test)
        flags |= (uint64_t) ContextFlags::Test;
#ifdef SWAG_DEV_MODE
    flags |= (uint64_t) ContextFlags::DevMode;
#endif
    return flags;
}
