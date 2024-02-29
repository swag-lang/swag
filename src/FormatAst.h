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

    static const AstNode* convertNode(const AstNode* node);
    bool                  outputChildren(const AstNode* node);
    bool                  outputCommaChildren(const AstNode* node);

    bool outputLambdaExpression(const AstNode* node);
    bool outputEnum(const AstEnum* node);
    bool outputFuncDeclReturnType(const AstNode* node);
    bool outputFuncDeclParams(const AstNode* node);
    bool outputFuncDecl(const AstFuncDecl* node);
    bool outputAttrUse(const AstNode* node, bool& hasSomething);
    bool outputFuncCallParams(const AstNode* node);
    bool outputFuncName(const AstFuncDecl* node) const;
    bool outputFuncSignature(AstNode* node, const AstNode* genericParameters, const AstNode* parameters, const AstNode* validIf);
    bool outputGenericParameters(const AstNode* node);
    bool outputAttributesUsage(const TypeInfoFuncAttr* typeFunc) const;
    bool outputAttributes(const AstNode* node, const TypeInfo* typeInfo, const AttributeList& attributes);
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
    bool outputIf(const Utf8& name, const AstNode* node);

    bool outputNode(const AstNode* node);

    FormatConcat* concat    = nullptr;
    uint32_t      indent    = 0;
    bool          forExport = false;
};
