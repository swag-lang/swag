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
struct AttributeList;
struct ByteCodeGenJob;
struct AstIdentifier;
struct AstFuncDecl;
struct DataSegmentLocation;
struct DataSegment;
struct SymbolName;

struct SemanticContext : public JobContext
{
    SemanticJob*              job = nullptr;
    VectorNative<SymbolName*> currentLockedSymbol;
};

struct LockSymbolOncePerContext
{
    LockSymbolOncePerContext(SemanticContext* context, SymbolName* sym)
    {
        if (context->currentLockedSymbol.contains(sym))
            return;
        savedContext = context;
        savedSymbol  = sym;
        context->currentLockedSymbol.push_back(sym);
        sym->mutex.lock();
    }

    ~LockSymbolOncePerContext()
    {
        if (!savedContext)
            return;
        savedSymbol->mutex.unlock();
        savedContext->currentLockedSymbol.erase_unordered_byval(savedSymbol);
    }

    SemanticContext* savedContext = nullptr;
    SymbolName*      savedSymbol  = nullptr;
};

struct OneOverload
{
    SymbolOverload* overload;
    Scope*          scope;
    uint32_t        cptOverloads;
    uint32_t        cptOverloadsInit;
};

struct OneTryMatch
{
    SymbolMatchContext symMatchContext;
    SymbolOverload*    overload          = nullptr;
    Scope*             scope             = nullptr;
    AstNode*           dependentVar      = nullptr;
    AstNode*           callParameters    = nullptr;
    AstNode*           genericParameters = nullptr;
    uint32_t           cptOverloads      = 0;
    uint32_t           cptOverloadsInit  = 0;
    bool               ufcs              = false;

    void reset()
    {
        symMatchContext.reset();
        overload          = nullptr;
        scope             = nullptr;
        dependentVar      = nullptr;
        callParameters    = nullptr;
        genericParameters = nullptr;
        cptOverloads      = 0;
        ufcs              = false;
    }
};

struct OneMatch
{
    VectorNative<TypeInfoParam*> solvedParameters;

    SymbolOverload* symbolOverload = nullptr;
    Scope*          scope          = nullptr;
    AstNode*        dependentVar   = nullptr;
    OneTryMatch*    oneOverload    = nullptr;

    bool ufcs   = false;
    bool remove = false;

    void reset()
    {
        solvedParameters.clear();
        symbolOverload = nullptr;
        dependentVar   = nullptr;
        ufcs           = false;
        remove         = false;
    }
};

struct OneGenericMatch
{
    VectorNative<TypeInfo*> genericParametersCallTypes;
    VectorNative<TypeInfo*> genericParametersGenTypes;
    map<Utf8, TypeInfo*>    genericReplaceTypes;

    SymbolName*     symbolName        = nullptr;
    SymbolOverload* symbolOverload    = nullptr;
    AstNode*        genericParameters = nullptr;

    uint32_t numOverloadsWhenChecked     = 0;
    uint32_t numOverloadsInitWhenChecked = 0;
    uint32_t flags                       = 0;

    void reset()
    {
        genericParametersCallTypes.clear();
        genericParametersGenTypes.clear();
        genericReplaceTypes.clear();
        symbolName              = nullptr;
        symbolOverload          = nullptr;
        genericParameters       = nullptr;
        numOverloadsWhenChecked = 0;
        flags                   = 0;
    }
};

static const uint32_t COLLECT_ALL       = 0x00000000;
static const uint32_t COLLECT_BACKTICK  = 0x00000001;
static const uint32_t COLLECT_NO_STRUCT = 0x00000002;

struct SemanticJob : public Job
{
    JobResult execute() override;

    void release() override
    {
        g_Allocator.free<SemanticJob>(this);
    }

    static bool checkTypeIsNative(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo);
    static bool checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool checkFuncPrototype(SemanticContext* context, AstFuncDecl* node);
    static bool checkFuncPrototypeOpNumParams(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t num, bool exact = true);
    static bool checkFuncPrototypeOpReturnType(SemanticContext* context, AstFuncDecl* node, TypeInfo* wanted);
    static bool checkFuncPrototypeOpParam(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t index, TypeInfo* wanted);
    static bool checkFuncPrototypeOp(SemanticContext* context, AstFuncDecl* node);
    static bool checkIsConcrete(SemanticContext* context, AstNode* node);
    static bool checkCanMakeFuncPointer(SemanticContext* context, AstFuncDecl* funcNode, AstNode* node);
    static bool checkCanTakeAddress(SemanticContext* context, AstNode* node);
    static bool checkIsConcreteOrType(SemanticContext* context, AstNode* node);
    static bool checkPublicAlias(SemanticContext* context, AstNode* node);
    static bool checkUnreachableCode(SemanticContext* context);
    static bool checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode);
    static void checkDeprecated(SemanticContext* context, AstNode* identifier);
    static Utf8 checkLiteralType(ComputedValue& computedValue, Token& token, TypeInfo* typeSuffix, bool negApplied);
    static bool checkCanThrow(SemanticContext* context);
    static bool checkCanCatch(SemanticContext* context);
    static bool checkImplScopes(SemanticContext* context, AstImpl* node, Scope* scopeImpl, Scope* scope);
    static void checkCaninstantiateGenericSymbol(SemanticContext* context, OneGenericMatch& firstMatch);

    static SemanticJob* newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* rootNode, bool run);
    static bool         error(SemanticContext* context, const Utf8& msg);
    static bool         notAllowed(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static void         decreaseInterfaceCount(TypeInfoStruct* typeInfoStruct);
    static void         decreaseMethodCount(AstFuncDecl* funcNode, TypeInfoStruct* typeInfoStruct);
    static SymbolName*  hasUserOp(const Utf8& name, TypeInfoStruct* leftStruct);
    static SymbolName*  hasUserOp(SemanticContext* context, const Utf8& name, AstNode* left);
    static SymbolName*  waitUserOp(SemanticContext* context, const Utf8& name, AstNode* left);
    static uint32_t     alignOf(AstVarDecl* node);
    static bool         isCompilerContext(AstNode* node);
    static DataSegment* getConstantSegFromContext(AstNode* node, bool forceCompiler = false);
    static void         enterState(AstNode* node);
    static void         inheritAttributesFromParent(AstNode* child);
    static void         inheritAttributesFromOwnerFunc(AstNode* child);
    static bool         collectAttributes(SemanticContext* context, AstNode* forNode, AttributeList* result);
    static bool         collectAttributes(SemanticContext* context, AstNode* forNode, AttributeList* result, AstAttrUse* attrUse);
    static void         collectAlternativeScopeHierarchy(SemanticContext* context, VectorNative<Scope*>& scopes, VectorNative<AlternativeScope>& scopesVars, AstNode* startNode, uint32_t flags);
    static bool         collectScopeHierarchy(SemanticContext* context, VectorNative<Scope*>& scopes, VectorNative<AlternativeScope>& scopesVars, AstNode* startNode, uint32_t flags = COLLECT_ALL);
    static bool         setupIdentifierRef(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool         derefConstantValue(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, DataSegment* storageSegment, void* ptr);
    static bool         derefConstantValue(SemanticContext* context, AstNode* node, TypeInfoKind kind, NativeTypeKind nativeKind, void* ptr);
    static bool         derefLiteralStruct(SemanticContext* context, uint8_t* ptr, SymbolOverload* overload, DataSegment* storageSegment);
    static bool         derefLiteralStruct(SemanticContext* context, AstIdentifierRef* parent, SymbolOverload* overload);
    static bool         makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* identifier);
    static bool         makeInline(SemanticContext* context, AstFuncDecl* funcDecl, AstNode* identifier);
    static void         sortParameters(AstNode* allParams);
    static void         dealWithIntrinsic(SemanticContext* context, AstIdentifier* identifier);
    static bool         createTmpVarStruct(SemanticContext* context, AstIdentifier* identifier);
    static bool         setSymbolMatchCallParams(SemanticContext* context, AstIdentifier* identifier, OneMatch& oneMatch);
    static bool         setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstIdentifier* identifier, OneMatch& oneMatch);
    static void         resolvePendingLambdaTyping(AstFuncCallParam* nodeCall, OneMatch* oneMatch, int i);
    static bool         setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, AstNode* funcAttr, AstNode* parameters, bool forGenerics);
    static bool         executeCompilerNode(SemanticContext* context, AstNode* node, bool onlyconstExpr);
    static bool         reserveAndStoreToSegment(JobContext* context, DataSegment* storageSegment, uint32_t& storageOffset, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool         storeToSegment(JobContext* context, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool         collectLiteralsToSegment(JobContext* context, DataSegment* storageSegment, uint32_t baseOffset, uint32_t& offset, AstNode* node);
    static bool         collectStructLiterals(JobContext* context, DataSegment* storageSegment, uint32_t offsetStruct, AstNode* node);
    static void         setupContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, SymbolOverload* symOverload);
    static void         getDiagnosticForMatch(SemanticContext* context, OneTryMatch& oneTry, vector<const Diagnostic*>& result0, vector<const Diagnostic*>& result1);
    static void         symbolErrorRemarks(SemanticContext* context, VectorNative<OneTryMatch*>& overloads, AstNode* node, Diagnostic* diag);
    static void         symbolNotFoundHint(SemanticContext* context, AstNode* node, VectorNative<Scope*>& scopeHierarchy, vector<const Diagnostic*>& notes);
    static bool         isFunctionButNotACall(SemanticContext* context, AstNode* node, SymbolName* symbol);
    static void         symbolErrorNotes(SemanticContext* context, VectorNative<OneTryMatch*>& overloads, AstNode* node, Diagnostic* diag, vector<const Diagnostic*>& notes);
    static bool         cannotMatchIdentifierError(SemanticContext* context, VectorNative<OneTryMatch*>& overloads, AstNode* node);
    static bool         matchIdentifierParameters(SemanticContext* context, VectorNative<OneTryMatch*>& overloads, AstNode* node, bool justCheck = false, bool forGhosting = false);
    static bool         evaluateConstExpression(SemanticContext* context, AstNode* node);
    static bool         evaluateConstExpression(SemanticContext* context, AstNode* node1, AstNode* node2);
    static bool         evaluateConstExpression(SemanticContext* context, AstNode* node1, AstNode* node2, AstNode* node3);
    static bool         waitForStructUserOps(SemanticContext* context, AstNode* node);
    static AstNode*     convertTypeToTypeExpression(SemanticContext* context, AstNode* parent, AstNode* assignment, TypeInfo* childType);
    static bool         convertLiteralTupleToStructDecl(SemanticContext* context, AstNode* assignment, AstStruct** result);
    static bool         convertLiteralTupleToStructDecl(SemanticContext* context, AstNode* parent, AstNode* assignment, AstNode** result);
    static bool         collectAssignment(SemanticContext* context, DataSegment* storageSegment, uint32_t& storageOffset, AstVarDecl* node);
    static void         disableCompilerIfBlock(SemanticContext* context, AstCompilerIfBlock* block);
    static void         propagateReturn(AstReturn* node);
    static bool         resolveMakePointer(SemanticContext* context);
    static bool         boundCheck(SemanticContext* context, AstNode* arrayAccess, uint64_t maxCount);
    static bool         getConstantArrayPtr(SemanticContext* context, uint32_t* storageOffset, DataSegment** storageSegment);
    static void         forceConstType(SemanticContext* context, AstTypeExpression* node);
    static void         setVarDeclResolve(AstVarDecl* varNode);
    static bool         convertTypeListToArray(SemanticContext* context, AstVarDecl* node, bool isCompilerConstant, uint32_t symbolFlags);
    static DataSegment* getSegmentForVar(SemanticContext* context, AstVarDecl* node);
    static bool         makeIntrinsicTypeOf(SemanticContext* context);
    static bool         getDigitHexa(SemanticContext* context, const char** pz, int& result, const char* errMsg);
    static bool         processLiteralString(SemanticContext* context);
    static bool         computeExpressionListTupleType(SemanticContext* context, AstNode* node);
    static bool         preResolveCompilerInstruction(SemanticContext* context);
    static bool         getUsingVar(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** result);
    static bool         canTryUfcs(SemanticContext* context, TypeInfoFuncAttr* typeFunc, AstFuncCallParams* parameters, AstNode* ufcsNode, bool nodeIsExplicit);
    static bool         getUfcs(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** ufcsFirstParam);
    static bool         appendLastCodeStatement(SemanticContext* context, AstIdentifier* node, SymbolOverload* overload);
    static bool         fillMatchContextCallParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload, AstNode* ufcsFirstParam);
    static bool         fillMatchContextGenericParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload);
    static bool         needToWaitForSymbol(SemanticContext* context, AstIdentifier* node, SymbolName* symbol, bool& needToWait);
    static bool         resolveIdentifier(SemanticContext* context, AstIdentifier* node, bool forGhosting);
    static TypeInfo*    findTypeInContext(SemanticContext* context, AstNode* node);
    static bool         findIdentifierInScopes(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node);
    static bool         ufcsSetFirstParam(SemanticContext* context, AstIdentifierRef* identifierRef, OneMatch& match);
    static bool         instantiateGenericSymbol(SemanticContext* context, OneGenericMatch& firstMatch, bool forStruct);
    static bool         filterGenericMatches(SemanticContext* context, VectorNative<OneGenericMatch*>& matches);
    static bool         filterMatchesInContext(SemanticContext* context, VectorNative<OneMatch*>& matches);
    static bool         solveSelectIf(SemanticContext* context, OneMatch* oneMatch, AstFuncDecl* funcDecl);
    static bool         filterMatches(SemanticContext* context, VectorNative<OneMatch*>& matches);
    static bool         filterSymbols(SemanticContext* context, AstIdentifier* node);
    static bool         preResolveGeneratedStruct(SemanticContext* context);
    static bool         preResolveStructContent(SemanticContext* context);
    static void         flattenStructChilds(SemanticContext* context, AstNode* parent, VectorNative<AstNode*>& result);
    static bool         setFullResolve(SemanticContext* context, AstFuncDecl* funcNode);
    static void         setFuncDeclParamsIndex(AstFuncDecl* funcNode);
    static bool         isMethod(AstFuncDecl* funcNode);
    static void         launchResolveSubDecl(JobContext* context, AstNode* node);
    static bool         preResolveAttrDecl(SemanticContext* context);
    static bool         registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode, uint32_t symbolFlags = 0);
    static void         resolveSubDecls(JobContext* context, AstFuncDecl* funcNode);
    AstNode*            backToSemError();

    static bool resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveUnaryOpExclam(SemanticContext* context, AstNode* child);
    static bool resolveUnaryOpMinus(SemanticContext* context, AstNode* child);
    static bool resolveUnaryOpInvert(SemanticContext* context, AstNode* child);
    static bool resolveUnaryOp(SemanticContext* context);
    static bool resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, TypeInfo** resultTypeInfo, uint32_t flags = 0);
    static bool resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo** resultTypeInfo, uint32_t flags = 0);
    static bool resolveType(SemanticContext* context);
    static bool resolveTypeLambda(SemanticContext* context);
    static bool resolveVarDeclAfterType(SemanticContext* context);
    static bool resolveVarDeclBefore(SemanticContext* context);
    static bool resolveVarDeclAfter(SemanticContext* context);
    static bool resolveVarDeclAfterAssign(SemanticContext* context);
    static bool resolveVarDecl(SemanticContext* context);
    static bool resolveTypeAliasBefore(SemanticContext* context);
    static bool resolveTypeAlias(SemanticContext* context);
    static bool resolveIntrinsicMakeAny(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicMakeCallback(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicMakeForeign(SemanticContext* context);
    static bool resolveIntrinsicMakeSlice(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, const char* name);
    static bool resolveIntrinsicMakeInterface(SemanticContext* context);
    static bool resolveIntrinsicDataOf(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicCountOf(SemanticContext* context, AstNode* node, AstNode* expression);
    static bool resolveIntrinsicStringOf(SemanticContext* context);
    static bool resolveIntrinsicSpread(SemanticContext* context);
    static bool resolveIntrinsicKindOf(SemanticContext* context);
    static bool resolveLiteralSuffix(SemanticContext* context);
    static bool resolveIntrinsicTypeOf(SemanticContext* context);
    static bool resolveIntrinsicProperty(SemanticContext* context);
    static bool resolveLiteral(SemanticContext* context);
    static bool resolveGetErr(SemanticContext* context);
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
    static bool resolveCompilerTestError(SemanticContext* context);
    static bool resolveCompilerForeignLib(SemanticContext* context);
    static bool resolveCompilerRun(SemanticContext* context);
    static bool resolveCompilerSelectIfExpression(SemanticContext* context);
    static bool resolveCompilerAstExpression(SemanticContext* context);
    static bool resolveCompilerSpecialFunction(SemanticContext* context);
    static bool resolveCompilerDefined(SemanticContext* context);
    static bool resolveCompilerScopeFct(SemanticContext* context);
    static bool resolveCompilerLoad(SemanticContext* context);
    static bool resolveUserOpCommutative(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right);
    static bool resolveUserOp(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right, bool justCheck);
    static bool resolveUserOp(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, VectorNative<AstNode*>& params, bool justCheck);
    static bool resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOp3Way(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveIdentifier(SemanticContext* context);
    static bool resolveIdentifierRef(SemanticContext* context);
    static bool resolveTryBlock(SemanticContext* context);
    static bool resolveTry(SemanticContext* context);
    static bool resolveAssume(SemanticContext* context);
    static bool resolveCatch(SemanticContext* context);
    static bool resolveThrow(SemanticContext* context);
    static bool resolveImpl(SemanticContext* context);
    static bool resolveImplForAfterFor(SemanticContext* context);
    static bool resolveImplForType(SemanticContext* context);
    static bool resolveImplFor(SemanticContext* context);
    static bool resolveStruct(SemanticContext* context);
    static bool resolveInterface(SemanticContext* context);
    static bool resolveEnum(SemanticContext* context);
    static bool resolveEnumType(SemanticContext* context);
    static bool resolveEnumValue(SemanticContext* context);
    static bool resolveFuncDeclParams(SemanticContext* context);
    static bool resolveAfterFuncDecl(SemanticContext* context);
    static bool resolveFuncDeclAfterSI(SemanticContext* context);
    static bool resolveFuncDecl(SemanticContext* context);
    static bool resolveFuncDeclType(SemanticContext* context);
    static bool resolveFuncCallParams(SemanticContext* context);
    static bool resolveFuncCallParam(SemanticContext* context);
    static bool resolveAttrDecl(SemanticContext* context);
    static bool resolveAttrUse(SemanticContext* context);
    static bool resolveReturn(SemanticContext* context);
    static bool resolveRetVal(SemanticContext* context);
    static bool resolveUsingVar(SemanticContext* context, AstNode* varNode, TypeInfo* typeInfoVar);
    static bool resolveAlias(SemanticContext* context);
    static bool resolveUsing(SemanticContext* context);
    static bool resolveBinaryOpModulo(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBitmaskOr(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBitmaskAnd(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveShiftLeft(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveShiftRight(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveXor(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveTilde(SemanticContext* context, AstNode* left, AstNode* right);
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
    static bool resolveSwitchAfterExpr(SemanticContext* context);
    static bool resolveSwitch(SemanticContext* context);
    static bool resolveCase(SemanticContext* context);
    static bool resolveLoop(SemanticContext* context);
    static bool resolveVisit(SemanticContext* context);
    static bool resolveAffect(SemanticContext* context);
    static bool resolveMove(SemanticContext* context);
    static bool resolveRange(SemanticContext* context);
    static bool resolveScopedStmtBefore(SemanticContext* context);
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
    static bool resolveTupleUnpackBefore(SemanticContext* context);

    void clearTryMatch()
    {
        for (auto p : cacheListTryMatch)
            cacheFreeTryMatch.push_back(p);
        cacheListTryMatch.clear();
    }

    OneTryMatch* getTryMatch()
    {
        if (cacheFreeTryMatch.empty())
            return g_Allocator.alloc<OneTryMatch>();
        auto res = cacheFreeTryMatch.back();
        cacheFreeTryMatch.pop_back();
        res->reset();
        return res;
    }

    void clearMatch()
    {
        for (auto p : cacheMatches)
            cacheFreeMatches.push_back(p);
        cacheMatches.clear();
    }

    OneMatch* getOneMatch()
    {
        if (cacheFreeMatches.empty())
            return g_Allocator.alloc<OneMatch>();
        auto res = cacheFreeMatches.back();
        cacheFreeMatches.pop_back();
        res->reset();
        return res;
    }

    void clearGenericMatch()
    {
        for (auto p : cacheGenericMatches)
            cacheFreeGenericMatches.push_back(p);
        cacheGenericMatches.clear();
        cacheGenericMatchesSI.clear();
    }

    OneGenericMatch* getOneGenericMatch()
    {
        if (cacheFreeGenericMatches.empty())
            return g_Allocator.alloc<OneGenericMatch>();
        auto res = cacheFreeGenericMatches.back();
        cacheFreeGenericMatches.pop_back();
        res->reset();
        return res;
    }

    struct OneSymbolMatch
    {
        SymbolName* first;
        Scope*      scope;
        bool        operator==(const OneSymbolMatch& other)
        {
            return first == other.first;
        }
    };

    VectorNative<AstNode*>         tmpNodes;
    VectorNative<OneSymbolMatch>   cacheDependentSymbols;
    VectorNative<OneSymbolMatch>   cacheToAddSymbols;
    VectorNative<Scope*>           cacheScopeHierarchy;
    VectorNative<AlternativeScope> cacheScopeHierarchyVars;
    VectorNative<Scope*>           scopesToProcess;
    VectorNative<OneOverload>      cacheToSolveOverload;
    VectorNative<OneMatch*>        cacheMatches;
    VectorNative<OneMatch*>        cacheFreeMatches;
    VectorNative<OneGenericMatch*> cacheGenericMatches;
    VectorNative<OneGenericMatch*> cacheGenericMatchesSI;
    VectorNative<OneGenericMatch*> cacheFreeGenericMatches;
    VectorNative<OneTryMatch*>     cacheListTryMatch;
    VectorNative<OneTryMatch*>     cacheFreeTryMatch;
    SemanticContext                context;
    Concat                         tmpConcat;
    AstIdentifierRef*              tmpIdRef = nullptr;

    MatchResult       bestMatchResult;
    BadSignatureInfos bestSignatureInfos;
    SymbolOverload*   bestOverload = nullptr;
    bool              canSpawn     = false;
};
