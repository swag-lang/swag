#pragma once
#include "CommandLine.h"
#include "Flags.h"
#include "Mutex.h"
#include "Tokenizer.h"

struct Module;
struct AstNode;
struct Diagnostic;
struct Scope;
struct Token;
struct AstAttrUse;
struct AstIdentifierRef;
struct Scope;

using AttributeFlags = Flags<uint64_t>;

using FileFlags = Flags<uint32_t>;
constexpr FileFlags FILE_IS_EXTERNAL         = 0x00000001;
constexpr FileFlags FILE_IS_FROM_AST         = 0x00000002;
constexpr FileFlags FILE_IS_CFG_FILE         = 0x00000004;
constexpr FileFlags FILE_IS_GENERATED        = 0x00000008;
constexpr FileFlags FILE_IS_BOOTSTRAP_FILE   = 0x00000010;
constexpr FileFlags FILE_IS_RUNTIME_FILE     = 0x00000020;
constexpr FileFlags FILE_IS_SCRIPT_FILE      = 0x00000040;
constexpr FileFlags FILE_IS_EMBEDDED         = 0x00000080;
constexpr FileFlags FILE_SHOULD_HAVE_ERROR   = 0x00000100;
constexpr FileFlags FILE_SHOULD_HAVE_WARNING = 0x00000200;
constexpr FileFlags FILE_FROM_TESTS          = 0x00000400;
constexpr FileFlags FILE_FORCE_EXPORT        = 0x00000800;
constexpr FileFlags FILE_MARK_DOWN           = 0x00001000;

struct SourceFile
{
    void release() const;
    bool checkFormat();
    bool load();
    Utf8 getLine(uint32_t lineNo, bool* eof = nullptr);
    void setExternalBuffer(const Utf8& content);
    void addGlobalUsing(Scope* scope);

    bool hasFlag(FileFlags fl) const { return flags.has(fl); }
    void addFlag(FileFlags fl) { flags.add(fl); }
    void removeFlag(FileFlags fl) { flags.remove(fl); }

    SharedMutex            mutex;
    Vector<Utf8>           allLines;
    VectorNative<Scope*>   globalUsing;
    VectorNative<AstNode*> globalUsingEmbedded;
    Utf8                   name;
    Path                   path;
    Utf8                   externalContent;
    Vector<Utf8>           shouldHaveErrorString;
    Vector<Utf8>           shouldHaveWarningString;
    Token                  tokenHasError;
    Token                  tokenHasWarning;

    Module*     module                = nullptr;
    AstNode*    astRoot               = nullptr;
    AstAttrUse* astAttrUse            = nullptr;
    SourceFile* fileForSourceLocation = nullptr;
    Scope*      scopeFile             = nullptr;
    AstNode*    fromNode              = nullptr;
    Module*     imported              = nullptr;
    char*       buffer                = nullptr;

    uint64_t       writeTime  = 0;
    AttributeFlags globalAttr = 0;

    FileFlags flags             = 0;
    uint32_t  offsetStartBuffer = 0;
    uint32_t  bufferSize        = 0;
    uint32_t  allocBufferSize   = 0;
    uint32_t  offsetGetLine     = 0;
    uint32_t  numErrors         = 0;
    uint32_t  numWarnings       = 0;
    uint32_t  indexInModule     = UINT32_MAX;
    uint32_t  privateId         = 0;
    BuildPass buildPass         = BuildPass::Full;
};
