#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64FunctionBodyJob.h"
#include "BackendX64_Macros.h"
#include "ByteCodeGenJob.h"
#include "ErrorIds.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"

#define UWOP_PUSH_NONVOL 0
#define UWOP_ALLOC_LARGE 1
#define UWOP_ALLOC_SMALL 2

BackendFunctionBodyJobBase* BackendX64::newFunctionJob()
{
    return g_Allocator.alloc<BackendX64FunctionBodyJob>();
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

uint16_t BackendX64::computeUnwindPushRDI(uint32_t offsetSubRSP)
{
    uint16_t unwind0 = 0;

    unwind0 = (RDI << 12);
    unwind0 |= (UWOP_PUSH_NONVOL << 8);
    unwind0 |= (uint8_t) offsetSubRSP;
    return unwind0;
}

void BackendX64::computeUnwindStack(uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind)
{
    // UNWIND_CODE
    // UBYTE:8: offset of the instruction after the "sub rsp"
    // UBYTE:4: code (UWOP_ALLOC_LARGE or UWOP_ALLOC_SMALL)
    // UBYTE:4: info (will code the size of the decrement of rsp)

    SWAG_ASSERT(offsetSubRSP <= 0xFF);
    SWAG_ASSERT((sizeStack & 7) == 0); // Must be aligned

    if (sizeStack <= 128)
    {
        SWAG_ASSERT(sizeStack >= 8);
        sizeStack -= 8;
        sizeStack /= 8;
        auto unwind0 = (uint16_t) (UWOP_ALLOC_SMALL | (sizeStack << 4));
        unwind0 <<= 8;
        unwind0 |= (uint16_t) offsetSubRSP;
        unwind.push_back(unwind0);
    }
    else
    {
        SWAG_ASSERT(sizeStack <= (512 * 1024) - 8);
        auto unwind0 = (uint16_t) (UWOP_ALLOC_LARGE);
        unwind0 <<= 8;
        unwind0 |= (uint16_t) offsetSubRSP;
        unwind.push_back(unwind0);
        unwind0 = (uint16_t) (sizeStack / 8);
        unwind.push_back(unwind0);
    }
}

void BackendX64::emitShiftArithmetic(X64PerThread& pp, Concat& concat, ByteCodeInstruction* ip, uint8_t numBits)
{
    if (ip->flags & BCI_IMM_B && ip->b.u32 >= numBits && !(ip->flags & BCI_SHIFT_SMALL))
    {
        switch (numBits)
        {
        case 8:
            concat.addString2("\xC0\xF8"); // sar al
            break;
        case 16:
            concat.addString3("\x66\xC1\xF8"); // sar ax
            break;
        case 32:
            concat.addString2("\xC1\xF8"); // sar eax
            break;
        case 64:
            concat.addString3("\x48\xC1\xF8"); // sar rax
            break;
        }

        concat.addU8(numBits - 1);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            BackendX64Inst::emit_LoadN_Immediate(pp, ip->a, RAX, numBits);
        else
            BackendX64Inst::emit_LoadN_Indirect(pp, regOffset(ip->a.u32), RAX, RDI, numBits);
        if (ip->flags & BCI_IMM_B)
            BackendX64Inst::emit_Load8_Immediate(pp, ip->b.u8 & (numBits - 1), RCX);
        else
        {
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            if (ip->flags & BCI_SHIFT_SMALL)
            {
                concat.addString2("\x80\xE1"); // and cl, ??
                concat.addU8(numBits - 1);
            }
            else
            {
                concat.addString2("\x83\xF9"); // cmp ecx, ??
                concat.addU8(numBits);
                BackendX64Inst::emit_NearJumpOp(pp, BackendX64Inst::JL);
                pp.concat.addU8(0); // mov below
                auto seekPtr = pp.concat.getSeekPtr() - 1;
                auto seekJmp = pp.concat.totalCount();
                BackendX64Inst::emit_Load8_Immediate(pp, numBits - 1, RCX);
                *seekPtr = (uint8_t) (concat.totalCount() - seekJmp);
            }
        }

        switch (numBits)
        {
        case 8:
            concat.addString2("\xD2\xF8"); // sar al, cl
            break;
        case 16:
            concat.addString3("\x66\xD3\xF8"); // sar ax, cl
            break;
        case 32:
            concat.addString2("\xD3\xF8"); // sar eax, cl
            break;
        case 64:
            concat.addString3("\x48\xD3\xF8"); // sar rax, cl
            break;
        }
    }

    BackendX64Inst::emit_StoreN_Indirect(pp, regOffset(ip->c.u32), RAX, RDI, numBits);
}

void BackendX64::emitShiftLogical(X64PerThread& pp, Concat& concat, ByteCodeInstruction* ip, uint8_t numBits, uint8_t op)
{
    if (ip->flags & BCI_IMM_B && ip->b.u32 >= numBits && !(ip->flags & BCI_SHIFT_SMALL))
        BackendX64Inst::emit_ClearN(pp, RAX, numBits);
    else
    {
        if (ip->flags & BCI_IMM_A)
            BackendX64Inst::emit_LoadN_Immediate(pp, ip->a, RAX, numBits);
        else
            BackendX64Inst::emit_LoadN_Indirect(pp, regOffset(ip->a.u32), RAX, RDI, numBits);

        if (ip->flags & BCI_IMM_B)
            BackendX64Inst::emit_Load8_Immediate(pp, ip->b.u8 & (numBits - 1), RCX);
        else
        {
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            if (ip->flags & BCI_SHIFT_SMALL)
            {
                concat.addString2("\x80\xE1"); // and cl, ??
                concat.addU8(numBits - 1);
            }
            else
            {
                concat.addString2("\x83\xF9"); // cmp ecx, ?
                concat.addU8(numBits);
                BackendX64Inst::emit_NearJumpOp(pp, BackendX64Inst::JL);
                pp.concat.addU8(0); // clear below
                auto seekPtr = pp.concat.getSeekPtr() - 1;
                auto seekJmp = pp.concat.totalCount();
                BackendX64Inst::emit_ClearN(pp, RAX, numBits);
                *seekPtr = (uint8_t) (concat.totalCount() - seekJmp);
            }
        }

        switch (numBits)
        {
        case 8:
            concat.addString1("\xD2");
            break;
        case 16:
            concat.addString2("\x66\xD3");
            break;
        case 32:
            concat.addString1("\xD3");
            break;
        case 64:
            concat.addString2("\x48\xD3");
            break;
        }

        concat.addU8(op);
    }

    BackendX64Inst::emit_StoreN_Indirect(pp, regOffset(ip->c.u32), RAX, RDI, numBits);
}

void BackendX64::emitShiftEqArithmetic(X64PerThread& pp, Concat& concat, ByteCodeInstruction* ip, uint8_t numBits)
{
    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
    if (ip->flags & BCI_IMM_B)
    {
        switch (numBits)
        {
        case 8:
            concat.addString2("\xC0\x38");
            break;
        case 16:
            concat.addString3("\x66\xC1\x38");
            break;
        case 32:
            concat.addString2("\xC1\x38");
            break;
        case 64:
            concat.addString3("\x48\xC1\x38");
            break;
        }

        if (ip->flags & BCI_SHIFT_SMALL)
            concat.addU8(ip->b.u8 & (numBits - 1));
        else
            concat.addU8(min(ip->b.u8, numBits - 1));
    }
    else
    {
        BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
        if (ip->flags & BCI_SHIFT_SMALL)
        {
            concat.addString2("\x80\xE1"); // and cl, ??
            concat.addU8(numBits - 1);
        }
        else
        {
            concat.addString2("\x83\xF9"); // cmp ecx, ??
            concat.addU8(numBits);
            BackendX64Inst::emit_NearJumpOp(pp, BackendX64Inst::JL);
            pp.concat.addU8(0); // move below
            auto seekPtr = pp.concat.getSeekPtr() - 1;
            auto seekJmp = pp.concat.totalCount();
            BackendX64Inst::emit_Load8_Immediate(pp, numBits - 1, RCX);
            *seekPtr = (uint8_t) (concat.totalCount() - seekJmp);
        }

        switch (numBits)
        {
        case 8:
            concat.addString2("\xd2\x38");
            break;
        case 16:
            concat.addString3("\x66\xd3\x38");
            break;
        case 32:
            concat.addString2("\xd3\x38");
            break;
        case 64:
            concat.addString3("\x48\xd3\x38");
            break;
        }
    }
}

void BackendX64::emitShiftEqLogical(X64PerThread& pp, Concat& concat, ByteCodeInstruction* ip, uint8_t numBits, uint8_t op)
{
    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
    if (ip->flags & BCI_IMM_B && ip->b.u32 >= numBits && !(ip->flags & BCI_SHIFT_SMALL))
    {
        BackendX64Inst::emit_ClearN(pp, RCX, numBits);
        BackendX64Inst::emit_StoreN_Indirect(pp, 0, RCX, RAX, numBits);
    }
    else if (ip->flags & BCI_IMM_B)
    {
        switch (numBits)
        {
        case 8:
            concat.addString1("\xc0");
            break;
        case 16:
            concat.addString2("\x66\xc1");
            break;
        case 32:
            concat.addString1("\xc1");
            break;
        case 64:
            concat.addString2("\x48\xc1");
            break;
        }

        concat.addU8(op);
        if (ip->flags & BCI_SHIFT_SMALL)
            concat.addU8(ip->b.u8 & (numBits - 1));
        else
            concat.addU8(ip->b.u8);
    }
    else
    {
        BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
        if (ip->flags & BCI_SHIFT_SMALL)
        {
            concat.addString2("\x80\xE1"); // and cl, ??
            concat.addU8(numBits - 1);
        }
        else
        {
            concat.addString2("\x83\xF9"); // cmp ecx, ??
            concat.addU8(numBits);
            BackendX64Inst::emit_NearJumpOp(pp, BackendX64Inst::JL);
            pp.concat.addU8(0); // clear + store below
            auto seekPtr = pp.concat.getSeekPtr() - 1;
            auto seekJmp = pp.concat.totalCount();
            BackendX64Inst::emit_ClearN(pp, RCX, numBits);
            BackendX64Inst::emit_StoreN_Indirect(pp, 0, RCX, RAX, numBits);
            *seekPtr = (uint8_t) (concat.totalCount() - seekJmp);
        }

        switch (numBits)
        {
        case 8:
            concat.addString1("\xd2");
            break;
        case 16:
            concat.addString2("\x66\xd3");
            break;
        case 32:
            concat.addString1("\xd3");
            break;
        case 64:
            concat.addString2("\x48\xd3");
            break;
        }

        concat.addU8(op);
    }
}

void BackendX64::emitOverflowSigned(const BuildParameters& buildParameters, Concat& concat, AstNode* node, const char* msg)
{
    if (!module->mustEmitSafetyOF(node))
        return;
    concat.addString2("\x0f\x81"); // jno
    concat.addU32(0);
    auto addr      = (uint32_t*) concat.getSeekPtr() - 1;
    auto prevCount = concat.totalCount();
    emitInternalPanic(buildParameters, node, msg);
    *addr = concat.totalCount() - prevCount;
}

void BackendX64::emitOverflowUnsigned(const BuildParameters& buildParameters, Concat& concat, AstNode* node, const char* msg)
{
    if (!module->mustEmitSafetyOF(node))
        return;
    concat.addString2("\x0f\x83"); // jnc
    concat.addU32(0);
    auto addr      = (uint32_t*) concat.getSeekPtr() - 1;
    auto prevCount = concat.totalCount();
    emitInternalPanic(buildParameters, node, msg);
    *addr = concat.totalCount() - prevCount;
}

void BackendX64::emitInternalPanic(const BuildParameters& buildParameters, AstNode* node, const char* msg)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];

    emitGlobalString(pp, precompileIndex, Utf8::normalizePath(node->sourceFile->path), RCX);
    BackendX64Inst::emit_Load64_Immediate(pp, node->token.startLocation.line, RDX);
    BackendX64Inst::emit_Load64_Immediate(pp, node->token.startLocation.column, R8);
    if (msg)
        emitGlobalString(pp, precompileIndex, msg, R9);
    else
        BackendX64Inst::emit_Clear64(pp, R9);
    emitCall(pp, g_LangSpec->name__panic);
}

bool BackendX64::emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc)
{
    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC) && (buildParameters.compileType != BackendCompileType::Test))
        return true;

    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;
    auto  typeFunc        = bc->getCallType();
    bool  ok              = true;
    bool  debug           = buildParameters.buildCfg->backendDebugInformations;

    concat.align(16);
    uint32_t startAddress = concat.totalCount();

    pp.labels.clear();
    pp.labelsToSolve.clear();
    bc->markLabels();

    // Symbol
    auto symbolFuncIndex  = getOrAddSymbol(pp, bc->getCallName(), CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct          = registerFunction(pp, bc->node, symbolFuncIndex);
    coffFct->startAddress = startAddress;
    if (debug)
        dbgSetLocation(coffFct, bc, nullptr, 0);

    // In order, starting at RSP, we have :
    //
    // sizeParamsStack to store function call parameters
    // ...padding to 16...

    // All registers:       bc->maxReservedRegisterRC
    // offsetRT:            bc->maxCallResults
    // offsetS4:            4 registers, to store RCX, RDX, R8, R9
    // offsetStack:         stackSize, function local stack
    // offsetFLT:           sizeof(Register)
    // ...padding to 16... => total is sizeStack

    uint32_t offsetRT    = bc->maxReservedRegisterRC * sizeof(Register);
    uint32_t offsetS4    = offsetRT + bc->maxCallResults * sizeof(Register);
    uint32_t offsetStack = offsetS4 + 4 * sizeof(Register);

    // For float load (should be reserved only if we have floating point operations in that function)
    uint32_t offsetFLT = offsetStack + typeFunc->stackSize;

    uint32_t sizeStack = offsetFLT + 8;
    MK_ALIGN16(sizeStack);

    // x64 calling convention, space for at least 4 parameters when calling a function
    // (should ideally be reserved only if we have a call)
    uint32_t sizeParamsStack = max(4 * sizeof(Register), (bc->maxCallParams + 1) * sizeof(Register));

    // Because of variadic parameters in fct calls, we need to add some extra room, in case we have to flatten them
    // We want to be sure to have the room to flatten the array of variadics (make all params contiguous). That's
    // why we multiply by 2.
    sizeParamsStack *= 2;

    MK_ALIGN16(sizeParamsStack);

    auto     beforeProlog = concat.totalCount();
    uint32_t sizeProlog   = 0;

    // RDI will be a pointer to the stack, and the list of registers is stored at the start
    // of the stack
    VectorNative<uint16_t> unwind;
    BackendX64Inst::emit_Push(pp, RDI);
    sizeProlog       = concat.totalCount() - beforeProlog;
    uint16_t unwind0 = computeUnwindPushRDI(sizeProlog);

    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, sizeStack + sizeParamsStack);

    coffFct->offsetStack  = offsetStack;
    coffFct->offsetRetVal = 16 + sizeStack;
    coffFct->offsetParam  = 16 + sizeStack + regOffset(typeFunc->numReturnRegisters());
    coffFct->frameSize    = sizeStack + sizeParamsStack;

    sizeProlog = concat.totalCount() - beforeProlog;
    computeUnwindStack(sizeStack + sizeParamsStack, sizeProlog, unwind);

    // At the end because array must be sorted in 'offset in prolog' descending order
    unwind.push_back(unwind0);

    // Registers are stored after the sizeParamsStack area, which is used to store parameters for function calls
    pp.concat.addString4("\x48\x8D\xBC\x24");
    pp.concat.addU32(sizeParamsStack); // lea rdi, [rsp + sizeParamsStack]

    // Save register parameters
    auto numReturnRegs = typeFunc->numReturnRegisters();
    auto numTotalRegs  = typeFunc->numTotalRegisters();
    for (int i = 0; i < min(4, numTotalRegs); i++)
    {
        static uint8_t x64Reg[]  = {RCX, RDX, R8, R9};
        static uint8_t x64RegF[] = {XMM0, XMM1, XMM2, XMM3};
        if (i < numReturnRegs)
            BackendX64Inst::emit_Store64_Indirect(pp, offsetS4 + (i * 8), x64Reg[i], RDI);
        else
        {
            auto typeParam = typeFunc->registerIdxToType(i - numReturnRegs);
            if (typeParam->isNativeFloat())
                BackendX64Inst::emit_StoreF64_Indirect(pp, offsetS4 + (i * 8), x64RegF[i], RDI);
            else
                BackendX64Inst::emit_Store64_Indirect(pp, offsetS4 + (i * 8), x64Reg[i], RDI);
        }
    }

    auto                                   ip = bc->out;
    VectorNative<uint32_t>                 pushRAParams;
    VectorNative<pair<uint32_t, uint32_t>> pushRVParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
            continue;
        if (ip->flags & BCI_NO_BACKEND)
            continue;

        if (debug)
            dbgSetLocation(coffFct, bc, ip, concat.totalCount() - beforeProlog);

        if (ip->flags & BCI_JUMP_DEST)
            getOrCreateLabel(pp, i);

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::Nop:
        case ByteCodeOp::DecSPBP:
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
        case ByteCodeOp::IntrinsicBcDbg:
            continue;
        }

        switch (ip->op)
        {
        case ByteCodeOp::DebugNop:
            concat.addU8(0x90); // nop
            break;

        case ByteCodeOp::MulAddVC64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\x83\xc1"); // add rcx, ??
            concat.addU8(ip->c.u8);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\x48\xf7\xe1"); // mul rax, rcx
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::Add32byVB32:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->b.u32 <= 0x7F)
            {
                concat.addString2("\x83\x00"); // add [rax], ??
                concat.addU8(ip->b.u8);
            }
            else
            {
                concat.addString2("\x81\x00"); // add [rax], ??????
                concat.addU32(ip->b.u32);
            }
            break;
        case ByteCodeOp::Add64byVB64:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->b.u64 <= 0x7F) // add [rax], ??
            {
                concat.addString3("\x48\x83\x00");
                concat.addU8(ip->b.u8);
            }
            else if (ip->b.u64 <= 0x7FFFFFFF) // add [rax], ????????
            {
                concat.addString3("\x48\x81\x00");
                concat.addU32(ip->b.u32);
            }
            else
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RCX);
                concat.addString3("\x48\x01\x08"); // add [rax], rcx
            }
            break;

        case ByteCodeOp::ClearRA:
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->a.u32), 0, RDI);
            break;
        case ByteCodeOp::ClearRA2:
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->a.u32), 0, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->b.u32), 0, RDI);
            break;
        case ByteCodeOp::ClearRA3:
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->a.u32), 0, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->b.u32), 0, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->c.u32), 0, RDI);
            break;
        case ByteCodeOp::ClearRA4:
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->a.u32), 0, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->b.u32), 0, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->c.u32), 0, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->d.u32), 0, RDI);
            break;

        case ByteCodeOp::CopyRBtoRA8:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_UnsignedExtend_8_To_64(pp, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CopyRBtoRA16:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_UnsignedExtend_16_To_64(pp, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CopyRBtoRA32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CopyRBtoRA64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopyRBAddrToRA:
        case ByteCodeOp::CopyRBAddrToRA2:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CastBool8:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastBool16:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Test16(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastBool32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Test32(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastBool64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Test64(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CastS8S16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CastS8S32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AL_To_EAX(pp);
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastS16S32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AX_To_EAX(pp);
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastF32S32:
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString4("\xf3\x0f\x2c\xc0"); // cvttss2si eax, xmm0
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CastS8S64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AL_To_RAX(pp);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastS16S64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AX_To_RAX(pp);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastS32S64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_EAX_To_RAX(pp);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastF64S64:
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString5("\xf2\x48\x0f\x2c\xc0"); // cvttss2si rax, xmm0
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CastS8F32:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AL_To_EAX(pp);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastS16F32:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AX_To_EAX(pp);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastS32F32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastS64F32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\xF3\x48\x0F\x2A\xC0"); // cvtsi2ss xmm0, rax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastU8F32:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_UnsignedExtend_8_To_32(pp, RAX);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastU16F32:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_UnsignedExtend_16_To_32(pp, RAX);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastU32F32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\xF3\x48\x0F\x2A\xC0"); // cvtsi2ss xmm0, rax => rax and not eax to avoid a signed conversion
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastU64F32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\xF3\x48\x0F\x2A\xC0"); // cvtsi2ss xmm0, rax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastF64F32:
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString4("\xf2\x0f\x5a\xc0"); // cvtsd2ss xmm0, xmm0
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;

        case ByteCodeOp::CastS8F64:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_UnsignedExtend_8_To_32(pp, RAX);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastS16F64:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_UnsignedExtend_16_To_32(pp, RAX);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastS32F64:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastS64F64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\xf2\x48\x0f\x2a\xc0"); // cvtsi2sd xmm0, rax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastU8F64:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_UnsignedExtend_8_To_32(pp, RAX);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastU16F64:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_UnsignedExtend_16_To_32(pp, RAX);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastU32F64:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastU64F64:
        {
            // Code from llvm/clang
            // movq      xmm1, rdi
            // punpckldq xmm1, xmmword ptr[rip + .LCPI0_0] # xmm1 = xmm1[0], mem[0], xmm1[1], mem[1]
            // subpd     xmm1, xmmword ptr[rip + .LCPI0_1]
            // movapd    xmm0, xmm1
            // unpckhpd  xmm0, xmm1                        # xmm0 = xmm0[1], xmm1[1]
            // addsd     xmm0, xmm1
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\x66\x48\x0F\x6E\xC8"); // movq xmm1, rax

            BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symCst_U64F64, 0);
            concat.addString4("\x66\x0F\x62\x09");     // punpckldq xmm1, xmmword ptr [rcx]
            concat.addString5("\x66\x0F\x5C\x49\x10"); // subpd xmm1, xmmword ptr [rcx + 16]
            concat.addString4("\x66\x0F\x28\xC1");     // movapd xmm0, xmm1
            concat.addString4("\x66\x0F\x15\xC1");     // unpckhpd xmm0, xmm1
            concat.addString4("\xF2\x0F\x58\xC1");     // addsd xmm0, xmm1

            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        }

        case ByteCodeOp::CastF32F64:
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString4("\xf3\x0f\x5a\xc0"); // cvtss2sd xmm0, xmm0
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;

        case ByteCodeOp::BinOpShiftLeftU8:
            emitShiftLogical(pp, concat, ip, 8, 0xE0);
            break;
        case ByteCodeOp::BinOpShiftLeftU16:
            emitShiftLogical(pp, concat, ip, 16, 0xE0);
            break;
        case ByteCodeOp::BinOpShiftLeftU32:
            emitShiftLogical(pp, concat, ip, 32, 0xE0);
            break;
        case ByteCodeOp::BinOpShiftLeftU64:
            emitShiftLogical(pp, concat, ip, 64, 0xE0);
            break;

        case ByteCodeOp::BinOpShiftRightU8:
            emitShiftLogical(pp, concat, ip, 8, 0xE8);
            break;
        case ByteCodeOp::BinOpShiftRightU16:
            emitShiftLogical(pp, concat, ip, 16, 0xE8);
            break;
        case ByteCodeOp::BinOpShiftRightU32:
            emitShiftLogical(pp, concat, ip, 32, 0xE8);
            break;
        case ByteCodeOp::BinOpShiftRightU64:
            emitShiftLogical(pp, concat, ip, 64, 0xE8);
            break;

        case ByteCodeOp::BinOpShiftRightS8:
            emitShiftArithmetic(pp, concat, ip, 8);
            break;
        case ByteCodeOp::BinOpShiftRightS16:
            emitShiftArithmetic(pp, concat, ip, 16);
            break;
        case ByteCodeOp::BinOpShiftRightS32:
            emitShiftArithmetic(pp, concat, ip, 32);
            break;
        case ByteCodeOp::BinOpShiftRightS64:
            emitShiftArithmetic(pp, concat, ip, 64);
            break;

        case ByteCodeOp::BinOpXorU8:
            BackendX64Inst::emit_BinOpInt8_At_Reg(pp, ip, X64Op::XOR);
            break;
        case ByteCodeOp::BinOpXorU32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::XOR);
            break;
        case ByteCodeOp::BinOpXorU64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::XOR);
            break;

        case ByteCodeOp::BinOpMulS32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::IMUL);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::BinOpMulU32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::MUL);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoU32));
            break;
        case ByteCodeOp::BinOpMulS64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::IMUL);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoS64));
            break;
        case ByteCodeOp::BinOpMulU64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::MUL);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoU64));
            break;
        case ByteCodeOp::BinOpMulF32:
            BackendX64Inst::emit_BinOpFloat32_At_Reg(pp, ip, X64Op::FMUL);
            break;
        case ByteCodeOp::BinOpMulF64:
            BackendX64Inst::emit_BinOpFloat64_At_Reg(pp, ip, X64Op::FMUL);
            break;

        case ByteCodeOp::BinOpModuloS32:
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, true, 32, true);
            break;
        case ByteCodeOp::BinOpModuloS64:
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, true, 64, true);
            break;
        case ByteCodeOp::BinOpModuloU32:
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, false, 32, true);
            break;
        case ByteCodeOp::BinOpModuloU64:
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, false, 64, true);
            break;

        case ByteCodeOp::BinOpDivS32:
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, true, 32);
            break;
        case ByteCodeOp::BinOpDivS64:
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, true, 64);
            break;
        case ByteCodeOp::BinOpDivU32:
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, false, 32);
            break;
        case ByteCodeOp::BinOpDivU64:
            BackendX64Inst::emit_BinOpInt_Div_At_Reg(pp, ip, false, 64);
            break;
        case ByteCodeOp::BinOpDivF32:
            BackendX64Inst::emit_BinOpFloat32_At_Reg(pp, ip, X64Op::FDIV);
            break;
        case ByteCodeOp::BinOpDivF64:
            BackendX64Inst::emit_BinOpFloat64_At_Reg(pp, ip, X64Op::FDIV);
            break;

        case ByteCodeOp::BinOpPlusS32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::BinOpPlusU32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoU32));
            break;
        case ByteCodeOp::BinOpPlusS64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoS64));
            break;
        case ByteCodeOp::BinOpPlusU64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoU64));
            break;
        case ByteCodeOp::BinOpPlusF32:
            BackendX64Inst::emit_BinOpFloat32_At_Reg(pp, ip, X64Op::FADD);
            break;
        case ByteCodeOp::BinOpPlusF64:
            BackendX64Inst::emit_BinOpFloat64_At_Reg(pp, ip, X64Op::FADD);
            break;

        case ByteCodeOp::BinOpMinusS32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::SUB);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::BinOpMinusU32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoU32));
            break;
        case ByteCodeOp::BinOpMinusS64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::SUB);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoS64));
            break;
        case ByteCodeOp::BinOpMinusU64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoU64));
            break;
        case ByteCodeOp::BinOpMinusF32:
            BackendX64Inst::emit_BinOpFloat32_At_Reg(pp, ip, X64Op::FSUB);
            break;
        case ByteCodeOp::BinOpMinusF64:
            BackendX64Inst::emit_BinOpFloat64_At_Reg(pp, ip, X64Op::FSUB);
            break;

        case ByteCodeOp::BinOpBitmaskAnd8:
            BackendX64Inst::emit_BinOpInt8_At_Reg(pp, ip, X64Op::AND);
            break;
        case ByteCodeOp::BinOpBitmaskAnd16:
            BackendX64Inst::emit_BinOpInt16_At_Reg(pp, ip, X64Op::AND);
            break;
        case ByteCodeOp::BinOpBitmaskAnd32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::AND);
            break;
        case ByteCodeOp::BinOpBitmaskAnd64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::AND);
            break;

        case ByteCodeOp::BinOpBitmaskOr8:
            BackendX64Inst::emit_BinOpInt8_At_Reg(pp, ip, X64Op::OR);
            break;
        case ByteCodeOp::BinOpBitmaskOr16:
            BackendX64Inst::emit_BinOpInt16_At_Reg(pp, ip, X64Op::OR);
            break;
        case ByteCodeOp::BinOpBitmaskOr32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::OR);
            break;
        case ByteCodeOp::BinOpBitmaskOr64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::OR);
            break;

        case ByteCodeOp::AffectOpShiftLeftEqU8:
            emitShiftEqLogical(pp, concat, ip, 8, 0x20);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            emitShiftEqLogical(pp, concat, ip, 16, 0x20);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            emitShiftEqLogical(pp, concat, ip, 32, 0x20);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            emitShiftEqLogical(pp, concat, ip, 64, 0x20);
            break;

        case ByteCodeOp::AffectOpShiftRightEqU8:
            emitShiftEqLogical(pp, concat, ip, 8, 0x28);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            emitShiftEqLogical(pp, concat, ip, 16, 0x28);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            emitShiftEqLogical(pp, concat, ip, 32, 0x28);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            emitShiftEqLogical(pp, concat, ip, 64, 0x28);
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            emitShiftEqArithmetic(pp, concat, ip, 8);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            emitShiftEqArithmetic(pp, concat, ip, 16);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            emitShiftEqArithmetic(pp, concat, ip, 32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            emitShiftEqArithmetic(pp, concat, ip, 64);
            break;

        case ByteCodeOp::AffectOpXorEqU8:
            MK_BINOPEQ8_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::AffectOpXorEqU16:
            MK_BINOPEQ16_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::AffectOpXorEqU32:
            MK_BINOPEQ32_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::AffectOpXorEqU64:
            MK_BINOPEQ64_CAB(X64Op::XOR);
            break;

        case ByteCodeOp::AffectOpOrEqS8:
            MK_BINOPEQ8_CAB(X64Op::OR);
            break;
        case ByteCodeOp::AffectOpOrEqS16:
            MK_BINOPEQ16_CAB(X64Op::OR);
            break;
        case ByteCodeOp::AffectOpOrEqS32:
            MK_BINOPEQ32_CAB(X64Op::OR);
            break;
        case ByteCodeOp::AffectOpOrEqS64:
            MK_BINOPEQ64_CAB(X64Op::OR);
            break;

        case ByteCodeOp::AffectOpAndEqS8:
            MK_BINOPEQ8_CAB(X64Op::AND);
            break;
        case ByteCodeOp::AffectOpAndEqS16:
            MK_BINOPEQ16_CAB(X64Op::AND);
            break;
        case ByteCodeOp::AffectOpAndEqS32:
            MK_BINOPEQ32_CAB(X64Op::AND);
            break;
        case ByteCodeOp::AffectOpAndEqS64:
            MK_BINOPEQ64_CAB(X64Op::AND);
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf6\xe9"); // imul cl
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr->typeInfoS8));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf6\xe1"); // mul cl
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr->typeInfoU8));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xf7\xe9"); // imul cx
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr->typeInfoS16));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xf7\xe1"); // mul cx
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr->typeInfoU16));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf7\xe9"); // imul ecx
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr->typeInfoS32));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf7\xe1"); // mul ecx
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr->typeInfoU32));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xf7\xe9"); // imul rcx
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr->typeInfoS64));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xf7\xe1"); // mul rcx
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr->typeInfoU64));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqF32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf3\x0f\x59\xc1"); // mulss xmm0, xmm1
            BackendX64Inst::emit_StoreF32_Indirect(pp, 0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMulEqF64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf2\x0f\x59\xc1"); // mulss xmm0, xmm1
            BackendX64Inst::emit_StoreF64_Indirect(pp, 0, XMM0, RAX);
            break;

        case ByteCodeOp::AffectOpModuloEqS8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf6\x9b"); // idiv al, bl
            concat.addString2("\x88\xe0"); // mov al, ah
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            concat.addString2("\x66\x99"); // cwd
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xf7\xf9"); // idiv ax, bx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            concat.addString1("\x99"); // cdq
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf7\xf9"); // idiv eax, ebx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            concat.addString2("\x48\x99"); // cqo
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xf7\xf9"); // idiv rax, rcx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqU8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf6\xf1"); // div al, cl
            concat.addString2("\x88\xe0"); // mov al, ah
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear16(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xf7\xf1"); // div ax, cx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear32(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf7\xf1"); // div eax, ecx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear64(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xf7\xf1"); // div rax, rcx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqS8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xF6\xF9"); // idiv al, cl
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            concat.addString2("\x66\x99"); // cwd
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xF7\xF9"); // idiv ax, cx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            concat.addString1("\x99"); // cdq
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xF7\xF9"); // idiv eax, ecx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            concat.addString2("\x48\x99"); // cqo
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xF7\xF9"); // idiv rax, rcx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqU8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xF6\xF1"); // div al, cl
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear16(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xF7\xF1"); // div ax, cx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear32(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xF7\xF1"); // div eax, ecx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear64(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xF7\xF1"); // div rax, rcx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqF32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf3\x0f\x5e\xc1"); // divss xmm0, xmm1
            BackendX64Inst::emit_StoreF32_Indirect(pp, 0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf2\x0f\x5e\xc1"); // divsd xmm0, xmm1
            BackendX64Inst::emit_StoreF64_Indirect(pp, 0, XMM0, RAX);
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            MK_BINOPEQ8_CAB(X64Op::SUB);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr->typeInfoS8));
            break;
        case ByteCodeOp::AffectOpMinusEqU8:
            MK_BINOPEQ8_CAB(X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr->typeInfoU8));
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            MK_BINOPEQ16_CAB(X64Op::SUB);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr->typeInfoU16));
            break;
        case ByteCodeOp::AffectOpMinusEqU16:
            MK_BINOPEQ16_CAB(X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr->typeInfoU16));
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            MK_BINOPEQ32_CAB(X64Op::SUB);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::AffectOpMinusEqU32:
            MK_BINOPEQ32_CAB(X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr->typeInfoU32));
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr->typeInfoS64));
            MK_BINOPEQ64_CAB(X64Op::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqU64:
            MK_BINOPEQ64_CAB(X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr->typeInfoU64));
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            MK_BINOPEQF32_CAB(X64Op::FSUB);
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            MK_BINOPEQF64_CAB(X64Op::FSUB);
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            MK_BINOPEQ8_CAB(X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr->typeInfoS8));
            break;
        case ByteCodeOp::AffectOpPlusEqU8:
            MK_BINOPEQ8_CAB(X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr->typeInfoU8));
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            MK_BINOPEQ16_CAB(X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr->typeInfoS16));
            break;
        case ByteCodeOp::AffectOpPlusEqU16:
            MK_BINOPEQ16_CAB(X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr->typeInfoU16));
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            MK_BINOPEQ32_CAB(X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::AffectOpPlusEqU32:
            MK_BINOPEQ32_CAB(X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr->typeInfoU32));
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            MK_BINOPEQ64_CAB(X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr->typeInfoS64));
            break;
        case ByteCodeOp::AffectOpPlusEqU64:
            MK_BINOPEQ64_CAB(X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr->typeInfoU64));
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            MK_BINOPEQF32_CAB(X64Op::FADD);
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            MK_BINOPEQF64_CAB(X64Op::FADD);
            break;

        case ByteCodeOp::ZeroToTrue:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test32(pp, RAX, RAX);
            BackendX64Inst::emit_SetE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::LowerZeroToTrue:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\xc1\xe8\x1f"); // shr eax, 31
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::LowerEqZeroToTrue:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test32(pp, RAX, RAX);
            concat.addString3("\x0f\x9e\xc0"); // setle al
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::GreaterZeroToTrue:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test32(pp, RAX, RAX);
            BackendX64Inst::emit_SetG(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::GreaterEqZeroToTrue:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString2("\xf7\xd0");     // not eax
            concat.addString3("\xc1\xe8\x1f"); // shr eax, 31
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CompareOpGreaterS32:
            MK_BINOP32_CAB(emit_Cmp32_IndirectDst, emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetG(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            MK_BINOP64_CAB(emit_Cmp64_IndirectDst, emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetG(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterU32:
            MK_BINOP32_CAB(emit_Cmp32_IndirectDst, emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            MK_BINOP64_CAB(emit_Cmp64_IndirectDst, emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::CompareOpGreaterEqS32:
            MK_BINOP32_CAB(emit_Cmp32_IndirectDst, emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetGE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterEqS64:
            MK_BINOP64_CAB(emit_Cmp64_IndirectDst, emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetGE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterEqU32:
            MK_BINOP32_CAB(emit_Cmp32_IndirectDst, emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetAE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterEqU64:
            MK_BINOP64_CAB(emit_Cmp64_IndirectDst, emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetAE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterEqF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            BackendX64Inst::emit_SetAE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterEqF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            BackendX64Inst::emit_SetAE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::CompareOpLowerS32:
            MK_BINOP32_CAB(emit_Cmp32_IndirectDst, emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetL(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerS64:
            MK_BINOP64_CAB(emit_Cmp64_IndirectDst, emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetL(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerU32:
            MK_BINOP32_CAB(emit_Cmp32_IndirectDst, emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetB(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerU64:
            MK_BINOP64_CAB(emit_Cmp64_IndirectDst, emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetB(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            BackendX64Inst::emit_SetB(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            BackendX64Inst::emit_SetB(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::CompareOpLowerEqS32:
            MK_BINOP32_CAB(emit_Cmp32_IndirectDst, emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetLE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerEqS64:
            MK_BINOP64_CAB(emit_Cmp64_IndirectDst, emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetLE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerEqU32:
            MK_BINOP32_CAB(emit_Cmp32_IndirectDst, emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetBE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerEqU64:
            MK_BINOP64_CAB(emit_Cmp64_IndirectDst, emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetBE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerEqF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            BackendX64Inst::emit_SetBE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerEqF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            BackendX64Inst::emit_SetBE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::CompareOp3WayU32:
        case ByteCodeOp::CompareOp3WayS32:
            BackendX64Inst::emit_BinOpInt32(pp, ip, X64Op::SUB);
            BackendX64Inst::emit_Clear32(pp, RAX);
            BackendX64Inst::emit_Test32(pp, RCX, RCX);
            BackendX64Inst::emit_SetG(pp);
            pp.concat.addString3("\xC1\xE9\x1F"); // shr ecx, 31
            pp.concat.addString2("\x29\xC8");     // sub eax, ecx
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOp3WayU64:
        case ByteCodeOp::CompareOp3WayS64:
            BackendX64Inst::emit_BinOpInt64(pp, ip, X64Op::SUB);
            BackendX64Inst::emit_Clear64(pp, RAX);
            BackendX64Inst::emit_Test64(pp, RCX, RCX);
            BackendX64Inst::emit_SetG(pp);
            pp.concat.addString4("\x48\xC1\xE9\x3F"); // shr rcx, 63
            pp.concat.addString2("\x29\xC8");         // sub eax, ecx
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOp3WayF32:
            BackendX64Inst::emit_BinOpFloat32(pp, ip, X64Op::FSUB);
            BackendX64Inst::emit_Clear32(pp, RAX);
            BackendX64Inst::emit_Clear32(pp, RCX);
            pp.concat.addString3("\x0F\x57\xC9"); // xorps xmm1, xmm1
            pp.concat.addString3("\x0F\x2F\xC1"); // comiss xmm0, xmm1
            BackendX64Inst::emit_SetA(pp);
            pp.concat.addString3("\x0F\x2F\xC8"); // comiss xmm1, xmm0
            BackendX64Inst::emit_SetA(pp, RCX);
            pp.concat.addString2("\x29\xC8"); // sub eax, ecx
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOp3WayF64:
            BackendX64Inst::emit_BinOpFloat64(pp, ip, X64Op::FSUB);
            BackendX64Inst::emit_Clear32(pp, RAX);
            BackendX64Inst::emit_Clear32(pp, RCX);
            pp.concat.addString3("\x0F\x57\xC9");     // xorps xmm1, xmm1
            pp.concat.addString4("\x66\x0F\x2F\xC1"); // comisd xmm0, xmm1
            BackendX64Inst::emit_SetA(pp);
            pp.concat.addString4("\x66\x0F\x2F\xC8"); // comisd xmm1, xmm0
            BackendX64Inst::emit_SetA(pp, RCX);
            pp.concat.addString2("\x29\xC8"); // sub eax, ecx
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::CompareOpEqual8:
            MK_BINOP8_CAB(emit_Cmp8_Indirect, emit_Cmp8);
            BackendX64Inst::emit_SetE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpEqual16:
            MK_BINOP16_CAB(emit_Cmp16_Indirect, emit_Cmp16);
            BackendX64Inst::emit_SetE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpEqual32:
            MK_BINOP32_CAB(emit_Cmp32_IndirectDst, emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpEqual64:
            MK_BINOP64_CAB(emit_Cmp64_IndirectDst, emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::CompareOpNotEqual8:
            MK_BINOP8_CAB(emit_Cmp8_Indirect, emit_Cmp8);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpNotEqual16:
            MK_BINOP16_CAB(emit_Cmp16_Indirect, emit_Cmp16);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpNotEqual32:
            MK_BINOP32_CAB(emit_Cmp32_IndirectDst, emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpNotEqual64:
            MK_BINOP64_CAB(emit_Cmp64_IndirectDst, emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::IntrinsicStringCmp:
            SWAG_ASSERT(sizeParamsStack >= 5 * sizeof(Register));
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->d.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), R8, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), R9, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 32, RAX, RSP);
            emitCall(pp, g_LangSpec->name_atstrcmp);
            break;
        case ByteCodeOp::IntrinsicTypeCmp:
            SWAG_ASSERT(sizeParamsStack >= 4 * sizeof(Register));
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->d.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), R8, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), R9, RDI);
            emitCall(pp, g_LangSpec->name_attypecmp);
            break;

        case ByteCodeOp::TestNotZero8:
            MK_IMMB_8(RAX);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::TestNotZero16:
            MK_IMMB_16(RAX);
            BackendX64Inst::emit_Test16(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::TestNotZero32:
            MK_IMMB_32(RAX);
            BackendX64Inst::emit_Test32(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::TestNotZero64:
            MK_IMMB_64(RAX);
            BackendX64Inst::emit_Test64(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::NegBool:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x34\x01"); // xor al, 1
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::NegS32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString2("\xf7\xd8"); // neg eax
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::NegS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\x48\xf7\xd8"); // neg rax
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::NegF32:
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, 0x80000000, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetFLT, RAX, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, offsetFLT, XMM1, RDI);
            concat.addString3("\x0f\x57\xc1"); // xorps xmm0, xmm1
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::NegF64:
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, 0x80000000'00000000, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetFLT, RAX, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, offsetFLT, XMM1, RDI);
            concat.addString4("\x66\x0f\x57\xc1"); // xorpd xmm0, xmm1
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;

        case ByteCodeOp::InvertU8:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString2("\xf6\xd0"); // not al
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::InvertU16:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\x66\xf7\xd0"); // not ax
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::InvertU32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString2("\xf7\xd0"); // not eax
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::InvertU64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\x48\xf7\xd0"); // not rax
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::JumpIfTrue:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfFalse:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero8:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero16:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test16(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test32(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test64(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero8:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero16:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test16(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test32(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test64(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::Jump:
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JUMP, i, ip->b.s32);
            break;

        case ByteCodeOp::JumpIfNotEqual8:
            MK_JMPCMP_8(BackendX64Inst::JNZ);
            break;
        case ByteCodeOp::JumpIfNotEqual16:
            MK_JMPCMP_16(BackendX64Inst::JNZ);
            break;
        case ByteCodeOp::JumpIfNotEqual32:
            MK_JMPCMP_32(BackendX64Inst::JNZ);
            break;
        case ByteCodeOp::JumpIfNotEqual64:
            MK_JMPCMP_64(BackendX64Inst::JNZ);
            break;

        case ByteCodeOp::JumpIfEqual8:
            MK_JMPCMP_8(BackendX64Inst::JZ);
            break;
        case ByteCodeOp::JumpIfEqual16:
            MK_JMPCMP_16(BackendX64Inst::JZ);
            break;
        case ByteCodeOp::JumpIfEqual32:
            MK_JMPCMP_32(BackendX64Inst::JZ);
            break;
        case ByteCodeOp::IncJumpIfEqual64:
            BackendX64Inst::emit_Inc64_Indirect(pp, regOffset(ip->a.u32), RDI);
            MK_JMPCMP_64(BackendX64Inst::JZ);
            break;
        case ByteCodeOp::JumpIfEqual64:
            MK_JMPCMP_64(BackendX64Inst::JZ);
            break;

        case ByteCodeOp::JumpIfLowerS32:
            MK_JMPCMP_32(BackendX64Inst::JL);
            break;
        case ByteCodeOp::JumpIfLowerS64:
            MK_JMPCMP_64(BackendX64Inst::JL);
            break;
        case ByteCodeOp::JumpIfLowerU32:
            MK_JMPCMP_32(BackendX64Inst::JB);
            break;
        case ByteCodeOp::JumpIfLowerU64:
            MK_JMPCMP_64(BackendX64Inst::JB);
            break;
        case ByteCodeOp::JumpIfLowerF32:
            MK_JMPCMP_F32(BackendX64Inst::JB);
            break;
        case ByteCodeOp::JumpIfLowerF64:
            MK_JMPCMP_F64(BackendX64Inst::JB);
            break;

        case ByteCodeOp::JumpIfLowerEqS32:
            MK_JMPCMP_32(BackendX64Inst::JLE);
            break;
        case ByteCodeOp::JumpIfLowerEqS64:
            MK_JMPCMP_64(BackendX64Inst::JLE);
            break;
        case ByteCodeOp::JumpIfLowerEqU32:
            MK_JMPCMP_32(BackendX64Inst::JBE);
            break;
        case ByteCodeOp::JumpIfLowerEqU64:
            MK_JMPCMP_64(BackendX64Inst::JBE);
            break;
        case ByteCodeOp::JumpIfLowerEqF32:
            MK_JMPCMP_F32(BackendX64Inst::JBE);
            break;
        case ByteCodeOp::JumpIfLowerEqF64:
            MK_JMPCMP_F64(BackendX64Inst::JBE);
            break;

        case ByteCodeOp::JumpIfGreaterS32:
            MK_JMPCMP_32(BackendX64Inst::JG);
            break;
        case ByteCodeOp::JumpIfGreaterS64:
            MK_JMPCMP_64(BackendX64Inst::JG);
            break;
        case ByteCodeOp::JumpIfGreaterU32:
            MK_JMPCMP_32(BackendX64Inst::JA);
            break;
        case ByteCodeOp::JumpIfGreaterU64:
            MK_JMPCMP_64(BackendX64Inst::JA);
            break;
        case ByteCodeOp::JumpIfGreaterF32:
            MK_JMPCMP_F32(BackendX64Inst::JA);
            break;
        case ByteCodeOp::JumpIfGreaterF64:
            MK_JMPCMP_F64(BackendX64Inst::JA);
            break;

        case ByteCodeOp::JumpIfGreaterEqS32:
            MK_JMPCMP_32(BackendX64Inst::JGE);
            break;
        case ByteCodeOp::JumpIfGreaterEqS64:
            MK_JMPCMP_64(BackendX64Inst::JGE);
            break;
        case ByteCodeOp::JumpIfGreaterEqU32:
            MK_JMPCMP_32(BackendX64Inst::JAE);
            break;
        case ByteCodeOp::JumpIfGreaterEqU64:
            MK_JMPCMP_64(BackendX64Inst::JAE);
            break;
        case ByteCodeOp::JumpIfGreaterEqF32:
            MK_JMPCMP_F32(BackendX64Inst::JAE);
            break;
        case ByteCodeOp::JumpIfGreaterEqF64:
            MK_JMPCMP_F64(BackendX64Inst::JAE);
            break;

        case ByteCodeOp::IncrementRA32:
            BackendX64Inst::emit_Inc32_Indirect(pp, regOffset(ip->a.u32), RDI);
            break;
        case ByteCodeOp::DecrementRA32:
            BackendX64Inst::emit_Dec32_Indirect(pp, regOffset(ip->a.u32), RDI);
            break;
        case ByteCodeOp::IncrementRA64:
            BackendX64Inst::emit_Inc64_Indirect(pp, regOffset(ip->a.u32), RDI);
            break;
        case ByteCodeOp::DecrementRA64:
            BackendX64Inst::emit_Dec64_Indirect(pp, regOffset(ip->a.u32), RDI);
            break;

        case ByteCodeOp::DeRef8:
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, ip->c.u32, RAX, RAX);
            BackendX64Inst::emit_UnsignedExtend_8_To_64(pp, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRef16:
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, ip->c.u32, RAX, RAX);
            BackendX64Inst::emit_UnsignedExtend_16_To_64(pp, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRef32:
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, ip->c.u32, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRef64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            if (ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF)
                BackendX64Inst::emit_Load64_Indirect(pp, ip->c.u32, RAX, RAX);
            else
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->c.u64, RCX);
                BackendX64Inst::emit_Op64(pp, RCX, RAX, X64Op::ADD);
                BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            }
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::DeRefStringSlice:
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFFF);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, ip->c.u32 + 8, RCX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, ip->c.u32 + 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::GetFromBssSeg64:
            BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symBSIndex, 0);
            BackendX64Inst::emit_Load64_Indirect(pp, ip->b.u32, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::GetFromMutableSeg64:
            BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symMSIndex, 0);
            BackendX64Inst::emit_Load64_Indirect(pp, ip->b.u32, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::GetFromStack8:
            BackendX64Inst::emit_Clear64(pp, RAX);
            BackendX64Inst::emit_Load8_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::GetFromStack16:
            BackendX64Inst::emit_Clear64(pp, RAX);
            BackendX64Inst::emit_Load16_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::GetFromStack32:
            BackendX64Inst::emit_Load32_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::GetFromStack64:
            BackendX64Inst::emit_Load64_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopyStack8:
            BackendX64Inst::emit_Load8_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            break;
        case ByteCodeOp::CopyStack16:
            BackendX64Inst::emit_Load16_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            break;
        case ByteCodeOp::CopyStack32:
            BackendX64Inst::emit_Load32_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            break;
        case ByteCodeOp::CopyStack64:
            BackendX64Inst::emit_Load64_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            break;

        case ByteCodeOp::GetFromStack64x2:
            BackendX64Inst::emit_Load64_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, offsetStack + ip->d.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::ClearMaskU32:
            if (ip->b.u32 == 0xFF)
            {
                concat.addString2("\x0F\xB6"); // movzx eax, byte ptr [rdi + ??]
                BackendX64Inst::emit_ModRM(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            else if (ip->b.u32 == 0xFFFF)
            {
                concat.addString2("\x0F\xB7"); // movzx eax, word ptr [rdi + ??]
                BackendX64Inst::emit_ModRM(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            else
            {
                SWAG_ASSERT(false);
            }
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::ClearMaskU64:
            if (ip->b.u32 == 0xFF)
            {
                concat.addString3("\x48\x0F\xB6"); // movzx rax, byte ptr [rdi + ??]
                BackendX64Inst::emit_ModRM(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            else if (ip->b.u32 == 0xFFFF)
            {
                concat.addString3("\x48\x0F\xB7"); // movzx rax, word ptr [rdi + ??]
                BackendX64Inst::emit_ModRM(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            else if (ip->b.u32 == 0xFFFFFFFF)
            {
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            else
            {
                SWAG_ASSERT(false);
            }
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::SetZeroAtPointer8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store8_Immediate(pp, ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store16_Immediate(pp, ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store32_Immediate(pp, ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointerX:
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
            // Expand
            if (ip->b.u32 <= 128 && buildParameters.buildCfg->backendOptimizeSpeed)
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
                BackendX64Inst::emitClearX(pp, ip->b.u32, ip->c.u32, RAX);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
                BackendX64Inst::emit_Add64_Immediate(pp, ip->c.u64, RCX);
                BackendX64Inst::emit_Clear64(pp, RDX);
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, R8);
                emitCall(pp, g_LangSpec->name_memset);
            }
            break;
        case ByteCodeOp::SetZeroAtPointerXRB:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Clear64(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_imul64_RAX(pp, ip->c.u64);
            BackendX64Inst::emit_Copy64(pp, RAX, R8);
            emitCall(pp, g_LangSpec->name_memset);
            break;

        case ByteCodeOp::SetZeroStack8:
            BackendX64Inst::emit_Store8_Immediate(pp, offsetStack + ip->a.u32, 0, RDI);
            break;
        case ByteCodeOp::SetZeroStack16:
            BackendX64Inst::emit_Store16_Immediate(pp, offsetStack + ip->a.u32, 0, RDI);
            break;
        case ByteCodeOp::SetZeroStack32:
            BackendX64Inst::emit_Store32_Immediate(pp, offsetStack + ip->a.u32, 0, RDI);
            break;
        case ByteCodeOp::SetZeroStack64:
            BackendX64Inst::emit_Store64_Immediate(pp, offsetStack + ip->a.u32, 0, RDI);
            break;
        case ByteCodeOp::SetZeroStackX:
        {
            // Expand
            if (ip->b.u32 <= 128 && buildParameters.buildCfg->backendOptimizeSpeed)
            {
                BackendX64Inst::emitClearX(pp, ip->b.u32, offsetStack + ip->a.u32, RDI);
            }
            else
            {
                BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetStack + ip->a.u32, RCX, RDI);
                BackendX64Inst::emit_Clear64(pp, RDX);
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, R8);
                emitCall(pp, g_LangSpec->name_memset);
            }
            break;
        }

        case ByteCodeOp::SetAtPointer8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store8_Immediate(pp, ip->c.u32, ip->b.u8, RAX);
            else
            {
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
                BackendX64Inst::emit_Store8_Indirect(pp, ip->c.u32, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store16_Immediate(pp, ip->c.u32, ip->b.u16, RAX);
            else
            {
                BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
                BackendX64Inst::emit_Store16_Indirect(pp, ip->c.u32, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store32_Immediate(pp, ip->c.u32, ip->b.u32, RAX);
            else
            {
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
                BackendX64Inst::emit_Store32_Indirect(pp, ip->c.u32, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)
                BackendX64Inst::emit_Store64_Immediate(pp, ip->c.u32, ip->b.u64, RAX);
            else if (ip->flags & BCI_IMM_B)
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RCX);
                BackendX64Inst::emit_Store64_Indirect(pp, ip->c.u32, RCX, RAX);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, ip->c.u32, RCX, RAX);
            }
            break;

        case ByteCodeOp::SetAtStackPointer8:
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store8_Immediate(pp, offsetStack + ip->a.u32, ip->b.u8, RDI);
            else
            {
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
                BackendX64Inst::emit_Store8_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer16:
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store16_Immediate(pp, offsetStack + ip->a.u32, ip->b.u16, RDI);
            else
            {
                BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
                BackendX64Inst::emit_Store16_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer32:
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store32_Immediate(pp, offsetStack + ip->a.u32, ip->b.u32, RDI);
            else
            {
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
                BackendX64Inst::emit_Store32_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer64:
            if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)
                BackendX64Inst::emit_Store64_Immediate(pp, offsetStack + ip->a.u32, ip->b.u64, RDI);
            else if (ip->flags & BCI_IMM_B)
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RAX);
                BackendX64Inst::emit_Store64_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            }
            break;

        case ByteCodeOp::SetAtStackPointer8x2:
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store8_Immediate(pp, offsetStack + ip->a.u32, ip->b.u8, RDI);
            else
            {
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
                BackendX64Inst::emit_Store8_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D)
                BackendX64Inst::emit_Store8_Immediate(pp, offsetStack + ip->c.u32, ip->d.u8, RDI);
            else
            {
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
                BackendX64Inst::emit_Store8_Indirect(pp, offsetStack + ip->c.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer16x2:
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store16_Immediate(pp, offsetStack + ip->a.u32, ip->b.u16, RDI);
            else
            {
                BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
                BackendX64Inst::emit_Store16_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D)
                BackendX64Inst::emit_Store16_Immediate(pp, offsetStack + ip->c.u32, ip->d.u16, RDI);
            else
            {
                BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
                BackendX64Inst::emit_Store16_Indirect(pp, offsetStack + ip->c.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer32x2:
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store32_Immediate(pp, offsetStack + ip->a.u32, ip->b.u32, RDI);
            else
            {
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
                BackendX64Inst::emit_Store32_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D)
                BackendX64Inst::emit_Store32_Immediate(pp, offsetStack + ip->c.u32, ip->d.u32, RDI);
            else
            {
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
                BackendX64Inst::emit_Store32_Indirect(pp, offsetStack + ip->c.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer64x2:
            if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)
                BackendX64Inst::emit_Store64_Immediate(pp, offsetStack + ip->a.u32, ip->b.u64, RDI);
            else if (ip->flags & BCI_IMM_B)
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RAX);
                BackendX64Inst::emit_Store64_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D && ip->d.u64 <= 0x7FFFFFFF)
                BackendX64Inst::emit_Store64_Immediate(pp, offsetStack + ip->c.u32, ip->d.u64, RDI);
            else if (ip->flags & BCI_IMM_D)
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->d.u64, RAX);
                BackendX64Inst::emit_Store64_Indirect(pp, offsetStack + ip->c.u32, RAX, RDI);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, offsetStack + ip->c.u32, RAX, RDI);
            }
            break;

        case ByteCodeOp::MakeStackPointer:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::MakeStackPointerRT:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetStack + ip->a.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetRT + regOffset(0), RAX, RDI);
            break;

        case ByteCodeOp::MakeMutableSegPointer:
            BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symMSIndex, ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::MakeBssSegPointer:
            BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symBSIndex, ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::MakeConstantSegPointer:
            BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symCSIndex, ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::MakeCompilerSegPointer:
            return ip->node->sourceFile->report({ip->node, Err(Err0060)});

        case ByteCodeOp::IncPointer64:
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RAX);
            else
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            if (ip->a.u32 == ip->c.u32)
                BackendX64Inst::emit_Op64_IndirectDst(pp, regOffset(ip->a.u32), RAX, RDI, X64Op::ADD);
            else
            {
                BackendX64Inst::emit_Op64_IndirectSrc(pp, regOffset(ip->a.u32), RAX, RDI, X64Op::ADD);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            }
            break;
        case ByteCodeOp::DecPointer64:
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RCX);
            else
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            if (ip->a.u32 == ip->c.u32)
                BackendX64Inst::emit_Op64_IndirectDst(pp, regOffset(ip->a.u32), RCX, RDI, X64Op::SUB);
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
                BackendX64Inst::emit_Op64(pp, RCX, RAX, X64Op::SUB);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            }
            break;

        case ByteCodeOp::Mul64byVB64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_imul64_RAX(pp, ip->b.u64);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::Div64byVB64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RCX);
            BackendX64Inst::emit_Clear64(pp, RDX);
            concat.addString3("\x48\xf7\xf9"); // idiv rax, rcx
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::SetImmediate32:
            BackendX64Inst::emit_Store32_Immediate(pp, regOffset(ip->a.u32), ip->b.u32, RDI);
            break;
        case ByteCodeOp::SetImmediate64:
            if (ip->b.u64 <= 0x7FFFFFFF)
                BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->a.u32), ip->b.u64, RDI);
            else
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RAX);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            break;

        case ByteCodeOp::MemCpy8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emitCopyX(pp, 1, 0, RCX, RDX);
            break;
        case ByteCodeOp::MemCpy16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emitCopyX(pp, 2, 0, RCX, RDX);
            break;
        case ByteCodeOp::MemCpy32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emitCopyX(pp, 4, 0, RCX, RDX);
            break;
        case ByteCodeOp::MemCpy64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emitCopyX(pp, 8, 0, RCX, RDX);
            break;

        case ByteCodeOp::IntrinsicMemCpy:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            if ((ip->flags & BCI_IMM_C) && ip->c.u64 <= 128 && buildParameters.buildCfg->backendOptimizeSpeed)
                BackendX64Inst::emitCopyX(pp, ip->c.u32, 0, RCX, RDX);
            else
            {
                MK_IMMC_64(R8);
                emitCall(pp, g_LangSpec->name_memcpy);
            }
            break;

        case ByteCodeOp::IntrinsicMemMove:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            MK_IMMC_64(R8);
            emitCall(pp, g_LangSpec->name_memmove);
            break;

        case ByteCodeOp::IntrinsicMemSet:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            if ((ip->flags & BCI_IMM_B) && (ip->flags & BCI_IMM_C) && (ip->b.u8 == 0) && (ip->c.u64 <= 128) && buildParameters.buildCfg->backendOptimizeSpeed)
                BackendX64Inst::emitClearX(pp, ip->c.u32, 0, RCX);
            else
            {
                MK_IMMB_8(RDX);
                MK_IMMC_64(R8);
                emitCall(pp, g_LangSpec->name_memset);
            }
            break;

        case ByteCodeOp::IntrinsicMemCmp:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
            MK_IMMD_64(R8);
            emitCall(pp, g_LangSpec->name_memcmp);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::IntrinsicStrLen:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            emitCall(pp, g_LangSpec->name_strlen);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicStrCmp:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
            emitCall(pp, g_LangSpec->name_strcmp);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::IntrinsicAlloc:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            emitCall(pp, g_LangSpec->name_malloc);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicRealloc:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
            emitCall(pp, g_LangSpec->name_realloc);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicFree:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            emitCall(pp, g_LangSpec->name_free);
            break;

        case ByteCodeOp::InternalInitStackTrace:
            emitCall(pp, g_LangSpec->name__initStackTrace);
            break;
        case ByteCodeOp::InternalStackTrace:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            emitCall(pp, g_LangSpec->name__stackTrace);
            break;
        case ByteCodeOp::InternalPanic:
            emitInternalPanic(buildParameters, ip->node, (const char*) ip->d.pointer);
            break;

        case ByteCodeOp::InternalGetTlsPtr:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Symbol_RelocationValue(pp, RDX, pp.symTls_threadLocalId, 0);
            BackendX64Inst::emit_Load64_Immediate(pp, module->tlsSegment.totalCount, R8, true);
            BackendX64Inst::emit_Symbol_RelocationAddr(pp, R9, pp.symTLSIndex, 0);
            emitCall(pp, g_LangSpec->name__tlsGetPtr);
            break;

        case ByteCodeOp::IntrinsicGetContext:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Symbol_RelocationValue(pp, RDX, pp.symPI_contextTlsId, 0);
            emitCall(pp, g_LangSpec->name__tlsGetValue);
            break;
        case ByteCodeOp::IntrinsicSetContext:
            BackendX64Inst::emit_Symbol_RelocationValue(pp, RCX, pp.symPI_contextTlsId, 0);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RDX, RDI);
            emitCall(pp, g_LangSpec->name__tlsSetValue);
            break;

        case ByteCodeOp::IntrinsicCVaStart:
        {
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            int stackOffset = 16 + sizeStack + regOffset(ip->c.u32);
            BackendX64Inst::emit_LoadAddress_Indirect(pp, stackOffset, RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);
            break;
        }
        case ByteCodeOp::IntrinsicCVaEnd:
            break;
        case ByteCodeOp::IntrinsicCVaArg:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RCX, RAX);
            switch (ip->c.u32)
            {
            case 1:
                BackendX64Inst::emit_Clear64(pp, RDX);
                BackendX64Inst::emit_Load8_Indirect(pp, 0, RDX, RCX);
                break;
            case 2:
                BackendX64Inst::emit_Clear64(pp, RDX);
                BackendX64Inst::emit_Load16_Indirect(pp, 0, RDX, RCX);
                break;
            case 4:
                BackendX64Inst::emit_Load32_Indirect(pp, 0, RDX, RCX);
                break;
            case 8:
                BackendX64Inst::emit_Load64_Indirect(pp, 0, RDX, RCX);
                break;
            }

            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, 8, RCX);
            BackendX64Inst::emit_Op64_IndirectDst(pp, 0, RCX, RAX, X64Op::ADD);
            break;

        case ByteCodeOp::IntrinsicArguments:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            emitCall(pp, g_LangSpec->name_atargs);
            break;

        case ByteCodeOp::IntrinsicCompiler:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, 0, 0, RAX);
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::IntrinsicIsByteCode:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store32_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::IntrinsicErrorMsg:
            SWAG_ASSERT(sizeParamsStack >= 3 * sizeof(Register));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), R8, RDI);
            emitCall(pp, g_LangSpec->name_aterrormsg);
            break;
        case ByteCodeOp::IntrinsicPanic:
            SWAG_ASSERT(sizeParamsStack >= 3 * sizeof(Register));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), R8, RDI);
            emitCall(pp, g_LangSpec->name_atpanic);
            break;
        case ByteCodeOp::IntrinsicInterfaceOf:
            SWAG_ASSERT(sizeParamsStack >= 3 * sizeof(Register));
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->c.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), R8, RDI);
            emitCall(pp, g_LangSpec->name_atinterfaceof);
            break;

        case ByteCodeOp::CopyRCtoRR:
            BackendX64Inst::emit_Load64_Indirect(pp, offsetS4 + regOffset(0), RAX, RDI);
            if (ip->flags & BCI_IMM_A && ip->a.u64 <= 0x7FFFFFFF)
            {
                BackendX64Inst::emit_Store64_Immediate(pp, 0, ip->a.u64, RAX);
            }
            else if (ip->flags & BCI_IMM_A)
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u64, RCX);
                BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RAX);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RAX);
            }
            break;

        case ByteCodeOp::CopyRCtoRR2:
            BackendX64Inst::emit_Load64_Indirect(pp, offsetS4 + regOffset(0), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RAX);

            BackendX64Inst::emit_Load64_Indirect(pp, offsetS4 + regOffset(1), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RAX);
            break;

        case ByteCodeOp::CopyRCtoRT:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetRT + regOffset(0), RAX, RDI);
            break;

        case ByteCodeOp::CopyRRtoRC:
            BackendX64Inst::emit_Load64_Indirect(pp, offsetS4 + regOffset(0), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(0), RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopyRTtoRC:
            BackendX64Inst::emit_Load64_Indirect(pp, offsetRT + regOffset(0), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopyRTtoRC2:
            BackendX64Inst::emit_Load64_Indirect(pp, offsetRT + regOffset(0), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, offsetRT + regOffset(1), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopySP:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopySPVaargs:
        {
            auto typeFuncCall    = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->d.pointer, TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);
            bool foreignOrLambda = ip->c.b;
            if (!foreignOrLambda)
            {
                // We are close to the byte code, as all PushRaParams are already in the correct order for variadics.
                // We need the RAX register to address the stack where all are stored.
                // There's 2 more PushRAParam to come after CopySPVaargs, so offset is 16.
                // We also need to take care of real parameters (offset is ip->b.u32)
                // We also need to take care of the return registers, which are always first
                BackendX64Inst::emit_LoadAddress_Indirect(pp, (uint8_t) (16 + ip->b.u32 + (typeFuncCall->numReturnRegisters() * 8)), RAX, RSP);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            else if (!pushRVParams.empty())
            {
                auto     sizeOf            = pushRVParams[0].second;
                int      idxParam          = (int) pushRVParams.size() - 1;
                uint32_t variadicStackSize = idxParam * sizeOf;
                MK_ALIGN16(variadicStackSize);
                uint32_t offset = sizeParamsStack - variadicStackSize;
                while (idxParam >= 0)
                {
                    auto reg = pushRVParams[idxParam].first;
                    switch (sizeOf)
                    {
                    case 1:
                        BackendX64Inst::emit_Load8_Indirect(pp, regOffset(reg), RAX, RDI);
                        BackendX64Inst::emit_Store8_Indirect(pp, offset, RAX, RSP);
                        break;
                    case 2:
                        BackendX64Inst::emit_Load16_Indirect(pp, regOffset(reg), RAX, RDI);
                        BackendX64Inst::emit_Store16_Indirect(pp, offset, RAX, RSP);
                        break;
                    case 4:
                        BackendX64Inst::emit_Load32_Indirect(pp, regOffset(reg), RAX, RDI);
                        BackendX64Inst::emit_Store32_Indirect(pp, offset, RAX, RSP);
                        break;
                    case 8:
                        BackendX64Inst::emit_Load64_Indirect(pp, regOffset(reg), RAX, RDI);
                        BackendX64Inst::emit_Store64_Indirect(pp, offset, RAX, RSP);
                        break;
                    }

                    idxParam--;
                    offset += sizeOf;
                }

                BackendX64Inst::emit_LoadAddress_Indirect(pp, (sizeParamsStack - variadicStackSize), RAX, RSP);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            else
            {
                // All of this is complicated. But ip->b.u32 has been reduced by one register in case of closure, and
                // we have a dynamic test for bytecode execution. But for runtime, be put it back.
                auto sizeB = ip->b.u32;
                if (typeFuncCall->isClosure())
                    sizeB += sizeof(Register);

                // We need to flatten all variadic registers, in order, in the stack, and emit the address of that array
                // We compute the number of variadic registers by removing registers of normal parameters (ip->b.u32)
                int      idxParam          = (int) pushRAParams.size() - (sizeB / sizeof(Register)) - 1;
                uint32_t variadicStackSize = (idxParam + 1) * sizeof(Register);
                MK_ALIGN16(variadicStackSize);
                uint32_t offset = sizeParamsStack - variadicStackSize;
                while (idxParam >= 0)
                {
                    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(pushRAParams[idxParam]), RAX, RDI);
                    BackendX64Inst::emit_Store64_Indirect(pp, offset, RAX, RSP);
                    idxParam--;
                    offset += 8;
                }

                BackendX64Inst::emit_LoadAddress_Indirect(pp, (sizeParamsStack - variadicStackSize), RAX, RSP);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            }
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

        case ByteCodeOp::GetFromStackParam8:
            emitLocalParam(pp, typeFunc, ip->a.u32, ip->c.u32, 1, offsetS4, sizeStack);
            break;
        case ByteCodeOp::GetFromStackParam16:
            emitLocalParam(pp, typeFunc, ip->a.u32, ip->c.u32, 2, offsetS4, sizeStack);
            break;
        case ByteCodeOp::GetFromStackParam32:
            emitLocalParam(pp, typeFunc, ip->a.u32, ip->c.u32, 4, offsetS4, sizeStack);
            break;
        case ByteCodeOp::GetFromStackParam64:
            emitLocalParam(pp, typeFunc, ip->a.u32, ip->c.u32, 8, offsetS4, sizeStack);
            break;

        case ByteCodeOp::MakeLambda:
        {
            auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->b.pointer, AstNodeKind::FuncDecl);
            SWAG_ASSERT(funcNode);

            Utf8 name;

            if (funcNode->attributeFlags & ATTRIBUTE_FOREIGN)
            {
                TypeInfoFuncAttr* typeFuncNode = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
                auto              foreignValue = typeFuncNode->attributes.getValue(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_function);
                SWAG_ASSERT(foreignValue && !foreignValue->text.empty());
                name = foreignValue->text;
            }
            else if (funcNode->attributeFlags & ATTRIBUTE_CALLBACK)
            {
                funcNode->computeFullNameForeign(true);
                name = funcNode->fullnameForeign;
            }
            else
            {
                auto funcBC = (ByteCode*) ip->c.pointer;
                SWAG_ASSERT(funcBC);
                name = funcBC->getCallName();
            }

            BackendX64Inst::emit_Load64_Immediate(pp, 0, RAX, true);

            CoffRelocation reloc;
            reloc.virtualAddress = concat.totalCount() - sizeof(uint64_t) - pp.textSectionOffset;
            auto callSym         = getOrAddSymbol(pp, name, CoffSymbolKind::Extern);
            reloc.symbolIndex    = callSym->index;
            reloc.type           = IMAGE_REL_AMD64_ADDR64;
            pp.relocTableTextSection.table.push_back(reloc);

            if (funcNode->attributeFlags & (ATTRIBUTE_FOREIGN | ATTRIBUTE_CALLBACK))
            {
                BackendX64Inst::emit_Load64_Immediate(pp, SWAG_LAMBDA_FOREIGN_MARKER, RCX);
                BackendX64Inst::emit_Op64(pp, RCX, RAX, X64Op::OR);
            }

            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }

        case ByteCodeOp::IntrinsicMakeForeign:
        {
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, SWAG_LAMBDA_FOREIGN_MARKER, RCX);
            BackendX64Inst::emit_Op64(pp, RCX, RAX, X64Op::OR);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }

        case ByteCodeOp::IntrinsicMakeCallback:
        {
            // Test if it's a bytecode lambda
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, SWAG_LAMBDA_BC_MARKER, RCX);
            BackendX64Inst::emit_Op64(pp, RAX, RCX, X64Op::AND);
            BackendX64Inst::emit_Test64(pp, RCX, RCX);
            concat.addString2("\x0f\x85"); // jnz ???????? => jump to bytecode lambda
            concat.addU32(0);
            auto jumpToBCAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpToBCOffset = concat.totalCount();

            // Foreign lambda
            //////////////////

            BackendX64Inst::emit_Load64_Immediate(pp, ~SWAG_LAMBDA_FOREIGN_MARKER, RCX);
            BackendX64Inst::emit_Op64(pp, RCX, RAX, X64Op::AND);
            concat.addString1("\xe9"); // jmp ???????? => jump after foreign lambda
            concat.addU32(0);
            auto jumpBCToAfterAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpBCToAfterOffset = concat.totalCount();

            // ByteCode lambda
            //////////////////
            *jumpToBCAddr = concat.totalCount() - jumpToBCOffset;

            BackendX64Inst::emit_Copy64(pp, RAX, RCX);
            BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symPI_makeCallback, 0);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            concat.addString2("\xff\xd0"); // call rax

            // End
            //////////////////
            *jumpBCToAfterAddr = concat.totalCount() - jumpBCToAfterOffset;
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);

            break;
        }

        case ByteCodeOp::LambdaCall:
        {
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

            // Test if it's a bytecode lambda
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), R10, RDI);
            concat.addString4("\x49\x0F\xBA\xE2"); // bt r10, ??
            concat.addU8(SWAG_LAMBDA_BC_MARKER_BIT);
            concat.addString2("\x0f\x82"); // jb ???????? => jump to bytecode lambda
            concat.addU32(0);
            auto jumpToBCAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpToBCOffset = concat.totalCount();

            // Test if it's a foreign lambda (and clear the bit if it is)
            concat.addString4("\x49\x0F\xBA\xF2"); // btr r10, ??
            concat.addU8(SWAG_LAMBDA_FOREIGN_MARKER_BIT);
            concat.addString2("\x0f\x82"); // jb ???????? => jump to foreign lambda
            concat.addU32(0);
            auto jumpToForeignAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpToForeignOffset = concat.totalCount();

            // Local lambda
            //////////////////
            emitLocalCallParameters(pp, sizeParamsStack, typeFuncBC, offsetRT, pushRAParams, pushRVParams);
            concat.addString3("\x41\xFF\xD2"); // call r10

            concat.addString1("\xe9"); // jmp ???????? => jump after bytecode lambda
            concat.addU32(0);
            auto jumpBCToAfterAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpBCToAfterOffset = concat.totalCount();

            // Foreign lambda
            //////////////////
            *jumpToForeignAddr = concat.totalCount() - jumpToForeignOffset;

            SWAG_CHECK(emitForeignCallParameters(pp, moduleToGen, offsetRT, typeFuncBC, pushRAParams));
            concat.addString3("\x41\xFF\xD2"); // call r10
            emitForeignCallResult(pp, typeFuncBC, offsetRT);

            concat.addString1("\xe9"); // jmp ???????? => jump after bytecode lambda
            concat.addU32(0);
            auto jumpForeignToAfterAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpForeignToAfterOffset = concat.totalCount();

            // ByteCode lambda
            //////////////////
            *jumpToBCAddr = concat.totalCount() - jumpToBCOffset;

            BackendX64Inst::emit_Copy64(pp, R10, RCX);
            emitByteCodeLambdaParams(pp, typeFuncBC, offsetRT, pushRAParams);
            BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symPI_byteCodeRun, 0);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            concat.addString2("\xff\xd0"); // call rax

            // End
            //////////////////
            *jumpBCToAfterAddr      = concat.totalCount() - jumpBCToAfterOffset;
            *jumpForeignToAfterAddr = concat.totalCount() - jumpForeignToAfterOffset;

            pushRAParams.clear();
            pushRVParams.clear();
            break;
        }

        case ByteCodeOp::IncSPPostCall:
        case ByteCodeOp::IncSPPostCallCond:
            pushRAParams.clear();
            pushRVParams.clear();
            break;

        case ByteCodeOp::LocalCall:
        {
            auto              funcBC     = (ByteCode*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;
            emitLocalCallParameters(pp, sizeParamsStack, typeFuncBC, offsetRT, pushRAParams, pushRVParams);
            emitCall(pp, funcBC->getCallName());
            pushRAParams.clear();
            pushRVParams.clear();
            break;
        }

        case ByteCodeOp::ForeignCall:
            emitForeignCall(pp, moduleToGen, ip, offsetRT, pushRAParams);
            pushRAParams.clear();
            pushRVParams.clear();
            break;

        case ByteCodeOp::Ret:
            BackendX64Inst::emit_Add_Cst32_To_RSP(pp, sizeStack + sizeParamsStack);
            BackendX64Inst::emit_Pop(pp, RDI);
            BackendX64Inst::emit_Ret(pp);
            break;

        case ByteCodeOp::IntrinsicAtomicAddS8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ8_LOCK_CAB(X64Op::ADD);
            break;
        case ByteCodeOp::IntrinsicAtomicAddS16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ16_LOCK_CAB(X64Op::ADD);
            break;
        case ByteCodeOp::IntrinsicAtomicAddS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ32_LOCK_CAB(X64Op::ADD);
            break;
        case ByteCodeOp::IntrinsicAtomicAddS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ64_LOCK_CAB(X64Op::ADD);
            break;

        case ByteCodeOp::IntrinsicAtomicAndS8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ8_LOCK_CAB(X64Op::AND);
            break;
        case ByteCodeOp::IntrinsicAtomicAndS16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ16_LOCK_CAB(X64Op::AND);
            break;
        case ByteCodeOp::IntrinsicAtomicAndS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ32_LOCK_CAB(X64Op::AND);
            break;
        case ByteCodeOp::IntrinsicAtomicAndS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ64_LOCK_CAB(X64Op::AND);
            break;

        case ByteCodeOp::IntrinsicAtomicOrS8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ8_LOCK_CAB(X64Op::OR);
            break;
        case ByteCodeOp::IntrinsicAtomicOrS16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ16_LOCK_CAB(X64Op::OR);
            break;
        case ByteCodeOp::IntrinsicAtomicOrS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ32_LOCK_CAB(X64Op::OR);
            break;
        case ByteCodeOp::IntrinsicAtomicOrS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ64_LOCK_CAB(X64Op::OR);
            break;

        case ByteCodeOp::IntrinsicAtomicXorS8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ8_LOCK_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::IntrinsicAtomicXorS16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ16_LOCK_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::IntrinsicAtomicXorS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ32_LOCK_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::IntrinsicAtomicXorS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ64_LOCK_CAB(X64Op::XOR);
            break;

        case ByteCodeOp::IntrinsicAtomicXchgS8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ8_LOCK_CAB(X64Op::XCHG);
            break;
        case ByteCodeOp::IntrinsicAtomicXchgS16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ16_LOCK_CAB(X64Op::XCHG);
            break;
        case ByteCodeOp::IntrinsicAtomicXchgS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ32_LOCK_CAB(X64Op::XCHG);
            break;
        case ByteCodeOp::IntrinsicAtomicXchgS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RCX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            MK_BINOPEQ64_LOCK_CAB(X64Op::XCHG);
            break;

        case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            MK_IMMB_8(RAX);
            MK_IMMC_8(RDX);
            pp.concat.addString4("\xF0\x0F\xB0\x11"); // lock CMPXCHG [rcx], dl
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            MK_IMMB_16(RAX);
            MK_IMMC_16(RDX);
            pp.concat.addString5("\x66\xF0\x0F\xB1\x11"); // lock CMPXCHG [rcx], dx
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            MK_IMMB_32(RAX);
            MK_IMMC_32(RDX);
            pp.concat.addString4("\xF0\x0F\xB1\x11"); // lock CMPXCHG [rcx], edx
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            MK_IMMB_64(RAX);
            MK_IMMC_64(RDX);
            pp.concat.addString5("\xF0\x48\x0F\xB1\x11"); // lock CMPXCHG [rcx], rdx
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
            break;

        case ByteCodeOp::IntrinsicS8x1:
        {
            MK_IMMB_8(RAX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                BackendX64Inst::emit_Copy8(pp, RAX, RCX);
                concat.addString3("\xC0\xF9\x07"); // sar cl, 7
                concat.addString2("\x30\xC8");     // xor al, cl
                concat.addString2("\x28\xC8");     // sub al, cl
                break;
            case TokenId::IntrinsicBitCountNz:
                BackendX64Inst::emit_UnsignedExtend_8_To_32(pp, RAX);
                concat.addString4("\xF3\x0F\xB8\xC0"); // popcnt eax, eax
                break;
            case TokenId::IntrinsicBitCountTz:
                BackendX64Inst::emit_UnsignedExtend_8_To_32(pp, RAX);
                concat.addString3("\x0F\xBC\xC0"); // bsf eax, eax
                BackendX64Inst::emit_Load32_Immediate(pp, 8, RCX);
                concat.addString3("\x0F\x44\xC1"); // cmove eax, ecx
                break;
            case TokenId::IntrinsicBitCountLz:
                BackendX64Inst::emit_UnsignedExtend_8_To_32(pp, RAX);
                concat.addString3("\x0F\xBD\xC0"); // bsr eax, eax
                BackendX64Inst::emit_Load32_Immediate(pp, 15, RCX);
                concat.addString3("\x0F\x44\xC1"); // cmove eax, ecx
                concat.addString3("\x83\xF0\x07"); // xor eax, 7
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicS16x1:
        {
            MK_IMMB_16(RAX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                BackendX64Inst::emit_Copy16(pp, RAX, RCX);
                concat.addString4("\x66\xC1\xF9\x0F"); // sar cx, 15
                concat.addString3("\x66\x31\xC8");     // xor ax, cx
                concat.addString3("\x66\x29\xC8");     // sub ax, cx
                break;
            case TokenId::IntrinsicBitCountNz:
                BackendX64Inst::emit_UnsignedExtend_16_To_32(pp, RAX);
                concat.addString5("\x66\xF3\x0F\xB8\xC0"); // popcnt ax, ax
                break;
            case TokenId::IntrinsicBitCountTz:
                concat.addString4("\x66\x0F\xBC\xC0"); // bsf ax, ax
                BackendX64Inst::emit_Load16_Immediate(pp, 16, RCX);
                concat.addString4("\x66\x0F\x44\xC1"); // cmove ax, cx
                break;
            case TokenId::IntrinsicBitCountLz:
                concat.addString4("\x66\x0F\xBD\xC0"); // bsr ax, ax
                BackendX64Inst::emit_Load16_Immediate(pp, 31, RCX);
                concat.addString4("\x66\x0F\x44\xC1"); // cmove ax, cx
                concat.addString4("\x66\x83\xF0\x0F"); // xor ax, 15
                break;
            case TokenId::IntrinsicByteSwap:
                concat.addString4("\x66\xC1\xC0\x08"); // rol ax, 8
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicS32x1:
        {
            MK_IMMB_32(RAX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                BackendX64Inst::emit_Copy32(pp, RAX, RCX);
                concat.addString3("\xC1\xF9\x1F"); // sar ecx, 31
                concat.addString2("\x31\xC8");     // xor eax, ecx
                concat.addString2("\x29\xC8");     // sub eax, ecx
                break;
            case TokenId::IntrinsicBitCountNz:
                concat.addString4("\xF3\x0F\xB8\xC0"); // popcnt eax, eax
                break;
            case TokenId::IntrinsicBitCountTz:
                concat.addString3("\x0F\xBC\xC0"); // bsf eax, eax
                BackendX64Inst::emit_Load32_Immediate(pp, 32, RCX);
                concat.addString3("\x0F\x44\xC1"); // cmove eax, ecx
                break;
            case TokenId::IntrinsicBitCountLz:
                concat.addString3("\x0F\xBD\xC0"); // bsr eax, eax
                BackendX64Inst::emit_Load32_Immediate(pp, 63, RCX);
                concat.addString3("\x0F\x44\xC1"); // cmove eax, ecx
                concat.addString3("\x83\xF0\x1F"); // xor eax, 31
                break;
            case TokenId::IntrinsicByteSwap:
                concat.addString2("\x0F\xC8"); // bswap eax
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicS64x1:
        {
            MK_IMMB_64(RAX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                BackendX64Inst::emit_Copy64(pp, RAX, RCX);
                concat.addString4("\x48\xC1\xF9\x3F"); // sar rcx, 63
                concat.addString3("\x48\x31\xC8");     // xor rax, rcx
                concat.addString3("\x48\x29\xC8");     // sub rax, rcx
                break;
            case TokenId::IntrinsicBitCountNz:
                concat.addString5("\xF3\x48\x0F\xB8\xC0"); // popcnt rax, rax
                break;
            case TokenId::IntrinsicBitCountTz:
                concat.addString4("\x48\x0F\xBC\xC0"); // bsf rax, rax
                BackendX64Inst::emit_Load32_Immediate(pp, 64, RCX);
                concat.addString4("\x48\x0F\x44\xC1"); // cmove rax, rcx
                break;
            case TokenId::IntrinsicBitCountLz:
                concat.addString4("\x48\x0F\xBD\xC0"); // bsr rax, rax
                BackendX64Inst::emit_Load64_Immediate(pp, 127, RCX);
                concat.addString4("\x48\x0F\x44\xC1"); // cmove rax, rcx
                concat.addString4("\x48\x83\xF0\x3F"); // xor rax, 63
                break;
            case TokenId::IntrinsicByteSwap:
                concat.addString3("\x48\x0F\xC8"); // bswap rax
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }

        case ByteCodeOp::IntrinsicS8x2:
        {
            MK_IMMB_S8_TO_S32(RAX);
            MK_IMMC_S8_TO_S32(RCX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                BackendX64Inst::emit_Cmp32(pp, RCX, RAX);
                concat.addString3("\x0F\x4C\xC1"); // cmovl eax, ecx
                break;
            case TokenId::IntrinsicMax:
                BackendX64Inst::emit_Cmp32(pp, RAX, RCX);
                concat.addString3("\x0F\x4C\xC1"); // cmovl eax, ecx
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicS16x2:
        {
            MK_IMMB_16(RAX);
            MK_IMMC_16(RCX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                BackendX64Inst::emit_Cmp16(pp, RCX, RAX);
                concat.addString4("\x66\x0F\x4C\xC1"); // cmovl ax, cx
                break;
            case TokenId::IntrinsicMax:
                BackendX64Inst::emit_Cmp16(pp, RAX, RCX);
                concat.addString4("\x66\x0F\x4C\xC1"); // cmovl ax, cx
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicS32x2:
        {
            MK_IMMB_32(RAX);
            MK_IMMC_32(RCX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                BackendX64Inst::emit_Cmp32(pp, RCX, RAX);
                concat.addString3("\x0F\x4C\xC1"); // cmovl eax, ecx
                break;
            case TokenId::IntrinsicMax:
                BackendX64Inst::emit_Cmp32(pp, RAX, RCX);
                concat.addString3("\x0F\x4C\xC1"); // cmovl eax, ecx
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicS64x2:
        {
            MK_IMMB_64(RAX);
            MK_IMMC_64(RCX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                BackendX64Inst::emit_Cmp64(pp, RCX, RAX);
                concat.addString4("\x48\x0F\x4C\xC1"); // cmovl rax, rcx
                break;
            case TokenId::IntrinsicMax:
                BackendX64Inst::emit_Cmp64(pp, RAX, RCX);
                concat.addString4("\x48\x0F\x4C\xC1"); // cmovl rax, rcx
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }

        case ByteCodeOp::IntrinsicU8x2:
        {
            MK_IMMB_U8_TO_U32(RAX);
            MK_IMMC_U8_TO_U32(RCX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                BackendX64Inst::emit_Cmp32(pp, RCX, RAX);
                concat.addString3("\x0F\x42\xC1"); // cmovb eax, ecx
                break;
            case TokenId::IntrinsicMax:
                BackendX64Inst::emit_Cmp32(pp, RAX, RCX);
                concat.addString3("\x0F\x42\xC1"); // cmovb eax, ecx
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicU16x2:
        {
            MK_IMMB_16(RAX);
            MK_IMMC_16(RCX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                BackendX64Inst::emit_Cmp16(pp, RCX, RAX);
                concat.addString4("\x66\x0F\x42\xC1"); // cmovb ax, cx
                break;
            case TokenId::IntrinsicMax:
                BackendX64Inst::emit_Cmp16(pp, RAX, RCX);
                concat.addString4("\x66\x0F\x42\xC1"); // cmovb ax, cx
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicU32x2:
        {
            MK_IMMB_32(RAX);
            MK_IMMC_32(RCX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                BackendX64Inst::emit_Cmp32(pp, RCX, RAX);
                concat.addString3("\x0F\x42\xC1"); // cmovb eax, ecx
                break;
            case TokenId::IntrinsicMax:
                BackendX64Inst::emit_Cmp32(pp, RAX, RCX);
                concat.addString3("\x0F\x42\xC1"); // cmovb eax, ecx
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicU64x2:
        {
            MK_IMMB_64(RAX);
            MK_IMMC_64(RCX);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                BackendX64Inst::emit_Cmp64(pp, RCX, RAX);
                concat.addString4("\x48\x0F\x42\xC1"); // cmovb rax, rcx
                break;
            case TokenId::IntrinsicMax:
                BackendX64Inst::emit_Cmp64(pp, RAX, RCX);
                concat.addString4("\x48\x0F\x42\xC1"); // cmovb rax, rcx
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }

        case ByteCodeOp::IntrinsicF32x2:
        {
            MK_IMMB_F32(XMM0);
            MK_IMMC_F32(XMM1);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicPow:
                emitCall(pp, g_LangSpec->name_powf);
                break;
            case TokenId::IntrinsicMin:
                concat.addString4("\xF3\x0F\x5D\xC1"); // minss xmm0, xmm1
                break;
            case TokenId::IntrinsicMax:
                concat.addString4("\xF3\x0F\x5F\xC1"); // maxss xmm0, xmm1
                break;
            case TokenId::IntrinsicATan2:
                emitCall(pp, g_LangSpec->name_atan2f);
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicF64x2:
        {
            MK_IMMB_F64(XMM0);
            MK_IMMC_F64(XMM1);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicPow:
                emitCall(pp, g_LangSpec->name_pow);
                break;
            case TokenId::IntrinsicMin:
                concat.addString4("\xF2\x0F\x5D\xC1"); // minss xmm0, xmm1
                break;
            case TokenId::IntrinsicMax:
                concat.addString4("\xF2\x0F\x5F\xC1"); // maxss xmm0, xmm1
                break;
            case TokenId::IntrinsicATan2:
                emitCall(pp, g_LangSpec->name_atan2);
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        }

        case ByteCodeOp::IntrinsicF32x1:
        {
            MK_IMMB_F32(XMM0);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                concat.addString3("\x0F\x51\xC0"); // sqrtps xmm0, xmm0
                break;
            case TokenId::IntrinsicSin:
                emitCall(pp, g_LangSpec->name_sinf);
                break;
            case TokenId::IntrinsicCos:
                emitCall(pp, g_LangSpec->name_cosf);
                break;
            case TokenId::IntrinsicTan:
                emitCall(pp, g_LangSpec->name_tanf);
                break;
            case TokenId::IntrinsicSinh:
                emitCall(pp, g_LangSpec->name_sinhf);
                break;
            case TokenId::IntrinsicCosh:
                emitCall(pp, g_LangSpec->name_coshf);
                break;
            case TokenId::IntrinsicTanh:
                emitCall(pp, g_LangSpec->name_tanhf);
                break;
            case TokenId::IntrinsicASin:
                emitCall(pp, g_LangSpec->name_asinf);
                break;
            case TokenId::IntrinsicACos:
                emitCall(pp, g_LangSpec->name_acosf);
                break;
            case TokenId::IntrinsicATan:
                emitCall(pp, g_LangSpec->name_atanf);
                break;
            case TokenId::IntrinsicLog:
                emitCall(pp, g_LangSpec->name_logf);
                break;
            case TokenId::IntrinsicLog2:
                emitCall(pp, g_LangSpec->name_log2f);
                break;
            case TokenId::IntrinsicLog10:
                emitCall(pp, g_LangSpec->name_log10f);
                break;
            case TokenId::IntrinsicFloor:
                emitCall(pp, g_LangSpec->name_floorf);
                break;
            case TokenId::IntrinsicCeil:
                emitCall(pp, g_LangSpec->name_ceilf);
                break;
            case TokenId::IntrinsicTrunc:
                emitCall(pp, g_LangSpec->name_truncf);
                break;
            case TokenId::IntrinsicRound:
                emitCall(pp, g_LangSpec->name_roundf);
                break;
            case TokenId::IntrinsicAbs:
                BackendX64Inst::emit_Load64_Immediate(pp, 0x7FFFFFFF, RAX);
                BackendX64Inst::emit_CopyF64(pp, RAX, XMM1);
                concat.addString3("\x0F\x54\xC1"); // andps xmm0, xmm1
                break;
            case TokenId::IntrinsicExp:
                emitCall(pp, g_LangSpec->name_expf);
                break;
            case TokenId::IntrinsicExp2:
                emitCall(pp, g_LangSpec->name_exp2f);
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        }

        case ByteCodeOp::IntrinsicF64x1:
        {
            MK_IMMB_F64(XMM0);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                concat.addString4("\x66\x0F\x51\xC0"); // sqrtpd xmm0, xmm0
                break;
            case TokenId::IntrinsicSin:
                emitCall(pp, g_LangSpec->name_sin);
                break;
            case TokenId::IntrinsicCos:
                emitCall(pp, g_LangSpec->name_cos);
                break;
            case TokenId::IntrinsicTan:
                emitCall(pp, g_LangSpec->name_tan);
                break;
            case TokenId::IntrinsicSinh:
                emitCall(pp, g_LangSpec->name_sinh);
                break;
            case TokenId::IntrinsicCosh:
                emitCall(pp, g_LangSpec->name_cosh);
                break;
            case TokenId::IntrinsicTanh:
                emitCall(pp, g_LangSpec->name_tanh);
                break;
            case TokenId::IntrinsicASin:
                emitCall(pp, g_LangSpec->name_asin);
                break;
            case TokenId::IntrinsicACos:
                emitCall(pp, g_LangSpec->name_acos);
                break;
            case TokenId::IntrinsicATan:
                emitCall(pp, g_LangSpec->name_atan);
                break;
            case TokenId::IntrinsicATan2:
                emitCall(pp, g_LangSpec->name_atan2);
                break;
            case TokenId::IntrinsicLog:
                emitCall(pp, g_LangSpec->name_log);
                break;
            case TokenId::IntrinsicLog2:
                emitCall(pp, g_LangSpec->name_log2);
                break;
            case TokenId::IntrinsicLog10:
                emitCall(pp, g_LangSpec->name_log10);
                break;
            case TokenId::IntrinsicFloor:
                emitCall(pp, g_LangSpec->name_floor);
                break;
            case TokenId::IntrinsicCeil:
                emitCall(pp, g_LangSpec->name_ceil);
                break;
            case TokenId::IntrinsicTrunc:
                emitCall(pp, g_LangSpec->name_trunc);
                break;
            case TokenId::IntrinsicRound:
                emitCall(pp, g_LangSpec->name_round);
                break;
            case TokenId::IntrinsicAbs:
                BackendX64Inst::emit_Load64_Immediate(pp, 0x7FFFFFFF'FFFFFFFF, RAX);
                BackendX64Inst::emit_CopyF64(pp, RAX, XMM1);
                concat.addString4("\x66\x0F\x54\xC1"); // andpd xmm0, xmm1
                break;
            case TokenId::IntrinsicExp:
                emitCall(pp, g_LangSpec->name_exp);
                break;
            case TokenId::IntrinsicExp2:
                emitCall(pp, g_LangSpec->name_exp2);
                break;
            default:
                ok = false;
                moduleToGen->internalError(Fmt("unknown intrinsic `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        }

        case ByteCodeOp::IntrinsicGetErr:
        {
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            emitCall(pp, g_LangSpec->name__geterr);
            break;
        }

        case ByteCodeOp::IntrinsicSetErr:
        {
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            emitCall(pp, g_LangSpec->name_atseterr);
            break;
        }

        case ByteCodeOp::InternalCheckAny:
        {
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), R8, RDI);
            emitCall(pp, g_LangSpec->name__checkAny);
            break;
        }

        case ByteCodeOp::InternalClearErr:
            emitCall(pp, g_LangSpec->name__clearerr);
            break;
        case ByteCodeOp::InternalPushErr:
            emitCall(pp, g_LangSpec->name__pusherr);
            break;
        case ByteCodeOp::InternalPopErr:
            emitCall(pp, g_LangSpec->name__poperr);
            break;

        default:
            ok = false;
            moduleToGen->internalError(Fmt("unknown instruction `%s` during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    return ok;
}
