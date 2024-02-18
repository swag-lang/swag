#pragma once
struct AstNode;
struct Module;
struct Diagnostic;
struct SourceFile;
struct ByteCodeRunContext;

namespace Report
{
    SourceFile* getDiagFile(const Diagnostic& err);

    bool report(const Diagnostic& err, const Vector<const Diagnostic*>& notes, ByteCodeRunContext* runContext = nullptr);
    bool report(const Diagnostic& err, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);

    void error(const Utf8& msg);
    void errorOS(const Utf8& msg);
    bool error(Module* module, const Utf8& msg);

    bool internalError(AstNode* node, const char* msg);
    bool internalError(Module* module, const char* msg);
}

extern thread_local int  g_SilentError;
extern thread_local Utf8 g_SilentErrorMsg;

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
