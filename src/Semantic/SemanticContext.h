#pragma once
#include "Format/FormatConcat.h"
#include "Semantic/Generic/Generic.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/AstNode.h"
#include "Threading/Job.h"

struct AstFuncCallParam;
struct SymbolName;
struct SymbolOverload;
using CastFlagsResult = Flags<uint32_t>;

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
    TypeInfo*          type              = nullptr;
    uint32_t           cptOverloads      = 0;
    uint32_t           cptOverloadsInit  = 0;
    bool               ufcs              = false;

    void reset()
    {
        symMatchContext.reset();
        overload          = nullptr;
        scope             = nullptr;
        dependentVar      = nullptr;
        dependentVarLeaf  = nullptr;
        callParameters    = nullptr;
        genericParameters = nullptr;
        type              = nullptr;
        cptOverloads      = 0;
        ufcs              = false;
    }
};

struct OneMatch
{
    struct ParamParameter
    {
        AstFuncCallParam* param;
        uint32_t          indexParam;
        TypeInfoParam*    resolvedParameter;
    };

    VectorNative<GenericReplaceType>    genericParametersCallTypes;
    VectorNative<ComputedValue*>        genericParametersCallValues;
    VectorMap<Utf8, GenericReplaceType> genericReplaceTypes;
    VectorMap<Utf8, ComputedValue*>     genericReplaceValues;
    VectorNative<TypeInfoParam*>        solvedParameters;
    VectorNative<CastFlagsResult>       solvedCastFlags;
    VectorNative<ParamParameter>        paramParameters;
    VectorNative<AstNode*>              parameters;

    SymbolName*     symbolName        = nullptr;
    SymbolOverload* symbolOverload    = nullptr;
    Scope*          scope             = nullptr;
    AstNode*        dependentVar      = nullptr;
    OneTryMatch*    oneOverload       = nullptr;
    TypeInfo*       typeWasForced     = nullptr;
    AstNode*        genericParameters = nullptr;

    uint32_t        matchFlags                  = 0;
    CastFlagsResult castFlagsResult             = 0;
    uint32_t        coerceCast                  = 0;
    uint32_t        numOverloadsWhenChecked     = 0;
    uint32_t        numOverloadsInitWhenChecked = 0;

    bool ufcs   = false;
    bool remove = false;

    void reset()
    {
        genericParametersCallTypes.clear();
        genericParametersCallValues.clear();
        genericReplaceTypes.clear();
        genericReplaceValues.clear();
        solvedParameters.clear();
        solvedCastFlags.clear();
        paramParameters.clear();
        parameters.clear();
        symbolName        = nullptr;
        symbolOverload    = nullptr;
        scope             = nullptr;
        dependentVar      = nullptr;
        oneOverload       = nullptr;
        typeWasForced     = nullptr;
        genericParameters = nullptr;

        matchFlags                  = 0;
        castFlagsResult             = 0;
        coerceCast                  = 0;
        numOverloadsWhenChecked     = 0;
        numOverloadsInitWhenChecked = 0;

        ufcs   = false;
        remove = false;
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
    SymbolName*   symbol   = nullptr;
    Scope*        scope    = nullptr;
    AltScopeFlags altFlags = 0;
    bool          remove   = false;

    bool operator==(const OneSymbolMatch& other) const
    {
        return symbol == other.symbol;
    }
};

struct SemanticContext : JobContext
{
    void         release();
    void         clearTryMatch();
    OneTryMatch* getTryMatch();
    void         clearMatch();
    OneMatch*    getOneMatch();
    void         clearGenericMatch();

    Vector<CastStructStructField>     castStructStructFields;
    Vector<CastCollectInterfaceField> castCollectInterfaceField;
    VectorNative<TypeInfoEnum*>       castCollectEnum;
    Set<TypeInfoEnum*>                castCollectEnumDone;
    CastFlagsResult                   castFlagsResult   = 0;
    TypeInfo*                         castErrorToType   = nullptr;
    TypeInfo*                         castErrorFromType = nullptr;
    CastFlags                         castErrorFlags    = 0;
    CastErrorType                     castErrorType     = CastErrorType::Zero;

    VectorNative<OneSymbolMatch>      cacheSymbolsMatch;
    VectorNative<AlternativeScope>    cacheScopeHierarchy;
    VectorNative<AlternativeScopeVar> cacheScopeHierarchyVars;
    VectorNative<OneOverload>         cacheToSolveOverload;
    VectorNative<OneMatch*>           cacheMatches;
    VectorNative<OneMatch*>           cacheFreeMatches;
    VectorNative<OneMatch*>           cacheGenericMatches;
    VectorNative<OneMatch*>           cacheGenericMatchesSI;
    VectorNative<OneTryMatch*>        cacheListTryMatch;
    VectorNative<OneTryMatch*>        cacheFreeTryMatch;
    VectorNative<AlternativeScope>    scopesToProcess;
    VectorNative<AstNode*>            tmpNodes;
    FormatConcat                      tmpConcat;
    AstIdentifierRef*                 tmpIdRef = nullptr;
    AstFuncCallParam                  closureFirstParam;
    MatchResult                       bestMatchResult;
    BadSignatureInfos                 bestSignatureInfos;
    SymbolOverload*                   bestOverload = nullptr;
    bool                              canSpawn     = false;
    bool                              forDebugger  = false;
};
