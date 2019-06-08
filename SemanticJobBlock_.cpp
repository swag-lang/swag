#include "pch.h"
#include "Global.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "SemanticJob.h"
#include "TypeInfo.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Scope.h"
#include "ByteCodeGenJob.h"

bool SemanticJob::resolveIf(SemanticContext* context)
{
    auto node = CastAst<AstIf>(context->node, AstNodeKind::If);
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoBool, node->boolExpression));
    node->byteCodeFct                      = &ByteCodeGenJob::emitIf;
    node->boolExpression->byteCodeAfterFct = &ByteCodeGenJob::emitIfAfterExpr;
    node->ifBlock->byteCodeAfterFct        = &ByteCodeGenJob::emitIfAfterIf;
    return true;
}
