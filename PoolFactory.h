#pragma once
#include "Pool.h"
#include "SyntaxJob.h"
#include "ModuleSemanticJob.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SymTable.h"
#include "Scope.h"

struct PoolFactory
{
    Pool<SourceFile>        sourceFile;
    Pool<SyntaxJob>         syntaxJob;
    Pool<ModuleSemanticJob> moduleJob;
    Pool<SemanticJob>       semanticJob;
    Pool<AstNode>           astNode;
    Pool<AstVarDecl>        astVarDecl;
    Pool<Scope>             scope;
    Pool<AstIdentifierRef>  astIdentifierRef;
    Pool<AstIdentifier>     astIdentifier;
    Pool<SymbolName>        symName;
    Pool<SymbolOverload>    symOverload;
};