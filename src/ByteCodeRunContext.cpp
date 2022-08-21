#include "pch.h"
#include "ByteCodeRunContext.h"
#include "ByteCode.h"
#include "AstNode.h"
#include "ErrorIds.h"
#include "Diagnostic.h"
#include "ByteCodeStack.h"

extern bool g_Exiting;

static Mutex g_FreeStackMutex;
static void* g_FirstFreeStack = nullptr;

ByteCodeRunContext::~ByteCodeRunContext()
{
    if (g_Exiting)
        return;
    releaseStack();
}

void ByteCodeRunContext::releaseStack()
{
    if (stack)
    {
        g_Allocator.free(registersRR, MAX_ALLOC_RR * sizeof(Register));
        registersRR = nullptr;

        // To avoid wasting memory, we recycle bytecode stacks
        ScopedLock lk(g_FreeStackMutex);
        *(void**) stack  = g_FirstFreeStack;
        g_FirstFreeStack = stack;
        stack            = nullptr;
    }
}

void ByteCodeRunContext::setup(SourceFile* sf, AstNode* nd, ByteCode* nodebc)
{
    if (!registersRR)
    {
        registersRR = (Register*) g_Allocator.alloc(MAX_ALLOC_RR * sizeof(Register));
#ifdef SWAG_DEV_MODE
        memset(registersRR, 0xFE, MAX_ALLOC_RR * sizeof(Register));
#endif
    }

    if (!stack)
    {
        // To avoid wasting memory, we recycle bytecode stacks
        ScopedLock lk(g_FreeStackMutex);
        if (g_FirstFreeStack)
        {
            stack            = (uint8_t*) g_FirstFreeStack;
            g_FirstFreeStack = *(void**) g_FirstFreeStack;
        }
        else
        {
            stack = (uint8_t*) g_Allocator.alloc(g_CommandLine->stackSizeBC);
            if (g_CommandLine->stats)
                g_Stats.memBcStack += g_CommandLine->stackSizeBC;
        }
    }

    jc.sourceFile = sf;
    node          = nd;

    bp    = stack + g_CommandLine->stackSizeBC;
    sp    = bp;
    spAlt = stack;
    bc    = nodebc;
    ip    = bc->out;
    SWAG_ASSERT(ip);

    registers.reserve(4096);
    registersRC.reserve(1024);
    registers.count   = 0;
    registersRC.count = 0;
    g_ByteCodeStack.steps.reserve(4096);

    curRC    = -1;
    firstRC  = -1;
    hasError = false;
    errorLoc = nullptr;
    errorMsg.clear();
}

void ByteCodeRunContext::stackOverflow()
{
    raiseError(Fmt(Err(Err0015), Utf8::toNiceSize(g_CommandLine->stackSizeBC).c_str()));
}

void ByteCodeRunContext::raiseError(const char* msg, SwagCompilerSourceLocation* loc)
{
    hasError = true;
    errorLoc = loc;
    errorMsg = msg;
    throw "raise error";
}

int ByteCodeRunContext::getRegCount(int cur)
{
    if (cur >= registersRC.size() - 1)
        return (int) bc->maxReservedRegisterRC;
    return (int) (registersRC[cur + 1] - registersRC[cur]);
}