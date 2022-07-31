#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "AstOutput.h"
#include "Module.h"

bool AstOutput::checkIsPublic(OutputContext& context, AstNode* testNode, AstNode* usedNode)
{
    if (!context.forExport || !testNode)
        return true;

    auto symbol   = testNode->resolvedSymbolName;
    auto overload = testNode->resolvedSymbolOverload;
    if (!symbol || !overload)
        return true;

    if (overload->node->sourceFile->isBootstrapFile ||
        overload->node->sourceFile->isRuntimeFile ||
        overload->node->sourceFile->forceExport ||
        overload->node->sourceFile->imported)
        return true;

    if (symbol->name.empty() || symbol->name[0] == '@')
        return true;
    if (!overload->node->ownerScope->isGlobalOrImpl())
        return true;

    if (((symbol->kind == SymbolKind::Variable) && (overload->flags & OVERLOAD_VAR_GLOBAL)) ||
        (symbol->kind == SymbolKind::Function) ||
        (symbol->kind == SymbolKind::Struct) ||
        (symbol->kind == SymbolKind::Enum) ||
        (symbol->kind == SymbolKind::Interface) ||
        (symbol->kind == SymbolKind::Alias) ||
        (symbol->kind == SymbolKind::TypeAlias))
    {
        Utf8 typeWhat = SymTable::getNakedKindName(overload);
        if (!overload->node->isPublic())
        {
            if (usedNode && overload->node != usedNode)
            {
                Utf8 what;
                if (context.exportedNode && context.exportedNode->resolvedSymbolOverload)
                {
                    auto symName = context.exportedNode->resolvedSymbolOverload->symbol;
                    what         = Fmt("%s `%s`", SymTable::getNakedKindName(symName->kind), symName->name.c_str());
                }
                else if (usedNode->kind == AstNodeKind::FuncCall)
                    what = "function call";
                else
                    what = "declaration";

                Diagnostic diag{usedNode, Fmt(Err(Err0018), what.c_str(), typeWhat.c_str(), overload->node->token.ctext())};
                Diagnostic note{overload->node, Fmt(Nte(Nte0040), overload->node->token.ctext()), DiagnosticLevel::Note};
                return context.report(diag, &note);
            }

            Diagnostic diag{overload->node, Fmt(Err(Err0316), typeWhat.c_str(), overload->node->token.ctext())};
            return context.report(diag);
        }
    }

    return true;
}

void AstOutput::incIndentStatement(AstNode* node, int& indent)
{
    if (node->kind == AstNodeKind::CompilerIfBlock && node->childs.front()->kind == AstNodeKind::Statement)
        return;
    if (node->kind != AstNodeKind::Statement)
        indent++;
}

void AstOutput::decIndentStatement(AstNode* node, int& indent)
{
    if (node->kind == AstNodeKind::CompilerIfBlock && node->childs.front()->kind == AstNodeKind::Statement)
        return;
    if (node->kind != AstNodeKind::Statement)
        indent--;
}

bool AstOutput::outputGenericParameters(OutputContext& context, Concat& concat, AstNode* node)
{
    CONCAT_FIXED_STR(concat, "(");
    int idx = 0;
    for (auto p : node->childs)
    {
        if (idx)
            CONCAT_FIXED_STR(concat, ", ");
        concat.addString(p->token.text);

        AstVarDecl* varDecl = CastAst<AstVarDecl>(p, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
        if (varDecl->type)
        {
            CONCAT_FIXED_STR(concat, ": ");
            SWAG_CHECK(outputNode(context, concat, varDecl->type));
        }

        if (varDecl->assignment)
        {
            CONCAT_FIXED_STR(concat, " = ");
            SWAG_CHECK(outputNode(context, concat, varDecl->assignment));
        }

        idx++;
    }

    CONCAT_FIXED_STR(concat, ")");
    return true;
}

bool AstOutput::outputFuncSignature(OutputContext& context, Concat& concat, AstNode* node, AstNode* parameters, AstNode* selectIf)
{
    ScopeExportNode sen(context, node);

    if (node->kind == AstNodeKind::AttrDecl)
        CONCAT_FIXED_STR(concat, "attr ");
    else
        CONCAT_FIXED_STR(concat, "func ");
    concat.addString(node->token.text);

    // Parameters
    if (parameters)
        SWAG_CHECK(outputNode(context, concat, parameters));
    else
        CONCAT_FIXED_STR(concat, "()");

    // Return type, in case of function (not attr)
    if (node->kind == AstNodeKind::FuncDecl)
    {
        auto funcNode = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

        auto returnNode = funcNode->returnType;
        if (returnNode && !returnNode->childs.empty())
            returnNode = returnNode->childs.front();

        if (typeFunc->returnType && typeFunc->returnType != g_TypeMgr->typeInfoVoid)
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputType(context, concat, returnNode, typeFunc->returnType));
        }
        else if (funcNode->returnType && funcNode->returnType->specFlags & AST_SPEC_FUNCTYPE_RETURN_DEFINED)
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputNode(context, concat, returnNode));
        }
    }

    // Throw
    if (node->specFlags & AST_SPEC_FUNCDECL_THROW)
        CONCAT_FIXED_STR(concat, " throw");

    // Select if must be exported
    if (selectIf)
    {
        context.indent++;
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, selectIf));
        context.indent--;
    }

    CONCAT_FIXED_STR(concat, ";");
    concat.addEol();
    return true;
}

bool AstOutput::outputFunc(OutputContext& context, Concat& concat, AstFuncDecl* node)
{
    PushErrContext ec(&context, node, JobContext::ErrorContextType::Export);
    CONCAT_FIXED_STR(concat, "func");

    // Emit generic parameter, except if the function is an instance
    if (node->genericParameters)
    {
        if (!(node->flags & AST_FROM_GENERIC) || (node->flags & AST_IS_GENERIC))
            SWAG_CHECK(outputGenericParameters(context, concat, node->genericParameters));
    }

    // Name
    CONCAT_FIXED_STR(concat, " ");
    concat.addString(node->token.text);

    // Parameters
    if (node->parameters)
        SWAG_CHECK(outputNode(context, concat, node->parameters));
    else
        CONCAT_FIXED_STR(concat, "()");

    // Return type
    auto returnNode = node->returnType;
    if (returnNode && !returnNode->childs.empty())
        returnNode = returnNode->childs.front();

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    if (typeFunc->returnType && typeFunc->returnType != g_TypeMgr->typeInfoVoid)
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputType(context, concat, returnNode, typeFunc->returnType));
    }
    else if (node->returnType && node->returnType->specFlags & AST_SPEC_FUNCTYPE_RETURN_DEFINED)
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputNode(context, concat, returnNode));
    }

    // Throw
    if (node->specFlags & AST_SPEC_FUNCDECL_THROW)
        CONCAT_FIXED_STR(concat, " throw");

    // Content, short lambda
    if (node->flags & AST_SHORT_LAMBDA)
    {
        CONCAT_FIXED_STR(concat, " => ");
        SWAG_ASSERT(node->content->kind == AstNodeKind::Return);
        SWAG_CHECK(outputNode(context, concat, node->content->childs.front()));
        concat.addEol();
        return true;
    }

    // Content, normal function
    concat.addEolIndent(context.indent);
    CONCAT_FIXED_STR(concat, "{");
    context.indent++;
    concat.addEol();

    if (node->content->kind != AstNodeKind::Statement)
    {
        concat.addIndent(context.indent);
        context.indent--;
        SWAG_CHECK(outputNode(context, concat, node->content));
        context.indent++;
        concat.addEol();
    }
    else
    {
        for (auto c : node->subDecls)
        {
            concat.addIndent(context.indent);
            if (c->parent && c->parent->kind == AstNodeKind::AttrUse)
                c = c->parent;
            SWAG_CHECK(outputNode(context, concat, c));
            concat.addEol();
        }

        for (auto c : node->content->childs)
        {
            concat.addIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, c));
            concat.addEol();
        }
    }

    context.indent--;
    concat.addIndent(context.indent);
    CONCAT_FIXED_STR(concat, "}");
    concat.addEol();
    return true;
}

bool AstOutput::outputEnum(OutputContext& context, Concat& concat, AstEnum* node)
{
    PushErrContext ec(&context, node, JobContext::ErrorContextType::Export);

    CONCAT_FIXED_STR(concat, "enum ");
    concat.addString(node->token.text);

    // Raw type
    if (!node->childs.front()->childs.empty())
    {
        CONCAT_FIXED_STR(concat, " : ");
        SWAG_ASSERT(node->childs[0]->kind == AstNodeKind::EnumType);
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
    }

    concat.addEolIndent(context.indent);
    CONCAT_FIXED_STR(concat, "{");
    concat.addEol();

    for (auto c : node->childs)
    {
        if (c->kind != AstNodeKind::EnumValue)
            continue;
        concat.addIndent(context.indent + 1);

        concat.addString(c->token.text);
        if (!c->childs.empty())
        {
            CONCAT_FIXED_STR(concat, " = ");
            SWAG_CHECK(outputNode(context, concat, c->childs.front()));
        }

        concat.addEol();
    }

    concat.addIndent(context.indent);
    CONCAT_FIXED_STR(concat, "}");
    concat.addEol();
    return true;
}

bool AstOutput::outputAttributesUsage(OutputContext& context, Concat& concat, TypeInfoFuncAttr* typeFunc)
{
    bool first = true;
    concat.addIndent(context.indent);
    concat.addString("#[AttrUsage(");

#define ADD_ATTRUSAGE(__f, __n)                      \
    if (typeFunc->attributeUsage & (int) __f)        \
    {                                                \
        if (!first)                                  \
            CONCAT_FIXED_STR(concat, "|");           \
        first = false;                               \
        CONCAT_FIXED_STR(concat, "AttributeUsage."); \
        CONCAT_FIXED_STR(concat, __n);               \
    }

    ADD_ATTRUSAGE(AttributeUsage::Enum, "Enum");
    ADD_ATTRUSAGE(AttributeUsage::EnumValue, "EnumValue");
    ADD_ATTRUSAGE(AttributeUsage::StructVariable, "Field");
    ADD_ATTRUSAGE(AttributeUsage::GlobalVariable, "GlobalVariable");
    ADD_ATTRUSAGE(AttributeUsage::Variable, "Variable");
    ADD_ATTRUSAGE(AttributeUsage::Struct, "Struct");
    ADD_ATTRUSAGE(AttributeUsage::Function, "Function");
    ADD_ATTRUSAGE(AttributeUsage::File, "File");
    ADD_ATTRUSAGE(AttributeUsage::Constant, "Constant");

    ADD_ATTRUSAGE(AttributeUsage::Multi, "Multi");
    ADD_ATTRUSAGE(AttributeUsage::Gen, "Gen");
    ADD_ATTRUSAGE(AttributeUsage::All, "All");

    concat.addString(")]");
    concat.addEol();
    return true;
}

bool AstOutput::outputAttributes(OutputContext& context, Concat& concat, AttributeList& attributes)
{
    auto attr = &attributes;
    if (attr && !attr->empty())
    {
        concat.addIndent(context.indent);
        CONCAT_FIXED_STR(concat, "#[");

        for (int j = 0; j < attr->allAttributes.size(); j++)
        {
            auto& one = attr->allAttributes[j];
            if (j)
                CONCAT_FIXED_STR(concat, ", ");

            // No need to write "Swag.", less to write, less to read, as export files have
            // a 'using Swag' on top.
            if (!strncmp(one.name.c_str(), "Swag.", 5))
                concat.addString(one.name.c_str() + 5);
            else
                concat.addString(one.name);

            if (!one.parameters.empty())
            {
                concat.addChar('(');

                for (int i = 0; i < one.parameters.size(); i++)
                {
                    auto& oneParam = one.parameters[i];
                    if (i)
                        CONCAT_FIXED_STR(concat, ", ");
                    SWAG_CHECK(outputLiteral(context, concat, one.node, oneParam.typeInfo, oneParam.value));
                }

                concat.addChar(')');
            }
        }

        CONCAT_FIXED_STR(concat, "]");
        concat.addEol();
    }

    return true;
}

bool AstOutput::outputAttributesGlobalUsing(OutputContext& context, Concat& concat, AstNode* node)
{
    // Global using
    bool one         = false;
    bool outputUsing = true;
    if (node->flags & AST_STRUCT_MEMBER)
        outputUsing = false;
    if (outputUsing)
    {
        for (const auto& p : node->sourceFile->globalUsings)
        {
            if (p->getFullName() == "Swag")
                continue;

            if (!one)
            {
                concat.addIndent(context.indent);
                CONCAT_FIXED_STR(concat, "#[Using(");
                one = true;
            }
            else
            {
                CONCAT_FIXED_STR(concat, ", ");
            }

            concat.addString(p->getFullName());
        }

        if (one)
        {
            CONCAT_FIXED_STR(concat, ")]");
            concat.addEol();
        }
    }

    return true;
}

bool AstOutput::outputAttributes(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo)
{
    switch (node->kind)
    {
    case AstNodeKind::ConstDecl:
        return true;
    }

    SWAG_CHECK(outputAttributesGlobalUsing(context, concat, node));

    AttributeList* attr = nullptr;
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Struct:
    case TypeInfoKind::Interface:
    {
        auto type = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);
        attr      = &type->attributes;
        break;
    }
    case TypeInfoKind::FuncAttr:
    {
        auto type = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, typeInfo->kind);
        attr      = &type->attributes;
        break;
    }
    case TypeInfoKind::Enum:
    {
        auto type = CastTypeInfo<TypeInfoEnum>(typeInfo, typeInfo->kind);
        attr      = &type->attributes;
        break;
    }
    }

    SWAG_CHECK(outputAttributes(context, concat, *attr));
    return true;
}

bool AstOutput::outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value)
{
    if (typeInfo == g_TypeMgr->typeInfoNull)
    {
        CONCAT_FIXED_STR(concat, "null");
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::TypeListTuple || typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        SWAG_CHECK(outputNode(context, concat, node));
        return true;
    }

    SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
    auto str = Ast::literalToString(typeInfo, value);
    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::String:
        concat.addString("\"" + str + "\"");
        break;
    case NativeTypeKind::Rune:
        concat.addString("\"" + str + "\"\'rune");
        break;
    case NativeTypeKind::Bool:
        concat.addString(str);
        break;
    default:
        if (!(typeInfo->flags & (TYPEINFO_UNTYPED_INTEGER | TYPEINFO_UNTYPED_FLOAT)))
        {
            str += '\'';
            str += typeInfo->name;
        }

        concat.addString(str);
        break;
    }

    return true;
}

bool AstOutput::outputLambdaExpression(OutputContext& context, Concat& concat, AstNode* node)
{
    AstFuncDecl* funcDecl = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
    concat.addChar('@');

    // Closure capture parameters
    if (funcDecl->captureParameters)
    {
        concat.addChar('|');

        bool first = true;
        for (auto p : funcDecl->captureParameters->childs)
        {
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");
            first = false;

            if (p->kind == AstNodeKind::MakePointer)
            {
                concat.addChar('&');
                concat.addString(p->childs.front()->token.text);
            }
            else
            {
                concat.addString(p->token.text);
            }
        }

        concat.addChar('|');
    }

    // Parameters
    concat.addChar('(');
    if (funcDecl->parameters && !funcDecl->parameters->childs.empty())
    {
        bool first = true;
        for (auto p : funcDecl->parameters->childs)
        {
            if ((p->flags & AST_GENERATED) && !(p->flags & AST_GENERATED_USER))
                continue;
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");

            first = false;
            concat.addString(p->token.text);
            if (!p->childs.empty())
            {
                CONCAT_FIXED_STR(concat, ": ");
                SWAG_CHECK(outputNode(context, concat, p->childs.front()));
            }

            auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
            if (param->assignment)
            {
                CONCAT_FIXED_STR(concat, " = ");
                SWAG_CHECK(outputNode(context, concat, param->assignment));
            }
        }
    }

    concat.addChar(')');

    if (funcDecl->flags & AST_SHORT_LAMBDA)
    {
        CONCAT_FIXED_STR(concat, " => ");
        SWAG_ASSERT(funcDecl->content->kind == AstNodeKind::Return);
        SWAG_CHECK(outputNode(context, concat, funcDecl->content->childs.front()));
    }
    else
    {
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, funcDecl->content));
    }

    return true;
}

bool AstOutput::outputVar(OutputContext& context, Concat& concat, AstVarDecl* node)
{
    if (node->flags & AST_DECL_USING)
        CONCAT_FIXED_STR(concat, "using ");

    bool kindSpecified = false;
    if (node->kind == AstNodeKind::ConstDecl)
    {
        kindSpecified = true;
        CONCAT_FIXED_STR(concat, "const ");
    }
    else if (node->type && node->ownerFct && node->kind != AstNodeKind::FuncDeclParam && !(node->flags & AST_STRUCT_MEMBER))
    {
        kindSpecified = true;
        CONCAT_FIXED_STR(concat, "var ");
    }

    bool isSelf = node->token.text == "self";
    if (isSelf && node->type && ((AstTypeExpression*) node->type)->typeFlags & TYPEFLAG_ISCONST)
        CONCAT_FIXED_STR(concat, "const ");

    if (!(node->flags & AST_AUTO_NAME))
    {
        if (!node->publicName.empty())
            concat.addString(node->publicName);
        else
            concat.addString(node->token.text);
    }

    if (node->type)
    {
        if (!(node->type->flags & AST_GENERATED) || (node->type->flags & AST_GENERATED_USER))
        {
            if (!isSelf)
            {
                if (!(node->flags & AST_AUTO_NAME))
                    CONCAT_FIXED_STR(concat, ": ");
                SWAG_CHECK(outputNode(context, concat, node->type));
            }
        }
        else
        {
            CONCAT_FIXED_STR(concat, " = ");
            auto typeExpr = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
            SWAG_ASSERT(!node->assignment);
            SWAG_ASSERT(typeExpr->identifier);
            SWAG_ASSERT(node->type->typeInfo && (node->type->typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE));
            auto id = CastAst<AstIdentifier>(typeExpr->identifier->childs.back(), AstNodeKind::Identifier);
            CONCAT_FIXED_STR(concat, "@{");
            SWAG_CHECK(outputNode(context, concat, id->callParameters));
            CONCAT_FIXED_STR(concat, "}");
        }
    }

    if (node->assignment)
    {
        if (node->type || kindSpecified || node->kind == AstNodeKind::FuncDeclParam || (node->flags & AST_STRUCT_MEMBER))
            CONCAT_FIXED_STR(concat, " = ");
        else
            CONCAT_FIXED_STR(concat, " := ");
        SWAG_CHECK(outputNode(context, concat, node->assignment));
    }

    return true;
}

bool AstOutput::outputStruct(OutputContext& context, Concat& concat, AstStruct* node)
{
    // If we need to export as opaque, and the struct has init values, then we add the
    // #[Swag.ExportType] attribute
    if (node->attributeFlags & ATTRIBUTE_OPAQUE)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct);
        if (typeStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
        {
            CONCAT_FIXED_STR(concat, "#[Swag.ExportType(\"nozero\")]");
            concat.addEolIndent(context.indent);
        }
    }

    PushErrContext ec(&context, node, JobContext::ErrorContextType::Export);

    if (node->kind == AstNodeKind::InterfaceDecl)
        CONCAT_FIXED_STR(concat, "interface");
    else
    {
        SWAG_ASSERT(node->kind == AstNodeKind::StructDecl)
        auto structNode = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
        if (structNode->structFlags & STRUCTFLAG_UNION)
            CONCAT_FIXED_STR(concat, "union");
        else
            CONCAT_FIXED_STR(concat, "struct");
    }

    if (node->genericParameters)
        SWAG_CHECK(outputGenericParameters(context, concat, node->genericParameters));

    if (!(node->structFlags & STRUCTFLAG_ANONYMOUS))
    {
        CONCAT_FIXED_STR(concat, " ");
        concat.addString(node->token.text);
    }

    concat.addEolIndent(context.indent);

    // Opaque export. Just simulate structure with the correct size.
    if (node->attributeFlags & ATTRIBUTE_OPAQUE)
    {
        CONCAT_FIXED_STR(concat, "{");
        concat.addEol();

        auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct);

        // Everything in the structure is not initialized
        if (typeStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED)
        {
            SWAG_ASSERT(!(typeStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES));
            concat.addIndent(context.indent + 1);
            concat.addStringFormat("padding: [%llu] u8 = ?", typeStruct->sizeOf);
            concat.addEol();
        }

        // Everything in the structure is initiaized to zero
        else
        {
            concat.addIndent(context.indent + 1);
            concat.addStringFormat("padding: [%llu] u8", typeStruct->sizeOf);
            concat.addEol();
        }

        concat.addIndent(context.indent);
        CONCAT_FIXED_STR(concat, "}");
    }
    else
    {
        SWAG_CHECK(outputNode(context, concat, node->content));
    }

    return true;
}

bool AstOutput::outputTypeTuple(OutputContext& context, Concat& concat, TypeInfo* typeInfo)
{
    typeInfo = TypeManager::concreteReference(typeInfo);
    SWAG_ASSERT(typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE);
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    auto nodeStruct = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);

    if (nodeStruct->structFlags & STRUCTFLAG_ANONYMOUS)
    {
        SWAG_CHECK(outputStruct(context, concat, nodeStruct));
        return true;
    }

    concat.addString("{");
    int idx = 0;
    for (auto field : typeStruct->fields)
    {
        if (idx)
            CONCAT_FIXED_STR(concat, ", ");

        if (field->declNode && field->declNode->kind == AstNodeKind::VarDecl)
        {
            auto varDecl    = CastAst<AstVarDecl>(field->declNode, AstNodeKind::VarDecl);
            auto saveIndent = context.indent;
            context.indent  = 0;
            SWAG_CHECK(outputVar(context, concat, varDecl));
            context.indent = saveIndent;
        }
        else
        {
            if (!field->namedParam.empty() && field->namedParam.find("item") != 0)
            {
                concat.addString(field->namedParam);
                concat.addString(":");
            }

            SWAG_CHECK(outputType(context, concat, nullptr, field->typeInfo));
        }

        idx++;
    }

    concat.addString("}");
    return true;
}

bool AstOutput::outputType(OutputContext& context, Concat& concat, AstTypeExpression* node)
{
    if (node->typeFlags & TYPEFLAG_RETVAL)
    {
        CONCAT_FIXED_STR(concat, "retval");
        return true;
    }

    if (node->typeInfo)
    {
        // Identifier can have an export node, so in that case we need to export by node, not by type, in
        // order to export the real node (for example for an array of lambdas/closures)
        if (!node->identifier || !node->identifier->extension || !node->identifier->extension->exportNode)
        {
            SWAG_CHECK(outputType(context, concat, node, node->typeInfo));

            if (node->identifier)
            {
                auto id = CastAst<AstIdentifier>(node->identifier->childs.back(), AstNodeKind::Identifier);
                if (id->callParameters)
                {
                    if (id->flags & AST_GENERATED)
                    {
                        CONCAT_FIXED_STR(concat, " = @{");
                        SWAG_CHECK(outputNode(context, concat, id->callParameters));
                        concat.addChar('}');
                    }
                    else
                    {
                        concat.addChar('{');
                        SWAG_CHECK(outputNode(context, concat, id->callParameters));
                        concat.addChar('}');
                    }
                }
            }

            return true;
        }
    }

    if (node->typeFlags & TYPEFLAG_ISCONST)
        CONCAT_FIXED_STR(concat, "const ");
    if (node->typeFlags & TYPEFLAG_ISSLICE)
        CONCAT_FIXED_STR(concat, "[..] ");
    if (node->arrayDim == UINT8_MAX)
        CONCAT_FIXED_STR(concat, "[] ");
    else if (node->arrayDim)
    {
        CONCAT_FIXED_STR(concat, "[");
        for (int i = 0; i < node->arrayDim; i++)
        {
            if (i)
                CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->childs[i]));
        }

        CONCAT_FIXED_STR(concat, "] ");
    }

    for (int i = 0; i < node->ptrCount; i++)
        concat.addChar('*');
    if (node->identifier)
    {
        SWAG_CHECK(outputNode(context, concat, node->identifier));
    }
    else
    {
        auto typeFromLiteral = node->typeFromLiteral;
        if (!typeFromLiteral)
            typeFromLiteral = TypeManager::literalTypeToType(node->token);
        SWAG_ASSERT(typeFromLiteral);
        SWAG_ASSERT(!typeFromLiteral->name.empty());
        concat.addString(typeFromLiteral->name);
    }

    return true;
}

bool AstOutput::outputType(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo)
{
    // Lambda
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::Lambda)
    {
        SWAG_ASSERT(typeInfo->declNode && typeInfo->declNode->kind == AstNodeKind::TypeLambda);
        SWAG_CHECK(outputNode(context, concat, typeInfo->declNode));
        return true;
    }

    // Tuple
    /////////////////////////////////
    if (typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
    {
        SWAG_CHECK(outputTypeTuple(context, concat, typeInfo));
        return true;
    }

    // Other types
    /////////////////////////////////

    // When this is a reference, take the original (struct) type instead
    if (typeInfo->kind == TypeInfoKind::Reference)
    {
        auto typeRef = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
        typeInfo     = typeRef->pointedType;
    }

    if (typeInfo->flags & TYPEINFO_SELF)
    {
        if (typeInfo->flags & TYPEINFO_CONST)
            CONCAT_FIXED_STR(concat, "const self");
        else
            CONCAT_FIXED_STR(concat, "self");
        return true;
    }

    // Check is public
    auto typeExport = typeInfo;
    if (typeExport->kind == TypeInfoKind::Array)
        typeExport = ((TypeInfoArray*) typeExport)->finalType;
    while (typeExport->kind == TypeInfoKind::Pointer)
        typeExport = ((TypeInfoPointer*) typeExport)->pointedType;
    SWAG_CHECK(checkIsPublic(context, typeExport->declNode, node));

    typeInfo->computeScopedNameExport();
    SWAG_ASSERT(!typeInfo->scopedNameExport.empty());
    concat.addString(typeInfo->scopedNameExport);

    return true;
}

bool AstOutput::outputNode(OutputContext& context, Concat& concat, AstNode* node)
{
    if (!node)
        return true;
    if (node->extension && node->extension->exportNode)
        node = node->extension->exportNode;
    if (node->flags & AST_GENERATED && !(node->flags & AST_GENERATED_USER))
        return true;

    switch (node->kind)
    {
    case AstNodeKind::Throw:
        concat.addString(node->token.text);
        concat.addChar(' ');
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::Try:
    case AstNodeKind::Assume:
    {
        if (node->specFlags & AST_SPEC_TCA_GENERATED && node->specFlags & AST_SPEC_TCA_BLOCK)
        {
            context.indent += 1;
            //concat.addIndent(context.indent);

            for (auto c : node->childs.front()->childs)
            {
                SWAG_CHECK(outputNode(context, concat, c));
                concat.addEolIndent(context.indent);
            }

            context.indent -= 1;
        }
        else
        {
            if (node->flags & AST_DISCARD)
                CONCAT_FIXED_STR(concat, "discard ");
            concat.addString(node->token.text);
            concat.addChar(' ');
            SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        }
        break;
    }

    case AstNodeKind::Catch:
        if (node->flags & AST_DISCARD)
            CONCAT_FIXED_STR(concat, "discard ");
        concat.addString(node->token.text);
        concat.addChar(' ');
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::FuncDeclType:
        if (!node->childs.empty())
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        }
        break;

    case AstNodeKind::FuncDeclParams:
    {
        concat.addChar('(');
        bool first = true;
        for (auto c : node->childs)
        {
            if ((c->flags & AST_GENERATED) && !(c->flags & AST_GENERATED_USER))
                continue;
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");
            first = false;
            SWAG_CHECK(outputNode(context, concat, c));
        }
        concat.addChar(')');
        break;
    }

    case AstNodeKind::FuncDecl:
    {
        auto nodeFunc = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        SWAG_CHECK(outputFunc(context, concat, nodeFunc));
        break;
    }

    case AstNodeKind::EnumType:
        if (!node->childs.empty())
            SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        break;

    case AstNodeKind::EnumDecl:
    {
        auto nodeEnum = CastAst<AstEnum>(node, AstNodeKind::EnumDecl);
        SWAG_CHECK(outputEnum(context, concat, nodeEnum));
        break;
    }

    case AstNodeKind::StructContent:
    case AstNodeKind::TupleContent:
        CONCAT_FIXED_STR(concat, "{ ");
        concat.addEol();
        context.indent++;
        for (auto c : node->childs)
        {
            concat.addIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, c));
            concat.addEol();
        }

        context.indent--;
        concat.addIndent(context.indent);
        CONCAT_FIXED_STR(concat, "}");
        concat.addEol();
        break;

    case AstNodeKind::StructDecl:
    case AstNodeKind::InterfaceDecl:
    {
        auto nodeStruct = CastAst<AstStruct>(node, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
        SWAG_CHECK(outputStruct(context, concat, nodeStruct));
        break;
    }

    case AstNodeKind::Defer:
        CONCAT_FIXED_STR(concat, "defer ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        concat.addEol();
        break;

    case AstNodeKind::AttrUse:
    {
        auto nodeAttr = CastAst<AstAttrUse>(node, AstNodeKind::AttrUse);

        // Be sure this is not an empty attribute block (empty or that contains
        // other empty blocks)
        bool hasSomething = true;
        auto scanAttr     = nodeAttr;
        while (scanAttr->content->kind == AstNodeKind::Statement)
        {
            if (scanAttr->content->childs.empty())
            {
                hasSomething = false;
                break;
            }
            if (scanAttr->content->childs.size() > 1)
                break;
            if (scanAttr->content->childs[0]->kind != AstNodeKind::AttrUse)
                break;
            scanAttr = (AstAttrUse*) scanAttr->content->childs[0];
        }

        if (!hasSomething)
            break;

        bool first = true;
        for (auto s : nodeAttr->childs)
        {
            if (s == nodeAttr->content)
                continue;
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");
            else
                CONCAT_FIXED_STR(concat, "#[");
            first = false;
            SWAG_CHECK(outputNode(context, concat, s));
        }

        if (!first)
        {
            concat.addChar(']');
            concat.addEolIndent(context.indent);
        }

        SWAG_CHECK(outputNode(context, concat, nodeAttr->content));
        break;
    }

    case AstNodeKind::ExplicitNoInit:
        concat.addChar('?');
        break;

    case AstNodeKind::Index:
        CONCAT_FIXED_STR(concat, "@index");
        break;
    case AstNodeKind::GetErr:
        CONCAT_FIXED_STR(concat, "@err");
        break;
    case AstNodeKind::Init:
        CONCAT_FIXED_STR(concat, "@init(");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        if (node->childs.size() == 2)
        {
            CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->childs.back()));
        }
        concat.addChar(')');
        break;
    case AstNodeKind::Drop:
        CONCAT_FIXED_STR(concat, "@drop(");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        if (node->childs.size() == 2)
        {
            CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->childs.back()));
        }
        concat.addChar(')');
        break;
    case AstNodeKind::PostMove:
        CONCAT_FIXED_STR(concat, "@postmove(");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        if (node->childs.size() == 2)
        {
            CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->childs.back()));
        }
        concat.addChar(')');
        break;
    case AstNodeKind::PostCopy:
        CONCAT_FIXED_STR(concat, "@postcopy(");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        if (node->childs.size() == 2)
        {
            CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->childs.back()));
        }
        concat.addChar(')');
        break;

    case AstNodeKind::Break:
    {
        auto nodeBreak = CastAst<AstBreakContinue>(node, AstNodeKind::Break);
        CONCAT_FIXED_STR(concat, "break ");
        concat.addString(nodeBreak->label);
        break;
    }

    case AstNodeKind::Continue:
    {
        auto nodeContinue = CastAst<AstBreakContinue>(node, AstNodeKind::Continue);
        CONCAT_FIXED_STR(concat, "continue ");
        concat.addString(nodeContinue->label);
        break;
    }

    case AstNodeKind::MakePointer:
    {
        concat.addChar('&');
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;
    }

    case AstNodeKind::MakePointerLambda:
    {
        auto lambdaNode = CastAst<AstMakePointer>(node, AstNodeKind::MakePointerLambda);
        SWAG_CHECK(outputLambdaExpression(context, concat, lambdaNode->lambda));
        break;
    }

    case AstNodeKind::NoDrop:
        CONCAT_FIXED_STR(concat, "nodrop ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::Move:
        CONCAT_FIXED_STR(concat, "move ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::ArrayPointerIndex:
    {
        auto arrayNode = CastAst<AstArrayPointerIndex>(node, AstNodeKind::ArrayPointerIndex);
        if (arrayNode->specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF)
        {
            CONCAT_FIXED_STR(concat, "dref ");
            SWAG_CHECK(outputNode(context, concat, arrayNode->array));
        }
        else
        {
            SWAG_CHECK(outputNode(context, concat, arrayNode->array));
            concat.addChar('[');
            SWAG_CHECK(outputNode(context, concat, arrayNode->access));
            concat.addChar(']');
        }
        break;
    }

    case AstNodeKind::ExpressionList:
    {
        auto exprNode = CastAst<AstExpressionList>(node, AstNodeKind::ExpressionList);
        if (exprNode->specFlags & AST_SPEC_EXPRLIST_FOR_TUPLE)
            concat.addString2("@{");
        else
            concat.addString2("@[");

        int idx = 0;
        for (auto child : exprNode->childs)
        {
            if (child->flags & AST_GENERATED)
                continue;
            if (idx++)
                CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, child));
        }

        if (exprNode->specFlags & AST_SPEC_EXPRLIST_FOR_TUPLE)
            concat.addChar('}');
        else
            concat.addChar(']');
        break;
    }

    case AstNodeKind::CompilerLoad:
        CONCAT_FIXED_STR(concat, "#load(");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        CONCAT_FIXED_STR(concat, ")");
        break;

    case AstNodeKind::CompilerRun:
    case AstNodeKind::CompilerAst:
    case AstNodeKind::CompilerSelectIf:
    case AstNodeKind::CompilerCheckIf:
    {
        if (node->kind == AstNodeKind::CompilerRun)
            CONCAT_FIXED_STR(concat, "#run ");
        else if (node->kind == AstNodeKind::CompilerAst)
            CONCAT_FIXED_STR(concat, "#ast ");
        else if (node->kind == AstNodeKind::CompilerSelectIf)
            CONCAT_FIXED_STR(concat, "#selectif ");
        else if (node->kind == AstNodeKind::CompilerCheckIf)
            CONCAT_FIXED_STR(concat, "#checkif ");

        auto front = node->childs.front();
        if (front->kind == AstNodeKind::FuncDecl)
        {
            AstFuncDecl* funcDecl = CastAst<AstFuncDecl>(front, AstNodeKind::FuncDecl);
            incIndentStatement(funcDecl->content, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, funcDecl->content));
            decIndentStatement(funcDecl->content, context.indent);
        }
        else
        {
            SWAG_CHECK(outputNode(context, concat, front));
        }
        break;
    }

    case AstNodeKind::CompilerIfBlock:
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::CompilerMacro:
        CONCAT_FIXED_STR(concat, "#macro");
        incIndentStatement(node->childs.front(), context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        decIndentStatement(node->childs.front(), context.indent);
        break;

    case AstNodeKind::CompilerInline:
        CONCAT_FIXED_STR(concat, "#inline");
        incIndentStatement(node->childs.front(), context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        decIndentStatement(node->childs.front(), context.indent);
        break;

    case AstNodeKind::CompilerMixin:
    {
        auto compilerMixin = CastAst<AstCompilerMixin>(node, AstNodeKind::CompilerMixin);
        CONCAT_FIXED_STR(concat, "#mixin ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        if (!compilerMixin->replaceTokens.empty())
        {
            CONCAT_FIXED_STR(concat, " { ");
            for (auto m : compilerMixin->replaceTokens)
            {
                switch (m.first)
                {
                case TokenId::KwdBreak:
                    CONCAT_FIXED_STR(concat, "break");
                    break;
                case TokenId::KwdContinue:
                    CONCAT_FIXED_STR(concat, "continue");
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
                }

                CONCAT_FIXED_STR(concat, " = ");
                SWAG_CHECK(outputNode(context, concat, m.second));
                CONCAT_FIXED_STR(concat, "; ");
            }
            concat.addChar('}');
        }
        break;
    }

    case AstNodeKind::CompilerPrint:
        CONCAT_FIXED_STR(concat, "#print ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::CompilerSemError:
        CONCAT_FIXED_STR(concat, "#semerror ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::CompilerAssert:
        CONCAT_FIXED_STR(concat, "#assert(");
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        if (node->childs.size() > 1)
        {
            concat.addChar(',');
            SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        }

        concat.addChar(')');
        break;

    case AstNodeKind::CompilerIf:
    {
        auto compilerIf = CastAst<AstIf>(node, AstNodeKind::CompilerIf);
        CONCAT_FIXED_STR(concat, "#if ");
        SWAG_CHECK(outputNode(context, concat, compilerIf->boolExpression));

        incIndentStatement(compilerIf->ifBlock, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, compilerIf->ifBlock));
        decIndentStatement(compilerIf->ifBlock, context.indent);

        if (compilerIf->elseBlock)
        {
            concat.addEolIndent(context.indent);
            CONCAT_FIXED_STR(concat, "#else ");
            if (compilerIf->elseBlock->childs.front()->kind != AstNodeKind::CompilerIf)
            {
                incIndentStatement(compilerIf->elseBlock, context.indent);
                concat.addEolIndent(context.indent);
            }
            SWAG_CHECK(outputNode(context, concat, compilerIf->elseBlock));
            if (compilerIf->elseBlock->childs.front()->kind != AstNodeKind::CompilerIf)
            {
                decIndentStatement(compilerIf->elseBlock, context.indent);
            }
        }
        break;
    }

    case AstNodeKind::If:
    {
        auto compilerIf = CastAst<AstIf>(node, AstNodeKind::If, AstNodeKind::CompilerIf);
        CONCAT_FIXED_STR(concat, "if ");
        SWAG_CHECK(outputNode(context, concat, compilerIf->boolExpression));

        incIndentStatement(compilerIf->ifBlock, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, compilerIf->ifBlock));
        decIndentStatement(compilerIf->ifBlock, context.indent);

        if (compilerIf->elseBlock)
        {
            concat.addEolIndent(context.indent);
            CONCAT_FIXED_STR(concat, "else");
            incIndentStatement(compilerIf->elseBlock, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, compilerIf->elseBlock));
            decIndentStatement(compilerIf->elseBlock, context.indent);
        }
        break;
    }

    case AstNodeKind::For:
    {
        auto forNode = CastAst<AstFor>(node, AstNodeKind::For);
        CONCAT_FIXED_STR(concat, "for ");
        SWAG_CHECK(outputNode(context, concat, forNode->preExpression));
        CONCAT_FIXED_STR(concat, "; ");
        SWAG_CHECK(outputNode(context, concat, forNode->boolExpression));
        CONCAT_FIXED_STR(concat, "; ");
        SWAG_CHECK(outputNode(context, concat, forNode->postExpression));
        incIndentStatement(forNode->block, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, forNode->block));
        decIndentStatement(forNode->block, context.indent);
        break;
    }

    case AstNodeKind::Visit:
    {
        auto visitNode = CastAst<AstVisit>(node, AstNodeKind::Visit);
        CONCAT_FIXED_STR(concat, "visit ");

        if (visitNode->specFlags & AST_SPEC_VISIT_WANTPOINTER)
            concat.addChar('&');

        bool first = true;
        for (auto& a : visitNode->aliasNames)
        {
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");
            first = false;
            concat.addString(a.text);
        }

        if (!visitNode->aliasNames.empty())
            CONCAT_FIXED_STR(concat, ": ");
        SWAG_CHECK(outputNode(context, concat, visitNode->expression));
        incIndentStatement(visitNode->block, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, visitNode->block));
        decIndentStatement(visitNode->block, context.indent);
        break;
    }

    case AstNodeKind::Loop:
    {
        auto loopNode = CastAst<AstLoop>(node, AstNodeKind::Loop);
        CONCAT_FIXED_STR(concat, "loop ");
        if (loopNode->specificName)
        {
            concat.addString(loopNode->specificName->token.text);
            CONCAT_FIXED_STR(concat, ": ");
        }

        SWAG_CHECK(outputNode(context, concat, loopNode->expression));
        incIndentStatement(loopNode->block, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, loopNode->block));
        decIndentStatement(loopNode->block, context.indent);
        break;
    }

    case AstNodeKind::While:
    {
        auto whileNode = CastAst<AstWhile>(node, AstNodeKind::While);
        CONCAT_FIXED_STR(concat, "while ");
        SWAG_CHECK(outputNode(context, concat, whileNode->boolExpression));
        incIndentStatement(whileNode->block, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, whileNode->block));
        decIndentStatement(whileNode->block, context.indent);
        break;
    }

    case AstNodeKind::CompilerSpecialFunction:
    {
        switch (node->token.id)
        {
        case TokenId::CompilerSelf:
            CONCAT_FIXED_STR(concat, "#self");
            break;
        case TokenId::CompilerCallerFunction:
            CONCAT_FIXED_STR(concat, "#callerfunction");
            break;
        case TokenId::CompilerCallerLocation:
            CONCAT_FIXED_STR(concat, "#callerlocation");
            break;
        case TokenId::CompilerLocation:
            CONCAT_FIXED_STR(concat, "#location");
            if (!node->childs.empty())
            {
                concat.addChar('(');
                SWAG_CHECK(outputNode(context, concat, node->childs[0]));
                concat.addChar(')');
            }
            break;
        case TokenId::CompilerArch:
            CONCAT_FIXED_STR(concat, "#arch");
            break;
        case TokenId::CompilerOs:
            CONCAT_FIXED_STR(concat, "#os");
            break;
        case TokenId::CompilerSwagOs:
            CONCAT_FIXED_STR(concat, "#swagos");
            break;
        case TokenId::CompilerBackend:
            CONCAT_FIXED_STR(concat, "#backend");
            break;
        case TokenId::CompilerBuildCfg:
            CONCAT_FIXED_STR(concat, "#cfg");
            break;
        default:
            return node->sourceFile->internalError(node, "Ast::outputNode, unknown compiler function");
        }
        break;
    }

    case AstNodeKind::ConditionalExpression:
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        CONCAT_FIXED_STR(concat, " ? (");
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        CONCAT_FIXED_STR(concat, ") : (");
        SWAG_CHECK(outputNode(context, concat, node->childs[2]));
        concat.addChar(')');
        break;

    case AstNodeKind::Alias:
    {
        CONCAT_FIXED_STR(concat, "alias ");
        concat.addString(node->token.text);
        CONCAT_FIXED_STR(concat, " = ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;
    }

    case AstNodeKind::ConstDecl:
    case AstNodeKind::VarDecl:
    case AstNodeKind::FuncDeclParam:
    {
        AstVarDecl* varDecl = static_cast<AstVarDecl*>(node);
        SWAG_CHECK(outputVar(context, concat, varDecl));
        break;
    }

    case AstNodeKind::Using:
    {
        CONCAT_FIXED_STR(concat, "using ");
        int idx = 0;
        for (auto child : node->childs)
        {
            if (idx)
                CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, child));
            idx++;
        }

        break;
    }

    case AstNodeKind::Return:
    {
        CONCAT_FIXED_STR(concat, "return ");
        if (!node->childs.empty())
            SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;
    }

    case AstNodeKind::IntrinsicProp:
    {
        auto propertyNode = CastAst<AstIntrinsicProp>(node, AstNodeKind::IntrinsicProp);
        concat.addString(propertyNode->token.text);
        concat.addChar('(');
        int idx = 0;
        for (auto child : node->childs)
        {
            if (idx)
                CONCAT_FIXED_STR(concat, ",");
            SWAG_CHECK(outputNode(context, concat, child));
            idx++;
        }

        concat.addChar(')');
        break;
    }

    case AstNodeKind::IdentifierRef:
    {
        if (node->flags & AST_DISCARD)
            CONCAT_FIXED_STR(concat, "discard ");
        if (node->specFlags & AST_SPEC_IDENTIFIERREF_AUTO_SCOPE)
            CONCAT_FIXED_STR(concat, ".");
        int idx = 0;
        for (auto child : node->childs)
        {
            if (idx)
                CONCAT_FIXED_STR(concat, ".");
            SWAG_CHECK(outputNode(context, concat, child));
            idx++;
        }

        break;
    }

    case AstNodeKind::Identifier:
        if (node->specFlags & AST_SPEC_IDENTIFIER_BACKTICK)
            concat.addChar('`');

    case AstNodeKind::FuncCall:
    {
        SWAG_CHECK(checkIsPublic(context, node, node));

        auto identifier = static_cast<AstIdentifier*>(node);
        concat.addString(node->token.text);

        if (identifier->genericParameters)
        {
            concat.addChar('\'');
            concat.addChar('(');
            SWAG_CHECK(outputNode(context, concat, identifier->genericParameters));
            concat.addChar(')');
        }

        if (identifier->callParameters)
        {
            if (identifier->callParameters->flags & AST_CALL_FOR_STRUCT)
                concat.addChar('{');
            else
                concat.addChar('(');
            SWAG_CHECK(outputNode(context, concat, identifier->callParameters));
            if (identifier->callParameters->flags & AST_CALL_FOR_STRUCT)
                concat.addChar('}');
            else
                concat.addChar(')');
        }

        break;
    }

    case AstNodeKind::Switch:
    {
        auto nodeSwitch = CastAst<AstSwitch>(node, AstNodeKind::Switch);
        CONCAT_FIXED_STR(concat, "switch ");
        SWAG_CHECK(outputNode(context, concat, nodeSwitch->expression));
        concat.addEolIndent(context.indent);
        concat.addChar('{');
        concat.addEolIndent(context.indent);

        for (auto c : nodeSwitch->cases)
        {
            if (c->expressions.empty())
                CONCAT_FIXED_STR(concat, "default");
            else
            {
                CONCAT_FIXED_STR(concat, "case ");
                bool first = true;
                for (auto e : c->expressions)
                {
                    if (!first)
                        CONCAT_FIXED_STR(concat, ", ");
                    SWAG_CHECK(outputNode(context, concat, e));
                    first = false;
                }
            }
            concat.addChar(':');
            concat.addEolIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, c->block));
            concat.addEolIndent(context.indent);
        }

        concat.addChar('}');
        concat.addEolIndent(context.indent);
        break;
    }

    case AstNodeKind::StatementNoScope:
    {
        for (auto child : node->childs)
        {
            if (child->flags & AST_GENERATED)
                continue;
            context.indent++;
            SWAG_CHECK(outputNode(context, concat, child));
            context.indent--;
            concat.addEolIndent(context.indent);
        }

        break;
    }

    case AstNodeKind::Statement:
    case AstNodeKind::SwitchCaseBlock:
        if (node->childs.count == 1 &&
            node->parent->kind != AstNodeKind::FuncDecl &&
            node->parent->kind != AstNodeKind::CompilerMacro &&
            node->childs.front()->kind != AstNodeKind::For &&
            node->childs.front()->kind != AstNodeKind::If &&
            node->childs.front()->kind != AstNodeKind::While &&
            node->childs.front()->kind != AstNodeKind::Visit &&
            node->childs.front()->kind != AstNodeKind::Loop &&
            node->childs.front()->kind != AstNodeKind::ScopeBreakable &&
            node->childs.front()->kind != AstNodeKind::Switch)
        {
            context.indent++;
            concat.addIndent(1);
            SWAG_CHECK(outputNode(context, concat, node->childs.front()));
            context.indent--;
        }
        else
        {
            concat.addChar('{');
            concat.addEol();
            for (auto child : node->childs)
            {
                concat.addIndent(context.indent + 1);
                context.indent++;
                SWAG_CHECK(outputNode(context, concat, child));
                context.indent--;
                concat.addEol();
            }

            concat.addIndent(context.indent);
            concat.addChar('}');
            concat.addEolIndent(context.indent);
        }

        break;

    case AstNodeKind::FuncCallParam:
    {
        auto funcParam = CastAst<AstFuncCallParam>(node, AstNodeKind::FuncCallParam);
        if (!funcParam->namedParam.empty())
        {
            concat.addString(funcParam->namedParam);
            concat.addChar(':');
        }

        SWAG_CHECK(outputNode(context, concat, funcParam->childs.front()));
        break;
    }

    case AstNodeKind::FuncCallParams:
    {
        auto funcCallParams = CastAst<AstFuncCallParams>(node, AstNodeKind::FuncCallParams);

        // Aliases
        bool first = true;
        if (!funcCallParams->aliasNames.empty())
        {
            CONCAT_FIXED_STR(concat, "|");
            for (auto& t : funcCallParams->aliasNames)
            {
                if (!first)
                    CONCAT_FIXED_STR(concat, ", ");
                concat.addString(t.text);
                first = false;
            }

            CONCAT_FIXED_STR(concat, "| ");
        }

        first = true;
        for (auto child : funcCallParams->childs)
        {
            if (child->flags & AST_GENERATED)
                continue;
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");
            first = false;
            SWAG_CHECK(outputNode(context, concat, child));
        }

        break;
    }

    case AstNodeKind::SingleOp:
        concat.addString(node->token.text);
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        break;

    case AstNodeKind::NullConditionalExpression:
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        concat.addString(" orelse ");
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        break;

    case AstNodeKind::AffectOp:
    {
        auto opNode = CastAst<AstOp>(node, AstNodeKind::AffectOp);
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        concat.addChar(' ');
        concat.addString(node->token.text);
        if (opNode->specFlags & AST_SPEC_OP_SAFE)
            CONCAT_FIXED_STR(concat, ",safe");
        if (opNode->specFlags & AST_SPEC_OP_SMALL)
            CONCAT_FIXED_STR(concat, ",small");
        concat.addChar(' ');
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        break;
    }

    case AstNodeKind::FactorOp:
    {
        auto opNode = CastAst<AstOp>(node, AstNodeKind::FactorOp);
        concat.addChar('(');
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        concat.addChar(' ');
        concat.addString(node->token.text);
        if (opNode->specFlags & AST_SPEC_OP_SAFE)
            CONCAT_FIXED_STR(concat, ",safe");
        if (opNode->specFlags & AST_SPEC_OP_SMALL)
            CONCAT_FIXED_STR(concat, ",small");
        concat.addChar(' ');
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        concat.addChar(')');
        break;
    }

    case AstNodeKind::BinaryOp:
        concat.addChar('(');
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        concat.addChar(' ');
        concat.addString(node->token.text);
        concat.addChar(' ');
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        concat.addChar(')');
        break;

    case AstNodeKind::AutoCast:
        CONCAT_FIXED_STR(concat, "acast ");
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        break;

    case AstNodeKind::Cast:
        CONCAT_FIXED_STR(concat, "cast(");
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        CONCAT_FIXED_STR(concat, ") ");
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        break;

    case AstNodeKind::BitCast:
        CONCAT_FIXED_STR(concat, "bitcast(");
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        CONCAT_FIXED_STR(concat, ") ");
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        break;

    case AstNodeKind::TypeExpression:
    {
        AstTypeExpression* typeNode = static_cast<AstTypeExpression*>(node);
        SWAG_CHECK(outputType(context, concat, typeNode));
        break;
    }

    case AstNodeKind::Literal:
        if (node->token.literalType == LiteralType::TT_RAW_STRING)
            CONCAT_FIXED_STR(concat, "@\"");
        else if (node->token.literalType == LiteralType::TT_STRING || node->token.literalType == LiteralType::TT_ESCAPE_STRING)
            CONCAT_FIXED_STR(concat, "\"");

        concat.addString(node->token.text);

        if (node->token.literalType == LiteralType::TT_RAW_STRING)
            CONCAT_FIXED_STR(concat, "\"@");
        else if (node->token.literalType == LiteralType::TT_STRING || node->token.literalType == LiteralType::TT_ESCAPE_STRING)
            CONCAT_FIXED_STR(concat, "\"");

        if (!node->childs.empty())
        {
            CONCAT_FIXED_STR(concat, "'");
            SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        }
        break;

    case AstNodeKind::ScopeBreakable:
        CONCAT_FIXED_STR(concat, "scope ");
        concat.addString(node->token.text);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        break;

    case AstNodeKind::Range:
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        CONCAT_FIXED_STR(concat, "..");
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        break;

    case AstNodeKind::TypeLambda:
    case AstNodeKind::TypeClosure:
    {
        AstTypeLambda* typeNode = static_cast<AstTypeLambda*>(node);
        if (node->kind == AstNodeKind::TypeLambda)
            CONCAT_FIXED_STR(concat, "func");
        else
            CONCAT_FIXED_STR(concat, "closure");
        if (!typeNode->parameters)
            CONCAT_FIXED_STR(concat, "()");
        else
            SWAG_CHECK(outputNode(context, concat, typeNode->parameters));
        if (typeNode->returnType)
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputNode(context, concat, typeNode->returnType));
        }

        if (node->specFlags & AST_SPEC_FUNCDECL_THROW)
            CONCAT_FIXED_STR(concat, " throw");
        break;
    }

    default:
        return node->sourceFile->internalError(node, "Ast::outputNode, unknown node");
    }

    return true;
}

bool AstOutput::outputScopeContent(OutputContext& context, Concat& concat, Module* moduleToGen, Scope* scope)
{
    auto publicSet = scope->publicSet;
    if (!publicSet)
        return true;

    // Stuff
    if (!publicSet->publicNodes.empty())
    {
        for (auto one : publicSet->publicNodes)
        {
            SWAG_CHECK(outputAttributes(context, concat, one, one->typeInfo));
            concat.addIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, one));
            concat.addEol();
        }
    }

    // Functions
    if (!publicSet->publicFunc.empty())
    {
        for (auto one : publicSet->publicFunc)
        {
            AstFuncDecl* node = CastAst<AstFuncDecl>(one, AstNodeKind::FuncDecl);

            // Can be removed in case of special functions
            if (!(node->attributeFlags & ATTRIBUTE_PUBLIC))
                continue;

            // Remape special functions to their generated equivalent
            node->computeFullNameForeign(true);
            concat.addIndent(context.indent);
            concat.addStringFormat("#[Foreign(\"%s\", \"%s\")]", moduleToGen->name.c_str(), node->fullnameForeign.c_str());
            concat.addEol();
            SWAG_CHECK(outputAttributes(context, concat, node, node->typeInfo));

            concat.addIndent(context.indent);
            if (node->token.text == g_LangSpec->name_opInitGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opInit(using self);");
                concat.addEol();
            }
            else if (node->token.text == g_LangSpec->name_opDropGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opDrop(using self);");
                concat.addEol();
            }
            else if (node->token.text == g_LangSpec->name_opPostCopyGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opPostCopy(using self);");
                concat.addEol();
            }
            else if (node->token.text == g_LangSpec->name_opPostMoveGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opPostMove(using self);");
                concat.addEol();
            }
            else
                SWAG_CHECK(outputFuncSignature(context, concat, node, node->parameters, node->selectIf));

            node->exportForeignLine = concat.eolCount;
        }
    }

    // Attributes
    if (!publicSet->publicAttr.empty())
    {
        for (auto one : publicSet->publicAttr)
        {
            auto              node     = CastAst<AstAttrDecl>(one, AstNodeKind::AttrDecl);
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            SWAG_CHECK(outputAttributesUsage(context, concat, typeFunc));
            concat.addIndent(context.indent);
            SWAG_CHECK(outputFuncSignature(context, concat, node, node->parameters, nullptr));
        }
    }

    return true;
}

bool AstOutput::outputScope(OutputContext& context, Concat& concat, Module* moduleToGen, Scope* scope)
{
    SWAG_ASSERT(moduleToGen);
    if (!(scope->flags & SCOPE_FLAG_HAS_EXPORTS))
        return true;
    if (scope->flags & SCOPE_IMPORTED)
        return true;

    context.forExport = true;

    // Namespace
    if (scope->kind == ScopeKind::Namespace && !scope->name.empty())
    {
        if (!(scope->flags & SCOPE_AUTO_GENERATED))
        {
            concat.addIndent(context.indent);
            CONCAT_FIXED_STR(concat, "namespace ");
            concat.addString(scope->name);
            concat.addEol();
            concat.addIndent(context.indent);
            CONCAT_FIXED_STR(concat, "{");
            concat.addEol();
            context.indent++;
        }

        SWAG_CHECK(outputScopeContent(context, concat, moduleToGen, scope));
        for (auto oneScope : scope->childScopes)
            SWAG_CHECK(outputScope(context, concat, moduleToGen, oneScope));

        if (!(scope->flags & SCOPE_AUTO_GENERATED))
        {
            context.indent--;
            concat.addIndent(context.indent);
            CONCAT_FIXED_STR(concat, "}");
            concat.addEol();
            concat.addEol();
        }
    }

    // Impl
    else if (!scope->isGlobal() && scope->isGlobalOrImpl() && !scope->name.empty())
    {
        concat.addIndent(context.indent);
        if (scope->kind == ScopeKind::Impl)
        {
            auto nodeImpl = CastAst<AstImpl>(scope->owner, AstNodeKind::Impl);
            auto symbol   = nodeImpl->identifier->resolvedSymbolOverload;
            concat.addStringFormat("impl %s for %s", symbol->node->getScopedName().c_str(), scope->parentScope->name.c_str());
            concat.addEol();
        }
        else if (scope->kind == ScopeKind::Enum)
        {
            CONCAT_FIXED_STR(concat, "impl enum ");
            concat.addString(scope->name);
            concat.addEol();
        }
        else
        {
            CONCAT_FIXED_STR(concat, "impl ");
            concat.addString(scope->name);
            concat.addEol();
        }

        concat.addIndent(context.indent);
        CONCAT_FIXED_STR(concat, "{");
        concat.addEol();

        context.indent++;
        SWAG_CHECK(outputScopeContent(context, concat, moduleToGen, scope));
        for (auto oneScope : scope->childScopes)
        {
            if (oneScope->kind == ScopeKind::Impl)
                continue;
            SWAG_CHECK(outputScope(context, concat, moduleToGen, oneScope));
        }

        context.indent--;
        concat.addIndent(context.indent);
        CONCAT_FIXED_STR(concat, "}");
        concat.addEol();
        concat.addEol();

        for (auto oneScope : scope->childScopes)
        {
            if (oneScope->kind != ScopeKind::Impl)
                continue;
            SWAG_CHECK(outputScope(context, concat, moduleToGen, oneScope));
        }
    }

    // Named scope
    else if (!scope->name.empty())
    {
        concat.addIndent(context.indent);
        CONCAT_FIXED_STR(concat, "{");
        concat.addEol();

        context.indent++;
        SWAG_CHECK(outputScopeContent(context, concat, moduleToGen, scope));
        for (auto oneScope : scope->childScopes)
            SWAG_CHECK(outputScope(context, concat, moduleToGen, oneScope));
        context.indent--;

        concat.addIndent(context.indent);
        CONCAT_FIXED_STR(concat, "}");
        concat.addEol();
        concat.addEol();
    }

    // Unnamed scope
    else
    {
        SWAG_CHECK(outputScopeContent(context, concat, moduleToGen, scope));
        for (auto oneScope : scope->childScopes)
            SWAG_CHECK(outputScope(context, concat, moduleToGen, oneScope));
    }

    return true;
}
