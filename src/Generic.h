
#pragma once
struct SemanticContext;
struct SymbolOverload;
struct OneGenericMatch;
struct AstVarDecl;
struct CloneContext;

struct Generic
{
    static bool      updateGenericParameters(SemanticContext* context, bool doType, bool doNode, VectorNative<TypeInfoParam*>& typeGenericParameters, VectorNative<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match);
    static Job*      end(SemanticContext* context, Job* dependentJob, SymbolName* symbol, AstNode* newNode, bool waitSymbol, Map<Utf8, TypeInfo*>& replaceTypes);
    static TypeInfo* doTypeSubstitution(Map<Utf8, TypeInfo*>& replaceTypes, TypeInfo* typeInfo);

    static void waitForGenericParameters(SemanticContext* context, OneGenericMatch& match);
    static void instantiateSpecialFunc(SemanticContext* context, Job* structJob, CloneContext& cloneContext, AstFuncDecl** funcNode);
    static bool instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool& alias);
    static bool instantiateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool validif = false);
    static bool instantiateDefaultGenericVar(SemanticContext* context, AstVarDecl* node);
    static bool instantiateDefaultGenericFunc(SemanticContext* context);
};