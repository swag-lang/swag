#pragma once
struct AstNode;

struct TypeManager
{
    static TypeInfo* makeCompatibles(SourceFile* sourceFile, AstNode* sourceNode, AstNode* fromNode);
};
