#include "pch.h"
#include "SourceFile.h"
#include "ThreadManager.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "SymTable.h"
#include "Module.h"
#include "Attribute.h"
#include "TypeManager.h"

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

        // Not everything is possible for types for attributes
        if (param->ownerScope->kind == ScopeKind::Attribute)
        {
            SWAG_VERIFY(funcParam->typeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, nodeParam, format("invalid type '%s' for attribute parameter", funcParam->typeInfo->name.c_str())}));
            SWAG_VERIFY(funcParam->typeInfo->nativeType != NativeTypeKind::Any, context->errorContext.report({sourceFile, nodeParam, format("invalid type '%s' for attribute parameter", funcParam->typeInfo->name.c_str())}));
        }

        parameters->inheritOrFlag(nodeParam->type, AST_IS_GENERIC);

        // Variadic must be the last one
        if (nodeParam->typeInfo->kind == TypeInfoKind::Variadic)
        {
            typeInfo->flags |= TYPEINFO_VARIADIC;
            if (index != parameters->childs.size())
                return context->errorContext.report({sourceFile, nodeParam, "variadic argument should be the last one"});
        }
        else if (nodeParam->typeInfo->kind == TypeInfoKind::TypedVariadic)
        {
            typeInfo->flags |= TYPEINFO_TYPED_VARIADIC;
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

    node->byteCodeFct   = &ByteCodeGenJob::emitLocalFuncDecl;
    typeInfo->stackSize = node->stackSize;

    // Reserve one RR register for each return value
    int countRR = 0;
    if (typeInfo->returnType != g_TypeMgr.typeInfoVoid)
        countRR += typeInfo->returnType->numRegisters();
    countRR += (int) typeInfo->parameters.size();
    context->sourceFile->module->reserveRegisterRR(countRR);

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
    if (node->name == "defaultAllocator" && sourceFile->swagFile)
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

    // Short lambda with a return type we must deduce
    // In that case, symbol registration will not be done at the end of that function but once the return expression
    // has been evaluated, and the type deduced
    bool shortLambda = false;
    if ((funcNode->flags & AST_SHORT_LAMBDA) && !(funcNode->returnType->flags & AST_FUNC_RETURN_DEFINED))
        shortLambda = true;

    // No semantic on content if function is generic, except if this is a short lambda, and we must deduce the return type
    // (because we need to parse the content of the function in order to deduce that type)
    if ((funcNode->flags & AST_IS_GENERIC) && !shortLambda)
        funcNode->content->flags |= AST_DISABLED;

    // We do want to do a full semantic pass on content for a short lambda with returned type inferred, se we need
    // to remove the AST_FROM_GENERIC flag, otherwise, some stuff won't be done (because typeinfo has been set on nodes)
    else if ((funcNode->flags & AST_FROM_GENERIC) && shortLambda)
        Ast::visit(funcNode->content, [](AstNode* x) { x->flags &= ~AST_FROM_GENERIC; });

    // Collect function attributes
    SWAG_CHECK(collectAttributes(context, funcNode->collectAttributes, funcNode->parentAttributes, funcNode, AstNodeKind::FuncDecl, funcNode->attributeFlags));
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
        typeInfo->returnType->kind != TypeInfoKind::Lambda &&
        typeInfo->returnType->kind != TypeInfoKind::Pointer)
        return context->errorContext.report({sourceFile, typeNode->childs.front(), format("invalid return type '%s'", typeInfo->returnType->name.c_str())});

    typeInfo->computeName();

    ComputedValue v;
    if (funcNode->collectAttributes.getValue("swag.waitsem.ms", v))
        this_thread::sleep_for(chrono::milliseconds(v.reg.s32));

    // Special functions registration
    if (funcNode->parameters && funcNode->parameters->childs.size() == 1)
    {
        if (funcNode->name == "opInit")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opInitFct = funcNode;
        }
        else if (funcNode->name == "opPostCopy")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserPostCopyFct = funcNode;
        }
        else if (funcNode->name == "opPostMove")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserPostMoveFct = funcNode;
        }
        else if (funcNode->name == "opDrop")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserDropFct = funcNode;
        }
    }

    SWAG_CHECK(checkFuncPrototype(context, funcNode));

    // For a short lambda without a specified return type, we need to defer the symbol registration, as we
    // need to infer it from the lambda expression
    if (!shortLambda)
        SWAG_CHECK(registerFuncSymbol(context, funcNode));
    return true;
}

bool SemanticJob::registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode)
{
    uint32_t symbolFlags = 0;
    if (funcNode->flags & AST_IS_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;

    auto typeFunc                    = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    typeFunc->attributes             = move(funcNode->collectAttributes);
    funcNode->resolvedSymbolOverload = funcNode->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, funcNode, funcNode->typeInfo, SymbolKind::Function, nullptr, symbolFlags, &funcNode->resolvedSymbolName);
    SWAG_CHECK(funcNode->resolvedSymbolOverload);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, funcNode->ownerScope, funcNode, SymbolKind::Function));
    return true;
}

bool SemanticJob::resolveFuncCallParams(SemanticContext* context)
{
    auto node = context->node;
    node->inheritOrFlag(AST_IS_GENERIC);
    node->inheritAndFlag(AST_CONST_EXPR);
    return true;
}

bool SemanticJob::resolveFuncCallParam(SemanticContext* context)
{
    auto node      = context->node;
    auto child     = node->childs.front();
    node->typeInfo = child->typeInfo;
    node->inheritComputedValue(child);
    node->inheritOrFlag(child, AST_CONST_EXPR);
    node->inheritOrFlag(child, AST_IS_GENERIC);
    node->byteCodeFct = &ByteCodeGenJob::emitFuncCallParam;

    // Can be called for generic parameters in type definition, in that case, we are a type, so no
    // test for concrete must be done
    if (!(node->parent->flags & AST_NO_BYTECODE))
    {
        SWAG_CHECK(checkIsConcrete(context, child));
        node->flags |= AST_R_VALUE;
    }

    SWAG_CHECK(evaluateConstExpression(context, node));
    if (context->result == SemanticResult::Pending)
        return true;

    return true;
}

bool SemanticJob::checkUnreachableCode(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;

    // Return must be the last of its block
    if (node->parent->childs.back() != node)
    {
        auto idx = Ast::findChildIndex(node->parent, node);
        return context->errorContext.report({sourceFile, node->parent->childs[idx + 1], "unreachable code"});
    }

    return true;
}

bool SemanticJob::resolveReturn(SemanticContext* context)
{
    SWAG_CHECK(checkUnreachableCode(context));

    auto node       = context->node;
    auto funcNode   = node->ownerFct;
    auto sourceFile = context->sourceFile;

    node->byteCodeFct = &ByteCodeGenJob::emitReturn;

    // Nothing to return
    if (funcNode->returnType->typeInfo == g_TypeMgr.typeInfoVoid && node->childs.empty())
        return true;

    // Check return type
    bool lateRegister = false;
    if (funcNode->returnType->typeInfo == g_TypeMgr.typeInfoVoid && !node->childs.empty())
    {
        // This is a short lambda without a specified return type. We now have it
        if ((funcNode->flags & AST_SHORT_LAMBDA) && !(funcNode->returnType->flags & AST_FUNC_RETURN_DEFINED))
        {
            auto typeInfoFunc        = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
            typeInfoFunc->returnType = node->childs.front()->typeInfo;
            typeInfoFunc->computeName();
            funcNode->returnType->typeInfo = typeInfoFunc->returnType;
            lateRegister                   = true;
        }
        else
        {
            Diagnostic diag{sourceFile, node, format("function '%s' does not have a return type", funcNode->name.c_str())};
            Diagnostic note{sourceFile, funcNode->token, format("this is the definition of '%s'", funcNode->name.c_str()), DiagnosticLevel::Note};
            return context->errorContext.report(diag, &note);
        }
    }

    // Check types
    auto child = node->childs[0];
    SWAG_CHECK(checkIsConcrete(context, child));
    auto returnType = funcNode->returnType->typeInfo;
    SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child));

    // When returning a struct, we need to know if postcopy or postmove are here, and wait for them to resolve
    if (returnType && returnType->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(waitForStructUserOps(context, funcNode->returnType));
        if (context->result == SemanticResult::Pending)
            return true;
    }

    // If we are returning a local variable, we can do a move
    if (child->resolvedSymbolOverload && (child->resolvedSymbolOverload->flags & OVERLOAD_VAR_LOCAL))
        child->flags |= AST_FORCE_MOVE;

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

    // Register symbol now that we have inferred the return type
    if (lateRegister)
        SWAG_CHECK(registerFuncSymbol(context, funcNode));

    return true;
}