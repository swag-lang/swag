
#pragma once
struct SemanticContext;
struct SymbolOverload;
struct OneGenericMatch;

struct Generic
{
    static void computeTypeReplacments(CloneContext& cloneContext, OneGenericMatch& match);
	static void end(SemanticContext* context, AstNode* newNode);

    static bool instanciateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match);
    static bool instanciateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match);
};