
#pragma once
struct SemanticContext;
struct SymbolOverload;
struct OneGenericMatch;
struct AstVarDecl;

struct Generic
{
    static bool      updateGenericParameters(SemanticContext* context, VectorNative<TypeInfoParam*>& typeGenericParameters, VectorNative<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match);
    static void      end(SemanticContext* context, AstNode* newNode, bool waitSymbol);
    static TypeInfo* doTypeSubstitution(CloneContext& cloneContext, TypeInfo* typeInfo);

    static bool instanciateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool waitSymbol);
    static bool instanciateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match);
    static bool instantiateDefaultGeneric(SemanticContext* context, AstVarDecl* node);
};