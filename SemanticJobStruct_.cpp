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

bool SemanticJob::resolveStruct(SemanticContext* context)
{
    auto node       = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);

    typeInfo->name = format("struct %s", node->name.c_str());

    uint32_t storageOffset = 0;
    uint32_t storageIndex  = 0;

    for (auto child : node->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);

        // Var has an initialization
        if (varDecl->astAssignment)
        {
            SWAG_VERIFY(varDecl->astAssignment->flags & AST_CONST_EXPR, sourceFile->report({sourceFile, varDecl->astAssignment, "cannot evaluate initialization expression at compile time"}));
            if (varDecl->astAssignment->computedValue.reg.u64)
                node->flags |= AST_STRUCT_HAS_CONSTRUCTOR;
        }

        typeInfo->childs.push_back(child->typeInfo);
        typeInfo->sizeOf += child->typeInfo->sizeOf;

        child->resolvedSymbolOverload->storageOffset = storageOffset;
        child->resolvedSymbolOverload->storageIndex  = storageIndex;
        storageOffset += child->typeInfo->sizeOf;
        storageIndex++;
    }

    node->typeInfo = g_TypeMgr.registerType(typeInfo);

    // Register symbol with its type
    SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Struct));

    return true;
}
