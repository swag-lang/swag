#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "TypeInfo.h"

bool ByteCodeGenJob::emitVarDecl(ByteCodeGenContext* context)
{
    auto node     = static_cast<AstVarDecl*>(context->node);
    auto resolved = node->resolvedSymbolOverload;
    auto typeInfo = resolved->typeInfo;

    // If structure has parameters for construction, need to save registers that store values before a call
    vector<uint32_t> regToSave;
    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
        {
            auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
            auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);
            for (auto child : identifier->callParameters->childs)
            {
                for (int r = 0; r < child->resultRegisterRC.size(); r++)
                    regToSave.push_back(child->resultRegisterRC[r]);
            }
        }
    }

    // User initialization
    if (node->assignment)
    {
        RegisterList r0   = reserveRegisterRC(context);
        auto         inst = emitInstruction(context, ByteCodeOp::RARefFromStack, r0);
        inst->b.s32       = resolved->storageOffset;
        emitAffectEqual(context, r0, node->assignment->resultRegisterRC, node->typeInfo, node->assignment->typeInfo);
        freeRegisterRC(context, r0);
        freeRegisterRC(context, node->assignment->resultRegisterRC);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        if (typeArray->pointedType->kind == TypeInfoKind::Struct)
        {
            if (typeArray->pointedType->flags & TYPEINFO_STRUCT_HAS_CONSTRUCTOR)
            {
                if (!(node->flags & AST_DISABLED_DEFAULT_INIT) && !(node->flags & AST_HAS_STRUCT_PARAMETERS))
                {
                    // Need to loop on every element of the array in order to initialize them
                    RegisterList r0;
                    reserveRegisterRC(context, r0, 3);
                    emitInstruction(context, ByteCodeOp::CopyRAVB32, r0[0])->b.u32 = typeArray->count;
                    emitInstruction(context, ByteCodeOp::ClearRA, r0[1]);
                    auto seekJump = context->bc->numInstructions;
                    regToSave.push_back(r0[0]);
                    regToSave.push_back(r0[1]);

                    emitStructInit(context, CastTypeInfo<TypeInfoStruct>(typeArray->pointedType, TypeInfoKind::Struct), r0[1], regToSave);
                    emitStructParameters(context, r0[1]);

                    emitInstruction(context, ByteCodeOp::DecRA, r0[0]);
                    emitInstruction(context, ByteCodeOp::IncRAVB, r0[1])->b.u32 = typeArray->pointedType->sizeOf;
                    emitInstruction(context, ByteCodeOp::IsNullU32, r0[0], r0[2]);
                    emitInstruction(context, ByteCodeOp::JumpNotTrue, r0[2])->b.s32 = seekJump - context->bc->numInstructions - 1;

                    freeRegisterRC(context, r0);
                }

                return true;
            }
        }
    }

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (!(node->flags & AST_DISABLED_DEFAULT_INIT) && !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
            SWAG_ASSERT(typeStruct->opInitFct);
            emitStructInit(context, typeStruct, UINT32_MAX, regToSave);
        }

        emitStructParameters(context, UINT32_MAX);
        return true;
    }

    if (!(node->flags & AST_DISABLED_DEFAULT_INIT))
    {
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::ClearRefFromStack8)->a.u32 = resolved->storageOffset;
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::ClearRefFromStack16)->a.u32 = resolved->storageOffset;
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::ClearRefFromStack32)->a.u32 = resolved->storageOffset;
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::ClearRefFromStack64)->a.u32 = resolved->storageOffset;
            break;
        default:
        {
            auto inst   = emitInstruction(context, ByteCodeOp::ClearRefFromStackX);
            inst->a.u32 = resolved->storageOffset;
            inst->b.u32 = typeInfo->sizeOf;
            break;
        }
        }
    }

    return true;
}
