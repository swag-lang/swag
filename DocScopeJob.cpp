#include "pch.h"
#include "DocScopeJob.h"
#include "Scope.h"
#include "ThreadManager.h"
#include "OutputFile.h"
#include "Module.h"
#include "AstNode.h"
#include "DocHtmlHelper.h"

thread_local Pool<DocScopeJob> g_Pool_docScopeJob;

JobResult DocScopeJob::execute()
{
    for (auto child : scope->childScopes)
    {
        switch (child->kind)
        {
        case ScopeKind::Namespace:
        case ScopeKind::File:
        case ScopeKind::Module:
        case ScopeKind::Struct:
            auto scopeJob    = g_Pool_docScopeJob.alloc();
            scopeJob->module = module;
            scopeJob->scope  = child;
            g_ThreadMgr.addJob(scopeJob);
            break;
        }
    }

    if (scope->name.empty())
        return JobResult::ReleaseJob;
    if (scope->publicFunc.empty() && scope->publicStruct.empty())
        return JobResult::ReleaseJob;

    OutputFile outFile;
    scope->fullname  = scope->parentScope ? Scope::makeFullName(scope->parentScope->fullname, scope->name) : scope->name;
    outFile.fileName = module->documentPath.string() + "/" + scope->fullname + ".html";
    DocHtmlHelper::htmlStart(outFile);
    DocHtmlHelper::title(outFile, format("%s %s", scope->name.c_str(), Scope::getNakedKindName(scope->kind)));
    DocHtmlHelper::summary(outFile, "Summary");

    if (!scope->parentScope->fullname.empty())
    {
        CONCAT_FIXED_STR(outFile, "<br/><div class=\"origin\">\n");
        auto parentRef = scope->parentScope->fullname + ".html";
        outFile.addStringFormat("<strong>Namespace: </strong><a href=\"%s\">%s</a><br/>\n", parentRef.c_str(), scope->parentScope->fullname.c_str());
        CONCAT_FIXED_STR(outFile, "</div>\n");
    }

    CONCAT_FIXED_STR(outFile, "<div class=\"members\">\n");
    if (!scope->publicNamespace.empty())
    {
        DocHtmlHelper::startSection(outFile, "Namespaces");
        DocHtmlHelper::table(outFile, scope, scope->publicNamespace);
        DocHtmlHelper::endSection(outFile);
    }

    if (!scope->publicFunc.empty())
    {
        DocHtmlHelper::startSection(outFile, "Functions");
        DocHtmlHelper::table(outFile, scope, scope->publicFunc);
        DocHtmlHelper::endSection(outFile);
    }

    if (!scope->publicGenericFunc.empty())
    {
        DocHtmlHelper::startSection(outFile, "Generic Functions");
        DocHtmlHelper::table(outFile, scope, scope->publicGenericFunc);
        DocHtmlHelper::endSection(outFile);
    }

    if (!scope->publicStruct.empty())
    {
        DocHtmlHelper::startSection(outFile, "Structures");
        DocHtmlHelper::table(outFile, scope, scope->publicStruct);
        DocHtmlHelper::endSection(outFile);
    }

    if (!scope->publicConst.empty())
    {
        DocHtmlHelper::startSection(outFile, "Constants");
        DocHtmlHelper::table(outFile, scope, scope->publicConst);
        DocHtmlHelper::endSection(outFile);
    }

    if (!scope->publicTypeAlias.empty())
    {
        DocHtmlHelper::startSection(outFile, "Types");
        DocHtmlHelper::table(outFile, scope, scope->publicTypeAlias);
        DocHtmlHelper::endSection(outFile);
    }

    CONCAT_FIXED_STR(outFile, "</div>\n");

    DocHtmlHelper::htmlEnd(outFile);
    outFile.flush(true);

    return JobResult::ReleaseJob;
}
