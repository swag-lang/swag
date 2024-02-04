#pragma once
#include "AstNode.h"
#include "AstOutput.h"
#include "Concat.h"
#include "Job.h"

struct Scope;
struct AstStruct;
struct AstFuncDecl;
enum class BuildCfgDocKind;

static constexpr uint32_t GENDOC_CODE_REFS       = 0x00000001;
static constexpr uint32_t GENDOC_CODE_BLOCK      = 0x00000002;
static constexpr uint32_t GENDOC_CODE_SYNTAX_COL = 0x00000004;

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
        List,
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
        UserBlockKind      kind = UserBlockKind::Paragraph;
        Vector<UserBlock*> subBlocks;
        Vector<Utf8>       lines;
    };

    struct UserComment
    {
        UserBlock          shortDesc;
        Vector<UserBlock*> blocks;
    };

    void        constructPage();
    Utf8        toRef(Utf8 str);
    Utf8        getTocTitleRef();
    void        addTocTitle(const Utf8& name, const Utf8& title, int titleLevel);
    Utf8        getFileExtension(Module* module);
    bool        generate(Module* mdl, BuildCfgDocKind kind);
    void        outputStyles();
    Utf8        findReference(const Utf8& name);
    void        computeUserComments(UserComment& result, const Utf8& txt, bool shortDesc = true);
    void        computeUserBlocks(Vector<UserBlock*>& blocks, Vector<Utf8>& lines, bool shortDesc);
    void        computeUserComments(UserComment& result, Vector<Utf8>& lines, bool shortDesc = true);
    const char* tokenizeReference(const char* pz, Utf8& name, Utf8& link, bool acceptLink = true);
    Utf8        getFormattedText(const Utf8& user);
    void        outputCode(const Utf8& code, uint32_t flags);
    void        outputUserBlock(const UserBlock& user, int titleLevel = 1, bool shortDescTd = false);
    void        outputUserComment(const UserComment& user, int titleLevel = 1);

    Module*         module = nullptr;
    BuildCfgDocKind docKind;
    Concat          concat;
    Utf8            helpOutput;
    Utf8            helpToc;
    Utf8            helpContent;
    Utf8            helpCode;
    Utf8            fullFileName;
    MapUtf8<Utf8>   collectInvert;
    int             tocLastTitleLevel = 0;

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

    bool generateApi();
    void collectNode(AstNode* node);
    void collectScopes(Scope* root);
    Utf8 getDocComment(AstNode* node);
    Utf8 getOutputNode(AstNode* node);
    void outputType(AstNode* node);
    void outputTable(Scope* scope, AstNodeKind kind, const char* title, uint32_t collectFlags);
    void outputTitle(OneRef& c);
    void generateTocCateg(bool& first, AstNodeKind kind, const char* sectionName, const char* categName, Vector<OneRef*>& pendingNodes);
    void generateTocSection(AstNodeKind kind, const char* name);
    void generateToc();
    void generateContent();

    Utf8                            titleToc;
    Utf8                            titleContent;
    AstOutput                       output;
    AstOutput::OutputContext        outputCxt;
    MapUtf8<VectorNative<AstNode*>> collect;
    Vector<OneRef>                  allNodes;
    Vector<Utf8>                    titleRefStack;
};
