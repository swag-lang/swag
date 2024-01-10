#pragma once
#include "Job.h"
#include "TypeInfo.h"
#include "AstNode.h"
#include "Concat.h"
#include "Generic.h"

struct AstFuncCallParam;
struct SymbolName;
struct SymbolOverload;

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

struct OneGenericMatch
{
    VectorNative<TypeInfo*>             genericParametersCallTypes;
    VectorNative<ComputedValue*>        genericParametersCallValues;
    VectorNative<AstNode*>              genericParametersCallFrom;
    VectorMap<Utf8, GenericReplaceType> genericReplaceTypes;
    VectorMap<Utf8, ComputedValue*>     genericReplaceValues;
    VectorNative<TypeInfo*>             genericParametersGenTypes;

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

struct OneOverload
{
    SymbolOverload* overload;
    Scope*          scope;
    uint32_t        cptOverloads;
    uint32_t        cptOverloadsInit;
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

struct SemanticContext : public JobContext
{
    void             release();
    void             clearTryMatch();
    OneTryMatch*     getTryMatch();
    void             clearMatch();
    OneMatch*        getOneMatch();
    void             clearGenericMatch();
    OneGenericMatch* getOneGenericMatch();

    Vector<CastStructStructField>     castStructStructFields;
    Vector<CastCollectInterfaceField> castCollectInterfaceField;
    VectorNative<TypeInfoEnum*>       castCollectEnum;
    Set<TypeInfoEnum*>                castCollectEnumDone;
    uint32_t                          castFlagsResult   = 0;
    TypeInfo*                         castErrorToType   = nullptr;
    TypeInfo*                         castErrorFromType = nullptr;
    uint64_t                          castErrorFlags    = 0;
    CastErrorType                     castErrorType     = CastErrorType::Zero;

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
    Concat                            tmpConcat;
    AstIdentifierRef*                 tmpIdRef = nullptr;
    AstFuncCallParam                  closureFirstParam;
    MatchResult                       bestMatchResult;
    BadSignatureInfos                 bestSignatureInfos;
    SymbolOverload*                   bestOverload = nullptr;
    bool                              canSpawn     = false;
    bool                              forDebugger  = false;
};
