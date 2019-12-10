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
            auto scopeJob    = g_Pool_docScopeJob.alloc();
            scopeJob->module = module;
            scopeJob->scope  = child;
            g_ThreadMgr.addJob(scopeJob);
            break;
        }
    }

    if (scope->kind == ScopeKind::Namespace)
    {
        OutputFile outFile;
        scope->fullname  = scope->parentScope ? Scope::makeFullName(scope->parentScope->fullname, scope->name) : scope->name;
        outFile.fileName = module->documentPath.string() + "/" + scope->fullname + ".html";
        DocHtmlHelper::htmlStart(outFile);
        DocHtmlHelper::title(outFile, format("%s namespace", scope->fullname.c_str()));
        DocHtmlHelper::summary(outFile, "Summary");

        //CONCAT_FIXED_STR(outFile, "<div class=\"members\">\n");
        if (!scope->publicFunc.empty())
        {
            DocHtmlHelper::startSection(outFile, "Functions");
            DocHtmlHelper::startTable(outFile);

            for (auto func : scope->publicFunc)
            {
                DocHtmlHelper::startTableRow(outFile);
                auto refName = scope->fullname + "." + func->name + ".html";
                DocHtmlHelper::tableNameCell(outFile, refName, func->name);
                DocHtmlHelper::tableDescCell(outFile, "description");
                DocHtmlHelper::endTableRow(outFile);
            }

            DocHtmlHelper::endTable(outFile);
            DocHtmlHelper::endSection(outFile);
        }

        //CONCAT_FIXED_STR(outFile, "</div>\n");

        DocHtmlHelper::htmlEnd(outFile);
        outFile.flush(true);
    }

    return JobResult::ReleaseJob;
}
