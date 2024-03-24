#pragma once
#include "Report/ErrorContext.h"
#include "Semantic/Scope.h"
#include "Syntax/AstNode.h"
#include "Syntax/Tokenizer/Tokenizer.h"

struct AstAttrUse;
struct AstBreakable;
struct AstCompilerIfBlock;
struct AstFuncCallParams;
struct AstFuncDecl;
struct AstIdentifier;
struct AstIdentifierRef;
struct AstInline;
struct AstNode;
struct AstStruct;
struct AstTryCatchAssume;
struct AstTypeExpression;
struct AstTypeLambda;
struct AstVarDecl;
struct AstWith;
struct JobContext;
struct Module;
struct Parser;
struct Scope;
struct SourceFile;
struct Utf8;

enum class AstNodeKind : uint8_t;

using AstNodeFlags    = Flags<uint64_t>;
using ModifierFlags   = Flags<uint32_t>;
using IdentifierFlags = Flags<uint32_t>;
using ExprFlags       = Flags<uint32_t>;
using ParserFlags     = Flags<uint32_t>;

enum class InvalidTokenError
{
    EmbeddedInstruction,
    TopLevelInstruction,
    PrimaryExpression,
    LeftExpression,
};

enum class SyntaxStructType
{
    Struct,
    Interface,
};

constexpr ModifierFlags MODIFIER_OVERFLOW      = 0x00000001;
constexpr ModifierFlags MODIFIER_UP            = 0x00000002;
constexpr ModifierFlags MODIFIER_NO_LEFT_DROP  = 0x00000004;
constexpr ModifierFlags MODIFIER_MOVE          = 0x00000008;
constexpr ModifierFlags MODIFIER_NO_RIGHT_DROP = 0x00000010;
constexpr ModifierFlags MODIFIER_BIT           = 0x00000020;
constexpr ModifierFlags MODIFIER_UN_CONST      = 0x00000040;
constexpr ModifierFlags MODIFIER_BACK          = 0x00000080;

constexpr IdentifierFlags IDENTIFIER_NO_FCT_PARAMS   = 0x00000001;
constexpr IdentifierFlags IDENTIFIER_NO_GEN_PARAMS   = 0x00000002;
constexpr IdentifierFlags IDENTIFIER_TYPE_DECL       = 0x00000004;
constexpr IdentifierFlags IDENTIFIER_ACCEPT_QUESTION = 0x00000008;
constexpr IdentifierFlags IDENTIFIER_NO_ARRAY        = 0x00000010;
constexpr IdentifierFlags IDENTIFIER_NO_PARAMS       = IDENTIFIER_NO_FCT_PARAMS | IDENTIFIER_NO_GEN_PARAMS | IDENTIFIER_NO_ARRAY;

constexpr ExprFlags EXPR_FLAG_NONE                  = 0x00000000;
constexpr ExprFlags EXPR_FLAG_SIMPLE                = 0x00000001;
constexpr ExprFlags EXPR_FLAG_ALIAS                 = 0x00000002;
constexpr ExprFlags EXPR_FLAG_TYPEOF                = 0x00000004;
constexpr ExprFlags EXPR_FLAG_STOP_AFFECT           = 0x00000008;
constexpr ExprFlags EXPR_FLAG_PARAMETER             = 0x00000010;
constexpr ExprFlags EXPR_FLAG_IN_CALL               = 0x00000020;
constexpr ExprFlags EXPR_FLAG_IN_VAR_DECL           = 0x00000040;
constexpr ExprFlags EXPR_FLAG_TYPE_EXPR             = 0x00000080;
constexpr ExprFlags EXPR_FLAG_IN_VAR_DECL_WITH_TYPE = 0x00000100;
constexpr ExprFlags EXPR_FLAG_NAMED_PARAM           = 0x00000200;
constexpr ExprFlags EXPR_FLAG_IN_GENERIC_PARAMS     = 0x00000400;

constexpr ParserFlags PARSER_DEFAULT             = 0x00000000;
constexpr ParserFlags PARSER_TRACK_DOCUMENTATION = 0x00000001;
constexpr ParserFlags PARSER_TRACK_FORMAT        = 0x00000002;

constexpr uint32_t CONTEXT_FLAG_EXPRESSION = 0x00000001;

struct Parser
{
    void setup(ErrorContext* errorCxt, Module* mdl, SourceFile* file, ParserFlags flags);
    bool generateAst();

    static bool saveEmbeddedAst(const Utf8& content, const AstNode* fromNode, Path& tmpFilePath, Utf8& tmpFileName, uint32_t& previousLogLine);
    bool        constructEmbeddedAst(const Utf8& content, AstNode* parent, AstNode* fromNode, CompilerAstKind kind, bool logGenerated, AstNode** result = nullptr);

    bool error(const Token& tk, const Utf8& msg, const char* help = nullptr, const char* hint = nullptr) const;
    bool error(AstNode* node, const Utf8& msg, const char* help = nullptr, const char* hint = nullptr) const;
    bool error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg, const char* help = nullptr) const;
    bool invalidTokenError(InvalidTokenError kind, const AstNode* parent = nullptr);
    bool invalidIdentifierError(const TokenParse& myToken, const char* msg = nullptr) const;

    bool eatToken();
    bool eatCloseToken(TokenId id, const SourceLocation& start, const char* msg = "");
    bool eatToken(TokenId id, const char* msg);
    void prepareExpectTokenError();
    bool eatTokenError(TokenId id, const Utf8& msg);
    bool eatSemiCol(const char* msg);

    static bool testIsSingleIdentifier(const AstNode* node);
    bool        checkIsSingleIdentifier(AstNode* node, const char* msg) const;
    bool        checkIsIdentifier(const TokenParse& myToken, const char* msg) const;
    bool        testIsValidUserName(const AstNode* node) const;
    bool        checkIsValidUserName(AstNode* node, const Token* loc = nullptr) const;
    bool        checkIsValidVarName(AstNode* node) const;
    bool        doCheckPublicInternalPrivate(const Token& tokenAttr) const;
    void        registerSubDecl(AstNode* subDecl);
    static void setForceTakeAddress(AstNode* node);

    bool doAnonymousStruct(AstNode* parent, AstNode** result, ExprFlags exprFlags, bool typeSpecified, bool isUnion);
    bool doCompilerScopeBreakable(AstNode* parent, AstNode** result);
    bool doGenericFuncCallParameters(AstNode* parent, AstFuncCallParams** result);
    bool doFuncCallParameters(AstNode* parent, AstFuncCallParams** result, TokenId closeToken);
    bool doFuncDeclParameterSelf(AstVarDecl* paramNode);
    bool doIntrinsicTag(AstNode* parent, AstNode** result);
    bool doCompilerIfFor(AstNode* parent, AstNode** result, AstNodeKind kind);
    bool doCompilerIf(AstNode* parent, AstNode** result);
    bool doCompilerMacro(AstNode* parent, AstNode** result);
    bool doCompilerMixin(AstNode* parent, AstNode** result);
    bool doCompilerAssert(AstNode* parent, AstNode** result);
    bool doCompilerPrint(AstNode* parent, AstNode** result);
    bool doCompilerError(AstNode* parent, AstNode** result);
    bool doCompilerWarning(AstNode* parent, AstNode** result);
    bool doCompilerAst(AstNode* parent, AstNode** result);
    bool doCompilerValidIf(AstNode* parent, AstNode** result);
    bool doCompilerRunTopLevel(AstNode* parent, AstNode** result);
    bool doCompilerRunEmbedded(AstNode* parent, AstNode** result);
    bool doCompilerForeignLib(AstNode* parent, AstNode** result);
    bool doCompilerGlobal(AstNode* parent, AstNode** result);
    bool doCompilerSpecialValue(AstNode* parent, AstNode** result);
    bool doIntrinsicDefined(AstNode* parent, AstNode** result);
    bool doIntrinsicLocation(AstNode* parent, AstNode** result);
    bool doCompilerDependencies(AstNode* parent);
    bool doCompilerInclude(AstNode* parent, AstNode** result);
    bool doCompilerLoad(AstNode* parent);
    bool doCompilerImport(AstNode* parent);
    bool doCompilerPlaceHolder(AstNode* parent);
    bool doTopLevelInstruction(AstNode* parent, AstNode** result);
    bool doVarDeclMultiIdentifier(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result, bool forLet, bool acceptDeref);
    bool doVarDeclMultiIdentifierTuple(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result, bool forLet, bool acceptDeref);
    bool doVarDeclSingleIdentifier(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result, bool forLet);
    bool doVarDecl(AstNode* parent, AstNode** result);
    bool doVarDecl(AstNode* parent, AstNode** result, AstNodeKind kind, bool forStruct = false, bool forLet = false);
    bool doTypeAlias(AstNode* parent, AstNode** result);
    bool doNameAlias(AstNode* parent, AstNode** result);
    bool doSingleTypeExpression(AstTypeExpression* node, ExprFlags exprFlags);
    bool doSubTypeExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doTypeExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doLambdaClosureType(AstNode* parent, AstNode** result, bool inTypeVarDecl = false);
    bool doLambdaClosureType(AstTypeLambda* node, bool inTypeVarDecl);
    bool doLambdaClosureParameters(AstTypeLambda* node, bool inTypeVarDecl, AstNode* params);
    bool doDefer(AstNode* parent, AstNode** result);
    bool doVarDeclExpression(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result, bool forLet = false);
    bool doAffectExpression(AstNode* parent, AstNode** result, const AstWith* withNode = nullptr);
    bool errorTopLevelIdentifier();
    bool doIdentifier(AstNode* parent, IdentifierFlags identifierFlags = 0);
    bool doIdentifierRef(AstNode* parent, AstNode** result, IdentifierFlags identifierFlags = 0);
    bool doDiscard(AstNode* parent, AstNode** result);
    bool doTryCatchAssume(AstNode* parent, AstNode** result, bool afterDiscard = false);
    bool doThrow(AstNode* parent, AstNode** result);
    bool doPublicInternal(AstNode* parent, AstNode** result, bool forGlobal);
    bool doNamespace(AstNode* parent, AstNode** result);
    bool doNamespace(AstNode* parent, AstNode** result, bool forGlobal, bool forUsing);
    bool doNamespaceOnName(AstNode* parent, AstNode** result, bool forGlobal, bool forUsing, const Token* privName = nullptr);
    bool doEnumContent(AstNode* parent, AstNode** result);
    bool doSubEnumValue(AstNode* parent, AstNode** result);
    bool doEnumValue(AstNode* parent, AstNode** result);
    bool doEnum(AstNode* parent, AstNode** result);
    bool doStructBody(AstNode* parent, SyntaxStructType structType, AstNode** result);
    bool doStruct(AstNode* parent, AstNode** result);
    bool doStructContent(AstStruct* structNode, SyntaxStructType structType);
    bool doImpl(AstNode* parent, AstNode** result);
    bool doAssignmentExpression(AstNode* parent, AstNode** result);
    bool doExpressionListTuple(AstNode* parent, AstNode** result);
    bool doExpressionListArray(AstNode* parent, AstNode** result);
    bool doInitializationExpression(const TokenParse& forToken, AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doLiteral(AstNode* parent, AstNode** result);
    bool doIntrinsicProp(AstNode* parent, AstNode** result);
    bool doIndex(AstNode* parent, AstNode** result);
    bool doKeepRef(AstNode* parent, AstNode** result);
    bool doMoveRef(AstNode* parent, AstNode** result);
    bool doDeRef(AstNode* parent, AstNode** result);
    bool doModifiers(const Token& forNode, TokenId tokenId, ModifierFlags& mdfFlags, AstNode* node = nullptr);
    bool doLambdaExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doSinglePrimaryExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doPrimaryExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doUnaryExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doFactorExpression(AstNode** parent, ExprFlags exprFlags, AstNode** result);
    bool doCompareExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doBoolExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doMoveExpression(const Token& forToken, TokenId tokenId, AstNode* parent, ExprFlags exprFlags, AstNode** result);
    bool doGenericDeclParameters(AstNode* parent, AstNode** result);
    bool doLambdaFuncDecl(AstNode* parent, AstNode** result, bool acceptMissingType = false, bool* hasMissingType = nullptr);
    bool doFuncDecl(AstNode* parent, AstNode** result, TokenId typeFuncId = TokenId::Invalid);
    bool doFuncDeclParameter(AstNode* parent, bool acceptMissingType = false, bool* hasMissingType = nullptr);
    bool doFuncDeclParameters(AstNode* parent, AstNode** result, bool acceptMissingType = false, bool* hasMissingType = nullptr, bool isMethod = false, bool isConstMethod = false, bool isItfMethod = false);
    bool doAttrDecl(AstNode* parent, AstNode** result);
    bool doAttrUse(AstNode* parent, AstNode** result, bool single = false);
    bool doEmbeddedInstruction(AstNode* parent, AstNode** result);
    bool doScopedStatement(AstNode* parent, const Token& forToken, AstNode** result, bool mustHaveDo = true);
    bool doCompilerIfStatementFor(AstNode* parent, AstNode** result, AstNodeKind kind, bool forIf);
    bool doCompilerIfStatement(AstNode* parent, AstNode** result);
    bool doGlobalCurlyStatement(AstNode* parent, AstNode** result);
    bool doCurlyStatement(AstNode* parent, AstNode** result);
    bool doScopedCurlyStatement(AstNode* parent, AstNode** result, ScopeKind scopeKind = ScopeKind::Statement);
    bool doReturn(AstNode* parent, AstNode** result);
    bool doWith(AstNode* parent, AstNode** result);
    bool doPrivate(AstNode* parent, AstNode** result);
    bool doUsing(AstNode* parent, AstNode** result, bool isGlobal);
    bool doCast(AstNode* parent, AstNode** result);
    bool doAutoCast(AstNode* parent, AstNode** result);
    bool doIf(AstNode* parent, AstNode** result);
    bool doWhile(AstNode* parent, AstNode** result);
    bool doFor(AstNode* parent, AstNode** result);
    bool doLoop(AstNode* parent, AstNode** result);
    bool doVisit(AstNode* parent, AstNode** result);
    bool doSwitch(AstNode* parent, AstNode** result);
    bool doBreak(AstNode* parent, AstNode** result);
    bool doFallThrough(AstNode* parent, AstNode** result);
    bool doUnreachable(AstNode* parent, AstNode** result);
    bool doContinue(AstNode* parent, AstNode** result);
    bool doArrayPointerIndex(AstNode** exprNode);
    bool doLeftInstruction(AstNode* parent, AstNode** result, const AstWith* withNode = nullptr);
    bool doLeftExpressionVar(AstNode* parent, AstNode** result, IdentifierFlags identifierFlags = 0, const AstWith* withNode = nullptr);
    bool doLeftExpressionAffect(AstNode* parent, AstNode** result, const AstWith* withNode = nullptr);
    bool doMultiIdentifierAffect(AstNode* parent, AstNode** result, AstNode* leftNode, SpecFlags opFlags, AttributeFlags opAttrFlags, TokenParse& savedToken);
    bool doTupleUnpacking(AstNode* parent, AstNode** result, AstNode* leftNode, SpecFlags opFlags, AttributeFlags opAttrFlags, TokenParse& savedToken);
    bool doSingleIdentifierAffect(AstNode* parent, AstNode** result, AstNode* leftNode, SpecFlags opFlags, AttributeFlags opAttrFlags, const TokenParse& savedToken);
    bool doInit(AstNode* parent, AstNode** result);
    bool doDropCopyMove(AstNode* parent, AstNode** result);
    bool doRange(AstNode* parent, AstNode* expression, AstNode** result);

    Tokenizer  tokenizer;
    TokenParse prevTokenParse;
    TokenParse tokenParse;

    ErrorContext* context     = nullptr;
    SourceFile*   sourceFile  = nullptr;
    Module*       module      = nullptr;
    AstNode*      dummyResult = nullptr;

    Scope*              currentScope           = nullptr;
    AstFuncDecl*        currentFct             = nullptr;
    AstBreakable*       currentBreakable       = nullptr;
    Scope*              currentStructScope     = nullptr;
    Scope*              currentSelfStructScope = nullptr;
    AstCompilerIfBlock* currentCompilerIfBlock = nullptr;
    AstTryCatchAssume*  currentTryCatchAssume  = nullptr;
    AstInline*          currentInline          = nullptr;
    AstNodeFlags        currentAstNodeFlags    = 0;

    ParserFlags parserFlags  = 0;
    uint32_t    freezeFormat = 0;

    bool afterGlobal = false;
};
