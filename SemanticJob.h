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
    bool execute() override;
    bool error(SemanticContext* context, const Utf8& msg);

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
    static bool resolveType(SemanticContext* context);
    static bool resolveVarDecl(SemanticContext* context);
    static bool resolveLiteral(SemanticContext* context);
    static bool resolveBoolExpression(SemanticContext* context);
    static bool resolveCompareExpression(SemanticContext* context);
    static bool resolveFactorExpression(SemanticContext* context);
    static bool resolveCompilerAssert(SemanticContext* context);
	static bool resolveCompilerPrint(SemanticContext* context);
	static bool resolveCompilerRun(SemanticContext* context);

    Module*          module;
    SourceFile*      sourceFile;
    AstNode*         astRoot;
    vector<AstNode*> nodes;
};
