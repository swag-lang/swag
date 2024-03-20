#pragma once
#include "Format/FormatConcat.h"
#include "Threading/Job.h"

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

struct FormatContext
{
    uint32_t indent = 0;
};

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

    static AstNode* convertNode(FormatContext& context, AstNode* node);
    auto            outputChildren(FormatContext& context, AstNode* node, uint32_t start = 0) -> bool;
    bool            outputCommaChildren(FormatContext& context, AstNode* node, uint32_t start = 0);
    bool            outputStatement(FormatContext& context, AstNode* node);
    bool            outputDoStatement(FormatContext& context, AstNode* node);

    void beautifyComment(const FormatContext& context, Vector<TokenComment>& comments) const;
    void beautifyBefore(const FormatContext& context, AstNode* node) const;
    void beautifyCommentBefore(const FormatContext& context, AstNode* node) const;
    void beautifyCommentJustBefore(const FormatContext& context, AstNode* node) const;
    void beautifyAfter(const FormatContext& context, AstNode* node) const;
    void beautifyBlankLine(const FormatContext& context, AstNode* node) const;

    bool outputClosureArguments(FormatContext& context, AstFuncDecl* funcNode);
    bool outputLambdaExpression(FormatContext& context, AstNode* node);
    bool outputEnumValue(FormatContext& context, AstNode* node);
    bool outputEnum(FormatContext& context, AstEnum* node);
    bool outputFuncDecl(FormatContext& context, AstFuncDecl* node);
    bool outputAttrUse(FormatContext& context, AstNode* node, bool& hasSomething);
    bool outputAttrUse(FormatContext& context, AstAttrUse* node);
    bool outputFuncCallParams(FormatContext& context, AstNode* node);
    bool outputInit(FormatContext& context, AstNode* node);
    bool outputDropCopyMove(FormatContext& context, AstNode* node);
    bool outputTypeLambda(FormatContext& context, AstNode* node);
    bool outputFuncDeclParameters(FormatContext& context, AstNode* parameters, bool isMethod);
    bool outputFuncReturnType(FormatContext& context, AstFuncDecl* funcNode);
    bool outputFuncSignature(FormatContext& context, AstNode* node, AstNode* genericParameters, AstNode* parameters, AstNode* validIf);
    bool outputGenericParameters(FormatContext& context, AstNode* node);
    bool outputAttributesUsage(FormatContext& context, const TypeInfoFuncAttr* typeFunc) const;
    bool outputAttributes(FormatContext& context, const TypeInfo* typeInfo, const AttributeList& attributes);
    bool outputAttributes(FormatContext& context, AstNode* node, TypeInfo* typeInfo);
    bool outputAttributesGlobalUsing(FormatContext& context, AstNode* node) const;
    bool outputCompilerSpecialValue(FormatContext& context, AstNode* node) const;
    bool outputCompilerIf(FormatContext& context, const Utf8& name, AstNode* node);
    bool outputCompilerExpr(FormatContext& context, AstNode* node);
    bool outputCompilerExport(FormatContext& context, AstNode* node) const;
    bool outputCompilerCode(FormatContext& context, AstNode* node);
    bool outputCompilerGlobal(FormatContext& context, AstNode* node);
    bool outputCompilerMixin(FormatContext& context, AstNode* node);
    bool outputExpressionList(FormatContext& context, AstNode* node);
    bool outputLiteral(FormatContext& context, AstNode* node);
    bool outputLiteral(FormatContext& context, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    bool outputVarDecl(FormatContext& context, AstVarDecl* varNode, bool isSelf);
    bool outputVar(FormatContext& context, AstVarDecl* varNode);
    bool outputStructDeclContent(FormatContext& context, AstNode* node);
    bool outputTupleDeclContent(FormatContext& context, AstNode* node);
    bool outputStructDecl(FormatContext& context, AstStruct* node);
    bool outputTypeTuple(FormatContext& context, TypeInfo* typeInfo);
    bool outputTypeExpression(FormatContext& context, AstNode* node);
    bool outputType(FormatContext& context, AstTypeExpression* node);

    bool outputScopeContent(FormatContext& context, const Module* module, const Scope* scope);
    bool outputScopeContentAndChilds(FormatContext& context, Module* module, const Scope* scope);
    bool outputScopeBlock(FormatContext& context, Module* module, const Scope* scope);
    bool outputScope(FormatContext& context, Module* module, Scope* scope);

    bool outputWhile(FormatContext& context, AstNode* node);
    bool outputLoop(FormatContext& context, AstNode* node);
    bool outputVisit(FormatContext& context, AstNode* node);
    bool outputFor(FormatContext& context, AstNode* node);
    bool outputSwitch(FormatContext& context, AstNode* node);
    bool outputIf(FormatContext& context, const Utf8& name, AstNode* node);
    bool outputNamespace(FormatContext& context, AstNode* node);
    bool outputDefer(FormatContext& context, AstNode* node);
    bool outputTryAssume(FormatContext& context, const AstNode* node);
    bool outputCatch(FormatContext& context, AstNode* node);
    bool outputImpl(FormatContext& context, AstNode* node);
    bool outputIdentifier(FormatContext& context, AstNode* node);
    bool outputIdentifierRef(FormatContext& context, AstNode* node);
    bool outputArrayPointerSlicing(FormatContext& context, AstNode* node);
    bool outputArrayPointerIndex(FormatContext& context, AstNode* node);
    bool outputAffectOp(FormatContext& context, AstNode* node);
    bool outputFactorOp(FormatContext& context, AstNode* node);
    bool outputBinaryOp(FormatContext& context, AstNode* node);
    bool outputNullConditionalExpression(FormatContext& context, AstNode* node);
    bool outputConditionalExpression(FormatContext& context, AstNode* node);
    bool outputCast(FormatContext& context, const AstNode* node);
    bool outputNode(FormatContext& context, AstNode* node, bool cmtAfter = true);

    FormatConcat  inConcat;
    FormatConcat* concat   = nullptr;
    FormatFlags   fmtFlags = 0;
};
