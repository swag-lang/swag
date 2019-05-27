#pragma once
#include "Job.h"
#include "Utf8.h"
struct AstNode;
struct Module;
struct SourceFile;
struct SemanticJob;

enum class SemanticResult
{
    Done,
    Pending,
};

struct SemanticContext
{
    SourceFile*    sourceFile;
    SemanticJob*   job;
    AstNode*       node;
    SemanticResult result;
};

struct SemanticJob : public Job
{
    JobResult execute() override;
    bool      error(SemanticContext* context, const Utf8& msg);

    void reset() override
    {
        nodes.clear();
    }

    static bool resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveSingleOpExclam(SemanticContext* context, AstNode* op);
    static bool resolveSingleOpMinus(SemanticContext* context, AstNode* op);
    static bool resolveSingleOp(SemanticContext* context);
    static bool resolveTypeExpression(SemanticContext* context);
    static bool resolveVarDecl(SemanticContext* context);
    static bool resolveTypeDecl(SemanticContext* context);
    static bool resolveLiteral(SemanticContext* context);
    static bool resolveBoolExpression(SemanticContext* context);
    static bool resolveCompareExpression(SemanticContext* context);
    static bool resolveFactorExpression(SemanticContext* context);
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
	static bool resolveFuncDeclParameters(SemanticContext* context);
	static bool resolveFuncDeclType(SemanticContext* context);

    Module*          module;
    SourceFile*      sourceFile;
    AstNode*         astRoot;
    vector<AstNode*> nodes;
};
