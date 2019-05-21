#pragma once
#include "Pool.h"
#include "SyntaxJob.h"
#include "ModuleSemanticJob.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SymTable.h"

struct PoolFactory
{
    Pool<SourceFile>        sourceFile;
    Pool<SyntaxJob>         syntaxJob;
    Pool<ModuleSemanticJob> moduleJob;
    Pool<SemanticJob>       semanticJob;
    Pool<AstNode>           astNode;
    Pool<AstVarDecl>        astVarDecl;
    Pool<AstScope>          astScope;
    Pool<AstType>           astType;
    Pool<SymbolName>        symName;
    Pool<SymbolOverload>    symOverload;
};