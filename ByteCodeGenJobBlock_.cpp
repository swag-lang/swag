#include "pch.h"
#include "ThreadManager.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "SourceFile.h"
#include "ByteCode.h"
#include "Ast.h"
#include "SymTable.h"

bool ByteCodeGenJob::emitIf(ByteCodeGenContext* context)
{
    auto node = CastAst<AstIf>(context->node, AstNodeKind::If);
    return true;
}
