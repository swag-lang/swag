
#pragma once
struct AstFuncDecl;
struct AstNode;
struct AstVarDecl;
struct CloneContext;
struct Job;
struct OneTryMatch;
struct OneGenericMatch;
struct SemanticContext;
struct SymbolMatchContext;
struct SymbolName;
struct SymbolOverload;
struct TypeInfo;
struct TypeInfoParam;

struct GenericReplaceType
{
    TypeInfo* typeInfoGeneric = nullptr;
    TypeInfo* typeInfoReplace = nullptr;
    AstNode*  fromNode        = nullptr;
};

namespace Generic
{
    bool      updateGenericParameters(SemanticContext* context, bool doType, bool doNode, VectorNative<TypeInfoParam*>& typeGenericParameters, VectorNative<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match);
    Job*      end(SemanticContext* context, Job* dependentJob, SymbolName* symbol, AstNode* newNode, bool waitSymbol, VectorMap<Utf8, GenericReplaceType>& replaceTypes);
    TypeInfo* doTypeSubstitution(VectorMap<Utf8, GenericReplaceType>& replaceTypes, TypeInfo* typeInfo);
    void      deduceGenericParam(SymbolMatchContext& context, AstNode* callParameter, TypeInfo* callTypeInfo, TypeInfo* wantedTypeInfo, int idxParam, uint64_t castFlags);
    void      setContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, SymbolOverload* symOverload, uint32_t flags);
    void      setUserGenericTypeReplacement(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& genericParameters);

    void instantiateSpecialFunc(SemanticContext* context, Job* structJob, CloneContext& cloneContext, AstFuncDecl** funcNode);
    bool instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool& alias);
    bool instantiateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool validif = false);
    bool instantiateDefaultGenericVar(SemanticContext* context, AstVarDecl* node);
    bool instantiateDefaultGenericFunc(SemanticContext* context);
}; // namespace Generic