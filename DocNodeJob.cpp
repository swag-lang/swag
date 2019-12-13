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

thread_local Pool<DocNodeJob> g_Pool_docNodeJob;

void DocNodeJob::emitFuncSignature(OutputFile& concat, TypeInfoFuncAttr* typeFunc, AstFuncDecl* funcNode)
{
    CONCAT_FIXED_STR(concat, "func ");
    concat.addString(funcNode->name.c_str());
    CONCAT_FIXED_STR(concat, "(");

    if (funcNode->parameters)
    {
        uint32_t idx = 0;
        for (auto p : funcNode->parameters->childs)
        {
            concat.addString(p->name);
            if (p->name != "self")
            {
                CONCAT_FIXED_STR(concat, ": ");
                concat.addString(p->typeInfo->getFullName());
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
        concat.addString(typeFunc->returnType->getFullName());
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

void DocNodeJob::emitFunction(OutputFile& outFile)
{
    auto node = nodes.front();
    DocHtmlHelper::summary(outFile, node->docSummary);
    DocHtmlHelper::origin(outFile, node->ownerScope);

    DocHtmlHelper::sectionTitle1(outFile, "Syntax");
    CONCAT_FIXED_STR(outFile, "<pre class='brush: csharp;'>");
    for (auto one : nodes)
        emitFuncSignature(outFile, (TypeInfoFuncAttr*) one->typeInfo, (AstFuncDecl*) one);
    CONCAT_FIXED_STR(outFile, "</pre>");

    bool firstSection;

    // Parameters
    ///////////////////////////
    firstSection = true;
    set<Utf8> doneParams;
    for (auto one : nodes)
    {
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(one->typeInfo, TypeInfoKind::FuncAttr);
        if (typeFunc->parameters.size())
        {
            if (firstSection)
                DocHtmlHelper::sectionTitle2(outFile, "Parameters");
            firstSection = false;
            for (auto p : typeFunc->parameters)
            {
                auto fullName = referencableType(p->typeInfo);
                auto ref      = p->namedParam + "#" + fullName;
                if (doneParams.find(ref) == doneParams.end())
                {
                    doneParams.insert(ref);
                    DocHtmlHelper::sectionTitle3(outFile, p->namedParam);
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
        }
    }

    // Return type
    ///////////////////////////
    firstSection = true;
    doneParams.clear();
    for (auto one : nodes)
    {
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(one->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeFunc->returnType->isNative(NativeTypeKind::Void))
        {
            auto fullName = referencableType(typeFunc->returnType);
            if (doneParams.find(fullName) == doneParams.end())
            {
                doneParams.insert(fullName);
                if (firstSection)
                    DocHtmlHelper::sectionTitle2(outFile, "Return Value");
                firstSection = false;
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
        }
    }

    // Description from the user
    ///////////////////////////
    firstSection = true;
    for (auto one : nodes)
    {
        if (!one->docDescription.empty())
        {
            if (firstSection)
                DocHtmlHelper::sectionTitle1(outFile, "Description");
            firstSection = false;
            outFile.addString(one->docDescription);
        }
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
    outFile.fileName = module->documentPath.string() + "/" + node->ownerScope->fullname + "." + node->name + ".html";

    DocHtmlHelper::htmlStart(outFile);
    DocHtmlHelper::title(outFile, format("%s.%s %s", node->ownerScope->name.c_str(), node->name.c_str(), "function"));

    switch (nodes.front()->kind)
    {
    case AstNodeKind::FuncDecl:
        emitFunction(outFile);
        break;
    case AstNodeKind::EnumDecl:
        emitEnum(outFile);
        break;
    }

    DocHtmlHelper::htmlEnd(outFile);
    outFile.flush(true);

    return JobResult::ReleaseJob;
}
