#pragma once
#include "AstNode.h"
#include "Concat.h"
#include "Generic.h"
#include "Job.h"
#include "TypeInfo.h"

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
		int               indexParam;
		TypeInfoParam*    resolvedParameter;
	};

	VectorNative<GenericReplaceType>    genericParametersCallTypes;
	VectorNative<ComputedValue*>        genericParametersCallValues;
	VectorMap<Utf8, GenericReplaceType> genericReplaceTypes;
	VectorMap<Utf8, ComputedValue*>     genericReplaceValues;
	VectorNative<TypeInfoParam*>        solvedParameters;
	VectorNative<uint32_t>              solvedCastFlags;
	VectorNative<ParamParameter>        paramParameters;
	VectorNative<AstNode*>              parameters;

	SymbolName*     symbolName        = nullptr;
	SymbolOverload* symbolOverload    = nullptr;
	Scope*          scope             = nullptr;
	AstNode*        dependentVar      = nullptr;
	OneTryMatch*    oneOverload       = nullptr;
	TypeInfo*       typeWasForced     = nullptr;
	AstNode*        genericParameters = nullptr;

	uint32_t matchFlags                  = 0;
	uint32_t castFlagsResult             = 0;
	uint32_t coerceCast                  = 0;
	uint32_t numOverloadsWhenChecked     = 0;
	uint32_t numOverloadsInitWhenChecked = 0;

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
	SymbolName* symbol  = nullptr;
	Scope*      scope   = nullptr;
	uint32_t    asFlags = 0;
	bool        remove  = false;

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
	OneMatch*    getOneGenericMatch();

	Vector<CastStructStructField>     castStructStructFields;
	Vector<CastCollectInterfaceField> castCollectInterfaceField;
	VectorNative<TypeInfoEnum*>       castCollectEnum;
	Set<TypeInfoEnum*>                castCollectEnumDone;
	uint32_t                          castFlagsResult   = 0;
	TypeInfo*                         castErrorToType   = nullptr;
	TypeInfo*                         castErrorFromType = nullptr;
	CastFlags                         castErrorFlags    = 0;
	CastErrorType                     castErrorType     = CastErrorType::Zero;

	VectorNative<OneSymbolMatch>      cacheDependentSymbols;
	VectorNative<AlternativeScope>    cacheScopeHierarchy;
	VectorNative<AlternativeScopeVar> cacheScopeHierarchyVars;
	VectorNative<OneOverload>         cacheToSolveOverload;
	VectorNative<OneMatch*>           cacheMatches;
	VectorNative<OneMatch*>           cacheFreeMatches;
	VectorNative<OneMatch*>           cacheGenericMatches;
	VectorNative<OneMatch*>           cacheGenericMatchesSI;
	VectorNative<OneMatch*>           cacheFreeGenericMatches;
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
