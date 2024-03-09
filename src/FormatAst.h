#pragma once
#include "FormatConcat.h"
#include "Job.h"

struct AstAttrUse;
struct AstEnum;
struct AstNode;
struct AstStruct;
struct AstTypeExpression;
struct AstVarDecl;
struct AttributeList;
struct ComputedValue;
struct FormatConcat;
struct Scope;
struct TokenComment;
struct TypeInfo;
struct TypeInfoEnum;
struct TypeInfoFuncAttr;

enum class ScopeKind : uint8_t;
using FormatFlags = Flags<uint32_t>;

constexpr FormatFlags FORMAT_FOR_EXPORT   = 0x00000001;
constexpr FormatFlags FORMAT_FOR_BEAUTIFY = 0x00000002;

struct FormatAst
{
    FormatAst()
    {
        concat = &inConcat;
        concat->init(4 * 1024);
    }

    explicit FormatAst(FormatConcat& c) :
        concat{&c}
    {
    }

    void clear() const;
    Utf8 getUtf8() const;

    static const AstNode* convertNode(const AstNode* node);
    bool                  outputChildren(const AstNode* node, uint32_t start = 0);
    bool                  outputCommaChildren(const AstNode* node, uint32_t start = 0);
    bool                  outputStatement(const AstNode* node);
    bool                  outputDoStatement(const AstNode* node);

    void beautifyComment(const Vector<TokenComment>& comments) const;
    void beautifyCommentBefore(const AstNode* node) const;
    void beautifyCommentJustBefore(const AstNode* node) const;
    void beautifyCommentAfterSameLine(const AstNode* node) const;
    void beautifyBlankLine(const AstNode* node) const;

    bool outputLambdaExpression(const AstNode* node);
    bool outputEnum(const AstEnum* node);
    bool outputFuncDecl(const AstFuncDecl* node);
    bool outputAttrUse(const AstNode* node, bool& hasSomething);
    bool outputAttrUse(const AstAttrUse* node);
    bool outputFuncCallParams(const AstNode* node);
    bool outputTypeLambda(const AstNode* node);
    bool outputFuncDeclParameters(const AstNode* parameters, bool isMethod);
    bool outputFuncReturnType(const AstFuncDecl* funcNode);
    bool outputFuncSignature(const AstNode* node, const AstNode* genericParameters, const AstNode* parameters, const AstNode* validIf);
    bool outputGenericParameters(const AstNode* node);
    bool outputAttributesUsage(const TypeInfoFuncAttr* typeFunc) const;
    bool outputAttributes(const TypeInfo* typeInfo, const AttributeList& attributes);
    bool outputAttributes(const AstNode* node, TypeInfo* typeInfo);
    bool outputAttributesGlobalUsing(const AstNode* node) const;
    bool outputCompilerSpecialValue(const AstNode* node) const;
    bool outputCompilerIf(const Utf8& name, const AstNode* node);
    bool outputCompilerExpr(const AstNode* node);
    bool outputCompilerMixin(const AstNode* node);
    bool outputLiteral(const AstNode* node);
    bool outputLiteral(const AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    bool outputVarDecl(const AstVarDecl* varNode, bool isSelf);
    bool outputVar(const AstVarDecl* varNode);
    bool outputStructDeclContent(const AstNode* node);
    bool outputTupleDeclContent(const AstNode* node);
    bool outputStructDecl(const AstStruct* node);
    bool outputTypeTuple(TypeInfo* typeInfo);
    bool outputType(const AstTypeExpression* node);
    bool outputType(const AstNode* node, TypeInfo* typeInfo);
    bool outputScopeContent(const Module* module, const Scope* scope);
    bool outputScopeContentAndChilds(Module* module, const Scope* scope);
    bool outputScopeBlock(Module* module, const Scope* scope);
    bool outputScope(Module* module, Scope* scope);
    bool outputWhile(const AstNode* node);
    bool outputLoop(const AstNode* node);
    bool outputVisit(const AstNode* node);
    bool outputFor(const AstNode* node);
    bool outputSwitch(const AstNode* node);
    bool outputIf(const Utf8& name, const AstNode* node);
    bool outputNamespace(const AstNode* node);
    bool outputImpl(const AstNode* node);
    bool outputIdentifier(const AstNode* node);
    bool outputIdentifierRef(const AstNode* node);

    bool outputNode(const AstNode* node, bool cmtAfter = true);

    FormatConcat  inConcat;
    FormatConcat* concat   = nullptr;
    uint32_t      indent   = 0;
    FormatFlags   fmtFlags = 0;
};
