#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SourceLocation.h"
struct Module;
struct AstNode;
struct PoolFactory;
struct Diagnostic;
struct Scope;

enum class BuildPass
{
    Lexer,
    Syntax,
    Semantic,
    Full,
};

enum class TextFormat
{
    UTF8,
};

struct SourceFile : public PoolElement
{
    SourceFile();
    char32_t getChar(unsigned& offset);
    Utf8     getLine(long seek);
    bool     report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool     report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);

    bool open();
    void cleanCache();
    void seekTo(long seek);
    long readTo(char* buffer);
    void notifyLoad();
    void close();
    void waitRequest(int reqNum);
    void validateRequest(int reqNum);
    void buildRequest(int reqNum);
    char getPrivateChar();
    void waitEndRequests();
    bool checkFormat(int bufferIndex);

    fs::path  path;
    BuildPass buildPass     = BuildPass::Full;
    int       unittestError = 0;
    int       silent        = 0;
    Module*   module        = nullptr;
    AstNode*  astRoot       = nullptr;
    uint32_t  indexInModule = UINT32_MAX;

    const char* externalBuffer = nullptr;
    int         seekExternal   = 0;

    TextFormat                   textFormat = TextFormat::UTF8;
    int                          bufferSize;
    int                          headerSize = 0;
    FILE*                        fileHandle;
    long                         fileSeek       = 0;
    long                         bufferCurSeek  = 0;
    int                          bufferCurIndex = 0;
    char*                        buffers[2];
    struct LoadingThreadRequest* requests[2];
    long                         buffersSize[2];
    mutex                        mutexNotify;
    bool                         doneLoading = false;
    bool                         directMode  = false;
    bool                         openedOnce  = false;
    bool                         formatDone  = false;
    int                          totalRead   = 0;
    condition_variable           condVar;
};
