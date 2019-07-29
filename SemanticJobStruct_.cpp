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
    auto node       = context->node;
    auto sourceFile = context->sourceFile;

    auto typeInfo  = g_Pool_typeInfoStruct.alloc();
    typeInfo->name = format("struct %s", node->name.c_str());

    for (auto child : node->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        if (varDecl->astAssignment)
        {
            SWAG_VERIFY(varDecl->astAssignment->flags & AST_CONST_EXPR, sourceFile->report({sourceFile, varDecl->astAssignment, "cannot evaluate initialization expression at compile time"}));
        }

        typeInfo->sizeOf += child->typeInfo->sizeOf;
    }

	node->typeInfo = g_TypeMgr.registerType(typeInfo);
    return true;
}
