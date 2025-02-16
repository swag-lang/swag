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
    auto&      pp              = encoder<LLVM_Encoder>(ct, precompileIndex);
    auto&      context         = *pp.llvmContext;
    auto&      builder         = *pp.builder;
    auto&      modu            = *pp.llvmModule;
    auto       typeFunc        = bc->getCallType();
    auto       returnType      = typeFunc->concreteReturnType();
    bool       ok              = true;

    pp.init(buildParameters);

    // Get function name
    Utf8         funcName   = bc->getCallNameFromDecl();
    AstFuncDecl* bcFuncNode = bc->node ? castAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl) : nullptr;

    // Function prototype
    auto funcType = getOrCreateFuncType(pp, typeFunc);
    pp.llvmFunc       = reinterpret_cast<llvm::Function*>(modu.getOrInsertFunction(funcName.cstr(), funcType).getCallee());
    auto func     = pp.llvmFunc;
    setFuncAttributes(pp, numPreCompileBuffers, bcFuncNode, bc);

    // Content
    llvm::BasicBlock* block         = llvm::BasicBlock::Create(context, "entry", func);
    bool              blockIsClosed = false;
    builder.SetInsertPoint(block);
    pp.labels.clear();
    bc->markLabels();

    // Reserve registers
    pp.allocR = nullptr;
    if (bc->maxReservedRegisterRC)
    {
        pp.allocR = builder.CreateAlloca(I64_TY(), builder.getInt32(bc->maxReservedRegisterRC));
        pp.allocR->setAlignment(llvm::Align{16});
    }

    pp.allocRR = nullptr;
    if (bc->maxCallResults)
    {
        pp.allocRR = builder.CreateAlloca(I64_TY(), builder.getInt32(bc->maxCallResults));
        pp.allocRR->setAlignment(llvm::Align{16});
    }

    pp.allocResult = builder.CreateAlloca(I64_TY(), builder.getInt32(1));
    pp.allocResult->setAlignment(llvm::Align{16});

    // To store variadic
    pp.allocVa = nullptr;
    if (bc->maxSpVaargs)
    {
        pp.allocVa = builder.CreateAlloca(I64_TY(), builder.getInt64(bc->maxSpVaargs));
        pp.allocVa->setAlignment(llvm::Align{16});
    }

    // Stack
    pp.allocStack = nullptr;
    if (bc->stackSize)
    {
        pp.allocStack = builder.CreateAlloca(I8_TY(), builder.getInt32(bc->stackSize));
        pp.allocStack->setAlignment(llvm::Align(2 * sizeof(void*)));
    }

    // Reserve room to pass parameters to embedded intrinsics
    static constexpr int ALLOC_T_NUM = 5;
    pp.allocT                        = builder.CreateAlloca(I64_TY(), builder.getInt64(ALLOC_T_NUM));
    pp.allocT->setAlignment(llvm::Align{16});

    const auto allocR      = pp.allocR;
    const auto allocRR     = pp.allocRR;
    const auto allocResult = pp.allocResult;
    const auto allocVa     = pp.allocVa;
    const auto allocStack  = pp.allocStack;
    const auto allocT      = pp.allocT;

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
        pp.ip      = ip;
        pp.ipIndex = i;

        // If we are the destination of a jump, be sure we have a block, and from now insert into that block
        if (ip->hasFlag(BCI_JUMP_DEST) || blockIsClosed)
        {
            auto label = getOrCreateLabel(pp, i);
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
            {
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1 = MK_IMMB_64();
                const auto r2 = builder.CreateInBoundsGEP(I8_TY(), r0, r1);
                builder.CreateStore(r2, GEP64_PTR_PTR_I8(allocR, ip->c.u32));
                break;
            }

            case ByteCodeOp::DecPointer64:
            {
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1 = MK_IMMB_64();
                const auto r2 = builder.CreateNeg(r1);
                const auto r3 = builder.CreateInBoundsGEP(I8_TY(), r0, r2);
                builder.CreateStore(r3, GEP64_PTR_PTR_I8(allocR, ip->c.u32));
                break;
            }

            case ByteCodeOp::IncMulPointer64:
            {
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1 = MK_IMMB_64();
                const auto r2 = builder.CreateMul(r1, builder.getInt64(ip->d.u64));
                const auto r3 = builder.CreateInBoundsGEP(I8_TY(), r0, r2);
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
                const auto r1      = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), r0), builder.getInt64(ip->c.s64));
                const auto r2      = builder.CreateLoad(IX_TY(numBits), r1);
                const auto r3      = builder.CreateIntCast(r2, I64_TY(), false);
                builder.CreateStore(r3, GEP64(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::DeRefStringSlice:
            {
                const auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                const auto r1 = GEP64_PTR_PTR_I8(allocR, ip->b.u32);
                const auto r2 = builder.CreateLoad(PTR_I8_TY(), r0);
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                const auto r3 = builder.CreateInBoundsGEP(I8_TY(), r2, builder.getInt32(ip->c.u32 + 0));
                const auto r4 = builder.CreateInBoundsGEP(I8_TY(), r2, builder.getInt32(ip->c.u32 + 8));
                const auto r5 = builder.CreateLoad(PTR_I8_TY(), r3);
                const auto r6 = builder.CreateLoad(PTR_I8_TY(), r4);
                builder.CreateStore(r6, r1);
                builder.CreateStore(r5, r0);
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
                const auto r2      = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r1), I64_TY(), false);
                builder.CreateStore(r2, r0);
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
                const auto r2      = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r1), I64_TY(), false);
                builder.CreateStore(r2, r0);
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
                const auto r1      = GEP8_PTR_IX(r0, ip->b.u32, numBits);
                builder.CreateStore(builder.getIntN(numBits, 0), r1);
                break;
            }

            case ByteCodeOp::SetZeroAtPointerX:
            {
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1 = builder.CreateInBoundsGEP(I8_TY(), r0, CST_RC32);
                builder.CreateMemSet(r1, builder.getInt8(0), ip->b.u64, llvm::Align{});
                break;
            }
            case ByteCodeOp::SetZeroAtPointerXRB:
            {
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->b.u32));
                const auto r2 = builder.CreateMul(r1, CST_RC64);
                builder.CreateMemSet(r0, builder.getInt8(0), r2, {});
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
                const auto r1      = GEP8_PTR_IX(r0, ip->c.u32, numBits);
                builder.CreateStore(builder.getIntN(numBits, 0), r1);
                break;
            }

            case ByteCodeOp::ClearRRX:
            {
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                const auto r0 = TO_PTR_I8(func->getArg(static_cast<int>(func->arg_size()) - 1));
                const auto r1 = builder.CreateInBoundsGEP(I8_TY(), r0, CST_RC32);
                builder.CreateMemSet(r1, builder.getInt8(0), ip->b.u64, llvm::Align{});
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::MakeMutableSegPointer:
            {
                const auto r0 = GEP8(pp.mutableSeg, ip->b.u32);
                builder.CreateStore(r0, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::MakeBssSegPointer:
            {
                const auto r0 = GEP8(pp.bssSeg, ip->b.u32);
                builder.CreateStore(r0, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::MakeConstantSegPointer:
            {
                const auto r0 = GEP8(pp.constantSeg, ip->b.u32);
                builder.CreateStore(r0, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::MakeCompilerSegPointer:
                break;

            case ByteCodeOp::MakeStackPointer:
            {
                const auto r0 = GEP8(allocStack, ip->b.u32);
                builder.CreateStore(r0, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::MakeStackPointerx2:
            {
                const auto r0 = GEP8(allocStack, ip->b.u32);
                builder.CreateStore(r0, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                const auto r1 = GEP8(allocStack, ip->d.u32);
                builder.CreateStore(r1, GEP64_PTR_PTR_I8(allocR, ip->c.u32));
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
                const auto r2      = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r1), I64_TY(), false);
                builder.CreateStore(r2, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8x2:
            case ByteCodeOp::GetFromStack16x2:
            case ByteCodeOp::GetFromStack32x2:
            case ByteCodeOp::GetFromStack64x2:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = GEP8(allocStack, ip->b.u32);
                const auto r2      = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r1), I64_TY(), false);
                builder.CreateStore(r2, r0);
                const auto r3 = GEP64(allocR, ip->c.u32);
                const auto r4 = GEP8(allocStack, ip->d.u32);
                const auto r5 = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r4), I64_TY(), false);
                builder.CreateStore(r5, r3);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::GetIncFromStack64:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = GEP8(allocStack, ip->b.u32);
                const auto r2 = builder.CreateLoad(I64_TY(), r1);
                const auto r3 = builder.CreateAdd(r2, builder.getInt64(ip->c.s64));
                builder.CreateStore(r3, r0);
                break;
            }

            case ByteCodeOp::GetIncFromStack64DeRef8:
            case ByteCodeOp::GetIncFromStack64DeRef16:
            case ByteCodeOp::GetIncFromStack64DeRef32:
            case ByteCodeOp::GetIncFromStack64DeRef64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8(allocStack, ip->b.u32);
                const auto r1      = builder.CreateLoad(PTR_I8_TY(), r0);
                const auto r2      = GEP8_PTR_IX(r1, ip->c.u64, numBits);
                const auto r3      = builder.CreateLoad(IX_TY(numBits), r2);
                const auto r4      = builder.CreateIntCast(r3, I64_TY(), false);
                builder.CreateStore(r4, GEP64(allocR, ip->a.u32));
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
                const auto result = emitCall(pp, g_LangSpec->name_at_dbgalloc, allocR, allocT, {}, {});
                builder.CreateStore(TO_PTR_I8(result), GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::IntrinsicSysAlloc:
            {
                const auto result = emitCall(pp, g_LangSpec->name_at_sysalloc, allocR, allocT, {}, {});
                builder.CreateStore(TO_PTR_I8(result), GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::IntrinsicRtFlags:
            {
                const auto result = emitCall(pp, g_LangSpec->name_at_rtflags, allocR, allocT, {}, {});
                builder.CreateStore(result, GEP64(allocR, ip->a.u32));
                break;
            }
            case ByteCodeOp::IntrinsicStringCmp:
            {
                const auto result = emitCall(pp, g_LangSpec->name_at_strcmp, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32}, {});
                builder.CreateStore(result, GEP64_PTR_I8(allocR, ip->d.u32));
                break;
            }
            case ByteCodeOp::IntrinsicTypeCmp:
            {
                const auto result = emitCall(pp, g_LangSpec->name_at_typecmp, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
                builder.CreateStore(result, GEP64_PTR_I8(allocR, ip->d.u32));
                break;
            }
            case ByteCodeOp::IntrinsicIs:
            {
                const auto result = emitCall(pp, g_LangSpec->name_at_is, allocR, allocT, {ip->a.u32, ip->b.u32}, {});
                builder.CreateStore(result, GEP64_PTR_I8(allocR, ip->c.u32));
                break;
            }
            case ByteCodeOp::IntrinsicAs:
            {
                const auto result = emitCall(pp, g_LangSpec->name_at_as, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
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
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->b.u32));
                const auto r2 = MK_IMMC_64();
                builder.CreateMemCpy(r0, {}, r1, {}, r2);
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
                const auto result = emitCall(pp, g_LangSpec->name_at_itftableof, allocR, allocT, {ip->a.u32, ip->b.u32}, {});
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
                const auto r2      = builder.CreateIntCast(builder.CreateLoad(IX_TY(numBits), r1), I64_TY(), false);
                builder.CreateStore(r2, r0);
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
                const auto r1      = builder.CreateSub(builder.CreateLoad(IX_TY(numBits), r0), builder.getIntN(numBits, 1));
                builder.CreateStore(r1, r0);
                break;
            }

            case ByteCodeOp::IncrementRA64:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = builder.CreateAdd(builder.CreateLoad(I64_TY(), r0), builder.getInt64(1));
                builder.CreateStore(r1, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::MulAddVC64:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = GEP64(allocR, ip->b.u32);
                const auto r2 = builder.CreateAdd(builder.CreateLoad(I64_TY(), r1), CST_RC64);
                const auto r3 = builder.CreateMul(builder.CreateLoad(I64_TY(), r0), r2);
                const auto r4 = GEP64(allocR, ip->a.u32);
                builder.CreateStore(r3, r4);
                break;
            }

            case ByteCodeOp::Add32byVB32:
            case ByteCodeOp::Add64byVB64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateAdd(builder.CreateLoad(IX_TY(numBits), r0), CST_RB(numBits));
                builder.CreateStore(r1, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetAtPointer8:
            case ByteCodeOp::SetAtPointer16:
            case ByteCodeOp::SetAtPointer32:
            case ByteCodeOp::SetAtPointer64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1      = GEP8_PTR_IX(r0, ip->c.u32, numBits);
                const auto r2      = MK_IMMB_IX(numBits);
                builder.CreateStore(r2, r1);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8:
            case ByteCodeOp::SetAtStackPointer16:
            case ByteCodeOp::SetAtStackPointer32:
            case ByteCodeOp::SetAtStackPointer64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8_PTR_IX(allocStack, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_IX(numBits);
                builder.CreateStore(r1, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8x2:
            case ByteCodeOp::SetAtStackPointer16x2:
            case ByteCodeOp::SetAtStackPointer32x2:
            case ByteCodeOp::SetAtStackPointer64x2:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8_PTR_IX(allocStack, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_IX(numBits);
                builder.CreateStore(r1, r0);
                const auto r2 = GEP8_PTR_IX(allocStack, ip->c.u32, numBits);
                const auto r3 = MK_IMMD_IX(numBits);
                builder.CreateStore(r3, r2);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpPlusS8:
            case ByteCodeOp::BinOpPlusS16:
            case ByteCodeOp::BinOpPlusS32:
            case ByteCodeOp::BinOpPlusS64:
            case ByteCodeOp::BinOpPlusU8:
            case ByteCodeOp::BinOpPlusU16:
            case ByteCodeOp::BinOpPlusU32:
            case ByteCodeOp::BinOpPlusU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1       = MK_IMMA_IX(numBits);
                const auto r2       = MK_IMMB_IX(numBits);
                if (BackendEncoder::mustCheckOverflow(module, ip))
                {
                    const auto op = isSigned ? llvm::Intrinsic::sadd_with_overflow : llvm::Intrinsic::uadd_with_overflow;
                    emitBinOpOverflow(pp, r0, r1, r2, op, SafetyMsg::Plus);
                }
                else
                    builder.CreateStore(builder.CreateAdd(r1, r2), r0);
                break;
            }

            case ByteCodeOp::BinOpPlusS8_Safe:
            case ByteCodeOp::BinOpPlusS16_Safe:
            case ByteCodeOp::BinOpPlusS32_Safe:
            case ByteCodeOp::BinOpPlusS64_Safe:
            case ByteCodeOp::BinOpPlusU8_Safe:
            case ByteCodeOp::BinOpPlusU16_Safe:
            case ByteCodeOp::BinOpPlusU32_Safe:
            case ByteCodeOp::BinOpPlusU64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_IX(numBits);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateAdd(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

            case ByteCodeOp::BinOpPlusF32_Safe:
            case ByteCodeOp::BinOpPlusF64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFAdd(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpMinusS8:
            case ByteCodeOp::BinOpMinusS16:
            case ByteCodeOp::BinOpMinusS32:
            case ByteCodeOp::BinOpMinusS64:
            case ByteCodeOp::BinOpMinusU8:
            case ByteCodeOp::BinOpMinusU16:
            case ByteCodeOp::BinOpMinusU32:
            case ByteCodeOp::BinOpMinusU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1       = MK_IMMA_IX(numBits);
                const auto r2       = MK_IMMB_IX(numBits);
                if (BackendEncoder::mustCheckOverflow(module, ip))
                {
                    const auto op = isSigned ? llvm::Intrinsic::ssub_with_overflow : llvm::Intrinsic::usub_with_overflow;
                    emitBinOpOverflow(pp, r0, r1, r2, op, SafetyMsg::Minus);
                }
                else
                    builder.CreateStore(builder.CreateSub(r1, r2), r0);
                break;
            }

            case ByteCodeOp::BinOpMinusS8_Safe:
            case ByteCodeOp::BinOpMinusS16_Safe:
            case ByteCodeOp::BinOpMinusS32_Safe:
            case ByteCodeOp::BinOpMinusS64_Safe:
            case ByteCodeOp::BinOpMinusU8_Safe:
            case ByteCodeOp::BinOpMinusU16_Safe:
            case ByteCodeOp::BinOpMinusU32_Safe:
            case ByteCodeOp::BinOpMinusU64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_IX(numBits);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateSub(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

            case ByteCodeOp::BinOpMinusF32_Safe:
            case ByteCodeOp::BinOpMinusF64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFSub(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpMulS8:
            case ByteCodeOp::BinOpMulS16:
            case ByteCodeOp::BinOpMulS32:
            case ByteCodeOp::BinOpMulS64:
            case ByteCodeOp::BinOpMulU8:
            case ByteCodeOp::BinOpMulU16:
            case ByteCodeOp::BinOpMulU32:
            case ByteCodeOp::BinOpMulU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1       = MK_IMMA_IX(numBits);
                const auto r2       = MK_IMMB_IX(numBits);
                if (BackendEncoder::mustCheckOverflow(module, ip))
                {
                    const auto op = isSigned ? llvm::Intrinsic::smul_with_overflow : llvm::Intrinsic::umul_with_overflow;
                    emitBinOpOverflow(pp, r0, r1, r2, op, SafetyMsg::Mul);
                }
                else
                    builder.CreateStore(builder.CreateMul(r1, r2), r0);
                break;
            }

            case ByteCodeOp::BinOpMulS8_Safe:
            case ByteCodeOp::BinOpMulS16_Safe:
            case ByteCodeOp::BinOpMulS32_Safe:
            case ByteCodeOp::BinOpMulS64_Safe:
            case ByteCodeOp::BinOpMulU8_Safe:
            case ByteCodeOp::BinOpMulU16_Safe:
            case ByteCodeOp::BinOpMulU32_Safe:
            case ByteCodeOp::BinOpMulU64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_IX(numBits);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateMul(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

            case ByteCodeOp::BinOpMulF32_Safe:
            case ByteCodeOp::BinOpMulF64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFMul(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpXorU8:
            case ByteCodeOp::BinOpXorU16:
            case ByteCodeOp::BinOpXorU32:
            case ByteCodeOp::BinOpXorU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_IX(numBits);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateXor(r1, r2);
                builder.CreateStore(r3, r0);
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
                emitShiftLogical(pp, allocR, numBits, true);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpShiftRightS8:
            case ByteCodeOp::BinOpShiftRightS16:
            case ByteCodeOp::BinOpShiftRightS32:
            case ByteCodeOp::BinOpShiftRightS64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                emitShiftRightArithmetic(pp, allocR, numBits);
                break;
            }

            case ByteCodeOp::BinOpShiftRightU8:
            case ByteCodeOp::BinOpShiftRightU16:
            case ByteCodeOp::BinOpShiftRightU32:
            case ByteCodeOp::BinOpShiftRightU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                emitShiftLogical(pp, allocR, numBits, false);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpModuloS8:
            case ByteCodeOp::BinOpModuloS16:
            case ByteCodeOp::BinOpModuloS32:
            case ByteCodeOp::BinOpModuloS64:
            case ByteCodeOp::BinOpModuloU8:
            case ByteCodeOp::BinOpModuloU16:
            case ByteCodeOp::BinOpModuloU32:
            case ByteCodeOp::BinOpModuloU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1       = MK_IMMA_IX(numBits);
                const auto r2       = MK_IMMB_IX(numBits);
                const auto r3       = isSigned ? builder.CreateSRem(r1, r2) : builder.CreateURem(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpDivS8:
            case ByteCodeOp::BinOpDivS16:
            case ByteCodeOp::BinOpDivS32:
            case ByteCodeOp::BinOpDivS64:
            case ByteCodeOp::BinOpDivU8:
            case ByteCodeOp::BinOpDivU16:
            case ByteCodeOp::BinOpDivU32:
            case ByteCodeOp::BinOpDivU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1       = MK_IMMA_IX(numBits);
                const auto r2       = MK_IMMB_IX(numBits);
                const auto r3       = isSigned ? builder.CreateSDiv(r1, r2) : builder.CreateUDiv(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

            case ByteCodeOp::BinOpDivF32:
            case ByteCodeOp::BinOpDivF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFDiv(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskAnd8:
            case ByteCodeOp::BinOpBitmaskAnd16:
            case ByteCodeOp::BinOpBitmaskAnd32:
            case ByteCodeOp::BinOpBitmaskAnd64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_IX(numBits);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateAnd(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskOr8:
            case ByteCodeOp::BinOpBitmaskOr16:
            case ByteCodeOp::BinOpBitmaskOr32:
            case ByteCodeOp::BinOpBitmaskOr64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_IX(numBits);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateOr(r1, r2);
                builder.CreateStore(r3, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMinusEqS8:
            case ByteCodeOp::AffectOpMinusEqS16:
            case ByteCodeOp::AffectOpMinusEqS32:
            case ByteCodeOp::AffectOpMinusEqS64:
            case ByteCodeOp::AffectOpMinusEqU8:
            case ByteCodeOp::AffectOpMinusEqU16:
            case ByteCodeOp::AffectOpMinusEqU32:
            case ByteCodeOp::AffectOpMinusEqU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64(allocR, ip->a.u32);
                const auto r1       = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2       = MK_IMMB_IX(numBits);
                if (BackendEncoder::mustCheckOverflow(module, ip))
                {
                    const auto op = isSigned ? llvm::Intrinsic::ssub_with_overflow : llvm::Intrinsic::usub_with_overflow;
                    emitBinOpEqOverflow(pp, r1, r2, op, SafetyMsg::MinusEq);
                }
                else
                {
                    auto r3 = builder.CreateLoad(IX_TY(numBits), r1);
                    builder.CreateStore(builder.CreateSub(r3, r2), r1);
                }
                break;
            }

            case ByteCodeOp::AffectOpMinusEqS8_Safe:
            case ByteCodeOp::AffectOpMinusEqS16_Safe:
            case ByteCodeOp::AffectOpMinusEqS32_Safe:
            case ByteCodeOp::AffectOpMinusEqS64_Safe:
            case ByteCodeOp::AffectOpMinusEqU8_Safe:
            case ByteCodeOp::AffectOpMinusEqU16_Safe:
            case ByteCodeOp::AffectOpMinusEqU32_Safe:
            case ByteCodeOp::AffectOpMinusEqU64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = GEP8_PTR_IX(r1, ip->c.u32, numBits);
                const auto r3      = MK_IMMB_IX(numBits);
                const auto r4      = builder.CreateSub(builder.CreateLoad(IX_TY(numBits), r2), r3);
                builder.CreateStore(r4, r2);
                break;
            }

            case ByteCodeOp::AffectOpMinusEqF32_Safe:
            case ByteCodeOp::AffectOpMinusEqF64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_FX_TY(numBits), r0);
                const auto r2      = GEP8_PTR_FX(r1, ip->c.u32, numBits);
                const auto r3      = MK_IMMB_FX(numBits);
                const auto r4      = builder.CreateFSub(builder.CreateLoad(FX_TY(numBits), r2), r3);
                builder.CreateStore(r4, r2);
                break;
            }

            case ByteCodeOp::AffectOpMinusEqS8_SSafe:
            case ByteCodeOp::AffectOpMinusEqS16_SSafe:
            case ByteCodeOp::AffectOpMinusEqS32_SSafe:
            case ByteCodeOp::AffectOpMinusEqS64_SSafe:
            case ByteCodeOp::AffectOpMinusEqU8_SSafe:
            case ByteCodeOp::AffectOpMinusEqU16_SSafe:
            case ByteCodeOp::AffectOpMinusEqU32_SSafe:
            case ByteCodeOp::AffectOpMinusEqU64_SSafe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8_PTR_IX(allocStack, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_IX(numBits);
                const auto r2      = builder.CreateSub(builder.CreateLoad(IX_TY(numBits), r0), r1);
                builder.CreateStore(r2, r0);
                break;
            }

            case ByteCodeOp::AffectOpMinusEqF32_SSafe:
            case ByteCodeOp::AffectOpMinusEqF64_SSafe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8_PTR_FX(allocStack, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_FX(numBits);
                auto       r2      = builder.CreateFSub(builder.CreateLoad(FX_TY(numBits), r0), r1);
                builder.CreateStore(r2, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpPlusEqS8:
            case ByteCodeOp::AffectOpPlusEqS16:
            case ByteCodeOp::AffectOpPlusEqS32:
            case ByteCodeOp::AffectOpPlusEqS64:
            case ByteCodeOp::AffectOpPlusEqU8:
            case ByteCodeOp::AffectOpPlusEqU16:
            case ByteCodeOp::AffectOpPlusEqU32:
            case ByteCodeOp::AffectOpPlusEqU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64(allocR, ip->a.u32);
                const auto r1       = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2       = MK_IMMB_IX(numBits);
                if (BackendEncoder::mustCheckOverflow(module, ip))
                {
                    const auto op = isSigned ? llvm::Intrinsic::sadd_with_overflow : llvm::Intrinsic::uadd_with_overflow;
                    emitBinOpEqOverflow(pp, r1, r2, op, SafetyMsg::PlusEq);
                }
                else
                {
                    auto r3 = builder.CreateLoad(IX_TY(numBits), r1);
                    builder.CreateStore(builder.CreateAdd(r3, r2), r1);
                }
                break;
            }

            case ByteCodeOp::AffectOpPlusEqS8_Safe:
            case ByteCodeOp::AffectOpPlusEqS16_Safe:
            case ByteCodeOp::AffectOpPlusEqS32_Safe:
            case ByteCodeOp::AffectOpPlusEqS64_Safe:
            case ByteCodeOp::AffectOpPlusEqU8_Safe:
            case ByteCodeOp::AffectOpPlusEqU16_Safe:
            case ByteCodeOp::AffectOpPlusEqU32_Safe:
            case ByteCodeOp::AffectOpPlusEqU64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = GEP8_PTR_IX(r1, ip->c.u32, numBits);
                const auto r3      = MK_IMMB_IX(numBits);
                const auto r4      = builder.CreateAdd(builder.CreateLoad(IX_TY(numBits), r2), r3);
                builder.CreateStore(r4, r2);
                break;
            }

            case ByteCodeOp::AffectOpPlusEqF32_Safe:
            case ByteCodeOp::AffectOpPlusEqF64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_FX_TY(numBits), r0);
                const auto r2      = GEP8_PTR_FX(r1, ip->c.u32, numBits);
                const auto r3      = MK_IMMB_FX(numBits);
                const auto r4      = builder.CreateFAdd(builder.CreateLoad(FX_TY(numBits), r2), r3);
                builder.CreateStore(r4, r2);
                break;
            }

            case ByteCodeOp::AffectOpPlusEqS8_SSafe:
            case ByteCodeOp::AffectOpPlusEqS16_SSafe:
            case ByteCodeOp::AffectOpPlusEqS32_SSafe:
            case ByteCodeOp::AffectOpPlusEqS64_SSafe:
            case ByteCodeOp::AffectOpPlusEqU8_SSafe:
            case ByteCodeOp::AffectOpPlusEqU16_SSafe:
            case ByteCodeOp::AffectOpPlusEqU32_SSafe:
            case ByteCodeOp::AffectOpPlusEqU64_SSafe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8_PTR_IX(allocStack, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_IX(numBits);
                const auto r2      = builder.CreateAdd(builder.CreateLoad(IX_TY(numBits), r0), r1);
                builder.CreateStore(r2, r0);
                break;
            }

            case ByteCodeOp::AffectOpPlusEqF32_SSafe:
            case ByteCodeOp::AffectOpPlusEqF64_SSafe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8_PTR_FX(allocStack, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_FX(numBits);
                const auto r2      = builder.CreateFAdd(builder.CreateLoad(FX_TY(numBits), r0), r1);
                builder.CreateStore(r2, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMulEqS8:
            case ByteCodeOp::AffectOpMulEqS16:
            case ByteCodeOp::AffectOpMulEqS32:
            case ByteCodeOp::AffectOpMulEqS64:
            case ByteCodeOp::AffectOpMulEqU8:
            case ByteCodeOp::AffectOpMulEqU16:
            case ByteCodeOp::AffectOpMulEqU32:
            case ByteCodeOp::AffectOpMulEqU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64(allocR, ip->a.u32);
                const auto r1       = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2       = MK_IMMB_IX(numBits);
                if (BackendEncoder::mustCheckOverflow(module, ip))
                {
                    const auto op = isSigned ? llvm::Intrinsic::smul_with_overflow : llvm::Intrinsic::umul_with_overflow;
                    emitBinOpEqOverflow(pp, r1, r2, op, SafetyMsg::MulEq);
                }
                else
                {
                    auto r3 = builder.CreateLoad(IX_TY(numBits), r1);
                    builder.CreateStore(builder.CreateMul(r3, r2), r1);
                }
                break;
            }

            case ByteCodeOp::AffectOpMulEqS8_Safe:
            case ByteCodeOp::AffectOpMulEqS16_Safe:
            case ByteCodeOp::AffectOpMulEqS32_Safe:
            case ByteCodeOp::AffectOpMulEqS64_Safe:
            case ByteCodeOp::AffectOpMulEqU8_Safe:
            case ByteCodeOp::AffectOpMulEqU16_Safe:
            case ByteCodeOp::AffectOpMulEqU32_Safe:
            case ByteCodeOp::AffectOpMulEqU64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = GEP8_PTR_IX(r1, ip->c.u32, numBits);
                const auto r3      = MK_IMMB_IX(numBits);
                const auto r4      = builder.CreateMul(builder.CreateLoad(IX_TY(numBits), r2), r3);
                builder.CreateStore(r4, r2);
                break;
            }

            case ByteCodeOp::AffectOpMulEqF32_Safe:
            case ByteCodeOp::AffectOpMulEqF64_Safe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_FX_TY(numBits), r0);
                const auto r2      = GEP8_PTR_FX(r1, ip->c.u32, numBits);
                const auto r3      = MK_IMMB_FX(numBits);
                const auto r4      = builder.CreateFMul(builder.CreateLoad(FX_TY(numBits), r2), r3);
                builder.CreateStore(r4, r2);
                break;
            }

            case ByteCodeOp::AffectOpMulEqS8_SSafe:
            case ByteCodeOp::AffectOpMulEqS16_SSafe:
            case ByteCodeOp::AffectOpMulEqS32_SSafe:
            case ByteCodeOp::AffectOpMulEqS64_SSafe:
            case ByteCodeOp::AffectOpMulEqU8_SSafe:
            case ByteCodeOp::AffectOpMulEqU16_SSafe:
            case ByteCodeOp::AffectOpMulEqU32_SSafe:
            case ByteCodeOp::AffectOpMulEqU64_SSafe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8_PTR_IX(allocStack, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_IX(numBits);
                const auto r2      = builder.CreateMul(builder.CreateLoad(IX_TY(numBits), r0), r1);
                builder.CreateStore(r2, r0);
                break;
            }

            case ByteCodeOp::AffectOpMulEqF32_SSafe:
            case ByteCodeOp::AffectOpMulEqF64_SSafe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8_PTR_FX(allocStack, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_FX(numBits);
                const auto r2      = builder.CreateFMul(builder.CreateLoad(FX_TY(numBits), r0), r1);
                builder.CreateStore(r2, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpDivEqS8:
            case ByteCodeOp::AffectOpDivEqS16:
            case ByteCodeOp::AffectOpDivEqS32:
            case ByteCodeOp::AffectOpDivEqS64:
            case ByteCodeOp::AffectOpDivEqU8:
            case ByteCodeOp::AffectOpDivEqU16:
            case ByteCodeOp::AffectOpDivEqU32:
            case ByteCodeOp::AffectOpDivEqU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64(allocR, ip->a.u32);
                const auto r1       = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2       = MK_IMMB_IX(numBits);
                const auto r3       = builder.CreateLoad(IX_TY(numBits), r1);
                const auto r4       = isSigned ? builder.CreateSDiv(r3, r2) : builder.CreateUDiv(r3, r2);
                builder.CreateStore(r4, r1);
                break;
            }

            case ByteCodeOp::AffectOpDivEqF32:
            case ByteCodeOp::AffectOpDivEqF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_FX_TY(numBits), r0);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFDiv(builder.CreateLoad(FX_TY(numBits), r1), r2);
                builder.CreateStore(r3, r1);
                break;
            }

            case ByteCodeOp::AffectOpDivEqS8_SSafe:
            case ByteCodeOp::AffectOpDivEqS16_SSafe:
            case ByteCodeOp::AffectOpDivEqS32_SSafe:
            case ByteCodeOp::AffectOpDivEqS64_SSafe:
            case ByteCodeOp::AffectOpDivEqU8_SSafe:
            case ByteCodeOp::AffectOpDivEqU16_SSafe:
            case ByteCodeOp::AffectOpDivEqU32_SSafe:
            case ByteCodeOp::AffectOpDivEqU64_SSafe:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP8_PTR_IX(allocStack, ip->a.u32, numBits);
                const auto r1       = MK_IMMB_IX(numBits);
                const auto r2       = builder.CreateLoad(IX_TY(numBits), r0);
                const auto r3       = isSigned ? builder.CreateSDiv(r2, r1) : builder.CreateUDiv(r2, r1);
                builder.CreateStore(r3, r0);
                break;
            }

            case ByteCodeOp::AffectOpDivEqF32_SSafe:
            case ByteCodeOp::AffectOpDivEqF64_SSafe:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP8_PTR_FX(allocStack, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_FX(numBits);
                const auto r2      = builder.CreateFDiv(builder.CreateLoad(FX_TY(numBits), r0), r1);
                builder.CreateStore(r2, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpModuloEqS8:
            case ByteCodeOp::AffectOpModuloEqS16:
            case ByteCodeOp::AffectOpModuloEqS32:
            case ByteCodeOp::AffectOpModuloEqS64:
            case ByteCodeOp::AffectOpModuloEqU8:
            case ByteCodeOp::AffectOpModuloEqU16:
            case ByteCodeOp::AffectOpModuloEqU32:
            case ByteCodeOp::AffectOpModuloEqU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64(allocR, ip->a.u32);
                const auto r1       = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2       = MK_IMMB_IX(numBits);
                const auto r3       = builder.CreateLoad(IX_TY(numBits), r1);
                const auto r4       = isSigned ? builder.CreateSRem(r3, r2) : builder.CreateURem(r3, r2);
                builder.CreateStore(r4, r1);
                break;
            }

            case ByteCodeOp::AffectOpModuloEqS8_SSafe:
            case ByteCodeOp::AffectOpModuloEqS16_SSafe:
            case ByteCodeOp::AffectOpModuloEqS32_SSafe:
            case ByteCodeOp::AffectOpModuloEqS64_SSafe:
            case ByteCodeOp::AffectOpModuloEqU8_SSafe:
            case ByteCodeOp::AffectOpModuloEqU16_SSafe:
            case ByteCodeOp::AffectOpModuloEqU32_SSafe:
            case ByteCodeOp::AffectOpModuloEqU64_SSafe:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP8_PTR_IX(allocStack, ip->a.u32, numBits);
                const auto r1       = MK_IMMB_IX(numBits);
                const auto r2       = builder.CreateLoad(IX_TY(numBits), r0);
                const auto r3       = isSigned ? builder.CreateSRem(r2, r1) : builder.CreateURem(r2, r1);
                builder.CreateStore(r3, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpAndEqU8:
            case ByteCodeOp::AffectOpAndEqU16:
            case ByteCodeOp::AffectOpAndEqU32:
            case ByteCodeOp::AffectOpAndEqU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateAnd(builder.CreateLoad(IX_TY(numBits), r1), r2);
                builder.CreateStore(r3, r1);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpOrEqU8:
            case ByteCodeOp::AffectOpOrEqU16:
            case ByteCodeOp::AffectOpOrEqU32:
            case ByteCodeOp::AffectOpOrEqU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateOr(builder.CreateLoad(IX_TY(numBits), r1), r2);
                builder.CreateStore(r3, r1);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpXorEqU8:
            case ByteCodeOp::AffectOpXorEqU16:
            case ByteCodeOp::AffectOpXorEqU32:
            case ByteCodeOp::AffectOpXorEqU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateXor(builder.CreateLoad(IX_TY(numBits), r1), r2);
                builder.CreateStore(r3, r1);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpShiftLeftEqS8:
            case ByteCodeOp::AffectOpShiftLeftEqU8:
            case ByteCodeOp::AffectOpShiftLeftEqS16:
            case ByteCodeOp::AffectOpShiftLeftEqU16:
            case ByteCodeOp::AffectOpShiftLeftEqS32:
            case ByteCodeOp::AffectOpShiftLeftEqU32:
            case ByteCodeOp::AffectOpShiftLeftEqS64:
            case ByteCodeOp::AffectOpShiftLeftEqU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                emitShiftEqLogical(pp, allocR, numBits, true);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::AffectOpShiftRightEqS8:
            case ByteCodeOp::AffectOpShiftRightEqS16:
            case ByteCodeOp::AffectOpShiftRightEqS32:
            case ByteCodeOp::AffectOpShiftRightEqS64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                emitShiftRightEqArithmetic(pp, allocR, numBits);
                break;
            }

            case ByteCodeOp::AffectOpShiftRightEqU8:
            case ByteCodeOp::AffectOpShiftRightEqU16:
            case ByteCodeOp::AffectOpShiftRightEqU32:
            case ByteCodeOp::AffectOpShiftRightEqU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                emitShiftEqLogical(pp, allocR, numBits, false);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CompareOpGreaterS8:
            case ByteCodeOp::CompareOpGreaterS16:
            case ByteCodeOp::CompareOpGreaterS32:
            case ByteCodeOp::CompareOpGreaterS64:
            case ByteCodeOp::CompareOpGreaterU8:
            case ByteCodeOp::CompareOpGreaterU16:
            case ByteCodeOp::CompareOpGreaterU32:
            case ByteCodeOp::CompareOpGreaterU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1       = MK_IMMA_IX(numBits);
                const auto r2       = MK_IMMB_IX(numBits);
                const auto r3       = isSigned ? builder.CreateICmpSGT(r1, r2) : builder.CreateICmpUGT(r1, r2);
                const auto r4       = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpGreaterF32:
            case ByteCodeOp::CompareOpGreaterF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFCmpUGT(r1, r2);
                const auto r4      = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpGreaterEqS8:
            case ByteCodeOp::CompareOpGreaterEqS16:
            case ByteCodeOp::CompareOpGreaterEqS32:
            case ByteCodeOp::CompareOpGreaterEqS64:
            case ByteCodeOp::CompareOpGreaterEqU8:
            case ByteCodeOp::CompareOpGreaterEqU16:
            case ByteCodeOp::CompareOpGreaterEqU32:
            case ByteCodeOp::CompareOpGreaterEqU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1       = MK_IMMA_IX(numBits);
                const auto r2       = MK_IMMB_IX(numBits);
                const auto r3       = isSigned ? builder.CreateICmpSGE(r1, r2) : builder.CreateICmpUGE(r1, r2);
                const auto r4       = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpGreaterEqF32:
            case ByteCodeOp::CompareOpGreaterEqF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFCmpUGE(r1, r2);
                const auto r4      = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpLowerS8:
            case ByteCodeOp::CompareOpLowerS16:
            case ByteCodeOp::CompareOpLowerS32:
            case ByteCodeOp::CompareOpLowerS64:
            case ByteCodeOp::CompareOpLowerU8:
            case ByteCodeOp::CompareOpLowerU16:
            case ByteCodeOp::CompareOpLowerU32:
            case ByteCodeOp::CompareOpLowerU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1       = MK_IMMA_IX(numBits);
                const auto r2       = MK_IMMB_IX(numBits);
                const auto r3       = isSigned ? builder.CreateICmpSLT(r1, r2) : builder.CreateICmpULT(r1, r2);
                const auto r4       = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpLowerF32:
            case ByteCodeOp::CompareOpLowerF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFCmpULT(r1, r2);
                const auto r4      = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpLowerEqS8:
            case ByteCodeOp::CompareOpLowerEqS16:
            case ByteCodeOp::CompareOpLowerEqS32:
            case ByteCodeOp::CompareOpLowerEqS64:
            case ByteCodeOp::CompareOpLowerEqU8:
            case ByteCodeOp::CompareOpLowerEqU16:
            case ByteCodeOp::CompareOpLowerEqU32:
            case ByteCodeOp::CompareOpLowerEqU64:
            {
                const auto numBits  = BackendEncoder::getNumBits(ip->op);
                const auto isSigned = BackendEncoder::getOpType(ip->op)->isNativeIntegerSigned();
                const auto r0       = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1       = MK_IMMA_IX(numBits);
                const auto r2       = MK_IMMB_IX(numBits);
                const auto r3       = isSigned ? builder.CreateICmpSLE(r1, r2) : builder.CreateICmpULE(r1, r2);
                const auto r4       = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpLowerEqF32:
            case ByteCodeOp::CompareOpLowerEqF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFCmpULE(r1, r2);
                const auto r4      = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpEqual8:
            case ByteCodeOp::CompareOpEqual16:
            case ByteCodeOp::CompareOpEqual32:
            case ByteCodeOp::CompareOpEqual64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_IX(numBits);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateICmpEQ(r1, r2);
                const auto r4      = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpEqualF32:
            case ByteCodeOp::CompareOpEqualF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFCmpOEQ(r1, r2);
                const auto r4      = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpNotEqual8:
            case ByteCodeOp::CompareOpNotEqual16:
            case ByteCodeOp::CompareOpNotEqual32:
            case ByteCodeOp::CompareOpNotEqual64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_IX(numBits);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateICmpNE(r1, r2);
                const auto r4      = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

            case ByteCodeOp::CompareOpNotEqualF32:
            case ByteCodeOp::CompareOpNotEqualF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->c.u32, numBits);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFCmpUNE(r1, r2);
                const auto r4      = builder.CreateIntCast(r3, I8_TY(), false);
                builder.CreateStore(r4, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CompareOp3Way8:
            case ByteCodeOp::CompareOp3Way16:
            case ByteCodeOp::CompareOp3Way32:
            case ByteCodeOp::CompareOp3Way64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_I32(allocR, ip->c.u32);
                const auto r1      = MK_IMMA_IX(numBits);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateSub(r1, r2);
                const auto r4      = builder.CreateIntCast(builder.CreateICmpSGT(r3, builder.getIntN(numBits, 0)), I32_TY(), false);
                const auto r5      = builder.CreateIntCast(builder.CreateICmpSLT(r3, builder.getIntN(numBits, 0)), I32_TY(), false);
                const auto r6      = builder.CreateSub(r4, r5);
                builder.CreateStore(r6, r0);
                break;
            }

            case ByteCodeOp::CompareOp3WayF32:
            case ByteCodeOp::CompareOp3WayF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_I32(allocR, ip->c.u32);
                const auto r1      = MK_IMMA_FX(numBits);
                const auto r2      = MK_IMMB_FX(numBits);
                const auto r3      = builder.CreateFSub(r1, r2);
                const auto r4      = builder.CreateIntCast(builder.CreateFCmpUGT(r3, llvm::ConstantFP::get(FX_TY(numBits), 0)), I32_TY(), false);
                const auto r5      = builder.CreateIntCast(builder.CreateFCmpULT(r3, llvm::ConstantFP::get(FX_TY(numBits), 0)), I32_TY(), false);
                const auto r6      = builder.CreateSub(r4, r5);
                builder.CreateStore(r6, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpDyn8:
            case ByteCodeOp::JumpDyn16:
            case ByteCodeOp::JumpDyn64:
            case ByteCodeOp::JumpDyn32:
            {
                const auto tableCompiler = reinterpret_cast<int32_t*>(buildParameters.module->compilerSegment.address(ip->d.u32));

                VectorNative<llvm::BasicBlock*> falseBlocks;
                VectorNative<llvm::BasicBlock*> trueBlocks;

                for (uint32_t idx = 0; idx < ip->c.u32; idx++)
                {
                    if (idx == 0)
                        falseBlocks.push_back(nullptr);
                    else
                        falseBlocks.push_back(getOrCreateLabel(pp, static_cast<int64_t>(UINT32_MAX) + g_UniqueID.fetch_add(1)));
                    trueBlocks.push_back(getOrCreateLabel(pp, i + static_cast<int64_t>(tableCompiler[idx]) + 1));
                }

                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = MK_IMMA_IX(numBits);
                const auto r1      = builder.getIntN(numBits, ip->b.u64);
                const auto r2      = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(r2, trueBlocks[1], falseBlocks[1]);
                builder.SetInsertPoint(falseBlocks[1]);

                for (uint32_t idx = 2; idx < ip->c.u32; idx++)
                {
                    const auto r3 = builder.getIntN(numBits, (idx - 1) + ip->b.u64);
                    const auto r4 = builder.CreateICmpEQ(r0, r3);
                    builder.CreateCondBr(r4, trueBlocks[idx], falseBlocks[idx]);
                    builder.SetInsertPoint(falseBlocks[idx]);
                }

                builder.CreateBr(trueBlocks[0]);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::Jump:
            {
                const auto label = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                builder.CreateBr(label);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfTrue:
            {
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = GEP64(allocR, ip->a.u32);
                const auto r1         = builder.CreateIsNotNull(builder.CreateLoad(I8_TY(), r0));
                builder.CreateCondBr(r1, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfFalse:
            {
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = GEP64(allocR, ip->a.u32);
                const auto r1         = builder.CreateIsNull(builder.CreateLoad(I8_TY(), r0));
                builder.CreateCondBr(r1, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfRTFalse:
            {
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = resultFuncCall ? resultFuncCall : builder.CreateLoad(I8_TY(), GEP8(allocRR, 0));
                const auto r1         = builder.CreateIsNull(r0);
                builder.CreateCondBr(r1, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }
            case ByteCodeOp::JumpIfRTTrue:
            {
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = resultFuncCall ? resultFuncCall : builder.CreateLoad(I8_TY(), GEP8(allocRR, 0));
                const auto r1         = builder.CreateIsNotNull(r0);
                builder.CreateCondBr(r1, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfZero8:
            case ByteCodeOp::JumpIfZero16:
            case ByteCodeOp::JumpIfZero32:
            case ByteCodeOp::JumpIfZero64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = GEP64(allocR, ip->a.u32);
                const auto r1         = builder.CreateIsNull(builder.CreateLoad(IX_TY(numBits), r0));
                builder.CreateCondBr(r1, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfNotZero8:
            case ByteCodeOp::JumpIfNotZero16:
            case ByteCodeOp::JumpIfNotZero32:
            case ByteCodeOp::JumpIfNotZero64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = GEP64(allocR, ip->a.u32);
                const auto r1         = builder.CreateIsNotNull(builder.CreateLoad(IX_TY(numBits), r0));
                builder.CreateCondBr(r1, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfNotEqual8:
            case ByteCodeOp::JumpIfNotEqual16:
            case ByteCodeOp::JumpIfNotEqual32:
            case ByteCodeOp::JumpIfNotEqual64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_IX(numBits);
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpNE(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfNotEqualF32:
            case ByteCodeOp::JumpIfNotEqualF64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_FX(numBits);
                const auto r1         = MK_IMMC_FX(numBits);
                const auto r2         = builder.CreateFCmpUNE(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfEqual8:
            case ByteCodeOp::JumpIfEqual16:
            case ByteCodeOp::JumpIfEqual32:
            case ByteCodeOp::JumpIfEqual64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_IX(numBits);
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfEqualF32:
            case ByteCodeOp::JumpIfEqualF64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_FX(numBits);
                const auto r1         = MK_IMMC_FX(numBits);
                const auto r2         = builder.CreateFCmpOEQ(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::IncJumpIfEqual64:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = builder.CreateAdd(builder.CreateLoad(I64_TY(), r0), builder.getInt64(1));
                builder.CreateStore(r1, r0);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r2         = MK_IMMA_64();
                const auto r3         = MK_IMMC_64();
                const auto r4         = builder.CreateICmpEQ(r2, r3);
                builder.CreateCondBr(r4, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfStackEqual8:
            case ByteCodeOp::JumpIfStackEqual16:
            case ByteCodeOp::JumpIfStackEqual32:
            case ByteCodeOp::JumpIfStackEqual64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = builder.CreateLoad(IX_TY(numBits), GEP8(allocStack, ip->a.u32));
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpEQ(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfStackNotEqual8:
            case ByteCodeOp::JumpIfStackNotEqual16:
            case ByteCodeOp::JumpIfStackNotEqual32:
            case ByteCodeOp::JumpIfStackNotEqual64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = builder.CreateLoad(IX_TY(numBits), GEP8(allocStack, ip->a.u32));
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpNE(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfDeRefEqual8:
            case ByteCodeOp::JumpIfDeRefEqual16:
            case ByteCodeOp::JumpIfDeRefEqual32:
            case ByteCodeOp::JumpIfDeRefEqual64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = GEP64(allocR, ip->a.u32);
                const auto r1         = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), r0), builder.getInt64(ip->d.s64));
                const auto r2         = builder.CreateLoad(IX_TY(numBits), r1);
                const auto r3         = MK_IMMC_IX(numBits);
                const auto r4         = builder.CreateICmpEQ(r2, r3);
                builder.CreateCondBr(r4, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfDeRefNotEqual8:
            case ByteCodeOp::JumpIfDeRefNotEqual16:
            case ByteCodeOp::JumpIfDeRefNotEqual32:
            case ByteCodeOp::JumpIfDeRefNotEqual64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = GEP64(allocR, ip->a.u32);
                const auto r1         = builder.CreateInBoundsGEP(I8_TY(), builder.CreateLoad(PTR_I8_TY(), r0), builder.getInt64(ip->d.s64));
                const auto r2         = builder.CreateLoad(IX_TY(numBits), r1);
                const auto r3         = MK_IMMC_IX(numBits);
                const auto r4         = builder.CreateICmpNE(r2, r3);
                builder.CreateCondBr(r4, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfLowerS8:
            case ByteCodeOp::JumpIfLowerS16:
            case ByteCodeOp::JumpIfLowerS32:
            case ByteCodeOp::JumpIfLowerS64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_IX(numBits);
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpSLT(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfLowerU8:
            case ByteCodeOp::JumpIfLowerU16:
            case ByteCodeOp::JumpIfLowerU32:
            case ByteCodeOp::JumpIfLowerU64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_IX(numBits);
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpULT(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfLowerF32:
            case ByteCodeOp::JumpIfLowerF64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_FX(numBits);
                const auto r1         = MK_IMMC_FX(numBits);
                const auto r2         = builder.CreateFCmpULT(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfLowerEqS8:
            case ByteCodeOp::JumpIfLowerEqS16:
            case ByteCodeOp::JumpIfLowerEqS32:
            case ByteCodeOp::JumpIfLowerEqS64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_IX(numBits);
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpSLE(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfLowerEqU8:
            case ByteCodeOp::JumpIfLowerEqU16:
            case ByteCodeOp::JumpIfLowerEqU32:
            case ByteCodeOp::JumpIfLowerEqU64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_IX(numBits);
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpULE(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfLowerEqF32:
            case ByteCodeOp::JumpIfLowerEqF64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_FX(numBits);
                const auto r1         = MK_IMMC_FX(numBits);
                const auto r2         = builder.CreateFCmpULE(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfGreaterEqS8:
            case ByteCodeOp::JumpIfGreaterEqS16:
            case ByteCodeOp::JumpIfGreaterEqS32:
            case ByteCodeOp::JumpIfGreaterEqS64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_IX(numBits);
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpSGE(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfGreaterEqU8:
            case ByteCodeOp::JumpIfGreaterEqU16:
            case ByteCodeOp::JumpIfGreaterEqU32:
            case ByteCodeOp::JumpIfGreaterEqU64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_IX(numBits);
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpUGE(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfGreaterEqF32:
            case ByteCodeOp::JumpIfGreaterEqF64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_FX(numBits);
                const auto r1         = MK_IMMC_FX(numBits);
                const auto r2         = builder.CreateFCmpUGE(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfGreaterS8:
            case ByteCodeOp::JumpIfGreaterS16:
            case ByteCodeOp::JumpIfGreaterS32:
            case ByteCodeOp::JumpIfGreaterS64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_IX(numBits);
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpSGT(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfGreaterU8:
            case ByteCodeOp::JumpIfGreaterU16:
            case ByteCodeOp::JumpIfGreaterU32:
            case ByteCodeOp::JumpIfGreaterU64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_IX(numBits);
                const auto r1         = MK_IMMC_IX(numBits);
                const auto r2         = builder.CreateICmpUGT(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfGreaterF32:
            case ByteCodeOp::JumpIfGreaterF64:
            {
                const auto numBits    = BackendEncoder::getNumBits(ip->op);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r0         = MK_IMMA_FX(numBits);
                const auto r1         = MK_IMMC_FX(numBits);
                const auto r2         = builder.CreateFCmpUGT(r0, r1);
                builder.CreateCondBr(r2, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CopyRAtoRRRet:
                getReturnResult(pp, returnType, ip->hasFlag(BCI_IMM_A), ip->a, allocR, allocResult);
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
                emitRet(pp, typeFunc, returnType, allocResult);
                blockIsClosed = true;
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicCompilerError:
            {
                const auto bcF = ip->node->resolvedSymbolOverload()->node->extByteCode()->bc;
                emitCall(pp, bcF->getCallName().cstr(), allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
                break;
            }

            case ByteCodeOp::IntrinsicCompilerWarning:
            {
                const auto bcF = ip->node->resolvedSymbolOverload()->node->extByteCode()->bc;
                emitCall(pp, bcF->getCallName().cstr(), allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
                break;
            }

            case ByteCodeOp::IntrinsicPanic:
                emitCall(pp, g_LangSpec->name_at_panic, allocR, allocT, {ip->a.u32, ip->b.u32, ip->c.u32}, {});
                break;

            case ByteCodeOp::Unreachable:
                emitInternalPanic(pp, allocR, allocT, "executing unreachable code");
                break;

            case ByteCodeOp::InternalUnreachable:
                builder.CreateUnreachable();
                blockIsClosed = true;
                break;

            case ByteCodeOp::InternalPanic:
                emitInternalPanic(pp, allocR, allocT, reinterpret_cast<const char*>(ip->d.pointer));
                break;

            case ByteCodeOp::InternalGetTlsPtr:
            {
                const auto r0     = builder.getInt64(module->tlsSegment.totalCount);
                const auto r1     = builder.CreateInBoundsGEP(I8_TY(), pp.tlsSeg, pp.cstAi64);
                const auto r2     = builder.CreateLoad(I64_TY(), pp.symTlsThreadLocalId);
                const auto result = emitCall(pp, g_LangSpec->name_priv_tlsGetPtr, allocR, allocT, {UINT32_MAX, UINT32_MAX, UINT32_MAX}, {r2, r0, r1});
                builder.CreateStore(result, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::IntrinsicGetContext:
            {
                const auto r0     = builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {pp.cstAi32, pp.cstCi32});
                const auto r1     = builder.CreateLoad(I64_TY(), r0);
                const auto result = emitCall(pp, g_LangSpec->name_priv_tlsGetValue, allocR, allocT, {UINT32_MAX}, {r1});
                builder.CreateStore(builder.CreatePtrToInt(result, I64_TY()), GEP64(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::IntrinsicSetContext:
            {
                const auto r0 = builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {pp.cstAi32, pp.cstCi32});
                const auto r1 = builder.CreateLoad(I64_TY(), r0);
                emitCall(pp, g_LangSpec->name_priv_tlsSetValue, allocR, allocT, {UINT32_MAX, ip->a.u32}, {r1, nullptr});
                break;
            }

            case ByteCodeOp::IntrinsicGetProcessInfos:
            {
                const auto r0 = TO_PTR_I8(builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, pp.cstAi64));
                const auto r1 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                builder.CreateStore(r0, r1);
                break;
            }

            case ByteCodeOp::IntrinsicCVaStart:
            {
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                builder.CreateIntrinsic(llvm::Intrinsic::vastart, {}, {r0});
                break;
            }

            case ByteCodeOp::IntrinsicCVaEnd:
            {
                const auto r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                builder.CreateIntrinsic(llvm::Intrinsic::vaend, {}, {r0});
                break;
            }

            case ByteCodeOp::IntrinsicCVaArg:
            {
                const auto numBits = ip->c.u32 * 8;
                const auto r0      = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, ip->a.u32));
                const auto r1      = builder.CreateVAArg(r0, I64_TY());
                const auto r2      = builder.CreateIntCast(r1, IX_TY(numBits), false);
                builder.CreateStore(r2, GEP64_PTR_IX(allocR, ip->b.u32, numBits));
                break;
            }

            case ByteCodeOp::IntrinsicArguments:
                emitCall(pp, g_LangSpec->name_at_args, allocR, allocT, {}, {});
                emitRT2ToRegisters(pp, ip->a.u32, ip->b.u32, allocR, allocT);
                break;

            case ByteCodeOp::IntrinsicIsByteCode:
            {
                const auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                builder.CreateStore(builder.getInt8(0), r0);
                break;
            }

            case ByteCodeOp::IntrinsicCompiler:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                builder.CreateStore(pp.cstAi64, r0);
                const auto r1 = GEP64(allocR, ip->b.u32);
                builder.CreateStore(pp.cstAi64, r1);
                break;
            }

            case ByteCodeOp::IntrinsicModules:
            {
                if (buildParameters.module->modulesSliceOffset == UINT32_MAX)
                {
                    const auto r0 = GEP64(allocR, ip->a.u32);
                    builder.CreateStore(pp.cstAi64, r0);
                    const auto r1 = GEP64(allocR, ip->b.u32);
                    builder.CreateStore(pp.cstAi64, r1);
                }
                else
                {
                    const auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                    const auto r1 = GEP8(pp.constantSeg, buildParameters.module->modulesSliceOffset);
                    builder.CreateStore(r1, r0);
                    const auto r2 = GEP64(allocR, ip->b.u32);
                    builder.CreateStore(builder.getInt64(buildParameters.module->moduleDependencies.count + 1), r2);
                }
                break;
            }

            case ByteCodeOp::IntrinsicGvtd:
            {
                if (buildParameters.module->globalVarsToDropSliceOffset == UINT32_MAX)
                {
                    const auto r0 = GEP64(allocR, ip->a.u32);
                    builder.CreateStore(pp.cstAi64, r0);
                    const auto r1 = GEP64(allocR, ip->b.u32);
                    builder.CreateStore(pp.cstAi64, r1);
                }
                else
                {
                    const auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                    const auto r1 = GEP8(pp.mutableSeg, buildParameters.module->globalVarsToDropSliceOffset);
                    builder.CreateStore(r1, r0);
                    const auto r2 = GEP64(allocR, ip->b.u32);
                    builder.CreateStore(builder.getInt64(buildParameters.module->globalVarsToDrop.count), r2);
                }
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::InternalFailedAssume:
                emitCall(pp, g_LangSpec->name_priv_failedAssume, allocR, allocT, {ip->a.u32}, {});
                break;

            case ByteCodeOp::IntrinsicGetErr:
                emitCall(pp, g_LangSpec->name_at_err, allocR, allocT, {}, {});
                emitRT2ToRegisters(pp, ip->a.u32, ip->b.u32, allocR, allocT);
                break;

            case ByteCodeOp::InternalSetErr:
                emitCall(pp, g_LangSpec->name_priv_seterr, allocR, allocT, {ip->a.u32, ip->b.u32}, {});
                break;

            case ByteCodeOp::InternalHasErr:
            {
                const auto r0 = GEP64(allocR, ip->b.u32);
                const auto r1 = builder.CreateLoad(PTR_I8_TY(), r0);
                const auto r2 = GEP8(r1, offsetof(SwagContext, hasError));
                const auto r3 = GEP64_PTR_I32(allocR, ip->a.u32);
                builder.CreateStore(builder.CreateLoad(I32_TY(), r2), r3);
                break;
            }

            case ByteCodeOp::JumpIfError:
            {
                const auto r0         = GEP64(allocR, ip->a.u32);
                const auto r1         = builder.CreateLoad(PTR_I8_TY(), r0);
                const auto r2         = GEP8(r1, offsetof(SwagContext, hasError));
                const auto r3         = builder.CreateLoad(I32_TY(), r2);
                const auto labelFalse = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelTrue  = getOrCreateLabel(pp, i + 1);
                const auto r4         = builder.CreateIsNull(r3);
                builder.CreateCondBr(r4, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::JumpIfNoError:
            {
                const auto r0         = GEP64(allocR, ip->a.u32);
                const auto r1         = builder.CreateLoad(PTR_I8_TY(), r0);
                const auto r2         = GEP8(r1, offsetof(SwagContext, hasError));
                const auto r3         = builder.CreateLoad(I32_TY(), r2);
                const auto labelTrue  = getOrCreateLabel(pp, i + ip->b.s32 + 1);
                const auto labelFalse = getOrCreateLabel(pp, i + 1);
                const auto r4         = builder.CreateIsNull(r3);
                builder.CreateCondBr(r4, labelTrue, labelFalse);
                blockIsClosed = true;
                break;
            }

            case ByteCodeOp::InternalPushErr:
                emitCall(pp, g_LangSpec->name_priv_pusherr, allocR, allocT, {}, {});
                break;

            case ByteCodeOp::InternalPopErr:
                emitCall(pp, g_LangSpec->name_priv_poperr, allocR, allocT, {}, {});
                break;

            case ByteCodeOp::InternalCatchErr:
                emitCall(pp, g_LangSpec->name_priv_catcherr, allocR, allocT, {}, {});
                break;

            case ByteCodeOp::InternalInitStackTrace:
            {
                const auto r0 = GEP64(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(PTR_I8_TY(), r0);
                const auto r2 = GEP8_PTR_I32(r1, offsetof(SwagContext, traceIndex));
                builder.CreateStore(pp.cstAi32, r2);
                break;
            }

            case ByteCodeOp::InternalStackTraceConst:
            {
                const auto r0 = GEP8(pp.constantSeg, ip->b.u32);
                builder.CreateStore(r0, GEP64_PTR_PTR_I8(allocR, ip->a.u32));
                emitCall(pp, g_LangSpec->name_priv_stackTrace, allocR, allocT, {ip->a.u32}, {});
                break;
            }

            case ByteCodeOp::InternalStackTrace:
                emitCall(pp, g_LangSpec->name_priv_stackTrace, allocR, allocT, {ip->a.u32}, {});
                break;

                /////////////////////////////////////

            case ByteCodeOp::NegBool:
            {
                const auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                const auto r1 = GEP64(allocR, ip->b.u32);
                const auto r2 = builder.CreateXor(builder.CreateLoad(I8_TY(), r1), pp.cstBi8);
                builder.CreateStore(r2, r0);
                break;
            }

            case ByteCodeOp::NegS32:
            case ByteCodeOp::NegS64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->a.u32, numBits);
                const auto r1      = GEP64(allocR, ip->b.u32);
                const auto r2      = builder.CreateNeg(builder.CreateLoad(IX_TY(numBits), r1));
                builder.CreateStore(r2, r0);
                break;
            }

            case ByteCodeOp::NegF32:
            case ByteCodeOp::NegF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->a.u32, numBits);
                const auto r1      = GEP64(allocR, ip->b.u32);
                const auto r2      = builder.CreateFNeg(builder.CreateLoad(FX_TY(numBits), r1));
                builder.CreateStore(r2, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::InvertU8:
            case ByteCodeOp::InvertU16:
            case ByteCodeOp::InvertU32:
            case ByteCodeOp::InvertU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->a.u32, numBits);
                const auto r1      = GEP64(allocR, ip->b.u32);
                const auto r2      = builder.CreateNot(builder.CreateLoad(IX_TY(numBits), r1));
                builder.CreateStore(r2, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::ClearMaskU32:
            case ByteCodeOp::ClearMaskU64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->a.u32, numBits);
                const auto r1      = builder.CreateAnd(builder.CreateLoad(IX_TY(numBits), r0), CST_RB(numBits));
                builder.CreateStore(r1, r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::CastBool8:
            case ByteCodeOp::CastBool16:
            case ByteCodeOp::CastBool32:
            case ByteCodeOp::CastBool64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_I8(allocR, ip->a.u32);
                const auto r1      = GEP64(allocR, ip->b.u32);
                const auto r2      = builder.CreateIsNotNull(builder.CreateLoad(IX_TY(numBits), r1));
                const auto r3      = builder.CreateIntCast(r2, I8_TY(), false);
                builder.CreateStore(r3, r0);
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
                getReturnResult(pp, returnType, ip->hasFlag(BCI_IMM_A), ip->a, allocR, allocResult);
                break;

            case ByteCodeOp::CopyRARBtoRR2:
            {
                const auto r0 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->a.u32));
                const auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->b.u32));
                const auto r2 = TO_PTR_I64(func->getArg(static_cast<int>(func->arg_size()) - 1));
                builder.CreateStore(r0, r2);
                builder.CreateStore(r1, builder.CreateInBoundsGEP(I64_TY(), r2, builder.getInt64(1)));
                break;
            }

            case ByteCodeOp::CopyRAtoRT:
            {
                const auto r0 = GEP64(allocRR, 0);
                const auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->a.u32));
                builder.CreateStore(r1, r0);
                break;
            }

            case ByteCodeOp::SaveRRtoRA:
            {
                const auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                const auto r1 = TO_PTR_I8(func->getArg(static_cast<int>(func->arg_size()) - 1));
                builder.CreateStore(r1, r0);
                break;
            }

            case ByteCodeOp::CopyRRtoRA:
            {
                const auto r0 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                const auto r1 = TO_PTR_I8(func->getArg(static_cast<int>(func->arg_size()) - 1));
                const auto r2 = builder.CreateInBoundsGEP(I8_TY(), r1, builder.getInt64(ip->b.u64));
                builder.CreateStore(r2, r0);
                break;
            }

            case ByteCodeOp::CopyRTtoRA:
            {
                if (resultFuncCall)
                    emitTypedValueToRegister(pp, resultFuncCall, ip->a.u32, allocR);
                else
                {
                    const auto r0 = GEP64(allocR, ip->a.u32);
                    const auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocRR, 0));
                    builder.CreateStore(r1, r0);
                }
                break;
            }

            case ByteCodeOp::CopyRT2toRARB:
                emitRT2ToRegisters(pp, ip->a.u32, ip->b.u32, allocR, allocRR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetParam8:
            case ByteCodeOp::GetParam16:
            case ByteCodeOp::GetParam32:
            {
                const auto numBytes = BackendEncoder::getNumBytes(ip->op);
                SWAG_CHECK(emitGetParam(pp, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, numBytes));
                break;
            }

            case ByteCodeOp::GetParam64:
                SWAG_CHECK(emitGetParam(pp, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR));
                break;
            case ByteCodeOp::GetParam64x2:
                SWAG_CHECK(emitGetParam(pp, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR));
                SWAG_CHECK(emitGetParam(pp, typeFunc, ip->c.u32, ip->d.mergeU64U32.high, allocR));
                break;
            case ByteCodeOp::GetIncParam64:
                SWAG_CHECK(emitGetParam(pp, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, ip->d.u64));
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetParam64DeRef8:
            case ByteCodeOp::GetParam64DeRef16:
            case ByteCodeOp::GetParam64DeRef32:
            case ByteCodeOp::GetParam64DeRef64:
            {
                const auto numBytes = BackendEncoder::getNumBytes(ip->op);
                SWAG_CHECK(emitGetParam(pp, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, 0, numBytes));
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::GetIncParam64DeRef8:
            case ByteCodeOp::GetIncParam64DeRef16:
            case ByteCodeOp::GetIncParam64DeRef32:
            case ByteCodeOp::GetIncParam64DeRef64:
            {
                const auto numBytes = BackendEncoder::getNumBytes(ip->op);
                SWAG_CHECK(emitGetParam(pp, typeFunc, ip->a.u32, ip->b.mergeU64U32.high, allocR, 0, ip->d.u64, numBytes));
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::ZeroToTrue:
            {
                const auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(I32_TY(), r0);
                const auto r2 = builder.CreateIntCast(builder.CreateICmpEQ(r1, builder.getInt32(0)), I8_TY(), false);
                builder.CreateStore(r2, r0);
                break;
            }

            case ByteCodeOp::LowerZeroToTrue:
            {
                const auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(I32_TY(), r0);
                const auto r2 = builder.CreateIntCast(builder.CreateICmpSLT(r1, builder.getInt32(0)), I8_TY(), false);
                builder.CreateStore(r2, r0);
                break;
            }

            case ByteCodeOp::LowerEqZeroToTrue:
            {
                const auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(I32_TY(), r0);
                const auto r2 = builder.CreateIntCast(builder.CreateICmpSLE(r1, builder.getInt32(0)), I8_TY(), false);
                builder.CreateStore(r2, r0);
                break;
            }

            case ByteCodeOp::GreaterZeroToTrue:
            {
                const auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(I32_TY(), r0);
                const auto r2 = builder.CreateIntCast(builder.CreateICmpSGT(r1, builder.getInt32(0)), I8_TY(), false);
                builder.CreateStore(r2, r0);
                break;
            }

            case ByteCodeOp::GreaterEqZeroToTrue:
            {
                const auto r0 = GEP64_PTR_I8(allocR, ip->a.u32);
                const auto r1 = builder.CreateLoad(I32_TY(), r0);
                const auto r2 = builder.CreateIntCast(builder.CreateICmpSGE(r1, builder.getInt32(0)), I8_TY(), false);
                builder.CreateStore(r2, r0);
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

                /////////////////////////////////////

            case ByteCodeOp::CopySP:
            {
                const auto r0 = GEP64_PTR_PTR_I64(allocR, ip->a.u32);
                const auto r1 = GEP64(allocR, ip->c.u32);
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

                /////////////////////////////////////

            case ByteCodeOp::MakeLambda:
            {
                const auto funcNode     = castAst<AstFuncDecl>(reinterpret_cast<AstNode*>(ip->b.pointer), AstNodeKind::FuncDecl);
                const auto callName     = funcNode->getCallName();
                const auto typeFuncNode = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
                const auto llvmFctTy    = getOrCreateFuncType(pp, typeFuncNode);
                const auto llvmFct      = reinterpret_cast<llvm::Function*>(modu.getOrInsertFunction(callName.cstr(), llvmFctTy).getCallee());
                const auto r0           = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                builder.CreateStore(TO_PTR_I8(llvmFct), r0);
                const auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->a.u32));
                builder.CreateStore(r1, GEP64(allocR, ip->a.u32));
                break;
            }

            case ByteCodeOp::IntrinsicMakeCallback:
            {
                const auto blockLambdaBC = llvm::BasicBlock::Create(context, "", func);
                const auto blockNext     = llvm::BasicBlock::Create(context, "", func);

                const auto r0 = builder.CreateLoad(I64_TY(), GEP64(allocR, ip->a.u32));
                const auto r1 = builder.CreateAnd(r0, builder.getInt64(SWAG_LAMBDA_BC_MARKER));
                const auto r2 = builder.CreateIsNotNull(r1);
                builder.CreateCondBr(r2, blockLambdaBC, blockNext);
                builder.SetInsertPoint(blockLambdaBC);
                const auto r3 = builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {pp.cstAi32, pp.cstFi32});
                const auto r4 = builder.CreateLoad(PTR_I8_TY(), r3);
                const auto r5 = llvm::PointerType::getUnqual(pp.makeCallbackTy);
                const auto r6 = builder.CreatePointerCast(r4, r5);
                const auto r7 = builder.CreateIntToPtr(r0, PTR_I8_TY());
                const auto r8 = builder.CreateCall(pp.makeCallbackTy, r6, {r7});
                const auto r9 = GEP64_PTR_PTR_I8(allocR, ip->a.u32);
                builder.CreateStore(r8, r9);
                builder.CreateBr(blockNext);
                builder.SetInsertPoint(blockNext);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::LocalCall:
            case ByteCodeOp::LocalCallPop:
            case ByteCodeOp::LocalCallPop16:
            case ByteCodeOp::LocalCallPop48:
            case ByteCodeOp::LocalCallPopRC:
                emitLocalCall(pp, allocR, allocRR, pushRVParams, pushRAParams, resultFuncCall);
                break;
            case ByteCodeOp::LocalCallPopParam:
                pushRAParams.push_back(ip->d.u32);
                emitLocalCall(pp, allocR, allocRR, pushRVParams, pushRAParams, resultFuncCall);
                break;
            case ByteCodeOp::LocalCallPop0Param2:
            case ByteCodeOp::LocalCallPop16Param2:
            case ByteCodeOp::LocalCallPop48Param2:
                pushRAParams.push_back(ip->c.u32);
                pushRAParams.push_back(ip->d.u32);
                emitLocalCall(pp, allocR, allocRR, pushRVParams, pushRAParams, resultFuncCall);
                break;

            case ByteCodeOp::ForeignCall:
            case ByteCodeOp::ForeignCallPop:
                emitForeignCall(pp, allocR, allocRR, pushRVParams, pushRAParams, resultFuncCall);
                break;
            case ByteCodeOp::ForeignCallPopParam:
                pushRAParams.push_back(ip->d.u32);
                emitForeignCall(pp, allocR, allocRR, pushRVParams, pushRAParams, resultFuncCall);
                break;
            case ByteCodeOp::ForeignCallPop0Param2:
            case ByteCodeOp::ForeignCallPop16Param2:
            case ByteCodeOp::ForeignCallPop48Param2:
                pushRAParams.push_back(ip->c.u32);
                pushRAParams.push_back(ip->d.u32);
                emitForeignCall(pp, allocR, allocRR, pushRVParams, pushRAParams, resultFuncCall);
                break;

            case ByteCodeOp::LambdaCall:
            case ByteCodeOp::LambdaCallPop:
                SWAG_CHECK(emitLambdaCall(pp, allocR, allocRR, allocT, pushRVParams, pushRAParams, resultFuncCall));
                break;
            case ByteCodeOp::LambdaCallPopParam:
                pushRAParams.push_back(ip->d.u32);
                SWAG_CHECK(emitLambdaCall(pp, allocR, allocRR, allocT, pushRVParams, pushRAParams, resultFuncCall));
                break;

            case ByteCodeOp::IncSPPostCall:
            case ByteCodeOp::IncSPPostCallCond:
                pushRAParams.clear();
                pushRVParams.clear();
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMulAddF32:
            case ByteCodeOp::IntrinsicMulAddF64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_FX(numBits);
                const auto r2      = MK_IMMC_FX(numBits);
                const auto r3      = MK_IMMD_FX(numBits);
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fmuladd, {FX_TY(numBits)}, {r1, r2, r3}), r0);
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicS8x1:
            case ByteCodeOp::IntrinsicS16x1:
            case ByteCodeOp::IntrinsicS32x1:
            case ByteCodeOp::IntrinsicS64x1:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_IX(numBits);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {IX_TY(numBits)}, {r1, builder.getInt1(false)}), r0);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctpop, {IX_TY(numBits)}, {r1}), r0);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cttz, {IX_TY(numBits)}, {r1, builder.getInt1(false)}), r0);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ctlz, {IX_TY(numBits)}, {r1, builder.getInt1(false)}), r0);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::bswap, {IX_TY(numBits)}, {r1}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicS8x2:
            case ByteCodeOp::IntrinsicS16x2:
            case ByteCodeOp::IntrinsicS32x2:
            case ByteCodeOp::IntrinsicS64x2:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_IX(numBits);
                const auto r2      = MK_IMMC_IX(numBits);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {IX_TY(numBits)}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {IX_TY(numBits)}, {r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicU8x2:
            case ByteCodeOp::IntrinsicU16x2:
            case ByteCodeOp::IntrinsicU32x2:
            case ByteCodeOp::IntrinsicU64x2:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_IX(allocR, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_IX(numBits);
                const auto r2      = MK_IMMC_IX(numBits);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {IX_TY(numBits)}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {IX_TY(numBits)}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicRol:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fshl, {IX_TY(numBits)}, {r1, r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicRor:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fshr, {IX_TY(numBits)}, {r1, r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicF32x2:
            case ByteCodeOp::IntrinsicF64x2:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_FX(numBits);
                const auto r2      = MK_IMMC_FX(numBits);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicPow:
                        builder.CreateStore(builder.CreateCall(numBits == 32 ? pp.fnPowF32 : pp.fnPowF64, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::minnum, {FX_TY(numBits)}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicMax:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::maxnum, {FX_TY(numBits)}, {r1, r2}), r0);
                        break;
                    case TokenId::IntrinsicATan2:
                        builder.CreateStore(builder.CreateCall(numBits == 32 ? pp.fnAtan2F32 : pp.fnAtan2F64, {r1, r2}), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicF32x1:
            case ByteCodeOp::IntrinsicF64x1:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64_PTR_FX(allocR, ip->a.u32, numBits);
                const auto r1      = MK_IMMB_FX(numBits);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicTan:
                        builder.CreateStore(builder.CreateCall(numBits == 32 ? pp.fnTanF32 : pp.fnTanF64, r1), r0);
                        break;
                    case TokenId::IntrinsicSinh:
                        builder.CreateStore(builder.CreateCall(numBits == 32 ? pp.fnSinhF32 : pp.fnSinhF64, r1), r0);
                        break;
                    case TokenId::IntrinsicCosh:
                        builder.CreateStore(builder.CreateCall(numBits == 32 ? pp.fnCoshF32 : pp.fnCoshF64, r1), r0);
                        break;
                    case TokenId::IntrinsicTanh:
                        builder.CreateStore(builder.CreateCall(numBits == 32 ? pp.fnTanhF32 : pp.fnTanhF64, r1), r0);
                        break;
                    case TokenId::IntrinsicASin:
                        builder.CreateStore(builder.CreateCall(numBits == 32 ? pp.fnAsinF32 : pp.fnAsinF64, r1), r0);
                        break;
                    case TokenId::IntrinsicACos:
                        builder.CreateStore(builder.CreateCall(numBits == 32 ? pp.fnAcosF32 : pp.fnAcosF64, r1), r0);
                        break;
                    case TokenId::IntrinsicATan:
                        builder.CreateStore(builder.CreateCall(numBits == 32 ? pp.fnAtanF32 : pp.fnAtanF64, r1), r0);
                        break;
                    case TokenId::IntrinsicSqrt:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sqrt, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicSin:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sin, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicCos:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cos, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicLog:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicLog2:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log2, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicLog10:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log10, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicFloor:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::floor, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicCeil:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ceil, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicTrunc:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::trunc, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicRound:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::round, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicAbs:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fabs, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicExp:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp, FX_TY(numBits), r1), r0);
                        break;
                    case TokenId::IntrinsicExp2:
                        builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp2, FX_TY(numBits), r1), r0);
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }
                break;
            }

            case ByteCodeOp::IntrinsicAtomicAddS8:
            case ByteCodeOp::IntrinsicAtomicAddS16:
            case ByteCodeOp::IntrinsicAtomicAddS32:
            case ByteCodeOp::IntrinsicAtomicAddS64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(r3, GEP64_PTR_IX(allocR, ip->c.u32, numBits));
                break;
            }

            case ByteCodeOp::IntrinsicAtomicAndS8:
            case ByteCodeOp::IntrinsicAtomicAndS16:
            case ByteCodeOp::IntrinsicAtomicAndS32:
            case ByteCodeOp::IntrinsicAtomicAndS64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(r3, GEP64_PTR_IX(allocR, ip->c.u32, numBits));
                break;
            }

            case ByteCodeOp::IntrinsicAtomicOrS8:
            case ByteCodeOp::IntrinsicAtomicOrS16:
            case ByteCodeOp::IntrinsicAtomicOrS32:
            case ByteCodeOp::IntrinsicAtomicOrS64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(r3, GEP64_PTR_IX(allocR, ip->c.u32, numBits));
                break;
            }

            case ByteCodeOp::IntrinsicAtomicXorS8:
            case ByteCodeOp::IntrinsicAtomicXorS16:
            case ByteCodeOp::IntrinsicAtomicXorS32:
            case ByteCodeOp::IntrinsicAtomicXorS64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(r3, GEP64_PTR_IX(allocR, ip->c.u32, numBits));
                break;
            }

            case ByteCodeOp::IntrinsicAtomicXchgS8:
            case ByteCodeOp::IntrinsicAtomicXchgS16:
            case ByteCodeOp::IntrinsicAtomicXchgS32:
            case ByteCodeOp::IntrinsicAtomicXchgS64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = GEP64(allocR, ip->a.u32);
                const auto r1      = builder.CreateLoad(PTR_IX_TY(numBits), r0);
                const auto r2      = MK_IMMB_IX(numBits);
                const auto r3      = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent);
                builder.CreateStore(r3, GEP64_PTR_IX(allocR, ip->c.u32, numBits));
                break;
            }

            case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
            {
                const auto numBits = BackendEncoder::getNumBits(ip->op);
                const auto r0      = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
                const auto r1      = MK_IMMB_IX(numBits);
                const auto r2      = MK_IMMC_IX(numBits);
                auto       r3      = builder.CreateAtomicCmpXchg(r0, r1, r2, {}, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
                r3->setVolatile(true);
                auto r4 = builder.CreateExtractValue(r3, 0);
                builder.CreateStore(r4, GEP64_PTR_IX(allocR, ip->d.u32, numBits));
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
        emitRet(pp, typeFunc, returnType, allocResult);
    }

    return ok;
}

llvm::Type* LLVM::getLLVMType(LLVM_Encoder& pp, TypeInfo* typeInfo)
{
    auto& context = *pp.llvmContext;

    typeInfo = typeInfo->getConcreteAlias();

    if (typeInfo->isEnum())
    {
        const auto typeInfoEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        return getLLVMType(pp, typeInfoEnum->rawType);
    }

    if (typeInfo->isPointer())
    {
        const auto typeInfoPointer = castTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        const auto pointedType     = TypeManager::concreteType(typeInfoPointer->pointedType);
        if (pointedType->isVoid())
            return PTR_I8_TY();
        return getLLVMType(pp, pointedType)->getPointerTo();
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

llvm::BasicBlock* LLVM::getOrCreateLabel(LLVM_Encoder& pp, int64_t ip)
{
    auto& context = *pp.llvmContext;

    const auto it = pp.labels.find(ip);
    if (it == pp.labels.end())
    {
        llvm::BasicBlock* label = llvm::BasicBlock::Create(context, form("%lld", ip).cstr(), pp.llvmFunc);
        pp.labels[ip]           = label;
        return label;
    }

    return it->second;
}

void LLVM::setFuncAttributes(LLVM_Encoder& pp, uint32_t numPreCompileBuffers, const AstFuncDecl* funcNode, const ByteCode* bc)
{
    const auto  func            = pp.llvmFunc;
    const auto& buildParameters = pp.buildParams;

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
