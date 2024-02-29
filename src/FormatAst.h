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
    FormatAst() = default;

    FormatAst(FormatConcat& c) :
        concat{&c}
    {
    }

    static void        incIndentStatement(const AstNode* node, uint32_t& indent);
    static void decIndentStatement(const AstNode* node, uint32_t& indent);

    bool outputLambdaExpression(AstNode* node);
    bool outputEnum(AstEnum* node);
    bool outputFunc(AstFuncDecl* node);
    bool outputAttrUse(AstNode* node, bool& hasSomething);
    bool outputFuncName(const AstFuncDecl* node) const;
    bool outputFuncSignature(AstNode* node, AstNode* genericParameters, AstNode* parameters, AstNode* validIf);
    bool outputGenericParameters(AstNode* node);
    bool outputAttributesUsage(const TypeInfoFuncAttr* typeFunc);
    bool outputAttributes(AstNode* node, const TypeInfo* typeInfo, const AttributeList& attributes);
    bool outputAttributes(AstNode* node, TypeInfo* typeInfo);
    bool outputAttributesGlobalUsing(const AstNode* node);
    bool outputLiteral(AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    bool outputVarDecl(const AstVarDecl* varNode, bool isSelf);
    bool outputVar(const AstVarDecl* varNode);
    bool outputStruct(AstStruct* node);
    bool outputTypeTuple(TypeInfo* typeInfo);
    bool outputType(AstTypeExpression* node);
    bool outputType(AstNode* node, TypeInfo* typeInfo);
    bool outputScopeContent(const Module* moduleToGen, const Scope* scope);
    bool outputScope(Module* moduleToGen, Scope* scope);
    bool outputNode(AstNode* node);

    FormatConcat* concat    = nullptr;
    uint32_t      indent    = 0;
    bool          forExport = false;
};
