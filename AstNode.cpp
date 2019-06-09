#include "pch.h"
#include "Pool.h"
#include "AstNode.h"

Pool<AstNode>          g_Pool_astNode;
Pool<AstAttrDecl>      g_Pool_astAttrDecl;
Pool<AstVarDecl>       g_Pool_astVarDecl;
Pool<AstFuncDecl>      g_Pool_astFuncDecl;
Pool<AstIdentifier>    g_Pool_astIdentifier;
Pool<AstIdentifierRef> g_Pool_astIdentifierRef;
Pool<AstFuncCallParam> g_Pool_astFuncCallParam;
Pool<AstIf>            g_Pool_astIf;
Pool<AstWhile>         g_Pool_astWhile;