#pragma once
struct AstNode;
struct Module;
struct Diagnostic;

namespace Report
{
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);
    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool error(Module* module, const Utf8& msg);
    bool internalError(AstNode* node, const char* msg);
    bool internalError(Module* module, const char* msg);
}; // namespace Report
