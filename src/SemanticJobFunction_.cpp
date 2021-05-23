#include "pch.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Os.h"
#include "ErrorIds.h"
#include "ErrorIds.h"

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
        if (!nodeExpr->forTuple)
            continue;

        if (!nodeParam->type)
        {
            nodeParam->semFlags |= AST_SEM_TUPLE_CONVERT;
            SWAG_ASSERT(nodeParam->typeInfo->kind == TypeInfoKind::TypeListTuple);
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

    auto sourceFile = context->sourceFile;
    for (auto param : parameters->childs)
    {
        auto nodeParam        = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        auto funcParam        = allocType<TypeInfoParam>();
        funcParam->namedParam = param->token.text;
        funcParam->name       = param->typeInfo->name;
        funcParam->typeInfo   = param->typeInfo;
        funcParam->sizeOf     = param->typeInfo->sizeOf;
        funcParam->index      = index++;
        funcParam->declNode   = nodeParam;

        // Code is only valid for a macro or mixin
        auto paramType = nodeParam->typeInfo;
        if (paramType->kind == TypeInfoKind::Code)
            SWAG_VERIFY(funcNode->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN), context->report({nodeParam, Msg0729}));
        if (paramType->kind == TypeInfoKind::NameAlias)
            SWAG_VERIFY(funcNode->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN), context->report({nodeParam, Msg0730}));

        // Not everything is possible for types for attributes
        if (param->ownerScope->kind == ScopeKind::Attribute)
        {
            SWAG_VERIFY(funcParam->typeInfo->kind == TypeInfoKind::Native || funcParam->typeInfo->kind == TypeInfoKind::Enum, context->report({nodeParam, format(Msg0731, funcParam->typeInfo->getDisplayName().c_str())}));
            SWAG_VERIFY(funcParam->typeInfo->nativeType != NativeTypeKind::Any, context->report({nodeParam, format(Msg0731, funcParam->typeInfo->getDisplayName().c_str())}));
        }

        parameters->inheritOrFlag(nodeParam->type, AST_IS_GENERIC);

        // Variadic must be the last one
        if (paramType->kind == TypeInfoKind::Variadic)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, Msg0733}));

            typeInfo->flags |= TYPEINFO_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, Msg0734});
        }
        else if (paramType->kind == TypeInfoKind::TypedVariadic)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, Msg0733}));

            typeInfo->flags |= TYPEINFO_TYPED_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, Msg0734});
        }

        // A struct/interface is forced to be a const reference
        else if (paramType->kind == TypeInfoKind::Struct)
        {
            auto typeRef          = allocType<TypeInfoReference>();
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

            if (nodeParam->assignment->kind == AstNodeKind::CompilerSpecialFunction)
            {
                switch (nodeParam->assignment->token.id)
                {
                case TokenId::CompilerCallerLocation:
                case TokenId::CompilerCallerFunction:
                    break;

                default:
                    context->report({nodeParam->assignment, format(Msg0737, nodeParam->assignment->token.text.c_str())});
                    break;
                }
            }
        }
        else if (nodeParam->typeInfo->kind != TypeInfoKind::Code)
        {
            SWAG_VERIFY(!defaultValueDone, context->report({nodeParam, format(Msg0738, index)}));
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

    if (module->kind == ModuleKind::BootStrap || module->kind == ModuleKind::Runtime)
        return true;

    // Post user message
    auto node     = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    // Filter what we send
    if (sourceFile->imported)
        return true;
    if (!node->ownerScope->isGlobalOrImpl())
        return true;
    if (node->attributeFlags & ATTRIBUTE_GENERATED_FUNC)
        return true;
    if (node->flags & AST_IS_GENERIC)
        return true;

    ConcreteCompilerMessage msg;
    msg.kind        = CompilerMsgKind::SemanticFunc;
    msg.name.buffer = (void*) node->token.text.c_str();
    msg.name.count  = node->token.text.length();
    msg.type        = (ConcreteTypeInfo*) typeInfo; // Will be converted if really sent
    module->postCompilerMessage(msg);

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
    context->node = saveNode;
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
            Diagnostic diag({node, node->token, Msg0739});
            Diagnostic note{module->mainIsDefined, sourceFile->module->mainIsDefined->token, Msg0884, DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        sourceFile->module->mainIsDefined = node;
    }

    // No semantic on a generic function, or a macro
    if (node->flags & AST_IS_GENERIC)
    {
        if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(node->flags & AST_GENERATED))
            node->ownerScope->addPublicInlinedFunc(node);
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
                return context->report({node, node->token, format(Msg0741, node->token.text.c_str(), n)});
            mask >>= 1;
        }
    }

    if (node->attributeFlags & ATTRIBUTE_MACRO)
    {
        if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(node->flags & AST_GENERATED) && !(node->flags & AST_FROM_GENERIC))
            node->ownerScope->addPublicInlinedFunc(node);
        SWAG_CHECK(setFullResolve(context, node));
        return true;
    }

    // An inline function will not have bytecode, so need to register public by hand now
    if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && (node->attributeFlags & ATTRIBUTE_INLINE) && !(node->flags & AST_FROM_GENERIC))
        node->ownerScope->addPublicInlinedFunc(node);

    node->byteCodeFct   = ByteCodeGenJob::emitLocalFuncDecl;
    typeInfo->stackSize = node->stackSize;

    // Check attributes
    if ((node->attributeFlags & ATTRIBUTE_FOREIGN) && node->content)
        return context->report({node, node->token, Msg0742});

    if (!(node->attributeFlags & ATTRIBUTE_GENERATED_FUNC))
    {
        if (node->flags & AST_SPECIAL_COMPILER_FUNC)
        {
            SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_INLINE), context->report({node, node->token, Msg0743}));
        }

        if (node->attributeFlags & ATTRIBUTE_TEST_FUNC)
        {
            SWAG_VERIFY(module->kind == ModuleKind::Test, context->report({node, node->token, Msg0744}));
            SWAG_VERIFY(node->returnType->typeInfo == g_TypeMgr.typeInfoVoid, context->report({node->returnType, Msg0745}));
            SWAG_VERIFY(!node->parameters || node->parameters->childs.size() == 0, context->report({node->parameters, Msg0746}));
        }

        if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        {
            SWAG_VERIFY(node->ownerScope->isGlobalOrImpl(), context->report({node, node->token, format(Msg0747, node->getDisplayName().c_str())}));
        }
    }

    // Can be null for intrinsics etc...
    if (node->content)
    {
        node->content->allocateExtension();
        node->content->extension->byteCodeBeforeFct = ByteCodeGenJob::emitBeforeFuncDeclContent;
    }

    // Do we have a return value
    if (node->content && node->returnType && node->returnType->typeInfo != g_TypeMgr.typeInfoVoid)
    {
        if (!(node->content->flags & AST_NO_SEMANTIC))
        {
            if (!(node->semFlags & AST_SEM_SCOPE_HAS_RETURN))
            {
                if (node->semFlags & AST_SEM_FCT_HAS_RETURN)
                    return context->report({node, node->token, format(Msg0748, node->getDisplayName().c_str())});
                return context->report({node, node->token, format(Msg0749, node->getDisplayName().c_str())});
            }
        }
    }

    // Content semantic can have been disabled (#selectif). In that case, we're not done yet, so
    // do not set the FULL_RESOLVE flag and do not generate bytecode
    if (node->content && (node->content->flags & AST_NO_SEMANTIC))
    {
        scoped_lock lk(node->mutex);
        node->semFlags |= AST_SEM_PARTIAL_RESOLVE;
        node->dependentJobs.setRunning();
        return true;
    }

    // Now the full function has been solved, so we wakeup jobs depending on that
    SWAG_CHECK(setFullResolve(context, node));

    // Ask for bytecode
    bool genByteCode = true;
    if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
        genByteCode = false;
    if (node->token.text[0] == '@' && !(node->flags & AST_DEFINED_INTRINSIC))
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
    funcNode->semFlags |= AST_SEM_FULL_RESOLVE | AST_SEM_PARTIAL_RESOLVE;
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

    // If this is a #compiler function, we must have a flag mask as parameters
    if ((funcNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC) && funcNode->parameters)
    {
        auto parameters = funcNode->parameters;
        auto paramType  = TypeManager::concreteType(parameters->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
        SWAG_VERIFY(paramType->kind == TypeInfoKind::Enum, context->report({parameters, Msg0750}));
        paramType->computeScopedName();
        SWAG_VERIFY(paramType->scopedName == "Swag.CompilerMsgMask", context->report({parameters, Msg0750}));
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

    SWAG_VERIFY(!typeNode->typeInfo->isArrayOfRelative(), context->report({typeNode, format(Msg0752)}));

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
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(funcNode->semanticState == AstNodeResolveState::ProcessingChilds);
    SWAG_CHECK(collectAttributes(context, funcNode, &typeInfo->attributes));

    // Check attributes
    if (funcNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
        funcNode->flags |= AST_CONST_EXPR;

    SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_COMPLETE) || funcNode->token.text == "opAffect", context->report({funcNode, funcNode->token, format(Msg0753, funcNode->token.text.c_str())}));
    SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_IMPLICIT) || funcNode->token.text == "opAffect" || funcNode->token.text == "opCast", context->report({funcNode, funcNode->token, format(Msg0754, funcNode->token.text.c_str())}));
    SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_NO_RETURN) || (funcNode->attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO)), context->report({funcNode, funcNode->token, format(Msg0755, funcNode->token.text.c_str())}));

    // implicit attribute cannot be used on a generic function
    if (funcNode->attributeFlags & ATTRIBUTE_IMPLICIT && (funcNode->flags & (AST_IS_GENERIC | AST_FROM_GENERIC)))
        return context->report({funcNode, funcNode->token, format(Msg0756, funcNode->token.text.c_str())});

    if (!(funcNode->flags & AST_FROM_GENERIC))
    {
        if (funcNode->attributeFlags & ATTRIBUTE_MACRO)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, format(Msg0757, funcNode->token.text.c_str())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MIXIN), context->report({funcNode, funcNode->token, format(Msg0758, funcNode->token.text.c_str())}));
            funcNode->attributeFlags |= ATTRIBUTE_INLINE;
        }

        if (funcNode->attributeFlags & ATTRIBUTE_MIXIN)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, format(Msg0759, funcNode->token.text.c_str())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MACRO), context->report({funcNode, funcNode->token, format(Msg0760, funcNode->token.text.c_str())}));
            funcNode->attributeFlags |= ATTRIBUTE_INLINE;
            funcNode->attributeFlags |= ATTRIBUTE_MACRO;
        }

        if (funcNode->flags & AST_SPECIAL_COMPILER_FUNC)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MACRO), context->report({funcNode, funcNode->token, format(Msg0761, funcNode->token.text.c_str())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MIXIN), context->report({funcNode, funcNode->token, format(Msg0762, funcNode->token.text.c_str())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, format(Msg0763, funcNode->token.text.c_str())}));
        }
    }

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
        typeInfo->returnType->kind != TypeInfoKind::TypeSet &&
        typeInfo->returnType->kind != TypeInfoKind::Array &&
        typeInfo->returnType->kind != TypeInfoKind::Pointer)
        return context->report({typeNode->childs.front(), format(Msg0764, typeInfo->returnType->getDisplayName().c_str())});

    typeInfo->forceComputeName();

    // Special functions registration
    if (funcNode->parameters && funcNode->parameters->childs.size() == 1)
    {
        if (funcNode->token.text == "opInit")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserInitFct = funcNode;
        }
        else if (funcNode->token.text == "opDrop")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserDropFct = funcNode;
        }
        else if (funcNode->token.text == "opReloc")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserRelocFct = funcNode;
        }
        else if (funcNode->token.text == "opPostCopy")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserPostCopyFct = funcNode;
            SWAG_VERIFY(!(typeStruct->flags & TYPEINFO_STRUCT_NO_COPY), context->report({funcNode, funcNode->token, format(Msg0765, typeStruct->name.c_str())}));
        }
        else if (funcNode->token.text == "opPostMove")
        {
            auto        typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto        typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            scoped_lock lk(typeStruct->mutex);
            typeStruct->opUserPostMoveFct = funcNode;
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

    // Set storageIndex of each parameters
    setFuncDeclParamsIndex(funcNode);

    // For a short lambda without a specified return type, we need to defer the symbol registration, as we
    // need to infer it from the lambda expression
    SWAG_CHECK(registerFuncSymbol(context, funcNode, shortLambda ? OVERLOAD_INCOMPLETE : 0));

    return true;
}

bool SemanticJob::registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode, uint32_t symbolFlags)
{
    if (!(symbolFlags & OVERLOAD_INCOMPLETE))
    {
        SWAG_CHECK(checkFuncPrototype(context, funcNode));

        // The function wants to return something, but has the 'Swag.noreturn' attribute
        if (!funcNode->returnType->typeInfo->isNative(NativeTypeKind::Void) && (funcNode->attributeFlags & ATTRIBUTE_NO_RETURN))
            return context->report({funcNode->returnType, Msg0766});
        // The function returns nothing but has the 'Swag.discardable' attribute
        if (funcNode->returnType->typeInfo->isNative(NativeTypeKind::Void) && funcNode->attributeFlags & ATTRIBUTE_DISCARDABLE)
            return context->report({funcNode, funcNode->token, Msg0767});
    }

    if (funcNode->flags & AST_IS_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;

    auto typeFunc                    = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    funcNode->resolvedSymbolOverload = funcNode->ownerScope->symTable.addSymbolTypeInfo(context, funcNode, funcNode->typeInfo, SymbolKind::Function, nullptr, symbolFlags, &funcNode->resolvedSymbolName);
    SWAG_CHECK(funcNode->resolvedSymbolOverload);

    // If the function returns a struct, register a type alias "retval". This way we can resolve an identifier
    // named retval for "var result: retval{xx, xxx}" syntax
    auto returnType = TypeManager::concreteType(funcNode->returnType->typeInfo, CONCRETE_ALIAS);
    if (returnType->kind == TypeInfoKind::Struct)
    {
        Utf8 retVal = "retval";
        funcNode->ownerScope->symTable.addSymbolTypeInfo(context, funcNode->returnType, returnType, SymbolKind::TypeAlias, nullptr, symbolFlags | OVERLOAD_RETVAL, nullptr, 0, &retVal);
    }

    // Register method
    if (!(symbolFlags & OVERLOAD_INCOMPLETE) && isMethod(funcNode))
    {
        SWAG_ASSERT(funcNode->methodParam);
        funcNode->methodParam->attributes = typeFunc->attributes;
        auto typeStruct                   = CastTypeInfo<TypeInfoStruct>(funcNode->ownerStructScope->owner->typeInfo, TypeInfoKind::Struct);
        context->job->decreaseMethodCount(typeStruct);
    }

    resolveSubDecls(context, funcNode);
    return true;
}

bool SemanticJob::isMethod(AstFuncDecl* funcNode)
{
    if (funcNode->ownerStructScope &&
        funcNode->parent->kind != AstNodeKind::CompilerAst &&
        funcNode->parent->kind != AstNodeKind::CompilerRun &&
        funcNode->parent->kind != AstNodeKind::CompilerSelectIf &&
        funcNode->parent->kind != AstNodeKind::CompilerCheckIf &&
        !(funcNode->flags & AST_FROM_GENERIC) &&
        (funcNode->ownerScope->kind == ScopeKind::Struct) &&
        (funcNode->ownerStructScope->owner->typeInfo->kind == TypeInfoKind::Struct))
    {
        return true;
    }

    return false;
}

void SemanticJob::launchResolveSubDecl(JobContext* context, AstNode* node)
{
    // If AST_DONE_FILE_JOB_PASS is set, then the file job has already seen the sub declaration, ignored it
    // because of AST_NO_SEMANTIC, but the attribute context is ok. So we need to trigger the job by hand.
    // If AST_DONE_FILE_JOB_PASS is not set, then we just have to remove the AST_NO_SEMANTIC flag, and the
    // file job will trigger the resolve itself
    node->flags &= ~AST_NO_SEMANTIC;
    if (node->doneFlags & AST_DONE_FILE_JOB_PASS)
    {
        auto job          = g_Pool_semanticJob.alloc();
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
            unique_lock lk(f->mutex);

            // Disabled by #if block
            if (f->semFlags & AST_SEM_DISABLED)
                continue;
            f->semFlags |= AST_SEM_DISABLED; // To avoid multiple resolutions

            if (f->ownerCompilerIfBlock && f->ownerCompilerIfBlock->ownerFct == funcNode)
            {
                unique_lock lk1(f->ownerCompilerIfBlock->mutex);
                f->ownerCompilerIfBlock->subDecls.push_back(f);
            }
            else
                launchResolveSubDecl(context, f);
        }
    }
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
    node->inheritOrFlag(child, AST_CONST_EXPR | AST_IS_GENERIC | AST_VALUE_IS_TYPEINFO);

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

    if (child->extension && child->extension->resolvedUserOpSymbolOverload)
    {
        node->allocateExtension();
        node->extension->resolvedUserOpSymbolOverload = child->extension->resolvedUserOpSymbolOverload;
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
        return context->report({node->parent->childs[idx + 1], Msg0768, DiagnosticLevel::Warning});
    }

    return true;
}

bool SemanticJob::resolveRetVal(SemanticContext* context)
{
    auto node    = context->node;
    auto fctDecl = node->ownerInline ? node->ownerInline->func : node->ownerFct;

    SWAG_VERIFY(fctDecl, context->report({node, node->token, Msg0769}));
    SWAG_VERIFY(node->ownerScope && node->ownerScope->kind != ScopeKind::Function, context->report({node, node->token, Msg0769}));

    auto fct     = CastAst<AstFuncDecl>(fctDecl, AstNodeKind::FuncDecl);
    auto typeFct = CastTypeInfo<TypeInfoFuncAttr>(fct->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_VERIFY(typeFct->returnType && !typeFct->returnType->isNative(NativeTypeKind::Void), context->report({node, node->token, Msg0771}));

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

void SemanticJob::propagateReturn(AstReturn* node)
{
    auto stopFct = node->ownerFct->parent;
    if (node->semFlags & AST_SEM_EMBEDDED_RETURN)
        stopFct = node->ownerInline->parent;

    AstNode* scanNode = node;
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
            if (sc->isDefault)
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

    while (scanNode && scanNode != stopFct)
    {
        if (scanNode->semFlags & AST_SEM_FCT_HAS_RETURN)
            break;
        scanNode->semFlags |= AST_SEM_FCT_HAS_RETURN;
        scanNode = scanNode->parent;
    }
}

bool SemanticJob::resolveReturn(SemanticContext* context)
{
    SWAG_CHECK(checkUnreachableCode(context));

    auto node = CastAst<AstReturn>(context->node, AstNodeKind::Return);

    // For a return inside an inline block, take the original function, except if it is flagged with 'Swag.noreturn'
    auto funcNode = node->ownerFct;
    if (node->ownerInline)
    {
        if (!(node->ownerInline->func->attributeFlags & ATTRIBUTE_NO_RETURN) && !(node->flags & AST_IN_MIXIN))
        {
            node->semFlags |= AST_SEM_EMBEDDED_RETURN;
            funcNode = node->ownerInline->func;
        }
    }

    node->byteCodeFct      = ByteCodeGenJob::emitReturn;
    node->resolvedFuncDecl = funcNode;

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
            typeInfoFunc->returnType = TypeManager::promoteUntyped(typeInfoFunc->returnType);
            auto concreteReturn      = TypeManager::concreteType(typeInfoFunc->returnType);
            if (concreteReturn->kind == TypeInfoKind::TypeListTuple)
            {
                SWAG_CHECK(convertLiteralTupleToStructDecl(context, funcNode->content, node->childs.front(), &funcNode->returnType));
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
    }

    if (node->childs.empty())
    {
        return context->report({node, node->token, format(Msg0772, funcNode->returnType->typeInfo->getDisplayName().c_str())});
    }

    auto returnType = funcNode->returnType->typeInfo;

    // Check types
    auto child = node->childs[0];
    SWAG_CHECK(checkIsConcrete(context, child));

    // Be sure we do not specify a return value, and the function does not have a return type
    // (better error message than just letting the makeCompatibles do its job)
    auto concreteType = TypeManager::concreteType(child->typeInfo);
    if (returnType->isNative(NativeTypeKind::Void) && !concreteType->isNative(NativeTypeKind::Void))
    {
        Diagnostic  diag{child, format(Msg0774, concreteType->getDisplayName().c_str(), funcNode->getDisplayName().c_str())};
        PushErrHint errh(Msg0773);

        if (node->ownerInline && !(node->semFlags & AST_SEM_EMBEDDED_RETURN))
        {
            Diagnostic note{funcNode,
                            funcNode->token,
                            format(Note011,
                                   node->ownerInline->func->getDisplayName().c_str(),
                                   funcNode->getDisplayName().c_str()),
                            DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        return context->report(diag);
    }

    // If returning retval, then returning nothing, as we will change the return parameter value in place
    if (child->resolvedSymbolOverload && child->resolvedSymbolOverload->flags & OVERLOAD_RETVAL)
        node->typeInfo = child->typeInfo;
    else
    {
        // If we are returning an interface, be sure they are defined before casting
        if (returnType && returnType->kind == TypeInfoKind::Interface)
        {
            context->job->waitForAllStructInterfaces(child->typeInfo);
            if (context->result != ContextResult::Done)
                return true;
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, CASTFLAG_UNCONST | CASTFLAG_AUTO_OPCAST | CASTFLAG_TRY_COERCE));
    }

    if (child->kind == AstNodeKind::ExpressionList)
    {
        child->flags |= AST_TRANSIENT;
    }

    // When returning a struct, we need to know if postcopy or postmove are here, and wait for them to resolve
    if (returnType && (returnType->kind == TypeInfoKind::Struct || returnType->isArrayOfStruct()))
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
        typeInfoFunc->returnType = funcNode->returnType->typeInfo;
        typeInfoFunc->forceComputeName();
        SWAG_CHECK(registerFuncSymbol(context, funcNode));
    }

    return true;
}

bool SemanticJob::makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    CloneContext cloneContext;

    // Be sure this is not recursive
    auto ownerInline = identifier->ownerInline;
    while (ownerInline)
    {
        if (ownerInline->func == funcDecl)
            return context->report({identifier, format(Msg0775, identifier->token.text.c_str())});
        ownerInline = ownerInline->ownerInline;
    }

    // The content will be inline in its separated syntax block
    auto inlineNode                 = Ast::newInline(identifier->sourceFile, identifier);
    inlineNode->attributeFlags      = funcDecl->attributeFlags;
    inlineNode->func                = funcDecl;
    inlineNode->scope               = identifier->ownerScope;
    inlineNode->ownerTryCatchAssume = identifier->ownerTryCatchAssume;
    inlineNode->allocateExtension();

    if (funcDecl->extension)
    {
        inlineNode->extension->alternativeScopes     = funcDecl->extension->alternativeScopes;
        inlineNode->extension->alternativeScopesVars = funcDecl->extension->alternativeScopesVars;
    }

    // Try/Assume
    if (inlineNode->ownerTryCatchAssume && (inlineNode->func->typeInfo->flags & TYPEINFO_CAN_THROW))
    {
        switch (inlineNode->ownerTryCatchAssume->kind)
        {
        case AstNodeKind::Try:
            inlineNode->extension->byteCodeAfterFct = ByteCodeGenJob::emitTry;
            break;
        case AstNodeKind::Assume:
            inlineNode->extension->byteCodeAfterFct = ByteCodeGenJob::emitAssume;
            break;
        }

        // Reset emit from the modifier if it exists, as the inline block will deal with that
        if (identifier->extension)
        {
            if (identifier->extension->byteCodeAfterFct == ByteCodeGenJob::emitTry)
                identifier->extension->byteCodeAfterFct = nullptr;
            if (identifier->extension->byteCodeAfterFct == ByteCodeGenJob::emitAssume)
                identifier->extension->byteCodeAfterFct = nullptr;
        }
    }

    // We need to add the parent scope of the inline function (the global one), in order for
    // the inline content to be resolved in the same context as the original function
    auto globalScope = funcDecl->ownerScope;
    while (!globalScope->isGlobalOrImpl())
        globalScope = globalScope->parentScope;
    inlineNode->extension->alternativeScopes.push_back(globalScope);

    // We also need to add all alternatives scopes (using), in case the function relies on them to
    // be solved
    AstNode* parentNode = funcDecl;
    while (parentNode)
    {
        if (parentNode->extension && !parentNode->extension->alternativeScopes.empty())
            inlineNode->extension->alternativeScopes.append(parentNode->extension->alternativeScopes);
        parentNode = parentNode->parent;
    }

    // If function has generic parameters, then the block resolution of identifiers needs to be able to find the generic parameters
    // so we register all those generic parameters in a special scope (we cannot just register the scope of the function because
    // they are other stuff here)
    if (funcDecl->genericParameters)
    {
        Scope* scope = Ast::newScope(inlineNode, "", ScopeKind::Statement, nullptr);
        inlineNode->extension->alternativeScopes.push_back(scope);
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
        newScope          = Ast::newScope(inlineNode, format("__inline%d", identifier->ownerScope->childScopes.size()), ScopeKind::Inline, identifier->ownerScope);
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

    // Register all aliases
    if (identifier->kind == AstNodeKind::Identifier)
    {
        // Replace user aliases of the form @alias?
        // Can come from the identifier itself (for visit) or from call parameters (for macros/mixins)
        auto id  = CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier);
        int  idx = 0;
        for (auto& alias : id->aliasNames)
            cloneContext.replaceNames[format("@alias%d", idx++)] = alias.text;
        idx = 0;
        for (auto& alias : id->callParameters->aliasNames)
            cloneContext.replaceNames[format("@alias%d", idx++)] = alias.text;

        for (auto child : id->callParameters->childs)
        {
            auto param = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            if (!param->resolvedParameter)
                continue;

            // Transmit code type
            if (param->typeInfo->kind == TypeInfoKind::Code)
            {
                inlineNode->parametersScope->symTable.addSymbolTypeInfo(context, param, param->typeInfo, SymbolKind::Variable, nullptr, 0, nullptr, 0, &param->resolvedParameter->namedParam);
            }

            // Replace named aliases
            if (param->resolvedParameter->typeInfo->kind == TypeInfoKind::NameAlias)
            {
                SWAG_VERIFY(child->kind == AstNodeKind::FuncCallParam, context->report({child, Msg0776}));
                auto back = child->childs.back();
                if (back->kind == AstNodeKind::CompilerCode)
                    back = back->childs.front();
                SWAG_VERIFY(back->kind == AstNodeKind::IdentifierRef, context->report({child, Msg0777}));

                auto idRef = CastAst<AstIdentifierRef>(back, AstNodeKind::IdentifierRef);
                SWAG_VERIFY(idRef->childs.size() == 1, context->report({child, Msg0778}));
                cloneContext.replaceNames[param->resolvedParameter->namedParam] = idRef->childs.back()->token.text;
            }
        }
    }

    // Clone !
    auto newContent = funcDecl->content->clone(cloneContext);

    // Check used aliases
    // Error if an alias has been defined, but not 'eaten' by the function
    if (identifier->kind == AstNodeKind::Identifier)
    {
        if (cloneContext.replaceNames.size() != cloneContext.usedReplaceNames.size())
        {
            auto id = CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier);
            for (auto& r : cloneContext.replaceNames)
            {
                auto it = cloneContext.usedReplaceNames.find(r.second);
                if (it == cloneContext.usedReplaceNames.end())
                {
                    context->expansionNode.push_back({identifier, JobContext::ExpansionType::Inline});
                    for (auto& alias : id->aliasNames)
                    {
                        if (alias.text == r.second)
                            return context->report({id, alias, format(Msg0780, alias.text.c_str())});
                    }

                    for (auto& alias : id->callParameters->aliasNames)
                    {
                        if (alias.text == r.second)
                            return context->report({id, alias, format(Msg0780, alias.text.c_str())});
                    }
                }
            }
        }
    }

    if (newContent->extension)
    {
        newContent->extension->byteCodeBeforeFct = nullptr;
        if (funcDecl->attributeFlags & ATTRIBUTE_MIXIN)
            newContent->extension->byteCodeAfterFct = nullptr; // Do not release the scope, as there's no specific scope
    }

    newContent->flags &= ~AST_NO_SEMANTIC;

    // Sub declarations in the inline block, like sub functions
    if (!funcDecl->subDecls.empty())
    {
        context->expansionNode.push_back({identifier, JobContext::ExpansionType::Inline});
        SWAG_VERIFY(inlineNode->ownerFct, context->report({funcDecl, funcDecl->token, format(Msg0781, identifier->token.text.c_str())}));

        // Authorize a sub function to access inline parameters, if possible
        // This will work for compile time values, otherwise we will have an out of stack frame when generating the code
        cloneContext.alternativeScope = inlineNode->parametersScope;

        SWAG_CHECK(funcDecl->cloneSubDecls(context, cloneContext, funcDecl->content, inlineNode->ownerFct, newContent));
        context->expansionNode.pop_back();
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

    return true;
}

bool SemanticJob::makeInline(SemanticContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    SWAG_CHECK(makeInline((JobContext*) context, funcDecl, identifier));
    context->result = ContextResult::NewChilds;
    return true;
}