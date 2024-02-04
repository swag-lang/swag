#pragma once
#include "Job.h"

struct AstEnum;
struct AstNode;
struct AstStruct;
struct AstTypeExpression;
struct AstVarDecl;
struct AttributeList;
struct ComputedValue;
struct Concat;
struct Scope;
struct TypeInfo;
struct TypeInfoEnum;
struct TypeInfoFuncAttr;

enum class ScopeKind : uint8_t;

struct AstOutput
{
    struct OutputContext : JobContext
    {
        AstNode* exportedNode = nullptr;
        int      indent       = 0;
        bool     forExport    = false;
    };

    static void incIndentStatement(const AstNode* node, int& indent);
    static void decIndentStatement(const AstNode* node, int& indent);
    static void removeLastBlankLine(Concat& concat);

    static bool outputLambdaExpression(OutputContext& context, Concat& concat, AstNode* node);
    static bool outputEnum(OutputContext& context, Concat& concat, AstEnum* node);
    static bool outputFunc(OutputContext& context, Concat& concat, AstFuncDecl* node);
    static bool outputAttrUse(OutputContext& context, Concat& concat, AstNode* node, bool& hasSomething);
    static bool outputFuncName(OutputContext& context, Concat& concat, const AstFuncDecl* node);
    static bool outputFuncSignature(OutputContext& context, Concat& concat, AstNode* node, AstNode* genericParameters, AstNode* parameters, AstNode* validif);
    static bool outputGenericParameters(OutputContext& context, Concat& concat, AstNode* node);
    static bool outputAttributesUsage(const OutputContext& context, Concat& concat, const TypeInfoFuncAttr* typeFunc);
    static bool outputAttributes(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const AttributeList& attributes);
    static bool outputAttributes(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo);
    static bool outputAttributesGlobalUsing(const OutputContext& context, Concat& concat, const AstNode* node);
    static bool outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    static bool outputVarDecl(OutputContext& context, Concat& concat, const AstVarDecl* node, bool isSelf);
    static bool outputVar(OutputContext& context, Concat& concat, const AstVarDecl* node);
    static bool outputStruct(OutputContext& context, Concat& concat, AstStruct* node);
    static bool outputTypeTuple(OutputContext& context, Concat& concat, TypeInfo* typeInfo);
    static bool outputType(OutputContext& context, Concat& concat, AstTypeExpression* node);
    static bool outputType(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo);
    static bool outputScopeContent(OutputContext& context, Concat& concat, const Module* moduleToGen, const Scope* scope);
    static bool outputScope(OutputContext& context, Concat& concat, Module* moduleToGen, Scope* scope);
    static bool outputNode(OutputContext& context, Concat& concat, AstNode* node);
};

struct ScopeExportNode
{
    ScopeExportNode(AstOutput::OutputContext& context, AstNode* node)
    {
        savedContext         = &context;
        savedExportedNode    = context.exportedNode;
        context.exportedNode = node;
    }

    ~ScopeExportNode()
    {
        savedContext->exportedNode = savedExportedNode;
    }

    AstOutput::OutputContext* savedContext;
    AstNode*                  savedExportedNode;
};
