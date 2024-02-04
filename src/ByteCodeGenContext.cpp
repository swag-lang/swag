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

void ByteCodeGenContext::setNoLocation()
{
    noLocation = true;
}

void ByteCodeGenContext::restoreNoLocation()
{
    noLocation = false;
}

void ByteCodeGenContext::pushLocation(SourceLocation* loc)
{
    stackForceLocation.push_back(loc);
    forceLocation = loc;
}

void ByteCodeGenContext::popLocation()
{
    stackForceLocation.pop_back();
    if (!stackForceLocation.empty())
        forceLocation = stackForceLocation.back();
    else
        forceLocation = nullptr;
}

void ByteCodeGenContext::pushNode(AstNode* pnode)
{
    stackForceNode.push_back(pnode);
    forceNode = pnode;
}

void ByteCodeGenContext::popNode()
{
    stackForceNode.pop_back();
    if (!stackForceNode.empty())
        forceNode = stackForceNode.back();
    else
        forceNode = nullptr;
}
