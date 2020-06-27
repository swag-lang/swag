#include "pch.h"
#include "ByteCodeRunContext.h"
#include "ByteCode.h"
#include "AstNode.h"

void ByteCodeRunContext::setup(SourceFile* sf, AstNode* nd, uint32_t numRR, uint32_t stackS)
{
    if (numRegistersRR < numRR)
    {
        numRegistersRR = numRR;
        registersRR    = (Register*) realloc(registersRR, numRR * sizeof(Register));
        if (g_CommandLine.devMode)
            memset(registersRR, 0xFE, numRR * sizeof(Register));
    }

    if (stackSize < stackS)
    {
        stackSize = stackS;
        stack     = (uint8_t*) realloc(stack, stackS);
        if (g_CommandLine.devMode)
            memset(stack, 0xFE, stackS);
    }

    bp = stack + stackSize;
    sp = bp;

    sourceFile = sf;
    node       = nd;
    bc         = node->bc;
    SWAG_ASSERT(bc);
    ip = bc->out;
    SWAG_ASSERT(ip);

    hasError = false;
    errorMsg.clear();
}

void ByteCodeRunContext::error(const Utf8& msg)
{
    hasError = true;
    errorMsg = msg;
}