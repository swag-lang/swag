#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SourceLocation.h"
#include "CommandLine.h"
struct Module;
struct AstNode;
struct Diagnostic;
struct Scope;
struct Token;
struct AstAttrUse;

struct SourceFile
{
    bool     load();
    char     getPrivateChar();
    uint32_t getChar(unsigned& offset);
    Utf8     getLine(long lineNo);

    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);

    void releaseBuffer();
    bool checkFormat();
    void setExternalBuffer(char* buf, uint32_t size);
    void computePrivateScopeName();

    shared_mutex mutex;
    Utf8         name;
    string       path;

    vector<string> allLines;
    int            getLineOffset = 0;
    Module*        module        = nullptr;
    AstNode*       astRoot       = nullptr;
    AstAttrUse*    astAttrUse    = nullptr;
    Utf8           externalContent;
    Utf8           scopeName;
    bool           isExternal            = false;
    SourceFile*    fileForSourceLocation = nullptr;

    uint64_t    writeTime            = 0;
    int         numErrors            = 0;
    int         numWarnings          = 0;
    atomic<int> numTestErrors        = 0;
    atomic<int> numTestWarnings      = 0;
    atomic<int> numSemErrors         = 0;
    bool        multipleTestErrors   = false;
    bool        multipleTestWarnings = false;
    int         silent               = 0;
    uint32_t    indexInModule        = UINT32_MAX;
    BuildPass   buildPass            = BuildPass::Full;
    bool        isBootstrapFile      = false;
    bool        isRuntimeFile        = false;
    bool        isScriptFile         = false;

    Scope*   scopePrivate = nullptr;
    AstNode* sourceNode   = nullptr;
    Module*  imported     = nullptr;

    char* buffer    = nullptr;
    char* curBuffer = nullptr;
    char* endBuffer = nullptr;

    long bufferSize      = 0;
    long allocBufferSize = 0;

    bool fromTests   = false;
    bool isCfgFile   = false;
    bool isGenerated = false;
    bool forceExport = false;
};
