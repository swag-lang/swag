#include "pch.h"
#include "Ast.h"
#include "Scoped.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"

bool SyntaxJob::doGenericFuncCallParameters(AstNode* parent, AstNode** result)
{
    auto callParams         = Ast::newNode<AstNode>(this, AstNodeKind::FuncCallParams, sourceFile, parent);
    *result                 = callParams;
    callParams->semanticFct = SemanticJob::resolveFuncCallParams;

    bool multi = false;
    if (token.id == TokenId::SymLeftParen)
    {
        multi = true;
        SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    }

    while (token.id != TokenId::SymRightParen)
    {
        auto param         = Ast::newNode<AstFuncCallParam>(this, AstNodeKind::FuncCallParam, sourceFile, callParams);
        param->semanticFct = SemanticJob::resolveFuncCallParam;
        param->token       = token;
        switch (token.id)
        {
        case TokenId::Identifier:
        {
            SWAG_CHECK(doIdentifierRef(param, nullptr, false));
            break;
        }
        case TokenId::LiteralCharacter:
        case TokenId::LiteralNumber:
        case TokenId::LiteralString:
            SWAG_CHECK(doLiteral(param));
            break;

        case TokenId::CompilerLocation:
            SWAG_CHECK(doCompilerSpecialFunction(param));
            break;

        default:
            SWAG_CHECK(doTypeExpression(param));
            break;
        }

        if (!multi)
            break;
        if (token.id == TokenId::SymRightParen)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    if (multi)
        SWAG_CHECK(eatToken(TokenId::SymRightParen));

    return true;
}

bool SyntaxJob::doFuncCallParameters(AstNode* parent, AstNode** result)
{
    auto callParams         = Ast::newNode<AstNode>(this, AstNodeKind::FuncCallParams, sourceFile, parent);
    *result                 = callParams;
    callParams->semanticFct = SemanticJob::resolveFuncCallParams;

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    while (token.id != TokenId::SymRightParen)
    {
        while (true)
        {
            auto param         = Ast::newNode<AstFuncCallParam>(this, AstNodeKind::FuncCallParam, sourceFile, callParams);
            param->semanticFct = SemanticJob::resolveFuncCallParam;
            param->token       = token;
            AstNode* paramExpression;

            inFunCall = true;
            SWAG_CHECK(doExpression(nullptr, &paramExpression));
            inFunCall = false;

            // Name
            if (token.id == TokenId::SymColon)
            {
                if (paramExpression->kind != AstNodeKind::IdentifierRef || paramExpression->childs.size() != 1)
                    return sourceFile->report({paramExpression, format("invalid named parameter '%s'", token.text.c_str())});
                param->namedParamNode = paramExpression->childs.front();
                param->namedParam     = param->namedParamNode->name;
                SWAG_CHECK(eatToken());
                SWAG_CHECK(doExpression(param));
            }
            else
            {
                Ast::addChildBack(param, paramExpression);
            }

            if (token.id == TokenId::SymRightParen)
                break;
            SWAG_CHECK(eatToken(TokenId::SymComma));
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));

    return true;
}

bool SyntaxJob::doFuncDeclParameter(AstNode* parent, bool acceptMissingType)
{
    auto paramNode = Ast::newVarDecl(sourceFile, "", parent, this, AstNodeKind::FuncDeclParam);

    // Using variable
    if (token.id == TokenId::KwdUsing)
    {
        SWAG_CHECK(eatToken());
        paramNode->flags |= AST_DECL_USING;
    }

    SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::KwdConst, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    paramNode->inheritTokenName(token);

    // 'self'
    if (token.id == TokenId::KwdConst || paramNode->name == "self")
    {
        bool isConst = false;
        if (token.id == TokenId::KwdConst)
        {
            isConst = true;
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id == TokenId::Identifier && token.text == "self", syntaxError(token, "const before a function parameter name can only be followed by 'self'"));
        }

        SWAG_CHECK(eatToken());
        SWAG_VERIFY(paramNode->ownerStructScope, error(token, "'self' can only be used in an 'impl' block"));
        if (paramNode->ownerStructScope->kind == ScopeKind::Enum)
        {
            auto typeNode        = Ast::newTypeExpression(sourceFile, paramNode);
            typeNode->isConst    = true;
            typeNode->isSelf     = true;
            typeNode->identifier = Ast::newIdentifierRef(sourceFile, paramNode->ownerStructScope->name, typeNode, this);
            paramNode->type      = typeNode;
        }
        else
        {
            SWAG_VERIFY(paramNode->ownerStructScope->kind == ScopeKind::Struct, error(token, "'self' can only be used in an 'impl' block"));
            auto typeNode        = Ast::newTypeExpression(sourceFile, paramNode);
            typeNode->ptrCount   = 1;
            typeNode->isConst    = isConst;
            typeNode->isSelf     = true;
            typeNode->identifier = Ast::newIdentifierRef(sourceFile, paramNode->ownerStructScope->name, typeNode, this);
            paramNode->type      = typeNode;
        }
    }
    else
    {
        // Multiple declaration
        VectorNative<AstVarDecl*> otherVariables;
        SWAG_CHECK(tokenizer.getToken(token));
        while (token.id == TokenId::SymComma)
        {
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid parameter name '%s'", token.text.c_str())));

            AstVarDecl* otherVarNode = Ast::newVarDecl(sourceFile, token.text, parent, this, AstNodeKind::FuncDeclParam);
            SWAG_CHECK(tokenizer.getToken(token));
            otherVariables.push_back(otherVarNode);
        }

        bool hasType       = false;
        bool hasAssignment = false;

        // Type
        if (token.id == TokenId::SymColon)
        {
            hasType = true;

            SWAG_CHECK(eatToken());

            // ...
            if (token.id == TokenId::SymDotDotDot)
            {
                auto newTypeExpression         = Ast::newTypeExpression(sourceFile, paramNode);
                paramNode->type                = newTypeExpression;
                newTypeExpression->literalType = g_TypeMgr.typeInfoVariadic;
                SWAG_CHECK(tokenizer.getToken(token));
            }
            else
            {
                AstNode* typeExpression;
                SWAG_CHECK(doTypeExpression(nullptr, &typeExpression));

                // type...
                if (token.id == TokenId::SymDotDotDot)
                {
                    auto newTypeExpression         = Ast::newTypeExpression(sourceFile, paramNode);
                    paramNode->type                = newTypeExpression;
                    newTypeExpression->literalType = g_TypeMgr.typeInfoVariadic;
                    SWAG_CHECK(tokenizer.getToken(token));
                    Ast::addChildBack(paramNode->type, typeExpression);
                }
                else
                {
                    Ast::addChildBack(paramNode, typeExpression);
                    paramNode->type = typeExpression;
                }
            }
        }

        // Assignment
        if (token.id == TokenId::SymEqual)
        {
            hasAssignment = true;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doAssignmentExpression(paramNode, &paramNode->assignment));
        }

        if (!acceptMissingType && !hasType && !hasAssignment)
            return error(token, "missing function parameter type or assignment");

        // Propagate types and assignment to multiple declarations
        for (auto one : otherVariables)
        {
            CloneContext cloneContext;
            cloneContext.parent = one;

            if (paramNode->type)
                one->type = (AstTypeExpression*) paramNode->type->clone(cloneContext);
            if (paramNode->assignment)
                one->assignment = paramNode->assignment->clone(cloneContext);
        }
    }

    return true;
}

bool SyntaxJob::doFuncDeclParameters(AstNode* parent, AstNode** result, bool acceptMissingType)
{
    SWAG_CHECK(verifyError(token, token.id != TokenId::SymLeftCurly, "missing function parameters before '{'"));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen, format("to declare function parameters of '%s'", parent->name.c_str())));
    if (token.id != TokenId::SymRightParen)
    {
        auto allParams         = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, parent);
        allParams->semanticFct = SemanticJob::resolveFuncDeclParams;
        allParams->flags |= AST_NO_BYTECODE_CHILDS; // We do not want default assignations to generate bytecode
        if (result)
            *result = allParams;

        while (token.id != TokenId::SymRightParen)
        {
            SWAG_CHECK(doFuncDeclParameter(allParams, acceptMissingType));
            if (token.id == TokenId::SymRightParen)
                break;
            SWAG_CHECK(eatToken(TokenId::SymComma));
            SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::KwdUsing, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doGenericDeclParameters(AstNode* parent, AstNode** result)
{
    auto allParams = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, parent);
    if (result)
        *result = allParams;

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_VERIFY(token.id != TokenId::SymRightParen, syntaxError(token, "missing generic parameters"));

    while (token.id != TokenId::SymRightParen)
    {
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, "missing generic name or type"));
        auto oneParam = Ast::newVarDecl(sourceFile, token.text, allParams, this, AstNodeKind::FuncDeclParam);
        oneParam->flags |= AST_IS_GENERIC;
        SWAG_CHECK(eatToken());

        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(oneParam, &oneParam->type));
        }

        if (token.id == TokenId::SymEqual)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doAssignmentExpression(oneParam, &oneParam->assignment));
        }

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doFuncDecl(AstNode* parent, AstNode** result, TokenId typeFuncId)
{
    auto funcNode              = Ast::newNode<AstFuncDecl>(this, AstNodeKind::FuncDecl, sourceFile, parent, 4);
    funcNode->semanticFct      = SemanticJob::resolveFuncDecl;
    funcNode->semanticAfterFct = SemanticJob::resolveAfterFuncDecl;
    if (result)
        *result = funcNode;

    bool funcForCompiler = false;
    bool isIntrinsic     = false;

    if (typeFuncId == TokenId::Invalid)
    {
        typeFuncId = token.id;
        SWAG_CHECK(tokenizer.getToken(token));
    }

    if (typeFuncId == TokenId::CompilerFuncTest ||
        typeFuncId == TokenId::CompilerFuncInit ||
        typeFuncId == TokenId::CompilerFuncDrop ||
        typeFuncId == TokenId::CompilerFuncMain ||
        typeFuncId == TokenId::CompilerFuncCompiler ||
        typeFuncId == TokenId::CompilerAst ||
        typeFuncId == TokenId::CompilerGeneratedRun ||
        typeFuncId == TokenId::CompilerRun)
        funcForCompiler = true;

    // Name
    if (funcForCompiler)
    {
        funcNode->flags |= AST_SPECIAL_COMPILER_FUNC;
        int id = g_Global.uniqueID.fetch_add(1);
        switch (typeFuncId)
        {
        case TokenId::CompilerFuncTest:
            funcNode->token.text = "#test";
            funcNode->name       = "__test" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_TEST_FUNC;
            break;
        case TokenId::CompilerFuncInit:
            funcNode->token.text = "#init";
            funcNode->name       = "__init" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_INIT_FUNC;
            break;
        case TokenId::CompilerFuncDrop:
            funcNode->token.text = "#drop";
            funcNode->name       = "__drop" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_DROP_FUNC;
            break;
        case TokenId::CompilerRun:
            funcNode->token.text = "#run";
            funcNode->name       = "__run" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_RUN_FUNC | ATTRIBUTE_COMPILER;
            break;
        case TokenId::CompilerGeneratedRun:
            funcNode->token.text = "#run";
            funcNode->name       = "__run" + to_string(id);
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_COMPILER;
            break;
        case TokenId::CompilerFuncMain:
            funcNode->token.text = "#main";
            funcNode->name       = "__main" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_MAIN_FUNC;
            break;
        case TokenId::CompilerFuncCompiler:
            funcNode->token.text = "#compiler";
            funcNode->name       = "__compiler" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_COMPILER_FUNC;
            sourceFile->addCompilerPassNode(funcNode);
            break;
        case TokenId::CompilerAst:
            funcNode->token.text = "#ast";
            funcNode->name       = "__ast" + to_string(id);
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_AST_FUNC | ATTRIBUTE_CONSTEXPR | ATTRIBUTE_COMPILER | ATTRIBUTE_GENERATED_FUNC;
            break;
        }
    }
    else
    {
        // Generic parameters
        if (token.id == TokenId::SymLeftParen)
            SWAG_CHECK(doGenericDeclParameters(funcNode, &funcNode->genericParameters));

        isIntrinsic = token.text[0] == '@';
        if (isIntrinsic)
            SWAG_VERIFY(sourceFile->isBootstrapFile, syntaxError(token, "function names starting with '@' are reserved for intrinsics"));
        else
        {
            if (token.id != TokenId::Identifier &&
                token.id != TokenId::IntrinsicInit &&
                token.id != TokenId::IntrinsicDrop &&
                token.id != TokenId::KwdBreak)
                return syntaxError(token, format("missing name instead of '%s'", token.text.c_str()));
            SWAG_VERIFY(token.text != "drop", syntaxError(token, "a function cannot be named 'drop' (reserved by the compiler)"));
        }

        funcNode->inheritTokenName(token);
        SWAG_CHECK(isValidUserName(funcNode));
    }

    // Register function name
    Scope* newScope = nullptr;
    {
        scoped_lock lk(currentScope->symTable.mutex);
        auto        typeInfo = g_Allocator.alloc<TypeInfoFuncAttr>();
        typeInfo->declNode   = funcNode;

        newScope           = Ast::newScope(funcNode, funcNode->name, ScopeKind::Function, currentScope);
        funcNode->typeInfo = typeInfo;
        funcNode->scope    = newScope;
        auto symbolName    = currentScope->symTable.registerSymbolNameNoLock(&context, funcNode, SymbolKind::Function);
        if (funcNode->ownerCompilerIfBlock)
            funcNode->ownerCompilerIfBlock->addSymbol(symbolName);
    }

    // Count number of methods to resolve
    if (currentScope->kind == ScopeKind::Struct && !funcForCompiler)
    {
        auto typeStruct       = CastTypeInfo<TypeInfoStruct>(currentScope->owner->typeInfo, TypeInfoKind::Struct);
        auto typeParam        = g_Allocator.alloc<TypeInfoParam>();
        typeParam->namedParam = funcNode->name;
        typeParam->typeInfo   = funcNode->typeInfo;
        typeParam->node       = funcNode;
        funcNode->methodParam = typeParam;
        unique_lock lk(typeStruct->mutex);
        typeStruct->cptRemainingMethods++;
        typeStruct->methods.push_back(typeParam);
        if (funcNode->ownerCompilerIfBlock)
            funcNode->ownerCompilerIfBlock->methodsCount.push_back({typeStruct, (int) typeStruct->methods.size()});
    }

    // Dispatch owners
    if (funcNode->genericParameters)
    {
        Ast::visit(funcNode->genericParameters, [&](AstNode* n) {
            n->ownerFct   = funcNode;
            n->ownerScope = newScope;
            n->flags |= AST_IN_FCT_PROTOTYPE;
        });
    }

    // Parameters
    if (!funcForCompiler)
    {
        Scoped      scoped(this, newScope);
        ScopedFct   scopedFct(this, funcNode);
        ScopedFlags scopedFlags(this, AST_IN_FCT_PROTOTYPE);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters));
    }

    // #compiler has an expression has parameters
    else if (funcNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC)
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(eatToken(TokenId::SymLeftParen));
        SWAG_CHECK(doExpression(funcNode, &funcNode->parameters));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }

    // Return type
    auto typeNode         = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclType, sourceFile, funcNode);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = SemanticJob::resolveFuncDeclType;
    if (!funcForCompiler)
    {
        if (token.id == TokenId::SymMinusGreat)
        {
            typeNode->flags |= AST_FUNC_RETURN_DEFINED;
            Scoped      scoped(this, newScope);
            ScopedFct   scopedFct(this, funcNode);
            ScopedFlags scopedFlags(this, AST_IN_FCT_PROTOTYPE);
            SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
            AstNode* typeExpression;
            SWAG_CHECK(doTypeExpression(typeNode, &typeExpression));
        }
    }
    else if (typeFuncId == TokenId::CompilerAst)
    {
        typeNode->flags |= AST_FUNC_RETURN_DEFINED;
        Scoped      scoped(this, newScope);
        ScopedFct   scopedFct(this, funcNode);
        ScopedFlags scopedFlags(this, AST_IN_FCT_PROTOTYPE);
        auto        typeExpression  = Ast::newTypeExpression(sourceFile, typeNode, this);
        typeExpression->literalType = g_TypeMgr.typeInfoString;
    }

    funcNode->typeInfo->computeName();

    // Content of function
    if (token.id == TokenId::SymSemiColon || isIntrinsic)
    {
        SWAG_VERIFY(!funcForCompiler, syntaxError(token, format("special function '%s' must have a body", funcNode->token.text.c_str())));
        SWAG_CHECK(eatSemiCol("after function declaration"));
        return true;
    }

    {
        newScope = Ast::newScope(funcNode, funcNode->name, ScopeKind::FunctionBody, newScope);
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);

        // One single return expression
        if (token.id == TokenId::SymEqualGreater)
        {
            SWAG_CHECK(eatToken());
            auto stmt               = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, funcNode);
            auto returnNode         = Ast::newNode<AstReturn>(this, AstNodeKind::Return, sourceFile, stmt);
            returnNode->semanticFct = SemanticJob::resolveReturn;
            funcNode->content       = returnNode;
            funcNode->flags |= AST_SHORT_LAMBDA;
            SWAG_CHECK(doExpression(returnNode));
            funcNode->endToken = token;
        }

        // Normal curly statement
        else
        {
            ScopedAttributeFlags scopedAccess(this, 0);
            SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content, &funcNode->endToken));
        }

        newScope->owner                     = funcNode->content;
        funcNode->content->byteCodeAfterFct = &ByteCodeGenJob::emitLeaveScope;
    }

    return true;
}

bool SyntaxJob::doReturn(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstReturn>(this, AstNodeKind::Return, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveReturn;
    if (result)
        *result = node;

    // Return value
    SWAG_CHECK(tokenizer.getToken(token));
    if (tokenizer.lastTokenIsEOL)
        return true;
    if (token.id != TokenId::SymSemiColon)
        SWAG_CHECK(doExpression(node));

    return true;
}

bool SyntaxJob::doLambdaFuncDecl(AstNode* parent, AstNode** result, bool acceptMissingType)
{
    auto funcNode         = Ast::newNode<AstFuncDecl>(this, AstNodeKind::FuncDecl, sourceFile, parent, 4);
    funcNode->semanticFct = SemanticJob::resolveFuncDecl;
    funcNode->flags |= AST_GENERATED;
    if (result)
        *result = funcNode;
    int id         = g_Global.uniqueID.fetch_add(1);
    funcNode->name = "__lambda" + to_string(id);

    auto typeInfo      = g_Allocator.alloc<TypeInfoFuncAttr>();
    typeInfo->declNode = funcNode;

    auto newScope      = Ast::newScope(funcNode, funcNode->name, ScopeKind::Function, currentScope);
    funcNode->typeInfo = typeInfo;
    funcNode->scope    = newScope;
    currentScope->symTable.registerSymbolName(&context, funcNode, SymbolKind::Function);

    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters, acceptMissingType));
    }

    // Return type
    auto typeNode         = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclType, sourceFile, funcNode);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = SemanticJob::resolveFuncDeclType;
    if (token.id == TokenId::SymMinusGreat)
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
        AstNode* typeExpression;
        SWAG_CHECK(doTypeExpression(typeNode, &typeExpression));
        Ast::setForceConstType(typeExpression);
        typeNode->flags |= AST_FUNC_RETURN_DEFINED;
    }

    // Body
    {
        newScope = Ast::newScope(funcNode, funcNode->name, ScopeKind::FunctionBody, newScope);
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);

        // One single return expression
        if (token.id == TokenId::SymEqualGreater)
        {
            SWAG_CHECK(eatToken());
            auto returnNode         = Ast::newNode<AstReturn>(this, AstNodeKind::Return, sourceFile, funcNode);
            returnNode->semanticFct = SemanticJob::resolveReturn;
            funcNode->content       = returnNode;
            funcNode->flags |= AST_SHORT_LAMBDA;
            SWAG_CHECK(doExpression(returnNode));
        }

        // Normal curly statement
        else
        {
            SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content));
            funcNode->content->token = token;
        }

        funcNode->content->byteCodeAfterFct = &ByteCodeGenJob::emitLeaveScope;
        newScope->owner                     = funcNode->content;
    }

    return true;
}

bool SyntaxJob::doLambdaExpression(AstNode* parent, AstNode** result)
{
    // We accept missing types only if lambda is in a function call, because this is the only way
    // we will be able to deduce the type
    bool acceptMissingType = inFunCall;

    AstNode* lambda = nullptr;
    SWAG_CHECK(doLambdaFuncDecl(sourceFile->astRoot, &lambda, acceptMissingType));
    lambda->flags |= AST_IS_LAMBDA_EXPRESSION;

    // Lambda sub function will be resolved by the owner function
    SWAG_ASSERT(lambda->ownerFct);
    lambda->ownerFct->subFunctions.push_back(lambda);
    lambda->flags |= AST_NO_SEMANTIC;

    // Retrieve the point of the function
    auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::MakePointerLambda, sourceFile, parent);
    exprNode->inheritTokenLocation(lambda->token);
    exprNode->ownerMainNode = lambda;
    exprNode->semanticFct   = SemanticJob::resolveMakePointer;
    AstNode* identifierRef  = Ast::newIdentifierRef(sourceFile, lambda->name, exprNode, this);
    identifierRef->inheritTokenLocation(lambda->token);
    forceTakeAddress(identifierRef);

    if (result)
        *result = exprNode;
    return true;
}