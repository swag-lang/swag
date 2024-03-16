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

    static const AstNode* convertNode(FormatContext& context, const AstNode* node);
    bool                  outputChildren(FormatContext& context, const AstNode* node, uint32_t start = 0);
    bool                  outputCommaChildren(FormatContext& context, const AstNode* node, uint32_t start = 0);
    bool                  outputStatement(FormatContext& context, const AstNode* node);
    bool                  outputDoStatement(FormatContext& context, const AstNode* node);

    void beautifyComment(FormatContext& context, Vector<TokenComment>& comments) const;
    void beautifyBefore(FormatContext& context, const AstNode* node) const;
    void beautifyCommentBefore(FormatContext& context, const AstNode* node) const;
    void beautifyCommentJustBefore(FormatContext& context, const AstNode* node) const;
    void beautifyAfter(FormatContext& context, const AstNode* node) const;
    void beautifyBlankLine(FormatContext& context, const AstNode* node) const;

    bool outputClosureArguments(FormatContext& context, const AstFuncDecl* funcNode);
    bool outputLambdaExpression(FormatContext& context, const AstNode* node);
    bool outputEnum(FormatContext& context, const AstEnum* node);
    bool outputFuncDecl(FormatContext& context, const AstFuncDecl* node);
    bool outputAttrUse(FormatContext& context, const AstNode* node, bool& hasSomething);
    bool outputAttrUse(FormatContext& context, const AstAttrUse* node);
    bool outputFuncCallParams(FormatContext& context, const AstNode* node);
    bool outputInit(FormatContext& context, const AstNode* node);
    bool outputDropCopyMove(FormatContext& context, const AstNode* node);
    bool outputTypeLambda(FormatContext& context, const AstNode* node);
    bool outputFuncDeclParameters(FormatContext& context, const AstNode* parameters, bool isMethod);
    bool outputFuncReturnType(FormatContext& context, const AstFuncDecl* funcNode);
    bool outputFuncSignature(FormatContext& context, const AstNode* node, const AstNode* genericParameters, const AstNode* parameters, const AstNode* validIf);
    bool outputGenericParameters(FormatContext& context, const AstNode* node);
    bool outputAttributesUsage(FormatContext& context, const TypeInfoFuncAttr* typeFunc) const;
    bool outputAttributes(FormatContext& context, const TypeInfo* typeInfo, const AttributeList& attributes);
    bool outputAttributes(FormatContext& context, const AstNode* node, TypeInfo* typeInfo);
    bool outputAttributesGlobalUsing(FormatContext& context, const AstNode* node) const;
    bool outputCompilerSpecialValue(FormatContext& context, const AstNode* node) const;
    bool outputCompilerIf(FormatContext& context, const Utf8& name, const AstNode* node);
    bool outputCompilerExpr(FormatContext& context, const AstNode* node);
    bool outputCompilerExport(FormatContext& context, const AstNode* node) const;
    bool outputCompilerCode(FormatContext& context, const AstNode* node);
    bool outputCompilerGlobal(FormatContext& context, const AstNode* node);
    bool outputCompilerMixin(FormatContext& context, const AstNode* node);
    bool outputExpressionList(FormatContext& context, const AstNode* node);
    bool outputLiteral(FormatContext& context, const AstNode* node);
    bool outputLiteral(FormatContext& context, const AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    bool outputVarDecl(FormatContext& context, const AstVarDecl* varNode, bool isSelf);
    bool outputVar(FormatContext& context, const AstVarDecl* varNode);
    bool outputStructDeclContent(FormatContext& context, const AstNode* node);
    bool outputTupleDeclContent(FormatContext& context, const AstNode* node);
    bool outputStructDecl(FormatContext& context, const AstStruct* node);
    bool outputTypeTuple(FormatContext& context, TypeInfo* typeInfo);
    bool outputTypeExpression(FormatContext& context, const AstNode* node);
    bool outputType(FormatContext& context, const AstTypeExpression* node);
    
    bool outputScopeContent(FormatContext& context, const Module* module, const Scope* scope);
    bool outputScopeContentAndChilds(FormatContext& context, Module* module, const Scope* scope);
    bool outputScopeBlock(FormatContext& context, Module* module, const Scope* scope);
    bool outputScope(FormatContext& context, Module* module, Scope* scope);
    
    bool outputWhile(FormatContext& context, const AstNode* node);
    bool outputLoop(FormatContext& context, const AstNode* node);
    bool outputVisit(FormatContext& context, const AstNode* node);
    bool outputFor(FormatContext& context, const AstNode* node);
    bool outputSwitch(FormatContext& context, const AstNode* node);
    bool outputIf(FormatContext& context, const Utf8& name, const AstNode* node);
    bool outputNamespace(FormatContext& context, const AstNode* node);
    bool outputDefer(FormatContext& context, const AstNode* node);
    bool outputTryAssume(FormatContext& context, const AstNode* node);
    bool outputCatch(FormatContext& context, const AstNode* node);
    bool outputImpl(FormatContext& context, const AstNode* node);
    bool outputIdentifier(FormatContext& context, const AstNode* node);
    bool outputIdentifierRef(FormatContext& context, const AstNode* node);
    bool outputArrayPointerSlicing(FormatContext& context, const AstNode* node);
    bool outputArrayPointerIndex(FormatContext& context, const AstNode* node);
    bool outputAffectOp(FormatContext& context, const AstNode* node);
    bool outputFactorOp(FormatContext& context, const AstNode* node);
    bool outputBinaryOp(FormatContext& context, const AstNode* node);
    bool outputNullConditionalExpression(FormatContext& context, const AstNode* node);
    bool outputConditionalExpression(FormatContext& context, const AstNode* node);
    bool outputCast(FormatContext& context, const AstNode* node);
    bool outputNode(FormatContext& context, const AstNode* node, bool cmtAfter = true);

    FormatConcat  inConcat;
    FormatConcat* concat   = nullptr;
    FormatFlags   fmtFlags = 0;
};
