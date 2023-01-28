#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Os.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool SemanticJob::setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, AstNode* funcNode, AstNode* parameters, bool forGenerics)
{
    if (!parameters || (funcNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC))
        return true;

    bool defaultValueDone = false;
    int  index            = 0;

    // If we have a tuple as a default parameter, without a user defined type, then we need to convert it to a tuple struct
    // and wait for the type to be solved.
    for (auto param : parameters->childs)
    {
        auto nodeParam = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        if (!nodeParam->assignment)
            continue;
        if (nodeParam->assignment->kind != AstNodeKind::ExpressionList)
            continue;

        auto nodeExpr = CastAst<AstExpressionList>(nodeParam->assignment, AstNodeKind::ExpressionList);
        if (!(nodeExpr->specFlags & AST_SPEC_EXPRLIST_FOR_TUPLE))
            continue;

        if (!nodeParam->type)
        {
            nodeParam->semFlags |= AST_SEM_TUPLE_CONVERT;
            SWAG_ASSERT(nodeParam->typeInfo->isListTuple());
            SWAG_CHECK(convertLiteralTupleToStructDecl(context, nodeParam, nodeParam->assignment, &nodeParam->type));
            context->result = ContextResult::NewChilds;
            context->job->nodes.push_back(nodeParam->type);
            return true;
        }
        else if (nodeParam->semFlags & AST_SEM_TUPLE_CONVERT)
        {
            SWAG_ASSERT(nodeParam->resolvedSymbolOverload);
            nodeParam->typeInfo                         = nodeParam->type->typeInfo;
            nodeParam->resolvedSymbolOverload->typeInfo = nodeParam->typeInfo;
        }
    }

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

    AstNode* firstParamWithDef = nullptr;
    auto     sourceFile        = context->sourceFile;
    for (auto param : parameters->childs)
    {
        auto nodeParam        = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        auto funcParam        = g_TypeMgr->makeParam();
        funcParam->namedParam = param->token.text;
        funcParam->name       = param->typeInfo->name;
        funcParam->typeInfo   = param->typeInfo;
        funcParam->index      = index++;
        funcParam->declNode   = nodeParam;
        funcParam->attributes = nodeParam->attributes;

        auto paramType     = nodeParam->typeInfo;
        auto paramNodeType = nodeParam->type ? nodeParam->type : nodeParam;

        // Code is only valid for a macro or mixin
        if (paramType->isCode())
            SWAG_VERIFY(funcNode->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN), context->report({paramNodeType, Err(Err0729)}));

        // Not everything is possible for types for attributes
        if (param->ownerScope->kind == ScopeKind::Attribute)
        {
            SWAG_VERIFY(!funcParam->typeInfo->isAny(), context->report({nodeParam, Fmt(Err(Err0731), funcParam->typeInfo->getDisplayNameC())}));

            if (!funcParam->typeInfo->isNative() &&
                !funcParam->typeInfo->isEnum() &&
                !funcParam->typeInfo->isPointerToTypeInfo() &&
                !funcParam->typeInfo->isTypedVariadic())
            {
                return context->report({nodeParam, Fmt(Err(Err0731), funcParam->typeInfo->getDisplayNameC())});
            }

            if (funcParam->typeInfo->isTypedVariadic())
            {
                auto typeVar = CastTypeInfo<TypeInfoVariadic>(funcParam->typeInfo, TypeInfoKind::TypedVariadic);
                SWAG_VERIFY(!typeVar->isAny(), context->report({paramNodeType, Fmt(Err(Err0731), funcParam->typeInfo->getDisplayNameC())}));
            }
        }

        parameters->inheritOrFlag(nodeParam->type, AST_IS_GENERIC);

        // Variadic must be the last one
        if (paramType->isVariadic())
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, Err(Err0733)}));
            typeInfo->flags |= TYPEINFO_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, Err(Err0734)});
        }
        else if (paramType->isTypedVariadic())
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, Err(Err0733)}));
            typeInfo->flags |= TYPEINFO_TYPED_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, Err(Err0734)});
        }
        else if (paramType->isCVariadic())
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, Err(Err0733)}));
            typeInfo->flags |= TYPEINFO_C_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, Err(Err0734)});
        }

        // Default parameter value
        if (nodeParam->assignment)
        {
            if (!defaultValueDone)
            {
                defaultValueDone               = true;
                typeInfo->firstDefaultValueIdx = index - 1;
                firstParamWithDef              = nodeParam;
            }

            if (nodeParam->assignment->kind == AstNodeKind::CompilerSpecialFunction)
            {
                switch (nodeParam->assignment->token.id)
                {
                case TokenId::CompilerCallerLocation:
                case TokenId::CompilerCallerFunction:
                case TokenId::CompilerBuildCfg:
                case TokenId::CompilerArch:
                case TokenId::CompilerOs:
                case TokenId::CompilerSwagOs:
                case TokenId::CompilerBackend:
                    break;

                default:
                    context->report({nodeParam->assignment, Fmt(Err(Err0737), nodeParam->assignment->token.ctext())});
                    break;
                }
            }
        }
        else if (!nodeParam->typeInfo->isCode())
        {
            auto name = SemanticJob::getTheNiceParameterRank(index);
            if (defaultValueDone)
            {
                Diagnostic diag{nodeParam, Fmt(Err(Err0738), name.c_str())};
                diag.hint = Hnt(Hnt0089);
                diag.addRange(firstParamWithDef, Hnt(Hnt0088));
                return context->report(diag);
            }
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

bool SemanticJob::sendCompilerMsgFuncDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    // Filter what we send
    if (module->kind == ModuleKind::BootStrap || module->kind == ModuleKind::Runtime)
        return true;
    if (sourceFile->imported && !sourceFile->isEmbbeded)
        return true;
    if (!context->node->ownerScope->isGlobalOrImpl())
        return true;
    if (context->node->attributeFlags & ATTRIBUTE_GENERATED_FUNC)
        return true;
    if (context->node->flags & (AST_IS_GENERIC | AST_FROM_GENERIC))
        return true;

    auto node     = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    CompilerMessage msg      = {0};
    msg.concrete.kind        = CompilerMsgKind::SemFunctions;
    msg.concrete.name.buffer = node->token.text.buffer;
    msg.concrete.name.count  = node->token.text.length();
    msg.typeInfo             = typeInfo;
    SWAG_CHECK(module->postCompilerMessage(context, msg));

    return true;
}

bool SemanticJob::resolveFuncDeclAfterSI(SemanticContext* context)
{
    auto saveNode = context->node;
    if (context->node->parent->kind == AstNodeKind::Inline)
    {
        auto node     = CastAst<AstInline>(context->node->parent, AstNodeKind::Inline);
        context->node = node->func;
    }
    else
    {
        auto node = CastAst<AstFuncDecl>(context->node->parent, AstNodeKind::FuncDecl);
        SWAG_ASSERT(node->content == context->node);
        context->node = node;
    }

    resolveFuncDecl(context);
    resolveScopedStmtAfter(context);
    context->node = saveNode;
    return true;
}

bool SemanticJob::resolveFuncDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto node       = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    // Only one main per module !
    if (node->attributeFlags & ATTRIBUTE_MAIN_FUNC)
    {
        ScopedLock lk(sourceFile->module->mutexFile);
        if (sourceFile->module->mainIsDefined)
        {
            Diagnostic diag{node, Err(Err0739)};
            Diagnostic note{module->mainIsDefined, Nte(Nte0036), DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        sourceFile->module->mainIsDefined = node;
    }

    // No semantic on a generic function, or a macro
    if (node->flags & AST_IS_GENERIC)
    {
        if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(node->flags & AST_GENERATED))
            node->ownerScope->addPublicNode(node);
        return true;
    }

    // Check that there is no 'hole' in alias names
    if (node->aliasMask && (node->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN)))
    {
        auto mask = node->aliasMask;
        auto maxN = OS::bitcountlz(node->aliasMask);
        for (uint32_t n = 0; n < 32 - maxN; n++)
        {
            if ((mask & 1) == 0)
                return context->report({node, Fmt(Err(Err0741), node->token.ctext(), n)});
            mask >>= 1;
        }
    }

    if (node->attributeFlags & ATTRIBUTE_MACRO)
    {
        if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(node->flags & AST_GENERATED) && !(node->flags & AST_FROM_GENERIC))
            node->ownerScope->addPublicNode(node);
        SWAG_CHECK(setFullResolve(context, node));
        return true;
    }

    // An inline function will not have bytecode, so need to register public by hand now
    if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && (node->attributeFlags & ATTRIBUTE_INLINE) && !(node->flags & AST_FROM_GENERIC))
        node->ownerScope->addPublicNode(node);

    node->byteCodeFct   = ByteCodeGenJob::emitLocalFuncDecl;
    typeInfo->stackSize = node->stackSize;

    // Check attributes
    if ((node->isForeign()) && node->content)
        return context->report({node, Err(Err0742)});

    if (!(node->attributeFlags & ATTRIBUTE_GENERATED_FUNC))
    {
        if (node->attributeFlags & ATTRIBUTE_SHARP_FUNC)
        {
            SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_INLINE), context->report({node, Err(Err0743)}));
        }

        if (node->attributeFlags & ATTRIBUTE_TEST_FUNC)
        {
            SWAG_VERIFY(module->kind == ModuleKind::Test, context->report({node, Err(Err0744)}));
            SWAG_VERIFY(node->returnType->typeInfo == g_TypeMgr->typeInfoVoid, context->report({node->returnType, Err(Err0745), Hnt(Hnt0026)}));
            SWAG_VERIFY(!node->parameters || node->parameters->childs.size() == 0, context->report({node->parameters, Err(Err0746), Hnt(Hnt0026)}));
        }

        if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        {
            SWAG_VERIFY(node->ownerScope->isGlobalOrImpl(), context->report({node, Fmt(Err(Err0747), node->getDisplayNameC())}));
        }
    }

    // Can be null for intrinsics etc...
    if (node->content)
    {
        node->content->allocateExtension(ExtensionKind::ByteCode);
        node->content->extension->bytecode->byteCodeBeforeFct = ByteCodeGenJob::emitBeforeFuncDeclContent;
    }

    // Do we have a return value
    if (node->content && node->returnType && node->returnType->typeInfo != g_TypeMgr->typeInfoVoid)
    {
        if (!(node->content->flags & AST_NO_SEMANTIC))
        {
            if (!(node->semFlags & AST_SEM_SCOPE_HAS_RETURN))
            {
                if (node->semFlags & AST_SEM_FCT_HAS_RETURN)
                    return context->report({node, node->token, Fmt(Err(Err0748), node->getDisplayNameC())});
                if (!node->returnType->childs.empty())
                    return context->report({node->returnType->childs.front(), Fmt(Err(Err0749), node->getDisplayNameC(), node->returnType->typeInfo->getDisplayNameC())});
                return context->report({node, node->token, Fmt(Err(Err0749), node->getDisplayNameC(), node->returnType->typeInfo->getDisplayNameC())});
            }
        }
    }

    // Content semantic can have been disabled (#selectifonce). In that case, we're not done yet, so
    // do not set the FULL_RESOLVE flag and do not generate bytecode
    if (node->content && (node->content->flags & AST_NO_SEMANTIC))
    {
        ScopedLock lk(node->funcMutex);
        node->funcFlags |= FUNC_FLAG_PARTIAL_RESOLVE;
        node->dependentJobs.setRunning();
        return true;
    }

    // Flag the function with AST_SPEC_FUNCDECL_IS_IMPL_MTD if this is an interface implementation method
    if (node->ownerScope && node->ownerScope->kind == ScopeKind::Impl)
    {
        auto implNode = CastAst<AstImpl>(node->ownerScope->owner, AstNodeKind::Impl);
        if (implNode->identifierFor)
        {
            auto forId = implNode->identifier->childs.back();

            // Be sure interface has been fully solved
            {
                ScopedLock lk(forId->mutex);
                ScopedLock lk1(forId->resolvedSymbolName->mutex);
                if (forId->resolvedSymbolName->cptOverloads)
                {
                    context->job->waitSymbolNoLock(forId->resolvedSymbolName);
                    return true;
                }
            }

            {
                auto       typeBaseInterface = CastTypeInfo<TypeInfoStruct>(forId->resolvedSymbolOverload->typeInfo, TypeInfoKind::Interface);
                auto       typeInterface     = CastTypeInfo<TypeInfoStruct>(typeBaseInterface->itable, TypeInfoKind::Struct);
                ScopedLock lk(typeInterface->mutex);

                // We need to search the function (as a variable) in the interface
                // If not found, then this is a normal function...
                auto symbolName = typeInterface->findChildByNameNoLock(node->token.text); // O(n) !
                if (symbolName)
                {
                    node->fromItfSymbol = symbolName;
                }
            }
        }
    }

    // Warnings
    SWAG_CHECK(warnUnusedVariables(context, node->scope));

    // Now the full function has been solved, so we wakeup jobs depending on that
    SWAG_CHECK(setFullResolve(context, node));

    // Ask for bytecode
    bool genByteCode = true;
    if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
        genByteCode = false;
    if (node->token.text[0] == '@' && !(node->flags & AST_DEFINED_INTRINSIC))
        genByteCode = false;
    if (node->isForeign())
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
    ScopedLock lk(funcNode->funcMutex);
    funcNode->funcFlags |= FUNC_FLAG_FULL_RESOLVE | FUNC_FLAG_PARTIAL_RESOLVE;
    funcNode->dependentJobs.setRunning();
    return true;
}

void SemanticJob::setFuncDeclParamsIndex(AstFuncDecl* funcNode)
{
    if (funcNode->parameters)
    {
        int storageIndex = 0;
        if (funcNode->typeInfo->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
        {
            auto param             = funcNode->parameters->childs.back();
            auto resolved          = param->resolvedSymbolOverload;
            resolved->storageIndex = 0; // Always the first one
            storageIndex += 2;
        }

        auto childSize = funcNode->parameters->childs.size();
        for (int i = 0; i < childSize; i++)
        {
            if ((i == childSize - 1) && funcNode->typeInfo->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
                break;
            auto param             = funcNode->parameters->childs[i];
            auto resolved          = param->resolvedSymbolOverload;
            resolved->storageIndex = storageIndex;

            auto typeParam    = TypeManager::concreteType(resolved->typeInfo);
            int  numRegisters = typeParam->numRegisters();
            storageIndex += numRegisters;
        }
    }
}

bool SemanticJob::resolveFuncDeclType(SemanticContext* context)
{
    auto typeNode = context->node;
    auto funcNode = CastAst<AstFuncDecl>(typeNode->parent, AstNodeKind::FuncDecl);

    // This is a lambda that was waiting for a match.
    // We are now awake, so everything has been done already
    if (funcNode->pendingLambdaJob)
    {
        setFuncDeclParamsIndex(funcNode);
        funcNode->pendingLambdaJob = nullptr;
        return true;
    }

    // If this is a #message function, we must have a flag mask as parameters
    if ((funcNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC) && funcNode->parameters)
    {
        auto parameters = funcNode->parameters;
        auto paramType  = TypeManager::concreteType(parameters->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
        SWAG_VERIFY(paramType->isEnum(), context->report({parameters, Fmt(Err(Err0804), paramType->getDisplayNameC()), Diagnostic::isType(paramType)}));
        paramType->computeScopedName();
        SWAG_VERIFY(paramType->scopedName == g_LangSpec->name_Swag_CompilerMsgMask, context->report({parameters, Fmt(Err(Err0804), paramType->getDisplayNameC())}));
        SWAG_CHECK(evaluateConstExpression(context, parameters));
        if (context->result != ContextResult::Done)
            return true;
        funcNode->parameters->flags |= AST_NO_BYTECODE;
    }

    // Return type
    if (!typeNode->childs.empty())
    {
        auto front         = typeNode->childs.front();
        typeNode->typeInfo = front->typeInfo;
        if (typeNode->typeInfo->isVoid())
        {
            Diagnostic diag{typeNode->sourceFile, typeNode->token.startLocation, front->token.endLocation, Err(Err0732)};
            diag.hint = Hnt(Hnt0026);
            return context->report(diag);
        }
    }
    else
    {
        typeNode->typeInfo = g_TypeMgr->typeInfoVoid;

        // :DeduceLambdaType
        if (funcNode->makePointerLambda && funcNode->makePointerLambda->parent->kind == AstNodeKind::AffectOp && !(funcNode->flags & AST_SHORT_LAMBDA))
        {
            auto op = CastAst<AstOp>(funcNode->makePointerLambda->parent, AstNodeKind::AffectOp);
            if (op->deducedLambdaType)
                typeNode->typeInfo = op->deducedLambdaType->returnType;
            else
            {
                auto front = op->childs.front();
                SWAG_ASSERT(front->typeInfo);
                if (front->typeInfo->isLambdaClosure())
                {
                    auto typeFct       = CastTypeInfo<TypeInfoFuncAttr>(front->typeInfo, TypeInfoKind::LambdaClosure);
                    typeNode->typeInfo = typeFct->returnType;
                }
            }
        }
    }

    // Collect function attributes
    auto       typeInfo = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    ScopedLock lkT(typeInfo->mutex);

    SWAG_ASSERT(funcNode->semanticState == AstNodeResolveState::ProcessingChilds);
    SWAG_CHECK(collectAttributes(context, funcNode, &typeInfo->attributes));

    // Check attributes
    if (funcNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
        funcNode->flags |= AST_CONST_EXPR;
    if (funcNode->ownerFct)
        funcNode->attributeFlags |= funcNode->ownerFct->attributeFlags & ATTRIBUTE_COMPILER;

    if (!(funcNode->flags & AST_FROM_GENERIC) && !(funcNode->doneFlags & AST_DONE_CHECK_ATTR))
    {
        // Can be called multiple times in case of a mixin/macro inside another inlined function
        funcNode->doneFlags |= AST_DONE_CHECK_ATTR;

        if (funcNode->attributeFlags & ATTRIBUTE_MACRO)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, Fmt(Err(Err0757), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MIXIN), context->report({funcNode, funcNode->token, Fmt(Err(Err0758), funcNode->getDisplayNameC())}));
            funcNode->attributeFlags |= ATTRIBUTE_INLINE;
        }

        if (funcNode->attributeFlags & ATTRIBUTE_MIXIN)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, Fmt(Err(Err0759), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MACRO), context->report({funcNode, funcNode->token, Fmt(Err(Err0760), funcNode->getDisplayNameC())}));
            funcNode->attributeFlags |= ATTRIBUTE_INLINE;
            funcNode->attributeFlags |= ATTRIBUTE_MACRO;
        }

        if (funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MACRO), context->report({funcNode, funcNode->token, Fmt(Err(Err0761), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MIXIN), context->report({funcNode, funcNode->token, Fmt(Err(Err0762), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, Fmt(Err(Err0763), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_NOT_GENERIC), context->report({funcNode, funcNode->token, Fmt(Err(Err0860), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_NO_RETURN), context->report({funcNode, funcNode->token, Fmt(Err(Err0512), funcNode->getDisplayNameC())}));
        }
    }

    SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_COMPLETE) || funcNode->token.text == g_LangSpec->name_opAffect || funcNode->token.text == g_LangSpec->name_opAffectSuffix, context->report({funcNode, funcNode->token, Fmt(Err(Err0753), funcNode->token.ctext())}));
    SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_IMPLICIT) || funcNode->token.text == g_LangSpec->name_opAffect || funcNode->token.text == g_LangSpec->name_opAffectSuffix || funcNode->token.text == g_LangSpec->name_opCast, context->report({funcNode, funcNode->token, Fmt(Err(Err0754), funcNode->token.ctext())}));
    SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_NO_RETURN) || (funcNode->attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO)), context->report({funcNode, funcNode->token, Fmt(Err(Err0755), funcNode->getDisplayNameC())}));

    // Implicit attribute cannot be used on a generic function
    // This is because "extra" generic parameters must be specified and not deduced, and this is not possible for an implicit cast
    if (funcNode->attributeFlags & ATTRIBUTE_IMPLICIT && (funcNode->flags & (AST_IS_GENERIC | AST_FROM_GENERIC)))
    {
        bool ok = false;
        if (funcNode->token.text == g_LangSpec->name_opAffectSuffix && funcNode->genericParameters->childs.size() <= 1)
            ok = true;
        if (funcNode->token.text == g_LangSpec->name_opAffect && !funcNode->genericParameters)
            ok = true;
        if (!ok)
            return context->report({funcNode, funcNode->token, Fmt(Err(Err0756), funcNode->getDisplayNameC())});
    }

    if (!(funcNode->flags & AST_FROM_GENERIC))
    {
        // Determine if function is generic
        if (funcNode->ownerStructScope && (funcNode->ownerStructScope->owner->flags & AST_IS_GENERIC) && !(funcNode->attributeFlags & ATTRIBUTE_NOT_GENERIC))
            funcNode->flags |= AST_IS_GENERIC;
        if (funcNode->ownerFct && (funcNode->ownerFct->flags & AST_IS_GENERIC) && !(funcNode->attributeFlags & ATTRIBUTE_NOT_GENERIC))
            funcNode->flags |= AST_IS_GENERIC;

        if (funcNode->parameters)
            funcNode->inheritOrFlag(funcNode->parameters, AST_IS_GENERIC);

        if (funcNode->genericParameters)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_NOT_GENERIC), context->report({funcNode->genericParameters, Fmt(Err(Err0752), funcNode->token.ctext()), Hnt(Hnt0026)}));
            funcNode->flags |= AST_IS_GENERIC;
        }

        if (funcNode->flags & AST_IS_GENERIC)
            typeInfo->flags |= TYPEINFO_GENERIC;

        if ((funcNode->attributeFlags & ATTRIBUTE_NOT_GENERIC) && funcNode->flags & AST_IS_GENERIC)
            return context->report({funcNode, funcNode->token, Fmt(Err(Err0751), funcNode->token.ctext())});

        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->genericParameters, true));
        if (context->result != ContextResult::Done)
            return true;
        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->parameters, false));
        if (context->result != ContextResult::Done)
            return true;
    }

    // If a lambda function will wait for a match, then no need to deduce the return type
    // It will be done in the same way as parameters
    if (!(funcNode->flags & AST_IS_GENERIC))
    {
        if ((funcNode->semFlags & AST_SEM_PENDING_LAMBDA_TYPING) && (typeNode->typeInfo == g_TypeMgr->typeInfoVoid))
        {
            typeNode->typeInfo = g_TypeMgr->typeInfoUndefined;
            funcNode->flags &= ~AST_SHORT_LAMBDA;
        }
    }

    // Short lambda without a return type we must deduced
    // In that case, symbol registration will not be done at the end of that function but once the return expression
    // has been evaluated, and the type deduced
    bool shortLambda = false;
    if ((funcNode->flags & AST_SHORT_LAMBDA) && !(funcNode->returnType->specFlags & AST_SPEC_FUNCTYPE_RETURN_DEFINED))
        shortLambda = true;

    // :RunGeneratedExp
    bool mustDeduceReturnType = false;
    if (funcNode->attributeFlags & ATTRIBUTE_RUN_GENERATED_EXP)
        mustDeduceReturnType = true;

    // No semantic on content if function is generic
    if (funcNode->flags & AST_IS_GENERIC)
    {
        shortLambda = false;
        funcNode->content->flags |= AST_NO_SEMANTIC;
    }

    // Macro will not evaluate its content before being inline
    if ((funcNode->attributeFlags & ATTRIBUTE_MACRO) && !shortLambda)
        funcNode->content->flags |= AST_NO_SEMANTIC;

    // Register symbol
    typeInfo->returnType = typeNode->typeInfo;

    // Be sure this is a valid return type
    if (!typeInfo->returnType->isNative() &&
        !typeInfo->returnType->isListTuple() &&
        !typeInfo->returnType->isListArray() &&
        !typeInfo->returnType->isStruct() &&
        !typeInfo->returnType->isGeneric() &&
        !typeInfo->returnType->isAlias() &&
        !typeInfo->returnType->isLambdaClosure() &&
        !typeInfo->returnType->isSlice() &&
        !typeInfo->returnType->isEnum() &&
        !typeInfo->returnType->isInterface() &&
        !typeInfo->returnType->isArray() &&
        !typeInfo->returnType->isPointer())
    {
        return context->report({typeNode->childs.front(), Fmt(Err(Err0764), typeInfo->returnType->getDisplayNameC())});
    }

    typeInfo->name.clear();
    typeInfo->displayName.clear();
    typeInfo->computeWhateverNameNoLock(COMPUTE_NAME);

    // Special functions registration
    if (funcNode->parameters && funcNode->parameters->childs.size() == 1)
    {
        if (funcNode->token.text == g_LangSpec->name_opInit)
        {
            auto       typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto       typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserInitFct = funcNode;
        }
        else if (funcNode->token.text == g_LangSpec->name_opDrop)
        {
            auto       typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto       typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserDropFct = funcNode;
            SWAG_VERIFY(!(typeStruct->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR), context->report({funcNode, funcNode->token, Fmt(Err(Err0199), typeStruct->getDisplayNameC())}));
        }
        else if (funcNode->token.text == g_LangSpec->name_opPostCopy)
        {
            auto       typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto       typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserPostCopyFct = funcNode;
            SWAG_VERIFY(!(typeStruct->flags & TYPEINFO_STRUCT_NO_COPY), context->report({funcNode, funcNode->token, Fmt(Err(Err0765), typeStruct->name.c_str())}));
        }
        else if (funcNode->token.text == g_LangSpec->name_opPostMove)
        {
            auto       typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto       typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserPostMoveFct = funcNode;
        }
    }

    // If this is a lambda waiting for a match to know the types of its parameters, need to wait
    // Function SemanticJob::setSymbolMatch will wake us up as soon as a valid match is found
    if (funcNode->semFlags & AST_SEM_PENDING_LAMBDA_TYPING)
    {
        if (!(funcNode->flags & AST_IS_GENERIC))
        {
            funcNode->pendingLambdaJob = context->job;
            context->job->setPending(nullptr, JobWaitKind::PendingLambdaTyping, funcNode, nullptr);
        }
    }

    // Set storageIndex of each parameters
    setFuncDeclParamsIndex(funcNode);

    // To avoid ambiguity, we do not want a function to declare a generic type 'T' if the struct
    // has the same generic parameter name (this is useless and implicit)
    if (funcNode->genericParameters && funcNode->ownerStructScope)
    {
        auto structDecl = CastAst<AstStruct>(funcNode->ownerStructScope->owner, AstNodeKind::StructDecl);
        if (structDecl->typeInfo->flags & TYPEINFO_GENERIC)
        {
            for (auto c : funcNode->genericParameters->childs)
            {
                if (!c->resolvedSymbolOverload)
                    continue;

                for (auto sc : structDecl->genericParameters->childs)
                {
                    if (!sc->resolvedSymbolOverload)
                        continue;

                    if (c->resolvedSymbolOverload->node->token.text == sc->resolvedSymbolOverload->node->token.text)
                    {
                        Diagnostic diag{c, Fmt(Err(Err0893), c->resolvedSymbolOverload->node->token.ctext())};
                        Diagnostic note{sc->resolvedSymbolOverload->node, Nte(Nte0037), DiagnosticLevel::Note};
                        return context->report(diag, &note);
                    }
                }
            }
        }
    }

    // Do we have capture parameters ? If it's the case, then we need to register all symbols as variables in the function scope
    if (funcNode->captureParameters)
    {
        uint32_t storageOffset = 0;
        for (auto c : funcNode->captureParameters->childs)
        {
            Utf8 name = c->token.text;
            if (c->kind == AstNodeKind::MakePointer)
                name = c->childs.front()->token.text;
            auto overload = funcNode->scope->symTable.addSymbolTypeInfo(context, c, c->typeInfo, SymbolKind::Variable, nullptr, OVERLOAD_VAR_CAPTURE, nullptr, 0, nullptr, &name);
            if (!overload)
                return false;
            c->resolvedSymbolOverload             = overload;
            overload->computedValue.storageOffset = storageOffset;
            storageOffset += overload->typeInfo->sizeOf;
        }
    }

    // Register runtime libc function type, by name
    if (funcNode->sourceFile && funcNode->sourceFile->isRuntimeFile && (funcNode->flags & AST_EMPTY_FCT))
    {
        ScopedLock lk(funcNode->sourceFile->module->mutexFile);
        funcNode->sourceFile->module->mapRuntimeFctsTypes[funcNode->token.text] = typeInfo;
    }

    // We should never reference an empty function
    // So consider this is a placeholder. This will generate an error in case the empty function is not replaced by a
    // real function at some point.
    if (funcNode->flags & AST_EMPTY_FCT && !(funcNode->isForeign()) && funcNode->token.text[0] != '@')
    {
        ScopedLock lk(funcNode->resolvedSymbolName->mutex);

        // We need to be sure that we only have empty functions, and not a real one.
        // As we can have multiple times the same empty function prototype, count them.
        int cptEmpty = 0;
        for (auto n : funcNode->resolvedSymbolName->nodes)
        {
            if (!(n->flags & AST_EMPTY_FCT))
                break;
            cptEmpty++;
        }

        if (cptEmpty == funcNode->resolvedSymbolName->nodes.size() && funcNode->resolvedSymbolName->cptOverloads == 1)
        {
            funcNode->resolvedSymbolName->kind = SymbolKind::PlaceHolder;
            return true;
        }
    }

    // For a short lambda without a specified return type, we need to defer the symbol registration, as we
    // need to infer it from the lambda expression
    SWAG_CHECK(registerFuncSymbol(context, funcNode, (shortLambda || mustDeduceReturnType) ? OVERLOAD_INCOMPLETE : 0));

    return true;
}

bool SemanticJob::registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode, uint32_t symbolFlags)
{
    if (!(symbolFlags & OVERLOAD_INCOMPLETE))
    {
        SWAG_CHECK(checkFuncPrototype(context, funcNode));

        // The function wants to return something, but has the 'Swag.NoReturn' attribute
        if (!funcNode->returnType->typeInfo->isVoid() && (funcNode->attributeFlags & ATTRIBUTE_NO_RETURN))
        {
            Diagnostic diag{funcNode->returnType->childs.front(), Err(Err0766)};
            diag.hint = Hnt(Hnt0026);
            return context->report(diag);
        }
        // The function returns nothing but has the 'Swag.Discardable' attribute
        if (funcNode->returnType->typeInfo->isVoid() && funcNode->attributeFlags & ATTRIBUTE_DISCARDABLE)
            return context->report({funcNode, Fmt(Err(Err0767), funcNode->token.ctext())});
    }

    if (funcNode->flags & AST_IS_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;

    auto typeFunc                    = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    funcNode->resolvedSymbolOverload = funcNode->ownerScope->symTable.addSymbolTypeInfo(context, funcNode, funcNode->typeInfo, SymbolKind::Function, nullptr, symbolFlags, &funcNode->resolvedSymbolName);
    SWAG_CHECK(funcNode->resolvedSymbolOverload);

    // If the function returns a struct, register a type alias "retval". This way we can resolve an identifier
    // named retval for "var result: retval{xx, xxx}" syntax
    auto returnType = TypeManager::concreteType(funcNode->returnType->typeInfo, CONCRETE_ALIAS);
    if (returnType->isStruct())
    {
        Utf8 retVal = g_LangSpec->name_retval;
        funcNode->scope->symTable.addSymbolTypeInfo(context, funcNode->returnType, returnType, SymbolKind::TypeAlias, nullptr, symbolFlags | OVERLOAD_RETVAL, nullptr, 0, nullptr, &retVal);
    }

    // Register method
    if (!(symbolFlags & OVERLOAD_INCOMPLETE) && isMethod(funcNode))
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(funcNode->ownerStructScope->owner->typeInfo, TypeInfoKind::Struct);

        {
            ScopedLock lk(typeStruct->mutex);
            SWAG_ASSERT(funcNode->methodParam);
            funcNode->methodParam->attributes = typeFunc->attributes;
        }

        context->job->decreaseMethodCount(funcNode, typeStruct);
    }

    resolveSubDecls(context, funcNode);
    return true;
}

bool SemanticJob::isMethod(AstFuncDecl* funcNode)
{
    if (funcNode->ownerStructScope &&
        funcNode->parent->kind != AstNodeKind::CompilerAst &&
        funcNode->parent->kind != AstNodeKind::CompilerRun &&
        funcNode->parent->kind != AstNodeKind::CompilerRunExpression &&
        funcNode->parent->kind != AstNodeKind::CompilerSelectIfOnce &&
        funcNode->parent->kind != AstNodeKind::CompilerSelectIf &&
        !(funcNode->flags & AST_FROM_GENERIC) &&
        !(funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC) &&
        (funcNode->ownerScope->kind == ScopeKind::Struct) &&
        (funcNode->ownerStructScope->owner->typeInfo->isStruct()))
    {
        return true;
    }

    return false;
}

void SemanticJob::launchResolveSubDecl(JobContext* context, AstNode* node)
{
    if (node->flags & AST_SPEC_SEMANTICX)
        return;

    // If AST_DONE_FILE_JOB_PASS is set, then the file job has already seen the sub declaration, ignored it
    // because of AST_NO_SEMANTIC, but the attribute context is ok. So we need to trigger the job by hand.
    // If AST_DONE_FILE_JOB_PASS is not set, then we just have to remove the AST_NO_SEMANTIC flag, and the
    // file job will trigger the resolve itself
    node->flags &= ~AST_NO_SEMANTIC;
    if (node->doneFlags & AST_DONE_FILE_JOB_PASS)
    {
        auto job          = g_Allocator.alloc<SemanticJob>();
        job->sourceFile   = context->sourceFile;
        job->module       = context->sourceFile->module;
        job->dependentJob = context->baseJob->dependentJob;
        job->nodes.push_back(node);
        g_ThreadMgr.addJob(job);
    }
}

void SemanticJob::resolveSubDecls(JobContext* context, AstFuncDecl* funcNode)
{
    if (!funcNode)
        return;

    // If we have sub declarations, then now we can solve them, except for a generic function.
    // Because for a generic function, the sub declarations will be cloned and solved after instanciation.
    // Otherwise, we can have a race condition by solving a generic sub declaration and cloning it for instancation
    // at the same time.
    if (!(funcNode->flags & AST_IS_GENERIC) && funcNode->content && !(funcNode->content->flags & AST_NO_SEMANTIC))
    {
        for (auto f : funcNode->subDecls)
        {
            ScopedLock lk(f->mutex);

            // Disabled by #if block
            if (f->semFlags & AST_SEM_DISABLED)
                continue;
            f->semFlags |= AST_SEM_DISABLED; // To avoid multiple resolutions

            if (f->ownerCompilerIfBlock && f->ownerCompilerIfBlock->ownerFct == funcNode)
            {
                ScopedLock lk1(f->ownerCompilerIfBlock->mutex);
                f->ownerCompilerIfBlock->subDecls.push_back(f);
            }
            else
                launchResolveSubDecl(context, f);
        }
    }
}

bool SemanticJob::resolveCaptureFuncCallParams(SemanticContext* context)
{
    auto node = CastAst<AstFuncCallParams>(context->node, AstNodeKind::FuncCallParams);
    node->inheritOrFlag(AST_IS_GENERIC);
    node->inheritAndFlag1(AST_CONST_EXPR);

    // Check capture types
    for (auto c : node->childs)
    {
        auto typeField = c->typeInfo;

        if (typeField->isArray())
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeField, TypeInfoKind::Array);
            typeField      = typeArray->finalType;
        }

        if (typeField->isPointer())
            continue;
        if (typeField->isNative())
            continue;
        if (typeField->isSlice())
            continue;
        if (typeField->isInterface())
            continue;
        if (typeField->isLambda())
            continue;

        if (typeField->isStruct())
        {
            SWAG_CHECK(waitForStructUserOps(context, c));
            if (context->result == ContextResult::Pending)
                return true;
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeField, TypeInfoKind::Struct);
            if (!typeStruct->isPlainOldData())
                return context->report({c, Fmt(Err(Err0884), c->token.ctext()), Diagnostic::isType(c->typeInfo)});
            continue;
        }

        if (typeField->isClosure())
            return context->report({c, Fmt(Err(Err0875), c->token.ctext()), Diagnostic::isType(c->typeInfo)});
        return context->report({c, Fmt(Err(Err0887), c->token.ctext(), typeField->getDisplayNameC()), Diagnostic::isType(c->typeInfo)});
    }

    // As this is the capture block resolved in the right context, we can now evaluate the corresponding slosure
    auto mpl = CastAst<AstMakePointer>(node->parent, AstNodeKind::MakePointerLambda);
    SWAG_ASSERT(mpl->lambda);

    ScopedLock lk(mpl->lambda->mutex);
    if (mpl->lambda->flags & AST_SPEC_SEMANTIC1)
    {
        mpl->lambda->flags &= ~AST_SPEC_SEMANTIC1;
        launchResolveSubDecl(context, mpl->lambda);
    }

    return true;
}

bool SemanticJob::resolveFuncCallGenParams(SemanticContext* context)
{
    auto node = context->node;
    node->inheritOrFlag(AST_IS_GENERIC);
    node->inheritAndFlag1(AST_CONST_EXPR);

    if (node->flags & AST_IS_GENERIC)
        return true;

    for (auto c : node->childs)
    {
        if (c->flags & AST_VALUE_COMPUTED)
            continue;

        auto symbol = c->childs.front()->resolvedSymbolName;
        if (!symbol)
            continue;

        if (symbol->kind == SymbolKind::Variable ||
            symbol->kind == SymbolKind::Namespace ||
            symbol->kind == SymbolKind::Attribute)
        {
            Diagnostic note{Hlp(Hlp0021), DiagnosticLevel::Help};
            return context->report({c, Fmt(Err(Err0815), SymTable::getArticleKindName(symbol->kind), symbol->name.c_str())}, &note);
        }
    }

    return true;
}

bool SemanticJob::resolveFuncCallParams(SemanticContext* context)
{
    auto node = context->node;
    node->inheritOrFlag(AST_IS_GENERIC);
    node->inheritAndFlag1(AST_CONST_EXPR);
    return true;
}

bool SemanticJob::resolveFuncCallParam(SemanticContext* context)
{
    auto node      = CastAst<AstFuncCallParam>(context->node, AstNodeKind::FuncCallParam);
    auto child     = node->childs.front();
    node->typeInfo = child->typeInfo;

    SWAG_VERIFY(!node->typeInfo->isCVariadic(), context->report({node, Err(Err0446)}));

    // Force const if necessary
    // func([.., ...]) must be const
    if (child->kind == AstNodeKind::ExpressionList)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
        if (typeList->isListArray())
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
    node->inheritOrFlag(child, AST_CONST_EXPR | AST_IS_GENERIC | AST_VALUE_IS_TYPEINFO | AST_OPAFFECT_CAST);
    if (node->childs.front()->semFlags & AST_SEM_LITERAL_SUFFIX)
        node->semFlags |= AST_SEM_LITERAL_SUFFIX;

    // Inherit the original type in case of computed values, in order to make the cast if necessary
    if (node->flags & (AST_VALUE_COMPUTED | node->flags & AST_OPAFFECT_CAST))
        node->castedTypeInfo = child->castedTypeInfo;

    if (checkForConcrete & !(node->flags & AST_OPAFFECT_CAST))
    {
        SWAG_CHECK(evaluateConstExpression(context, node));
        if (context->result == ContextResult::Pending)
            return true;
    }

    node->resolvedSymbolName     = child->resolvedSymbolName;
    node->resolvedSymbolOverload = child->resolvedSymbolOverload;

    if (child->extension && child->extension->misc && child->extension->misc->resolvedUserOpSymbolOverload)
    {
        node->allocateExtension(ExtensionKind::Resolve);
        node->extension->misc->resolvedUserOpSymbolOverload = child->extension->misc->resolvedUserOpSymbolOverload;
    }

    // If the call has been generated because of a 'return tuple', then we force a move
    // instead of a copy, in case the parameter to the tuple init is a local variable
    if (node->autoTupleReturn)
    {
        if (node->resolvedSymbolOverload && (node->resolvedSymbolOverload->flags & OVERLOAD_VAR_LOCAL))
        {
            node->flags |= AST_FORCE_MOVE | AST_NO_RIGHT_DROP;
            node->autoTupleReturn->forceNoDrop.push_back(child->resolvedSymbolOverload);
        }
    }

    return true;
}

bool SemanticJob::resolveRetVal(SemanticContext* context)
{
    auto node    = context->node;
    auto fctDecl = node->ownerInline ? node->ownerInline->func : node->ownerFct;

    SWAG_VERIFY(fctDecl, context->report({node, Err(Err0769)}));
    SWAG_VERIFY(node->ownerScope && node->ownerScope->kind != ScopeKind::Function, context->report({node, Err(Err0769)}));

    auto fct     = CastAst<AstFuncDecl>(fctDecl, AstNodeKind::FuncDecl);
    auto typeFct = CastTypeInfo<TypeInfoFuncAttr>(fct->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_VERIFY(typeFct->returnType && !typeFct->returnType->isVoid(), context->report({node, Err(Err0771)}));

    // If this is a simple return type, remove the retval stuff.
    // Variable will behaves normally, in the stack
    if (!(typeFct->returnType->flags & TYPEINFO_RETURN_BY_COPY))
    {
        auto typeExpr = CastAst<AstTypeExpression>(node, AstNodeKind::TypeExpression);
        typeExpr->typeFlags &= ~TYPEFLAG_RETVAL;
    }

    node->typeInfo = typeFct->returnType;
    return true;
}

void SemanticJob::propagateReturn(AstNode* node)
{
    auto stopFct = node->ownerFct ? node->ownerFct->parent : nullptr;
    if (node->semFlags & AST_SEM_EMBEDDED_RETURN)
        stopFct = node->ownerInline->parent;
    SWAG_ASSERT(stopFct);

    AstNode* scanNode = node;

    // Search if we are in an infinit loop
    auto breakable = node->ownerBreakable;
    while (breakable)
    {
        if (breakable->kind == AstNodeKind::Loop)
        {
            auto loopNode = CastAst<AstLoop>(breakable, AstNodeKind::Loop);
            if (!loopNode->expression)
                loopNode->breakableFlags |= BREAKABLE_RETURN_IN_INFINIT_LOOP;
            break;
        }

        if (breakable->kind == AstNodeKind::While)
        {
            auto whileNode = CastAst<AstWhile>(breakable, AstNodeKind::While);
            if ((whileNode->boolExpression->flags & AST_VALUE_COMPUTED) && (whileNode->boolExpression->computedValue->reg.b))
                whileNode->breakableFlags |= BREAKABLE_RETURN_IN_INFINIT_LOOP;
            break;
        }

        if (breakable->kind == AstNodeKind::For)
        {
            auto forNode = CastAst<AstFor>(breakable, AstNodeKind::For);
            if ((forNode->boolExpression->flags & AST_VALUE_COMPUTED) && (forNode->boolExpression->computedValue->reg.b))
                forNode->breakableFlags |= BREAKABLE_RETURN_IN_INFINIT_LOOP;
            break;
        }

        breakable = breakable->ownerBreakable;
    }

    // Propage the return in the corresponding scope
    while (scanNode && scanNode != stopFct)
    {
        if (scanNode->semFlags & AST_SEM_SCOPE_HAS_RETURN && !(scanNode->semFlags & AST_SEM_SCOPE_FORCE_HAS_RETURN))
            break;
        scanNode->semFlags |= AST_SEM_SCOPE_HAS_RETURN;
        if (scanNode->parent && scanNode->parent->kind == AstNodeKind::If)
        {
            auto ifNode = CastAst<AstIf>(scanNode->parent, AstNodeKind::If);
            if (ifNode->elseBlock != scanNode)
                break;
            if (!(ifNode->ifBlock->semFlags & AST_SEM_SCOPE_HAS_RETURN))
                break;
        }
        else if (scanNode->kind == AstNodeKind::SwitchCase)
        {
            auto sc = CastAst<AstSwitchCase>(scanNode, AstNodeKind::SwitchCase);
            if (sc->specFlags & AST_SPEC_SWITCHCASE_ISDEFAULT)
                sc->ownerSwitch->semFlags |= AST_SEM_SCOPE_FORCE_HAS_RETURN;
        }
        else if (scanNode->kind == AstNodeKind::Switch)
        {
            if (!(scanNode->semFlags & AST_SEM_SCOPE_FORCE_HAS_RETURN))
                break;
        }
        else if (scanNode->kind == AstNodeKind::While ||
                 scanNode->kind == AstNodeKind::Loop ||
                 scanNode->kind == AstNodeKind::For)
        {
            break;
        }

        scanNode = scanNode->parent;
    }

    // To tell that the function as at least one return (this will change the error message)
    while (scanNode && scanNode != stopFct)
    {
        if (scanNode->semFlags & AST_SEM_FCT_HAS_RETURN)
            break;
        scanNode->semFlags |= AST_SEM_FCT_HAS_RETURN;
        scanNode = scanNode->parent;
    }
}

AstFuncDecl* SemanticJob::getFunctionForReturn(AstNode* node)
{
    SWAG_ASSERT(node->kind == AstNodeKind::Return);

    // For a return inside an inline block, take the original function, except if it is flagged with 'Swag.noreturn'
    auto funcNode = node->ownerFct;
    if (node->ownerInline && node->ownerInline->isParentOf(node))
    {
        if (!(node->ownerInline->func->attributeFlags & ATTRIBUTE_NO_RETURN) && !(node->flags & AST_IN_MIXIN))
        {
            node->semFlags |= AST_SEM_EMBEDDED_RETURN;
            funcNode = node->ownerInline->func;
        }
    }

    return funcNode;
}

bool SemanticJob::resolveReturn(SemanticContext* context)
{
    SWAG_CHECK(warnUnreachableCode(context));

    auto node     = CastAst<AstReturn>(context->node, AstNodeKind::Return);
    auto funcNode = getFunctionForReturn(node);

    node->byteCodeFct      = ByteCodeGenJob::emitReturn;
    node->resolvedFuncDecl = funcNode;
    auto funcReturnType    = TypeManager::concreteType(funcNode->returnType->typeInfo);

    // As the type of the function is deduced from the return type, be sure they match in case
    // of multiple returns
    if (funcNode->returnTypeDeducedNode)
    {

        // We return nothing, but the previous return had something
        if (node->childs.empty())
        {
            if (!funcReturnType->isVoid())
            {
                Diagnostic diag{node, Fmt(Err(Err0779), funcReturnType->getDisplayNameC())};
                Diagnostic note{funcNode->returnTypeDeducedNode->childs.front(), Nte(Nte0063), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }

            return true;
        }

        auto child     = node->childs[0];
        auto childType = TypeManager::concreteType(child->typeInfo);

        // We try to return something, but the previous return had nothing
        if (funcReturnType->isVoid() && !childType->isVoid())
        {
            Diagnostic diag{child, Fmt(Err(Err0773), childType->getDisplayNameC())};
            Diagnostic note{funcNode->returnTypeDeducedNode, Nte(Nte0063), DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        if (!TypeManager::makeCompatibles(context, funcNode->returnType->typeInfo, nullptr, child, CASTFLAG_NO_ERROR | CASTFLAG_JUST_CHECK | CASTFLAG_UNCONST | CASTFLAG_AUTO_OPCAST | CASTFLAG_TRY_COERCE | CASTFLAG_FOR_AFFECT))
        {
            Diagnostic diag{child, Fmt(Err(Err0770), funcNode->returnType->typeInfo->getDisplayNameC(), child->typeInfo->getDisplayNameC())};
            Diagnostic note{funcNode->returnTypeDeducedNode->childs.front(), Nte(Nte0063), DiagnosticLevel::Note};
            return context->report(diag, &note);
        }
    }

    // Nothing to return
    if (funcNode->returnType->typeInfo == g_TypeMgr->typeInfoVoid && node->childs.empty())
    {
        if (funcNode->attributeFlags & ATTRIBUTE_RUN_GENERATED_EXP)
        {
            funcNode->returnType->typeInfo  = g_TypeMgr->typeInfoVoid;
            funcNode->returnTypeDeducedNode = node;
        }

        return true;
    }

    // Deduce return type
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    bool lateRegister = funcNode->specFlags & AST_SPEC_FUNCDECL_FORCE_LATE_REGISTER;
    if (funcReturnType->isVoid() && !(funcNode->specFlags & AST_SPEC_FUNCDECL_LATE_REGISTER_DONE))
    {
        // This is a short lambda without a specified return type. We now have it
        bool tryDeduce = false;
        if ((funcNode->flags & AST_SHORT_LAMBDA) && !(funcNode->returnType->specFlags & AST_SPEC_FUNCTYPE_RETURN_DEFINED))
            tryDeduce = true;
        if (funcNode->attributeFlags & ATTRIBUTE_RUN_GENERATED_EXP)
            tryDeduce = true;
        if (tryDeduce)
        {
            typeInfoFunc->returnType = TypeManager::concreteType(node->childs.front()->typeInfo, CONCRETE_FUNC);
            typeInfoFunc->returnType = TypeManager::promoteUntyped(typeInfoFunc->returnType);
            auto concreteReturn      = TypeManager::concreteType(typeInfoFunc->returnType);
            if (concreteReturn->isListTuple())
            {
                SWAG_CHECK(convertLiteralTupleToStructDecl(context, funcNode->content, node->childs.front(), &funcNode->returnType));
                funcNode->specFlags |= AST_SPEC_FUNCDECL_FORCE_LATE_REGISTER;
                Ast::setForceConstType(funcNode->returnType);
                context->job->nodes.push_back(funcNode->returnType);
                context->result = ContextResult::NewChilds;
                return true;
            }

            typeInfoFunc->forceComputeName();
            funcNode->returnType->typeInfo  = typeInfoFunc->returnType;
            funcNode->returnTypeDeducedNode = node;
            funcNode->specFlags |= AST_SPEC_FUNCDECL_LATE_REGISTER_DONE;
            lateRegister = true;
        }
    }

    if (node->childs.empty())
    {
        Diagnostic diag{node, Fmt(Err(Err0772), funcNode->returnType->typeInfo->getDisplayNameC())};
        Diagnostic note{funcNode->returnType->childs.front(), Fmt(Nte(Nte0067), typeInfoFunc->returnType->getDisplayNameC()), DiagnosticLevel::Note};
        return context->report(diag, &note);
    }

    auto returnType = funcNode->returnType->typeInfo;

    // Check types
    auto child = node->childs[0];
    SWAG_CHECK(checkIsConcreteOrType(context, child));
    if (context->result != ContextResult::Done)
        return true;

    auto concreteType = TypeManager::concreteType(child->typeInfo);

    // No return value in a #run block
    if (!concreteType->isVoid())
    {
        if (funcNode->attributeFlags & (ATTRIBUTE_RUN_FUNC |
                                        ATTRIBUTE_RUN_GENERATED_FUNC |
                                        ATTRIBUTE_MAIN_FUNC |
                                        ATTRIBUTE_INIT_FUNC |
                                        ATTRIBUTE_DROP_FUNC |
                                        ATTRIBUTE_PREMAIN_FUNC |
                                        ATTRIBUTE_TEST_FUNC))
        {
            if (funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC)
                return context->report({child, Fmt(Err(Err0052), funcNode->getDisplayNameC()), Hnt(Hnt0026)});
        }
    }

    // Be sure we do not specify a return value, and the function does not have a return type
    // (better error message than just letting the makeCompatibles do its job)
    if (returnType->isVoid() && !concreteType->isVoid())
    {
        Diagnostic diag{child, Fmt(Err(Err0774), concreteType->getDisplayNameC(), funcNode->getDisplayNameC())};
        diag.hint = Hnt(Hnt0026);

        if (node->ownerInline && !(node->semFlags & AST_SEM_EMBEDDED_RETURN))
        {
            Diagnostic note{funcNode, Fmt(Nte(Nte0011), node->ownerInline->func->getDisplayNameC(), funcNode->getDisplayNameC()), DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        return context->report(diag);
    }

    // If returning retval, then returning nothing, as we will change the return parameter value in place
    if (child->resolvedSymbolOverload && child->resolvedSymbolOverload->flags & OVERLOAD_RETVAL)
    {
        node->typeInfo = child->typeInfo;
    }
    else
    {
        // If we are returning an interface, be sure they are defined before casting
        if (returnType->isInterface())
        {
            context->job->waitAllStructInterfaces(child->typeInfo);
            if (context->result != ContextResult::Done)
                return true;
        }

        uint32_t castFlags = CASTFLAG_UNCONST | CASTFLAG_AUTO_OPCAST | CASTFLAG_TRY_COERCE | CASTFLAG_FOR_AFFECT | CASTFLAG_PTR_REF;

        if (funcNode->attributeFlags & ATTRIBUTE_AST_FUNC)
        {
            PushErrContext ec{context, funcNode, ErrorContextKind::Note, Nte(Nte0005), nullptr, true};
            SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, castFlags));
        }
        else if (funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC)
        {
            PushErrContext ec{context, funcNode, ErrorContextKind::Note, Fmt(Nte(Nte0067), returnType->getDisplayNameC()), nullptr, true};
            SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, castFlags));
        }
        else
        {
            PushErrContext ec{context, funcNode->returnType, ErrorContextKind::Note, Fmt(Nte(Nte0067), returnType->getDisplayNameC())};
            SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, castFlags));
        }
    }

    if (child->kind == AstNodeKind::ExpressionList)
    {
        child->flags |= AST_TRANSIENT;
    }

    // When returning a struct, we need to know if postcopy or postmove are here, and wait for them to resolve
    if (returnType && (returnType->isStruct() || returnType->isArrayOfStruct()))
    {
        SWAG_CHECK(waitForStructUserOps(context, funcNode->returnType));
        if (context->result == ContextResult::Pending)
            return true;
    }

    // If we are returning a local variable, we can do a move
    if (child->resolvedSymbolOverload && (child->resolvedSymbolOverload->flags & OVERLOAD_VAR_LOCAL))
    {
        child->flags |= AST_FORCE_MOVE | AST_NO_RIGHT_DROP;
        node->forceNoDrop.push_back(child->resolvedSymbolOverload);
    }

    if (child->resolvedSymbolOverload && (child->resolvedSymbolOverload->flags & OVERLOAD_RETVAL))
    {
        child->flags |= AST_NO_BYTECODE;
        node->forceNoDrop.push_back(child->resolvedSymbolOverload);
    }

    // Propagate return so that we can detect if some paths are missing one
    propagateReturn(node);

    // Register symbol now that we have inferred the return type
    if (lateRegister)
    {
        funcNode->specFlags &= ~AST_SPEC_FUNCDECL_FORCE_LATE_REGISTER;
        typeInfoFunc->returnType = funcNode->returnType->typeInfo;
        typeInfoFunc->forceComputeName();
        SWAG_CHECK(registerFuncSymbol(context, funcNode));
    }

    return true;
}

uint32_t SemanticJob::getMaxStackSize(AstNode* node)
{
    auto decSP = node->ownerScope->startStackSize;

    if (node->semFlags & AST_SEM_SPEC_STACKSIZE)
    {
        auto p = node;
        while (p->parent && p->parent->kind != AstNodeKind::File)
            p = p->parent;
        SWAG_ASSERT(p);
        ScopedLock mk(p->mutex);
        p->allocateExtensionNoLock(ExtensionKind::StackSize);
        decSP = max(decSP, p->extension->misc->stackSize);
        return decSP;
    }

    if (node->ownerFct)
        decSP = max(decSP, node->ownerFct->stackSize);
    return decSP;
}

void SemanticJob::setOwnerMaxStackSize(AstNode* node, uint32_t size)
{
    size = max(size, 1);
    size = (uint32_t) TypeManager::align(size, sizeof(void*));

    if (node->semFlags & AST_SEM_SPEC_STACKSIZE)
    {
        auto p = node;
        while (p->parent && p->parent->kind != AstNodeKind::File)
            p = p->parent;
        SWAG_ASSERT(p);
        ScopedLock mk(p->mutex);
        p->allocateExtensionNoLock(ExtensionKind::StackSize);
        p->extension->misc->stackSize = max(p->extension->misc->stackSize, size);
    }
    else if (node->ownerFct)
    {
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, size);
    }
}

bool SemanticJob::makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    CloneContext cloneContext;

    // Be sure this is not recursive
    auto ownerInline = identifier->ownerInline;
    while (ownerInline)
    {
        if (ownerInline->func == funcDecl)
            return context->report({identifier, identifier->token, Fmt(Err(Err0775), identifier->token.ctext())});
        ownerInline = ownerInline->ownerInline;
    }

    // The content will be inline in its separated syntax block
    auto inlineNode            = Ast::newInline(identifier->sourceFile, identifier);
    inlineNode->attributeFlags = funcDecl->attributeFlags;
    inlineNode->func           = funcDecl;
    inlineNode->scope          = identifier->ownerScope;
    inlineNode->typeInfo       = TypeManager::concreteType(funcDecl->typeInfo);

    if (identifier->extension && identifier->extension->owner && identifier->extension->owner->ownerTryCatchAssume)
    {
        inlineNode->allocateExtension(ExtensionKind::Owner);
        inlineNode->extension->owner->ownerTryCatchAssume = identifier->extension->owner->ownerTryCatchAssume;
    }

    if (funcDecl->extension && funcDecl->extension->misc)
    {
        SharedLock lk1(funcDecl->extension->misc->mutexAltScopes);
        if (funcDecl->extension->misc->alternativeScopes.size() || funcDecl->extension->misc->alternativeScopesVars.size())
        {
            inlineNode->allocateExtension(ExtensionKind::AltScopes);
            ScopedLock lk(inlineNode->extension->misc->mutexAltScopes);
            inlineNode->extension->misc->alternativeScopes     = funcDecl->extension->misc->alternativeScopes;
            inlineNode->extension->misc->alternativeScopesVars = funcDecl->extension->misc->alternativeScopesVars;
        }
    }

    // Try/Assume
    if (inlineNode->extension &&
        inlineNode->extension->owner &&
        inlineNode->extension->owner->ownerTryCatchAssume &&
        (inlineNode->func->typeInfo->flags & TYPEINFO_CAN_THROW))
    {
        inlineNode->allocateExtension(ExtensionKind::ByteCode);
        auto extension = inlineNode->extension->bytecode;
        switch (inlineNode->extension->owner->ownerTryCatchAssume->kind)
        {
        case AstNodeKind::Try:
            extension->byteCodeAfterFct = ByteCodeGenJob::emitTry;
            break;
        case AstNodeKind::TryCatch:
            extension->byteCodeAfterFct = ByteCodeGenJob::emitTryCatch;
            break;
        case AstNodeKind::Assume:
            extension->byteCodeAfterFct = ByteCodeGenJob::emitAssume;
            break;
        }

        // Reset emit from the modifier if it exists, as the inline block will deal with that
        if (identifier->extension && identifier->extension->bytecode)
        {
            extension = identifier->extension->bytecode;
            if (extension->byteCodeAfterFct == ByteCodeGenJob::emitTry)
                extension->byteCodeAfterFct = nullptr;
            else if (extension->byteCodeAfterFct == ByteCodeGenJob::emitTryCatch)
                extension->byteCodeAfterFct = nullptr;
            else if (extension->byteCodeAfterFct == ByteCodeGenJob::emitAssume)
                extension->byteCodeAfterFct = nullptr;
        }
    }

    // We need to add the parent scope of the inline function (the global one), in order for
    // the inline content to be resolved in the same context as the original function
    auto globalScope = funcDecl->ownerScope;
    while (!globalScope->isGlobalOrImpl())
        globalScope = globalScope->parentScope;
    inlineNode->addAlternativeScope(globalScope);

    // We also need to add all alternatives scopes (using), in case the function relies on them to
    // be solved
    AstNode* parentNode = funcDecl;
    while (parentNode)
    {
        if (parentNode->extension && parentNode->extension->misc && !parentNode->extension->misc->alternativeScopes.empty())
            inlineNode->addAlternativeScopes(parentNode->extension->misc->alternativeScopes);
        parentNode = parentNode->parent;
    }

    // If function has generic parameters, then the block resolution of identifiers needs to be able to find the generic parameters
    // so we register all those generic parameters in a special scope (we cannot just register the scope of the function because
    // they are other stuff here)
    if (funcDecl->genericParameters)
    {
        Scope* scope = Ast::newScope(inlineNode, "", ScopeKind::Statement, nullptr);
        inlineNode->addAlternativeScope(scope);
        for (auto child : funcDecl->genericParameters->childs)
        {
            auto symName = scope->symTable.registerSymbolNameNoLock(context, child, SymbolKind::Variable);
            symName->addOverloadNoLock(child, child->typeInfo, &child->resolvedSymbolOverload->computedValue);
            symName->cptOverloads = 0; // Simulate a done resolution
        }
    }

    // A mixin behave exactly like if it is in the caller scope, so do not create a subscope for them
    Scope* newScope = identifier->ownerScope;
    if (!(funcDecl->attributeFlags & ATTRIBUTE_MIXIN))
    {
        newScope          = Ast::newScope(inlineNode, Fmt("__inline%d", identifier->ownerScope->childScopes.size()), ScopeKind::Inline, identifier->ownerScope);
        inlineNode->scope = newScope;
    }

    // Create a scope that will contain all parameters.
    // Parameters are specific to each inline block, that's why we create it even for mixins
    inlineNode->parametersScope = Ast::newScope(inlineNode, "", ScopeKind::Statement, nullptr);

    // Clone the function body
    cloneContext.parent         = inlineNode;
    cloneContext.ownerInline    = inlineNode;
    cloneContext.ownerFct       = identifier->ownerFct;
    cloneContext.ownerBreakable = identifier->ownerBreakable;
    cloneContext.parentScope    = newScope;
    cloneContext.forceFlags |= identifier->flags & AST_NO_BACKEND;
    cloneContext.forceFlags |= identifier->flags & AST_RUN_BLOCK;
    cloneContext.forceFlags |= identifier->flags & AST_IN_DEFER;
    cloneContext.cloneFlags |= CLONE_FORCE_OWNER_FCT;

    // Here we inline a call in a global declaration, like a variable/constant initialization
    // We do not want the function to be the original one, in case of local variables, because we
    // do not want to change the stackSize of the original function because of local variables.
    if (!cloneContext.ownerFct)
    {
        identifier->semFlags |= AST_SEM_SPEC_STACKSIZE;
        if (identifier->kind == AstNodeKind::Identifier)
            identifier->parent->semFlags |= AST_SEM_SPEC_STACKSIZE;
        cloneContext.forceSemFlags = AST_SEM_SPEC_STACKSIZE;
    }

    // Register all aliases
    if (identifier->kind == AstNodeKind::Identifier)
    {
        // Replace user aliases of the form @alias?
        // Can come from the identifier itself (for visit) or from call parameters (for macros/mixins)
        auto id = CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier);

        int idx = 0;
        for (auto& alias : id->aliasNames)
            cloneContext.replaceNames[Fmt("@alias%d", idx++)] = alias.text;

        idx = 0;
        for (auto& alias : id->callParameters->aliasNames)
            cloneContext.replaceNames[Fmt("@alias%d", idx++)] = alias.text;

        // Replace user @mixin
        if (funcDecl->hasSpecMixin)
        {
            for (int i = 0; i < 10; i++)
                cloneContext.replaceNames[Fmt("@mixin%d", i)] = Fmt("__mixin%d", g_UniqueID.fetch_add(1));
        }

        for (auto child : id->callParameters->childs)
        {
            auto param = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            if (!param->resolvedParameter)
                continue;

            // Transmit code type
            if (param->typeInfo->isCode())
            {
                inlineNode->parametersScope->symTable.addSymbolTypeInfo(context, param, param->typeInfo, SymbolKind::Variable, nullptr, 0, nullptr, 0, nullptr, &param->resolvedParameter->namedParam);
            }
        }
    }

    // Clone !
    auto newContent = funcDecl->content->clone(cloneContext);

    if (newContent->extension && newContent->extension->bytecode)
    {
        newContent->extension->bytecode->byteCodeBeforeFct = nullptr;
        if (funcDecl->attributeFlags & ATTRIBUTE_MIXIN)
            newContent->extension->bytecode->byteCodeAfterFct = nullptr; // Do not release the scope, as there's no specific scope
    }

    if (newContent->kind == AstNodeKind::Try || newContent->kind == AstNodeKind::TryCatch || newContent->kind == AstNodeKind::Assume)
    {
        if (funcDecl->attributeFlags & ATTRIBUTE_MIXIN && newContent->childs.front()->extension)
        {
            auto front = newContent->childs.front();
            if (front->extension && front->extension->bytecode)
                front->extension->bytecode->byteCodeAfterFct = nullptr; // Do not release the scope, as there's no specific scope
        }
    }

    newContent->flags &= ~AST_NO_SEMANTIC;

    // Sub declarations in the inline block, like sub functions
    if (!funcDecl->subDecls.empty())
    {
        PushErrContext ec(context, identifier, ErrorContextKind::Inline);
        SWAG_VERIFY(inlineNode->ownerFct, context->report({funcDecl, Fmt(Err(Err0781), identifier->token.ctext())}));

        // Authorize a sub function to access inline parameters, if possible
        // This will work for compile time values, otherwise we will have an out of stack frame when generating the code
        cloneContext.alternativeScope = inlineNode->parametersScope;

        SWAG_CHECK(funcDecl->cloneSubDecls(context, cloneContext, funcDecl->content, inlineNode->ownerFct, newContent));
    }

    // Need to reevaluate the identifier (if this is an identifier) because the makeInline can be called
    // for something else, like a loop node for example (opCount). In that case, we let the specific node
    // deal with the (re)evaluation.
    if (identifier->kind == AstNodeKind::Identifier)
    {
        // Do not reevaluate function parameters
        auto castId = CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier);
        if (castId->callParameters)
            castId->callParameters->flags |= AST_NO_SEMANTIC;

        identifier->semanticState = AstNodeResolveState::Enter;
        identifier->bytecodeState = AstNodeResolveState::Enter;
    }

    // Check used aliases
    // Error if an alias has been defined, but not 'eaten' by the function
    if (identifier->kind == AstNodeKind::Identifier)
    {
        if (cloneContext.replaceNames.size() != cloneContext.usedReplaceNames.size())
        {
            PushErrContext ec(context, identifier, ErrorContextKind::Inline);
            auto           id = CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier);
            for (auto& r : cloneContext.replaceNames)
            {
                auto it = cloneContext.usedReplaceNames.find(r.second);
                if (it == cloneContext.usedReplaceNames.end())
                {
                    for (auto& alias : id->aliasNames)
                    {
                        if (alias.text == r.second)
                        {
                            Diagnostic diag{id, alias, Fmt(Err(Err0780), alias.ctext())};
                            diag.hint = Hnt(Hnt0026);
                            return context->report(diag);
                        }
                    }

                    for (auto& alias : id->callParameters->aliasNames)
                    {
                        if (alias.text == r.second)
                        {
                            Diagnostic diag{id, alias, Fmt(Err(Err0780), alias.ctext())};
                            diag.hint = Hnt(Hnt0026);
                            return context->report(diag);
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool SemanticJob::makeInline(SemanticContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    SWAG_CHECK(makeInline((JobContext*) context, funcDecl, identifier));
    context->result = ContextResult::NewChilds;
    return true;
}