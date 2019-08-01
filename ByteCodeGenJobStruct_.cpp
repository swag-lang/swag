#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "TypeInfo.h"
#include "SourceFile.h"


/*
auto symboleName = typeInfo -> scope -> symTable -> find("opInit");
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
}*/

bool ByteCodeGenJob::generateStructInit(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto resolved   = node->resolvedSymbolOverload;
    auto typeInfo   = CastTypeInfo<TypeInfoStruct>(resolved->typeInfo, TypeInfoKind::Struct);
    auto structNode = CastAst<AstStruct>(typeInfo->structNode, AstNodeKind::StructDecl);

    structNode->lock();
    if (!structNode->opInit)
    {
        structNode->opInit             = g_Pool_byteCode.alloc();
        structNode->opInit->sourceFile = context->sourceFile;
        structNode->opInit->name       = "opInit";

        ByteCodeGenContext cxt{*context};
        cxt.bc = structNode->opInit;

        for (auto child : structNode->childs)
        {
            auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
            if (varDecl->astAssignment)
            {
                if (varDecl->typeInfo->kind == TypeInfoKind::Native)
                {
                    emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
                    emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = varDecl->resolvedSymbolOverload->storageOffset;
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB32, 1)->b.u32   = varDecl->astAssignment->computedValue.reg.u32;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp32, 0, 1);
                }
                else
                {
                    return internalError(context, "generateStructInit, invalid type");
                }
            }
        }

        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
    }

    structNode->unlock();
    return true;
}

bool ByteCodeGenJob::emitStructInit(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto resolved   = node->resolvedSymbolOverload;
    auto typeInfo   = CastTypeInfo<TypeInfoStruct>(resolved->typeInfo, TypeInfoKind::Struct);
    auto structNode = CastAst<AstStruct>(typeInfo->structNode, AstNodeKind::StructDecl);

    // Just clear the content of the structure
    if (!(typeInfo->flags & TYPEINFO_STRUCT_HAS_CONSTRUCTOR))
    {
        auto inst   = emitInstruction(context, ByteCodeOp::ClearRefFromStackX);
        inst->a.u32 = resolved->storageOffset;
        inst->b.u32 = typeInfo->sizeOf;
        return true;
    }

    // Be sure referenced function has bytecode
    if (!generateStructInit(context))
        return false;

    // Push self
    RegisterList r0;
    r0          = reserveRegisterRC(context);
    auto inst   = emitInstruction(context, ByteCodeOp::RARefFromStack, r0);
    inst->b.s32 = resolved->storageOffset;

    // Then call
    emitInstruction(context, ByteCodeOp::PushRAParam, r0, 0);
    inst            = emitInstruction(context, ByteCodeOp::LocalCall, 0);
    inst->a.pointer = (uint8_t*) structNode->opInit;
    inst->b.u64     = 1;
    inst->c.pointer = (uint8_t*) typeInfo;
    emitInstruction(context, ByteCodeOp::IncSP, 8);

    return true;
}
