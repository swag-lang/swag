#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/ModuleManager.h"

void ByteCodeGen::emitOpCallUser(const ByteCodeGenContext* context, const TypeInfoStruct* typeStruct, EmitOpUserKind kind, bool pushParam, uint32_t offset, uint32_t numParams)
{
    switch (kind)
    {
        case EmitOpUserKind::Init:
            emitOpCallUser(context, typeStruct->opUserInitFct, typeStruct->opInit, pushParam, offset, numParams);
            break;
        case EmitOpUserKind::Drop:
            emitOpCallUser(context, typeStruct->opUserDropFct, typeStruct->opDrop, pushParam, offset, numParams);
            break;
        case EmitOpUserKind::PostCopy:
            emitOpCallUser(context, typeStruct->opUserPostCopyFct, typeStruct->opPostCopy, pushParam, offset, numParams);
            break;
        case EmitOpUserKind::PostMove:
            emitOpCallUser(context, typeStruct->opUserPostMoveFct, typeStruct->opPostMove, pushParam, offset, numParams);
            break;
    }
}

void ByteCodeGen::emitOpCallUser(const ByteCodeGenContext* context, AstFuncDecl* funcDecl, ByteCode* bc, bool pushParam, uint32_t offset, uint32_t numParams)
{
    if (!funcDecl && !bc)
        return;

    if (funcDecl && funcDecl->resolvedSymbolName())
        funcDecl->resolvedSymbolName()->flags.add(SYMBOL_USED);
    if (bc)
        bc->isUsed = true;

    if (funcDecl)
        askForByteCode(context->baseJob, funcDecl, 0, context->bc);
    else if (bc && bc->node)
        askForByteCode(context->baseJob, bc->node, 0, context->bc);

    if (pushParam)
    {
        SWAG_ASSERT(numParams == 1);
        auto inst               = EMIT_INST0(context, ByteCodeOp::GetParam64);
        inst->b.mergeU64U32.low = 24;
        if (offset)
        {
            inst = EMIT_INST0(context, ByteCodeOp::IncPointer64);
            SWAG_ASSERT(offset != 0xFFFFFFFF);
            inst->b.u64 = offset;
            inst->addFlag(BCI_IMM_B);
        }

        EMIT_INST1(context, ByteCodeOp::PushRAParam, 0);
    }

    if (funcDecl && !bc && funcDecl->hasAttribute(ATTRIBUTE_FOREIGN))
    {
        const auto inst = EMIT_INST0(context, ByteCodeOp::ForeignCall);
        inst->a.pointer = reinterpret_cast<uint8_t*>(funcDecl);
        inst->b.pointer = reinterpret_cast<uint8_t*>(funcDecl->typeInfo);
        context->bc->hasForeignFunctionCallsModules.insert(ModuleManager::getForeignModuleName(funcDecl));
        SWAG_ASSERT(inst->a.pointer);
    }
    else
    {
        const auto inst = EMIT_INST0(context, ByteCodeOp::LocalCall);
        SWAG_ASSERT(bc || (funcDecl && funcDecl->hasExtByteCode() && funcDecl->extByteCode()->bc));
        const auto bcReal = bc ? bc : funcDecl->extByteCode()->bc;
        bcReal->isUsed    = true;
        inst->a.pointer   = reinterpret_cast<uint8_t*>(bcReal);
        SWAG_ASSERT(inst->a.pointer);
        SWAG_ASSERT(numParams <= 2);
        inst->b.pointer = numParams == 1 ? reinterpret_cast<uint8_t*>(g_TypeMgr->typeInfoOpCall) : reinterpret_cast<uint8_t*>(g_TypeMgr->typeInfoOpCall2);
    }

    EMIT_INST1(context, ByteCodeOp::IncSPPostCall, numParams * 8);
}

void ByteCodeGen::generateStructAlloc(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->hasFlag(TYPEINFO_SPEC_OP_GENERATED))
        return;

    const auto structNode = castAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    // Type of those functions
    const auto typeInfoFunc               = castTypeInfo<TypeInfoFuncAttr>(g_TypeMgr->typeInfoOpCall->clone());
    typeInfoFunc->parameters[0]->typeInfo = g_TypeMgr->makePointerTo(typeInfoStruct, typeInfoFunc->parameters[0]->typeInfo->flags);
    typeInfoFunc->forceComputeName();

    for (int i = 0; i < static_cast<int>(EmitOpUserKind::Max); i++)
    {
        const auto  kind  = static_cast<EmitOpUserKind>(i);
        ByteCode**  resOp = nullptr;
        Utf8        addName;
        SymbolName* symbol = nullptr;

        bool needDrop     = false;
        bool needPostCopy = false;
        bool needPostMove = false;

        switch (kind)
        {
            case EmitOpUserKind::Init:
            {
                if (typeInfoStruct->opInit)
                    continue;

                // Need to be sure that function has been solved
                {
                    ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
                    symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opInit, g_LangSpec->name_opInitCrc);
                    if (symbol && symbol->cptOverloads)
                    {
                        symbol->addDependentJob(context->baseJob);
                        context->baseJob->setPending(JobWaitKind::EmitInit, symbol, structNode, nullptr);
                        return;
                    }
                }

                // For generic function, symbol is not registered in the scope of the instantiated struct, but in the
                // generic struct
                if (!symbol && typeInfoStruct->opUserInitFct)
                {
                    ScopedLock lockTable(typeInfoStruct->opUserInitFct->ownerScope->symTable.mutex);
                    symbol = typeInfoStruct->opUserInitFct->ownerScope->symTable.findNoLock(g_LangSpec->name_opInit, g_LangSpec->name_opInitCrc);
                    if (symbol && symbol->cptOverloads)
                    {
                        symbol->addDependentJob(context->baseJob);
                        context->baseJob->setPending(JobWaitKind::EmitInit, symbol, structNode, nullptr);
                        return;
                    }
                }

                if (typeInfoStruct->opUserInitFct && typeInfoStruct->opUserInitFct->hasAttribute(ATTRIBUTE_FOREIGN))
                    continue;
                resOp   = &typeInfoStruct->opInit;
                addName = ".opInit";
                break;
            }
            case EmitOpUserKind::Drop:
            {
                if (typeInfoStruct->opDrop)
                    continue;

                // Need to be sure that function has been solved
                {
                    ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
                    symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opDrop, g_LangSpec->name_opDropCrc);
                    if (symbol && symbol->cptOverloads)
                    {
                        symbol->addDependentJob(context->baseJob);
                        context->baseJob->setPending(JobWaitKind::EmitDrop, symbol, structNode, nullptr);
                        return;
                    }
                }

                // For generic function, symbol is not registered in the scope of the instantiated struct, but in the
                // generic struct
                if (!symbol && typeInfoStruct->opUserDropFct)
                {
                    ScopedLock lockTable(typeInfoStruct->opUserDropFct->ownerScope->symTable.mutex);
                    symbol = typeInfoStruct->opUserDropFct->ownerScope->symTable.findNoLock(g_LangSpec->name_opDrop, g_LangSpec->name_opDropCrc);
                    if (symbol && symbol->cptOverloads)
                    {
                        symbol->addDependentJob(context->baseJob);
                        context->baseJob->setPending(JobWaitKind::EmitDrop, symbol, structNode, nullptr);
                        return;
                    }
                }

                if (typeInfoStruct->opUserDropFct && typeInfoStruct->opUserDropFct->hasAttribute(ATTRIBUTE_FOREIGN))
                    continue;
                if (typeInfoStruct->opUserDropFct)
                    needDrop = true;
                resOp   = &typeInfoStruct->opDrop;
                addName = ".opDropGenerated";
                break;
            }
            case EmitOpUserKind::PostCopy:
            {
                if (typeInfoStruct->opPostCopy)
                    continue;
                if (typeInfoStruct->hasFlag(TYPEINFO_STRUCT_NO_COPY))
                    continue;

                // Need to be sure that function has been solved
                {
                    ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
                    symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opPostCopy, g_LangSpec->name_opPostCopyCrc);
                    if (symbol && symbol->cptOverloads)
                    {
                        symbol->addDependentJob(context->baseJob);
                        context->baseJob->setPending(JobWaitKind::EmitPostCopy, symbol, structNode, nullptr);
                        return;
                    }
                }

                // For generic function, symbol is not registered in the scope of the instantiated struct, but in the
                // generic struct
                if (!symbol && typeInfoStruct->opUserPostCopyFct)
                {
                    ScopedLock lockTable(typeInfoStruct->opUserPostCopyFct->ownerScope->symTable.mutex);
                    symbol = typeInfoStruct->opUserPostCopyFct->ownerScope->symTable.findNoLock(g_LangSpec->name_opPostCopy, g_LangSpec->name_opPostCopyCrc);
                    if (symbol && symbol->cptOverloads)
                    {
                        symbol->addDependentJob(context->baseJob);
                        context->baseJob->setPending(JobWaitKind::EmitPostCopy, symbol, structNode, nullptr);
                        return;
                    }
                }

                if (typeInfoStruct->opUserPostCopyFct && typeInfoStruct->opUserPostCopyFct->hasAttribute(ATTRIBUTE_FOREIGN))
                    continue;
                if (typeInfoStruct->opUserPostCopyFct)
                    needPostCopy = true;
                resOp   = &typeInfoStruct->opPostCopy;
                addName = ".opPostCopyGenerated";
                break;
            }
            case EmitOpUserKind::PostMove:
            {
                if (typeInfoStruct->opPostMove)
                    continue;

                // Need to be sure that function has been solved
                {
                    ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
                    symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opPostMove, g_LangSpec->name_opPostMoveCrc);
                    if (symbol && symbol->cptOverloads)
                    {
                        symbol->addDependentJob(context->baseJob);
                        context->baseJob->setPending(JobWaitKind::EmitPostMove, symbol, structNode, nullptr);
                        return;
                    }
                }

                // For generic function, symbol is not registered in the scope of the instantiated struct, but in the
                // generic struct
                if (!symbol && typeInfoStruct->opUserPostMoveFct)
                {
                    ScopedLock lockTable(typeInfoStruct->opUserPostMoveFct->ownerScope->symTable.mutex);
                    symbol = typeInfoStruct->opUserPostMoveFct->ownerScope->symTable.findNoLock(g_LangSpec->name_opPostMove, g_LangSpec->name_opPostMoveCrc);
                    if (symbol && symbol->cptOverloads)
                    {
                        symbol->addDependentJob(context->baseJob);
                        context->baseJob->setPending(JobWaitKind::EmitPostMove, symbol, structNode, nullptr);
                        return;
                    }
                }

                if (typeInfoStruct->opUserPostMoveFct && typeInfoStruct->opUserPostMoveFct->hasAttribute(ATTRIBUTE_FOREIGN))
                    continue;
                if (typeInfoStruct->opUserPostMoveFct)
                    needPostMove = true;
                resOp   = &typeInfoStruct->opPostMove;
                addName = ".opPostMoveGenerated";
                break;
            }
            default:
                break;
        }

        // Be sure sub structs are generated too
        {
            SWAG_RACE_CONDITION_READ(typeInfoStruct->raceFields);
            for (const auto typeParam : typeInfoStruct->fields)
            {
                auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
                if (typeVar->isArray())
                    typeVar = castTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array)->pointedType;
                if (!typeVar->isStruct())
                    continue;
                const auto typeStructVar = castTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
                generateStructAlloc(context, typeStructVar);
                YIELD_VOID();
                if (typeStructVar->opDrop || typeStructVar->opUserDropFct)
                    needDrop = true;
                if (typeStructVar->opPostCopy || typeStructVar->opUserPostCopyFct)
                    needPostCopy = true;
                if (typeStructVar->opPostMove || typeStructVar->opUserPostMoveFct)
                    needPostMove = true;
            }
        }

        switch (kind)
        {
            case EmitOpUserKind::Drop:
                if (!needDrop)
                {
                    typeInfoStruct->addFlag(TYPEINFO_STRUCT_NO_DROP);
                    continue;
                }
                break;
            case EmitOpUserKind::PostCopy:
                if (!needPostCopy)
                {
                    typeInfoStruct->addFlag(TYPEINFO_STRUCT_NO_POST_COPY);
                    continue;
                }
                break;
            case EmitOpUserKind::PostMove:
                if (!needPostMove)
                {
                    typeInfoStruct->addFlag(TYPEINFO_STRUCT_NO_POST_MOVE);
                    continue;
                }
                break;
            default:
                break;
        }

        const auto sourceFile   = context->sourceFile;
        ByteCode*  opSpecial    = Allocator::alloc<ByteCode>();
        opSpecial->sourceFile   = sourceFile;
        opSpecial->typeInfoFunc = typeInfoFunc;
        opSpecial->name         = structNode->ownerScope->getFullName();
        opSpecial->name += ".";
        opSpecial->name += structNode->token.text;
        opSpecial->name += addName;
        opSpecial->maxReservedRegisterRC = 3;
        opSpecial->isCompilerGenerated   = true;
        *resOp                           = opSpecial;

        switch (kind)
        {
            case EmitOpUserKind::Init:
                // Export generated function if necessary
                if (!structNode->hasAstFlag(AST_FROM_GENERIC))
                {
                    const auto funcNode  = Ast::newNode<AstFuncDecl>(AstNodeKind::FuncDecl, nullptr, structNode);
                    funcNode->typeInfo   = opSpecial->typeInfoFunc;
                    funcNode->ownerScope = structNode->scope;
                    funcNode->token.text = g_LangSpec->name_opInitGenerated;
                    funcNode->allocateExtension(ExtensionKind::ByteCode);
                    funcNode->extByteCode()->bc = opSpecial;
                    opSpecial->node             = funcNode;
                }
                break;

            case EmitOpUserKind::Drop:
                // Export generated function if necessary
                if (structNode->hasAttribute(ATTRIBUTE_PUBLIC) && !structNode->hasAstFlag(AST_FROM_GENERIC))
                {
                    const auto funcNode  = Ast::newNode<AstFuncDecl>(AstNodeKind::FuncDecl, nullptr, structNode);
                    funcNode->typeInfo   = opSpecial->typeInfoFunc;
                    funcNode->ownerScope = structNode->scope;
                    funcNode->token.text = g_LangSpec->name_opDropGenerated;
                    funcNode->addAttribute(ATTRIBUTE_PUBLIC);
                    funcNode->allocateExtension(ExtensionKind::ByteCode);
                    funcNode->extByteCode()->bc = opSpecial;
                    opSpecial->node             = funcNode;
                }
                break;

            case EmitOpUserKind::PostCopy:
                // Export generated function if necessary
                if (structNode->hasAttribute(ATTRIBUTE_PUBLIC) && !structNode->hasAstFlag(AST_FROM_GENERIC))
                {
                    const auto funcNode  = Ast::newNode<AstFuncDecl>(AstNodeKind::FuncDecl, nullptr, structNode);
                    funcNode->typeInfo   = opSpecial->typeInfoFunc;
                    funcNode->ownerScope = structNode->scope;
                    funcNode->token.text = g_LangSpec->name_opPostCopyGenerated;
                    funcNode->addAttribute(ATTRIBUTE_PUBLIC);
                    funcNode->allocateExtension(ExtensionKind::ByteCode);
                    funcNode->extByteCode()->bc = opSpecial;
                    opSpecial->node             = funcNode;
                }
                break;

            case EmitOpUserKind::PostMove:
                // Export generated function if necessary
                if (structNode->hasAttribute(ATTRIBUTE_PUBLIC) && !structNode->hasAstFlag(AST_FROM_GENERIC))
                {
                    const auto funcNode  = Ast::newNode<AstFuncDecl>(AstNodeKind::FuncDecl, nullptr, structNode);
                    funcNode->typeInfo   = opSpecial->typeInfoFunc;
                    funcNode->ownerScope = structNode->scope;
                    funcNode->token.text = g_LangSpec->name_opPostMoveGenerated;
                    funcNode->addAttribute(ATTRIBUTE_PUBLIC);
                    funcNode->allocateExtension(ExtensionKind::ByteCode);
                    funcNode->extByteCode()->bc = opSpecial;
                    opSpecial->node             = funcNode;
                }
                break;
            default:
                break;
        }
    }

    ScopedLock lk1(structNode->mutex);
    typeInfoStruct->addFlag(TYPEINFO_SPEC_OP_GENERATED);
    structNode->addSemFlag(SEMFLAG_STRUCT_OP_ALLOCATED);
    structNode->dependentJobs.setRunning();
    SWAG_ASSERT(structNode->hasSemFlag(SEMFLAG_STRUCT_OP_ALLOCATED));
}

void ByteCodeGen::emitOpCallUserArrayOfStruct(const ByteCodeGenContext* context, TypeInfo* typeVar, EmitOpUserKind kind, bool pushParam, uint32_t offset)
{
    SWAG_ASSERT(typeVar->isArrayOfStruct());
    const auto typeArray     = castTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array);
    const auto typeStructVar = castTypeInfo<TypeInfoStruct>(typeArray->finalType, TypeInfoKind::Struct);
    if (typeArray->totalCount == 1)
    {
        if (!pushParam)
            EMIT_INST1(context, ByteCodeOp::PushRAParam, 0);
        emitOpCallUser(context, typeStructVar, kind, pushParam, offset);
    }
    else if (typeArray->totalCount == 2)
    {
        if (!pushParam)
            EMIT_INST1(context, ByteCodeOp::PushRAParam, 0);
        emitOpCallUser(context, typeStructVar, kind, pushParam, offset);
        const auto inst = EMIT_INST0(context, ByteCodeOp::IncPointer64);
        inst->addFlag(BCI_IMM_B);
        inst->b.u64 = typeStructVar->sizeOf;
        EMIT_INST1(context, ByteCodeOp::PushRAParam, 0);
        emitOpCallUser(context, typeStructVar, kind, false);
    }
    else
    {
        // Reference to the field
        if (pushParam)
        {
            auto inst               = EMIT_INST0(context, ByteCodeOp::GetParam64);
            inst->b.mergeU64U32.low = 24;
            if (offset)
            {
                inst        = EMIT_INST0(context, ByteCodeOp::IncPointer64);
                inst->b.u64 = offset;
                inst->addFlag(BCI_IMM_B);
            }
        }

        // Need to loop on every element of the array
        RegisterList r0 = reserveRegisterRC(context);

        auto inst           = EMIT_INST1(context, ByteCodeOp::SetImmediate32, r0);
        inst->b.u64         = typeArray->totalCount;
        const auto seekJump = context->bc->numInstructions;

        EMIT_INST1(context, ByteCodeOp::PushRAParam, 0);
        emitOpCallUser(context, typeStructVar, kind, false);

        inst = EMIT_INST0(context, ByteCodeOp::IncPointer64);
        inst->addFlag(BCI_IMM_B);
        inst->b.u64 = typeStructVar->sizeOf;

        EMIT_INST1(context, ByteCodeOp::DecrementRA32, r0);
        EMIT_INST1(context, ByteCodeOp::JumpIfNotZero32, r0)->b.s32 = static_cast<int32_t>(seekJump - context->bc->numInstructions - 1);

        freeRegisterRC(context, r0);
    }
}

void ByteCodeGen::emitOpCallUserFields(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct, EmitOpUserKind kind)
{
    for (const auto typeParam : typeInfoStruct->fields)
    {
        PushLocation pl(context, typeParam->declNode);

        const auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->isArrayOfStruct())
        {
            const auto typeArray     = castTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array);
            const auto typeStructVar = castTypeInfo<TypeInfoStruct>(typeArray->finalType, TypeInfoKind::Struct);

            switch (kind)
            {
                case EmitOpUserKind::Init:
                    if (!typeStructVar->opInit && !typeStructVar->opUserInitFct)
                        continue;
                    break;
                case EmitOpUserKind::Drop:
                    if (!typeStructVar->opDrop && !typeStructVar->opUserDropFct)
                        continue;
                    break;
                case EmitOpUserKind::PostCopy:
                    if (!typeStructVar->opPostCopy && !typeStructVar->opUserPostCopyFct)
                        continue;
                    break;
                case EmitOpUserKind::PostMove:
                    if (!typeStructVar->opPostMove && !typeStructVar->opUserPostMoveFct)
                        continue;
                    break;
                default:
                    break;
            }

            emitOpCallUserArrayOfStruct(context, typeVar, kind, true, typeParam->offset);
        }
        else if (typeVar->isStruct())
        {
            const auto typeStructVar = castTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
            emitOpCallUser(context, typeStructVar, kind, true, typeParam->offset);
        }
    }
}

bool ByteCodeGen::generateStructOpInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->hasFlag(TYPEINFO_STRUCT_NO_INIT))
        return true;

    const auto structNode = castAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    SymbolName* symbol;

    // Need to be sure that function has been solved
    {
        ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
        symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opInit, g_LangSpec->name_opInitCrc);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->baseJob);
            context->baseJob->setPending(JobWaitKind::EmitInit, symbol, structNode, nullptr);
            return true;
        }
    }

    // For generic function, symbol is not registered in the scope of the instantiated struct, but in the
    // generic struct
    if (!symbol && typeInfoStruct->opUserInitFct)
    {
        ScopedLock lockTable(typeInfoStruct->opUserInitFct->ownerScope->symTable.mutex);
        symbol = typeInfoStruct->opUserInitFct->ownerScope->symTable.findNoLock(g_LangSpec->name_opInit, g_LangSpec->name_opInitCrc);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->baseJob);
            context->baseJob->setPending(JobWaitKind::EmitInit, symbol, structNode, nullptr);
            return true;
        }
    }

    // If user function is foreign, then this is the generated version with everything already done
    if (typeInfoStruct->opUserInitFct && typeInfoStruct->opUserInitFct->hasAttribute(ATTRIBUTE_FOREIGN))
        return true;

    // Need to wait for user function full semantic resolve
    if (typeInfoStruct->opUserInitFct)
    {
        // Content must have been solved ! where pb
        SWAG_ASSERT(!typeInfoStruct->opUserInitFct->content->hasAstFlag(AST_NO_SEMANTIC));

        askForByteCode(context->baseJob, typeInfoStruct->opUserInitFct, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
        YIELD();
    }

    // Be sure sub structs are generated too
    for (const auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->isArray())
            typeVar = castTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array)->pointedType;
        if (!typeVar->isStruct())
            continue;
        const auto typeStructVar = castTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        Semantic::waitStructGenerated(context->baseJob, typeStructVar);
        YIELD();
        SWAG_ASSERT(typeStructVar->hasFlag(TYPEINFO_SPEC_OP_GENERATED));
        generateStructOpInit(context, typeStructVar);
        YIELD();
    }

    const auto sourceFile = context->sourceFile;
    const auto opInit     = typeInfoStruct->opInit;

    typeInfoStruct->addFlag(TYPEINFO_STRUCT_NO_INIT);
    SWAG_ASSERT(typeInfoStruct->opInit);
    sourceFile->module->addByteCodeFunc(opInit);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opInit;
    if (cxt.bc->node)
        cxt.bc->node->addSemFlag(SEMFLAG_BYTECODE_RESOLVED | SEMFLAG_BYTECODE_GENERATED);

    // All fields are explicitly not initialized, so we are done, function is empty
    if (typeInfoStruct->hasFlag(TYPEINFO_STRUCT_ALL_UNINITIALIZED))
    {
        EMIT_INST0(&cxt, ByteCodeOp::Ret);
        EMIT_INST0(&cxt, ByteCodeOp::End);
        if (structNode->hasAttribute(ATTRIBUTE_PRINT_BC))
        {
            ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
            cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
        }

        if (structNode->hasAttribute(ATTRIBUTE_PRINT_GEN_BC))
        {
            constexpr ByteCodePrintOptions opt;
            cxt.bc->print(opt);
        }
        return true;
    }

    // No special value, so we can just clear the struct
    if (!typeInfoStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
    {
        const auto inst         = EMIT_INST0(&cxt, ByteCodeOp::GetParam64);
        inst->b.mergeU64U32.low = 24;
        emitSetZeroAtPointer(&cxt, typeInfoStruct->sizeOf, 0);
        EMIT_INST0(&cxt, ByteCodeOp::Ret);
        EMIT_INST0(&cxt, ByteCodeOp::End);
        if (structNode->hasAttribute(ATTRIBUTE_PRINT_BC))
        {
            ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
            cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
        }

        if (structNode->hasAttribute(ATTRIBUTE_PRINT_GEN_BC))
        {
            constexpr ByteCodePrintOptions opt;
            cxt.bc->print(opt);
        }
        return true;
    }

    for (const auto param : typeInfoStruct->fields)
    {
        const auto varDecl = castAst<AstVarDecl>(param->declNode, AstNodeKind::VarDecl);
        const auto typeVar = TypeManager::concreteType(param->typeInfo);

        // Reference to the field
        auto inst               = EMIT_INST0(&cxt, ByteCodeOp::GetParam64);
        inst->b.mergeU64U32.low = 24;
        if (param->offset)
        {
            inst = EMIT_INST0(&cxt, ByteCodeOp::IncPointer64);
            SWAG_ASSERT(param->offset != 0xFFFFFFFF);
            inst->b.u64 = param->offset;
            inst->addFlag(BCI_IMM_B);
        }

        // A structure initialized with a literal
        if (varDecl->type && varDecl->type->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS))
        {
            const auto varType = varDecl->type;
            SWAG_ASSERT(varType->computedValue()->storageSegment);
            SWAG_ASSERT(varType->computedValue()->storageOffset != 0xFFFFFFFF);
            emitMakeSegPointer(&cxt, varType->computedValue()->storageSegment, varType->computedValue()->storageOffset, 1);
            emitMemCpy(&cxt, 0, 1, typeVar->sizeOf);
            continue;
        }

        // User initialization
        if (varDecl->assignment && varDecl->assignment->isNot(AstNodeKind::ExplicitNoInit))
        {
            if (typeVar->isArray())
            {
                if (varDecl->assignment->is(AstNodeKind::ExpressionList))
                {
                    const auto exprList = castAst<AstExpressionList>(varDecl->assignment, AstNodeKind::ExpressionList);
                    SWAG_ASSERT(exprList->computedValue()->storageSegment);
                    SWAG_ASSERT(exprList->computedValue()->storageOffset != UINT32_MAX);
                    emitMakeSegPointer(&cxt, exprList->computedValue()->storageSegment, exprList->computedValue()->storageOffset, 1);
                    emitMemCpy(&cxt, 0, 1, typeVar->sizeOf);
                }
                else
                {
                    RegisterList r0 = 0;
                    if (varDecl->assignment->typeInfo->isString())
                    {
                        const auto storageSegment = Semantic::getConstantSegFromContext(varDecl);
                        const auto storageOffset  = storageSegment->addString(varDecl->assignment->computedValue()->text);
                        SWAG_ASSERT(storageOffset != UINT32_MAX);
                        emitMakeSegPointer(&cxt, storageSegment, storageOffset, 1);
                        EMIT_INST1(&cxt, ByteCodeOp::SetImmediate64, 2)->b.u64 = varDecl->assignment->computedValue()->text.length();
                        RegisterList r1;
                        r1 += 1;
                        r1 += 2;
                        emitCopyArray(&cxt, typeVar, r0, r1, varDecl->assignment);
                    }
                    else
                    {
                        RegisterList r1 = 1;
                        inst            = EMIT_INST1(&cxt, ByteCodeOp::SetImmediate64, 1);
                        inst->b.u64     = varDecl->assignment->computedValue()->reg.u64;
                        emitCopyArray(&cxt, typeVar, r0, r1, varDecl->assignment);
                    }
                }
            }
            else if (typeVar->isString())
            {
                const auto storageSegment = Semantic::getConstantSegFromContext(varDecl);
                const auto storageOffset  = storageSegment->addString(varDecl->assignment->computedValue()->text);
                SWAG_ASSERT(storageOffset != UINT32_MAX);
                emitMakeSegPointer(&cxt, storageSegment, storageOffset, 1);
                EMIT_INST1(&cxt, ByteCodeOp::SetImmediate64, 2)->b.u64 = varDecl->assignment->computedValue()->text.length();
                EMIT_INST2(&cxt, ByteCodeOp::SetAtPointer64, 0, 1);
                EMIT_INST2(&cxt, ByteCodeOp::SetAtPointer64, 0, 2)->c.u32 = 8;
            }
            // :opAffectConstExpr
            else if (typeVar->isStruct() &&
                     varDecl->resolvedSymbolOverload() &&
                     varDecl->resolvedSymbolOverload()->hasFlag(OVERLOAD_STRUCT_AFFECT) &&
                     varDecl->computedValue() &&
                     varDecl->computedValue()->storageSegment)
            {
                const auto storageSegment = varDecl->computedValue()->storageSegment;
                const auto storageOffset  = varDecl->computedValue()->storageOffset;
                SWAG_ASSERT(storageSegment);
                SWAG_ASSERT(storageOffset != UINT32_MAX);
                emitMakeSegPointer(&cxt, storageSegment, storageOffset, 1);
                emitMemCpy(&cxt, 0, 1, typeVar->sizeOf);
            }
            else
            {
                switch (typeVar->sizeOf)
                {
                    case 1:
                        EMIT_INST1(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u64 = varDecl->assignment->computedValue()->reg.u8;
                        EMIT_INST2(&cxt, ByteCodeOp::SetAtPointer8, 0, 1);
                        break;
                    case 2:
                        EMIT_INST1(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u64 = varDecl->assignment->computedValue()->reg.u16;
                        EMIT_INST2(&cxt, ByteCodeOp::SetAtPointer16, 0, 1);
                        break;
                    case 4:
                        EMIT_INST1(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u64 = varDecl->assignment->computedValue()->reg.u32;
                        EMIT_INST2(&cxt, ByteCodeOp::SetAtPointer32, 0, 1);
                        break;
                    case 8:
                        EMIT_INST1(&cxt, ByteCodeOp::SetImmediate64, 1)->b.u64 = varDecl->assignment->computedValue()->reg.u64;
                        EMIT_INST2(&cxt, ByteCodeOp::SetAtPointer64, 0, 1);
                        break;
                    default:
                        return Report::internalError(varDecl, "generateStructInit, invalid native type sizeof");
                }
            }

            continue;
        }

        // Default initialization
        if (typeVar->isArrayOfStruct())
        {
            const auto typeArray     = castTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array);
            const auto typeVarStruct = castTypeInfo<TypeInfoStruct>(typeArray->finalType, TypeInfoKind::Struct);
            if (typeVarStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
                emitOpCallUserArrayOfStruct(&cxt, typeVar, EmitOpUserKind::Init, false, 0);
            else
                emitSetZeroAtPointer(&cxt, typeVar->sizeOf, 0);
        }
        else if (typeVar->isStruct() && typeVar->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
        {
            const auto typeVarStruct = castTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
            SWAG_ASSERT(typeVarStruct->opInit || typeVarStruct->opUserInitFct);
            EMIT_INST1(&cxt, ByteCodeOp::PushRAParam, 0);
            emitOpCallUser(&cxt, typeVarStruct->opUserInitFct, typeVarStruct->opInit, false);
        }
        else
        {
            emitSetZeroAtPointer(&cxt, typeVar->sizeOf, 0);
        }
    }

    EMIT_INST0(&cxt, ByteCodeOp::Ret);
    EMIT_INST0(&cxt, ByteCodeOp::End);

    if (structNode->hasAttribute(ATTRIBUTE_PRINT_BC))
    {
        ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    if (structNode->hasAttribute(ATTRIBUTE_PRINT_GEN_BC))
    {
        constexpr ByteCodePrintOptions opt;
        cxt.bc->print(opt);
    }

    return true;
}

bool ByteCodeGen::generateStructOpDrop(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->hasFlag(TYPEINFO_STRUCT_NO_DROP))
        return true;

    SWAG_ASSERT(typeInfoStruct->declNode);
    const auto sourceFile = context->sourceFile;
    const auto structNode = castAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    SymbolName* symbol = nullptr;

    // Need to be sure that function has been solved
    {
        ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
        symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opDrop, g_LangSpec->name_opDropCrc);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->baseJob);
            context->baseJob->setPending(JobWaitKind::EmitDrop, symbol, structNode, nullptr);
            return true;
        }
    }

    // For generic function, symbol is not registered in the scope of the instantiated struct, but in the
    // generic struct
    if (!symbol && typeInfoStruct->opUserDropFct)
    {
        ScopedLock lockTable(typeInfoStruct->opUserDropFct->ownerScope->symTable.mutex);
        symbol = typeInfoStruct->opUserDropFct->ownerScope->symTable.findNoLock(g_LangSpec->name_opDrop, g_LangSpec->name_opDropCrc);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->baseJob);
            context->baseJob->setPending(JobWaitKind::EmitDrop, symbol, structNode, nullptr);
            return true;
        }
    }

    // If user function is foreign, then this is the generated version with everything already done
    if (typeInfoStruct->opUserDropFct && typeInfoStruct->opUserDropFct->hasAttribute(ATTRIBUTE_FOREIGN))
        return true;

    // Need to wait for user function full semantic resolve
    bool needDrop = false;
    if (typeInfoStruct->opUserDropFct)
    {
        // Content must have been solved ! where pb
        SWAG_ASSERT(!typeInfoStruct->opUserDropFct->content->hasAstFlag(AST_NO_SEMANTIC));

        needDrop = true;
        askForByteCode(context->baseJob, typeInfoStruct->opUserDropFct, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
        YIELD();
    }

    // Be sure sub structs are generated too
    for (const auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->isArray())
            typeVar = castTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array)->pointedType;
        if (!typeVar->isStruct())
            continue;
        const auto typeStructVar = castTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        Semantic::waitStructGenerated(context->baseJob, typeStructVar);
        YIELD();
        SWAG_ASSERT(typeStructVar->hasFlag(TYPEINFO_SPEC_OP_GENERATED));
        generateStructOpDrop(context, typeStructVar);
        YIELD();
        if (typeStructVar->opDrop || typeStructVar->opUserDropFct)
            needDrop = true;
        if (typeStructVar->opDrop || typeStructVar->opUserDropFct)
            SWAG_VERIFY(!structNode->hasSpecFlag(AstStruct::SPEC_FLAG_UNION), context->report({typeParam->declNode, formErr(Err0709, typeStructVar->getDisplayNameC(), "opDrop")}));
    }

    typeInfoStruct->addFlag(TYPEINFO_STRUCT_NO_DROP);
    if (!needDrop)
        return true;

    const auto opDrop = typeInfoStruct->opDrop;
    SWAG_ASSERT(opDrop);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opDrop;
    if (cxt.bc->node)
        cxt.bc->node->addSemFlag(SEMFLAG_BYTECODE_RESOLVED | SEMFLAG_BYTECODE_GENERATED);

    // Call user function if defined
    emitOpCallUser(&cxt, typeInfoStruct->opUserDropFct);

    // Call for each field
    emitOpCallUserFields(&cxt, typeInfoStruct, EmitOpUserKind::Drop);

    EMIT_INST0(&cxt, ByteCodeOp::Ret);
    EMIT_INST0(&cxt, ByteCodeOp::End);

    if (structNode->hasAttribute(ATTRIBUTE_PRINT_BC))
    {
        ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    if (structNode->hasAttribute(ATTRIBUTE_PRINT_GEN_BC))
    {
        constexpr ByteCodePrintOptions opt;
        cxt.bc->print(opt);
    }

    sourceFile->module->addByteCodeFunc(opDrop);
    return true;
}

bool ByteCodeGen::generateStructOpPostCopy(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->hasFlag(TYPEINFO_STRUCT_NO_POST_COPY))
        return true;
    if (typeInfoStruct->hasFlag(TYPEINFO_STRUCT_NO_COPY))
        return true;

    const auto sourceFile = context->sourceFile;
    const auto structNode = castAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    SymbolName* symbol = nullptr;

    // Need to be sure that function has been solved
    {
        ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
        symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opPostCopy, g_LangSpec->name_opPostCopyCrc);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->baseJob);
            context->baseJob->setPending(JobWaitKind::EmitPostCopy, symbol, structNode, nullptr);
            return true;
        }
    }

    // For generic function, symbol is not registered in the scope of the instantiated struct, but in the
    // generic struct
    if (!symbol && typeInfoStruct->opUserPostCopyFct)
    {
        ScopedLock lockTable(typeInfoStruct->opUserPostCopyFct->ownerScope->symTable.mutex);
        symbol = typeInfoStruct->opUserPostCopyFct->ownerScope->symTable.findNoLock(g_LangSpec->name_opPostCopy, g_LangSpec->name_opPostCopyCrc);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->baseJob);
            context->baseJob->setPending(JobWaitKind::EmitPostCopy, symbol, structNode, nullptr);
            return true;
        }
    }

    // If user function is foreign, then this is the generated version with everything already done
    if (typeInfoStruct->opUserPostCopyFct && typeInfoStruct->opUserPostCopyFct->hasAttribute(ATTRIBUTE_FOREIGN))
        return true;

    // Need to wait for function full semantic resolve
    bool needPostCopy = false;
    if (typeInfoStruct->opUserPostCopyFct)
    {
        // Content must have been solved ! where pb
        SWAG_ASSERT(!typeInfoStruct->opUserPostCopyFct->content->hasAstFlag(AST_NO_SEMANTIC));

        needPostCopy = true;
        askForByteCode(context->baseJob, typeInfoStruct->opUserPostCopyFct, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
        YIELD();
    }

    // Be sure sub structs are generated too
    for (const auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->isArray())
            typeVar = castTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array)->pointedType;
        if (!typeVar->isStruct())
            continue;
        const auto typeStructVar = castTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        Semantic::waitStructGenerated(context->baseJob, typeStructVar);
        YIELD();
        SWAG_ASSERT(typeStructVar->hasFlag(TYPEINFO_SPEC_OP_GENERATED));
        generateStructOpPostCopy(context, typeStructVar);
        YIELD();
        if (typeStructVar->opPostCopy || typeStructVar->opUserPostCopyFct)
            needPostCopy = true;
        if (typeStructVar->opPostCopy || typeStructVar->opUserPostCopyFct)
            SWAG_VERIFY(!structNode->hasSpecFlag(AstStruct::SPEC_FLAG_UNION), context->report({typeParam->declNode, formErr(Err0709, typeStructVar->getDisplayNameC(), "opPostCopy")}));
    }

    typeInfoStruct->addFlag(TYPEINFO_STRUCT_NO_POST_COPY);
    if (!needPostCopy)
        return true;

    const auto opPostCopy = typeInfoStruct->opPostCopy;
    SWAG_ASSERT(opPostCopy);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostCopy;
    if (cxt.bc->node)
        cxt.bc->node->addSemFlag(SEMFLAG_BYTECODE_RESOLVED | SEMFLAG_BYTECODE_GENERATED);

    // Call for each field
    emitOpCallUserFields(&cxt, typeInfoStruct, EmitOpUserKind::PostCopy);

    // Then call user function if defined
    emitOpCallUser(&cxt, typeInfoStruct->opUserPostCopyFct);

    EMIT_INST0(&cxt, ByteCodeOp::Ret);
    EMIT_INST0(&cxt, ByteCodeOp::End);

    if (structNode->hasAttribute(ATTRIBUTE_PRINT_BC))
    {
        ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    if (structNode->hasAttribute(ATTRIBUTE_PRINT_GEN_BC))
    {
        constexpr ByteCodePrintOptions opt;
        cxt.bc->print(opt);
    }

    sourceFile->module->addByteCodeFunc(opPostCopy);
    return true;
}

bool ByteCodeGen::generateStructOpPostMove(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->hasFlag(TYPEINFO_STRUCT_NO_POST_MOVE))
        return true;

    const auto sourceFile = context->sourceFile;
    const auto structNode = castAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    SymbolName* symbol = nullptr;

    // Need to be sure that function has been solved
    {
        ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
        symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opPostMove, g_LangSpec->name_opPostMoveCrc);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->baseJob);
            context->baseJob->setPending(JobWaitKind::EmitPostMove, symbol, structNode, nullptr);
            return true;
        }
    }

    // For generic function, symbol is not registered in the scope of the instantiated struct, but in the
    // generic struct
    if (!symbol && typeInfoStruct->opUserPostMoveFct)
    {
        ScopedLock lockTable(typeInfoStruct->opUserPostMoveFct->ownerScope->symTable.mutex);
        symbol = typeInfoStruct->opUserPostMoveFct->ownerScope->symTable.findNoLock(g_LangSpec->name_opPostMove, g_LangSpec->name_opPostMoveCrc);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->baseJob);
            context->baseJob->setPending(JobWaitKind::EmitPostMove, symbol, structNode, nullptr);
            return true;
        }
    }

    // If user function is foreign, then this is the generated version with everything already done
    if (typeInfoStruct->opUserPostMoveFct && typeInfoStruct->opUserPostMoveFct->hasAttribute(ATTRIBUTE_FOREIGN))
        return true;

    // Need to wait for function full semantic resolve
    bool needPostMove = false;
    if (typeInfoStruct->opUserPostMoveFct)
    {
        // Content must have been solved ! where pb
        SWAG_ASSERT(!typeInfoStruct->opUserPostMoveFct->content->hasAstFlag(AST_NO_SEMANTIC));

        needPostMove = true;
        askForByteCode(context->baseJob, typeInfoStruct->opUserPostMoveFct, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
        YIELD();
    }

    // Be sure sub structs are generated too
    for (const auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->isArray())
            typeVar = castTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array)->pointedType;
        if (!typeVar->isStruct())
            continue;
        const auto typeStructVar = castTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        Semantic::waitStructGenerated(context->baseJob, typeStructVar);
        YIELD();
        SWAG_ASSERT(typeStructVar->hasFlag(TYPEINFO_SPEC_OP_GENERATED));
        generateStructOpPostMove(context, typeStructVar);
        YIELD();
        if (typeStructVar->opPostMove || typeStructVar->opUserPostMoveFct)
            needPostMove = true;
        if (typeStructVar->opPostMove || typeStructVar->opUserPostMoveFct)
            SWAG_VERIFY(!structNode->hasSpecFlag(AstStruct::SPEC_FLAG_UNION),
                        context->report({typeParam->declNode, formErr(Err0709, typeStructVar->getDisplayNameC(), "opPostMove")}));
    }

    typeInfoStruct->addFlag(TYPEINFO_STRUCT_NO_POST_MOVE);
    if (!needPostMove)
        return true;

    const auto opPostMove = typeInfoStruct->opPostMove;
    SWAG_ASSERT(opPostMove);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostMove;
    if (cxt.bc->node)
        cxt.bc->node->addSemFlag(SEMFLAG_BYTECODE_RESOLVED | SEMFLAG_BYTECODE_GENERATED);

    // Call for each field
    emitOpCallUserFields(&cxt, typeInfoStruct, EmitOpUserKind::PostMove);

    // Then call user function if defined
    emitOpCallUser(&cxt, typeInfoStruct->opUserPostMoveFct);

    EMIT_INST0(&cxt, ByteCodeOp::Ret);
    EMIT_INST0(&cxt, ByteCodeOp::End);

    if (structNode->hasAttribute(ATTRIBUTE_PRINT_BC))
    {
        ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    if (structNode->hasAttribute(ATTRIBUTE_PRINT_GEN_BC))
    {
        constexpr ByteCodePrintOptions opt;
        cxt.bc->print(opt);
    }

    sourceFile->module->addByteCodeFunc(opPostMove);
    return true;
}

bool ByteCodeGen::emitStruct(ByteCodeGenContext* context)
{
    const AstStruct* node           = castAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    TypeInfoStruct*  typeInfoStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);

    generateStructAlloc(context, typeInfoStruct);
    YIELD();

    SWAG_CHECK(generateStructOpInit(context, typeInfoStruct));
    YIELD();
    SWAG_CHECK(generateStructOpDrop(context, typeInfoStruct));
    YIELD();
    SWAG_CHECK(generateStructOpPostCopy(context, typeInfoStruct));
    YIELD();
    SWAG_CHECK(generateStructOpPostMove(context, typeInfoStruct));
    YIELD();

    const auto structNode = castAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);
    ScopedLock lk(structNode->mutex);
    structNode->addSemFlag(SEMFLAG_BYTECODE_GENERATED);
    structNode->dependentJobs.setRunning();
    return true;
}

bool ByteCodeGen::emitCopyStruct(ByteCodeGenContext* context, const RegisterList& r0, const RegisterList& r1, TypeInfo* typeInfo, AstNode* from)
{
    const TypeInfoStruct* typeInfoStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);

    // Need to drop first
    if (typeInfoStruct->opDrop || typeInfoStruct->opUserDropFct)
    {
        const bool mustDrop = !from->hasAstFlag(AST_NO_LEFT_DROP);
        if (mustDrop)
        {
            EMIT_INST1(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, typeInfoStruct->opUserDropFct, typeInfoStruct->opDrop, false);
        }
    }

    // Shallow copy
    if (!typeInfoStruct->hasFlag(TYPEINFO_STRUCT_EMPTY))
        emitMemCpy(context, r0, r1, typeInfoStruct->sizeOf);

    // A copy
    const bool mustCopy = !from->hasAstFlag(AST_TRANSIENT | AST_FORCE_MOVE);
    if (mustCopy)
    {
        if (typeInfoStruct->hasFlag(TYPEINFO_STRUCT_NO_COPY))
        {
            const Diagnostic err{from, formErr(Err0090, typeInfo->getDisplayNameC())};
            return context->report(err);
        }

        PushICFlags sf(context, BCI_POST_COPY_MOVE);
        if (typeInfoStruct->opPostCopy || typeInfoStruct->opUserPostCopyFct)
        {
            EMIT_INST1(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, typeInfoStruct->opUserPostCopyFct, typeInfoStruct->opPostCopy, false);
        }
    }

    // A move
    else
    {
        PushICFlags sf(context, BCI_POST_COPY_MOVE);
        if (typeInfoStruct->opPostMove || typeInfoStruct->opUserPostMoveFct)
        {
            EMIT_INST1(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, typeInfoStruct->opUserPostMoveFct, typeInfoStruct->opPostMove, false);
        }

        // If the current scope contains a drop for that variable, then we remove it, because we have
        // just reset the content
        bool mustReinit = true;

        {
            SharedLock lk(from->ownerScope->symTable.mutexDrop);
            for (auto& toDrop : from->ownerScope->symTable.structVarsToDrop)
            {
                if (toDrop.overload && toDrop.overload->symbol->is(SymbolKind::Function) && from->is(AstNodeKind::IdentifierRef))
                {
                    if (toDrop.overload == from->resolvedSymbolOverload() && toDrop.storageOffset == from->lastChild()->computedValue()->storageOffset)
                    {
                        mustReinit        = false;
                        toDrop.typeStruct = nullptr;
                        break;
                    }
                }
            }
        }

        // Re-init source struct, except if AST_NO_RIGHT_DROP, because if we do not drop the
        // right expression, then this is not necessary to reinitialize it
        // Note that if we have remove the opDrop in the code above, no need to reinitialize the variable.
        if (mustReinit && (typeInfoStruct->opDrop || typeInfoStruct->opUserDropFct) && !from->hasAstFlag(AST_NO_RIGHT_DROP))
        {
            if ((typeInfoStruct->opInit || typeInfoStruct->opUserInitFct) && typeInfoStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
            {
                EMIT_INST1(context, ByteCodeOp::PushRAParam, r1);
                emitOpCallUser(context, typeInfoStruct->opUserInitFct, typeInfoStruct->opInit, false);
            }
            else
            {
                emitSetZeroAtPointer(context, typeInfoStruct->sizeOf, r1);
            }
        }
    }

    return true;
}

void ByteCodeGen::emitRetValRef(const ByteCodeGenContext* context, SymbolOverload* resolved, const RegisterList& r0, bool retVal, uint32_t stackOffset)
{
    const auto node = context->node;
    if (retVal)
    {
        const auto overload = node->resolvedSymbolOverload();
        SWAG_ASSERT(overload);
        if (overload->node->hasOwnerInline() && overload->node->ownerInline()->resultRegisterRc.countResults)
            EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0, overload->node->ownerInline()->resultRegisterRc);
        else if (node->hasOwnerInline() && node->ownerInline()->resultRegisterRc.countResults)
            EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0, node->ownerInline()->resultRegisterRc);
        else
            EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, r0);
    }
    else
    {
        const auto inst = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, r0);
        SWAG_ASSERT(stackOffset != UINT32_MAX);
        inst->b.u32     = stackOffset;
        inst->c.pointer = reinterpret_cast<uint8_t*>(resolved);
    }
}

bool ByteCodeGen::emitStructInit(const ByteCodeGenContext* context, const TypeInfoStruct* typeInfoStruct, uint32_t regOffset, bool retVal)
{
    const auto node     = context->node;
    const auto resolved = node->resolvedSymbolOverload();

    // All fields are explicitly not initialized, so we are done
    if (typeInfoStruct->hasFlag(TYPEINFO_STRUCT_ALL_UNINITIALIZED))
        return true;

    // Just clear the content of the structure
    if (!typeInfoStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
    {
        if (retVal)
        {
            RegisterList r0 = reserveRegisterRC(context);
            emitRetValRef(context, resolved, r0, true, 0);
            emitSetZeroAtPointer(context, typeInfoStruct->sizeOf, r0);
            freeRegisterRC(context, r0);
        }
        else
        {
            emitSetZeroStack(context, resolved->computedValue.storageOffset, typeInfoStruct->sizeOf);
        }
    }
    else
    {
        // Push self
        RegisterList r0 = reserveRegisterRC(context);
        emitRetValRef(context, resolved, r0, retVal, resolved->computedValue.storageOffset);

        // Offset variable reference
        if (regOffset != UINT32_MAX)
            EMIT_INST3(context, ByteCodeOp::IncPointer64, r0, regOffset, r0);

        // Then call
        SWAG_ASSERT(typeInfoStruct->opInit || typeInfoStruct->opUserInitFct);
        EMIT_INST1(context, ByteCodeOp::PushRAParam, r0);
        emitOpCallUser(context, typeInfoStruct->opUserInitFct, typeInfoStruct->opInit, false);
        freeRegisterRC(context, r0);
    }

    return true;
}

void ByteCodeGen::emitStructParameters(ByteCodeGenContext* context, uint32_t regOffset, bool retVal)
{
    PushContextFlags cf(context, BCC_FLAG_NO_SAFETY);
    const auto       node     = castAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
    const auto       resolved = node->resolvedSymbolOverload();

    if (node->type && node->type->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS))
    {
        RegisterList r0 = reserveRegisterRC(context);

        const auto typeExpression = castAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        auto       typeId         = typeExpression;
        while (typeId->typeFlags.has(TYPE_FLAG_IS_SUB_TYPE))
            typeId = castAst<AstTypeExpression>(typeId->lastChild(), AstNodeKind::TypeExpression);
        const auto identifier = castAst<AstIdentifier>(typeId->identifier->lastChild(), AstNodeKind::Identifier);

        if (identifier->callParameters)
        {
            for (const auto child : identifier->callParameters->children)
            {
                // Already set by something else, as a direct reference, so no need to copy
                if (child->hasSemFlag(SEMFLAG_FIELD_STRUCT))
                {
                    freeRegisterRC(context, child);
                    continue;
                }

                const auto param = castAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
                SWAG_ASSERT(param->resolvedParameter);
                const auto typeParam = param->resolvedParameter;

                emitRetValRef(context, resolved, r0, retVal, resolved->computedValue.storageOffset + typeParam->offset);
                if (retVal && typeParam->offset)
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, r0, 0, r0);
                    SWAG_ASSERT(typeParam->offset != 0xFFFFFFFF);
                    inst->b.u64 = typeParam->offset;
                    inst->addFlag(BCI_IMM_B);
                }

                if (regOffset != UINT32_MAX)
                    EMIT_INST3(context, ByteCodeOp::IncPointer64, r0, regOffset, r0);

                child->addAstFlag(AST_NO_LEFT_DROP);

                // When generating parameters for a closure call, keep the reference if we want one !
                auto       noRef = child->typeInfo;
                const auto front = param->firstChild();
                if (front->isNot(AstNodeKind::MakePointer) || !front->hasSpecFlag(AstMakePointer::SPEC_FLAG_TO_REF))
                    noRef = TypeManager::concretePtrRefType(noRef);

                emitAffectEqual(context, r0, child->resultRegisterRc, noRef, child);
                SWAG_ASSERT(context->result == ContextResult::Done);

                freeRegisterRC(context, child);
            }
        }

        freeRegisterRC(context, r0);
    }
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ByteCodeGen::freeStructParametersRegisters(ByteCodeGenContext* context)
{
    const auto node = castAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);
    if (node->type && node->type->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS))
    {
        auto typeExpression = castAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        while (typeExpression->typeFlags.has(TYPE_FLAG_IS_SUB_TYPE))
            typeExpression = castAst<AstTypeExpression>(typeExpression->lastChild(), AstNodeKind::TypeExpression);
        const auto identifier = castAst<AstIdentifier>(typeExpression->identifier->lastChild(), AstNodeKind::Identifier);
        if (identifier->callParameters)
        {
            for (const auto child : identifier->callParameters->children)
            {
                freeRegisterRC(context, child);
            }
        }
    }
}

bool ByteCodeGen::emitInit(ByteCodeGenContext* context)
{
    const auto node = castAst<AstInit>(context->node, AstNodeKind::Init);

    // Number of values to initialize. 0 is dynamic (comes from a register)
    uint64_t numToInit = 0;
    if (!node->count)
        numToInit = 1;
    else if (node->count->hasFlagComputedValue())
        numToInit = node->count->computedValue()->reg.u64;
    else if (!node->count->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->count, node->count->typeInfo, node->count->typeInfoCast));
        YIELD();
        node->count->addSemFlag(SEMFLAG_CAST1);
    }

    TypeInfo* pointedType = nullptr;
    if (node->count)
    {
        const auto typeExpression = castTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->expression->typeInfo), TypeInfoKind::Pointer);
        pointedType               = typeExpression->pointedType;
    }
    else
    {
        pointedType = node->expression->typeInfo;
        if (pointedType->isArray())
        {
            const auto typeArray = castTypeInfo<TypeInfoArray>(pointedType, TypeInfoKind::Array);
            pointedType          = typeArray->finalType;
            numToInit            = typeArray->totalCount;
        }
    }

    SWAG_CHECK(emitInit(context, pointedType, node->expression->resultRegisterRc, numToInit, node->count, node->parameters));
    YIELD();

    freeRegisterRC(context, node->expression);
    freeRegisterRC(context, node->count);
    if (node->parameters)
    {
        for (const auto c : node->parameters->children)
            freeRegisterRC(context, c);
    }

    return true;
}

bool ByteCodeGen::emitInit(ByteCodeGenContext* context, TypeInfo* pointedType, RegisterList& rExpr, uint64_t numToInit, const AstNode* count, AstNode* parameters)
{
    // Determine if we just need to clear the memory
    bool justClear = true;
    if (parameters)
    {
        for (const auto child : parameters->children)
        {
            if (!child->hasFlagComputedValue())
            {
                justClear = false;
                break;
            }

            if (child->computedValue()->reg.u64)
            {
                justClear = false;
                break;
            }
        }
    }

    TypeInfoStruct* typeStruct = nullptr;
    if (pointedType->isStruct())
    {
        typeStruct = castTypeInfo<TypeInfoStruct>(pointedType, TypeInfoKind::Struct);
        Semantic::waitStructGenerated(context->baseJob, typeStruct);
        YIELD();
        if (typeStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
            justClear = false;
    }

    if (justClear)
    {
        uint64_t sizeToClear = pointedType->sizeOf;
        if (sizeToClear)
        {
            if (numToInit)
            {
                sizeToClear *= numToInit;
                emitSetZeroAtPointer(context, sizeToClear, rExpr);
            }
            else
            {
                SWAG_ASSERT(count);
                EMIT_INST2(context, ByteCodeOp::SetZeroAtPointerXRB, rExpr, count->resultRegisterRc)->c.u64 = sizeToClear;
            }
        }
    }
    else if (!parameters || parameters->children.empty())
    {
        SWAG_ASSERT(typeStruct);
        if (!typeStruct->hasFlag(TYPEINFO_STRUCT_ALL_UNINITIALIZED))
        {
            SWAG_ASSERT(typeStruct->opInit || typeStruct->opUserInitFct);
            generateStructAlloc(context, typeStruct);
            SWAG_ASSERT(context->result == ContextResult::Done);
            SWAG_CHECK(generateStructOpInit(context, typeStruct));
            SWAG_ASSERT(context->result == ContextResult::Done);

            // Constant loop
            uint32_t regCount = 0;
            if (numToInit > 1)
            {
                regCount        = reserveRegisterRC(context);
                const auto inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regCount);
                inst->b.u64     = numToInit;
            }

            ensureCanBeChangedRC(context, rExpr);
            const auto startLoop = context->bc->numInstructions;

            // Dynamic loop
            uint32_t jumpAfter = 0;
            if (numToInit == 0)
            {
                jumpAfter = context->bc->numInstructions;
                EMIT_INST1(context, ByteCodeOp::JumpIfZero64, count->resultRegisterRc);
                EMIT_INST1(context, ByteCodeOp::DecrementRA64, count->resultRegisterRc);
            }

            EMIT_INST1(context, ByteCodeOp::PushRAParam, rExpr);
            SWAG_ASSERT(typeStruct->opInit || typeStruct->opUserInitFct);
            emitOpCallUser(context, typeStruct->opUserInitFct, typeStruct->opInit, false);

            // Dynamic loop
            if (numToInit == 0)
            {
                const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, rExpr, 0, rExpr);
                inst->addFlag(BCI_IMM_B);
                inst->b.u64                       = typeStruct->sizeOf;
                const auto instJump               = EMIT_INST0(context, ByteCodeOp::Jump);
                instJump->b.s32                   = static_cast<int32_t>(startLoop - context->bc->numInstructions);
                context->bc->out[jumpAfter].b.s32 = static_cast<int32_t>(context->bc->numInstructions - jumpAfter - 1);
            }

            // Constant loop
            else if (numToInit > 1)
            {
                const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, rExpr, 0, rExpr);
                inst->addFlag(BCI_IMM_B);
                inst->b.u64 = typeStruct->sizeOf;
                EMIT_INST1(context, ByteCodeOp::DecrementRA64, regCount);
                const auto instJump = EMIT_INST1(context, ByteCodeOp::JumpIfNotZero64, regCount);
                instJump->b.s32     = static_cast<int32_t>(startLoop - context->bc->numInstructions);
                freeRegisterRC(context, regCount);
            }
        }
    }
    else if (!typeStruct)
    {
        const auto child = parameters->firstChild();
        ensureCanBeChangedRC(context, rExpr);

        uint32_t regCount     = count ? count->resultRegisterRc[0] : 0;
        bool     freeRegCount = false;
        if (numToInit > 1 && !count)
        {
            regCount        = reserveRegisterRC(context);
            const auto inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regCount);
            inst->b.u64     = numToInit;
            freeRegCount    = true;
        }

        const auto startLoop = context->bc->numInstructions;

        uint32_t jumpAfter = 0;
        if (numToInit != 1)
        {
            jumpAfter = context->bc->numInstructions;
            EMIT_INST1(context, ByteCodeOp::JumpIfZero64, regCount);
            EMIT_INST1(context, ByteCodeOp::DecrementRA64, regCount);
        }

        SWAG_CHECK(emitAffectEqual(context, rExpr, child->resultRegisterRc, child->typeInfo, child));
        SWAG_ASSERT(context->result == ContextResult::Done);

        if (numToInit != 1)
        {
            const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, rExpr, 0, rExpr);
            inst->addFlag(BCI_IMM_B);
            inst->b.u64 = pointedType->sizeOf;

            const auto instJump               = EMIT_INST0(context, ByteCodeOp::Jump);
            instJump->b.s32                   = static_cast<int32_t>(startLoop - context->bc->numInstructions);
            context->bc->out[jumpAfter].b.s32 = static_cast<int32_t>(context->bc->numInstructions - jumpAfter - 1);
        }

        if (freeRegCount)
            freeRegisterRC(context, regCount);
    }
    else
    {
        RegisterList r1;
        reserveRegisterRC(context, r1, 1);
        ensureCanBeChangedRC(context, rExpr);

        uint32_t regCount     = count ? count->resultRegisterRc[0] : 0;
        bool     freeRegCount = false;
        if (numToInit > 1 && !count)
        {
            regCount        = reserveRegisterRC(context);
            const auto inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regCount);
            inst->b.u64     = numToInit;
            freeRegCount    = true;
        }

        const auto startLoop = context->bc->numInstructions;

        uint32_t jumpAfter = 0;
        if (numToInit != 1)
        {
            jumpAfter = context->bc->numInstructions;
            EMIT_INST1(context, ByteCodeOp::JumpIfZero64, regCount);
            EMIT_INST1(context, ByteCodeOp::DecrementRA64, regCount);
        }

        for (const auto child : parameters->children)
        {
            const auto param     = castAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            const auto typeParam = param->resolvedParameter;
            EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r1, rExpr);
            if (typeParam->offset)
                EMIT_INST1(context, ByteCodeOp::Add64byVB64, r1)->b.u64 = typeParam->offset;
            emitAffectEqual(context, r1, child->resultRegisterRc, child->typeInfo, child);
            SWAG_ASSERT(context->result == ContextResult::Done);
        }

        if (numToInit != 1)
        {
            const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, rExpr, 0, rExpr);
            inst->b.u64     = pointedType->sizeOf;
            inst->addFlag(BCI_IMM_B);

            const auto instJump               = EMIT_INST0(context, ByteCodeOp::Jump);
            instJump->b.s32                   = static_cast<int32_t>(startLoop - context->bc->numInstructions);
            context->bc->out[jumpAfter].b.s32 = static_cast<int32_t>(context->bc->numInstructions - jumpAfter - 1);
        }

        for (const auto child : parameters->children)
            freeRegisterRC(context, child);
        freeRegisterRC(context, r1);
        if (freeRegCount)
            freeRegisterRC(context, regCount);
    }

    return true;
}

bool ByteCodeGen::emitDropCopyMove(ByteCodeGenContext* context)
{
    const auto node           = castAst<AstDropCopyMove>(context->node, AstNodeKind::Drop, AstNodeKind::PostCopy, AstNodeKind::PostMove);
    const auto typeExpression = castTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->expression->typeInfo), TypeInfoKind::Pointer);

    if (!typeExpression->pointedType->isStruct())
    {
        freeRegisterRC(context, node->expression);
        freeRegisterRC(context, node->count);
        return true;
    }

    // Number of elements to deal with. If 0, then this is dynamic
    uint64_t numToDo = 0;
    if (!node->count)
        numToDo = 1;
    else if (node->count->hasFlagComputedValue())
        numToDo = node->count->computedValue()->reg.u64;
    else if (!node->count->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->count, node->count->typeInfo, node->count->typeInfoCast));
        YIELD();
        node->count->addSemFlag(SEMFLAG_CAST1);
    }

    const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeExpression->pointedType, TypeInfoKind::Struct);
    Semantic::waitTypeCompleted(context->baseJob, typeStruct);
    YIELD();

    bool somethingToDo = false;
    switch (node->kind)
    {
        case AstNodeKind::Drop:
            generateStructAlloc(context, typeStruct);
            YIELD();
            generateStructOpDrop(context, typeStruct);
            YIELD();
            somethingToDo = typeStruct->opDrop || typeStruct->opUserDropFct;
            break;
        case AstNodeKind::PostCopy:
            generateStructAlloc(context, typeStruct);
            YIELD();
            generateStructOpPostCopy(context, typeStruct);
            YIELD();
            somethingToDo = typeStruct->opPostCopy || typeStruct->opUserPostCopyFct;
            break;
        case AstNodeKind::PostMove:
            generateStructAlloc(context, typeStruct);
            YIELD();
            generateStructOpPostMove(context, typeStruct);
            YIELD();
            somethingToDo = typeStruct->opPostMove || typeStruct->opUserPostMoveFct;
            break;
    }

    if (somethingToDo)
    {
        ensureCanBeChangedRC(context, node->expression->resultRegisterRc);

        const auto startLoop = context->bc->numInstructions;

        uint32_t jumpAfter = 0;
        if (numToDo != 1)
        {
            jumpAfter = context->bc->numInstructions;
            EMIT_INST1(context, ByteCodeOp::JumpIfZero64, node->count->resultRegisterRc);
            EMIT_INST1(context, ByteCodeOp::DecrementRA64, node->count->resultRegisterRc);
        }

        EMIT_INST1(context, ByteCodeOp::PushRAParam, node->expression->resultRegisterRc);

        switch (node->kind)
        {
            case AstNodeKind::Drop:
                emitOpCallUser(context, typeStruct->opUserDropFct, typeStruct->opDrop, false);
                break;
            case AstNodeKind::PostCopy:
                emitOpCallUser(context, typeStruct->opUserPostCopyFct, typeStruct->opPostCopy, false);
                break;
            case AstNodeKind::PostMove:
                emitOpCallUser(context, typeStruct->opUserPostMoveFct, typeStruct->opPostMove, false);
                break;
            default:
                break;
        }

        if (numToDo != 1)
        {
            const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, node->expression->resultRegisterRc, 0, node->expression->resultRegisterRc);
            inst->b.u64     = typeExpression->pointedType->sizeOf;
            inst->addFlag(BCI_IMM_B);
            const auto instJump               = EMIT_INST0(context, ByteCodeOp::Jump);
            instJump->b.s32                   = static_cast<int32_t>(startLoop - context->bc->numInstructions);
            context->bc->out[jumpAfter].b.s32 = static_cast<int32_t>(context->bc->numInstructions - jumpAfter - 1);
        }
    }

    freeRegisterRC(context, node->expression);
    freeRegisterRC(context, node->count);
    return true;
}
