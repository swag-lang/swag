#include "pch.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ThreadManager.h"

bool SemanticJob::setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, AstNode* funcNode, AstNode* parameters, bool forGenerics)
{
    if (!parameters || (funcNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC))
        return true;

    bool defaultValueDone = false;
    int  index            = 0;

    if (forGenerics)
    {
        typeInfo->genericParameters.clear();
        typeInfo->genericParameters.reserve((int) parameters->childs.size());
        typeInfo->flags |= TYPEINFO_GENERIC;
    }
    else
    {
        typeInfo->parameters.clear();
        typeInfo->parameters.reserve((int) parameters->childs.size());
    }

    auto sourceFile = context->sourceFile;
    for (auto param : parameters->childs)
    {
        auto nodeParam        = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        auto funcParam        = g_Allocator.alloc<TypeInfoParam>();
        funcParam->namedParam = param->name;
        funcParam->name       = param->typeInfo->name;
        funcParam->typeInfo   = param->typeInfo;
        funcParam->sizeOf     = param->typeInfo->sizeOf;
        funcParam->index      = index++;
        funcParam->node       = nodeParam;

        // Code is only valid for a macro or mixin
        auto paramType = nodeParam->typeInfo;
        if (paramType->kind == TypeInfoKind::Code)
        {
            SWAG_VERIFY(funcNode->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN), context->report({nodeParam, "type 'code' is only valid in a 'swag.macro' or 'swag.mixin' function"}));
        }

        // Not everything is possible for types for attributes
        if (param->ownerScope->kind == ScopeKind::Attribute)
        {
            SWAG_VERIFY(funcParam->typeInfo->kind == TypeInfoKind::Native || funcParam->typeInfo->kind == TypeInfoKind::Enum, context->report({nodeParam, format("invalid type '%s' for attribute parameter", funcParam->typeInfo->name.c_str())}));
            SWAG_VERIFY(funcParam->typeInfo->nativeType != NativeTypeKind::Any, context->report({nodeParam, format("invalid type '%s' for attribute parameter", funcParam->typeInfo->name.c_str())}));
        }

        parameters->inheritOrFlag(nodeParam->type, AST_IS_GENERIC);

        // Variadic must be the last one
        if (paramType->kind == TypeInfoKind::Variadic)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, "inline function has variadic arguments, this is not yet supported"}));

            typeInfo->flags |= TYPEINFO_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, "variadic argument should be the last one"});
        }
        else if (paramType->kind == TypeInfoKind::TypedVariadic)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, "inline function has variadic arguments, this is not yet supported"}));

            typeInfo->flags |= TYPEINFO_TYPED_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, "variadic argument should be the last one"});
        }

        // A struct/interface is forced to be a const reference
        else if (paramType->kind == TypeInfoKind::Struct || paramType->kind == TypeInfoKind::Interface)
        {
            auto typeRef          = g_Allocator.alloc<TypeInfoReference>();
            typeRef->flags        = paramType->flags | TYPEINFO_CONST;
            typeRef->pointedType  = paramType;
            typeRef->originalType = paramType;
            typeRef->computeName();
            nodeParam->typeInfo = typeRef;
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

bool SemanticJob::resolveAfterFuncDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    if (module->isBootStrap || module->isRuntime || !module->runContext.canSendCompilerMessages)
        return true;
    if (!module->hasCompilerFuncFor(CompilerMsgKind::SemanticFunc))
        return true;

    // Send user message
    auto node     = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    // Filter what we send
    if (!node->ownerScope->isGlobalOrImpl())
        return true;
    if (node->attributeFlags & ATTRIBUTE_GENERATED_FUNC)
        return true;
    if (node->flags & AST_IS_GENERIC)
        return true;

    ConcreteCompilerMessage msg;
    msg.kind        = CompilerMsgKind::SemanticFunc;
    msg.name.buffer = (void*) node->name.c_str();
    msg.name.count  = node->name.length();

    uint32_t storageOffset;
    auto&    typeTable = module->typeTable;
    SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, typeInfo, nullptr, &storageOffset, CONCRETE_SHOULD_WAIT | CONCRETE_FOR_COMPILER));
    if (context->result != ContextResult::Done)
        return true;
    msg.type = (ConcreteTypeInfo*) module->constantSegmentCompiler.address(storageOffset);

    module->sendCompilerMessage((ConcreteCompilerMessage*) &msg, context->job);
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
            Diagnostic diag({node, node->token, "#main directive already defined"});
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
        if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(node->flags & AST_GENERATED) && !(node->flags & AST_FROM_GENERIC))
            node->ownerScope->addPublicGenericFunc(node);
        SWAG_CHECK(setFullResolve(context, node));
        return true;
    }

    // An inline function will not have bytecode, so need to register public by hand now
    if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && (node->attributeFlags & ATTRIBUTE_INLINE))
        node->ownerScope->addPublicGenericFunc(node);

    node->byteCodeFct   = ByteCodeGenJob::emitLocalFuncDecl;
    typeInfo->stackSize = node->stackSize;

    // Check attributes
    if ((node->attributeFlags & ATTRIBUTE_FOREIGN) && node->content)
        return context->report({node, node->token, "function with the 'swag.foreign' attribute cannot have a body"});
    if (node->attributeFlags & ATTRIBUTE_FOREIGN)
        sourceFile->module->registerForeign(node);

    if (!(node->attributeFlags & ATTRIBUTE_GENERATED_FUNC))
    {
        if (node->flags & AST_SPECIAL_COMPILER_FUNC)
        {
            SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_INLINE), context->report({node, node->token, "compiler special function cannot have the 'swag.inline' attribute"}));
            SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_PUBLIC), context->report({node, node->token, "compiler special function cannot have the 'swag.public' attribute"}));
            SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_PRIVATE), context->report({node, node->token, "compiler special function cannot have the 'swag.private' attribute"}));
        }

        if (node->attributeFlags & ATTRIBUTE_TEST_FUNC)
        {
            SWAG_VERIFY(module->fromTestsFolder, context->report({node, node->token, "#test functions can only be used in a test module (in the './tests' folder of the workspace)"}));
            SWAG_VERIFY(node->returnType->typeInfo == g_TypeMgr.typeInfoVoid, context->report({node->returnType, "function with the 'swag.test' attribute cannot have a return value"}));
            SWAG_VERIFY(!node->parameters || node->parameters->childs.size() == 0, context->report({node->parameters, "function with the 'swag.test' attribute cannot have parameters"}));
        }

        if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        {
            SWAG_VERIFY(node->ownerScope->isGlobalOrImpl() && !(node->flags & AST_SPECIAL_COMPILER_FUNC), context->report({node, node->token, format("%s cannot be public", node->getNameForMessage().c_str())}));
        }
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
                return context->report({node, node->token, format("not all control paths of %s return a value", node->getNameForMessage().c_str())});
            return context->report({node, node->token, format("%s must return a value", node->getNameForMessage().c_str())});
        }
    }

    // Now the full function has been solved, so we wakeup jobs depending on that
    SWAG_CHECK(setFullResolve(context, node));

    // Ask for bytecode
    bool genByteCode = true;
    if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
        genByteCode = false;
    if (node->name[0] == '@' && !(node->flags & AST_DEFINED_INTRINSIC))
        genByteCode = false;
    if (node->attributeFlags & ATTRIBUTE_FOREIGN)
        genByteCode = false;
    if (node->flags & AST_IS_GENERIC)
        genByteCode = false;
    if (!node->content)
        genByteCode = false;
    if (genByteCode)
        ByteCodeGenJob::askForByteCode(context->job, node, 0);

    return true;
}

bool SemanticJob::setFullResolve(SemanticContext* context, AstFuncDecl* funcNode)
{
    scoped_lock lk(funcNode->mutex);
    funcNode->flags |= AST_FULL_RESOLVE;
    funcNode->dependentJobs.setRunning();
    return true;
}

bool SemanticJob::resolveFuncDeclType(SemanticContext* context)
{
    auto typeNode   = context->node;
    auto sourceFile = context->sourceFile;
    auto funcNode   = CastAst<AstFuncDecl>(typeNode->parent, AstNodeKind::FuncDecl);

    // This is a lambda that was waiting for a match.
    // We are now awake, so everything has been done already
    if (funcNode->pendingLambdaJob)
    {
        funcNode->pendingLambdaJob = nullptr;
        return true;
    }

    // If this is a #compiler function, we must have a flag mask as parameters
    if ((funcNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC) && funcNode->parameters)
    {
        auto parameters = funcNode->parameters;
        auto paramType  = TypeManager::concreteType(parameters->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
        SWAG_VERIFY(paramType->kind == TypeInfoKind::Enum, context->report({parameters, "'#compiler' function must have 'swag.CompilerMsgMask' as a parameter"}));
        paramType->computeScopedName();
        SWAG_VERIFY(paramType->scopedName == "swag.CompilerMsgMask", context->report({parameters, "'#compiler' function must have 'swag.CompilerMsgMask' as a parameter"}));
        SWAG_CHECK(evaluateConstExpression(context, parameters));
        if (context->result != ContextResult::Done)
            return true;
        funcNode->parameters->flags |= AST_NO_BYTECODE;
    }

    // Return type
    if (!typeNode->childs.empty())
        typeNode->typeInfo = typeNode->childs.front()->typeInfo;
    else
        typeNode->typeInfo = g_TypeMgr.typeInfoVoid;

    // If the function returns a reference, then transform it to a normal return type if 
    // this is not a reference to a "by copy" type
    // const &u32 => u32 etc...
    if (typeNode->typeInfo->kind == TypeInfoKind::Reference)
    {
        auto typeRef = CastTypeInfo<TypeInfoReference>(typeNode->typeInfo, TypeInfoKind::Reference);
        SWAG_ASSERT(typeRef->pointedType->kind != TypeInfoKind::Reference); // Can happen ?
        if (!(typeRef->pointedType->flags & TYPEINFO_RETURN_BY_COPY))
        {
            typeNode->typeInfo = typeRef->pointedType;
        }
    }

    // Collect function attributes
    SWAG_ASSERT(funcNode->semanticState == AstNodeResolveState::ProcessingChilds);
    SWAG_CHECK(collectAttributes(context, funcNode->collectAttributes, funcNode->parentAttributes, funcNode, AstNodeKind::FuncDecl, funcNode->attributeFlags));

    if (funcNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
        funcNode->flags |= AST_CONST_EXPR | AST_PURE;

    if (!(funcNode->flags & AST_FROM_GENERIC))
    {
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

        if (funcNode->flags & AST_SPECIAL_COMPILER_FUNC)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MACRO), context->report({funcNode, funcNode->token, format("function '%s' cannot be marked with 'swag.macro' attribute", funcNode->token.text.c_str())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MIXIN), context->report({funcNode, funcNode->token, format("function '%s' cannot be marked with 'swag.mixin' attribute", funcNode->token.text.c_str())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, format("function '%s' cannot be marked with 'swag.inline' attribute", funcNode->token.text.c_str())}));
        }
    }

    // The function wants to return something, but has the 'swag.noreturn' attribute
    if (!typeNode->typeInfo->isNative(NativeTypeKind::Void) && (funcNode->attributeFlags & ATTRIBUTE_NORETURN))
        return context->report({typeNode, "function cannot have a return type because it is flagged with the 'swag.noreturn' attribute"});

    // Register symbol with its type
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

    if (!(funcNode->flags & AST_FROM_GENERIC))
    {
        // Determine if function is generic
        if (funcNode->parameters)
            funcNode->inheritOrFlag(funcNode->parameters, AST_IS_GENERIC);
        if (funcNode->ownerStructScope && (funcNode->ownerStructScope->owner->flags & AST_IS_GENERIC))
            funcNode->flags |= AST_IS_GENERIC;
        if (funcNode->ownerFct && (funcNode->ownerFct->flags & AST_IS_GENERIC))
            funcNode->flags |= AST_IS_GENERIC;
        if (funcNode->genericParameters)
            funcNode->flags |= AST_IS_GENERIC;
        if (funcNode->flags & AST_IS_GENERIC)
            typeInfo->flags |= TYPEINFO_GENERIC;

        // Register parameters
        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->genericParameters, true));
        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->parameters, false));
    }

    // If a lambda function will wait for a match, then no need to deduce the return type
    // It will be done in the same way as parameters
    if (!(funcNode->flags & AST_IS_GENERIC))
    {
        if ((funcNode->flags & AST_PENDING_LAMBDA_TYPING) && (funcNode->flags & AST_SHORT_LAMBDA) && (typeNode->typeInfo == g_TypeMgr.typeInfoVoid))
        {
            typeNode->typeInfo = g_TypeMgr.typeInfoUndefined;
            funcNode->flags &= ~AST_SHORT_LAMBDA;
        }
    }

    // Short lambda without a return type we must deduced
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
        typeInfo->returnType->kind != TypeInfoKind::TypeListTuple &&
        typeInfo->returnType->kind != TypeInfoKind::TypeListArray &&
        typeInfo->returnType->kind != TypeInfoKind::Struct &&
        typeInfo->returnType->kind != TypeInfoKind::Generic &&
        typeInfo->returnType->kind != TypeInfoKind::Alias &&
        typeInfo->returnType->kind != TypeInfoKind::Lambda &&
        typeInfo->returnType->kind != TypeInfoKind::Slice &&
        typeInfo->returnType->kind != TypeInfoKind::Enum &&
        typeInfo->returnType->kind != TypeInfoKind::Interface &&
        typeInfo->returnType->kind != TypeInfoKind::Reference &&
        typeInfo->returnType->kind != TypeInfoKind::Array &&
        typeInfo->returnType->kind != TypeInfoKind::Pointer)
        return context->report({typeNode->childs.front(), format("a function cannot return a value of type '%s'", typeInfo->returnType->name.c_str())});

    typeInfo->computeName();

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

    // If this is a lambda waiting for a match to know the types of its parameters, need to wait
    // Function SemanticJob::setSymbolMatch will wake us up as soon as a valid match is found
    if (funcNode->flags & AST_PENDING_LAMBDA_TYPING)
    {
        if (!(funcNode->flags & AST_IS_GENERIC))
        {
            funcNode->pendingLambdaJob = context->job;
            context->job->setPending(nullptr, "AST_PENDING_LAMBDA_TYPING", funcNode, nullptr);
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

    // Register method
    if (!(symbolFlags & OVERLOAD_INCOMPLETE) &&
        funcNode->ownerStructScope &&
        funcNode->parent->kind != AstNodeKind::CompilerAst &&
        funcNode->parent->kind != AstNodeKind::CompilerRun &&
        !(funcNode->flags & AST_FROM_GENERIC) &&
        (funcNode->ownerScope->kind == ScopeKind::Struct) &&
        (funcNode->ownerStructScope->owner->typeInfo->kind == TypeInfoKind::Struct))
    {
        SWAG_ASSERT(funcNode->methodParam);
        funcNode->methodParam->attributes = typeFunc->attributes;
        auto typeStruct                   = CastTypeInfo<TypeInfoStruct>(funcNode->ownerStructScope->owner->typeInfo, TypeInfoKind::Struct);
        context->job->decreaseMethodCount(typeStruct);
    }

    // If we have sub functions, then now we can solve them
    for (auto f : funcNode->subFunctions)
    {
        scoped_lock lk(f->mutex);
        f->flags &= ~AST_NO_SEMANTIC;

        // If AST_DONE_FILE_JOB_PASS is set, then the file job has already seen the sub function, ignored it
        // because of AST_NO_SEMANTIC, but the attribute context is ok. So we need to trigger the job by hand.
        // If AST_DONE_FILE_JOB_PASS is not set, then we just have to remove the AST_NO_SEMANTIC flag, and the
        // file job will trigger the resolve itself
        if (f->doneFlags & AST_DONE_FILE_JOB_PASS)
        {
            auto job          = g_Pool_semanticJob.alloc();
            job->sourceFile   = context->sourceFile;
            job->module       = context->sourceFile->module;
            job->dependentJob = context->job->dependentJob;
            job->nodes.push_back(f);
            g_ThreadMgr.addJob(job);
        }
    }

    return true;
}

bool SemanticJob::resolveFuncCallParams(SemanticContext* context)
{
    auto node = context->node;
    node->inheritOrFlag(AST_IS_GENERIC);
    node->inheritAndFlag2(AST_CONST_EXPR, AST_PURE);
    return true;
}

bool SemanticJob::resolveFuncCallParam(SemanticContext* context)
{
    auto node      = context->node;
    auto child     = node->childs.front();
    node->typeInfo = child->typeInfo;

    // Force const if necessary
    // func([.., ...]) must be const
    if (child->kind == AstNodeKind::ExpressionList)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
        if (typeList->kind == TypeInfoKind::TypeListArray)
            node->typeInfo->setConst();
    }

    node->byteCodeFct = ByteCodeGenJob::emitFuncCallParam;

    // Can be called for generic parameters in type definition, in that case, we are a type, so no
    // test for concrete must be done
    bool checkForConcrete = true;
    if (node->parent->flags & AST_NO_BYTECODE)
        checkForConcrete = false;
    if (checkForConcrete)
    {
        SWAG_CHECK(checkIsConcreteOrType(context, child));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = child->typeInfo;
        node->flags |= AST_R_VALUE;
    }

    node->inheritComputedValue(child);
    node->inheritOrFlag(child, AST_CONST_EXPR | AST_IS_GENERIC | AST_VALUE_IS_TYPEINFO | AST_PURE);

    // Inherit the original type in case of computed values, in order to make the cast if necessary
    if (node->flags & AST_VALUE_COMPUTED)
        node->castedTypeInfo = child->castedTypeInfo;

    if (checkForConcrete)
    {
        SWAG_CHECK(evaluateConstExpression(context, node));
        if (context->result == ContextResult::Pending)
            return true;
    }

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

    auto node     = CastAst<AstReturn>(context->node, AstNodeKind::Return);
    auto funcNode = node->ownerFct;

    // For a return inside an inline block, take the original function, except if it is flags with 'swag.noreturn'
    if (node->ownerInline)
    {
        if (!(node->ownerInline->func->attributeFlags & ATTRIBUTE_NORETURN))
        {
            node->flags |= AST_EMBEDDED_RETURN;
            funcNode = node->ownerInline->func;
        }
    }

    node->byteCodeFct = ByteCodeGenJob::emitReturn;

    // Nothing to return
    if (funcNode->returnType->typeInfo == g_TypeMgr.typeInfoVoid && node->childs.empty())
        return true;

    // Check return type
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    bool lateRegister = funcNode->returnType->flags & AST_FORCE_FUNC_LATE_REGISTER;
    if (funcNode->returnType->typeInfo == g_TypeMgr.typeInfoVoid && !node->childs.empty())
    {
        // This is a short lambda without a specified return type. We now have it
        if ((funcNode->flags & AST_SHORT_LAMBDA) && !(funcNode->returnType->flags & AST_FUNC_RETURN_DEFINED))
        {
            typeInfoFunc->returnType = TypeManager::concreteType(node->childs.front()->typeInfo, CONCRETE_FUNC);
            auto concreteReturn      = TypeManager::concreteType(typeInfoFunc->returnType);
            if (concreteReturn->kind == TypeInfoKind::TypeListTuple)
            {
                SWAG_CHECK(convertAssignementToStruct(context, funcNode->content, node->childs.front(), &funcNode->returnType));
                funcNode->returnType->flags |= AST_FORCE_FUNC_LATE_REGISTER;
                Ast::setForceConstType(funcNode->returnType);
                context->job->nodes.push_back(funcNode->returnType);
                context->result = ContextResult::NewChilds;
                return true;
            }

            // Pure function automatic detection just in case of short lambdas, because we need to parse the full content
            // of the function to know if it's pure or not. In case of normal function, we emit its type before parsing
            // the content
            funcNode->content->inheritAndFlag1(AST_PURE);
            if (funcNode->content->flags & AST_PURE)
                funcNode->flags |= AST_CONST_EXPR | AST_PURE;

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

    SWAG_VERIFY(!node->childs.empty(), context->report({node, node->token, format("missing return value of type '%s'", funcNode->returnType->typeInfo->name.c_str())}));
    auto returnType = funcNode->returnType->typeInfo;

    // Check types
    auto child = node->childs[0];
    SWAG_CHECK(checkIsConcrete(context, child));

    // Be sure we do not specify a return value, and the function does not have a return type
    // (better error message than just letting the makeCompatibles do its job)
    auto concreteType = TypeManager::concreteType(child->typeInfo);
    if (returnType->isNative(NativeTypeKind::Void) && !concreteType->isNative(NativeTypeKind::Void))
        return context->report({child, format("returning a value of type '%s', but the function does not declare a return type", concreteType->name.c_str())});

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
    {
        child->flags |= AST_FORCE_MOVE | AST_NO_RIGHT_DROP;
        node->forceNoDrop = child->resolvedSymbolOverload;
    }

    // Propagate return
    AstNode* scanNode = node;
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

bool SemanticJob::makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    CloneContext cloneContext;

    // The content will be inline in its separated syntax block
    auto inlineNode                   = Ast::newInline(identifier->sourceFile, identifier);
    inlineNode->attributeFlags        = funcDecl->attributeFlags;
    inlineNode->func                  = funcDecl;
    inlineNode->scope                 = identifier->ownerScope;
    inlineNode->alternativeScopes     = funcDecl->alternativeScopes;
    inlineNode->alternativeScopesVars = funcDecl->alternativeScopesVars;

    // We need to add the parent scope of the inline function (the global one), in order for
    // the inline content to be resolved in the same context as the original function
    auto globalScope = funcDecl->ownerScope;
    while (!globalScope->isGlobalOrImpl())
        globalScope = globalScope->parentScope;
    inlineNode->alternativeScopes.push_back(globalScope);

    // If function has generic parameters, then the block resolution of identifiers needs to be able to find the generic parameters
    // so we register all those generic parameters in a special scope (we cannot just register the scope of the function because
    // they are other stuff here)
    if (funcDecl->genericParameters)
    {
        Scope* scope = Ast::newScope(inlineNode, "", ScopeKind::Statement, nullptr);
        inlineNode->alternativeScopes.push_back(scope);
        for (auto child : funcDecl->genericParameters->childs)
        {
            auto symName = scope->symTable.registerSymbolNameNoLock(context, child, SymbolKind::Variable);
            symName->addOverloadNoLock(child, child->typeInfo, &child->resolvedSymbolOverload->computedValue);
            symName->cptOverloads = 0; // Simulate a done resolution
        }
    }

    Scope* newScope = identifier->ownerScope;
    if (!(funcDecl->attributeFlags & ATTRIBUTE_MIXIN))
    {
        newScope          = Ast::newScope(inlineNode, format("__inline%d", identifier->ownerScope->childScopes.size()), ScopeKind::Inline, identifier->ownerScope);
        inlineNode->scope = newScope;
    }

    // Clone the function body
    cloneContext.parent         = inlineNode;
    cloneContext.ownerInline    = inlineNode;
    cloneContext.ownerFct       = identifier->ownerFct;
    cloneContext.ownerBreakable = identifier->ownerBreakable;
    cloneContext.parentScope    = newScope;

    // Register all aliases
    if (identifier->kind == AstNodeKind::Identifier)
    {
        auto id  = CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier);
        int  idx = 0;
        for (const auto& alias : id->aliasNames)
        {
            cloneContext.replaceNames[format("@alias%d", idx++)] = alias;
        }
    }

    cloneContext.forceFlags |= identifier->flags & AST_NO_BACKEND;
    cloneContext.forceFlags |= identifier->flags & AST_RUN_BLOCK;

    auto newContent               = funcDecl->content->clone(cloneContext);
    newContent->byteCodeBeforeFct = nullptr;
    newContent->flags &= ~AST_NO_SEMANTIC;

    // Need to reevaluate the identifier, if this is an identifier, cause the makeInline can be called
    // for something else, like a loop node for example (opCount). In that case, we let the specific node
    // deal with th (re)evaluation.
    if (identifier->kind == AstNodeKind::Identifier)
    {
        identifier->semanticState = AstNodeResolveState::Enter;
        identifier->bytecodeState = AstNodeResolveState::Enter;
    }

    return true;
}

bool SemanticJob::makeInline(SemanticContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    SWAG_CHECK(makeInline((JobContext*) context, funcDecl, identifier));
    context->result = ContextResult::NewChilds;
    return true;
}