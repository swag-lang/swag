#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ErrorIds.h"

namespace Ast
{
    bool checkIsPublic(OutputContext& context, AstNode* node)
    {
        if (!context.forExport)
            return true;

        auto symbol   = node->resolvedSymbolName;
        auto overload = node->resolvedSymbolOverload;

        if (overload)
        {
            if (overload->node->sourceFile->isBootstrapFile ||
                overload->node->sourceFile->isRuntimeFile ||
                overload->node->sourceFile->forceExport ||
                overload->node->sourceFile->imported)
                return true;
        }

        if (symbol && overload && symbol->name[0] != '@' && overload->node->ownerScope->isGlobalOrImpl())
        {
            if (((symbol->kind == SymbolKind::Variable) && (overload->flags & OVERLOAD_VAR_GLOBAL)) ||
                (symbol->kind == SymbolKind::Function) ||
                (symbol->kind == SymbolKind::Alias) ||
                (symbol->kind == SymbolKind::TypeAlias))
            {
                SWAG_VERIFY(overload->node->attributeFlags & ATTRIBUTE_PUBLIC, node->sourceFile->report({node, Utf8::format(g_E[Err0316], node->token.text.c_str())}));
            }
        }

        return true;
    }

    void incIndentStatement(AstNode* node, int& indent)
    {
        if (node->kind == AstNodeKind::CompilerIfBlock && node->childs.front()->kind == AstNodeKind::Statement)
            return;
        if (node->kind != AstNodeKind::Statement)
            indent++;
    }

    void decIndentStatement(AstNode* node, int& indent)
    {
        if (node->kind == AstNodeKind::CompilerIfBlock && node->childs.front()->kind == AstNodeKind::Statement)
            return;
        if (node->kind != AstNodeKind::Statement)
            indent--;
    }

    bool outputAttributesUsage(OutputContext& context, Concat& concat, TypeInfoFuncAttr* typeFunc)
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

        if ((typeFunc->attributeUsage & AttributeUsage::All) == AttributeUsage::All)
            CONCAT_FIXED_STR(concat, "AttributeUsage.All");
        else
        {
            ADD_ATTRUSAGE(AttributeUsage::Enum, "Enum");
            ADD_ATTRUSAGE(AttributeUsage::EnumValue, "EnumValue");
            ADD_ATTRUSAGE(AttributeUsage::StructVariable, "Field");
            ADD_ATTRUSAGE(AttributeUsage::GlobalVariable, "GlobalVariable");
            ADD_ATTRUSAGE(AttributeUsage::Struct, "Struct");
            ADD_ATTRUSAGE(AttributeUsage::Function, "Function");
        }

        concat.addString(")]");
        concat.addEol();
        return true;
    }

    bool outputAttributes(OutputContext& context, Concat& concat, AttributeList& attributes)
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
                        Ast::outputLiteral(context, concat, one.node, oneParam.typeInfo, oneParam.value);
                    }

                    concat.addChar(')');
                }
            }

            CONCAT_FIXED_STR(concat, "]");
            concat.addEol();
        }

        return true;
    }

    bool outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value)
    {
        if (typeInfo == g_TypeMgr->typeInfoNull)
        {
            CONCAT_FIXED_STR(concat, "null");
            return true;
        }

        if (typeInfo->kind == TypeInfoKind::TypeListTuple || typeInfo->kind == TypeInfoKind::TypeListArray)
        {
            SWAG_CHECK(Ast::output(context, concat, node));
            return true;
        }

        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
        auto str = literalToString(typeInfo, value);
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

    bool outputLambdaExpression(OutputContext& context, Concat& concat, AstNode* node)
    {
        AstFuncDecl* funcDecl = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        concat.addString("@(");

        if (funcDecl->parameters && !funcDecl->parameters->childs.empty())
        {
            for (auto p : funcDecl->parameters->childs)
            {
                if (p != funcDecl->parameters->childs.front())
                    CONCAT_FIXED_STR(concat, ", ");
                concat.addString(p->token.text);
                if (!p->childs.empty())
                {
                    CONCAT_FIXED_STR(concat, ": ");
                    SWAG_CHECK(output(context, concat, p->childs.front()));
                }

                auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
                if (param->assignment)
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(output(context, concat, param->assignment));
                }
            }
        }

        concat.addChar(')');

        if (funcDecl->flags & AST_SHORT_LAMBDA)
        {
            CONCAT_FIXED_STR(concat, " => ");
            SWAG_ASSERT(funcDecl->content->kind == AstNodeKind::Return);
            SWAG_CHECK(output(context, concat, funcDecl->content->childs.front()));
        }
        else
        {
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, funcDecl->content));
        }

        return true;
    }

    bool outputVar(OutputContext& context, Concat& concat, const char* kindName, AstVarDecl* node)
    {
        concat.addIndent(context.indent);
        if (node->flags & AST_DECL_USING)
            CONCAT_FIXED_STR(concat, "using ");
        if (kindName)
            concat.addString(kindName);

        if (!(node->flags & AST_AUTO_NAME))
        {
            concat.addString(node->token.text);
            if (node->type)
            {
                CONCAT_FIXED_STR(concat, ": ");
                outputType(context, concat, node->typeInfo);

                // Type with parameters
                if (node->type->kind == AstNodeKind::TypeExpression)
                {
                    auto typeNode = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
                    if (typeNode->identifier && typeNode->identifier->childs.back()->kind == AstNodeKind::Identifier)
                    {
                        auto typeId = CastAst<AstIdentifier>(typeNode->identifier->childs.back(), AstNodeKind::Identifier);
                        if (typeId->callParameters)
                        {
                            concat.addChar('{');
                            SWAG_CHECK(output(context, concat, typeId->callParameters));
                            concat.addChar('}');
                        }
                    }
                }
            }
        }
        else
        {
            outputType(context, concat, node->typeInfo);
        }

        if (node->assignment)
        {
            CONCAT_FIXED_STR(concat, " = ");
            SWAG_CHECK(output(context, concat, node->assignment));
        }

        return true;
    }

    bool outputAttributes(OutputContext& context, Concat& concat, TypeInfo* typeInfo)
    {
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

    bool outputGenericParameters(OutputContext& context, Concat& concat, AstNode* node)
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
                outputType(context, concat, varDecl->type->typeInfo);
            }

            if (varDecl->assignment)
            {
                CONCAT_FIXED_STR(concat, " = ");
                SWAG_CHECK(output(context, concat, varDecl->assignment));
            }

            idx++;
        }

        CONCAT_FIXED_STR(concat, ")");
        return true;
    }

    bool outputStruct(OutputContext& context, Concat& concat, TypeInfoStruct* typeStruct, AstStruct* node)
    {
        SWAG_CHECK(outputAttributes(context, concat, typeStruct));

        if (!(node->structFlags & STRUCTFLAG_ANONYMOUS))
            concat.addIndent(context.indent);

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

        concat.addEol();
        concat.addIndent(context.indent);
        CONCAT_FIXED_STR(concat, "{");
        concat.addEol();

        // Opaque export. Just simulate structure with the correct size.
        // Simulate also TYPEINFO_STRUCT_HAS_INIT_VALUES and TYPEINFO_STRUCT_ALL_UNINITIALIZED flags.
        if (node->attributeFlags & ATTRIBUTE_OPAQUE)
        {
            // We initialize one field with a dummy value to force the compiler to acknowledge that the
            // struct has some initialized fields (not all to zero)
            if (typeStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
            {
                if (typeStruct->sizeOf != 1)
                {
                    concat.addIndent(context.indent + 1);
                    concat.addStringFormat("padding0: [%llu] u8", typeStruct->sizeOf - 1);
                    concat.addEol();
                }

                concat.addIndent(context.indent + 1);
                CONCAT_FIXED_STR(concat, "padding1: u8 = 1");
                concat.addEol();
            }

            // Everything in the structure is not initialized
            else if (typeStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED)
            {
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
        }
        else
        {
            context.indent++;
            for (auto p : typeStruct->fields)
            {
                SWAG_CHECK(outputAttributes(context, concat, p->attributes));

                // Struct/interface content
                if (p->declNode->kind == AstNodeKind::VarDecl)
                {
                    auto varDecl = CastAst<AstVarDecl>(p->declNode, AstNodeKind::VarDecl);
                    SWAG_CHECK(outputVar(context, concat, nullptr, varDecl));
                    concat.addEol();
                }
            }

            for (auto p : typeStruct->consts)
            {
                auto varDecl = CastAst<AstVarDecl>(p->declNode, AstNodeKind::ConstDecl);
                SWAG_CHECK(outputAttributes(context, concat, p->attributes));
                SWAG_CHECK(outputVar(context, concat, "const ", varDecl));
                concat.addEol();
            }

            context.indent--;
        }

        concat.addIndent(context.indent);
        CONCAT_FIXED_STR(concat, "}");

        if (!(node->structFlags & STRUCTFLAG_ANONYMOUS))
            concat.addEol();
        concat.addEol();

        return true;
    }

    bool outputTypeTuple(OutputContext& context, Concat& concat, TypeInfo* typeInfo)
    {
        typeInfo = TypeManager::concreteReference(typeInfo);
        SWAG_ASSERT(typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE);
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        auto nodeStruct = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
        if (nodeStruct->structFlags & STRUCTFLAG_ANONYMOUS)
            return outputStruct(context, concat, typeStruct, (AstStruct*) typeStruct->declNode);

        concat.addString("{");
        int idx = 0;
        for (auto field : typeStruct->fields)
        {
            if (idx)
                CONCAT_FIXED_STR(concat, ", ");

            if (field->declNode && field->declNode->kind == AstNodeKind::VarDecl)
            {
                auto varDecl = CastAst<AstVarDecl>(field->declNode, AstNodeKind::VarDecl);
                SWAG_CHECK(outputVar(context, concat, nullptr, varDecl));
            }
            else
            {
                if (!field->namedParam.empty() && field->namedParam.find("item") != 0)
                {
                    concat.addString(field->namedParam);
                    concat.addString(":");
                }

                SWAG_CHECK(outputType(context, concat, field->typeInfo));
            }

            idx++;
        }

        concat.addString("}");
        return true;
    }

    bool outputType(OutputContext& context, Concat& concat, TypeInfo* typeInfo)
    {
        if (typeInfo->kind == TypeInfoKind::Lambda)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::Lambda);
            CONCAT_FIXED_STR(concat, "func(");
            for (auto p : typeFunc->parameters)
            {
                if (p != typeFunc->parameters[0])
                    CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(outputType(context, concat, p->typeInfo));
            }

            CONCAT_FIXED_STR(concat, ")->");
            SWAG_CHECK(outputType(context, concat, typeFunc->returnType));

            if (typeInfo->flags & TYPEINFO_CAN_THROW)
                CONCAT_FIXED_STR(concat, " throw");
        }
        else
        {
            if (typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
                return outputTypeTuple(context, concat, typeInfo);

            // Be sure to keep the original reference. That way, only user references are exported as references, otherwise
            // we take the 'converted' original struct type
            if (typeInfo->kind == TypeInfoKind::Reference)
            {
                auto typeRef = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
                if (typeRef->originalType)
                    typeInfo = typeRef->originalType;
            }

            if (typeInfo->flags & TYPEINFO_SELF)
            {
                if (typeInfo->flags & TYPEINFO_CONST)
                    CONCAT_FIXED_STR(concat, "const self");
                else
                    CONCAT_FIXED_STR(concat, "self");
            }
            else
            {
                typeInfo->computeScopedNameExport();
                SWAG_ASSERT(!typeInfo->scopedNameExport.empty());
                concat.addString(typeInfo->scopedNameExport);
            }
        }

        return true;
    }

    bool output(OutputContext& context, Concat& concat, AstNode* node)
    {
        if (!node)
            return true;
        if (node->extension && node->extension->exportNode)
            node = node->extension->exportNode;
        if (node->flags & AST_GENERATED)
            return true;

        switch (node->kind)
        {
        case AstNodeKind::FuncDeclType:
            if (!node->childs.empty())
            {
                CONCAT_FIXED_STR(concat, "->");
                SWAG_CHECK(output(context, concat, node->childs.front()));
            }
            break;

        case AstNodeKind::FuncDeclParams:
        {
            concat.addChar('(');
            bool first = true;
            for (auto c : node->childs)
            {
                if (!first)
                    CONCAT_FIXED_STR(concat, ", ");
                first = false;
                SWAG_CHECK(output(context, concat, c));
            }
            concat.addChar(')');
            break;
        }

        case AstNodeKind::FuncDecl:
        {
            auto nodeFunc = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
            concat.addEolIndent(context.indent);
            CONCAT_FIXED_STR(concat, "func ");
            if (nodeFunc->genericParameters)
                SWAG_CHECK(output(context, concat, nodeFunc->genericParameters));
            concat.addString(nodeFunc->token.text);
            if (!nodeFunc->parameters)
                CONCAT_FIXED_STR(concat, "()");
            else
                SWAG_CHECK(output(context, concat, nodeFunc->parameters));
            if (nodeFunc->returnType)
                SWAG_CHECK(output(context, concat, nodeFunc->returnType));
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, nodeFunc->content));
            break;
        }

        case AstNodeKind::StructContent:
            concat.addEolIndent(context.indent);
            CONCAT_FIXED_STR(concat, "{ ");
            concat.addEol();
            for (auto c : node->childs)
            {
                concat.addIndent(context.indent + 1);
                SWAG_CHECK(output(context, concat, c));
                concat.addEol();
            }

            concat.addIndent(context.indent);
            CONCAT_FIXED_STR(concat, "}");
            concat.addEol();
            break;

        case AstNodeKind::StructDecl:
        case AstNodeKind::InterfaceDecl:
        {
            auto nodeStruct = CastAst<AstStruct>(node, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
            switch (node->kind)
            {
            case AstNodeKind::StructDecl:
                CONCAT_FIXED_STR(concat, "struct ");
                break;
            case AstNodeKind::InterfaceDecl:
                CONCAT_FIXED_STR(concat, "interface ");
                break;
            }
            concat.addString(nodeStruct->token.text);
            concat.addIndent(context.indent);
            SWAG_CHECK(output(context, concat, nodeStruct->content));
            break;
        }

        case AstNodeKind::Defer:
            CONCAT_FIXED_STR(concat, "defer ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            concat.addEol();
            break;

        case AstNodeKind::AttrUse:
        {
            auto nodeAttr = CastAst<AstAttrUse>(node, AstNodeKind::AttrUse);
            bool first    = true;
            CONCAT_FIXED_STR(concat, "#[");
            for (auto s : nodeAttr->childs)
            {
                if (s == nodeAttr->content)
                    continue;
                if (!first)
                    CONCAT_FIXED_STR(concat, ", ");
                first = false;
                SWAG_CHECK(output(context, concat, s));
            }

            concat.addChar(']');
            concat.addEol();
            concat.addIndent(context.indent);
            SWAG_CHECK(output(context, concat, nodeAttr->content));
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
            SWAG_CHECK(output(context, concat, node->childs.front()));
            if (node->childs.size() == 2)
            {
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(output(context, concat, node->childs.back()));
            }
            concat.addChar(')');
            break;
        case AstNodeKind::Drop:
            CONCAT_FIXED_STR(concat, "@drop(");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            if (node->childs.size() == 2)
            {
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(output(context, concat, node->childs.back()));
            }
            concat.addChar(')');
            break;
        case AstNodeKind::PostMove:
            CONCAT_FIXED_STR(concat, "@postmove(");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            if (node->childs.size() == 2)
            {
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(output(context, concat, node->childs.back()));
            }
            concat.addChar(')');
            break;
        case AstNodeKind::PostCopy:
            CONCAT_FIXED_STR(concat, "@postcopy(");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            if (node->childs.size() == 2)
            {
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(output(context, concat, node->childs.back()));
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
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;
        }

        case AstNodeKind::MakePointerLambda:
        {
            auto lambdaNode = CastAst<AstMakePointerLambda>(node, AstNodeKind::MakePointerLambda);
            outputLambdaExpression(context, concat, lambdaNode->lambda);
            break;
        }

        case AstNodeKind::NoDrop:
            CONCAT_FIXED_STR(concat, "nodrop ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;

        case AstNodeKind::Move:
            CONCAT_FIXED_STR(concat, "move ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;

        case AstNodeKind::ArrayPointerIndex:
        {
            auto arrayNode = CastAst<AstArrayPointerIndex>(node, AstNodeKind::ArrayPointerIndex);
            if (arrayNode->specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF)
            {
                CONCAT_FIXED_STR(concat, "dref ");
                SWAG_CHECK(output(context, concat, arrayNode->array));
            }
            else
            {
                SWAG_CHECK(output(context, concat, arrayNode->array));
                concat.addChar('[');
                SWAG_CHECK(output(context, concat, arrayNode->access));
                concat.addChar(']');
            }
            break;
        }

        case AstNodeKind::ExpressionList:
        {
            auto exprNode = CastAst<AstExpressionList>(node, AstNodeKind::ExpressionList);
            if (exprNode->specFlags & AST_SPEC_EXPRLIST_FORTUPLE)
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
                SWAG_CHECK(output(context, concat, child));
            }

            if (exprNode->specFlags & AST_SPEC_EXPRLIST_FORTUPLE)
                concat.addChar('}');
            else
                concat.addChar(']');
            break;
        }

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
                SWAG_CHECK(output(context, concat, funcDecl->content));
                decIndentStatement(funcDecl->content, context.indent);
            }
            else
            {
                SWAG_CHECK(output(context, concat, front));
            }
            break;
        }

        case AstNodeKind::CompilerIfBlock:
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;

        case AstNodeKind::CompilerMacro:
            CONCAT_FIXED_STR(concat, "#macro");
            incIndentStatement(node->childs.front(), context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, node->childs.front()));
            decIndentStatement(node->childs.front(), context.indent);
            break;

        case AstNodeKind::CompilerInline:
            CONCAT_FIXED_STR(concat, "#inline");
            incIndentStatement(node->childs.front(), context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, node->childs.front()));
            decIndentStatement(node->childs.front(), context.indent);
            break;

        case AstNodeKind::CompilerMixin:
        {
            auto compilerMixin = CastAst<AstCompilerMixin>(node, AstNodeKind::CompilerMixin);
            CONCAT_FIXED_STR(concat, "#mixin ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
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
                    SWAG_CHECK(output(context, concat, m.second));
                    CONCAT_FIXED_STR(concat, "; ");
                }
                concat.addChar('}');
            }
            break;
        }

        case AstNodeKind::CompilerPrint:
            CONCAT_FIXED_STR(concat, "#print ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;

        case AstNodeKind::CompilerSemError:
            CONCAT_FIXED_STR(concat, "#semerror ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;

        case AstNodeKind::CompilerAssert:
            CONCAT_FIXED_STR(concat, "#assert(");
            SWAG_CHECK(output(context, concat, node->childs[0]));
            if (node->childs.size() > 1)
            {
                concat.addChar(',');
                SWAG_CHECK(output(context, concat, node->childs[1]));
            }

            concat.addChar(')');
            break;

        case AstNodeKind::CompilerIf:
        {
            auto compilerIf = CastAst<AstIf>(node, AstNodeKind::CompilerIf);
            CONCAT_FIXED_STR(concat, "#if ");
            SWAG_CHECK(output(context, concat, compilerIf->boolExpression));

            incIndentStatement(compilerIf->ifBlock, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, compilerIf->ifBlock));
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
                SWAG_CHECK(output(context, concat, compilerIf->elseBlock));
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
            SWAG_CHECK(output(context, concat, compilerIf->boolExpression));

            incIndentStatement(compilerIf->ifBlock, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, compilerIf->ifBlock));
            decIndentStatement(compilerIf->ifBlock, context.indent);

            if (compilerIf->elseBlock)
            {
                concat.addEolIndent(context.indent);
                CONCAT_FIXED_STR(concat, "else");
                incIndentStatement(compilerIf->elseBlock, context.indent);
                concat.addEolIndent(context.indent);
                SWAG_CHECK(output(context, concat, compilerIf->elseBlock));
                decIndentStatement(compilerIf->elseBlock, context.indent);
            }
            break;
        }

        case AstNodeKind::For:
        {
            auto forNode = CastAst<AstFor>(node, AstNodeKind::For);
            CONCAT_FIXED_STR(concat, "for ");
            SWAG_CHECK(output(context, concat, forNode->preExpression));
            CONCAT_FIXED_STR(concat, "; ");
            SWAG_CHECK(output(context, concat, forNode->boolExpression));
            CONCAT_FIXED_STR(concat, "; ");
            SWAG_CHECK(output(context, concat, forNode->postExpression));
            incIndentStatement(forNode->block, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, forNode->block));
            decIndentStatement(forNode->block, context.indent);
            break;
        }

        case AstNodeKind::Visit:
        {
            auto visitNode = CastAst<AstVisit>(node, AstNodeKind::Visit);
            CONCAT_FIXED_STR(concat, "visit ");

            if (visitNode->specFlags & AST_SPEC_VISIT_WANTPOINTER)
                concat.addChar('*');

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
            SWAG_CHECK(output(context, concat, visitNode->expression));
            incIndentStatement(visitNode->block, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, visitNode->block));
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

            SWAG_CHECK(output(context, concat, loopNode->expression));
            incIndentStatement(loopNode->block, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, loopNode->block));
            decIndentStatement(loopNode->block, context.indent);
            break;
        }

        case AstNodeKind::While:
        {
            auto whileNode = CastAst<AstWhile>(node, AstNodeKind::While);
            CONCAT_FIXED_STR(concat, "while ");
            SWAG_CHECK(output(context, concat, whileNode->boolExpression));
            incIndentStatement(whileNode->block, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, whileNode->block));
            decIndentStatement(whileNode->block, context.indent);
            break;
        }

        case AstNodeKind::CompilerSpecialFunction:
        {
            switch (node->token.id)
            {
            case TokenId::CompilerFunction:
                CONCAT_FIXED_STR(concat, "#function");
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
                    SWAG_CHECK(output(context, concat, node->childs[0]));
                    concat.addChar(')');
                }
                break;

            case TokenId::CompilerOs:
                CONCAT_FIXED_STR(concat, "#os");
                break;
            case TokenId::CompilerAbi:
                CONCAT_FIXED_STR(concat, "#abi");
                break;
            case TokenId::CompilerBackend:
                CONCAT_FIXED_STR(concat, "#backend");
                break;
            case TokenId::CompilerArch:
                CONCAT_FIXED_STR(concat, "#arch");
                break;
            case TokenId::CompilerBuildCfg:
                CONCAT_FIXED_STR(concat, "#cfg");
                break;
            case TokenId::CompilerHasTag:
                CONCAT_FIXED_STR(concat, "#hastag(");
                SWAG_CHECK(output(context, concat, node->childs[0]));
                CONCAT_FIXED_STR(concat, ")");
                break;
            case TokenId::CompilerGetTag:
                CONCAT_FIXED_STR(concat, "#gettag(");
                SWAG_CHECK(output(context, concat, node->childs[0]));
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(output(context, concat, node->childs[1]));
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(output(context, concat, node->childs[2]));
                CONCAT_FIXED_STR(concat, ")");
                break;
            default:
                return node->sourceFile->internalError(node, "Ast::output, unknown compiler function");
            }
            break;
        }

        case AstNodeKind::ConditionalExpression:
            SWAG_CHECK(output(context, concat, node->childs[0]));
            CONCAT_FIXED_STR(concat, " ? (");
            SWAG_CHECK(output(context, concat, node->childs[1]));
            CONCAT_FIXED_STR(concat, ") : (");
            SWAG_CHECK(output(context, concat, node->childs[2]));
            concat.addChar(')');
            break;

        case AstNodeKind::Alias:
        {
            CONCAT_FIXED_STR(concat, "alias ");
            concat.addString(node->token.text);
            CONCAT_FIXED_STR(concat, " = ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;
        }

        case AstNodeKind::VarDecl:
        case AstNodeKind::FuncDeclParam:
        {
            AstVarDecl* varDecl = static_cast<AstVarDecl*>(node);
            if (varDecl->flags & AST_DECL_USING)
                CONCAT_FIXED_STR(concat, "using ");
            if (varDecl->type && node->ownerFct && node->kind != AstNodeKind::FuncDeclParam)
                CONCAT_FIXED_STR(concat, "var ");

            if (varDecl->type)
            {
                if (!varDecl->publicName.empty())
                    concat.addString(varDecl->publicName);
                else
                    concat.addString(varDecl->token.text);

                if (!(varDecl->type->flags & AST_GENERATED))
                {
                    CONCAT_FIXED_STR(concat, ": ");
                    SWAG_CHECK(output(context, concat, varDecl->type));
                }
                else
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    auto typeExpr = CastAst<AstTypeExpression>(varDecl->type, AstNodeKind::TypeExpression);
                    SWAG_ASSERT(!varDecl->assignment);
                    SWAG_ASSERT(typeExpr->identifier);
                    SWAG_ASSERT(varDecl->type->typeInfo && (varDecl->type->typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE));
                    auto id = CastAst<AstIdentifier>(typeExpr->identifier->childs.back(), AstNodeKind::Identifier);
                    CONCAT_FIXED_STR(concat, "@{");
                    SWAG_CHECK(output(context, concat, id->callParameters));
                    CONCAT_FIXED_STR(concat, "}");
                }

                if (varDecl->assignment)
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(output(context, concat, varDecl->assignment));
                }
            }
            else
            {
                if (!varDecl->publicName.empty())
                    concat.addString(varDecl->publicName);
                else
                    concat.addString(varDecl->token.text);
                CONCAT_FIXED_STR(concat, " := ");
                if (varDecl->assignment)
                    SWAG_CHECK(output(context, concat, varDecl->assignment));
            }

            break;
        }

        case AstNodeKind::ConstDecl:
        {
            AstVarDecl* varDecl = static_cast<AstVarDecl*>(node);
            CONCAT_FIXED_STR(concat, "const ");

            if (!varDecl->publicName.empty())
                concat.addString(varDecl->publicName);
            else
                concat.addString(varDecl->token.text);

            if (varDecl->type)
            {
                CONCAT_FIXED_STR(concat, ": ");
                SWAG_CHECK(output(context, concat, varDecl->type));
            }

            if (varDecl->assignment)
            {
                CONCAT_FIXED_STR(concat, " = ");
                SWAG_CHECK(output(context, concat, varDecl->assignment));
            }
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
                SWAG_CHECK(output(context, concat, child));
                idx++;
            }

            break;
        }

        case AstNodeKind::Return:
        {
            CONCAT_FIXED_STR(concat, "return ");
            if (!node->childs.empty())
                SWAG_CHECK(output(context, concat, node->childs.front()));
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
                SWAG_CHECK(output(context, concat, child));
                idx++;
            }

            concat.addChar(')');
            break;
        }

        case AstNodeKind::IdentifierRef:
        {
            if (node->specFlags & AST_SPEC_IDENTIFIERREF_AUTO_SCOPE)
                CONCAT_FIXED_STR(concat, ".");
            auto back = node->childs.back();
            if (back->resolvedSymbolName)
            {
                auto symbol = back->resolvedSymbolName;
                if (symbol && symbol->ownerTable->scope->isGlobal())
                {
                    SWAG_CHECK(output(context, concat, back));
                    break;
                }
            }

            int idx = 0;
            for (auto child : node->childs)
            {
                // No need to output some scope names because the identifier will be generated with a full name
                if (child->resolvedSymbolName &&
                    child->resolvedSymbolName->kind == SymbolKind::Namespace &&
                    child == node->childs.front() &&
                    node->childs.size() > 1)
                    continue;

                if (idx)
                    CONCAT_FIXED_STR(concat, ".");
                SWAG_CHECK(output(context, concat, child));
                idx++;
            }

            break;
        }

        case AstNodeKind::Identifier:
            if (node->specFlags & AST_SPEC_IDENTIFIER_BACKTICK)
                concat.addChar('`');
        case AstNodeKind::FuncCall:
        {
            auto identifier = static_cast<AstIdentifier*>(node);
            auto symbol     = identifier->resolvedSymbolName;

            SWAG_CHECK(checkIsPublic(context, node));
            if (symbol && symbol->ownerTable->scope->isGlobal())
                concat.addString(symbol->getFullName());
            else
                concat.addString(node->token.text);

            if (identifier->genericParameters)
            {
                concat.addChar('\'');
                concat.addChar('(');
                SWAG_CHECK(output(context, concat, identifier->genericParameters));
                concat.addChar(')');
            }

            if (identifier->callParameters)
            {
                if (identifier->callParameters->flags & AST_CALL_FOR_STRUCT)
                    concat.addChar('{');
                else
                    concat.addChar('(');
                SWAG_CHECK(output(context, concat, identifier->callParameters));
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
            SWAG_CHECK(output(context, concat, nodeSwitch->expression));
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
                        SWAG_CHECK(output(context, concat, e));
                        first = false;
                    }
                }
                concat.addChar(':');
                concat.addEolIndent(context.indent);
                SWAG_CHECK(output(context, concat, c->block));
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
                SWAG_CHECK(output(context, concat, child));
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
                node->childs.front()->kind != AstNodeKind::LabelBreakable &&
                node->childs.front()->kind != AstNodeKind::Switch)
            {
                context.indent++;
                concat.addIndent(1);
                SWAG_CHECK(output(context, concat, node->childs.front()));
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
                    SWAG_CHECK(output(context, concat, child));
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

            SWAG_CHECK(output(context, concat, funcParam->childs.front()));
            break;
        }

        case AstNodeKind::FuncCallParams:
        {
            auto funcCallParams = CastAst<AstFuncCallParams>(node, AstNodeKind::FuncCallParams);
            bool first          = true;
            for (auto child : funcCallParams->childs)
            {
                if (child->flags & AST_GENERATED)
                    continue;
                if (!first)
                    CONCAT_FIXED_STR(concat, ", ");
                first = false;
                SWAG_CHECK(output(context, concat, child));
            }

            // Capture parameters
            if (!funcCallParams->aliasNames.empty())
            {
                first = true;
                CONCAT_FIXED_STR(concat, " <- |");
                for (auto& t : funcCallParams->aliasNames)
                {
                    if (!first)
                        CONCAT_FIXED_STR(concat, ", ");
                    concat.addString(t.text);
                    first = false;
                }

                CONCAT_FIXED_STR(concat, "|");
            }

            break;
        }

        case AstNodeKind::SingleOp:
            concat.addString(node->token.text);
            SWAG_CHECK(output(context, concat, node->childs[0]));
            break;

        case AstNodeKind::NullConditionalExpression:
            SWAG_CHECK(output(context, concat, node->childs[0]));
            concat.addString(" orelse ");
            SWAG_CHECK(output(context, concat, node->childs[1]));
            break;

        case AstNodeKind::AffectOp:
        {
            auto opNode = CastAst<AstOp>(node, AstNodeKind::AffectOp);
            SWAG_CHECK(output(context, concat, node->childs[0]));
            concat.addChar(' ');
            concat.addString(node->token.text);
            if (opNode->specFlags & AST_SPEC_OP_SAFE)
                CONCAT_FIXED_STR(concat, ",safe");
            if (opNode->specFlags & AST_SPEC_OP_SMALL)
                CONCAT_FIXED_STR(concat, ",small");
            concat.addChar(' ');
            SWAG_CHECK(output(context, concat, node->childs[1]));
            break;
        }

        case AstNodeKind::FactorOp:
        {
            auto opNode = CastAst<AstOp>(node, AstNodeKind::FactorOp);
            concat.addChar('(');
            SWAG_CHECK(output(context, concat, node->childs[0]));
            concat.addChar(' ');
            concat.addString(node->token.text);
            if (opNode->specFlags & AST_SPEC_OP_SAFE)
                CONCAT_FIXED_STR(concat, ",safe");
            if (opNode->specFlags & AST_SPEC_OP_SMALL)
                CONCAT_FIXED_STR(concat, ",small");
            concat.addChar(' ');
            SWAG_CHECK(output(context, concat, node->childs[1]));
            concat.addChar(')');
            break;
        }

        case AstNodeKind::BinaryOp:
            concat.addChar('(');
            SWAG_CHECK(output(context, concat, node->childs[0]));
            concat.addChar(' ');
            concat.addString(node->token.text);
            concat.addChar(' ');
            SWAG_CHECK(output(context, concat, node->childs[1]));
            concat.addChar(')');
            break;

        case AstNodeKind::AutoCast:
            CONCAT_FIXED_STR(concat, "acast ");
            SWAG_CHECK(output(context, concat, node->childs[0]));
            break;

        case AstNodeKind::Cast:
            CONCAT_FIXED_STR(concat, "cast(");
            SWAG_CHECK(output(context, concat, node->childs[0]));
            CONCAT_FIXED_STR(concat, ") ");
            SWAG_CHECK(output(context, concat, node->childs[1]));
            break;

        case AstNodeKind::BitCast:
            CONCAT_FIXED_STR(concat, "bitcast(");
            SWAG_CHECK(output(context, concat, node->childs[0]));
            CONCAT_FIXED_STR(concat, ") ");
            SWAG_CHECK(output(context, concat, node->childs[1]));
            break;

        case AstNodeKind::TypeExpression:
        {
            AstTypeExpression* typeNode = static_cast<AstTypeExpression*>(node);
            if (typeNode->typeFlags & TYPEFLAG_RETVAL)
            {
                CONCAT_FIXED_STR(concat, "retval");
                break;
            }

            if (typeNode->typeFlags & TYPEFLAG_ISCONST)
                CONCAT_FIXED_STR(concat, "const ");
            if (typeNode->typeFlags & TYPEFLAG_ISSLICE)
                CONCAT_FIXED_STR(concat, "[..] ");
            if (typeNode->arrayDim == UINT8_MAX)
                CONCAT_FIXED_STR(concat, "[] ");
            else if (typeNode->arrayDim)
            {
                CONCAT_FIXED_STR(concat, "[");
                for (int i = 0; i < typeNode->arrayDim; i++)
                {
                    if (i)
                        CONCAT_FIXED_STR(concat, ", ");
                    SWAG_CHECK(output(context, concat, node->childs[i]));
                }

                CONCAT_FIXED_STR(concat, "] ");
            }

            for (int i = 0; i < typeNode->ptrCount; i++)
                concat.addChar('*');
            if (typeNode->identifier)
            {
                SWAG_CHECK(output(context, concat, typeNode->identifier));
            }
            else
            {
                auto literalType = typeNode->literalType;
                if (!literalType)
                    literalType = TypeManager::literalTypeToType(typeNode->token);
                SWAG_ASSERT(literalType);
                SWAG_ASSERT(!literalType->name.empty());
                concat.addString(literalType->name);
            }

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
                SWAG_CHECK(output(context, concat, node->childs[0]));
            }
            break;

        case AstNodeKind::LabelBreakable:
            CONCAT_FIXED_STR(concat, "label ");
            concat.addString(node->token.text);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, node->childs[0]));
            break;

        case AstNodeKind::Range:
            SWAG_CHECK(output(context, concat, node->childs[0]));
            CONCAT_FIXED_STR(concat, "..");
            SWAG_CHECK(output(context, concat, node->childs[1]));
            break;

        default:
            return node->sourceFile->internalError(node, "Ast::output, unknown node");
        }

        return true;
    }
}; // namespace Ast
