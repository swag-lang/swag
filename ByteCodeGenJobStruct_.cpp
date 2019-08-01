#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "TypeInfo.h"

bool ByteCodeGenJob::emitStructInit(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto resolved = node->resolvedSymbolOverload;
    auto typeInfo = CastTypeInfo<TypeInfoStruct>(resolved->typeInfo, TypeInfoKind::Struct);

    // Just clear the content of the structure
    if (!(typeInfo->flags & TYPEINFO_STRUCT_HAS_CONSTRUCTOR))
    {
        auto inst   = emitInstruction(context, ByteCodeOp::ClearRefFromStackX);
        inst->a.u32 = resolved->storageOffset;
        inst->b.u32 = typeInfo->sizeOf;
        return true;
    }

    // Be sure referenced function has bytecode
	if (typeInfo->defaultInit)
	{
		askForByteCode(context, CastAst<AstFuncDecl>(typeInfo->defaultInit, AstNodeKind::FuncDecl));
		if (context->result == ByteCodeResult::Pending)
			return true;

		// Push self
		RegisterList r0;
		r0 = reserveRegisterRC(context);
		auto inst = emitInstruction(context, ByteCodeOp::RARefFromStack, r0);
		inst->b.s32 = resolved->storageOffset;

		// Then call
		emitInstruction(context, ByteCodeOp::PushRAParam, r0, 0);
		inst = emitInstruction(context, ByteCodeOp::LocalCall, 0);
		inst->a.pointer = (uint8_t*)typeInfo->defaultInit->bc;
		inst->b.u64 = 1;
		inst->c.pointer = (uint8_t*)typeInfo->defaultInit->typeInfo;
		emitInstruction(context, ByteCodeOp::IncSP, 8);
	}

    return true;
}
