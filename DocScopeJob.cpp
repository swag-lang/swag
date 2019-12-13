#include "pch.h"
#include "DocScopeJob.h"
#include "DocNodeJob.h"
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
        case ScopeKind::Enum:
            auto scopeJob    = g_Pool_docScopeJob.alloc();
            scopeJob->module = module;
            scopeJob->scope  = child;
            g_ThreadMgr.addJob(scopeJob);
            break;
        }
    }

    if (scope->name.empty())
        return JobResult::ReleaseJob;
    if (!(scope->flags & SCOPE_FLAG_HAS_EXPORTS))
        return JobResult::ReleaseJob;

    OutputFile outFile;
    scope->fullname  = scope->parentScope ? Scope::makeFullName(scope->parentScope->fullname, scope->name) : scope->name;
    outFile.fileName = module->documentPath.string() + "/" + scope->fullname + ".html";
    DocHtmlHelper::htmlStart(outFile);
    DocHtmlHelper::title(outFile, format("%s %s", scope->name.c_str(), Scope::getNakedKindName(scope->kind)));
    DocHtmlHelper::summary(outFile, scope->owner->docSummary);
    DocHtmlHelper::origin(outFile, scope->parentScope);

    if (scope->kind == ScopeKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(scope->owner->typeInfo, TypeInfoKind::Struct);
        DocHtmlHelper::sectionTitle1(outFile, "Members");
        DocHtmlHelper::table(outFile, scope, typeStruct->childs);
    }

    if (!scope->publicNamespace.empty())
    {
        set<AstNode*> namespaces;
        for (auto ns : scope->publicNamespace)
        {
            auto typeNamespace = CastTypeInfo<TypeInfoNamespace>(ns->typeInfo, TypeInfoKind::Namespace);
            if (!(typeNamespace->scope->flags & SCOPE_FLAG_HAS_EXPORTS))
                continue;
            namespaces.insert(ns);
        }

        if (!namespaces.empty())
        {
            DocHtmlHelper::sectionTitle1(outFile, "Namespaces");
            DocHtmlHelper::table(outFile, scope, namespaces);
        }
    }

    if (!scope->publicFunc.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Functions");
        DocHtmlHelper::table(outFile, scope, scope->publicFunc);

        for (auto child : scope->publicFunc)
        {
            auto nodeJob    = g_Pool_docNodeJob.alloc();
            nodeJob->module = module;
            nodeJob->node   = child;
            g_ThreadMgr.addJob(nodeJob);
        }
    }

    if (!scope->publicGenericFunc.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Generic Functions");
        DocHtmlHelper::table(outFile, scope, scope->publicGenericFunc);

        for (auto child : scope->publicGenericFunc)
        {
            auto nodeJob    = g_Pool_docNodeJob.alloc();
            nodeJob->module = module;
            nodeJob->node   = child;
            g_ThreadMgr.addJob(nodeJob);
        }
    }

    if (!scope->publicStruct.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Structures");
        DocHtmlHelper::table(outFile, scope, scope->publicStruct);
    }

    if (!scope->publicEnum.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Enumerations");
        DocHtmlHelper::table(outFile, scope, scope->publicEnum);

        for (auto child : scope->publicEnum)
        {
            auto nodeJob    = g_Pool_docNodeJob.alloc();
            nodeJob->module = module;
            nodeJob->node   = child;
            g_ThreadMgr.addJob(nodeJob);
        }
    }

    if (!scope->publicConst.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Constants");
        DocHtmlHelper::table(outFile, scope, scope->publicConst);
    }

    if (!scope->publicTypeAlias.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Types");
        DocHtmlHelper::table(outFile, scope, scope->publicTypeAlias);
    }

    DocHtmlHelper::htmlEnd(outFile);
    outFile.flush(true);

    return JobResult::ReleaseJob;
}
