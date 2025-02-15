#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/LLVM/Debug/LLVMDebug.h"
#include "Backend/LLVM/Main/LLVM.h"
#include "Backend/LLVM/Main/LLVM_Macros.h"
#include "Report/Diagnostic.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool LLVM::emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc)
{
    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && bc->node->hasAttribute(ATTRIBUTE_TEST_FUNC) && buildParameters.compileType != Test)
        return true;

    const auto ct              = buildParameters.compileType;
    const auto precompileIndex = buildParameters.precompileIndex;
    auto&      pp              = encoder<LLVMEncoder>(ct, precompileIndex);
    auto&      context         = *pp.llvmContext;
    auto&      builder         = *pp.builder;
    auto&      modu            = *pp.llvmModule;
    auto       typeFunc        = bc->getCallType();
    auto       returnType      = typeFunc->concreteReturnType();
    bool       ok              = true;

    // Get function name
    Utf8         funcName   = bc->getCallNameFromDecl();
    AstFuncDecl* bcFuncNode = bc->node ? castAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl) : nullptr;

    // Function prototype
    auto funcType = getOrCreateFuncType(buildParameters, typeFunc);
    auto func     = reinterpret_cast<llvm::Function*>(modu.getOrInsertFunction(funcName.cstr(), funcType).getCallee());
    setFuncAttributes(buildParameters, bcFuncNode, bc, func);

    // Content
    llvm::BasicBlock* block         = llvm::BasicBlock::Create(context, "entry", func);
    bool              blockIsClosed = false;
    builder.SetInsertPoint(block);
    pp.labels.clear();
    bc->markLabels();

    // Reserve registers
    llvm::AllocaInst* allocR = nullptr;
    if (bc->maxReservedRegisterRC)
    {
        allocR = builder.CreateAlloca(I64_TY(), builder.getInt32(bc->maxReservedRegisterRC));
        allocR->setAlignment(llvm::Align{16});
    }

    llvm::AllocaInst* allocRR = nullptr;
    if (bc->maxCallResults)
    {
        allocRR = builder.CreateAlloca(I64_TY(), builder.getInt32(bc->maxCallResults));
        allocRR->setAlignment(llvm::Align{16});
    }

    llvm::AllocaInst* allocResult = builder.CreateAlloca(I64_TY(), builder.getInt32(1));
    allocResult->setAlignment(llvm::Align{16});

    // To store variadic
    llvm::AllocaInst* allocVa = nullptr;
    if (bc->maxSpVaargs)
    {
        allocVa = builder.CreateAlloca(I64_TY(), builder.getInt64(bc->maxSpVaargs));
        allocVa->setAlignment(llvm::Align{16});
    }

    // Stack
    llvm::AllocaInst* allocStack = nullptr;
    if (bc->stackSize)
    {
        allocStack = builder.CreateAlloca(I8_TY(), builder.getInt32(bc->stackSize));
        allocStack->setAlignment(llvm::Align(2 * sizeof(void*)));
    }

    // Reserve room to pass parameters to embedded intrinsics
    static constexpr int ALLOC_T_NUM = 5;
    auto                 allocT      = builder.CreateAlloca(I64_TY(), builder.getInt64(ALLOC_T_NUM));
    allocT->setAlignment(llvm::Align{16});

    // Debug infos
    if (pp.dbg && bc->node)
    {
        pp.dbg->startFunction(buildParameters, pp, bc, func, allocStack);
        pp.dbg->setLocation(pp.builder, bc, bc->out);
    }

    // Generate bytecode
    auto                                        ip = bc->out;
    VectorNative<std::pair<uint32_t, uint32_t>> pushRVParams;
    VectorNative<uint32_t>                      pushRAParams;
    llvm::Value*                                resultFuncCall = nullptr;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->hasAstFlag(AST_NO_BACKEND))
            continue;
        if (ip->hasFlag(BCI_NO_BACKEND))
            continue;

        // If we are the destination of a jump, be sure we have a block, and from now insert into that block
        if (ip->hasFlag(BCI_JUMP_DEST) || blockIsClosed)
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
            case ByteCodeOp::IntrinsicBcBreakpoint:
            case ByteCodeOp::PushRR:
            case ByteCodeOp::PopRR:
                continue;
            default:
                break;
        }

        switch (ip->op)
        {
            case ByteCodeOp::DebugNop:
            {
                const auto r0 = GEP(I32_TY(), allocT, 0);
                builder.CreateStore(builder.getInt32(0), r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::IncPointer64:
            case ByteCodeOp::DecPointer64:
            {
                const auto   r1 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                llvm::Value* r2;
                if (ip->hasFlag(BCI_IMM_B))
                    r2 = builder.getInt64(ip->b.u64);
                else
                    r2 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->b.u32));
                if (ip->op == ByteCodeOp::DecPointer64)
                    r2 = builder.CreateNeg(r2);

                const auto r3 = builder.CreateInBoundsGEP(I8_TY(), r1, r2);
                builder.CreateStore(r3, GEP64_PTR_PTR_I8(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IncMulPointer64:
            {
                const auto   r1 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                llvm::Value* r2;
                if (ip->hasFlag(BCI_IMM_B))
                    r2 = builder.getInt64(ip->b.u64 * ip->d.u64);
                else
                {
                    r2 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->b.u32));
                    r2 = builder.CreateMul(r2, builder.getInt64(ip->d.u64));
                }

                const auto r3 = builder.CreateInBoundsGEP(I8_TY(), r1, r2);
                builder.CreateStore(r3, GEP64_PTR_PTR_I8(allocR, ip->c.u32));
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::DeRef8:
            case ByteCodeOp::DeRef16:
            case ByteCodeOp::DeRef32:
            case ByteCodeOp::DeRef64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->b.u32);
                const auto ptr     = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), r0), builder.getInt64(ip->c.s64));
                const auto v1      = builder.CreateLoad(IX_TY(numBits), ptr);
                const auto v2      = builder.CreateIntCast(v1, I64_TY(), false);
                builder.CreateStore(v2, GEP64(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::DeRefStringSlice:
            {
                const auto r0  = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                const auto r1  = GEP64_PTR_PTR_I8(allocR, ip->b.u32);
                const auto ptr = builder.CreateLoad(PTR_I8_TY(), r0);
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                const auto ptr0 = builder.CreateInBoundsGEP(I8_TY(), ptr, builder.getInt32(ip->c.u32 + 0));
                const auto ptr8 = builder.CreateInBoundsGEP(I8_TY(), ptr, builder.getInt32(ip->c.u32 + 8));
                const auto v0   = builder.CreateLoad(PTR_I8_TY(), ptr0);
                const auto v8   = builder.CreateLoad(PTR_I8_TY(), ptr8);
                builder.CreateStore(v8, r1);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::Mul64byVB64:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = builder.getInt64(ip->b.u64);
                builder.CreateStore(builder.CreateMul(builder.CreateLoad(I64_TY(), r0), r1), r0);
                break;
            }
            case ByteCodeOp::Div64byVB64:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = builder.getInt64(ip->b.u64);
                builder.CreateStore(builder.CreateUDiv(builder.CreateLoad(I64_TY(), r0), r1), r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::GetFromMutableSeg8:
            case ByteCodeOp::GetFromMutableSeg16:
            case ByteCodeOp::GetFromMutableSeg32:
            case ByteCodeOp::GetFromMutableSeg64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = GEP8(pp.mutableSeg, ip->b.u32);
                const auto v0      = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r1), I64_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::GetFromBssSeg8:
            case ByteCodeOp::GetFromBssSeg16:
            case ByteCodeOp::GetFromBssSeg32:
            case ByteCodeOp::GetFromBssSeg64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = GEP8(pp.bssSeg, ip->b.u32);
                const auto v0      = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r1), I64_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetZeroStack8:
            case ByteCodeOp::SetZeroStack16:
            case ByteCodeOp::SetZeroStack32:
            case ByteCodeOp::SetZeroStack64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8_PTR_IX(allocStack, ip->a.u32, numBits);
                builder.CreateStore(builder.getIntN(numBits, 0), r0);
                break;
            }

            case ByteCodeOp::SetZeroStackX:
            {
                const auto r0 = GEP8(allocStack, ip->a.u32);
                builder.CreateMemSet(r0, builder.getInt8(0), ip->b.u32, {});
                break;
            }

                ///////////////////////////////////////////

            case ByteCodeOp::SetZeroAtPointer8:
            case ByteCodeOp::SetZeroAtPointer16:
            case ByteCodeOp::SetZeroAtPointer32:
            case ByteCodeOp::SetZeroAtPointer64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto v0      = GEP8_PTR_IX(r0, ip->b.u32, numBits);
                builder.CreateStore(builder.getIntN(numBits, 0), v0);
                break;
            }

            case ByteCodeOp::SetZeroAtPointerX:
            {
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto v0 = builder.CreateInBoundsGEP(I8_TY(), r0, CST_RC32);
                builder.CreateMemSet(v0, builder.getInt8(0), ip->b.u64, llvm::Align{});
                break;
            }
            case ByteCodeOp::SetZeroAtPointerXRB:
            {
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r2 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->b.u32));
                const auto v2 = builder.CreateMul(r2, CST_RC64);
                builder.CreateMemSet(r0, builder.getInt8(0), v2, {});
                break;
            }

                ///////////////////////////////////////////

            case ByteCodeOp::ClearRR8:
            case ByteCodeOp::ClearRR16:
            case ByteCodeOp::ClearRR32:
            case ByteCodeOp::ClearRR64:
            {
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = TO_PTR_I8(func->getArg(static_cast<int>(func->arg_size()) - 1));
                const auto v0      = GEP8_PTR_IX(r0, ip->c.u32, numBits);
                builder.CreateStore(builder.getIntN(numBits, 0), v0);
                break;
            }

            case ByteCodeOp::ClearRRX:
            {
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                const auto r0 = TO_PTR_I8(func->getArg(static_cast<int>(func->arg_size()) - 1));
                const auto v0 = builder.CreateInBoundsGEP(I8_TY(), r0, CST_RC32);
                builder.CreateMemSet(v0, builder.getInt8(0), ip->b.u64, llvm::Align{});
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::MakeMutableSegPointer:
            {
                const auto r1 = GEP8(pp.mutableSeg, ip->b.u32);
                builder.CreateStore(r1, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::MakeBssSegPointer:
            {
                const auto r1 = GEP8(pp.bssSeg, ip->b.u32);
                builder.CreateStore(r1, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::MakeConstantSegPointer:
            {
                const auto r1 = GEP8(pp.constantSeg, ip->b.u32);
                builder.CreateStore(r1, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::MakeCompilerSegPointer:
                break;

            case ByteCodeOp::MakeStackPointer:
            {
                const auto r1 = GEP8(allocStack, ip->b.u32);
                builder.CreateStore(r1, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::MakeStackPointerx2:
            {
                const auto r1 = GEP8(allocStack, ip->b.u32);
                builder.CreateStore(r1, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                const auto r2 = GEP8(allocStack, ip->d.u32);
                builder.CreateStore(r2, GEP64_PTR_PTR_I8(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::MakeStackPointerRT:
            {
                const auto r0 = GEP64_PTR_PTR_I8(allocRR, 0);
                const auto r1 = GEP8(allocStack, ip->a.u32);
                builder.CreateStore(r1, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8:
            case ByteCodeOp::GetFromStack16:
            case ByteCodeOp::GetFromStack32:
            case ByteCodeOp::GetFromStack64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = GEP8(allocStack, ip->b.u32);
                const auto v0      = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r1), I64_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8x2:
            case ByteCodeOp::GetFromStack16x2:
            case ByteCodeOp::GetFromStack32x2:
            case ByteCodeOp::GetFromStack64x2:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);

                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = GEP8(allocStack, ip->b.u32);
                const auto v0 = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r1), I64_TY(), false);
                builder.CreateStore(v0, r0);
                const auto r2 = GEP64(allocR, ip->c.u32);
                const auto r3 = GEP8(allocStack, ip->d.u32);
                const auto v1 = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r3), I64_TY(), false);
                builder.CreateStore(v1, r2);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::GetIncFromStack64:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = GEP8(allocStack, ip->b.u32);
                const auto v0 = builder.CreateLoad(I64_TY(), r1);
                const auto v1 = builder.CreateAdd(v0, builder.getInt64(ip->c.s64));
                builder.CreateStore(v1, r0);
                break;
            }

            case ByteCodeOp::GetIncFromStack64DeRef8:
            case ByteCodeOp::GetIncFromStack64DeRef16:
            case ByteCodeOp::GetIncFromStack64DeRef32:
            case ByteCodeOp::GetIncFromStack64DeRef64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r1      = GEP8(allocStack, ip->b.u32);
                const auto v0      = builder.CreateLoad(PTR_I8_TY(), r1);
                const auto v1      = GEP8_PTR_IX(v0, ip->c.u64, numBits);
                const auto v2      = builder.CreateLoad(IX_TY(numBits), v1);
                const auto v3      = builder.CreateIntCast(v2, I64_TY(), false);
                builder.CreateStore(v3, GEP64(allocR, ip->a.u32));
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CopyStack8:
            case ByteCodeOp::CopyStack16:
            case ByteCodeOp::CopyStack32:
            case ByteCodeOp::CopyStack64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8(allocStack, ip->a.u32);
                const auto r1      = GEP8(allocStack, ip->b.u32);
                builder.CreateStore(builder.CreateLoad(IX_TY(numBits), r1), r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicDbgAlloc:
            {
                const auto result = emitCall(buildParameters, g_LangSpec->name_at_dbgalloc, allocR, allocT, {}, {});
                builder.CreateStore(TO_PTR_I8(result), GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::IntrinsicSysAlloc:
            {
                const auto result = emitCall(buildParameters, g_LangSpec->name_at_sysalloc, allocR, allocT, {}, {});
                builder.CreateStore(TO_PTR_I8(result), GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::IntrinsicRtFlags:
            {
                const auto result = emitCall(buildParameters, g_LangSpec->name_at_rtflags, allocR, allocT, {}, {});
                builder.CreateStore(result, GEP64(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::IntrinsicStringCmp:
            {
                const auto result = emitCall(buildParameters, g_LangSpec->name_at_strcmp, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32}, {});
                builder.CreateStore(result, GEP64_PTR_I8(allocR, ip->d.u32));
                break;
            }
            case ByteCodeOp::IntrinsicTypeCmp:
            {
                const auto result = emitCall(buildParameters, g_LangSpec->name_at_typecmp, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
                builder.CreateStore(result, GEP64_PTR_I8(allocR, ip->d.u32));
                break;
            }
            case ByteCodeOp::IntrinsicIs:
            {
                const auto result = emitCall(buildParameters, g_LangSpec->name_at_is, allocR, allocT, {ip->a.u32, ip->b.u32}, {});
                builder.CreateStore(result, GEP64_PTR_I8(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAs:
            {
                const auto result = emitCall(buildParameters, g_LangSpec->name_at_as, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
                builder.CreateStore(result, GEP64_PTR_I8(allocR, ip->d.u32));
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::MemCpy8:
            case ByteCodeOp::MemCpy16:
            case ByteCodeOp::MemCpy32:
            case ByteCodeOp::MemCpy64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->b.u32));
                builder.CreateStore(builder.CreateLoad(IX_TY(numBits), r1), r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMemCpy:
            {
                const auto r0   = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1   = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->b.u32));
                const auto size = MK_IMMC_64();
                builder.CreateMemCpy(r0, {}, r1, {}, size);
                break;
            }

            case ByteCodeOp::IntrinsicMemMove:
            {
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->b.u32));
                const auto r2 = MK_IMMC_64();
                builder.CreateMemMove(r0, {}, r1, {}, r2);
                break;
            }

            case ByteCodeOp::IntrinsicMemSet:
            {
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1 = MK_IMMB_8();
                const auto r2 = MK_IMMC_64();
                builder.CreateMemSet(r0, r1, r2, llvm::Align{});
                break;
            }

            case ByteCodeOp::IntrinsicMemCmp:
            {
                const auto r0 = GEP64_PTR_I32(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->b.u32));
                const auto r2 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->c.u32));
                const auto r3 = MK_IMMD_64();
                builder.CreateStore(builder.CreateCall(pp.fnMemCmp, {r1, r2, r3}), r0);
                break;
            }

            case ByteCodeOp::IntrinsicStrLen:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->b.u32));
                builder.CreateStore(builder.CreateCall(pp.fnStrlen, {r1}), r0);
                break;
            }

            case ByteCodeOp::IntrinsicStrCmp:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->b.u32));
                const auto r2 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->c.u32));
                builder.CreateStore(builder.CreateCall(pp.fnStrcmp, {r1, r2}), r0);
                break;
            }

            case ByteCodeOp::IntrinsicAlloc:
            {
                const auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->b.u32));
                builder.CreateStore(builder.CreateCall(pp.fnMalloc, {r1}), r0);
                break;
            }

            case ByteCodeOp::IntrinsicRealloc:
            {
                const auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->b.u32));
                const auto r2 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->c.u32));
                builder.CreateStore(builder.CreateCall(pp.fnReAlloc, {r1, r2}), r0);
                break;
            }

            case ByteCodeOp::IntrinsicFree:
            {
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                builder.CreateCall(pp.fnFree, {r0});
                break;
            }

            case ByteCodeOp::IntrinsicItfTableOf:
            {
                const auto result = emitCall(buildParameters, g_LangSpec->name_at_itftableof, allocR, allocT, {ip->a.u32, ip->b.u32}, {});
                builder.CreateStore(result, GEP64_PTR_PTR_I8(allocR, ip->c.u32));
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetImmediate32:
            case ByteCodeOp::SetImmediate64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->a.u32, numBits);
                builder.CreateStore(CST_RB(numBits), r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CopyRBtoRA8:
            case ByteCodeOp::CopyRBtoRA16:
            case ByteCodeOp::CopyRBtoRA32:
            case ByteCodeOp::CopyRBtoRA64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = GEP64(allocR, ip->b.u32);
                const auto v1      = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r1), I64_TY(), false);
                builder.CreateStore(v1, r0);
                break;
            }

            case ByteCodeOp::CopyRBtoRA64x2:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = GEP64(allocR, ip->b.u32);
                builder.CreateStore(builder.CreateLoad(I64_TY(), r1), r0);
                const auto r2 = GEP64(allocR, ip->c.u32);
                const auto r3 = GEP64(allocR, ip->d.u32);
                builder.CreateStore(builder.CreateLoad(I64_TY(), r3), r2);
                break;
            }
            case ByteCodeOp::CopyRBAddrToRA:
            {
                const auto r0 = GEP64_PTR_PTR_I64(allocR, ip->a.u32);
                const auto r1 = GEP64(allocR, ip->b.u32);
                builder.CreateStore(r1, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::ClearRA:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                builder.CreateStore(pp.cstAi64, r0);
                break;
            }
            case ByteCodeOp::ClearRAx2:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                builder.CreateStore(pp.cstAi64, r0);
                const auto r1 = GEP64(allocR, ip->b.u32);
                builder.CreateStore(pp.cstAi64, r1);
                break;
            }
            case ByteCodeOp::ClearRAx3:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                builder.CreateStore(pp.cstAi64, r0);
                const auto r1 = GEP64(allocR, ip->b.u32);
                builder.CreateStore(pp.cstAi64, r1);
                const auto r2 = GEP64(allocR, ip->c.u32);
                builder.CreateStore(pp.cstAi64, r2);
                break;
            }
            case ByteCodeOp::ClearRAx4:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                builder.CreateStore(pp.cstAi64, r0);
                const auto r1 = GEP64(allocR, ip->b.u32);
                builder.CreateStore(pp.cstAi64, r1);
                const auto r2 = GEP64(allocR, ip->c.u32);
                builder.CreateStore(pp.cstAi64, r2);
                const auto r3 = GEP64(allocR, ip->d.u32);
                builder.CreateStore(pp.cstAi64, r3);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::DecrementRA32:
            case ByteCodeOp::DecrementRA64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto v0      = builder.CreateSub(builder.CreateLoad(IX_TY(numBits), r0), builder.getIntN(numBits, 1));
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::IncrementRA64:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto v0 = builder.CreateAdd(builder.CreateLoad(I64_TY(), r0), builder.getInt64(1));
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::MulAddVC64:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = GEP64(allocR, ip->b.u32);
                const auto v1 = builder.CreateAdd(builder.CreateLoad(I64_TY(), r1), CST_RC64);
                const auto v2 = builder.CreateMul(builder.CreateLoad(I64_TY(), r0), v1);
                const auto r2 = GEP64(allocR, ip->a.u32);
                builder.CreateStore(v2, r2);
                break;
            }

            case ByteCodeOp::Add32byVB32:
            case ByteCodeOp::Add64byVB64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto v0      = builder.CreateAdd(builder.CreateLoad(IX_TY(numBits), r0), CST_RB(numBits));
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetAtPointer8:
            case ByteCodeOp::SetAtPointer16:
            case ByteCodeOp::SetAtPointer32:
            case ByteCodeOp::SetAtPointer64:
            {
                const auto   numBits = BackendEncoder::getNumBits(ip->op);
                const auto   ra      = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto   r0      = GEP8_PTR_IX(ra, ip->c.u32, numBits);
                llvm::Value* r1      = MK_IMMB_IX(numBits);
                builder.CreateStore(r1, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8:
            case ByteCodeOp::SetAtStackPointer16:
            case ByteCodeOp::SetAtStackPointer32:
            case ByteCodeOp::SetAtStackPointer64:
            {
                const auto   numBits = BackendEncoder::getNumBits(ip->op);
                const auto   r0      = GEP8_PTR_IX(allocStack, ip->a.u32, numBits);
                llvm::Value* r1      = MK_IMMB_IX(numBits);
                builder.CreateStore(r1, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8x2:
            case ByteCodeOp::SetAtStackPointer16x2:
            case ByteCodeOp::SetAtStackPointer32x2:
            case ByteCodeOp::SetAtStackPointer64x2:
            {
                const auto   numBits = BackendEncoder::getNumBits(ip->op);
                const auto   r0      = GEP8_PTR_IX(allocStack, ip->a.u32, numBits);
                llvm::Value* r1      = MK_IMMB_IX(numBits);
                builder.CreateStore(r1, r0);
                const auto   r2 = GEP8_PTR_IX(allocStack, ip->c.u32, numBits);
                llvm::Value* r3 = MK_IMMD_IX(numBits);
                builder.CreateStore(r3, r2);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpPlusS8:
            case ByteCodeOp::BinOpPlusS8_Safe:
            {
                MK_BINOP8_CAB();
                OP_OVERFLOW(sadd_with_overflow, CreateAdd, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS8), true);
                break;
            }
            case ByteCodeOp::BinOpPlusU8:
            case ByteCodeOp::BinOpPlusU8_Safe:
            {
                MK_BINOP8_CAB();
                OP_OVERFLOW(uadd_with_overflow, CreateAdd, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU8), false);
                break;
            }
            case ByteCodeOp::BinOpPlusS16:
            case ByteCodeOp::BinOpPlusS16_Safe:
            {
                MK_BINOP16_CAB();
                OP_OVERFLOW(sadd_with_overflow, CreateAdd, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS16), true);
                break;
            }
            case ByteCodeOp::BinOpPlusU16:
            case ByteCodeOp::BinOpPlusU16_Safe:
            {
                MK_BINOP16_CAB();
                OP_OVERFLOW(uadd_with_overflow, CreateAdd, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU16), false);
                break;
            }
            case ByteCodeOp::BinOpPlusS32:
            case ByteCodeOp::BinOpPlusS32_Safe:
            {
                MK_BINOP32_CAB();
                OP_OVERFLOW(sadd_with_overflow, CreateAdd, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS32), true);
                break;
            }
            case ByteCodeOp::BinOpPlusU32:
            case ByteCodeOp::BinOpPlusU32_Safe:
            {
                MK_BINOP32_CAB();
                OP_OVERFLOW(uadd_with_overflow, CreateAdd, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU32), false);
                break;
            }
            case ByteCodeOp::BinOpPlusS64:
            case ByteCodeOp::BinOpPlusS64_Safe:
            {
                MK_BINOP64_CAB();
                OP_OVERFLOW(sadd_with_overflow, CreateAdd, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS64), true);
                break;
            }
            case ByteCodeOp::BinOpPlusU64:
            case ByteCodeOp::BinOpPlusU64_Safe:
            {
                MK_BINOP64_CAB();
                OP_OVERFLOW(uadd_with_overflow, CreateAdd, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU64), false);
                break;
            }
            case ByteCodeOp::BinOpPlusF32:
            {
                MK_BINOPF32_CAB();
                auto v0 = builder.CreateFAdd(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpPlusF64:
            {
                MK_BINOPF64_CAB();
                auto v0 = builder.CreateFAdd(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpMinusS8:
            case ByteCodeOp::BinOpMinusS8_Safe:
            {
                MK_BINOP8_CAB();
                OP_OVERFLOW(ssub_with_overflow, CreateSub, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS8), true);
                break;
            }
            case ByteCodeOp::BinOpMinusS16:
            case ByteCodeOp::BinOpMinusS16_Safe:
            {
                MK_BINOP16_CAB();
                OP_OVERFLOW(ssub_with_overflow, CreateSub, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS16), true);
                break;
            }
            case ByteCodeOp::BinOpMinusS32:
            case ByteCodeOp::BinOpMinusS32_Safe:
            {
                MK_BINOP32_CAB();
                OP_OVERFLOW(ssub_with_overflow, CreateSub, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS32), true);
                break;
            }
            case ByteCodeOp::BinOpMinusS64:
            case ByteCodeOp::BinOpMinusS64_Safe:
            {
                MK_BINOP64_CAB();
                OP_OVERFLOW(ssub_with_overflow, CreateSub, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS64), true);
                break;
            }
            case ByteCodeOp::BinOpMinusU8:
            case ByteCodeOp::BinOpMinusU8_Safe:
            {
                MK_BINOP8_CAB();
                OP_OVERFLOW(usub_with_overflow, CreateSub, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU8), false);
                break;
            }
            case ByteCodeOp::BinOpMinusU16:
            case ByteCodeOp::BinOpMinusU16_Safe:
            {
                MK_BINOP16_CAB();
                OP_OVERFLOW(usub_with_overflow, CreateSub, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU16), false);
                break;
            }
            case ByteCodeOp::BinOpMinusU32:
            case ByteCodeOp::BinOpMinusU32_Safe:
            {
                MK_BINOP32_CAB();
                OP_OVERFLOW(usub_with_overflow, CreateSub, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU32), false);
                break;
            }
            case ByteCodeOp::BinOpMinusU64:
            case ByteCodeOp::BinOpMinusU64_Safe:
            {
                MK_BINOP64_CAB();
                OP_OVERFLOW(usub_with_overflow, CreateSub, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU64), false);
                break;
            }
            case ByteCodeOp::BinOpMinusF32_Safe:
            {
                MK_BINOPF32_CAB();
                auto v0 = builder.CreateFSub(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpMinusF64_Safe:
            {
                MK_BINOPF64_CAB();
                auto v0 = builder.CreateFSub(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpMulS8:
            case ByteCodeOp::BinOpMulS8_Safe:
            {
                MK_BINOP8_CAB();
                OP_OVERFLOW(smul_with_overflow, CreateMul, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS8), true);
                break;
            }
            case ByteCodeOp::BinOpMulS16:
            case ByteCodeOp::BinOpMulS16_Safe:
            {
                MK_BINOP16_CAB();
                OP_OVERFLOW(smul_with_overflow, CreateMul, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS16), true);
                break;
            }
            case ByteCodeOp::BinOpMulS32:
            case ByteCodeOp::BinOpMulS32_Safe:
            {
                MK_BINOP32_CAB();
                OP_OVERFLOW(smul_with_overflow, CreateMul, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS32), true);
                break;
            }
            case ByteCodeOp::BinOpMulS64:
            case ByteCodeOp::BinOpMulS64_Safe:
            {
                MK_BINOP64_CAB();
                OP_OVERFLOW(smul_with_overflow, CreateMul, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS64), true);
                break;
            }
            case ByteCodeOp::BinOpMulU8:
            case ByteCodeOp::BinOpMulU8_Safe:
            {
                MK_BINOP8_CAB();
                OP_OVERFLOW(umul_with_overflow, CreateMul, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU8), false);
                break;
            }
            case ByteCodeOp::BinOpMulU16:
            case ByteCodeOp::BinOpMulU16_Safe:
            {
                MK_BINOP16_CAB();
                OP_OVERFLOW(umul_with_overflow, CreateMul, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU16), false);
                break;
            }
            case ByteCodeOp::BinOpMulU32:
            case ByteCodeOp::BinOpMulU32_Safe:
            {
                MK_BINOP32_CAB();
                OP_OVERFLOW(umul_with_overflow, CreateMul, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU32), false);
                break;
            }
            case ByteCodeOp::BinOpMulU64:
            case ByteCodeOp::BinOpMulU64_Safe:
            {
                MK_BINOP64_CAB();
                OP_OVERFLOW(umul_with_overflow, CreateMul, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU64), false);
                break;
            }
            case ByteCodeOp::BinOpMulF32:
            {
                MK_BINOPF32_CAB();
                auto v0 = builder.CreateFMul(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpMulF64:
            {
                MK_BINOPF64_CAB();
                auto v0 = builder.CreateFMul(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpXorU8:
            {
                MK_BINOP8_CAB();
                auto v0 = builder.CreateXor(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpXorU16:
            {
                MK_BINOP16_CAB();
                auto v0 = builder.CreateXor(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpXorU32:
            {
                MK_BINOP32_CAB();
                auto v0 = builder.CreateXor(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpXorU64:
            {
                MK_BINOP64_CAB();
                auto v0 = builder.CreateXor(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpShiftLeftS8:
            case ByteCodeOp::BinOpShiftLeftU8:
            case ByteCodeOp::BinOpShiftLeftS16:
            case ByteCodeOp::BinOpShiftLeftU16:
            case ByteCodeOp::BinOpShiftLeftS32:
            case ByteCodeOp::BinOpShiftLeftU32:
            case ByteCodeOp::BinOpShiftLeftS64:
            case ByteCodeOp::BinOpShiftLeftU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                emitShiftLogical(context, builder, allocR, ip, numBits, true);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpShiftRightU8:
            case ByteCodeOp::BinOpShiftRightU16:
            case ByteCodeOp::BinOpShiftRightU32:
            case ByteCodeOp::BinOpShiftRightU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                emitShiftLogical(context, builder, allocR, ip, numBits, false);
                break;
            }

            case ByteCodeOp::BinOpShiftRightS8:
            case ByteCodeOp::BinOpShiftRightS16:
            case ByteCodeOp::BinOpShiftRightS32:
            case ByteCodeOp::BinOpShiftRightS64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                emitShiftRightArithmetic(context, builder, allocR, ip, numBits);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpModuloS8:
            case ByteCodeOp::BinOpModuloS16:
            case ByteCodeOp::BinOpModuloS32:
            case ByteCodeOp::BinOpModuloS64:
            {
                const auto   numBits = BackendEncoder::getNumBits(ip->op);
                auto         r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                llvm::Value* r1      = MK_IMMA_IX(numBits);
                llvm::Value* r2      = MK_IMMB_IX(numBits);
                auto         v0      = builder.CreateSRem(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::BinOpModuloU8:
            case ByteCodeOp::BinOpModuloU16:
            case ByteCodeOp::BinOpModuloU32:
            case ByteCodeOp::BinOpModuloU64:
            {
                const auto   numBits = BackendEncoder::getNumBits(ip->op);
                auto         r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                llvm::Value* r1      = MK_IMMA_IX(numBits);
                llvm::Value* r2      = MK_IMMB_IX(numBits);
                auto         v0      = builder.CreateURem(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpDivS8:
            case ByteCodeOp::BinOpDivS16:
            case ByteCodeOp::BinOpDivS32:
            case ByteCodeOp::BinOpDivS64:
            {
                const auto   numBits = BackendEncoder::getNumBits(ip->op);
                auto         r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                llvm::Value* r1      = MK_IMMA_IX(numBits);
                llvm::Value* r2      = MK_IMMB_IX(numBits);
                auto         v0      = builder.CreateSDiv(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::BinOpDivU8:
            case ByteCodeOp::BinOpDivU16:
            case ByteCodeOp::BinOpDivU32:
            case ByteCodeOp::BinOpDivU64:
            {
                const auto   numBits = BackendEncoder::getNumBits(ip->op);
                auto         r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                llvm::Value* r1      = MK_IMMA_IX(numBits);
                llvm::Value* r2      = MK_IMMB_IX(numBits);
                auto         v0      = builder.CreateUDiv(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::BinOpDivF32:
            case ByteCodeOp::BinOpDivF64:
            {
                const auto   numBits = BackendEncoder::getNumBits(ip->op);
                auto         r0 = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                llvm::Value* r1 = MK_IMMA_FX(numBits);
                llvm::Value* r2 = MK_IMMB_FX(numBits);
                auto         v0 = builder.CreateFDiv(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskAnd8:
            {
                MK_BINOP8_CAB();
                auto v0 = builder.CreateAnd(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpBitmaskAnd16:
            {
                MK_BINOP16_CAB();
                auto v0 = builder.CreateAnd(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpBitmaskAnd32:
            {
                MK_BINOP32_CAB();
                auto v0 = builder.CreateAnd(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpBitmaskAnd64:
            {
                MK_BINOP64_CAB();
                auto v0 = builder.CreateAnd(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskOr8:
            {
                MK_BINOP8_CAB();
                auto v0 = builder.CreateOr(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpBitmaskOr16:
            {
                MK_BINOP16_CAB();
                auto v0 = builder.CreateOr(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpBitmaskOr32:
            {
                MK_BINOP32_CAB();
                auto v0 = builder.CreateOr(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::BinOpBitmaskOr64:
            {
                MK_BINOP64_CAB();
                auto v0 = builder.CreateOr(r1, r2);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMinusEqS8:
            {
                MK_BINOP_EQ8_CAB();
                OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS8), true);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqS16:
            {
                MK_BINOP_EQ16_CAB();
                OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS16), true);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqS32:
            {
                MK_BINOP_EQ32_CAB();
                OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS32), true);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqS64:
            {
                MK_BINOP_EQ64_CAB();
                OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS64), true);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqU8:
            {
                MK_BINOP_EQ8_CAB();
                OPEQ_OVERFLOW(usub_with_overflow, CreateSub, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU8), false);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqU16:
            {
                MK_BINOP_EQ16_CAB();
                OPEQ_OVERFLOW(usub_with_overflow, CreateSub, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16), false);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqU32:
            {
                MK_BINOP_EQ32_CAB();
                OPEQ_OVERFLOW(usub_with_overflow, CreateSub, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU32), false);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqU64:
            {
                MK_BINOP_EQ64_CAB();
                OPEQ_OVERFLOW(usub_with_overflow, CreateSub, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU64), false);
                break;
            }

            case ByteCodeOp::AffectOpMinusEqS8_Safe:
            {
                MK_BINOP_EQ8_CAB_OFF();
                OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS8), true);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqS16_Safe:
            {
                MK_BINOP_EQ16_CAB_OFF();
                OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS16), true);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqS32_Safe:
            {
                MK_BINOP_EQ32_CAB_OFF();
                OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS32), true);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqS64_Safe:
            {
                MK_BINOP_EQ64_CAB_OFF();
                OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS64), true);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqU8_Safe:
            {
                MK_BINOP_EQ8_CAB_OFF();
                OPEQ_OVERFLOW(usub_with_overflow, CreateSub, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU8), false);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqU16_Safe:
            {
                MK_BINOP_EQ16_CAB_OFF();
                OPEQ_OVERFLOW(usub_with_overflow, CreateSub, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16), false);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqU32_Safe:
            {
                MK_BINOP_EQ32_CAB_OFF();
                OPEQ_OVERFLOW(usub_with_overflow, CreateSub, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU32), false);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqU64_Safe:
            {
                MK_BINOP_EQ64_CAB_OFF();
                OPEQ_OVERFLOW(usub_with_overflow, CreateSub, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU64), false);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqF32_Safe:
            {
                MK_BINOP_EQF32_CAB_OFF();
                auto v0 = builder.CreateFSub(builder.CreateLoad(F32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqF64_Safe:
            {
                MK_BINOP_EQF64_CAB_OFF();
                auto v0 = builder.CreateFSub(builder.CreateLoad(F64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

            case ByteCodeOp::AffectOpMinusEqS8_SSafe:
            case ByteCodeOp::AffectOpMinusEqU8_SSafe:
            {
                MK_BINOP_EQ8_SCAB();
                auto v0 = builder.CreateSub(builder.CreateLoad(I8_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqS16_SSafe:
            case ByteCodeOp::AffectOpMinusEqU16_SSafe:
            {
                MK_BINOP_EQ16_SCAB();
                auto v0 = builder.CreateSub(builder.CreateLoad(I16_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqS32_SSafe:
            case ByteCodeOp::AffectOpMinusEqU32_SSafe:
            {
                MK_BINOP_EQ32_SCAB();
                auto v0 = builder.CreateSub(builder.CreateLoad(I32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqS64_SSafe:
            case ByteCodeOp::AffectOpMinusEqU64_SSafe:
            {
                MK_BINOP_EQ64_SCAB();
                auto v0 = builder.CreateSub(builder.CreateLoad(I64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqF32_SSafe:
            {
                MK_BINOP_EQF32_SCAB();
                auto v0 = builder.CreateFSub(builder.CreateLoad(F32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpMinusEqF64_SSafe:
            {
                MK_BINOP_EQF64_SCAB();
                auto v0 = builder.CreateFSub(builder.CreateLoad(F64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpPlusEqS8:
            {
                MK_BINOP_EQ8_CAB();
                OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS8), true);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqS16:
            {
                MK_BINOP_EQ16_CAB();
                OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS16), true);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqS32:
            {
                MK_BINOP_EQ32_CAB();
                OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS32), true);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqS64:
            {
                MK_BINOP_EQ64_CAB();
                OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS64), true);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqU8:
            {
                MK_BINOP_EQ8_CAB();
                OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU8), false);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqU16:
            {
                MK_BINOP_EQ16_CAB();
                OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU16), false);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqU32:
            {
                MK_BINOP_EQ32_CAB();
                OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU32), false);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqU64:
            {
                MK_BINOP_EQ64_CAB();
                OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU64), false);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqF32:
            {
                MK_BINOP_EQF32_CAB_OFF();
                auto v0 = builder.CreateFAdd(builder.CreateLoad(F32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqF64:
            {
                MK_BINOP_EQF64_CAB_OFF();
                auto v0 = builder.CreateFAdd(builder.CreateLoad(F64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

            case ByteCodeOp::AffectOpPlusEqS8_Safe:
            {
                MK_BINOP_EQ8_CAB_OFF();
                OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS8), true);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqS16_Safe:
            {
                MK_BINOP_EQ16_CAB_OFF();
                OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS16), true);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqS32_Safe:
            {
                MK_BINOP_EQ32_CAB_OFF();
                OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS32), true);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqS64_Safe:
            {
                MK_BINOP_EQ64_CAB_OFF();
                OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS64), true);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqU8_Safe:
            {
                MK_BINOP_EQ8_CAB_OFF();
                OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU8), false);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqU16_Safe:
            {
                MK_BINOP_EQ16_CAB_OFF();
                OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU16), false);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqU32_Safe:
            {
                MK_BINOP_EQ32_CAB_OFF();
                OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU32), false);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqU64_Safe:
            {
                MK_BINOP_EQ64_CAB_OFF();
                OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU64), false);
                break;
            }

            case ByteCodeOp::AffectOpPlusEqF32_SSafe:
            {
                MK_BINOP_EQF32_SCAB();
                auto v0 = builder.CreateFAdd(builder.CreateLoad(F32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqF64_SSafe:
            {
                MK_BINOP_EQF64_SCAB();
                auto v0 = builder.CreateFAdd(builder.CreateLoad(F64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqS8_SSafe:
            case ByteCodeOp::AffectOpPlusEqU8_SSafe:
            {
                MK_BINOP_EQ8_SCAB();
                auto v0 = builder.CreateAdd(builder.CreateLoad(I8_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqS16_SSafe:
            case ByteCodeOp::AffectOpPlusEqU16_SSafe:
            {
                MK_BINOP_EQ16_SCAB();
                auto v0 = builder.CreateAdd(builder.CreateLoad(I16_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqS32_SSafe:
            case ByteCodeOp::AffectOpPlusEqU32_SSafe:
            {
                MK_BINOP_EQ32_SCAB();
                auto v0 = builder.CreateAdd(builder.CreateLoad(I32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpPlusEqS64_SSafe:
            case ByteCodeOp::AffectOpPlusEqU64_SSafe:
            {
                MK_BINOP_EQ64_SCAB();
                auto v0 = builder.CreateAdd(builder.CreateLoad(I64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMulEqS8:
            {
                MK_BINOP_EQ8_CAB();
                OPEQ_OVERFLOW(smul_with_overflow, CreateMul, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS8), true);
                break;
            }
            case ByteCodeOp::AffectOpMulEqS16:
            {
                MK_BINOP_EQ16_CAB();
                OPEQ_OVERFLOW(smul_with_overflow, CreateMul, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS16), true);
                break;
            }
            case ByteCodeOp::AffectOpMulEqS32:
            {
                MK_BINOP_EQ32_CAB();
                OPEQ_OVERFLOW(smul_with_overflow, CreateMul, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS32), true);
                break;
            }
            case ByteCodeOp::AffectOpMulEqS64:
            {
                MK_BINOP_EQ64_CAB();
                OPEQ_OVERFLOW(smul_with_overflow, CreateMul, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS64), true);
                break;
            }
            case ByteCodeOp::AffectOpMulEqU8:
            {
                MK_BINOP_EQ8_CAB();
                OPEQ_OVERFLOW(umul_with_overflow, CreateMul, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU8), false);
                break;
            }
            case ByteCodeOp::AffectOpMulEqU16:
            {
                MK_BINOP_EQ16_CAB();
                OPEQ_OVERFLOW(umul_with_overflow, CreateMul, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU16), false);
                break;
            }
            case ByteCodeOp::AffectOpMulEqU32:
            {
                MK_BINOP_EQ32_CAB();
                OPEQ_OVERFLOW(umul_with_overflow, CreateMul, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU32), false);
                break;
            }
            case ByteCodeOp::AffectOpMulEqU64:
            {
                MK_BINOP_EQ64_CAB();
                OPEQ_OVERFLOW(umul_with_overflow, CreateMul, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU64), false);
                break;
            }

            case ByteCodeOp::AffectOpMulEqF32_Safe:
            {
                MK_BINOP_EQF32_CAB_OFF();
                auto v0 = builder.CreateFMul(builder.CreateLoad(F32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpMulEqF64_Safe:
            {
                MK_BINOP_EQF64_CAB_OFF();
                auto v0 = builder.CreateFMul(builder.CreateLoad(F64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

            case ByteCodeOp::AffectOpMulEqS8_Safe:
            {
                MK_BINOP_EQ8_CAB_OFF();
                OPEQ_OVERFLOW(smul_with_overflow, CreateMul, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS8), true);
                break;
            }
            case ByteCodeOp::AffectOpMulEqS16_Safe:
            {
                MK_BINOP_EQ16_CAB_OFF();
                OPEQ_OVERFLOW(smul_with_overflow, CreateMul, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS16), true);
                break;
            }
            case ByteCodeOp::AffectOpMulEqS32_Safe:
            {
                MK_BINOP_EQ32_CAB_OFF();
                OPEQ_OVERFLOW(smul_with_overflow, CreateMul, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS32), true);
                break;
            }
            case ByteCodeOp::AffectOpMulEqS64_Safe:
            {
                MK_BINOP_EQ64_CAB_OFF();
                OPEQ_OVERFLOW(smul_with_overflow, CreateMul, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS64), true);
                break;
            }
            case ByteCodeOp::AffectOpMulEqU8_Safe:
            {
                MK_BINOP_EQ8_CAB_OFF();
                OPEQ_OVERFLOW(umul_with_overflow, CreateMul, I8_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU8), false);
                break;
            }
            case ByteCodeOp::AffectOpMulEqU16_Safe:
            {
                MK_BINOP_EQ16_CAB_OFF();
                OPEQ_OVERFLOW(umul_with_overflow, CreateMul, I16_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU16), false);
                break;
            }
            case ByteCodeOp::AffectOpMulEqU32_Safe:
            {
                MK_BINOP_EQ32_CAB_OFF();
                OPEQ_OVERFLOW(umul_with_overflow, CreateMul, I32_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU32), false);
                break;
            }
            case ByteCodeOp::AffectOpMulEqU64_Safe:
            {
                MK_BINOP_EQ64_CAB_OFF();
                OPEQ_OVERFLOW(umul_with_overflow, CreateMul, I64_TY(), ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU64), false);
                break;
            }

            case ByteCodeOp::AffectOpMulEqF32_SSafe:
            {
                MK_BINOP_EQF32_SCAB();
                auto v0 = builder.CreateFMul(builder.CreateLoad(F32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpMulEqF64_SSafe:
            {
                MK_BINOP_EQF64_SCAB();
                auto v0 = builder.CreateFMul(builder.CreateLoad(F64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpMulEqS8_SSafe:
            case ByteCodeOp::AffectOpMulEqU8_SSafe:
            {
                MK_BINOP_EQ8_SCAB();
                auto v0 = builder.CreateMul(builder.CreateLoad(I8_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpMulEqS16_SSafe:
            case ByteCodeOp::AffectOpMulEqU16_SSafe:
            {
                MK_BINOP_EQ16_SCAB();
                auto v0 = builder.CreateMul(builder.CreateLoad(I16_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpMulEqS32_SSafe:
            case ByteCodeOp::AffectOpMulEqU32_SSafe:
            {
                MK_BINOP_EQ32_SCAB();
                auto v0 = builder.CreateMul(builder.CreateLoad(I32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpMulEqS64_SSafe:
            case ByteCodeOp::AffectOpMulEqU64_SSafe:
            {
                MK_BINOP_EQ64_SCAB();
                auto v0 = builder.CreateMul(builder.CreateLoad(I64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpDivEqS8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateSDiv(builder.CreateLoad(I8_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpDivEqS16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateSDiv(builder.CreateLoad(I16_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpDivEqS32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateSDiv(builder.CreateLoad(I32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpDivEqS64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateSDiv(builder.CreateLoad(I64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpDivEqU8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateUDiv(builder.CreateLoad(I8_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpDivEqU16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateUDiv(builder.CreateLoad(I16_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpDivEqU32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateUDiv(builder.CreateLoad(I32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpDivEqU64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateUDiv(builder.CreateLoad(I64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpDivEqF32:
            {
                MK_BINOP_EQF32_CAB();
                auto v0 = builder.CreateFDiv(builder.CreateLoad(F32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpDivEqF64:
            {
                MK_BINOP_EQF64_CAB();
                auto v0 = builder.CreateFDiv(builder.CreateLoad(F64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

            case ByteCodeOp::AffectOpDivEqS8_SSafe:
            {
                MK_BINOP_EQ8_SCAB();
                auto v0 = builder.CreateSDiv(builder.CreateLoad(I8_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpDivEqS16_SSafe:
            {
                MK_BINOP_EQ16_SCAB();
                auto v0 = builder.CreateSDiv(builder.CreateLoad(I16_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpDivEqS32_SSafe:
            {
                MK_BINOP_EQ32_SCAB();
                auto v0 = builder.CreateSDiv(builder.CreateLoad(I32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpDivEqS64_SSafe:
            {
                MK_BINOP_EQ64_SCAB();
                auto v0 = builder.CreateSDiv(builder.CreateLoad(I64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpDivEqU8_SSafe:
            {
                MK_BINOP_EQ8_SCAB();
                auto v0 = builder.CreateUDiv(builder.CreateLoad(I8_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpDivEqU16_SSafe:
            {
                MK_BINOP_EQ16_SCAB();
                auto v0 = builder.CreateUDiv(builder.CreateLoad(I16_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::AffectOpDivEqU32_SSafe:
            {
                MK_BINOP_EQ32_SCAB();
                auto v0 = builder.CreateUDiv(builder.CreateLoad(I32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpDivEqU64_SSafe:
            {
                MK_BINOP_EQ64_SCAB();
                auto v0 = builder.CreateUDiv(builder.CreateLoad(I64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpDivEqF32_SSafe:
            {
                MK_BINOP_EQF32_SCAB();
                auto v0 = builder.CreateFDiv(builder.CreateLoad(F32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpDivEqF64_SSafe:
            {
                MK_BINOP_EQF64_SCAB();
                auto v0 = builder.CreateFDiv(builder.CreateLoad(F64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpModuloEqS8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateSRem(builder.CreateLoad(I8_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqS16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateSRem(builder.CreateLoad(I16_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqS32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateSRem(builder.CreateLoad(I32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqS64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateSRem(builder.CreateLoad(I64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqU8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateURem(builder.CreateLoad(I8_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqU16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateURem(builder.CreateLoad(I16_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqU32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateURem(builder.CreateLoad(I32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqU64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateURem(builder.CreateLoad(I64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

            case ByteCodeOp::AffectOpModuloEqS8_SSafe:
            {
                MK_BINOP_EQ8_SCAB();
                auto v0 = builder.CreateSRem(builder.CreateLoad(I8_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqS16_SSafe:
            {
                MK_BINOP_EQ16_SCAB();
                auto v0 = builder.CreateSRem(builder.CreateLoad(I16_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqS32_SSafe:
            {
                MK_BINOP_EQ32_SCAB();
                auto v0 = builder.CreateSRem(builder.CreateLoad(I32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqS64_SSafe:
            {
                MK_BINOP_EQ64_SCAB();
                auto v0 = builder.CreateSRem(builder.CreateLoad(I64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqU8_SSafe:
            {
                MK_BINOP_EQ8_SCAB();
                auto v0 = builder.CreateURem(builder.CreateLoad(I8_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqU16_SSafe:
            {
                MK_BINOP_EQ16_SCAB();
                auto v0 = builder.CreateURem(builder.CreateLoad(I16_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqU32_SSafe:
            {
                MK_BINOP_EQ32_SCAB();
                auto v0 = builder.CreateURem(builder.CreateLoad(I32_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::AffectOpModuloEqU64_SSafe:
            {
                MK_BINOP_EQ64_SCAB();
                auto v0 = builder.CreateURem(builder.CreateLoad(I64_TY(), r0), r1);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpAndEqU8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateAnd(builder.CreateLoad(I8_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpAndEqU16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateAnd(builder.CreateLoad(I16_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpAndEqU32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateAnd(builder.CreateLoad(I32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpAndEqU64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateAnd(builder.CreateLoad(I64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpOrEqU8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateOr(builder.CreateLoad(I8_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpOrEqU16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateOr(builder.CreateLoad(I16_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpOrEqU32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateOr(builder.CreateLoad(I32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }
            case ByteCodeOp::AffectOpOrEqU64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateOr(builder.CreateLoad(I64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpShiftLeftEqS8:
                emitShiftEqLogical(context, builder, allocR, ip, 8, true);
                break;
            case ByteCodeOp::AffectOpShiftLeftEqU8:
                emitShiftEqLogical(context, builder, allocR, ip, 8, true);
                break;
            case ByteCodeOp::AffectOpShiftLeftEqS16:
                emitShiftEqLogical(context, builder, allocR, ip, 16, true);
                break;
            case ByteCodeOp::AffectOpShiftLeftEqU16:
                emitShiftEqLogical(context, builder, allocR, ip, 16, true);
                break;
            case ByteCodeOp::AffectOpShiftLeftEqS32:
                emitShiftEqLogical(context, builder, allocR, ip, 32, true);
                break;
            case ByteCodeOp::AffectOpShiftLeftEqU32:
                emitShiftEqLogical(context, builder, allocR, ip, 32, true);
                break;
            case ByteCodeOp::AffectOpShiftLeftEqS64:
                emitShiftEqLogical(context, builder, allocR, ip, 64, true);
                break;
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
                emitShiftRightEqArithmetic(context, builder, allocR, ip, 8);
                break;
            case ByteCodeOp::AffectOpShiftRightEqS16:
                emitShiftRightEqArithmetic(context, builder, allocR, ip, 16);
                break;
            case ByteCodeOp::AffectOpShiftRightEqS32:
                emitShiftRightEqArithmetic(context, builder, allocR, ip, 32);
                break;
            case ByteCodeOp::AffectOpShiftRightEqS64:
                emitShiftRightEqArithmetic(context, builder, allocR, ip, 64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpXorEqU8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateXor(builder.CreateLoad(I8_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

            case ByteCodeOp::AffectOpXorEqU16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateXor(builder.CreateLoad(I16_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

            case ByteCodeOp::AffectOpXorEqU32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateXor(builder.CreateLoad(I32_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

            case ByteCodeOp::AffectOpXorEqU64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateXor(builder.CreateLoad(I64_TY(), r1), r2);
                builder.CreateStore(v0, r1);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CompareOpGreaterS8:
            {
                MK_BINOP8_CAB8();
                auto v0 = builder.CreateICmpSGT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterS16:
            {
                MK_BINOP16_CAB8();
                auto v0 = builder.CreateICmpSGT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterS32:
            {
                MK_BINOP32_CAB8();
                auto v0 = builder.CreateICmpSGT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterS64:
            {
                MK_BINOP64_CAB8();
                auto v0 = builder.CreateICmpSGT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterU8:
            {
                MK_BINOP8_CAB8();
                auto v0 = builder.CreateICmpUGT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterU16:
            {
                MK_BINOP16_CAB8();
                auto v0 = builder.CreateICmpUGT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterU32:
            {
                MK_BINOP32_CAB8();
                auto v0 = builder.CreateICmpUGT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterU64:
            {
                MK_BINOP64_CAB8();
                auto v0 = builder.CreateICmpUGT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterF32:
            {
                MK_BINOPF32_CAB8();
                auto v0 = builder.CreateFCmpUGT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterF64:
            {
                MK_BINOPF64_CAB8();
                auto v0 = builder.CreateFCmpUGT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::CompareOpGreaterEqS8:
            {
                MK_BINOP8_CAB8();
                auto v0 = builder.CreateICmpSGE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterEqS16:
            {
                MK_BINOP16_CAB8();
                auto v0 = builder.CreateICmpSGE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterEqS32:
            {
                MK_BINOP32_CAB8();
                auto v0 = builder.CreateICmpSGE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterEqS64:
            {
                MK_BINOP64_CAB8();
                auto v0 = builder.CreateICmpSGE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterEqU8:
            {
                MK_BINOP8_CAB8();
                auto v0 = builder.CreateICmpUGE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterEqU16:
            {
                MK_BINOP16_CAB8();
                auto v0 = builder.CreateICmpUGE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterEqU32:
            {
                MK_BINOP32_CAB8();
                auto v0 = builder.CreateICmpUGE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterEqU64:
            {
                MK_BINOP64_CAB8();
                auto v0 = builder.CreateICmpUGE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterEqF32:
            {
                MK_BINOPF32_CAB8();
                auto v0 = builder.CreateFCmpUGE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpGreaterEqF64:
            {
                MK_BINOPF64_CAB8();
                auto v0 = builder.CreateFCmpUGE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::CompareOpLowerS8:
            {
                MK_BINOP8_CAB8();
                auto v0 = builder.CreateICmpSLT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerS16:
            {
                MK_BINOP16_CAB8();
                auto v0 = builder.CreateICmpSLT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerS32:
            {
                MK_BINOP32_CAB8();
                auto v0 = builder.CreateICmpSLT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerS64:
            {
                MK_BINOP64_CAB8();
                auto v0 = builder.CreateICmpSLT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerU8:
            {
                MK_BINOP8_CAB8();
                auto v0 = builder.CreateICmpULT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerU16:
            {
                MK_BINOP16_CAB8();
                auto v0 = builder.CreateICmpULT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerU32:
            {
                MK_BINOP32_CAB8();
                auto v0 = builder.CreateICmpULT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerU64:
            {
                MK_BINOP64_CAB8();
                auto v0 = builder.CreateICmpULT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerF32:
            {
                MK_BINOPF32_CAB8();
                auto v0 = builder.CreateFCmpULT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerF64:
            {
                MK_BINOPF64_CAB8();
                auto v0 = builder.CreateFCmpULT(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::CompareOpLowerEqS8:
            {
                MK_BINOP8_CAB8();
                auto v0 = builder.CreateICmpSLE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerEqS16:
            {
                MK_BINOP16_CAB8();
                auto v0 = builder.CreateICmpSLE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerEqS32:
            {
                MK_BINOP32_CAB8();
                auto v0 = builder.CreateICmpSLE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerEqS64:
            {
                MK_BINOP64_CAB8();
                auto v0 = builder.CreateICmpSLE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerEqU8:
            {
                MK_BINOP8_CAB8();
                auto v0 = builder.CreateICmpULE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerEqU16:
            {
                MK_BINOP16_CAB8();
                auto v0 = builder.CreateICmpULE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerEqU32:
            {
                MK_BINOP32_CAB8();
                auto v0 = builder.CreateICmpULE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerEqU64:
            {
                MK_BINOP64_CAB8();
                auto v0 = builder.CreateICmpULE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerEqF32:
            {
                MK_BINOPF32_CAB8();
                auto v0 = builder.CreateFCmpULE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpLowerEqF64:
            {
                MK_BINOPF64_CAB8();
                auto v0 = builder.CreateFCmpULE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CompareOp3Way8:
            {
                auto         r0 = GEP64_PTR_I32(allocR, ip->c.u32);
                llvm::Value* r1 = MK_IMMA_8();
                llvm::Value* r2 = MK_IMMB_8();
                auto         v0 = builder.CreateSub(r1, r2);
                auto         v1 = builder.CreateIntCast(builder.CreateICmpSGT(v0, builder.getInt8(0)), I32_TY(), false);
                auto         v2 = builder.CreateIntCast(builder.CreateICmpSLT(v0, builder.getInt8(0)), I32_TY(), false);
                auto         v3 = builder.CreateSub(v1, v2);
                builder.CreateStore(v3, r0);
                break;
            }

            case ByteCodeOp::CompareOp3Way16:
            {
                auto         r0 = GEP64_PTR_I32(allocR, ip->c.u32);
                llvm::Value* r1 = MK_IMMA_16();
                llvm::Value* r2 = MK_IMMB_16();
                auto         v0 = builder.CreateSub(r1, r2);
                auto         v1 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cstAi16), I32_TY(), false);
                auto         v2 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cstAi16), I32_TY(), false);
                auto         v3 = builder.CreateSub(v1, v2);
                builder.CreateStore(v3, r0);
                break;
            }

            case ByteCodeOp::CompareOp3Way32:
            {
                auto         r0 = GEP64_PTR_I32(allocR, ip->c.u32);
                llvm::Value* r1 = MK_IMMA_32();
                llvm::Value* r2 = MK_IMMB_32();
                auto         v0 = builder.CreateSub(r1, r2);
                auto         v1 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cstAi32), I32_TY(), false);
                auto         v2 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cstAi32), I32_TY(), false);
                auto         v3 = builder.CreateSub(v1, v2);
                builder.CreateStore(v3, r0);
                break;
            }

            case ByteCodeOp::CompareOp3Way64:
            {
                auto         r0 = GEP64_PTR_I32(allocR, ip->c.u32);
                llvm::Value* r1 = MK_IMMA_64();
                llvm::Value* r2 = MK_IMMB_64();
                auto         v0 = builder.CreateSub(r1, r2);
                auto         v1 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cstAi64), I32_TY(), false);
                auto         v2 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cstAi64), I32_TY(), false);
                auto         v3 = builder.CreateSub(v1, v2);
                builder.CreateStore(v3, r0);
                break;
            }

            case ByteCodeOp::CompareOp3WayF32:
            {
                auto         r0 = GEP64_PTR_I32(allocR, ip->c.u32);
                llvm::Value* r1 = MK_IMMA_F32();
                llvm::Value* r2 = MK_IMMB_F32();
                auto         v0 = builder.CreateFSub(r1, r2);
                auto         v1 = builder.CreateIntCast(builder.CreateFCmpUGT(v0, pp.cstAf32), I32_TY(), false);
                auto         v2 = builder.CreateIntCast(builder.CreateFCmpULT(v0, pp.cstAf32), I32_TY(), false);
                auto         v3 = builder.CreateSub(v1, v2);
                builder.CreateStore(v3, r0);
                break;
            }
            case ByteCodeOp::CompareOp3WayF64:
            {
                auto         r0 = GEP64_PTR_I32(allocR, ip->c.u32);
                llvm::Value* r1 = MK_IMMA_F64();
                llvm::Value* r2 = MK_IMMB_F64();
                auto         v0 = builder.CreateFSub(r1, r2);
                auto         v1 = builder.CreateIntCast(builder.CreateFCmpUGT(v0, pp.cstAf64), I32_TY(), false);
                auto         v2 = builder.CreateIntCast(builder.CreateFCmpULT(v0, pp.cstAf64), I32_TY(), false);
                auto         v3 = builder.CreateSub(v1, v2);
                builder.CreateStore(v3, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CompareOpEqual8:
            {
                MK_BINOP8_CAB();
                auto v0 = builder.CreateICmpEQ(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpEqual16:
            {
                MK_BINOP16_CAB8();
                auto v0 = builder.CreateICmpEQ(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpEqual32:
            {
                MK_BINOP32_CAB8();
                auto v0 = builder.CreateICmpEQ(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpEqual64:
            {
                MK_BINOP64_CAB8();
                auto v0 = builder.CreateICmpEQ(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpEqualF32:
            {
                MK_BINOPF32_CAB8();
                auto v0 = builder.CreateFCmpOEQ(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpEqualF64:
            {
                MK_BINOPF64_CAB8();
                auto v0 = builder.CreateFCmpOEQ(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::CompareOpNotEqual8:
            {
                MK_BINOP8_CAB();
                auto v0 = builder.CreateICmpNE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpNotEqual16:
            {
                MK_BINOP16_CAB8();
                auto v0 = builder.CreateICmpNE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpNotEqual32:
            {
                MK_BINOP32_CAB8();
                auto v0 = builder.CreateICmpNE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpNotEqual64:
            {
                MK_BINOP64_CAB8();
                auto v0 = builder.CreateICmpNE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpNotEqualF32:
            {
                MK_BINOPF32_CAB8();
                auto v0 = builder.CreateFCmpUNE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CompareOpNotEqualF64:
            {
                MK_BINOPF64_CAB8();
                auto v0 = builder.CreateFCmpUNE(r1, r2);
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpDyn8:
            case ByteCodeOp::JumpDyn16:
            case ByteCodeOp::JumpDyn64:
            case ByteCodeOp::JumpDyn32:
            {
                auto tableCompiler = reinterpret_cast<int32_t*>(buildParameters.module->compilerSegment.address(ip->d.u32));

                VectorNative<llvm::BasicBlock*> falseBlocks;
                VectorNative<llvm::BasicBlock*> trueBlocks;

                for (uint32_t idx = 0; idx < ip->c.u32; idx++)
                {
                    if (idx == 0)
                        falseBlocks.push_back(nullptr);
                    else
                        falseBlocks.push_back(getOrCreateLabel(pp, func, static_cast<int64_t>(UINT32_MAX) + g_UniqueID.fetch_add(1)));
                    trueBlocks.push_back(getOrCreateLabel(pp, func, i + static_cast<int64_t>(tableCompiler[idx]) + 1));
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
                    default:
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
                            r1 = builder.getInt8(static_cast<uint8_t>(idx - 1) + ip->b.u8);
                            break;
                        case ByteCodeOp::JumpDyn16:
                            r1 = builder.getInt16(static_cast<uint16_t>(idx - 1) + ip->b.u16);
                            break;
                        case ByteCodeOp::JumpDyn32:
                            r1 = builder.getInt32(idx - 1 + ip->b.u32);
                            break;
                        case ByteCodeOp::JumpDyn64:
                            r1 = builder.getInt64(static_cast<uint64_t>(idx - 1) + ip->b.u64);
                            break;
                        default:
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

                /////////////////////////////////////

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
                auto r0         = GEP64(allocR, ip->a.u32);
                auto b0         = builder.CreateIsNull(builder.CreateLoad(I8_TY(), r0));
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfRTFalse:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto v0         = resultFuncCall ? resultFuncCall : builder.CreateLoad(I8_TY(), GEP8(allocRR, 0));
                auto b0         = builder.CreateIsNull(v0);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfTrue:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = GEP64(allocR, ip->a.u32);
                auto b0         = builder.CreateIsNotNull(builder.CreateLoad(I8_TY(), r0));
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfRTTrue:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto v0         = resultFuncCall ? resultFuncCall : builder.CreateLoad(I8_TY(), GEP8(allocRR, 0));
                auto b0         = builder.CreateIsNotNull(v0);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfZero8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = GEP64(allocR, ip->a.u32);
                auto b0         = builder.CreateIsNull(builder.CreateLoad(I8_TY(), r0));
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfZero16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = GEP64(allocR, ip->a.u32);
                auto b0         = builder.CreateIsNull(builder.CreateLoad(I16_TY(), r0));
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfZero32:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = GEP64(allocR, ip->a.u32);
                auto b0         = builder.CreateIsNull(builder.CreateLoad(I32_TY(), r0));
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfZero64:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = GEP64(allocR, ip->a.u32);
                auto b0         = builder.CreateIsNull(builder.CreateLoad(I64_TY(), r0));
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfNotZero8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = GEP64(allocR, ip->a.u32);
                auto b0         = builder.CreateIsNotNull(builder.CreateLoad(I8_TY(), r0));
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfNotZero16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = GEP64(allocR, ip->a.u32);
                auto b0         = builder.CreateIsNotNull(builder.CreateLoad(I16_TY(), r0));
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfNotZero32:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = GEP64(allocR, ip->a.u32);
                auto b0         = builder.CreateIsNotNull(builder.CreateLoad(I32_TY(), r0));
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfNotZero64:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = GEP64(allocR, ip->a.u32);
                auto b0         = builder.CreateIsNotNull(builder.CreateLoad(I64_TY(), r0));
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
                    auto r0 = GEP64(allocR, ip->a.u32);
                    auto v0 = builder.CreateAdd(builder.CreateLoad(I64_TY(), r0), pp.cstBi64);
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

                /////////////////////////////////////

            case ByteCodeOp::JumpIfStackEqual8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = builder.CreateLoad(I8_TY(), GEP8(allocStack, ip->a.u32));
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfStackNotEqual8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = builder.CreateLoad(I8_TY(), GEP8(allocStack, ip->a.u32));
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpNE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfStackEqual16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = builder.CreateLoad(I16_TY(), GEP8(allocStack, ip->a.u32));
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfStackNotEqual16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = builder.CreateLoad(I16_TY(), GEP8(allocStack, ip->a.u32));
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpNE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfStackEqual32:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = builder.CreateLoad(I32_TY(), GEP8(allocStack, ip->a.u32));
                auto r1         = MK_IMMC_32();
                auto b0         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfStackNotEqual32:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = builder.CreateLoad(I32_TY(), GEP8(allocStack, ip->a.u32));
                auto r1         = MK_IMMC_32();
                auto b0         = builder.CreateICmpNE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfStackEqual64:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = builder.CreateLoad(I64_TY(), GEP8(allocStack, ip->a.u32));
                auto r1         = MK_IMMC_64();
                auto b0         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfStackNotEqual64:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = builder.CreateLoad(I64_TY(), GEP8(allocStack, ip->a.u32));
                auto r1         = MK_IMMC_64();
                auto b0         = builder.CreateICmpNE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfDeRefEqual8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto p0         = GEP64(allocR, ip->a.u32);
                auto ptr        = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), p0), builder.getInt64(ip->d.s64));
                auto r0         = builder.CreateLoad(I8_TY(), ptr);
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfDeRefNotEqual8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto p0         = GEP64(allocR, ip->a.u32);
                auto ptr        = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), p0), builder.getInt64(ip->d.s64));
                auto r0         = builder.CreateLoad(I8_TY(), ptr);
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpNE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfDeRefEqual16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto p0         = GEP64(allocR, ip->a.u32);
                auto ptr        = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), p0), builder.getInt64(ip->d.s64));
                auto r0         = builder.CreateLoad(I16_TY(), ptr);
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfDeRefNotEqual16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto p0         = GEP64(allocR, ip->a.u32);
                auto ptr        = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), p0), builder.getInt64(ip->d.s64));
                auto r0         = builder.CreateLoad(I16_TY(), ptr);
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpNE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfDeRefEqual32:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto p0         = GEP64(allocR, ip->a.u32);
                auto ptr        = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), p0), builder.getInt64(ip->d.s64));
                auto r0         = builder.CreateLoad(I32_TY(), ptr);
                auto r1         = MK_IMMC_32();
                auto b0         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfDeRefNotEqual32:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto p0         = GEP64(allocR, ip->a.u32);
                auto ptr        = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), p0), builder.getInt64(ip->d.s64));
                auto r0         = builder.CreateLoad(I32_TY(), ptr);
                auto r1         = MK_IMMC_32();
                auto b0         = builder.CreateICmpNE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfDeRefEqual64:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto p0         = GEP64(allocR, ip->a.u32);
                auto ptr        = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), p0), builder.getInt64(ip->d.s64));
                auto r0         = builder.CreateLoad(I64_TY(), ptr);
                auto r1         = MK_IMMC_64();
                auto b0         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfDeRefNotEqual64:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto p0         = GEP64(allocR, ip->a.u32);
                auto ptr        = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), p0), builder.getInt64(ip->d.s64));
                auto r0         = builder.CreateLoad(I64_TY(), ptr);
                auto r1         = MK_IMMC_64();
                auto b0         = builder.CreateICmpNE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfLowerS8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_8();
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpSLT(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfLowerS16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_16();
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpSLT(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
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
            case ByteCodeOp::JumpIfLowerU8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_8();
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpULT(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfLowerU16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_16();
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpULT(r0, r1);
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

            case ByteCodeOp::JumpIfLowerEqS8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_8();
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpSLE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfLowerEqS16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_16();
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpSLE(r0, r1);
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
            case ByteCodeOp::JumpIfLowerEqU8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_8();
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpULE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfLowerEqU16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_16();
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpULE(r0, r1);
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

            case ByteCodeOp::JumpIfGreaterEqS8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_8();
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpSGE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfGreaterEqS16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_16();
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpSGE(r0, r1);
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
            case ByteCodeOp::JumpIfGreaterEqU8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_8();
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpUGE(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfGreaterEqU16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_16();
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpUGE(r0, r1);
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

            case ByteCodeOp::JumpIfGreaterS8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_8();
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpSGT(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfGreaterS16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_16();
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpSGT(r0, r1);
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
            case ByteCodeOp::JumpIfGreaterU8:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_8();
                auto r1         = MK_IMMC_8();
                auto b0         = builder.CreateICmpUGT(r0, r1);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfGreaterU16:
            {
                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto r0         = MK_IMMA_16();
                auto r1         = MK_IMMC_16();
                auto b0         = builder.CreateICmpUGT(r0, r1);
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

                /////////////////////////////////////

            case ByteCodeOp::CopyRAtoRRRet:
                getReturnResult(context, buildParameters, returnType, ip->hasFlag(BCI_IMM_A), ip->a, allocR, allocResult);
                [[fallthrough]];
            case ByteCodeOp::Ret:
                // :OptimizedAwayDebugCrap
                // Hack thanks to llvm in debug mode : we need to force the usage of function parameters until the very end of the function (i.e. each return),
                // otherwise :
                // - parameters not used in the function body will be removed by llvm (even without optim activated !)
                // - a parameter will not be visible anymore ("optimized away") after its last usage.
                // So we force a read/write of each parameter just before the "ret" to avoid that debug mess.
                // RIDICULOUS !!
                //
                // If we request an optimized code, do not do that crap.
                createRet(buildParameters, typeFunc, returnType, allocResult);
                blockIsClosed = true;
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicCompilerError:
            {
                auto bcF = ip->node->resolvedSymbolOverload()->node->extByteCode()->bc;
                emitCall(buildParameters, bcF->getCallName().cstr(), allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
                break;
            }
            case ByteCodeOp::IntrinsicCompilerWarning:
            {
                auto bcF = ip->node->resolvedSymbolOverload()->node->extByteCode()->bc;
                emitCall(buildParameters, bcF->getCallName().cstr(), allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
                break;
            }

            case ByteCodeOp::IntrinsicPanic:
                emitCall(buildParameters, g_LangSpec->name_at_panic, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
                break;

            case ByteCodeOp::Unreachable:
                emitInternalPanic(buildParameters, allocR, allocT, ip->node, "executing unreachable code");
                break;
            case ByteCodeOp::InternalUnreachable:
                builder.CreateUnreachable();
                blockIsClosed = true;
                break;
            case ByteCodeOp::InternalPanic:
                emitInternalPanic(buildParameters, allocR, allocT, ip->node, reinterpret_cast<const char*>(ip->d.pointer));
                break;

            case ByteCodeOp::InternalGetTlsPtr:
            {
                auto v0     = builder.getInt64(module->tlsSegment.totalCount);
                auto r1     = builder.CreateInBoundsGEP(I8_TY(), pp.tlsSeg, pp.cstAi64);
                auto vid    = builder.CreateLoad(I64_TY(), pp.symTlsThreadLocalId);
                auto result = emitCall(buildParameters, g_LangSpec->name_priv_tlsGetPtr, allocR, allocT, {UINT32_MAX, UINT32_MAX, UINT32_MAX}, {vid, v0, r1});
                builder.CreateStore(result, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::IntrinsicGetContext:
            {
                auto r0     = builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {pp.cstAi32, pp.cstCi32});
                auto v0     = builder.CreateLoad(I64_TY(), r0);
                auto result = emitCall(buildParameters, g_LangSpec->name_priv_tlsGetValue, allocR, allocT, {UINT32_MAX}, {v0});
                builder.CreateStore(builder.CreatePtrToInt(result, I64_TY()), GEP64(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::IntrinsicSetContext:
            {
                auto r0 = builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {pp.cstAi32, pp.cstCi32});
                auto v0 = builder.CreateLoad(I64_TY(), r0);
                emitCall(buildParameters, g_LangSpec->name_priv_tlsSetValue, allocR, allocT, {UINT32_MAX, ip->a.u32}, {v0, nullptr});
                break;
            }
            case ByteCodeOp::IntrinsicGetProcessInfos:
            {
                auto v0 = TO_PTR_I8(builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, pp.cstAi64));
                auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::IntrinsicCVaStart:
            {
                auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                builder.CreateIntrinsic(llvm::Intrinsic::vastart, {}, {r0});
                break;
            }
            case ByteCodeOp::IntrinsicCVaEnd:
            {
                auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                builder.CreateIntrinsic(llvm::Intrinsic::vaend, {}, {r0});
                break;
            }
            case ByteCodeOp::IntrinsicCVaArg:
            {
                auto         r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                llvm::Value* v0 = builder.CreateVAArg(r0, I64_TY());
                switch (ip->c.u32)
                {
                    case 1:
                        v0 = builder.CreateIntCast(v0, I8_TY(), false);
                        builder.CreateStore(v0, GEP64_PTR_I8(allocR, ip->b.u32));
                        break;
                    case 2:
                        v0 = builder.CreateIntCast(v0, I16_TY(), false);
                        builder.CreateStore(v0, GEP64_PTR_I16(allocR, ip->b.u32));
                        break;
                    case 4:
                        v0 = builder.CreateIntCast(v0, I32_TY(), false);
                        builder.CreateStore(v0, GEP64_PTR_I32(allocR, ip->b.u32));
                        break;
                    case 8:
                        builder.CreateStore(v0, GEP64(allocR, ip->b.u32));
                        break;
                    default:
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicArguments:
                emitCall(buildParameters, g_LangSpec->name_at_args, allocR, allocT, {}, {});
                storeRT2ToRegisters(context, buildParameters, ip->a.u32, ip->b.u32, allocR, allocT);
                break;

            case ByteCodeOp::IntrinsicIsByteCode:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                builder.CreateStore(builder.getInt8(0), r0);
                break;
            }
            case ByteCodeOp::IntrinsicCompiler:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                builder.CreateStore(pp.cstAi64, r0);
                auto r1 = GEP64(allocR, ip->b.u32);
                builder.CreateStore(pp.cstAi64, r1);
                break;
            }
            case ByteCodeOp::IntrinsicModules:
            {
                if (buildParameters.module->modulesSliceOffset == UINT32_MAX)
                {
                    auto r0 = GEP64(allocR, ip->a.u32);
                    builder.CreateStore(pp.cstAi64, r0);
                    auto r1 = GEP64(allocR, ip->b.u32);
                    builder.CreateStore(pp.cstAi64, r1);
                }
                else
                {
                    auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                    auto r1 = GEP8(pp.constantSeg, buildParameters.module->modulesSliceOffset);
                    builder.CreateStore(r1, r0);
                    auto r2 = GEP64(allocR, ip->b.u32);
                    builder.CreateStore(builder.getInt64(buildParameters.module->moduleDependencies.count + 1), r2);
                }
                break;
            }
            case ByteCodeOp::IntrinsicGvtd:
            {
                if (buildParameters.module->globalVarsToDropSliceOffset == UINT32_MAX)
                {
                    auto r0 = GEP64(allocR, ip->a.u32);
                    builder.CreateStore(pp.cstAi64, r0);
                    auto r1 = GEP64(allocR, ip->b.u32);
                    builder.CreateStore(pp.cstAi64, r1);
                }
                else
                {
                    auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                    auto r1 = GEP8(pp.mutableSeg, buildParameters.module->globalVarsToDropSliceOffset);
                    builder.CreateStore(r1, r0);
                    auto r2 = GEP64(allocR, ip->b.u32);
                    builder.CreateStore(builder.getInt64(buildParameters.module->globalVarsToDrop.count), r2);
                }
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::InternalFailedAssume:
                emitCall(buildParameters, g_LangSpec->name_priv_failedAssume, allocR, allocT, {ip->a.u32}, {});
                break;
            case ByteCodeOp::IntrinsicGetErr:
                emitCall(buildParameters, g_LangSpec->name_at_err, allocR, allocT, {}, {});
                storeRT2ToRegisters(context, buildParameters, ip->a.u32, ip->b.u32, allocR, allocT);
                break;
            case ByteCodeOp::InternalSetErr:
                emitCall(buildParameters, g_LangSpec->name_priv_seterr, allocR, allocT, {ip->a.u32, ip->b.u32}, {});
                break;

            case ByteCodeOp::InternalHasErr:
            {
                auto r0 = GEP64(allocR, ip->b.u32);
                auto ra = builder.CreateLoad(PTR_I8_TY(), r0);
                auto v0 = GEP8(ra, offsetof(SwagContext, hasError));
                auto r1 = GEP64_PTR_I32(allocR, ip->a.u32);
                builder.CreateStore(builder.CreateLoad(I32_TY(), v0), r1);
                break;
            }
            case ByteCodeOp::JumpIfError:
            {
                auto r0     = GEP64(allocR, ip->a.u32);
                auto ra     = builder.CreateLoad(PTR_I8_TY(), r0);
                auto v0     = GEP8(ra, offsetof(SwagContext, hasError));
                auto hasErr = builder.CreateLoad(I32_TY(), v0);

                auto labelFalse = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelTrue  = getOrCreateLabel(pp, func, i + 1);
                auto b0         = builder.CreateIsNull(hasErr);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfNoError:
            {
                auto r0     = GEP64(allocR, ip->a.u32);
                auto ra     = builder.CreateLoad(PTR_I8_TY(), r0);
                auto v0     = GEP8(ra, offsetof(SwagContext, hasError));
                auto hasErr = builder.CreateLoad(I32_TY(), v0);

                auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
                auto labelFalse = getOrCreateLabel(pp, func, i + 1);
                auto b0         = builder.CreateIsNull(hasErr);
                builder.CreateCondBr(b0, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::InternalPushErr:
                emitCall(buildParameters, g_LangSpec->name_priv_pusherr, allocR, allocT, {}, {});
                break;
            case ByteCodeOp::InternalPopErr:
                emitCall(buildParameters, g_LangSpec->name_priv_poperr, allocR, allocT, {}, {});
                break;
            case ByteCodeOp::InternalCatchErr:
                emitCall(buildParameters, g_LangSpec->name_priv_catcherr, allocR, allocT, {}, {});
                break;
            case ByteCodeOp::InternalInitStackTrace:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto ra = builder.CreateLoad(PTR_I8_TY(), r0);
                auto v0 = GEP8_PTR_I32(ra, offsetof(SwagContext, traceIndex));
                builder.CreateStore(pp.cstAi32, v0);
                break;
            }
            case ByteCodeOp::InternalStackTraceConst:
            {
                auto r1 = GEP8(pp.constantSeg, ip->b.u32);
                builder.CreateStore(r1, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                emitCall(buildParameters, g_LangSpec->name_priv_stackTrace, allocR, allocT, {ip->a.u32}, {});
                break;
            }
            case ByteCodeOp::InternalStackTrace:
                emitCall(buildParameters, g_LangSpec->name_priv_stackTrace, allocR, allocT, {ip->a.u32}, {});
                break;

                /////////////////////////////////////

            case ByteCodeOp::NegBool:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateXor(builder.CreateLoad(I8_TY(), r1), pp.cstBi8);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::NegF32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateFNeg(builder.CreateLoad(F32_TY(), r1));
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::NegF64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateFNeg(builder.CreateLoad(F64_TY(), r1));
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::NegS32:
            {
                auto r0 = GEP64_PTR_I32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateNeg(builder.CreateLoad(I32_TY(), r1));
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::NegS64:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateNeg(builder.CreateLoad(I64_TY(), r1));
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::InvertU8:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateNot(builder.CreateLoad(I8_TY(), r1));
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::InvertU16:
            {
                auto r0 = GEP64_PTR_I16(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateNot(builder.CreateLoad(I16_TY(), r1));
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::InvertU32:
            {
                auto r0 = GEP64_PTR_I32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateNot(builder.CreateLoad(I32_TY(), r1));
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::InvertU64:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateNot(builder.CreateLoad(I64_TY(), r1));
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::ClearMaskU32:
            {
                auto r0 = GEP64_PTR_I32(allocR, ip->a.u32);
                auto v0 = builder.CreateAnd(builder.CreateLoad(I32_TY(), r0), CST_RB32);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::ClearMaskU64:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto v0 = builder.CreateAnd(builder.CreateLoad(I64_TY(), r0), CST_RB64);
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CastBool8:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateIsNotNull(builder.CreateLoad(I8_TY(), r1));
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastBool16:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateIsNotNull(builder.CreateLoad(I16_TY(), r1));
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastBool32:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateIsNotNull(builder.CreateLoad(I32_TY(), r1));
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastBool64:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateIsNotNull(builder.CreateLoad(I64_TY(), r1));
                v0      = builder.CreateIntCast(v0, I8_TY(), false);
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::CastS8S16:
            {
                auto r0 = GEP64_PTR_I16(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateIntCast(builder.CreateLoad(I8_TY(), r1), I16_TY(), true);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastS8S32:
            {
                auto r0 = GEP64_PTR_I32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateIntCast(builder.CreateLoad(I8_TY(), r1), I32_TY(), true);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastS16S32:
            {
                auto r0 = GEP64_PTR_I32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateIntCast(builder.CreateLoad(I16_TY(), r1), I32_TY(), true);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastF32S32:
            {
                auto r0 = GEP64_PTR_I32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(F32_TY(), r1), I32_TY());
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::CastS8S64:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateIntCast(builder.CreateLoad(I8_TY(), r1), I64_TY(), true);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastS16S64:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateIntCast(builder.CreateLoad(I16_TY(), r1), I64_TY(), true);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastS32S64:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateIntCast(builder.CreateLoad(I32_TY(), r1), I64_TY(), true);
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastF64S64:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(F64_TY(), r1), I64_TY());
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::CastS8F32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(I8_TY(), r1), F32_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastS16F32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(I16_TY(), r1), F32_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastS32F32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(I32_TY(), r1), F32_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastS64F32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(I64_TY(), r1), F32_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastU8F32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(I8_TY(), r1), F32_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastU16F32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(I16_TY(), r1), F32_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastU32F32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(I32_TY(), r1), F32_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastU64F32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(I64_TY(), r1), F32_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastF64F32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPTrunc, builder.CreateLoad(F64_TY(), r1), F32_TY());
                builder.CreateStore(v0, r0);
                break;
            }

            case ByteCodeOp::CastS8F64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(I8_TY(), r1), F64_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastS16F64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(I16_TY(), r1), F64_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastS32F64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(I32_TY(), r1), F64_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastS64F64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(I64_TY(), r1), F64_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastU8F64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(I8_TY(), r1), F64_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastU16F64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(I16_TY(), r1), F64_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastU32F64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(I32_TY(), r1), F64_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastU64F64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(I64_TY(), r1), F64_TY());
                builder.CreateStore(v0, r0);
                break;
            }
            case ByteCodeOp::CastF32F64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->b.u32);
                auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPExt, builder.CreateLoad(F32_TY(), r1), F64_TY());
                builder.CreateStore(v0, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CopyRAtoRR:
                getReturnResult(context, buildParameters, returnType, ip->hasFlag(BCI_IMM_A), ip->a, allocR, allocResult);
                break;
            case ByteCodeOp::CopyRARBtoRR2:
            {
                auto r1        = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->a.u32));
                auto r2        = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->b.u32));
                auto resultPtr = TO_PTR_I64(func->getArg(static_cast<int>(func->arg_size()) - 1));
                builder.CreateStore(r1, resultPtr);
                builder.CreateStore(r2, builder.CreateInBoundsGEP(I64_TY(), resultPtr, builder.getInt64(1)));
                break;
            }
            case ByteCodeOp::CopyRAtoRT:
            {
                auto r0 = GEP64(allocRR, 0);
                auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->a.u32));
                builder.CreateStore(r1, r0);
                break;
            }
            case ByteCodeOp::SaveRRtoRA:
            {
                auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                auto r1 = TO_PTR_I8(func->getArg(static_cast<int>(func->arg_size()) - 1));
                builder.CreateStore(r1, r0);
                break;
            }
            case ByteCodeOp::CopyRRtoRA:
            {
                auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                auto r1 = TO_PTR_I8(func->getArg(static_cast<int>(func->arg_size()) - 1));
                if (ip->b.u64)
                    r1 = builder.CreateInBoundsGEP(I8_TY(), r1, builder.getInt64(ip->b.u64));
                builder.CreateStore(r1, r0);
                break;
            }

            case ByteCodeOp::CopyRTtoRA:
            {
                if (resultFuncCall)
                    storeTypedValueToRegister(context, buildParameters, resultFuncCall, ip->a.u32, allocR);
                else
                {
                    auto r0 = GEP64(allocR, ip->a.u32);
                    auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocRR, 0));
                    builder.CreateStore(r1, r0);
                }
                break;
            }
            case ByteCodeOp::CopyRT2toRARB:
                storeRT2ToRegisters(context, buildParameters, ip->a.u32, ip->b.u32, allocR, allocRR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetParam8:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 1));
                break;
            case ByteCodeOp::GetParam16:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 2));
                break;
            case ByteCodeOp::GetParam32:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 4));
                break;
            case ByteCodeOp::GetParam64:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR));
                break;
            case ByteCodeOp::GetParam64x2:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR));
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->c.u32, ip->d.mergeU64U32.high, allocR));
                break;
            case ByteCodeOp::GetIncParam64:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, ip->d.u64));
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetParam64DeRef8:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, 0, 1));
                break;
            case ByteCodeOp::GetParam64DeRef16:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, 0, 2));
                break;
            case ByteCodeOp::GetParam64DeRef32:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, 0, 4));
                break;
            case ByteCodeOp::GetParam64DeRef64:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, 0, 8));
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetIncParam64DeRef8:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, ip->d.u64, 1));
                break;
            case ByteCodeOp::GetIncParam64DeRef16:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, ip->d.u64, 2));
                break;
            case ByteCodeOp::GetIncParam64DeRef32:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, ip->d.u64, 4));
                break;
            case ByteCodeOp::GetIncParam64DeRef64:
                SWAG_CHECK(emitGetParam(context, buildParameters, func, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, ip->d.u64, 8));
                break;

                /////////////////////////////////////

            case ByteCodeOp::ZeroToTrue:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto v0 = builder.CreateLoad(I32_TY(), r0);
                auto a0 = builder.CreateIntCast(builder.CreateICmpEQ(v0, pp.cstAi32), I8_TY(), false);
                builder.CreateStore(a0, r0);
                break;
            }
            case ByteCodeOp::LowerZeroToTrue:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto v0 = builder.CreateLoad(I32_TY(), r0);
                auto a0 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cstAi32), I8_TY(), false);
                builder.CreateStore(a0, r0);
                break;
            }
            case ByteCodeOp::LowerEqZeroToTrue:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto v0 = builder.CreateLoad(I32_TY(), r0);
                auto a0 = builder.CreateIntCast(builder.CreateICmpSLE(v0, pp.cstAi32), I8_TY(), false);
                builder.CreateStore(a0, r0);
                break;
            }
            case ByteCodeOp::GreaterZeroToTrue:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto v0 = builder.CreateLoad(I32_TY(), r0);
                auto a0 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cstAi32), I8_TY(), false);
                builder.CreateStore(a0, r0);
                break;
            }
            case ByteCodeOp::GreaterEqZeroToTrue:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto v0 = builder.CreateLoad(I32_TY(), r0);
                auto a0 = builder.CreateIntCast(builder.CreateICmpSGE(v0, pp.cstAi32), I8_TY(), false);
                builder.CreateStore(a0, r0);
                break;
            }

                /////////////////////////////////////

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
                auto r0 = GEP64_PTR_PTR_I64(allocR, ip->a.u32);
                auto r1 = GEP64(allocR, ip->c.u32);
                builder.CreateStore(r1, r0);
                break;
            }

            case ByteCodeOp::CopySPVaargs:
            {
                SWAG_ASSERT(allocVa);

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
                            auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                            auto r1 = builder.CreateInBoundsGEP(I8_TY(), allocVa, pp.cstAi32);
                            builder.CreateStore(r1, r0);
                            break;
                        }
                        case 2:
                        {
                            auto r0 = GEP64_PTR_PTR_I16(allocR, ip->a.u32);
                            auto r1 = builder.CreateInBoundsGEP(I16_TY(), allocVa, pp.cstAi32);
                            builder.CreateStore(r1, r0);
                            break;
                        }
                        case 4:
                        {
                            auto r0 = GEP64_PTR_PTR_I32(allocR, ip->a.u32);
                            auto r1 = builder.CreateInBoundsGEP(I32_TY(), allocVa, pp.cstAi32);
                            builder.CreateStore(r1, r0);
                            break;
                        }
                        default:
                            break;
                    }

                    uint32_t idx      = 0;
                    uint32_t idxParam = pushRVParams.size() - 1;
                    while (idxParam != UINT32_MAX)
                    {
                        auto reg = pushRVParams[idxParam].first;
                        switch (sizeOf)
                        {
                            case 1:
                            {
                                auto r0 = GEP(I8_TY(), allocVa, idx);
                                auto r1 = GEP64(allocR, reg);
                                builder.CreateStore(builder.CreateLoad(I8_TY(), r1), r0);
                                break;
                            }
                            case 2:
                            {
                                auto r0 = GEP(I16_TY(), allocVa, idx);
                                auto r1 = GEP64(allocR, reg);
                                builder.CreateStore(builder.CreateLoad(I16_TY(), r1), r0);
                                break;
                            }
                            case 4:
                            {
                                auto r0 = GEP(I32_TY(), allocVa, idx);
                                auto r1 = GEP64(allocR, reg);
                                builder.CreateStore(builder.CreateLoad(I32_TY(), r1), r0);
                                break;
                            }
                            default:
                                break;
                        }

                        idx++;
                        idxParam--;
                    }
                }
                else
                {
                    // All of this is complicated. But ip->b.u32 has been reduced by one register in case of closure, and
                    // we have a dynamic test for bytecode execution. But for runtime, be put it back.
                    auto typeFuncCall = castTypeInfo<TypeInfoFuncAttr>(reinterpret_cast<TypeInfo*>(ip->d.pointer), TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                    auto sizeB        = ip->b.u32;
                    if (typeFuncCall->isClosure())
                        sizeB += sizeof(Register);

                    // In the pushRAParams array, we have first all the variadic registers
                    //
                    // And then, we have all normal parameters. So we start at pushRAParams.size() less the number of registers
                    // used to pass the normal parameters.
                    //
                    // The number of normal parameters is deduced from the 'offset' of the CopySPVaargs instruction (ip->b.u32)
                    uint32_t idx      = 0;
                    uint32_t idxParam = pushRAParams.size() - sizeB / static_cast<uint32_t>(sizeof(Register)) - 1;
                    while (idxParam != UINT32_MAX)
                    {
                        SWAG_ASSERT(idx < bc->maxSpVaargs);
                        auto r0 = GEP64(allocVa, idx);
                        auto r1 = GEP64(allocR, pushRAParams[idxParam]);
                        builder.CreateStore(builder.CreateLoad(I64_TY(), r1), r0);
                        idx++;
                        idxParam--;
                    }

                    auto r0 = GEP64_PTR_PTR_I64(allocR, ip->a.u32);
                    auto r1 = builder.CreateInBoundsGEP(I64_TY(), allocVa, pp.cstAi32);
                    builder.CreateStore(r1, r0);
                }
                break;
            }

            case ByteCodeOp::MakeLambda:
            {
                auto              funcNode     = castAst<AstFuncDecl>(reinterpret_cast<AstNode*>(ip->b.pointer), AstNodeKind::FuncDecl);
                Utf8              callName     = funcNode->getCallName();
                TypeInfoFuncAttr* typeFuncNode = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

                auto llvmFctTy = getOrCreateFuncType(buildParameters, typeFuncNode);
                auto llvmFct   = reinterpret_cast<llvm::Function*>(modu.getOrInsertFunction(callName.cstr(), llvmFctTy).getCallee());

                auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                builder.CreateStore(TO_PTR_I8(llvmFct), r0);

                auto v0 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->a.u32));
                builder.CreateStore(v0, GEP64(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::IntrinsicMakeCallback:
            {
                llvm::BasicBlock* blockLambdaBC = llvm::BasicBlock::Create(context, "", func);
                llvm::BasicBlock* blockNext     = llvm::BasicBlock::Create(context, "", func);
                auto              v0            = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->a.u32));

                {
                    auto v1 = builder.CreateAnd(v0, builder.getInt64(SWAG_LAMBDA_BC_MARKER));
                    auto v2 = builder.CreateIsNotNull(v1);
                    builder.CreateCondBr(v2, blockLambdaBC, blockNext);
                }

                builder.SetInsertPoint(blockLambdaBC);
                {
                    auto r0 = builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {pp.cstAi32, pp.cstFi32});
                    auto r1 = builder.CreateLoad(PTR_I8_TY(), r0);
                    auto pt = llvm::PointerType::getUnqual(pp.makeCallbackTy);
                    auto r2 = builder.CreatePointerCast(r1, pt);
                    auto v2 = builder.CreateIntToPtr(v0, PTR_I8_TY());
                    auto v1 = builder.CreateCall(pp.makeCallbackTy, r2, {v2});
                    auto r3 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                    builder.CreateStore(v1, r3);
                    builder.CreateBr(blockNext);
                }

                builder.SetInsertPoint(blockNext);
                break;
            }

            case ByteCodeOp::LocalCall:
            case ByteCodeOp::LocalCallPop:
            case ByteCodeOp::LocalCallPop16:
            case ByteCodeOp::LocalCallPop48:
            case ByteCodeOp::LocalCallPopRC:
                emitLocalCall(buildParameters, context, allocR, allocRR, ip, pushRVParams, pushRAParams, resultFuncCall);
                break;
            case ByteCodeOp::LocalCallPopParam:
                pushRAParams.push_back(ip->d.u32);
                emitLocalCall(buildParameters, context, allocR, allocRR, ip, pushRVParams, pushRAParams, resultFuncCall);
                break;
            case ByteCodeOp::LocalCallPop0Param2:
            case ByteCodeOp::LocalCallPop16Param2:
            case ByteCodeOp::LocalCallPop48Param2:
                pushRAParams.push_back(ip->c.u32);
                pushRAParams.push_back(ip->d.u32);
                emitLocalCall(buildParameters, context, allocR, allocRR, ip, pushRVParams, pushRAParams, resultFuncCall);
                break;

            case ByteCodeOp::ForeignCall:
            case ByteCodeOp::ForeignCallPop:
                emitForeignCall(buildParameters, allocR, allocRR, ip, pushRVParams, pushRAParams, resultFuncCall);
                break;
            case ByteCodeOp::ForeignCallPopParam:
                pushRAParams.push_back(ip->d.u32);
                emitForeignCall(buildParameters, allocR, allocRR, ip, pushRVParams, pushRAParams, resultFuncCall);
                break;
            case ByteCodeOp::ForeignCallPop0Param2:
            case ByteCodeOp::ForeignCallPop16Param2:
            case ByteCodeOp::ForeignCallPop48Param2:
                pushRAParams.push_back(ip->c.u32);
                pushRAParams.push_back(ip->d.u32);
                emitForeignCall(buildParameters, allocR, allocRR, ip, pushRVParams, pushRAParams, resultFuncCall);
                break;

            case ByteCodeOp::LambdaCall:
            case ByteCodeOp::LambdaCallPop:
                SWAG_CHECK(emitLambdaCall(buildParameters, pp, context, builder, func, allocR, allocRR, allocT, ip, pushRVParams, pushRAParams, resultFuncCall));
                break;
            case ByteCodeOp::LambdaCallPopParam:
                pushRAParams.push_back(ip->d.u32);
                SWAG_CHECK(emitLambdaCall(buildParameters, pp, context, builder, func, allocR, allocRR, allocT, ip, pushRVParams, pushRAParams, resultFuncCall));
                break;

            case ByteCodeOp::IncSPPostCall:
            case ByteCodeOp::IncSPPostCallCond:
                pushRAParams.clear();
                pushRVParams.clear();
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMulAddF32:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = MK_IMMB_F32();
                auto r2 = MK_IMMC_F32();
                auto r3 = MK_IMMD_F32();
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fmuladd, {F32_TY()}, {r1, r2, r3}), r0);
                break;
            }
            case ByteCodeOp::IntrinsicMulAddF64:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = MK_IMMB_F64();
                auto r2 = MK_IMMC_F64();
                auto r3 = MK_IMMD_F64();
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fmuladd, {F64_TY()}, {r1, r2, r3}), r0);
                break;
            }

            case ByteCodeOp::IntrinsicS8x1:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto r1 = MK_IMMB_8();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {I8_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctpop, {I8_TY()}, {r1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cttz, {I8_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctlz, {I8_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }
            case ByteCodeOp::IntrinsicS16x1:
            {
                auto r0 = GEP64_PTR_I16(allocR, ip->a.u32);
                auto r1 = MK_IMMB_16();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {I16_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctpop, {I16_TY()}, {r1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cttz, {I16_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctlz, {I16_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::bswap, {I16_TY()}, {r1}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }
            case ByteCodeOp::IntrinsicS32x1:
            {
                auto r0 = GEP64_PTR_I32(allocR, ip->a.u32);
                auto r1 = MK_IMMB_32();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {I32_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctpop, {I32_TY()}, {r1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cttz, {I32_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctlz, {I32_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::bswap, {I32_TY()}, {r1}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }
            case ByteCodeOp::IntrinsicS64x1:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto r1 = MK_IMMB_64();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {I64_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctpop, {I64_TY()}, {r1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cttz, {I64_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctlz, {I64_TY()}, {r1, pp.cstAi1}), r0);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::bswap, {I64_TY()}, {r1}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicS8x2:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto r1 = MK_IMMB_8();
                auto r2 = MK_IMMC_8();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {I8_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {I8_TY()}, {r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }
            case ByteCodeOp::IntrinsicS16x2:
            {
                auto r0 = GEP64_PTR_I16(allocR, ip->a.u32);
                auto r1 = MK_IMMB_16();
                auto r2 = MK_IMMC_16();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {I16_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {I16_TY()}, {r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }
            case ByteCodeOp::IntrinsicS32x2:
            {
                auto r0 = GEP64_PTR_I32(allocR, ip->a.u32);
                auto r1 = MK_IMMB_32();
                auto r2 = MK_IMMC_32();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {I32_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {I32_TY()}, {r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }
            case ByteCodeOp::IntrinsicS64x2:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto r1 = MK_IMMB_64();
                auto r2 = MK_IMMC_64();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {I64_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {I64_TY()}, {r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicU8x2:
            {
                auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                auto r1 = MK_IMMB_8();
                auto r2 = MK_IMMC_8();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {I8_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {I8_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicRol:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fshl, {I8_TY()}, {r1, r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicRor:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fshr, {I8_TY()}, {r1, r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }
            case ByteCodeOp::IntrinsicU16x2:
            {
                auto r0 = GEP64_PTR_I16(allocR, ip->a.u32);
                auto r1 = MK_IMMB_16();
                auto r2 = MK_IMMC_16();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {I16_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {I16_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicRol:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fshl, {I16_TY()}, {r1, r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicRor:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fshr, {I16_TY()}, {r1, r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }
            case ByteCodeOp::IntrinsicU32x2:
            {
                auto r0 = GEP64_PTR_I32(allocR, ip->a.u32);
                auto r1 = MK_IMMB_32();
                auto r2 = MK_IMMC_32();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {I32_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {I32_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicRol:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fshl, {I32_TY()}, {r1, r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicRor:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fshr, {I32_TY()}, {r1, r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }
            case ByteCodeOp::IntrinsicU64x2:
            {
                auto r0 = GEP64(allocR, ip->a.u32);
                auto r1 = MK_IMMB_64();
                auto r2 = MK_IMMC_64();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {I64_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {I64_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicRol:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fshl, {I64_TY()}, {r1, r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicRor:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fshr, {I64_TY()}, {r1, r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicF32x2:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = MK_IMMB_F32();
                auto r2 = MK_IMMC_F32();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicPow:
                        builder.CreateStore(builder.CreateCall(pp.fnPowF32, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::minnum, {F32_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::maxnum, {F32_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicATan2:
                        builder.CreateStore(builder.CreateCall(pp.fnAtan2F32, {r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }
            case ByteCodeOp::IntrinsicF64x2:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = MK_IMMB_F64();
                auto r2 = MK_IMMC_F64();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicPow:
                        builder.CreateStore(builder.CreateCall(pp.fnPowF64, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::minnum, {F64_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::maxnum, {F64_TY()}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicATan2:
                        builder.CreateStore(builder.CreateCall(pp.fnAtan2F64, {r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicF32x1:
            {
                auto r0 = GEP64_PTR_F32(allocR, ip->a.u32);
                auto r1 = MK_IMMB_F32();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicTan:
                        builder.CreateStore(builder.CreateCall(pp.fnTanF32, r1), r0);
                        break;
                    case TokenId::IntrinsicSinh:
                        builder.CreateStore(builder.CreateCall(pp.fnSinhF32, r1), r0);
                        break;
                    case TokenId::IntrinsicCosh:
                        builder.CreateStore(builder.CreateCall(pp.fnCoshF32, r1), r0);
                        break;
                    case TokenId::IntrinsicTanh:
                        builder.CreateStore(builder.CreateCall(pp.fnTanhF32, r1), r0);
                        break;
                    case TokenId::IntrinsicASin:
                        builder.CreateStore(builder.CreateCall(pp.fnAsinF32, r1), r0);
                        break;
                    case TokenId::IntrinsicACos:
                        builder.CreateStore(builder.CreateCall(pp.fnAcosF32, r1), r0);
                        break;
                    case TokenId::IntrinsicATan:
                        builder.CreateStore(builder.CreateCall(pp.fnAtanF32, r1), r0);
                        break;
                    case TokenId::IntrinsicSqrt:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sqrt, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicSin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sin, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicCos:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cos, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicLog:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicLog2:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log2, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicLog10:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log10, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicFloor:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::floor, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicCeil:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ceil, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicTrunc:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::trunc, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicRound:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::round, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicAbs:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fabs, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicExp:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp, F32_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicExp2:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp2, F32_TY(), r1), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicF64x1:
            {
                auto r0 = GEP64_PTR_F64(allocR, ip->a.u32);
                auto r1 = MK_IMMB_F64();
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicTan:
                        builder.CreateStore(builder.CreateCall(pp.fnTanF64, r1), r0);
                        break;
                    case TokenId::IntrinsicSinh:
                        builder.CreateStore(builder.CreateCall(pp.fnSinhF64, r1), r0);
                        break;
                    case TokenId::IntrinsicCosh:
                        builder.CreateStore(builder.CreateCall(pp.fnCoshF64, r1), r0);
                        break;
                    case TokenId::IntrinsicTanh:
                        builder.CreateStore(builder.CreateCall(pp.fnTanhF64, r1), r0);
                        break;
                    case TokenId::IntrinsicASin:
                        builder.CreateStore(builder.CreateCall(pp.fnAsinF64, r1), r0);
                        break;
                    case TokenId::IntrinsicACos:
                        builder.CreateStore(builder.CreateCall(pp.fnAcosF64, r1), r0);
                        break;
                    case TokenId::IntrinsicATan:
                        builder.CreateStore(builder.CreateCall(pp.fnAtanF64, r1), r0);
                        break;
                    case TokenId::IntrinsicSqrt:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sqrt, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicSin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sin, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicCos:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cos, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicLog:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicLog2:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log2, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicLog10:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log10, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicFloor:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::floor, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicCeil:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ceil, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicTrunc:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::trunc, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicRound:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::round, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicAbs:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fabs, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicExp:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp, F64_TY(), r1), r0);
                        break;
                    case TokenId::IntrinsicExp2:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp2, F64_TY(), r1), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicAtomicAddS8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I8(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicAddS16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I16(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicAddS32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I32(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicAddS64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64(allocR, ip->c.u32));
                break;
            }

            case ByteCodeOp::IntrinsicAtomicAndS8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I8(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicAndS16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I16(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicAndS32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I32(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicAndS64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64(allocR, ip->c.u32));
                break;
            }

            case ByteCodeOp::IntrinsicAtomicOrS8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I8(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicOrS16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I16(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicOrS32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I32(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicOrS64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64(allocR, ip->c.u32));
                break;
            }

            case ByteCodeOp::IntrinsicAtomicXorS8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I8(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicXorS16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I16(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicXorS32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I32(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicXorS64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64(allocR, ip->c.u32));
                break;
            }

            case ByteCodeOp::IntrinsicAtomicXchgS8:
            {
                MK_BINOP_EQ8_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I8(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicXchgS16:
            {
                MK_BINOP_EQ16_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I16(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicXchgS32:
            {
                MK_BINOP_EQ32_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64_PTR_I32(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicXchgS64:
            {
                MK_BINOP_EQ64_CAB();
                auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(v0, GEP64(allocR, ip->c.u32));
                break;
            }

            case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
            {
                auto ptr0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));

                llvm::Value* r1 = MK_IMMB_8();
                llvm::Value* r2 = MK_IMMC_8();
                auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
                v0->setVolatile(true);

                auto v1 = builder.CreateExtractValue(v0, 0);
                builder.CreateStore(v1, GEP64_PTR_I8(allocR, ip->d.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
            {
                auto ptr0 = builder.CreateLoad(PTR_I16_TY(), GEP64(allocR, ip->a.u32));

                llvm::Value* r1 = MK_IMMB_16();
                llvm::Value* r2 = MK_IMMC_16();
                auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
                v0->setVolatile(true);

                auto v1 = builder.CreateExtractValue(v0, 0);
                builder.CreateStore(v1, GEP64_PTR_I16(allocR, ip->d.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
            {
                auto ptr0 = builder.CreateLoad(PTR_I32_TY(), GEP64(allocR, ip->a.u32));

                llvm::Value* r1 = MK_IMMB_32();
                llvm::Value* r2 = MK_IMMC_32();
                auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
                v0->setVolatile(true);

                auto v1 = builder.CreateExtractValue(v0, 0);
                builder.CreateStore(v1, GEP64_PTR_I32(allocR, ip->d.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
            {
                auto ptr0 = builder.CreateLoad(PTR_I64_TY(), GEP64(allocR, ip->a.u32));

                llvm::Value* r1 = MK_IMMB_64();
                llvm::Value* r2 = MK_IMMC_64();
                auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
                v0->setVolatile(true);

                auto v1 = builder.CreateExtractValue(v0, 0);
                builder.CreateStore(v1, GEP64(allocR, ip->d.u32));
                break;
            }

            default:
                ok = false;
                Report::internalError(buildParameters.module, form("unknown instruction [[%s]] during backend generation", ByteCode::opName(ip->op)));
                break;
        }
    }

    if (!blockIsClosed)
    {
        createRet(buildParameters, typeFunc, returnType, allocResult);
    }

    return ok;
}

llvm::Type* LLVM::swagTypeToLLVMType(const BuildParameters& buildParameters, TypeInfo* typeInfo)
{
    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = encoder<LLVMEncoder>(ct, precompileIndex);
    auto&       context         = *pp.llvmContext;

    typeInfo = typeInfo->getConcreteAlias();

    if (typeInfo->isEnum())
    {
        const auto typeInfoEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        return swagTypeToLLVMType(buildParameters, typeInfoEnum->rawType);
    }

    if (typeInfo->isPointer())
    {
        const auto typeInfoPointer = castTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        const auto pointedType     = TypeManager::concreteType(typeInfoPointer->pointedType);
        if (pointedType->isVoid())
            return PTR_I8_TY();
        return swagTypeToLLVMType(buildParameters, pointedType)->getPointerTo();
    }

    if (typeInfo->isSlice() ||
        typeInfo->isArray() ||
        typeInfo->isStruct() ||
        typeInfo->isInterface() ||
        typeInfo->isLambdaClosure() ||
        typeInfo->isAny() ||
        typeInfo->isString())
    {
        return PTR_I8_TY();
    }

    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
                return I8_TY();
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                return I8_TY();
            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                return I16_TY();
            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                return I32_TY();
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                return I64_TY();
            case NativeTypeKind::F32:
                return F32_TY();
            case NativeTypeKind::F64:
                return F64_TY();
            case NativeTypeKind::Void:
                return VOID_TY();
            default:
                break;
        }
    }

    SWAG_ASSERT(false);
    return nullptr;
}

llvm::BasicBlock* LLVM::getOrCreateLabel(LLVMEncoder& pp, llvm::Function* func, int64_t ip)
{
    auto& context = *pp.llvmContext;

    const auto it = pp.labels.find(ip);
    if (it == pp.labels.end())
    {
        llvm::BasicBlock* label = llvm::BasicBlock::Create(context, form("%lld", ip).cstr(), func);
        pp.labels[ip]           = label;
        return label;
    }

    return it->second;
}

void LLVM::setFuncAttributes(const BuildParameters& buildParameters, const AstFuncDecl* funcNode, const ByteCode* bc, llvm::Function* func) const
{
    if (!buildParameters.module->mustOptimizeBackend(bc->node))
    {
        func->addFnAttr(llvm::Attribute::AttrKind::OptimizeNone);
        func->addFnAttr(llvm::Attribute::AttrKind::NoInline);
    }

    if (funcNode && funcNode->hasAttribute(ATTRIBUTE_NO_INLINE))
        func->addFnAttr(llvm::Attribute::AttrKind::NoInline);

    // Export public symbol in case of a dll
    if (funcNode &&
        funcNode->hasAttribute(ATTRIBUTE_PUBLIC) &&
        buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
    {
        func->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);
    }

    // If we just have one user compile unit, then force private linkage if possible
    else if (funcNode &&
             !funcNode->token.sourceFile->hasFlag(FILE_RUNTIME) &&
             !funcNode->token.sourceFile->hasFlag(FILE_BOOTSTRAP) &&
             !bc->isInDataSegment &&
             !funcNode->hasAttribute(ATTRIBUTE_SHARP_FUNC) &&
             numPreCompileBuffers == 2) // :SegZeroIsData
    {
        func->setLinkage(llvm::GlobalValue::InternalLinkage);
    }
}
