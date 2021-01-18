
#pragma once
struct SemanticContext;
struct SymbolOverload;
struct OneGenericMatch;
struct AstVarDecl;
struct CloneContext;

struct Generic
{
    static bool      updateGenericParameters(SemanticContext* context, bool doType, bool doNode, VectorNative<TypeInfoParam*>& typeGenericParameters, VectorNative<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match);
    static Job*      end(SemanticContext* context, Job* dependentJob, SymbolName* symbol, AstNode* newNode, bool waitSymbol);
    static TypeInfo* doTypeSubstitution(map<Utf8, TypeInfo*>& replaceTypes, TypeInfo* typeInfo);

    static void instantiateSpecialFunc(SemanticContext* context, Job* structJob, CloneContext& cloneContext, AstFuncDecl** funcNode);
    static bool instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match);
    static bool instantiateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool selectIf = false);
    static bool instantiateDefaultGeneric(SemanticContext* context, AstVarDecl* node);
};