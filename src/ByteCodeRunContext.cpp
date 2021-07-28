#include "pch.h"
#include "ByteCodeRunContext.h"
#include "ByteCode.h"
#include "AstNode.h"

extern bool g_Exiting;

static mutex g_FreeStackMutex;
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
        unique_lock lk(g_FreeStackMutex);
        *(void**) stack  = g_FirstFreeStack;
        g_FirstFreeStack = stack;
        stack            = nullptr;
    }
}

void ByteCodeRunContext::setup(SourceFile* sf, AstNode* nd)
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
        unique_lock lk(g_FreeStackMutex);
        if (g_FirstFreeStack)
        {
            stack            = (uint8_t*) g_FirstFreeStack;
            g_FirstFreeStack = *(void**) g_FirstFreeStack;
        }
        else
        {
            stack = (uint8_t*) g_Allocator.alloc(g_CommandLine.stackSizeBC);
            if (g_CommandLine.stats)
                g_Stats.memBcStack += g_CommandLine.stackSizeBC;
        }

#ifdef SWAG_DEV_MODE
        memset(stack, 0xFE, g_CommandLine.stackSizeBC);
#endif
    }

    sourceFile = sf;
    node       = nd;

    SWAG_ASSERT(node->extension->bc);
    SWAG_ASSERT(node->extension->bc->out);

    bp = stack + g_CommandLine.stackSizeBC;
    sp = bp;
    bc = node->extension->bc;
    ip = bc->out;

    curRC    = -1;
    firstRC  = -1;
    hasError = false;
    errorLoc = nullptr;
    errorMsg.clear();
}

void ByteCodeRunContext::error(const Utf8& msg, SwagCompilerSourceLocation* loc)
{
    hasError = true;
    errorLoc = loc;
    errorMsg = msg;
}

void ByteCodeRunContext::stackOverflow()
{
    hasError = true;
    errorMsg = Utf8::format("bytecode stack overflow (maximum stack size is '--stack-size:%s')", Utf8::toNiceSize(g_CommandLine.stackSizeBC).c_str());
}