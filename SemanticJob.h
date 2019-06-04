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
enum class AstNodeKind;
enum class SymbolKind;
struct ByteCodeGenJob;

enum class SemanticResult
{
    Done,
    Pending,
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
    JobResult execute() override;
    bool      error(SemanticContext* context, const Utf8& msg);

    void reset() override
    {
        nodes.clear();
        dependentSymbols.clear();
        scopeHierarchy.clear();
    }

    static bool checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode, AstNodeKind kind);
    static bool collectAttributes(SemanticContext* context, set<TypeInfoFuncAttr*>& result, AstNode* attrUse, AstNode* forNode, AstNodeKind kind, uint64_t& flags);
    static void collectScopeHiearchy(SemanticContext* context, vector<Scope*>& scopes, Scope* startScope);
    static void setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstNode* node, SymbolName* symbol, SymbolOverload* overload);
    static bool checkSymbolGhosting(SemanticContext* context, Scope* startScope, AstNode* node, SymbolKind kind);
    static bool setupFuncDeclParameters(SourceFile* sourceFile, TypeInfoFuncAttr* typeInfo, AstNode* parameters);
    static bool executeNode(SemanticContext* context, AstNode* node, bool onlyconstExpr);

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
    static bool resolveLiteral(SemanticContext* context);
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
    static bool resolveFuncCall(SemanticContext* context);
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

    Module*             module;
    SourceFile*         sourceFile;
    vector<AstNode*>    nodes;
    vector<SymbolName*> dependentSymbols;
    vector<Scope*>      scopeHierarchy;
    set<Scope*>         scopesHere;
};
