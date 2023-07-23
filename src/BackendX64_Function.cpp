#include "pch.h"
#include "BackendX64.h"
#include "BackendX64_Macros.h"
#include "ByteCodeGenJob.h"
#include "Report.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "ByteCode.h"
#include "Module.h"

#define UWOP_PUSH_NONVOL 0
#define UWOP_ALLOC_LARGE 1
#define UWOP_ALLOC_SMALL 2

bool BackendX64::emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc)
{
    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC) && (buildParameters.compileType != BackendCompileType::Test))
        return true;

    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *perThread[ct][precompileIndex];
    auto&       concat          = pp.concat;
    auto        typeFunc        = bc->getCallType();
    auto        returnType      = typeFunc->concreteReturnType();
    bool        ok              = true;
    bool        debug           = buildParameters.buildCfg->backendDebugInformations;
    const auto& cc              = typeFunc->getCallConv();

    concat.align(16);
    uint32_t startAddress = concat.totalCount();

    pp.clearInstructionCache();
    pp.labels.clear();
    pp.labelsToSolve.clear();
    bc->markLabels();

    // Get function name
    Utf8         funcName   = bc->getCallNameFromDecl();
    AstFuncDecl* bcFuncNode = bc->node ? CastAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl) : nullptr;

    // Export symbol
    if (bcFuncNode && bcFuncNode->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
            pp.directives += Fmt("/EXPORT:%s ", funcName.c_str());
    }

    // Symbol
    auto symbolFuncIndex  = pp.getOrAddSymbol(funcName, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct          = registerFunction(pp, bc->node, symbolFuncIndex);
    coffFct->typeFunc     = typeFunc;
    coffFct->startAddress = startAddress;
    if (debug)
        dbgSetLocation(coffFct, bc, nullptr, 0);

    // In order, starting at RSP, we have :
    //
    // sizeParamsStack to store function call parameters
    // ...padding to 16...

    // All registers:       bc->maxReservedRegisterRC
    // offsetRT:            bc->maxCallResults
    // offsetS4:            N registers to store registers used to pass parameters (4 in Swag calling convention)
    // offsetResult:        sizeof(Register) to store return value
    // offsetStack:         stackSize, function local stack
    // offsetFLT:           sizeof(Register)
    // ...padding to 16... => total is sizeStack

    uint32_t offsetRT     = bc->maxReservedRegisterRC * sizeof(Register);
    uint32_t offsetS4     = offsetRT + bc->maxCallResults * sizeof(Register);
    uint32_t offsetResult = offsetS4 + cc.paramByRegisterCount * sizeof(Register);
    uint32_t offsetStack  = offsetResult + sizeof(Register);

    // For float load (should be reserved only if we have floating point operations in that function)
    uint32_t offsetFLT = offsetStack + typeFunc->stackSize;

    uint32_t sizeStack = offsetFLT + 8;
    MK_ALIGN16(sizeStack);

    // Calling convention, space for at least 'cc.paramByRegisterCount' parameters when calling a function
    // (should ideally be reserved only if we have a call)
    uint32_t sizeParamsStack = max(cc.paramByRegisterCount * sizeof(Register), (bc->maxCallParams + 1) * sizeof(Register));

    // Because of variadic parameters in fct calls, we need to add some extra room, in case we have to flatten them
    // We want to be sure to have the room to flatten the array of variadics (make all params contiguous). That's
    // why we multiply by 2.
    sizeParamsStack *= 2;

    MK_ALIGN16(sizeParamsStack);

    auto     beforeProlog = concat.totalCount();
    uint32_t numTotalRegs = typeFunc->numParamsRegisters();

    VectorNative<CPURegister> unwindRegs;
    VectorNative<uint32_t>    unwindOffsetRegs;

    // RDI will be a pointer to the stack, and the list of registers is stored at the start
    // of the stack
    pp.emit_Push(RDI);
    unwindRegs.push_back(RDI);
    unwindOffsetRegs.push_back(concat.totalCount() - beforeProlog);

    // Push on scratch register per parameter
    while (coffFct->numScratchRegs < min(cc.numScratchRegisters, min(cc.paramByRegisterCount, numTotalRegs)))
    {
        auto cpuReg = (CPURegister) (cc.firstScratchRegister + coffFct->numScratchRegs);
        pp.emit_Push(cpuReg);
        unwindRegs.push_back(cpuReg);
        unwindOffsetRegs.push_back(concat.totalCount() - beforeProlog);
        coffFct->numScratchRegs++;
    }

    // Stack align
    if ((unwindRegs.size() & 1) == 0)
        sizeStack += sizeof(void*);

    // Check stack
    if (sizeStack + sizeParamsStack >= SWAG_LIMIT_PAGE_STACK)
    {
        pp.emit_Load64_Immediate(RAX, sizeStack + sizeParamsStack);
        emitCall(pp, "__chkstk");
    }

    pp.emit_Sub32_RSP(sizeStack + sizeParamsStack);

    // We need to start at sizeof(void*) because the call has pushed one register on the stack
    coffFct->offsetCallerStackParams = (uint32_t) (sizeof(void*) + (unwindRegs.size() * sizeof(void*)) + sizeStack);
    coffFct->offsetStack             = offsetStack;
    coffFct->offsetLocalStackParams  = offsetS4;
    coffFct->frameSize               = sizeStack + sizeParamsStack;

    // Unwind information (with the pushed registers)
    VectorNative<uint16_t> unwind;
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    computeUnwind(unwindRegs, unwindOffsetRegs, sizeStack + sizeParamsStack, sizeProlog, unwind);

    // Registers are stored after the sizeParamsStack area, which is used to store parameters for function calls
    pp.concat.addString4("\x48\x8D\xBC\x24");
    pp.concat.addU32(sizeParamsStack); // lea rdi, [rsp + sizeParamsStack]

    // Save register parameters
    uint32_t iReg = 0;
    while (iReg < min(cc.paramByRegisterCount, numTotalRegs))
    {
        auto     typeParam   = typeFunc->registerIdxToType(iReg);
        uint32_t stackOffset = getParamStackOffset(coffFct, iReg);
        if (cc.useRegisterFloat && typeParam->isNativeFloat())
            pp.emit_StoreF64_Indirect(stackOffset, cc.paramByRegisterFloat[iReg], RDI);
        else
            pp.emit_Store64_Indirect(stackOffset, cc.paramByRegisterInteger[iReg], RDI);

        if (iReg < coffFct->numScratchRegs)
            pp.emit_Load64_Indirect(stackOffset, (CPURegister) (cc.firstScratchRegister + iReg), RDI);

        iReg++;
    }

    // Save pointer to return value if this is a return by copy
    if (CallConv::returnByAddress(typeFunc) && iReg < cc.paramByRegisterCount)
    {
        uint32_t stackOffset = getParamStackOffset(coffFct, iReg);
        pp.emit_Store64_Indirect(stackOffset, cc.paramByRegisterInteger[iReg], RDI);
        iReg++;
    }

    // Save C variadics
    if (typeFunc->isCVariadic())
    {
        while (iReg < cc.paramByRegisterCount)
        {
            uint32_t stackOffset = coffFct->offsetCallerStackParams + regOffset(iReg);
            pp.emit_Store64_Indirect(stackOffset, cc.paramByRegisterInteger[iReg], RDI);
            iReg++;
        }
    }

    // Use R11 as base pointer for capture parameters
    // This is used to debug and have access to capture parameters, even if we "lose" rcx
    // which is the register that will have a pointer to the capture buffer (but rcx is volatile)
    if (typeFunc->isClosure() && debug)
        pp.emit_CopyN(R12, RCX, X64Bits::B64);

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

        if (ip->flags & (BCI_START_STMT | BCI_JUMP_DEST))
            pp.clearInstructionCache();

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::Nop:
        case ByteCodeOp::DecSPBP:
        case ByteCodeOp::SetBP:
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
        case ByteCodeOp::IntrinsicBcBreakpoint:
            continue;
        default:
            break;
        }

        switch (ip->op)
        {
        case ByteCodeOp::DebugNop:
            concat.addU8(0x90); // nop
            break;

            /////////////////////////////////////

        case ByteCodeOp::MulAddVC64:
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RCX);
            pp.emit_OpN_Immediate(RCX, ip->c.u8, X64Op::ADD, X64Bits::B64);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B64);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::Add32byVB32:
            pp.emit_OpN_IndirectDst(regOffset(ip->a.u32), ip->b.u64, RDI, X64Op::ADD, X64Bits::B32);
            break;
        case ByteCodeOp::Add64byVB64:
            pp.emit_OpN_IndirectDst(regOffset(ip->a.u32), ip->b.u64, RDI, X64Op::ADD, X64Bits::B64);
            break;

            /////////////////////////////////////

        case ByteCodeOp::ClearRA:
            pp.emit_Store64_Immediate(regOffset(ip->a.u32), 0, RDI);
            break;
        case ByteCodeOp::ClearRAx2:
            pp.emit_Store64_Immediate(regOffset(ip->a.u32), 0, RDI);
            pp.emit_Store64_Immediate(regOffset(ip->b.u32), 0, RDI);
            break;
        case ByteCodeOp::ClearRAx3:
            pp.emit_Store64_Immediate(regOffset(ip->a.u32), 0, RDI);
            pp.emit_Store64_Immediate(regOffset(ip->b.u32), 0, RDI);
            pp.emit_Store64_Immediate(regOffset(ip->c.u32), 0, RDI);
            break;
        case ByteCodeOp::ClearRAx4:
            pp.emit_Store64_Immediate(regOffset(ip->a.u32), 0, RDI);
            pp.emit_Store64_Immediate(regOffset(ip->b.u32), 0, RDI);
            pp.emit_Store64_Immediate(regOffset(ip->c.u32), 0, RDI);
            pp.emit_Store64_Immediate(regOffset(ip->d.u32), 0, RDI);
            break;

            /////////////////////////////////////

        case ByteCodeOp::CopyRBtoRA8:
            pp.emit_LoadU8U64_Indirect(regOffset(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CopyRBtoRA16:
            pp.emit_LoadU16U64_Indirect(regOffset(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CopyRBtoRA32:
            pp.emit_Load32_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CopyRBtoRA64:
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CopyRBtoRA64x2:
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(regOffset(ip->d.u32), RAX);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            break;

        case ByteCodeOp::CopyRBAddrToRA:
        case ByteCodeOp::CopyRBAddrToRA2:
            pp.emit_LoadAddress_Indirect(regOffset(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::CastBool8:
            pp.emit_Load8_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B8);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastBool16:
            pp.emit_Load16_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B16);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastBool32:
            pp.emit_Load32_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B32);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastBool64:
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B64);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CastS8S16:
            pp.emit_LoadS8S16_Indirect(regOffset(ip->a.u32), RAX, RDI);
            pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CastS8S32:
            pp.emit_LoadS8S32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastS16S32:
            pp.emit_LoadS16S32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CastS8S64:
            pp.emit_LoadS8S64_Indirect(regOffset(ip->a.u32), RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastS16S64:
            pp.emit_LoadS16S64_Indirect(regOffset(ip->a.u32), RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastS32S64:
            pp.emit_LoadS32S64_Indirect(regOffset(ip->a.u32), RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CastF32S32:
            pp.emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0);
            concat.addString4("\xf3\x0f\x2c\xc0"); // cvttss2si eax, xmm0
            pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastF64S64:
            pp.emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0);
            concat.addString5("\xf2\x48\x0f\x2c\xc0"); // cvttss2si rax, xmm0
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CastS8F32:
            pp.emit_LoadS8S32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS16F32:
            pp.emit_LoadS16S32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS32F32:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS64F32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            concat.addString5("\xF3\x48\x0F\x2A\xC0"); // cvtsi2ss xmm0, rax
            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU8F32:
            pp.emit_LoadU8U32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU16F32:
            pp.emit_LoadU16U32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU32F32:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            concat.addString5("\xF3\x48\x0F\x2A\xC0"); // cvtsi2ss xmm0, rax => rax and not eax to avoid a signed conversion
            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU64F32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            concat.addString5("\xF3\x48\x0F\x2A\xC0"); // cvtsi2ss xmm0, rax
            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastF64F32:
            pp.emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0);
            concat.addString4("\xf2\x0f\x5a\xc0"); // cvtsd2ss xmm0, xmm0
            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;

        case ByteCodeOp::CastS8F64:
            pp.emit_LoadS8S32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS16F64:
            pp.emit_LoadS16S32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS32F64:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS64F64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            concat.addString5("\xf2\x48\x0f\x2a\xc0"); // cvtsi2sd xmm0, rax
            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU8F64:
            pp.emit_LoadU8U32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU16F64:
            pp.emit_LoadU16U32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU32F64:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            concat.addString4("\xF2\x0F\x2A\xC0"); // cvtsi2sd xmm0, eax
            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
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
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            concat.addString5("\x66\x48\x0F\x6E\xC8"); // movq xmm1, rax

            pp.emit_Symbol_RelocationAddr(RCX, pp.symCst_U64F64, 0);
            concat.addString4("\x66\x0F\x62\x09");     // punpckldq xmm1, xmmword ptr [rcx]
            concat.addString5("\x66\x0F\x5C\x49\x10"); // subpd xmm1, xmmword ptr [rcx + 16]
            concat.addString4("\x66\x0F\x28\xC1");     // movapd xmm0, xmm1
            concat.addString4("\x66\x0F\x15\xC1");     // unpckhpd xmm0, xmm1
            concat.addString4("\xF2\x0F\x58\xC1");     // addsd xmm0, xmm1

            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        }

        case ByteCodeOp::CastF32F64:
            pp.emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0);
            concat.addString4("\xf3\x0f\x5a\xc0"); // cvtss2sd xmm0, xmm0
            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
            break;

            /////////////////////////////////////

        case ByteCodeOp::BinOpShiftLeftS8:
        case ByteCodeOp::BinOpShiftLeftU8:
            emitShiftLogical(pp, ip, X64Bits::B8, X64Op::SHL);
            break;
        case ByteCodeOp::BinOpShiftLeftS16:
        case ByteCodeOp::BinOpShiftLeftU16:
            emitShiftLogical(pp, ip, X64Bits::B16, X64Op::SHL);
            break;
        case ByteCodeOp::BinOpShiftLeftS32:
        case ByteCodeOp::BinOpShiftLeftU32:
            emitShiftLogical(pp, ip, X64Bits::B32, X64Op::SHL);
            break;
        case ByteCodeOp::BinOpShiftLeftS64:
        case ByteCodeOp::BinOpShiftLeftU64:
            emitShiftLogical(pp, ip, X64Bits::B64, X64Op::SHL);
            break;

            /////////////////////////////////////

        case ByteCodeOp::BinOpShiftRightU8:
            emitShiftLogical(pp, ip, X64Bits::B8, X64Op::SHR);
            break;
        case ByteCodeOp::BinOpShiftRightU16:
            emitShiftLogical(pp, ip, X64Bits::B16, X64Op::SHR);
            break;
        case ByteCodeOp::BinOpShiftRightU32:
            emitShiftLogical(pp, ip, X64Bits::B32, X64Op::SHR);
            break;
        case ByteCodeOp::BinOpShiftRightU64:
            emitShiftLogical(pp, ip, X64Bits::B64, X64Op::SHR);
            break;

        case ByteCodeOp::BinOpShiftRightS8:
            emitShiftRightArithmetic(pp, ip, X64Bits::B8);
            break;
        case ByteCodeOp::BinOpShiftRightS16:
            emitShiftRightArithmetic(pp, ip, X64Bits::B16);
            break;
        case ByteCodeOp::BinOpShiftRightS32:
            emitShiftRightArithmetic(pp, ip, X64Bits::B32);
            break;
        case ByteCodeOp::BinOpShiftRightS64:
            emitShiftRightArithmetic(pp, ip, X64Bits::B64);
            break;

            /////////////////////////////////////

        case ByteCodeOp::BinOpXorU8:
            emitBinOpIntNAtReg(pp, ip, X64Op::XOR, X64Bits::B8);
            break;
        case ByteCodeOp::BinOpXorU16:
            emitBinOpIntNAtReg(pp, ip, X64Op::XOR, X64Bits::B16);
            break;
        case ByteCodeOp::BinOpXorU32:
            emitBinOpIntNAtReg(pp, ip, X64Op::XOR, X64Bits::B32);
            break;
        case ByteCodeOp::BinOpXorU64:
            emitBinOpIntNAtReg(pp, ip, X64Op::XOR, X64Bits::B64);
            break;

            /////////////////////////////////////

        case ByteCodeOp::BinOpMulS8:
        case ByteCodeOp::BinOpMulS8_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::IMUL, X64Bits::B8);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS8));
            break;
        case ByteCodeOp::BinOpMulS16:
        case ByteCodeOp::BinOpMulS16_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::IMUL, X64Bits::B16);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS16));
            break;
        case ByteCodeOp::BinOpMulS32:
        case ByteCodeOp::BinOpMulS32_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::IMUL, X64Bits::B32);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::BinOpMulS64:
        case ByteCodeOp::BinOpMulS64_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::IMUL, X64Bits::B64);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS64));
            break;
        case ByteCodeOp::BinOpMulU8:
        case ByteCodeOp::BinOpMulU8_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::MUL, X64Bits::B8);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU8));
            break;
        case ByteCodeOp::BinOpMulU16:
        case ByteCodeOp::BinOpMulU16_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::MUL, X64Bits::B16);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU16));
            break;
        case ByteCodeOp::BinOpMulU32:
        case ByteCodeOp::BinOpMulU32_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::MUL, X64Bits::B32);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU32));
            break;
        case ByteCodeOp::BinOpMulU64:
        case ByteCodeOp::BinOpMulU64_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::MUL, X64Bits::B64);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU64));
            break;
        case ByteCodeOp::BinOpMulF32:
            emitBinOpFloat32AtReg(pp, ip, X64Op::FMUL);
            break;
        case ByteCodeOp::BinOpMulF64:
            emitBinOpFloat64AtReg(pp, ip, X64Op::FMUL);
            break;

            /////////////////////////////////////

        case ByteCodeOp::BinOpModuloS8:
            emitBinOpDivIntNAtReg(pp, ip, true, X64Bits::B8, true);
            break;
        case ByteCodeOp::BinOpModuloS16:
            emitBinOpDivIntNAtReg(pp, ip, true, X64Bits::B16, true);
            break;
        case ByteCodeOp::BinOpModuloS32:
            emitBinOpDivIntNAtReg(pp, ip, true, X64Bits::B32, true);
            break;
        case ByteCodeOp::BinOpModuloS64:
            emitBinOpDivIntNAtReg(pp, ip, true, X64Bits::B64, true);
            break;
        case ByteCodeOp::BinOpModuloU8:
            emitBinOpDivIntNAtReg(pp, ip, false, X64Bits::B8, true);
            break;
        case ByteCodeOp::BinOpModuloU16:
            emitBinOpDivIntNAtReg(pp, ip, false, X64Bits::B16, true);
            break;
        case ByteCodeOp::BinOpModuloU32:
            emitBinOpDivIntNAtReg(pp, ip, false, X64Bits::B32, true);
            break;
        case ByteCodeOp::BinOpModuloU64:
            emitBinOpDivIntNAtReg(pp, ip, false, X64Bits::B64, true);
            break;

            /////////////////////////////////////

        case ByteCodeOp::BinOpDivS8:
            emitBinOpDivIntNAtReg(pp, ip, true, X64Bits::B8);
            break;
        case ByteCodeOp::BinOpDivS16:
            emitBinOpDivIntNAtReg(pp, ip, true, X64Bits::B16);
            break;
        case ByteCodeOp::BinOpDivS32:
            emitBinOpDivIntNAtReg(pp, ip, true, X64Bits::B32);
            break;
        case ByteCodeOp::BinOpDivS64:
            emitBinOpDivIntNAtReg(pp, ip, true, X64Bits::B64);
            break;
        case ByteCodeOp::BinOpDivU8:
            emitBinOpDivIntNAtReg(pp, ip, false, X64Bits::B8);
            break;
        case ByteCodeOp::BinOpDivU16:
            emitBinOpDivIntNAtReg(pp, ip, false, X64Bits::B16);
            break;
        case ByteCodeOp::BinOpDivU32:
            emitBinOpDivIntNAtReg(pp, ip, false, X64Bits::B32);
            break;
        case ByteCodeOp::BinOpDivU64:
            emitBinOpDivIntNAtReg(pp, ip, false, X64Bits::B64);
            break;
        case ByteCodeOp::BinOpDivF32:
            emitBinOpFloat32AtReg(pp, ip, X64Op::FDIV);
            break;
        case ByteCodeOp::BinOpDivF64:
            emitBinOpFloat64AtReg(pp, ip, X64Op::FDIV);
            break;

            /////////////////////////////////////

        case ByteCodeOp::BinOpPlusS8:
        case ByteCodeOp::BinOpPlusS8_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::ADD, X64Bits::B8);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS8));
            break;
        case ByteCodeOp::BinOpPlusS16:
        case ByteCodeOp::BinOpPlusS16_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::ADD, X64Bits::B16);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS16));
            break;
        case ByteCodeOp::BinOpPlusS32:
        case ByteCodeOp::BinOpPlusS32_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::ADD, X64Bits::B32);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::BinOpPlusS64:
        case ByteCodeOp::BinOpPlusS64_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::ADD, X64Bits::B64);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS64));
            break;
        case ByteCodeOp::BinOpPlusU8:
        case ByteCodeOp::BinOpPlusU8_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::ADD, X64Bits::B8);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU8));
            break;
        case ByteCodeOp::BinOpPlusU16:
        case ByteCodeOp::BinOpPlusU16_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::ADD, X64Bits::B16);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU16));
            break;
        case ByteCodeOp::BinOpPlusU32:
        case ByteCodeOp::BinOpPlusU32_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::ADD, X64Bits::B32);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU32));
            break;
        case ByteCodeOp::BinOpPlusU64:
        case ByteCodeOp::BinOpPlusU64_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::ADD, X64Bits::B64);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU64));
            break;
        case ByteCodeOp::BinOpPlusF32:
            emitBinOpFloat32AtReg(pp, ip, X64Op::FADD);
            break;
        case ByteCodeOp::BinOpPlusF64:
            emitBinOpFloat64AtReg(pp, ip, X64Op::FADD);
            break;

            /////////////////////////////////////

        case ByteCodeOp::BinOpMinusS8:
        case ByteCodeOp::BinOpMinusS8_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::SUB, X64Bits::B8);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS8));
            break;
        case ByteCodeOp::BinOpMinusS16:
        case ByteCodeOp::BinOpMinusS16_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::SUB, X64Bits::B16);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS16));
            break;
        case ByteCodeOp::BinOpMinusS32:
        case ByteCodeOp::BinOpMinusS32_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::SUB, X64Bits::B32);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::BinOpMinusS64:
        case ByteCodeOp::BinOpMinusS64_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::SUB, X64Bits::B64);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS64));
            break;
        case ByteCodeOp::BinOpMinusU8:
        case ByteCodeOp::BinOpMinusU8_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::SUB, X64Bits::B8);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU8));
            break;
        case ByteCodeOp::BinOpMinusU16:
        case ByteCodeOp::BinOpMinusU16_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::SUB, X64Bits::B16);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU16));
            break;
        case ByteCodeOp::BinOpMinusU32:
        case ByteCodeOp::BinOpMinusU32_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::SUB, X64Bits::B32);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU32));
            break;
        case ByteCodeOp::BinOpMinusU64:
        case ByteCodeOp::BinOpMinusU64_Safe:
            emitBinOpIntNAtReg(pp, ip, X64Op::SUB, X64Bits::B64);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU64));
            break;

        case ByteCodeOp::BinOpMinusF32:
            emitBinOpFloat32AtReg(pp, ip, X64Op::FSUB);
            break;
        case ByteCodeOp::BinOpMinusF64:
            emitBinOpFloat64AtReg(pp, ip, X64Op::FSUB);
            break;

            /////////////////////////////////////

        case ByteCodeOp::BinOpBitmaskAnd8:
            emitBinOpIntNAtReg(pp, ip, X64Op::AND, X64Bits::B8);
            break;
        case ByteCodeOp::BinOpBitmaskAnd16:
            emitBinOpIntNAtReg(pp, ip, X64Op::AND, X64Bits::B16);
            break;
        case ByteCodeOp::BinOpBitmaskAnd32:
            emitBinOpIntNAtReg(pp, ip, X64Op::AND, X64Bits::B32);
            break;
        case ByteCodeOp::BinOpBitmaskAnd64:
            emitBinOpIntNAtReg(pp, ip, X64Op::AND, X64Bits::B64);
            break;

            /////////////////////////////////////

        case ByteCodeOp::BinOpBitmaskOr8:
            emitBinOpIntNAtReg(pp, ip, X64Op::OR, X64Bits::B8);
            break;
        case ByteCodeOp::BinOpBitmaskOr16:
            emitBinOpIntNAtReg(pp, ip, X64Op::OR, X64Bits::B16);
            break;
        case ByteCodeOp::BinOpBitmaskOr32:
            emitBinOpIntNAtReg(pp, ip, X64Op::OR, X64Bits::B32);
            break;
        case ByteCodeOp::BinOpBitmaskOr64:
            emitBinOpIntNAtReg(pp, ip, X64Op::OR, X64Bits::B64);
            break;

            /////////////////////////////////////

        case ByteCodeOp::AffectOpShiftLeftEqS8:
        case ByteCodeOp::AffectOpShiftLeftEqU8:
            emitShiftEqLogical(pp, ip, X64Bits::B8, X64Op::SHLEQ);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS16:
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            emitShiftEqLogical(pp, ip, X64Bits::B16, X64Op::SHLEQ);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS32:
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            emitShiftEqLogical(pp, ip, X64Bits::B32, X64Op::SHLEQ);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS64:
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            emitShiftEqLogical(pp, ip, X64Bits::B64, X64Op::SHLEQ);
            break;

            /////////////////////////////////////

        case ByteCodeOp::AffectOpShiftRightEqU8:
            emitShiftEqLogical(pp, ip, X64Bits::B8, X64Op::SHREQ);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            emitShiftEqLogical(pp, ip, X64Bits::B16, X64Op::SHREQ);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            emitShiftEqLogical(pp, ip, X64Bits::B32, X64Op::SHREQ);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            emitShiftEqLogical(pp, ip, X64Bits::B64, X64Op::SHREQ);
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            emitShiftRightEqArithmetic(pp, ip, X64Bits::B8);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            emitShiftRightEqArithmetic(pp, ip, X64Bits::B16);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            emitShiftRightEqArithmetic(pp, ip, X64Bits::B32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            emitShiftRightEqArithmetic(pp, ip, X64Bits::B64);
            break;

            /////////////////////////////////////

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

            /////////////////////////////////////

        case ByteCodeOp::AffectOpOrEqU8:
            MK_BINOPEQ8_CAB(X64Op::OR);
            break;
        case ByteCodeOp::AffectOpOrEqU16:
            MK_BINOPEQ16_CAB(X64Op::OR);
            break;
        case ByteCodeOp::AffectOpOrEqU32:
            MK_BINOPEQ32_CAB(X64Op::OR);
            break;
        case ByteCodeOp::AffectOpOrEqU64:
            MK_BINOPEQ64_CAB(X64Op::OR);
            break;

            /////////////////////////////////////

        case ByteCodeOp::AffectOpAndEqU8:
            MK_BINOPEQ8_CAB(X64Op::AND);
            break;
        case ByteCodeOp::AffectOpAndEqU16:
            MK_BINOPEQ16_CAB(X64Op::AND);
            break;
        case ByteCodeOp::AffectOpAndEqU32:
            MK_BINOPEQ32_CAB(X64Op::AND);
            break;
        case ByteCodeOp::AffectOpAndEqU64:
            MK_BINOPEQ64_CAB(X64Op::AND);
            break;

            /////////////////////////////////////

        case ByteCodeOp::AffectOpMulEqS8:
        case ByteCodeOp::AffectOpMulEqS8_Safe:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load8_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B8);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS8));
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS8_SSafe:
            pp.emit_Load8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS8_SSSafe:
            pp.emit_Load8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqS16:
        case ByteCodeOp::AffectOpMulEqS16_Safe:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B16);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS16));
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS16_SSafe:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS16_SSSafe:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqS32:
        case ByteCodeOp::AffectOpMulEqS32_Safe:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B32);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS32));
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS32_SSafe:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS32_SSSafe:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqS64:
        case ByteCodeOp::AffectOpMulEqS64_Safe:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B64);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS64));
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS64_SSafe:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS64_SSSafe:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IMUL, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqU8:
        case ByteCodeOp::AffectOpMulEqU8_Safe:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load8_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B8);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU8));
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU8_SSafe:
            pp.emit_Load8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU8_SSSafe:
            pp.emit_Load8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqU16:
        case ByteCodeOp::AffectOpMulEqU16_Safe:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B16);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU16));
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU16_SSafe:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU16_SSSafe:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqU32:
        case ByteCodeOp::AffectOpMulEqU32_Safe:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B32);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU32));
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU32_SSafe:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU32_SSSafe:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqU64:
        case ByteCodeOp::AffectOpMulEqU64_Safe:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B64);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU64));
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU64_SSafe:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU64_SSSafe:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::MUL, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqF32:
            MK_IMMB_F32(XMM1);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadF32_Indirect(0, XMM0, RAX);
            pp.emit_OpF32(XMM0, XMM1, X64Op::MUL);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMulEqF32_S:
            pp.emit_LoadF32_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            MK_IMMB_F32(XMM1);
            pp.emit_OpF32(XMM0, XMM1, X64Op::MUL);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMulEqF32_SS:
            pp.emit_LoadF32_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            pp.emit_LoadF32_Indirect(offsetStack + ip->b.u32, XMM1, RDI);
            pp.emit_OpF32(XMM0, XMM1, X64Op::MUL);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;

        case ByteCodeOp::AffectOpMulEqF64:
            MK_IMMB_F64(XMM1);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadF64_Indirect(0, XMM0, RAX);
            pp.emit_OpF64(XMM0, XMM1, X64Op::MUL);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMulEqF64_S:
            pp.emit_LoadF64_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            MK_IMMB_F64(XMM1);
            pp.emit_OpF64(XMM0, XMM1, X64Op::MUL);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMulEqF64_SS:
            pp.emit_LoadF64_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            pp.emit_LoadF64_Indirect(offsetStack + ip->b.u32, XMM1, RDI);
            pp.emit_OpF64(XMM0, XMM1, X64Op::MUL);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::AffectOpDivEqS8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadS8S16_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B8);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS8_S:
            pp.emit_LoadS8S16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS8_SS:
            pp.emit_LoadS8S16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqS16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            pp.emit_Cwd();
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B16);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS16_S:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cwd();
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS16_SS:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cwd();
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqS32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            pp.emit_Cdq();
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B32);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS32_S:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cdq();
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS32_SS:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cdq();
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqS64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_Cqo();
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B64);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS64_S:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cqo();
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS64_SS:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cqo();
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqU8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadU8U32_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B8);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU8_S:
            pp.emit_LoadU8U32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU8_SS:
            pp.emit_LoadU8U32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqU16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, X64Bits::B16);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B16);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU16_S:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B16);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU16_SS:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B16);
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqU32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, X64Bits::B32);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B32);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU32_S:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B32);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU32_SS:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B32);
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqU64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, X64Bits::B64);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B64);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU64_S:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B64);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU64_SS:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B64);
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqF32:
            MK_IMMB_F32(XMM1);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadF32_Indirect(0, XMM0, RAX);
            pp.emit_OpF32(XMM0, XMM1, X64Op::DIV);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpDivEqF32_S:
            pp.emit_LoadF32_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            MK_IMMB_F32(XMM1);
            pp.emit_OpF32(XMM0, XMM1, X64Op::DIV);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpDivEqF32_SS:
            pp.emit_LoadF32_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            pp.emit_LoadF32_Indirect(offsetStack + ip->b.u32, XMM1, RDI);
            pp.emit_OpF32(XMM0, XMM1, X64Op::DIV);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;

        case ByteCodeOp::AffectOpDivEqF64:
            MK_IMMB_F64(XMM1);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadF64_Indirect(0, XMM0, RAX);
            pp.emit_OpF64(XMM0, XMM1, X64Op::DIV);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpDivEqF64_S:
            pp.emit_LoadF64_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            MK_IMMB_F64(XMM1);
            pp.emit_OpF64(XMM0, XMM1, X64Op::DIV);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpDivEqF64_SS:
            pp.emit_LoadF64_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            pp.emit_LoadF64_Indirect(offsetStack + ip->b.u32, XMM1, RDI);
            pp.emit_OpF64(XMM0, XMM1, X64Op::DIV);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::AffectOpModuloEqS8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadS8S16_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B8);
            concat.addString2("\x88\xe0"); // mov al, ah
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS8_S:
            pp.emit_LoadS8S16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B8);
            concat.addString2("\x88\xe0"); // mov al, ah
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS8_SS:
            pp.emit_LoadS8S16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B8);
            concat.addString2("\x88\xe0"); // mov al, ah
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqS16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            pp.emit_Cwd();
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B16);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS16_S:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cwd();
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS16_SS:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cwd();
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqS32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            pp.emit_Cdq();
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B32);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS32_S:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cdq();
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS32_SS:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cdq();
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqS64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_Cqo();
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B64);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS64_S:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cqo();
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS64_SS:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cqo();
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::IDIV, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqU8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadS8S16_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B8);
            concat.addString2("\x88\xe0"); // mov al, ah
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU8_S:
            pp.emit_LoadU8U32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B8);
            concat.addString2("\x88\xe0"); // mov al, ah
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU8_SS:
            pp.emit_LoadU8U32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B8);
            concat.addString2("\x88\xe0"); // mov al, ah
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqU16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, X64Bits::B16);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B16);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU16_S:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B16);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU16_SS:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B16);
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqU32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, X64Bits::B32);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B32);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU32_S:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B32);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU32_SS:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B32);
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqU64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, X64Bits::B64);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B64);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU64_S:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B64);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU64_SS:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, X64Bits::B64);
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, X64Op::DIV, X64Bits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::AffectOpPlusEqS8:
        case ByteCodeOp::AffectOpPlusEqS8_Safe:
            MK_BINOPEQ8_CAB(X64Op::ADD);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS8));
            break;
        case ByteCodeOp::AffectOpPlusEqS8_SSafe:
        case ByteCodeOp::AffectOpPlusEqU8_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_IncN_Indirect(offsetStack + ip->a.u32, RDI, X64Bits::B8);
            else
                MK_BINOPEQ8_SCAB(X64Op::ADD);
            break;
        case ByteCodeOp::AffectOpPlusEqS8_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU8_SSSafe:
            MK_BINOPEQ8_SSCAB(X64Op::ADD);
            break;

        case ByteCodeOp::AffectOpPlusEqS16:
        case ByteCodeOp::AffectOpPlusEqS16_Safe:
            MK_BINOPEQ16_CAB(X64Op::ADD);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS16));
            break;
        case ByteCodeOp::AffectOpPlusEqS16_SSafe:
        case ByteCodeOp::AffectOpPlusEqU16_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_IncN_Indirect(offsetStack + ip->a.u32, RDI, X64Bits::B16);
            else
                MK_BINOPEQ16_SCAB(X64Op::ADD);
            break;
        case ByteCodeOp::AffectOpPlusEqS16_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU16_SSSafe:
            MK_BINOPEQ16_SSCAB(X64Op::ADD);
            break;

        case ByteCodeOp::AffectOpPlusEqS32:
        case ByteCodeOp::AffectOpPlusEqS32_Safe:
            MK_BINOPEQ32_CAB(X64Op::ADD);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::AffectOpPlusEqS32_SSafe:
        case ByteCodeOp::AffectOpPlusEqU32_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_IncN_Indirect(offsetStack + ip->a.u32, RDI, X64Bits::B32);
            else
                MK_BINOPEQ32_SCAB(X64Op::ADD);
            break;
        case ByteCodeOp::AffectOpPlusEqS32_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU32_SSSafe:
            MK_BINOPEQ32_SSCAB(X64Op::ADD);
            break;

        case ByteCodeOp::AffectOpPlusEqS64:
        case ByteCodeOp::AffectOpPlusEqS64_Safe:
            MK_BINOPEQ64_CAB(X64Op::ADD);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS64));
            break;
        case ByteCodeOp::AffectOpPlusEqS64_SSafe:
        case ByteCodeOp::AffectOpPlusEqU64_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_IncN_Indirect(offsetStack + ip->a.u32, RDI, X64Bits::B64);
            else
                MK_BINOPEQ64_SCAB(X64Op::ADD);
            break;
        case ByteCodeOp::AffectOpPlusEqS64_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU64_SSSafe:
            MK_BINOPEQ64_SSCAB(X64Op::ADD);
            break;

        case ByteCodeOp::AffectOpPlusEqU8:
        case ByteCodeOp::AffectOpPlusEqU8_Safe:
            MK_BINOPEQ8_CAB(X64Op::ADD);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU8));
            break;

        case ByteCodeOp::AffectOpPlusEqU16:
        case ByteCodeOp::AffectOpPlusEqU16_Safe:
            MK_BINOPEQ16_CAB(X64Op::ADD);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU16));
            break;

        case ByteCodeOp::AffectOpPlusEqU32:
        case ByteCodeOp::AffectOpPlusEqU32_Safe:
            MK_BINOPEQ32_CAB(X64Op::ADD);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU32));
            break;

        case ByteCodeOp::AffectOpPlusEqU64:
        case ByteCodeOp::AffectOpPlusEqU64_Safe:
            MK_BINOPEQ64_CAB(X64Op::ADD);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU64));
            break;

        case ByteCodeOp::AffectOpPlusEqF32:
            MK_BINOPEQF32_CAB(X64Op::FADD);
            break;
        case ByteCodeOp::AffectOpPlusEqF32_S:
            MK_BINOPEQF32_SCAB(X64Op::FADD);
            break;
        case ByteCodeOp::AffectOpPlusEqF32_SS:
            MK_BINOPEQF32_SSCAB(X64Op::FADD);
            break;

        case ByteCodeOp::AffectOpPlusEqF64:
            MK_BINOPEQF64_CAB(X64Op::FADD);
            break;
        case ByteCodeOp::AffectOpPlusEqF64_S:
            MK_BINOPEQF64_SCAB(X64Op::FADD);
            break;
        case ByteCodeOp::AffectOpPlusEqF64_SS:
            MK_BINOPEQF64_SSCAB(X64Op::FADD);
            break;

            /////////////////////////////////////

        case ByteCodeOp::AffectOpMinusEqS8:
        case ByteCodeOp::AffectOpMinusEqS8_Safe:
            MK_BINOPEQ8_CAB(X64Op::SUB);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS8));
            break;
        case ByteCodeOp::AffectOpMinusEqS8_SSafe:
        case ByteCodeOp::AffectOpMinusEqU8_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_DecN_Indirect(offsetStack + ip->a.u32, RDI, X64Bits::B8);
            else
                MK_BINOPEQ8_SCAB(X64Op::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqS8_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU8_SSSafe:
            MK_BINOPEQ8_SSCAB(X64Op::SUB);
            break;

        case ByteCodeOp::AffectOpMinusEqS16:
        case ByteCodeOp::AffectOpMinusEqS16_Safe:
            MK_BINOPEQ16_CAB(X64Op::SUB);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16));
            break;
        case ByteCodeOp::AffectOpMinusEqS16_SSafe:
        case ByteCodeOp::AffectOpMinusEqU16_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_DecN_Indirect(offsetStack + ip->a.u32, RDI, X64Bits::B16);
            else
                MK_BINOPEQ16_SCAB(X64Op::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqS16_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU16_SSSafe:
            MK_BINOPEQ16_SSCAB(X64Op::SUB);
            break;

        case ByteCodeOp::AffectOpMinusEqS32:
        case ByteCodeOp::AffectOpMinusEqS32_Safe:
            MK_BINOPEQ32_CAB(X64Op::SUB);
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::AffectOpMinusEqS32_SSafe:
        case ByteCodeOp::AffectOpMinusEqU32_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_DecN_Indirect(offsetStack + ip->a.u32, RDI, X64Bits::B32);
            else
                MK_BINOPEQ32_SCAB(X64Op::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqS32_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU32_SSSafe:
            MK_BINOPEQ32_SSCAB(X64Op::SUB);
            break;

        case ByteCodeOp::AffectOpMinusEqS64:
        case ByteCodeOp::AffectOpMinusEqS64_Safe:
            emitOverflowSigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS64));
            MK_BINOPEQ64_CAB(X64Op::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqS64_SSafe:
        case ByteCodeOp::AffectOpMinusEqU64_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_DecN_Indirect(offsetStack + ip->a.u32, RDI, X64Bits::B64);
            else
                MK_BINOPEQ64_SCAB(X64Op::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqS64_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU64_SSSafe:
            MK_BINOPEQ64_SSCAB(X64Op::SUB);
            break;

        case ByteCodeOp::AffectOpMinusEqU8:
        case ByteCodeOp::AffectOpMinusEqU8_Safe:
            MK_BINOPEQ8_CAB(X64Op::SUB);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU8));
            break;

        case ByteCodeOp::AffectOpMinusEqU16:
        case ByteCodeOp::AffectOpMinusEqU16_Safe:
            MK_BINOPEQ16_CAB(X64Op::SUB);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16));
            break;

        case ByteCodeOp::AffectOpMinusEqU32:
        case ByteCodeOp::AffectOpMinusEqU32_Safe:
            MK_BINOPEQ32_CAB(X64Op::SUB);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU32));
            break;

        case ByteCodeOp::AffectOpMinusEqU64:
        case ByteCodeOp::AffectOpMinusEqU64_Safe:
            MK_BINOPEQ64_CAB(X64Op::SUB);
            emitOverflowUnsigned(pp, ip->node, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU64));
            break;

        case ByteCodeOp::AffectOpMinusEqF32:
            MK_BINOPEQF32_CAB(X64Op::FSUB);
            break;
        case ByteCodeOp::AffectOpMinusEqF32_S:
            MK_BINOPEQF32_SCAB(X64Op::FSUB);
            break;
        case ByteCodeOp::AffectOpMinusEqF32_SS:
            MK_BINOPEQF32_SSCAB(X64Op::FSUB);
            break;

        case ByteCodeOp::AffectOpMinusEqF64:
            MK_BINOPEQF64_CAB(X64Op::FSUB);
            break;
        case ByteCodeOp::AffectOpMinusEqF64_S:
            MK_BINOPEQF64_SCAB(X64Op::FSUB);
            break;
        case ByteCodeOp::AffectOpMinusEqF64_SS:
            MK_BINOPEQF64_SSCAB(X64Op::FSUB);
            break;

            /////////////////////////////////////

        case ByteCodeOp::ZeroToTrue:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B32);
            pp.emit_SetE();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::LowerZeroToTrue:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_ShiftN(RAX, 31, X64Bits::B32, X64Op::SHR);
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::LowerEqZeroToTrue:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B32);
            pp.emit_SetLE();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GreaterZeroToTrue:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B32);
            pp.emit_SetG();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GreaterEqZeroToTrue:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_NotN(RAX, X64Bits::B32);
            pp.emit_ShiftN(RAX, 31, X64Bits::B32, X64Op::SHR);
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::CompareOpGreaterS8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterS16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterS32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterU8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterU16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterU32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;

        case ByteCodeOp::CompareOpGreaterEqS8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetGE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqS16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetGE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqS32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetGE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqS64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetGE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqU8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqU16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqU32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqU64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::CompareOpLowerS8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetL();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerS16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetL();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerS32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetL();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerS64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetL();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerU8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerU16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerU32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerU64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;

        case ByteCodeOp::CompareOpLowerEqS8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetLE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqS16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetLE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqS32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetLE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqS64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetLE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqU8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqU16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqU32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqU64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::CompareOp3Way8:
            emitBinOpIntN(pp, ip, X64Op::SUB, X64Bits::B8);
            pp.emit_ClearN(RCX, X64Bits::B8);
            pp.emit_TestN(RAX, RAX, X64Bits::B8);
            pp.concat.addString3("\x0F\x9F\xC1"); // setg cl
            pp.emit_ShiftN(RAX, 7, X64Bits::B8, X64Op::SHR);
            pp.emit_Extend_U8U64(RAX, RAX);
            pp.emit_OpN(RAX, RCX, X64Op::SUB, X64Bits::B32);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RCX);
            break;
        case ByteCodeOp::CompareOp3Way16:
            emitBinOpIntN(pp, ip, X64Op::SUB, X64Bits::B16);
            pp.emit_ClearN(RCX, X64Bits::B16);
            pp.emit_TestN(RAX, RAX, X64Bits::B16);
            pp.concat.addString3("\x0F\x9F\xC1"); // setg cl
            pp.emit_ShiftN(RAX, 15, X64Bits::B16, X64Op::SHR);
            pp.emit_Extend_U8U64(RAX, RAX);
            pp.emit_OpN(RAX, RCX, X64Op::SUB, X64Bits::B32);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RCX);
            break;
        case ByteCodeOp::CompareOp3Way32:
            emitBinOpIntN(pp, ip, X64Op::SUB, X64Bits::B32);
            pp.emit_ClearN(RCX, X64Bits::B32);
            pp.emit_TestN(RAX, RAX, X64Bits::B32);
            pp.concat.addString3("\x0F\x9F\xC1"); // setg cl
            pp.emit_ShiftN(RAX, 31, X64Bits::B32, X64Op::SHR);
            pp.emit_OpN(RAX, RCX, X64Op::SUB, X64Bits::B32);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RCX);
            break;
        case ByteCodeOp::CompareOp3Way64:
            emitBinOpIntN(pp, ip, X64Op::SUB, X64Bits::B64);
            pp.emit_ClearN(RCX, X64Bits::B64);
            pp.emit_TestN(RAX, RAX, X64Bits::B64);
            pp.concat.addString3("\x0F\x9F\xC1"); // setg cl
            pp.emit_ShiftN(RAX, 63, X64Bits::B64, X64Op::SHR);
            pp.emit_OpN(RAX, RCX, X64Op::SUB, X64Bits::B32);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RCX);
            break;
        case ByteCodeOp::CompareOp3WayF32:
            emitBinOpFloat32(pp, ip, X64Op::FSUB);
            pp.emit_ClearN(RAX, X64Bits::B32);
            pp.emit_ClearN(RCX, X64Bits::B32);
            pp.concat.addString3("\x0F\x57\xC9"); // xorps xmm1, xmm1
            pp.concat.addString3("\x0F\x2F\xC1"); // comiss xmm0, xmm1
            pp.emit_SetA();
            pp.concat.addString3("\x0F\x2F\xC8"); // comiss xmm1, xmm0
            pp.emit_SetA(RCX);
            pp.emit_OpN(RCX, RAX, X64Op::SUB, X64Bits::B32);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOp3WayF64:
            emitBinOpFloat64(pp, ip, X64Op::FSUB);
            pp.emit_ClearN(RAX, X64Bits::B32);
            pp.emit_ClearN(RCX, X64Bits::B32);
            pp.concat.addString3("\x0F\x57\xC9");     // xorps xmm1, xmm1
            pp.concat.addString4("\x66\x0F\x2F\xC1"); // comisd xmm0, xmm1
            pp.emit_SetA();
            pp.concat.addString4("\x66\x0F\x2F\xC8"); // comisd xmm1, xmm0
            pp.emit_SetA(RCX);
            pp.emit_OpN(RCX, RAX, X64Op::SUB, X64Bits::B32);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::CompareOpEqual8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpEqual16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpEqual32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpEqual64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpEqualF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetEP();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpEqualF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetEP();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::CompareOpNotEqual8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpNotEqual16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpNotEqual32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpNotEqual64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpNotEqualF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetNEP();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpNotEqualF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetNEP();
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::IntrinsicDbgAlloc:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_atdbgalloc, {}, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicSysAlloc:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_atsysalloc, {}, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicRtFlags:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_atrtflags, {}, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicStringCmp:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_atstrcmp, {ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32}, regOffset(ip->d.u32));
            break;
        case ByteCodeOp::IntrinsicTypeCmp:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_attypecmp, {ip->a.u32, ip->b.u32, ip->c.u32}, regOffset(ip->d.u32));
            break;

            /////////////////////////////////////

        case ByteCodeOp::TestNotZero8:
            MK_IMMB_8(RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B8);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::TestNotZero16:
            MK_IMMB_16(RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B16);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::TestNotZero32:
            MK_IMMB_32(RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B32);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::TestNotZero64:
            MK_IMMB_64(RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B64);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::NegBool:
            pp.emit_Load8_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_OpN_Immediate(RAX, 1, X64Op::XOR, X64Bits::B8);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::NegS32:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NegN_Indirect(regOffset(ip->a.u32), RDI, X64Bits::B32);
            else
            {
                pp.emit_Load32_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_NegN(RAX, X64Bits::B64);
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
            }
            break;
        case ByteCodeOp::NegS64:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NegN_Indirect(regOffset(ip->a.u32), RDI, X64Bits::B64);
            else
            {
                pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_NegN(RAX, X64Bits::B64);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            }
            break;
        case ByteCodeOp::NegF32:
            pp.emit_LoadF32_Indirect(regOffset(ip->b.u32), XMM0);
            pp.emit_Load64_Immediate(RAX, 0x80000000);
            pp.emit_Store64_Indirect(offsetFLT, RAX, RDI);
            pp.emit_LoadF32_Indirect(offsetFLT, XMM1, RDI);
            concat.addString3("\x0f\x57\xc1"); // xorps xmm0, xmm1
            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::NegF64:
            pp.emit_LoadF64_Indirect(regOffset(ip->b.u32), XMM0);
            pp.emit_Load64_Immediate(RAX, 0x80000000'00000000);
            pp.emit_Store64_Indirect(offsetFLT, RAX, RDI);
            pp.emit_LoadF64_Indirect(offsetFLT, XMM1, RDI);
            concat.addString4("\x66\x0f\x57\xc1"); // xorpd xmm0, xmm1
            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
            break;

            /////////////////////////////////////

        case ByteCodeOp::InvertU8:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NotN_Indirect(regOffset(ip->a.u32), RDI, X64Bits::B8);
            else
            {
                pp.emit_Load8_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_NotN(RAX, X64Bits::B8);
                pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
            }
            break;
        case ByteCodeOp::InvertU16:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NotN_Indirect(regOffset(ip->a.u32), RDI, X64Bits::B16);
            else
            {
                pp.emit_Load16_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_NotN(RAX, X64Bits::B16);
                pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
            }
            break;
        case ByteCodeOp::InvertU32:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NotN_Indirect(regOffset(ip->a.u32), RDI, X64Bits::B32);
            else
            {
                pp.emit_Load32_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_NotN(RAX, X64Bits::B32);
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
            }
            break;
        case ByteCodeOp::InvertU64:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NotN_Indirect(regOffset(ip->a.u32), RDI, X64Bits::B64);
            else
            {
                pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_NotN(RAX, X64Bits::B64);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            }
            break;

            /////////////////////////////////////

        case ByteCodeOp::JumpDyn8:
        case ByteCodeOp::JumpDyn16:
        case ByteCodeOp::JumpDyn32:
        case ByteCodeOp::JumpDyn64:
        {
            int32_t* tableCompiler = (int32_t*) moduleToGen->compilerSegment.address(ip->d.u32);

            if (ip->op == ByteCodeOp::JumpDyn8)
                pp.emit_LoadS8S64_Indirect(regOffset(ip->a.u32), RAX, RDI);
            else if (ip->op == ByteCodeOp::JumpDyn16)
                pp.emit_LoadS16S64_Indirect(regOffset(ip->a.u32), RAX, RDI);
            else if (ip->op == ByteCodeOp::JumpDyn32)
                pp.emit_LoadS32S64_Indirect(regOffset(ip->a.u32), RAX, RDI);
            else
                pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);

            // Note:
            //
            // This is not optimal yet.
            // The sub could be removed by baking it in the 'cmp', and by changing the jump table address by substracting the min value
            // Also, if the jumptable was encoded in the text segment, then there will be no need to have two relocations
            //
            // We could in the end remove two instructions and be as the llvm generation

            pp.emit_OpN_Immediate(RAX, ip->b.u64 - 1, X64Op::SUB, X64Bits::B64);
            pp.emit_CmpN_Immediate(RAX, ip->c.u64, X64Bits::B64);
            pp.emit_Jump(JAE, i, tableCompiler[0]);

            uint8_t* addrConstant        = nullptr;
            auto     offsetTableConstant = moduleToGen->constantSegment.reserve(((uint32_t) ip->c.u64) * sizeof(uint32_t), &addrConstant);

            pp.emit_Symbol_RelocationAddr(RCX, pp.symCSIndex, offsetTableConstant); // rcx = jump table
            concat.addString4("\x48\x63\x0C\x81");                                  // movsx rcx, dword ptr [rcx + rax*4]

            // + 5 for the two following instructions
            // + 7 for this instruction
            pp.emit_Symbol_RelocationAddr(RAX, symbolFuncIndex, concat.totalCount() - startAddress + 5 + 7);
            concat.addString3("\x48\x01\xC8"); // add rax, rcx
            concat.addString2("\xFF\xE0");     // jmp rax

            auto currentOffset = (int32_t) pp.concat.totalCount();

            int32_t*     tableConstant = (int32_t*) addrConstant;
            LabelToSolve label;
            for (uint32_t idx = 0; idx < ip->c.u32; idx++)
            {
                label.ipDest        = tableCompiler[idx] + i + 1;
                label.currentOffset = currentOffset;
                label.patch         = (uint8_t*) (tableConstant + idx);
                pp.labelsToSolve.push_back(label);
            }

            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::JumpIfTrue:
            pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B8);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfFalse:
            pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B8);
            pp.emit_Jump(JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero8:
            pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B8);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero16:
            pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B16);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero32:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B32);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B64);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero8:
            pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B8);
            pp.emit_Jump(JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero16:
            pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B16);
            pp.emit_Jump(JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero32:
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B32);
            pp.emit_Jump(JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, X64Bits::B64);
            pp.emit_Jump(JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::Jump:
            pp.emit_Jump(JUMP, i, ip->b.s32);
            break;

            /////////////////////////////////////

        case ByteCodeOp::JumpIfNotEqual8:
            MK_JMPCMP_8(JNZ);
            break;
        case ByteCodeOp::JumpIfNotEqual16:
            MK_JMPCMP_16(JNZ);
            break;
        case ByteCodeOp::JumpIfNotEqual32:
            MK_JMPCMP_32(JNZ);
            break;
        case ByteCodeOp::JumpIfNotEqual64:
            MK_JMPCMP_64(JNZ);
            break;
        case ByteCodeOp::JumpIfNotEqualF32:
            MK_JMPCMP2_F32(JP, JNZ);
            break;
        case ByteCodeOp::JumpIfNotEqualF64:
            MK_JMPCMP2_F64(JP, JNZ);
            break;

            /////////////////////////////////////

        case ByteCodeOp::JumpIfEqual8:
            MK_JMPCMP_8(JZ);
            break;
        case ByteCodeOp::JumpIfEqual16:
            MK_JMPCMP_16(JZ);
            break;
        case ByteCodeOp::JumpIfEqual32:
            MK_JMPCMP_32(JZ);
            break;
        case ByteCodeOp::JumpIfEqual64:
            MK_JMPCMP_64(JZ);
            break;
        case ByteCodeOp::JumpIfEqualF32:
            MK_JMPCMP3_F32(JP, JZ);
            break;
        case ByteCodeOp::JumpIfEqualF64:
            MK_JMPCMP3_F64(JP, JZ);
            break;
        case ByteCodeOp::IncJumpIfEqual64:
            pp.emit_IncN_Indirect(regOffset(ip->a.u32), RDI, X64Bits::B64);
            MK_JMPCMP_64(JZ);
            break;

            /////////////////////////////////////

        case ByteCodeOp::JumpIfLowerS8:
            MK_JMPCMP_8(JL);
            break;
        case ByteCodeOp::JumpIfLowerS16:
            MK_JMPCMP_16(JL);
            break;
        case ByteCodeOp::JumpIfLowerS32:
            MK_JMPCMP_32(JL);
            break;
        case ByteCodeOp::JumpIfLowerS64:
            MK_JMPCMP_64(JL);
            break;
        case ByteCodeOp::JumpIfLowerU8:
            MK_JMPCMP_8(JB);
            break;
        case ByteCodeOp::JumpIfLowerU16:
            MK_JMPCMP_16(JB);
            break;
        case ByteCodeOp::JumpIfLowerU32:
            MK_JMPCMP_32(JB);
            break;
        case ByteCodeOp::JumpIfLowerU64:
            MK_JMPCMP_64(JB);
            break;
        case ByteCodeOp::JumpIfLowerF32:
            MK_JMPCMP_F32(JB);
            break;
        case ByteCodeOp::JumpIfLowerF64:
            MK_JMPCMP_F64(JB);
            break;

            /////////////////////////////////////

        case ByteCodeOp::JumpIfLowerEqS8:
            MK_JMPCMP_8(JLE);
            break;
        case ByteCodeOp::JumpIfLowerEqS16:
            MK_JMPCMP_16(JLE);
            break;
        case ByteCodeOp::JumpIfLowerEqS32:
            MK_JMPCMP_32(JLE);
            break;
        case ByteCodeOp::JumpIfLowerEqS64:
            MK_JMPCMP_64(JLE);
            break;
        case ByteCodeOp::JumpIfLowerEqU8:
            MK_JMPCMP_8(JBE);
            break;
        case ByteCodeOp::JumpIfLowerEqU16:
            MK_JMPCMP_16(JBE);
            break;
        case ByteCodeOp::JumpIfLowerEqU32:
            MK_JMPCMP_32(JBE);
            break;
        case ByteCodeOp::JumpIfLowerEqU64:
            MK_JMPCMP_64(JBE);
            break;
        case ByteCodeOp::JumpIfLowerEqF32:
            MK_JMPCMP_F32(JBE);
            break;
        case ByteCodeOp::JumpIfLowerEqF64:
            MK_JMPCMP_F64(JBE);
            break;

            /////////////////////////////////////

        case ByteCodeOp::JumpIfGreaterS8:
            MK_JMPCMP_8(JG);
            break;
        case ByteCodeOp::JumpIfGreaterS16:
            MK_JMPCMP_16(JG);
            break;
        case ByteCodeOp::JumpIfGreaterS32:
            MK_JMPCMP_32(JG);
            break;
        case ByteCodeOp::JumpIfGreaterS64:
            MK_JMPCMP_64(JG);
            break;
        case ByteCodeOp::JumpIfGreaterU8:
            MK_JMPCMP_8(JA);
            break;
        case ByteCodeOp::JumpIfGreaterU16:
            MK_JMPCMP_16(JA);
            break;
        case ByteCodeOp::JumpIfGreaterU32:
            MK_JMPCMP_32(JA);
            break;
        case ByteCodeOp::JumpIfGreaterU64:
            MK_JMPCMP_64(JA);
            break;
        case ByteCodeOp::JumpIfGreaterF32:
            MK_JMPCMP_F32(JA);
            break;
        case ByteCodeOp::JumpIfGreaterF64:
            MK_JMPCMP_F64(JA);
            break;

            /////////////////////////////////////

        case ByteCodeOp::JumpIfGreaterEqS8:
            MK_JMPCMP_8(JGE);
            break;
        case ByteCodeOp::JumpIfGreaterEqS16:
            MK_JMPCMP_16(JGE);
            break;
        case ByteCodeOp::JumpIfGreaterEqS32:
            MK_JMPCMP_32(JGE);
            break;
        case ByteCodeOp::JumpIfGreaterEqS64:
            MK_JMPCMP_64(JGE);
            break;
        case ByteCodeOp::JumpIfGreaterEqU8:
            MK_JMPCMP_8(JAE);
            break;
        case ByteCodeOp::JumpIfGreaterEqU16:
            MK_JMPCMP_16(JAE);
            break;
        case ByteCodeOp::JumpIfGreaterEqU32:
            MK_JMPCMP_32(JAE);
            break;
        case ByteCodeOp::JumpIfGreaterEqU64:
            MK_JMPCMP_64(JAE);
            break;
        case ByteCodeOp::JumpIfGreaterEqF32:
            MK_JMPCMP_F32(JAE);
            break;
        case ByteCodeOp::JumpIfGreaterEqF64:
            MK_JMPCMP_F64(JAE);
            break;

            /////////////////////////////////////

        case ByteCodeOp::DecrementRA32:
            pp.emit_DecN_Indirect(regOffset(ip->a.u32), RDI, X64Bits::B32);
            break;
        case ByteCodeOp::IncrementRA64:
            pp.emit_IncN_Indirect(regOffset(ip->a.u32), RDI, X64Bits::B64);
            break;
        case ByteCodeOp::DecrementRA64:
            pp.emit_DecN_Indirect(regOffset(ip->a.u32), RDI, X64Bits::B64);
            break;

            /////////////////////////////////////

        case ByteCodeOp::DeRef8:
            SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_LoadU8U64_Indirect(ip->c.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::DeRef16:
            SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_LoadU16U64_Indirect(ip->c.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::DeRef32:
            SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_Load32_Indirect(ip->c.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::DeRef64:
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
            if (ip->c.u64 <= 0x7FFFFFFF)
                pp.emit_Load64_Indirect(ip->c.u32, RAX, RAX);
            else
            {
                pp.emit_Load64_Immediate(RCX, ip->c.u64);
                pp.emit_OpN(RCX, RAX, X64Op::ADD, X64Bits::B64);
                pp.emit_Load64_Indirect(0, RAX, RAX);
            }
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::DeRefStringSlice:
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFFF);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(ip->c.u32 + 8, RCX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->b.u32), RCX);
            pp.emit_Load64_Indirect(ip->c.u32 + 0, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::GetFromBssSeg8:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symBSIndex, 0);
            pp.emit_LoadU8U64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromBssSeg16:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symBSIndex, 0);
            pp.emit_LoadU16U64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromBssSeg32:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symBSIndex, 0);
            pp.emit_Load32_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromBssSeg64:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symBSIndex, 0);
            pp.emit_Load64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::GetFromMutableSeg8:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, 0);
            pp.emit_LoadU8U64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromMutableSeg16:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, 0);
            pp.emit_LoadU16U64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromMutableSeg32:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, 0);
            pp.emit_Load32_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromMutableSeg64:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, 0);
            pp.emit_Load64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::GetFromStack8:
            pp.emit_LoadU8U64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack16:
            pp.emit_LoadU16U64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack32:
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack64:
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetIncFromStack64:
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_OpN_Immediate(RAX, ip->c.u64, X64Op::ADD, X64Bits::B64);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::GetFromStack8x2:
            pp.emit_LoadU8U64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadU8U64_Indirect(offsetStack + ip->d.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack16x2:
            pp.emit_LoadU16U64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadU16U64_Indirect(offsetStack + ip->d.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack32x2:
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(offsetStack + ip->d.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack64x2:
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(offsetStack + ip->d.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::GetIncFromStack64DeRef8:
            if (ip->c.u64 <= 0x7FFFFFFF)
            {
                pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emit_LoadU8U64_Indirect((uint32_t) ip->c.u64, RAX, RAX);
            }
            else
            {
                pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emit_OpN_Immediate(RAX, ip->c.u64, X64Op::ADD, X64Bits::B64);
                pp.emit_LoadU8U64_Indirect(0, RAX, RAX);
            }
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetIncFromStack64DeRef16:
            if (ip->c.u64 <= 0x7FFFFFFF)
            {
                pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emit_LoadU16U64_Indirect((uint32_t) ip->c.u64, RAX, RAX);
            }
            else
            {
                pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emit_OpN_Immediate(RAX, ip->c.u64, X64Op::ADD, X64Bits::B64);
                pp.emit_LoadU16U64_Indirect(0, RAX, RAX);
            }
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetIncFromStack64DeRef32:
            if (ip->c.u64 <= 0x7FFFFFFF)
            {
                pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emit_Load32_Indirect((uint32_t) ip->c.u64, RAX, RAX);
            }
            else
            {
                pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emit_OpN_Immediate(RAX, ip->c.u64, X64Op::ADD, X64Bits::B64);
                pp.emit_Load32_Indirect(0, RAX, RAX);
            }
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetIncFromStack64DeRef64:
            if (ip->c.u64 <= 0x7FFFFFFF)
            {
                pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emit_Load64_Indirect((uint32_t) ip->c.u64, RAX, RAX);
            }
            else
            {
                pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emit_OpN_Immediate(RAX, ip->c.u64, X64Op::ADD, X64Bits::B64);
                pp.emit_Load64_Indirect(0, RAX, RAX);
            }
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::CopyStack8:
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            break;
        case ByteCodeOp::CopyStack16:
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            break;
        case ByteCodeOp::CopyStack32:
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            break;
        case ByteCodeOp::CopyStack64:
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            break;

            /////////////////////////////////////

        case ByteCodeOp::ClearMaskU32:
            if (ip->b.u32 == 0xFF)
                pp.emit_LoadU8U32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            else if (ip->b.u32 == 0xFFFF)
                pp.emit_LoadU16U32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            else
                SWAG_ASSERT(false);
            pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::ClearMaskU64:
            if (ip->b.u32 == 0xFF)
                pp.emit_LoadU8U64_Indirect(regOffset(ip->a.u32), RAX, RDI);
            else if (ip->b.u32 == 0xFFFF)
                pp.emit_LoadU16U64_Indirect(regOffset(ip->a.u32), RAX, RDI);
            else if (ip->b.u32 == 0xFFFFFFFF)
                pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
            else
                SWAG_ASSERT(false);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::SetZeroAtPointer8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Store8_Immediate(ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Store16_Immediate(ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Store32_Immediate(ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Store64_Immediate(ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointerX:
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
            if (ip->b.u32 <= 128 && !buildParameters.isDebug())
            {
                pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
                pp.emit_ClearX(ip->b.u32, ip->c.u32, RAX);
            }
            else
            {
                pp.pushParams.clear();
                pp.pushParams.push_back({X64PushParamType::RegAdd, ip->a.u32, ip->c.u64});
                pp.pushParams.push_back({X64PushParamType::Imm, 0});
                pp.pushParams.push_back({X64PushParamType::Imm, ip->b.u64});
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_memset, pp.pushParams);
            }
            break;
        case ByteCodeOp::SetZeroAtPointerXRB:
            pp.pushParams.clear();
            pp.pushParams.push_back({X64PushParamType::Reg, ip->a.u32});
            pp.pushParams.push_back({X64PushParamType::Imm, 0});
            pp.pushParams.push_back({X64PushParamType::RegMul, ip->b.u32, ip->c.u64});
            emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_memset, pp.pushParams);
            break;

            /////////////////////////////////////

        case ByteCodeOp::SetZeroStack8:
            pp.emit_Store8_Immediate(offsetStack + ip->a.u32, 0, RDI);
            break;
        case ByteCodeOp::SetZeroStack16:
            pp.emit_Store16_Immediate(offsetStack + ip->a.u32, 0, RDI);
            break;
        case ByteCodeOp::SetZeroStack32:
            pp.emit_Store32_Immediate(offsetStack + ip->a.u32, 0, RDI);
            break;
        case ByteCodeOp::SetZeroStack64:
            pp.emit_Store64_Immediate(offsetStack + ip->a.u32, 0, RDI);
            break;
        case ByteCodeOp::SetZeroStackX:
            if (ip->b.u32 <= 128 && !buildParameters.isDebug())
                pp.emit_ClearX(ip->b.u32, offsetStack + ip->a.u32, RDI);
            else
            {
                pp.pushParams.clear();
                pp.pushParams.push_back({X64PushParamType::Addr, offsetStack + ip->a.u32});
                pp.pushParams.push_back({X64PushParamType::Imm, 0});
                pp.pushParams.push_back({X64PushParamType::Imm, ip->b.u32});
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_memset, pp.pushParams);
            }
            break;

            /////////////////////////////////////

        case ByteCodeOp::SetAtPointer8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store8_Immediate(ip->c.u32, ip->b.u8, RAX);
            else
            {
                pp.emit_Load8_Indirect(regOffset(ip->b.u32), RCX);
                pp.emit_Store8_Indirect(ip->c.u32, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store16_Immediate(ip->c.u32, ip->b.u16, RAX);
            else
            {
                pp.emit_Load16_Indirect(regOffset(ip->b.u32), RCX);
                pp.emit_Store16_Indirect(ip->c.u32, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store32_Immediate(ip->c.u32, ip->b.u32, RAX);
            else
            {
                pp.emit_Load32_Indirect(regOffset(ip->b.u32), RCX);
                pp.emit_Store32_Indirect(ip->c.u32, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer64:
            if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)
            {
                pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
                pp.emit_Store64_Immediate(ip->c.u32, ip->b.u64, RAX);
            }
            else if (ip->flags & BCI_IMM_B)
            {
                pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
                pp.emit_Load64_Immediate(RCX, ip->b.u64);
                pp.emit_Store64_Indirect(ip->c.u32, RCX, RAX);
            }
            else
            {
                pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
                pp.emit_Store64_Indirect(ip->c.u32, RAX, RCX);
            }
            break;

            /////////////////////////////////////

        case ByteCodeOp::SetAtStackPointer8:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store8_Immediate(offsetStack + ip->a.u32, ip->b.u8, RDI);
            else
            {
                pp.emit_Load8_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_Store8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer16:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store16_Immediate(offsetStack + ip->a.u32, ip->b.u16, RDI);
            else
            {
                pp.emit_Load16_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_Store16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer32:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store32_Immediate(offsetStack + ip->a.u32, ip->b.u32, RDI);
            else
            {
                pp.emit_Load32_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_Store32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer64:
            if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)
                pp.emit_Store64_Immediate(offsetStack + ip->a.u32, ip->b.u64, RDI);
            else if (ip->flags & BCI_IMM_B)
            {
                pp.emit_Load64_Immediate(RAX, ip->b.u64);
                pp.emit_Store64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }
            else
            {
                pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_Store64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }
            break;

            /////////////////////////////////////

        case ByteCodeOp::SetAtStackPointer8x2:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store8_Immediate(offsetStack + ip->a.u32, ip->b.u8, RDI);
            else
            {
                pp.emit_Load8_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_Store8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D)
                pp.emit_Store8_Immediate(offsetStack + ip->c.u32, ip->d.u8, RDI);
            else
            {
                pp.emit_Load8_Indirect(regOffset(ip->d.u32), RAX);
                pp.emit_Store8_Indirect(offsetStack + ip->c.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer16x2:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store16_Immediate(offsetStack + ip->a.u32, ip->b.u16, RDI);
            else
            {
                pp.emit_Load16_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_Store16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D)
                pp.emit_Store16_Immediate(offsetStack + ip->c.u32, ip->d.u16, RDI);
            else
            {
                pp.emit_Load16_Indirect(regOffset(ip->d.u32), RAX);
                pp.emit_Store16_Indirect(offsetStack + ip->c.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer32x2:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store32_Immediate(offsetStack + ip->a.u32, ip->b.u32, RDI);
            else
            {
                pp.emit_Load32_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_Store32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D)
                pp.emit_Store32_Immediate(offsetStack + ip->c.u32, ip->d.u32, RDI);
            else
            {
                pp.emit_Load32_Indirect(regOffset(ip->d.u32), RAX);
                pp.emit_Store32_Indirect(offsetStack + ip->c.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer64x2:
            if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)
                pp.emit_Store64_Immediate(offsetStack + ip->a.u32, ip->b.u64, RDI);
            else if (ip->flags & BCI_IMM_B)
            {
                pp.emit_Load64_Immediate(RAX, ip->b.u64);
                pp.emit_Store64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }
            else
            {
                pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
                pp.emit_Store64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D && ip->d.u64 <= 0x7FFFFFFF)
                pp.emit_Store64_Immediate(offsetStack + ip->c.u32, ip->d.u64, RDI);
            else if (ip->flags & BCI_IMM_D)
            {
                pp.emit_Load64_Immediate(RAX, ip->d.u64);
                pp.emit_Store64_Indirect(offsetStack + ip->c.u32, RAX, RDI);
            }
            else
            {
                pp.emit_Load64_Indirect(regOffset(ip->d.u32), RAX);
                pp.emit_Store64_Indirect(offsetStack + ip->c.u32, RAX, RDI);
            }
            break;

            /////////////////////////////////////

        case ByteCodeOp::MakeStackPointer:
            pp.emit_LoadAddress_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::MakeStackPointerx2:
            pp.emit_LoadAddress_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->d.u32, RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            break;
        case ByteCodeOp::MakeStackPointerRT:
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Store64_Indirect(offsetRT + regOffset(0), RAX, RDI);
            break;

            /////////////////////////////////////

        case ByteCodeOp::MakeMutableSegPointer:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, ip->b.u32);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::MakeBssSegPointer:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symBSIndex, ip->b.u32);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::MakeConstantSegPointer:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symCSIndex, ip->b.u32);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        case ByteCodeOp::MakeCompilerSegPointer:
            return Report::report({ip->node, Err(Err0060)});

            /////////////////////////////////////

        case ByteCodeOp::IncPointer64:
            emitAddSubMul64(pp, ip, 1, X64Op::ADD);
            break;
        case ByteCodeOp::DecPointer64:
            emitAddSubMul64(pp, ip, 1, X64Op::SUB);
            break;
        case ByteCodeOp::IncMulPointer64:
            emitAddSubMul64(pp, ip, ip->d.u64, X64Op::ADD);
            break;

            /////////////////////////////////////

        case ByteCodeOp::Mul64byVB64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_OpN_Immediate(RAX, ip->b.u64, X64Op::IMUL, X64Bits::B64);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::Div64byVB64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Immediate(RCX, ip->b.u64);
            pp.emit_ClearN(RDX, X64Bits::B64);
            concat.addString3("\x48\xf7\xf9"); // idiv rax, rcx
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::SetImmediate32:
            pp.emit_Store32_Immediate(regOffset(ip->a.u32), ip->b.u32, RDI);
            break;
        case ByteCodeOp::SetImmediate64:
            if (ip->b.u64 <= 0x7FFFFFFF || ip->b.u64 >> 32 == 0xFFFFFFFF)
                pp.emit_Store64_Immediate(regOffset(ip->a.u32), ip->b.u32, RDI);
            else
            {
                pp.emit_Load64_Immediate(RAX, ip->b.u64);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            }
            break;

            /////////////////////////////////////

        case ByteCodeOp::MemCpy8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RDX);
            pp.emit_CopyX(1, 0, RCX, RDX);
            break;
        case ByteCodeOp::MemCpy16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RDX);
            pp.emit_CopyX(2, 0, RCX, RDX);
            break;
        case ByteCodeOp::MemCpy32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RDX);
            pp.emit_CopyX(4, 0, RCX, RDX);
            break;
        case ByteCodeOp::MemCpy64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RDX);
            pp.emit_CopyX(8, 0, RCX, RDX);
            break;

            /////////////////////////////////////

        case ByteCodeOp::IntrinsicMemCpy:
            if ((ip->flags & BCI_IMM_C) && ip->c.u64 <= 128 && !buildParameters.isDebug())
            {
                pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
                pp.emit_Load64_Indirect(regOffset(ip->b.u32), RDX);
                pp.emit_CopyX(ip->c.u32, 0, RCX, RDX);
            }
            else
            {
                pp.pushParams.clear();
                pp.pushParams.push_back({X64PushParamType::Reg, ip->a.u32});
                pp.pushParams.push_back({X64PushParamType::Reg, ip->b.u32});
                if (ip->flags & BCI_IMM_C)
                    pp.pushParams.push_back({X64PushParamType::Imm, ip->c.u64});
                else
                    pp.pushParams.push_back({X64PushParamType::Reg, ip->c.u32});
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_memcpy, pp.pushParams);
            }
            break;
        case ByteCodeOp::IntrinsicMemSet:
            if ((ip->flags & BCI_IMM_B) && (ip->flags & BCI_IMM_C) && (ip->b.u8 == 0) && (ip->c.u64 <= 128) && !buildParameters.isDebug())
            {
                pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
                pp.emit_ClearX(ip->c.u32, 0, RCX);
            }
            else
            {
                pp.pushParams.clear();
                pp.pushParams.push_back({X64PushParamType::Reg, ip->a.u32});
                if (ip->flags & BCI_IMM_B)
                    pp.pushParams.push_back({X64PushParamType::Imm, ip->b.u8});
                else
                    pp.pushParams.push_back({X64PushParamType::Reg, ip->b.u32});
                if (ip->flags & BCI_IMM_C)
                    pp.pushParams.push_back({X64PushParamType::Imm, ip->c.u64});
                else
                    pp.pushParams.push_back({X64PushParamType::Reg, ip->c.u32});
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_memset, pp.pushParams);
            }
            break;
        case ByteCodeOp::IntrinsicMemMove:
            pp.pushParams.clear();
            pp.pushParams.push_back({X64PushParamType::Reg, ip->a.u32});
            pp.pushParams.push_back({X64PushParamType::Reg, ip->b.u32});
            if (ip->flags & BCI_IMM_C)
                pp.pushParams.push_back({X64PushParamType::Imm, ip->c.u64});
            else
                pp.pushParams.push_back({X64PushParamType::Reg, ip->c.u32});
            emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_memmove, pp.pushParams);
            break;
        case ByteCodeOp::IntrinsicMemCmp:
            pp.pushParams.clear();
            pp.pushParams.push_back({X64PushParamType::Reg, ip->b.u32});
            pp.pushParams.push_back({X64PushParamType::Reg, ip->c.u32});
            if (ip->flags & BCI_IMM_D)
                pp.pushParams.push_back({X64PushParamType::Imm, ip->d.u64});
            else
                pp.pushParams.push_back({X64PushParamType::Reg, ip->d.u32});
            emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_memcmp, pp.pushParams, regOffset(ip->a.u32));
            break;

        case ByteCodeOp::IntrinsicStrLen:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_strlen, {ip->b.u32}, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicStrCmp:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_strcmp, {ip->b.u32, ip->c.u32}, regOffset(ip->a.u32));
            break;

        case ByteCodeOp::IntrinsicAlloc:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_malloc, {ip->b.u32}, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicRealloc:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_realloc, {ip->b.u32, ip->c.u32}, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicFree:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_free, {ip->a.u32});
            break;

        case ByteCodeOp::InternalInitStackTrace:
            emitCall(pp, g_LangSpec->name__initStackTrace);
            break;
        case ByteCodeOp::InternalStackTrace:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name__stackTrace, {ip->a.u32});
            break;
        case ByteCodeOp::InternalPanic:
            emitInternalPanic(pp, ip->node, (const char*) ip->d.pointer);
            break;
        case ByteCodeOp::InternalUnreachable:
            break;
        case ByteCodeOp::Unreachable:
            emitInternalPanic(pp, ip->node, "executing unreachable code");
            break;

        case ByteCodeOp::InternalGetTlsPtr:
            pp.pushParams.clear();
            pp.pushParams.push_back({X64PushParamType::RelocV, pp.symTls_threadLocalId});
            pp.pushParams.push_back({X64PushParamType::Imm64, module->tlsSegment.totalCount});
            pp.pushParams.push_back({X64PushParamType::RelocAddr, pp.symTLSIndex});
            emitInternalCallExt(pp, moduleToGen, g_LangSpec->name__tlsGetPtr, pp.pushParams, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicGetContext:
            pp.pushParams.clear();
            pp.pushParams.push_back({X64PushParamType::RelocV, pp.symPI_contextTlsId});
            emitInternalCallExt(pp, moduleToGen, g_LangSpec->name__tlsGetValue, pp.pushParams, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicSetContext:
            pp.pushParams.clear();
            pp.pushParams.push_back({X64PushParamType::RelocV, pp.symPI_contextTlsId});
            pp.pushParams.push_back({X64PushParamType::Reg, ip->a.u32});
            emitInternalCallExt(pp, moduleToGen, g_LangSpec->name__tlsSetValue, pp.pushParams);
            break;

        case ByteCodeOp::IntrinsicGetProcessInfos:
            pp.emit_Symbol_RelocationAddr(RCX, pp.symPI_processInfos, 0);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RCX);
            break;

        case ByteCodeOp::IntrinsicCVaStart:
        {
            int stackOffset = 0;
            int paramIdx    = typeFunc->numParamsRegisters();
            if (CallConv::returnByAddress(typeFunc))
                paramIdx += 1;
            stackOffset = coffFct->offsetCallerStackParams + regOffset(paramIdx);
            pp.emit_LoadAddress_Indirect(stackOffset, RAX, RDI);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        }
        case ByteCodeOp::IntrinsicCVaEnd:
            break;
        case ByteCodeOp::IntrinsicCVaArg:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RCX, RAX);
            switch (ip->c.u32)
            {
            case 1:
                pp.emit_ClearN(RDX, X64Bits::B64);
                pp.emit_Load8_Indirect(0, RDX, RCX);
                break;
            case 2:
                pp.emit_ClearN(RDX, X64Bits::B64);
                pp.emit_Load16_Indirect(0, RDX, RCX);
                break;
            case 4:
                pp.emit_Load32_Indirect(0, RDX, RCX);
                break;
            case 8:
                pp.emit_Load64_Indirect(0, RDX, RCX);
                break;
            }

            pp.emit_Store64_Indirect(regOffset(ip->b.u32), RDX);
            pp.emit_Load64_Immediate(RCX, 8);
            pp.emit_OpN_Indirect(0, RCX, RAX, X64Op::ADD, X64Bits::B64);
            break;

        case ByteCodeOp::IntrinsicArguments:
            SWAG_ASSERT(ip->b.u32 == ip->a.u32 + 1);
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_atargs, {}, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicModules:
            if (moduleToGen->modulesSliceOffset == UINT32_MAX)
            {
                pp.emit_LoadAddress_Indirect(regOffset(ip->a.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, 0, RAX);
                pp.emit_LoadAddress_Indirect(regOffset(ip->b.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, 0, RAX);
            }
            else
            {
                pp.emit_Symbol_RelocationAddr(RAX, pp.symCSIndex, moduleToGen->modulesSliceOffset);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
                pp.emit_LoadAddress_Indirect(regOffset(ip->b.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, moduleToGen->moduleDependencies.count + 1, RAX);
            }
            break;
        case ByteCodeOp::IntrinsicGvtd:
            if (moduleToGen->globalVarsToDropSliceOffset == UINT32_MAX)
            {
                pp.emit_LoadAddress_Indirect(regOffset(ip->a.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, 0, RAX);
                pp.emit_LoadAddress_Indirect(regOffset(ip->b.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, 0, RAX);
            }
            else
            {
                pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, moduleToGen->globalVarsToDropSliceOffset);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
                pp.emit_LoadAddress_Indirect(regOffset(ip->b.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, moduleToGen->globalVarsToDrop.count, RAX);
            }
            break;

        case ByteCodeOp::IntrinsicGetErr:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_aterr, {}, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicCompiler:
            pp.emit_LoadAddress_Indirect(regOffset(ip->a.u32), RAX, RDI);
            pp.emit_Store64_Immediate(0, 0, RAX);
            pp.emit_LoadAddress_Indirect(regOffset(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Immediate(0, 0, RAX);
            break;
        case ByteCodeOp::IntrinsicIsByteCode:
            pp.emit_LoadAddress_Indirect(regOffset(ip->a.u32), RAX, RDI);
            pp.emit_Store32_Immediate(0, 0, RAX);
            break;
        case ByteCodeOp::IntrinsicCompilerError:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_atcompilererror, {ip->a.u32, ip->b.u32, ip->c.u32});
            break;
        case ByteCodeOp::IntrinsicCompilerWarning:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_atcompilerwarning, {ip->a.u32, ip->b.u32, ip->c.u32});
            break;
        case ByteCodeOp::IntrinsicPanic:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_atpanic, {ip->a.u32, ip->b.u32, ip->c.u32});
            break;
        case ByteCodeOp::IntrinsicItfTableOf:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name_atitftableof, {ip->a.u32, ip->b.u32}, regOffset(ip->c.u32));
            break;

            /////////////////////////////////////

        case ByteCodeOp::CopyRCtoRR:
            pp.emit_LoadAddress_Indirect(offsetResult, RAX, RDI);
            if (ip->flags & BCI_IMM_A && ip->a.u64 <= 0x7FFFFFFF)
            {
                pp.emit_Store64_Immediate(0, ip->a.u64, RAX);
            }
            else if (ip->flags & BCI_IMM_A)
            {
                pp.emit_Load64_Immediate(RCX, ip->a.u64);
                pp.emit_Store64_Indirect(0, RCX, RAX);
            }
            else
            {
                pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
                pp.emit_Store64_Indirect(0, RCX, RAX);
            }

            break;

        case ByteCodeOp::CopyRCtoRR2:
        {
            int stackOffset = getParamStackOffset(coffFct, typeFunc->numParamsRegisters());
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RCX, RAX);
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RCX);
            pp.emit_Store64_Indirect(8, RCX, RAX);
            break;
        }

        case ByteCodeOp::CopyRCtoRT:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Store64_Indirect(offsetRT + regOffset(0), RAX, RDI);
            break;

        case ByteCodeOp::CopyRRtoRC:
        {
            int stackOffset = getParamStackOffset(coffFct, typeFunc->numParamsRegisters());
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            if (ip->b.u64)
            {
                pp.emit_Load64_Immediate(RCX, ip->b.u64);
                pp.emit_OpN(RCX, RAX, X64Op::ADD, X64Bits::B64);
            }

            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        }

        case ByteCodeOp::CopyRTtoRC:
            pp.emit_Load64_Indirect(offsetRT + regOffset(0), RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CopyRTtoRC2:
            pp.emit_Load64_Indirect(offsetRT + regOffset(0), RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(offsetRT + regOffset(1), RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->b.u32), RAX);
            break;

        case ByteCodeOp::CopySP:
            pp.emit_LoadAddress_Indirect(regOffset(ip->c.u32), RAX, RDI);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CopySPVaargs:
        {
            auto typeFuncCall = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->d.pointer, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
            if (!pushRVParams.empty())
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
                        pp.emit_Load8_Indirect(regOffset(reg), RAX);
                        pp.emit_Store8_Indirect(offset, RAX, RSP);
                        break;
                    case 2:
                        pp.emit_Load16_Indirect(regOffset(reg), RAX);
                        pp.emit_Store16_Indirect(offset, RAX, RSP);
                        break;
                    case 4:
                        pp.emit_Load32_Indirect(regOffset(reg), RAX);
                        pp.emit_Store32_Indirect(offset, RAX, RSP);
                        break;
                    case 8:
                        pp.emit_Load64_Indirect(regOffset(reg), RAX);
                        pp.emit_Store64_Indirect(offset, RAX, RSP);
                        break;
                    }

                    idxParam--;
                    offset += sizeOf;
                }

                pp.emit_LoadAddress_Indirect((sizeParamsStack - variadicStackSize), RAX, RSP);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
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
                    pp.emit_Load64_Indirect(regOffset(pushRAParams[idxParam]), RAX);
                    pp.emit_Store64_Indirect(offset, RAX, RSP);
                    idxParam--;
                    offset += 8;
                }

                pp.emit_LoadAddress_Indirect((sizeParamsStack - variadicStackSize), RAX, RSP);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            }
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

        case ByteCodeOp::GetParam8:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 1);
            break;
        case ByteCodeOp::GetParam16:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 2);
            break;
        case ByteCodeOp::GetParam32:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 4);
            break;
        case ByteCodeOp::GetParam64:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 8);
            break;
        case ByteCodeOp::GetIncParam64:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 8, ip->d.u64);
            break;

            /////////////////////////////////////

        case ByteCodeOp::GetParam64DeRef8:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 8, 0, 1);
            break;
        case ByteCodeOp::GetParam64DeRef16:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 8, 0, 2);
            break;
        case ByteCodeOp::GetParam64DeRef32:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 8, 0, 4);
            break;
        case ByteCodeOp::GetParam64DeRef64:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 8, 0, 8);
            break;

            /////////////////////////////////////

        case ByteCodeOp::GetIncParam64DeRef8:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 8, ip->d.u64, 1);
            break;
        case ByteCodeOp::GetIncParam64DeRef16:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 8, ip->d.u64, 2);
            break;
        case ByteCodeOp::GetIncParam64DeRef32:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 8, ip->d.u64, 4);
            break;
        case ByteCodeOp::GetIncParam64DeRef64:
            emitGetParam(pp, coffFct, ip->a.u32, ip->c.u32, 8, ip->d.u64, 8);
            break;

            /////////////////////////////////////

        case ByteCodeOp::MakeLambda:
        {
            auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->b.pointer, AstNodeKind::FuncDecl);
            SWAG_ASSERT(!ip->c.pointer || (funcNode && funcNode->hasExtByteCode() && funcNode->extByteCode()->bc == (ByteCode*) ip->c.pointer));
            Utf8 callName = funcNode->getCallName();
            pp.emit_Load64_Immediate(RAX, 0, true);

            CoffRelocation reloc;
            reloc.virtualAddress = concat.totalCount() - sizeof(uint64_t) - pp.textSectionOffset;
            auto callSym         = pp.getOrAddSymbol(callName, CoffSymbolKind::Extern);
            reloc.symbolIndex    = callSym->index;
            reloc.type           = IMAGE_REL_AMD64_ADDR64;
            pp.relocTableTextSection.table.push_back(reloc);
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
            break;
        }

        case ByteCodeOp::IntrinsicMakeCallback:
        {
            // Test if it's a bytecode lambda
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Load64_Immediate(RCX, SWAG_LAMBDA_BC_MARKER);
            pp.emit_OpN(RAX, RCX, X64Op::AND, X64Bits::B64);
            pp.emit_TestN(RCX, RCX, X64Bits::B64);
            pp.emit_LongJumpOp(JZ);
            concat.addU32(0);
            auto jumpBCToAfterAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpBCToAfterOffset = concat.totalCount();

            // ByteCode lambda
            //////////////////

            pp.emit_CopyN(RCX, RAX, X64Bits::B64);
            pp.emit_Symbol_RelocationAddr(RAX, pp.symPI_makeCallback, 0);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_Call_Indirect(RAX);

            // End
            //////////////////
            *jumpBCToAfterAddr = concat.totalCount() - jumpBCToAfterOffset;
            pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);

            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::LocalCallPopParam:
            pushRAParams.push_back(ip->d.u32);
        case ByteCodeOp::LocalCall:
        case ByteCodeOp::LocalCallPop:
        case ByteCodeOp::LocalCallPopRC:
        {
            ByteCode* callBc = (ByteCode*) ip->a.pointer;
            emitCall(pp, (TypeInfoFuncAttr*) ip->b.pointer, callBc->getCallNameFromDecl(), pushRAParams, offsetRT, true);
            pushRAParams.clear();
            pushRVParams.clear();

            if (ip->op == ByteCodeOp::LocalCallPopRC)
            {
                pp.emit_Load64_Indirect(offsetRT + regOffset(0), RAX, RDI);
                pp.emit_Store64_Indirect(regOffset(ip->d.u32), RAX);
            }

            break;
        }

        case ByteCodeOp::ForeignCall:
        case ByteCodeOp::ForeignCallPop:
        {
            auto funcNode = (AstFuncDecl*) ip->a.pointer;
            funcNode->computeFullNameForeign(false);
            emitCall(pp, (TypeInfoFuncAttr*) ip->b.pointer, funcNode->fullnameForeign, pushRAParams, offsetRT, false);
            pushRAParams.clear();
            pushRVParams.clear();
            break;
        }

        case ByteCodeOp::LambdaCall:
        case ByteCodeOp::LambdaCallPop:
        {
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

            // Test if it's a bytecode lambda
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), R10);
            concat.addString4("\x49\x0F\xBA\xE2"); // bt r10, ??
            concat.addU8(SWAG_LAMBDA_BC_MARKER_BIT);
            pp.emit_LongJumpOp(JB);
            concat.addU32(0);
            auto jumpToBCAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpToBCOffset = concat.totalCount();

            // Native lambda
            //////////////////
            pp.emit_Call_Parameters(typeFuncBC, pushRAParams, offsetRT);
            pp.emit_Call_Indirect(R10);
            pp.emit_Call_Result(typeFuncBC, offsetRT);

            pp.emit_LongJumpOp(JUMP);
            concat.addU32(0);
            auto jumpBCToAfterAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpBCToAfterOffset = concat.totalCount();

            // ByteCode lambda
            //////////////////
            *jumpToBCAddr = concat.totalCount() - jumpToBCOffset;

            pp.emit_CopyN(RCX, R10, X64Bits::B64);
            emitByteCodeCallParameters(pp, typeFuncBC, offsetRT, pushRAParams);
            pp.emit_Symbol_RelocationAddr(RAX, pp.symPI_byteCodeRun, 0);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_Call_Indirect(RAX);

            // End
            //////////////////
            *jumpBCToAfterAddr = concat.totalCount() - jumpBCToAfterOffset;

            pushRAParams.clear();
            pushRVParams.clear();
            break;
        }

            /////////////////////////////////////

        case ByteCodeOp::IncSPPostCall:
        case ByteCodeOp::IncSPPostCallCond:
            pushRAParams.clear();
            pushRVParams.clear();
            break;

        case ByteCodeOp::CopyRCtoRRRet:
            pp.emit_LoadAddress_Indirect(offsetResult, RAX, RDI);
            if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)
            {
                pp.emit_Store64_Immediate(0, ip->b.u64, RAX);
            }
            else if (ip->flags & BCI_IMM_B)
            {
                pp.emit_Load64_Immediate(RCX, ip->b.u64);
                pp.emit_Store64_Indirect(0, RCX, RAX);
            }
            else
            {
                pp.emit_Load64_Indirect(regOffset(ip->b.u32), RCX);
                pp.emit_Store64_Indirect(0, RCX, RAX);
            }

        case ByteCodeOp::Ret:

            // Emit result
            if (!returnType->isVoid() && !CallConv::returnByAddress(typeFunc))
            {
                pp.emit_Load64_Indirect(offsetResult, cc.returnByRegisterInteger, RDI);
                if (returnType->isNative(NativeTypeKind::F32))
                    pp.emit_CopyF32(cc.returnByRegisterFloat, RAX);
                else if (returnType->isNative(NativeTypeKind::F64))
                    pp.emit_CopyF64(cc.returnByRegisterFloat, RAX);
            }

            pp.emit_Add32_RSP(sizeStack + sizeParamsStack);
            for (int32_t rRet = (int32_t) unwindRegs.size() - 1; rRet >= 0; rRet--)
                pp.emit_Pop(unwindRegs[rRet]);
            pp.emit_Ret();
            break;

            /////////////////////////////////////

        case ByteCodeOp::IntrinsicMulAddF32:
        {
            MK_IMMB_F32(XMM0);
            MK_IMMC_F32(XMM1);
            MK_IMMD_F32(XMM2);

            pp.concat.addU8(0xF3);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) X64Op::FMUL);
            pp.concat.addU8(0xC1);

            pp.concat.addU8(0xF3);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) X64Op::FADD);
            pp.concat.addU8(0xC2);

            pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        }
        case ByteCodeOp::IntrinsicMulAddF64:
        {
            MK_IMMB_F64(XMM0);
            MK_IMMC_F64(XMM1);
            MK_IMMD_F64(XMM2);

            pp.concat.addU8(0xF2);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) X64Op::FMUL);
            pp.concat.addU8(0xC1);

            pp.concat.addU8(0xF2);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) X64Op::FADD);
            pp.concat.addU8(0xC2);

            pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicAddS8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load8_Indirect(0, RAX, RCX);
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ8_LOCK_CAB(X64Op::ADD);
            break;
        case ByteCodeOp::IntrinsicAtomicAddS16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load16_Indirect(0, RAX, RCX);
            pp.emit_Store16_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ16_LOCK_CAB(X64Op::ADD);
            break;
        case ByteCodeOp::IntrinsicAtomicAddS32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load32_Indirect(0, RAX, RCX);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ32_LOCK_CAB(X64Op::ADD);
            break;
        case ByteCodeOp::IntrinsicAtomicAddS64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(0, RAX, RCX);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ64_LOCK_CAB(X64Op::ADD);
            break;

        case ByteCodeOp::IntrinsicAtomicAndS8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load8_Indirect(0, RAX, RCX);
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ8_LOCK_CAB(X64Op::AND);
            break;
        case ByteCodeOp::IntrinsicAtomicAndS16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load16_Indirect(0, RAX, RCX);
            pp.emit_Store16_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ16_LOCK_CAB(X64Op::AND);
            break;
        case ByteCodeOp::IntrinsicAtomicAndS32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load32_Indirect(0, RAX, RCX);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ32_LOCK_CAB(X64Op::AND);
            break;
        case ByteCodeOp::IntrinsicAtomicAndS64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(0, RAX, RCX);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ64_LOCK_CAB(X64Op::AND);
            break;

        case ByteCodeOp::IntrinsicAtomicOrS8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load8_Indirect(0, RAX, RCX);
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ8_LOCK_CAB(X64Op::OR);
            break;
        case ByteCodeOp::IntrinsicAtomicOrS16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load16_Indirect(0, RAX, RCX);
            pp.emit_Store16_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ16_LOCK_CAB(X64Op::OR);
            break;
        case ByteCodeOp::IntrinsicAtomicOrS32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load32_Indirect(0, RAX, RCX);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ32_LOCK_CAB(X64Op::OR);
            break;
        case ByteCodeOp::IntrinsicAtomicOrS64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(0, RAX, RCX);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ64_LOCK_CAB(X64Op::OR);
            break;

        case ByteCodeOp::IntrinsicAtomicXorS8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load8_Indirect(0, RAX, RCX);
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ8_LOCK_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::IntrinsicAtomicXorS16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load16_Indirect(0, RAX, RCX);
            pp.emit_Store16_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ16_LOCK_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::IntrinsicAtomicXorS32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load32_Indirect(0, RAX, RCX);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ32_LOCK_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::IntrinsicAtomicXorS64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(0, RAX, RCX);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ64_LOCK_CAB(X64Op::XOR);
            break;

        case ByteCodeOp::IntrinsicAtomicXchgS8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load8_Indirect(0, RAX, RCX);
            pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ8_LOCK_CAB(X64Op::XCHG);
            break;
        case ByteCodeOp::IntrinsicAtomicXchgS16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load16_Indirect(0, RAX, RCX);
            pp.emit_Store16_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ16_LOCK_CAB(X64Op::XCHG);
            break;
        case ByteCodeOp::IntrinsicAtomicXchgS32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load32_Indirect(0, RAX, RCX);
            pp.emit_Store32_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ32_LOCK_CAB(X64Op::XCHG);
            break;
        case ByteCodeOp::IntrinsicAtomicXchgS64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(0, RAX, RCX);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
            MK_BINOPEQ64_LOCK_CAB(X64Op::XCHG);
            break;

        case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            MK_IMMB_8(RAX);
            MK_IMMC_8(RDX);
            pp.concat.addString4("\xF0\x0F\xB0\x11"); // lock CMPXCHG [rcx], dl
            pp.emit_Store8_Indirect(regOffset(ip->d.u32), RAX);
            break;
        case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            MK_IMMB_16(RAX);
            MK_IMMC_16(RDX);
            pp.concat.addString5("\x66\xF0\x0F\xB1\x11"); // lock CMPXCHG [rcx], dx
            pp.emit_Store16_Indirect(regOffset(ip->d.u32), RAX);
            break;
        case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            MK_IMMB_32(RAX);
            MK_IMMC_32(RDX);
            pp.concat.addString4("\xF0\x0F\xB1\x11"); // lock CMPXCHG [rcx], edx
            pp.emit_Store32_Indirect(regOffset(ip->d.u32), RAX);
            break;
        case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            MK_IMMB_64(RAX);
            MK_IMMC_64(RDX);
            pp.concat.addString5("\xF0\x48\x0F\xB1\x11"); // lock CMPXCHG [rcx], rdx
            pp.emit_Store64_Indirect(regOffset(ip->d.u32), RAX);
            break;

        case ByteCodeOp::IntrinsicS8x1:
        {
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                MK_IMMB_8(RAX);
                pp.emit_CopyN(RCX, RAX, X64Bits::B8);
                concat.addString3("\xC0\xF9\x07"); // sar cl, 7
                pp.emit_OpN(RCX, RAX, X64Op::XOR, X64Bits::B8);
                pp.emit_OpN(RCX, RAX, X64Op::SUB, X64Bits::B8);
                pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountNz:
                MK_IMMB_U8_TO_U32(RAX);
                concat.addString4("\xF3\x0F\xB8\xC0"); // popcnt eax, eax
                pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountTz:
                MK_IMMB_U8_TO_U32(RAX);
                concat.addString3("\x0F\xBC\xC0"); // bsf eax, eax
                pp.emit_Load32_Immediate(RCX, 8);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVE);
                pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountLz:
                MK_IMMB_U8_TO_U32(RAX);
                concat.addString3("\x0F\xBD\xC0"); // bsr eax, eax
                pp.emit_Load32_Immediate(RCX, 15);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVE);
                pp.emit_OpN_Immediate(RAX, 7, X64Op::XOR, X64Bits::B32);
                pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                MK_IMMB_16(RAX);
                pp.emit_CopyN(RCX, RAX, X64Bits::B16);
                concat.addString4("\x66\xC1\xF9\x0F"); // sar cx, 15
                pp.emit_OpN(RCX, RAX, X64Op::XOR, X64Bits::B16);
                pp.emit_OpN(RCX, RAX, X64Op::SUB, X64Bits::B16);
                pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountNz:
                MK_IMMB_U8_TO_U16(RAX);
                concat.addString5("\x66\xF3\x0F\xB8\xC0"); // popcnt ax, ax
                pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountTz:
                MK_IMMB_16(RAX);
                concat.addString4("\x66\x0F\xBC\xC0"); // bsf ax, ax
                pp.emit_Load16_Immediate(RCX, 16);
                pp.emit_CMovN(RAX, RCX, X64Bits::B16, X64Op::CMOVE);
                pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountLz:
                MK_IMMB_16(RAX);
                concat.addString4("\x66\x0F\xBD\xC0"); // bsr ax, ax
                pp.emit_Load16_Immediate(RCX, 31);
                pp.emit_CMovN(RAX, RCX, X64Bits::B16, X64Op::CMOVE);
                pp.emit_OpN_Immediate(RAX, 15, X64Op::XOR, X64Bits::B16);
                pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicByteSwap:
                MK_IMMB_16(RAX);
                concat.addString4("\x66\xC1\xC0\x08"); // rol ax, 8
                pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                MK_IMMB_32(RAX);
                pp.emit_CopyN(RCX, RAX, X64Bits::B32);
                concat.addString3("\xC1\xF9\x1F"); // sar ecx, 31
                pp.emit_OpN(RCX, RAX, X64Op::XOR, X64Bits::B32);
                pp.emit_OpN(RCX, RAX, X64Op::SUB, X64Bits::B32);
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountNz:
                MK_IMMB_32(RAX);
                concat.addString4("\xF3\x0F\xB8\xC0"); // popcnt eax, eax
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountTz:
                MK_IMMB_32(RAX);
                concat.addString3("\x0F\xBC\xC0"); // bsf eax, eax
                pp.emit_Load32_Immediate(RCX, 32);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVE);
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountLz:
                MK_IMMB_32(RAX);
                concat.addString3("\x0F\xBD\xC0"); // bsr eax, eax
                pp.emit_Load32_Immediate(RCX, 63);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVE);
                pp.emit_OpN_Immediate(RAX, 31, X64Op::XOR, X64Bits::B32);
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicByteSwap:
                MK_IMMB_32(RAX);
                pp.emit_BSwapN(RAX, X64Bits::B32);
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                MK_IMMB_64(RAX);
                pp.emit_CopyN(RCX, RAX, X64Bits::B64);
                concat.addString4("\x48\xC1\xF9\x3F"); // sar rcx, 63
                pp.emit_OpN(RCX, RAX, X64Op::XOR, X64Bits::B64);
                pp.emit_OpN(RCX, RAX, X64Op::SUB, X64Bits::B64);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountNz:
                MK_IMMB_64(RAX);
                concat.addString5("\xF3\x48\x0F\xB8\xC0"); // popcnt rax, rax
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountTz:
                MK_IMMB_64(RAX);
                concat.addString4("\x48\x0F\xBC\xC0"); // bsf rax, rax
                pp.emit_Load32_Immediate(RCX, 64);
                pp.emit_CMovN(RAX, RCX, X64Bits::B64, X64Op::CMOVE);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountLz:
                MK_IMMB_64(RAX);
                concat.addString4("\x48\x0F\xBD\xC0"); // bsr rax, rax
                pp.emit_Load64_Immediate(RCX, 127);
                pp.emit_CMovN(RAX, RCX, X64Bits::B64, X64Op::CMOVE);
                pp.emit_OpN_Immediate(RAX, 63, X64Op::XOR, X64Bits::B64);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicByteSwap:
                MK_IMMB_64(RAX);
                pp.emit_BSwapN(RAX, X64Bits::B64);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_S8_TO_S32(RAX);
                MK_IMMC_S8_TO_S32(RCX);
                pp.emit_CmpN(RCX, RAX, X64Bits::B32);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVL);
                pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_S8_TO_S32(RAX);
                MK_IMMC_S8_TO_S32(RCX);
                pp.emit_CmpN(RAX, RCX, X64Bits::B32);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVL);
                pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_16(RAX);
                MK_IMMC_16(RCX);
                pp.emit_CmpN(RCX, RAX, X64Bits::B16);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVL);
                pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_16(RAX);
                MK_IMMC_16(RCX);
                pp.emit_CmpN(RAX, RCX, X64Bits::B16);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVL);
                pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_32(RAX);
                MK_IMMC_32(RCX);
                pp.emit_CmpN(RCX, RAX, X64Bits::B32);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVL);
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_32(RAX);
                MK_IMMC_32(RCX);
                pp.emit_CmpN(RAX, RCX, X64Bits::B32);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVL);
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_64(RAX);
                MK_IMMC_64(RCX);
                pp.emit_CmpN(RCX, RAX, X64Bits::B64);
                pp.emit_CMovN(RAX, RCX, X64Bits::B64, X64Op::CMOVL);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_64(RAX);
                MK_IMMC_64(RCX);
                pp.emit_CmpN(RAX, RCX, X64Bits::B64);
                pp.emit_CMovN(RAX, RCX, X64Bits::B64, X64Op::CMOVL);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_U8_TO_U32(RAX);
                MK_IMMC_U8_TO_U32(RCX);
                pp.emit_CmpN(RCX, RAX, X64Bits::B32);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVB);
                pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_U8_TO_U32(RAX);
                MK_IMMC_U8_TO_U32(RCX);
                pp.emit_CmpN(RAX, RCX, X64Bits::B32);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVB);
                pp.emit_Store8_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_16(RAX);
                MK_IMMC_16(RCX);
                pp.emit_CmpN(RCX, RAX, X64Bits::B16);
                pp.emit_CMovN(RAX, RCX, X64Bits::B16, X64Op::CMOVB);
                pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_16(RAX);
                MK_IMMC_16(RCX);
                pp.emit_CmpN(RAX, RCX, X64Bits::B16);
                pp.emit_CMovN(RAX, RCX, X64Bits::B16, X64Op::CMOVB);
                pp.emit_Store16_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_32(RAX);
                MK_IMMC_32(RCX);
                pp.emit_CmpN(RCX, RAX, X64Bits::B32);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVB);
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_32(RAX);
                MK_IMMC_32(RCX);
                pp.emit_CmpN(RAX, RCX, X64Bits::B32);
                pp.emit_CMovN(RAX, RCX, X64Bits::B32, X64Op::CMOVB);
                pp.emit_Store32_Indirect(regOffset(ip->a.u32), RAX);
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
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_64(RAX);
                MK_IMMC_64(RCX);
                pp.emit_CmpN(RCX, RAX, X64Bits::B64);
                pp.emit_CMovN(RAX, RCX, X64Bits::B64, X64Op::CMOVB);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_64(RAX);
                MK_IMMC_64(RCX);
                pp.emit_CmpN(RAX, RCX, X64Bits::B64);
                pp.emit_CMovN(RAX, RCX, X64Bits::B64, X64Op::CMOVB);
                pp.emit_Store64_Indirect(regOffset(ip->a.u32), RAX);
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
            pp.pushParams.clear();
            if (ip->flags & BCI_IMM_B)
                pp.pushParams.push_back({X64PushParamType::Imm, ip->b.u32});
            else
                pp.pushParams.push_back({X64PushParamType::Reg, ip->b.u32});
            if (ip->flags & BCI_IMM_C)
                pp.pushParams.push_back({X64PushParamType::Imm, ip->c.u32});
            else
                pp.pushParams.push_back({X64PushParamType::Reg, ip->c.u32});

            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_F32(XMM0);
                MK_IMMC_F32(XMM1);
                concat.addString4("\xF3\x0F\x5D\xC1"); // minss xmm0, xmm1
                pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_F32(XMM0);
                MK_IMMC_F32(XMM1);
                concat.addString4("\xF3\x0F\x5F\xC1"); // maxss xmm0, xmm1
                pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
                break;

            case TokenId::IntrinsicPow:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_powf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicATan2:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_atan2f, pp.pushParams, regOffset(ip->a.u32));
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
            pp.pushParams.clear();
            if (ip->flags & BCI_IMM_B)
                pp.pushParams.push_back({X64PushParamType::Imm, ip->b.u64});
            else
                pp.pushParams.push_back({X64PushParamType::Reg, ip->b.u32});
            if (ip->flags & BCI_IMM_C)
                pp.pushParams.push_back({X64PushParamType::Imm, ip->c.u64});
            else
                pp.pushParams.push_back({X64PushParamType::Reg, ip->c.u32});

            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_F64(XMM0);
                MK_IMMC_F64(XMM1);
                concat.addString4("\xF2\x0F\x5D\xC1"); // minss xmm0, xmm1
                pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_F64(XMM0);
                MK_IMMC_F64(XMM1);
                concat.addString4("\xF2\x0F\x5F\xC1"); // maxss xmm0, xmm1
                pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
                break;

            case TokenId::IntrinsicPow:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_pow, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicATan2:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_atan2, pp.pushParams, regOffset(ip->a.u32));
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
            pp.pushParams.clear();
            if (ip->flags & BCI_IMM_B)
                pp.pushParams.push_back({X64PushParamType::Imm, ip->b.u32});
            else
                pp.pushParams.push_back({X64PushParamType::Reg, ip->b.u32});

            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                MK_IMMB_F32(XMM0);
                concat.addString3("\x0F\x51\xC0"); // sqrtps xmm0, xmm0
                pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
                break;
            case TokenId::IntrinsicAbs:
                MK_IMMB_F32(XMM0);
                pp.emit_Load64_Immediate(RAX, 0x7FFFFFFF);
                pp.emit_CopyF64(XMM1, RAX);
                concat.addString3("\x0F\x54\xC1"); // andps xmm0, xmm1
                pp.emit_StoreF32_Indirect(regOffset(ip->a.u32), XMM0);
                break;

            case TokenId::IntrinsicSin:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_sinf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicCos:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_cosf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicTan:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_tanf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicSinh:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_sinhf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicCosh:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_coshf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicTanh:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_tanhf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicASin:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_asinf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicACos:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_acosf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicATan:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_atanf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicLog:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_logf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicLog2:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_log2f, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicLog10:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_log10f, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicFloor:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_floorf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicCeil:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_ceilf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicTrunc:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_truncf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicRound:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_roundf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicExp:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_expf, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicExp2:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_exp2f, pp.pushParams, regOffset(ip->a.u32));
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
            pp.pushParams.clear();
            if (ip->flags & BCI_IMM_B)
                pp.pushParams.push_back({X64PushParamType::Imm, ip->b.u64});
            else
                pp.pushParams.push_back({X64PushParamType::Reg, ip->b.u32});

            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                MK_IMMB_F64(XMM0);
                concat.addString4("\x66\x0F\x51\xC0"); // sqrtpd xmm0, xmm0
                pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
                break;
            case TokenId::IntrinsicAbs:
                MK_IMMB_F64(XMM0);
                pp.emit_Load64_Immediate(RAX, 0x7FFFFFFF'FFFFFFFF);
                pp.emit_CopyF64(XMM1, RAX);
                concat.addString4("\x66\x0F\x54\xC1"); // andpd xmm0, xmm1
                pp.emit_StoreF64_Indirect(regOffset(ip->a.u32), XMM0);
                break;

            case TokenId::IntrinsicSin:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_sin, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicCos:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_cos, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicTan:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_tan, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicSinh:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_sinh, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicCosh:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_cosh, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicTanh:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_tanh, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicASin:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_asin, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicACos:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_acos, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicATan:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_atan, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicLog:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_log, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicLog2:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_log2, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicLog10:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_log10, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicFloor:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_floor, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicCeil:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_ceil, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicTrunc:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_trunc, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicRound:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_round, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicExp:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_exp, pp.pushParams, regOffset(ip->a.u32));
                break;
            case TokenId::IntrinsicExp2:
                emitInternalCallExt(pp, moduleToGen, g_LangSpec->name_exp2, pp.pushParams, regOffset(ip->a.u32));
                break;
            default:
                ok = false;
                Report::internalError(moduleToGen, Fmt("unknown intrinsic '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            break;
        }

        case ByteCodeOp::IntrinsicGetErrMsg:
            SWAG_ASSERT(ip->b.u32 == ip->a.u32 + 1);
            emitInternalCall(pp, moduleToGen, g_LangSpec->name__geterrmsg, {}, regOffset(ip->a.u32));
            break;
        case ByteCodeOp::InternalSetErr:
            emitInternalCall(pp, moduleToGen, g_LangSpec->name__seterr, {ip->a.u32, ip->b.u32});
            break;
        case ByteCodeOp::InternalHasErr:
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
            pp.emit_Load32_Indirect(offsetof(SwagContext, hasError), RCX, RAX);
            pp.emit_Store32_Indirect(regOffset(ip->a.u32), RCX);
            break;
        case ByteCodeOp::InternalClearErr:
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
            pp.emit_Store32_Immediate(offsetof(SwagContext, hasError), 0, RAX);
            break;
        case ByteCodeOp::InternalPushErr:
            emitCall(pp, g_LangSpec->name__pusherr);
            break;
        case ByteCodeOp::InternalPopErr:
            emitCall(pp, g_LangSpec->name__poperr);
            break;

        default:
            ok = false;
            Report::internalError(moduleToGen, Fmt("unknown instruction '%s' during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
            break;
        }
    }

    // Solve labels
    for (auto& toSolve : pp.labelsToSolve)
    {
        auto it = pp.labels.find(toSolve.ipDest);
        SWAG_ASSERT(it != pp.labels.end());

        auto relOffset             = it->second - toSolve.currentOffset;
        *(uint32_t*) toSolve.patch = relOffset;
    }

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    pp.clearInstructionCache();

    return ok;
}

uint32_t BackendX64::getOrCreateLabel(X64Gen& pp, uint32_t ip)
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

uint16_t BackendX64::computeUnwindPush(CPURegister reg, uint32_t offsetSubRSP)
{
    uint16_t unwind0 = 0;
    unwind0          = (reg << 12);
    unwind0 |= (UWOP_PUSH_NONVOL << 8);
    unwind0 |= (uint8_t) offsetSubRSP;
    return unwind0;
}

void BackendX64::computeUnwind(const VectorNative<CPURegister>& unwindRegs,
                               const VectorNative<uint32_t>&    unwindOffsetRegs,
                               uint32_t                         sizeStack,
                               uint32_t                         offsetSubRSP,
                               VectorNative<uint16_t>&          unwind)
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

    // Now we put the registers.
    // At the end because array must be sorted in 'offset in prolog' descending order.
    // So RDI, which is the first 'push', must be the last
    for (int32_t i = (int32_t) unwindRegs.size() - 1; i >= 0; i--)
        unwind.push_back(computeUnwindPush(unwindRegs[i], unwindOffsetRegs[i]));
}
