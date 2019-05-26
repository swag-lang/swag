#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "Scope.h"

bool SemanticJob::resolveIdentifierRef(SemanticContext* context)
{
    auto           sourceFile = context->sourceFile;
    auto           node       = static_cast<AstIdentifierRef*>(context->node);
    AstIdentifier* identifier = static_cast<AstIdentifier*>(node->childs.back());

    // Be sure the found symbol is of the correct kind
    auto symName = identifier->resolvedSymbolName;
    auto symOver = identifier->resolvedSymbolOverload;
    if (symName->kind != node->symbolKind)
    {
        switch (node->symbolKind)
        {
        case SymbolKind::TypeDecl:
        {
            Diagnostic diag{sourceFile, identifier->token, format("symbol '%s' is not a type", identifier->name.c_str())};
            Diagnostic note{symOver->sourceFile, symOver->startLocation, symOver->endLocation, format("this is the definition of '%s'", symName->name.c_str()), DiagnosticLevel::Note};
            return sourceFile->report(diag, &note);
        }
        default:
        {
            Diagnostic diag{sourceFile, node->token, format("invalid usage of symbol '%s'", identifier->name.c_str())};
            Diagnostic note{symOver->sourceFile, symOver->startLocation, symOver->endLocation, format("this is the definition of '%s'", symName->name.c_str()), DiagnosticLevel::Note};
            return sourceFile->report(diag, &note);
        }
        }
    }

    node->typeInfo = identifier->typeInfo;
    return true;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto node       = static_cast<AstIdentifier*>(context->node);
    auto parent     = static_cast<AstIdentifierRef*>(node->parent);
    auto sourceFile = context->sourceFile;

    // If node->matchScope is defined, no need to rescan the scope hiearchy, as it is already
    // the scope where we have found the symbol in the first place (if it is defined, it means
    // that we come from a symbol wakeup)
    auto scope = node->matchScope;
    if (!scope)
        scope = parent->startScope;
    if (!scope)
        scope = node->scope;

    while (scope)
    {
        auto symTable = scope->symTable;
        {
            scoped_lock lk(symTable->mutex);
            auto        name = symTable->findNoLock(node->name);
            if (name)
            {
                node->matchScope = scope;
                if (!name->overloads.empty())
                {
                    node->resolvedSymbolName     = name;
                    node->resolvedSymbolOverload = name->overloads[0];
                    node->typeInfo               = node->resolvedSymbolOverload->typeInfo;
                    context->result              = SemanticResult::Done;
                    return true;
                }

                // Need to wait for the symbol to be resolved
                name->dependentJobs.push_back(context->job);
                context->result = SemanticResult::Pending;
                return true;
            }
        }

        scope = scope->parentScope;
    }

    return sourceFile->report({sourceFile, node->token, format("unknown identifier '%s'", node->name.c_str())});
}
