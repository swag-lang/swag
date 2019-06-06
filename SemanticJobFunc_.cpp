#include "pch.h"
#include "Global.h"
#include "TypeManager.h"
#include "SourceFile.h"
#include "ThreadManager.h"
#include "Diagnostic.h"
#include "SemanticJob.h"
#include "TypeInfo.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "SymTable.h"
#include "Scope.h"

bool SemanticJob::setupFuncDeclParameters(SourceFile* sourceFile, TypeInfoFuncAttr* typeInfo, AstNode* parameters)
{
    if (!parameters)
        return true;

    typeInfo->parameters.reserve(parameters->childs.size());
    for (auto param : parameters->childs)
    {
        auto funcParam      = g_Pool_typeInfoFuncAttrParam.alloc();
        funcParam->name     = param->name;
        funcParam->typeInfo = param->typeInfo;
        typeInfo->parameters.push_back(funcParam);
    }

    return true;
}

bool SemanticJob::resolveFuncDeclParams(SemanticContext* context)
{
    return true;
}

bool SemanticJob::resolveFuncDecl(SemanticContext* context)
{
    auto sourceFile   = context->sourceFile;
    auto node         = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    node->byteCodeFct = &ByteCodeGenJob::emitLocalFuncDecl;

    // Do we have a return value
    if (node->returnType && node->returnType->typeInfo != g_TypeMgr.typeInfoVoid)
    {
        if (!(node->flags & AST_SCOPE_HAS_RETURN))
        {
            if (node->flags & AST_FCT_HAS_RETURN)
                return sourceFile->report({sourceFile, node->content->token.endLocation, node->content->token.endLocation, format("not all control paths of function '%s' return a value", node->name.c_str())});
            return sourceFile->report({sourceFile, node->content->token.endLocation, node->content->token.endLocation, format("function '%s' must return a value", node->name.c_str())});
        }
    }

    // Now the full function has been solved, so we wakeup jobs depending on that
    scoped_lock lk(node->mutex);
    node->flags |= AST_FULL_RESOLVE;
    for (auto job : node->dependentJobs)
        g_ThreadMgr.addJob(job);

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

	// Collect function attributes
    SWAG_CHECK(collectAttributes(context, typeInfo->attributes, funcNode->attributes, funcNode, AstNodeKind::FuncDecl, funcNode->attributeFlags));
    if (funcNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
        funcNode->flags |= AST_CONST_EXPR;

    typeInfo->returnType = typeNode->typeInfo;
    SWAG_CHECK(typeNode->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, funcNode, typeInfo, SymbolKind::Function));
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, funcNode->ownerScope, funcNode, SymbolKind::Function));

    return true;
}

bool SemanticJob::resolveFuncCallParams(SemanticContext* context)
{
    return true;
}

bool SemanticJob::resolveFuncCallParam(SemanticContext* context)
{
    auto node      = context->node;
    node->typeInfo = node->childs[0]->typeInfo;
	node->byteCodeFct = &ByteCodeGenJob::emitFuncCallParam;
    return true;
}

bool SemanticJob::resolveReturn(SemanticContext* context)
{
    auto node       = context->node;
    auto funcNode   = node->ownerFct;
    auto sourceFile = context->sourceFile;

    // Check return type
    if (funcNode->returnType->typeInfo == g_TypeMgr.typeInfoVoid && !node->childs.empty())
    {
        Diagnostic diag{sourceFile, node->childs[0], format("function '%s' does not have a return type", funcNode->name.c_str())};
        Diagnostic note{sourceFile, funcNode->token, format("this is the definition of '%s'", funcNode->name.c_str()), DiagnosticLevel::Note};
        return sourceFile->report(diag, &note);
    }

    // Nothing to return
    if (funcNode->returnType->typeInfo == g_TypeMgr.typeInfoVoid && node->childs.empty())
        return true;

    // Check types
    auto returnType = funcNode->returnType->typeInfo;
    SWAG_CHECK(g_TypeMgr.makeCompatibles(sourceFile, returnType, node->childs[0]));
    context->result = SemanticResult::Done;

    // Propagate return
    auto scanNode = node->parent;
    while (scanNode && scanNode != node->ownerFct->parent)
    {
        scanNode->flags |= AST_SCOPE_HAS_RETURN;
        scanNode = scanNode->parent;
    }

    while (scanNode && scanNode != node->ownerFct->parent)
    {
        scanNode->flags |= AST_FCT_HAS_RETURN;
        scanNode = scanNode->parent;
    }

    return true;
}