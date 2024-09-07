#pragma once
#include "Format/FormatAst.h"
#include "Syntax/AstNode.h"

struct Scope;
struct AstStruct;
struct AstFuncDecl;
enum class BuildCfgDocKind;

using GenDocFlags = Flags<uint32_t>;

static constexpr GenDocFlags GENDOC_CODE_REFS       = 0x00000001;
static constexpr GenDocFlags GENDOC_CODE_BLOCK      = 0x00000002;
static constexpr GenDocFlags GENDOC_CODE_SYNTAX_COL = 0x00000004;

struct GenDoc
{
    enum class UserBlockKind
    {
        Paragraph,
        ParagraphRaw,
        CodeSwag,
        CodeRaw,
        CodeAuto,
        Blockquote,
        BlockquoteNote,
        BlockquoteTip,
        BlockquoteWarning,
        BlockquoteAttention,
        BlockquoteExample,
        Table,
        ListStar,
        ListNeg,
        OrderedList,
        DescriptionList,
        Title1,
        Title2,
        Title3,
        Title4,
        Title5,
        Title6,
    };

    struct UserBlock
    {
        UserBlockKind            kind = UserBlockKind::Paragraph;
        VectorNative<UserBlock*> subBlocks;
        Vector<Utf8>             lines;

        bool empty() const { return subBlocks.empty() && lines.empty(); }
    };

    struct UserComment
    {
        UserBlock                shortDesc;
        VectorNative<UserBlock*> blocks;
    };

    void               constructPage();
    static Utf8        toRef(Utf8 str);
    Utf8               getTocTitleRef() const;
    void               addTocTitle(const Utf8& name, const Utf8& title, uint32_t titleLevel);
    static Utf8        getFileExtension(const Module* module);
    bool               constructAndSave();
    bool               generate(Module* mdl, BuildCfgDocKind kind);
    void               outputStyles();
    Utf8               findReference(const Utf8& name);
    static void        computeUserComments(UserComment& result, const Utf8& txt, bool shortDesc = true);
    static void        computeUserBlocks(VectorNative<UserBlock*>& blocks, const Vector<Utf8>& lines, bool shortDesc);
    static void        computeUserComments(UserComment& result, const Vector<Utf8>& lines, bool shortDesc = true);
    static const char* tokenizeReference(const char* pz, Utf8& name, Utf8& link, bool acceptLink = true);
    Utf8               getFormattedText(const Utf8& user);
    void               outputCode(const Utf8& code, GenDocFlags flags);
    void               outputUserBlock(const UserBlock& user, int titleLevel = 1, bool shortDescTd = false);
    void               outputUserComment(const UserComment& user, int titleLevel = 1);

    FormatAst       fmtAst;
    Module*         module = nullptr;
    BuildCfgDocKind docKind;
    Utf8            helpOutput;
    Utf8            helpToc;
    Utf8            helpContent;
    Utf8            helpCode;
    Utf8            fullFileName;
    MapUtf8<Utf8>   collectInvert;
    uint32_t        tocLastTitleLevel = 0;

    // Pages
    ///////////////////////////////////

    bool generatePages();

    // Examples
    ///////////////////////////////////

    bool generateExamples();
    void addTitle(const Utf8& title, int level);
    bool processMarkDownFile(const Path& fileName, int titleLevel);
    bool processSourceFile(const Path& fileName, int titleLevel);

    // Api
    ///////////////////////////////////

    struct OneRef
    {
        Utf8                   category;
        Utf8                   tocName;
        Utf8                   fullName;
        Utf8                   displayName;
        VectorNative<AstNode*> nodes;
    };

    bool        generateApi();
    void        collectNode(AstNode* node);
    void        collectScopes(Scope* root);
    static Utf8 getDocComment(const AstNode* node);
    Utf8        getOutputNode(AstNode* node);
    void        outputType(AstNode* node);
    void        outputTable(Scope* scope, AstNodeKind kind, const char* title, uint32_t collectFlags);
    void        outputTitle(OneRef& c);
    void        generateTocCategory(bool& first, AstNodeKind kind, const char* sectionName, const char* categName, Vector<OneRef*>& pendingNodes);
    void        generateTocSection(AstNodeKind kind, const char* sectionName);
    void        generateToc();
    void        generateContentFuncSignature(Utf8& code, const AstFuncDecl* funcNode);
    void        generateContentFunc(OneRef& c);
    void        generateContentAttr(OneRef& c);
    void        generateContentEnum(OneRef& c, AstNode* n0);
    void        generateContentStruct(OneRef& c, AstNode* n0);
    void        generateContentTypeAlias(uint32_t& i, OneRef& c);
    void        generateContentConst(uint32_t& i, OneRef& c);
    void        generateContentNamespace(OneRef& c, AstNode* n0);
    void        generateContent();

    FormatAst                       output;
    Utf8                            titleToc;
    Utf8                            titleContent;
    MapUtf8<VectorNative<AstNode*>> collect;
    Vector<OneRef>                  allNodes;
    Vector<Utf8>                    titleRefStack;
};
