#pragma once
#include "Pool.h"
#include "SyntaxJob.h"
#include "ModuleSemanticJob.h"
#include "SemanticJob.h"
#include "Ast.h"

struct PoolFactory
{
    Pool<SourceFile>  m_sourceFile;
    Pool<SyntaxJob>   m_syntaxJob;
    Pool<ModuleSemanticJob>   m_moduleJob;
    Pool<SemanticJob> m_semanticJob;
    Pool<AstNode>     m_astNode;
    Pool<AstVarDecl>  m_astVarDecl;
    Pool<AstType>     m_astType;
};