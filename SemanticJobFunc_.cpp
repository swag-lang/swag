#include "pch.h"
#include "SourceFile.h"
#include "ThreadManager.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"

bool SemanticJob::setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, AstNode* funcAttr, AstNode* parameters, bool forGenerics)
{
    if (!parameters)
        return true;

    bool defaultValueDone = false;
    int  index            = 0;

    if (forGenerics)
    {
        typeInfo->genericParameters.clear();
        typeInfo->genericParameters.reserve(parameters->childs.size());
        typeInfo->flags |= TYPEINFO_GENERIC;
    }
    else
    {
        typeInfo->parameters.clear();
        typeInfo->parameters.reserve(parameters->childs.size());
    }

    auto sourceFile = context->sourceFile;
    for (auto param : parameters->childs)
    {
        auto nodeParam        = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        auto funcParam        = g_Pool_typeInfoParam.alloc();
        funcParam->namedParam = param->name;
        funcParam->name       = param->typeInfo->name;
        funcParam->typeInfo   = param->typeInfo;
        funcParam->sizeOf     = param->typeInfo->sizeOf;
        funcParam->index      = index++;
        funcParam->node       = nodeParam;

        // Not everything is possible for types for attributes
        if (param->ownerScope->kind == ScopeKind::Attribute)
        {
            SWAG_VERIFY(funcParam->typeInfo->kind == TypeInfoKind::Native, context->report({nodeParam, format("invalid type '%s' for attribute parameter", funcParam->typeInfo->name.c_str())}));
            SWAG_VERIFY(funcParam->typeInfo->nativeType != NativeTypeKind::Any, context->report({nodeParam, format("invalid type '%s' for attribute parameter", funcParam->typeInfo->name.c_str())}));
        }

        parameters->inheritOrFlag(nodeParam->type, AST_IS_GENERIC);

        // Variadic must be the last one
        if (nodeParam->typeInfo->kind == TypeInfoKind::Variadic)
        {
            SWAG_VERIFY(!(funcAttr->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, "inline function has variadic arguments, this is not yet supported"}));

            typeInfo->flags |= TYPEINFO_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, "variadic argument should be the last one"});
        }
        else if (nodeParam->typeInfo->kind == TypeInfoKind::TypedVariadic)
        {
            SWAG_VERIFY(!(funcAttr->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, "inline function has variadic arguments, this is not yet supported"}));

            typeInfo->flags |= TYPEINFO_TYPED_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, "variadic argument should be the last one"});
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
            SWAG_VERIFY(!defaultValueDone, context->report({nodeParam, format("parameter '%d', missing default value", index)}));
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
    node->byteCodeFct = ByteCodeGenJob::emitFuncDeclParams;
    return true;
}

bool SemanticJob::resolveFuncDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto node       = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node, SymbolKind::Function));
    if (context->result == ContextResult::Pending)
        return true;

    // Only one main per module !
    if (node->attributeFlags & ATTRIBUTE_MAIN_FUNC)
    {
        scoped_lock lk(sourceFile->module->mutexFile);
        if (sourceFile->module->mainIsDefined)
        {
            Diagnostic diag({node, node->token, "#main directive already defined one"});
            Diagnostic note{module->mainIsDefined, sourceFile->module->mainIsDefined->token, "this is the other definition", DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        sourceFile->module->mainIsDefined = node;
    }

    // No semantic on a generic function, or a macro
    if (node->flags & AST_IS_GENERIC)
    {
        if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(node->flags & AST_GENERATED))
            node->ownerScope->addPublicGenericFunc(node);
        return true;
    }

    if (node->attributeFlags & ATTRIBUTE_MACRO)
    {
        if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(node->flags & AST_GENERATED))
            node->ownerScope->addPublicGenericFunc(node);
        SWAG_CHECK(setFullResolve(context, node));
        return true;
    }

    // An inline function will not have bytecode, so need to register public by hand now
    if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && (node->attributeFlags & ATTRIBUTE_INLINE))
        node->ownerScope->addPublicGenericFunc(node);

    node->byteCodeFct   = ByteCodeGenJob::emitLocalFuncDecl;
    typeInfo->stackSize = node->stackSize;

    // Reserve one RR register for each return value
    int countRR = 0;
    if (typeInfo->returnType != g_TypeMgr.typeInfoVoid)
        countRR += typeInfo->returnType->numRegisters();
    countRR += (int) typeInfo->parameters.size();
    context->sourceFile->module->reserveRegisterRR(countRR);

    // Check attributes
    if ((node->attributeFlags & ATTRIBUTE_FOREIGN) && node->content)
        return context->report({node, node->token, "function with the 'swag.foreign' attribute cannot have a body"});
	if (node->attributeFlags & ATTRIBUTE_FOREIGN)
		sourceFile->module->registerForeign(node);

    if (node->flags & AST_SPECIAL_COMPILER_FUNC)
    {
        SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_INLINE), context->report({node, node->token, "compiler special function cannot have the 'swag.inline' attribute"}));
        SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_PUBLIC), context->report({node, node->token, "compiler special function cannot have the 'swag.public' attribute"}));
        SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_PRIVATE), context->report({node, node->token, "compiler special function cannot have the 'swag.private' attribute"}));
    }

    if (node->attributeFlags & ATTRIBUTE_TEST_FUNC)
    {
        SWAG_VERIFY(node->returnType->typeInfo == g_TypeMgr.typeInfoVoid, context->report({node->returnType, "function with the 'swag.test' attribute cannot have a return value"}));
        SWAG_VERIFY(!node->parameters || node->parameters->childs.size() == 0, context->report({node->parameters, "function with the 'swag.test' attribute cannot have parameters"}));
    }

    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        if (!node->ownerScope->isGlobal() && node->ownerScope->kind != ScopeKind::Struct)
            return context->report({node, node->token, format("embedded function '%s' cannot be public", node->name.c_str())});
        if (node->attributeFlags & ATTRIBUTE_TEST_FUNC)
            return context->report({node, node->token, format("test function '%s' cannot be public", node->name.c_str())});
        if (node->attributeFlags & ATTRIBUTE_COMPILER)
            return context->report({node, node->token, format("compiler function '%s' cannot be public", node->name.c_str())});
        if (node->attributeFlags & ATTRIBUTE_FOREIGN)
            return context->report({node, node->token, format("foreign function '%s' cannot be public", node->name.c_str())});
    }

    // Can be null for intrinsics etc...
    if (node->content)
        node->content->byteCodeBeforeFct = ByteCodeGenJob::emitBeforeFuncDeclContent;

    // Do we have a return value
    if (node->content && node->returnType && node->returnType->typeInfo != g_TypeMgr.typeInfoVoid)
    {
        if (!(node->flags & AST_SCOPE_HAS_RETURN))
        {
            if (node->flags & AST_FCT_HAS_RETURN)
                return context->report({node, node->token, format("not all control paths of function '%s' return a value", node->name.c_str())});
            return context->report({node, node->token, format("function '%s' must return a value", node->name.c_str())});
        }
    }

    // Now the full function has been solved, so we wakeup jobs depending on that
    SWAG_CHECK(setFullResolve(context, node));

    // Ask for bytecode
    bool genByteCode = true;
    if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
        genByteCode = false;
    if (node->token.id == TokenId::Intrinsic)
        genByteCode = false;
    if (node->attributeFlags & ATTRIBUTE_FOREIGN)
        genByteCode = false;
    if (node->flags & AST_IS_GENERIC)
        genByteCode = false;
    if (!node->content)
        genByteCode = false;
    if (node->attributeFlags & ATTRIBUTE_INLINE)
    {
        node->flags |= AST_NO_BYTECODE;
        genByteCode = false;
    }

    if (genByteCode)
        ByteCodeGenJob::askForByteCode(context->job->dependentJob, nullptr, node, 0);

    return true;
}

bool SemanticJob::setFullResolve(SemanticContext* context, AstFuncDecl* funcNode)
{
    scoped_lock lk(funcNode->mutex);
    funcNode->flags |= AST_FULL_RESOLVE;
    for (auto job : funcNode->dependentJobs.list)
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
        typeNode->typeInfo = typeNode->childs.front()->typeInfo;
    else
        typeNode->typeInfo = g_TypeMgr.typeInfoVoid;

    // Collect function attributes
    SWAG_ASSERT(funcNode->semanticState == AstNodeResolveState::ProcessingChilds);
    SWAG_CHECK(collectAttributes(context, funcNode->collectAttributes, funcNode->parentAttributes, funcNode, AstNodeKind::FuncDecl, funcNode->attributeFlags));

    if (funcNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
        funcNode->flags |= AST_CONST_EXPR;

    if (funcNode->attributeFlags & ATTRIBUTE_MACRO)
    {
        SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, format("function '%s' is marked with 'swag.macro' and 'swag.inline' attributes at the same time", funcNode->name.c_str())}));
        SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MIXIN), context->report({funcNode, funcNode->token, format("function '%s' is marked with 'swag.macro' and 'swag.mixin' attributes at the same time", funcNode->name.c_str())}));
        funcNode->attributeFlags |= ATTRIBUTE_INLINE;
    }

    if (funcNode->attributeFlags & ATTRIBUTE_MIXIN)
    {
        SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, format("function '%s' is marked with 'swag.mixin' and 'swag.inline' attributes at the same time", funcNode->name.c_str())}));
        SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MACRO), context->report({funcNode, funcNode->token, format("function '%s' is marked with 'swag.mixin' and 'swag.macro' attributes at the same time", funcNode->name.c_str())}));
        funcNode->attributeFlags |= ATTRIBUTE_INLINE;
        funcNode->attributeFlags |= ATTRIBUTE_MACRO;
    }

    // Register symbol with its type
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

    if (!(funcNode->flags & AST_FROM_GENERIC))
    {
        // Determine if function is generic
        if (funcNode->parameters)
            funcNode->inheritOrFlag(funcNode->parameters, AST_IS_GENERIC);
        if (funcNode->ownerStructScope && (funcNode->ownerStructScope->owner->flags & AST_IS_GENERIC))
            funcNode->flags |= AST_IS_GENERIC;
        if (funcNode->genericParameters)
            funcNode->flags |= AST_IS_GENERIC;

        // Register parameters
        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->genericParameters, true));
        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->parameters, false));
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
        funcNode->content->flags |= AST_NO_SEMANTIC;

    // We do want to do a full semantic pass on content for a short lambda with returned type inferred, so we need
    // to remove the AST_FROM_GENERIC flag, otherwise, some stuff won't be done (because typeinfo has been set on nodes)
    else if ((funcNode->flags & AST_FROM_GENERIC) && shortLambda)
        Ast::visit(funcNode->content, [](AstNode* x) { x->flags &= ~AST_FROM_GENERIC; });

    // Macro will not evaluate its content before being inline
    if ((funcNode->attributeFlags & ATTRIBUTE_MACRO) && !shortLambda)
        funcNode->content->flags |= AST_NO_SEMANTIC;

    // Register symbol
    typeInfo->returnType = typeNode->typeInfo;

    // Be sure this is a valid return type
    if (typeInfo->returnType->kind != TypeInfoKind::Native &&
        typeInfo->returnType->kind != TypeInfoKind::TypeList &&
        typeInfo->returnType->kind != TypeInfoKind::Struct &&
        typeInfo->returnType->kind != TypeInfoKind::Generic &&
        typeInfo->returnType->kind != TypeInfoKind::Alias &&
        typeInfo->returnType->kind != TypeInfoKind::Lambda &&
        typeInfo->returnType->kind != TypeInfoKind::Slice &&
        typeInfo->returnType->kind != TypeInfoKind::Enum &&
        typeInfo->returnType->kind != TypeInfoKind::Pointer)
        return context->report({typeNode->childs.front(), format("invalid return type '%s'", typeInfo->returnType->name.c_str())});

    typeInfo->computeName();

    ComputedValue v;
    if (funcNode->collectAttributes.getValue("swag.waitsem.ms", v))
        this_thread::sleep_for(chrono::milliseconds(v.reg.s32));

    SWAG_VERIFY(funcNode->name != "opInit" || funcNode->flags & AST_GENERATED || funcNode->attributeFlags & ATTRIBUTE_FOREIGN, sourceFile->report({funcNode, funcNode->token, "'opInit' function cannot be user defined, as it is generated by the compiler"}));

    // Special functions registration
    if (funcNode->parameters && funcNode->parameters->childs.size() == 1)
    {
        if (funcNode->name == "opPostCopy")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->finalType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserPostCopyFct = funcNode;
        }
        else if (funcNode->name == "opPostMove")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->finalType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserPostMoveFct = funcNode;
        }
        else if (funcNode->name == "opDrop")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->finalType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserDropFct = funcNode;
        }
    }

    // For a short lambda without a specified return type, we need to defer the symbol registration, as we
    // need to infer it from the lambda expression
    SWAG_CHECK(registerFuncSymbol(context, funcNode, shortLambda ? OVERLOAD_INCOMPLETE : 0));
    return true;
}

bool SemanticJob::registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode, uint32_t symbolFlags)
{
    if (!(symbolFlags & OVERLOAD_INCOMPLETE))
        SWAG_CHECK(checkFuncPrototype(context, funcNode));

    if (funcNode->flags & AST_IS_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;

    auto typeFunc                    = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    typeFunc->attributes             = move(funcNode->collectAttributes);
    funcNode->resolvedSymbolOverload = funcNode->ownerScope->symTable.addSymbolTypeInfo(context, funcNode, funcNode->typeInfo, SymbolKind::Function, nullptr, symbolFlags, &funcNode->resolvedSymbolName);
    SWAG_CHECK(funcNode->resolvedSymbolOverload);

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
    node->byteCodeFct = ByteCodeGenJob::emitFuncCallParam;

    // Can be called for generic parameters in type definition, in that case, we are a type, so no
    // test for concrete must be done
    if (!(node->parent->flags & AST_NO_BYTECODE))
    {
        SWAG_CHECK(checkIsConcrete(context, child));
        node->flags |= AST_R_VALUE;
    }

    SWAG_CHECK(evaluateConstExpression(context, node));
    if (context->result == ContextResult::Pending)
        return true;

    node->resolvedSymbolName     = child->resolvedSymbolName;
    node->resolvedSymbolOverload = child->resolvedSymbolOverload;
    return true;
}

bool SemanticJob::checkUnreachableCode(SemanticContext* context)
{
    auto node = context->node;

    // Return must be the last of its block
    if (node->parent->childs.back() != node)
    {
        if (node->parent->kind == AstNodeKind::If)
        {
            AstIf* ifNode = CastAst<AstIf>(node->parent, AstNodeKind::If);
            if (ifNode->ifBlock == node || ifNode->elseBlock == node)
                return true;
        }

        auto idx = Ast::findChildIndex(node->parent, node);
        return context->report({node->parent->childs[idx + 1], "unreachable code"});
    }

    return true;
}

bool SemanticJob::resolveReturn(SemanticContext* context)
{
    SWAG_CHECK(checkUnreachableCode(context));

    auto node     = context->node;
    auto funcNode = node->ownerFct;

    node->byteCodeFct = ByteCodeGenJob::emitReturn;

    // Nothing to return
    if (funcNode->returnType->typeInfo == g_TypeMgr.typeInfoVoid && node->childs.empty())
        return true;

    // For a return inside an inline block, just get the type of the expression
    if (node->ownerInline)
    {
        if (!node->childs.empty())
            node->typeInfo = node->childs.front()->typeInfo;
        return true;
    }

    // Check return type
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    bool lateRegister = funcNode->returnType->flags & AST_FORCE_FUNC_LATE_REGISTER;
    if (funcNode->returnType->typeInfo == g_TypeMgr.typeInfoVoid && !node->childs.empty())
    {
        // This is a short lambda without a specified return type. We now have it
        if ((funcNode->flags & AST_SHORT_LAMBDA) && !(funcNode->returnType->flags & AST_FUNC_RETURN_DEFINED))
        {
            typeInfoFunc->returnType = TypeManager::concreteType(node->childs.front()->typeInfo);
            if (typeInfoFunc->returnType->kind == TypeInfoKind::TypeList)
            {
                SWAG_CHECK(convertAssignementToStruct(context, funcNode->content, node->childs.front(), &funcNode->returnType));
                funcNode->returnType->flags |= AST_FORCE_FUNC_LATE_REGISTER;
                Ast::setForceConstType(funcNode->returnType);
                context->job->nodes.push_back(funcNode->returnType);
                context->result = ContextResult::NewChilds;
                return true;
            }

            typeInfoFunc->computeName();
            funcNode->returnType->typeInfo = typeInfoFunc->returnType;
            lateRegister                   = true;
        }
        else
        {
            // This is fine to return void
            node->byteCodeFct = nullptr;
        }
    }

    // Check types
    auto child = node->childs[0];
    SWAG_CHECK(checkIsConcrete(context, child));
    auto returnType = funcNode->returnType->typeInfo;
    SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, CASTFLAG_UNCONST));

    // When returning a struct, we need to know if postcopy or postmove are here, and wait for them to resolve
    if (returnType && returnType->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(waitForStructUserOps(context, funcNode->returnType));
        if (context->result == ContextResult::Pending)
            return true;
    }

    // If we are returning a local variable, we can do a move
    if (child->resolvedSymbolOverload && (child->resolvedSymbolOverload->flags & OVERLOAD_VAR_LOCAL))
        child->flags |= AST_FORCE_MOVE;

    // Propagate return
    auto scanNode = node;
    while (scanNode && scanNode != node->ownerFct->parent)
    {
        scanNode->flags |= AST_SCOPE_HAS_RETURN;
        if (scanNode->parent->kind == AstNodeKind::If)
        {
            auto ifNode = CastAst<AstIf>(scanNode->parent, AstNodeKind::If);
            if (ifNode->elseBlock != scanNode)
                break;
            if (!(ifNode->ifBlock->flags & AST_SCOPE_HAS_RETURN))
                break;
        }
        else if (scanNode->kind == AstNodeKind::While ||
                 scanNode->kind == AstNodeKind::Loop ||
                 scanNode->kind == AstNodeKind::For ||
                 scanNode->kind == AstNodeKind::Switch)
        {
            break;
        }

        scanNode = scanNode->parent;
    }

    while (scanNode && scanNode != node->ownerFct->parent)
    {
        scanNode->flags |= AST_FCT_HAS_RETURN;
        scanNode = scanNode->parent;
    }

    // Register symbol now that we have inferred the return type
    if (lateRegister)
    {
        typeInfoFunc->returnType = funcNode->returnType->typeInfo;
        SWAG_CHECK(registerFuncSymbol(context, funcNode));
    }

    return true;
}