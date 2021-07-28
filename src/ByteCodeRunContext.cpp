#include "pch.h"
#include "ByteCodeRunContext.h"
#include "ByteCode.h"
#include "AstNode.h"

extern bool g_Exiting;

ByteCodeRunContext::~ByteCodeRunContext()
{
    if (g_Exiting)
        return;

    g_Allocator.free(registersRR, MAX_ALLOC_RR * sizeof(Register));
    g_Allocator.free(stack, g_CommandLine.stackSize);
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
        stack = (uint8_t*) g_Allocator.alloc(g_CommandLine.stackSize);
        if (g_CommandLine.stats)
            g_Stats.memBcStack += g_CommandLine.stackSize;
#ifdef SWAG_DEV_MODE
        memset(stack, 0xFE, g_CommandLine.stackSize);
#endif
    }

    SWAG_ASSERT(node->extension->bc);
    SWAG_ASSERT(node->extension->bc->out);

    sourceFile = sf;
    node       = nd;

    bp = stack + g_CommandLine.stackSize;
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
    errorMsg = Utf8::format("bytecode stack overflow (maximum stack size is '--stack-size:%s')", Utf8::toNiceSize(g_CommandLine.stackSize).c_str());
}