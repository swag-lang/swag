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
enum class SymbolKind : uint8_t;
struct AttributeList;
struct ByteCodeGenJob;
struct AstIdentifier;
struct AstFuncDecl;
struct DataSegmentLocation;
struct DataSegment;
struct SymbolName;
struct AstFuncCallParam;

struct CastStructStructField
{
    TypeInfoStruct* typeStruct;
    uint32_t        offset;
    TypeInfoParam*  field;
};

struct CastCollectInterfaceField
{
    TypeInfoStruct* typeStruct;
    uint32_t        offset;
    TypeInfoParam*  field;
    Utf8            fieldAccessName;
};

struct OneTryMatch
{
    SymbolMatchContext symMatchContext;
    SymbolOverload*    overload          = nullptr;
    Scope*             scope             = nullptr;
    AstNode*           dependentVar      = nullptr;
    AstNode*           dependentVarLeaf  = nullptr;
    AstNode*           callParameters    = nullptr;
    AstNode*           genericParameters = nullptr;
    uint32_t           cptOverloads      = 0;
    uint32_t           cptOverloadsInit  = 0;
    bool               ufcs              = false;
    bool               secondTry         = false;

    void reset()
    {
        symMatchContext.reset();
        overload          = nullptr;
        scope             = nullptr;
        dependentVar      = nullptr;
        dependentVarLeaf  = nullptr;
        callParameters    = nullptr;
        genericParameters = nullptr;
        cptOverloads      = 0;
        ufcs              = false;
        secondTry         = false;
    }
};

struct OneMatch
{
    struct ParamParameter
    {
        AstFuncCallParam* param;
        int               indexParam;
        TypeInfoParam*    resolvedParameter;
    };

    VectorNative<TypeInfoParam*> solvedParameters;
    VectorNative<ParamParameter> paramParameters;

    SymbolOverload* symbolOverload = nullptr;
    Scope*          scope          = nullptr;
    AstNode*        dependentVar   = nullptr;
    OneTryMatch*    oneOverload    = nullptr;
    TypeInfo*       typeWasForced  = nullptr;

    uint32_t flags = 0;

    bool ufcs       = false;
    bool autoOpCast = false;
    bool secondTry  = false;
    bool remove     = false;

    void reset()
    {
        solvedParameters.clear();
        paramParameters.clear();
        symbolOverload = nullptr;
        dependentVar   = nullptr;
        typeWasForced  = nullptr;
        flags          = 0;
        ufcs           = false;
        secondTry      = false;
        autoOpCast     = false;
        remove         = false;
    }
};

struct SemanticContext : public JobContext
{
    Vector<CastStructStructField>     castStructStructFields;
    Vector<CastCollectInterfaceField> castCollectInterfaceField;
    SemanticJob*                      job               = nullptr;
    uint32_t                          castFlagsResult   = 0;
    TypeInfo*                         castErrorToType   = nullptr;
    TypeInfo*                         castErrorFromType = nullptr;
    uint64_t                          castErrorFlags    = 0;
    CastErrorType                     castErrorType     = CastErrorType::Zero;
    bool                              forDebugger       = false;
};

struct OneOverload
{
    SymbolOverload* overload;
    Scope*          scope;
    uint32_t        cptOverloads;
    uint32_t        cptOverloadsInit;
};

struct OneGenericMatch
{
    VectorNative<TypeInfo*>         genericParametersCallTypes;
    VectorNative<ComputedValue*>    genericParametersCallValues;
    VectorNative<AstNode*>          genericParametersCallFrom;
    VectorMap<Utf8, TypeInfo*>      genericReplaceTypes;
    VectorMap<Utf8, ComputedValue*> genericReplaceValues;
    VectorMap<Utf8, AstNode*>       genericReplaceFrom;
    VectorNative<TypeInfo*>         genericParametersGenTypes;

    VectorNative<AstNode*>       parameters;
    VectorNative<TypeInfoParam*> solvedParameters;
    SymbolName*                  symbolName        = nullptr;
    SymbolOverload*              symbolOverload    = nullptr;
    AstNode*                     genericParameters = nullptr;

    uint32_t numOverloadsWhenChecked     = 0;
    uint32_t numOverloadsInitWhenChecked = 0;
    uint32_t flags                       = 0;
    bool     secondTry                   = false;

    void reset()
    {
        genericParametersCallTypes.clear();
        genericParametersCallValues.clear();
        genericParametersCallFrom.clear();
        genericReplaceTypes.clear();
        genericReplaceValues.clear();
        genericReplaceFrom.clear();
        genericParametersGenTypes.clear();
        parameters.clear();
        solvedParameters.clear();
        symbolName                  = nullptr;
        symbolOverload              = nullptr;
        genericParameters           = nullptr;
        numOverloadsWhenChecked     = 0;
        numOverloadsInitWhenChecked = 0;
        flags                       = 0;
        secondTry                   = false;
    }
};

struct OneSymbolMatch
{
    SymbolName* symbol  = nullptr;
    Scope*      scope   = nullptr;
    uint32_t    asFlags = 0;
    bool        remove  = false;

    bool operator==(const OneSymbolMatch& other)
    {
        return symbol == other.symbol;
    }
};

enum class IdentifierSearchFor
{
    Whatever,
    Type,
    Function,
    Attribute
};

struct FindUserOp
{
    SymbolName*     symbol;
    TypeInfoStruct* parentStruct;
    TypeInfoParam*  usingField;
};

const uint32_t COLLECT_ALL              = 0x00000000;
const uint32_t COLLECT_NO_STRUCT        = 0x00000001;
const uint32_t COLLECT_NO_INLINE_PARAMS = 0x00000002;

const uint32_t MIP_JUST_CHECK         = 0x00000001;
const uint32_t MIP_FOR_GHOSTING       = 0x00000002;
const uint32_t MIP_FOR_ZERO_GHOSTING  = 0x00000004;
const uint32_t MIP_SECOND_GENERIC_TRY = 0x00000008;

const uint32_t ROP_JUST_CHECK  = 0x00000001;
const uint32_t ROP_SIMPLE_CAST = 0x00000002;

const uint32_t RI_ZERO              = 0x00000000;
const uint32_t RI_FOR_GHOSTING      = 0x00000001;
const uint32_t RI_FOR_ZERO_GHOSTING = 0x00000002;

struct SemanticJob : public Job
{
    JobResult execute() override;

    virtual ~SemanticJob() = default;
    void                release() override;
    static SemanticJob* newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* rootNode, bool run);

    static bool valueEqualsTo(const ComputedValue* value, AstNode* node);
    static bool valueEqualsTo(const ComputedValue* value1, const ComputedValue* value2, TypeInfo* typeInfo, uint64_t flags);
    static bool checkTypeIsNative(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo, AstNode* left, AstNode* right);
    static bool checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool checkFuncPrototype(SemanticContext* context, AstFuncDecl* node);
    static bool checkFuncPrototypeOpNumParams(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t num, bool exact = true);
    static bool checkFuncPrototypeOpReturnType(SemanticContext* context, AstFuncDecl* node, TypeInfo* wanted);
    static bool checkFuncPrototypeOpParam(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t index, TypeInfo* wanted);
    static bool checkFuncPrototypeOp(SemanticContext* context, AstFuncDecl* node);
    static bool checkIsConcrete(SemanticContext* context, AstNode* node);
    static bool checkCanMakeFuncPointer(SemanticContext* context, AstFuncDecl* funcNode, AstNode* node);
    static bool checkCanTakeAddress(SemanticContext* context, AstNode* node);
    static bool checkIsConcreteOrType(SemanticContext* context, AstNode* node, bool typeOnly = false);
    static bool checkPublicAlias(SemanticContext* context, AstNode* node);
    static bool checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode);
    static Utf8 checkLiteralValue(ComputedValue& computedValue, LiteralType& literalType, Register& literalValue, TypeInfo* typeSuffix, bool negApplied);
    static bool checkCanThrow(SemanticContext* context);
    static bool checkCanCatch(SemanticContext* context);
    static bool checkImplScopes(SemanticContext* context, AstImpl* node, Scope* scopeImpl, Scope* scope);
    static void checkCanInstantiateGenericSymbol(SemanticContext* context, OneGenericMatch& firstMatch);
    static bool checkIsConstAffect(SemanticContext* context, AstNode* left, AstNode* right);
    static bool checkAccess(JobContext* context, AstNode* node);
    static bool checkIsConstExpr(JobContext* context, bool test, AstNode* expression, const Utf8& errMsg = "", const Utf8& errParam = "");
    static bool checkIsConstExpr(JobContext* context, AstNode* expression, const Utf8& errMsg = "", const Utf8& errParam = "");

    static bool warnUnusedFunction(Module* moduleToGen, ByteCode* one);
    static bool warnUnusedVariables(SemanticContext* context, Scope* scope);
    static bool warnUnreachableCode(SemanticContext* context);
    static bool warnDeprecated(SemanticContext* context, AstNode* identifier);

    static bool error(SemanticContext* context, const Utf8& msg);
    static bool notAllowed(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg = nullptr, AstNode* hintType = nullptr);
    static bool preprocessMatchError(SemanticContext* context, OneTryMatch& oneTry, Vector<const Diagnostic*>& result0, Vector<const Diagnostic*>& result1);
    static void getDiagnosticForMatch(SemanticContext* context, OneTryMatch& oneTry, Vector<const Diagnostic*>& result0, Vector<const Diagnostic*>& result1);
    static void symbolErrorRemarks(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag);
    static void symbolErrorNotes(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag, Vector<const Diagnostic*>& notes);

    static bool hasUserOp(SemanticContext* context, const Utf8& name, TypeInfoStruct* leftStruct, TypeInfoParam* parentField, VectorNative<FindUserOp>& result);
    static bool hasUserOp(SemanticContext* context, const Utf8& name, TypeInfoStruct* leftStruct, SymbolName** result);
    static bool hasUserOp(SemanticContext* context, const Utf8& name, AstNode* left, SymbolName** result);
    static bool waitUserOp(SemanticContext* context, const Utf8& name, AstNode* left, SymbolName** result);

    static void addAlternativeScopeOnce(VectorNative<AlternativeScope>& scopes, Scope* scope, uint32_t flags = 0);
    static void addAlternativeScope(VectorNative<AlternativeScope>& scopes, Scope* scope, uint32_t flags = 0);
    static bool hasAlternativeScope(VectorNative<AlternativeScope>& scopes, Scope* scope);
    static bool collectAttributes(SemanticContext* context, AstNode* forNode, AttributeList* result);
    static bool collectAttributes(SemanticContext* context, AstNode* forNode, AttributeList* result, AstAttrUse* attrUse);
    static void collectAlternativeScopes(AstNode* startNode, VectorNative<AlternativeScope>& scopes);
    static void collectAlternativeScopeVars(AstNode* startNode, VectorNative<AlternativeScope>& scopes, VectorNative<AlternativeScopeVar>& scopesVars);
    static void collectAlternativeScopeHierarchy(SemanticContext* context, VectorNative<AlternativeScope>& scopes, VectorNative<AlternativeScopeVar>& scopesVars, AstNode* startNode, uint32_t flags, IdentifierScopeUpMode scopeUpMode = IdentifierScopeUpMode::None, TokenParse* scopeUpValue = nullptr);
    static bool collectScopeHierarchy(SemanticContext* context, VectorNative<AlternativeScope>& scopes, VectorNative<AlternativeScopeVar>& scopesVars, AstNode* startNode, uint32_t flags, IdentifierScopeUpMode scopeUpMode = IdentifierScopeUpMode::None, TokenParse* scopeUpValue = nullptr);
    static bool findIdentifierInScopes(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node);
    static bool findIdentifierInScopes(SemanticContext* context, VectorNative<OneSymbolMatch>& dependentSymbols, AstIdentifierRef* identifierRef, AstIdentifier* node);

    static void         decreaseInterfaceRegCount(TypeInfoStruct* typeInfoStruct);
    static void         decreaseInterfaceCount(TypeInfoStruct* typeInfoStruct);
    static void         decreaseMethodCount(AstFuncDecl* funcNode, TypeInfoStruct* typeInfoStruct);
    static uint32_t     alignOf(AstVarDecl* node);
    static bool         isCompilerContext(AstNode* node);
    static DataSegment* getConstantSegFromContext(AstNode* node, bool forceCompiler = false);
    static void         enterState(AstNode* node);
    static void         inheritAttributesFromParent(AstNode* child);
    static void         inheritAttributesFrom(AstNode* child, uint64_t attributeFlags, uint16_t safetyOn, uint16_t safetyOff);
    static void         inheritAttributesFromOwnerFunc(AstNode* child);
    static bool         setupIdentifierRef(SemanticContext* context, AstNode* node);
    static bool         derefConstantValue(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, DataSegment* storageSegment, uint8_t* ptr);
    static bool         derefConstant(SemanticContext* context, uint8_t* ptr, SymbolOverload* overload, DataSegment* storageSegment);
    static uint32_t     getMaxStackSize(AstNode* node);
    static void         setOwnerMaxStackSize(AstNode* node, uint32_t size);
    static bool         makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* identifier);
    static bool         makeInline(SemanticContext* context, AstFuncDecl* funcDecl, AstNode* identifier);
    static void         sortParameters(AstNode* allParams);
    static void         dealWithIntrinsic(SemanticContext* context, AstIdentifier* identifier);
    static bool         setSymbolMatchCallParams(SemanticContext* context, AstIdentifier* identifier, OneMatch& oneMatch);
    static bool         setSymbolMatch(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* identifier, OneMatch& oneMatch);
    static void         resolvePendingLambdaTyping(AstFuncCallParam* nodeCall, OneMatch* oneMatch, int i);
    static void         allocateOnStack(AstNode* node, TypeInfo* typeInfo);
    static bool         setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, AstNode* funcAttr, AstNode* parameters, bool forGenerics);
    static Diagnostic*  computeNonConstExprNote(AstNode* node);
    static bool         executeCompilerNode(SemanticContext* context, AstNode* node, bool onlyConstExpr);
    static bool         doExecuteCompilerNode(SemanticContext* context, AstNode* node, bool onlyConstExpr);
    static bool         reserveAndStoreToSegment(JobContext* context, DataSegment* storageSegment, uint32_t& storageOffset, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool         storeToSegment(JobContext* context, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment);
    static bool         collectLiteralsToSegment(JobContext* context, DataSegment* storageSegment, uint32_t baseOffset, uint32_t& offset, AstNode* node);
    static bool         collectStructLiterals(JobContext* context, DataSegment* storageSegment, uint32_t offsetStruct, AstNode* node);
    static void         setupContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, SymbolOverload* symOverload, uint32_t flags);
    static bool         canInheritAccess(AstNode* node);
    static uint64_t     attributeToAccess(uint64_t attribute);
    static void         doInheritAccess(AstNode* forNode, AstNode* node);
    static void         inheritAccess(AstNode* node);
    static void         setIdentifierAccess(AstIdentifier* identifier, SymbolOverload* overload);
    static void         setDefaultAccess(AstNode* node);
    static bool         canHaveGlobalAccess(AstNode* node);

    static Utf8           findClosestMatchesMsg(SemanticContext* context, Vector<Utf8>& best);
    static void           findClosestMatches(SemanticContext* context, const Utf8& searchName, const Vector<Utf8>& searchList, Vector<Utf8>& result);
    static void           findClosestMatches(SemanticContext* context, AstNode* node, const VectorNative<AlternativeScope>& scopeHierarchy, Vector<Utf8>& best, IdentifierSearchFor searchFor = IdentifierSearchFor::Whatever);
    static Utf8           findClosestMatchesMsg(SemanticContext* context, AstNode* node, const VectorNative<AlternativeScope>& scopeHierarchy, IdentifierSearchFor searchFor = IdentifierSearchFor::Whatever);
    static bool           isFunctionButNotACall(SemanticContext* context, AstNode* node, SymbolName* symbol);
    static bool           cannotMatchIdentifierError(SemanticContext* context, MatchResult result, int paramIdx, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Vector<const Diagnostic*>& notes);
    static bool           cannotMatchIdentifierError(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node);
    static bool           matchIdentifierParameters(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, uint32_t flags = 0);
    static bool           evaluateConstExpression(SemanticContext* context, AstNode* node);
    static bool           evaluateConstExpression(SemanticContext* context, AstNode* node1, AstNode* node2);
    static bool           evaluateConstExpression(SemanticContext* context, AstNode* node1, AstNode* node2, AstNode* node3);
    static bool           waitForStructUserOps(SemanticContext* context, AstNode* node);
    static bool           collectConstantAssignment(SemanticContext* context, DataSegment** storageSegmentResult, uint32_t* storageOffsetResult, uint32_t& symbolFlags);
    static bool           collectConstantSlice(SemanticContext* context, AstNode* assignNode, TypeInfo* assignType, DataSegment* storageSegment, uint32_t& storageOffset);
    static bool           collectAssignment(SemanticContext* context, DataSegment* storageSegment, uint32_t& storageOffset, AstVarDecl* node, TypeInfo* typeInfo = nullptr);
    static void           disableCompilerIfBlock(SemanticContext* context, AstCompilerIfBlock* block);
    static void           propagateReturn(AstNode* node);
    static bool           resolveMakePointer(SemanticContext* context);
    static bool           resolveMakePointerLambda(SemanticContext* context);
    static bool           boundCheck(SemanticContext* context, TypeInfo* forType, AstNode* arrayNode, AstNode* arrayAccess, uint64_t maxCount);
    static bool           getConstantArrayPtr(SemanticContext* context, uint32_t* storageOffset, DataSegment** storageSegment);
    static void           forceConstType(SemanticContext* context, AstTypeExpression* node);
    static void           setVarDeclResolve(AstVarDecl* varNode);
    static bool           convertTypeListToArray(SemanticContext* context, AstVarDecl* node, bool isCompilerConstant, uint32_t symbolFlags, uint32_t castFlags = 0);
    static DataSegment*   getSegmentForVar(SemanticContext* context, AstVarDecl* node);
    static bool           getDigitHexa(SemanticContext* context, const char** pz, int& result, const char* errMsg);
    static bool           processLiteralString(SemanticContext* context);
    static bool           computeExpressionListTupleType(SemanticContext* context, AstNode* node);
    static bool           getUsingVar(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** result, AstNode** resultLeaf);
    static bool           canTryUfcs(SemanticContext* context, TypeInfoFuncAttr* typeFunc, AstFuncCallParams* parameters, AstNode* ufcsNode, bool nodeIsExplicit);
    static bool           getUfcs(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** ufcsFirstParam);
    static bool           appendLastCodeStatement(SemanticContext* context, AstIdentifier* node, SymbolOverload* overload);
    static bool           fillMatchContextCallParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload, AstNode* ufcsFirstParam);
    static bool           fillMatchContextGenericParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload);
    static bool           needToCompleteSymbol(SemanticContext* context, AstIdentifier* identifier, SymbolName* symbol);
    static bool           needToWaitForSymbol(SemanticContext* context, AstIdentifier* identifier, SymbolName* symbol);
    static bool           resolveIdentifier(SemanticContext* context, AstIdentifier* identifier, uint32_t riFlags);
    static TypeInfoEnum*  findEnumTypeInContext(SemanticContext* context, TypeInfo* typeInfo);
    static bool           findEnumTypeInContext(SemanticContext* context, AstNode* node, VectorNative<TypeInfoEnum*>& result, VectorNative<std::pair<AstNode*, TypeInfoEnum*>>& has);
    static void           addDependentSymbol(VectorNative<OneSymbolMatch>& symbols, SymbolName* symName, Scope* scope, uint32_t asflags);
    static void           unknownIdentifier(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node);
    static bool           ufcsSetFirstParam(SemanticContext* context, AstIdentifierRef* identifierRef, OneMatch& match);
    static bool           instantiateGenericSymbol(SemanticContext* context, OneGenericMatch& firstMatch, bool forStruct);
    static bool           filterGenericMatches(SemanticContext* context, VectorNative<OneMatch*>& matches, VectorNative<OneGenericMatch*>& genMatches);
    static bool           filterMatchesInContext(SemanticContext* context, VectorNative<OneMatch*>& matches);
    static bool           solveValidIf(SemanticContext* context, AstStruct* structDecl);
    static bool           solveValidIf(SemanticContext* context, OneMatch* oneMatch, AstFuncDecl* funcDecl);
    static bool           filterMatches(SemanticContext* context, VectorNative<OneMatch*>& matches);
    static bool           filterSymbols(SemanticContext* context, AstIdentifier* node);
    static void           flattenStructChilds(SemanticContext* context, AstNode* parent, VectorNative<AstNode*>& result);
    static bool           setFullResolve(SemanticContext* context, AstFuncDecl* funcNode);
    static void           setFuncDeclParamsIndex(AstFuncDecl* funcNode);
    static bool           isMethod(AstFuncDecl* funcNode);
    static void           launchResolveSubDecl(JobContext* context, AstNode* node);
    static bool           registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode, uint32_t symbolFlags = 0);
    static void           resolveSubDecls(JobContext* context, AstFuncDecl* funcNode);
    static Utf8           getSpecialOpSignature(AstFuncDecl* node);
    static TypeInfo*      getDeducedLambdaType(SemanticContext* context, AstMakePointer* node);
    static bool           deduceLambdaParamTypeFrom(SemanticContext* context, AstVarDecl* nodeParam, bool& lambdaExpr, bool& genericType);
    static AstFuncDecl*   getFunctionForReturn(AstNode* node);
    static bool           setUnRef(AstNode* node);
    static TypeInfo*      getConcreteTypeUnRef(AstNode* node, uint32_t concreteFlags);
    static AstIdentifier* createTmpId(SemanticContext* context, AstNode* node, const Utf8& name);

    static Utf8 getCompilerFunctionString(AstNode* node, TokenId id);
    static bool sendCompilerMsgFuncDecl(SemanticContext* context);
    static bool sendCompilerMsgTypeDecl(SemanticContext* context);
    static bool sendCompilerMsgGlobalVar(SemanticContext* context);

    static bool preResolveCompilerInstruction(SemanticContext* context);
    static bool preResolveGeneratedStruct(SemanticContext* context);
    static bool preResolveStructContent(SemanticContext* context);
    static bool preResolveAttrDecl(SemanticContext* context);
    static bool preResolveSubstBreakContinue(SemanticContext* context);
    static bool preResolveIdentifierRef(SemanticContext* context);

    static bool resolveUserOpAffect(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo, AstNode* left, AstNode* right);
    static bool resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveUnaryOpExclam(SemanticContext* context, AstNode* child);
    static bool resolveUnaryOpMinus(SemanticContext* context, AstNode* op, AstNode* child);
    static bool resolveUnaryOpInvert(SemanticContext* context, AstNode* child);
    static bool resolveUnaryOp(SemanticContext* context);
    static bool resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, TypeInfo** resultTypeInfo, uint32_t flags = 0);
    static bool resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo** resultTypeInfo, uint32_t flags = 0);
    static bool resolveType(SemanticContext* context);
    static bool resolveTypeLambdaClosure(SemanticContext* context);
    static bool resolveVarDeclAfterType(SemanticContext* context);
    static bool resolveVarDeclBefore(SemanticContext* context);
    static bool resolveVarDeclAfter(SemanticContext* context);
    static bool resolveVarDeclAfterAssign(SemanticContext* context);
    static bool resolveVarDecl(SemanticContext* context);
    static bool resolveTypeAliasBefore(SemanticContext* context);
    static bool resolveTypeAlias(SemanticContext* context);
    static bool resolveIntrinsicMakeAny(SemanticContext* context, AstNode* node, TypeInfo* typeInfo);
    static bool resolveIntrinsicMakeCallback(SemanticContext* context, AstNode* node);
    static bool resolveIntrinsicTag(SemanticContext* context);
    static bool resolveIntrinsicMakeSlice(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, const char* name);
    static bool resolveIntrinsicMakeInterface(SemanticContext* context);
    static bool resolveIntrinsicDataOf(SemanticContext* context, AstNode* node, AstNode* expression);
    static bool resolveIntrinsicCountOf(SemanticContext* context, AstNode* node, AstNode* expression);
    static bool resolveIntrinsicStringOf(SemanticContext* context);
    static bool resolveIntrinsicNameOf(SemanticContext* context);
    static bool resolveIntrinsicRunes(SemanticContext* context);
    static bool resolveIntrinsicSpread(SemanticContext* context);
    static bool resolveIntrinsicKindOf(SemanticContext* context);
    static bool resolveIntrinsicTypeOf(SemanticContext* context);
    static bool resolveIntrinsicDeclType(SemanticContext* context);
    static bool resolveIntrinsicProperty(SemanticContext* context);
    static bool resolveLiteral(SemanticContext* context);
    static bool resolveLiteralSuffix(SemanticContext* context);
    static bool resolveGetErr(SemanticContext* context);
    static bool resolveIndex(SemanticContext* context);
    static bool resolveBreak(SemanticContext* context);
    static bool resolveUnreachable(SemanticContext* context);
    static bool resolveFallThrough(SemanticContext* context);
    static bool resolveContinue(SemanticContext* context);
    static bool resolveScopeBreakable(SemanticContext* context);
    static bool resolveExpressionListTuple(SemanticContext* context);
    static bool resolveExpressionListArray(SemanticContext* context);
    static bool resolveExplicitNoInit(SemanticContext* context);
    static bool resolveBoolExpression(SemanticContext* context);
    static bool resolveCompareExpression(SemanticContext* context);
    static bool resolveFactorExpression(SemanticContext* context);
    static bool resolveShiftExpression(SemanticContext* context);
    static bool resolveCompilerMacro(SemanticContext* context);
    static bool resolveCompilerMixin(SemanticContext* context);
    static bool resolveCompilerAssert(SemanticContext* context);
    static bool resolveCompilerPrint(SemanticContext* context);
    static bool resolveCompilerError(SemanticContext* context);
    static bool resolveCompilerWarning(SemanticContext* context);
    static bool resolveCompilerForeignLib(SemanticContext* context);
    static bool resolveCompilerRun(SemanticContext* context);
    static bool resolveCompilerValidIfExpression(SemanticContext* context);
    static bool resolveCompilerAstExpression(SemanticContext* context);
    static bool resolveCompilerSpecialValue(SemanticContext* context);
    static bool resolveIntrinsicDefined(SemanticContext* context);
    static bool resolveIntrinsicLocation(SemanticContext* context);
    static bool resolveCompilerInclude(SemanticContext* context);
    static bool resolveUserOpCommutative(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right);
    static bool resolveUserOp(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right, uint32_t ropFlags = 0);
    static bool resolveUserOp(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, VectorNative<AstNode*>& params, uint32_t ropFlags = 0);
    static bool resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOp3Way(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveIdentifier(SemanticContext* context);
    static bool resolveIdentifierRef(SemanticContext* context);
    static bool resolveTryBlock(SemanticContext* context);
    static bool resolveTry(SemanticContext* context);
    static bool resolveTryCatch(SemanticContext* context);
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
    static bool resolveFuncDeclAfterSI(SemanticContext* context);
    static bool resolveFuncDecl(SemanticContext* context);
    static bool resolveFuncDeclType(SemanticContext* context);
    static bool resolveCaptureFuncCallParams(SemanticContext* context);
    static bool resolveFuncCallGenParams(SemanticContext* context);
    static bool resolveFuncCallParams(SemanticContext* context);
    static bool resolveFuncCallParam(SemanticContext* context);
    static bool resolveAttrDecl(SemanticContext* context);
    static bool resolveAttrUse(SemanticContext* context, AstAttrUse* node);
    static bool resolveAttrUse(SemanticContext* context);
    static bool resolveReturn(SemanticContext* context);
    static bool resolveRetVal(SemanticContext* context);
    static bool resolveUsingVar(SemanticContext* context, AstNode* varNode, TypeInfo* typeInfoVar);
    static bool resolveNameAlias(SemanticContext* context);
    static bool resolveWith(SemanticContext* context);
    static bool resolveWithVarDeclAfter(SemanticContext* context);
    static bool resolveWithAfterKnownType(SemanticContext* context);
    static bool resolveUsing(SemanticContext* context);
    static bool resolveBinaryOpModulo(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBitmaskOr(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveBitmaskAnd(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveShiftLeft(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveShiftRight(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveXor(SemanticContext* context, AstNode* left, AstNode* right);
    static bool resolveAppend(SemanticContext* context, AstNode* left, AstNode* right);
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
    static bool resolveAfterKnownType(SemanticContext* context);
    static bool resolveAffect(SemanticContext* context);
    static bool resolveMove(SemanticContext* context);
    static bool resolveRange(SemanticContext* context);
    static bool resolveScopedStmtBefore(SemanticContext* context);
    static bool resolveScopedStmtAfter(SemanticContext* context);
    static bool resolveArrayPointerDeRef(SemanticContext* context);
    static bool resolveArrayPointerSlicingUpperBound(SemanticContext* context);
    static bool resolveArrayPointerSlicing(SemanticContext* context);
    static bool resolveMoveRef(SemanticContext* context);
    static bool resolveKeepRef(SemanticContext* context);
    static bool resolveArrayPointerIndex(SemanticContext* context);
    static bool resolveArrayPointerRef(SemanticContext* context);
    static bool resolveTypeList(SemanticContext* context);
    static bool resolveConditionalOp(SemanticContext* context);
    static bool resolveDefer(SemanticContext* context);
    static bool resolveNullConditionalOp(SemanticContext* context);
    static bool resolveInit(SemanticContext* context);
    static bool resolveDropCopyMove(SemanticContext* context);
    static bool resolveTupleUnpackBefore(SemanticContext* context);
    static bool resolveTupleUnpackBeforeVar(SemanticContext* context);

    void clearTryMatch()
    {
        for (auto p : cacheListTryMatch)
            cacheFreeTryMatch.push_back(p);
        cacheListTryMatch.clear();
    }

    OneTryMatch* getTryMatch()
    {
        if (cacheFreeTryMatch.empty())
            return Allocator::alloc<OneTryMatch>();
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
            return Allocator::alloc<OneMatch>();
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
            return Allocator::alloc<OneGenericMatch>();
        auto res = cacheFreeGenericMatches.back();
        cacheFreeGenericMatches.pop_back();
        res->reset();
        return res;
    }

    VectorNative<OneSymbolMatch>      cacheDependentSymbols;
    VectorNative<AlternativeScope>    cacheScopeHierarchy;
    VectorNative<AlternativeScopeVar> cacheScopeHierarchyVars;
    VectorNative<OneOverload>         cacheToSolveOverload;
    VectorNative<OneMatch*>           cacheMatches;
    VectorNative<OneMatch*>           cacheFreeMatches;
    VectorNative<OneGenericMatch*>    cacheGenericMatches;
    VectorNative<OneGenericMatch*>    cacheGenericMatchesSI;
    VectorNative<OneGenericMatch*>    cacheFreeGenericMatches;
    VectorNative<OneTryMatch*>        cacheListTryMatch;
    VectorNative<OneTryMatch*>        cacheFreeTryMatch;
    VectorNative<AlternativeScope>    scopesToProcess;
    VectorNative<AstNode*>            tmpNodes;
    SemanticContext                   context;
    Concat                            tmpConcat;
    AstIdentifierRef*                 tmpIdRef = nullptr;

    AstFuncCallParam  closureFirstParam;
    MatchResult       bestMatchResult;
    BadSignatureInfos bestSignatureInfos;
    SymbolOverload*   bestOverload = nullptr;
    bool              canSpawn     = false;
};
