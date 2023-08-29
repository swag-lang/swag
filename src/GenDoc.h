#pragma once
#include "Job.h"
#include "AstOutput.h"
#include "Concat.h"
struct Scope;
struct AstStruct;
struct AstFuncDecl;

static const uint32_t GENDOC_CODE_REFS       = 0x00000001;
static const uint32_t GENDOC_CODE_BLOCK      = 0x00000002;
static const uint32_t GENDOC_CODE_SYNTAX_COL = 0x00000004;

struct GenDoc
{
    enum class UserBlockKind
    {
        Paragraph,
        ParagraphRaw,
        CodeSwag,
        CodeRaw,
        Blockquote,
        Table,
        List,
        Title1,
        Title2,
        Title3,
    };

    struct UserBlock
    {
        UserBlockKind kind = UserBlockKind::Paragraph;
        Vector<Utf8>  lines;
    };

    struct UserComment
    {
        UserBlock         shortDesc;
        Vector<UserBlock> blocks;
    };

    void constructPage();
    Utf8 toRef(Utf8 str);
    void addTocTitle(const Utf8& name, const Utf8& title, int titleLevel);
    bool generate(Module* mdl, BuildCfgDocKind kind);
    void outputStyles();
    Utf8 findReference(const Utf8& name);
    void computeUserComments(UserComment& result, const Utf8& txt, bool shortDesc = true);
    void computeUserComments(UserComment& result, Vector<Utf8>& lines, bool shortDesc = true);
    Utf8 getFormattedText(const Utf8& user);
    void outputCode(const Utf8& code, uint32_t flags);
    void outputUserBlock(const UserBlock& user, int titleLevel = 1, bool shortDescTd = false);
    void outputUserComment(const UserComment& user, int titleLevel = 1);

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
    bool processMarkDownFile(const Path& fileName);
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
};
