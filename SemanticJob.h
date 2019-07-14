#pragma once
#include "Job.h"
#include "Utf8.h"
struct AstNode;
struct Module;
struct SourceFile;
struct SemanticJob;
struct TypeInfoFuncAttr;
struct SymbolName;
struct Scope;
struct AstIdentifierRef;
struct SymbolOverload;
struct AstAttrUse;
enum class AstNodeKind;
enum class SymbolKind;
struct SymbolAttributes;
struct ByteCodeGenJob;
struct TypeInfo;

enum class SemanticResult
{
    Done,
    Pending,
};

enum class SegmentBuffer
{
    Data,
    Constant,
};

struct SemanticContext
{
    SourceFile*    sourceFile = nullptr;
    SemanticJob*   job        = nullptr;
    AstNode*       node       = nullptr;
    SemanticResult result;
};

struct SemanticJob : public Job
{
    JobResult   execute() override;
    static bool error(SemanticContext* context, const Utf8& msg);
    static bool internalError(SemanticContext* context, const char* msg);

    void reset() override
    {
        nodes.clear();
        cacheDependentSymbols.clear();
        cacheScopeHierarchy.clear();
    }

    static bool checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode, AstNodeKind kind);
    static bool collectAttributes(SemanticContext* context, SymbolAttributes& result, AstAttrUse* attrUse, AstNode* forNode, AstNodeKind kind, uint64_t& flags);
    static void collectScopeHiearchy(SemanticContext* context, vector<Scope*>& scopes, Scope* startScope);
    static bool setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstNode* node, SymbolName* symbol, SymbolOverload* overload);
    static bool checkSymbolGhosting(SemanticContext* context, Scope* startScope, AstNode* node, SymbolKind kind);
    static bool setupFuncDeclParams(SourceFile* sourceFile, TypeInfoFuncAttr* typeInfo, AstNode* parameters);
    static bool executeNode(SemanticContext* context, AstNode* node, bool onlyconstExpr);
    static bool forceExecuteNode(SemanticContext* context);
    static bool collectLiterals(SourceFile* sourceFile, uint32_t& offset, AstNode* node, vector<AstNode*>* orderedChilds, SegmentBuffer buffer);

    static bool resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveUnaryOpExclam(SemanticContext* context, AstNode* op);
    static bool resolveUnaryOpMinus(SemanticContext* context, AstNode* op);
    static bool resolveUnaryOp(SemanticContext* context);
    static bool resolveTypeExpression(SemanticContext* context);
    static bool resolveVarDecl(SemanticContext* context);
    static bool resolveTypeDecl(SemanticContext* context);
    static bool resolveCountProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicProperty(SemanticContext* context);
    static bool resolveLiteral(SemanticContext* context);
    static bool resolveIndex(SemanticContext* context);
    static bool resolveExpressionList(SemanticContext* context);
    static bool resolveBoolExpression(SemanticContext* context);
    static bool resolveCompareExpression(SemanticContext* context);
    static bool resolveFactorExpression(SemanticContext* context);
    static bool resolveShiftExpression(SemanticContext* context);
    static bool resolveCompilerAssert(SemanticContext* context);
    static bool resolveCompilerPrint(SemanticContext* context);
    static bool resolveCompilerRun(SemanticContext* context);
    static bool resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveIdentifier(SemanticContext* context);
    static bool resolveIdentifierRef(SemanticContext* context);
    static bool resolveEnumType(SemanticContext* context);
    static bool resolveEnumValue(SemanticContext* context);
    static bool resolveFuncDecl(SemanticContext* context);
    static bool resolveFuncDeclParams(SemanticContext* context);
    static bool resolveFuncDeclType(SemanticContext* context);
    static bool resolveFuncCallParams(SemanticContext* context);
    static bool resolveFuncCallParam(SemanticContext* context);
    static bool resolveAttrDecl(SemanticContext* context);
    static bool resolveAttrUse(SemanticContext* context);
    static bool resolveReturn(SemanticContext* context);
    static bool resolveNamespace(SemanticContext* context);
    static bool resolveUsing(SemanticContext* context);
    static bool resolveBitmaskOr(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBitmaskAnd(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveShiftLeft(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveShiftRight(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveXor(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveUnaryOpInvert(SemanticContext* context, AstNode* op);
    static bool resolveCast(SemanticContext* context);
    static bool resolveIf(SemanticContext* context);
    static bool resolveWhile(SemanticContext* context);
	static bool resolveSwitch(SemanticContext* context);
	static bool resolveCase(SemanticContext* context);
    static bool resolveLoop(SemanticContext* context);
    static bool resolveAffect(SemanticContext* context);
    static bool resolveMakePointer(SemanticContext* context);
    static bool resolveScopedStmtBefore(SemanticContext* context);
    static bool resolveArrayPointerDeRef(SemanticContext* context);
    static bool resolveArrayOrPointerRef(SemanticContext* context);
    static bool resolveTypeList(SemanticContext* context);

    Module*                 module;
    SourceFile*             sourceFile;
    vector<AstNode*>        nodes;
    vector<SymbolName*>     cacheDependentSymbols;
    vector<Scope*>          cacheScopeHierarchy;
    set<Scope*>             scopesHere;
    set<Scope*>             scopesHereNoAlt;
    vector<SymbolOverload*> cacheMatches;
    vector<SymbolOverload*> cacheBadSignature;
};

extern Pool<SemanticJob> g_Pool_semanticJob;