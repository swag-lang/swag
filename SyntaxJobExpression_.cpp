#include "pch.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"
#include "TypeManager.h"
#include "Scoped.h"

bool SyntaxJob::doLiteral(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Literal, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveLiteral;
    node->token       = move(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doArrayPointerIndex(AstNode** exprNode)
{
    SWAG_CHECK(eatToken(TokenId::SymLeftSquare));
    while (true)
    {
        auto arrayNode         = Ast::newNode<AstPointerDeRef>(this, AstNodeKind::ArrayPointerIndex, sourceFile);
        arrayNode->token       = token;
        arrayNode->semanticFct = SemanticJob::resolveArrayPointerIndex;

        Ast::addChildBack(arrayNode, *exprNode);
        arrayNode->array = *exprNode;
        SWAG_CHECK(doExpression(arrayNode, &arrayNode->access));
        *exprNode = arrayNode;
        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    SWAG_VERIFY(token.id != TokenId::SymLeftSquare, syntaxError(token, "invalid token '['"));

    return true;
}

bool SyntaxJob::doIntrinsicProp(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstProperty>(this, AstNodeKind::IntrinsicProp, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveIntrinsicProperty;
    node->inheritTokenName(token);
    node->prop = g_LangSpec.properties[node->name];
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doTopExpression(node, &node->expression));
    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doSinglePrimaryExpression(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::CompilerFunction:
    case TokenId::CompilerCallerLine:
    case TokenId::CompilerCallerFile:
    case TokenId::CompilerCallerFunction:
    case TokenId::CompilerConfiguration:
    case TokenId::CompilerPlatform:
    {
        auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::CompilerSpecialFunction, sourceFile, parent);
        if (result)
            *result = exprNode;
        exprNode->inheritTokenLocation(token);
        SWAG_CHECK(eatToken());
        exprNode->semanticFct = SemanticJob::resolveCompilerSpecialFunction;
        break;
    }

    case TokenId::SymLeftParen:
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doExpression(parent, result));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
        break;

    case TokenId::SymLeftCurly:
        SWAG_CHECK(doExpressionListCurly(parent, result));
        break;
    case TokenId::SymLeftSquare:
        SWAG_CHECK(doExpressionListArray(parent, result));
        break;

    case TokenId::LiteralNumber:
    case TokenId::LiteralCharacter:
    case TokenId::LiteralString:
        SWAG_CHECK(doLiteral(parent, result));
        break;

    case TokenId::KwdMove:
    {
        AstNode* id;
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doIdentifierRef(parent, &id));
        if (result)
            *result = id;
        id->flags |= AST_FORCE_MOVE;
    }
    break;

    case TokenId::SymBackTick:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::Identifier:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::IntrinsicIndex:
        SWAG_CHECK(doIndex(parent, result));
        break;

    case TokenId::Intrinsic:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::KwdCode:
    case TokenId::NativeType:
    case TokenId::SymAsterisk:
        SWAG_CHECK(doTypeExpression(parent, result));
        break;

    case TokenId::KwdFunc:
    {
        AstNode* lambda;
        SWAG_CHECK(doLambdaFuncDecl(sourceFile->astRoot, &lambda));
        auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::MakePointer, sourceFile, parent);
        exprNode->inheritTokenLocation(lambda->token);
        exprNode->semanticFct  = SemanticJob::resolveMakePointer;
        AstNode* identifierRef = Ast::newIdentifierRef(sourceFile, lambda->name, exprNode, this);
        identifierRef->inheritTokenLocation(lambda->token);
        forceTakeAddress(identifierRef);
        if (result)
            *result = exprNode;
        break;
    }

    default:
        return syntaxError(token, format("invalid token '%s'", token.text.c_str()));
    }

    return true;
}

bool SyntaxJob::doPrimaryExpression(AstNode* parent, AstNode** result)
{
    AstNode* exprNode;

    // Take pointer
    if (token.id == TokenId::SymAmpersand)
    {
        exprNode              = Ast::newNode<AstNode>(this, AstNodeKind::MakePointer, sourceFile);
        exprNode->semanticFct = SemanticJob::resolveMakePointer;
        SWAG_CHECK(eatToken());

        AstNode* identifierRef;
        SWAG_CHECK(doIdentifierRef(nullptr, &identifierRef));
        forceTakeAddress(identifierRef);

        if (token.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doArrayPointerIndex(&identifierRef));

        Ast::addChildBack(exprNode, identifierRef);
        identifierRef->flags |= AST_TAKE_ADDRESS;
    }

    // Dereference pointer
    else if (token.id == TokenId::KwdDeRef)
    {
        SWAG_CHECK(eatToken());

        auto arrayNode         = Ast::newNode<AstPointerDeRef>(this, AstNodeKind::ArrayPointerIndex, sourceFile);
        arrayNode->semanticFct = SemanticJob::resolveArrayPointerIndex;
        SWAG_CHECK(doSinglePrimaryExpression(arrayNode, &arrayNode->array));

        auto literal                   = Ast::newNode<AstNode>(this, AstNodeKind::Literal, sourceFile, arrayNode);
        literal->computedValue.reg.u64 = 0;
        literal->token.literalType     = g_TypeMgr.typeInfoS32;
        literal->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
        literal->semanticFct = SemanticJob::resolveLiteral;
        arrayNode->access    = literal;
        exprNode             = arrayNode;
    }
    else
    {
        SWAG_CHECK(doSinglePrimaryExpression(nullptr, &exprNode));
    }

    if (parent)
        Ast::addChildBack(parent, exprNode);
    if (result)
        *result = exprNode;
    return true;
}

bool SyntaxJob::doUnaryExpression(AstNode* parent, AstNode** result)
{
    // Cast
    if (token.id == TokenId::KwdCast)
    {
        SWAG_CHECK(doCast(parent, result));
        return true;
    }

    // Cast
    if (token.id == TokenId::KwdAutoCast)
    {
        SWAG_CHECK(doAutoCast(parent, result));
        return true;
    }

    if (token.id == TokenId::SymMinus || token.id == TokenId::SymExclam || token.id == TokenId::SymTilde)
    {
        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::SingleOp, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveUnaryOp;
        node->token       = move(token);
        if (result)
            *result = node;
        SWAG_CHECK(tokenizer.getToken(token));
        return doPrimaryExpression(node);
    }

    return doPrimaryExpression(parent, result);
}

bool SyntaxJob::doFactorExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doUnaryExpression(nullptr, &leftNode));

    bool isBinary = false;
    if ((token.id == TokenId::SymPlus) ||
        (token.id == TokenId::SymMinus) ||
        (token.id == TokenId::SymAsterisk) ||
        (token.id == TokenId::SymSlash) ||
        (token.id == TokenId::SymVertical) ||
        (token.id == TokenId::SymAmpersand) ||
        (token.id == TokenId::SymGreaterGreater) ||
        (token.id == TokenId::SymLowerLower) ||
        (token.id == TokenId::SymPercent) ||
        (token.id == TokenId::SymTilde) ||
        (token.id == TokenId::SymCircumflex))
    {
        if (parent && parent->kind == AstNodeKind::FactorOp)
        {
            if (parent->token.id != token.id)
                return syntaxError(token, "operator order ambiguity, please add parenthesis");

            // Associative operations
            if (token.id != TokenId::SymPlus &&
                token.id != TokenId::SymAsterisk &&
                token.id != TokenId::SymVertical &&
                token.id != TokenId::SymCircumflex &&
                token.id != TokenId::SymTilde)
                return syntaxError(token, "operator order ambiguity, please add parenthesis");
        }

        auto binaryNode = Ast::newNode<AstNode>(this, AstNodeKind::FactorOp, sourceFile, parent);
        if (token.id == TokenId::SymGreaterGreater || token.id == TokenId::SymLowerLower)
            binaryNode->semanticFct = SemanticJob::resolveShiftExpression;
        else
            binaryNode->semanticFct = SemanticJob::resolveFactorExpression;
        binaryNode->token = move(token);

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFactorExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChildBack(parent, leftNode);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doCompareExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doFactorExpression(nullptr, &leftNode));

    bool isBinary = false;
    if ((token.id == TokenId::SymEqualEqual) ||
        (token.id == TokenId::SymExclamEqual) ||
        (token.id == TokenId::SymLowerEqual) ||
        (token.id == TokenId::SymGreaterEqual) ||
        (token.id == TokenId::SymLower) ||
        (token.id == TokenId::SymGreater))
    {
        auto binaryNode         = Ast::newNode<AstNode>(this, AstNodeKind::BinaryOp, sourceFile, parent);
        binaryNode->semanticFct = SemanticJob::resolveCompareExpression;
        binaryNode->token       = move(token);

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFactorExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }
    else if (token.id == TokenId::KwdIs)
    {
        auto binaryNode         = Ast::newNode<AstNode>(this, AstNodeKind::BinaryOp, sourceFile, parent);
        binaryNode->semanticFct = SemanticJob::resolveIsExpression;
        binaryNode->token       = move(token);

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doTypeExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }
    else if (token.id == TokenId::SymEqual)
    {
        return syntaxError(token, "invalid token '=', did you mean '==' ?");
    }

    if (!isBinary)
        Ast::addChildBack(parent, leftNode);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doTopExpression(AstNode* parent, AstNode** result)
{
    // Is this a type ?
    if (token.id == TokenId::KwdConst || token.id == TokenId::SymLeftSquare || token.id == TokenId::SymAsterisk)
    {
        SWAG_CHECK(doTypeExpression(parent, result));
        return true;
    }

    SWAG_CHECK(doExpression(parent, result));
    return true;
}

bool SyntaxJob::doBoolExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doCompareExpression(nullptr, &leftNode));

    bool isBinary = false;
    if ((token.id == TokenId::SymVerticalVertical) || (token.id == TokenId::SymAmpersandAmpersand))
    {
        auto binaryNode         = Ast::newNode<AstNode>(this, AstNodeKind::BinaryOp, sourceFile, parent);
        binaryNode->semanticFct = SemanticJob::resolveBoolExpression;
        binaryNode->token       = move(token);

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doBoolExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChildBack(parent, leftNode);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doExpression(AstNode* parent, AstNode** result)
{
    AstNode* boolExpression;
    switch (token.id)
    {
    case TokenId::CompilerRun:
    {
        SWAG_CHECK(eatToken());
        boolExpression              = Ast::newNode<AstNode>(nullptr, AstNodeKind::CompilerRun, sourceFile, parent);
        boolExpression->semanticFct = SemanticJob::resolveCompilerRun;
        SWAG_CHECK(doBoolExpression(boolExpression));
        break;
    }
    case TokenId::CompilerMixin:
    {
        SWAG_CHECK(eatToken());
        boolExpression              = Ast::newNode<AstCompilerMixin>(nullptr, AstNodeKind::CompilerMixin, sourceFile, parent);
        boolExpression->semanticFct = SemanticJob::resolveCompilerMixin;
        SWAG_CHECK(doExpression(boolExpression));
        break;
    }
    case TokenId::CompilerCode:
    {
        SWAG_CHECK(eatToken());
        AstNode* block;
        boolExpression = Ast::newNode<AstNode>(nullptr, AstNodeKind::CompilerCode, sourceFile, parent);
        if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doEmbeddedStatement(boolExpression, &block));
        else
            SWAG_CHECK(doBoolExpression(boolExpression, &block));
        auto typeCode     = g_Allocator.alloc<TypeInfoCode>();
        typeCode->content = boolExpression->childs.front();
        typeCode->content->flags |= AST_NO_SEMANTIC;
        boolExpression->typeInfo = typeCode;
        boolExpression->flags |= AST_NO_BYTECODE;
        break;
    }

    default:
        SWAG_CHECK(doBoolExpression(nullptr, &boolExpression));
        break;
    }

    if (token.id == TokenId::SymQuestion)
    {
        SWAG_CHECK(eatToken());
        auto triNode         = Ast::newNode<AstNode>(this, AstNodeKind::QuestionExpression, sourceFile, parent);
        triNode->semanticFct = SemanticJob::resolveTrinaryOp;
        if (result)
            *result = triNode;
        Ast::addChildBack(triNode, boolExpression);

        SWAG_CHECK(doExpression(triNode));
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doExpression(triNode));
    }
    else
    {
        Ast::addChildBack(parent, boolExpression);
        if (result)
            *result = boolExpression;
    }

    return true;
}

bool SyntaxJob::doAssignmentExpression(AstNode* parent, AstNode** result)
{
    return doExpression(parent, result);
}

bool SyntaxJob::doExpressionListCurly(AstNode* parent, AstNode** result)
{
    auto initNode         = Ast::newNode<AstExpressionList>(this, AstNodeKind::ExpressionList, sourceFile, parent);
    initNode->semanticFct = SemanticJob::resolveExpressionListCurly;
    initNode->listKind    = TypeInfoListKind::Curly;
    SWAG_CHECK(tokenizer.getToken(token));

    if (token.id == TokenId::SymRightCurly)
        return syntaxError(token, format("initializer list is empty"));
    if (result)
        *result = initNode;

    while (token.id != TokenId::SymRightCurly)
    {
        if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doExpressionListCurly(initNode));
        else
        {
            AstNode* paramExpression;
            SWAG_CHECK(doExpression(nullptr, &paramExpression));

            // Name
            if (token.id == TokenId::SymColon)
            {
                if (paramExpression->kind != AstNodeKind::IdentifierRef || paramExpression->childs.size() != 1)
                    return sourceFile->report({paramExpression, format("invalid named value '%s'", token.text.c_str())});
                auto name = paramExpression->childs.front()->name;
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymLeftCurly)
                    SWAG_CHECK(doExpressionListCurly(initNode, &paramExpression));
                else
                    SWAG_CHECK(doExpression(initNode, &paramExpression));
                paramExpression->name = name;
            }
            else
            {
                Ast::addChildBack(initNode, paramExpression);
            }
        }

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    initNode->childs.back()->token.endLocation = token.endLocation;
    SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    return true;
}

bool SyntaxJob::doExpressionListArray(AstNode* parent, AstNode** result)
{
    auto initNode         = Ast::newNode<AstExpressionList>(this, AstNodeKind::ExpressionList, sourceFile, parent);
    initNode->semanticFct = SemanticJob::resolveExpressionListArray;
    initNode->listKind    = TypeInfoListKind::Bracket;
    SWAG_CHECK(tokenizer.getToken(token));

    if (token.id == TokenId::SymRightSquare)
        return syntaxError(token, format("initializer list is empty"));
    if (result)
        *result = initNode;

    while (token.id != TokenId::SymRightSquare)
    {
        if (token.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doExpressionListArray(initNode));
        else
            SWAG_CHECK(doExpression(initNode));

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    initNode->childs.back()->token.endLocation = token.endLocation;
    SWAG_CHECK(eatToken(TokenId::SymRightSquare));

    return true;
}

bool SyntaxJob::doInitializationExpression(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymQuestion)
    {
        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::ExplicitNoInit, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveExplicitNoInit;
        if (parent)
            parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
        if (result)
            *result = node;
        SWAG_CHECK(eatToken(TokenId::SymQuestion));
        return true;
    }

    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doExpressionListCurly(parent, result));
        return true;
    }

    return doExpression(parent, result);
}

void SyntaxJob::forceTakeAddress(AstNode* node)
{
    node->flags |= AST_TAKE_ADDRESS;
    switch (node->kind)
    {
    case AstNodeKind::IdentifierRef:
        forceTakeAddress(node->childs.back());
        break;
    case AstNodeKind::ArrayPointerIndex:
        forceTakeAddress(static_cast<AstPointerDeRef*>(node)->array);
        break;
    }
}

bool SyntaxJob::doDefer(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::Defer, sourceFile, parent);
    if (result)
        *result = node;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(doAffectExpression(node, nullptr));

    auto expr = node->childs.front();
    node->ownerScope->deferredNodes.push_back(expr);
    expr->flags |= AST_NO_BYTECODE;

    return true;
}

bool SyntaxJob::doLeftExpression(AstNode** result)
{
    switch (token.id)
    {
    case TokenId::Intrinsic:
        SWAG_CHECK(doIdentifierRef(nullptr, result));
        return true;

    case TokenId::SymLeftParen:
    {
        auto multi = Ast::newNode<AstNode>(this, AstNodeKind::MultiIdentifierTuple, sourceFile, nullptr);
        *result    = multi;
        SWAG_CHECK(eatToken());
        while (true)
        {
            SWAG_CHECK(doIdentifierRef(multi));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen, "after tuple affectation"));
        break;
    }

    case TokenId::Identifier:
    case TokenId::SymBackTick:
    {
        AstNode* exprNode = nullptr;
        AstNode* multi    = nullptr;
        while (true)
        {
            SWAG_CHECK(doIdentifierRef(multi, &exprNode));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());

            if (!multi)
            {
                multi = Ast::newNode<AstNode>(this, AstNodeKind::MultiIdentifier, sourceFile, nullptr);
                Ast::addChildBack(multi, exprNode);
            }
        }

        *result = multi ? multi : exprNode;
        break;
    }

    default:
        return syntaxError(token, format("invalid token '%s' in left expression", token.text.c_str()));
    }

    return true;
}

bool SyntaxJob::isValidVarName(AstNode* node)
{
    if (node->name[0] != '@')
        return true;

    // @alias must be of the form @aliasNUM
    if (node->name.length() >= 6)
    {
        if (node->name == "@alias")
            return syntaxError(node->token, "@alias variable name must be followed by a number");

        if (node->name.find("@alias") == 0)
        {
            const char* pz = node->name.c_str() + 6;
            while (*pz)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return syntaxError(node->token, format("invalid @alias variable name '%s', '%s' is not a valid number", node->name.c_str(), node->name.c_str() + 6));
                pz++;
            }

            return true;
        }
    }

    return syntaxError(node->token, format("invalid variable name '%s', cannot start with '@'", node->name.c_str()));
}

bool SyntaxJob::doVarDeclExpression(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, AstNodeKind kind, AstNode** result)
{
    bool acceptDeref = true;
    if (currentScope->kind == ScopeKind::Struct || currentScope->kind == ScopeKind::File)
        acceptDeref = false;

    // Multiple affectation
    if (leftNode->kind == AstNodeKind::MultiIdentifier)
    {
        auto parentNode = parent;
        if (acceptDeref)
        {
            parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            if (result)
                *result = parentNode;
        }

        // Declare first variable, and affect it
        auto savedtoken = token;
        auto front      = CastAst<AstIdentifierRef>(leftNode->childs.front(), AstNodeKind::IdentifierRef);

        // Then declare all other variables, and assign them to the first one
        bool firstDone = false;
        for (auto child : leftNode->childs)
        {
            SWAG_CHECK(checkIsSingleIdentifier(child));
            auto identifier = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(isValidVarName(identifier));
            AstVarDecl* varNode = Ast::newVarDecl(sourceFile, identifier->name, parentNode, this);
            varNode->kind       = kind;
            varNode->token      = identifier->token;
            varNode->flags |= AST_R_VALUE;

            if (!firstDone)
            {
                firstDone = true;
                Ast::addChildBack(varNode, type);
                Ast::addChildBack(varNode, assign);
                varNode->type       = type;
                varNode->assignment = assign;
            }

            // We are supposed to be constexpr, so we need to duplicate the assignment instead of generating an
            // affectation to the first variable
            else if (!acceptDeref)
            {
                varNode->type       = Ast::clone(type, varNode);
                varNode->assignment = Ast::clone(assign, varNode);
            }
            else
            {
                varNode->assignment        = Ast::newIdentifierRef(sourceFile, front->name, varNode, this);
                varNode->assignment->token = savedtoken;
            }

            if (varNode->assignment)
                varNode->assignment->semanticAfterFct = SemanticJob::resolveVarDeclAfterAssign;
            SWAG_CHECK(currentScope->symTable.registerSymbolName(&context, varNode, SymbolKind::Variable));
        }
    }

    // Tuple dereference
    else if (leftNode->kind == AstNodeKind::MultiIdentifierTuple)
    {
        SWAG_VERIFY(acceptDeref, error(leftNode->token, format("cannot destructure a tuple in %s", Scope::getArticleKindName(currentScope->kind))));

        auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
        if (result)
            *result = parentNode;

        // Generate an expression of the form "var __tmp_0 = assignment"
        auto        savedtoken = token;
        auto        tmpVarName = format("__tmp_%d", g_Global.uniqueID.fetch_add(1));
        AstVarDecl* varNode    = Ast::newVarDecl(sourceFile, tmpVarName, parentNode, this);
        varNode->kind          = kind;
        varNode->token         = savedtoken;
        Ast::addChildBack(varNode, type);
        varNode->type = type;
        Ast::addChildBack(varNode, assign);
        varNode->assignment = assign;
        if (assign)
            varNode->assignment->semanticAfterFct = SemanticJob::resolveVarDeclAfterAssign;
        varNode->flags |= AST_R_VALUE;
        SWAG_CHECK(currentScope->symTable.registerSymbolName(&context, varNode, SymbolKind::Variable));

        // And reference that variable, in the form value = __tmp_0.item?
        int idx = 0;
        for (auto child : leftNode->childs)
        {
            SWAG_CHECK(checkIsSingleIdentifier(child));
            auto identifier = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(isValidVarName(identifier));
            varNode        = Ast::newVarDecl(sourceFile, identifier->name, parentNode, this);
            varNode->kind  = kind;
            varNode->token = identifier->token;
            varNode->flags |= AST_R_VALUE;
            SWAG_CHECK(currentScope->symTable.registerSymbolName(&context, varNode, SymbolKind::Variable));
            identifier                            = Ast::newIdentifierRef(sourceFile, format("%s.item%d", tmpVarName.c_str(), idx++), varNode, this);
            identifier->token                     = savedtoken;
            varNode->assignment                   = identifier;
            varNode->assignment->semanticAfterFct = SemanticJob::resolveVarDeclAfterAssign;
        }
    }

    // Single declaration/affectation
    else
    {
        SWAG_CHECK(checkIsSingleIdentifier(leftNode));
        auto identifier = leftNode->childs.back();
        SWAG_CHECK(isValidVarName(identifier));
        AstVarDecl* varNode = Ast::newVarDecl(sourceFile, identifier->name, parent, this);
        varNode->kind       = kind;
        varNode->inheritTokenLocation(leftNode->token);

        if (result)
            *result = varNode;
        Ast::addChildBack(varNode, type);
        varNode->type = type;
        Ast::addChildBack(varNode, assign);
        varNode->assignment = assign;
        if (assign)
            varNode->assignment->semanticAfterFct = SemanticJob::resolveVarDeclAfterAssign;
        varNode->flags |= AST_R_VALUE;
        SWAG_CHECK(currentScope->symTable.registerSymbolName(&context, varNode, SymbolKind::Variable));
    }

    return true;
}

bool SyntaxJob::doAffectExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doLeftExpression(&leftNode));

    // Variable declaration and initialization by ':='
    if (token.id == TokenId::SymColonEqual)
    {
        AstNode* assign;
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doInitializationExpression(nullptr, &assign));
        SWAG_CHECK(doVarDeclExpression(parent, leftNode, nullptr, assign, AstNodeKind::VarDecl, result));
    }

    // Labeled statement with identifier:
    else if (token.id == TokenId::SymColon)
    {
        auto labelNode = Ast::newNode<AstLabelBreakable>(this, AstNodeKind::LabelBreakable, sourceFile, parent);
        if (result)
            *result = labelNode;
        labelNode->semanticFct = SemanticJob::resolveLabel;
        SWAG_VERIFY(leftNode->kind == AstNodeKind::IdentifierRef, syntaxError(leftNode->token, "invalid labelled statement name"));
        SWAG_VERIFY(leftNode->childs.size() == 1, syntaxError(leftNode->token, "invalid labelled statement name"));
        auto childBack   = leftNode->childs.back();
        labelNode->name  = childBack->name;
        labelNode->token = childBack->token;
        SWAG_CHECK(tokenizer.getToken(token));

        ScopedBreakable scoped(this, labelNode);
        SWAG_CHECK(doEmbeddedInstruction(labelNode, &labelNode->block));
        return true;
    }

    // Affect operator
    else if (token.id == TokenId::SymEqual ||
             token.id == TokenId::SymPlusEqual ||
             token.id == TokenId::SymMinusEqual ||
             token.id == TokenId::SymAsteriskEqual ||
             token.id == TokenId::SymSlashEqual ||
             token.id == TokenId::SymAmpersandEqual ||
             token.id == TokenId::SymVerticalEqual ||
             token.id == TokenId::SymCircumflexEqual ||
             token.id == TokenId::SymTildeEqual ||
             token.id == TokenId::SymPercentEqual ||
             token.id == TokenId::SymLowerLowerEqual ||
             token.id == TokenId::SymGreaterGreaterEqual)
    {
        // Multiple affectation
        if (leftNode->kind == AstNodeKind::MultiIdentifier)
        {
            auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            if (result)
                *result = parentNode;

            // Generate an expression of the form "var firstVar = assignment"
            bool firstDone  = false;
            auto savedtoken = token;
            auto front      = CastAst<AstIdentifierRef>(leftNode->childs.front(), AstNodeKind::IdentifierRef);
            front->computeName();
            while (!leftNode->childs.empty())
            {
                auto child        = leftNode->childs.front();
                auto affectNode   = Ast::newAffectOp(sourceFile, parentNode);
                affectNode->token = savedtoken;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                forceTakeAddress(child);

                if (!firstDone)
                {
                    firstDone = true;
                    SWAG_CHECK(tokenizer.getToken(token));
                    SWAG_CHECK(doExpression(affectNode));
                }
                else
                {
                    Ast::newIdentifierRef(sourceFile, front->name, affectNode, this)->token = savedtoken;
                }
            }
        }

        // Tuple destruct
        else if (leftNode->kind == AstNodeKind::MultiIdentifierTuple)
        {
            auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            if (result)
                *result = parentNode;

            // Generate an expression of the form "var __tmp_0 = assignment"
            auto        savedtoken = token;
            auto        tmpVarName = format("__tmp_%d", g_Global.uniqueID.fetch_add(1));
            AstVarDecl* varNode    = Ast::newVarDecl(sourceFile, tmpVarName, parentNode, this);
            varNode->token         = savedtoken;
            SWAG_CHECK(tokenizer.getToken(token));
            SWAG_CHECK(doExpression(varNode, &varNode->assignment));

            // And reference that variable, in the form value = __tmp_0.item?
            int idx = 0;
            while (!leftNode->childs.empty())
            {
                auto child        = leftNode->childs.front();
                auto affectNode   = Ast::newAffectOp(sourceFile, parentNode);
                affectNode->token = savedtoken;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                forceTakeAddress(child);
                Ast::newIdentifierRef(sourceFile, format("%s.item%d", tmpVarName.c_str(), idx++), affectNode, this)->token = savedtoken;
            }
        }

        // One normal simple affectation
        else
        {
            auto affectNode   = Ast::newAffectOp(sourceFile, parent, this);
            affectNode->token = move(token);

            Ast::addChildBack(affectNode, leftNode);
            forceTakeAddress(leftNode);

            SWAG_CHECK(tokenizer.getToken(token));
            SWAG_CHECK(doExpression(affectNode));
            if (result)
                *result = affectNode;
        }
    }
    else
    {
        Ast::addChildBack(parent, leftNode);
        if (result)
            *result = leftNode;
    }

    SWAG_CHECK(eatSemiCol("after left expression"));
    return true;
}

bool SyntaxJob::doInit(AstNode* parent, AstNode** result)
{
    AstInit* node     = Ast::newNode<AstInit>(this, AstNodeKind::Init, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveInit;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, &node->expression));

    if (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, &node->count));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));

    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(doFuncCallParameters(node, &node->parameters));
    }

    return true;
}

bool SyntaxJob::doDrop(AstNode* parent, AstNode** result)
{
    AstDrop* node     = Ast::newNode<AstDrop>(this, AstNodeKind::Drop, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveDrop;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, &node->expression));

    if (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, &node->count));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}