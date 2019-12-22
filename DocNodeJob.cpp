#include "pch.h"
#include "DocNodeJob.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "DocHtmlHelper.h"
#include "AstNode.h"
#include "Scope.h"
#include "OutputFile.h"
#include "Ast.h"
#include "TypeManager.h"

Pool<DocNodeJob> g_Pool_docNodeJob;

void DocNodeJob::emitFuncSignature(OutputFile& concat, TypeInfoFuncAttr* typeFunc, AstFuncDecl* funcNode, bool light)
{
    if (!light)
        CONCAT_FIXED_STR(concat, "func ");
    concat.addString(funcNode->name.c_str());
    CONCAT_FIXED_STR(concat, "(");

    if (funcNode->parameters)
    {
        uint32_t idx = 0;
        for (auto p : funcNode->parameters->childs)
        {
            if (p->name != "self")
            {
                if (!light)
                {
                    concat.addString(p->name);
                    CONCAT_FIXED_STR(concat, ": ");
                }

                concat.addString(light ? p->typeInfo->name : p->typeInfo->getFullName());
            }
            else
            {
                concat.addString(p->name);
            }

            AstVarDecl* varDecl = CastAst<AstVarDecl>(p, AstNodeKind::VarDecl, AstNodeKind::FuncDeclParam);
            if (varDecl->assignment)
            {
                CONCAT_FIXED_STR(concat, " = ");
                Ast::output(concat, varDecl->assignment);
            }

            if (idx != funcNode->parameters->childs.size() - 1)
                CONCAT_FIXED_STR(concat, ", ");
            idx++;
        }
    }

    CONCAT_FIXED_STR(concat, ")");

    if (typeFunc->returnType && typeFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        CONCAT_FIXED_STR(concat, "->");
        concat.addString(light ? typeFunc->returnType->name : typeFunc->returnType->getFullName());
    }

    concat.addEol();
}

Utf8 DocNodeJob::referencableType(TypeInfo* typeInfo)
{
    Utf8 name;

    if (typeInfo->kind == TypeInfoKind::Pointer)
        name = ((TypeInfoPointer*) typeInfo)->finalType->fullname;
    else
        name = typeInfo->fullname;

    return name;
}

void DocNodeJob::emitFunctions(OutputFile& outFile)
{
    auto node = nodes.front();
    DocHtmlHelper::summary(outFile, node->docSummary);
    DocHtmlHelper::origin(outFile, node->ownerScope);

    // Overloads
    ///////////////////////////
    if (nodes.size() > 1)
    {
        DocHtmlHelper::sectionTitle1(outFile, "Overloads");
        CONCAT_FIXED_STR(outFile, "<pre class='brush: csharp;'>");
        for (auto one : nodes)
            emitFuncSignature(outFile, (TypeInfoFuncAttr*) one->typeInfo, (AstFuncDecl*) one);
        CONCAT_FIXED_STR(outFile, "</pre>");
    }

    for (auto one : nodes)
    {
        if (nodes.size() > 1)
        {
            CONCAT_FIXED_STR(outFile, "<h2>");
            emitFuncSignature(outFile, (TypeInfoFuncAttr*) one->typeInfo, (AstFuncDecl*) one, true);
            CONCAT_FIXED_STR(outFile, "</h2>\n");
        }

        emitFunction(outFile, one);
    }
}

void DocNodeJob::emitFunction(OutputFile& outFile, AstNode* node)
{
    CONCAT_FIXED_STR(outFile, "<pre class='brush: csharp;'>");
    emitFuncSignature(outFile, (TypeInfoFuncAttr*) node->typeInfo, (AstFuncDecl*) node);
    CONCAT_FIXED_STR(outFile, "</pre>");

    // Parameters
    ///////////////////////////
    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    if (typeFunc->parameters.size())
    {
        DocHtmlHelper::sectionTitle3(outFile, "Parameters");
        for (auto p : typeFunc->parameters)
        {
            auto fullName = referencableType(p->typeInfo);
            DocHtmlHelper::sectionTitle4(outFile, p->namedParam);
            if (!fullName.empty())
            {
                auto parentRef = fullName + ".html";
                outFile.addStringFormat("Type: <a href=\"%s\">%s</a><br/>\n", parentRef.c_str(), fullName.c_str());
            }
            else
            {
                outFile.addStringFormat("Type: %s<br/>\n", p->typeInfo->name.c_str());
            }
        }
    }

    // Return type
    ///////////////////////////
    if (!typeFunc->returnType->isNative(NativeTypeKind::Void))
    {
        auto fullName = referencableType(typeFunc->returnType);
        DocHtmlHelper::sectionTitle3(outFile, "Return Value");
        if (!fullName.empty())
        {
            auto parentRef = fullName + ".html";
            outFile.addStringFormat("Type: <a href=\"%s\">%s</a><br/>\n", parentRef.c_str(), fullName.c_str());
        }
        else
        {
            outFile.addStringFormat("Type: %s<br/>\n", typeFunc->returnType->name.c_str());
        }
    }

    // Description from the user
    ///////////////////////////
    if (!node->docDescription.empty())
    {
        DocHtmlHelper::sectionTitle2(outFile, "Description");
        outFile.addString(node->docDescription);
    }
}

void DocNodeJob::emitEnumSignature(OutputFile& concat, TypeInfoEnum* typeEnum, AstNode* enumNode)
{
    CONCAT_FIXED_STR(concat, "enum ");
    concat.addString(enumNode->name.c_str());
    CONCAT_FIXED_STR(concat, ": ");
    concat.addString(typeEnum->rawType->getFullName());
}

void DocNodeJob::emitEnum(OutputFile& outFile)
{
    auto node = nodes.front();

    DocHtmlHelper::summary(outFile, node->docSummary);
    DocHtmlHelper::origin(outFile, node->ownerScope);

    DocHtmlHelper::sectionTitle1(outFile, "Syntax");
    CONCAT_FIXED_STR(outFile, "<pre class='brush: csharp;'>");
    emitEnumSignature(outFile, (TypeInfoEnum*) node->typeInfo, (AstNode*) node);
    CONCAT_FIXED_STR(outFile, "</pre>");

    DocHtmlHelper::sectionTitle1(outFile, "Members");
    auto typeInfo = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
    DocHtmlHelper::table(outFile, typeInfo->scope, typeInfo->values, false);
}

JobResult DocNodeJob::execute()
{
    OutputFile outFile;
    auto       node  = nodes.front();
    outFile.path = module->documentPath.string() + "/" + node->ownerScope->fullname + "." + node->name + ".html";

    DocHtmlHelper::htmlStart(outFile);
    DocHtmlHelper::title(outFile, format("%s.%s %s", node->ownerScope->name.c_str(), node->name.c_str(), "function"));

    switch (nodes.front()->kind)
    {
    case AstNodeKind::FuncDecl:
        emitFunctions(outFile);
        break;
    case AstNodeKind::EnumDecl:
        emitEnum(outFile);
        break;
    }

    DocHtmlHelper::htmlEnd(outFile);
    outFile.flush(true);

    return JobResult::ReleaseJob;
}
