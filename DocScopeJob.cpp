#include "pch.h"
#include "DocScopeJob.h"
#include "DocNodeJob.h"
#include "ThreadManager.h"
#include "OutputFile.h"
#include "Module.h"
#include "AstNode.h"
#include "DocHtmlHelper.h"
#include "DocContent.h"

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
    scope->fullname = scope->parentScope ? Scope::makeFullName(scope->parentScope->fullname, scope->name) : scope->name;
    outFile.path    = module->documentPath.string() + "/" + scope->fullname + ".html";
    DocHtmlHelper::htmlStart(outFile);
    DocHtmlHelper::title(outFile, format("%s.%s %s", scope->parentScope->name.c_str(), scope->name.c_str(), Scope::getNakedKindName(scope->kind)));
    DocHtmlHelper::summary(outFile, scope->owner->docContent ? scope->owner->docContent->docSummary : Utf8(""));
    DocHtmlHelper::origin(outFile, scope->parentScope);

    // Struct members
    /////////////////////////////////
    if (scope->kind == ScopeKind::Struct)
    {
        auto                   typeStruct = CastTypeInfo<TypeInfoStruct>(scope->owner->typeInfo, TypeInfoKind::Struct);
        vector<TypeInfoParam*> members;
        for (auto param : typeStruct->childs)
        {
            if (param->node->attributeFlags & ATTRIBUTE_INTERNAL)
                continue;
            members.push_back(param);
        }

        if (!members.empty())
        {
            DocHtmlHelper::sectionTitle1(outFile, "Members");
            DocHtmlHelper::table(outFile, scope, members);
        }
    }

    // Namespaces
    /////////////////////////////////
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

    map<Utf8, DocNodeJob*> nameToJob;
    set<AstNode*>          properties;
    set<AstNode*>          functions;
    set<AstNode*>          operators;

    // Functions
    /////////////////////////////////
    if (!scope->publicFunc.empty())
    {
        for (auto node : scope->publicFunc)
        {
            auto it = nameToJob.find(node->fullnameDot);
            if (it == nameToJob.end())
            {
                if (node->flags & AST_IS_SPECIAL_FUNC)
                    operators.insert(node);
                else if (node->attributeFlags & ATTRIBUTE_PROPERTY)
                    properties.insert(node);
                else
                    functions.insert(node);
                auto nodeJob    = g_Pool_docNodeJob.alloc();
                nodeJob->module = module;
                nodeJob->nodes.push_back(node);
                nameToJob[node->fullnameDot] = nodeJob;
            }
            else
                it->second->nodes.push_back(node);
        }

        for (auto node : scope->publicGenericFunc)
        {
            auto it = nameToJob.find(node->fullnameDot);
            if (it == nameToJob.end())
            {
                if (node->flags & AST_IS_SPECIAL_FUNC)
                    operators.insert(node);
                else
                    functions.insert(node);
                auto nodeJob    = g_Pool_docNodeJob.alloc();
                nodeJob->module = module;
                nodeJob->nodes.push_back(node);
                nameToJob[node->fullnameDot] = nodeJob;
            }
            else
                it->second->nodes.push_back(node);
        }
    }

    if (!properties.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Properties");
        DocHtmlHelper::table(outFile, scope, properties);
    }

    if (!functions.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Functions");
        DocHtmlHelper::table(outFile, scope, functions);
    }

    if (!operators.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Special Functions");
        DocHtmlHelper::table(outFile, scope, operators);
    }

    // Run one job per function
    for (auto it : nameToJob)
        g_ThreadMgr.addJob(it.second);

    // Structs
    /////////////////////////////////
    if (!scope->publicStruct.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Structs");
        DocHtmlHelper::table(outFile, scope, scope->publicStruct);
    }

    // Enums
    /////////////////////////////////
    if (!scope->publicEnum.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Enumerations");
        DocHtmlHelper::table(outFile, scope, scope->publicEnum);

        for (auto child : scope->publicEnum)
        {
            auto nodeJob    = g_Pool_docNodeJob.alloc();
            nodeJob->module = module;
            nodeJob->nodes.push_back(child);
            g_ThreadMgr.addJob(nodeJob);
        }
    }

    // Constants
    /////////////////////////////////
    if (!scope->publicConst.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Constants");
        DocHtmlHelper::table(outFile, scope, scope->publicConst);
    }

    // Types
    /////////////////////////////////
    if (!scope->publicTypeAlias.empty())
    {
        DocHtmlHelper::sectionTitle1(outFile, "Types");
        DocHtmlHelper::table(outFile, scope, scope->publicTypeAlias);
    }

    DocHtmlHelper::htmlEnd(outFile);
    outFile.flush(true);

    return JobResult::ReleaseJob;
}
