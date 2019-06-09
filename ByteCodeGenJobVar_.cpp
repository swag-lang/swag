#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Module.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"

bool ByteCodeGenJob::emitVarDecl(ByteCodeGenContext* context)
{
    auto node       = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);
    auto sourceFile = context->sourceFile;
    auto resolved   = node->resolvedSymbolOverload;

    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        if (node->astAssignment)
        {
            auto r0 = sourceFile->module->reserveRegisterRC();
            auto inst   = emitInstruction(context, ByteCodeOp::RCxRefFromStack, r0);
            inst->b.s32 = resolved->stackOffset;
            emitAffectEqual(context, r0, node->astAssignment->resultRegisterRC);
			sourceFile->module->freeRegisterRC(r0);
        }
    }

    return true;
}
