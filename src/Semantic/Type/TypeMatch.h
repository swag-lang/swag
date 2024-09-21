#pragma once
#include "Semantic/Generic/Generic.h"

struct AstNode;
struct TypeInfoFuncAttr;
struct TypeInfoStruct;
struct SymbolMatchContext;
struct TypeInfo;
struct TypeInfoParam;
struct ComputedValue;
struct SemanticContext;
using CastFlagsResult = Flags<uint32_t>;
using MatchFlags      = Flags<uint32_t>;

enum class MatchResult
{
    Ok,
    TooManyArguments,
    TooManyGenericArguments,
    NotEnoughArguments,
    MissingArguments,
    NotEnoughGenericArguments,
    BadSignature,
    BadGenericSignature,
    InvalidNamedArgument,
    MissingNamedArgument,
    DuplicatedNamedArgument,
    MismatchGenericValue,
    WhereFailed,
    MismatchThrow,
    NoReturnType,
    MissingReturnType,
    MismatchReturnType,
};

enum class CastErrorType
{
    Zero = 0,
    Const,
    SliceArray,
    SafetyCastAny,
};

struct BadSignatureInfos
{
    Utf8                                badGenMatch;
    VectorMap<Utf8, GenericReplaceType> genericReplaceTypes;
    VectorMap<Utf8, ComputedValue*>     genericReplaceValues;

    AstNode*       badNode                   = nullptr;
    AstNode*       genMatchFromNode          = nullptr;
    AstNode*       failedWhere               = nullptr;
    TypeInfo*      badSignatureRequestedType = nullptr;
    TypeInfo*      badSignatureGivenType     = nullptr;
    TypeInfo*      castErrorToType           = nullptr;
    TypeInfo*      castErrorFromType         = nullptr;
    ComputedValue* badGenValue1              = nullptr;
    ComputedValue* badGenValue2              = nullptr;

    MatchResult matchResult = MatchResult::Ok;

    CastFlags     castErrorFlags;
    CastErrorType castErrorType            = CastErrorType::Zero;
    uint32_t      badSignatureParameterIdx = UINT32_MAX;
    uint32_t      badSignatureNum1         = 0;
    uint32_t      badSignatureNum2         = 0;

    void clear()
    {
        badGenMatch.clear();
        genericReplaceTypes.clear();
        genericReplaceValues.clear();
        badNode                   = nullptr;
        genMatchFromNode          = nullptr;
        failedWhere               = nullptr;
        badSignatureRequestedType = nullptr;
        badSignatureGivenType     = nullptr;
        castErrorToType           = nullptr;
        castErrorFromType         = nullptr;
        badGenValue1              = nullptr;
        badGenValue2              = nullptr;
        matchResult               = MatchResult::Ok;
        castErrorFlags            = 0;
        castErrorType             = CastErrorType::Zero;
        badSignatureParameterIdx  = UINT32_MAX;
        badSignatureNum1          = 0;
        badSignatureNum2          = 0;
    }
};

struct SymbolMatchContext
{
    static constexpr MatchFlags MATCH_ACCEPT_NO_GENERIC = 0x00000001;
    static constexpr MatchFlags MATCH_FOR_LAMBDA        = 0x00000002;
    static constexpr MatchFlags MATCH_GENERIC_AUTO      = 0x00000004;
    static constexpr MatchFlags MATCH_ERROR_VALUE_TYPE  = 0x00000008;
    static constexpr MatchFlags MATCH_ERROR_TYPE_VALUE  = 0x00000010;
    static constexpr MatchFlags MATCH_UN_CONST          = 0x00000020;
    static constexpr MatchFlags MATCH_UFCS              = 0x00000040;
    static constexpr MatchFlags MATCH_CLOSURE_PARAM     = 0x00000080;

    VectorNative<AstNode*>              genericParameters;
    VectorNative<AstNode*>              parameters;
    VectorNative<TypeInfoParam*>        solvedParameters;
    VectorNative<TypeInfoParam*>        solvedCallParameters;
    VectorNative<CastFlagsResult>       solvedCastFlags;
    VectorNative<bool>                  doneParameters;
    VectorNative<GenericReplaceType>    genericParametersCallTypes;
    VectorNative<ComputedValue*>        genericParametersCallValues;
    VectorMap<Utf8, GenericReplaceType> genericReplaceTypes;
    VectorMap<Utf8, ComputedValue*>     genericReplaceValues;
    VectorMap<Utf8, uint32_t>           mapGenericTypeToIndex;
    BadSignatureInfos                   badSignatureInfos;

    SemanticContext* semContext = nullptr;

    MatchFlags      matchFlags;
    CastFlagsResult castFlagsResult;
    MatchResult     result;
    uint32_t        cptResolved;
    uint32_t        firstDefault;

    bool hasNamedParameters;

    SymbolMatchContext()
    {
        reset();
    }

    void reset()
    {
        genericParameters.clear();
        parameters.clear();
        solvedParameters.clear();
        solvedCallParameters.clear();
        solvedCastFlags.clear();
        doneParameters.clear();
        genericParametersCallTypes.clear();
        genericParametersCallValues.clear();
        genericReplaceTypes.clear();
        genericReplaceValues.clear();
        mapGenericTypeToIndex.clear();
        badSignatureInfos.clear();

        semContext = nullptr;

        matchFlags      = 0;
        castFlagsResult = 0;
        result          = MatchResult::Ok;
        cptResolved     = 0;
        firstDefault    = UINT32_MAX;

        hasNamedParameters = false;
    }

    void resetTmp()
    {
        cptResolved        = 0;
        hasNamedParameters = false;
    }
};

namespace Match
{
    void match(TypeInfoFuncAttr* typeFunc, SymbolMatchContext& context);
    void match(TypeInfoStruct* typeStruct, SymbolMatchContext& context);
}
