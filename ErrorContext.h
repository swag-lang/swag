#pragma once
struct SourceFile;
struct AstNode;
struct Diagnostic;

struct ErrorContext
{
    SourceFile*             sourceFile;
    vector<struct AstNode*> genericInstanceTree;

    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);
};
