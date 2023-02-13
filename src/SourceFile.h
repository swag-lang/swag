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
    bool     checkFormat();
    bool     load();
    uint32_t getChar(unsigned& offset);
    Utf8     getLine(long lineNo, bool* eof = nullptr);
    void     setExternalBuffer(const Utf8& content);
    void     computeFileScopeName();
    void     addGlobalUsing(Scope* scope);

    SharedMutex            mutex;
    Vector<string>         allLines;
    VectorNative<Scope*>   globalUsings;
    VectorNative<AstNode*> globalUsingsEmbbed;
    Utf8                   name;
    string                 path;
    Utf8                   externalContent;
    Utf8                   scopeName;

    Module*     module                = nullptr;
    AstNode*    astRoot               = nullptr;
    AstAttrUse* astAttrUse            = nullptr;
    SourceFile* fileForSourceLocation = nullptr;
    Scope*      scopeFile             = nullptr;
    AstNode*    sourceNode            = nullptr;
    Module*     imported              = nullptr;
    char*       buffer                = nullptr;
    char*       curBuffer             = nullptr;
    char*       endBuffer             = nullptr;

    uint64_t writeTime = 0;

    long      bufferSize      = 0;
    long      allocBufferSize = 0;
    int       getLineOffset   = 0;
    int       numErrors       = 0;
    int       numWarnings     = 0;
    uint32_t  indexInModule   = UINT32_MAX;
    BuildPass buildPass       = BuildPass::Full;
    uint32_t  globalAttr      = 0;

    bool isExternal           = false;
    bool isCfgFile            = false;
    bool isGenerated          = false;
    bool isBootstrapFile      = false;
    bool isRuntimeFile        = false;
    bool isScriptFile         = false;
    bool isEmbbeded           = false;
    bool shouldHaveError   = false;
    bool shouldHaveWarning = false;
    bool fromTests            = false;
    bool forceExport          = false;
    bool duringSyntax         = false;
};
