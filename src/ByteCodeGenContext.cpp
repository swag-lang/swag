#include "pch.h"
#include "ByteCodeGenContext.h"
#include "Ast.h"

void ByteCodeGenContext::release()
{
    if (allParamsTmp)
    {
        allParamsTmp->childs.clear();
        allParamsTmp->release();
    }
}

void ByteCodeGenContext::allocateTempCallParams()
{
    if (!allParamsTmp)
        allParamsTmp = Ast::newFuncCallParams(node->sourceFile, nullptr);
    allParamsTmp->childs.clear();
}
