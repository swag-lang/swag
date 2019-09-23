#pragma once
#include "Job.h"
#include "Utf8.h"
#include "TypeInfo.h"
#include "Diagnostic.h"
#include "Scope.h"
struct AstNode;
struct Module;
struct SourceFile;
struct SemanticJob;
struct TypeInfoFuncAttr;
struct SymbolName;
struct AstIdentifierRef;
struct SymbolOverload;
struct AstAttrUse;
enum class AstNodeKind;
enum class SymbolKind;
struct SymbolAttributes;
struct ByteCodeGenJob;
struct AstIdentifier;
struct AstFuncDecl;
struct DataSegmentLocation;
struct DataSegment;

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
    SemanticResult result     = SemanticResult::Done;
    ErrorContext   errorContext;

    void reset()
    {
        errorContext.genericInstanceTree.clear();
        errorContext.genericInstanceTreeFile.clear();
    }
};

struct OneMatch
{
    SymbolOverload*        symbolOverload;
    vector<TypeInfoParam*> solvedParameters;
};

struct OneGenericMatch
{
    uint32_t              flags;
    SymbolOverload*       symbolOverload;
    vector<ComputedValue> genericParametersCallValues;
    vector<TypeInfo*>     genericParametersCallTypes;
    vector<TypeInfo*>     genericParametersGenTypes;
};

struct SemanticJob : public Job
{
    JobResult   execute() override;
    static bool error(SemanticContext* context, const Utf8& msg);
    static bool internalError(SemanticContext* context, const char* msg, AstNode* node = nullptr);

    static bool checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode, AstNodeKind kind);
    static bool collectAttributes(SemanticContext* context, SymbolAttributes& result, AstAttrUse* attrUse, AstNode* forNode, AstNodeKind kind, uint64_t& flags);
    static void collectScopeHiearchy(SemanticContext* context, vector<Scope*>& scopes, vector<AlternativeScope>& scopesVars, Scope* startScope);
    static bool setupIdentifierRef(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstIdentifier* identifier, SymbolName* symbol, SymbolOverload* overload, OneMatch* oneMatch, AstNode* dependentVar);
    static bool checkSymbolGhosting(SemanticContext* context, Scope* startScope, AstNode* node, SymbolKind kind);
    static bool setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, AstNode* funcAttr, AstNode* parameters, bool forGenerics);
    static bool executeNode(SemanticContext* context, AstNode* node, bool onlyconstExpr);
    static bool forceExecuteNode(SemanticContext* context);
    static bool collectLiterals(SemanticContext* context, uint32_t& offset, AstNode* node, DataSegment* segment);
	static bool reserveAndStoreToSegmentNoLock(SemanticContext* context, uint32_t& storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
	static bool storeToSegmentNoLock(SemanticContext* context, uint32_t storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool collectStructLiterals(SemanticContext* context, SourceFile* sourceFile, uint32_t& offset, AstNode* node, DataSegment* segment);
    static bool matchIdentifierParameters(SemanticContext* context, AstNode* genericParameters, AstNode* callParameters, AstIdentifier* node);
    static bool checkFuncPrototype(SemanticContext* context, AstFuncDecl* node);
	static bool dealWithAny(SemanticContext* context, AstNode* anyNode, AstNode* castedNode);
    static bool checkIsConcrete(SemanticContext* context, AstNode* node);
    static bool evaluateConstExpression(SemanticContext* context, AstNode* node);
    static bool checkUnreachableCode(SemanticContext* context);
    static bool waitForStructUserOps(SemanticContext* context, AstNode* node);
	static bool waitForSwagScope(SemanticContext* context);

    void waitForSymbol(SymbolName* symbol);
    void setPending();

    static bool resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveUnaryOpExclam(SemanticContext* context, AstNode* op);
    static bool resolveUnaryOpMinus(SemanticContext* context, AstNode* op);
    static bool resolveUnaryOp(SemanticContext* context);
    static bool resolveTypeExpression(SemanticContext* context);
    static bool resolveTypeLambda(SemanticContext* context);
    static bool resolveTypeTuple(SemanticContext* context);
    static bool resolveVarDecl(SemanticContext* context);
    static bool resolveTypeAlias(SemanticContext* context);
    static bool resolveCountProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicProperty(SemanticContext* context);
    static bool resolveLiteral(SemanticContext* context);
    static bool resolveIndex(SemanticContext* context);
    static bool resolveBreak(SemanticContext* context);
    static bool resolveContinue(SemanticContext* context);
    static bool resolveExpressionListCurly(SemanticContext* context);
    static bool resolveExpressionListArray(SemanticContext* context);
    static bool resolveBoolExpression(SemanticContext* context);
    static bool resolveCompareExpression(SemanticContext* context);
    static bool resolveFactorExpression(SemanticContext* context);
    static bool resolveShiftExpression(SemanticContext* context);
    static bool resolveCompilerAssert(SemanticContext* context);
    static bool resolveCompilerPrint(SemanticContext* context);
    static bool resolveCompilerRun(SemanticContext* context);
    static bool resolveUserOp(SemanticContext* context, const char* name, const char* op, AstNode* left, AstNode* right, bool optionnal = false);
    static bool resolveUserOp(SemanticContext* context, const char* name, const char* op, AstNode* left, vector<AstNode*>& params, bool optionnal = false);
    static bool resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveTupleAccess(SemanticContext* context, bool& eaten);
    static bool resolveIdentifier(SemanticContext* context);
    static bool resolveIdentifierRef(SemanticContext* context);
    static bool resolveImpl(SemanticContext* context);
    static bool preResolveStruct(SemanticContext* context);
    static bool resolveStruct(SemanticContext* context);
    static bool resolveEnum(SemanticContext* context);
    static bool resolveEnumType(SemanticContext* context);
    static bool resolveEnumValue(SemanticContext* context);
    static bool resolveFuncDeclParams(SemanticContext* context);
    static bool resolveFuncDecl(SemanticContext* context);
    static bool resolveFuncDeclType(SemanticContext* context);
	static bool registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode);
    static bool resolveFuncCallParams(SemanticContext* context);
    static bool resolveFuncCallParam(SemanticContext* context);
    static bool resolveAttrDecl(SemanticContext* context);
    static bool resolveAttrUse(SemanticContext* context);
    static bool resolveReturn(SemanticContext* context);
    static bool resolveNamespace(SemanticContext* context);
    static bool resolveUsingVar(SemanticContext* context, AstNode* varNode, TypeInfo* typeInfoVar);
    static bool resolveUsing(SemanticContext* context);
    static bool resolveBitmaskOr(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBitmaskAnd(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveShiftLeft(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveShiftRight(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveXor(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveUnaryOpInvert(SemanticContext* context, AstNode* op);
    static bool resolveExplicitCast(SemanticContext* context);
    static bool resolveExplicitAutoCast(SemanticContext* context);
    static bool resolveCompilerIf(SemanticContext* context);
    static bool resolveIf(SemanticContext* context);
    static bool resolveWhile(SemanticContext* context);
    static bool resolveLoopBefore(SemanticContext* context);
    static bool resolveForBefore(SemanticContext* context);
    static bool resolveFor(SemanticContext* context);
    static bool resolveSwitch(SemanticContext* context);
    static bool resolveCase(SemanticContext* context);
    static bool resolveLoop(SemanticContext* context);
    static bool resolveAffect(SemanticContext* context);
    static bool resolveMakePointer(SemanticContext* context);
    static bool resolveScopedStmtBefore(SemanticContext* context);
    static bool resolveArrayPointerDeRef(SemanticContext* context);
    static bool resolveArrayPointerIndex(SemanticContext* context);
    static bool resolveArrayPointerRef(SemanticContext* context);
    static bool resolveTypeList(SemanticContext* context);
	static bool resolveTrinaryOp(SemanticContext* context);

    void reset() override
    {
        nodes.clear();
        cacheDependentSymbols.clear();
        cacheScopeHierarchy.clear();
        cacheScopeHierarchyVars.clear();
        scopesHere.clear();
        scopesHereNoAlt.clear();
        cacheMatches.clear();
        cacheBadSignature.clear();
        cacheGenericMatches.clear();
        cacheBadSignature.clear();
        cacheBadGenericSignature.clear();
        symMatch.reset();
        waitingSymbolSolved = nullptr;
        context.reset();
    }

    Module*                  module;
    SourceFile*              sourceFile;
    vector<AstNode*>         nodes;
    vector<SymbolName*>      cacheDependentSymbols;
    vector<Scope*>           cacheScopeHierarchy;
    vector<AlternativeScope> cacheScopeHierarchyVars;
    set<Scope*>              scopesHere;
    set<Scope*>              scopesHereNoAlt;
    vector<OneMatch>         cacheMatches;
    vector<OneGenericMatch>  cacheGenericMatches;
    vector<SymbolOverload*>  cacheBadSignature;
    vector<SymbolOverload*>  cacheBadGenericSignature;
    SymbolMatchContext       symMatch;
    SymbolName*              waitingSymbolSolved;
    SemanticContext          context;
};

extern Pool<SemanticJob> g_Pool_semanticJob;