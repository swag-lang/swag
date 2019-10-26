
#pragma once
struct SemanticContext;
struct SymbolOverload;
struct OneGenericMatch;
struct AstVarDecl;

struct Generic
{
    static void computeTypeReplacements(CloneContext& cloneContext, OneGenericMatch& match);
    static void updateGenericParameters(vector<TypeInfoParam*>& typeGenericParameters, vector<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match);
    static void end(SemanticContext* context, AstNode* newNode, bool waitSymbol);
	static void doTypeSubstitution(SemanticContext* context, CloneContext& cloneContext, TypeInfo** typeInfo);

    static bool instanciateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool waitSymbol);
    static bool instanciateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match);
	static bool instantiateDefaultGeneric(SemanticContext* context, AstVarDecl* node);
};