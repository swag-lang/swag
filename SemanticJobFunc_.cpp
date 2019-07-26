#include "pch.h"
#include "Global.h"
#include "TypeManager.h"
#include "SourceFile.h"
#include "ThreadManager.h"
#include "Diagnostic.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "SymTable.h"
#include "Scope.h"
#include "CommandLine.h"
#include "Module.h"
#include "Attribute.h"

bool SemanticJob::setupFuncDeclParams(SourceFile* sourceFile, TypeInfoFuncAttr* typeInfo, AstNode* funcAttr, AstNode* parameters)
{
    if (!parameters)
        return true;

    bool defaultValueDone = false;
    int  index            = 0;
    typeInfo->parameters.reserve(parameters->childs.size());
    for (auto param : parameters->childs)
    {
        auto nodeParam      = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        auto funcParam      = g_Pool_typeInfoFuncAttrParam.alloc();
        funcParam->name     = param->name;
        funcParam->typeInfo = param->typeInfo;
        funcParam->index    = index++;

		// Variadic must be the last one
		if (nodeParam->typeInfo == g_TypeMgr.typeInfoVariadic)
        {
			funcAttr->flags |= AST_VARIADIC;
            if (index != parameters->childs.size())
                return sourceFile->report({sourceFile, nodeParam, "variadic argument should be the last one"});
        }

        // Default parameter value
        if (nodeParam->astAssignment)
        {
            if (!defaultValueDone)
            {
                defaultValueDone               = true;
                typeInfo->firstDefaultValueIdx = index - 1;
            }
        }
        else
        {
            SWAG_VERIFY(!defaultValueDone, sourceFile->report({sourceFile, nodeParam, format("parameter '%d', missing default value", index)}));
        }

        typeInfo->parameters.push_back(funcParam);
    }

    return true;
}

bool SemanticJob::resolveFuncDecl(SemanticContext* context)
{
    auto sourceFile   = context->sourceFile;
    auto node         = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    node->byteCodeFct = &ByteCodeGenJob::emitLocalFuncDecl;

    // Check prototype
    if ((node->attributeFlags & ATTRIBUTE_FOREIGN) && node->content)
    {
        sourceFile->report({sourceFile, node->token, "function marker with the 'foreign' attribute can't have a body"});
    }

    if (node->attributeFlags & ATTRIBUTE_TEST)
    {
        SWAG_VERIFY(node->returnType->typeInfo == g_TypeMgr.typeInfoVoid, sourceFile->report({sourceFile, node->returnType, "function marked with the 'test' attribute can't have a return value"}));
        SWAG_VERIFY(!node->parameters || node->parameters->childs.size() == 0, sourceFile->report({sourceFile, node->parameters, "function marked with the 'test' attribute can't have parameters"}));
    }

    // Can be null for intrinsics etc...
    if (node->content)
        node->content->byteCodeBeforeFct = &ByteCodeGenJob::emitBeforeFuncDeclContent;

    // Do we have a return value
    if (node->returnType && node->returnType->typeInfo != g_TypeMgr.typeInfoVoid)
    {
        if (!(node->flags & AST_SCOPE_HAS_RETURN))
        {
            if (node->flags & AST_FCT_HAS_RETURN)
                return sourceFile->report({sourceFile, node->token, format("not all control paths of function '%s' return a value", node->name.c_str())});
            return sourceFile->report({sourceFile, node->token, format("function '%s' must return a value", node->name.c_str())});
        }
    }

    // Now the full function has been solved, so we wakeup jobs depending on that
    {
        scoped_lock lk(node->mutex);
        node->flags |= AST_FULL_RESOLVE;
        for (auto job : node->dependentJobs)
            g_ThreadMgr.addJob(job);
    }

    // Ask for bytecode
    bool genByteCode = false;
    if (g_CommandLine.output && (sourceFile->buildPass > BuildPass::Semantic) && (sourceFile->module->buildPass > BuildPass::Semantic))
        genByteCode = true;
    if ((node->attributeFlags & ATTRIBUTE_TEST) && g_CommandLine.test && g_CommandLine.runByteCodeTests)
        genByteCode = true;
    if ((node->attributeFlags & ATTRIBUTE_TEST) && !g_CommandLine.test && !g_CommandLine.unittest)
        genByteCode = false;
    if (node->attributeFlags & ATTRIBUTE_PRINTBYTECODE)
        genByteCode = true;
    if (node->token.id == TokenId::IntrisicPrint || node->token.id == TokenId::IntrisicAssert)
        genByteCode = false;
    if (node->attributeFlags & ATTRIBUTE_FOREIGN)
        genByteCode = false;

    if (genByteCode)
    {
        scoped_lock lk(node->mutex);
        if (!(node->flags & AST_BYTECODE_GENERATED))
        {
            if (!node->byteCodeJob)
            {
                node->byteCodeJob               = g_Pool_byteCodeGenJob.alloc();
                node->byteCodeJob->sourceFile   = sourceFile;
                node->byteCodeJob->originalNode = node;
                node->byteCodeJob->nodes.push_back(node);
                ByteCodeGenJob::setupBC(context->sourceFile->module, node);
                g_ThreadMgr.addJob(node->byteCodeJob);
            }
        }
    }

    return true;
}

bool SemanticJob::resolveFuncDeclType(SemanticContext* context)
{
    auto typeNode   = context->node;
    auto sourceFile = context->sourceFile;
    auto funcNode   = CastAst<AstFuncDecl>(typeNode->parent, AstNodeKind::FuncDecl);

    // Return type
    if (!typeNode->childs.empty())
        typeNode->typeInfo = typeNode->childs.front()->typeInfo;
    else
        typeNode->typeInfo = g_TypeMgr.typeInfoVoid;

    // Register symbol with its type
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

    // Register parameters
    SWAG_CHECK(setupFuncDeclParams(sourceFile, typeInfo, funcNode, funcNode->parameters));

    // Collect function attributes
    SymbolAttributes attributes;
    SWAG_CHECK(collectAttributes(context, attributes, funcNode->parentAttributes, funcNode, AstNodeKind::FuncDecl, funcNode->attributeFlags));
    if (funcNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
        funcNode->flags |= AST_CONST_EXPR;

    // Register symbol
    typeInfo->returnType = typeNode->typeInfo;

    // Be sure this is a valid return type
    if (typeInfo->returnType->kind != TypeInfoKind::Native && typeInfo->returnType->kind != TypeInfoKind::TypeList)
        return sourceFile->report({sourceFile, typeNode->childs.front(), format("invalid return type '%s'", typeInfo->returnType->name.c_str())});

    typeInfo->computeName();
    funcNode->resolvedSymbolOverload = typeNode->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, funcNode, typeInfo, SymbolKind::Function, nullptr, 0, &funcNode->resolvedSymbolName);
    SWAG_CHECK(funcNode->resolvedSymbolOverload);
    funcNode->resolvedSymbolOverload->attributes = move(attributes);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, funcNode->ownerScope, funcNode, SymbolKind::Function));

    return true;
}

bool SemanticJob::resolveFuncCallParams(SemanticContext* context)
{
    auto node         = context->node;
    node->byteCodeFct = &ByteCodeGenJob::emitFuncCallParams;
    return true;
}

bool SemanticJob::resolveFuncCallParam(SemanticContext* context)
{
    auto node      = context->node;
    auto child     = node->childs.front();
    node->typeInfo = child->typeInfo;
    node->inheritComputedValue(child);
    node->inheritAndFlag(child, AST_CONST_EXPR);
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
        if (scanNode->kind == AstNodeKind::If ||
            scanNode->kind == AstNodeKind::Else ||
            scanNode->kind == AstNodeKind::While ||
            scanNode->kind == AstNodeKind::Loop ||
            scanNode->kind == AstNodeKind::For ||
            scanNode->kind == AstNodeKind::Switch)
            break;
        scanNode = scanNode->parent;
    }

    while (scanNode && scanNode != node->ownerFct->parent)
    {
        scanNode->flags |= AST_FCT_HAS_RETURN;
        scanNode = scanNode->parent;
    }

    return true;
}