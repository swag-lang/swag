#pragma once
#include "Flags.h"

struct AstFuncDecl;
struct AstNode;
struct AstVarDecl;
struct CloneContext;
struct Job;
struct OneMatch;
struct OneTryMatch;
struct SemanticContext;
struct SymbolMatchContext;
struct SymbolName;
struct SymbolOverload;
struct TypeInfo;
struct TypeInfoParam;
using CastFlags = Flags<uint64_t>;

struct GenericReplaceType
{
	TypeInfo* typeInfoGeneric = nullptr;
	TypeInfo* typeInfoReplace = nullptr;
	AstNode*  fromNode        = nullptr;
};

namespace Generic
{
	bool         replaceGenericParameters(SemanticContext* context, bool doType, bool doNode, VectorNative<TypeInfoParam*>& typeGenericParameters, VectorNative<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneMatch& match);
	TypeInfo*    replaceGenericTypes(VectorMap<Utf8, GenericReplaceType>& replaceTypes, TypeInfo* typeInfo);
	Job*         end(SemanticContext* context, Job* job, SymbolName* symbol, AstNode* newNode, bool waitSymbol, const VectorMap<Utf8, GenericReplaceType>& replaceTypes);
	void         deduceSubType(SymbolMatchContext& context, TypeInfo* wantedTypeInfo, TypeInfo*& callTypeInfo, VectorNative<TypeInfo*>& wantedTypeInfos, VectorNative<TypeInfo*>& callTypeInfos, AstNode* callParameter);
	void         deduceType(SymbolMatchContext& context, TypeInfo* wantedTypeInfo, TypeInfo* callTypeInfo, CastFlags castFlags, int idxParam, AstNode* callParameter);
	void         deduceGenericTypes(SymbolMatchContext& context, AstNode* callParameter, TypeInfo* callTypeInfo, TypeInfo* wantedTypeInfo, int idxParam, CastFlags castFlags);
	void         setContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, const SymbolOverload* symOverload, uint32_t flags);
	void         setUserGenericTypeReplacement(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& genericParameters);
	void         checkCanInstantiateGenericSymbol(SemanticContext* context, OneMatch& firstMatch);
	bool         instantiateGenericSymbol(SemanticContext* context, OneMatch& firstMatch, bool forStruct);
	Vector<Utf8> computeGenericParametersReplacement(const VectorMap<Utf8, GenericReplaceType>& replace);

	void instantiateSpecialFunc(SemanticContext* context, Job* structJob, CloneContext& cloneContext, AstFuncDecl** specialFct);
	bool instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneMatch& match, bool& alias);
	bool instantiateFunction(SemanticContext* context, AstNode* genericParameters, OneMatch& match, bool validif = false);
	bool instantiateDefaultGenericVar(SemanticContext* context, AstVarDecl* node);
	bool instantiateDefaultGenericFunc(SemanticContext* context);
}
