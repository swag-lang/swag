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
struct Token;

struct LoadRequest
{
    long seek       = 0;
    long loadedSize = 0;
};

struct SourceFile : public File
{
    SourceFile();

    char32_t getChar(unsigned& offset);
    char32_t getCharExtended(char c, unsigned& offset);
    Utf8     getLine(long lineNo);
    bool     report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool     report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);
    void     load(LoadRequest* request);

    void cleanCache();
    void seekTo(long seek);
    long readTo();
    void loadRequest();
    char loadAndGetPrivateChar();
    char getPrivateChar();
    bool checkFormat();
    void setExternalBuffer(char* buf, uint32_t size);

    vector<string> allLines;
    Module*        module  = nullptr;
    AstNode*       astRoot = nullptr;
    Utf8           externalContent;
    bool           isExternal     = false;
    bool           lastBuffer     = true;

    uint64_t  writeTime       = 0;
    int       numErrors       = 0;
    int       unittestError   = 0;
    int       silent          = 0;
    uint32_t  indexInModule   = UINT32_MAX;
    BuildPass buildPass       = BuildPass::Full;
    bool      isBootstrapFile = false;

    char*    buffer       = nullptr;
    Scope*   scopePrivate = nullptr;
    AstNode* sourceNode   = nullptr;

    VectorNative<AstNode*> compilerPassFunctions;
    VectorNative<AstNode*> compilerPassUsing;
    shared_mutex           mutexCompilerPass;
    void                   addCompilerPassNode(AstNode* node);

    int          headerSize    = 0;
    long         fileSeek      = 0;
    long         bufferCurSeek = 0;
    long         bufferSize    = 0;
    int          totalRead     = 0;
    bool         doneLoading   = false;
    bool         formatDone    = false;
    bool         fromTests     = false;
    bool         generated     = false;
    shared_mutex mutexGetLine;
};
