#pragma once
struct AstNode;

struct TypeManager
{
    static bool makeCompatibles(SourceFile* sourceFile, AstNode* sourceNode, AstNode* fromNode);
};
