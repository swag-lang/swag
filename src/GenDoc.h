#pragma once
#include "Job.h"
#include "AstOutput.h"
#include "Concat.h"
struct Scope;
struct AstStruct;
struct AstFuncDecl;

struct GenDoc
{
    enum class UserBlockKind
    {
        Paragraph,
        RawParagraph,
        List,
        Test,
        Code,
        Blockquote,
        Table,
        Title1,
        Title2,
    };

    Utf8 toRef(Utf8 str);
    bool generate(Module* mdl, DocKind docKind);
    void outputStyles();
    Utf8 findReference(const Utf8& name);
    Utf8 getReference(const Utf8& name);
    Utf8 getFormattedText(const Utf8& user);
    void outputCode(const Utf8& code);

    Module*       module = nullptr;
    Concat        concat;
    Utf8          helpOutput;
    Utf8          helpToc;
    Utf8          helpContent;
    Utf8          helpCode;
    Utf8          fullFileName;
    MapUtf8<Utf8> collectInvert;

    // Examples
    ///////////////////////////////////

    bool generateExamples();
    void addTitle(const Utf8& title, int level);
    bool processFile(const Path& fileName, int titleLevel);
    void stateEnter(UserBlockKind st);
    void stateLeave(UserBlockKind st);
    void pushState(UserBlockKind st);
    void popState();

    Vector<UserBlockKind> state;

    // Api
    ///////////////////////////////////

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
    void computeUserComments(UserComment& result, const Utf8& txt);
    Utf8 getDocComment(AstNode* node);
    Utf8 getOutputNode(AstNode* node);
    void outputType(AstNode* node);
    void outputTable(Scope* scope, AstNodeKind kind, const char* title, uint32_t collectFlags);
    void outputUserBlock(const UserBlock& user);
    void outputUserComment(const UserComment& user);
    void outputTitle(OneRef& c);
    void generateTocCateg(bool& first, AstNodeKind kind, const char* sectionName, const char* categName, Vector<OneRef*>& pendingNodes);
    void generateTocSection(AstNodeKind kind, const char* name);
    void generateToc();
    void generateContent();

    AstOutput                       output;
    AstOutput::OutputContext        outputCxt;
    MapUtf8<VectorNative<AstNode*>> collect;
    Vector<OneRef>                  allNodes;
};
