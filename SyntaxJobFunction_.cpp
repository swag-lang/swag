#include "pch.h"
#include "Ast.h"
#include "Scoped.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "TypeManager.h"

bool SyntaxJob::doFuncCallParameters(AstNode* parent, AstNode** result)
{
    auto callParams         = Ast::newNode<AstNode>(this, AstNodeKind::FuncCallParams, sourceFile, parent);
    *result                 = callParams;
    callParams->semanticFct = SemanticJob::resolveFuncCallParams;

    if (token.id != TokenId::SymLeftParen)
    {
        auto param         = Ast::newNode<AstFuncCallParam>(this, AstNodeKind::FuncCallParam, sourceFile, callParams);
        param->semanticFct = SemanticJob::resolveFuncCallParam;
        switch (token.id)
        {
        case TokenId::Identifier:
        {
            auto identifierRef         = Ast::newNode<AstIdentifierRef>(this, AstNodeKind::IdentifierRef, sourceFile, param);
            identifierRef->semanticFct = SemanticJob::resolveIdentifierRef;
            SWAG_CHECK(doIdentifier(identifierRef, false));
            break;
        }
        case TokenId::LiteralCharacter:
        case TokenId::LiteralNumber:
        case TokenId::LiteralString:
            SWAG_CHECK(doLiteral(param));
            break;
        case TokenId::NativeType:
            SWAG_CHECK(doTypeExpression(param));
            break;
        default:
            return sourceFile->report({param, format("invalid generic argument '%s'", token.text.c_str())});
        }
    }
    else
    {
        SWAG_CHECK(eatToken(TokenId::SymLeftParen));
        while (token.id != TokenId::SymRightParen)
        {
            while (true)
            {
                auto param         = Ast::newNode<AstFuncCallParam>(this, AstNodeKind::FuncCallParam, sourceFile, callParams);
                param->semanticFct = SemanticJob::resolveFuncCallParam;
                param->token       = token;
                AstNode* paramExpression;
                SWAG_CHECK(doExpression(nullptr, &paramExpression));

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
    }

    return true;
}

bool SyntaxJob::doFuncDeclParameter(AstNode* parent)
{
    auto paramNode = Ast::newVarDecl(sourceFile, "", parent, this, AstNodeKind::FuncDeclParam);

    // Using variable
    if (token.id == TokenId::KwdUsing)
    {
        SWAG_CHECK(eatToken());
        paramNode->flags |= AST_DECL_USING;
    }

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    paramNode->inheritTokenName(token);

    // 'self'
    if (paramNode->name == "self")
    {
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(paramNode->ownerStructScope, error(token, "'self' can only be used in an 'impl' block"));
        auto typeNode        = Ast::newTypeExpression(sourceFile, paramNode);
        typeNode->ptrCount   = 1;
        typeNode->identifier = Ast::newIdentifierRef(sourceFile, paramNode->ownerStructScope->name, typeNode, this);
        paramNode->type      = typeNode;
    }
    else
    {
        // Multiple declaration
        vector<AstVarDecl*> otherVariables;
        SWAG_CHECK(tokenizer.getToken(token));
        while (token.id == TokenId::SymComma)
        {
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid parameter name '%s'", token.text.c_str())));

            AstVarDecl* otherVarNode = Ast::newVarDecl(sourceFile, token.text, parent, this, AstNodeKind::FuncDeclParam);
            SWAG_CHECK(tokenizer.getToken(token));
            otherVariables.push_back(otherVarNode);
        }

        // Type
        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());

            // ...
            if (token.id == TokenId::SymDotDotDot)
            {
                paramNode->type                    = Ast::newTypeExpression(sourceFile, paramNode);
                paramNode->type->token.literalType = g_TypeMgr.typeInfoVariadic;
                SWAG_CHECK(tokenizer.getToken(token));
            }
            else
            {
                AstNode* typeExpression;
                SWAG_CHECK(doTypeExpression(nullptr, &typeExpression));

                // type...
                if (token.id == TokenId::SymDotDotDot)
                {
                    paramNode->type                    = Ast::newTypeExpression(sourceFile, paramNode);
                    paramNode->type->token.literalType = g_TypeMgr.typeInfoVariadic;
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
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doAssignmentExpression(paramNode, &paramNode->assignment));
        }

        // Propagate types and assignment to multiple declarations
        for (auto one : otherVariables)
        {
            CloneContext cloneContext;
            cloneContext.parent = one;
            if (paramNode->type)
                one->type = paramNode->type->clone(cloneContext);
            if (paramNode->assignment)
                one->assignment = paramNode->assignment->clone(cloneContext);
        }
    }

    Ast::setForceConstType(paramNode->type);

    // Be sure we will be able to have a type
    if (!paramNode->type && !paramNode->assignment)
        return error(paramNode->token, "parameter must be initialized because no type is specified");

    return true;
}

bool SyntaxJob::doFuncDeclParameters(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        auto allParams         = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, parent);
        allParams->semanticFct = SemanticJob::resolveFuncDeclParams;
        allParams->flags |= AST_NO_BYTECODE_CHILDS; // We do not want default assignations to generate bytecode
        if (result)
            *result = allParams;

        ScopedFlags scopedFlags(this, AST_IN_FCT_PROTOTYPE);
        while (token.id != TokenId::SymRightParen)
        {
            SWAG_CHECK(doFuncDeclParameter(allParams));
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

bool SyntaxJob::doLambdaFuncDecl(AstNode* parent, AstNode** result)
{
    auto funcNode         = Ast::newNode<AstFuncDecl>(this, AstNodeKind::FuncDecl, sourceFile, parent);
    funcNode->semanticFct = SemanticJob::resolveFuncDecl;
    if (result)
        *result = funcNode;
    int id         = g_Global.uniqueID.fetch_add(1);
    funcNode->name = "__lambda" + to_string(id);

    auto typeInfo      = g_Allocator.alloc<TypeInfoFuncAttr>();
    auto newScope      = Ast::newScope(funcNode, funcNode->name, ScopeKind::Function, currentScope);
    funcNode->typeInfo = typeInfo;
    funcNode->scope    = newScope;
    currentScope->symTable.registerSymbolName(&context, funcNode, SymbolKind::Function);

    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters));
    }

    funcNode->computeFullName();

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
    }

    return true;
}

bool SyntaxJob::doFuncDecl(AstNode* parent, AstNode** result, TokenId typeFuncId)
{
    auto funcNode         = Ast::newNode<AstFuncDecl>(this, AstNodeKind::FuncDecl, sourceFile, parent);
    funcNode->semanticFct = SemanticJob::resolveFuncDecl;
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
            funcNode->name = "test" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_TEST_FUNC;
            break;
        case TokenId::CompilerFuncInit:
            funcNode->token.text = "#init";
            funcNode->name = "init" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_INIT_FUNC;
            break;
        case TokenId::CompilerFuncDrop:
            funcNode->token.text = "#drop";
            funcNode->name = "drop" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_DROP_FUNC;
            break;
        case TokenId::CompilerRun:
            funcNode->token.text = "#run";
            funcNode->name = "run" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_RUN_FUNC | ATTRIBUTE_COMPILER;
            break;
        case TokenId::CompilerFuncMain:
            funcNode->token.text = "#main";
            funcNode->name = "main" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_MAIN_FUNC;
            break;
        }
    }
    else
    {
        // Generic parameters
        if (token.id == TokenId::SymLeftParen)
            SWAG_CHECK(doGenericDeclParameters(funcNode, &funcNode->genericParameters));

        SWAG_CHECK(checkIsName(token));
        SWAG_VERIFY(token.id != TokenId::Intrinsic || sourceFile->swagFile, syntaxError(token, "function names starting with '@' are reserved for intrinsics"));

        isIntrinsic = token.id == TokenId::Intrinsic;
        funcNode->inheritTokenName(token);
    }

    funcNode->computeFullName();

    // Register function name
    Scope* newScope = nullptr;
    {
        scoped_lock lk(currentScope->symTable.mutex);
        auto        typeInfo = g_Allocator.alloc<TypeInfoFuncAttr>();
        newScope             = Ast::newScope(funcNode, funcNode->name, ScopeKind::Function, currentScope);
        funcNode->typeInfo   = typeInfo;
        funcNode->scope      = newScope;
        auto symbolName      = currentScope->symTable.registerSymbolNameNoLock(&context, funcNode, SymbolKind::Function);
        if (funcNode->ownerCompilerIfBlock)
            funcNode->ownerCompilerIfBlock->addSymbol(symbolName);
    }

    // Dispatch owners
    if (funcNode->genericParameters)
    {
        Ast::visit(funcNode->genericParameters, [&](AstNode* n) {
            n->ownerFct   = funcNode;
            n->ownerScope = newScope;
        });
    }

    // Parameters
    if (!funcForCompiler)
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters));
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
            Scoped    scoped(this, newScope);
            ScopedFct scopedFct(this, funcNode);
            SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
            AstNode* typeExpression;
            SWAG_CHECK(doTypeExpression(typeNode, &typeExpression));
        }
    }

    // Content of function
    if (token.id == TokenId::SymSemiColon || isIntrinsic)
    {
        SWAG_CHECK(eatSemiCol("after function declaration"));
        return true;
    }

    {
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
        }

        // Normal curly statement
        else
        {
            ScopedAccessFlags scopedAccess(this, 0);
            SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content));
            funcNode->content->token = token;
        }
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