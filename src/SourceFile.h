#pragma once
#include "Utf8.h"
#include "CommandLine.h"
#include "Mutex.h"
struct Module;
struct AstNode;
struct Diagnostic;
struct Scope;
struct Token;
struct AstAttrUse;
struct AstIdentifierRef;
struct Scope;

struct SourceFile
{
    bool     load();
    uint32_t getChar(unsigned& offset);
    Utf8     getLine(long lineNo);

    void reportNotes(const vector<const Diagnostic*>& notes, bool verbose = false);
    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);
    bool internalError(AstNode* node, const char* msg);

    bool checkFormat();
    void setExternalBuffer(char* buf, uint32_t size);
    void computeFileScopeName();
    void addGlobalUsing(Scope* scope);

    SharedMutex mutex;
    Utf8        name;
    string      path;

    vector<string> allLines;
    int            getLineOffset = 0;
    Module*        module        = nullptr;
    AstNode*       astRoot       = nullptr;
    AstAttrUse*    astAttrUse    = nullptr;
    Utf8           externalContent;
    Utf8           scopeName;
    bool           isExternal            = false;
    SourceFile*    fileForSourceLocation = nullptr;
    vector<Scope*> globalUsings;

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

    Scope*   scopeFile  = nullptr;
    AstNode* sourceNode = nullptr;
    Module*  imported   = nullptr;

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
