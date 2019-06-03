#include "pch.h"
#include "ByteCodeRunContext.h"
#include "ByteCode.h"
#include "AstNode.h"

void ByteCodeRunContext::setup(SourceFile* sf, AstNode* nd, uint32_t numRC, uint32_t numRR, uint32_t stackS)
{
    if (numRegistersRC < numRC)
    {
        numRegistersRC = numRC;
        registersRC    = (Register*) realloc(registersRC, numRC * sizeof(Register));
    }

    if (numRegistersRR < numRR)
    {
        numRegistersRR = numRR;
        registersRR    = (Register*) realloc(registersRR, numRR * sizeof(Register));
    }

    if (stackSize < stackS)
    {
        stackSize = stackS;
        stack     = (uint8_t*) realloc(stack, stackS);
    }

    bp = stack + stackSize;
    sp = bp;

    sourceFile = sf;
    node       = nd;
    bc         = node->bc;
    assert(bc);
    ip = bc->out;
    assert(ip);

    hasError = false;
    errorMsg.clear();
}

void ByteCodeRunContext::error(const string& msg)
{
    hasError = true;
    errorMsg = msg;
}