#pragma once
#include "Job.h"
#include "Utf8.h"
#include "TypeInfo.h"
#include "Diagnostic.h"
#include "Scope.h"
#include "Concat.h"
struct AstNode;
struct Module;
struct SourceFile;
struct SemanticJob;
struct TypeInfoFuncAttr;
struct SymbolName;
struct AstIdentifierRef;
struct SymbolOverload;
struct AstAttrUse;
enum class AstNodeKind : uint8_t;
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

struct OneOverload
{
    SymbolOverload* overload;
    uint32_t        cptOverloads;
};

struct OneTryMatch
{
    SymbolMatchContext symMatchContext;
    SymbolOverload*    overload          = nullptr;
    AstNode*           dependentVar      = nullptr;
    AstNode*           callParameters    = nullptr;
    AstNode*           genericParameters = nullptr;
    uint32_t           cptOverloads      = 0;
    bool               ufcs              = false;
};

struct OneMatch
{
    VectorNative<TypeInfoParam*> solvedParameters;

    SymbolName*     symbolName     = nullptr;
    SymbolOverload* symbolOverload = nullptr;
    AstNode*        dependentVar   = nullptr;
    bool            ufcs           = false;
    bool            remove         = false;
};

struct OneGenericMatch
{
    VectorNative<TypeInfo*> genericParametersCallTypes;
    VectorNative<TypeInfo*> genericParametersGenTypes;
    map<Utf8Crc, TypeInfo*> genericReplaceTypes;

    SymbolName*     symbolName        = nullptr;
    SymbolOverload* symbolOverload    = nullptr;
    AstNode*        genericParameters = nullptr;

    uint32_t matchNumOverloadsWhenChecked = 0;
    uint32_t flags                        = 0;
};

static const uint32_t COLLECT_ALL         = 0x00000000;
static const uint32_t COLLECT_PASS_INLINE = 0x00000001;
static const uint32_t COLLECT_NO_STRUCT   = 0x00000002;

struct SemanticJob : public Job
{
    SemanticJob()
    {
        affinity = AFFINITY_ALL ^ AFFINITY_EXECBC;
    }

    JobResult execute() override;

    static SemanticJob* newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* rootNode, bool run);
    static bool         error(SemanticContext* context, const Utf8& msg);
    static bool         internalError(JobContext* context, const char* msg, AstNode* node = nullptr);
    static bool         checkTypeIsNative(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo);
    static bool         checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool         notAllowed(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static void         decreaseInterfaceCount(TypeInfoStruct* typeInfoStruct);
    static void         decreaseMethodCount(TypeInfoStruct* typeInfoStruct);
    static SymbolName*  hasUserOp(const char* name, TypeInfoStruct* leftStruct);
    static SymbolName*  hasUserOp(SemanticContext* context, const char* name, AstNode* left);
    static SymbolName*  waitUserOp(SemanticContext* context, const char* name, AstNode* left);

    static void enterState(AstNode* node);
    static void propagateAttributes(AstNode* child);
    static bool checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode);
    static bool collectAttributes(SemanticContext* context, AstNode* forNode, SymbolAttributes& result);
    static bool collectAttributes(SemanticContext* context, AstNode* forNode, SymbolAttributes& result, AstAttrUse* attrUse);
    static void collectAlternativeScopeHierarchy(SemanticContext* context, set<Scope*>& scopes, VectorNative<AlternativeScope>& scopesVars, AstNode* startNode);
    static bool collectScopeHierarchy(SemanticContext* context, set<Scope*>& scopes, VectorNative<AlternativeScope>& scopesVars, AstNode* startNode, uint32_t flags = COLLECT_ALL);
    static bool setupIdentifierRef(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool derefConstantValue(SemanticContext* context, AstNode* node, TypeInfoKind kind, NativeTypeKind nativeKind, void* ptr);
    static bool derefLiteralStruct(SemanticContext* context, AstIdentifierRef* parent, SymbolOverload* overload, DataSegment* segment);
    static bool makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* identifier);
    static bool makeInline(SemanticContext* context, AstFuncDecl* funcDecl, AstNode* identifier);
    static void sortParameters(AstNode* allParams);
    static void dealWithIntrinsic(SemanticContext* context, AstIdentifier* identifier);
    static bool createTmpVarStruct(SemanticContext* context, AstIdentifier* identifier);
    static bool setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstIdentifier* identifier, OneMatch& oneMatch);
    static void resolvePendingLambdaTyping(AstFuncCallParam* nodeCall, OneMatch* oneMatch, int i);
    static bool checkSymbolGhosting(SemanticContext* context, AstNode* node, SymbolKind kind);
    static bool setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, AstNode* funcAttr, AstNode* parameters, bool forGenerics);
    static bool executeNode(SemanticContext* context, AstNode* node, bool onlyconstExpr);
    static bool reserveAndStoreToSegment(JobContext* context, uint32_t& storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool storeToSegment(SemanticContext* context, uint32_t storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool collectLiteralsToSegmentNoLock(JobContext* context, uint32_t baseOffset, uint32_t& offset, AstNode* node, DataSegment* segment);
    static bool reserveAndStoreToSegmentNoLock(JobContext* context, uint32_t& storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool storeToSegmentNoLock(JobContext* context, uint32_t storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool collectStructLiteralsNoLock(JobContext* context, SourceFile* sourceFile, uint32_t& offset, AstNode* node, DataSegment* segment);
    static void setupContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, SymbolOverload* symOverload);
    static void getDiagnosticForMatch(SemanticContext* context, OneTryMatch& oneTry, vector<const Diagnostic*>& result0, vector<const Diagnostic*>& result1);
    static bool cannotMatchIdentifierError(SemanticContext* context, vector<OneTryMatch>& overloads, AstNode* node);
    static bool matchIdentifierParameters(SemanticContext* context, vector<OneTryMatch>& overloads, AstNode* node);
    static bool checkFuncPrototype(SemanticContext* context, AstFuncDecl* node);
    static bool checkFuncPrototypeOpNumParams(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t num, bool exact = true);
    static bool checkFuncPrototypeOpReturnType(SemanticContext* context, AstFuncDecl* node, TypeInfo* wanted);
    static bool checkFuncPrototypeOpParam(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t index, TypeInfo* wanted);
    static bool checkFuncPrototypeOp(SemanticContext* context, AstFuncDecl* node);
    static bool checkFuncPrototypeProperty(SemanticContext* context, AstFuncDecl* node);
    static bool checkIsConcrete(SemanticContext* context, AstNode* node);
    static bool checkIsConcreteOrType(SemanticContext* context, AstNode* node);
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
    static void forceConstType(SemanticContext* context, AstTypeExpression* node);
    static void forceConstNode(SemanticContext* context, AstNode* node);
    static bool resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo** resultTypeInfo);
    static bool resolveType(SemanticContext* context);
    static bool resolveTypeLambda(SemanticContext* context);
    static bool resolveVarDeclBeforeAssign(SemanticContext* context);
    static bool resolveVarDeclAfterAssign(SemanticContext* context);
    static bool convertTypeListToArray(SemanticContext* context, AstVarDecl* node, bool isCompilerConstant, uint32_t symbolFlags);
    static bool resolveVarDecl(SemanticContext* context);
    static bool resolveTypeAlias(SemanticContext* context);
    static bool resolveIntrinsicMakeAny(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicMakeSlice(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicMakeInterface(SemanticContext* context);
    static bool resolveIntrinsicDataOf(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicCountOf(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicKindOf(SemanticContext* context);
    static bool makeIntrinsicTypeOf(SemanticContext* context);
    static bool resolveIntrinsicTypeOf(SemanticContext* context);
    static bool resolveIntrinsicProperty(SemanticContext* context);
    static bool resolveLiteral(SemanticContext* context);
    static bool resolveIndex(SemanticContext* context);
    static bool resolveBreak(SemanticContext* context);
    static bool resolveFallThrough(SemanticContext* context);
    static bool resolveContinue(SemanticContext* context);
    static bool resolveLabel(SemanticContext* context);
    static bool resolveExpressionListTuple(SemanticContext* context);
    static bool resolveExpressionListArray(SemanticContext* context);
    static bool resolveExplicitNoInit(SemanticContext* context);
    static bool resolveBoolExpression(SemanticContext* context);
    static bool resolveCompareExpression(SemanticContext* context);
    static bool resolveFactorExpression(SemanticContext* context);
    static bool resolveShiftExpression(SemanticContext* context);
    static bool resolveCompilerMacro(SemanticContext* context);
    static bool resolveCompilerInline(SemanticContext* context);
    static bool resolveCompilerMixin(SemanticContext* context);
    static bool resolveCompilerAssert(SemanticContext* context);
    static bool resolveCompilerPrint(SemanticContext* context);
    static bool resolveCompilerForeignLib(SemanticContext* context);
    static bool resolveCompilerRun(SemanticContext* context);
    static bool preResolveCompilerInstruction(SemanticContext* context);
    static bool resolveCompilerAstExpression(SemanticContext* context);
    static bool resolveCompilerSpecialFunction(SemanticContext* context);
    static bool resolveCompilerDefined(SemanticContext* context);
    static bool resolveCompilerLoad(SemanticContext* context);
    static bool resolveUserOp(SemanticContext* context, const char* name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right, bool optionnal);
    static bool resolveUserOp(SemanticContext* context, const char* name, const char* opConst, TypeInfo* opType, AstNode* left, VectorNative<AstNode*>& params, bool optionnal);
    static bool resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOp3Way(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right);
    static bool getUsingVar(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** result);
    static bool getUfcs(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** ufcsFirstParam, AstNode** ufcsLastParam);
    static bool appendLastCodeStatement(SemanticContext* context, AstIdentifier* node, SymbolOverload* overload);
    static bool fillMatchContextCallParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload, AstNode* ufcsFirstParam, AstNode* ufcsLastParam);
    static bool fillMatchContextGenericParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload);
    static bool resolveIdentifier(SemanticContext* context);
    static bool findIdentifierInScopes(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node);
    static bool ufcsSetLastParam(SemanticContext* context, AstIdentifierRef* identifierRef, SymbolName* symbol);
    static bool ufcsSetFirstParam(SemanticContext* context, AstIdentifierRef* identifierRef, OneMatch& match);
    static bool resolveIdentifierRef(SemanticContext* context);
    static bool CheckImplScopes(SemanticContext* context, AstImpl* node, Scope* scopeImpl, Scope* scope);
    static bool resolveImpl(SemanticContext* context);
    static bool resolveImplFor(SemanticContext* context);
    static bool instantiateGenericSymbol(SemanticContext* context, OneGenericMatch& firstMatch, bool forStruct);
    static bool filterMatches(SemanticContext* context, vector<OneMatch>& matches);
    static bool filterSymbols(SemanticContext* context, AstIdentifier* node);
    static bool preResolveStruct(SemanticContext* context);
    static void flattenStructChilds(SemanticContext* context, AstNode* parent, VectorNative<AstNode*>& result);
    static bool resolveStruct(SemanticContext* context);
    static bool resolveInterface(SemanticContext* context);
    static bool resolveTypeSet(SemanticContext* context);
    static bool resolveEnum(SemanticContext* context);
    static bool resolveEnumType(SemanticContext* context);
    static bool resolveEnumValue(SemanticContext* context);
    static bool resolveFuncDeclParams(SemanticContext* context);
    static bool resolveAfterFuncDecl(SemanticContext* context);
    static bool resolveFuncDecl(SemanticContext* context);
    static bool setFullResolve(SemanticContext* context, AstFuncDecl* funcNode);
    static bool resolveFuncDeclType(SemanticContext* context);
    static bool registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode, uint32_t symbolFlags = 0);
    static bool resolveFuncCallParams(SemanticContext* context);
    static bool resolveFuncCallParam(SemanticContext* context);
    static bool preResolveAttrDecl(SemanticContext* context);
    static bool resolveAttrDecl(SemanticContext* context);
    static bool resolveAttrUse(SemanticContext* context);
    static bool resolveReturn(SemanticContext* context);
    static bool resolveRetVal(SemanticContext* context);
    static bool resolveNamespace(SemanticContext* context);
    static bool resolveUsingVar(SemanticContext* context, AstNode* varNode, TypeInfo* typeInfoVar);
    static bool checkPublicAlias(SemanticContext* context, AstNode* node);
    static bool resolveAlias(SemanticContext* context);
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
    static bool resolveExplicitBitCast(SemanticContext* context);
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
    static bool resolveVisit(SemanticContext* context);
    static bool resolveAffect(SemanticContext* context);
    static bool resolveMove(SemanticContext* context);
    static bool resolveMakePointer(SemanticContext* context);
    static bool resolveScopedStmtBefore(SemanticContext* context);
    static bool boundCheck(SemanticContext* context, AstNode* arrayAccess, uint32_t maxCount);
    static bool resolveArrayPointerDeRef(SemanticContext* context);
    static bool resolveArrayPointerSlicing(SemanticContext* context);
    static bool resolveArrayPointerIndex(SemanticContext* context);
    static bool resolveArrayPointerRef(SemanticContext* context);
    static bool resolveTypeList(SemanticContext* context);
    static bool resolveConditionalOp(SemanticContext* context);
    static bool resolveDefer(SemanticContext* context);
    static bool resolveNullConditionalOp(SemanticContext* context);
    static bool resolveInit(SemanticContext* context);
    static bool resolveDropCopyMove(SemanticContext* context);
    static bool resolveUserCast(SemanticContext* context);

    VectorNative<AstNode*>         tmpNodes;
    set<SymbolName*>               cacheDependentSymbols;
    set<Scope*>                    cacheScopeHierarchy;
    VectorNative<AlternativeScope> cacheScopeHierarchyVars;
    VectorNative<Scope*>           scopesHere;
    vector<OneMatch>               cacheMatches;
    vector<OneGenericMatch>        cacheGenericMatches;
    SemanticContext                context;
    Concat                         tmpConcat;

    MatchResult       bestMatchResult;
    BadSignatureInfos bestSignatureInfos;
    SymbolOverload*   bestOverload;

    void reset() override
    {
        Job::reset();
        tmpNodes.clear();
        cacheDependentSymbols.clear();
        cacheScopeHierarchy.clear();
        cacheScopeHierarchyVars.clear();
        scopesHere.clear();
        cacheMatches.clear();
        cacheGenericMatches.clear();
        context.reset();
    }

    void release() override
    {
        extern thread_local Pool<SemanticJob> g_Pool_semanticJob;
        g_Pool_semanticJob.release(this);
    }
};

extern thread_local Pool<SemanticJob> g_Pool_semanticJob;
