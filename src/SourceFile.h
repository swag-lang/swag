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
    void release();
    bool checkFormat();
    bool load();
    Utf8 getLine(uint32_t lineNo, bool* eof = nullptr);
    void setExternalBuffer(const Utf8& content);
    void addGlobalUsing(Scope* scope);

    SharedMutex            mutex;
    Vector<Utf8>           allLines;
    VectorNative<Scope*>   globalUsings;
    VectorNative<AstNode*> globalUsingsEmbedded;
    Utf8                   name;
    Path                   path;
    Utf8                   externalContent;
    Vector<Utf8>           shouldHaveErrorString;
    Vector<Utf8>           shouldHaveWarningString;

    Module*     module                = nullptr;
    AstNode*    astRoot               = nullptr;
    AstAttrUse* astAttrUse            = nullptr;
    SourceFile* fileForSourceLocation = nullptr;
    Scope*      scopeFile             = nullptr;
    AstNode*    sourceNode            = nullptr;
    Module*     imported              = nullptr;
    char*       buffer                = nullptr;

    uint64_t writeTime  = 0;
    uint64_t globalAttr = 0;

    uint32_t  offsetStartBuffer = 0;
    uint32_t  bufferSize        = 0;
    uint32_t  allocBufferSize   = 0;
    uint32_t  offsetGetLine     = 0;
    uint32_t  numErrors         = 0;
    uint32_t  numWarnings       = 0;
    uint32_t  indexInModule     = UINT32_MAX;
    BuildPass buildPass         = BuildPass::Full;

    bool isExternal        = false;
    bool isCfgFile         = false;
    bool isGenerated       = false;
    bool isBootstrapFile   = false;
    bool isRuntimeFile     = false;
    bool isScriptFile      = false;
    bool isEmbedded        = false;
    bool shouldHaveError   = false;
    bool shouldHaveWarning = false;
    bool fromTests         = false;
    bool forceExport       = false;
    bool markDown          = false;
};
