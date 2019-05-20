#pragma once
#include "Pool.h"
#include "SyntaxJob.h"
#include "ModuleJob.h"
#include "Ast.h"

struct PoolFactory
{
    Pool<SourceFile> m_sourceFile;
    Pool<SyntaxJob>  m_syntaxJob;
    Pool<ModuleJob>  m_moduleJob;
    Pool<AstNode>    m_astNode;
    Pool<AstVarDecl> m_astVarDecl;
};