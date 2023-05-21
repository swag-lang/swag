#pragma once
#include "Job.h"
#include "AstNode.h"
#include "Allocator.h"
#include "SourceFile.h"
#include "CommandLine.h"
struct Utf8;
struct Scope;
struct Concat;
struct TypeInfoEnum;
enum class ScopeKind : uint8_t;

struct AstOutput
{
    struct OutputContext : public JobContext
    {
        AstNode*                  exportedNode = nullptr;
        int                       indent       = 0;
        bool                      forExport    = false;
        bool                      checkPublic  = true;
        function<Utf8(TypeInfo*)> exportType;
    };

    static bool checkIsPublic(OutputContext& context, AstNode* testNode, AstNode* usedNode);
    static void incIndentStatement(AstNode* node, int& indent);
    static void decIndentStatement(AstNode* node, int& indent);
    static void removeLastBlankLine(Concat& concat);

    static bool outputLambdaExpression(OutputContext& context, Concat& concat, AstNode* node);
    static bool outputEnum(OutputContext& context, Concat& concat, AstEnum* node);
    static bool outputFunc(OutputContext& context, Concat& concat, AstFuncDecl* node);
    static bool outputAttrUse(OutputContext& context, Concat& concat, AstNode* node, bool& hasSomething);
    static bool outputFuncSignature(OutputContext& context, Concat& concat, AstNode* node, AstNode* genericParameters, AstNode* parameters, AstNode* validif);
    static bool outputGenericParameters(OutputContext& context, Concat& concat, AstNode* node);
    static bool outputAttributesUsage(OutputContext& context, Concat& concat, TypeInfoFuncAttr* typeFunc);
    static bool outputAttributes(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, AttributeList& attributes);
    static bool outputAttributes(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo);
    static bool outputAttributesGlobalUsing(OutputContext& context, Concat& concat, AstNode* node);
    static bool outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    static bool outputVarDecl(OutputContext& context, Concat& concat, AstVarDecl* node, bool isSelf, bool kindSpecified);
    static bool outputVar(OutputContext& context, Concat& concat, AstVarDecl* node);
    static bool outputStruct(OutputContext& context, Concat& concat, AstStruct* node);
    static bool outputTypeTuple(OutputContext& context, Concat& concat, TypeInfo* typeInfo);
    static bool outputType(OutputContext& context, Concat& concat, AstTypeExpression* node);
    static bool outputType(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo);
    static bool outputScopeContent(OutputContext& context, Concat& concat, Module* moduleToGen, Scope* scope);
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
