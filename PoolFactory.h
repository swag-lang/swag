#pragma once
#include "Pool.h"
#include "SyntaxJob.h"
#include "ModuleSemanticJob.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SymTable.h"
#include "Scope.h"
#include "TypeInfo.h"

struct PoolFactory
{
    Pool<SourceFile> sourceFile;
    Pool<Scope>      scope;

    Pool<SyntaxJob>         syntaxJob;
    Pool<ModuleSemanticJob> moduleJob;
    Pool<SemanticJob>       semanticJob;

    Pool<AstNode>          astNode;
    Pool<AstVarDecl>       astVarDecl;
    Pool<AstIdentifierRef> astIdentifierRef;
    Pool<AstIdentifier>    astIdentifier;

    Pool<SymbolName>     symName;
    Pool<SymbolOverload> symOverload;

    Pool<TypeInfoNamespace> typeInfoNamespace;
};