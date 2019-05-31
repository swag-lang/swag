#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Scope.h"
#include "TypeManager.h"
#include "ModuleSemanticJob.h"
#include "Diagnostic.h"

bool SemanticJob::resolveFuncDeclParams(SemanticContext* context)
{
    auto node       = context->node;
    node->typeInfo  = g_TypeMgr.typeInfoVoid;
    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveFuncDecl(SemanticContext* context)
{
    if (context->node->attributes)
    {
        auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(context->node->typeInfo, TypeInfoKind::FunctionAttribute);
        collectAttributes(context, typeInfo->attributes, context->node->attributes, AstNodeKind::FuncDecl);
    }

    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveFuncDeclType(SemanticContext* context)
{
    auto typeNode = context->node;
    auto funcNode = CastAst<AstFuncDecl>(typeNode->parent, AstNodeKind::FuncDecl);
    if (!typeNode->childs.empty())
        typeNode->typeInfo = typeNode->childs[0]->typeInfo;
    else
        typeNode->typeInfo = g_TypeMgr.typeInfoVoid;

    // Register symbol with its type
    auto typeInfo        = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FunctionAttribute);
    typeInfo->returnType = typeNode->typeInfo;
    SWAG_CHECK(typeNode->scope->symTable->addSymbolTypeInfo(context->sourceFile, funcNode->token, funcNode->name, typeInfo, SymbolKind::Function));

    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveFuncCallParams(SemanticContext* context)
{
    auto node       = context->node;
    node->typeInfo  = g_TypeMgr.typeInfoVoid;
    context->result = SemanticResult::Done;
    return true;
}