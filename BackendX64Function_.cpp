#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64FunctionBodyJob.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "TypeInfo.h"

BackendFunctionBodyJob* BackendX64::newFunctionJob()
{
    return g_Pool_backendX64FunctionBodyJob.alloc();
}

uint32_t BackendX64::getOrCreateLabel(X64PerThread& pp, uint32_t ip)
{
    auto it = pp.labels.find(ip);
    if (it == pp.labels.end())
    {
        pp.labels[ip] = pp.concat.totalCount;
        return pp.concat.totalCount;
    }

    return it->second;
}

bool BackendX64::emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc)
{
    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC) && (buildParameters.compileType != BackendCompileType::Test))
        return true;

    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;
    auto  typeFunc        = bc->callType();

    pp.labels.clear();
    pp.labelsToSolve.clear();
    bc->markLabels();

    // Symbol
    getOrAddSymbol(pp, bc->callName(), CoffSymbolKind::Function, concat.totalCount - pp.textSectionOffset);

    // Reserve registers
    uint32_t sizeStack   = 0;
    uint32_t offsetFLT   = 0;
    uint32_t offsetStack = 0;
    uint32_t offsetRT    = 0;

    // For float load 
    // (should be reserved only if we have floating point operations in that function)
    sizeStack += 8;

    if (typeFunc->stackSize)
    {
        sizeStack += typeFunc->stackSize;
        offsetFLT += typeFunc->stackSize;
    }

    if (bc->maxCallResults)
    {
        offsetStack += bc->maxCallResults * sizeof(Register);
        sizeStack += bc->maxCallResults * sizeof(Register);
        offsetFLT += bc->maxCallResults * sizeof(Register);
    }

    if (bc->maxReservedRegisterRC)
    {
        offsetRT += bc->maxReservedRegisterRC * sizeof(Register);
        offsetStack += bc->maxReservedRegisterRC * sizeof(Register);
        sizeStack += bc->maxReservedRegisterRC * sizeof(Register);
        offsetFLT += bc->maxReservedRegisterRC * sizeof(Register);
    }

    // RDI will be a pointer to the stack, and the list of registers is stored at the start
    // of the stack
    concat.addU8(0x57); // push rdi
    BackendX64Inst::emitSubRsp(pp, sizeStack);
    concat.addString3("\x48\x89\xE7"); // mov rdi, rsp

    // C calling convention, space for at least 4 parameters when calling a function
    // (should be reserved only if we have a call)
    BackendX64Inst::emitSubRsp(pp, 4 * sizeof(uint64_t));

    auto                   ip = bc->out;
    VectorNative<uint32_t> pushRAParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
        {
            SWAG_ASSERT(!(ip->flags & BCI_JUMP_DEST));
            continue;
        }

        if (ip->flags & BCI_JUMP_DEST)
            getOrCreateLabel(pp, i);

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::DecSP:
        case ByteCodeOp::IncSP:
        case ByteCodeOp::CopySPtoBP:
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
            continue;
        }

        switch (ip->op)
        {
        case ByteCodeOp::CastS8S16:
            BackendX64Inst::emitMoveReg2RAX(pp, ip->a.u32);
            concat.addString4("\x66\x0F\xBE\xC0"); // movsx ax, al
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::CastS16S32:
            BackendX64Inst::emitMoveReg2RAX(pp, ip->a.u32);
            concat.addString3("\x0F\xBF\xC0"); // movsx eax, ax
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::CastS32S64:
            BackendX64Inst::emitMoveReg2RAX(pp, ip->a.u32);
            concat.addString3("\x48\x63\xC0"); // movsx rax, eax
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::JumpIfZero32:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emitMoveReg2RAX(pp, ip->a.u32);
            concat.addString2("\x85\xC0"); // test eax, eax
            concat.addString2("\x0F\x84"); // jz ?
            pp.labelsToSolve.push_back({ip->b.s32 + i + 1, (int32_t) concat.totalCount, concat.getSeekPtr()});
            concat.addU32(0);
            break;
        case ByteCodeOp::Jump:
            //CONCAT_FIXED_STR(concat, "goto _");
            //concat.addS32Str8(ip->a.s32 + i + 1);
            concat.addU8(0xE9); // jmp ?
            pp.labelsToSolve.push_back({ip->a.s32 + i + 1, (int32_t) concat.totalCount, concat.getSeekPtr()});
            concat.addU32(0);
            break;

        case ByteCodeOp::DecrementRA32:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32--;");
            BackendX64Inst::emitMoveReg2RAX(pp, ip->a.u32);
            BackendX64Inst::emitSub2RAX(pp, 1);
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::GetFromBssSeg64:
            //concat.addStringFormat("r[%u].u64 = *(__u64_t*) (__bs + %u);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emitSymbol2RAX(pp, pp.bsIndex);
            BackendX64Inst::emitAdd2RAX(pp, ip->b.u32);
            BackendX64Inst::emitDeRef64Rax(pp);
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::GetFromMutableSeg64:
            //concat.addStringFormat("r[%u].u64 = *(__u64_t*) (__ms + %u);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emitSymbol2RAX(pp, pp.msIndex);
            BackendX64Inst::emitAdd2RAX(pp, ip->b.u32);
            BackendX64Inst::emitDeRef64Rax(pp);
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::GetFromStack64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u64 = *(__u64_t*) (stack + ", ip->b.u32, ");");
            BackendX64Inst::emitLeaStack2Rax(pp, offsetStack + ip->b.u32);
            BackendX64Inst::emitDeRef64Rax(pp);
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::SetAtPointer8:
            //concat.addStringFormat("*(__u8_t*)(r[%u].pointer + %u) = r[%u].u8;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emitMoveReg2RAX(pp, ip->a.u32);
            BackendX64Inst::emitAdd2RAX(pp, ip->c.u32);
            BackendX64Inst::emitMoveReg2RBX(pp, ip->b.u32);
            concat.addString2("\x88\x18"); // mov [rax], bl
            break;
        case ByteCodeOp::SetAtPointer16:
            //concat.addStringFormat("*(__u16_t*)(r[%u].pointer + %u) = r[%u].u16;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emitMoveReg2RAX(pp, ip->a.u32);
            BackendX64Inst::emitAdd2RAX(pp, ip->c.u32);
            BackendX64Inst::emitMoveReg2RBX(pp, ip->b.u32);
            concat.addString3("\x66\x89\x18"); // mov [rax], bx
            break;
        case ByteCodeOp::SetAtPointer32:
            //concat.addStringFormat("*(__u32_t*)(r[%u].pointer + %u) = r[%u].u32;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emitMoveReg2RAX(pp, ip->a.u32);
            BackendX64Inst::emitAdd2RAX(pp, ip->c.u32);
            BackendX64Inst::emitMoveReg2RBX(pp, ip->b.u32);
            concat.addString2("\x89\x18"); // mov [rax], ebx
            break;
        case ByteCodeOp::SetAtPointer64:
            //concat.addStringFormat("*(__u64_t*)(r[%u].pointer + %u) = r[%u].u64;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emitMoveReg2RAX(pp, ip->a.u32);
            BackendX64Inst::emitAdd2RAX(pp, ip->c.u32);
            BackendX64Inst::emitMoveReg2RBX(pp, ip->b.u32);
            concat.addString3("\x48\x89\x18"); // mov [rax], rbx
            break;

        case ByteCodeOp::MakeStackPointer:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = stack + ", ip->b.u32, ";");
            concat.addString3("\x48\x89\xF8"); // mov rax, rdi
            BackendX64Inst::emitAdd2RAX(pp, offsetStack + ip->b.u32);
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::MakeMutableSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__ms + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emitSymbol2RAX(pp, pp.msIndex);
            BackendX64Inst::emitAdd2RAX(pp, ip->b.u32);
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::MakeBssSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__bs + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emitSymbol2RAX(pp, pp.bsIndex);
            BackendX64Inst::emitAdd2RAX(pp, ip->b.u32);
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::MakeConstantSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__cs + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emitSymbol2RAX(pp, pp.csIndex);
            BackendX64Inst::emitAdd2RAX(pp, ip->b.u32);
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::CopyVBtoRA32:
            //concat.addStringFormat("r[%u].u32 = 0x%x;", ip->a.u32, ip->b.u32);
            // mov [rdi + ?] = ?
            concat.addString2("\xC7\x87");
            concat.addU32(ip->a.u32 * sizeof(Register));
            concat.addU32(ip->b.u32);
            break;
        case ByteCodeOp::CopyVBtoRA64:
            //concat.addStringFormat("r[%u].u64 = 0x%I64x;", ip->a.u32, ip->b.u64);
            concat.addString2("\x48\xB8"); // mov rax, ?
            concat.addU64(ip->b.u64);
            BackendX64Inst::emitMoveRAX2Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::IntrinsicPrintString:
            //swag_runtime_print_n(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32);
            concat.addString2("\x8B\x97"); // mov edx, [rdi + ?]
            concat.addU32(ip->b.u32 * sizeof(Register));
            BackendX64Inst::emitMoveReg2RCX(pp, ip->a.u32);
            emitCall(pp, "swag_runtime_print_n");
            break;
        case ByteCodeOp::IntrinsicPrintS64:
            //CONCAT_STR_1(concat, "swag_runtime_print_i64(r[", ip->a.u32, "].s64);");
            BackendX64Inst::emitMoveReg2RCX(pp, ip->a.u32);
            emitCall(pp, "swag_runtime_print_i64");
            break;
        case ByteCodeOp::IntrinsicPrintF64:
            //CONCAT_STR_1(concat, "swag_runtime_print_i64(r[", ip->a.u32, "].f64);");
            BackendX64Inst::emitMoveReg2RAX(pp, ip->a.u32);
            BackendX64Inst::emitMoveRAX2Stack(pp, offsetFLT);
            concat.addString4("\xF2\x0F\x10\x87"); // movsd xmm0, [rdi + ?]
            concat.addU32(offsetFLT);
            emitCall(pp, "swag_runtime_print_f64");
            break;

        case ByteCodeOp::Ret:
            if (sizeStack)
            {
                BackendX64Inst::emitAddRsp(pp, sizeStack + 4 * sizeof(uint64_t));
                concat.addU8(0x5F); // pop rdi
            }

            // ret
            concat.addU8(0xc3);
            break;
        }
    }

    // Solve labels
    for (auto& toSolve : pp.labelsToSolve)
    {
        auto it = pp.labels.find(toSolve.ipDest);
        SWAG_ASSERT(it != pp.labels.end());

        auto relOffset             = it->second - (toSolve.currentOffset + 4);
        *(uint32_t*) toSolve.patch = relOffset;
    }

    return true;
}
