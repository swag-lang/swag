#include "pch.h"
#include "Ast.h"
#include "Scoped.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "Naming.h"
#include "LanguageSpec.h"

bool Parser::doGenericFuncCallParameters(AstNode* parent, AstFuncCallParams** result)
{
    auto callParams = Ast::newFuncCallGenParams(sourceFile, parent, this);
    *result         = callParams;

    bool multi    = false;
    auto startLoc = token.startLocation;
    if (token.id == TokenId::SymLeftParen)
    {
        multi = true;
        SWAG_CHECK(eatToken());
    }

    while (token.id != TokenId::SymRightParen)
    {
        auto param   = Ast::newFuncCallParam(sourceFile, callParams, this);
        param->token = token;
        switch (token.id)
        {
        case TokenId::Identifier:
        {
            SWAG_CHECK(doIdentifierRef(param, &dummyResult, IDENTIFIER_NO_FCT_PARAMS));
            break;
        }

        case TokenId::KwdTrue:
        case TokenId::KwdFalse:
        case TokenId::KwdNull:
        case TokenId::CompilerFile:
        case TokenId::CompilerModule:
        case TokenId::CompilerLine:
        case TokenId::CompilerBuildVersion:
        case TokenId::CompilerBuildRevision:
        case TokenId::CompilerBuildNum:
        case TokenId::LiteralNumber:
        case TokenId::LiteralString:
        case TokenId::LiteralCharacter:
            SWAG_CHECK(doLiteral(param, &dummyResult));
            break;

        case TokenId::CompilerLocation:
            SWAG_CHECK(doCompilerSpecialValue(param, &dummyResult));
            break;

        case TokenId::SymLeftSquare:
        {
            // This is ambiguous. Can be a literal array or an array type.
            // If parameters are inside parenthesis, then this means that we can differentiate between the 2 cases
            // without the need of #type, as what follows a literal should be another parameter (,) or the closing parenthesis.
            // And this is a good idea to write Arr'([2] s32) instead of Arr'[2] s32 anyway. So this should remove some ambiguities.
            tokenizer.saveState(token);
            SWAG_CHECK(doExpressionListArray(param, &dummyResult));
            if (multi && token.id != TokenId::SymComma && token.id != TokenId::SymRightParen)
            {
                tokenizer.restoreState(token);
                Ast::removeFromParent(param->childs.back());
                SWAG_CHECK(doTypeExpression(param, EXPR_FLAG_NONE, &dummyResult));
            }
            break;
        }

        case TokenId::CompilerType:
        {
            SWAG_CHECK(eatToken());
            AstNode* resNode;
            SWAG_CHECK(doTypeExpression(param, EXPR_FLAG_NONE, &resNode));
            resNode->addSpecFlags(AstType::SPECFLAG_FORCE_TYPE);
            break;
        }

        default:
            SWAG_CHECK(doTypeExpression(param, EXPR_FLAG_NONE, &dummyResult));
            break;
        }

        if (!multi)
            break;
        if (token.id == TokenId::SymRightParen)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma, "to define another argument or ')' to end the list"));
    }

    if (multi)
        SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));

    return true;
}

bool Parser::doFuncCallParameters(AstNode* parent, AstFuncCallParams** result, TokenId closeToken)
{
    auto callParams = Ast::newFuncCallParams(sourceFile, parent, this);
    *result         = callParams;

    // Capturing
    if (closeToken == TokenId::SymRightParen && token.id == TokenId::SymVertical)
    {
        SWAG_CHECK(eatToken());
        while (token.id != TokenId::SymVertical)
        {
            SWAG_CHECK(checkIsIdentifier(token, Err(Err1077)));
            callParams->aliasNames.push_back(token);
            SWAG_CHECK(eatToken());
            if (token.id == TokenId::SymVertical)
                break;
            SWAG_CHECK(eatToken(TokenId::SymComma, "to declare another alias name"));
            SWAG_VERIFY(token.id != TokenId::SymVertical, error(token, Err(Err1199)));
        }

        SWAG_CHECK(eatToken());
    }

    while (token.id != closeToken)
    {
        while (true)
        {
            auto param         = Ast::newNode<AstFuncCallParam>(this, AstNodeKind::FuncCallParam, sourceFile, callParams);
            param->semanticFct = SemanticJob::resolveFuncCallParam;
            param->token       = token;
            AstNode* paramExpression;

            SWAG_CHECK(doExpression(param, EXPR_FLAG_PARAMETER | EXPR_FLAG_IN_CALL | EXPR_FLAG_NAMED_PARAM, &paramExpression));
            Ast::removeFromParent(paramExpression);

            // Name
            if (token.id == TokenId::SymColon)
            {
                if (paramExpression->kind != AstNodeKind::IdentifierRef || paramExpression->childs.size() != 1)
                    return context->report({paramExpression, Fmt(Err(Err1110), paramExpression->token.ctext())});
                param->allocateExtension(ExtensionKind::Misc);
                param->extMisc()->isNamed = paramExpression->childs.front();
                param->allocateExtension(ExtensionKind::Owner);
                param->extOwner()->nodesToFree.push_back(paramExpression);
                SWAG_CHECK(eatToken());
                SWAG_CHECK(doExpression(param, EXPR_FLAG_PARAMETER, &dummyResult));
            }
            else
            {
                Ast::addChildBack(param, paramExpression);
            }

            param->token.endLocation = token.startLocation;
            if (token.id == closeToken)
                break;

            auto tokenComma = token;
            if (callParams->specFlags & AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT)
                SWAG_CHECK(eatToken(TokenId::SymComma, "in 'struct' initialization parameters"));
            else
                SWAG_CHECK(eatToken(TokenId::SymComma, "in function call parameters"));

            // Accept ending comma in struct initialization
            if (closeToken == TokenId::SymRightCurly && token.id == closeToken)
                break;

            if (token.id == closeToken)
                return context->report({callParams, tokenComma, Err(Err1201)});
        }
    }

    if (callParams->specFlags & AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT)
        SWAG_CHECK(eatToken(closeToken, "to close struct initialization parameters"));
    else
        SWAG_CHECK(eatToken(closeToken, "to close function call parameters"));

    return true;
}

bool Parser::doFuncDeclParameter(AstNode* parent, bool acceptMissingType, bool* hasMissingType)
{
    // Attribute
    AstAttrUse* attrUse = nullptr;
    if (token.id == TokenId::SymAttrStart)
    {
        SWAG_CHECK(doAttrUse(nullptr, (AstNode**) &attrUse));
    }

    auto paramNode = Ast::newVarDecl(sourceFile, "", parent, this, AstNodeKind::FuncDeclParam);

    // Using variable
    if (token.id == TokenId::KwdUsing)
    {
        SWAG_CHECK(eatToken());
        paramNode->flags |= AST_DECL_USING;
    }

    // :QuestionAsParam
    // Unused parameter
    Vector<Token> unnamedTokens;
    if (token.id == TokenId::SymQuestion && acceptMissingType)
    {
        token.id   = TokenId::Identifier;
        token.text = Fmt("__%d", g_UniqueID.fetch_add(1));
        paramNode->addSpecFlags(AstVarDecl::SPECFLAG_UNNAMED);
        unnamedTokens.push_back(token);
    }

    if (token.id != TokenId::KwdConst)
        SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err1112), token.ctext())));
    paramNode->token.text = token.text;

    // 'self'
    if (token.id == TokenId::KwdConst || paramNode->token.text == g_LangSpec->name_self)
    {
        bool isConst = false;
        if (token.id == TokenId::KwdConst)
        {
            auto constToken = token;
            isConst         = true;
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id == TokenId::Identifier && token.text == g_LangSpec->name_self, error(constToken, Fmt(Err(Err1045), token.ctext())));
            paramNode->token.text = g_LangSpec->name_self;
        }

        SWAG_VERIFY(paramNode->ownerStructScope, error(token, Err(Err1027)));
        SWAG_CHECK(eatToken());

        // For an enum, 'self' is replaced with the type itself, not a pointer to the type like for a struct
        if (paramNode->ownerStructScope->kind == ScopeKind::Enum)
        {
            auto typeNode = Ast::newTypeExpression(sourceFile, paramNode);
            typeNode->typeFlags |= TYPEFLAG_IS_SELF;
            if (paramNode->flags & AST_DECL_USING)
                typeNode->typeFlags |= TYPEFLAG_HAS_USING;
            typeNode->identifier = Ast::newIdentifierRef(sourceFile, paramNode->ownerStructScope->name, typeNode, this);
            paramNode->type      = typeNode;
        }
        else
        {
            SWAG_VERIFY(paramNode->ownerStructScope->kind == ScopeKind::Struct, error(token, Err(Err1027)));
            auto typeNode = Ast::newTypeExpression(sourceFile, paramNode);
            typeNode->typeFlags |= isConst ? TYPEFLAG_IS_CONST : 0;
            typeNode->typeFlags |= TYPEFLAG_IS_SELF | TYPEFLAG_IS_PTR | TYPEFLAG_IS_SUB_TYPE;
            if (paramNode->flags & AST_DECL_USING)
                typeNode->typeFlags |= TYPEFLAG_HAS_USING;
            typeNode->identifier = Ast::newIdentifierRef(sourceFile, paramNode->ownerStructScope->name, typeNode, this);
            paramNode->type      = typeNode;
        }

        if (token.id == TokenId::SymEqual)
        {
            Diagnostic diag(paramNode, token, Err(Err1193));
            return context->report(diag);
        }

        if (token.id == TokenId::SymColon)
        {
            Diagnostic diag(paramNode, token, Err(Err1197));
            return context->report(diag);
        }
    }
    else
    {
        // Multiple declaration
        VectorNative<AstVarDecl*> otherVariables;
        SWAG_CHECK(eatToken());
        while (token.id == TokenId::SymComma)
        {
            SWAG_CHECK(eatToken());
            AstVarDecl* otherVarNode = Ast::newVarDecl(sourceFile, token.text, parent, this, AstNodeKind::FuncDeclParam);

            // :QuestionAsParam
            if (token.id == TokenId::SymQuestion && acceptMissingType)
            {
                token.id   = TokenId::Identifier;
                token.text = Fmt("__%d", g_UniqueID.fetch_add(1));
                otherVarNode->addSpecFlags(AstVarDecl::SPECFLAG_UNNAMED);
                unnamedTokens.push_back(token);
            }

            SWAG_VERIFY(token.id != TokenId::SymRightParen, error(token, Fmt(Err(Err1202), token.ctext())));
            SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err1112), token.ctext())));
            SWAG_CHECK(eatToken());
            otherVariables.push_back(otherVarNode);
        }

        bool hasType       = false;
        bool hasAssignment = false;

        // Type
        if (token.id == TokenId::SymColon)
        {
            if (unnamedTokens.size() == parent->childs.size())
            {
                Diagnostic diag{sourceFile, token, Err(Err1189)};
                diag.addRange(unnamedTokens.front(), Nte(Nte1097));
                for (size_t i = 1; i < unnamedTokens.size(); i++)
                    diag.addRange(unnamedTokens[i], "");
                return context->report(diag);
            }

            hasType = true;

            SWAG_CHECK(eatToken());

            // ...
            if (token.id == TokenId::SymDotDotDot)
            {
                auto newTypeExpression             = Ast::newTypeExpression(sourceFile, paramNode);
                paramNode->type                    = newTypeExpression;
                newTypeExpression->typeFromLiteral = g_TypeMgr->typeInfoVariadic;
                SWAG_CHECK(eatToken());
            }
            // cvarargs
            else if (token.id == TokenId::KwdCVarArgs)
            {
                auto newTypeExpression             = Ast::newTypeExpression(sourceFile, paramNode);
                paramNode->type                    = newTypeExpression;
                newTypeExpression->typeFromLiteral = g_TypeMgr->typeInfoCVariadic;
                SWAG_CHECK(eatToken());
            }
            else
            {
                AstNode* typeExpression;
                SWAG_CHECK(doTypeExpression(paramNode, EXPR_FLAG_NONE, &typeExpression));

                // type...
                if (token.id == TokenId::SymDotDotDot)
                {
                    Ast::removeFromParent(typeExpression);
                    auto newTypeExpression               = Ast::newTypeExpression(sourceFile, paramNode);
                    paramNode->type                      = newTypeExpression;
                    newTypeExpression->typeFromLiteral   = g_TypeMgr->typeInfoVariadic;
                    newTypeExpression->token.endLocation = token.endLocation;
                    SWAG_CHECK(eatToken());
                    Ast::addChildBack(paramNode->type, typeExpression);
                    newTypeExpression->token.startLocation = typeExpression->token.startLocation;
                }
                else
                {
                    paramNode->type = typeExpression;
                }
            }
        }

        // Assignment
        if (token.id == TokenId::SymEqual)
        {
            if (unnamedTokens.size() == parent->childs.size())
            {
                Diagnostic diag{sourceFile, token, Err(Err1190)};
                diag.addRange(unnamedTokens.front(), Nte(Nte1099));
                for (size_t i = 1; i < unnamedTokens.size(); i++)
                    diag.addRange(unnamedTokens[i], "");
                return context->report(diag);
            }

            paramNode->assignToken = token;
            hasAssignment          = true;

            SWAG_CHECK(eatToken());
            SWAG_CHECK(doAssignmentExpression(paramNode, &paramNode->assignment));

            // Used to automatically solve enums
            if (paramNode->assignment && paramNode->type)
            {
                paramNode->type->allocateExtension(ExtensionKind::Semantic);
                paramNode->type->extSemantic()->semanticAfterFct = SemanticJob::resolveVarDeclAfterType;
            }
        }

        if (!hasType && !hasAssignment)
        {
            if (!acceptMissingType)
            {
                prepareExpectTokenError();
                Diagnostic  diag{sourceFile, token, Fmt(Err(Err0648), token.ctext())};
                Diagnostic* note;
                if (otherVariables.empty())
                    note = Diagnostic::note(paramNode, Nte(Nte0077));
                else
                    note = Diagnostic::note(sourceFile, paramNode->token.startLocation, otherVariables.back()->token.endLocation, Nte(Nte1111));
                return context->report(diag, note);
            }
            else if (hasMissingType)
            {
                *hasMissingType = true;
            }
        }

        // Add attribute as the last child, to avoid messing around with the first FuncDeclParam node.
        if (attrUse)
        {
            paramNode->attrUse          = attrUse;
            paramNode->attrUse->content = paramNode;
            Ast::addChildBack(paramNode, paramNode->attrUse);
        }

        // Propagate types and assignment to multiple declarations
        for (auto one : otherVariables)
        {
            CloneContext cloneContext;
            cloneContext.parent = one;

            if (paramNode->type)
            {
                one->type = (AstTypeExpression*) paramNode->type->clone(cloneContext);
            }

            if (paramNode->assignment)
            {
                one->assignment = paramNode->assignment->clone(cloneContext);
            }

            if (paramNode->attrUse)
            {
                one->attrUse          = (AstAttrUse*) paramNode->attrUse->clone(cloneContext);
                one->attrUse->content = one;
            }
        }
    }

    return true;
}

bool Parser::doFuncDeclParameters(AstNode* parent, AstNode** result, bool acceptMissingType, bool* hasMissingType, bool isMethod, bool isConstMethod, bool isItfMethod)
{
    SWAG_VERIFY(token.id != TokenId::SymLeftCurly, error(token, Err(Err1091)));

    // To avoid calling 'format' in case we know this is fine, otherwise it will be called each time, even when ok
    auto startLoc = token.startLocation;
    if (token.id != TokenId::SymLeftParen && parent->kind == AstNodeKind::AttrDecl)
        SWAG_CHECK(eatToken(TokenId::SymLeftParen, Fmt("to declare the attribute parameters of '%s'", parent->token.ctext())));
    else if (token.id != TokenId::SymLeftParen)
        SWAG_CHECK(eatToken(TokenId::SymLeftParen, Fmt("to declare the function parameters of '%s'", parent->token.ctext())));
    else
        SWAG_CHECK(eatToken());

    if (token.id != TokenId::SymRightParen || isMethod || isConstMethod)
    {
        auto allParams = Ast::newFuncDeclParams(sourceFile, parent, this);
        *result        = allParams;

        // Add 'using self' as the first parameter in case of a method
        if (isMethod || isConstMethod)
        {
            auto paramNode = Ast::newVarDecl(sourceFile, "", allParams, this, AstNodeKind::FuncDeclParam);
            if (!isItfMethod)
                paramNode->flags |= AST_DECL_USING;
            paramNode->addSpecFlags(AstVarDecl::SPECFLAG_GENERATED_SELF);
            paramNode->token.text = g_LangSpec->name_self;
            auto typeNode         = Ast::newTypeExpression(sourceFile, paramNode);
            typeNode->typeFlags |= TYPEFLAG_IS_SELF | TYPEFLAG_IS_PTR | TYPEFLAG_IS_SUB_TYPE;
            if (!isItfMethod)
                typeNode->typeFlags |= TYPEFLAG_HAS_USING;
            if (isConstMethod)
                typeNode->typeFlags |= TYPEFLAG_IS_CONST;
            typeNode->identifier = Ast::newIdentifierRef(sourceFile, paramNode->ownerStructScope->name, typeNode, this);
            paramNode->type      = typeNode;
        }

        ScopedFlags sf(this, AST_IN_FUNC_DECL_PARAMS);
        bool        oneParamDone = false;
        while (token.id != TokenId::SymRightParen)
        {
            bool missingTypes = false;
            SWAG_CHECK(doFuncDeclParameter(allParams, acceptMissingType, &missingTypes));

            // Do not mix deduced types and specified types in lambdas
            if (acceptMissingType)
            {
                SWAG_ASSERT(hasMissingType);
                if (!missingTypes && *hasMissingType)
                    return error(allParams->childs.back(), Err(Err1170));
                if (oneParamDone && !(*hasMissingType) && missingTypes)
                    return error(allParams->childs.back(), Err(Err1090));
                *hasMissingType = *hasMissingType || missingTypes;
            }

            if (token.id == TokenId::SymRightParen)
                break;

            oneParamDone    = true;
            auto tokenComma = token;
            SWAG_CHECK(eatToken(TokenId::SymComma, "to define another parameter or ')' to end the list"));
            if (token.id == TokenId::SymRightParen)
                return context->report({allParams, tokenComma, Err(Err1202)});

            SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::KwdUsing || token.id == TokenId::SymAttrStart, error(token, Fmt(Err(Err1112), token.ctext())));
        }
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    return true;
}

bool Parser::doGenericDeclParameters(AstNode* parent, AstNode** result)
{
    auto allParams = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, parent);
    *result        = allParams;

    SWAG_ASSERT(token.id == TokenId::SymLeftParen);
    auto startLoc = token.startLocation;
    eatToken();
    SWAG_VERIFY(token.id != TokenId::SymRightParen, error(token, Err(Err1092)));

    while (token.id != TokenId::SymRightParen)
    {
        bool isConstant = false;
        bool isType     = false;
        if (token.id == TokenId::KwdConst)
        {
            isConstant = true;
            SWAG_CHECK(eatToken());
        }
        else if (token.id == TokenId::KwdVar)
        {
            isType = true;
            SWAG_CHECK(eatToken());
        }

        SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err1058), token.ctext())));
        auto oneParam = Ast::newVarDecl(sourceFile, token.text, allParams, this, AstNodeKind::FuncDeclParam);
        oneParam->flags |= AST_IS_GENERIC;
        SWAG_CHECK(eatToken());

        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id != TokenId::SymLeftCurly, error(token, Err(Err1134)));

            if (isType)
            {
                SWAG_CHECK(doExpression(oneParam, EXPR_FLAG_STOP_AFFECT, &oneParam->typeConstraint));
                oneParam->typeConstraint->flags |= AST_NO_SEMANTIC;
            }
            else
            {
                isConstant = true;
                SWAG_CHECK(doTypeExpression(oneParam, EXPR_FLAG_NONE, &oneParam->type));
            }
        }

        if (token.id == TokenId::SymEqual)
        {
            SWAG_CHECK(eatToken());
            if (isConstant)
            {
                SWAG_CHECK(doAssignmentExpression(oneParam, &oneParam->assignment));
            }
            else if (isType)
            {
                SWAG_CHECK(doTypeExpression(oneParam, EXPR_FLAG_NONE, &oneParam->assignment));
            }
            else if (!oneParam->type)
            {
                isType = true;
                PushErrCxtStep ec1(
                    context, nullptr, ErrCxtStepKind::Note, [oneParam]()
                    { return Fmt(Nte(Nte1043), oneParam->token.ctext()); },
                    true);
                SWAG_CHECK(doTypeExpression(oneParam, EXPR_FLAG_NONE, &oneParam->assignment));
            }
            else
            {
                SWAG_CHECK(doAssignmentExpression(oneParam, &oneParam->assignment));
            }
        }

        if (isType)
            oneParam->addSpecFlags(AstVarDecl::SPECFLAG_GENERIC_TYPE);
        else if (isConstant)
            oneParam->addSpecFlags(AstVarDecl::SPECFLAG_GENERIC_CONSTANT);

        if (isConstant && !oneParam->type && !oneParam->assignment)
            return error(token, Fmt(Err(Err1070), token.ctext()));

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id != TokenId::SymRightParen, error(token, Err(Err1202)));
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    return true;
}

bool Parser::doFuncDecl(AstNode* parent, AstNode** result, TokenId typeFuncId)
{
    auto funcNode         = Ast::newNode<AstFuncDecl>(this, AstNodeKind::FuncDecl, sourceFile, parent);
    *result               = funcNode;
    funcNode->semanticFct = SemanticJob::resolveFuncDecl;
    funcNode->allocateExtension(ExtensionKind::Semantic);
    funcNode->extSemantic()->semanticAfterFct = SemanticJob::sendCompilerMsgFuncDecl;

    if (tokenizer.comment.length())
    {
        funcNode->allocateExtension(ExtensionKind::Misc);
        funcNode->extMisc()->docComment = std::move(tokenizer.comment);
    }

    bool isMethod      = token.id == TokenId::KwdMethod;
    bool isConstMethod = false;
    if (isMethod || isConstMethod)
    {
        if (!funcNode->ownerStructScope)
            return error(token, Err(Err1039));

        if (funcNode->ownerStructScope->kind == ScopeKind::Enum)
            return error(token, Err(Err1038));

        if (funcNode->ownerStructScope->kind != ScopeKind::Struct)
            return error(token, Err(Err1039));
    }

    if (typeFuncId == TokenId::Invalid)
    {
        typeFuncId = token.id;
        SWAG_CHECK(eatToken());
    }

    bool isIntrinsic     = false;
    auto funcForCompiler = (g_TokenFlags[(int) typeFuncId] & TOKEN_COMPILER_FUNC);

    // Name
    if (funcForCompiler)
    {
        int id = g_UniqueID.fetch_add(1);
        switch (typeFuncId)
        {
        case TokenId::CompilerFuncTest:
            funcNode->token.text = "__test" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->attributeFlags |= ATTRIBUTE_TEST_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerFuncInit:
            funcNode->token.text = "__init" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->attributeFlags |= ATTRIBUTE_INIT_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerFuncDrop:
            funcNode->token.text = "__drop" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->attributeFlags |= ATTRIBUTE_DROP_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerFuncPreMain:
            funcNode->token.text = "__premain" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->attributeFlags |= ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerRun:
            funcNode->token.text = "__run" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->attributeFlags |= ATTRIBUTE_RUN_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerGeneratedRun:
            funcNode->token.text = "__run" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_RUN_GENERATED_FUNC | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerGeneratedRunExp:
            funcNode->token.text = "__run" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_RUN_GENERATED_EXP | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerFuncMain:
            funcNode->token.text = "__main" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->attributeFlags |= ATTRIBUTE_MAIN_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerFuncMessage:
            funcNode->token.text = "__compiler" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->attributeFlags |= ATTRIBUTE_COMPILER_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC;
            module->numCompilerFunctions++;
            break;
        case TokenId::CompilerAst:
            funcNode->token.text = "__ast" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_AST_FUNC | ATTRIBUTE_CONSTEXPR | ATTRIBUTE_COMPILER | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerValidIf:
            funcNode->token.text = "__validif" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_MATCH_VALIDIF_FUNC | ATTRIBUTE_CONSTEXPR | ATTRIBUTE_COMPILER | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerValidIfx:
            funcNode->token.text = "__validifx" + to_string(id);
            funcNode->tokenName  = funcNode->token;
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_MATCH_VALIDIFX_FUNC | ATTRIBUTE_CONSTEXPR | ATTRIBUTE_COMPILER | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        default:
            break;
        }
    }
    else
    {
        // Generic parameters
        if (token.id == TokenId::SymLeftParen)
            SWAG_CHECK(doGenericDeclParameters(funcNode, &funcNode->genericParameters));

        // const
        if (token.id == TokenId::KwdConst)
        {
            SWAG_VERIFY(isMethod, error(token, Err(Err1061)));
            isConstMethod = true;
            SWAG_CHECK(eatToken());
        }

        // Interface implementation function
        if (token.id == TokenId::KwdImpl)
        {
            funcNode->implLoc = token.startLocation;
            funcNode->addSpecFlags(AstFuncDecl::SPECFLAG_IMPL);
            SWAG_CHECK(eatToken());
        }

        isIntrinsic = token.text[0] == '@';
        if (isIntrinsic)
        {
            SWAG_VERIFY(sourceFile->isBootstrapFile || sourceFile->isRuntimeFile, error(token, Fmt(Err(Err1106), token.ctext())));
        }
        else
        {
            SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err1089), token.ctext())));
            funcNode->tokenName = token;
        }

        funcNode->inheritTokenName(token);
        SWAG_CHECK(checkIsValidUserName(funcNode, &token));

        SWAG_CHECK(eatToken());
        if (token.id == TokenId::SymComma)
        {
            // 'opVisit' variant after ','
            if (funcNode->token.text == g_LangSpec->name_opVisit)
            {
                SWAG_CHECK(eatToken());
                SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Err1115), token.ctext())));
                funcNode->token.text += token.text;
                funcNode->addSpecFlags(AstFuncDecl::SPECFLAG_VISIT_VARIANT);
                SWAG_CHECK(eatToken());
            }
        }
    }

    // Register function name
    auto typeInfo                = makeType<TypeInfoFuncAttr>();
    typeInfo->declNode           = funcNode;
    auto newScope                = Ast::newScope(funcNode, funcNode->token.text, ScopeKind::Function, currentScope);
    funcNode->typeInfo           = typeInfo;
    funcNode->scope              = newScope;
    funcNode->resolvedSymbolName = currentScope->symTable.registerSymbolName(context, funcNode, SymbolKind::Function);

    // Store specific symbols for fast retrieve
    if (funcNode->token.text == g_LangSpec->name_opAffect)
        currentScope->symbolOpAffect = funcNode->resolvedSymbolName;
    else if (funcNode->token.text == g_LangSpec->name_opAffectSuffix)
        currentScope->symbolOpAffectSuffix = funcNode->resolvedSymbolName;
    else if (funcNode->token.text == g_LangSpec->name_opCast)
        currentScope->symbolOpCast = funcNode->resolvedSymbolName;

    // Count number of methods to resolve
    if (currentScope->kind == ScopeKind::Struct && !funcForCompiler)
    {
        auto typeStruct       = CastTypeInfo<TypeInfoStruct>(currentScope->owner->typeInfo, TypeInfoKind::Struct);
        auto typeParam        = g_TypeMgr->makeParam();
        typeParam->name       = funcNode->token.text;
        typeParam->typeInfo   = funcNode->typeInfo;
        typeParam->declNode   = funcNode;
        typeParam->declNode   = funcNode;
        funcNode->methodParam = typeParam;

        ScopedLock lk(typeStruct->mutex);
        typeStruct->cptRemainingMethods++;
        typeStruct->methods.push_back(typeParam);
        if (funcNode->hasExtOwner() && funcNode->extOwner()->ownerCompilerIfBlock)
            funcNode->extOwner()->ownerCompilerIfBlock->methodsCount.push_back({funcNode, typeStruct, (int) typeStruct->methods.size() - 1});
        if (funcNode->isSpecialFunctionName())
            typeStruct->cptRemainingSpecialMethods++;
    }

    // Dispatch owners
    if (funcNode->genericParameters)
    {
        Ast::visit(funcNode->genericParameters, [&](AstNode* n)
                   {
                       n->ownerFct   = funcNode;
                       n->ownerScope = newScope; });
    }

    // Parameters
    if (!funcForCompiler)
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters, false, nullptr, isMethod, isConstMethod));
    }

    // #message has an expression has parameters
    else if (funcNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC)
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        auto      startLoc = token.startLocation;
        SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, Err(Err1175)));
        SWAG_VERIFY(token.id != TokenId::SymRightParen, error(funcNode, Err(Err1033)));
        SWAG_CHECK(doExpression(funcNode, EXPR_FLAG_NONE, &funcNode->parameters));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    }
    else if (token.id == TokenId::SymLeftParen)
    {
        Utf8 note;
        if (funcNode->attributeFlags & ATTRIBUTE_MAIN_FUNC)
            note = Nte(Nte0008);
        return error(token, Fmt(Err(Err0295), funcNode->getDisplayNameC()), note.c_str());
    }

    // Return type
    auto typeNode         = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclType, sourceFile, funcNode);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = SemanticJob::resolveFuncDeclType;
    if (!funcForCompiler)
    {
        if (token.id == TokenId::SymMinusGreat)
        {
            typeNode->addSpecFlags(AstFuncDecl::SPECFLAG_RETURN_DEFINED);
            Scoped    scoped(this, newScope);
            ScopedFct scopedFct(this, funcNode);
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id != TokenId::KwdRetVal, error(token, Err(Err1144)));
            AstNode* typeExpression;
            SWAG_CHECK(doTypeExpression(typeNode, EXPR_FLAG_NONE, &typeExpression));
        }

        if (token.id == TokenId::KwdThrow)
        {
            SWAG_CHECK(eatToken());
            funcNode->addSpecFlags(AstFuncDecl::SPECFLAG_THROW);
            funcNode->typeInfo->flags |= TYPEINFO_CAN_THROW;
        }
    }
    else if (typeFuncId == TokenId::CompilerAst)
    {
        typeNode->addSpecFlags(AstFuncDecl::SPECFLAG_RETURN_DEFINED);
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        auto      typeExpression        = Ast::newTypeExpression(sourceFile, typeNode, this);
        typeExpression->typeFromLiteral = g_TypeMgr->typeInfoString;
    }
    else if (typeFuncId == TokenId::CompilerValidIf || typeFuncId == TokenId::CompilerValidIfx)
    {
        typeNode->addSpecFlags(AstFuncDecl::SPECFLAG_RETURN_DEFINED);
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        auto      typeExpression        = Ast::newTypeExpression(sourceFile, typeNode, this);
        typeExpression->typeFromLiteral = g_TypeMgr->typeInfoBool;
    }

    funcNode->typeInfo->computeName();

    // '#validif' block
    if (token.id == TokenId::CompilerValidIf || token.id == TokenId::CompilerValidIfx)
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(doCompilerValidIf(funcNode, &funcNode->validif));
    }

    // If we have now a semicolon, then this is an empty function, like a forward decl in c++
    if (token.id == TokenId::SymSemiColon)
    {
        SWAG_VERIFY(!funcForCompiler, error(token, Fmt(Err(Err1054), funcNode->getDisplayNameC())));
        SWAG_CHECK(eatSemiCol("function declaration"));
        funcNode->addSpecFlags(AstFuncDecl::SPECFLAG_EMPTY_FCT);
        return true;
    }

    if (isIntrinsic)
        funcNode->flags |= AST_DEFINED_INTRINSIC;

    // Content of function
    {
        newScope = Ast::newScope(funcNode, funcNode->token.text, ScopeKind::FunctionBody, newScope);
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        AstNode*  resStmt = nullptr;

        // One single return expression
        if (token.id == TokenId::SymEqualGreater)
        {
            SWAG_CHECK(eatToken());

            if (funcNode->specFlags & AstFuncDecl::SPECFLAG_THROW)
            {
                auto node         = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Try, sourceFile, funcNode);
                node->semanticFct = SemanticJob::resolveTryBlock;
                node->addSpecFlags(AstTryCatchAssume::SPECFLAG_GENERATED | AstTryCatchAssume::SPECFLAG_BLOCK);
                funcNode->content = node;

                ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);
                auto                 returnNode = Ast::newNode<AstReturn>(this, AstNodeKind::Return, sourceFile, node);
                returnNode->semanticFct         = SemanticJob::resolveReturn;
                funcNode->addSpecFlags(AstFuncDecl::SPECFLAG_SHORT_LAMBDA);
                SWAG_CHECK(doExpression(returnNode, EXPR_FLAG_NONE, &dummyResult));
            }
            else
            {

                auto stmt               = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, funcNode);
                auto returnNode         = Ast::newNode<AstReturn>(this, AstNodeKind::Return, sourceFile, stmt);
                returnNode->semanticFct = SemanticJob::resolveReturn;
                funcNode->content       = returnNode;
                funcNode->addSpecFlags(AstFuncDecl::SPECFLAG_SHORT_LAMBDA);
                SWAG_CHECK(doExpression(returnNode, EXPR_FLAG_NONE, &dummyResult));
            }

            funcNode->addSpecFlags(AstFuncDecl::SPECFLAG_SHORT_FORM);
            funcNode->content->token.endLocation = token.startLocation;
            resStmt                              = funcNode->content;
        }

        // One single statement
        else if (token.id == TokenId::SymEqual)
        {
            SWAG_CHECK(eatToken());

            if (funcNode->specFlags & AstFuncDecl::SPECFLAG_THROW)
            {
                auto node         = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Try, sourceFile, funcNode);
                node->semanticFct = SemanticJob::resolveTryBlock;
                node->addSpecFlags(AstTryCatchAssume::SPECFLAG_GENERATED | AstTryCatchAssume::SPECFLAG_BLOCK);
                funcNode->content = node;

                auto stmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, node);

                ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);
                SWAG_CHECK(doEmbeddedInstruction(stmt, &dummyResult));
            }
            else
            {
                auto stmt         = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, funcNode);
                funcNode->content = stmt;

                SWAG_CHECK(doEmbeddedInstruction(stmt, &dummyResult));
            }

            funcNode->content->token.endLocation = token.startLocation;
            resStmt                              = funcNode->content;
            funcNode->addSpecFlags(AstFuncDecl::SPECFLAG_SHORT_FORM);
        }

        // Normal curly statement
        else
        {
            if (funcNode->specFlags & AstFuncDecl::SPECFLAG_THROW)
            {
                auto node         = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Try, sourceFile, funcNode);
                funcNode->content = node; // :AutomaticTryContent
                node->semanticFct = SemanticJob::resolveTryBlock;
                node->addSpecFlags(AstTryCatchAssume::SPECFLAG_GENERATED | AstTryCatchAssume::SPECFLAG_BLOCK);
                ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);
                SWAG_CHECK(doCurlyStatement(node, &resStmt));
                node->token.endLocation = resStmt->token.endLocation;
            }
            else
            {
                SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content));
                resStmt = funcNode->content;
            }
        }

        newScope->owner = resStmt;
        resStmt->allocateExtension(ExtensionKind::Semantic);
        resStmt->extSemantic()->semanticAfterFct = SemanticJob::resolveScopedStmtAfter;
        resStmt->setBcNotifAfter(ByteCodeGenJob::emitLeaveScope);
    }

    return true;
}

bool Parser::doReturn(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstReturn>(this, AstNodeKind::Return, sourceFile, parent);
    *result           = node;
    node->semanticFct = SemanticJob::resolveReturn;

    // Return value
    SWAG_CHECK(eatToken());
    if (token.flags & TOKENPARSE_LAST_EOL)
        return true;
    if (token.id != TokenId::SymSemiColon)
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));

    return true;
}

bool Parser::doLambdaFuncDecl(AstNode* parent, AstNode** result, bool acceptMissingType, bool* hasMissingType)
{
    auto funcNode         = Ast::newNode<AstFuncDecl>(this, AstNodeKind::FuncDecl, sourceFile, parent);
    *result               = funcNode;
    funcNode->semanticFct = SemanticJob::resolveFuncDecl;
    funcNode->flags |= AST_GENERATED;

    int id               = g_UniqueID.fetch_add(1);
    funcNode->token.text = "__lambda" + to_string(id);

    auto typeInfo      = makeType<TypeInfoFuncAttr>();
    typeInfo->declNode = funcNode;

    auto newScope      = Ast::newScope(funcNode, funcNode->token.text, ScopeKind::Function, currentScope);
    funcNode->typeInfo = typeInfo;
    funcNode->scope    = newScope;
    currentScope->symTable.registerSymbolName(context, funcNode, SymbolKind::Function);

    // Closure capture arguments
    if (token.id == TokenId::KwdClosure)
    {
        // captureParameters will be solved with capture block, that's why we do NOT put it as a child
        // of the function.
        auto capture      = Ast::newFuncCallParams(sourceFile, funcNode, this);
        capture->ownerFct = funcNode;
        Ast::removeFromParent(capture);
        capture->semanticFct        = SemanticJob::resolveCaptureFuncCallParams;
        funcNode->captureParameters = capture;

        SWAG_CHECK(eatToken());
        if (token.id == TokenId::SymVerticalVertical)
        {
            SWAG_CHECK(eatToken());
        }
        else
        {
            {
                PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, []()
                                  { return Nte(Nte0145); });
                SWAG_CHECK(eatToken(TokenId::SymVertical, "to start the 'closure' capture block"));
            }

            while (token.id != TokenId::SymVertical)
            {
                auto parentId = (AstNode*) capture;
                auto byRef    = false;
                if (token.id == TokenId::SymAmpersand)
                {
                    parentId              = Ast::newNode<AstMakePointer>(this, AstNodeKind::MakePointer, sourceFile, capture);
                    parentId->semanticFct = SemanticJob::resolveMakePointer;
                    eatToken();
                    byRef = true;
                }
                else if (token.id == TokenId::KwdRef)
                {
                    parentId              = Ast::newNode<AstMakePointer>(this, AstNodeKind::MakePointer, sourceFile, parentId);
                    parentId->semanticFct = SemanticJob::resolveMakePointer;
                    parentId->addSpecFlags(AstMakePointer::SPECFLAG_TOREF);
                    eatToken();
                    byRef = true;
                }

                AstNode* idRef = nullptr;
                SWAG_CHECK(doIdentifierRef(parentId, &idRef, IDENTIFIER_NO_PARAMS));

                if (byRef)
                    forceTakeAddress(idRef);

                if (token.id == TokenId::SymVertical)
                    break;

                SWAG_CHECK(eatToken(TokenId::SymComma, "in capture block"));
                SWAG_VERIFY(token.id != TokenId::SymVertical, error(token, Err(Err1057)));
            }

            capture->token.endLocation = token.endLocation;
            SWAG_CHECK(eatToken());
        }

        SWAG_VERIFY(token.id == TokenId::SymLeftParen, error(token, Fmt(Err(Err1203), token.ctext())));
        typeInfo->flags |= TYPEINFO_CLOSURE;
    }
    else
    {
        SWAG_CHECK(eatToken());
    }

    // Lambda parameters
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters, acceptMissingType, hasMissingType));
    }

    // :ClosureForceFirstParam
    // Closure first parameter is a void* pointer that will point to the context
    if (typeInfo->isClosure())
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);

        if (!funcNode->parameters)
            funcNode->parameters = Ast::newFuncDeclParams(sourceFile, funcNode, this);
        auto v = Ast::newVarDecl(sourceFile, "__captureCxt", funcNode->parameters, this, AstNodeKind::FuncDeclParam);
        v->flags |= AST_GENERATED;
        Ast::removeFromParent(v);
        Ast::addChildFront(funcNode->parameters, v);
        v->type           = Ast::newTypeExpression(sourceFile, v, this);
        v->type->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        v->type->flags |= AST_NO_SEMANTIC;
    }

    // Return type
    auto typeNode         = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclType, sourceFile, funcNode);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = SemanticJob::resolveFuncDeclType;
    if (token.id == TokenId::SymMinusGreat)
    {
        // Do not accept a specified return type if lambda parameters are deduced
        if (acceptMissingType && hasMissingType && *hasMissingType)
            return error(token, Err(Err1165));

        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(eatToken());
        AstNode* typeExpression;
        SWAG_CHECK(doTypeExpression(typeNode, EXPR_FLAG_NONE, &typeExpression));
        Ast::setForceConstType(typeExpression);
        typeNode->addSpecFlags(AstFuncDecl::SPECFLAG_RETURN_DEFINED);
    }

    // Body
    {
        newScope = Ast::newScope(funcNode, funcNode->token.text, ScopeKind::FunctionBody, newScope);
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);

        // One single return expression
        if (token.id == TokenId::SymEqualGreater)
        {
            SWAG_CHECK(eatToken());
            auto returnNode         = Ast::newNode<AstReturn>(this, AstNodeKind::Return, sourceFile, funcNode);
            returnNode->semanticFct = SemanticJob::resolveReturn;
            funcNode->content       = returnNode;
            funcNode->addSpecFlags(AstFuncDecl::SPECFLAG_SHORT_LAMBDA);
            SWAG_CHECK(doExpression(returnNode, EXPR_FLAG_NONE, &dummyResult));
        }

        // Normal curly statement
        else
        {
            SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content));
            funcNode->content->token = token;
        }

        funcNode->content->setBcNotifAfter(ByteCodeGenJob::emitLeaveScope);
        newScope->owner = funcNode->content;
    }

    return true;
}

bool Parser::doLambdaExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    bool acceptMissingType = false;
    bool deduceMissingType = false;

    // We accept missing types if lambda is in a function call, because we can deduce them from the called
    // function parameters
    if (exprFlags & EXPR_FLAG_IN_CALL)
        acceptMissingType = true;

    // We accept missing types if lambda is in an affectation, because we can deduce them from the
    // type on the left
    else if (parent->kind == AstNodeKind::AffectOp)
    {
        acceptMissingType = true;
        deduceMissingType = true;
    }

    // We accept missing types if lambda is in a variable declaration with a type, because we can deduce them from the
    // type
    else if (exprFlags & EXPR_FLAG_IN_VAR_DECL_WITH_TYPE)
    {
        acceptMissingType = true;
        deduceMissingType = true;
    }

    AstNode* lambda         = nullptr;
    bool     hasMissingType = false;

    {
        ScopedBreakable sb(this, nullptr);
        SWAG_CHECK(doLambdaFuncDecl(currentFct, &lambda, acceptMissingType, &hasMissingType));
    }

    SourceLocation start, end;
    lambda->computeLocation(start, end);
    lambda->token.startLocation = start;
    lambda->token.endLocation   = end;

    auto lambdaDecl = CastAst<AstFuncDecl>(lambda, AstNodeKind::FuncDecl);
    lambdaDecl->addSpecFlags(AstFuncDecl::SPECFLAG_IS_LAMBDA_EXPRESSION);
    if (!lambda->ownerFct && lambdaDecl->captureParameters)
        return error(lambdaDecl, Err(Err1153));

    // Lambda sub function will be resolved by the owner function
    if (lambda->ownerFct)
        registerSubDecl(lambda);
    // If the lambda is created at global scope, register it as a normal function
    else
        Ast::addChildBack(sourceFile->astRoot, lambda);

    // Retrieve the pointer of the function
    auto exprNode = Ast::newNode<AstMakePointer>(this, AstNodeKind::MakePointerLambda, sourceFile, parent);
    exprNode->inheritTokenLocation(lambda);
    exprNode->lambda      = lambdaDecl;
    exprNode->semanticFct = SemanticJob::resolveMakePointerLambda;

    if (lambdaDecl->captureParameters)
    {
        // To solve captured parameters
        auto cp = CastAst<AstFuncCallParams>(lambdaDecl->captureParameters, AstNodeKind::FuncCallParams);
        Ast::addChildBack(exprNode, cp);
        cp->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

        // We want the lambda to be evaluated only once the captured block has been typed
        // See resolveCaptureFuncCallParams
        lambdaDecl->flags |= AST_NO_SEMANTIC | AST_SPEC_SEMANTIC1;

        // Reference to the function
        AstNode* identifierRef = Ast::newIdentifierRef(sourceFile, lambda->token.text, exprNode, this);
        identifierRef->inheritTokenLocation(lambda);
        identifierRef->childs.back()->inheritTokenLocation(lambda);
        forceTakeAddress(identifierRef);

        // Create the capture block (a tuple)
        auto nameCaptureBlock = Fmt("__captureblock%d", g_UniqueID.fetch_add(1));
        auto block            = Ast::newVarDecl(sourceFile, nameCaptureBlock, exprNode, this);
        block->inheritTokenLocation(lambdaDecl->captureParameters);
        block->flags |= AST_GENERATED;
        auto exprList         = Ast::newNode<AstExpressionList>(this, AstNodeKind::ExpressionList, sourceFile, block);
        exprList->semanticFct = SemanticJob::resolveExpressionListTuple;
        exprList->addSpecFlags(AstExpressionList::SPECFLAG_FOR_TUPLE | AstExpressionList::SPECFLAG_FOR_CAPTURE);
        exprList->inheritTokenLocation(lambdaDecl->captureParameters);
        block->assignment = exprList;
        SemanticJob::setVarDeclResolve(block);

        for (auto c : lambdaDecl->captureParameters->childs)
        {
            Ast::clone(c, exprList);
        }

        // Reference to the captured block
        identifierRef = Ast::newIdentifierRef(sourceFile, nameCaptureBlock, exprNode, this);
        identifierRef->inheritTokenLocation(lambdaDecl->captureParameters);
        identifierRef->childs.back()->inheritTokenLocation(lambdaDecl->captureParameters);
        forceTakeAddress(identifierRef);
    }
    else
    {
        // Reference to the function
        AstNode* identifierRef = Ast::newIdentifierRef(sourceFile, lambda->token.text, exprNode, this);
        identifierRef->inheritTokenLocation(lambda);
        identifierRef->childs.back()->inheritTokenLocation(lambda);
        forceTakeAddress(identifierRef);
    }

    // :DeduceLambdaType
    if (deduceMissingType && acceptMissingType && hasMissingType)
    {
        exprNode->addSpecFlags(AstMakePointer::SPECFLAG_DEP_TYPE);
        lambdaDecl->makePointerLambda = exprNode;
        lambdaDecl->flags |= AST_NO_SEMANTIC | AST_SPEC_SEMANTIC2;
    }

    *result = exprNode;
    return true;
}