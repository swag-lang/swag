#pragma once
struct AstNode;
struct Module;
struct Diagnostic;
struct SourceFile;

namespace Report
{
    SourceFile* getDiagFile(const Diagnostic& diag);

    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);
    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);

    void error(const Utf8& msg);
    void errorOS(const Utf8& msg);
    bool error(Module* module, const Utf8& msg);

    bool internalError(AstNode* node, const char* msg);
    bool internalError(Module* module, const char* msg);
}; // namespace Report

extern thread_local int    g_SilentError;
extern thread_local string g_SilentErrorMsg;

struct PushSilentError
{
    PushSilentError()
    {
        g_SilentError++;
        g_SilentErrorMsg.clear();
    }

    ~PushSilentError()
    {
        g_SilentError--;
    }
};