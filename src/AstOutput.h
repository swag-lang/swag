#pragma once
#include "AstNode.h"
#include "Allocator.h"
#include "SourceFile.h"
#include "CommandLine.h"
struct Utf8;
struct Scope;
struct Concat;
struct TypeInfoEnum;
enum class ScopeKind;

struct AstOutput
{
    struct OutputContext : public JobContext
    {
        int      indent       = 0;
        bool     forExport    = false;
        AstNode* exportedNode = nullptr;
    };

    static bool checkIsPublic(OutputContext& context, AstNode* testNode, AstNode* usedNode);
    static void incIndentStatement(AstNode* node, int& indent);
    static void decIndentStatement(AstNode* node, int& indent);

    static bool outputLambdaExpression(OutputContext& context, Concat& concat, AstNode* node);
    static bool outputEnum(OutputContext& context, Concat& concat, AstNode* node);
    static bool outputFunc(OutputContext& context, Concat& concat, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    static bool outputFuncSignature(OutputContext& context, Concat& concat, AstNode* node, AstNode* parameters, AstNode* selectIf);
    static bool outputGenericParameters(OutputContext& context, Concat& concat, AstNode* node);
    static bool outputAttributesUsage(OutputContext& context, Concat& concat, TypeInfoFuncAttr* typeFunc);
    static bool outputAttributes(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo);
    static bool outputAttributesGlobalUsing(OutputContext& context, Concat& concat, AstNode* node);
    static bool outputAttributes(OutputContext& context, Concat& concat, AttributeList& attributes);
    static bool outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    static bool outputVar(OutputContext& context, Concat& concat, const char* kindName, AstVarDecl* node);
    static bool outputStruct(OutputContext& context, Concat& concat, TypeInfoStruct* typeStruct, AstStruct* node);
    static bool outputTypeTuple(OutputContext& context, Concat& concat, TypeInfo* typeInfo);
    static bool outputType(OutputContext& context, Concat& concat, TypeInfo* typeInfo, AstNode* node = nullptr);
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
