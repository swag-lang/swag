#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SourceLocation.h"
#include "BuildPass.h"
#include "File.h"
struct Module;
struct AstNode;
struct Diagnostic;
struct Scope;

struct LoadRequest
{
    long seek       = 0;
    long loadedSize = 0;
};

enum class TextFormat
{
    UTF8,
};

struct SourceFile : public File
{
    SourceFile();

    char32_t getChar(unsigned& offset);
    Utf8     getLine(long seek);
    bool     report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool     report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);
    void     load(LoadRequest* request);

    void cleanCache();
    void seekTo(long seek);
    long readTo();
    void loadRequest();
    char getPrivateChar();
    bool checkFormat();

    Module*  module         = nullptr;
    AstNode* astRoot        = nullptr;
    uint8_t* externalBuffer = nullptr;
    uint32_t externalSize   = 0;
    uint32_t seekExternal   = 0;

    uint64_t  writeTime     = 0;
    int       numErrors     = 0;
    int       unittestError = 0;
    int       silent        = 0;
    bool      swagFile      = false;
    uint32_t  indexInModule = UINT32_MAX;
    BuildPass buildPass     = BuildPass::Full;

    char*  buffer       = nullptr;
    Scope* scopeRoot    = nullptr;
    Scope* scopePrivate = nullptr;

    TextFormat   textFormat    = TextFormat::UTF8;
    int          headerSize    = 0;
    long         fileSeek      = 0;
    long         bufferCurSeek = 0;
    long         bufferSize    = 0;
    int          totalRead     = 0;
    bool         doneLoading   = false;
    bool         directMode    = false;
    bool         formatDone    = false;
    bool         fromTests     = false;
    bool         generated     = false;
    shared_mutex mutexGetLine;
};
