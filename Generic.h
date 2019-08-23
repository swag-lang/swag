
#pragma once
struct SemanticContext;
struct SymbolOverload;
struct OneGenericMatch;

struct Generic
{
	static bool InstanciateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match);
	static bool InstanciateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match);
};