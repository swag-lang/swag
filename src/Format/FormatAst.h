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
struct Parser;
struct Scope;
struct TokenComment;
struct TokenParse;
struct TypeInfo;
struct TypeInfoEnum;
struct TypeInfoFuncAttr;

enum class ScopeKind : uint8_t;
using FormatFlags  = Flags<uint32_t>;
using CollectFlags = Flags<uint32_t>;

constexpr CollectFlags STOP_CMT_BEFORE        = 0x00000001;
constexpr CollectFlags STOP_EMPTY_LINE_BEFORE = 0x00000002;

struct FormatContext
{
    uint32_t indent              = 0;
    uint32_t countFactorOp       = 0;
    uint32_t startFactorOpColumn = 0;
    uint32_t countBinaryOp       = 0;
    uint32_t startBinaryOpColumn = 0;
    bool     outputComments      = false;
    bool     outputBlankLines    = false;
    bool     beautifyAfter       = true;
    bool     canConcatStatement  = false;

    bool     alignVarDecl                           = false;
    bool     alignEnumValue                         = false;
    bool     alignAffectEqual                       = false;
    bool     alignShortFunc                         = false;
    bool     alignTypeAlias                         = false;
    uint32_t addBlanksBeforeAlignedLastLineComments = 0;
    uint32_t alignStructVarTypeAddBlanks            = 0;
    bool     keepSameLineStruct                     = false;
    bool     keepSameLineEnum                       = false;
    bool     keepSameLineFuncBody                   = false;
    bool     keepSameLineAffect                     = false;
    bool     keepSameCodeBlock                      = false;

    void setDefaultBeautify()
    {
        outputComments                         = true;
        outputBlankLines                       = true;
        addBlanksBeforeAlignedLastLineComments = 4;
        alignStructVarTypeAddBlanks            = 4;
        keepSameLineStruct                     = true;
        keepSameLineEnum                       = true;
        keepSameLineFuncBody                   = true;
        keepSameLineAffect                     = true;
        keepSameCodeBlock                      = true;
    }
};

struct FormatAst
{
    FormatAst()
    {
        concat = &inConcat;
        concat->init(4 * 1024);
        tmpConcat.init(1024);
    }

    explicit FormatAst(FormatConcat& c) :
        concat{&c}
    {
        tmpConcat.init(1024);
    }

    void clear() const;
    Utf8 getUtf8() const;

    static void              inheritLastFormatAfter(const Parser* parser, AstNode* node);
    static void              inheritFormatBefore(const Parser* parser, AstNode* node, AstNode* other);
    static void              inheritFormatAfter(const Parser* parser, AstNode* node, AstNode* other);
    static void              inheritFormatBefore(const Parser* parser, AstNode* node, TokenParse* tokenParse);
    static void              inheritFormatAfter(const Parser* parser, AstNode* node, TokenParse* tokenParse);
    static TokenParse*       getTokenParse(AstNode* node);
    static const TokenParse* getTokenParse(const AstNode* node);
    static TokenParse*       getOrCreateTokenParse(AstNode* node);

    static bool     hasEOLInside(AstNode* node);
    void            addBlank(const AstNode* next) const;
    static AstNode* convertNode(FormatContext& context, AstNode* node);
    void            addEOLOrSemi(const FormatContext& context, const AstNode* child) const;
    bool            outputChildrenEol(FormatContext& context, AstNode* node, uint32_t start = 0);
    bool            outputChildrenChar(FormatContext& context, AstNode* node, char betweenChar, char endChar, uint32_t start);
    bool            outputChildrenBlank(FormatContext& context, AstNode* node, uint32_t start = 0);
    bool            outputStatement(FormatContext& context, AstNode* node);
    bool            outputDoStatement(FormatContext& context, AstNode* node);
    static bool     collectChildrenToAlign(FormatContext& context, CollectFlags flags, AstNode* node, uint32_t start, VectorNative<AstNode*>& nodes, uint32_t& processed, const std::function<bool(AstNode*)>& stopFn);

    void beautifyComment(const FormatContext& context, Vector<TokenComment>& comments) const;
    void beautifyBefore(const FormatContext& context, AstNode* node) const;
    void beautifyCommentBefore(const FormatContext& context, AstNode* node) const;
    void beautifyCommentJustBefore(const FormatContext& context, AstNode* node) const;
    void beautifyAfter(const FormatContext& context, AstNode* node) const;
    void beautifyBlankLine(const FormatContext& context, AstNode* node) const;

    bool outputClosureArguments(FormatContext& context, const AstFuncDecl* funcNode);
    bool outputLambdaExpression(FormatContext& context, AstNode* node);
    bool outputChildrenEnumValues(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed);
    bool outputEnumValue(FormatContext& context, AstNode* node, uint32_t maxLenName = 0, uint32_t maxLenValue = 0);
    bool outputEnum(FormatContext& context, AstEnum* node);
    bool outputFuncDecl(FormatContext& context, AstNode* node, uint32_t maxLenSignature = 0);
    bool outputAttrUse(FormatContext& context, AstNode* node, bool& hasSomething);
    bool outputAttrUse(FormatContext& context, AstAttrUse* node);
    bool outputFuncCallParams(FormatContext& context, AstNode* node);
    bool outputChildrenFuncDecl(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed);
    bool outputInit(FormatContext& context, AstNode* node);
    bool outputDropCopyMove(FormatContext& context, AstNode* node);
    bool outputTypeLambda(FormatContext& context, AstNode* node);
    bool outputFuncDeclParameters(FormatContext& context, AstNode* parameters, bool isMethod);
    bool outputFuncReturnType(FormatContext& context, const AstFuncDecl* funcNode);
    bool outputFuncSignature(FormatContext& context, AstNode* node, AstNode* genericParameters, AstNode* parameters, const VectorNative<AstNode*> *constraints);
    bool outputGenericParameters(FormatContext& context, AstNode* node);
    bool outputChildrenTypeAlias(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed);
    bool outputTypeAlias(FormatContext& context, AstNode* node, uint32_t maxLenName = 0);
    bool outputAttributesUsage(const FormatContext& context, const TypeInfoFuncAttr* typeFunc) const;
    bool outputAttributes(FormatContext& context, const TypeInfo* typeInfo, const AttributeList& attributes);
    bool outputAttributes(FormatContext& context, const AstNode* node, TypeInfo* typeInfo);
    bool outputAttributesGlobalUsing(const FormatContext& context, const AstNode* node) const;
    bool outputCompilerSpecialValue(FormatContext& context, AstNode* node) const;
    bool outputCompilerIf(FormatContext& context, const Utf8& name, AstNode* node);
    bool outputCompilerExpr(FormatContext& context, const AstNode* node);
    bool outputCompilerExport(FormatContext& context, AstNode* node) const;
    bool outputCompilerCode(FormatContext& context, AstNode* node);
    bool outputCompilerGlobal(FormatContext& context, const AstNode* node);
    bool outputCompilerMixin(FormatContext& context, AstNode* node);
    bool outputExpressionList(FormatContext& context, AstNode* node);
    bool outputLiteral(FormatContext& context, AstNode* node);
    bool outputLiteral(FormatContext& context, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    bool outputChildrenMultiVar(const FormatContext& context, AstNode* node);
    bool outputChildrenVar(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed);
    bool outputVarName(FormatContext& context, const AstVarDecl* varNode) const;
    bool outputVarContent(FormatContext& context, AstNode* node, uint32_t startColumn = 0, uint32_t maxLenName = 0, uint32_t maxLenType = 0);
    bool outputVarHeader(FormatContext& context, AstNode* node);
    bool outputVar(FormatContext& context, AstNode* node, uint32_t maxLenName = 0, uint32_t maxLenType = 0);
    bool outputStructDeclContent(FormatContext& context, AstNode* node);
    bool outputTupleDeclContent(const FormatContext& context, AstNode* node);
    bool outputStructDecl(FormatContext& context, AstStruct* node);
    bool outputTypeTuple(const FormatContext& context, TypeInfo* typeInfo);
    bool outputTypeExpression(FormatContext& context, AstNode* node);
    bool outputType(FormatContext& context, AstTypeExpression* node);

    bool outputScopeContent(FormatContext& context, Module* module, const Scope* scope);
    bool outputScopeContentAndChildren(FormatContext& context, Module* module, const Scope* scope);
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
    bool outputCatch(FormatContext& context, const AstNode* node);
    bool outputImpl(FormatContext& context, AstNode* node);
    bool outputIdentifier(const FormatContext& context, AstNode* node);
    bool outputIdentifierRef(FormatContext& context, AstNode* node);
    bool outputArrayPointerSlicing(FormatContext& context, AstNode* node);
    bool outputArrayPointerIndex(FormatContext& context, AstNode* node);
    bool outputChildrenAffectEqual(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed);
    bool outputAffectOp(FormatContext& context, AstNode* node, uint32_t maxLenName = 0);
    bool outputFactorOp(FormatContext& context, const AstNode* node);
    bool outputBinaryOp(FormatContext& context, const AstNode* node);
    bool outputNullConditionalExpression(FormatContext& context, const AstNode* node);
    bool outputConditionalExpression(FormatContext& context, AstNode* node);
    bool outputCast(FormatContext& context, const AstNode* node);
    bool outputNode(FormatContext& context, AstNode* node);

    FormatConcat  inConcat;
    FormatConcat  tmpConcat;
    FormatConcat* concat = nullptr;
};

struct PushConcatFormatTmp
{
    explicit PushConcatFormatTmp(FormatAst* ast, const FormatContext& context)
    {
        cxt                  = context;
        cxt.outputComments   = false;
        cxt.outputBlankLines = false;
        savedAst             = ast;
        savedConcat          = savedAst->concat;
        savedAst->concat     = &savedAst->tmpConcat;
    }

    ~PushConcatFormatTmp()
    {
        savedAst->concat = savedConcat;
    }

    FormatContext cxt;
    FormatAst*    savedAst    = nullptr;
    FormatConcat* savedConcat = nullptr;
};
