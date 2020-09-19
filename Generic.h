
#pragma once
struct SemanticContext;
struct SymbolOverload;
struct OneGenericMatch;
struct AstVarDecl;

struct InstantiateContext
{
    bool fromBake     = false;
    bool bakeIsPublic = false;
    Utf8 bakeName;
};

struct Generic
{
    static bool      updateGenericParameters(SemanticContext* context, VectorNative<TypeInfoParam*>& typeGenericParameters, VectorNative<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match);
    static Job*      end(SemanticContext* context, Job* dependentJob, SymbolName* symbol, AstNode* newNode, bool waitSymbol);
    static TypeInfo* doTypeSubstitution(CloneContext& cloneContext, TypeInfo* typeInfo);

    static void instantiateSpecialFunc(SemanticContext* context, Job* structJob, CloneContext& cloneContext, AstFuncDecl** funcNode);
    static bool instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, InstantiateContext &instContext);
    static bool instantiateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, InstantiateContext& instContext);
    static bool instantiateDefaultGeneric(SemanticContext* context, AstVarDecl* node);
};