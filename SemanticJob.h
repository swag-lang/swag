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

struct SemanticContext : public JobContext
{
    SemanticJob* job = nullptr;
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

static const uint32_t COLLECT_ALL         = 0x00000000;
static const uint32_t COLLECT_PASS_INLINE = 0x00000001;

struct SemanticJob : public Job
{
    JobResult execute() override;

    static SemanticJob* newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* rootNode, bool run);
    static bool         error(SemanticContext* context, const Utf8& msg);
    static bool         internalError(SemanticContext* context, const char* msg, AstNode* node = nullptr);
    static bool         checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool         notAllowed(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static void         decreaseInterfaceCount(TypeInfoStruct* typeInfoStruct);

    static void enterState(AstNode* node);
    static bool checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode, AstNodeKind kind);
    static bool collectAttributes(SemanticContext* context, SymbolAttributes& result, AstAttrUse* attrUse, AstNode* forNode, AstNodeKind kind, uint32_t& flags);
    static void collectAlternativeScopeHierarchy(SemanticContext* context, vector<Scope*>& scopes, vector<AlternativeScope>& scopesVars, AstNode* startNode);
    static bool collectScopeHierarchy(SemanticContext* context, vector<Scope*>& scopes, vector<AlternativeScope>& scopesVars, AstNode* startNode, uint32_t flags = COLLECT_ALL);
    static bool setupIdentifierRef(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool derefTypeInfo(SemanticContext* context, AstIdentifierRef* parent, SymbolOverload* overload);
    static bool makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* identifier);
    static bool makeInline(SemanticContext* context, AstFuncDecl* funcDecl, AstNode* identifier);
    static bool setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstIdentifier* identifier, SymbolName* symbol, SymbolOverload* overload, OneMatch* oneMatch, AstNode* dependentVar);
    static bool checkSymbolGhosting(SemanticContext* context, AstNode* node, SymbolKind kind);
    static bool setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, AstNode* funcAttr, AstNode* parameters, bool forGenerics);
    static bool executeNode(SemanticContext* context, AstNode* node, bool onlyconstExpr);
    static bool reserveAndStoreToSegment(SemanticContext* context, uint32_t& storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool storeToSegment(SemanticContext* context, uint32_t storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool collectLiteralsToSegmentNoLock(SemanticContext* context, uint32_t baseOffset, uint32_t& offset, AstNode* node, DataSegment* segment);
    static bool reserveAndStoreToSegmentNoLock(SemanticContext* context, uint32_t& storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool storeToSegmentNoLock(SemanticContext* context, uint32_t storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool collectStructLiteralsNoLock(SemanticContext* context, SourceFile* sourceFile, uint32_t& offset, AstNode* node, DataSegment* segment);
    static bool matchIdentifierParameters(SemanticContext* context, AstNode* genericParameters, AstNode* callParameters, AstNode* node);
    static bool checkFuncPrototype(SemanticContext* context, AstFuncDecl* node);
    static bool checkIsConcrete(SemanticContext* context, AstNode* node);
    static bool evaluateConstExpression(SemanticContext* context, AstNode* node);
    static bool checkUnreachableCode(SemanticContext* context);
    static bool waitForStructUserOps(SemanticContext* context, AstNode* node);
    static bool convertAssignementToStruct(SemanticContext* context, AstNode* assignment, AstStruct** result);
    static bool convertAssignementToStruct(SemanticContext* context, AstNode* parent, AstNode* assignment, AstNode** result);
    static bool collectAssignment(SemanticContext* context, uint32_t& storageOffset, AstVarDecl* node, DataSegment* seg);
    static void disableCompilerIfBlock(SemanticContext* context, AstCompilerIfBlock* block);

    static bool resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveUnaryOpExclam(SemanticContext* context, AstNode* op);
    static bool resolveUnaryOpMinus(SemanticContext* context, AstNode* op);
    static bool resolveUnaryOp(SemanticContext* context);
    static bool resolveTypeExpression(SemanticContext* context);
    static bool resolveTypeLambda(SemanticContext* context);
    static bool resolveVarDeclAfterAssign(SemanticContext* context);
    static bool resolveVarDecl(SemanticContext* context);
    static bool resolveTypeAlias(SemanticContext* context);
    static bool resolveCountProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicProperty(SemanticContext* context);
    static bool resolveLiteral(SemanticContext* context);
    static bool resolveIndex(SemanticContext* context);
    static bool resolveBreak(SemanticContext* context);
    static bool resolveContinue(SemanticContext* context);
    static bool resolveLabel(SemanticContext* context);
    static bool resolveExpressionListCurly(SemanticContext* context);
    static bool resolveExpressionListArray(SemanticContext* context);
    static bool resolveExplicitNoInit(SemanticContext* context);
    static bool resolveBoolExpression(SemanticContext* context);
    static bool resolveCompareExpression(SemanticContext* context);
    static bool resolveIsExpression(SemanticContext* context);
    static bool resolveFactorExpression(SemanticContext* context);
    static bool resolveShiftExpression(SemanticContext* context);
    static bool resolveCompilerInline(SemanticContext* context);
    static bool resolveCompilerMixin(SemanticContext* context);
    static bool resolveCompilerAssert(SemanticContext* context);
    static bool resolveCompilerPrint(SemanticContext* context);
    static bool resolveCompilerRun(SemanticContext* context);
    static bool resolveCompilerSpecialFunction(SemanticContext* context);
    static bool resolveUserOp(SemanticContext* context, const char* name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right, bool optionnal = false);
    static bool resolveUserOp(SemanticContext* context, const char* name, const char* opConst, TypeInfo* opType, AstNode* left, vector<AstNode*>& params, bool optionnal = false);
    static bool resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveIdentifier(SemanticContext* context);
    static bool resolveIdentifierRef(SemanticContext* context);
    static bool resolveImpl(SemanticContext* context);
    static bool resolveImplFor(SemanticContext* context);
    static bool pickSymbol(SemanticContext* context, AstIdentifier* node, SymbolName** result);
    static bool preResolveStruct(SemanticContext* context);
    static bool resolveStruct(SemanticContext* context);
    static bool resolveInterface(SemanticContext* context);
    static bool resolveEnum(SemanticContext* context);
    static bool resolveEnumType(SemanticContext* context);
    static bool resolveEnumValue(SemanticContext* context);
    static bool resolveFuncDeclParams(SemanticContext* context);
    static bool resolveFuncDecl(SemanticContext* context);
    static bool setFullResolve(SemanticContext* context, AstFuncDecl* funcNode);
    static bool resolveFuncDeclType(SemanticContext* context);
    static bool registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode, uint32_t symbolFlags = 0);
    static bool resolveFuncCallParams(SemanticContext* context);
    static bool resolveFuncCallParam(SemanticContext* context);
    static bool resolveAttrDecl(SemanticContext* context);
    static bool resolveAttrUse(SemanticContext* context);
    static bool resolveReturn(SemanticContext* context);
    static bool resolveNamespace(SemanticContext* context);
    static bool resolveUsingVar(SemanticContext* context, AstNode* varNode, TypeInfo* typeInfoVar);
    static bool resolveUsing(SemanticContext* context);
    static bool resolveBinaryOpModulo(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBitmaskOr(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBitmaskAnd(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveShiftLeft(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveShiftRight(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveXor(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveTilde(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveUnaryOpInvert(SemanticContext* context, AstNode* op);
    static bool resolveExplicitCast(SemanticContext* context);
    static bool resolveExplicitAutoCast(SemanticContext* context);
    static bool resolveCompilerIf(SemanticContext* context);
    static bool resolveIf(SemanticContext* context);
    static bool resolveWhile(SemanticContext* context);
    static bool resolveLoopBefore(SemanticContext* context);
    static bool resolveInlineBefore(SemanticContext* context);
    static bool resolveInlineAfter(SemanticContext* context);
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
    static bool resolveInit(SemanticContext* context);
    static bool resolveDrop(SemanticContext* context);
    static bool resolveUserCast(SemanticContext* context);

    vector<AstNode*>         tmpNodes;
    vector<SymbolName*>      cacheDependentSymbols;
    vector<Scope*>           cacheScopeHierarchy;
    vector<AlternativeScope> cacheScopeHierarchyVars;
    set<Scope*>              scopesHere;
    vector<OneMatch>         cacheMatches;
    vector<OneGenericMatch>  cacheGenericMatches;
    vector<SymbolOverload*>  cacheBadSignature;
    vector<SymbolOverload*>  cacheBadGenericSignature;
    SymbolMatchContext       symMatch;
    SemanticContext          context;
};

extern Pool<SemanticJob> g_Pool_semanticJob;