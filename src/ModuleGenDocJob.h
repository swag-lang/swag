#pragma once
#pragma once
#include "Job.h"
#include "AstOutput.h"
#include "Concat.h"
struct Scope;
struct AstStruct;
struct AstFuncDecl;

struct ModuleGenDocJob : public Job
{
    enum class UserBlockKind
    {
        Paragraph,
        RawParagraph,
        Code,
        Blockquote,
        Table,
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

    struct OneRef
    {
        Utf8                   category;
        Utf8                   tocName;
        Utf8                   fullName;
        Utf8                   displayName;
        VectorNative<AstNode*> nodes;
    };

    JobResult execute() override;

    void collectNode(AstNode* node);
    void collectScopes(Scope* root);

    void computeUserComments(UserComment& result, const Utf8& txt);
    Utf8 getDocComment(AstNode* node);
    int  sortOrder(AstNodeKind kind);

    Utf8 findReference(const Utf8& name);
    Utf8 getFormattedText(const Utf8& user);
    Utf8 getOutputNode(AstNode* node);
    Utf8 getOutputType(TypeInfo* typeInfo);
    void outputType(AstNode* node);
    void outputTable(Scope* scope, AstNodeKind kind, const char* title, uint32_t collectFlags);
    void outputUserBlock(const UserBlock& user);
    void outputUserComment(const UserComment& user);
    void outputCode(const Utf8& code);
    void outputStyles();
    void outputTitle(OneRef& c);

    void generateTocCateg(bool& first, AstNodeKind kind, const char* sectionName, const char* categName, Vector<OneRef*>& pendingNodes);
    void generateTocSection(AstNodeKind kind, const char* name);
    void generateToc();
    void generateContent();

    Concat                   concat;
    AstOutput                output;
    AstOutput::OutputContext outputCxt;
    Utf8                     helpContent;
    Utf8                     fullFileName;
    Utf8                     fileName;

    MapUtf8<VectorNative<AstNode*>> collect;
    MapUtf8<Utf8>                   collectInvert;
    Vector<OneRef>                  allNodes;
};
