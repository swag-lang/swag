#pragma once
#include "Pool.h"
#include "SyntaxJob.h"
#include "ModuleSemanticJob.h"
#include "ModuleOutputJob.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SymTable.h"
#include "Scope.h"
#include "TypeInfo.h"
#include "ConcatBucket.h"
#include "Attribute.h"

struct PoolFactory
{
    Pool<SourceFile>   sourceFile;
    Pool<Scope>        scope;
    Pool<ConcatBucket> concatBucket;
    Pool<Attribute>    attribute;

    Pool<SyntaxJob>         syntaxJob;
    Pool<ModuleSemanticJob> moduleSemanticJob;
    Pool<ModuleOutputJob>   moduleOutputJob;
    Pool<SemanticJob>       semanticJob;

    Pool<AstNode>          astNode;
    Pool<AstVarDecl>       astVarDecl;
    Pool<AstIdentifierRef> astIdentifierRef;
    Pool<AstIdentifier>    astIdentifier;
    Pool<AstFuncDecl>      astFuncDecl;
    Pool<AstAttribute>     astAttribute;

    Pool<SymbolName>     symName;
    Pool<SymbolOverload> symOverload;

    Pool<TypeInfoNamespace> typeInfoNamespace;
    Pool<TypeInfoEnum>      typeInfoEnum;
    Pool<TypeInfoFunc>      typeInfoFunc;
};