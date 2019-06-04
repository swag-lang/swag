#pragma once
#include "Pool.h"
#include "SyntaxJob.h"
#include "ModuleSemanticJob.h"
#include "FileSemanticJob.h"
#include "ModuleOutputJob.h"
#include "ByteCodeGenJob.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SymTable.h"
#include "Scope.h"
#include "TypeInfo.h"
#include "ConcatBucket.h"
#include "SymTable.h"
#include "ByteCode.h"
#include "AstNode.h"
#include "SourceFile.h"

struct PoolFactory
{
    Pool<SourceFile>   sourceFile;
    Pool<Scope>        scope;
    Pool<ConcatBucket> concatBucket;
    Pool<ByteCode>     byteCode;

    Pool<SyntaxJob>         syntaxJob;
    Pool<ModuleSemanticJob> moduleSemanticJob;
    Pool<FileSemanticJob>   fileSemanticJob;
    Pool<ModuleOutputJob>   moduleOutputJob;
    Pool<SemanticJob>       semanticJob;
    Pool<ByteCodeGenJob>    bytecodeJob;

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
    Pool<TypeInfoEnumValue>     typeInfoEnumValue;
    Pool<TypeInfoFuncAttr>      typeInfoFuncAttr;
    Pool<TypeInfoFuncAttrParam> typeInfoFuncAttrParam;
    Pool<SymbolMatchParameter>  symbolMatchParameter;
};

extern PoolFactory g_PoolFactory;