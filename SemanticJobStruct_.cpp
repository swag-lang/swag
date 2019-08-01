#include "pch.h"
#include "SemanticJob.h"
#include "Global.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "Scope.h"
#include "Ast.h"
#include "AstNode.h"
#include "ThreadManager.h"

bool SemanticJob::resolveImpl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstImpl>(context->node, AstNodeKind::Impl);

    // Be sure this is a struct
    auto typeInfo = node->identifier->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Struct)
    {
        Diagnostic diag{sourceFile, node->identifier, format("'%s' is %s and should be a struct", node->identifier->name.c_str(), TypeInfo::getKindName(typeInfo))};
        Diagnostic note{node->identifier->resolvedSymbolOverload->sourceFile, node->identifier->resolvedSymbolOverload->node->token.startLocation, node->identifier->resolvedSymbolOverload->node->token.endLocation, format("this is the definition of '%s'", node->identifier->name.c_str()), DiagnosticLevel::Note};
        return sourceFile->report(diag, &note);
    }

    return true;
}

bool SemanticJob::resolveStruct(SemanticContext* context)
{
    auto node       = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    auto job        = context->job;

    if (job->resolvedStage == 0)
    {
        typeInfo->name = format("struct %s", node->name.c_str());

        uint32_t storageOffset = 0;
        uint32_t storageIndex  = 0;
        uint32_t structFlags   = 0;

        for (auto child : node->childs)
        {
            if (child->kind != AstNodeKind::VarDecl)
                continue;

            auto varDecl = static_cast<AstVarDecl*>(child);

			// Var is a struct
            if (varDecl->typeInfo->kind == TypeInfoKind::Struct)
            {
				if (varDecl->typeInfo->flags & TYPEINFO_STRUCT_HAS_CONSTRUCTOR)
					structFlags |= TYPEINFO_STRUCT_HAS_CONSTRUCTOR;
            }

            // Var has an initialization
            else if (varDecl->astAssignment)
            {
                SWAG_VERIFY(varDecl->astAssignment->flags & AST_CONST_EXPR, sourceFile->report({sourceFile, varDecl->astAssignment, "cannot evaluate initialization expression at compile time"}));
                if (varDecl->astAssignment->computedValue.reg.u64)
                    structFlags |= TYPEINFO_STRUCT_HAS_CONSTRUCTOR;
            }

            typeInfo->childs.push_back(child->typeInfo);
            typeInfo->sizeOf += child->typeInfo->sizeOf;
            typeInfo->flags |= structFlags;

            child->resolvedSymbolOverload->storageOffset = storageOffset;
            child->resolvedSymbolOverload->storageIndex  = storageIndex;
            storageOffset += child->typeInfo->sizeOf;
            storageIndex++;
        }

        node->typeInfo = g_TypeMgr.registerType(typeInfo);

        // Register symbol with its type
        SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Struct));
    }

    job->resolvedStage = 1;

    // Search init function
    auto symboleName = typeInfo->scope->symTable->find("opInit");
    if (symboleName)
    {
        if (symboleName->cptOverloads)
        {
            symboleName->dependentJobs.push_back(context->job);
            g_ThreadMgr.addPendingJob(context->job);
            context->result = SemanticResult::Pending;
            return true;
        }

        node->ownerScope->symTable->mutex.lock();
        auto typeInfoFunc = g_Pool_typeInfoFuncAttr.alloc();
        auto param        = g_Pool_typeInfoFuncAttrParam.alloc();
        param->typeInfo   = typeInfo;
        typeInfoFunc->parameters.push_back(param);

        auto overload = symboleName->findOverload(typeInfoFunc);
        if (overload)
            typeInfo->defaultInit = overload->node;
        node->ownerScope->symTable->mutex.unlock();

        typeInfoFunc->release();
    }

    return true;
}
