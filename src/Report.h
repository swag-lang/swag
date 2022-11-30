#pragma once
struct AstNode;
struct Diagnostic;

namespace Report
{
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);
    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool internalError(AstNode* node, const char* msg);
}; // namespace Report
