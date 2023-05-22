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
        Utf8                   fullName;
        Utf8                   displayName;
        VectorNative<AstNode*> nodes;
    };

    JobResult execute() override;

    void collectNode(AstNode* node);
    void collectScopes(Scope* root);

    void computeUserComment(UserComment& result, const Utf8& txt);
    Utf8 getDocComment(AstNode* node);

    Utf8 outputType(TypeInfo* typeInfo);
    Utf8 outputNode(AstNode* node);
    void outputUserLine(const Utf8& user);
    void outputUserBlock(const UserBlock& user);
    void outputUserComment(const UserComment& user);
    void outputCode(const Utf8& code);
    void outputStyles();
    void outputTitle(OneRef& c);

    void generateToc();

    Concat                   concat;
    AstOutput                output;
    AstOutput::OutputContext outputCxt;
    Utf8                     helpContent;

    MapUtf8<VectorNative<AstNode*>> collect;
    Vector<OneRef>                  allNodes;
};
