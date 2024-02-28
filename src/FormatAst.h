#pragma once
#include "FormatConcat.h"
#include "Job.h"

struct AstEnum;
struct AstNode;
struct AstStruct;
struct AstTypeExpression;
struct AstVarDecl;
struct AttributeList;
struct ComputedValue;
struct FormatConcat;
struct Scope;
struct TypeInfo;
struct TypeInfoEnum;
struct TypeInfoFuncAttr;

enum class ScopeKind : uint8_t;

struct FormatAst
{
    struct OutputContext : JobContext
    {
        AstNode* exportedNode = nullptr;
        int      indent       = 0;
        bool     forExport    = false;
    };

    static void incIndentStatement(const AstNode* node, int& indent);
    static void decIndentStatement(const AstNode* node, int& indent);
    static void removeLastBlankLine(FormatConcat& concat);

    static bool outputLambdaExpression(OutputContext& context, FormatConcat& concat, AstNode* node);
    static bool outputEnum(OutputContext& context, FormatConcat& concat, AstEnum* node);
    static bool outputFunc(OutputContext& context, FormatConcat& concat, AstFuncDecl* node);
    static bool outputAttrUse(OutputContext& context, FormatConcat& concat, AstNode* node, bool& hasSomething);
    static bool outputFuncName(OutputContext& context, FormatConcat& concat, const AstFuncDecl* node);
    static bool outputFuncSignature(OutputContext& context, FormatConcat& concat, AstNode* node, AstNode* genericParameters, AstNode* parameters, AstNode* validIf);
    static bool outputGenericParameters(OutputContext& context, FormatConcat& concat, AstNode* node);
    static bool outputAttributesUsage(const OutputContext& context, FormatConcat& concat, const TypeInfoFuncAttr* typeFunc);
    static bool outputAttributes(OutputContext& context, FormatConcat& concat, AstNode* node, const TypeInfo* typeInfo, const AttributeList& attributes);
    static bool outputAttributes(OutputContext& context, FormatConcat& concat, AstNode* node, TypeInfo* typeInfo);
    static bool outputAttributesGlobalUsing(const OutputContext& context, FormatConcat& concat, const AstNode* node);
    static bool outputLiteral(OutputContext& context, FormatConcat& concat, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    static bool outputVarDecl(OutputContext& context, FormatConcat& concat, const AstVarDecl* varNode, bool isSelf);
    static bool outputVar(OutputContext& context, FormatConcat& concat, const AstVarDecl* varNode);
    static bool outputStruct(OutputContext& context, FormatConcat& concat, AstStruct* node);
    static bool outputTypeTuple(OutputContext& context, FormatConcat& concat, TypeInfo* typeInfo);
    static bool outputType(OutputContext& context, FormatConcat& concat, AstTypeExpression* node);
    static bool outputType(OutputContext& context, FormatConcat& concat, AstNode* node, TypeInfo* typeInfo);
    static bool outputScopeContent(OutputContext& context, FormatConcat& concat, const Module* moduleToGen, const Scope* scope);
    static bool outputScope(OutputContext& context, FormatConcat& concat, Module* moduleToGen, Scope* scope);
    static bool outputNode(OutputContext& context, FormatConcat& concat, AstNode* node);
};

struct ScopeExportNode
{
    ScopeExportNode(FormatAst::OutputContext& context, AstNode* node)
    {
        savedContext         = &context;
        savedExportedNode    = context.exportedNode;
        context.exportedNode = node;
    }

    ~ScopeExportNode()
    {
        savedContext->exportedNode = savedExportedNode;
    }

    FormatAst::OutputContext* savedContext;
    AstNode*                  savedExportedNode;
};
