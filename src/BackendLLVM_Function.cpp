#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "BackendLLVM_Macros.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Diagnostic.h"

bool BackendLLVM::emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc)
{
    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC) && (buildParameters.compileType != BackendCompileType::Test))
        return true;

    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;
    auto  typeFunc        = bc->getCallType();
    auto  returnType      = TypeManager::concreteType(typeFunc->returnType);
    bool  ok              = true;

    // Get function name
    Utf8         funcName;
    AstFuncDecl* node = bc->node ? CastAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl) : nullptr;
    if (node)
        funcName = node->getCallName();
    else
        funcName = bc->getCallName();

    // Function prototype
    auto            funcType = getOrCreateFuncType(buildParameters, moduleToGen, typeFunc);
    llvm::Function* func     = (llvm::Function*) modu.getOrInsertFunction(funcName.c_str(), funcType).getCallee();
    setFuncAttributes(buildParameters, moduleToGen, node, bc, func);

    // No pointer aliasing, on all pointers. Is this correct ??
    // Note that without the NoAlias flag, some optims will not trigger (like vectorisation)
    /*for (int i = 0; i < func->arg_size(); i++)
    {
        auto arg = func->getArg(i);
        if (!arg->getType()->isPointerTy())
            continue;
        arg->addAttr(llvm::Attribute::NoAlias);
    }*/

    // Export symbol
    if (node && node->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
            func->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);
    }

    // Content
    llvm::BasicBlock* block         = llvm::BasicBlock::Create(context, "entry", func);
    bool              blockIsClosed = false;
    builder.SetInsertPoint(block);
    pp.labels.clear();
    bc->markLabels();

    // Reserve registers
    llvm::AllocaInst* allocR = nullptr;
    if (bc->maxReservedRegisterRC)
        allocR = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt32(bc->maxReservedRegisterRC));
    llvm::AllocaInst* allocRR = nullptr;
    if (bc->maxCallResults)
        allocRR = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt32(bc->maxCallResults));
    llvm::AllocaInst* allocResult = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt32(1));

    // To store variadics
    llvm::AllocaInst* allocVA = nullptr;
    if (bc->maxSPVaargs)
        allocVA = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(bc->maxSPVaargs));

    // Stack
    llvm::AllocaInst* allocStack = nullptr;
    if (typeFunc->stackSize)
    {
        allocStack = builder.CreateAlloca(builder.getInt8Ty(), builder.getInt32(typeFunc->stackSize));
        allocStack->setAlignment(llvm::Align(16));
    }

    // Reserve room to pass parameters to embedded intrinsics
    const int ALLOCT_NUM = 5;
    auto      allocT     = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(ALLOCT_NUM));

    // Debug infos
    if (pp.dbg && bc->node)
    {
        pp.dbg->startFunction(buildParameters, pp, bc, func, allocStack);
        pp.dbg->setLocation(pp.builder, bc, bc->out);
    }

    // Generate bytecode
    auto                                   ip = bc->out;
    VectorNative<pair<uint32_t, uint32_t>> pushRVParams;
    VectorNative<uint32_t>                 pushRAParams;
    llvm::Value*                           resultFuncCall = nullptr;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
            continue;
        if (ip->flags & BCI_NO_BACKEND)
            continue;

        // If we are the destination of a jump, be sure we have a block, and from now insert into that block
        if ((ip->flags & BCI_JUMP_DEST) || blockIsClosed)
        {
            auto label = getOrCreateLabel(pp, func, i);
            if (!blockIsClosed)
                builder.CreateBr(label); // Make a jump from previous block to this one
            else
                blockIsClosed = false; // Each block must be closed by a valid terminator instruction
            builder.SetInsertPoint(label);
        }

        if (pp.dbg && bc->node)
            pp.dbg->setLocation(pp.builder, bc, ip);

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::Nop:
        case ByteCodeOp::DecSPBP:
        case ByteCodeOp::SetBP:
        case ByteCodeOp::IntrinsicBcBreakpoint:
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
            continue;
        }

        switch (ip->op)
        {
        case ByteCodeOp::DebugNop:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocT, 0));
            builder.CreateStore(builder.getInt32(0), r0);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicAddS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAddS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I16(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAddS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAddS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(v0, r3);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicAndS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAndS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I16(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAndS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAndS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(v0, r3);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicOrS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicOrS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I16(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicOrS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicOrS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(v0, r3);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicXorS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXorS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I16(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXorS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXorS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(v0, r3);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicXchgS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXchgS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I16(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXchgS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXchgS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(v0, r3);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
        {
            auto r0   = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto ptr0 = builder.CreateLoad(r0);

            llvm::Value* r1 = MK_IMMB_8();
            llvm::Value* r2 = MK_IMMC_8();
            auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
            v0->setVolatile(true);
            auto v1 = builder.CreateExtractValue(v0, 0);

            auto r4 = TO_PTR_I8(GEP_I32(allocR, ip->d.u32));
            builder.CreateStore(v1, r4);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
        {
            auto r0   = TO_PTR_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto ptr0 = builder.CreateLoad(r0);

            llvm::Value* r1 = MK_IMMB_16();
            llvm::Value* r2 = MK_IMMC_16();
            auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
            v0->setVolatile(true);
            auto v1 = builder.CreateExtractValue(v0, 0);

            auto r4 = TO_PTR_I16(GEP_I32(allocR, ip->d.u32));
            builder.CreateStore(v1, r4);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
        {
            auto r0   = TO_PTR_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto ptr0 = builder.CreateLoad(r0);

            llvm::Value* r1 = MK_IMMB_32();
            llvm::Value* r2 = MK_IMMC_32();
            auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
            v0->setVolatile(true);
            auto v1 = builder.CreateExtractValue(v0, 0);

            auto r4 = TO_PTR_I32(GEP_I32(allocR, ip->d.u32));
            builder.CreateStore(v1, r4);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
        {
            auto r0   = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto ptr0 = builder.CreateLoad(r0);

            llvm::Value* r1 = MK_IMMB_64();
            llvm::Value* r2 = MK_IMMC_64();
            auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
            v0->setVolatile(true);
            auto v1 = builder.CreateExtractValue(v0, 0);

            auto r4 = GEP_I32(allocR, ip->d.u32);
            builder.CreateStore(v1, r4);
            break;
        }

        case ByteCodeOp::IncPointer64:
        case ByteCodeOp::DecPointer64:
        {
            auto         r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto         r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            llvm::Value* r2;
            if (ip->flags & BCI_IMM_B)
                r2 = builder.getInt64(ip->b.u64);
            else
                r2 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            if (ip->op == ByteCodeOp::DecPointer64)
                r2 = builder.CreateNeg(r2);
            auto r3 = builder.CreateInBoundsGEP(r1, r2);
            builder.CreateStore(r3, r0);
            break;
        }

        case ByteCodeOp::IncMulPointer64:
        {
            auto         r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto         r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            llvm::Value* r2;
            if (ip->flags & BCI_IMM_B)
                r2 = builder.getInt64(ip->b.u64 * ip->d.u64);
            else
            {
                r2 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
                r2 = builder.CreateMul(r2, builder.getInt64(ip->d.u64));
            }

            auto r3 = builder.CreateInBoundsGEP(r1, r2);
            builder.CreateStore(r3, r0);
            break;
        }

        case ByteCodeOp::DeRef8:
        {
            auto r0  = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto ptr = builder.CreateInBoundsGEP(builder.CreateLoad(r0), builder.getInt64(ip->c.s64));
            auto v1  = builder.CreateLoad(TO_PTR_I8(ptr));
            auto v2  = builder.CreateIntCast(v1, builder.getInt64Ty(), false);
            auto r1  = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(v2, r1);
            break;
        }
        case ByteCodeOp::DeRef16:
        {
            auto r0  = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto ptr = builder.CreateInBoundsGEP(builder.CreateLoad(r0), builder.getInt64(ip->c.s64));
            auto v1  = builder.CreateLoad(TO_PTR_I16(ptr));
            auto v2  = builder.CreateIntCast(v1, builder.getInt64Ty(), false);
            auto r1  = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(v2, r1);
            break;
        }
        case ByteCodeOp::DeRef32:
        {
            auto r0  = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto ptr = builder.CreateInBoundsGEP(builder.CreateLoad(r0), builder.getInt64(ip->c.s64));
            auto v1  = builder.CreateLoad(TO_PTR_I32(ptr));
            auto v2  = builder.CreateIntCast(v1, builder.getInt64Ty(), false);
            auto r1  = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(v2, r1);
            break;
        }
        case ByteCodeOp::DeRef64:
        {
            auto r0  = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto ptr = builder.CreateInBoundsGEP(builder.CreateLoad(r0), builder.getInt64(ip->c.s64));
            auto v1  = builder.CreateLoad(TO_PTR_I64(ptr));
            auto r1  = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(v1, r1);
            break;
        }
        case ByteCodeOp::DeRefStringSlice:
        {
            auto r0  = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1  = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto ptr = builder.CreateLoad(r0);
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
            auto ptr0 = builder.CreateInBoundsGEP(ptr, builder.getInt32(ip->c.u32 + 0));
            auto ptr8 = builder.CreateInBoundsGEP(ptr, builder.getInt32(ip->c.u32 + 8));
            auto v0   = builder.CreateLoad(TO_PTR_PTR_I8(ptr0));
            auto v8   = builder.CreateLoad(TO_PTR_PTR_I8(ptr8));
            builder.CreateStore(v8, r1);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::Mul64byVB64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.getInt64(ip->b.u64);
            builder.CreateStore(builder.CreateMul(builder.CreateLoad(r0), r1), r0);
            break;
        }
        case ByteCodeOp::Div64byVB64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.getInt64(ip->b.u64);
            builder.CreateStore(builder.CreateUDiv(builder.CreateLoad(r0), r1), r0);
            break;
        }

        case ByteCodeOp::GetFromMutableSeg64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(TO_PTR_I8(pp.mutableSeg), CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromBssSeg64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(TO_PTR_I8(pp.bssSeg), CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::SetZeroStack8:
        {
            auto r0 = builder.CreateInBoundsGEP(allocStack, CST_RA32);
            builder.CreateStore(pp.cst0_i8, r0);
            break;
        }
        case ByteCodeOp::SetZeroStack16:
        {
            auto r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            builder.CreateStore(pp.cst0_i16, r0);
            break;
        }
        case ByteCodeOp::SetZeroStack32:
        {
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            builder.CreateStore(pp.cst0_i32, r0);
            break;
        }
        case ByteCodeOp::SetZeroStack64:
        {
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            builder.CreateStore(pp.cst0_i64, r0);
            break;
        }
        case ByteCodeOp::SetZeroStackX:
        {
            builder.CreateMemSet(builder.CreateInBoundsGEP(allocStack, CST_RA32), pp.cst0_i8, ip->b.u32, llvm::Align{});
            break;
        }

        case ByteCodeOp::SetZeroAtPointer8:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i8, v0);
            break;
        }
        case ByteCodeOp::SetZeroAtPointer16:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i16, TO_PTR_I16(v0));
            break;
        }
        case ByteCodeOp::SetZeroAtPointer32:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i32, TO_PTR_I32(v0));
            break;
        }
        case ByteCodeOp::SetZeroAtPointer64:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i64, TO_PTR_I64(v0));
            break;
        }
        case ByteCodeOp::SetZeroAtPointerX:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RC32);
            builder.CreateMemSet(v0, pp.cst0_i8, ip->b.u64, llvm::Align{});
            break;
        }
        case ByteCodeOp::SetZeroAtPointerXRB:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r2 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            auto v2 = builder.CreateMul(r2, builder.getInt64(ip->c.u64));
            builder.CreateMemSet(builder.CreateLoad(r0), pp.cst0_i8, v2, llvm::Align{});
            break;
        }

        case ByteCodeOp::MakeMutableSegPointer:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.mutableSeg), CST_RB32);
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeBssSegPointer:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.bssSeg), CST_RB32);
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeConstantSegPointer:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.constantSeg), CST_RB32);
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeCompilerSegPointer:
            return Report::report({ip->node, Err(Err0060)});

        case ByteCodeOp::MakeStackPointer:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeStackPointer2:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(r1, r0);

            r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32));
            r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RD32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeStackPointerRT:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocRR, 0));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::GetFromStack8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::GetFromStack16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::GetFromStack32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::GetFromStack64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetIncFromStack64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateLoad(r1);
            auto v1 = builder.CreateAdd(v0, builder.getInt64(ip->c.s64));
            builder.CreateStore(v1, r0);
            break;
        }
        case ByteCodeOp::GetIncFromStack64DeRef8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateLoad(r1);
            auto v1 = builder.CreateAdd(v0, builder.getInt64(ip->c.s64));
            auto v2 = builder.CreateLoad(TO_PTR_I8(builder.CreateIntToPtr(v1, builder.getInt8PtrTy())));
            auto v3 = builder.CreateIntCast(v2, builder.getInt64Ty(), false);
            builder.CreateStore(v3, r0);
            break;
        }
        case ByteCodeOp::GetIncFromStack64DeRef16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateLoad(r1);
            auto v1 = builder.CreateAdd(v0, builder.getInt64(ip->c.s64));
            auto v2 = builder.CreateLoad(TO_PTR_I16(builder.CreateIntToPtr(v1, builder.getInt8PtrTy())));
            auto v3 = builder.CreateIntCast(v2, builder.getInt64Ty(), false);
            builder.CreateStore(v3, r0);
            break;
        }
        case ByteCodeOp::GetIncFromStack64DeRef32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateLoad(r1);
            auto v1 = builder.CreateAdd(v0, builder.getInt64(ip->c.s64));
            auto v2 = builder.CreateLoad(TO_PTR_I32(builder.CreateIntToPtr(v1, builder.getInt8PtrTy())));
            auto v3 = builder.CreateIntCast(v2, builder.getInt64Ty(), false);
            builder.CreateStore(v3, r0);
            break;
        }
        case ByteCodeOp::GetIncFromStack64DeRef64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateLoad(r1);
            auto v1 = builder.CreateAdd(v0, builder.getInt64(ip->c.s64));
            auto v2 = builder.CreateLoad(TO_PTR_I64(builder.CreateIntToPtr(v1, builder.getInt8PtrTy())));
            builder.CreateStore(v2, r0);
            break;
        }

        case ByteCodeOp::CopyStack8:
        {
            auto r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::CopyStack16:
        {
            auto r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            auto r1 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::CopyStack32:
        {
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            auto r1 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::CopyStack64:
        {
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::GetFromStack64x2:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);

            auto r2 = GEP_I32(allocR, ip->c.u32);
            auto r3 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RD32));
            builder.CreateStore(builder.CreateLoad(r3), r2);
            break;
        }

        case ByteCodeOp::IntrinsicDbgAlloc:
        {
            auto result = emitCall(buildParameters, moduleToGen, g_LangSpec->name_atdbgalloc, allocR, allocT, {}, {});
            builder.CreateStore(TO_PTR_I8(result), TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            break;
        }
        case ByteCodeOp::IntrinsicSysAlloc:
        {
            auto result = emitCall(buildParameters, moduleToGen, g_LangSpec->name_atsysalloc, allocR, allocT, {}, {});
            builder.CreateStore(TO_PTR_I8(result), TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            break;
        }
        case ByteCodeOp::IntrinsicRtFlags:
        {
            auto result = emitCall(buildParameters, moduleToGen, g_LangSpec->name_atrtflags, allocR, allocT, {}, {});
            builder.CreateStore(result, TO_PTR_I64(GEP_I32(allocR, ip->a.u32)));
            break;
        }
        case ByteCodeOp::IntrinsicStringCmp:
        {
            auto result = emitCall(buildParameters, moduleToGen, g_LangSpec->name_atstrcmp, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32}, {});
            builder.CreateStore(result, TO_PTR_I8(GEP_I32(allocR, ip->d.u32)));
            break;
        }
        case ByteCodeOp::IntrinsicTypeCmp:
        {
            auto result = emitCall(buildParameters, moduleToGen, g_LangSpec->name_attypecmp, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
            builder.CreateStore(result, TO_PTR_I8(GEP_I32(allocR, ip->d.u32)));
            break;
        }

        case ByteCodeOp::MemCpy8:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::MemCpy16:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I16(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::MemCpy32:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I32(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::MemCpy64:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::IntrinsicMemCpy:
        {
            auto r0   = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r1   = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto size = MK_IMMC_64();
            builder.CreateMemCpy(r0, llvm::Align{}, r1, llvm::Align{}, size);
            break;
        }

        case ByteCodeOp::IntrinsicMemMove:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto r2 = MK_IMMC_64();
            builder.CreateMemMove(r0, llvm::Align{}, r1, llvm::Align{}, r2);
            break;
        }

        case ByteCodeOp::IntrinsicMemSet:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r1 = MK_IMMB_8();
            auto r2 = MK_IMMC_64();
            builder.CreateMemSet(r0, r1, r2, llvm::Align{});
            break;
        }

        case ByteCodeOp::IntrinsicMemCmp:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto r2 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32)));
            auto r3 = MK_IMMD_64();
            builder.CreateStore(builder.CreateCall(pp.fn_memcmp, {r1, r2, r3}), r0);
            break;
        }

        case ByteCodeOp::IntrinsicStrLen:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(builder.CreateCall(pp.fn_strlen, {r1}), r0);
            break;
        }
        case ByteCodeOp::IntrinsicStrCmp:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto r2 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32)));
            builder.CreateStore(builder.CreateCall(pp.fn_strcmp, {r1, r2}), r0);
            break;
        }

        case ByteCodeOp::IntrinsicAlloc:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(builder.CreateCall(pp.fn_malloc, {r1}), r0);
            break;
        }
        case ByteCodeOp::IntrinsicRealloc:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto r2 = builder.CreateLoad(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(builder.CreateCall(pp.fn_realloc, {r1, r2}), r0);
            break;
        }
        case ByteCodeOp::IntrinsicFree:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            builder.CreateCall(pp.fn_free, {r0});
            break;
        }

        case ByteCodeOp::IntrinsicItfTableOf:
        {
            auto result = emitCall(buildParameters, moduleToGen, g_LangSpec->name_atitftableof, allocR, allocT, {ip->a.u32, ip->b.u32}, {});
            builder.CreateStore(result, TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32)));
            break;
        }

        case ByteCodeOp::SetImmediate32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(CST_RB32, r0);
            break;
        }
        case ByteCodeOp::SetImmediate64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(CST_RB64, r0);
            break;
        }

        case ByteCodeOp::CopyRBtoRA8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v1 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            builder.CreateStore(v1, r0);
            break;
        }
        case ByteCodeOp::CopyRBtoRA16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v1 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            builder.CreateStore(v1, r0);
            break;
        }
        case ByteCodeOp::CopyRBtoRA32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v1 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            builder.CreateStore(v1, r0);
            break;
        }
        case ByteCodeOp::CopyRBtoRA64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::CopyRBAddrToRA:
        case ByteCodeOp::CopyRBAddrToRA2:
        {
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::ClearRA:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(pp.cst0_i64, r0);
            break;
        }
        case ByteCodeOp::ClearRA2:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(pp.cst0_i64, r0);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(pp.cst0_i64, r1);
            break;
        }
        case ByteCodeOp::ClearRA3:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(pp.cst0_i64, r0);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(pp.cst0_i64, r1);
            auto r2 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(pp.cst0_i64, r2);
            break;
        }
        case ByteCodeOp::ClearRA4:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(pp.cst0_i64, r0);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(pp.cst0_i64, r1);
            auto r2 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(pp.cst0_i64, r2);
            auto r3 = GEP_I32(allocR, ip->d.u32);
            builder.CreateStore(pp.cst0_i64, r3);
            break;
        }

        case ByteCodeOp::DecrementRA32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), pp.cst1_i32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::IncrementRA64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), pp.cst1_i64);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::DecrementRA64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), pp.cst1_i64);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::MulAddVC64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            auto v1 = builder.CreateAdd(builder.CreateLoad(r1), CST_RC64);
            auto v2 = builder.CreateMul(builder.CreateLoad(r0), v1);
            auto r2 = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(v2, r2);
            break;
        }

        case ByteCodeOp::Add32byVB32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::Add64byVB64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), CST_RB64);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::SetAtPointer8:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            r0      = TO_PTR_I8(builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RC32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt8(ip->b.u8);
            else
                r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtPointer16:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            r0      = TO_PTR_I16(builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RC32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt16(ip->b.u16);
            else
                r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtPointer32:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            r0      = TO_PTR_I32(builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RC32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt32(ip->b.u32);
            else
                r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtPointer64:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            r0      = TO_PTR_I64(builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RC32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt64(ip->b.u64);
            else
                r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::SetAtStackPointer8:
        {
            auto         r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt8(ip->b.u8);
            else
                r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtStackPointer16:
        {
            auto         r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt16(ip->b.u16);
            else
                r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtStackPointer32:
        {
            auto         r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt32(ip->b.u32);
            else
                r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtStackPointer64:
        {
            auto         r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt64(ip->b.u64);
            else
                r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::SetAtStackPointer8x2:
        {
            {
                auto         r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RA32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_B)
                    r1 = builder.getInt8(ip->b.u8);
                else
                    r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
                builder.CreateStore(r1, r0);
            }
            {
                auto         r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RC32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_D)
                    r1 = builder.getInt8(ip->d.u8);
                else
                    r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->d.u32)));
                builder.CreateStore(r1, r0);
            }
            break;
        }
        case ByteCodeOp::SetAtStackPointer16x2:
        {
            {
                auto         r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RA32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_B)
                    r1 = builder.getInt16(ip->b.u16);
                else
                    r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
                builder.CreateStore(r1, r0);
            }
            {
                auto         r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RC32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_D)
                    r1 = builder.getInt16(ip->d.u16);
                else
                    r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->d.u32)));
                builder.CreateStore(r1, r0);
            }
            break;
        }
        case ByteCodeOp::SetAtStackPointer32x2:
        {
            {
                auto         r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RA32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_B)
                    r1 = builder.getInt32(ip->b.u32);
                else
                    r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                builder.CreateStore(r1, r0);
            }
            {
                auto         r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RC32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_D)
                    r1 = builder.getInt32(ip->d.u32);
                else
                    r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->d.u32)));
                builder.CreateStore(r1, r0);
            }
            break;
        }
        case ByteCodeOp::SetAtStackPointer64x2:
        {
            {
                auto         r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RA32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_B)
                    r1 = builder.getInt64(ip->b.u64);
                else
                    r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
                builder.CreateStore(r1, r0);
            }
            {
                auto         r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RC32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_D)
                    r1 = builder.getInt64(ip->d.u64);
                else
                    r1 = builder.CreateLoad(GEP_I32(allocR, ip->d.u32));
                builder.CreateStore(r1, r0);
            }
            break;
        }

        case ByteCodeOp::BinOpPlusS32:
        case ByteCodeOp::BinOpPlusS32_Safe:
        {
            MK_BINOP32_CAB();
            OP_OVERFLOW(sadd_with_overflow, CreateAdd, TO_PTR_I32, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS32));
            break;
        }
        case ByteCodeOp::BinOpPlusU32:
        case ByteCodeOp::BinOpPlusU32_Safe:
        {
            MK_BINOP32_CAB();
            OP_OVERFLOW(uadd_with_overflow, CreateAdd, TO_PTR_I32, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU32));
            break;
        }
        case ByteCodeOp::BinOpPlusS64:
        case ByteCodeOp::BinOpPlusS64_Safe:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(sadd_with_overflow, CreateAdd, TO_PTR_I64, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS64));
            break;
        }
        case ByteCodeOp::BinOpPlusU64:
        case ByteCodeOp::BinOpPlusU64_Safe:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(uadd_with_overflow, CreateAdd, TO_PTR_I64, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU64));
            break;
        }
        case ByteCodeOp::BinOpPlusF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFAdd(r1, r2);
            builder.CreateStore(v0, TO_PTR_F32(r0));
            break;
        }
        case ByteCodeOp::BinOpPlusF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFAdd(r1, r2);
            builder.CreateStore(v0, TO_PTR_F64(r0));
            break;
        }

        case ByteCodeOp::BinOpMinusS32:
        case ByteCodeOp::BinOpMinusS32_Safe:
        {
            MK_BINOP32_CAB();
            OP_OVERFLOW(ssub_with_overflow, CreateSub, TO_PTR_I32, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS32));
            break;
        }
        case ByteCodeOp::BinOpMinusU32:
        case ByteCodeOp::BinOpMinusU32_Safe:
        {
            MK_BINOP32_CAB();
            OP_OVERFLOW(usub_with_overflow, CreateSub, TO_PTR_I32, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU32));
            break;
        }
        case ByteCodeOp::BinOpMinusS64:
        case ByteCodeOp::BinOpMinusS64_Safe:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(ssub_with_overflow, CreateSub, TO_PTR_I64, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS64));
            break;
        }
        case ByteCodeOp::BinOpMinusU64:
        case ByteCodeOp::BinOpMinusU64_Safe:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(usub_with_overflow, CreateSub, TO_PTR_I64, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU64));
            break;
        }
        case ByteCodeOp::BinOpMinusF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFSub(r1, r2);
            builder.CreateStore(v0, TO_PTR_F32(r0));
            break;
        }
        case ByteCodeOp::BinOpMinusF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFSub(r1, r2);
            builder.CreateStore(v0, TO_PTR_F64(r0));
            break;
        }

        case ByteCodeOp::BinOpMulS32:
        case ByteCodeOp::BinOpMulS32_Safe:
        {
            MK_BINOP32_CAB();
            OP_OVERFLOW(smul_with_overflow, CreateMul, TO_PTR_I32, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS32));
            break;
        }
        case ByteCodeOp::BinOpMulU32:
        case ByteCodeOp::BinOpMulU32_Safe:
        {
            MK_BINOP32_CAB();
            OP_OVERFLOW(umul_with_overflow, CreateMul, TO_PTR_I32, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU32));
            break;
        }
        case ByteCodeOp::BinOpMulS64:
        case ByteCodeOp::BinOpMulS64_Safe:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(smul_with_overflow, CreateMul, TO_PTR_I64, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS64));
            break;
        }
        case ByteCodeOp::BinOpMulU64:
        case ByteCodeOp::BinOpMulU64_Safe:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(umul_with_overflow, CreateMul, TO_PTR_I64, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU64));
            break;
        }
        case ByteCodeOp::BinOpMulF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFMul(r1, r2);
            builder.CreateStore(v0, TO_PTR_F32(r0));
            break;
        }
        case ByteCodeOp::BinOpMulF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFMul(r1, r2);
            builder.CreateStore(v0, TO_PTR_F64(r0));
            break;
        }

        case ByteCodeOp::BinOpXorU8:
        {
            MK_BINOP8_CAB();
            auto v0 = builder.CreateXor(r1, r2);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::BinOpXorU16:
        {
            MK_BINOP16_CAB();
            auto v0 = builder.CreateXor(r1, r2);
            builder.CreateStore(v0, TO_PTR_I16(r0));
            break;
        }
        case ByteCodeOp::BinOpXorU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateXor(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpXorU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateXor(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }

        case ByteCodeOp::BinOpShiftLeftS8:
        case ByteCodeOp::BinOpShiftLeftU8:
            emitShiftLogical(context, builder, allocR, ip, 8, true);
            break;
        case ByteCodeOp::BinOpShiftLeftS16:
        case ByteCodeOp::BinOpShiftLeftU16:
            emitShiftLogical(context, builder, allocR, ip, 16, true);
            break;
        case ByteCodeOp::BinOpShiftLeftS32:
        case ByteCodeOp::BinOpShiftLeftU32:
            emitShiftLogical(context, builder, allocR, ip, 32, true);
            break;
        case ByteCodeOp::BinOpShiftLeftS64:
        case ByteCodeOp::BinOpShiftLeftU64:
            emitShiftLogical(context, builder, allocR, ip, 64, true);
            break;

        case ByteCodeOp::BinOpShiftRightU8:
            emitShiftLogical(context, builder, allocR, ip, 8, false);
            break;
        case ByteCodeOp::BinOpShiftRightU16:
            emitShiftLogical(context, builder, allocR, ip, 16, false);
            break;
        case ByteCodeOp::BinOpShiftRightU32:
            emitShiftLogical(context, builder, allocR, ip, 32, false);
            break;
        case ByteCodeOp::BinOpShiftRightU64:
            emitShiftLogical(context, builder, allocR, ip, 64, false);
            break;

        case ByteCodeOp::BinOpShiftRightS8:
            emitShiftArithmetic(context, builder, allocR, ip, 8);
            break;
        case ByteCodeOp::BinOpShiftRightS16:
            emitShiftArithmetic(context, builder, allocR, ip, 16);
            break;
        case ByteCodeOp::BinOpShiftRightS32:
            emitShiftArithmetic(context, builder, allocR, ip, 32);
            break;
        case ByteCodeOp::BinOpShiftRightS64:
            emitShiftArithmetic(context, builder, allocR, ip, 64);
            break;

        case ByteCodeOp::BinOpModuloS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateSRem(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpModuloS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateSRem(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpModuloU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateURem(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpModuloU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateURem(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }

        case ByteCodeOp::BinOpDivS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateSDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpDivS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateSDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpDivU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateUDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpDivU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateUDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpDivF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_F32(r0));
            break;
        }
        case ByteCodeOp::BinOpDivF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_F64(r0));
            break;
        }

        case ByteCodeOp::BinOpBitmaskAnd8:
        {
            MK_BINOP8_CAB();
            auto v0 = builder.CreateAnd(r1, r2);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::BinOpBitmaskAnd16:
        {
            MK_BINOP16_CAB();
            auto v0 = builder.CreateAnd(r1, r2);
            builder.CreateStore(v0, TO_PTR_I16(r0));
            break;
        }
        case ByteCodeOp::BinOpBitmaskAnd32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateAnd(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpBitmaskAnd64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateAnd(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }

        case ByteCodeOp::BinOpBitmaskOr8:
        {
            MK_BINOP8_CAB();
            auto v0 = builder.CreateOr(r1, r2);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::BinOpBitmaskOr16:
        {
            MK_BINOP16_CAB();
            auto v0 = builder.CreateOr(r1, r2);
            builder.CreateStore(v0, TO_PTR_I16(r0));
            break;
        }
        case ByteCodeOp::BinOpBitmaskOr32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateOr(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpBitmaskOr64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateOr(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::AffectOpMinusEqS8:
        case ByteCodeOp::AffectOpMinusEqS8_Safe:
        {
            MK_BINOPEQ8_CAB();
            OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, getInt8Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS8));
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS8_SSafe:
        case ByteCodeOp::AffectOpMinusEqU8_SSafe:
        {
            MK_BINOPEQ8_SCAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS8_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU8_SSSafe:
        {
            MK_BINOPEQ8_SSCAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMinusEqS16:
        case ByteCodeOp::AffectOpMinusEqS16_Safe:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, getInt16Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS16));
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS16_SSafe:
        case ByteCodeOp::AffectOpMinusEqU16_SSafe:
        {
            MK_BINOPEQ16_SCAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS16_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU16_SSSafe:
        {
            MK_BINOPEQ16_SSCAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMinusEqS32:
        case ByteCodeOp::AffectOpMinusEqS32_Safe:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS32));
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS32_SSafe:
        case ByteCodeOp::AffectOpMinusEqU32_SSafe:
        {
            MK_BINOPEQ32_SCAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS32_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU32_SSSafe:
        {
            MK_BINOPEQ32_SSCAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMinusEqS64:
        case ByteCodeOp::AffectOpMinusEqS64_Safe:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS64));
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS64_SSafe:
        case ByteCodeOp::AffectOpMinusEqU64_SSafe:
        {
            MK_BINOPEQ64_SCAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS64_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU64_SSSafe:
        {
            MK_BINOPEQ64_SSCAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMinusEqU8:
        case ByteCodeOp::AffectOpMinusEqU8_Safe:
        {
            MK_BINOPEQ8_CAB();
            OPEQ_OVERFLOW(usub_with_overflow, CreateSub, getInt8Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU8));
            break;
        }

        case ByteCodeOp::AffectOpMinusEqU16:
        case ByteCodeOp::AffectOpMinusEqU16_Safe:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(usub_with_overflow, CreateSub, getInt16Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16));
            break;
        }

        case ByteCodeOp::AffectOpMinusEqU32:
        case ByteCodeOp::AffectOpMinusEqU32_Safe:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(usub_with_overflow, CreateSub, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU32));
            break;
        }

        case ByteCodeOp::AffectOpMinusEqU64:
        case ByteCodeOp::AffectOpMinusEqU64_Safe:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(usub_with_overflow, CreateSub, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU64));
            break;
        }

        case ByteCodeOp::AffectOpMinusEqF32:
        {
            MK_BINOPEQF32_CAB();
            auto v0 = builder.CreateFSub(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqF32_S:
        {
            MK_BINOPEQF32_SCAB();
            auto v0 = builder.CreateFSub(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqF32_SS:
        {
            MK_BINOPEQF32_SSCAB();
            auto v0 = builder.CreateFSub(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMinusEqF64:
        {
            MK_BINOPEQF64_CAB();
            auto v0 = builder.CreateFSub(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqF64_S:
        {
            MK_BINOPEQF64_SCAB();
            auto v0 = builder.CreateFSub(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqF64_SS:
        {
            MK_BINOPEQF64_SSCAB();
            auto v0 = builder.CreateFSub(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::AffectOpPlusEqS8:
        case ByteCodeOp::AffectOpPlusEqS8_Safe:
        {
            MK_BINOPEQ8_CAB();
            OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, getInt8Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS8));
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS8_SSafe:
        case ByteCodeOp::AffectOpPlusEqU8_SSafe:
        {
            MK_BINOPEQ8_SCAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS8_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU8_SSSafe:
        {
            MK_BINOPEQ8_SSCAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpPlusEqS16:
        case ByteCodeOp::AffectOpPlusEqS16_Safe:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, getInt16Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS16));
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS16_SSafe:
        case ByteCodeOp::AffectOpPlusEqU16_SSafe:
        {
            MK_BINOPEQ16_SCAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS16_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU16_SSSafe:
        {
            MK_BINOPEQ16_SSCAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpPlusEqS32:
        case ByteCodeOp::AffectOpPlusEqS32_Safe:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS32));
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS32_SSafe:
        case ByteCodeOp::AffectOpPlusEqU32_SSafe:
        {
            MK_BINOPEQ32_SCAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS32_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU32_SSSafe:
        {
            MK_BINOPEQ32_SSCAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpPlusEqS64:
        case ByteCodeOp::AffectOpPlusEqS64_Safe:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS64));
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS64_SSafe:
        case ByteCodeOp::AffectOpPlusEqU64_SSafe:
        {
            MK_BINOPEQ64_SCAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS64_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU64_SSSafe:
        {
            MK_BINOPEQ64_SSCAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpPlusEqU8:
        case ByteCodeOp::AffectOpPlusEqU8_Safe:
        {
            MK_BINOPEQ8_CAB();
            OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, getInt8Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU8));
            break;
        }

        case ByteCodeOp::AffectOpPlusEqU16:
        case ByteCodeOp::AffectOpPlusEqU16_Safe:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, getInt16Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU16));
            break;
        }

        case ByteCodeOp::AffectOpPlusEqU32:
        case ByteCodeOp::AffectOpPlusEqU32_Safe:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU32));
            break;
        }

        case ByteCodeOp::AffectOpPlusEqU64:
        case ByteCodeOp::AffectOpPlusEqU64_Safe:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU64));
            break;
        }

        case ByteCodeOp::AffectOpPlusEqF32:
        {
            MK_BINOPEQF32_CAB();
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqF32_S:
        {
            MK_BINOPEQF32_SCAB();
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqF32_SS:
        {
            MK_BINOPEQF32_SSCAB();
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpPlusEqF64:
        {
            MK_BINOPEQF64_CAB();
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqF64_S:
        {
            MK_BINOPEQF64_SCAB();
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqF64_SS:
        {
            MK_BINOPEQF64_SSCAB();
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::AffectOpMulEqS8:
        case ByteCodeOp::AffectOpMulEqS8_Safe:
        {
            MK_BINOPEQ8_CAB();
            OPEQ_OVERFLOW(smul_with_overflow, CreateMul, getInt8Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS8));
            break;
        }
        case ByteCodeOp::AffectOpMulEqS8_SSafe:
        case ByteCodeOp::AffectOpMulEqU8_SSafe:
        {
            MK_BINOPEQ8_SCAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMulEqS8_SSSafe:
        case ByteCodeOp::AffectOpMulEqU8_SSSafe:
        {
            MK_BINOPEQ8_SSCAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMulEqS16:
        case ByteCodeOp::AffectOpMulEqS16_Safe:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(smul_with_overflow, CreateMul, getInt16Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS16));
            break;
        }
        case ByteCodeOp::AffectOpMulEqS16_SSafe:
        case ByteCodeOp::AffectOpMulEqU16_SSafe:
        {
            MK_BINOPEQ16_SCAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMulEqS16_SSSafe:
        case ByteCodeOp::AffectOpMulEqU16_SSSafe:
        {
            MK_BINOPEQ16_SSCAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMulEqS32:
        case ByteCodeOp::AffectOpMulEqS32_Safe:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(smul_with_overflow, CreateMul, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS32));
            break;
        }
        case ByteCodeOp::AffectOpMulEqS32_SSafe:
        case ByteCodeOp::AffectOpMulEqU32_SSafe:
        {
            MK_BINOPEQ32_SCAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMulEqS32_SSSafe:
        case ByteCodeOp::AffectOpMulEqU32_SSSafe:
        {
            MK_BINOPEQ32_SSCAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMulEqS64:
        case ByteCodeOp::AffectOpMulEqS64_Safe:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(smul_with_overflow, CreateMul, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS64));
            break;
        }
        case ByteCodeOp::AffectOpMulEqS64_SSafe:
        case ByteCodeOp::AffectOpMulEqU64_SSafe:
        {
            MK_BINOPEQ64_SCAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMulEqS64_SSSafe:
        case ByteCodeOp::AffectOpMulEqU64_SSSafe:
        {
            MK_BINOPEQ64_SSCAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMulEqU8:
        case ByteCodeOp::AffectOpMulEqU8_Safe:
        {
            MK_BINOPEQ8_CAB();
            OPEQ_OVERFLOW(umul_with_overflow, CreateMul, getInt8Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU8));
            break;
        }

        case ByteCodeOp::AffectOpMulEqU16:
        case ByteCodeOp::AffectOpMulEqU16_Safe:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(umul_with_overflow, CreateMul, getInt16Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU16));
            break;
        }

        case ByteCodeOp::AffectOpMulEqU32:
        case ByteCodeOp::AffectOpMulEqU32_Safe:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(umul_with_overflow, CreateMul, getInt32Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU32));
            break;
        }

        case ByteCodeOp::AffectOpMulEqU64:
        case ByteCodeOp::AffectOpMulEqU64_Safe:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(umul_with_overflow, CreateMul, getInt64Ty(), ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU64));
            break;
        }

        case ByteCodeOp::AffectOpMulEqF32:
        {
            MK_BINOPEQF32_CAB();
            auto v0 = builder.CreateFMul(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqF32_S:
        {
            MK_BINOPEQF32_SCAB();
            auto v0 = builder.CreateFMul(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMulEqF32_SS:
        {
            MK_BINOPEQF32_SSCAB();
            auto v0 = builder.CreateFMul(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMulEqF64:
        {
            MK_BINOPEQF64_CAB();
            auto v0 = builder.CreateFMul(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqF64_S:
        {
            MK_BINOPEQF64_SCAB();
            auto v0 = builder.CreateFMul(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpMulEqF64_SS:
        {
            MK_BINOPEQF64_SSCAB();
            auto v0 = builder.CreateFMul(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::AffectOpDivEqS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS8_S:
        {
            MK_BINOPEQ8_SCAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS8_SS:
        {
            MK_BINOPEQ8_SSCAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpDivEqS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS16_S:
        {
            MK_BINOPEQ16_SCAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS16_SS:
        {
            MK_BINOPEQ16_SSCAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpDivEqS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS32_S:
        {
            MK_BINOPEQ32_SCAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS32_SS:
        {
            MK_BINOPEQ32_SSCAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpDivEqS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS64_S:
        {
            MK_BINOPEQ64_SCAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS64_SS:
        {
            MK_BINOPEQ64_SSCAB();
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpDivEqU8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU8_S:
        {
            MK_BINOPEQ8_SCAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU8_SS:
        {
            MK_BINOPEQ8_SSCAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpDivEqU16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU16_S:
        {
            MK_BINOPEQ16_SCAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU16_SS:
        {
            MK_BINOPEQ16_SSCAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpDivEqU32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU32_S:
        {
            MK_BINOPEQ32_SCAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU32_SS:
        {
            MK_BINOPEQ32_SSCAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpDivEqU64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU64_S:
        {
            MK_BINOPEQ64_SCAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU64_SS:
        {
            MK_BINOPEQ64_SSCAB();
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpDivEqF32:
        {
            MK_BINOPEQF32_CAB();
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqF32_S:
        {
            MK_BINOPEQF32_SCAB();
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpDivEqF32_SS:
        {
            MK_BINOPEQF32_SSCAB();
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpDivEqF64:
        {
            MK_BINOPEQF64_CAB();
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqF64_S:
        {
            MK_BINOPEQF64_SCAB();
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpDivEqF64_SS:
        {
            MK_BINOPEQF64_SSCAB();
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::AffectOpModuloEqS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS8_S:
        {
            MK_BINOPEQ8_SCAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS8_SS:
        {
            MK_BINOPEQ8_SSCAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpModuloEqS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS16_S:
        {
            MK_BINOPEQ16_SCAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS16_SS:
        {
            MK_BINOPEQ16_SSCAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpModuloEqS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS32_S:
        {
            MK_BINOPEQ32_SCAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS32_SS:
        {
            MK_BINOPEQ32_SSCAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpModuloEqS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS64_S:
        {
            MK_BINOPEQ64_SCAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS64_SS:
        {
            MK_BINOPEQ64_SSCAB();
            auto v0 = builder.CreateSRem(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpModuloEqU8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU8_S:
        {
            MK_BINOPEQ8_SCAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU8_SS:
        {
            MK_BINOPEQ8_SSCAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpModuloEqU16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU16_S:
        {
            MK_BINOPEQ16_SCAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU16_SS:
        {
            MK_BINOPEQ16_SSCAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpModuloEqU32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU32_S:
        {
            MK_BINOPEQ32_SCAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU32_SS:
        {
            MK_BINOPEQ32_SSCAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpModuloEqU64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU64_S:
        {
            MK_BINOPEQ64_SCAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r0), r1);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU64_SS:
        {
            MK_BINOPEQ64_SSCAB();
            auto v0 = builder.CreateURem(builder.CreateLoad(r0), builder.CreateLoad(r1));
            builder.CreateStore(v0, r0);
            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::AffectOpAndEqU8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpAndEqU16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpAndEqU32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpAndEqU64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::AffectOpOrEqU8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpOrEqU16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpOrEqU32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpOrEqU64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::AffectOpShiftLeftEqS8:
        case ByteCodeOp::AffectOpShiftLeftEqU8:
            emitShiftEqLogical(context, builder, allocR, ip, 8, true);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS16:
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            emitShiftEqLogical(context, builder, allocR, ip, 16, true);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS32:
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            emitShiftEqLogical(context, builder, allocR, ip, 32, true);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS64:
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            emitShiftEqLogical(context, builder, allocR, ip, 64, true);
            break;

            /////////////////////////////////////

        case ByteCodeOp::AffectOpShiftRightEqU8:
            emitShiftEqLogical(context, builder, allocR, ip, 8, false);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            emitShiftEqLogical(context, builder, allocR, ip, 16, false);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            emitShiftEqLogical(context, builder, allocR, ip, 32, false);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            emitShiftEqLogical(context, builder, allocR, ip, 64, false);
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            emitShiftEqArithmetic(context, builder, allocR, ip, 8);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            emitShiftEqArithmetic(context, builder, allocR, ip, 16);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            emitShiftEqArithmetic(context, builder, allocR, ip, 32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            emitShiftEqArithmetic(context, builder, allocR, ip, 64);
            break;

            /////////////////////////////////////

        case ByteCodeOp::AffectOpXorEqU8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpXorEqU16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpXorEqU32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpXorEqU64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::CompareOpGreaterS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpSGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpSGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpUGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpUGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFCmpUGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFCmpUGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CompareOpGreaterEqS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpSGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterEqS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpSGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterEqU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpUGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterEqU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpUGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterEqF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFCmpUGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterEqF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFCmpUGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CompareOpLowerS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpSLT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpSLT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpULT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpULT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFCmpULT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFCmpULT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CompareOpLowerEqS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpSLE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerEqS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpSLE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerEqU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpULE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerEqU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpULE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerEqF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFCmpULE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerEqF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFCmpULE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CompareOp3WayS32:
        case ByteCodeOp::CompareOp3WayU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateSub(r1, r2);
            auto v1 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cst0_i32), builder.getInt32Ty(), false);
            auto v2 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cst0_i32), builder.getInt32Ty(), false);
            auto v3 = builder.CreateSub(v1, v2);
            builder.CreateStore(v3, TO_PTR_I32(r0));
            break;
        }

        case ByteCodeOp::CompareOp3WayU64:
        case ByteCodeOp::CompareOp3WayS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateSub(r1, r2);
            auto v1 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cst0_i64), builder.getInt32Ty(), false);
            auto v2 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cst0_i64), builder.getInt32Ty(), false);
            auto v3 = builder.CreateSub(v1, v2);
            builder.CreateStore(v3, TO_PTR_I32(r0));
            break;
        }

        case ByteCodeOp::CompareOp3WayF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFSub(r1, r2);
            auto v1 = builder.CreateIntCast(builder.CreateFCmpUGT(v0, pp.cst0_f32), builder.getInt32Ty(), false);
            auto v2 = builder.CreateIntCast(builder.CreateFCmpULT(v0, pp.cst0_f32), builder.getInt32Ty(), false);
            auto v3 = builder.CreateSub(v1, v2);
            builder.CreateStore(v3, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::CompareOp3WayF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFSub(r1, r2);
            auto v1 = builder.CreateIntCast(builder.CreateFCmpUGT(v0, pp.cst0_f64), builder.getInt32Ty(), false);
            auto v2 = builder.CreateIntCast(builder.CreateFCmpULT(v0, pp.cst0_f64), builder.getInt32Ty(), false);
            auto v3 = builder.CreateSub(v1, v2);
            builder.CreateStore(v3, TO_PTR_I32(r0));
            break;
        }

        case ByteCodeOp::CompareOpEqual8:
        {
            MK_BINOP8_CAB();
            auto v0 = builder.CreateICmpEQ(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqual16:
        {
            MK_BINOP16_CAB();
            auto v0 = builder.CreateICmpEQ(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqual32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpEQ(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqual64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpEQ(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqualF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFCmpOEQ(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqualF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFCmpOEQ(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CompareOpNotEqual8:
        {
            MK_BINOP8_CAB();
            auto v0 = builder.CreateICmpNE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpNotEqual16:
        {
            MK_BINOP16_CAB();
            auto v0 = builder.CreateICmpNE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpNotEqual32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpNE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpNotEqual64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpNE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpNotEqualF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFCmpUNE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpNotEqualF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFCmpUNE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::TestNotZero8:
        {
            auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt8(ip->b.u8);
            else
                r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto b0 = builder.CreateIsNotNull(r1);
            builder.CreateStore(builder.CreateIntCast(b0, builder.getInt8Ty(), false), r0);
            break;
        }
        case ByteCodeOp::TestNotZero16:
        {
            auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt16(ip->b.u16);
            else
                r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
            auto b0 = builder.CreateIsNotNull(r1);
            builder.CreateStore(builder.CreateIntCast(b0, builder.getInt8Ty(), false), r0);
            break;
        }
        case ByteCodeOp::TestNotZero32:
        {
            auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt32(ip->b.u32);
            else
                r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            auto b0 = builder.CreateIsNotNull(r1);
            builder.CreateStore(builder.CreateIntCast(b0, builder.getInt8Ty(), false), r0);
            break;
        }
        case ByteCodeOp::TestNotZero64:
        {
            auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt64(ip->b.u64);
            else
                r1 = builder.CreateLoad(TO_PTR_I64(GEP_I32(allocR, ip->b.u32)));
            auto b0 = builder.CreateIsNotNull(r1);
            builder.CreateStore(builder.CreateIntCast(b0, builder.getInt8Ty(), false), r0);
            break;
        }

        case ByteCodeOp::JumpDyn8:
        case ByteCodeOp::JumpDyn16:
        case ByteCodeOp::JumpDyn64:
        case ByteCodeOp::JumpDyn32:
        {
            int32_t* tableCompiler = (int32_t*) moduleToGen->compilerSegment.address(ip->d.u32);

            VectorNative<llvm::BasicBlock*> falseBlocks;
            VectorNative<llvm::BasicBlock*> trueBlocks;

            for (uint32_t idx = 0; idx < ip->c.u32; idx++)
            {
                if (idx == 0)
                    falseBlocks.push_back(nullptr);
                else
                    falseBlocks.push_back(getOrCreateLabel(pp, func, (int64_t) UINT32_MAX + g_UniqueID.fetch_add(1)));
                trueBlocks.push_back(getOrCreateLabel(pp, func, i + (int64_t) tableCompiler[idx] + 1));
            }

            llvm::Value* r0 = nullptr;
            llvm::Value* r1 = nullptr;
            switch (ip->op)
            {
            case ByteCodeOp::JumpDyn8:
                r0 = MK_IMMA_8();
                r1 = builder.getInt8(ip->b.u8);
                break;
            case ByteCodeOp::JumpDyn16:
                r0 = MK_IMMA_16();
                r1 = builder.getInt16(ip->b.u16);
                break;
            case ByteCodeOp::JumpDyn32:
                r0 = MK_IMMA_32();
                r1 = builder.getInt32(ip->b.u32);
                break;
            case ByteCodeOp::JumpDyn64:
                r0 = MK_IMMA_64();
                r1 = builder.getInt64(ip->b.u64);
                break;
            }

            auto b0 = builder.CreateICmpEQ(r0, r1);
            builder.CreateCondBr(b0, trueBlocks[1], falseBlocks[1]);
            builder.SetInsertPoint(falseBlocks[1]);

            for (uint32_t idx = 2; idx < ip->c.u32; idx++)
            {
                switch (ip->op)
                {
                case ByteCodeOp::JumpDyn8:
                    r1 = builder.getInt8((uint8_t) (idx - 1) + ip->b.u8);
                    break;
                case ByteCodeOp::JumpDyn16:
                    r1 = builder.getInt16((uint16_t) (idx - 1) + ip->b.u16);
                    break;
                case ByteCodeOp::JumpDyn32:
                    r1 = builder.getInt32((uint32_t) (idx - 1) + ip->b.u32);
                    break;
                case ByteCodeOp::JumpDyn64:
                    r1 = builder.getInt64((uint64_t) (idx - 1) + ip->b.u64);
                    break;
                }

                b0 = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(b0, trueBlocks[idx], falseBlocks[idx]);
                builder.SetInsertPoint(falseBlocks[idx]);
            }

            builder.CreateBr(trueBlocks[0]);
            blockIsClosed = true;
            break;
        }

        case ByteCodeOp::Jump:
        {
            auto label = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            builder.CreateBr(label);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfFalse:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfTrue:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfZero8:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfZero16:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfZero32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfZero64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = GEP_I32(allocR, ip->a.u32);
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotZero8:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotZero16:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotZero32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotZero64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = GEP_I32(allocR, ip->a.u32);
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotEqual8:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_8();
            auto r1         = MK_IMMC_8();
            auto b0         = builder.CreateICmpNE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotEqual16:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_16();
            auto r1         = MK_IMMC_16();
            auto b0         = builder.CreateICmpNE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotEqual32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_32();
            auto r1         = MK_IMMC_32();
            auto b0         = builder.CreateICmpNE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotEqual64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_64();
            auto r1         = MK_IMMC_64();
            auto b0         = builder.CreateICmpNE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotEqualF32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F32();
            auto r1         = MK_IMMC_F32();
            auto b0         = builder.CreateFCmpUNE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotEqualF64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F64();
            auto r1         = MK_IMMC_F64();
            auto b0         = builder.CreateFCmpUNE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfEqual8:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_8();
            auto r1         = MK_IMMC_8();
            auto b0         = builder.CreateICmpEQ(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfEqual16:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_16();
            auto r1         = MK_IMMC_16();
            auto b0         = builder.CreateICmpEQ(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfEqual32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_32();
            auto r1         = MK_IMMC_32();
            auto b0         = builder.CreateICmpEQ(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfEqual64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_64();
            auto r1         = MK_IMMC_64();
            auto b0         = builder.CreateICmpEQ(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfEqualF32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F32();
            auto r1         = MK_IMMC_F32();
            auto b0         = builder.CreateFCmpOEQ(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfEqualF64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F64();
            auto r1         = MK_IMMC_F64();
            auto b0         = builder.CreateFCmpOEQ(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::IncJumpIfEqual64:
        {
            {
                auto r0 = GEP_I32(allocR, ip->a.u32);
                auto v0 = builder.CreateAdd(builder.CreateLoad(r0), pp.cst1_i64);
                builder.CreateStore(v0, r0);
            }
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_64();
                auto r1         = MK_IMMC_64();
                auto b0         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
            }
            break;
        }

        case ByteCodeOp::JumpIfLowerS32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_32();
            auto r1         = MK_IMMC_32();
            auto b0         = builder.CreateICmpSLT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfLowerS64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_64();
            auto r1         = MK_IMMC_64();
            auto b0         = builder.CreateICmpSLT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfLowerU32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_32();
            auto r1         = MK_IMMC_32();
            auto b0         = builder.CreateICmpULT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfLowerU64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_64();
            auto r1         = MK_IMMC_64();
            auto b0         = builder.CreateICmpULT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfLowerF32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F32();
            auto r1         = MK_IMMC_F32();
            auto b0         = builder.CreateFCmpULT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfLowerF64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F64();
            auto r1         = MK_IMMC_F64();
            auto b0         = builder.CreateFCmpULT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }

        case ByteCodeOp::JumpIfLowerEqS32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_32();
            auto r1         = MK_IMMC_32();
            auto b0         = builder.CreateICmpSLE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfLowerEqS64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_64();
            auto r1         = MK_IMMC_64();
            auto b0         = builder.CreateICmpSLE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfLowerEqU32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_32();
            auto r1         = MK_IMMC_32();
            auto b0         = builder.CreateICmpULE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfLowerEqU64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_64();
            auto r1         = MK_IMMC_64();
            auto b0         = builder.CreateICmpULE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfLowerEqF32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F32();
            auto r1         = MK_IMMC_F32();
            auto b0         = builder.CreateFCmpULE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfLowerEqF64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F64();
            auto r1         = MK_IMMC_F64();
            auto b0         = builder.CreateFCmpULE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }

        case ByteCodeOp::JumpIfGreaterEqS32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_32();
            auto r1         = MK_IMMC_32();
            auto b0         = builder.CreateICmpSGE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfGreaterEqS64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_64();
            auto r1         = MK_IMMC_64();
            auto b0         = builder.CreateICmpSGE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfGreaterEqU32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_32();
            auto r1         = MK_IMMC_32();
            auto b0         = builder.CreateICmpUGE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfGreaterEqU64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_64();
            auto r1         = MK_IMMC_64();
            auto b0         = builder.CreateICmpUGE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfGreaterEqF32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F32();
            auto r1         = MK_IMMC_F32();
            auto b0         = builder.CreateFCmpUGE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfGreaterEqF64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F64();
            auto r1         = MK_IMMC_F64();
            auto b0         = builder.CreateFCmpUGE(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }

        case ByteCodeOp::JumpIfGreaterS32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_32();
            auto r1         = MK_IMMC_32();
            auto b0         = builder.CreateICmpSGT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfGreaterS64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_64();
            auto r1         = MK_IMMC_64();
            auto b0         = builder.CreateICmpSGT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfGreaterU32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_32();
            auto r1         = MK_IMMC_32();
            auto b0         = builder.CreateICmpUGT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfGreaterU64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_64();
            auto r1         = MK_IMMC_64();
            auto b0         = builder.CreateICmpUGT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfGreaterF32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F32();
            auto r1         = MK_IMMC_F32();
            auto b0         = builder.CreateFCmpUGT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfGreaterF64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = MK_IMMA_F64();
            auto r1         = MK_IMMC_F64();
            auto b0         = builder.CreateFCmpUGT(r0, r1);
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }

        case ByteCodeOp::CopyRCtoRRRet:
            getReturnResult(context, buildParameters, moduleToGen, returnType, ip->flags & BCI_IMM_B, ip->b, allocR, allocResult);
        case ByteCodeOp::Ret:
        {
            // :OptimizedAwayDebugCrap
            // Hack thanks to llvm in debug mode : we need to force the usage of function parameters until the very end of the function (i.e. each return),
            // otherwhise :
            // - parameters not used in the function body will be removed by llvm (even without optims activated !)
            // - a parameter will not be visible anymore ("optimized away") after its last usage.
            // So we force a read/write of each parameter just before the "ret" to avoid that debug mess.
            // RIDICULOUS !!
            //
            // If we request an optimized code, do not do that crap.
            bool isDebug = !buildParameters.buildCfg->backendOptimizeSpeed && !buildParameters.buildCfg->backendOptimizeSize;
            if (isDebug && buildParameters.buildCfg->backendDebugInformations)
            {
                auto r1        = GEP_I32(allocT, 0);
                auto numParams = typeFunc->parameters.size();
                if (typeFunc->flags & TYPEINFO_C_VARIADIC)
                    numParams--;
                for (int iparam = 0; iparam < numParams; iparam++)
                {
                    SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, iparam, r1));
                }
            }

            // Emit result
            if (returnType != g_TypeMgr->typeInfoVoid && !typeFunc->returnByCopy())
            {
                if (returnType->isNative())
                {
                    switch (returnType->nativeType)
                    {
                    case NativeTypeKind::U8:
                    case NativeTypeKind::S8:
                    case NativeTypeKind::Bool:
                        builder.CreateRet(builder.CreateLoad(TO_PTR_I8(allocResult)));
                        break;
                    case NativeTypeKind::U16:
                    case NativeTypeKind::S16:
                        builder.CreateRet(builder.CreateLoad(TO_PTR_I16(allocResult)));
                        break;
                    case NativeTypeKind::U32:
                    case NativeTypeKind::S32:
                    case NativeTypeKind::Rune:
                        builder.CreateRet(builder.CreateLoad(TO_PTR_I32(allocResult)));
                        break;
                    case NativeTypeKind::U64:
                    case NativeTypeKind::S64:
                        builder.CreateRet(builder.CreateLoad(TO_PTR_I64(allocResult)));
                        break;
                    case NativeTypeKind::F32:
                        builder.CreateRet(builder.CreateLoad(TO_PTR_F32(allocResult)));
                        break;
                    case NativeTypeKind::F64:
                        builder.CreateRet(builder.CreateLoad(TO_PTR_F64(allocResult)));
                        break;
                    default:
                        SWAG_ASSERT(false);
                        break;
                    }
                }
                else if (returnType->isPointer() || returnType->isLambdaClosure())
                {
                    auto llvmType = swagTypeToLLVMType(buildParameters, moduleToGen, returnType);
                    auto ptr      = builder.CreatePointerCast(allocResult, llvmType->getPointerTo());
                    builder.CreateRet(builder.CreateLoad(ptr));
                }
                else
                {
                    SWAG_ASSERT(false);
                }
            }
            else
                builder.CreateRetVoid();

            blockIsClosed = true;
            break;
        }

        case ByteCodeOp::IntrinsicCompilerError:
        {
            auto bcF = ((AstFuncDecl*) ip->node->resolvedSymbolOverload->node)->extension->bytecode->bc;
            emitCall(buildParameters, moduleToGen, bcF->getCallName().c_str(), allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
            break;
        }
        case ByteCodeOp::IntrinsicCompilerWarning:
        {
            auto bcF = ((AstFuncDecl*) ip->node->resolvedSymbolOverload->node)->extension->bytecode->bc;
            emitCall(buildParameters, moduleToGen, bcF->getCallName().c_str(), allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
            break;
        }

        case ByteCodeOp::IntrinsicPanic:
            emitCall(buildParameters, moduleToGen, g_LangSpec->name_atpanic, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
            break;

        case ByteCodeOp::InternalInitStackTrace:
            emitCall(buildParameters, moduleToGen, g_LangSpec->name__initStackTrace, allocR, allocT, {}, {});
            break;
        case ByteCodeOp::InternalStackTrace:
            emitCall(buildParameters, moduleToGen, g_LangSpec->name__stackTrace, allocR, allocT, {ip->a.u32}, {});
            break;
        case ByteCodeOp::InternalPanic:
            emitInternalPanic(buildParameters, moduleToGen, allocR, allocT, ip->node, (const char*) ip->d.pointer);
            break;

        case ByteCodeOp::InternalGetTlsPtr:
        {
            auto v0     = builder.getInt64(module->tlsSegment.totalCount);
            auto r1     = builder.CreateInBoundsGEP(TO_PTR_I8(pp.tlsSeg), pp.cst0_i64);
            auto vid    = builder.CreateLoad(pp.symTls_threadLocalId);
            auto result = emitCall(buildParameters, moduleToGen, g_LangSpec->name__tlsGetPtr, allocR, allocT, {UINT32_MAX, UINT32_MAX, UINT32_MAX}, {vid, v0, r1});
            builder.CreateStore(result, TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            break;
        }

        case ByteCodeOp::IntrinsicGetContext:
        {
            auto v0     = builder.CreateLoad(TO_PTR_I64(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst2_i32})));
            auto result = emitCall(buildParameters, moduleToGen, g_LangSpec->name__tlsGetValue, allocR, allocT, {UINT32_MAX}, {v0});
            builder.CreateStore(builder.CreatePtrToInt(result, builder.getInt64Ty()), GEP_I32(allocR, ip->a.u32));
            break;
        }
        case ByteCodeOp::IntrinsicSetContext:
        {
            auto v0 = builder.CreateLoad(TO_PTR_I64(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst2_i32})));
            emitCall(buildParameters, moduleToGen, g_LangSpec->name__tlsSetValue, allocR, allocT, {UINT32_MAX, ip->a.u32}, {v0, 0});
            break;
        }
        case ByteCodeOp::IntrinsicGetProcessInfos:
        {
            auto v0 = TO_PTR_I8(builder.CreateInBoundsGEP(pp.processInfos, pp.cst0_i64));
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::IntrinsicCVaStart:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            builder.CreateIntrinsic(llvm::Intrinsic::vastart, {}, {r0});
            break;
        }
        case ByteCodeOp::IntrinsicCVaEnd:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            builder.CreateIntrinsic(llvm::Intrinsic::vaend, {}, {r0});
            break;
        }
        case ByteCodeOp::IntrinsicCVaArg:
        {
            auto         r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            llvm::Value* v0 = builder.CreateVAArg(r0, builder.getInt64Ty());
            switch (ip->c.u32)
            {
            case 1:
                v0 = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
                builder.CreateStore(v0, TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
                break;
            case 2:
                v0 = builder.CreateIntCast(v0, builder.getInt16Ty(), false);
                builder.CreateStore(v0, TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
                break;
            case 4:
                v0 = builder.CreateIntCast(v0, builder.getInt32Ty(), false);
                builder.CreateStore(v0, TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                break;
            case 8:
                builder.CreateStore(v0, GEP_I32(allocR, ip->b.u32));
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicArguments:
        {
            emitCall(buildParameters, moduleToGen, g_LangSpec->name_atargs, allocR, allocT, {}, {});
            storeRT2ToRegisters(context, buildParameters, ip->a.u32, ip->b.u32, allocR, allocT);
            break;
        }

        case ByteCodeOp::IntrinsicGetErr:
        {
            emitCall(buildParameters, moduleToGen, g_LangSpec->name__geterr, allocR, allocT, {}, {});
            storeRT2ToRegisters(context, buildParameters, ip->a.u32, ip->b.u32, allocR, allocT);
            break;
        }
        case ByteCodeOp::InternalSetErr:
        {
            emitCall(buildParameters, moduleToGen, g_LangSpec->name__seterr, allocR, allocT, {ip->a.u32, ip->b.u32}, {});
            break;
        }
        case ByteCodeOp::InternalCheckAny:
        {
            emitCall(buildParameters, moduleToGen, g_LangSpec->name__checkAny, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
            break;
        }

        case ByteCodeOp::InternalHasErr:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), builder.getInt32(offsetof(SwagContext, errorMsgLen)));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(builder.CreateLoad(TO_PTR_I32(v0)), r1);
            break;
        }
        case ByteCodeOp::InternalClearErr:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), builder.getInt32(offsetof(SwagContext, errorMsgLen)));
            builder.CreateStore(pp.cst0_i32, TO_PTR_I32(v0));
            break;
        }
        case ByteCodeOp::InternalPushErr:
        {
            emitCall(buildParameters, moduleToGen, g_LangSpec->name__pusherr, allocR, allocT, {}, {});
            break;
        }
        case ByteCodeOp::InternalPopErr:
        {
            emitCall(buildParameters, moduleToGen, g_LangSpec->name__poperr, allocR, allocT, {}, {});
            break;
        }

        case ByteCodeOp::IntrinsicIsByteCode:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(pp.cst0_i8, r0);
            break;
        }
        case ByteCodeOp::IntrinsicCompiler:
        {
            auto r0 = TO_PTR_I64(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(pp.cst0_i64, r0);
            auto r1 = TO_PTR_I64(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(pp.cst0_i64, r1);
            break;
        }
        case ByteCodeOp::IntrinsicModules:
        {
            if (moduleToGen->modulesSliceOffset == UINT32_MAX)
            {
                auto r0 = TO_PTR_I64(GEP_I32(allocR, ip->a.u32));
                builder.CreateStore(pp.cst0_i64, r0);
                auto r1 = TO_PTR_I64(GEP_I32(allocR, ip->b.u32));
                builder.CreateStore(pp.cst0_i64, r1);
            }
            else
            {
                auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
                auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.constantSeg), builder.getInt32(moduleToGen->modulesSliceOffset));
                builder.CreateStore(r1, r0);
                auto r2 = GEP_I32(allocR, ip->b.u32);
                builder.CreateStore(builder.getInt64(moduleToGen->moduleDependencies.count + 1), r2);
            }
            break;
        }
        case ByteCodeOp::IntrinsicGvtd:
        {
            if (moduleToGen->globalVarsToDropSliceOffset == UINT32_MAX)
            {
                auto r0 = TO_PTR_I64(GEP_I32(allocR, ip->a.u32));
                builder.CreateStore(pp.cst0_i64, r0);
                auto r1 = TO_PTR_I64(GEP_I32(allocR, ip->b.u32));
                builder.CreateStore(pp.cst0_i64, r1);
            }
            else
            {
                auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
                auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.mutableSeg), builder.getInt32(moduleToGen->globalVarsToDropSliceOffset));
                builder.CreateStore(r1, r0);
                auto r2 = GEP_I32(allocR, ip->b.u32);
                builder.CreateStore(builder.getInt64(moduleToGen->globalVarsToDrop.count), r2);
            }
            break;
        }

        case ByteCodeOp::NegBool:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), pp.cst1_i8);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegF32:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateFNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegF64:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateFNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegS32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegS64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::InvertU8:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertU16:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertU32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertU64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::ClearMaskU32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateAnd(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::ClearMaskU64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateAnd(builder.CreateLoad(r0), CST_RB64);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CastBool8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I8(r1)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastBool16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I16(r1)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastBool32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I32(r1)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastBool64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I64(r1)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }

        case ByteCodeOp::CastS8S16:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt16Ty(), true);
            r0      = TO_PTR_I16(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS8S32:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt32Ty(), true);
            r0      = TO_PTR_I32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS16S32:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt32Ty(), true);
            r0      = TO_PTR_I32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF32S32:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(r0), builder.getInt32Ty());
            r0      = TO_PTR_I32(r0);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CastS8S64:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt64Ty(), true);
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS16S64:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt64Ty(), true);
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS32S64:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt64Ty(), true);
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF64S64:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(r0), builder.getInt64Ty());
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CastS8F32:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS16F32:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS32F32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS64F32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU8F32:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU16F32:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU32F32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU64F32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF64F32:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPTrunc, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CastS8F64:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS16F64:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS32F64:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS64F64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU8F64:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU16F64:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU32F64:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU64F64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF32F64:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPExt, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CopyRCtoRR:
        {
            getReturnResult(context, buildParameters, moduleToGen, returnType, ip->flags & BCI_IMM_A, ip->a, allocR, allocResult);
            break;
        }
        case ByteCodeOp::CopyRCtoRR2:
        {
            auto r1        = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
            auto r2        = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            auto resultPtr = builder.CreatePointerCast(func->getArg((int) func->arg_size() - 1), builder.getInt64Ty()->getPointerTo());
            builder.CreateStore(r1, resultPtr);
            builder.CreateStore(r2, builder.CreateInBoundsGEP(resultPtr, builder.getInt64(1)));
            break;
        }
        case ByteCodeOp::CopyRCtoRT:
        {
            auto r0 = GEP_I32(allocRR, 0);
            auto r1 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::CopyRRtoRC:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(func->getArg((int) func->arg_size() - 1));
            if (ip->b.u64)
                r1 = builder.CreateInBoundsGEP(r1, builder.getInt64(ip->b.u64));
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::CopyRTtoRC:
        {
            if (resultFuncCall)
                storeTypedValueToRegister(context, buildParameters, resultFuncCall, ip->a.u32, allocR);
            else
            {
                auto r0 = GEP_I32(allocR, ip->a.u32);
                auto r1 = builder.CreateLoad(GEP_I32(allocRR, 0));
                builder.CreateStore(r1, r0);
            }
            break;
        }
        case ByteCodeOp::CopyRTtoRC2:
        {
            storeRT2ToRegisters(context, buildParameters, ip->a.u32, ip->b.u32, allocR, allocRR);
            break;
        }

        case ByteCodeOp::GetParam8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 1));
            break;
        }
        case ByteCodeOp::GetParam16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 2));
            break;
        }
        case ByteCodeOp::GetParam32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 4));
            break;
        }
        case ByteCodeOp::GetParam64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0));
            break;
        }
        case ByteCodeOp::GetIncParam64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 0, ip->d.u64));
            break;
        }

        case ByteCodeOp::GetParam64DeRef8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 0, 0, 1));
            break;
        }
        case ByteCodeOp::GetParam64DeRef16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 0, 0, 2));
            break;
        }
        case ByteCodeOp::GetParam64DeRef32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 0, 0, 4));
            break;
        }
        case ByteCodeOp::GetParam64DeRef64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 0, 0, 8));
            break;
        }

        case ByteCodeOp::GetIncParam64DeRef8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 0, ip->d.u64, 1));
            break;
        }
        case ByteCodeOp::GetIncParam64DeRef16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 0, ip->d.u64, 2));
            break;
        }
        case ByteCodeOp::GetIncParam64DeRef32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 0, ip->d.u64, 4));
            break;
        }
        case ByteCodeOp::GetIncParam64DeRef64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, r0, 0, ip->d.u64, 8));
            break;
        }

        case ByteCodeOp::ZeroToTrue:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpEQ(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::LowerZeroToTrue:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::LowerEqZeroToTrue:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSLE(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::GreaterZeroToTrue:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::GreaterEqZeroToTrue:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSGE(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }

        case ByteCodeOp::PushRVParam:
            pushRVParams.push_back({ip->a.u32, ip->b.u32});
            break;
        case ByteCodeOp::PushRAParam:
        case ByteCodeOp::PushRAParamCond:
            pushRAParams.push_back(ip->a.u32);
            break;
        case ByteCodeOp::PushRAParam2:
            pushRAParams.push_back(ip->a.u32);
            pushRAParams.push_back(ip->b.u32);
            break;
        case ByteCodeOp::PushRAParam3:
            pushRAParams.push_back(ip->a.u32);
            pushRAParams.push_back(ip->b.u32);
            pushRAParams.push_back(ip->c.u32);
            break;
        case ByteCodeOp::PushRAParam4:
            pushRAParams.push_back(ip->a.u32);
            pushRAParams.push_back(ip->b.u32);
            pushRAParams.push_back(ip->c.u32);
            pushRAParams.push_back(ip->d.u32);
            break;
        case ByteCodeOp::CopySP:
        {
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::CopySPVaargs:
        {
            SWAG_ASSERT(allocVA);

            // We need to make all variadic parameters contiguous in stack, and point to that address
            // Two cases here :
            // pushRVParams has something: then we have a typed param, of 1/2/4 bytes each
            // pushRVParams is empty: we make an array of registers

            if (!pushRVParams.empty())
            {
                auto sizeOf = pushRVParams[0].second;
                switch (sizeOf)
                {
                case 1:
                {
                    auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
                    auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(allocVA), pp.cst0_i32);
                    builder.CreateStore(r1, r0);
                    break;
                }
                case 2:
                {
                    auto r0 = TO_PTR_PTR_I16(GEP_I32(allocR, ip->a.u32));
                    auto r1 = builder.CreateInBoundsGEP(TO_PTR_I16(allocVA), pp.cst0_i32);
                    builder.CreateStore(r1, r0);
                    break;
                }
                case 4:
                {
                    auto r0 = TO_PTR_PTR_I32(GEP_I32(allocR, ip->a.u32));
                    auto r1 = builder.CreateInBoundsGEP(TO_PTR_I32(allocVA), pp.cst0_i32);
                    builder.CreateStore(r1, r0);
                    break;
                }
                }

                int idx      = 0;
                int idxParam = (int) pushRVParams.size() - 1;
                while (idxParam >= 0)
                {
                    auto reg = pushRVParams[idxParam].first;
                    switch (sizeOf)
                    {
                    case 1:
                    {
                        auto r0 = GEP_I32(TO_PTR_I8(allocVA), idx);
                        auto r1 = TO_PTR_I8(GEP_I32(allocR, reg));
                        builder.CreateStore(builder.CreateLoad(r1), r0);
                        break;
                    }
                    case 2:
                    {
                        auto r0 = GEP_I32(TO_PTR_I16(allocVA), idx);
                        auto r1 = TO_PTR_I16(GEP_I32(allocR, reg));
                        builder.CreateStore(builder.CreateLoad(r1), r0);
                        break;
                    }
                    case 4:
                    {
                        auto r0 = GEP_I32(TO_PTR_I32(allocVA), idx);
                        auto r1 = TO_PTR_I32(GEP_I32(allocR, reg));
                        builder.CreateStore(builder.CreateLoad(r1), r0);
                        break;
                    }
                    }

                    idx++;
                    idxParam--;
                }
            }
            else
            {
                // All of this is complicated. But ip->b.u32 has been reduced by one register in case of closure, and
                // we have a dynamic test for bytecode execution. But for runtime, be put it back.
                auto typeFuncCall = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->d.pointer, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                auto sizeB        = ip->b.u32;
                if (typeFuncCall->isClosure())
                    sizeB += sizeof(Register);

                // In the pushRAParams array, we have first all the variadic registers
                //
                // And then, we have all normal parameters. So we start at pushRAParams.size() less the number of registers
                // used to pass the normal parameters.
                //
                // The number of normal parameters is deduced from the 'offset' of the CopySPVaargs instruction (ip->b.u32)
                int idx      = 0;
                int idxParam = (int) pushRAParams.size() - (sizeB / sizeof(Register)) - 1;
                while (idxParam >= 0)
                {
                    SWAG_ASSERT((uint32_t) idx < bc->maxSPVaargs);
                    auto r0 = GEP_I32(allocVA, idx);
                    auto r1 = GEP_I32(allocR, pushRAParams[idxParam]);
                    builder.CreateStore(builder.CreateLoad(r1), r0);
                    idx++;
                    idxParam--;
                }

                auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
                auto r1 = builder.CreateInBoundsGEP(allocVA, pp.cst0_i32);
                builder.CreateStore(r1, r0);
            }
            break;
        }

        case ByteCodeOp::MakeLambda:
        {
            auto              funcNode     = CastAst<AstFuncDecl>((AstNode*) ip->b.pointer, AstNodeKind::FuncDecl);
            Utf8              callName     = funcNode->getCallName();
            TypeInfoFuncAttr* typeFuncNode = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

            auto T = getOrCreateFuncType(buildParameters, moduleToGen, typeFuncNode);
            auto F = (llvm::Function*) modu.getOrInsertFunction(callName.c_str(), T).getCallee();

            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(TO_PTR_I8(F), r0);

            auto v0 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(v0, GEP_I32(allocR, ip->a.u32));
            break;
        }

        case ByteCodeOp::IntrinsicMakeCallback:
        {
            llvm::BasicBlock* blockLambdaBC = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockNext     = llvm::BasicBlock::Create(context, "", func);
            auto              v0            = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));

            {
                auto v1 = builder.CreateAnd(v0, builder.getInt64(SWAG_LAMBDA_BC_MARKER));
                auto v2 = builder.CreateIsNotNull(v1);
                builder.CreateCondBr(v2, blockLambdaBC, blockNext);
            }

            builder.SetInsertPoint(blockLambdaBC);
            {
                auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst5_i32})));
                auto PT = llvm::PointerType::getUnqual(pp.makeCallbackTy);
                auto r2 = builder.CreatePointerCast(r1, PT);
                auto v2 = builder.CreateIntToPtr(v0, builder.getInt8PtrTy());
                auto v1 = builder.CreateCall(pp.makeCallbackTy, r2, {v2});
                auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
                builder.CreateStore(v1, r0);
                builder.CreateBr(blockNext);
            }

            builder.SetInsertPoint(blockNext);
            break;
        }

        case ByteCodeOp::LocalCall:
        case ByteCodeOp::LocalCallPop:
        case ByteCodeOp::LocalCallPopRC:
        {
            auto              callBc       = (ByteCode*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncCall = (TypeInfoFuncAttr*) ip->b.pointer;

            Utf8 callName;
            if (callBc->node)
            {
                auto funcNode = CastAst<AstFuncDecl>(callBc->node, AstNodeKind::FuncDecl);
                callName      = funcNode->getCallName();
            }
            else
                callName = callBc->getCallName();
            resultFuncCall = emitCall(buildParameters, moduleToGen, callName, typeFuncCall, allocR, allocRR, pushRAParams, {}, true);
            if (ip->op == ByteCodeOp::LocalCallPopRC)
            {
                storeTypedValueToRegister(context, buildParameters, resultFuncCall, ip->d.u32, allocR);
                resultFuncCall = nullptr;
            }

            pushRAParams.clear();
            pushRVParams.clear();
            break;
        }

        case ByteCodeOp::ForeignCall:
        case ByteCodeOp::ForeignCallPop:
        {
            auto              funcNode     = (AstFuncDecl*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncCall = (TypeInfoFuncAttr*) ip->b.pointer;

            funcNode->computeFullNameForeign(false);
            resultFuncCall = emitCall(buildParameters, moduleToGen, funcNode->fullnameForeign, typeFuncCall, allocR, allocRR, pushRAParams, {}, false);
            pushRAParams.clear();
            pushRVParams.clear();
            break;
        }

        case ByteCodeOp::LambdaCall:
        case ByteCodeOp::LambdaCallPop:
        {
            TypeInfoFuncAttr* typeFuncCall = (TypeInfoFuncAttr*) ip->b.pointer;

            llvm::BasicBlock* blockLambdaBC     = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockLambdaNative = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockNext         = llvm::BasicBlock::Create(context, "", func);

            {
                auto v0 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
                auto v1 = builder.CreateAnd(v0, builder.getInt64(SWAG_LAMBDA_BC_MARKER));
                auto v2 = builder.CreateIsNotNull(v1);
                builder.CreateCondBr(v2, blockLambdaBC, blockLambdaNative);
            }

            // Foreign
            //////////////////////////////
            builder.SetInsertPoint(blockLambdaNative);
            {
                llvm::FunctionType*        FT = nullptr;
                auto                       v1 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
                VectorNative<llvm::Value*> fctParams;
                emitCallParameters(buildParameters, allocR, allocRR, moduleToGen, typeFuncCall, fctParams, pushRAParams, {});

                if (typeFuncCall->isClosure())
                {
                    llvm::BasicBlock* blockLambda  = llvm::BasicBlock::Create(context, "", func);
                    llvm::BasicBlock* blockClosure = llvm::BasicBlock::Create(context, "", func);

                    // Test closure context pointer. If null, this is a lambda.
                    auto v0 = builder.CreateLoad(GEP_I32(allocR, pushRAParams.back()));
                    auto v2 = builder.CreateIsNotNull(v0);
                    builder.CreateCondBr(v2, blockClosure, blockLambda);

                    // Lambda call. We must eliminate the first parameter (closure context)
                    builder.SetInsertPoint(blockLambda);
                    VectorNative<llvm::Value*> fctParamsLocal;
                    emitCallParameters(buildParameters, allocR, allocRR, moduleToGen, typeFuncCall, fctParamsLocal, pushRAParams, {}, true);

                    FT            = getOrCreateFuncType(buildParameters, moduleToGen, typeFuncCall, true);
                    auto l_PT     = llvm::PointerType::getUnqual(FT);
                    auto l_r1     = builder.CreateIntToPtr(v1, l_PT);
                    auto l_result = builder.CreateCall(FT, l_r1, {fctParamsLocal.begin(), fctParamsLocal.end()});
                    SWAG_CHECK(emitCallReturnValue(buildParameters, allocRR, moduleToGen, typeFuncCall, l_result));
                    builder.CreateBr(blockNext);

                    // Closure call. Normal call, as the type contains the first parameter.
                    builder.SetInsertPoint(blockClosure);
                    FT            = getOrCreateFuncType(buildParameters, moduleToGen, typeFuncCall);
                    auto c_PT     = llvm::PointerType::getUnqual(FT);
                    auto c_r1     = builder.CreateIntToPtr(v1, c_PT);
                    auto c_result = builder.CreateCall(FT, c_r1, {fctParams.begin(), fctParams.end()});
                    SWAG_CHECK(emitCallReturnValue(buildParameters, allocRR, moduleToGen, typeFuncCall, c_result));
                    builder.CreateBr(blockNext);
                }
                else
                {
                    FT                = getOrCreateFuncType(buildParameters, moduleToGen, typeFuncCall);
                    auto PT           = llvm::PointerType::getUnqual(FT);
                    auto r1           = builder.CreateIntToPtr(v1, PT);
                    auto returnResult = builder.CreateCall(FT, r1, {fctParams.begin(), fctParams.end()});
                    SWAG_CHECK(emitCallReturnValue(buildParameters, allocRR, moduleToGen, typeFuncCall, returnResult));
                    builder.CreateBr(blockNext);
                }
            }

            // Bytecode
            //////////////////////////////
            builder.SetInsertPoint(blockLambdaBC);
            {
                auto                       r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
                VectorNative<llvm::Value*> fctParams;
                fctParams.push_front(r0);
                emitByteCodeCallParameters(buildParameters, allocR, allocRR, allocT, fctParams, typeFuncCall, pushRAParams, {});

                auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst4_i32})));
                auto PT = llvm::PointerType::getUnqual(pp.bytecodeRunTy);
                auto r2 = builder.CreatePointerCast(r1, PT);

                if (typeFuncCall->isClosure())
                {
                    llvm::BasicBlock* blockLambda  = llvm::BasicBlock::Create(context, "", func);
                    llvm::BasicBlock* blockClosure = llvm::BasicBlock::Create(context, "", func);

                    // Test closure context pointer. If null, this is a lambda.
                    auto v0 = builder.CreateLoad(GEP_I32(allocR, pushRAParams.back()));
                    auto v2 = builder.CreateIsNotNull(v0);
                    builder.CreateCondBr(v2, blockClosure, blockLambda);

                    // Lambda call. We must eliminate the first parameter (closure context)
                    builder.SetInsertPoint(blockLambda);
                    VectorNative<llvm::Value*> fctParamsLocal;
                    fctParamsLocal.push_front(r0);
                    emitByteCodeCallParameters(buildParameters, allocR, allocRR, allocT, fctParamsLocal, typeFuncCall, pushRAParams, {}, true);
                    builder.CreateCall(pp.bytecodeRunTy, r2, {fctParamsLocal.begin(), fctParamsLocal.end()});
                    builder.CreateBr(blockNext);

                    // Closure call. Normal call, as the type contains the first parameter.
                    builder.SetInsertPoint(blockClosure);
                    builder.CreateCall(pp.bytecodeRunTy, r2, {fctParams.begin(), fctParams.end()});
                    builder.CreateBr(blockNext);
                }
                else
                {
                    builder.CreateCall(pp.bytecodeRunTy, r2, {fctParams.begin(), fctParams.end()});
                    builder.CreateBr(blockNext);
                }
            }

            builder.SetInsertPoint(blockNext);
            pushRAParams.clear();
            pushRVParams.clear();
            resultFuncCall = nullptr;
            break;
        }

        case ByteCodeOp::IncSPPostCall:
        case ByteCodeOp::IncSPPostCallCond:
            pushRAParams.clear();
            pushRVParams.clear();
            break;

        case ByteCodeOp::IntrinsicS8x1:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_8();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {builder.getInt8Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicBitCountNz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctpop, {builder.getInt8Ty()}, {r1}), r0);
                break;
            case TokenId::IntrinsicBitCountTz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cttz, {builder.getInt8Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicBitCountLz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctlz, {builder.getInt8Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS16x1:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_16();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {builder.getInt16Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicBitCountNz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctpop, {builder.getInt16Ty()}, {r1}), r0);
                break;
            case TokenId::IntrinsicBitCountTz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cttz, {builder.getInt16Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicBitCountLz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctlz, {builder.getInt16Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicByteSwap:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::bswap, {builder.getInt16Ty()}, {r1}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS32x1:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_32();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {builder.getInt32Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicBitCountNz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctpop, {builder.getInt32Ty()}, {r1}), r0);
                break;
            case TokenId::IntrinsicBitCountTz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cttz, {builder.getInt32Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicBitCountLz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctlz, {builder.getInt32Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicByteSwap:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::bswap, {builder.getInt32Ty()}, {r1}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS64x1:
        {
            auto r0 = TO_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_64();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {builder.getInt64Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicBitCountNz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctpop, {builder.getInt64Ty()}, {r1}), r0);
                break;
            case TokenId::IntrinsicBitCountTz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cttz, {builder.getInt64Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicBitCountLz:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctlz, {builder.getInt64Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            case TokenId::IntrinsicByteSwap:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::bswap, {builder.getInt64Ty()}, {r1}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicS8x2:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_8();
            auto r2 = MK_IMMC_8();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {builder.getInt8Ty(), builder.getInt8Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {builder.getInt8Ty(), builder.getInt8Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS16x2:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_16();
            auto r2 = MK_IMMC_16();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {builder.getInt16Ty(), builder.getInt16Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {builder.getInt16Ty(), builder.getInt16Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS32x2:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_32();
            auto r2 = MK_IMMC_32();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {builder.getInt32Ty(), builder.getInt32Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {builder.getInt32Ty(), builder.getInt32Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS64x2:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = MK_IMMB_64();
            auto r2 = MK_IMMC_64();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {builder.getInt64Ty(), builder.getInt64Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {builder.getInt64Ty(), builder.getInt64Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicU8x2:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_8();
            auto r2 = MK_IMMC_8();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {builder.getInt8Ty(), builder.getInt8Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {builder.getInt8Ty(), builder.getInt8Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicU16x2:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_16();
            auto r2 = MK_IMMC_16();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {builder.getInt16Ty(), builder.getInt16Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {builder.getInt16Ty(), builder.getInt16Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicU32x2:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_32();
            auto r2 = MK_IMMC_32();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {builder.getInt32Ty(), builder.getInt32Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {builder.getInt32Ty(), builder.getInt32Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicU64x2:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = MK_IMMB_64();
            auto r2 = MK_IMMC_64();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {builder.getInt64Ty(), builder.getInt64Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {builder.getInt64Ty(), builder.getInt64Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicF32x2:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_F32();
            auto r2 = MK_IMMC_F32();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicPow:
                builder.CreateStore(builder.CreateCall(pp.fn_powf32, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::minnum, {builder.getFloatTy(), builder.getFloatTy()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::maxnum, {builder.getFloatTy(), builder.getFloatTy()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicATan2:
                builder.CreateStore(builder.CreateCall(pp.fn_atan2f32, {r1, r2}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicF64x2:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_F64();
            auto r2 = MK_IMMC_F64();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicPow:
                builder.CreateStore(builder.CreateCall(pp.fn_powf64, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::minnum, {builder.getDoubleTy(), builder.getDoubleTy()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::maxnum, {builder.getDoubleTy(), builder.getDoubleTy()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicATan2:
                builder.CreateStore(builder.CreateCall(pp.fn_atan2f64, {r1, r2}), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicF32x1:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_F32();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicTan:
                builder.CreateStore(builder.CreateCall(pp.fn_tanf32, r1), r0);
                break;
            case TokenId::IntrinsicSinh:
                builder.CreateStore(builder.CreateCall(pp.fn_sinhf32, r1), r0);
                break;
            case TokenId::IntrinsicCosh:
                builder.CreateStore(builder.CreateCall(pp.fn_coshf32, r1), r0);
                break;
            case TokenId::IntrinsicTanh:
                builder.CreateStore(builder.CreateCall(pp.fn_tanhf32, r1), r0);
                break;
            case TokenId::IntrinsicASin:
                builder.CreateStore(builder.CreateCall(pp.fn_asinf32, r1), r0);
                break;
            case TokenId::IntrinsicACos:
                builder.CreateStore(builder.CreateCall(pp.fn_acosf32, r1), r0);
                break;
            case TokenId::IntrinsicATan:
                builder.CreateStore(builder.CreateCall(pp.fn_atanf32, r1), r0);
                break;
            case TokenId::IntrinsicSqrt:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sqrt, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicSin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sin, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicCos:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cos, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicLog:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicLog2:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log2, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicLog10:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log10, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicFloor:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::floor, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicCeil:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ceil, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicTrunc:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::trunc, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicRound:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::round, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fabs, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicExp:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicExp2:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp2, builder.getFloatTy(), r1), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicF64x1:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_F64();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicTan:
                builder.CreateStore(builder.CreateCall(pp.fn_tanf64, r1), r0);
                break;
            case TokenId::IntrinsicSinh:
                builder.CreateStore(builder.CreateCall(pp.fn_sinhf64, r1), r0);
                break;
            case TokenId::IntrinsicCosh:
                builder.CreateStore(builder.CreateCall(pp.fn_coshf64, r1), r0);
                break;
            case TokenId::IntrinsicTanh:
                builder.CreateStore(builder.CreateCall(pp.fn_tanhf64, r1), r0);
                break;
            case TokenId::IntrinsicASin:
                builder.CreateStore(builder.CreateCall(pp.fn_asinf64, r1), r0);
                break;
            case TokenId::IntrinsicACos:
                builder.CreateStore(builder.CreateCall(pp.fn_acosf64, r1), r0);
                break;
            case TokenId::IntrinsicATan:
                builder.CreateStore(builder.CreateCall(pp.fn_atanf64, r1), r0);
                break;
            case TokenId::IntrinsicSqrt:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sqrt, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicSin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sin, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicCos:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cos, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicLog:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicLog2:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log2, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicLog10:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log10, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicFloor:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::floor, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicCeil:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ceil, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicTrunc:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::trunc, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicRound:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::round, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fabs, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicExp:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicExp2:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp2, builder.getDoubleTy(), r1), r0);
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }

        default:
            ok = false;
            Report::internalError(moduleToGen, Fmt("unknown instruction '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
            break;
        }
    }

    if (!blockIsClosed)
        builder.CreateRetVoid();
    return ok;
}

BackendFunctionBodyJobBase* BackendLLVM::newFunctionJob()
{
    return Allocator::alloc<BackendLLVMFunctionBodyJob>();
}

llvm::Value* BackendLLVM::getImmediateConstantA(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits)
{
    if (ip->flags & BCI_IMM_A)
    {
        switch (numBits)
        {
        case 8:
            return builder.getInt8(ip->a.u8);
        case 16:
            return builder.getInt16(ip->a.u16);
        case 32:
            return builder.getInt32(ip->a.u32);
        case 64:
            return builder.getInt64(ip->a.u64);
        }
    }

    auto r0 = TO_PTR_I_N(GEP_I32(allocR, ip->a.u32), numBits);
    return builder.CreateLoad(r0);
}

llvm::Type* BackendLLVM::getIntType(llvm::LLVMContext& context, uint8_t numBits)
{
    switch (numBits)
    {
    case 8:
        return llvm::Type::getInt8Ty(context);
    case 16:
        return llvm::Type::getInt16Ty(context);
    case 32:
        return llvm::Type::getInt32Ty(context);
    case 64:
        return llvm::Type::getInt64Ty(context);
    default:
        SWAG_ASSERT(false);
        break;
    }

    return nullptr;
}

llvm::Type* BackendLLVM::getIntPtrType(llvm::LLVMContext& context, uint8_t numBits)
{
    return getIntType(context, numBits)->getPointerTo();
}

llvm::Type* BackendLLVM::swagTypeToLLVMType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* typeInfo)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;

    typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS | CONCRETE_FORCEALIAS);

    if (typeInfo->isEnum())
    {
        auto typeInfoEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        return swagTypeToLLVMType(buildParameters, moduleToGen, typeInfoEnum->rawType);
    }

    if (typeInfo->isPointer())
    {
        auto typeInfoPointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        auto pointedType     = TypeManager::concreteType(typeInfoPointer->pointedType);
        if (pointedType->isVoid())
            return llvm::Type::getInt8PtrTy(context);
        else
            return swagTypeToLLVMType(buildParameters, moduleToGen, pointedType)->getPointerTo();
    }

    if (typeInfo->isSlice() ||
        typeInfo->isArray() ||
        typeInfo->isStruct() ||
        typeInfo->isInterface() ||
        typeInfo->isLambdaClosure() ||
        typeInfo->isAny() ||
        typeInfo->isString())
    {
        return llvm::Type::getInt8PtrTy(context);
    }

    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            return llvm::Type::getInt8Ty(context);
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            return llvm::Type::getInt8Ty(context);
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            return llvm::Type::getInt16Ty(context);
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            return llvm::Type::getInt32Ty(context);
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            return llvm::Type::getInt64Ty(context);
        case NativeTypeKind::F32:
            return llvm::Type::getFloatTy(context);
        case NativeTypeKind::F64:
            return llvm::Type::getDoubleTy(context);
        case NativeTypeKind::Void:
            return llvm::Type::getVoidTy(context);
        }
    }

    SWAG_ASSERT(false);
    return nullptr;
}

llvm::BasicBlock* BackendLLVM::getOrCreateLabel(LLVMPerThread& pp, llvm::Function* func, int64_t ip)
{
    auto& context = *pp.context;

    auto it = pp.labels.find(ip);
    if (it == pp.labels.end())
    {
        llvm::BasicBlock* label = llvm::BasicBlock::Create(context, Fmt("%lld", ip).c_str(), func);
        pp.labels[ip]           = label;
        return label;
    }

    return it->second;
}

void BackendLLVM::emitShiftArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits)
{
    auto         iType = getIntType(context, numBits);
    auto         r0    = TO_PTR_I_N(GEP_I32(allocR, ip->c.u32), numBits);
    llvm::Value* r1    = getImmediateConstantA(context, builder, allocR, ip, numBits);

    llvm::Value* r2;
    if (ip->flags & BCI_IMM_B)
        r2 = builder.getInt32(ip->b.u32 & (numBits - 1));
    else
        r2 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));

    auto c2    = builder.CreateIntCast(r2, iType, false);
    auto shift = llvm::ConstantInt::get(iType, numBits - 1);

    // Smart shift, imm mode overflow
    if ((ip->flags & BCI_IMM_B) && !(ip->flags & BCI_SHIFT_SMALL) && ip->b.u32 >= numBits)
        c2 = shift;

    // Smart shift, dyn mode
    else if (!(ip->flags & BCI_IMM_B) && !(ip->flags & BCI_SHIFT_SMALL))
    {
        auto cond    = builder.CreateICmpULT(r2, builder.getInt32(numBits));
        auto iftrue  = c2;
        auto iffalse = shift;
        c2           = builder.CreateSelect(cond, iftrue, iffalse);
    }

    // Small shift, dyn mode, we mask the operand
    else if (!(ip->flags & BCI_IMM_B))
        c2 = builder.CreateAnd(c2, llvm::ConstantInt::get(iType, numBits - 1));

    auto v0 = builder.CreateAShr(r1, c2);
    builder.CreateStore(v0, TO_PTR_I_N(r0, numBits));
}

void BackendLLVM::emitShiftLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits, bool left)
{
    auto iType = getIntType(context, numBits);
    auto r0    = TO_PTR_I_N(GEP_I32(allocR, ip->c.u32), numBits);
    auto zero  = llvm::ConstantInt::get(iType, 0);

    // Smart shift, imm mode overflow
    if ((ip->flags & BCI_IMM_B) && !(ip->flags & BCI_SHIFT_SMALL) && ip->b.u32 >= numBits)
        builder.CreateStore(zero, r0);
    else
    {
        llvm::Value* r1 = getImmediateConstantA(context, builder, allocR, ip, numBits);

        llvm::Value* r2;
        if (ip->flags & BCI_IMM_B)
            r2 = builder.getInt32(ip->b.u32 & (numBits - 1));
        else
            r2 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
        auto c2 = builder.CreateIntCast(r2, iType, false);

        // Smart shift, dyn mode
        if (!(ip->flags & BCI_IMM_B) && !(ip->flags & BCI_SHIFT_SMALL))
        {
            auto cond    = builder.CreateICmpULT(r2, builder.getInt32(numBits));
            auto iftrue  = left ? builder.CreateShl(r1, c2) : builder.CreateLShr(r1, c2);
            auto iffalse = zero;
            auto v0      = builder.CreateSelect(cond, iftrue, iffalse);
            builder.CreateStore(v0, r0);
        }
        else
        {
            // Small shift, dyn mode, we mask the operand
            if (!(ip->flags & BCI_IMM_B))
                c2 = builder.CreateAnd(c2, llvm::ConstantInt::get(iType, numBits - 1));
            auto v0 = left ? builder.CreateShl(r1, c2) : builder.CreateLShr(r1, c2);
            builder.CreateStore(v0, r0);
        }
    }
}

void BackendLLVM::emitShiftEqArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits)
{
    auto iType = getIntType(context, numBits);
    auto r0    = GEP_I32(allocR, ip->a.u32);
    auto r1    = builder.CreateLoad(TO_PTR_PTR_I_N(r0, numBits));

    llvm::Value* r2;
    if (ip->flags & BCI_IMM_B)
        r2 = builder.getInt32(ip->b.u32 & (numBits - 1));
    else
        r2 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));

    auto c2    = builder.CreateIntCast(r2, iType, false);
    auto shift = llvm::ConstantInt::get(iType, numBits - 1);

    // Smart shift, imm mode overflow
    if ((ip->flags & BCI_IMM_B) && !(ip->flags & BCI_SHIFT_SMALL) && ip->b.u32 >= numBits)
        c2 = shift;

    // Smart shift, dyn mode
    else if (!(ip->flags & BCI_IMM_B) && !(ip->flags & BCI_SHIFT_SMALL))
    {
        auto cond    = builder.CreateICmpULT(r2, builder.getInt32(numBits));
        auto iftrue  = c2;
        auto iffalse = shift;
        c2           = builder.CreateSelect(cond, iftrue, iffalse);
    }

    // Small shift, dyn mode, we mask the operand
    else if (!(ip->flags & BCI_IMM_B))
        c2 = builder.CreateAnd(c2, llvm::ConstantInt::get(iType, numBits - 1));

    auto v0 = builder.CreateAShr(builder.CreateLoad(r1), c2);
    builder.CreateStore(v0, r1);
}

void BackendLLVM::emitShiftEqLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits, bool left)
{
    auto iType = getIntType(context, numBits);
    auto r0    = GEP_I32(allocR, ip->a.u32);
    auto r1    = builder.CreateLoad(TO_PTR_PTR_I_N(r0, numBits));

    llvm::Value* r2;
    if (ip->flags & BCI_IMM_B)
        r2 = builder.getInt32(ip->b.u32 & (numBits - 1));
    else
        r2 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));

    auto         c2   = builder.CreateIntCast(r2, iType, false);
    auto         zero = llvm::ConstantInt::get(iType, 0);
    auto         v1   = builder.CreateLoad(r1);
    llvm::Value* v0;

    // Smart shift, imm mode overflow
    if ((ip->flags & BCI_IMM_B) && !(ip->flags & BCI_SHIFT_SMALL) && ip->b.u32 >= numBits)
        v0 = zero;

    // Smart shift, dyn mode
    else if (!(ip->flags & BCI_IMM_B) && !(ip->flags & BCI_SHIFT_SMALL))
    {
        auto cond    = builder.CreateICmpULT(r2, builder.getInt32(numBits));
        auto iftrue  = left ? builder.CreateShl(v1, c2) : builder.CreateLShr(v1, c2);
        auto iffalse = zero;
        v0           = builder.CreateSelect(cond, iftrue, iffalse);
    }

    // Imm mode
    else
    {
        // Small shift, dyn mode, we mask the operand
        if (!(ip->flags & BCI_IMM_B))
            c2 = builder.CreateAnd(c2, llvm::ConstantInt::get(iType, numBits - 1));
        v0 = left ? builder.CreateShl(v1, c2) : builder.CreateLShr(v1, c2);
    }

    builder.CreateStore(v0, r1);
}

void BackendLLVM::emitInternalPanic(const BuildParameters& buildParameters, Module* moduleToGen, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, AstNode* node, const char* message)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;

    // Filename
    llvm::Value* r1 = builder.CreateGlobalString(Utf8::normalizePath(node->sourceFile->path).c_str());
    r1              = TO_PTR_I8(builder.CreateInBoundsGEP(r1, {pp.cst0_i32}));

    // Line & column
    auto r2 = builder.getInt32(node->token.startLocation.line);
    auto r3 = builder.getInt32(node->token.startLocation.column);

    // Message
    llvm::Value* r4;
    if (message)
    {
        r4 = builder.CreateGlobalString(message);
        r4 = TO_PTR_I8(builder.CreateInBoundsGEP(r4, {pp.cst0_i32}));
    }
    else
        r4 = builder.CreateIntToPtr(pp.cst0_i64, builder.getInt8PtrTy());

    emitCall(buildParameters, moduleToGen, g_LangSpec->name__panic, allocR, allocT, {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX}, {r1, r2, r3, r4});
}

void BackendLLVM::setFuncAttributes(const BuildParameters& buildParameters, Module* moduleToGen, AstFuncDecl* funcNode, ByteCode* bc, llvm::Function* func)
{
    llvm::AttrBuilder attr;

    if (!moduleToGen->mustOptimizeBK(bc->node))
        attr.addAttribute(llvm::Attribute::OptimizeNone);
    if (funcNode && funcNode->attributeFlags & ATTRIBUTE_NO_INLINE)
        attr.addAttribute(llvm::Attribute::NoInline);

    func->addAttributes(llvm::AttributeList::FunctionIndex, attr);
}

void BackendLLVM::storeTypedValueToRegister(llvm::LLVMContext& context, const BuildParameters& buildParameters, llvm::Value* value, uint32_t reg, llvm::AllocaInst* allocR)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    SWAG_ASSERT(value);
    auto r0 = GEP_I32(allocR, reg);
    auto r1 = value;
    if (value->getType()->isPointerTy())
        builder.CreateStore(builder.CreatePtrToInt(r1, builder.getInt64Ty()), r0);
    else if (value->getType()->isIntegerTy(8))
        builder.CreateStore(r1, TO_PTR_I8(r0));
    else if (value->getType()->isIntegerTy(16))
        builder.CreateStore(r1, TO_PTR_I16(r0));
    else if (value->getType()->isIntegerTy(32))
        builder.CreateStore(r1, TO_PTR_I32(r0));
    else if (value->getType()->isFloatTy())
        builder.CreateStore(r1, TO_PTR_F32(r0));
    else if (value->getType()->isDoubleTy())
        builder.CreateStore(r1, TO_PTR_F64(r0));
    else
        builder.CreateStore(r1, r0);
}

void BackendLLVM::storeRT2ToRegisters(llvm::LLVMContext& context, const BuildParameters& buildParameters, uint32_t reg0, uint32_t reg1, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    auto r0 = GEP_I32(allocR, reg0);
    auto r1 = builder.CreateLoad(GEP_I32(allocRR, 0));
    builder.CreateStore(r1, r0);
    r0 = GEP_I32(allocR, reg1);
    r1 = builder.CreateLoad(GEP_I32(allocRR, 1));
    builder.CreateStore(r1, r0);
}

void BackendLLVM::getReturnResult(llvm::LLVMContext&     context,
                                  const BuildParameters& buildParameters,
                                  Module*                moduleToGen,
                                  TypeInfo*              returnType,
                                  bool                   imm,
                                  Register&              reg,
                                  llvm::AllocaInst*      allocR,
                                  llvm::AllocaInst*      allocResult)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    llvm::Value* returnResult = nullptr;
    if (returnType->isNative())
    {
        switch (returnType->nativeType)
        {
        case NativeTypeKind::U8:
        case NativeTypeKind::S8:
        case NativeTypeKind::Bool:
            if (imm)
                returnResult = builder.getInt8(reg.u8);
            else
                returnResult = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, reg.u32)));
            builder.CreateStore(returnResult, TO_PTR_I8(allocResult));
            break;
        case NativeTypeKind::U16:
        case NativeTypeKind::S16:
            if (imm)
                returnResult = builder.getInt16(reg.u16);
            else
                returnResult = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, reg.u32)));
            builder.CreateStore(returnResult, TO_PTR_I16(allocResult));
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::S32:
        case NativeTypeKind::Rune:
            if (imm)
                returnResult = builder.getInt32(reg.u32);
            else
                returnResult = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, reg.u32)));
            builder.CreateStore(returnResult, TO_PTR_I32(allocResult));
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::S64:
            if (imm)
                returnResult = builder.getInt64(reg.u64);
            else
                returnResult = builder.CreateLoad(GEP_I32(allocR, reg.u32));
            builder.CreateStore(returnResult, TO_PTR_I64(allocResult));
            break;
        case NativeTypeKind::F32:
            if (imm)
                returnResult = llvm::ConstantFP::get(builder.getFloatTy(), reg.f32);
            else
                returnResult = builder.CreateLoad(TO_PTR_F32(GEP_I32(allocR, reg.u32)));
            builder.CreateStore(returnResult, TO_PTR_F32(allocResult));
            break;
        case NativeTypeKind::F64:
            if (imm)
                returnResult = llvm::ConstantFP::get(builder.getDoubleTy(), reg.f64);
            else
                returnResult = builder.CreateLoad(TO_PTR_F64(GEP_I32(allocR, reg.u32)));
            builder.CreateStore(returnResult, TO_PTR_F64(allocResult));
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }
    else if (returnType->isPointer() || returnType->isLambdaClosure())
    {
        auto llvmType = swagTypeToLLVMType(buildParameters, moduleToGen, returnType);
        if (imm)
            returnResult = builder.CreateIntToPtr(builder.getInt64(reg.u64), llvmType);
        else
            returnResult = builder.CreateIntToPtr(builder.CreateLoad(GEP_I32(allocR, reg.u32)), llvmType);
        builder.CreateStore(returnResult, builder.CreatePointerCast(allocResult, llvmType->getPointerTo()));
    }
    else
    {
        SWAG_ASSERT(false);
    }
}