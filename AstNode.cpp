#include "pch.h"
#include "Pool.h"
#include "AstNode.h"

Pool<AstNode>     g_Pool_astNode;
Pool<AstAttrDecl> g_Pool_astAttrDecl;
Pool<AstVarDecl>  g_Pool_astVarDecl;
Pool<AstFuncDecl> g_Pool_astFuncDecl;
