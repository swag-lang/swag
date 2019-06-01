#include "pch.h"
#include "Global.h"
#include "TypeManager.h"
#include "PoolFactory.h"
#include "SourceFile.h"
#include "ThreadManager.h"

bool SemanticJob::setupFuncDeclParameters(SourceFile* sourceFile, TypeInfoFuncAttr* typeInfo, AstNode* parameters)
{
    if (!parameters)
        return true;

    typeInfo->parameters.reserve(parameters->childs.size());
    for (auto param : parameters->childs)
    {
        auto funcParam      = sourceFile->poolFactory->typeInfoFuncAttrParam.alloc();
        funcParam->name     = param->name;
        funcParam->typeInfo = param->typeInfo;
        typeInfo->parameters.push_back(funcParam);
    }

    return true;
}

bool SemanticJob::resolveFuncDeclParams(SemanticContext* context)
{
    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveFuncDecl(SemanticContext* context)
{
    auto node = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);

    // Now the full fonction has been solved, so we wakeup jobs depending on that
    scoped_lock lk(node->mutex);
    node->flags |= AST_FULL_RESOLVE;
    for (auto job : node->dependentJobs)
        g_ThreadMgr.addJob(job);
    context->result = SemanticResult::Done;

    return true;
}

bool SemanticJob::resolveFuncDeclType(SemanticContext* context)
{
    auto typeNode   = context->node;
    auto sourceFile = context->sourceFile;
    auto funcNode   = CastAst<AstFuncDecl>(typeNode->parent, AstNodeKind::FuncDecl);

    // Return type
    if (!typeNode->childs.empty())
        typeNode->typeInfo = typeNode->childs[0]->typeInfo;
    else
        typeNode->typeInfo = g_TypeMgr.typeInfoVoid;

    // Register symbol with its type
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FunctionAttribute);
    SWAG_CHECK(setupFuncDeclParameters(sourceFile, typeInfo, funcNode->parameters));
    SWAG_CHECK(collectAttributes(context, typeInfo->attributes, context->node->attributes, funcNode, AstNodeKind::FuncDecl));
    typeInfo->returnType = typeNode->typeInfo;
    SWAG_CHECK(typeNode->scope->symTable->addSymbolTypeInfo(context->sourceFile, funcNode, typeInfo, SymbolKind::Function));
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context->sourceFile, funcNode->scope->parentScope, funcNode, SymbolKind::Function));

    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveFuncCallParams(SemanticContext* context)
{
    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveFuncCall(SemanticContext* context)
{
    context->result = SemanticResult::Done;
    return true;
}