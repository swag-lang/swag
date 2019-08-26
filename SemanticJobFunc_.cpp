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

bool SemanticJob::setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, AstNode* funcAttr, AstNode* parameters, bool forGenerics)
{
    if (!parameters)
        return true;

    bool        defaultValueDone = false;
    int         index            = 0;
    SourceFile* sourceFile       = context->sourceFile;

    if (forGenerics)
    {
        typeInfo->genericParameters.reserve(parameters->childs.size());
        typeInfo->flags |= TYPEINFO_GENERIC;
    }
    else
        typeInfo->parameters.reserve(parameters->childs.size());

    for (auto param : parameters->childs)
    {
        auto nodeParam        = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        auto funcParam        = g_Pool_typeInfoParam.alloc();
        funcParam->namedParam = param->name;
        funcParam->name       = param->typeInfo->name;
        funcParam->typeInfo   = param->typeInfo;
        funcParam->sizeOf     = param->typeInfo->sizeOf;
        funcParam->index      = index++;

        parameters->inheritOrFlag(nodeParam->type, AST_IS_GENERIC);

        // Variadic must be the last one
        if (nodeParam->typeInfo == g_TypeMgr.typeInfoVariadic)
        {
            typeInfo->flags |= TYPEINFO_VARIADIC;
            if (index != parameters->childs.size())
                return context->errorContext.report({sourceFile, nodeParam, "variadic argument should be the last one"});
        }

        // Default parameter value
        if (nodeParam->assignment)
        {
            if (!defaultValueDone)
            {
                defaultValueDone               = true;
                typeInfo->firstDefaultValueIdx = index - 1;
            }
        }
        else
        {
            SWAG_VERIFY(!defaultValueDone, context->errorContext.report({sourceFile, nodeParam, format("parameter '%d', missing default value", index)}));
        }

        if (forGenerics)
            typeInfo->genericParameters.push_back(funcParam);
        else
            typeInfo->parameters.push_back(funcParam);
    }

    return true;
}

bool SemanticJob::checkFuncPrototype(SemanticContext* context, AstFuncDecl* node)
{
    if (node->flags & AST_GENERATED)
        return true;

    const auto& name = node->name;

    // Check operators
    if (name.length() < 3)
        return true;

    // A special function starts with 'op', and then there's an upper case letter
    if (name[0] != 'o' || name[1] != 'p' || name[2] < 'A' || name[2] > 'Z')
        return true;

    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(node->parent->kind == AstNodeKind::Impl, context->errorContext.report({sourceFile, node->token, format("special function '%s' should be defined in a 'impl' scope", name.c_str())}));
    auto implNode = CastAst<AstImpl>(node->parent, AstNodeKind::Impl);

    // No need to raise an error, the semantic pass on the impl node will fail
    auto typeStruct = implNode->identifier->typeInfo;
    if (typeStruct->kind != TypeInfoKind::Struct)
        return true;

    // Already checked by the syntax pass
    if (name == "opInit")
        return true;

    // First type must be struct
    auto firstType = node->parameters->childs.front()->typeInfo;
    SWAG_VERIFY(firstType->isSame(typeStruct), context->errorContext.report({sourceFile, node->parameters->childs.front(), format("invalid first parameter type for special function '%s' ('%s' expected, '%s' provided)", name.c_str(), typeStruct->name.c_str(), firstType->name.c_str())}));

    // Generic operator must have one generic parameter of type string
    if (name == "opBinary" || name == "opUnary" || name == "opAssign" || name == "opIndexAssign")
    {
        SWAG_VERIFY(node->genericParameters && node->genericParameters->childs.size() == 1, context->errorContext.report({sourceFile, node->token, format("invalid number of generic parameters for special function '%s'", name.c_str())}));
        auto firstGen = node->genericParameters->childs.front();
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr.typeInfoString), context->errorContext.report({sourceFile, firstGen, format("invalid generic parameter for special function '%s' ('string' expected, '%s' provided)", name.c_str(), firstGen->name.c_str())}));
    }

    auto parameters = node->parameters;
    auto returnType = node->returnType;
    if (name == "opEquals")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 2, context->errorContext.report({sourceFile, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo->isSame(g_TypeMgr.typeInfoBool), context->errorContext.report({sourceFile, returnType, format("invalid return type for special function '%s' ('bool' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opCmp")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 2, context->errorContext.report({sourceFile, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo->isSame(g_TypeMgr.typeInfoS32), context->errorContext.report({sourceFile, returnType, format("invalid return type for special function '%s' ('bool' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opBinary")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 2, context->errorContext.report({sourceFile, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo->isSame(typeStruct), context->errorContext.report({sourceFile, returnType, format("invalid return type for special function '%s' ('%s' expected, '%s' provided)", name.c_str(), typeStruct->name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opUnary")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 1, context->errorContext.report({sourceFile, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo->isSame(typeStruct), context->errorContext.report({sourceFile, returnType, format("invalid return type for special function '%s' ('%s' expected, '%s' provided)", name.c_str(), typeStruct->name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opAssign")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 2, context->errorContext.report({sourceFile, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType->typeInfo->isSame(g_TypeMgr.typeInfoVoid), context->errorContext.report({sourceFile, returnType, format("invalid return type for special function '%s' ('void' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opIndex")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() >= 2, context->errorContext.report({sourceFile, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(!returnType->typeInfo->isSame(g_TypeMgr.typeInfoVoid), context->errorContext.report({sourceFile, returnType, format("missing return type for special function '%s'", name.c_str())}));
        for (int i = 1; i < parameters->childs.size(); i++)
            SWAG_VERIFY(parameters->childs[i]->typeInfo->isSame(g_TypeMgr.typeInfoS32), context->errorContext.report({sourceFile, parameters->childs[i], format("invalid parameter '%d' for special function '%s' ('s32' expected, '%s' provided)", i + 1, name.c_str(), parameters->childs[i]->typeInfo->name.c_str())}));
    }
    else if (name == "opIndexAssign")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() >= 3, context->errorContext.report({sourceFile, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType->typeInfo->isSame(g_TypeMgr.typeInfoVoid), context->errorContext.report({sourceFile, returnType, format("invalid return type for special function '%s' ('void' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
        for (int i = 2; i < parameters->childs.size(); i++)
            SWAG_VERIFY(parameters->childs[i]->typeInfo->isSame(g_TypeMgr.typeInfoS32), context->errorContext.report({sourceFile, parameters->childs[i], format("invalid parameter '%d' for special function '%s' ('s32' expected, '%s' provided)", i + 1, name.c_str(), parameters->childs[i]->typeInfo->name.c_str())}));
    }
    else
    {
        return context->errorContext.report({sourceFile, node->token, format("function '%s' does not match a special function/operator overload", name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveFuncDeclParams(SemanticContext* context)
{
    auto node = context->node;
    node->inheritOrFlag(AST_IS_GENERIC);
    node->byteCodeFct = &ByteCodeGenJob::emitFuncDeclParams;
    return true;
}

bool SemanticJob::resolveFuncDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    // No semantic on a generic function
    if (node->flags & AST_IS_GENERIC)
        return true;

    SWAG_CHECK(checkFuncPrototype(context, node));
    node->byteCodeFct   = &ByteCodeGenJob::emitLocalFuncDecl;
    typeInfo->stackSize = node->stackSize;

    // Check prototype
    if ((node->attributeFlags & ATTRIBUTE_FOREIGN) && node->content)
    {
        context->errorContext.report({sourceFile, node->token, "function with the 'foreign' attribute can't have a body"});
    }

    if (node->attributeFlags & ATTRIBUTE_TEST)
    {
        SWAG_VERIFY(node->returnType->typeInfo == g_TypeMgr.typeInfoVoid, context->errorContext.report({sourceFile, node->returnType, "function marked with the 'test' attribute can't have a return value"}));
        SWAG_VERIFY(!node->parameters || node->parameters->childs.size() == 0, context->errorContext.report({sourceFile, node->parameters, "function marked with the 'test' attribute can't have parameters"}));
    }

    // Can be null for intrinsics etc...
    if (node->content)
        node->content->byteCodeBeforeFct = &ByteCodeGenJob::emitBeforeFuncDeclContent;

    // Do we have a return value
    if (node->content && node->returnType && node->returnType->typeInfo != g_TypeMgr.typeInfoVoid)
    {
        if (!(node->flags & AST_SCOPE_HAS_RETURN))
        {
            if (node->flags & AST_FCT_HAS_RETURN)
                return context->errorContext.report({sourceFile, node->token, format("not all control paths of function '%s' return a value", node->name.c_str())});
            return context->errorContext.report({sourceFile, node->token, format("function '%s' must return a value", node->name.c_str())});
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
    if (node->token.id == TokenId::Intrinsic)
        genByteCode = false;
    if (node->attributeFlags & ATTRIBUTE_FOREIGN)
        genByteCode = false;
    if (node->name == "opInit")
        genByteCode = true;
    if (node->attributeFlags & AST_IS_GENERIC)
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

    // Function is generic if parameters are
    if (funcNode->parameters)
        funcNode->inheritOrFlag(funcNode->parameters, AST_IS_GENERIC);

    if (!(funcNode->flags & AST_FROM_GENERIC))
    {
        // Register parameters
        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->genericParameters, true));
        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->parameters, false));
        if (funcNode->genericParameters)
            funcNode->flags |= AST_IS_GENERIC;
    }

    // No semantic on content if function is generic
    if (funcNode->flags & AST_IS_GENERIC)
        funcNode->content->flags |= AST_DISABLED;

    // Collect function attributes
    SymbolAttributes attributes;
    SWAG_CHECK(collectAttributes(context, attributes, funcNode->parentAttributes, funcNode, AstNodeKind::FuncDecl, funcNode->attributeFlags));
    if (funcNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
        funcNode->flags |= AST_CONST_EXPR;

    // Register symbol
    typeInfo->returnType = typeNode->typeInfo;

    // Be sure this is a valid return type
    if (typeInfo->returnType->kind != TypeInfoKind::Native &&
        typeInfo->returnType->kind != TypeInfoKind::TypeList &&
        typeInfo->returnType->kind != TypeInfoKind::Struct &&
        typeInfo->returnType->kind != TypeInfoKind::Generic &&
        typeInfo->returnType->kind != TypeInfoKind::Alias &&
        typeInfo->returnType->kind != TypeInfoKind::Pointer)
        return context->errorContext.report({sourceFile, typeNode->childs.front(), format("invalid return type '%s'", typeInfo->returnType->name.c_str())});

    typeInfo->computeName();

    // opInit registration
    if (funcNode->name == "opInit")
    {
        SWAG_ASSERT(funcNode->parameters);
        SWAG_ASSERT(funcNode->parameters->childs.size() >= 1);
        auto typeStruct       = CastTypeInfo<TypeInfoStruct>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Struct);
        typeStruct->opInitFct = funcNode;
    }

    uint32_t symbolFlags = 0;
    if (funcNode->flags & AST_IS_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;

    funcNode->resolvedSymbolOverload = typeNode->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, funcNode, typeInfo, SymbolKind::Function, nullptr, symbolFlags, &funcNode->resolvedSymbolName);
    SWAG_CHECK(funcNode->resolvedSymbolOverload);
    funcNode->resolvedSymbolOverload->attributes = move(attributes);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, funcNode->ownerScope, funcNode, SymbolKind::Function));
    return true;
}

bool SemanticJob::resolveFuncCallParams(SemanticContext* context)
{
    auto node = context->node;
    node->inheritOrFlag(AST_IS_GENERIC);
    return true;
}

bool SemanticJob::resolveFuncCallParam(SemanticContext* context)
{
    auto node      = context->node;
    auto child     = node->childs.front();
    node->typeInfo = child->typeInfo;
    node->inheritComputedValue(child);
    node->inheritOrFlag(child, AST_CONST_EXPR);
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
        return context->errorContext.report(diag, &note);
    }

    // Nothing to return
    if (funcNode->returnType->typeInfo == g_TypeMgr.typeInfoVoid && node->childs.empty())
        return true;

    // Check types
    auto returnType = funcNode->returnType->typeInfo;
    SWAG_CHECK(g_TypeMgr.makeCompatibles(&context->errorContext, returnType, node->childs[0]));
    context->result = SemanticResult::Done;

    // Propagate return
    auto scanNode = node->parent;
    while (scanNode && scanNode != node->ownerFct->parent)
    {
        scanNode->flags |= AST_SCOPE_HAS_RETURN;
        if (scanNode->kind == AstNodeKind::If ||
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