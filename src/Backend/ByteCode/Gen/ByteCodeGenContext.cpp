#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Syntax/Ast.h"

void ByteCodeGenContext::release() const
{
    if (allParamsTmp)
    {
        allParamsTmp->children.clear();
        allParamsTmp->release();
    }
}

void ByteCodeGenContext::allocateTempCallParams()
{
    if (!allParamsTmp)
        allParamsTmp = Ast::newFuncCallParams(nullptr, nullptr);
    allParamsTmp->token.sourceFile = node->token.sourceFile;
    allParamsTmp->children.clear();
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

void ByteCodeGenContext::pushNode(AstNode* myNode)
{
    stackForceNode.push_back(myNode);
    forceNode = myNode;
}

void ByteCodeGenContext::popNode()
{
    stackForceNode.pop_back();
    if (!stackForceNode.empty())
        forceNode = stackForceNode.back();
    else
        forceNode = nullptr;
}
