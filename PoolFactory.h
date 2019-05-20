#pragma once
#include "Pool.h"
#include "SyntaxJob.h"
#include "Ast.h"

struct PoolFactory
{
    Pool<SourceFile> m_sourceFile;
    Pool<SyntaxJob>  m_syntaxJob;
    Pool<AstNode>    m_astNode;
};