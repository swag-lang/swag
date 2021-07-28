#include "pch.h"
#include "Context.h"
#include "ByteCode.h"
#include "Module.h"
#include "Backend.h"
#include "ByteCodeStack.h"

uint64_t                        g_TlsContextId     = 0;
uint64_t                        g_TlsThreadLocalId = 0;
SwagContext                     g_DefaultContext   = {0};
SwagProcessInfos                g_ProcessInfos     = {0};
thread_local ByteCodeRunContext g_RunContext;
static mutex                    g_MakeCallbackMutex;
static uint32_t                 g_MakeCallbackCount = 0;

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

    auto saveNode       = g_RunContext.node;
    auto saveSourceFile = g_RunContext.sourceFile;

    auto node   = bc->node;
    auto module = node->sourceFile->module;
    if (!g_RunContext.stack)
        g_RunContext.setup(node->sourceFile, node);
    else
    {
        g_RunContext.sourceFile = node->sourceFile;
        g_RunContext.node       = node;
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

    auto saveSp      = g_RunContext.sp;
    auto saveFirstRC = g_RunContext.firstRC;

    while (!paramRegisters.empty())
    {
        auto r = paramRegisters.back();
        paramRegisters.pop_back();
        g_RunContext.push(r->pointer);
    }

    // Simulate a LocalCall
    g_RunContext.push(g_RunContext.bp);
    g_RunContext.push(g_RunContext.bc);
    g_RunContext.push(g_RunContext.ip);
    g_RunContext.bc      = bc;
    g_RunContext.ip      = bc->out;
    g_RunContext.bp      = g_RunContext.sp;
    g_RunContext.firstRC = g_RunContext.curRC;
    g_RunContext.bc->enterByteCode(&g_RunContext);

    module->runner.run(&g_RunContext);

    g_RunContext.sp         = saveSp;
    g_RunContext.node       = saveNode;
    g_RunContext.sourceFile = saveSourceFile;
    g_RunContext.firstRC    = saveFirstRC;

    // Get result
    for (int i = 0; i < returnRegisters.size(); i++)
    {
        auto r = returnRegisters[i];
        *r     = g_RunContext.registersRR[i];
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
// g_CallbackArr will be the same !!!

// clang-format off
DECL_CB(a); DECL_CB(b); DECL_CB(c); DECL_CB(d); DECL_CB(e); DECL_CB(f); DECL_CB(g); DECL_CB(h); DECL_CB(i);
DECL_CB(aa); DECL_CB(ab); DECL_CB(ac); DECL_CB(ad); DECL_CB(ae); DECL_CB(af); DECL_CB(ag); DECL_CB(ah); DECL_CB(ai);
DECL_CB(aaa); DECL_CB(aab); DECL_CB(aac); DECL_CB(aad); DECL_CB(aae); DECL_CB(aaf); DECL_CB(aag); DECL_CB(aah); DECL_CB(aai);

static Callback g_CallbackArr[] = {
    USE_CB(a), USE_CB(b), USE_CB(c), USE_CB(d), USE_CB(e), USE_CB(f), USE_CB(g), USE_CB(h), USE_CB(i),
    USE_CB(aa), USE_CB(ab), USE_CB(ac), USE_CB(ad), USE_CB(ae), USE_CB(af), USE_CB(ag), USE_CB(ah), USE_CB(ai),
    USE_CB(aaa), USE_CB(aab), USE_CB(aac), USE_CB(aad), USE_CB(aae), USE_CB(aaf), USE_CB(aag), USE_CB(aah), USE_CB(aai),
};
// clang-format on

// This is the actual callback that will be called by external libraries
static void* doCallback(void* cb, void* p1, void* p2, void* p3, void* p4, void* p5, void* p6)
{
    uint32_t cbIndex = UINT32_MAX;

    // Find the slot that corresponds to the native callback, because
    // we beed the bytecode pointer that matches 'cb'
    {
        unique_lock lk(g_MakeCallbackMutex);
        for (uint32_t i = 0; i < g_MakeCallbackCount; i++)
        {
            if (g_CallbackArr[i].cb == cb)
            {
                cbIndex = i;
                break;
            }
        }
    }

    SWAG_ASSERT(cbIndex != UINT32_MAX);

    void*             result   = nullptr;
    ByteCode*         bc       = (ByteCode*) ByteCode::undoByteCodeLambda(g_CallbackArr[cbIndex].bytecode);
    TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(bc->node->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(typeFunc->numReturnRegisters() <= 1);

    if (typeFunc->numReturnRegisters())
        byteCodeRunCB(g_CallbackArr[cbIndex].bytecode, &result, &p1, &p2, &p3, &p4, &p5, &p6);
    else
        byteCodeRunCB(g_CallbackArr[cbIndex].bytecode, &p1, &p2, &p3, &p4, &p5, &p6);

    return result;
}

// Runtime function called by user code
void* makeCallback(void* lambda)
{
    unique_lock lk(g_MakeCallbackMutex);

    // Search if the lambda pointer has already been associated with a given callback
    for (uint32_t i = 0; i < g_MakeCallbackCount; i++)
    {
        if (g_CallbackArr[i].bytecode == lambda)
            return g_CallbackArr[i].cb;
    }

    // No more room
    if (g_MakeCallbackCount == sizeof(g_CallbackArr) / sizeof(g_CallbackArr[0]))
        return nullptr;

    // Use a new slot
    g_CallbackArr[g_MakeCallbackCount].bytecode = lambda;
    return g_CallbackArr[g_MakeCallbackCount++].cb;
}

void initDefaultContext()
{
    g_TlsContextId     = OS::tlsAlloc();
    g_TlsThreadLocalId = OS::tlsAlloc();
    OS::tlsSetValue(g_TlsContextId, &g_DefaultContext);

    g_ProcessInfos.arguments.buffer = g_CommandLine.userArgumentsSlice.first;
    g_ProcessInfos.arguments.count  = (uint64_t) g_CommandLine.userArgumentsSlice.second;
    g_ProcessInfos.contextTlsId     = g_TlsContextId;
    g_ProcessInfos.defaultContext   = &g_DefaultContext;
    g_ProcessInfos.byteCodeRun      = byteCodeRun;
    g_ProcessInfos.makeCallback     = makeCallback;
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
