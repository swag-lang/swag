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

bool DocNodeJob::emitFuncSignature(OutputFile& concat, TypeInfoFuncAttr* typeFunc, AstFuncDecl* funcNode)
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

    return true;
}

void DocNodeJob::emitFunction(OutputFile& outFile)
{
    if (node->ownerScope->kind == ScopeKind::Struct)
        DocHtmlHelper::title(outFile, format("%s.%s %s", node->ownerScope->name.c_str(), node->name.c_str(), "function"));
    else
        DocHtmlHelper::title(outFile, format("%s %s", node->name.c_str(), "function"));

    DocHtmlHelper::summary(outFile, node->docSummary);
    DocHtmlHelper::origin(outFile, node->ownerScope);

    DocHtmlHelper::startSection(outFile, "Syntax");
    CONCAT_FIXED_STR(outFile, "<pre class='brush: csharp;'>");
    emitFuncSignature(outFile, (TypeInfoFuncAttr*) node->typeInfo, (AstFuncDecl*) node);
    CONCAT_FIXED_STR(outFile, "</pre>");
    DocHtmlHelper::endSection(outFile);
}

void DocNodeJob::emitEnum(OutputFile& outFile)
{
    DocHtmlHelper::title(outFile, format("%s %s", node->name.c_str(), "enumeration"));

    DocHtmlHelper::summary(outFile, "Summary");
    DocHtmlHelper::origin(outFile, node->ownerScope);

    DocHtmlHelper::startSection(outFile, "Fields");
    auto typeInfo = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
    DocHtmlHelper::table(outFile, typeInfo->scope, typeInfo->values);
}

JobResult DocNodeJob::execute()
{
    OutputFile outFile;
    outFile.fileName = module->documentPath.string() + "/" + node->ownerScope->fullname + "." + node->name + ".html";
    DocHtmlHelper::htmlStart(outFile);

    switch (node->kind)
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
