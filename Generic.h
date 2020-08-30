
#pragma once
struct SemanticContext;
struct SymbolOverload;
struct OneGenericMatch;
struct AstVarDecl;

struct InstanciateContext
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

    static void instanciateSpecialFunc(SemanticContext* context, Job* structJob, CloneContext& cloneContext, TypeInfoStruct* typeStruct, AstFuncDecl** funcNode);
    static bool instanciateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, InstanciateContext &instContext);
    static bool instanciateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, InstanciateContext& instContext);
    static bool instantiateDefaultGeneric(SemanticContext* context, AstVarDecl* node);
};