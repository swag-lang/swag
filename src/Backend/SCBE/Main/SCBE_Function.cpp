#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Backend/SCBE/Obj/SCBE_Coff.h"
#include "Report/Diagnostic.h"
#include "Report/Report.h"
#include "SCBE_Macros.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

void SCBE::computeUnwind(const VectorNative<CPURegister>& unwindRegs,
                         const VectorNative<uint32_t>&    unwindOffsetRegs,
                         uint32_t                         sizeStack,
                         uint32_t                         offsetSubRSP,
                         VectorNative<uint16_t>&          unwind) const
{
    const auto objFileType = getObjType(g_CommandLine.target);
    switch (objFileType)
    {
        case BackendObjType::Coff:
            SCBE_Coff::computeUnwind(unwindRegs, unwindOffsetRegs, sizeStack, offsetSubRSP, unwind);
            break;
        default:
            Report::internalError(module, "SCBE::computeUnwind, unsupported output");
            break;
    }
}

bool SCBE::emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc)
{
    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && bc->node->hasAttribute(ATTRIBUTE_TEST_FUNC) && buildParameters.compileType != Test)
        return true;

    auto        ct              = buildParameters.compileType;
    auto        precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = encoder<SCBE_X64>(ct, precompileIndex);
    auto&       concat          = pp.concat;
    auto        typeFunc        = bc->getCallType();
    auto        returnType      = typeFunc->concreteReturnType();
    bool        ok              = true;
    bool        debug           = buildParameters.buildCfg->backendDebugInfos;
    const auto& cc              = typeFunc->getCallConv();

    concat.align(16);
    uint32_t startAddress = concat.totalCount();

    pp.clearInstructionCache();
    pp.labels.clear();
    pp.labelsToSolve.clear();
    bc->markLabels();

    // Get function name
    Utf8         funcName   = bc->getCallNameFromDecl();
    AstFuncDecl* bcFuncNode = bc->node ? castAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl) : nullptr;

    // Export symbol
    if (bcFuncNode && bcFuncNode->hasAttribute(ATTRIBUTE_PUBLIC))
    {
        if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
            pp.directives += form("/EXPORT:%s ", funcName.cstr());
    }

    // Symbol
    auto symbolFuncIndex = pp.getOrAddSymbol(funcName, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto cpuFct          = pp.registerFunction(bc->node, symbolFuncIndex);
    cpuFct->typeFunc     = typeFunc;
    cpuFct->startAddress = startAddress;
    if (debug)
        SCBEDebug::setLocation(cpuFct, bc, nullptr, 0);

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
    uint32_t offsetFLT = offsetStack + bc->stackSize;

    uint32_t sizeStack = offsetFLT + 8;
    MK_ALIGN16(sizeStack);

    // Calling convention, space for at least 'cc.paramByRegisterCount' parameters when calling a function
    // (should ideally be reserved only if we have a call)
    uint32_t sizeParamsStack = max(cc.paramByRegisterCount * sizeof(Register), (bc->maxCallParams + 1) * sizeof(Register));

    // Because of variadic parameters in fct calls, we need to add some extra room, in case we have to flatten them
    // We want to be sure to have the room to flatten the array of variadic (make all params contiguous). That's
    // why we multiply by 2.
    sizeParamsStack *= 2;

    MK_ALIGN16(sizeParamsStack);

    auto     beforeProlog = concat.totalCount();
    uint32_t numTotalRegs = typeFunc->numParamsRegisters();

    VectorNative<CPURegister> unwindRegs;
    VectorNative<uint32_t>    unwindOffsetRegs;

    // RDI will be a pointer to the stack, and the list of registers is stored at the start
    // of the stack
    pp.emitPush(RDI);
    unwindRegs.push_back(RDI);
    unwindOffsetRegs.push_back(concat.totalCount() - beforeProlog);

    // Push on scratch register per parameter
    while (cpuFct->numScratchRegs < min(cc.numScratchRegisters, min(cc.paramByRegisterCount, numTotalRegs)))
    {
        auto cpuReg = static_cast<CPURegister>(cc.firstScratchRegister + cpuFct->numScratchRegs);
        pp.emitPush(cpuReg);
        unwindRegs.push_back(cpuReg);
        unwindOffsetRegs.push_back(concat.totalCount() - beforeProlog);
        cpuFct->numScratchRegs++;
    }

    // Stack align
    if ((unwindRegs.size() & 1) == 0)
        sizeStack += sizeof(void*);

    // Check stack
    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        if (sizeStack + sizeParamsStack >= SWAG_LIMIT_PAGE_STACK)
        {
            pp.emitLoad64Immediate(RAX, sizeStack + sizeParamsStack);
            pp.emitCall(R"(__chkstk)");
        }
    }

    pp.emitOpNImmediate(RSP, sizeStack + sizeParamsStack, CPUOp::SUB, CPUBits::B64);

    // We need to start at sizeof(void*) because the call has pushed one register on the stack
    cpuFct->offsetCallerStackParams = static_cast<uint32_t>(sizeof(void*) + unwindRegs.size() * sizeof(void*) + sizeStack);
    cpuFct->offsetStack             = offsetStack;
    cpuFct->offsetLocalStackParams  = offsetS4;
    cpuFct->frameSize               = sizeStack + sizeParamsStack;

    // Unwind information (with the pushed registers)
    VectorNative<uint16_t> unwind;
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    computeUnwind(unwindRegs, unwindOffsetRegs, sizeStack + sizeParamsStack, sizeProlog, unwind);

    // Registers are stored after the sizeParamsStack area, which is used to store parameters for function calls
    pp.emitLoadAddressIndirect(sizeParamsStack, RDI, RSP);

    // Save register parameters
    uint32_t iReg = 0;
    while (iReg < min(cc.paramByRegisterCount, numTotalRegs))
    {
        auto     typeParam   = typeFunc->registerIdxToType(iReg);
        uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, iReg);
        if (cc.useRegisterFloat && typeParam->isNativeFloat())
            pp.emitStoreF64Indirect(stackOffset, cc.paramByRegisterFloat[iReg], RDI);
        else
            pp.emitStore64Indirect(stackOffset, cc.paramByRegisterInteger[iReg], RDI);

        if (iReg < cpuFct->numScratchRegs)
            pp.emitLoad64Indirect(stackOffset, static_cast<CPURegister>(cc.firstScratchRegister + iReg), RDI);

        iReg++;
    }

    // Save pointer to return value if this is a return by copy
    if (CallConv::returnByAddress(typeFunc) && iReg < cc.paramByRegisterCount)
    {
        uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, iReg);
        pp.emitStore64Indirect(stackOffset, cc.paramByRegisterInteger[iReg], RDI);
        iReg++;
    }

    // Save C variadic
    if (typeFunc->isFctCVariadic())
    {
        while (iReg < cc.paramByRegisterCount)
        {
            uint32_t stackOffset = cpuFct->offsetCallerStackParams + REG_OFFSET(iReg);
            pp.emitStore64Indirect(stackOffset, cc.paramByRegisterInteger[iReg], RDI);
            iReg++;
        }
    }

    // Use R11 as base pointer for capture parameters
    // This is used to debug and have access to capture parameters, even if we "lose" rcx
    // which is the register that will have a pointer to the capture buffer (but rcx is volatile)
    if (typeFunc->isClosure() && debug)
        pp.emitCopyN(R12, RCX, CPUBits::B64);

    auto                                        ip = bc->out;
    VectorNative<uint32_t>                      pushRAParams;
    VectorNative<std::pair<uint32_t, uint32_t>> pushRVParams;
    for (int32_t i = 0; i < static_cast<int32_t>(bc->numInstructions); i++, ip++)
    {
        if (ip->node->hasAstFlag(AST_NO_BACKEND))
            continue;
        if (ip->hasFlag(BCI_NO_BACKEND))
            continue;

        if (debug)
            SCBEDebug::setLocation(cpuFct, bc, ip, concat.totalCount() - beforeProlog);

        if (ip->hasFlag(BCI_JUMP_DEST))
            pp.getOrCreateLabel(i);

        if (ip->hasFlag(BCI_START_STMT | BCI_JUMP_DEST))
            pp.clearInstructionCache();

        switch (ip->op)
        {
            case ByteCodeOp::End:
            case ByteCodeOp::Nop:
            case ByteCodeOp::DecSPBP:
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
                pp.emitNop();
                break;

                /////////////////////////////////////

            case ByteCodeOp::MulAddVC64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RCX, RDI);
                pp.emitOpNImmediate(RCX, ip->c.u8, CPUOp::ADD, CPUBits::B64);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B64);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::Add32byVB32:
                pp.emitOpNIndirectDst(REG_OFFSET(ip->a.u32), ip->b.u64, RDI, CPUOp::ADD, CPUBits::B32);
                break;
            case ByteCodeOp::Add64byVB64:
                pp.emitOpNIndirectDst(REG_OFFSET(ip->a.u32), ip->b.u64, RDI, CPUOp::ADD, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearRA:
                pp.emitStore64Immediate(REG_OFFSET(ip->a.u32), 0, RDI);
                break;
            case ByteCodeOp::ClearRAx2:
                pp.emitStore64Immediate(REG_OFFSET(ip->a.u32), 0, RDI);
                pp.emitStore64Immediate(REG_OFFSET(ip->b.u32), 0, RDI);
                break;
            case ByteCodeOp::ClearRAx3:
                pp.emitStore64Immediate(REG_OFFSET(ip->a.u32), 0, RDI);
                pp.emitStore64Immediate(REG_OFFSET(ip->b.u32), 0, RDI);
                pp.emitStore64Immediate(REG_OFFSET(ip->c.u32), 0, RDI);
                break;
            case ByteCodeOp::ClearRAx4:
                pp.emitStore64Immediate(REG_OFFSET(ip->a.u32), 0, RDI);
                pp.emitStore64Immediate(REG_OFFSET(ip->b.u32), 0, RDI);
                pp.emitStore64Immediate(REG_OFFSET(ip->c.u32), 0, RDI);
                pp.emitStore64Immediate(REG_OFFSET(ip->d.u32), 0, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyRBtoRA8:
                pp.emitLoadU8U64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::CopyRBtoRA16:
                pp.emitLoadU16U64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::CopyRBtoRA32:
                pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::CopyRBtoRA64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::CopyRBtoRA64x2:
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(REG_OFFSET(ip->d.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;

            case ByteCodeOp::CopyRBAddrToRA:
                pp.emitLoadAddressIndirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CastBool8:
                pp.emitLoad8Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B8);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::CastBool16:
                pp.emitLoad16Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B16);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::CastBool32:
                pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B32);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::CastBool64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B64);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::CastS8S16:
                pp.emitLoadS8S16Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::CastS8S32:
                pp.emitLoadS8S32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::CastS16S32:
                pp.emitLoadS16S32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::CastS8S64:
                pp.emitLoadS8S64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::CastS16S64:
                pp.emitLoadS16S64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::CastS32S64:
                pp.emitLoadS32S64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::CastF32S32:
                pp.emitLoadF32Indirect(REG_OFFSET(ip->b.u32), XMM0, RDI);
                pp.emitOpF32(XMM0, RAX, CPUOp::CVTF2I);
                pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::CastF64S64:
                pp.emitLoadF64Indirect(REG_OFFSET(ip->b.u32), XMM0, RDI);
                pp.emitOpF64(XMM0, RAX, CPUOp::CVTF2I, CPUBits::B64);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::CastS8F32:
                pp.emitLoadS8S32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF32(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastS16F32:
                pp.emitLoadS16S32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF32(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastS32F32:
                pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF32(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastS64F32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF32(RAX, XMM0, CPUOp::CVTI2F, CPUBits::B64);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastU8F32:
                pp.emitLoadU8U32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF32(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastU16F32:
                pp.emitLoadU16U32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF32(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastU32F32:
                pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF32(RAX, XMM0, CPUOp::CVTI2F, CPUBits::B64);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastU64F32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF32(RAX, XMM0, CPUOp::CVTI2F, CPUBits::B64);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastF64F32:
                pp.emitLoadF64Indirect(REG_OFFSET(ip->b.u32), XMM0, RDI);
                pp.emitOpF64(RAX, XMM0, CPUOp::CVTF2F);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;

            case ByteCodeOp::CastS8F64:
                pp.emitLoadS8S32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF64(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastS16F64:
                pp.emitLoadS16S32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF64(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastS32F64:
                pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF64(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastS64F64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF64(RAX, XMM0, CPUOp::CVTI2F, CPUBits::B64);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastU8F64:
                pp.emitLoadU8U32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF64(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastU16F64:
                pp.emitLoadU16U32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF64(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastU32F64:
                pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpF64(RAX, XMM0, CPUOp::CVTI2F);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::CastU64F64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitCastU64F64(XMM0, RAX);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;

            case ByteCodeOp::CastF32F64:
                pp.emitLoadF32Indirect(REG_OFFSET(ip->b.u32), XMM0, RDI);
                pp.emitOpF32(RAX, XMM0, CPUOp::CVTF2F);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpShiftLeftS8:
            case ByteCodeOp::BinOpShiftLeftU8:
                emitShiftLogical(pp, ip, CPUOp::SHL, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpShiftLeftS16:
            case ByteCodeOp::BinOpShiftLeftU16:
                emitShiftLogical(pp, ip, CPUOp::SHL, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpShiftLeftS32:
            case ByteCodeOp::BinOpShiftLeftU32:
                emitShiftLogical(pp, ip, CPUOp::SHL, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpShiftLeftS64:
            case ByteCodeOp::BinOpShiftLeftU64:
                emitShiftLogical(pp, ip, CPUOp::SHL, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpShiftRightU8:
                emitShiftLogical(pp, ip, CPUOp::SHR, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpShiftRightU16:
                emitShiftLogical(pp, ip, CPUOp::SHR, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpShiftRightU32:
                emitShiftLogical(pp, ip, CPUOp::SHR, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpShiftRightU64:
                emitShiftLogical(pp, ip, CPUOp::SHR, CPUBits::B64);
                break;

            case ByteCodeOp::BinOpShiftRightS8:
                emitShiftRightArithmetic(pp, ip, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpShiftRightS16:
                emitShiftRightArithmetic(pp, ip, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpShiftRightS32:
                emitShiftRightArithmetic(pp, ip, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpShiftRightS64:
                emitShiftRightArithmetic(pp, ip, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpXorU8:
                emitBinOpIntNAtReg(pp, ip, CPUOp::XOR, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpXorU16:
                emitBinOpIntNAtReg(pp, ip, CPUOp::XOR, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpXorU32:
                emitBinOpIntNAtReg(pp, ip, CPUOp::XOR, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpXorU64:
                emitBinOpIntNAtReg(pp, ip, CPUOp::XOR, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpMulS8:
            case ByteCodeOp::BinOpMulS8_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::IMUL, CPUBits::B8);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS8));
                break;
            case ByteCodeOp::BinOpMulS16:
            case ByteCodeOp::BinOpMulS16_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::IMUL, CPUBits::B16);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS16));
                break;
            case ByteCodeOp::BinOpMulS32:
            case ByteCodeOp::BinOpMulS32_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::IMUL, CPUBits::B32);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS32));
                break;
            case ByteCodeOp::BinOpMulS64:
            case ByteCodeOp::BinOpMulS64_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::IMUL, CPUBits::B64);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS64));
                break;
            case ByteCodeOp::BinOpMulU8:
            case ByteCodeOp::BinOpMulU8_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::MUL, CPUBits::B8);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU8));
                break;
            case ByteCodeOp::BinOpMulU16:
            case ByteCodeOp::BinOpMulU16_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::MUL, CPUBits::B16);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU16));
                break;
            case ByteCodeOp::BinOpMulU32:
            case ByteCodeOp::BinOpMulU32_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::MUL, CPUBits::B32);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU32));
                break;
            case ByteCodeOp::BinOpMulU64:
            case ByteCodeOp::BinOpMulU64_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::MUL, CPUBits::B64);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU64));
                break;
            case ByteCodeOp::BinOpMulF32:
                emitBinOpFloat32AtReg(pp, ip, CPUOp::FMUL);
                break;
            case ByteCodeOp::BinOpMulF64:
                emitBinOpFloat64AtReg(pp, ip, CPUOp::FMUL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpModuloS8:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::IMOD, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpModuloS16:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::IMOD, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpModuloS32:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::IMOD, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpModuloS64:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::IMOD, CPUBits::B64);
                break;
            case ByteCodeOp::BinOpModuloU8:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::MOD, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpModuloU16:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::MOD, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpModuloU32:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::MOD, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpModuloU64:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::MOD, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpDivS8:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::IDIV, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpDivS16:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::IDIV, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpDivS32:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::IDIV, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpDivS64:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::IDIV, CPUBits::B64);
                break;
            case ByteCodeOp::BinOpDivU8:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::DIV, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpDivU16:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::DIV, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpDivU32:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::DIV, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpDivU64:
                emitBinOpDivIntNAtReg(pp, ip, CPUOp::DIV, CPUBits::B64);
                break;
            case ByteCodeOp::BinOpDivF32:
                emitBinOpFloat32AtReg(pp, ip, CPUOp::FDIV);
                break;
            case ByteCodeOp::BinOpDivF64:
                emitBinOpFloat64AtReg(pp, ip, CPUOp::FDIV);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpPlusS8:
            case ByteCodeOp::BinOpPlusS8_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::ADD, CPUBits::B8);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS8));
                break;
            case ByteCodeOp::BinOpPlusS16:
            case ByteCodeOp::BinOpPlusS16_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::ADD, CPUBits::B16);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS16));
                break;
            case ByteCodeOp::BinOpPlusS32:
            case ByteCodeOp::BinOpPlusS32_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::ADD, CPUBits::B32);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS32));
                break;
            case ByteCodeOp::BinOpPlusS64:
            case ByteCodeOp::BinOpPlusS64_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::ADD, CPUBits::B64);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS64));
                break;
            case ByteCodeOp::BinOpPlusU8:
            case ByteCodeOp::BinOpPlusU8_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::ADD, CPUBits::B8);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU8));
                break;
            case ByteCodeOp::BinOpPlusU16:
            case ByteCodeOp::BinOpPlusU16_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::ADD, CPUBits::B16);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU16));
                break;
            case ByteCodeOp::BinOpPlusU32:
            case ByteCodeOp::BinOpPlusU32_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::ADD, CPUBits::B32);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU32));
                break;
            case ByteCodeOp::BinOpPlusU64:
            case ByteCodeOp::BinOpPlusU64_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::ADD, CPUBits::B64);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU64));
                break;
            case ByteCodeOp::BinOpPlusF32:
                emitBinOpFloat32AtReg(pp, ip, CPUOp::FADD);
                break;
            case ByteCodeOp::BinOpPlusF64:
                emitBinOpFloat64AtReg(pp, ip, CPUOp::FADD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpMinusS8:
            case ByteCodeOp::BinOpMinusS8_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::SUB, CPUBits::B8);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS8));
                break;
            case ByteCodeOp::BinOpMinusS16:
            case ByteCodeOp::BinOpMinusS16_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::SUB, CPUBits::B16);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS16));
                break;
            case ByteCodeOp::BinOpMinusS32:
            case ByteCodeOp::BinOpMinusS32_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::SUB, CPUBits::B32);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS32));
                break;
            case ByteCodeOp::BinOpMinusS64:
            case ByteCodeOp::BinOpMinusS64_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::SUB, CPUBits::B64);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS64));
                break;
            case ByteCodeOp::BinOpMinusU8:
            case ByteCodeOp::BinOpMinusU8_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::SUB, CPUBits::B8);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU8));
                break;
            case ByteCodeOp::BinOpMinusU16:
            case ByteCodeOp::BinOpMinusU16_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::SUB, CPUBits::B16);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU16));
                break;
            case ByteCodeOp::BinOpMinusU32:
            case ByteCodeOp::BinOpMinusU32_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::SUB, CPUBits::B32);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU32));
                break;
            case ByteCodeOp::BinOpMinusU64:
            case ByteCodeOp::BinOpMinusU64_Safe:
                emitBinOpIntNAtReg(pp, ip, CPUOp::SUB, CPUBits::B64);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU64));
                break;

            case ByteCodeOp::BinOpMinusF32:
                emitBinOpFloat32AtReg(pp, ip, CPUOp::FSUB);
                break;
            case ByteCodeOp::BinOpMinusF64:
                emitBinOpFloat64AtReg(pp, ip, CPUOp::FSUB);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskAnd8:
                emitBinOpIntNAtReg(pp, ip, CPUOp::AND, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpBitmaskAnd16:
                emitBinOpIntNAtReg(pp, ip, CPUOp::AND, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpBitmaskAnd32:
                emitBinOpIntNAtReg(pp, ip, CPUOp::AND, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpBitmaskAnd64:
                emitBinOpIntNAtReg(pp, ip, CPUOp::AND, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskOr8:
                emitBinOpIntNAtReg(pp, ip, CPUOp::OR, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpBitmaskOr16:
                emitBinOpIntNAtReg(pp, ip, CPUOp::OR, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpBitmaskOr32:
                emitBinOpIntNAtReg(pp, ip, CPUOp::OR, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpBitmaskOr64:
                emitBinOpIntNAtReg(pp, ip, CPUOp::OR, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpShiftLeftEqS8:
            case ByteCodeOp::AffectOpShiftLeftEqU8:
                emitShiftEqLogical(pp, ip, CPUOp::SHL, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpShiftLeftEqS16:
            case ByteCodeOp::AffectOpShiftLeftEqU16:
                emitShiftEqLogical(pp, ip, CPUOp::SHL, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpShiftLeftEqS32:
            case ByteCodeOp::AffectOpShiftLeftEqU32:
                emitShiftEqLogical(pp, ip, CPUOp::SHL, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpShiftLeftEqS64:
            case ByteCodeOp::AffectOpShiftLeftEqU64:
                emitShiftEqLogical(pp, ip, CPUOp::SHL, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpShiftRightEqU8:
                emitShiftEqLogical(pp, ip, CPUOp::SHR, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpShiftRightEqU16:
                emitShiftEqLogical(pp, ip, CPUOp::SHR, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpShiftRightEqU32:
                emitShiftEqLogical(pp, ip, CPUOp::SHR, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpShiftRightEqU64:
                emitShiftEqLogical(pp, ip, CPUOp::SHR, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpShiftRightEqS8:
                emitShiftRightEqArithmetic(pp, ip, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpShiftRightEqS16:
                emitShiftRightEqArithmetic(pp, ip, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpShiftRightEqS32:
                emitShiftRightEqArithmetic(pp, ip, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpShiftRightEqS64:
                emitShiftRightEqArithmetic(pp, ip, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpXorEqU8:
                MK_BINOP_EQ8_CAB(CPUOp::XOR);
                break;
            case ByteCodeOp::AffectOpXorEqU16:
                MK_BINOP_EQ16_CAB(CPUOp::XOR);
                break;
            case ByteCodeOp::AffectOpXorEqU32:
                MK_BINOP_EQ32_CAB(CPUOp::XOR);
                break;
            case ByteCodeOp::AffectOpXorEqU64:
                MK_BINOP_EQ64_CAB(CPUOp::XOR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpOrEqU8:
                MK_BINOP_EQ8_CAB(CPUOp::OR);
                break;
            case ByteCodeOp::AffectOpOrEqU16:
                MK_BINOP_EQ16_CAB(CPUOp::OR);
                break;
            case ByteCodeOp::AffectOpOrEqU32:
                MK_BINOP_EQ32_CAB(CPUOp::OR);
                break;
            case ByteCodeOp::AffectOpOrEqU64:
                MK_BINOP_EQ64_CAB(CPUOp::OR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpAndEqU8:
                MK_BINOP_EQ8_CAB(CPUOp::AND);
                break;
            case ByteCodeOp::AffectOpAndEqU16:
                MK_BINOP_EQ16_CAB(CPUOp::AND);
                break;
            case ByteCodeOp::AffectOpAndEqU32:
                MK_BINOP_EQ32_CAB(CPUOp::AND);
                break;
            case ByteCodeOp::AffectOpAndEqU64:
                MK_BINOP_EQ64_CAB(CPUOp::AND);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMulEqS8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad8Indirect(0, RAX, RAX);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B8);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS8));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS8_Safe:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad8Indirect(ip->c.u32, RAX, RAX);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B8);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS8));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore8Indirect(ip->c.u32, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS8_SSafe:
                pp.emitLoad8Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS8_SSSafe:
                pp.emitLoad8Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad8Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpMulEqS16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad16Indirect(0, RAX, RAX);
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B16);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS16));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS16_Safe:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad16Indirect(ip->c.u32, RAX, RAX);
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B16);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS16));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore16Indirect(ip->c.u32, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS16_SSafe:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS16_SSSafe:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad16Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpMulEqS32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(0, RAX, RAX);
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B32);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS32));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS32_Safe:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(ip->c.u32, RAX, RAX);
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B32);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS32));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore32Indirect(ip->c.u32, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS32_SSafe:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS32_SSSafe:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad32Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpMulEqS64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(0, RAX, RAX);
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B64);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS64));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS64_Safe:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(ip->c.u32, RAX, RAX);
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B64);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS64));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore64Indirect(ip->c.u32, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS64_SSafe:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqS64_SSSafe:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad64Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IMUL, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpMulEqU8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad8Indirect(0, RAX, RAX);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B8);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU8));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU8_Safe:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad8Indirect(ip->c.u32, RAX, RAX);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B8);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU8));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore8Indirect(ip->c.u32, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU8_SSafe:
                pp.emitLoad8Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU8_SSSafe:
                pp.emitLoad8Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad8Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpMulEqU16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad16Indirect(0, RAX, RAX);
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B16);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU16));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU16_Safe:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad16Indirect(ip->c.u32, RAX, RAX);
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B16);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU16));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore16Indirect(ip->c.u32, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU16_SSafe:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU16_SSSafe:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad16Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpMulEqU32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(0, RAX, RAX);
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B32);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU32));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU32_Safe:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(ip->c.u32, RAX, RAX);
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B32);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU32));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore32Indirect(ip->c.u32, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU32_SSafe:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU32_SSSafe:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad32Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpMulEqU64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(0, RAX, RAX);
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B64);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU64));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU64_Safe:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(ip->c.u32, RAX, RAX);
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B64);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU64));
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore64Indirect(ip->c.u32, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU64_SSafe:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpMulEqU64_SSSafe:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad64Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::MUL, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpMulEqF32:
                MK_IMMB_F32(XMM1);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadF32Indirect(ip->c.u32, XMM0, RAX);
                pp.emitOpF32(XMM0, XMM1, CPUOp::FMUL);
                pp.emitStoreF32Indirect(ip->c.u32, XMM0, RAX);
                break;
            case ByteCodeOp::AffectOpMulEqF32_S:
                pp.emitLoadF32Indirect(offsetStack + ip->a.u32, XMM0, RDI);
                MK_IMMB_F32(XMM1);
                pp.emitOpF32(XMM0, XMM1, CPUOp::FMUL);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitStoreF32Indirect(0, XMM0, RAX);
                break;
            case ByteCodeOp::AffectOpMulEqF32_SS:
                pp.emitLoadF32Indirect(offsetStack + ip->a.u32, XMM0, RDI);
                pp.emitLoadF32Indirect(offsetStack + ip->b.u32, XMM1, RDI);
                pp.emitOpF32(XMM0, XMM1, CPUOp::FMUL);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitStoreF32Indirect(0, XMM0, RAX);
                break;

            case ByteCodeOp::AffectOpMulEqF64:
                MK_IMMB_F64(XMM1);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadF64Indirect(ip->c.u32, XMM0, RAX);
                pp.emitOpF64(XMM0, XMM1, CPUOp::FMUL);
                pp.emitStoreF64Indirect(ip->c.u32, XMM0, RAX);
                break;
            case ByteCodeOp::AffectOpMulEqF64_S:
                pp.emitLoadF64Indirect(offsetStack + ip->a.u32, XMM0, RDI);
                MK_IMMB_F64(XMM1);
                pp.emitOpF64(XMM0, XMM1, CPUOp::FMUL);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitStoreF64Indirect(0, XMM0, RAX);
                break;
            case ByteCodeOp::AffectOpMulEqF64_SS:
                pp.emitLoadF64Indirect(offsetStack + ip->a.u32, XMM0, RDI);
                pp.emitLoadF64Indirect(offsetStack + ip->b.u32, XMM1, RDI);
                pp.emitOpF64(XMM0, XMM1, CPUOp::FMUL);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitStoreF64Indirect(0, XMM0, RAX);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpDivEqS8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadS8S16Indirect(0, RAX, RAX);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqS8_S:
                pp.emitLoadS8S16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqS8_SS:
                pp.emitLoadS8S16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad8Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpDivEqS16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad16Indirect(0, RAX, RAX);
                pp.emitCwd();
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqS16_S:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCwd();
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqS16_SS:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCwd();
                pp.emitLoad16Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpDivEqS32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(0, RAX, RAX);
                pp.emitCdq();
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqS32_S:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCdq();
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqS32_SS:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCdq();
                pp.emitLoad32Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpDivEqS64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(0, RAX, RAX);
                pp.emitCqo();
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqS64_S:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCqo();
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqS64_SS:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCqo();
                pp.emitLoad64Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpDivEqU8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadU8U32Indirect(0, RAX, RAX);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqU8_S:
                pp.emitLoadU8U32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqU8_SS:
                pp.emitLoadU8U32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad8Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpDivEqU16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad16Indirect(0, RAX, RAX);
                pp.emitClearN(RDX, CPUBits::B16);
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqU16_S:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B16);
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqU16_SS:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B16);
                pp.emitLoad16Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpDivEqU32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(0, RAX, RAX);
                pp.emitClearN(RDX, CPUBits::B32);
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqU32_S:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B32);
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqU32_SS:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B32);
                pp.emitLoad32Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpDivEqU64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(0, RAX, RAX);
                pp.emitClearN(RDX, CPUBits::B64);
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqU64_S:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B64);
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpDivEqU64_SS:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B64);
                pp.emitLoad64Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpDivEqF32:
                MK_IMMB_F32(XMM1);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadF32Indirect(0, XMM0, RAX);
                pp.emitOpF32(XMM0, XMM1, CPUOp::FDIV);
                pp.emitStoreF32Indirect(0, XMM0, RAX);
                break;
            case ByteCodeOp::AffectOpDivEqF32_S:
                pp.emitLoadF32Indirect(offsetStack + ip->a.u32, XMM0, RDI);
                MK_IMMB_F32(XMM1);
                pp.emitOpF32(XMM0, XMM1, CPUOp::FDIV);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitStoreF32Indirect(0, XMM0, RAX);
                break;
            case ByteCodeOp::AffectOpDivEqF32_SS:
                pp.emitLoadF32Indirect(offsetStack + ip->a.u32, XMM0, RDI);
                pp.emitLoadF32Indirect(offsetStack + ip->b.u32, XMM1, RDI);
                pp.emitOpF32(XMM0, XMM1, CPUOp::FDIV);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitStoreF32Indirect(0, XMM0, RAX);
                break;

            case ByteCodeOp::AffectOpDivEqF64:
                MK_IMMB_F64(XMM1);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadF64Indirect(0, XMM0, RAX);
                pp.emitOpF64(XMM0, XMM1, CPUOp::FDIV);
                pp.emitStoreF64Indirect(0, XMM0, RAX);
                break;
            case ByteCodeOp::AffectOpDivEqF64_S:
                pp.emitLoadF64Indirect(offsetStack + ip->a.u32, XMM0, RDI);
                MK_IMMB_F64(XMM1);
                pp.emitOpF64(XMM0, XMM1, CPUOp::FDIV);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitStoreF64Indirect(0, XMM0, RAX);
                break;
            case ByteCodeOp::AffectOpDivEqF64_SS:
                pp.emitLoadF64Indirect(offsetStack + ip->a.u32, XMM0, RDI);
                pp.emitLoadF64Indirect(offsetStack + ip->b.u32, XMM1, RDI);
                pp.emitOpF64(XMM0, XMM1, CPUOp::FDIV);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitStoreF64Indirect(0, XMM0, RAX);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpModuloEqS8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadS8S16Indirect(0, RAX, RAX);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitCopyDownUp(RAX, CPUBits::B8);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqS8_S:
                pp.emitLoadS8S16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitCopyDownUp(RAX, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqS8_SS:
                pp.emitLoadS8S16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad8Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitCopyDownUp(RAX, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpModuloEqS16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad16Indirect(0, RAX, RAX);
                pp.emitCwd();
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore16Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqS16_S:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCwd();
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqS16_SS:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCwd();
                pp.emitLoad16Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RDX, RCX);
                break;

            case ByteCodeOp::AffectOpModuloEqS32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(0, RAX, RAX);
                pp.emitCdq();
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore32Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqS32_S:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCdq();
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqS32_SS:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCdq();
                pp.emitLoad32Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RDX, RCX);
                break;

            case ByteCodeOp::AffectOpModuloEqS64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(0, RAX, RAX);
                pp.emitCqo();
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore64Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqS64_S:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCqo();
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqS64_SS:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitCqo();
                pp.emitLoad64Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RDX, RCX);
                break;

            case ByteCodeOp::AffectOpModuloEqU8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadS8S16Indirect(0, RAX, RAX);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitCopyDownUp(RAX, CPUBits::B8);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqU8_S:
                pp.emitLoadU8U32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                MK_IMMB_8(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitCopyDownUp(RAX, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqU8_SS:
                pp.emitLoadU8U32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitLoad8Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitCopyDownUp(RAX, CPUBits::B8);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore8Indirect(0, RAX, RCX);
                break;

            case ByteCodeOp::AffectOpModuloEqU16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad16Indirect(0, RAX, RAX);
                pp.emitClearN(RDX, CPUBits::B16);
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore16Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqU16_S:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B16);
                MK_IMMB_16(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqU16_SS:
                pp.emitLoad16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B16);
                pp.emitLoad16Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore16Indirect(0, RDX, RCX);
                break;

            case ByteCodeOp::AffectOpModuloEqU32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(0, RAX, RAX);
                pp.emitClearN(RDX, CPUBits::B32);
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore32Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqU32_S:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B32);
                MK_IMMB_32(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqU32_SS:
                pp.emitLoad32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B32);
                pp.emitLoad32Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore32Indirect(0, RDX, RCX);
                break;

            case ByteCodeOp::AffectOpModuloEqU64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(0, RAX, RAX);
                pp.emitClearN(RDX, CPUBits::B64);
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore64Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqU64_S:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B64);
                MK_IMMB_64(RCX);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RDX, RCX);
                break;
            case ByteCodeOp::AffectOpModuloEqU64_SS:
                pp.emitLoad64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitClearN(RDX, CPUBits::B64);
                pp.emitLoad64Indirect(offsetStack + ip->b.u32, RCX, RDI);
                pp.emitOpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);
                pp.emitStore64Indirect(0, RDX, RCX);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpPlusEqS8:
                MK_BINOP_EQ8_CAB(CPUOp::ADD);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS8));
                break;
            case ByteCodeOp::AffectOpPlusEqS8_Safe:
                MK_BINOP_EQ8_CAB_OFF(CPUOp::ADD);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS8));
                break;
            case ByteCodeOp::AffectOpPlusEqS8_SSafe:
            case ByteCodeOp::AffectOpPlusEqU8_SSafe:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 == 1)
                    pp.emitIncNIndirect(offsetStack + ip->a.u32, RDI, CPUBits::B8);
                else
                    MK_BINOP_EQN_SCAB(CPUOp::ADD, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpPlusEqS8_SSSafe:
            case ByteCodeOp::AffectOpPlusEqU8_SSSafe:
                MK_BINOP_EQN_SSCAB(CPUOp::ADD, CPUBits::B8);
                break;

            case ByteCodeOp::AffectOpPlusEqS16:
                MK_BINOP_EQ16_CAB(CPUOp::ADD);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS16));
                break;
            case ByteCodeOp::AffectOpPlusEqS16_Safe:
                MK_BINOP_EQ16_CAB_OFF(CPUOp::ADD);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS16));
                break;
            case ByteCodeOp::AffectOpPlusEqS16_SSafe:
            case ByteCodeOp::AffectOpPlusEqU16_SSafe:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 == 1)
                    pp.emitIncNIndirect(offsetStack + ip->a.u32, RDI, CPUBits::B16);
                else
                    MK_BINOP_EQN_SCAB(CPUOp::ADD, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpPlusEqS16_SSSafe:
            case ByteCodeOp::AffectOpPlusEqU16_SSSafe:
                MK_BINOP_EQN_SSCAB(CPUOp::ADD, CPUBits::B16);
                break;

            case ByteCodeOp::AffectOpPlusEqS32:
                MK_BINOP_EQ32_CAB(CPUOp::ADD);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS32));
                break;
            case ByteCodeOp::AffectOpPlusEqS32_Safe:
                MK_BINOP_EQ32_CAB_OFF(CPUOp::ADD);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS32));
                break;
            case ByteCodeOp::AffectOpPlusEqS32_SSafe:
            case ByteCodeOp::AffectOpPlusEqU32_SSafe:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 == 1)
                    pp.emitIncNIndirect(offsetStack + ip->a.u32, RDI, CPUBits::B32);
                else
                    MK_BINOP_EQN_SCAB(CPUOp::ADD, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpPlusEqS32_SSSafe:
            case ByteCodeOp::AffectOpPlusEqU32_SSSafe:
                MK_BINOP_EQN_SSCAB(CPUOp::ADD, CPUBits::B32);
                break;

            case ByteCodeOp::AffectOpPlusEqS64:
                MK_BINOP_EQ64_CAB(CPUOp::ADD);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS64));
                break;
            case ByteCodeOp::AffectOpPlusEqS64_Safe:
                MK_BINOP_EQ64_CAB_OFF(CPUOp::ADD);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS64));
                break;
            case ByteCodeOp::AffectOpPlusEqS64_SSafe:
            case ByteCodeOp::AffectOpPlusEqU64_SSafe:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 == 1)
                    pp.emitIncNIndirect(offsetStack + ip->a.u32, RDI, CPUBits::B64);
                else
                    MK_BINOP_EQN_SCAB(CPUOp::ADD, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpPlusEqS64_SSSafe:
            case ByteCodeOp::AffectOpPlusEqU64_SSSafe:
                MK_BINOP_EQN_SSCAB(CPUOp::ADD, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpPlusEqU8:
                MK_BINOP_EQ8_CAB(CPUOp::ADD);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU8));
                break;
            case ByteCodeOp::AffectOpPlusEqU8_Safe:
                MK_BINOP_EQ8_CAB_OFF(CPUOp::ADD);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU8));
                break;

            case ByteCodeOp::AffectOpPlusEqU16:
                MK_BINOP_EQ16_CAB(CPUOp::ADD);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU16));
                break;
            case ByteCodeOp::AffectOpPlusEqU16_Safe:
                MK_BINOP_EQ16_CAB_OFF(CPUOp::ADD);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU16));
                break;

            case ByteCodeOp::AffectOpPlusEqU32:
                MK_BINOP_EQ32_CAB(CPUOp::ADD);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU32));
                break;
            case ByteCodeOp::AffectOpPlusEqU32_Safe:
                MK_BINOP_EQ32_CAB_OFF(CPUOp::ADD);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU32));
                break;

            case ByteCodeOp::AffectOpPlusEqU64:
                MK_BINOP_EQ64_CAB(CPUOp::ADD);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU64));
                break;
            case ByteCodeOp::AffectOpPlusEqU64_Safe:
                MK_BINOP_EQ64_CAB_OFF(CPUOp::ADD);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU64));
                break;

            case ByteCodeOp::AffectOpPlusEqF32:
                MK_BINOP_EQF32_CAB_OFF(CPUOp::FADD);
                break;
            case ByteCodeOp::AffectOpPlusEqF32_S:
                MK_BINOP_EQF32_SCAB(CPUOp::FADD);
                break;
            case ByteCodeOp::AffectOpPlusEqF32_SS:
                MK_BINOP_EQF32_SSCAB(CPUOp::FADD);
                break;

            case ByteCodeOp::AffectOpPlusEqF64:
                MK_BINOP_EQF64_CAB_OFF(CPUOp::FADD);
                break;
            case ByteCodeOp::AffectOpPlusEqF64_S:
                MK_BINOP_EQF64_SCAB(CPUOp::FADD);
                break;
            case ByteCodeOp::AffectOpPlusEqF64_SS:
                MK_BINOP_EQF64_SSCAB(CPUOp::FADD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMinusEqS8:
                MK_BINOP_EQ8_CAB(CPUOp::SUB);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS8));
                break;
            case ByteCodeOp::AffectOpMinusEqS8_Safe:
                MK_BINOP_EQ8_CAB_OFF(CPUOp::SUB);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS8));
                break;
            case ByteCodeOp::AffectOpMinusEqS8_SSafe:
            case ByteCodeOp::AffectOpMinusEqU8_SSafe:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 == 1)
                    pp.emitDecNIndirect(offsetStack + ip->a.u32, RDI, CPUBits::B8);
                else
                    MK_BINOP_EQN_SCAB(CPUOp::SUB, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpMinusEqS8_SSSafe:
            case ByteCodeOp::AffectOpMinusEqU8_SSSafe:
                MK_BINOP_EQN_SSCAB(CPUOp::SUB, CPUBits::B8);
                break;

            case ByteCodeOp::AffectOpMinusEqS16:
                MK_BINOP_EQ16_CAB(CPUOp::SUB);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16));
                break;
            case ByteCodeOp::AffectOpMinusEqS16_Safe:
                MK_BINOP_EQ16_CAB_OFF(CPUOp::SUB);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16));
                break;
            case ByteCodeOp::AffectOpMinusEqS16_SSafe:
            case ByteCodeOp::AffectOpMinusEqU16_SSafe:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 == 1)
                    pp.emitDecNIndirect(offsetStack + ip->a.u32, RDI, CPUBits::B16);
                else
                    MK_BINOP_EQN_SCAB(CPUOp::SUB, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpMinusEqS16_SSSafe:
            case ByteCodeOp::AffectOpMinusEqU16_SSSafe:
                MK_BINOP_EQN_SSCAB(CPUOp::SUB, CPUBits::B16);
                break;

            case ByteCodeOp::AffectOpMinusEqS32:
                MK_BINOP_EQ32_CAB(CPUOp::SUB);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS32));
                break;
            case ByteCodeOp::AffectOpMinusEqS32_Safe:
                MK_BINOP_EQ32_CAB_OFF(CPUOp::SUB);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS32));
                break;
            case ByteCodeOp::AffectOpMinusEqS32_SSafe:
            case ByteCodeOp::AffectOpMinusEqU32_SSafe:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 == 1)
                    pp.emitDecNIndirect(offsetStack + ip->a.u32, RDI, CPUBits::B32);
                else
                    MK_BINOP_EQN_SCAB(CPUOp::SUB, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpMinusEqS32_SSSafe:
            case ByteCodeOp::AffectOpMinusEqU32_SSSafe:
                MK_BINOP_EQN_SSCAB(CPUOp::SUB, CPUBits::B32);
                break;

            case ByteCodeOp::AffectOpMinusEqS64:
                MK_BINOP_EQ64_CAB(CPUOp::SUB);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS64));
                break;
            case ByteCodeOp::AffectOpMinusEqS64_Safe:
                MK_BINOP_EQ64_CAB_OFF(CPUOp::SUB);
                emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS64));
                break;
            case ByteCodeOp::AffectOpMinusEqS64_SSafe:
            case ByteCodeOp::AffectOpMinusEqU64_SSafe:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 == 1)
                    pp.emitDecNIndirect(offsetStack + ip->a.u32, RDI, CPUBits::B64);
                else
                    MK_BINOP_EQN_SCAB(CPUOp::SUB, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpMinusEqS64_SSSafe:
            case ByteCodeOp::AffectOpMinusEqU64_SSSafe:
                MK_BINOP_EQN_SSCAB(CPUOp::SUB, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpMinusEqU8:
                MK_BINOP_EQ8_CAB(CPUOp::SUB);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU8));
                break;
            case ByteCodeOp::AffectOpMinusEqU8_Safe:
                MK_BINOP_EQ8_CAB_OFF(CPUOp::SUB);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU8));
                break;

            case ByteCodeOp::AffectOpMinusEqU16:
                MK_BINOP_EQ16_CAB(CPUOp::SUB);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16));
                break;
            case ByteCodeOp::AffectOpMinusEqU16_Safe:
                MK_BINOP_EQ16_CAB_OFF(CPUOp::SUB);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16));
                break;

            case ByteCodeOp::AffectOpMinusEqU32:
                MK_BINOP_EQ32_CAB(CPUOp::SUB);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU32));
                break;
            case ByteCodeOp::AffectOpMinusEqU32_Safe:
                MK_BINOP_EQ32_CAB_OFF(CPUOp::SUB);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU32));
                break;

            case ByteCodeOp::AffectOpMinusEqU64:
                MK_BINOP_EQ64_CAB(CPUOp::SUB);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU64));
                break;
            case ByteCodeOp::AffectOpMinusEqU64_Safe:
                MK_BINOP_EQ64_CAB_OFF(CPUOp::SUB);
                emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU64));
                break;

            case ByteCodeOp::AffectOpMinusEqF32:
                MK_BINOP_EQF32_CAB_OFF(CPUOp::FSUB);
                break;
            case ByteCodeOp::AffectOpMinusEqF32_S:
                MK_BINOP_EQF32_SCAB(CPUOp::FSUB);
                break;
            case ByteCodeOp::AffectOpMinusEqF32_SS:
                MK_BINOP_EQF32_SSCAB(CPUOp::FSUB);
                break;

            case ByteCodeOp::AffectOpMinusEqF64:
                MK_BINOP_EQF64_CAB_OFF(CPUOp::FSUB);
                break;
            case ByteCodeOp::AffectOpMinusEqF64_S:
                MK_BINOP_EQF64_SCAB(CPUOp::FSUB);
                break;
            case ByteCodeOp::AffectOpMinusEqF64_SS:
                MK_BINOP_EQF64_SSCAB(CPUOp::FSUB);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ZeroToTrue:
                pp.emitLoad32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B32);
                pp.emitSetE();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::LowerZeroToTrue:
                pp.emitLoad32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitOpNImmediate(RAX, 31, CPUOp::SHR, CPUBits::B32);
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::LowerEqZeroToTrue:
                pp.emitLoad32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B32);
                pp.emitSetLE();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GreaterZeroToTrue:
                pp.emitLoad32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B32);
                pp.emitSetG();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GreaterEqZeroToTrue:
                pp.emitLoad32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitNotN(RAX, CPUBits::B32);
                pp.emitOpNImmediate(RAX, 31, CPUOp::SHR, CPUBits::B32);
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpGreaterS8:
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetG();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterS16:
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetG();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterS32:
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetG();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterS64:
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetG();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterU8:
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetA();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterU16:
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetA();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterU32:
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetA();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterU64:
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetA();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterF32:
                MK_BINOPF32_CAB(emitCmpF32Indirect, emitCmpF32);
                pp.emitSetA();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterF64:
                MK_BINOPF64_CAB(emitCmpF64Indirect, emitCmpF64);
                pp.emitSetA();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;

            case ByteCodeOp::CompareOpGreaterEqS8:
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetGE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterEqS16:
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetGE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterEqS32:
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetGE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterEqS64:
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetGE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterEqU8:
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetAE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterEqU16:
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetAE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterEqU32:
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetAE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterEqU64:
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetAE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterEqF32:
                MK_BINOPF32_CAB(emitCmpF32Indirect, emitCmpF32);
                pp.emitSetAE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpGreaterEqF64:
                MK_BINOPF64_CAB(emitCmpF64Indirect, emitCmpF64);
                pp.emitSetAE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpLowerS8:
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetL();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerS16:
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetL();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerS32:
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetL();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerS64:
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetL();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerU8:
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetB();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerU16:
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetB();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerU32:
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetB();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerU64:
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetB();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerF32:
                MK_BINOPF32_CAB(emitCmpF32Indirect, emitCmpF32);
                pp.emitSetB();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerF64:
                MK_BINOPF64_CAB(emitCmpF64Indirect, emitCmpF64);
                pp.emitSetB();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;

            case ByteCodeOp::CompareOpLowerEqS8:
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetLE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerEqS16:
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetLE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerEqS32:
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetLE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerEqS64:
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetLE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerEqU8:
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetBE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerEqU16:
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetBE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerEqU32:
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetBE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerEqU64:
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetBE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerEqF32:
                MK_BINOPF32_CAB(emitCmpF32Indirect, emitCmpF32);
                pp.emitSetBE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpLowerEqF64:
                MK_BINOPF64_CAB(emitCmpF64Indirect, emitCmpF64);
                pp.emitSetBE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOp3Way8:
                pp.emitClearN(R8, CPUBits::B32);
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetG(R8);
                pp.emitLoad32Immediate(RAX, 0xFFFFFFFF);
                pp.emitCMovN(RAX, R8, CPUOp::CMOVGE, CPUBits::B32);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOp3Way16:
                pp.emitClearN(R8, CPUBits::B32);
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetG(R8);
                pp.emitLoad32Immediate(RAX, 0xFFFFFFFF);
                pp.emitCMovN(RAX, R8, CPUOp::CMOVGE, CPUBits::B32);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOp3Way32:
                pp.emitClearN(R8, CPUBits::B32);
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetG(R8);
                pp.emitLoad32Immediate(RAX, 0xFFFFFFFF);
                pp.emitCMovN(RAX, R8, CPUOp::CMOVGE, CPUBits::B32);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOp3Way64:
                pp.emitClearN(R8, CPUBits::B32);
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetG(R8);
                pp.emitLoad32Immediate(RAX, 0xFFFFFFFF);
                pp.emitCMovN(RAX, R8, CPUOp::CMOVGE, CPUBits::B32);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOp3WayF32:
                pp.emitClearN(R8, CPUBits::B32);
                MK_IMMA(XMM0, CPUBits::F32);
                MK_IMMB_F32(XMM1);
                pp.emitOpF32(XMM0, XMM1, CPUOp::UCOMIF);
                pp.emitSetA(R8);
                pp.emitOpF32(XMM1, XMM0, CPUOp::UCOMIF);
                pp.emitLoad32Immediate(RAX, 0xFFFFFFFF);
                pp.emitCMovN(RAX, R8, CPUOp::CMOVBE, CPUBits::B32);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOp3WayF64:
                pp.emitClearN(R8, CPUBits::B32);
                MK_IMMA(XMM0, CPUBits::F64);
                MK_IMMB_F64(XMM1);
                pp.emitOpF64(XMM0, XMM1, CPUOp::UCOMIF);
                pp.emitSetA(R8);
                pp.emitOpF64(XMM1, XMM0, CPUOp::UCOMIF);
                pp.emitLoad32Immediate(RAX, 0xFFFFFFFF);
                pp.emitCMovN(RAX, R8, CPUOp::CMOVBE, CPUBits::B32);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpEqual8:
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpEqual16:
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpEqual32:
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpEqual64:
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpEqualF32:
                MK_BINOPF32_CAB(emitCmpF32Indirect, emitCmpF32);
                pp.emitSetEP();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpEqualF64:
                MK_BINOPF64_CAB(emitCmpF64Indirect, emitCmpF64);
                pp.emitSetEP();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpNotEqual8:
                MK_BINOP8_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpNotEqual16:
                MK_BINOP16_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpNotEqual32:
                MK_BINOP32_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpNotEqual64:
                MK_BINOP64_CAB(emitCmpNIndirectDst, emitCmpNIndirect, emitCmpN);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpNotEqualF32:
                MK_BINOPF32_CAB(emitCmpF32Indirect, emitCmpF32);
                pp.emitSetNEP();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::CompareOpNotEqualF64:
                MK_BINOPF64_CAB(emitCmpF64Indirect, emitCmpF64);
                pp.emitSetNEP();
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicDbgAlloc:
                emitInternalCall(pp, g_LangSpec->name_at_dbgalloc, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicSysAlloc:
                emitInternalCall(pp, g_LangSpec->name_at_sysalloc, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicRtFlags:
                emitInternalCall(pp, g_LangSpec->name_at_rtflags, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicStringCmp:
                emitInternalCall(pp, g_LangSpec->name_at_strcmp, {ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32}, REG_OFFSET(ip->d.u32));
                break;
            case ByteCodeOp::IntrinsicTypeCmp:
                emitInternalCall(pp, g_LangSpec->name_at_typecmp, {ip->a.u32, ip->b.u32, ip->c.u32}, REG_OFFSET(ip->d.u32));
                break;
            case ByteCodeOp::IntrinsicIs:
                emitInternalCall(pp, g_LangSpec->name_at_is, {ip->a.u32, ip->b.u32}, REG_OFFSET(ip->c.u32));
                break;
            case ByteCodeOp::IntrinsicAs:
                emitInternalCall(pp, g_LangSpec->name_at_as, {ip->a.u32, ip->b.u32, ip->c.u32}, REG_OFFSET(ip->d.u32));
                break;

                /////////////////////////////////////

            case ByteCodeOp::TestNotZero8:
                MK_IMMB_8(RAX);
                pp.emitTestN(RAX, RAX, CPUBits::B8);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::TestNotZero16:
                MK_IMMB_16(RAX);
                pp.emitTestN(RAX, RAX, CPUBits::B16);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::TestNotZero32:
                MK_IMMB_32(RAX);
                pp.emitTestN(RAX, RAX, CPUBits::B32);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::TestNotZero64:
                MK_IMMB_64(RAX);
                pp.emitTestN(RAX, RAX, CPUBits::B64);
                pp.emitSetNE();
                pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::NegBool:
                pp.emitLoad8Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitOpNImmediate(RAX, 1, CPUOp::XOR, CPUBits::B8);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::NegS32:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNegNIndirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B32);
                else
                {
                    pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitNegN(RAX, CPUBits::B64);
                    pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                }
                break;
            case ByteCodeOp::NegS64:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNegNIndirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B64);
                else
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitNegN(RAX, CPUBits::B64);
                    pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                }
                break;
            case ByteCodeOp::NegF32:
                pp.emitLoadF32Indirect(REG_OFFSET(ip->b.u32), XMM0, RDI);
                pp.emitLoad64Immediate(RAX, 0x80000000);
                pp.emitStore64Indirect(offsetFLT, RAX, RDI);
                pp.emitLoadF32Indirect(offsetFLT, XMM1, RDI);
                pp.emitOpF32(XMM0, XMM1, CPUOp::FXOR);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            case ByteCodeOp::NegF64:
                pp.emitLoadF64Indirect(REG_OFFSET(ip->b.u32), XMM0, RDI);
                pp.emitLoad64Immediate(RAX, 0x80000000'00000000);
                pp.emitStore64Indirect(offsetFLT, RAX, RDI);
                pp.emitLoadF64Indirect(offsetFLT, XMM1, RDI);
                pp.emitOpF64(XMM0, XMM1, CPUOp::FXOR);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::InvertU8:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNotNIndirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B8);
                else
                {
                    pp.emitLoad8Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitNotN(RAX, CPUBits::B8);
                    pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                }
                break;
            case ByteCodeOp::InvertU16:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNotNIndirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B16);
                else
                {
                    pp.emitLoad16Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitNotN(RAX, CPUBits::B16);
                    pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                }
                break;
            case ByteCodeOp::InvertU32:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNotNIndirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B32);
                else
                {
                    pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitNotN(RAX, CPUBits::B32);
                    pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                }
                break;
            case ByteCodeOp::InvertU64:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNotNIndirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B64);
                else
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitNotN(RAX, CPUBits::B64);
                    pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpDyn8:
            case ByteCodeOp::JumpDyn16:
            case ByteCodeOp::JumpDyn32:
            case ByteCodeOp::JumpDyn64:
            {
                auto tableCompiler = reinterpret_cast<int32_t*>(buildParameters.module->compilerSegment.address(ip->d.u32));

                if (ip->op == ByteCodeOp::JumpDyn8)
                    pp.emitLoadS8S64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                else if (ip->op == ByteCodeOp::JumpDyn16)
                    pp.emitLoadS16S64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                else if (ip->op == ByteCodeOp::JumpDyn32)
                    pp.emitLoadS32S64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                else
                    pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);

                // Note:
                //
                // This is not optimal yet.
                // The sub could be removed by baking it in the 'cmp', and by changing the jump table address by subtracting the min value
                // Also, if the jump table was encoded in the text segment, then there will be no need to have two relocations
                //
                // We could in the end remove two instructions and be as the llvm generation

                pp.emitOpNImmediate(RAX, ip->b.u64 - 1, CPUOp::SUB, CPUBits::B64);
                pp.emitCmpNImmediate(RAX, ip->c.u64, CPUBits::B64);
                pp.emitJump(JAE, i, tableCompiler[0]);

                uint8_t* addrConstant        = nullptr;
                auto     offsetTableConstant = buildParameters.module->constantSegment.reserve(static_cast<uint32_t>(ip->c.u64) * sizeof(uint32_t), &addrConstant);

                pp.emitSymbolRelocationAddr(RCX, pp.symCSIndex, offsetTableConstant); // rcx = jump table
                pp.emitJumpTable(RCX, RAX);

                // + 5 for the two following instructions
                // + 7 for this instruction
                pp.emitSymbolRelocationAddr(RAX, symbolFuncIndex, concat.totalCount() - startAddress + 5 + 7);
                pp.emitOpN(RCX, RAX, CPUOp::ADD, CPUBits::B64);
                pp.emitJump(RAX);

                auto currentOffset = static_cast<int32_t>(pp.concat.totalCount());

                auto            tableConstant = reinterpret_cast<int32_t*>(addrConstant);
                CPULabelToSolve label;
                for (uint32_t idx = 0; idx < ip->c.u32; idx++)
                {
                    label.ipDest        = tableCompiler[idx] + i + 1;
                    label.currentOffset = currentOffset;
                    label.patch         = reinterpret_cast<uint8_t*>(tableConstant + idx);
                    pp.labelsToSolve.push_back(label);
                }

                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::JumpIfTrue:
                pp.emitLoad8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B8);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfRTTrue:
                pp.emitLoad8Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B8);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfFalse:
                pp.emitLoad8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B8);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfRTFalse:
                pp.emitLoad8Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B8);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNotZero8:
                pp.emitLoad8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B8);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNotZero16:
                pp.emitLoad16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B16);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNotZero32:
                pp.emitLoad32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B32);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNotZero64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B64);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfZero8:
                pp.emitLoad8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B8);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfZero16:
                pp.emitLoad16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B16);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfZero32:
                pp.emitLoad32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B32);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfZero64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitTestN(RAX, RAX, CPUBits::B64);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::Jump:
                pp.emitJump(JUMP, i, ip->b.s32);
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
                pp.emitIncNIndirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B64);
                MK_JMPCMP_64(JZ);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfStackEqual8:
                MK_JMPCMP_ADDR_8(JZ, offsetStack + ip->a.u32, RDI);
                break;
            case ByteCodeOp::JumpIfStackNotEqual8:
                MK_JMPCMP_ADDR_8(JNZ, offsetStack + ip->a.u32, RDI);
                break;

            case ByteCodeOp::JumpIfStackEqual16:
                MK_JMPCMP_ADDR_16(JZ, offsetStack + ip->a.u32, RDI);
                break;
            case ByteCodeOp::JumpIfStackNotEqual16:
                MK_JMPCMP_ADDR_16(JNZ, offsetStack + ip->a.u32, RDI);
                break;

            case ByteCodeOp::JumpIfStackEqual32:
                MK_JMPCMP_ADDR_32(JZ, offsetStack + ip->a.u32, RDI);
                break;
            case ByteCodeOp::JumpIfStackNotEqual32:
                MK_JMPCMP_ADDR_32(JNZ, offsetStack + ip->a.u32, RDI);
                break;

            case ByteCodeOp::JumpIfStackEqual64:
                MK_JMPCMP_ADDR_64(JZ, offsetStack + ip->a.u32, RDI);
                break;
            case ByteCodeOp::JumpIfStackNotEqual64:
                MK_JMPCMP_ADDR_64(JNZ, offsetStack + ip->a.u32, RDI);
                break;

            case ByteCodeOp::JumpIfDeRefEqual8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_JMPCMP_ADDR_8(JZ, ip->d.u32, RCX);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_JMPCMP_ADDR_8(JNZ, ip->d.u32, RCX);
                break;

            case ByteCodeOp::JumpIfDeRefEqual16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_JMPCMP_ADDR_16(JZ, ip->d.u32, RCX);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_JMPCMP_ADDR_16(JNZ, ip->d.u32, RCX);
                break;

            case ByteCodeOp::JumpIfDeRefEqual32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_JMPCMP_ADDR_32(JZ, ip->d.u32, RCX);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_JMPCMP_ADDR_32(JNZ, ip->d.u32, RCX);
                break;

            case ByteCodeOp::JumpIfDeRefEqual64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_JMPCMP_ADDR_64(JZ, ip->d.u32, RCX);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_JMPCMP_ADDR_64(JNZ, ip->d.u32, RCX);
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
                pp.emitDecNIndirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B32);
                break;
            case ByteCodeOp::IncrementRA64:
                pp.emitIncNIndirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B64);
                break;
            case ByteCodeOp::DecrementRA64:
                pp.emitDecNIndirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::DeRef8:
                SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitLoadU8U64Indirect(ip->c.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::DeRef16:
                SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitLoadU16U64Indirect(ip->c.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::DeRef32:
                SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitLoad32Indirect(ip->c.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::DeRef64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                if (ip->c.u64 <= 0x7FFFFFFF)
                    pp.emitLoad64Indirect(ip->c.u32, RAX, RAX);
                else
                {
                    pp.emitLoad64Immediate(RCX, ip->c.u64);
                    pp.emitOpN(RCX, RAX, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoad64Indirect(0, RAX, RAX);
                }
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::DeRefStringSlice:
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFFF);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(ip->c.u32 + 8, RCX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->b.u32), RCX, RDI);
                pp.emitLoad64Indirect(ip->c.u32 + 0, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromBssSeg8:
                pp.emitSymbolRelocationAddr(RAX, pp.symBSIndex, 0);
                pp.emitLoadU8U64Indirect(ip->b.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromBssSeg16:
                pp.emitSymbolRelocationAddr(RAX, pp.symBSIndex, 0);
                pp.emitLoadU16U64Indirect(ip->b.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromBssSeg32:
                pp.emitSymbolRelocationAddr(RAX, pp.symBSIndex, 0);
                pp.emitLoad32Indirect(ip->b.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromBssSeg64:
                pp.emitSymbolRelocationAddr(RAX, pp.symBSIndex, 0);
                pp.emitLoad64Indirect(ip->b.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::GetFromMutableSeg8:
                pp.emitSymbolRelocationAddr(RAX, pp.symMSIndex, 0);
                pp.emitLoadU8U64Indirect(ip->b.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromMutableSeg16:
                pp.emitSymbolRelocationAddr(RAX, pp.symMSIndex, 0);
                pp.emitLoadU16U64Indirect(ip->b.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromMutableSeg32:
                pp.emitSymbolRelocationAddr(RAX, pp.symMSIndex, 0);
                pp.emitLoad32Indirect(ip->b.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromMutableSeg64:
                pp.emitSymbolRelocationAddr(RAX, pp.symMSIndex, 0);
                pp.emitLoad64Indirect(ip->b.u32, RAX, RAX);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8:
                pp.emitLoadU8U64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromStack16:
                pp.emitLoadU16U64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromStack32:
                pp.emitLoad32Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromStack64:
                pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetIncFromStack64:
                pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitOpNImmediate(RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8x2:
                pp.emitLoadU8U64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadU8U64Indirect(offsetStack + ip->d.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromStack16x2:
                pp.emitLoadU16U64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadU16U64Indirect(offsetStack + ip->d.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromStack32x2:
                pp.emitLoad32Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(offsetStack + ip->d.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetFromStack64x2:
                pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(offsetStack + ip->d.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetIncFromStack64DeRef8:
                if (ip->c.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                    pp.emitLoadU8U64Indirect(static_cast<uint32_t>(ip->c.u64), RAX, RAX);
                }
                else
                {
                    pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                    pp.emitOpNImmediate(RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoadU8U64Indirect(0, RAX, RAX);
                }
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetIncFromStack64DeRef16:
                if (ip->c.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                    pp.emitLoadU16U64Indirect(static_cast<uint32_t>(ip->c.u64), RAX, RAX);
                }
                else
                {
                    pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                    pp.emitOpNImmediate(RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoadU16U64Indirect(0, RAX, RAX);
                }
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetIncFromStack64DeRef32:
                if (ip->c.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                    pp.emitLoad32Indirect(static_cast<uint32_t>(ip->c.u64), RAX, RAX);
                }
                else
                {
                    pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                    pp.emitOpNImmediate(RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoad32Indirect(0, RAX, RAX);
                }
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::GetIncFromStack64DeRef64:
                if (ip->c.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                    pp.emitLoad64Indirect(static_cast<uint32_t>(ip->c.u64), RAX, RAX);
                }
                else
                {
                    pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                    pp.emitOpNImmediate(RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoad64Indirect(0, RAX, RAX);
                }
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyStack8:
                pp.emitLoad8Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore8Indirect(offsetStack + ip->a.u32, RAX, RDI);
                break;
            case ByteCodeOp::CopyStack16:
                pp.emitLoad16Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                break;
            case ByteCodeOp::CopyStack32:
                pp.emitLoad32Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                break;
            case ByteCodeOp::CopyStack64:
                pp.emitLoad64Indirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearMaskU32:
                if (ip->b.u32 == 0xFF)
                    pp.emitLoadU8U32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                else if (ip->b.u32 == 0xFFFF)
                    pp.emitLoadU16U32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                else
                    SWAG_ASSERT(false);
                pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::ClearMaskU64:
                if (ip->b.u32 == 0xFF)
                    pp.emitLoadU8U64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                else if (ip->b.u32 == 0xFFFF)
                    pp.emitLoadU16U64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                else if (ip->b.u32 == 0xFFFFFFFF)
                    pp.emitLoad32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                else
                    SWAG_ASSERT(false);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetZeroAtPointer8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitStore8Immediate(ip->b.u32, 0, RAX);
                break;
            case ByteCodeOp::SetZeroAtPointer16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitStore16Immediate(ip->b.u32, 0, RAX);
                break;
            case ByteCodeOp::SetZeroAtPointer32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitStore32Immediate(ip->b.u32, 0, RAX);
                break;
            case ByteCodeOp::SetZeroAtPointer64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitStore64Immediate(ip->b.u32, 0, RAX);
                break;
            case ByteCodeOp::SetZeroAtPointerX:
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                if (ip->b.u32 <= 128 && !buildParameters.isDebug())
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                    pp.emitClearX(ip->b.u32, ip->c.u32, RAX);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({CPUPushParamType::RegAdd, ip->a.u32, ip->c.u64});
                    pp.pushParams.push_back({CPUPushParamType::Imm, 0});
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u64});
                    emitInternalCallExt(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            case ByteCodeOp::SetZeroAtPointerXRB:
                pp.pushParams.clear();
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->a.u32});
                pp.pushParams.push_back({CPUPushParamType::Imm, 0});
                pp.pushParams.push_back({CPUPushParamType::RegMul, ip->b.u32, ip->c.u64});
                emitInternalCallExt(pp, g_LangSpec->name_memset, pp.pushParams);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearRR8:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad64Indirect(stackOffset, RAX, RDI);
                pp.emitStore8Immediate(ip->c.u32, 0, RAX);
                break;
            }
            case ByteCodeOp::ClearRR16:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad64Indirect(stackOffset, RAX, RDI);
                pp.emitStore16Immediate(ip->c.u32, 0, RAX);
                break;
            }
            case ByteCodeOp::ClearRR32:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad64Indirect(stackOffset, RAX, RDI);
                pp.emitStore32Immediate(ip->c.u32, 0, RAX);
                break;
            }
            case ByteCodeOp::ClearRR64:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad64Indirect(stackOffset, RAX, RDI);
                pp.emitStore64Immediate(ip->c.u32, 0, RAX);
                break;
            }
            case ByteCodeOp::ClearRRX:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad64Indirect(stackOffset, RAX, RDI);

                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                if (ip->b.u32 <= 128 && !buildParameters.isDebug())
                {
                    pp.emitClearX(ip->b.u32, ip->c.u32, RAX);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({CPUPushParamType::RAX});
                    pp.pushParams.push_back({CPUPushParamType::Imm, 0});
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u64});
                    emitInternalCallExt(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetZeroStack8:
                pp.emitStore8Immediate(offsetStack + ip->a.u32, 0, RDI);
                break;
            case ByteCodeOp::SetZeroStack16:
                pp.emitStore16Immediate(offsetStack + ip->a.u32, 0, RDI);
                break;
            case ByteCodeOp::SetZeroStack32:
                pp.emitStore32Immediate(offsetStack + ip->a.u32, 0, RDI);
                break;
            case ByteCodeOp::SetZeroStack64:
                pp.emitStore64Immediate(offsetStack + ip->a.u32, 0, RDI);
                break;
            case ByteCodeOp::SetZeroStackX:
                if (ip->b.u32 <= 128 && !buildParameters.isDebug())
                    pp.emitClearX(ip->b.u32, offsetStack + ip->a.u32, RDI);
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({CPUPushParamType::Addr, offsetStack + ip->a.u32});
                    pp.pushParams.push_back({CPUPushParamType::Imm, 0});
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u32});
                    emitInternalCallExt(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtPointer8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore8Immediate(ip->c.u32, ip->b.u8, RAX);
                else
                {
                    pp.emitLoad8Indirect(REG_OFFSET(ip->b.u32), RCX, RDI);
                    pp.emitStore8Indirect(ip->c.u32, RCX, RAX);
                }
                break;
            case ByteCodeOp::SetAtPointer16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore16Immediate(ip->c.u32, ip->b.u16, RAX);
                else
                {
                    pp.emitLoad16Indirect(REG_OFFSET(ip->b.u32), RCX, RDI);
                    pp.emitStore16Indirect(ip->c.u32, RCX, RAX);
                }
                break;
            case ByteCodeOp::SetAtPointer32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore32Immediate(ip->c.u32, ip->b.u32, RAX);
                else
                {
                    pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RCX, RDI);
                    pp.emitStore32Indirect(ip->c.u32, RCX, RAX);
                }
                break;
            case ByteCodeOp::SetAtPointer64:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                    pp.emitStore64Immediate(ip->c.u32, ip->b.u64, RAX);
                }
                else if (ip->hasFlag(BCI_IMM_B))
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                    pp.emitLoad64Immediate(RCX, ip->b.u64);
                    pp.emitStore64Indirect(ip->c.u32, RCX, RAX);
                }
                else
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                    pp.emitStore64Indirect(ip->c.u32, RAX, RCX);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore8Immediate(offsetStack + ip->a.u32, ip->b.u8, RDI);
                else
                {
                    pp.emitLoad8Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore8Indirect(offsetStack + ip->a.u32, RAX, RDI);
                }
                break;
            case ByteCodeOp::SetAtStackPointer16:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore16Immediate(offsetStack + ip->a.u32, ip->b.u16, RDI);
                else
                {
                    pp.emitLoad16Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                }
                break;
            case ByteCodeOp::SetAtStackPointer32:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore32Immediate(offsetStack + ip->a.u32, ip->b.u32, RDI);
                else
                {
                    pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                }
                break;
            case ByteCodeOp::SetAtStackPointer64:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF)
                    pp.emitStore64Immediate(offsetStack + ip->a.u32, ip->b.u64, RDI);
                else if (ip->hasFlag(BCI_IMM_B))
                {
                    pp.emitLoad64Immediate(RAX, ip->b.u64);
                    pp.emitStore64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                }
                else
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8x2:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore8Immediate(offsetStack + ip->a.u32, ip->b.u8, RDI);
                else
                {
                    pp.emitLoad8Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore8Indirect(offsetStack + ip->a.u32, RAX, RDI);
                }

                if (ip->hasFlag(BCI_IMM_D))
                    pp.emitStore8Immediate(offsetStack + ip->c.u32, ip->d.u8, RDI);
                else
                {
                    pp.emitLoad8Indirect(REG_OFFSET(ip->d.u32), RAX, RDI);
                    pp.emitStore8Indirect(offsetStack + ip->c.u32, RAX, RDI);
                }
                break;
            case ByteCodeOp::SetAtStackPointer16x2:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore16Immediate(offsetStack + ip->a.u32, ip->b.u16, RDI);
                else
                {
                    pp.emitLoad16Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore16Indirect(offsetStack + ip->a.u32, RAX, RDI);
                }

                if (ip->hasFlag(BCI_IMM_D))
                    pp.emitStore16Immediate(offsetStack + ip->c.u32, ip->d.u16, RDI);
                else
                {
                    pp.emitLoad16Indirect(REG_OFFSET(ip->d.u32), RAX, RDI);
                    pp.emitStore16Indirect(offsetStack + ip->c.u32, RAX, RDI);
                }
                break;
            case ByteCodeOp::SetAtStackPointer32x2:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore32Immediate(offsetStack + ip->a.u32, ip->b.u32, RDI);
                else
                {
                    pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore32Indirect(offsetStack + ip->a.u32, RAX, RDI);
                }

                if (ip->hasFlag(BCI_IMM_D))
                    pp.emitStore32Immediate(offsetStack + ip->c.u32, ip->d.u32, RDI);
                else
                {
                    pp.emitLoad32Indirect(REG_OFFSET(ip->d.u32), RAX, RDI);
                    pp.emitStore32Indirect(offsetStack + ip->c.u32, RAX, RDI);
                }
                break;
            case ByteCodeOp::SetAtStackPointer64x2:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF)
                    pp.emitStore64Immediate(offsetStack + ip->a.u32, ip->b.u64, RDI);
                else if (ip->hasFlag(BCI_IMM_B))
                {
                    pp.emitLoad64Immediate(RAX, ip->b.u64);
                    pp.emitStore64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                }
                else
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore64Indirect(offsetStack + ip->a.u32, RAX, RDI);
                }

                if (ip->hasFlag(BCI_IMM_D) && ip->d.u64 <= 0x7FFFFFFF)
                    pp.emitStore64Immediate(offsetStack + ip->c.u32, ip->d.u64, RDI);
                else if (ip->hasFlag(BCI_IMM_D))
                {
                    pp.emitLoad64Immediate(RAX, ip->d.u64);
                    pp.emitStore64Indirect(offsetStack + ip->c.u32, RAX, RDI);
                }
                else
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->d.u32), RAX, RDI);
                    pp.emitStore64Indirect(offsetStack + ip->c.u32, RAX, RDI);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeStackPointer:
                pp.emitLoadAddressIndirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::MakeStackPointerx2:
                pp.emitLoadAddressIndirect(offsetStack + ip->b.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoadAddressIndirect(offsetStack + ip->d.u32, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                break;
            case ByteCodeOp::MakeStackPointerRT:
                pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RAX, RDI);
                pp.emitStore64Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeMutableSegPointer:
                pp.emitSymbolRelocationAddr(RAX, pp.symMSIndex, ip->b.u32);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::MakeBssSegPointer:
                pp.emitSymbolRelocationAddr(RAX, pp.symBSIndex, ip->b.u32);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::MakeConstantSegPointer:
                pp.emitSymbolRelocationAddr(RAX, pp.symCSIndex, ip->b.u32);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            case ByteCodeOp::MakeCompilerSegPointer:
                break;

                /////////////////////////////////////

            case ByteCodeOp::IncPointer64:
                emitAddSubMul64(pp, ip, 1, CPUOp::ADD);
                break;
            case ByteCodeOp::DecPointer64:
                emitAddSubMul64(pp, ip, 1, CPUOp::SUB);
                break;
            case ByteCodeOp::IncMulPointer64:
                emitAddSubMul64(pp, ip, ip->d.u64, CPUOp::ADD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::Mul64byVB64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitOpNImmediate(RAX, ip->b.u64, CPUOp::IMUL, CPUBits::B64);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::Div64byVB64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Immediate(RCX, ip->b.u64);
                pp.emitClearN(RDX, CPUBits::B64);
                pp.emitOpN(RCX, RAX, CPUOp::IDIV, CPUBits::B64);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetImmediate32:
                pp.emitStore32Immediate(REG_OFFSET(ip->a.u32), ip->b.u32, RDI);
                break;
            case ByteCodeOp::SetImmediate64:
                if (ip->b.u64 <= 0x7FFFFFFF || ip->b.u64 >> 32 == 0xFFFFFFFF)
                    pp.emitStore64Immediate(REG_OFFSET(ip->a.u32), ip->b.u32, RDI);
                else
                {
                    pp.emitLoad64Immediate(RAX, ip->b.u64);
                    pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::MemCpy8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RDX, RDI);
                pp.emitCopyX(RCX, RDX, 1, 0);
                break;
            case ByteCodeOp::MemCpy16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RDX, RDI);
                pp.emitCopyX(RCX, RDX, 2, 0);
                break;
            case ByteCodeOp::MemCpy32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RDX, RDI);
                pp.emitCopyX(RCX, RDX, 4, 0);
                break;
            case ByteCodeOp::MemCpy64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RDX, RDI);
                pp.emitCopyX(RCX, RDX, 8, 0);
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMemCpy:
                if (ip->hasFlag(BCI_IMM_C) && ip->c.u64 <= 128 && !buildParameters.isDebug())
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                    pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RDX, RDI);
                    pp.emitCopyX(RCX, RDX, ip->c.u32, 0);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->a.u32});
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
                    if (ip->hasFlag(BCI_IMM_C))
                        pp.pushParams.push_back({CPUPushParamType::Imm, ip->c.u64});
                    else
                        pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});
                    emitInternalCallExt(pp, g_LangSpec->name_memcpy, pp.pushParams);
                }
                break;
            case ByteCodeOp::IntrinsicMemSet:
                if (ip->hasFlag(BCI_IMM_B) && ip->hasFlag(BCI_IMM_C) && ip->b.u8 == 0 && ip->c.u64 <= 128 && !buildParameters.isDebug())
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                    pp.emitClearX(ip->c.u32, 0, RCX);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->a.u32});
                    if (ip->hasFlag(BCI_IMM_B))
                        pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u8});
                    else
                        pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
                    if (ip->hasFlag(BCI_IMM_C))
                        pp.pushParams.push_back({CPUPushParamType::Imm, ip->c.u64});
                    else
                        pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});
                    emitInternalCallExt(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            case ByteCodeOp::IntrinsicMemMove:
                pp.pushParams.clear();
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->a.u32});
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
                if (ip->hasFlag(BCI_IMM_C))
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->c.u64});
                else
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});
                emitInternalCallExt(pp, g_LangSpec->name_memmove, pp.pushParams);
                break;
            case ByteCodeOp::IntrinsicMemCmp:
                pp.pushParams.clear();
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});
                if (ip->hasFlag(BCI_IMM_D))
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->d.u64});
                else
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->d.u32});
                emitInternalCallExt(pp, g_LangSpec->name_memcmp, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;

            case ByteCodeOp::IntrinsicStrLen:
                emitInternalCall(pp, g_LangSpec->name_strlen, {ip->b.u32}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicStrCmp:
                emitInternalCall(pp, g_LangSpec->name_strcmp, {ip->b.u32, ip->c.u32}, REG_OFFSET(ip->a.u32));
                break;

            case ByteCodeOp::IntrinsicAlloc:
                emitInternalCall(pp, g_LangSpec->name_malloc, {ip->b.u32}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicRealloc:
                emitInternalCall(pp, g_LangSpec->name_realloc, {ip->b.u32, ip->c.u32}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicFree:
                emitInternalCall(pp, g_LangSpec->name_free, {ip->a.u32});
                break;

            case ByteCodeOp::InternalPanic:
                emitInternalPanic(pp, ip->node, reinterpret_cast<const char*>(ip->d.pointer));
                break;
            case ByteCodeOp::InternalUnreachable:
                break;
            case ByteCodeOp::Unreachable:
                emitInternalPanic(pp, ip->node, "executing unreachable code");
                break;

            case ByteCodeOp::InternalGetTlsPtr:
                pp.pushParams.clear();
                pp.pushParams.push_back({CPUPushParamType::RelocV, pp.symTls_threadLocalId});
                pp.pushParams.push_back({CPUPushParamType::Imm64, module->tlsSegment.totalCount});
                pp.pushParams.push_back({CPUPushParamType::RelocAddr, pp.symTLSIndex});
                emitInternalCallExt(pp, g_LangSpec->name_priv_tlsGetPtr, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicGetContext:
                pp.pushParams.clear();
                pp.pushParams.push_back({CPUPushParamType::RelocV, pp.symPI_contextTlsId});
                emitInternalCallExt(pp, g_LangSpec->name_priv_tlsGetValue, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicSetContext:
                pp.pushParams.clear();
                pp.pushParams.push_back({CPUPushParamType::RelocV, pp.symPI_contextTlsId});
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->a.u32});
                emitInternalCallExt(pp, g_LangSpec->name_priv_tlsSetValue, pp.pushParams);
                break;

            case ByteCodeOp::IntrinsicGetProcessInfos:
                pp.emitSymbolRelocationAddr(RCX, pp.symPI_processInfos, 0);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                break;

            case ByteCodeOp::IntrinsicCVaStart:
            {
                uint32_t paramIdx = typeFunc->numParamsRegisters();
                if (CallConv::returnByAddress(typeFunc))
                    paramIdx += 1;
                uint32_t stackOffset = cpuFct->offsetCallerStackParams + REG_OFFSET(paramIdx);
                pp.emitLoadAddressIndirect(stackOffset, RAX, RDI);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore64Indirect(0, RAX, RCX);
                break;
            }
            case ByteCodeOp::IntrinsicCVaEnd:
                break;
            case ByteCodeOp::IntrinsicCVaArg:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(0, RCX, RAX);
                switch (ip->c.u32)
                {
                    case 1:
                        pp.emitClearN(RDX, CPUBits::B64);
                        pp.emitLoad8Indirect(0, RDX, RCX);
                        break;
                    case 2:
                        pp.emitClearN(RDX, CPUBits::B64);
                        pp.emitLoad16Indirect(0, RDX, RCX);
                        break;
                    case 4:
                        pp.emitLoad32Indirect(0, RDX, RCX);
                        break;
                    case 8:
                        pp.emitLoad64Indirect(0, RDX, RCX);
                        break;
                    default:
                        break;
                }

                pp.emitStore64Indirect(REG_OFFSET(ip->b.u32), RDX, RDI);
                pp.emitLoad64Immediate(RCX, 8);
                pp.emitOpNIndirect(0, RCX, RAX, CPUOp::ADD, CPUBits::B64);
                break;

            case ByteCodeOp::IntrinsicArguments:
                SWAG_ASSERT(ip->b.u32 == ip->a.u32 + 1);
                emitInternalCall(pp, g_LangSpec->name_at_args, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicModules:
                if (buildParameters.module->modulesSliceOffset == UINT32_MAX)
                {
                    pp.emitLoadAddressIndirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                    pp.emitStore64Immediate(0, 0, RAX);
                    pp.emitLoadAddressIndirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore64Immediate(0, 0, RAX);
                }
                else
                {
                    pp.emitSymbolRelocationAddr(RAX, pp.symCSIndex, buildParameters.module->modulesSliceOffset);
                    pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                    pp.emitLoadAddressIndirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore64Immediate(0, buildParameters.module->moduleDependencies.count + 1, RAX);
                }
                break;
            case ByteCodeOp::IntrinsicGvtd:
                if (buildParameters.module->globalVarsToDropSliceOffset == UINT32_MAX)
                {
                    pp.emitLoadAddressIndirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                    pp.emitStore64Immediate(0, 0, RAX);
                    pp.emitLoadAddressIndirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore64Immediate(0, 0, RAX);
                }
                else
                {
                    pp.emitSymbolRelocationAddr(RAX, pp.symMSIndex, buildParameters.module->globalVarsToDropSliceOffset);
                    pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                    pp.emitLoadAddressIndirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                    pp.emitStore64Immediate(0, buildParameters.module->globalVarsToDrop.count, RAX);
                }
                break;

            case ByteCodeOp::IntrinsicCompiler:
                pp.emitLoadAddressIndirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitStore64Immediate(0, 0, RAX);
                pp.emitLoadAddressIndirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitStore64Immediate(0, 0, RAX);
                break;
            case ByteCodeOp::IntrinsicIsByteCode:
                pp.emitLoadAddressIndirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitStore32Immediate(0, 0, RAX);
                break;
            case ByteCodeOp::IntrinsicCompilerError:
                emitInternalCall(pp, g_LangSpec->name_at_compilererror, {ip->a.u32, ip->b.u32, ip->c.u32});
                break;
            case ByteCodeOp::IntrinsicCompilerWarning:
                emitInternalCall(pp, g_LangSpec->name_at_compilerwarning, {ip->a.u32, ip->b.u32, ip->c.u32});
                break;
            case ByteCodeOp::IntrinsicPanic:
                emitInternalCall(pp, g_LangSpec->name_at_panic, {ip->a.u32, ip->b.u32, ip->c.u32});
                break;
            case ByteCodeOp::IntrinsicItfTableOf:
                emitInternalCall(pp, g_LangSpec->name_at_itftableof, {ip->a.u32, ip->b.u32}, REG_OFFSET(ip->c.u32));
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyRAtoRR:
                pp.emitLoadAddressIndirect(offsetResult, RAX, RDI);
                if (ip->hasFlag(BCI_IMM_A) && ip->a.u64 <= 0x7FFFFFFF)
                {
                    pp.emitStore64Immediate(0, ip->a.u64, RAX);
                }
                else if (ip->hasFlag(BCI_IMM_A))
                {
                    pp.emitLoad64Immediate(RCX, ip->a.u64);
                    pp.emitStore64Indirect(0, RCX, RAX);
                }
                else
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                    pp.emitStore64Indirect(0, RCX, RAX);
                }

                break;

            case ByteCodeOp::CopyRARBtoRR2:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad64Indirect(stackOffset, RAX, RDI);
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitStore64Indirect(0, RCX, RAX);
                pp.emitLoad64Indirect(stackOffset, RAX, RDI);
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RCX, RDI);
                pp.emitStore64Indirect(8, RCX, RAX);
                break;
            }

            case ByteCodeOp::CopyRAtoRT:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitStore64Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
                break;

            case ByteCodeOp::SaveRRtoRA:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad64Indirect(stackOffset, RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            }
            case ByteCodeOp::CopyRRtoRA:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad64Indirect(stackOffset, RAX, RDI);
                if (ip->b.u64)
                {
                    pp.emitLoad64Immediate(RCX, ip->b.u64);
                    pp.emitOpN(RCX, RAX, CPUOp::ADD, CPUBits::B64);
                }

                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            }

            case ByteCodeOp::CopyRTtoRA:
                pp.emitLoad64Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::CopyRT2toRARB:
                pp.emitLoad64Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Indirect(offsetRT + REG_OFFSET(1), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                break;

            case ByteCodeOp::CopySP:
                pp.emitLoadAddressIndirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;

            case ByteCodeOp::CopySPVaargs:
            {
                auto typeFuncCall = castTypeInfo<TypeInfoFuncAttr>(reinterpret_cast<TypeInfo*>(ip->d.pointer), TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                if (!pushRVParams.empty())
                {
                    auto     sizeOf   = pushRVParams[0].second;
                    uint32_t idxParam = pushRVParams.size() - 1;

                    uint32_t variadicStackSize = idxParam * sizeOf;
                    MK_ALIGN16(variadicStackSize);
                    uint32_t offset = sizeParamsStack - variadicStackSize;

                    while (idxParam != UINT32_MAX)
                    {
                        auto reg = pushRVParams[idxParam].first;
                        switch (sizeOf)
                        {
                            case 1:
                                pp.emitLoad8Indirect(REG_OFFSET(reg), RAX, RDI);
                                pp.emitStore8Indirect(offset, RAX, RSP);
                                break;
                            case 2:
                                pp.emitLoad16Indirect(REG_OFFSET(reg), RAX, RDI);
                                pp.emitStore16Indirect(offset, RAX, RSP);
                                break;
                            case 4:
                                pp.emitLoad32Indirect(REG_OFFSET(reg), RAX, RDI);
                                pp.emitStore32Indirect(offset, RAX, RSP);
                                break;
                            case 8:
                                pp.emitLoad64Indirect(REG_OFFSET(reg), RAX, RDI);
                                pp.emitStore64Indirect(offset, RAX, RSP);
                                break;
                            default:
                                break;
                        }

                        idxParam--;
                        offset += sizeOf;
                    }

                    pp.emitLoadAddressIndirect(sizeParamsStack - variadicStackSize, RAX, RSP);
                    pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                    uint32_t idxParam          = pushRAParams.size() - sizeB / sizeof(Register) - 1;
                    uint32_t variadicStackSize = (idxParam + 1) * sizeof(Register);
                    MK_ALIGN16(variadicStackSize);
                    uint32_t offset = sizeParamsStack - variadicStackSize;
                    while (idxParam != UINT32_MAX)
                    {
                        pp.emitLoad64Indirect(REG_OFFSET(pushRAParams[idxParam]), RAX, RDI);
                        pp.emitStore64Indirect(offset, RAX, RSP);
                        idxParam--;
                        offset += 8;
                    }

                    pp.emitLoadAddressIndirect(sizeParamsStack - variadicStackSize, RAX, RSP);
                    pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 1);
                break;
            case ByteCodeOp::GetParam16:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 2);
                break;
            case ByteCodeOp::GetParam32:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 4);
                break;
            case ByteCodeOp::GetParam64:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8);
                break;
            case ByteCodeOp::GetParam64x2:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8);
                emitGetParam(pp, cpuFct, ip->c.u32, ip->d.mergeU64U32.high, 8);
                break;
            case ByteCodeOp::GetIncParam64:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8, ip->d.u64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetParam64DeRef8:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8, 0, 1);
                break;
            case ByteCodeOp::GetParam64DeRef16:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8, 0, 2);
                break;
            case ByteCodeOp::GetParam64DeRef32:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8, 0, 4);
                break;
            case ByteCodeOp::GetParam64DeRef64:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8, 0, 8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetIncParam64DeRef8:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8, ip->d.u64, 1);
                break;
            case ByteCodeOp::GetIncParam64DeRef16:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8, ip->d.u64, 2);
                break;
            case ByteCodeOp::GetIncParam64DeRef32:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8, ip->d.u64, 4);
                break;
            case ByteCodeOp::GetIncParam64DeRef64:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, 8, ip->d.u64, 8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeLambda:
            {
                auto funcNode = castAst<AstFuncDecl>(reinterpret_cast<AstNode*>(ip->b.pointer), AstNodeKind::FuncDecl);
                SWAG_ASSERT(!ip->c.pointer || (funcNode && funcNode->hasExtByteCode() && funcNode->extByteCode()->bc == reinterpret_cast<ByteCode*>(ip->c.pointer)));
                Utf8 callName = funcNode->getCallName();
                pp.emitLoad64Immediate(RAX, 0, true);

                CPURelocation reloc;
                reloc.virtualAddress = concat.totalCount() - sizeof(uint64_t) - pp.textSectionOffset;
                auto callSym         = pp.getOrAddSymbol(callName, CPUSymbolKind::Extern);
                reloc.symbolIndex    = callSym->index;
                reloc.type           = IMAGE_REL_AMD64_ADDR64;
                pp.relocTableTextSection.table.push_back(reloc);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                break;
            }

            case ByteCodeOp::IntrinsicMakeCallback:
            {
                // Test if it's a bytecode lambda
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad64Immediate(RCX, SWAG_LAMBDA_BC_MARKER);
                pp.emitOpN(RAX, RCX, CPUOp::AND, CPUBits::B64);
                pp.emitTestN(RCX, RCX, CPUBits::B64);
                auto jumpBCToAfterAddr   = pp.emitLongJumpOp(JZ);
                auto jumpBCToAfterOffset = concat.totalCount();

                // ByteCode lambda
                //////////////////

                pp.emitCopyN(RCX, RAX, CPUBits::B64);
                pp.emitSymbolRelocationAddr(RAX, pp.symPI_makeCallback, 0);
                pp.emitLoad64Indirect(0, RAX, RAX);
                pp.emitCallIndirect(RAX);

                // End
                //////////////////
                *jumpBCToAfterAddr = concat.totalCount() - jumpBCToAfterOffset;
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);

                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::LocalCall:
            case ByteCodeOp::LocalCallPop:
            case ByteCodeOp::LocalCallPop16:
            case ByteCodeOp::LocalCallPop48:
            case ByteCodeOp::LocalCallPopRC:
                emitLocalCall(pp, offsetRT, ip, pushRAParams, pushRVParams);
                break;
            case ByteCodeOp::LocalCallPopParam:
                pushRAParams.push_back(ip->d.u32);
                emitLocalCall(pp, offsetRT, ip, pushRAParams, pushRVParams);
                break;
            case ByteCodeOp::LocalCallPop0Param2:
            case ByteCodeOp::LocalCallPop16Param2:
            case ByteCodeOp::LocalCallPop48Param2:
                pushRAParams.push_back(ip->c.u32);
                pushRAParams.push_back(ip->d.u32);
                emitLocalCall(pp, offsetRT, ip, pushRAParams, pushRVParams);
                break;

            case ByteCodeOp::ForeignCall:
            case ByteCodeOp::ForeignCallPop:
                emitForeignCall(pp, offsetRT, ip, pushRAParams, pushRVParams);
                break;
            case ByteCodeOp::ForeignCallPopParam:
                pushRAParams.push_back(ip->d.u32);
                emitForeignCall(pp, offsetRT, ip, pushRAParams, pushRVParams);
                break;
            case ByteCodeOp::ForeignCallPop0Param2:
            case ByteCodeOp::ForeignCallPop16Param2:
            case ByteCodeOp::ForeignCallPop48Param2:
                pushRAParams.push_back(ip->c.u32);
                pushRAParams.push_back(ip->d.u32);
                emitForeignCall(pp, offsetRT, ip, pushRAParams, pushRVParams);
                break;

            case ByteCodeOp::LambdaCall:
            case ByteCodeOp::LambdaCallPop:
                emitLambdaCall(pp, concat, offsetRT, ip, pushRAParams, pushRVParams);
                break;
            case ByteCodeOp::LambdaCallPopParam:
                pushRAParams.push_back(ip->d.u32);
                emitLambdaCall(pp, concat, offsetRT, ip, pushRAParams, pushRVParams);
                break;

                /////////////////////////////////////

            case ByteCodeOp::IncSPPostCall:
            case ByteCodeOp::IncSPPostCallCond:
                pushRAParams.clear();
                pushRVParams.clear();
                break;

            case ByteCodeOp::CopyRAtoRRRet:
                pp.emitLoadAddressIndirect(offsetResult, RAX, RDI);
                if (ip->hasFlag(BCI_IMM_A) && ip->a.u64 <= 0x7FFFFFFF)
                {
                    pp.emitStore64Immediate(0, ip->a.u64, RAX);
                }
                else if (ip->hasFlag(BCI_IMM_A))
                {
                    pp.emitLoad64Immediate(RCX, ip->a.u64);
                    pp.emitStore64Indirect(0, RCX, RAX);
                }
                else
                {
                    pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                    pp.emitStore64Indirect(0, RCX, RAX);
                }
                [[fallthrough]];

            case ByteCodeOp::Ret:

                // Emit result
                if (!returnType->isVoid() && !CallConv::returnByAddress(typeFunc))
                {
                    pp.emitLoad64Indirect(offsetResult, cc.returnByRegisterInteger, RDI);
                    if (returnType->isNative(NativeTypeKind::F32))
                        pp.emitCopyF32(cc.returnByRegisterFloat, RAX);
                    else if (returnType->isNative(NativeTypeKind::F64))
                        pp.emitCopyF64(cc.returnByRegisterFloat, RAX);
                }

                pp.emitOpNImmediate(RSP, sizeStack + sizeParamsStack, CPUOp::ADD, CPUBits::B64);
                for (int32_t rRet = static_cast<int32_t>(unwindRegs.size()) - 1; rRet >= 0; rRet--)
                    pp.emitPop(unwindRegs[rRet]);
                pp.emitRet();
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMulAddF32:
            {
                MK_IMMB_F32(XMM0);
                MK_IMMC_F32(XMM1);
                MK_IMMD_F32(XMM2);
                pp.emitMulAddF32(XMM0, XMM1, XMM2);
                pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            }
            case ByteCodeOp::IntrinsicMulAddF64:
            {
                MK_IMMB_F64(XMM0);
                MK_IMMC_F64(XMM1);
                MK_IMMD_F64(XMM2);
                pp.emitMulAddF64(XMM0, XMM1, XMM2);
                pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                break;
            }

            case ByteCodeOp::IntrinsicAtomicAddS8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad8Indirect(0, RAX, RCX);
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ8_LOCK_CAB(CPUOp::ADD);
                break;
            case ByteCodeOp::IntrinsicAtomicAddS16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad16Indirect(0, RAX, RCX);
                pp.emitStore16Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ16_LOCK_CAB(CPUOp::ADD);
                break;
            case ByteCodeOp::IntrinsicAtomicAddS32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad32Indirect(0, RAX, RCX);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ32_LOCK_CAB(CPUOp::ADD);
                break;
            case ByteCodeOp::IntrinsicAtomicAddS64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad64Indirect(0, RAX, RCX);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ64_LOCK_CAB(CPUOp::ADD);
                break;

            case ByteCodeOp::IntrinsicAtomicAndS8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad8Indirect(0, RAX, RCX);
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ8_LOCK_CAB(CPUOp::AND);
                break;
            case ByteCodeOp::IntrinsicAtomicAndS16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad16Indirect(0, RAX, RCX);
                pp.emitStore16Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ16_LOCK_CAB(CPUOp::AND);
                break;
            case ByteCodeOp::IntrinsicAtomicAndS32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad32Indirect(0, RAX, RCX);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ32_LOCK_CAB(CPUOp::AND);
                break;
            case ByteCodeOp::IntrinsicAtomicAndS64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad64Indirect(0, RAX, RCX);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ64_LOCK_CAB(CPUOp::AND);
                break;

            case ByteCodeOp::IntrinsicAtomicOrS8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad8Indirect(0, RAX, RCX);
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ8_LOCK_CAB(CPUOp::OR);
                break;
            case ByteCodeOp::IntrinsicAtomicOrS16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad16Indirect(0, RAX, RCX);
                pp.emitStore16Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ16_LOCK_CAB(CPUOp::OR);
                break;
            case ByteCodeOp::IntrinsicAtomicOrS32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad32Indirect(0, RAX, RCX);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ32_LOCK_CAB(CPUOp::OR);
                break;
            case ByteCodeOp::IntrinsicAtomicOrS64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad64Indirect(0, RAX, RCX);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ64_LOCK_CAB(CPUOp::OR);
                break;

            case ByteCodeOp::IntrinsicAtomicXorS8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad8Indirect(0, RAX, RCX);
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ8_LOCK_CAB(CPUOp::XOR);
                break;
            case ByteCodeOp::IntrinsicAtomicXorS16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad16Indirect(0, RAX, RCX);
                pp.emitStore16Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ16_LOCK_CAB(CPUOp::XOR);
                break;
            case ByteCodeOp::IntrinsicAtomicXorS32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad32Indirect(0, RAX, RCX);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ32_LOCK_CAB(CPUOp::XOR);
                break;
            case ByteCodeOp::IntrinsicAtomicXorS64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad64Indirect(0, RAX, RCX);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ64_LOCK_CAB(CPUOp::XOR);
                break;

            case ByteCodeOp::IntrinsicAtomicXchgS8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad8Indirect(0, RAX, RCX);
                pp.emitStore8Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ8_LOCK_CAB(CPUOp::XCHG);
                break;
            case ByteCodeOp::IntrinsicAtomicXchgS16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad16Indirect(0, RAX, RCX);
                pp.emitStore16Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ16_LOCK_CAB(CPUOp::XCHG);
                break;
            case ByteCodeOp::IntrinsicAtomicXchgS32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad32Indirect(0, RAX, RCX);
                pp.emitStore32Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ32_LOCK_CAB(CPUOp::XCHG);
                break;
            case ByteCodeOp::IntrinsicAtomicXchgS64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                pp.emitLoad64Indirect(0, RAX, RCX);
                pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
                MK_BINOP_EQ64_LOCK_CAB(CPUOp::XCHG);
                break;

            case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_IMMB_8(RAX);
                MK_IMMC_8(RDX);
                pp.emitCmpXChg(RCX, RDX, CPUBits::B8);
                pp.emitStore8Indirect(REG_OFFSET(ip->d.u32), RAX, RDI);
                break;
            case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_IMMB_16(RAX);
                MK_IMMC_16(RDX);
                pp.emitCmpXChg(RCX, RDX, CPUBits::B16);
                pp.emitStore16Indirect(REG_OFFSET(ip->d.u32), RAX, RDI);
                break;
            case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_IMMB_32(RAX);
                MK_IMMC_32(RDX);
                pp.emitCmpXChg(RCX, RDX, CPUBits::B32);
                pp.emitStore32Indirect(REG_OFFSET(ip->d.u32), RAX, RDI);
                break;
            case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                MK_IMMB_64(RAX);
                MK_IMMC_64(RDX);
                pp.emitCmpXChg(RCX, RDX, CPUBits::B64);
                pp.emitStore64Indirect(REG_OFFSET(ip->d.u32), RAX, RDI);
                break;

            case ByteCodeOp::IntrinsicS8x1:
            {
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        MK_IMMB_8(RAX);
                        pp.emitCopyN(RCX, RAX, CPUBits::B8);
                        pp.emitOpNImmediate(RCX, 7, CPUOp::SAR, CPUBits::B8);
                        pp.emitOpN(RCX, RAX, CPUOp::XOR, CPUBits::B8);
                        pp.emitOpN(RCX, RAX, CPUOp::SUB, CPUBits::B8);
                        pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        MK_IMMB_U8_TO_U32(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::POPCNT, CPUBits::B32);
                        pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        MK_IMMB_U8_TO_U32(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::BSF, CPUBits::B32);
                        pp.emitLoad32Immediate(RCX, 8);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B32);
                        pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        MK_IMMB_U8_TO_U32(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::BSR, CPUBits::B32);
                        pp.emitLoad32Immediate(RCX, 15);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B32);
                        pp.emitOpNImmediate(RAX, 7, CPUOp::XOR, CPUBits::B32);
                        pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        MK_IMMB_16(RAX);
                        pp.emitCopyN(RCX, RAX, CPUBits::B16);
                        pp.emitOpNImmediate(RCX, 15, CPUOp::SAR, CPUBits::B16);
                        pp.emitOpN(RCX, RAX, CPUOp::XOR, CPUBits::B16);
                        pp.emitOpN(RCX, RAX, CPUOp::SUB, CPUBits::B16);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        MK_IMMB_16(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::POPCNT, CPUBits::B16);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        MK_IMMB_16(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::BSF, CPUBits::B16);
                        pp.emitLoad16Immediate(RCX, 16);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B16);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        MK_IMMB_16(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::BSR, CPUBits::B16);
                        pp.emitLoad16Immediate(RCX, 31);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B16);
                        pp.emitOpNImmediate(RAX, 15, CPUOp::XOR, CPUBits::B16);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        MK_IMMB_16(RAX);
                        pp.emitBSwapN(RAX, CPUBits::B16);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        MK_IMMB_32(RAX);
                        pp.emitCopyN(RCX, RAX, CPUBits::B32);
                        pp.emitOpNImmediate(RCX, 31, CPUOp::SAR, CPUBits::B32);
                        pp.emitOpN(RCX, RAX, CPUOp::XOR, CPUBits::B32);
                        pp.emitOpN(RCX, RAX, CPUOp::SUB, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        MK_IMMB_32(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::POPCNT, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        MK_IMMB_32(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::BSF, CPUBits::B32);
                        pp.emitLoad32Immediate(RCX, 32);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        MK_IMMB_32(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::BSR, CPUBits::B32);
                        pp.emitLoad32Immediate(RCX, 63);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B32);
                        pp.emitOpNImmediate(RAX, 31, CPUOp::XOR, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        MK_IMMB_32(RAX);
                        pp.emitBSwapN(RAX, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        MK_IMMB_64(RAX);
                        pp.emitCopyN(RCX, RAX, CPUBits::B64);
                        pp.emitOpNImmediate(RCX, 63, CPUOp::SAR, CPUBits::B64);
                        pp.emitOpN(RCX, RAX, CPUOp::XOR, CPUBits::B64);
                        pp.emitOpN(RCX, RAX, CPUOp::SUB, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        MK_IMMB_64(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::POPCNT, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        MK_IMMB_64(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::BSF, CPUBits::B64);
                        pp.emitLoad32Immediate(RCX, 64);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        MK_IMMB_64(RAX);
                        pp.emitOpN(RAX, RAX, CPUOp::BSR, CPUBits::B64);
                        pp.emitLoad64Immediate(RCX, 127);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B64);
                        pp.emitOpNImmediate(RAX, 63, CPUOp::XOR, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        MK_IMMB_64(RAX);
                        pp.emitBSwapN(RAX, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        MK_IMMB_S8_TO_S32(RAX);
                        MK_IMMC_S8_TO_S32(RCX);
                        pp.emitCmpN(RCX, RAX, CPUBits::B32);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicMax:
                        MK_IMMB_S8_TO_S32(RAX);
                        MK_IMMC_S8_TO_S32(RCX);
                        pp.emitCmpN(RAX, RCX, CPUBits::B32);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        MK_IMMB_16(RAX);
                        MK_IMMC_16(RCX);
                        pp.emitCmpN(RCX, RAX, CPUBits::B16);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicMax:
                        MK_IMMB_16(RAX);
                        MK_IMMC_16(RCX);
                        pp.emitCmpN(RAX, RCX, CPUBits::B16);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        MK_IMMB_32(RAX);
                        MK_IMMC_32(RCX);
                        pp.emitCmpN(RCX, RAX, CPUBits::B32);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicMax:
                        MK_IMMB_32(RAX);
                        MK_IMMC_32(RCX);
                        pp.emitCmpN(RAX, RCX, CPUBits::B32);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        MK_IMMB_64(RAX);
                        MK_IMMC_64(RCX);
                        pp.emitCmpN(RCX, RAX, CPUBits::B64);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicMax:
                        MK_IMMB_64(RAX);
                        MK_IMMC_64(RCX);
                        pp.emitCmpN(RAX, RCX, CPUBits::B64);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        MK_IMMB_U8_TO_U32(RAX);
                        MK_IMMC_U8_TO_U32(RCX);
                        pp.emitCmpN(RCX, RAX, CPUBits::B32);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B32);
                        pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicMax:
                        MK_IMMB_U8_TO_U32(RAX);
                        MK_IMMC_U8_TO_U32(RCX);
                        pp.emitCmpN(RAX, RCX, CPUBits::B32);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B32);
                        pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicRol:
                        MK_IMMB_S8_TO_S32(RAX);
                        MK_IMMC_S8_TO_S32(RCX);
                        pp.emitRotateN(RAX, RCX, CPUOp::ROL, CPUBits::B32);
                        pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicRor:
                        MK_IMMB_S8_TO_S32(RAX);
                        MK_IMMC_S8_TO_S32(RCX);
                        pp.emitRotateN(RAX, RCX, CPUOp::ROR, CPUBits::B32);
                        pp.emitStore8Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        MK_IMMB_16(RAX);
                        MK_IMMC_16(RCX);
                        pp.emitCmpN(RCX, RAX, CPUBits::B16);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B16);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicMax:
                        MK_IMMB_16(RAX);
                        MK_IMMC_16(RCX);
                        pp.emitCmpN(RAX, RCX, CPUBits::B16);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B16);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicRol:
                        MK_IMMB_16(RAX);
                        MK_IMMC_16(RCX);
                        pp.emitRotateN(RAX, RCX, CPUOp::ROL, CPUBits::B32);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicRor:
                        MK_IMMB_16(RAX);
                        MK_IMMC_16(RCX);
                        pp.emitRotateN(RAX, RCX, CPUOp::ROR, CPUBits::B32);
                        pp.emitStore16Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        MK_IMMB_32(RAX);
                        MK_IMMC_32(RCX);
                        pp.emitCmpN(RCX, RAX, CPUBits::B32);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicMax:
                        MK_IMMB_32(RAX);
                        MK_IMMC_32(RCX);
                        pp.emitCmpN(RAX, RCX, CPUBits::B32);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicRol:
                        MK_IMMB_32(RAX);
                        MK_IMMC_32(RCX);
                        pp.emitRotateN(RAX, RCX, CPUOp::ROL, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicRor:
                        MK_IMMB_32(RAX);
                        MK_IMMC_32(RCX);
                        pp.emitRotateN(RAX, RCX, CPUOp::ROR, CPUBits::B32);
                        pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        MK_IMMB_64(RAX);
                        MK_IMMC_64(RCX);
                        pp.emitCmpN(RCX, RAX, CPUBits::B64);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicMax:
                        MK_IMMB_64(RAX);
                        MK_IMMC_64(RCX);
                        pp.emitCmpN(RAX, RCX, CPUBits::B64);
                        pp.emitCMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicRol:
                        MK_IMMB_64(RAX);
                        MK_IMMC_64(RCX);
                        pp.emitRotateN(RAX, RCX, CPUOp::ROL, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                        break;
                    case TokenId::IntrinsicRor:
                        MK_IMMB_64(RAX);
                        MK_IMMC_64(RCX);
                        pp.emitRotateN(RAX, RCX, CPUOp::ROR, CPUBits::B64);
                        pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
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
                pp.pushParams.clear();
                if (ip->hasFlag(BCI_IMM_B))
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u32});
                else
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
                if (ip->hasFlag(BCI_IMM_C))
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->c.u32});
                else
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});

                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        MK_IMMB_F32(XMM0);
                        MK_IMMC_F32(XMM1);
                        pp.emitOpF32(XMM0, XMM1, CPUOp::FMIN);
                        pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                        break;
                    case TokenId::IntrinsicMax:
                        MK_IMMB_F32(XMM0);
                        MK_IMMC_F32(XMM1);
                        pp.emitOpF32(XMM0, XMM1, CPUOp::FMAX);
                        pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                        break;

                    case TokenId::IntrinsicPow:
                        emitInternalCallExt(pp, g_LangSpec->name_powf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicATan2:
                        emitInternalCallExt(pp, g_LangSpec->name_atan2f, pp.pushParams, REG_OFFSET(ip->a.u32));
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
                pp.pushParams.clear();
                if (ip->hasFlag(BCI_IMM_B))
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u64});
                else
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
                if (ip->hasFlag(BCI_IMM_C))
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->c.u64});
                else
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});

                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        MK_IMMB_F64(XMM0);
                        MK_IMMC_F64(XMM1);
                        pp.emitOpF64(XMM0, XMM1, CPUOp::FMIN);
                        pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                        break;
                    case TokenId::IntrinsicMax:
                        MK_IMMB_F64(XMM0);
                        MK_IMMC_F64(XMM1);
                        pp.emitOpF64(XMM0, XMM1, CPUOp::FMAX);
                        pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                        break;

                    case TokenId::IntrinsicPow:
                        emitInternalCallExt(pp, g_LangSpec->name_pow, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicATan2:
                        emitInternalCallExt(pp, g_LangSpec->name_atan2, pp.pushParams, REG_OFFSET(ip->a.u32));
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
                pp.pushParams.clear();
                if (ip->hasFlag(BCI_IMM_B))
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u32});
                else
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});

                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicSqrt:
                        MK_IMMB_F32(XMM0);
                        pp.emitOpF32(XMM0, XMM0, CPUOp::FSQRT);
                        pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                        break;
                    case TokenId::IntrinsicAbs:
                        MK_IMMB_F32(XMM0);
                        pp.emitLoad64Immediate(RAX, 0x7FFFFFFF);
                        pp.emitCopyF64(XMM1, RAX);
                        pp.emitOpF32(XMM0, XMM1, CPUOp::FAND);
                        pp.emitStoreF32Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                        break;

                    case TokenId::IntrinsicSin:
                        emitInternalCallExt(pp, g_LangSpec->name_sinf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCos:
                        emitInternalCallExt(pp, g_LangSpec->name_cosf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTan:
                        emitInternalCallExt(pp, g_LangSpec->name_tanf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicSinh:
                        emitInternalCallExt(pp, g_LangSpec->name_sinhf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCosh:
                        emitInternalCallExt(pp, g_LangSpec->name_coshf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTanh:
                        emitInternalCallExt(pp, g_LangSpec->name_tanhf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicASin:
                        emitInternalCallExt(pp, g_LangSpec->name_asinf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicACos:
                        emitInternalCallExt(pp, g_LangSpec->name_acosf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicATan:
                        emitInternalCallExt(pp, g_LangSpec->name_atanf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog:
                        emitInternalCallExt(pp, g_LangSpec->name_logf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog2:
                        emitInternalCallExt(pp, g_LangSpec->name_log2f, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog10:
                        emitInternalCallExt(pp, g_LangSpec->name_log10f, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicFloor:
                        emitInternalCallExt(pp, g_LangSpec->name_floorf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCeil:
                        emitInternalCallExt(pp, g_LangSpec->name_ceilf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTrunc:
                        emitInternalCallExt(pp, g_LangSpec->name_truncf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicRound:
                        emitInternalCallExt(pp, g_LangSpec->name_roundf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicExp:
                        emitInternalCallExt(pp, g_LangSpec->name_expf, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicExp2:
                        emitInternalCallExt(pp, g_LangSpec->name_exp2f, pp.pushParams, REG_OFFSET(ip->a.u32));
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
                pp.pushParams.clear();
                if (ip->hasFlag(BCI_IMM_B))
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u64});
                else
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});

                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicSqrt:
                        MK_IMMB_F64(XMM0);
                        pp.emitOpF64(XMM0, XMM0, CPUOp::FSQRT);
                        pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                        break;
                    case TokenId::IntrinsicAbs:
                        MK_IMMB_F64(XMM0);
                        pp.emitLoad64Immediate(RAX, 0x7FFFFFFF'FFFFFFFF);
                        pp.emitCopyF64(XMM1, RAX);
                        pp.emitOpF64(XMM0, XMM1, CPUOp::FAND);
                        pp.emitStoreF64Indirect(REG_OFFSET(ip->a.u32), XMM0, RDI);
                        break;

                    case TokenId::IntrinsicSin:
                        emitInternalCallExt(pp, g_LangSpec->name_sin, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCos:
                        emitInternalCallExt(pp, g_LangSpec->name_cos, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTan:
                        emitInternalCallExt(pp, g_LangSpec->name_tan, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicSinh:
                        emitInternalCallExt(pp, g_LangSpec->name_sinh, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCosh:
                        emitInternalCallExt(pp, g_LangSpec->name_cosh, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTanh:
                        emitInternalCallExt(pp, g_LangSpec->name_tanh, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicASin:
                        emitInternalCallExt(pp, g_LangSpec->name_asin, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicACos:
                        emitInternalCallExt(pp, g_LangSpec->name_acos, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicATan:
                        emitInternalCallExt(pp, g_LangSpec->name_atan, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog:
                        emitInternalCallExt(pp, g_LangSpec->name_log, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog2:
                        emitInternalCallExt(pp, g_LangSpec->name_log2, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog10:
                        emitInternalCallExt(pp, g_LangSpec->name_log10, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicFloor:
                        emitInternalCallExt(pp, g_LangSpec->name_floor, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCeil:
                        emitInternalCallExt(pp, g_LangSpec->name_ceil, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTrunc:
                        emitInternalCallExt(pp, g_LangSpec->name_trunc, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicRound:
                        emitInternalCallExt(pp, g_LangSpec->name_round, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicExp:
                        emitInternalCallExt(pp, g_LangSpec->name_exp, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicExp2:
                        emitInternalCallExt(pp, g_LangSpec->name_exp2, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }

                break;
            }

            case ByteCodeOp::InternalFailedAssume:
                emitInternalCall(pp, g_LangSpec->name_priv_failedAssume, {ip->a.u32});
                break;
            case ByteCodeOp::IntrinsicGetErr:
                SWAG_ASSERT(ip->b.u32 == ip->a.u32 + 1);
                emitInternalCall(pp, g_LangSpec->name_at_err, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::InternalSetErr:
                emitInternalCall(pp, g_LangSpec->name_priv_seterr, {ip->a.u32, ip->b.u32});
                break;
            case ByteCodeOp::InternalHasErr:
                pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emitLoad32Indirect(offsetof(SwagContext, hasError), RCX, RAX);
                pp.emitStore32Indirect(REG_OFFSET(ip->a.u32), RCX, RDI);
                break;
            case ByteCodeOp::JumpIfError:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(offsetof(SwagContext, hasError), RCX, RAX);
                pp.emitTestN(RCX, RCX, CPUBits::B32);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNoError:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitLoad32Indirect(offsetof(SwagContext, hasError), RCX, RAX);
                pp.emitTestN(RCX, RCX, CPUBits::B32);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::InternalPushErr:
                pp.emitCall(g_LangSpec->name_priv_pusherr);
                break;
            case ByteCodeOp::InternalPopErr:
                pp.emitCall(g_LangSpec->name_priv_poperr);
                break;
            case ByteCodeOp::InternalCatchErr:
                pp.emitCall(g_LangSpec->name_priv_catcherr);
                break;
            case ByteCodeOp::InternalInitStackTrace:
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emitStore32Immediate(offsetof(SwagContext, traceIndex), 0, RAX);
                break;
            case ByteCodeOp::InternalStackTraceConst:
                pp.emitSymbolRelocationAddr(RAX, pp.symCSIndex, ip->b.u32);
                pp.emitStore64Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                emitInternalCall(pp, g_LangSpec->name_priv_stackTrace, {ip->a.u32});
                break;
            case ByteCodeOp::InternalStackTrace:
                emitInternalCall(pp, g_LangSpec->name_priv_stackTrace, {ip->a.u32});
                break;

            default:
                ok = false;
                Report::internalError(buildParameters.module, form("unknown instruction [[%s]] during backend generation", ByteCode::opName(ip->op)));
                break;
        }
    }

    // Solve labels
    for (auto& toSolve : pp.labelsToSolve)
    {
        auto it = pp.labels.find(toSolve.ipDest);
        SWAG_ASSERT(it != pp.labels.end());

        auto relOffset                              = it->second - toSolve.currentOffset;
        *reinterpret_cast<uint32_t*>(toSolve.patch) = relOffset;
    }

    uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);
    pp.clearInstructionCache();

    return ok;
}
