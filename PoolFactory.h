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

struct PoolFactory
{
    Pool<SourceFile>   sourceFile;
    Pool<Scope>        scope;
    Pool<ConcatBucket> concatBucket;

    Pool<SyntaxJob>         syntaxJob;
    Pool<ModuleSemanticJob> moduleSemanticJob;
    Pool<ModuleOutputJob>   moduleOutputJob;
    Pool<SemanticJob>       semanticJob;

    Pool<AstNode>          astNode;
    Pool<AstVarDecl>       astVarDecl;
    Pool<AstIdentifierRef> astIdentifierRef;
    Pool<AstIdentifier>    astIdentifier;
    Pool<AstFuncDecl>      astFuncDecl;
    Pool<AstAttrDecl>      astAttrDecl;

    Pool<SymbolName>     symName;
    Pool<SymbolOverload> symOverload;

    Pool<TypeInfoNamespace>     typeInfoNamespace;
    Pool<TypeInfoEnum>          typeInfoEnum;
    Pool<TypeInfoFuncAttr>      typeInfoFuncAttr;
    Pool<TypeInfoFuncAttrParam> typeInfoFuncAttrParam;
};