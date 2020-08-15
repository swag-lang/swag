#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64FunctionBodyJob.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "TypeInfo.h"
#include "Module.h"
#include "TypeInfo.h"
#include "TypeManager.h"

BackendFunctionBodyJob* BackendX64::newFunctionJob()
{
    return g_Pool_backendX64FunctionBodyJob.alloc();
}

uint32_t BackendX64::getOrCreateLabel(X64PerThread& pp, uint32_t ip)
{
    auto it = pp.labels.find(ip);
    if (it == pp.labels.end())
    {
        auto count    = pp.concat.totalCount();
        pp.labels[ip] = count;
        return count;
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
    bool  ok              = true;

    pp.labels.clear();
    pp.labelsToSolve.clear();
    bc->markLabels();

    // Symbol
    getOrAddSymbol(pp, bc->callName(), CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);

    // Reserve registers
    uint32_t sizeStack   = 0;
    uint32_t offsetFLT   = 0;
    uint32_t offsetStack = 0;
    uint32_t offsetRT    = 0;

    // For float load
    // (should be reserved only if we have floating point operations in that function)
    // In order we have :
    // RC at 0 (bc->maxReservedRegisterRC)
    // RT (max call results)
    // Local function stack
    // FLT
    offsetRT    = bc->maxReservedRegisterRC * sizeof(Register);
    offsetStack = offsetRT + bc->maxCallResults * sizeof(Register);
    offsetFLT   = offsetStack + typeFunc->stackSize;
    sizeStack   = offsetFLT + 8;

    // RDI will be a pointer to the stack, and the list of registers is stored at the start
    // of the stack
    concat.addU8(0x57); // push rdi
    while ((sizeStack % 16) != 8)
        sizeStack++; // Align to 16 bytes (we have a push just before, that's already 8 bytes)
    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, sizeStack);
    concat.addString3("\x48\x89\xE7"); // mov rdi, rsp

    // C calling convention, space for at least 4 parameters when calling a function
    // (should be reserved only if we have a call)
    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 4 * sizeof(uint64_t));

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
        case ByteCodeOp::ClearRA:
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst64_In_RBX(pp, 0);
            BackendX64Inst::emit_Move_RBX_At_RAX(pp);
            break;
        case ByteCodeOp::CopyRBtoRA:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = r[", ip->b.u32, "];");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::CopyRBAddrToRA:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = (__u8_t*) &r[", ip->b.u32, "];");
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::CastBool32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u32 ? 1 : 0;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Test_EAX_With_EAX(pp);
            concat.addString3("\x0F\x95\xC0"); // setne al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::CastS8S16:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16 = (__s16_t) r[", ip->a.u32, "].s8;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::CastS16S32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = (__s32_t) r[", ip->a.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_SignedExtend_AX_To_EAX(pp);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::CastS32S64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (__f64_t) r[", ip->a.u32, "].f32;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_SignedExtend_EAX_To_RAX(pp);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::CastF32F64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (__f64_t) r[", ip->a.u32, "].f32;");
            BackendX64Inst::emit_Move_Reg_In_XMM0_F32(pp, ip->a.u32);
            concat.addString4("\xf3\x0f\x5a\xc0"); // cvtss2sd xmm0, xmm0
            BackendX64Inst::emit_Move_XMM0_At_Reg_F64(pp, ip->a.u32);
            break;

        case ByteCodeOp::BinOpShiftLeftU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_ECX(pp, ip->b.u32);
            concat.addString2("\xd3\xe0"); // sal eax, cl
            BackendX64Inst::emit_Move_EAX_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::BinOpShiftLeftU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_ECX(pp, ip->b.u32);
            concat.addString3("\x48\xd3\xe0"); // sal rax, cl
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->c.u32);
            break;

        case ByteCodeOp::BinOpShiftRightS32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_ECX(pp, ip->b.u32);
            concat.addString2("\xd3\xf8"); // sar eax, cl
            BackendX64Inst::emit_Move_EAX_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::BinOpShiftRightS64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_ECX(pp, ip->b.u32);
            concat.addString3("\x48\xd3\xf8"); // sar rax, cl
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::BinOpShiftRightU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_ECX(pp, ip->b.u32);
            concat.addString2("\xd3\xe8"); // shr eax, cl
            BackendX64Inst::emit_Move_EAX_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::BinOpShiftRightU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_ECX(pp, ip->b.u32);
            concat.addString3("\x48\xd3\xe8"); // shr rax, cl
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->c.u32);
            break;

        case ByteCodeOp::BinOpXorU32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 ^ r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x33, 32);
            break;
        case ByteCodeOp::BinOpXorU64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 ^ r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x33, 64);
            break;

        case ByteCodeOp::BinOpMulS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 * r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0xAF0F, 32);
            break;
        case ByteCodeOp::BinOpMulS64:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 * r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0xAF0F, 64);
            break;
        case ByteCodeOp::BinOpMulF32:
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 * r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpFloat_At_Reg(pp, ip, 0x59, 32);
            break;
        case ByteCodeOp::BinOpMulF64:
            //concat.addStringFormat("r[%u].f64 = r[%u].f64 * r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpFloat_At_Reg(pp, ip, 0x59, 64);
            break;

        case ByteCodeOp::BinOpModuloS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 % r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, true, 32, true);
            break;
        case ByteCodeOp::BinOpModuloS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 % r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, true, 64, true);
            break;
        case ByteCodeOp::BinOpModuloU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 % r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, false, 32, true);
            break;
        case ByteCodeOp::BinOpModuloU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 % r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, false, 64, true);
            break;

        case ByteCodeOp::BinOpDivS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 / r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, true, 32);
            break;
        case ByteCodeOp::BinOpDivS64:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 / r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, true, 64);
            break;
        case ByteCodeOp::BinOpDivU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 / r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, false, 32);
            break;
        case ByteCodeOp::BinOpDivU64:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 / r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, false, 64);
            break;
        case ByteCodeOp::BinOpDivF32:
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 / r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpFloat_At_Reg(pp, ip, 0x5E, 32);
            break;
        case ByteCodeOp::BinOpDivF64:
            //concat.addStringFormat("r[%u].f64 = r[%u].f64 / r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpFloat_At_Reg(pp, ip, 0x5E, 64);
            break;

        case ByteCodeOp::BinOpPlusS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 + r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x03, 32);
            break;
        case ByteCodeOp::BinOpPlusS64:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 + r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x03, 64);
            break;
        case ByteCodeOp::BinOpPlusF32:
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 + r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpFloat_At_Reg(pp, ip, 0x58, 32);
            break;
        case ByteCodeOp::BinOpPlusF64:
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 + r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpFloat_At_Reg(pp, ip, 0x58, 64);
            break;

        case ByteCodeOp::BinOpMinusS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 - r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x2b, 32);
            break;
        case ByteCodeOp::BinOpMinusS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 - r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x2b, 64);
            break;
        case ByteCodeOp::BinOpMinusF32:
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 + r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpFloat_At_Reg(pp, ip, 0x5C, 32);
            break;
        case ByteCodeOp::BinOpMinusF64:
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 + r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpFloat_At_Reg(pp, ip, 0x5C, 64);
            break;

        case ByteCodeOp::BinOpBitmaskAndS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 & r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x23, 32);
            break;
        case ByteCodeOp::BinOpBitmaskAndS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 & r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x23, 64);
            break;

        case ByteCodeOp::BinOpBitmaskOrS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 | r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x0b, 32);
            break;
        case ByteCodeOp::BinOpBitmaskOrS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 | r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x0b, 64);
            break;

        case ByteCodeOp::BinOpAnd:
            //concat.addStringFormat("r[%u].b = r[%u].b && r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x22, 8);
            break;
        case ByteCodeOp::BinOpOr:
            //concat.addStringFormat("r[%u].b = r[%u].b || r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_BinOpInt_At_Reg(pp, ip, 0x0a, 8);
            break;

        case ByteCodeOp::AffectOpXOrEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_AL(pp, ip->b.u32);
            concat.addString2("\x30\x03"); // xor [rbx], al
            break;
        case ByteCodeOp::AffectOpXOrEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_AX(pp, ip->b.u32);
            concat.addString3("\x66\x31\x03"); // xor [rbx], ax
            break;
        case ByteCodeOp::AffectOpXOrEqS32:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->b.u32);
            concat.addString2("\x31\x03"); // xor [rbx], eax
            break;
        case ByteCodeOp::AffectOpXOrEqS64:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            concat.addString3("\x48\x31\x03"); // xor [rbx], rax
            break;

        case ByteCodeOp::AffectOpOrEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_AL(pp, ip->b.u32);
            concat.addString2("\x08\x03"); // xor [rbx], al
            break;
        case ByteCodeOp::AffectOpOrEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_AX(pp, ip->b.u32);
            concat.addString3("\x66\x09\x03"); // or [rbx], ax
            break;
        case ByteCodeOp::AffectOpOrEqS32:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->b.u32);
            concat.addString2("\x09\x03"); // or [rbx], eax
            break;
        case ByteCodeOp::AffectOpOrEqS64:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            concat.addString3("\x48\x09\x03"); // or [rbx], rax
            break;

        case ByteCodeOp::AffectOpAndEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_AL(pp, ip->b.u32);
            concat.addString2("\x20\x03"); // and [rbx], al
            break;
        case ByteCodeOp::AffectOpAndEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_AX(pp, ip->b.u32);
            concat.addString3("\x66\x21\x03"); // and [rbx], ax
            break;
        case ByteCodeOp::AffectOpAndEqS32:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->b.u32);
            concat.addString2("\x21\x03"); // and [rbx], eax
            break;
        case ByteCodeOp::AffectOpAndEqS64:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            concat.addString3("\x48\x21\x03"); // and [rbx], rax
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef8_RAX(pp);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString2("\xf6\xeb"); // imul bl
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_AL_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef16_RAX(pp);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString3("\x66\xf7\xeb"); // imul bx
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_AX_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef32_RAX(pp);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString2("\xf7\xeb"); // imul ebx
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_EAX_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s64;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef64_RAX(pp);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString3("\x48\xf7\xeb"); // imul rbx
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_RAX_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpMulEqF32:
            //CONCAT_STR_2(concat, "*(__f32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].f32;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef_RAX_In_XMM0_F32(pp);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F32(pp, ip->b.u32);
            concat.addString4("\xf3\x0f\x59\xc1"); // mulss xmm0, xmm1
            BackendX64Inst::emit_Move_XMM0_At_RAX_F32(pp);
            break;
        case ByteCodeOp::AffectOpMulEqF64:
            //CONCAT_STR_2(concat, "*(__f64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].f64;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef_RAX_In_XMM0_F64(pp);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F64(pp, ip->b.u32);
            concat.addString4("\xf2\x0f\x59\xc1"); // mulss xmm0, xmm1
            BackendX64Inst::emit_Move_XMM0_At_RAX_F64(pp);
            break;

        case ByteCodeOp::AffectOpDivEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef8_RAX(pp);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString2("\xf6\xfb"); // idiv al, bl
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_AL_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpDivEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef16_RAX(pp);
            concat.addString2("\x66\x99"); // cwd
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString3("\x66\xf7\xfb"); // idiv ax, bx
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_AX_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpDivEqS32:
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef32_RAX(pp);
            concat.addString1("\x99"); // cdq
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString2("\xf7\xfb"); // idiv eax, ebx
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_EAX_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpDivEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s64;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef64_RAX(pp);
            concat.addString2("\x48\x99"); // cqo
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString3("\x48\xf7\xfb"); // idiv rax, rbx
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_RAX_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpDivEqU8:
            //CONCAT_STR_2(concat, "*(__u8_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u8;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef8_RAX(pp);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString2("\xf6\xf3"); // idiv al, bl
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_AL_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpDivEqU16:
            //CONCAT_STR_2(concat, "*(__u16_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u16;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef16_RAX(pp);
            BackendX64Inst::emit_Clear_DX(pp);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString3("\x66\xf7\xf3"); // idiv ax, bx
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_AX_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpDivEqU32:
            //CONCAT_STR_2(concat, "*(__u32_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef32_RAX(pp);
            BackendX64Inst::emit_Clear_EDX(pp);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString2("\xf7\xf3"); // idiv eax, ebx
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_EAX_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpDivEqU64:
            //CONCAT_STR_2(concat, "*(__u64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u64;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef64_RAX(pp);
            BackendX64Inst::emit_Clear_RDX(pp);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            concat.addString3("\x48\xf7\xf3"); // idiv rax, rbx
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_RAX_At_RBX(pp);
            break;
        case ByteCodeOp::AffectOpDivEqF32:
            //CONCAT_STR_2(concat, "*(__f32_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].f32;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef_RAX_In_XMM0_F32(pp);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F32(pp, ip->b.u32);
            concat.addString4("\xf3\x0f\x5e\xc1"); // divss xmm0, xmm1
            BackendX64Inst::emit_Move_XMM0_At_RAX_F32(pp);
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            //CONCAT_STR_2(concat, "*(__f64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].f64;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef_RAX_In_XMM0_F64(pp);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F64(pp, ip->b.u32);
            concat.addString4("\xf2\x0f\x5e\xc1"); // divsd xmm0, xmm1
            BackendX64Inst::emit_Move_XMM0_At_RAX_F64(pp);
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_AL(pp, ip->b.u32);
            concat.addString2("\x28\x03"); // sub [rbx], al
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_AX(pp, ip->b.u32);
            concat.addString3("\x66\x29\x03"); // sub [rbx], ax
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->b.u32);
            concat.addString2("\x29\x03"); // sub [rbx], eax
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s64;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            concat.addString3("\x48\x29\x03"); // sub [rbx], rax
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            //CONCAT_STR_2(concat, "*(__f32_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].f32;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef_RAX_In_XMM0_F32(pp);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F32(pp, ip->b.u32);
            concat.addString4("\xf3\x0f\x5c\xc1"); // subss xmm0, xmm1
            BackendX64Inst::emit_Move_XMM0_At_RAX_F32(pp);
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            //CONCAT_STR_2(concat, "*(__f64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].f64;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef_RAX_In_XMM0_F64(pp);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F64(pp, ip->b.u32);
            concat.addString4("\xf2\x0f\x5c\xc1"); // subss xmm0, xmm1
            BackendX64Inst::emit_Move_XMM0_At_RAX_F64(pp);
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_AL(pp, ip->b.u32);
            concat.addString2("\x00\x03"); // add [rbx], al
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_AX(pp, ip->b.u32);
            concat.addString3("\x66\x01\x03"); // add [rbx], ax
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->b.u32);
            concat.addString2("\x01\x03"); // add [rbx], eax
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s64;");
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            concat.addString3("\x48\x01\x03"); // add [rbx], rax
            break;
        case ByteCodeOp::AffectOpPlusEqPointer:
            //CONCAT_STR_2(concat, "*(__u8_t**)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef64_RBX(pp);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_SignedExtend_EAX_To_RAX(pp);
            concat.addString3("\x48\x01\xc3"); // add rbx, rax
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_RBX_At_RAX(pp);
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            //CONCAT_STR_2(concat, "*(__f32_t**)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].f32;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef_RAX_In_XMM0_F32(pp);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F32(pp, ip->b.u32);
            concat.addString4("\xf3\x0f\x58\xc1"); // addss xmm0, xmm1
            BackendX64Inst::emit_Move_XMM0_At_RAX_F32(pp);
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            //CONCAT_STR_2(concat, "*(__f64_t**)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].f64;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef_RAX_In_XMM0_F64(pp);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F64(pp, ip->b.u32);
            concat.addString4("\xf2\x0f\x58\xc1"); // addss xmm0, xmm1
            BackendX64Inst::emit_Move_XMM0_At_RAX_F64(pp);
            break;

        case ByteCodeOp::CompareOpGreaterU32:
            //concat.addStringFormat("r[%u].b = r[%u].u32 > r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_EBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_EAX_With_EBX(pp);
            concat.addString3("\x0f\x97\xc0"); // seta al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            //concat.addStringFormat("r[%u].b = r[%u].u64 > r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_RAX_With_RBX(pp);
            concat.addString3("\x0f\x97\xc0"); // seta al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpGreaterS32:
            //concat.addStringFormat("r[%u].b = r[%u].s32 > r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_EBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_EAX_With_EBX(pp);
            concat.addString3("\x0f\x9f\xc0"); // setg al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            //concat.addStringFormat("r[%u].b = r[%u].s64 > r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_RAX_With_RBX(pp);
            concat.addString3("\x0f\x9f\xc0"); // setg al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            //concat.addStringFormat("r[%u].b = r[%u].f32 > r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_XMM0_F32(pp, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F32(pp, ip->a.u32);
            BackendX64Inst::emit_Cmp_XMM0_With_XMM1_F32(pp);
            concat.addString3("\x0f\x97\xc0"); // seta al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            //concat.addStringFormat("r[%u].b = r[%u].f64 > r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_XMM0_F64(pp, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F64(pp, ip->a.u32);
            BackendX64Inst::emit_Cmp_XMM0_With_XMM1_F64(pp);
            concat.addString3("\x0f\x97\xc0"); // seta al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;

        case ByteCodeOp::CompareOpLowerU32:
            //concat.addStringFormat("r[%u].b = r[%u].u32 < r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_EBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_EAX_With_EBX(pp);
            concat.addString3("\x0f\x92\xc0"); // setb al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpLowerU64:
            //concat.addStringFormat("r[%u].b = r[%u].u64 < r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_RAX_With_RBX(pp);
            concat.addString3("\x0f\x92\xc0"); // setb al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpLowerS32:
            //concat.addStringFormat("r[%u].b = r[%u].s32 < r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_EBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_EAX_With_EBX(pp);
            concat.addString3("\x0F\x9C\xC0"); // setl al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpLowerS64:
            //concat.addStringFormat("r[%u].b = r[%u].s64 < r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_RAX_With_RBX(pp);
            concat.addString3("\x0F\x9C\xC0"); // setl al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpLowerF32:
            //concat.addStringFormat("r[%u].b = r[%u].f32 < r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_XMM0_F32(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F32(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_XMM0_With_XMM1_F32(pp);
            concat.addString3("\x0f\x97\xc0"); // seta al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpLowerF64:
            //concat.addStringFormat("r[%u].b = r[%u].f64 < r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_XMM0_F64(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_XMM1_F64(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_XMM0_With_XMM1_F64(pp);
            concat.addString3("\x0f\x97\xc0"); // seta al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;

        case ByteCodeOp::CompareOpEqual8:
            //concat.addStringFormat("r[%u].b = r[%u].u8 == r[%u].u8;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_AL_With_BL(pp);
            concat.addString3("\x0F\x94\xC0"); // sete al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpEqual16:
            //concat.addStringFormat("r[%u].b = r[%u].u16 == r[%u].u16;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_AX_With_BX(pp);
            concat.addString3("\x0F\x94\xC0"); // sete al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpEqual32:
            //concat.addStringFormat("r[%u].b = r[%u].u32 == r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_EAX_With_EBX(pp);
            concat.addString3("\x0F\x94\xC0"); // sete al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpEqual64:
            //concat.addStringFormat("r[%u].b = r[%u].u64 == r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Cmp_RAX_With_RBX(pp);
            concat.addString3("\x0F\x94\xC0"); // sete al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpEqualString:
            //concat.addStringFormat("r[%u].b = swag_runtime_comparestring(r[%u].pointer, r[%u].pointer, r[%u].u32, r[%u].u32);", ip->c.u32, ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32);
            BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RDX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_R8D(pp, ip->c.u32);
            BackendX64Inst::emit_Move_Reg_In_R9D(pp, ip->d.u32);
            emitCall(pp, "swag_runtime_comparestring");
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case ByteCodeOp::CompareOpEqualTypeInfo:
            //concat.addStringFormat("r[%u].b = swag_runtime_comparetype(r[%u].pointer, r[%u].pointer);", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RDX(pp, ip->b.u32);
            emitCall(pp, "swag_runtime_comparetype");
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;

        case ByteCodeOp::TestNotZero8:
            //concat.addStringFormat("r[%u].b=r[%u].u8!=0;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Test_AL_With_AL(pp);
            concat.addString3("\x0F\x95\xC0"); // setne al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::TestNotZero16:
            //concat.addStringFormat("r[%u].b=r[%u].u16!=0;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Test_AX_With_AX(pp);
            concat.addString3("\x0F\x95\xC0"); // setne al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::TestNotZero32:
            //concat.addStringFormat("r[%u].b=r[%u].u32!=0;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Test_EAX_With_EAX(pp);
            concat.addString3("\x0F\x95\xC0"); // setne al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::TestNotZero64:
            //concat.addStringFormat("r[%u].b=r[%u].u64!=0;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Test_RAX_With_RAX(pp);
            concat.addString3("\x0F\x95\xC0"); // setne al
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::NegBool:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].b ^= 1;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            concat.addString2("\x34\x01"); // xor al, 1
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::NegS32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = -r[", ip->a.u32, "].s32;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            concat.addString2("\xf7\xd8"); // neg eax
            BackendX64Inst::emit_Move_EAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::NegS64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = -r[", ip->a.u32, "].s64;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            concat.addString3("\x48\xf7\xd8"); // neg rax
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::NegF32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = -r[", ip->a.u32, "].f32;");
            BackendX64Inst::emit_Move_Reg_In_XMM0_F32(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst64_In_RAX(pp, 0x80000000);
            BackendX64Inst::emit_Move_RAX_At_Stack(pp, offsetFLT);
            BackendX64Inst::emit_Move_Stack_In_XMM1_F32(pp, offsetFLT);
            concat.addString3("\x0f\x57\xc1"); // xorps xmm0, xmm1
            BackendX64Inst::emit_Move_XMM0_At_Reg_F32(pp, ip->a.u32);
            break;
        case ByteCodeOp::NegF64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = -r[", ip->a.u32, "].f64;");
            BackendX64Inst::emit_Move_Reg_In_XMM0_F64(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst64_In_RAX(pp, 0x80000000'00000000);
            BackendX64Inst::emit_Move_RAX_At_Stack(pp, offsetFLT);
            BackendX64Inst::emit_Move_Stack_In_XMM1_F64(pp, offsetFLT);
            concat.addString4("\x66\x0f\x57\xc1"); // xorpd xmm0, xmm1
            BackendX64Inst::emit_Move_XMM0_At_Reg_F64(pp, ip->a.u32);
            break;

        case ByteCodeOp::JumpIfTrue:
            //CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Test_AL_With_AL(pp);
            BackendX64Inst::emitJump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfFalse:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Test_AL_With_AL(pp);
            BackendX64Inst::emitJump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero32:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Test_EAX_With_EAX(pp);
            BackendX64Inst::emitJump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero64:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u64) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Test_RAX_With_RAX(pp);
            BackendX64Inst::emitJump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::Jump:
            //CONCAT_FIXED_STR(concat, "goto _");
            //concat.addS32Str8(ip->a.s32 + i + 1);
            BackendX64Inst::emitJump(pp, BackendX64Inst::JUMP, i, ip->a.s32);
            break;

        case ByteCodeOp::IncrementRA32:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32++;");
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Add_Cst32_At_RAX(pp, 1);
            break;
        case ByteCodeOp::DecrementRA32:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32--;");
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Sub_Cst32_At_RAX(pp, 1);
            break;

        case ByteCodeOp::DeRef8:
            //concat.addStringFormat("r[%u].u8 = *(__u8_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef8_RAX(pp);
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::DeRef16:
            //concat.addStringFormat("r[%u].u16 = *(__u16_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef16_RAX(pp);
            BackendX64Inst::emit_Move_AX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::DeRef32:
            //concat.addStringFormat("r[%u].u32 = *(__u32_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef32_RAX(pp);
            BackendX64Inst::emit_Move_EAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::DeRef64:
            //concat.addStringFormat("r[%u].u64 = *(__u64_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_DeRef64_RAX(pp);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::DeRefPointer:
            //concat.addStringFormat("r[%u].pointer = *(__u8_t**) (r[%u].pointer + %u);", ip->b.u32, ip->a.u32, ip->c.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst64_In_RBX(pp, ip->c.u32);
            concat.addString3("\x48\x01\xD8"); // add rax, rbx
            BackendX64Inst::emit_DeRef64_RAX(pp);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->b.u32);
            break;
        case ByteCodeOp::DeRefStringSlice:
            //concat.addStringFormat("r[%u].u64 = *(__u64_t*) (r[%u].pointer + 8); ", ip->b.u32, ip->a.u32);
            //concat.addStringFormat("r[%u].pointer = *(__u8_t**) r[%u].pointer; ", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            concat.addString4("\x48\x8b\x50\x08"); // mov rdx, [rax + 8]
            BackendX64Inst::emit_Move_RDX_At_Reg(pp, ip->b.u32);
            BackendX64Inst::emit_DeRef64_RAX(pp);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::GetFromBssSeg64:
            //concat.addStringFormat("r[%u].u64 = *(__u64_t*) (__bs + %u);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Symbol_In_RAX(pp, pp.symBSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_DeRef64_RAX(pp);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::GetFromMutableSeg64:
            //concat.addStringFormat("r[%u].u64 = *(__u64_t*) (__ms + %u);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Symbol_In_RAX(pp, pp.symMSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_DeRef64_RAX(pp);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::GetFromStack64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u64 = *(__u64_t*) (stack + ", ip->b.u32, ");");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->b.u32);
            BackendX64Inst::emit_DeRef64_RAX(pp);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::ClearMaskU64:
            //concat.addStringFormat("r[%u].u64 &= 0x%llx;", ip->a.u32, ip->b.u64);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst64_In_RBX(pp, ip->b.u64);
            concat.addString3("\x48\x21\xd8"); // and rax, rbx
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::SetZeroAtPointer8:
            //CONCAT_STR_2(concat, "*(__u8_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst8_At_RAX(pp, ip->b.u32, 0);
            break;
        case ByteCodeOp::SetZeroAtPointer16:
            //CONCAT_STR_2(concat, "*(__u16_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst16_At_RAX(pp, ip->b.u32, 0);
            break;
        case ByteCodeOp::SetZeroAtPointer32:
            //CONCAT_STR_2(concat, "*(__u32_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst32_At_RAX(pp, ip->b.u32, 0);
            break;
        case ByteCodeOp::SetZeroAtPointer64:
            //CONCAT_STR_2(concat, "*(__u64_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst64_At_RAX(pp, ip->b.u32, 0);
            break;
        case ByteCodeOp::SetZeroAtPointerX:
            //concat.addStringFormat("memset(r[%u].pointer, 0, %u);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->a.u32);
            BackendX64Inst::emit_Clear_RDX(pp);
            BackendX64Inst::emit_Move_Cst64_In_R8(pp, ip->b.u32);
            emitCall(pp, "memset");
            break;
        case ByteCodeOp::SetZeroAtPointerXRB:
            //concat.addStringFormat("memset(r[%u].pointer, 0, r[%u].u32 * %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->a.u32);
            BackendX64Inst::emit_Clear_RDX(pp);
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->b.u32);
            concat.addString2("\x69\xc0"); // imul eax, ????????
            concat.addU32(ip->c.u32);
            concat.addString3("\x49\x89\xc0"); // mov r8, rax
            emitCall(pp, "memset");
            break;

        case ByteCodeOp::SetZeroStack8:
            //CONCAT_STR_1(concat, "*(__u8_t*)(s+", ip->a.u32, ")=0;");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->a.u32);
            BackendX64Inst::emit_Move_Cst8_At_RAX(pp, 0, 0);
            break;
        case ByteCodeOp::SetZeroStack16:
            //CONCAT_STR_1(concat, "*(__u16_t*)(s+", ip->a.u32, ")=0;");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->a.u32);
            BackendX64Inst::emit_Move_Cst16_At_RAX(pp, 0, 0);
            break;
        case ByteCodeOp::SetZeroStack32:
            //CONCAT_STR_1(concat, "*(__u32_t*)(s+", ip->a.u32, ")=0;");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->a.u32);
            BackendX64Inst::emit_Move_Cst32_At_RAX(pp, 0, 0);
            break;
        case ByteCodeOp::SetZeroStack64:
            //CONCAT_STR_1(concat, "*(__u64_t*)(s+", ip->a.u32, ")=0;");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->a.u32);
            BackendX64Inst::emit_Move_Cst64_At_RAX(pp, 0, 0);
            break;
        case ByteCodeOp::SetZeroStackX:
            //concat.addStringFormat("memset(stack + %u, 0, %u);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Lea_Stack_In_RCX(pp, offsetStack + ip->a.u32);
            BackendX64Inst::emit_Clear_RDX(pp);
            BackendX64Inst::emit_Move_Cst64_In_R8(pp, ip->b.u32);
            emitCall(pp, "memset");
            break;

        case ByteCodeOp::SetAtPointer8:
            //concat.addStringFormat("*(__u8_t*)(r[%u].pointer + %u) = r[%u].u8;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->c.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_BL_At_RAX(pp);
            break;
        case ByteCodeOp::SetAtPointer16:
            //concat.addStringFormat("*(__u16_t*)(r[%u].pointer + %u) = r[%u].u16;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->c.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_BX_At_RAX(pp);
            break;
        case ByteCodeOp::SetAtPointer32:
            //concat.addStringFormat("*(__u32_t*)(r[%u].pointer + %u) = r[%u].u32;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->c.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_EBX_At_RAX(pp);
            break;
        case ByteCodeOp::SetAtPointer64:
            //concat.addStringFormat("*(__u64_t*)(r[%u].pointer + %u) = r[%u].u64;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->c.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_RBX_At_RAX(pp);
            break;

        case ByteCodeOp::MakeStackPointer:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = stack + ", ip->b.u32, ";");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->b.u32);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::MakeTypeSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__ts + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Symbol_In_RAX(pp, pp.symTSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::MakeMutableSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__ms + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Symbol_In_RAX(pp, pp.symMSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::MakeBssSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__bs + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Symbol_In_RAX(pp, pp.symBSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::MakeConstantSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__cs + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Symbol_In_RAX(pp, pp.symCSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::IncPointerVB32:
            //concat.addStringFormat("r[%u].pointer += %u;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->a.u32);
            concat.addString3("\x48\x81\x00"); // add [rax], ????????
            concat.addU32(ip->b.s32);
            break;

        case ByteCodeOp::IncPointer32:
            //concat.addStringFormat("r[%u].pointer = r[%u].pointer + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RBX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_SignedExtend_EAX_To_RAX(pp);
            concat.addString3("\x48\x01\xD8"); // add rax, rbx
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->c.u32);
            break;

        case ByteCodeOp::Mul64byVB32:
            //concat.addStringFormat("r[%u].s64 *= %u;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            concat.addString3("\x48\x69\xC0"); // imul rax, rax, ????????
            concat.addU32(ip->b.u32);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::CopyVBtoRA32:
            //concat.addStringFormat("r[%u].u32 = 0x%x;", ip->a.u32, ip->b.u32);
            concat.addString2("\xC7\x87"); // mov [rdi + ?] = ????????
            concat.addU32(ip->a.u32 * sizeof(Register));
            concat.addU32(ip->b.u32);
            break;
        case ByteCodeOp::CopyVBtoRA64:
            //concat.addStringFormat("r[%u].u64 = 0x%I64x;", ip->a.u32, ip->b.u64);
            BackendX64Inst::emit_Move_Cst64_In_RAX(pp, ip->b.u64);
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::MemCpyVC32:
            //concat.addStringFormat("memcpy(r[%u].pointer, r[%u].pointer, %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RDX(pp, ip->b.u32);
            concat.addString3("\x49\xC7\xC0"); // mov r8, ????????
            concat.addU32(ip->c.u32);
            emitCall(pp, "memcpy");
            break;

        case ByteCodeOp::IntrinsicAssert:
            //concat.addStringFormat("swag_runtime_assert(r[%u].b, \"%s\", %d, \"%s\");", ip->a.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1, ip->d.pointer);
            BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->a.u32);
            concat.addString2("\x48\xBA"); // mov rdx, ????????
            emitGlobalString(pp, precompileIndex, normalizePath(ip->node->sourceFile->path));
            concat.addString3("\x49\xC7\xC0"); // mov r8, ????????
            concat.addU32(ip->node->token.startLocation.line + 1);
            if (ip->d.pointer)
            {
                concat.addString2("\x49\xB9"); // mov r9, ????????
                emitGlobalString(pp, precompileIndex, (const char*) ip->d.pointer);
            }
            else
                concat.addString3("\x4D\x31\xC9"); // xor r9, r9
            emitCall(pp, "swag_runtime_assert");
            break;

        case ByteCodeOp::IntrinsicAlloc:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) malloc(r[%u].u32);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_ECX(pp, ip->b.u32);
            emitCall(pp, "malloc");
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicRealloc:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) realloc(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->b.u32);
            BackendX64Inst::emit_Move_Reg_In_EDX(pp, ip->c.u32);
            emitCall(pp, "realloc");
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicFree:
            //concat.addStringFormat("free(r[%u].pointer);", ip->a.u32);
            BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->a.u32);
            emitCall(pp, "free");
            break;

        case ByteCodeOp::IntrinsicIsByteCode:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].b = 0;");
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst32_At_RAX(pp, 0, 0);
            break;
        case ByteCodeOp::IntrinsicPrintString:
            //swag_runtime_print_n(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32);
            concat.addString2("\x8B\x97"); // mov edx, [rdi + ?]
            concat.addU32(ip->b.u32 * sizeof(Register));
            BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->a.u32);
            emitCall(pp, "swag_runtime_print_n");
            break;
        case ByteCodeOp::IntrinsicPrintS64:
            //CONCAT_STR_1(concat, "swag_runtime_print_i64(r[", ip->a.u32, "].s64);");
            BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->a.u32);
            emitCall(pp, "swag_runtime_print_i64");
            break;
        case ByteCodeOp::IntrinsicPrintF64:
            //CONCAT_STR_1(concat, "swag_runtime_print_i64(r[", ip->a.u32, "].f64);");
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_RAX_At_Stack(pp, offsetFLT);
            concat.addString4("\xF2\x0F\x10\x87"); // movsd xmm0, [rdi + ?]
            concat.addU32(offsetFLT);
            emitCall(pp, "swag_runtime_print_f64");
            break;

        case ByteCodeOp::CopyRCtoRR:
            //registersRR[ip->a.u32] = registersRC[ip->b.u32];
            SWAG_ASSERT(ip->a.u32 <= 1); // Can only return 2 registers
            if (ip->a.u32 == 0)
                BackendX64Inst::emit_Move_Reg_In_RCX(pp, ip->b.u32);
            else
                BackendX64Inst::emit_Move_Reg_In_RDX(pp, ip->b.u32);
            break;
        case ByteCodeOp::CopyRRtoRCCall:
            //registersRC[ip->a.u32] = registersRR[ip->b.u32];
            SWAG_ASSERT(ip->b.u32 <= 1); // Can only return 2 registers
            if (ip->b.u32 == 0)
                BackendX64Inst::emit_Move_RCX_At_Reg(pp, ip->a.u32);
            else
                BackendX64Inst::emit_Move_RDX_At_Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::PushRAParam:
            pushRAParams.push_back(ip->a.u32);
            break;
        case ByteCodeOp::GetFromStackParam64:
            concat.addString3("\x48\x8b\x87"); // mov rax, [rdi + ????????]
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            concat.addU32(16 + sizeStack + ip->c.u32 * sizeof(Register));
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;

        case ByteCodeOp::MakeLambda:
        {
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) &%s;", ip->a.u32, funcBC->callName().c_str());
            auto funcBC = (ByteCode*) ip->b.pointer;
            SWAG_ASSERT(funcBC);
            BackendX64Inst::emit_Move_Cst64_In_RAX(pp, 0);

            CoffRelocation reloc;
            reloc.virtualAddress = concat.totalCount() - sizeof(uint64_t) - pp.textSectionOffset;
            auto callSym         = getOrAddSymbol(pp, funcBC->callName(), CoffSymbolKind::Extern);
            reloc.symbolIndex    = callSym->index;
            reloc.type           = IMAGE_REL_AMD64_ADDR64;
            pp.relocTableTextSection.table.push_back(reloc);

            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->a.u32);
            break;
        }

        case ByteCodeOp::LambdaCall:
        {
            //concat.addStringFormat("if(r[%u].u64 & 0x%llx) { ", ip->a.u32, SWAG_LAMBDA_MARKER);

            // Test if it's a native lambda or a bytecode one
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Move_Cst64_In_RBX(pp, SWAG_LAMBDA_MARKER);
            concat.addString3("\x48\x21\xc3"); // and rbx, rax
            BackendX64Inst::emit_Test_RBX_With_RBX(pp);
            concat.addString2("\x0f\x85"); // jnz ???????? => jump to bytecode lambda
            auto jumpToBC = (uint32_t*) concat.getSeekPtr();
            concat.addU32(0);           

            // Native lambda
            for (int iParam = 0; iParam < pushRAParams.size(); iParam++)
            {
                concat.addString2("\xff\xb7"); // push [rdi + ????????]
                concat.addU32(pushRAParams[iParam] * sizeof(Register));
            }

            concat.addString2("\xff\xd0"); // call rax
            BackendX64Inst::emit_Add_Cst32_To_RSP(pp, (int) pushRAParams.size() * sizeof(Register));
            //BackendX64Inst::emitJump(pp, BackendX64Inst::JUMP, i, 0);

            pushRAParams.clear();
            break;
        }

        case ByteCodeOp::LocalCall:
        {
            auto funcBC = (ByteCode*) ip->a.pointer;

            for (int iParam = 0; iParam < pushRAParams.size(); iParam++)
            {
                concat.addString2("\xff\xb7"); // push [rdi + ????????]
                concat.addU32(pushRAParams[iParam] * sizeof(Register));
            }

            emitCall(pp, funcBC->callName());
            BackendX64Inst::emit_Add_Cst32_To_RSP(pp, (int) pushRAParams.size() * sizeof(Register));
            pushRAParams.clear();
            break;
        }

        case ByteCodeOp::Ret:
            if (sizeStack)
            {
                BackendX64Inst::emit_Add_Cst32_To_RSP(pp, sizeStack + 4 * sizeof(uint64_t));
                concat.addU8(0x5F); // pop rdi
            }

            // ret
            concat.addU8(0xc3);
            break;

        default:
            ok = false;
            moduleToGen->internalError(format("unknown instruction '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
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

    return ok;
}

void BackendX64::emitGetParameter(X64PerThread& pp, uint32_t r, uint32_t idx)
{
    switch (idx)
    {
    case 0:
        BackendX64Inst::emit_Move_RCX_At_Reg(pp, r);
        break;
    case 1:
        BackendX64Inst::emit_Move_RDX_At_Reg(pp, r);
        break;
    }
}

void BackendX64::emitOneCallParameter(X64PerThread& pp, int indexCallParam, int numCallParams, uint32_t offsetStackR, uint32_t r)
{
    switch (indexCallParam)
    {
    case 0:
        BackendX64Inst::emit_Move_Reg_In_RCX(pp, r);
        break;
    case 1:
        BackendX64Inst::emit_Move_Reg_In_RDX(pp, r);
        break;
    }
}

void BackendX64::emitCallParameters(X64PerThread& pp, uint32_t offsetRT, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams)
{
    int      numCallParams  = (int) typeFuncBC->numParamsRegisters() + typeFuncBC->numReturnRegisters();
    uint32_t indexCallParam = numCallParams;

    // Emit parameter from right to left
    // Note that pushRAParams is already inverted
    for (int idxCall = 0; idxCall < pushRAParams.size(); idxCall++)
    {
        auto regIndex = pushRAParams[idxCall];
        emitOneCallParameter(pp, --indexCallParam, numCallParams, 0, regIndex);
    }

    for (int j = typeFuncBC->numReturnRegisters() - 1; j >= 0; j--)
        emitOneCallParameter(pp, --indexCallParam, numCallParams, offsetRT, j);
    SWAG_ASSERT(indexCallParam == 0);
}