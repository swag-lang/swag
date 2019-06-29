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
    auto node     = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);
    auto resolved = node->resolvedSymbolOverload;

    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        if (node->astAssignment)
        {
            RegisterList r0;
            r0          = reserveRegisterRC(context);
            auto inst   = emitInstruction(context, ByteCodeOp::RCxRefFromStack, r0);
            inst->b.s32 = resolved->storageOffset;

            emitAffectEqual(context, r0, node->astAssignment->resultRegisterRC);
            freeRegisterRC(context, r0);
            freeRegisterRC(context, node->astAssignment->resultRegisterRC);
        }
    }

    return true;
}
