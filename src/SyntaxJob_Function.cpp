#include "pch.h"
#include "Ast.h"
#include "Scoped.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Report.h"
#include "LanguageSpec.h"

bool SyntaxJob::doGenericFuncCallParameters(AstNode* parent, AstNode** result)
{
    auto callParams = Ast::newFuncCallGenParams(sourceFile, parent, this);
    *result         = callParams;

    bool multi = false;
    if (token.id == TokenId::SymLeftParen)
    {
        multi = true;
        SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    }

    while (token.id != TokenId::SymRightParen)
    {
        auto param   = Ast::newFuncCallParam(sourceFile, callParams, this);
        param->token = token;
        switch (token.id)
        {
        case TokenId::Identifier:
        {
            SWAG_CHECK(doIdentifierRef(param, nullptr, IDENTIFIER_NO_FCT_PARAMS));
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
            SWAG_CHECK(doLiteral(param));
            break;

        case TokenId::CompilerLocation:
            SWAG_CHECK(doCompilerLocation(param));
            break;

        case TokenId::SymLeftSquare:
            SWAG_CHECK(doExpressionListArray(param));
            break;

        case TokenId::CompilerType:
        {
            SWAG_CHECK(eatToken(TokenId::CompilerType));
            AstNode* resNode;
            SWAG_CHECK(doTypeExpression(param, &resNode));
            resNode->flags |= AST_FORCE_TYPE;
            break;
        }

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

bool SyntaxJob::doFuncCallParameters(AstNode* parent, AstFuncCallParams** result, TokenId closeToken)
{
    auto callParams = Ast::newFuncCallParams(sourceFile, parent, this);
    *result         = callParams;

    // Capturing
    if (closeToken == TokenId::SymRightParen && token.id == TokenId::SymVertical)
    {
        SWAG_CHECK(eatToken());
        while (token.id != TokenId::SymVertical)
        {
            SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Err(Err0401)));
            callParams->aliasNames.push_back(token);
            SWAG_CHECK(eatToken());
            if (token.id == TokenId::SymVertical)
                break;
            SWAG_VERIFY(token.id == TokenId::SymComma, error(token, Err(Err0402)));
            SWAG_CHECK(eatToken());
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

            inFunCall = true;
            SWAG_CHECK(doExpression(param, EXPR_FLAG_NONE, &paramExpression));
            Ast::removeFromParent(paramExpression);
            inFunCall = false;

            // Name
            if (token.id == TokenId::SymColon)
            {
                if (paramExpression->kind != AstNodeKind::IdentifierRef || paramExpression->childs.size() != 1)
                    return Report::report({paramExpression, Fmt(Err(Err0403), token.ctext())});
                param->namedParamNode = paramExpression->childs.front();
                param->namedParam     = param->namedParamNode->token.text;
                SWAG_CHECK(eatToken());
                SWAG_CHECK(doExpression(param, EXPR_FLAG_NONE));
            }
            else
            {
                Ast::addChildBack(param, paramExpression);
            }

            if (token.id == closeToken)
                break;

            auto tokenComma = token;
            if (callParams->flags & AST_CALL_FOR_STRUCT)
                SWAG_CHECK(eatToken(TokenId::SymComma, "in struct initialization parameters"));
            else
                SWAG_CHECK(eatToken(TokenId::SymComma, "in function call parameters"));

            // Accept ending comma in struct initialization
            if (closeToken == TokenId::SymRightCurly && token.id == closeToken)
                break;

            if (token.id == closeToken)
                return Report::report({callParams, tokenComma, Err(Err0856)});
        }
    }

    if (callParams->flags & AST_CALL_FOR_STRUCT)
        SWAG_CHECK(eatToken(closeToken, "to close struct initialization parameters"));
    else
        SWAG_CHECK(eatToken(closeToken, "to close function call parameters"));

    return true;
}

bool SyntaxJob::doFuncDeclParameter(AstNode* parent, bool acceptMissingType, bool* hasMissingType)
{
    ScopedContextual sc(this, &contextualNoInline);
    auto             paramNode = Ast::newVarDecl(sourceFile, "", parent, this, AstNodeKind::FuncDeclParam);

    // Using variable
    if (token.id == TokenId::KwdUsing)
    {
        SWAG_CHECK(eatToken());
        paramNode->flags |= AST_DECL_USING;
    }

    SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::KwdConst, error(token, Fmt(Err(Err0410), token.ctext())));
    paramNode->token.text = token.text;

    // 'self'
    if (token.id == TokenId::KwdConst || paramNode->token.text == g_LangSpec->name_self)
    {
        bool isConst = false;
        if (token.id == TokenId::KwdConst)
        {
            isConst = true;
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id == TokenId::Identifier && token.text == g_LangSpec->name_self, error(token, Err(Err0405)));
            paramNode->token.text = g_LangSpec->name_self;
        }

        SWAG_CHECK(eatToken());
        SWAG_VERIFY(paramNode->ownerStructScope, error(token, Err(Err0406)));

        // For an enum, 'self' is replaced with the type itself, not a pointer to the type like for a struct
        if (paramNode->ownerStructScope->kind == ScopeKind::Enum)
        {
            auto typeNode = Ast::newTypeExpression(sourceFile, paramNode);
            typeNode->typeFlags |= TYPEFLAG_IS_SELF;
            if (paramNode->flags & AST_DECL_USING)
                typeNode->typeFlags |= TYPEFLAG_USING;
            typeNode->identifier = Ast::newIdentifierRef(sourceFile, paramNode->ownerStructScope->name, typeNode, this);
            paramNode->type      = typeNode;
        }
        else
        {
            SWAG_VERIFY(paramNode->ownerStructScope->kind == ScopeKind::Struct, error(token, Err(Err0406)));
            auto typeNode         = Ast::newTypeExpression(sourceFile, paramNode);
            typeNode->ptrCount    = 1;
            typeNode->ptrFlags[0] = isConst ? AstTypeExpression::PTR_CONST : 0;
            typeNode->typeFlags |= isConst ? TYPEFLAG_IS_CONST : 0;
            typeNode->typeFlags |= TYPEFLAG_IS_SELF;
            if (paramNode->flags & AST_DECL_USING)
                typeNode->typeFlags |= TYPEFLAG_USING;
            typeNode->identifier = Ast::newIdentifierRef(sourceFile, paramNode->ownerStructScope->name, typeNode, this);
            paramNode->type      = typeNode;
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
            SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Err0408), token.ctext())));

            AstVarDecl* otherVarNode = Ast::newVarDecl(sourceFile, token.text, parent, this, AstNodeKind::FuncDeclParam);
            SWAG_CHECK(eatToken());
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
                SWAG_CHECK(doTypeExpression(paramNode, &typeExpression));

                // type...
                if (token.id == TokenId::SymDotDotDot)
                {
                    Ast::removeFromParent(typeExpression);
                    auto newTypeExpression             = Ast::newTypeExpression(sourceFile, paramNode);
                    paramNode->type                    = newTypeExpression;
                    newTypeExpression->typeFromLiteral = g_TypeMgr->typeInfoVariadic;
                    SWAG_CHECK(eatToken());
                    Ast::addChildBack(paramNode->type, typeExpression);
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
            hasAssignment = true;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doAssignmentExpression(paramNode, &paramNode->assignment));

            // Used to automatically solve enums
            if (paramNode->assignment && paramNode->type)
            {
                paramNode->type->allocateExtension(ExtensionKind::Semantic);
                paramNode->type->extension->semantic->semanticAfterFct = SemanticJob::resolveVarDeclAfterType;
            }
        }

        if (!hasType && !hasAssignment)
        {
            if (!acceptMissingType)
                return error(token, Err(Err0409));
            else if (hasMissingType)
                *hasMissingType = true;
        }

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

bool SyntaxJob::doFuncDeclParameters(AstNode* parent, AstNode** result, bool acceptMissingType, bool* hasMissingType, bool isMethod, bool isConstMethod, bool isItfMethod)
{
    SWAG_VERIFY(token.id != TokenId::SymLeftCurly, error(token, Err(Err0883)));

    // To avoid calling 'format' in case we know this is fine, otherwise it will be called each time, even when ok
    if (token.id != TokenId::SymLeftParen && parent->kind == AstNodeKind::AttrDecl)
        SWAG_CHECK(eatToken(TokenId::SymLeftParen, Fmt("to declare the attribute parameters of `%s`", parent->token.ctext())));
    else if (token.id != TokenId::SymLeftParen)
        SWAG_CHECK(eatToken(TokenId::SymLeftParen, Fmt("to declare the function parameters of `%s`", parent->token.ctext())));
    else
        SWAG_CHECK(eatToken());

    if (token.id != TokenId::SymRightParen || isMethod || isConstMethod)
    {
        auto allParams = Ast::newFuncDeclParams(sourceFile, parent, this);
        if (result)
            *result = allParams;

        // Add 'using self' as the first parameter in case of a method
        if (isMethod || isConstMethod)
        {
            auto paramNode = Ast::newVarDecl(sourceFile, "", allParams, this, AstNodeKind::FuncDeclParam);
            if (!isItfMethod)
                paramNode->flags |= AST_DECL_USING;
            paramNode->specFlags |= AST_SPEC_DECLPARAM_GENERATED_SELF;
            paramNode->token.text = g_LangSpec->name_self;
            auto typeNode         = Ast::newTypeExpression(sourceFile, paramNode);
            typeNode->ptrCount    = 1;
            typeNode->typeFlags   = TYPEFLAG_IS_SELF;
            if (!isItfMethod)
                typeNode->typeFlags |= TYPEFLAG_USING;
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
                    return error(allParams->childs.back(), Err(Err0088), Hlp(Hlp0019));
                if (oneParamDone && !(*hasMissingType) && missingTypes)
                    return error(allParams->childs.back(), Err(Err0409), Hlp(Hlp0019));
                *hasMissingType = *hasMissingType || missingTypes;
            }

            if (token.id == TokenId::SymRightParen)
                break;

            oneParamDone    = true;
            auto tokenComma = token;
            SWAG_CHECK(eatToken(TokenId::SymComma));
            if (token.id == TokenId::SymRightParen)
                return Report::report({allParams, tokenComma, Err(Err0855)});

            SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::KwdUsing, error(token, Fmt(Err(Err0410), token.ctext())));
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doGenericDeclParameters(AstNode* parent, AstNode** result)
{
    ScopedContextual sc(this, &contextualNoInline);
    auto             allParams = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, parent);
    if (result)
        *result = allParams;

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_VERIFY(token.id != TokenId::SymRightParen, error(token, Err(Err0411)));

    while (token.id != TokenId::SymRightParen)
    {
        SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Err(Err0412)));
        auto oneParam = Ast::newVarDecl(sourceFile, token.text, allParams, this, AstNodeKind::FuncDeclParam);
        oneParam->flags |= AST_IS_GENERIC;
        SWAG_CHECK(eatToken());

        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id != TokenId::SymLeftCurly, error(token, Err(Err0555)));
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
    auto funcNode         = Ast::newNode<AstFuncDecl>(this, AstNodeKind::FuncDecl, sourceFile, parent, 4);
    funcNode->semanticFct = SemanticJob::resolveFuncDecl;
    funcNode->allocateExtension(ExtensionKind::Semantic);
    funcNode->extension->semantic->semanticAfterFct = SemanticJob::sendCompilerMsgFuncDecl;
    if (result)
        *result = funcNode;

    bool isMethod      = token.id == TokenId::KwdMethod;
    bool isConstMethod = token.id == TokenId::KwdConstMethod;
    if (isMethod || isConstMethod)
    {
        if (!funcNode->ownerStructScope)
            return error(token, Err(Err0407), nullptr, Hnt(Hnt0042));

        if (funcNode->ownerStructScope->kind == ScopeKind::Enum)
            return error(token, Err(Err0452), Hlp(Hlp0007), Hnt(Hnt0042));

        if (funcNode->ownerStructScope->kind != ScopeKind::Struct)
            return error(token, Err(Err0407), Hlp(Hlp0007), Hnt(Hnt0042));
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
            funcNode->attributeFlags |= ATTRIBUTE_TEST_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerFuncInit:
            funcNode->token.text = "__init" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_INIT_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerFuncDrop:
            funcNode->token.text = "__drop" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_DROP_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerFuncPreMain:
            funcNode->token.text = "__premain" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerRun:
            funcNode->token.text = "__run" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_RUN_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerGeneratedRun:
            funcNode->token.text = "__run" + to_string(id);
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_RUN_GENERATED_FUNC | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerGeneratedRunExp:
            funcNode->token.text = "__run" + to_string(id);
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_RUN_GENERATED_EXP | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerFuncMain:
            funcNode->token.text = "__main" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_MAIN_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerFuncCompiler:
            funcNode->token.text = "__compiler" + to_string(id);
            funcNode->attributeFlags |= ATTRIBUTE_COMPILER_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC;
            module->numCompilerFunctions++;
            break;
        case TokenId::CompilerAst:
            funcNode->token.text = "__ast" + to_string(id);
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_AST_FUNC | ATTRIBUTE_CONSTEXPR | ATTRIBUTE_COMPILER | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerSelectIf:
            funcNode->token.text = "__selectif" + to_string(id);
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_SELECTIF_FUNC | ATTRIBUTE_CONSTEXPR | ATTRIBUTE_COMPILER | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_SHARP_FUNC;
            break;
        case TokenId::CompilerCheckIf:
            funcNode->token.text = "__checkif" + to_string(id);
            funcNode->flags |= AST_GENERATED;
            funcNode->attributeFlags |= ATTRIBUTE_SELECTIF_FUNC | ATTRIBUTE_CONSTEXPR | ATTRIBUTE_COMPILER | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_SHARP_FUNC;
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
        {
            SWAG_VERIFY(sourceFile->isBootstrapFile || sourceFile->isRuntimeFile, error(token, Fmt(Err(Err0413), token.ctext()), Hlp(Hlp0008)));
        }
        else
        {
            relaxIdentifier(token);
            SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Err0414), token.ctext())));
            funcNode->tokenName = token;
        }

        funcNode->inheritTokenName(token);
        SWAG_CHECK(checkIsValidUserName(funcNode));
    }

    // Register function name
    auto typeInfo                = allocType<TypeInfoFuncAttr>();
    typeInfo->declNode           = funcNode;
    auto newScope                = Ast::newScope(funcNode, funcNode->token.text, ScopeKind::Function, currentScope);
    funcNode->typeInfo           = typeInfo;
    funcNode->scope              = newScope;
    funcNode->resolvedSymbolName = currentScope->symTable.registerSymbolName(&context, funcNode, SymbolKind::Function);

    // Count number of methods to resolve
    if (currentScope->kind == ScopeKind::Struct && !funcForCompiler)
    {
        auto typeStruct       = CastTypeInfo<TypeInfoStruct>(currentScope->owner->typeInfo, TypeInfoKind::Struct);
        auto typeParam        = g_TypeMgr->makeParam();
        typeParam->namedParam = funcNode->token.text;
        typeParam->typeInfo   = funcNode->typeInfo;
        typeParam->declNode   = funcNode;
        typeParam->declNode   = funcNode;
        funcNode->methodParam = typeParam;

        ScopedLock lk(typeStruct->mutex);
        typeStruct->cptRemainingMethods++;
        typeStruct->methods.push_back(typeParam);
        if (funcNode->ownerCompilerIfBlock)
            funcNode->ownerCompilerIfBlock->methodsCount.push_back({funcNode, typeStruct, (int) typeStruct->methods.size() - 1});
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
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters, false, nullptr, isMethod, isConstMethod));
    }

    // #message has an expression has parameters
    else if (funcNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC)
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(eatToken(TokenId::SymLeftParen));
        SWAG_VERIFY(token.id != TokenId::SymRightParen, error(funcNode, Err(Err0750)));
        SWAG_CHECK(doExpression(funcNode, EXPR_FLAG_NONE, &funcNode->parameters));
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
            typeNode->specFlags |= AST_SPEC_FUNCTYPE_RETURN_DEFINED;
            Scoped    scoped(this, newScope);
            ScopedFct scopedFct(this, funcNode);
            SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
            SWAG_VERIFY(token.id != TokenId::KwdRetVal, error(token, Err(Err0560)));
            AstNode* typeExpression;
            SWAG_CHECK(doTypeExpression(typeNode, &typeExpression));
        }

        if (token.id == TokenId::KwdThrow)
        {
            SWAG_CHECK(eatToken(TokenId::KwdThrow));
            funcNode->specFlags |= AST_SPEC_FUNCDECL_THROW;
            funcNode->typeInfo->flags |= TYPEINFO_CAN_THROW;
        }
    }
    else if (typeFuncId == TokenId::CompilerAst)
    {
        typeNode->specFlags |= AST_SPEC_FUNCTYPE_RETURN_DEFINED;
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        auto      typeExpression        = Ast::newTypeExpression(sourceFile, typeNode, this);
        typeExpression->typeFromLiteral = g_TypeMgr->typeInfoString;
    }
    else if (typeFuncId == TokenId::CompilerSelectIf || typeFuncId == TokenId::CompilerCheckIf)
    {
        typeNode->specFlags |= AST_SPEC_FUNCTYPE_RETURN_DEFINED;
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        auto      typeExpression        = Ast::newTypeExpression(sourceFile, typeNode, this);
        typeExpression->typeFromLiteral = g_TypeMgr->typeInfoBool;
    }

    funcNode->typeInfo->computeName();

    // '#selectif' block
    if (token.id == TokenId::CompilerSelectIf || token.id == TokenId::CompilerCheckIf)
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(doCompilerSelectIf(funcNode, &funcNode->selectIf));
    }

    // If we have now a semi colon, then this is an empty function, like a forward decl in c++
    if (token.id == TokenId::SymSemiColon)
    {
        SWAG_VERIFY(!funcForCompiler, error(token, Fmt(Err(Err0416), funcNode->getDisplayNameC())));
        SWAG_CHECK(eatSemiCol("function declaration"));
        funcNode->flags |= AST_EMPTY_FCT;
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

            if (funcNode->specFlags & AST_SPEC_FUNCDECL_THROW)
            {
                auto node = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Try, sourceFile, funcNode);
                node->specFlags |= AST_SPEC_TCA_GENERATED | AST_SPEC_TCA_BLOCK;
                node->semanticFct = SemanticJob::resolveTryBlock;

                ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);
                auto                 returnNode = Ast::newNode<AstReturn>(this, AstNodeKind::Return, sourceFile, node); // stmt);
                returnNode->semanticFct         = SemanticJob::resolveReturn;
                funcNode->content               = returnNode;
                funcNode->flags |= AST_SHORT_LAMBDA;
                SWAG_CHECK(doExpression(returnNode, EXPR_FLAG_NONE));
            }
            else
            {

                auto stmt               = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, funcNode);
                auto returnNode         = Ast::newNode<AstReturn>(this, AstNodeKind::Return, sourceFile, stmt);
                returnNode->semanticFct = SemanticJob::resolveReturn;
                funcNode->content       = returnNode;
                funcNode->flags |= AST_SHORT_LAMBDA;
                SWAG_CHECK(doExpression(returnNode, EXPR_FLAG_NONE));
            }

            funcNode->content->token.endLocation = token.startLocation;
            resStmt                              = funcNode->content;
        }

        // One single statement
        else if (token.id == TokenId::SymEqual)
        {
            SWAG_CHECK(eatToken());

            auto stmt         = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, funcNode);
            funcNode->content = stmt;

            if (funcNode->specFlags & AST_SPEC_FUNCDECL_THROW)
            {
                auto node = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Try, sourceFile, stmt);
                node->specFlags |= AST_SPEC_TCA_GENERATED | AST_SPEC_TCA_BLOCK;
                node->semanticFct = SemanticJob::resolveTryBlock;

                ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);
                SWAG_CHECK(doEmbeddedInstruction(stmt));
            }
            else
            {
                SWAG_CHECK(doEmbeddedInstruction(stmt));
            }

            funcNode->content->token.endLocation = token.startLocation;
            resStmt                              = funcNode->content;
        }

        // Normal curly statement
        else
        {
            if (funcNode->specFlags & AST_SPEC_FUNCDECL_THROW)
            {
                auto node = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Try, sourceFile, funcNode);
                node->specFlags |= AST_SPEC_TCA_GENERATED | AST_SPEC_TCA_BLOCK;
                funcNode->content = node; // :AutomaticTryContent
                node->semanticFct = SemanticJob::resolveTryBlock;
                ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);
                SWAG_CHECK(doCurlyStatement(node, &resStmt));
            }
            else
            {
                SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content));
                resStmt = funcNode->content;
            }
        }

        newScope->owner = resStmt;
        resStmt->allocateExtension(ExtensionKind::ByteCode);
        resStmt->extension->bytecode->byteCodeAfterFct = &ByteCodeGenJob::emitLeaveScope;
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
    SWAG_CHECK(eatToken());
    if (token.lastTokenIsEOL)
        return true;
    if (token.id != TokenId::SymSemiColon)
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));

    return true;
}

bool SyntaxJob::doLambdaFuncDecl(AstNode* parent, AstNode** result, bool acceptMissingType, bool* hasMissingType)
{
    auto funcNode         = Ast::newNode<AstFuncDecl>(this, AstNodeKind::FuncDecl, sourceFile, parent, 4);
    funcNode->semanticFct = SemanticJob::resolveFuncDecl;
    funcNode->flags |= AST_GENERATED;
    if (result)
        *result = funcNode;
    int id               = g_UniqueID.fetch_add(1);
    funcNode->token.text = "__lambda" + to_string(id);

    auto typeInfo      = allocType<TypeInfoFuncAttr>();
    typeInfo->declNode = funcNode;

    auto newScope      = Ast::newScope(funcNode, funcNode->token.text, ScopeKind::Function, currentScope);
    funcNode->typeInfo = typeInfo;
    funcNode->scope    = newScope;
    currentScope->symTable.registerSymbolName(&context, funcNode, SymbolKind::Function);

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
            SWAG_CHECK(eatToken(TokenId::SymVertical, "to start capture block"));
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
                    parentId->specFlags |= AST_SPEC_MAKEPOINTER_TOREF;
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
                SWAG_VERIFY(token.id != TokenId::SymVertical, error(token, Err(Err0120)));
            }

            capture->token.endLocation = token.endLocation;
            SWAG_CHECK(eatToken(TokenId::SymVertical));
        }

        SWAG_VERIFY(token.id == TokenId::SymLeftParen, error(token, Err(Err0456)));
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
        v->type->typeInfo = g_TypeMgr->typeInfoPointers[(int) NativeTypeKind::Void];
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
            return error(token, Err(Err0458), Hlp(Hlp0018), Hnt(Hnt0026));

        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
        AstNode* typeExpression;
        SWAG_CHECK(doTypeExpression(typeNode, &typeExpression));
        Ast::setForceConstType(typeExpression);
        typeNode->specFlags |= AST_SPEC_FUNCTYPE_RETURN_DEFINED;
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
            funcNode->flags |= AST_SHORT_LAMBDA;
            SWAG_CHECK(doExpression(returnNode, EXPR_FLAG_NONE));
        }

        // Normal curly statement
        else
        {
            SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content));
            funcNode->content->token = token;
        }

        funcNode->content->allocateExtension(ExtensionKind::ByteCode);
        funcNode->content->extension->bytecode->byteCodeAfterFct = &ByteCodeGenJob::emitLeaveScope;
        newScope->owner                                          = funcNode->content;
    }

    return true;
}

bool SyntaxJob::doLambdaExpression(AstNode* parent, AstNode** result)
{
    // We accept missing types if lambda is in a function call
    bool acceptMissingType = inFunCall;
    // We accept missing types if lambda is in an affectation
    if (parent->kind == AstNodeKind::AffectOp)
        acceptMissingType = true;

    AstNode* lambda         = nullptr;
    bool     hasMissingType = false;

    {
        ScopedBreakable sb(this, nullptr);
        SWAG_CHECK(doLambdaFuncDecl(currentFct, &lambda, acceptMissingType, &hasMissingType));
        lambda->flags |= AST_IS_LAMBDA_EXPRESSION;
        lambda->computeEndLocation();
    }

    auto lambdaDecl = CastAst<AstFuncDecl>(lambda, AstNodeKind::FuncDecl);
    if (!lambda->ownerFct && lambdaDecl->captureParameters)
        return error(lambdaDecl, Err(Err0179), Hlp(Hlp0017));

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
        exprList->specFlags |= AST_SPEC_EXPRLIST_FOR_TUPLE | AST_SPEC_EXPRLIST_FOR_CAPTURE;
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
    if (exprNode->parent->kind == AstNodeKind::AffectOp && acceptMissingType && hasMissingType)
    {
        auto op             = CastAst<AstOp>(exprNode->parent, AstNodeKind::AffectOp);
        op->dependentLambda = lambda;

        auto lambdaFunc               = CastAst<AstFuncDecl>(lambda, AstNodeKind::FuncDecl);
        lambdaFunc->makePointerLambda = exprNode;
        lambdaFunc->flags |= AST_NO_SEMANTIC | AST_SPEC_SEMANTIC2;
    }

    if (result)
        *result = exprNode;
    return true;
}