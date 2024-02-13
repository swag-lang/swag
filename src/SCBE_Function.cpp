#include "pch.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Report.h"
#include "ModuleManager.h"
#include "SCBE.h"
#include "SCBE_Coff.h"
#include "SCBE_Macros.h"
#include "TypeManager.h"

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
    if (bc->node && (bc->node->hasAttribute(ATTRIBUTE_TEST_FUNC)) && (buildParameters.compileType != Test))
        return true;

    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *(SCBE_X64*) perThread[ct][precompileIndex];
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
            pp.directives += FMT("/EXPORT:%s ", funcName.c_str());
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
    while (cpuFct->numScratchRegs < min(cc.numScratchRegisters, min(cc.paramByRegisterCount, numTotalRegs)))
    {
        auto cpuReg = (CPURegister) (cc.firstScratchRegister + cpuFct->numScratchRegs);
        pp.emit_Push(cpuReg);
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
            pp.emit_Load64_Immediate(RAX, sizeStack + sizeParamsStack);
            pp.emit_Call("__chkstk");
        }
    }

    pp.emit_OpN_Immediate(RSP, sizeStack + sizeParamsStack, CPUOp::SUB, CPUBits::B64);

    // We need to start at sizeof(void*) because the call has pushed one register on the stack
    cpuFct->offsetCallerStackParams = (uint32_t) (sizeof(void*) + (unwindRegs.size() * sizeof(void*)) + sizeStack);
    cpuFct->offsetStack             = offsetStack;
    cpuFct->offsetLocalStackParams  = offsetS4;
    cpuFct->frameSize               = sizeStack + sizeParamsStack;

    // Unwind information (with the pushed registers)
    VectorNative<uint16_t> unwind;
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    computeUnwind(unwindRegs, unwindOffsetRegs, sizeStack + sizeParamsStack, sizeProlog, unwind);

    // Registers are stored after the sizeParamsStack area, which is used to store parameters for function calls
    pp.emit_LoadAddress_Indirect(sizeParamsStack, RDI, RSP);

    // Save register parameters
    uint32_t iReg = 0;
    while (iReg < min(cc.paramByRegisterCount, numTotalRegs))
    {
        auto     typeParam   = typeFunc->registerIdxToType(iReg);
        uint32_t stackOffset = SCBE_X64::getParamStackOffset(cpuFct, iReg);
        if (cc.useRegisterFloat && typeParam->isNativeFloat())
            pp.emit_StoreF64_Indirect(stackOffset, cc.paramByRegisterFloat[iReg], RDI);
        else
            pp.emit_Store64_Indirect(stackOffset, cc.paramByRegisterInteger[iReg], RDI);

        if (iReg < cpuFct->numScratchRegs)
            pp.emit_Load64_Indirect(stackOffset, (CPURegister) (cc.firstScratchRegister + iReg), RDI);

        iReg++;
    }

    // Save pointer to return value if this is a return by copy
    if (CallConv::returnByAddress(typeFunc) && iReg < cc.paramByRegisterCount)
    {
        uint32_t stackOffset = SCBE_X64::getParamStackOffset(cpuFct, iReg);
        pp.emit_Store64_Indirect(stackOffset, cc.paramByRegisterInteger[iReg], RDI);
        iReg++;
    }

    // Save C variadics
    if (typeFunc->isCVariadic())
    {
        while (iReg < cc.paramByRegisterCount)
        {
            uint32_t stackOffset = cpuFct->offsetCallerStackParams + REG_OFFSET(iReg);
            pp.emit_Store64_Indirect(stackOffset, cc.paramByRegisterInteger[iReg], RDI);
            iReg++;
        }
    }

    // Use R11 as base pointer for capture parameters
    // This is used to debug and have access to capture parameters, even if we "lose" rcx
    // which is the register that will have a pointer to the capture buffer (but rcx is volatile)
    if (typeFunc->isClosure() && debug)
        pp.emit_CopyN(R12, RCX, CPUBits::B64);

    auto                                   ip = bc->out;
    VectorNative<uint32_t>                 pushRAParams;
    VectorNative<pair<uint32_t, uint32_t>> pushRVParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->hasAstFlag(AST_NO_BACKEND))
            continue;
        if (ip->flags & BCI_NO_BACKEND)
            continue;

        if (debug)
            SCBEDebug::setLocation(cpuFct, bc, ip, concat.totalCount() - beforeProlog);

        if (ip->flags & BCI_JUMP_DEST)
            pp.getOrCreateLabel(i);

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
            pp.emit_Nop();
            break;

        /////////////////////////////////////

        case ByteCodeOp::MulAddVC64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RCX);
            pp.emit_OpN_Immediate(RCX, ip->c.u8, CPUOp::ADD, CPUBits::B64);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B64);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::Add32byVB32:
            pp.emit_OpN_IndirectDst(REG_OFFSET(ip->a.u32), ip->b.u64, RDI, CPUOp::ADD, CPUBits::B32);
            break;
        case ByteCodeOp::Add64byVB64:
            pp.emit_OpN_IndirectDst(REG_OFFSET(ip->a.u32), ip->b.u64, RDI, CPUOp::ADD, CPUBits::B64);
            break;

        /////////////////////////////////////

        case ByteCodeOp::ClearRA:
            pp.emit_Store64_Immediate(REG_OFFSET(ip->a.u32), 0, RDI);
            break;
        case ByteCodeOp::ClearRAx2:
            pp.emit_Store64_Immediate(REG_OFFSET(ip->a.u32), 0, RDI);
            pp.emit_Store64_Immediate(REG_OFFSET(ip->b.u32), 0, RDI);
            break;
        case ByteCodeOp::ClearRAx3:
            pp.emit_Store64_Immediate(REG_OFFSET(ip->a.u32), 0, RDI);
            pp.emit_Store64_Immediate(REG_OFFSET(ip->b.u32), 0, RDI);
            pp.emit_Store64_Immediate(REG_OFFSET(ip->c.u32), 0, RDI);
            break;
        case ByteCodeOp::ClearRAx4:
            pp.emit_Store64_Immediate(REG_OFFSET(ip->a.u32), 0, RDI);
            pp.emit_Store64_Immediate(REG_OFFSET(ip->b.u32), 0, RDI);
            pp.emit_Store64_Immediate(REG_OFFSET(ip->c.u32), 0, RDI);
            pp.emit_Store64_Immediate(REG_OFFSET(ip->d.u32), 0, RDI);
            break;

        /////////////////////////////////////

        case ByteCodeOp::CopyRBtoRA8:
            pp.emit_LoadU8U64_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CopyRBtoRA16:
            pp.emit_LoadU16U64_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CopyRBtoRA32:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CopyRBtoRA64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CopyRBtoRA64x2:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->d.u32), RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;

        case ByteCodeOp::CopyRBAddrToRA:
            pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::CastBool8:
            pp.emit_Load8_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B8);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastBool16:
            pp.emit_Load16_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B16);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastBool32:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B32);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastBool64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B64);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CastS8S16:
            pp.emit_LoadS8S16_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CastS8S32:
            pp.emit_LoadS8S32_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastS16S32:
            pp.emit_LoadS16S32_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CastS8S64:
            pp.emit_LoadS8S64_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastS16S64:
            pp.emit_LoadS16S64_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastS32S64:
            pp.emit_LoadS32S64_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CastF32S32:
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->b.u32), XMM0);
            pp.emit_OpF32(XMM0, RAX, CPUOp::CVTF2I);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::CastF64S64:
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->b.u32), XMM0);
            pp.emit_OpF64(XMM0, RAX, CPUOp::CVTF2I, CPUBits::B64);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CastS8F32:
            pp.emit_LoadS8S32_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_OpF32(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS16F32:
            pp.emit_LoadS16S32_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_OpF32(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS32F32:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_OpF32(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS64F32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_OpF32(RAX, XMM0, CPUOp::CVTI2F, CPUBits::B64);
            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU8F32:
            pp.emit_LoadU8U32_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_OpF32(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU16F32:
            pp.emit_LoadU16U32_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_OpF32(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU32F32:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_OpF32(RAX, XMM0, CPUOp::CVTI2F, CPUBits::B64);
            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU64F32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_OpF32(RAX, XMM0, CPUOp::CVTI2F, CPUBits::B64);
            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastF64F32:
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->b.u32), XMM0);
            pp.emit_OpF64(RAX, XMM0, CPUOp::CVTF2F);
            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;

        case ByteCodeOp::CastS8F64:
            pp.emit_LoadS8S32_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_OpF64(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS16F64:
            pp.emit_LoadS16S32_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_OpF64(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS32F64:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_OpF64(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastS64F64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_OpF64(RAX, XMM0, CPUOp::CVTI2F, CPUBits::B64);
            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU8F64:
            pp.emit_LoadU8U32_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_OpF64(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU16F64:
            pp.emit_LoadU16U32_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_OpF64(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU32F64:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_OpF64(RAX, XMM0, CPUOp::CVTI2F);
            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::CastU64F64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_CastU64F64(XMM0, RAX);
            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;

        case ByteCodeOp::CastF32F64:
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->b.u32), XMM0);
            pp.emit_OpF32(RAX, XMM0, CPUOp::CVTF2F);
            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
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
            MK_BINOPEQ8_CAB(CPUOp::XOR);
            break;
        case ByteCodeOp::AffectOpXorEqU16:
            MK_BINOPEQ16_CAB(CPUOp::XOR);
            break;
        case ByteCodeOp::AffectOpXorEqU32:
            MK_BINOPEQ32_CAB(CPUOp::XOR);
            break;
        case ByteCodeOp::AffectOpXorEqU64:
            MK_BINOPEQ64_CAB(CPUOp::XOR);
            break;

        /////////////////////////////////////

        case ByteCodeOp::AffectOpOrEqU8:
            MK_BINOPEQ8_CAB(CPUOp::OR);
            break;
        case ByteCodeOp::AffectOpOrEqU16:
            MK_BINOPEQ16_CAB(CPUOp::OR);
            break;
        case ByteCodeOp::AffectOpOrEqU32:
            MK_BINOPEQ32_CAB(CPUOp::OR);
            break;
        case ByteCodeOp::AffectOpOrEqU64:
            MK_BINOPEQ64_CAB(CPUOp::OR);
            break;

        /////////////////////////////////////

        case ByteCodeOp::AffectOpAndEqU8:
            MK_BINOPEQ8_CAB(CPUOp::AND);
            break;
        case ByteCodeOp::AffectOpAndEqU16:
            MK_BINOPEQ16_CAB(CPUOp::AND);
            break;
        case ByteCodeOp::AffectOpAndEqU32:
            MK_BINOPEQ32_CAB(CPUOp::AND);
            break;
        case ByteCodeOp::AffectOpAndEqU64:
            MK_BINOPEQ64_CAB(CPUOp::AND);
            break;

        /////////////////////////////////////

        case ByteCodeOp::AffectOpMulEqS8:
        case ByteCodeOp::AffectOpMulEqS8_Safe:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load8_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B8);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS8));
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS8_SSafe:
            pp.emit_Load8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS8_SSSafe:
            pp.emit_Load8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqS16:
        case ByteCodeOp::AffectOpMulEqS16_Safe:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B16);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS16));
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS16_SSafe:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS16_SSSafe:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqS32:
        case ByteCodeOp::AffectOpMulEqS32_Safe:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B32);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS32));
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS32_SSafe:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS32_SSSafe:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqS64:
        case ByteCodeOp::AffectOpMulEqS64_Safe:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B64);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS64));
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS64_SSafe:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS64_SSSafe:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IMUL, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqU8:
        case ByteCodeOp::AffectOpMulEqU8_Safe:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load8_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B8);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU8));
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU8_SSafe:
            pp.emit_Load8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU8_SSSafe:
            pp.emit_Load8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqU16:
        case ByteCodeOp::AffectOpMulEqU16_Safe:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B16);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU16));
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU16_SSafe:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU16_SSSafe:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqU32:
        case ByteCodeOp::AffectOpMulEqU32_Safe:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B32);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU32));
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU32_SSafe:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU32_SSSafe:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqU64:
        case ByteCodeOp::AffectOpMulEqU64_Safe:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B64);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU64));
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU64_SSafe:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU64_SSSafe:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::MUL, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpMulEqF32:
            MK_IMMB_F32(XMM1);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadF32_Indirect(0, XMM0, RAX);
            pp.emit_OpF32(XMM0, XMM1, CPUOp::FMUL);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMulEqF32_S:
            pp.emit_LoadF32_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            MK_IMMB_F32(XMM1);
            pp.emit_OpF32(XMM0, XMM1, CPUOp::FMUL);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMulEqF32_SS:
            pp.emit_LoadF32_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            pp.emit_LoadF32_Indirect(offsetStack + ip->b.u32, XMM1, RDI);
            pp.emit_OpF32(XMM0, XMM1, CPUOp::FMUL);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;

        case ByteCodeOp::AffectOpMulEqF64:
            MK_IMMB_F64(XMM1);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadF64_Indirect(0, XMM0, RAX);
            pp.emit_OpF64(XMM0, XMM1, CPUOp::FMUL);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMulEqF64_S:
            pp.emit_LoadF64_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            MK_IMMB_F64(XMM1);
            pp.emit_OpF64(XMM0, XMM1, CPUOp::FMUL);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMulEqF64_SS:
            pp.emit_LoadF64_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            pp.emit_LoadF64_Indirect(offsetStack + ip->b.u32, XMM1, RDI);
            pp.emit_OpF64(XMM0, XMM1, CPUOp::FMUL);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::AffectOpDivEqS8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadS8S16_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS8_S:
            pp.emit_LoadS8S16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS8_SS:
            pp.emit_LoadS8S16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqS16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            pp.emit_Cwd();
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS16_S:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cwd();
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS16_SS:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cwd();
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqS32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            pp.emit_Cdq();
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS32_S:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cdq();
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS32_SS:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cdq();
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqS64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_Cqo();
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS64_S:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cqo();
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqS64_SS:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cqo();
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqU8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadU8U32_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU8_S:
            pp.emit_LoadU8U32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU8_SS:
            pp.emit_LoadU8U32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqU16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, CPUBits::B16);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU16_S:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B16);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU16_SS:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B16);
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqU32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, CPUBits::B32);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU32_S:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B32);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU32_SS:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B32);
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqU64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, CPUBits::B64);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU64_S:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B64);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpDivEqU64_SS:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B64);
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpDivEqF32:
            MK_IMMB_F32(XMM1);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadF32_Indirect(0, XMM0, RAX);
            pp.emit_OpF32(XMM0, XMM1, CPUOp::FDIV);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpDivEqF32_S:
            pp.emit_LoadF32_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            MK_IMMB_F32(XMM1);
            pp.emit_OpF32(XMM0, XMM1, CPUOp::FDIV);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpDivEqF32_SS:
            pp.emit_LoadF32_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            pp.emit_LoadF32_Indirect(offsetStack + ip->b.u32, XMM1, RDI);
            pp.emit_OpF32(XMM0, XMM1, CPUOp::FDIV);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF32_Indirect(0, XMM0, RAX);
            break;

        case ByteCodeOp::AffectOpDivEqF64:
            MK_IMMB_F64(XMM1);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadF64_Indirect(0, XMM0, RAX);
            pp.emit_OpF64(XMM0, XMM1, CPUOp::FDIV);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpDivEqF64_S:
            pp.emit_LoadF64_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            MK_IMMB_F64(XMM1);
            pp.emit_OpF64(XMM0, XMM1, CPUOp::FDIV);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpDivEqF64_SS:
            pp.emit_LoadF64_Indirect(offsetStack + ip->a.u32, XMM0, RDI);
            pp.emit_LoadF64_Indirect(offsetStack + ip->b.u32, XMM1, RDI);
            pp.emit_OpF64(XMM0, XMM1, CPUOp::FDIV);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_StoreF64_Indirect(0, XMM0, RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::AffectOpModuloEqS8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadS8S16_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
            pp.emit_CopyDownUp(RAX, CPUBits::B8);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS8_S:
            pp.emit_LoadS8S16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
            pp.emit_CopyDownUp(RAX, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS8_SS:
            pp.emit_LoadS8S16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B8);
            pp.emit_CopyDownUp(RAX, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqS16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            pp.emit_Cwd();
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS16_S:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cwd();
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS16_SS:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cwd();
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqS32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            pp.emit_Cdq();
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS32_S:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cdq();
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS32_SS:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cdq();
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqS64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_Cqo();
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS64_S:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cqo();
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqS64_SS:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Cqo();
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::IDIV, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqU8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadS8S16_Indirect(0, RAX, RAX);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
            pp.emit_CopyDownUp(RAX, CPUBits::B8);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU8_S:
            pp.emit_LoadU8U32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            MK_IMMB_8(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
            pp.emit_CopyDownUp(RAX, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU8_SS:
            pp.emit_LoadU8U32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B8);
            pp.emit_CopyDownUp(RAX, CPUBits::B8);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store8_Indirect(0, RAX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqU16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load16_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, CPUBits::B16);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU16_S:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B16);
            MK_IMMB_16(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU16_SS:
            pp.emit_Load16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B16);
            pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B16);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store16_Indirect(0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqU32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, CPUBits::B32);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU32_S:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B32);
            MK_IMMB_32(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU32_SS:
            pp.emit_Load32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B32);
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B32);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store32_Indirect(0, RDX, RCX);
            break;

        case ByteCodeOp::AffectOpModuloEqU64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_ClearN(RDX, CPUBits::B64);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU64_S:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B64);
            MK_IMMB_64(RCX);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;
        case ByteCodeOp::AffectOpModuloEqU64_SS:
            pp.emit_Load64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_ClearN(RDX, CPUBits::B64);
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RCX, RDI);
            pp.emit_OpN(RAX, RCX, CPUOp::DIV, CPUBits::B64);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI);
            pp.emit_Store64_Indirect(0, RDX, RCX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::AffectOpPlusEqS8:
        case ByteCodeOp::AffectOpPlusEqS8_Safe:
            MK_BINOPEQ8_CAB(CPUOp::ADD);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS8));
            break;
        case ByteCodeOp::AffectOpPlusEqS8_SSafe:
        case ByteCodeOp::AffectOpPlusEqU8_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_IncN_Indirect(offsetStack + ip->a.u32, RDI, CPUBits::B8);
            else
                MK_BINOPEQ8_SCAB(CPUOp::ADD);
            break;
        case ByteCodeOp::AffectOpPlusEqS8_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU8_SSSafe:
            MK_BINOPEQ8_SSCAB(CPUOp::ADD);
            break;

        case ByteCodeOp::AffectOpPlusEqS16:
        case ByteCodeOp::AffectOpPlusEqS16_Safe:
            MK_BINOPEQ16_CAB(CPUOp::ADD);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS16));
            break;
        case ByteCodeOp::AffectOpPlusEqS16_SSafe:
        case ByteCodeOp::AffectOpPlusEqU16_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_IncN_Indirect(offsetStack + ip->a.u32, RDI, CPUBits::B16);
            else
                MK_BINOPEQ16_SCAB(CPUOp::ADD);
            break;
        case ByteCodeOp::AffectOpPlusEqS16_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU16_SSSafe:
            MK_BINOPEQ16_SSCAB(CPUOp::ADD);
            break;

        case ByteCodeOp::AffectOpPlusEqS32:
        case ByteCodeOp::AffectOpPlusEqS32_Safe:
            MK_BINOPEQ32_CAB(CPUOp::ADD);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::AffectOpPlusEqS32_SSafe:
        case ByteCodeOp::AffectOpPlusEqU32_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_IncN_Indirect(offsetStack + ip->a.u32, RDI, CPUBits::B32);
            else
                MK_BINOPEQ32_SCAB(CPUOp::ADD);
            break;
        case ByteCodeOp::AffectOpPlusEqS32_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU32_SSSafe:
            MK_BINOPEQ32_SSCAB(CPUOp::ADD);
            break;

        case ByteCodeOp::AffectOpPlusEqS64:
        case ByteCodeOp::AffectOpPlusEqS64_Safe:
            MK_BINOPEQ64_CAB(CPUOp::ADD);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS64));
            break;
        case ByteCodeOp::AffectOpPlusEqS64_SSafe:
        case ByteCodeOp::AffectOpPlusEqU64_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_IncN_Indirect(offsetStack + ip->a.u32, RDI, CPUBits::B64);
            else
                MK_BINOPEQ64_SCAB(CPUOp::ADD);
            break;
        case ByteCodeOp::AffectOpPlusEqS64_SSSafe:
        case ByteCodeOp::AffectOpPlusEqU64_SSSafe:
            MK_BINOPEQ64_SSCAB(CPUOp::ADD);
            break;

        case ByteCodeOp::AffectOpPlusEqU8:
        case ByteCodeOp::AffectOpPlusEqU8_Safe:
            MK_BINOPEQ8_CAB(CPUOp::ADD);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU8));
            break;

        case ByteCodeOp::AffectOpPlusEqU16:
        case ByteCodeOp::AffectOpPlusEqU16_Safe:
            MK_BINOPEQ16_CAB(CPUOp::ADD);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU16));
            break;

        case ByteCodeOp::AffectOpPlusEqU32:
        case ByteCodeOp::AffectOpPlusEqU32_Safe:
            MK_BINOPEQ32_CAB(CPUOp::ADD);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU32));
            break;

        case ByteCodeOp::AffectOpPlusEqU64:
        case ByteCodeOp::AffectOpPlusEqU64_Safe:
            MK_BINOPEQ64_CAB(CPUOp::ADD);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU64));
            break;

        case ByteCodeOp::AffectOpPlusEqF32:
            MK_BINOPEQF32_CAB(CPUOp::FADD);
            break;
        case ByteCodeOp::AffectOpPlusEqF32_S:
            MK_BINOPEQF32_SCAB(CPUOp::FADD);
            break;
        case ByteCodeOp::AffectOpPlusEqF32_SS:
            MK_BINOPEQF32_SSCAB(CPUOp::FADD);
            break;

        case ByteCodeOp::AffectOpPlusEqF64:
            MK_BINOPEQF64_CAB(CPUOp::FADD);
            break;
        case ByteCodeOp::AffectOpPlusEqF64_S:
            MK_BINOPEQF64_SCAB(CPUOp::FADD);
            break;
        case ByteCodeOp::AffectOpPlusEqF64_SS:
            MK_BINOPEQF64_SSCAB(CPUOp::FADD);
            break;

        /////////////////////////////////////

        case ByteCodeOp::AffectOpMinusEqS8:
        case ByteCodeOp::AffectOpMinusEqS8_Safe:
            MK_BINOPEQ8_CAB(CPUOp::SUB);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS8));
            break;
        case ByteCodeOp::AffectOpMinusEqS8_SSafe:
        case ByteCodeOp::AffectOpMinusEqU8_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_DecN_Indirect(offsetStack + ip->a.u32, RDI, CPUBits::B8);
            else
                MK_BINOPEQ8_SCAB(CPUOp::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqS8_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU8_SSSafe:
            MK_BINOPEQ8_SSCAB(CPUOp::SUB);
            break;

        case ByteCodeOp::AffectOpMinusEqS16:
        case ByteCodeOp::AffectOpMinusEqS16_Safe:
            MK_BINOPEQ16_CAB(CPUOp::SUB);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16));
            break;
        case ByteCodeOp::AffectOpMinusEqS16_SSafe:
        case ByteCodeOp::AffectOpMinusEqU16_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_DecN_Indirect(offsetStack + ip->a.u32, RDI, CPUBits::B16);
            else
                MK_BINOPEQ16_SCAB(CPUOp::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqS16_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU16_SSSafe:
            MK_BINOPEQ16_SSCAB(CPUOp::SUB);
            break;

        case ByteCodeOp::AffectOpMinusEqS32:
        case ByteCodeOp::AffectOpMinusEqS32_Safe:
            MK_BINOPEQ32_CAB(CPUOp::SUB);
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS32));
            break;
        case ByteCodeOp::AffectOpMinusEqS32_SSafe:
        case ByteCodeOp::AffectOpMinusEqU32_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_DecN_Indirect(offsetStack + ip->a.u32, RDI, CPUBits::B32);
            else
                MK_BINOPEQ32_SCAB(CPUOp::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqS32_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU32_SSSafe:
            MK_BINOPEQ32_SSCAB(CPUOp::SUB);
            break;

        case ByteCodeOp::AffectOpMinusEqS64:
        case ByteCodeOp::AffectOpMinusEqS64_Safe:
            emitOverflowSigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS64));
            MK_BINOPEQ64_CAB(CPUOp::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqS64_SSafe:
        case ByteCodeOp::AffectOpMinusEqU64_SSafe:
            if (ip->flags & BCI_IMM_B && ip->b.u32 == 1)
                pp.emit_DecN_Indirect(offsetStack + ip->a.u32, RDI, CPUBits::B64);
            else
                MK_BINOPEQ64_SCAB(CPUOp::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqS64_SSSafe:
        case ByteCodeOp::AffectOpMinusEqU64_SSSafe:
            MK_BINOPEQ64_SSCAB(CPUOp::SUB);
            break;

        case ByteCodeOp::AffectOpMinusEqU8:
        case ByteCodeOp::AffectOpMinusEqU8_Safe:
            MK_BINOPEQ8_CAB(CPUOp::SUB);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU8));
            break;

        case ByteCodeOp::AffectOpMinusEqU16:
        case ByteCodeOp::AffectOpMinusEqU16_Safe:
            MK_BINOPEQ16_CAB(CPUOp::SUB);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16));
            break;

        case ByteCodeOp::AffectOpMinusEqU32:
        case ByteCodeOp::AffectOpMinusEqU32_Safe:
            MK_BINOPEQ32_CAB(CPUOp::SUB);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU32));
            break;

        case ByteCodeOp::AffectOpMinusEqU64:
        case ByteCodeOp::AffectOpMinusEqU64_Safe:
            MK_BINOPEQ64_CAB(CPUOp::SUB);
            emitOverflowUnsigned(pp, ip, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU64));
            break;

        case ByteCodeOp::AffectOpMinusEqF32:
            MK_BINOPEQF32_CAB(CPUOp::FSUB);
            break;
        case ByteCodeOp::AffectOpMinusEqF32_S:
            MK_BINOPEQF32_SCAB(CPUOp::FSUB);
            break;
        case ByteCodeOp::AffectOpMinusEqF32_SS:
            MK_BINOPEQF32_SSCAB(CPUOp::FSUB);
            break;

        case ByteCodeOp::AffectOpMinusEqF64:
            MK_BINOPEQF64_CAB(CPUOp::FSUB);
            break;
        case ByteCodeOp::AffectOpMinusEqF64_S:
            MK_BINOPEQF64_SCAB(CPUOp::FSUB);
            break;
        case ByteCodeOp::AffectOpMinusEqF64_SS:
            MK_BINOPEQF64_SSCAB(CPUOp::FSUB);
            break;

        /////////////////////////////////////

        case ByteCodeOp::ZeroToTrue:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B32);
            pp.emit_SetE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::LowerZeroToTrue:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_OpN_Immediate(RAX, 31, CPUOp::SHR, CPUBits::B32);
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::LowerEqZeroToTrue:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B32);
            pp.emit_SetLE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GreaterZeroToTrue:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B32);
            pp.emit_SetG();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GreaterEqZeroToTrue:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_NotN(RAX, CPUBits::B32);
            pp.emit_OpN_Immediate(RAX, 31, CPUOp::SHR, CPUBits::B32);
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::CompareOpGreaterS8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterS16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterS32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterU8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterU16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterU32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetA();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;

        case ByteCodeOp::CompareOpGreaterEqS8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetGE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqS16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetGE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqS32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetGE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqS64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetGE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqU8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqU16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqU32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqU64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpGreaterEqF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetAE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::CompareOpLowerS8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetL();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerS16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetL();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerS32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetL();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerS64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetL();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerU8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerU16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerU32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerU64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetB();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;

        case ByteCodeOp::CompareOpLowerEqS8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetLE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqS16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetLE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqS32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetLE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqS64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetLE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqU8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqU16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqU32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqU64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpLowerEqF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetBE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::CompareOp3Way8:
            pp.emit_ClearN(R8, CPUBits::B32);
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG(R8);
            pp.emit_Load32_Immediate(RAX, 0xFFFFFFFF);
            pp.emit_CMovN(RAX, R8, CPUOp::CMOVGE, CPUBits::B32);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOp3Way16:
            pp.emit_ClearN(R8, CPUBits::B32);
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG(R8);
            pp.emit_Load32_Immediate(RAX, 0xFFFFFFFF);
            pp.emit_CMovN(RAX, R8, CPUOp::CMOVGE, CPUBits::B32);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOp3Way32:
            pp.emit_ClearN(R8, CPUBits::B32);
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG(R8);
            pp.emit_Load32_Immediate(RAX, 0xFFFFFFFF);
            pp.emit_CMovN(RAX, R8, CPUOp::CMOVGE, CPUBits::B32);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOp3Way64:
            pp.emit_ClearN(R8, CPUBits::B32);
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetG(R8);
            pp.emit_Load32_Immediate(RAX, 0xFFFFFFFF);
            pp.emit_CMovN(RAX, R8, CPUOp::CMOVGE, CPUBits::B32);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOp3WayF32:
            pp.emit_ClearN(R8, CPUBits::B32);
            MK_IMMA_F32(XMM0);
            MK_IMMB_F32(XMM1);
            pp.emit_OpF32(XMM0, XMM1, CPUOp::UCOMIF);
            pp.emit_SetA(R8);
            pp.emit_OpF32(XMM1, XMM0, CPUOp::UCOMIF);
            pp.emit_Load32_Immediate(RAX, 0xFFFFFFFF);
            pp.emit_CMovN(RAX, R8, CPUOp::CMOVBE, CPUBits::B32);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOp3WayF64:
            pp.emit_ClearN(R8, CPUBits::B32);
            MK_IMMA_F64(XMM0);
            MK_IMMB_F64(XMM1);
            pp.emit_OpF64(XMM0, XMM1, CPUOp::UCOMIF);
            pp.emit_SetA(R8);
            pp.emit_OpF64(XMM1, XMM0, CPUOp::UCOMIF);
            pp.emit_Load32_Immediate(RAX, 0xFFFFFFFF);
            pp.emit_CMovN(RAX, R8, CPUOp::CMOVBE, CPUBits::B32);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::CompareOpEqual8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpEqual16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpEqual32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpEqual64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpEqualF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetEP();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpEqualF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetEP();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::CompareOpNotEqual8:
            MK_BINOP8_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpNotEqual16:
            MK_BINOP16_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpNotEqual32:
            MK_BINOP32_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpNotEqual64:
            MK_BINOP64_CAB(emit_CmpN_IndirectDst, emit_CmpN_Indirect, emit_CmpN);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpNotEqualF32:
            MK_BINOPF32_CAB(emit_CmpF32_Indirect, emit_CmpF32);
            pp.emit_SetNEP();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::CompareOpNotEqualF64:
            MK_BINOPF64_CAB(emit_CmpF64_Indirect, emit_CmpF64);
            pp.emit_SetNEP();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::IntrinsicDbgAlloc:
            emitInternalCall(pp, g_LangSpec->name_atdbgalloc, {}, REG_OFFSET(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicSysAlloc:
            emitInternalCall(pp, g_LangSpec->name_atsysalloc, {}, REG_OFFSET(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicRtFlags:
            emitInternalCall(pp, g_LangSpec->name_atrtflags, {}, REG_OFFSET(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicStringCmp:
            emitInternalCall(pp, g_LangSpec->name_atstrcmp, {ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32}, REG_OFFSET(ip->d.u32));
            break;
        case ByteCodeOp::IntrinsicTypeCmp:
            emitInternalCall(pp, g_LangSpec->name_attypecmp, {ip->a.u32, ip->b.u32, ip->c.u32}, REG_OFFSET(ip->d.u32));
            break;

        /////////////////////////////////////

        case ByteCodeOp::TestNotZero8:
            MK_IMMB_8(RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B8);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::TestNotZero16:
            MK_IMMB_16(RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B16);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::TestNotZero32:
            MK_IMMB_32(RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B32);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::TestNotZero64:
            MK_IMMB_64(RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B64);
            pp.emit_SetNE();
            pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::NegBool:
            pp.emit_Load8_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_OpN_Immediate(RAX, 1, CPUOp::XOR, CPUBits::B8);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::NegS32:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NegN_Indirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B32);
            else
            {
                pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_NegN(RAX, CPUBits::B64);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            }
            break;
        case ByteCodeOp::NegS64:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NegN_Indirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B64);
            else
            {
                pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_NegN(RAX, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            }
            break;
        case ByteCodeOp::NegF32:
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->b.u32), XMM0);
            pp.emit_Load64_Immediate(RAX, 0x80000000);
            pp.emit_Store64_Indirect(offsetFLT, RAX, RDI);
            pp.emit_LoadF32_Indirect(offsetFLT, XMM1, RDI);
            pp.emit_OpF32(XMM0, XMM1, CPUOp::FXOR);
            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        case ByteCodeOp::NegF64:
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->b.u32), XMM0);
            pp.emit_Load64_Immediate(RAX, 0x80000000'00000000);
            pp.emit_Store64_Indirect(offsetFLT, RAX, RDI);
            pp.emit_LoadF64_Indirect(offsetFLT, XMM1, RDI);
            pp.emit_OpF64(XMM0, XMM1, CPUOp::FXOR);
            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;

        /////////////////////////////////////

        case ByteCodeOp::InvertU8:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NotN_Indirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B8);
            else
            {
                pp.emit_Load8_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_NotN(RAX, CPUBits::B8);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            }
            break;
        case ByteCodeOp::InvertU16:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NotN_Indirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B16);
            else
            {
                pp.emit_Load16_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_NotN(RAX, CPUBits::B16);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
            }
            break;
        case ByteCodeOp::InvertU32:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NotN_Indirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B32);
            else
            {
                pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_NotN(RAX, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            }
            break;
        case ByteCodeOp::InvertU64:
            if (ip->a.u32 == ip->b.u32)
                pp.emit_NotN_Indirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B64);
            else
            {
                pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_NotN(RAX, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            }
            break;

        /////////////////////////////////////

        case ByteCodeOp::JumpDyn8:
        case ByteCodeOp::JumpDyn16:
        case ByteCodeOp::JumpDyn32:
        case ByteCodeOp::JumpDyn64:
        {
            auto tableCompiler = (int32_t*) buildParameters.module->compilerSegment.address(ip->d.u32);

            if (ip->op == ByteCodeOp::JumpDyn8)
                pp.emit_LoadS8S64_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            else if (ip->op == ByteCodeOp::JumpDyn16)
                pp.emit_LoadS16S64_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            else if (ip->op == ByteCodeOp::JumpDyn32)
                pp.emit_LoadS32S64_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            else
                pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);

            // Note:
            //
            // This is not optimal yet.
            // The sub could be removed by baking it in the 'cmp', and by changing the jump table address by substracting the min value
            // Also, if the jumptable was encoded in the text segment, then there will be no need to have two relocations
            //
            // We could in the end remove two instructions and be as the llvm generation

            pp.emit_OpN_Immediate(RAX, ip->b.u64 - 1, CPUOp::SUB, CPUBits::B64);
            pp.emit_CmpN_Immediate(RAX, ip->c.u64, CPUBits::B64);
            pp.emit_Jump(JAE, i, tableCompiler[0]);

            uint8_t* addrConstant        = nullptr;
            auto     offsetTableConstant = buildParameters.module->constantSegment.reserve(((uint32_t) ip->c.u64) * sizeof(uint32_t), &addrConstant);

            pp.emit_Symbol_RelocationAddr(RCX, pp.symCSIndex, offsetTableConstant); // rcx = jump table
            pp.emit_JumpTable(RCX, RAX);

            // + 5 for the two following instructions
            // + 7 for this instruction
            pp.emit_Symbol_RelocationAddr(RAX, symbolFuncIndex, concat.totalCount() - startAddress + 5 + 7);
            pp.emit_OpN(RCX, RAX, CPUOp::ADD, CPUBits::B64);
            pp.emit_Jump(RAX);

            auto currentOffset = (int32_t) pp.concat.totalCount();

            auto            tableConstant = (int32_t*) addrConstant;
            CPULabelToSolve label;
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
            pp.emit_Load8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B8);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfFalse:
            pp.emit_Load8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B8);
            pp.emit_Jump(JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero8:
            pp.emit_Load8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B8);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero16:
            pp.emit_Load16_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B16);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero32:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B32);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B64);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero8:
            pp.emit_Load8_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B8);
            pp.emit_Jump(JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero16:
            pp.emit_Load16_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B16);
            pp.emit_Jump(JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero32:
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B32);
            pp.emit_Jump(JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_TestN(RAX, RAX, CPUBits::B64);
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
            pp.emit_IncN_Indirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B64);
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
            pp.emit_DecN_Indirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B32);
            break;
        case ByteCodeOp::IncrementRA64:
            pp.emit_IncN_Indirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B64);
            break;
        case ByteCodeOp::DecrementRA64:
            pp.emit_DecN_Indirect(REG_OFFSET(ip->a.u32), RDI, CPUBits::B64);
            break;

        /////////////////////////////////////

        case ByteCodeOp::DeRef8:
            SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_LoadU8U64_Indirect(ip->c.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::DeRef16:
            SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_LoadU16U64_Indirect(ip->c.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::DeRef32:
            SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_Load32_Indirect(ip->c.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::DeRef64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            if (ip->c.u64 <= 0x7FFFFFFF)
                pp.emit_Load64_Indirect(ip->c.u32, RAX, RAX);
            else
            {
                pp.emit_Load64_Immediate(RCX, ip->c.u64);
                pp.emit_OpN(RCX, RAX, CPUOp::ADD, CPUBits::B64);
                pp.emit_Load64_Indirect(0, RAX, RAX);
            }
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::DeRefStringSlice:
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFFF);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(ip->c.u32 + 8, RCX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->b.u32), RCX);
            pp.emit_Load64_Indirect(ip->c.u32 + 0, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::GetFromBssSeg8:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symBSIndex, 0);
            pp.emit_LoadU8U64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromBssSeg16:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symBSIndex, 0);
            pp.emit_LoadU16U64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromBssSeg32:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symBSIndex, 0);
            pp.emit_Load32_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromBssSeg64:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symBSIndex, 0);
            pp.emit_Load64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::GetFromMutableSeg8:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, 0);
            pp.emit_LoadU8U64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromMutableSeg16:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, 0);
            pp.emit_LoadU16U64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromMutableSeg32:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, 0);
            pp.emit_Load32_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromMutableSeg64:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, 0);
            pp.emit_Load64_Indirect(ip->b.u32, RAX, RAX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::GetFromStack8:
            pp.emit_LoadU8U64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack16:
            pp.emit_LoadU16U64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack32:
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack64:
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::GetIncFromStack64:
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_OpN_Immediate(RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::GetFromStack8x2:
            pp.emit_LoadU8U64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadU8U64_Indirect(offsetStack + ip->d.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack16x2:
            pp.emit_LoadU16U64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadU16U64_Indirect(offsetStack + ip->d.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack32x2:
            pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(offsetStack + ip->d.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::GetFromStack64x2:
            pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(offsetStack + ip->d.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
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
                pp.emit_OpN_Immediate(RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                pp.emit_LoadU8U64_Indirect(0, RAX, RAX);
            }
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
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
                pp.emit_OpN_Immediate(RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                pp.emit_LoadU16U64_Indirect(0, RAX, RAX);
            }
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
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
                pp.emit_OpN_Immediate(RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                pp.emit_Load32_Indirect(0, RAX, RAX);
            }
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
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
                pp.emit_OpN_Immediate(RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                pp.emit_Load64_Indirect(0, RAX, RAX);
            }
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
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
                pp.emit_LoadU8U32_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            else if (ip->b.u32 == 0xFFFF)
                pp.emit_LoadU16U32_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            else
                SWAG_ASSERT(false);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::ClearMaskU64:
            if (ip->b.u32 == 0xFF)
                pp.emit_LoadU8U64_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            else if (ip->b.u32 == 0xFFFF)
                pp.emit_LoadU16U64_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            else if (ip->b.u32 == 0xFFFFFFFF)
                pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);
            else
                SWAG_ASSERT(false);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::SetZeroAtPointer8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Store8_Immediate(ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Store16_Immediate(ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Store32_Immediate(ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Store64_Immediate(ip->b.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointerX:
            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
            if (ip->b.u32 <= 128 && !buildParameters.isDebug())
            {
                pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                pp.emit_ClearX(ip->b.u32, ip->c.u32, RAX);
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
            int stackOffset = SCBE_X64::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            pp.emit_Store8_Immediate(ip->c.u32, 0, RAX);
            break;
        }
        case ByteCodeOp::ClearRR16:
        {
            int stackOffset = SCBE_X64::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            pp.emit_Store16_Immediate(ip->c.u32, 0, RAX);
            break;
        }
        case ByteCodeOp::ClearRR32:
        {
            int stackOffset = SCBE_X64::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            pp.emit_Store32_Immediate(ip->c.u32, 0, RAX);
            break;
        }
        case ByteCodeOp::ClearRR64:
        {
            int stackOffset = SCBE_X64::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            pp.emit_Store64_Immediate(ip->c.u32, 0, RAX);
            break;
        }
        case ByteCodeOp::ClearRRX:
        {
            int stackOffset = SCBE_X64::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);

            SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
            if (ip->b.u32 <= 128 && !buildParameters.isDebug())
            {
                pp.emit_ClearX(ip->b.u32, ip->c.u32, RAX);
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
                pp.pushParams.push_back({CPUPushParamType::Addr, offsetStack + ip->a.u32});
                pp.pushParams.push_back({CPUPushParamType::Imm, 0});
                pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u32});
                emitInternalCallExt(pp, g_LangSpec->name_memset, pp.pushParams);
            }
            break;

        /////////////////////////////////////

        case ByteCodeOp::SetAtPointer8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store8_Immediate(ip->c.u32, ip->b.u8, RAX);
            else
            {
                pp.emit_Load8_Indirect(REG_OFFSET(ip->b.u32), RCX);
                pp.emit_Store8_Indirect(ip->c.u32, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store16_Immediate(ip->c.u32, ip->b.u16, RAX);
            else
            {
                pp.emit_Load16_Indirect(REG_OFFSET(ip->b.u32), RCX);
                pp.emit_Store16_Indirect(ip->c.u32, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store32_Immediate(ip->c.u32, ip->b.u32, RAX);
            else
            {
                pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RCX);
                pp.emit_Store32_Indirect(ip->c.u32, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer64:
            if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)
            {
                pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                pp.emit_Store64_Immediate(ip->c.u32, ip->b.u64, RAX);
            }
            else if (ip->flags & BCI_IMM_B)
            {
                pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                pp.emit_Load64_Immediate(RCX, ip->b.u64);
                pp.emit_Store64_Indirect(ip->c.u32, RCX, RAX);
            }
            else
            {
                pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
                pp.emit_Store64_Indirect(ip->c.u32, RAX, RCX);
            }
            break;

        /////////////////////////////////////

        case ByteCodeOp::SetAtStackPointer8:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store8_Immediate(offsetStack + ip->a.u32, ip->b.u8, RDI);
            else
            {
                pp.emit_Load8_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_Store8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer16:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store16_Immediate(offsetStack + ip->a.u32, ip->b.u16, RDI);
            else
            {
                pp.emit_Load16_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_Store16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer32:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store32_Immediate(offsetStack + ip->a.u32, ip->b.u32, RDI);
            else
            {
                pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);
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
                pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_Store64_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }
            break;

        /////////////////////////////////////

        case ByteCodeOp::SetAtStackPointer8x2:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store8_Immediate(offsetStack + ip->a.u32, ip->b.u8, RDI);
            else
            {
                pp.emit_Load8_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_Store8_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D)
                pp.emit_Store8_Immediate(offsetStack + ip->c.u32, ip->d.u8, RDI);
            else
            {
                pp.emit_Load8_Indirect(REG_OFFSET(ip->d.u32), RAX);
                pp.emit_Store8_Indirect(offsetStack + ip->c.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer16x2:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store16_Immediate(offsetStack + ip->a.u32, ip->b.u16, RDI);
            else
            {
                pp.emit_Load16_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_Store16_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D)
                pp.emit_Store16_Immediate(offsetStack + ip->c.u32, ip->d.u16, RDI);
            else
            {
                pp.emit_Load16_Indirect(REG_OFFSET(ip->d.u32), RAX);
                pp.emit_Store16_Indirect(offsetStack + ip->c.u32, RAX, RDI);
            }
            break;
        case ByteCodeOp::SetAtStackPointer32x2:
            if (ip->flags & BCI_IMM_B)
                pp.emit_Store32_Immediate(offsetStack + ip->a.u32, ip->b.u32, RDI);
            else
            {
                pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);
                pp.emit_Store32_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            }

            if (ip->flags & BCI_IMM_D)
                pp.emit_Store32_Immediate(offsetStack + ip->c.u32, ip->d.u32, RDI);
            else
            {
                pp.emit_Load32_Indirect(REG_OFFSET(ip->d.u32), RAX);
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
                pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
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
                pp.emit_Load64_Indirect(REG_OFFSET(ip->d.u32), RAX);
                pp.emit_Store64_Indirect(offsetStack + ip->c.u32, RAX, RDI);
            }
            break;

        /////////////////////////////////////

        case ByteCodeOp::MakeStackPointer:
            pp.emit_LoadAddress_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::MakeStackPointerx2:
            pp.emit_LoadAddress_Indirect(offsetStack + ip->b.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_LoadAddress_Indirect(offsetStack + ip->d.u32, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
            break;
        case ByteCodeOp::MakeStackPointerRT:
            pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RAX, RDI);
            pp.emit_Store64_Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
            break;

        /////////////////////////////////////

        case ByteCodeOp::MakeMutableSegPointer:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, ip->b.u32);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::MakeBssSegPointer:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symBSIndex, ip->b.u32);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::MakeConstantSegPointer:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symCSIndex, ip->b.u32);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        case ByteCodeOp::MakeCompilerSegPointer:
            return Report::report({ip->node, ip->node->token, Err(Err0177)});

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
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_OpN_Immediate(RAX, ip->b.u64, CPUOp::IMUL, CPUBits::B64);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::Div64byVB64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Immediate(RCX, ip->b.u64);
            pp.emit_ClearN(RDX, CPUBits::B64);
            pp.emit_OpN(RCX, RAX, CPUOp::IDIV, CPUBits::B64);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        /////////////////////////////////////

        case ByteCodeOp::SetImmediate32:
            pp.emit_Store32_Immediate(REG_OFFSET(ip->a.u32), ip->b.u32, RDI);
            break;
        case ByteCodeOp::SetImmediate64:
            if (ip->b.u64 <= 0x7FFFFFFF || ip->b.u64 >> 32 == 0xFFFFFFFF)
                pp.emit_Store64_Immediate(REG_OFFSET(ip->a.u32), ip->b.u32, RDI);
            else
            {
                pp.emit_Load64_Immediate(RAX, ip->b.u64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            }
            break;

        /////////////////////////////////////

        case ByteCodeOp::MemCpy8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RDX);
            pp.emit_CopyX(RCX, RDX, 1, 0);
            break;
        case ByteCodeOp::MemCpy16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RDX);
            pp.emit_CopyX(RCX, RDX, 2, 0);
            break;
        case ByteCodeOp::MemCpy32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RDX);
            pp.emit_CopyX(RCX, RDX, 4, 0);
            break;
        case ByteCodeOp::MemCpy64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RDX);
            pp.emit_CopyX(RCX, RDX, 8, 0);
            break;

        /////////////////////////////////////

        case ByteCodeOp::IntrinsicMemCpy:
            if ((ip->flags & BCI_IMM_C) && ip->c.u64 <= 128 && !buildParameters.isDebug())
            {
                pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
                pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RDX);
                pp.emit_CopyX(RCX, RDX, ip->c.u32, 0);
            }
            else
            {
                pp.pushParams.clear();
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->a.u32});
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
                if (ip->flags & BCI_IMM_C)
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->c.u64});
                else
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});
                emitInternalCallExt(pp, g_LangSpec->name_memcpy, pp.pushParams);
            }
            break;
        case ByteCodeOp::IntrinsicMemSet:
            if ((ip->flags & BCI_IMM_B) && (ip->flags & BCI_IMM_C) && (ip->b.u8 == 0) && (ip->c.u64 <= 128) && !buildParameters.isDebug())
            {
                pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
                pp.emit_ClearX(ip->c.u32, 0, RCX);
            }
            else
            {
                pp.pushParams.clear();
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->a.u32});
                if (ip->flags & BCI_IMM_B)
                    pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u8});
                else
                    pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
                if (ip->flags & BCI_IMM_C)
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
            if (ip->flags & BCI_IMM_C)
                pp.pushParams.push_back({CPUPushParamType::Imm, ip->c.u64});
            else
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});
            emitInternalCallExt(pp, g_LangSpec->name_memmove, pp.pushParams);
            break;
        case ByteCodeOp::IntrinsicMemCmp:
            pp.pushParams.clear();
            pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
            pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});
            if (ip->flags & BCI_IMM_D)
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
            emitInternalPanic(pp, ip->node, (const char*) ip->d.pointer);
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
            emitInternalCallExt(pp, g_LangSpec->name__tlsGetPtr, pp.pushParams, REG_OFFSET(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicGetContext:
            pp.pushParams.clear();
            pp.pushParams.push_back({CPUPushParamType::RelocV, pp.symPI_contextTlsId});
            emitInternalCallExt(pp, g_LangSpec->name__tlsGetValue, pp.pushParams, REG_OFFSET(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicSetContext:
            pp.pushParams.clear();
            pp.pushParams.push_back({CPUPushParamType::RelocV, pp.symPI_contextTlsId});
            pp.pushParams.push_back({CPUPushParamType::Reg, ip->a.u32});
            emitInternalCallExt(pp, g_LangSpec->name__tlsSetValue, pp.pushParams);
            break;

        case ByteCodeOp::IntrinsicGetProcessInfos:
            pp.emit_Symbol_RelocationAddr(RCX, pp.symPI_processInfos, 0);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            break;

        case ByteCodeOp::IntrinsicCVaStart:
        {
            int stackOffset = 0;
            int paramIdx    = typeFunc->numParamsRegisters();
            if (CallConv::returnByAddress(typeFunc))
                paramIdx += 1;
            stackOffset = cpuFct->offsetCallerStackParams + REG_OFFSET(paramIdx);
            pp.emit_LoadAddress_Indirect(stackOffset, RAX, RDI);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RAX, RCX);
            break;
        }
        case ByteCodeOp::IntrinsicCVaEnd:
            break;
        case ByteCodeOp::IntrinsicCVaArg:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(0, RCX, RAX);
            switch (ip->c.u32)
            {
            case 1:
                pp.emit_ClearN(RDX, CPUBits::B64);
                pp.emit_Load8_Indirect(0, RDX, RCX);
                break;
            case 2:
                pp.emit_ClearN(RDX, CPUBits::B64);
                pp.emit_Load16_Indirect(0, RDX, RCX);
                break;
            case 4:
                pp.emit_Load32_Indirect(0, RDX, RCX);
                break;
            case 8:
                pp.emit_Load64_Indirect(0, RDX, RCX);
                break;
            default:
                break;
            }

            pp.emit_Store64_Indirect(REG_OFFSET(ip->b.u32), RDX);
            pp.emit_Load64_Immediate(RCX, 8);
            pp.emit_OpN_Indirect(0, RCX, RAX, CPUOp::ADD, CPUBits::B64);
            break;

        case ByteCodeOp::IntrinsicArguments:
            SWAG_ASSERT(ip->b.u32 == ip->a.u32 + 1);
            emitInternalCall(pp, g_LangSpec->name_atargs, {}, REG_OFFSET(ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicModules:
            if (buildParameters.module->modulesSliceOffset == UINT32_MAX)
            {
                pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, 0, RAX);
                pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, 0, RAX);
            }
            else
            {
                pp.emit_Symbol_RelocationAddr(RAX, pp.symCSIndex, buildParameters.module->modulesSliceOffset);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, buildParameters.module->moduleDependencies.count + 1, RAX);
            }
            break;
        case ByteCodeOp::IntrinsicGvtd:
            if (buildParameters.module->globalVarsToDropSliceOffset == UINT32_MAX)
            {
                pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, 0, RAX);
                pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, 0, RAX);
            }
            else
            {
                pp.emit_Symbol_RelocationAddr(RAX, pp.symMSIndex, buildParameters.module->globalVarsToDropSliceOffset);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
                pp.emit_Store64_Immediate(0, buildParameters.module->globalVarsToDrop.count, RAX);
            }
            break;

        case ByteCodeOp::IntrinsicCompiler:
            pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            pp.emit_Store64_Immediate(0, 0, RAX);
            pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->b.u32), RAX, RDI);
            pp.emit_Store64_Immediate(0, 0, RAX);
            break;
        case ByteCodeOp::IntrinsicIsByteCode:
            pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            pp.emit_Store32_Immediate(0, 0, RAX);
            break;
        case ByteCodeOp::IntrinsicCompilerError:
            emitInternalCall(pp, g_LangSpec->name_atcompilererror, {ip->a.u32, ip->b.u32, ip->c.u32});
            break;
        case ByteCodeOp::IntrinsicCompilerWarning:
            emitInternalCall(pp, g_LangSpec->name_atcompilerwarning, {ip->a.u32, ip->b.u32, ip->c.u32});
            break;
        case ByteCodeOp::IntrinsicPanic:
            emitInternalCall(pp, g_LangSpec->name_atpanic, {ip->a.u32, ip->b.u32, ip->c.u32});
            break;
        case ByteCodeOp::IntrinsicItfTableOf:
            emitInternalCall(pp, g_LangSpec->name_atitftableof, {ip->a.u32, ip->b.u32}, REG_OFFSET(ip->c.u32));
            break;

        /////////////////////////////////////

        case ByteCodeOp::CopyRAtoRR:
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
                pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
                pp.emit_Store64_Indirect(0, RCX, RAX);
            }

            break;

        case ByteCodeOp::CopyRARBtoRR2:
        {
            int stackOffset = SCBE_X64::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Store64_Indirect(0, RCX, RAX);
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RCX);
            pp.emit_Store64_Indirect(8, RCX, RAX);
            break;
        }

        case ByteCodeOp::CopyRAtoRT:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Store64_Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
            break;

        case ByteCodeOp::SaveRRtoRA:
        {
            int stackOffset = SCBE_X64::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        }
        case ByteCodeOp::CopyRRtoRA:
        {
            int stackOffset = SCBE_X64::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
            pp.emit_Load64_Indirect(stackOffset, RAX, RDI);
            if (ip->b.u64)
            {
                pp.emit_Load64_Immediate(RCX, ip->b.u64);
                pp.emit_OpN(RCX, RAX, CPUOp::ADD, CPUBits::B64);
            }

            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        }

        case ByteCodeOp::CopyRTtoRA:
            pp.emit_Load64_Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CopyRT2toRARB:
            pp.emit_Load64_Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Indirect(offsetRT + REG_OFFSET(1), RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            break;

        case ByteCodeOp::CopySP:
            pp.emit_LoadAddress_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;

        case ByteCodeOp::CopySPVaargs:
        {
            auto typeFuncCall = castTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->d.pointer, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
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
                        pp.emit_Load8_Indirect(REG_OFFSET(reg), RAX);
                        pp.emit_Store8_Indirect(offset, RAX, RSP);
                        break;
                    case 2:
                        pp.emit_Load16_Indirect(REG_OFFSET(reg), RAX);
                        pp.emit_Store16_Indirect(offset, RAX, RSP);
                        break;
                    case 4:
                        pp.emit_Load32_Indirect(REG_OFFSET(reg), RAX);
                        pp.emit_Store32_Indirect(offset, RAX, RSP);
                        break;
                    case 8:
                        pp.emit_Load64_Indirect(REG_OFFSET(reg), RAX);
                        pp.emit_Store64_Indirect(offset, RAX, RSP);
                        break;
                    default:
                        break;
                    }

                    idxParam--;
                    offset += sizeOf;
                }

                pp.emit_LoadAddress_Indirect((sizeParamsStack - variadicStackSize), RAX, RSP);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
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
                    pp.emit_Load64_Indirect(REG_OFFSET(pushRAParams[idxParam]), RAX);
                    pp.emit_Store64_Indirect(offset, RAX, RSP);
                    idxParam--;
                    offset += 8;
                }

                pp.emit_LoadAddress_Indirect((sizeParamsStack - variadicStackSize), RAX, RSP);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
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
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 1);
            break;
        case ByteCodeOp::GetParam16:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 2);
            break;
        case ByteCodeOp::GetParam32:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 4);
            break;
        case ByteCodeOp::GetParam64:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8);
            break;
        case ByteCodeOp::GetParam64x2:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8);
            emitGetParam(pp, cpuFct, ip->c.u32, ip->d.u64u32.high, 8);
            break;
        case ByteCodeOp::GetIncParam64:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8, ip->d.u64);
            break;

        /////////////////////////////////////

        case ByteCodeOp::GetParam64DeRef8:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8, 0, 1);
            break;
        case ByteCodeOp::GetParam64DeRef16:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8, 0, 2);
            break;
        case ByteCodeOp::GetParam64DeRef32:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8, 0, 4);
            break;
        case ByteCodeOp::GetParam64DeRef64:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8, 0, 8);
            break;

        /////////////////////////////////////

        case ByteCodeOp::GetIncParam64DeRef8:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8, ip->d.u64, 1);
            break;
        case ByteCodeOp::GetIncParam64DeRef16:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8, ip->d.u64, 2);
            break;
        case ByteCodeOp::GetIncParam64DeRef32:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8, ip->d.u64, 4);
            break;
        case ByteCodeOp::GetIncParam64DeRef64:
            emitGetParam(pp, cpuFct, ip->a.u32, ip->b.u64u32.high, 8, ip->d.u64, 8);
            break;

        /////////////////////////////////////

        case ByteCodeOp::MakeLambda:
        {
            auto funcNode = castAst<AstFuncDecl>((AstNode*) ip->b.pointer, AstNodeKind::FuncDecl);
            SWAG_ASSERT(!ip->c.pointer || (funcNode && funcNode->hasExtByteCode() && funcNode->extByteCode()->bc == (ByteCode*) ip->c.pointer));
            Utf8 callName = funcNode->getCallName();
            pp.emit_Load64_Immediate(RAX, 0, true);

            CPURelocation reloc;
            reloc.virtualAddress = concat.totalCount() - sizeof(uint64_t) - pp.textSectionOffset;
            auto callSym         = pp.getOrAddSymbol(callName, CPUSymbolKind::Extern);
            reloc.symbolIndex    = callSym->index;
            reloc.type           = IMAGE_REL_AMD64_ADDR64;
            pp.relocTableTextSection.table.push_back(reloc);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            break;
        }

        case ByteCodeOp::IntrinsicMakeCallback:
        {
            // Test if it's a bytecode lambda
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load64_Immediate(RCX, SWAG_LAMBDA_BC_MARKER);
            pp.emit_OpN(RAX, RCX, CPUOp::AND, CPUBits::B64);
            pp.emit_TestN(RCX, RCX, CPUBits::B64);
            auto jumpBCToAfterAddr   = pp.emit_LongJumpOp(JZ);
            auto jumpBCToAfterOffset = concat.totalCount();

            // ByteCode lambda
            //////////////////

            pp.emit_CopyN(RCX, RAX, CPUBits::B64);
            pp.emit_Symbol_RelocationAddr(RAX, pp.symPI_makeCallback, 0);
            pp.emit_Load64_Indirect(0, RAX, RAX);
            pp.emit_Call_Indirect(RAX);

            // End
            //////////////////
            *jumpBCToAfterAddr = concat.totalCount() - jumpBCToAfterOffset;
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);

            break;
        }

        /////////////////////////////////////

        case ByteCodeOp::LocalCallPopParam:
            pushRAParams.push_back(ip->d.u32);
            [[fallthrough]];

        case ByteCodeOp::LocalCall:
        case ByteCodeOp::LocalCallPop:
        case ByteCodeOp::LocalCallPopRC:
        {
            auto callBc  = (ByteCode*) ip->a.pointer;
            auto typeFct = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);
            emitCall(pp, typeFct, callBc->getCallNameFromDecl(), pushRAParams, offsetRT, true);
            pushRAParams.clear();
            pushRVParams.clear();

            if (ip->op == ByteCodeOp::LocalCallPopRC)
            {
                pp.emit_Load64_Indirect(offsetRT + REG_OFFSET(0), RAX, RDI);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->d.u32), RAX);
            }

            break;
        }

        case ByteCodeOp::ForeignCall:
        case ByteCodeOp::ForeignCallPop:
        {
            auto funcNode = reinterpret_cast<AstFuncDecl*>(ip->a.pointer);
            auto typeFct  = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

            auto moduleName = ModuleManager::getForeignModuleName(funcNode);

            // Dll imported function name will have "__imp_" before (imported mangled name)
            auto callFuncName = funcNode->getFullNameForeignImport();
            callFuncName      = "__imp_" + callFuncName;

            emitCall(pp, typeFct, callFuncName, pushRAParams, offsetRT, false);
            pushRAParams.clear();
            pushRVParams.clear();
            break;
        }

        case ByteCodeOp::LambdaCall:
        case ByteCodeOp::LambdaCallPop:
        {
            auto typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

            // Test if it's a bytecode lambda
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), R10);
            pp.emit_OpN_Immediate(R10, SWAG_LAMBDA_BC_MARKER_BIT, CPUOp::BT, CPUBits::B64);
            auto jumpToBCAddr   = pp.emit_LongJumpOp(JB);
            auto jumpToBCOffset = concat.totalCount();

            // Native lambda
            //////////////////
            pp.emit_Call_Parameters(typeFuncBC, pushRAParams, offsetRT);
            pp.emit_Call_Indirect(R10);
            pp.emit_Call_Result(typeFuncBC, offsetRT);

            auto jumpBCToAfterAddr   = pp.emit_LongJumpOp(JUMP);
            auto jumpBCToAfterOffset = concat.totalCount();

            // ByteCode lambda
            //////////////////
            *jumpToBCAddr = concat.totalCount() - jumpToBCOffset;

            pp.emit_CopyN(RCX, R10, CPUBits::B64);
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

        case ByteCodeOp::CopyRBtoRRRet:
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
                pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RCX);
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

            pp.emit_OpN_Immediate(RSP, sizeStack + sizeParamsStack, CPUOp::ADD, CPUBits::B64);
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
            pp.concat.addU8((uint8_t) CPUOp::FMUL);
            pp.concat.addU8(0xC1);

            pp.concat.addU8(0xF3);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) CPUOp::FADD);
            pp.concat.addU8(0xC2);

            pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        }
        case ByteCodeOp::IntrinsicMulAddF64:
        {
            MK_IMMB_F64(XMM0);
            MK_IMMC_F64(XMM1);
            MK_IMMD_F64(XMM2);

            pp.concat.addU8(0xF2);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) CPUOp::FMUL);
            pp.concat.addU8(0xC1);

            pp.concat.addU8(0xF2);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) CPUOp::FADD);
            pp.concat.addU8(0xC2);

            pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicAddS8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load8_Indirect(0, RAX, RCX);
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ8_LOCK_CAB(CPUOp::ADD);
            break;
        case ByteCodeOp::IntrinsicAtomicAddS16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load16_Indirect(0, RAX, RCX);
            pp.emit_Store16_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ16_LOCK_CAB(CPUOp::ADD);
            break;
        case ByteCodeOp::IntrinsicAtomicAddS32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load32_Indirect(0, RAX, RCX);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ32_LOCK_CAB(CPUOp::ADD);
            break;
        case ByteCodeOp::IntrinsicAtomicAddS64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(0, RAX, RCX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ64_LOCK_CAB(CPUOp::ADD);
            break;

        case ByteCodeOp::IntrinsicAtomicAndS8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load8_Indirect(0, RAX, RCX);
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ8_LOCK_CAB(CPUOp::AND);
            break;
        case ByteCodeOp::IntrinsicAtomicAndS16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load16_Indirect(0, RAX, RCX);
            pp.emit_Store16_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ16_LOCK_CAB(CPUOp::AND);
            break;
        case ByteCodeOp::IntrinsicAtomicAndS32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load32_Indirect(0, RAX, RCX);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ32_LOCK_CAB(CPUOp::AND);
            break;
        case ByteCodeOp::IntrinsicAtomicAndS64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(0, RAX, RCX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ64_LOCK_CAB(CPUOp::AND);
            break;

        case ByteCodeOp::IntrinsicAtomicOrS8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load8_Indirect(0, RAX, RCX);
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ8_LOCK_CAB(CPUOp::OR);
            break;
        case ByteCodeOp::IntrinsicAtomicOrS16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load16_Indirect(0, RAX, RCX);
            pp.emit_Store16_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ16_LOCK_CAB(CPUOp::OR);
            break;
        case ByteCodeOp::IntrinsicAtomicOrS32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load32_Indirect(0, RAX, RCX);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ32_LOCK_CAB(CPUOp::OR);
            break;
        case ByteCodeOp::IntrinsicAtomicOrS64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(0, RAX, RCX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ64_LOCK_CAB(CPUOp::OR);
            break;

        case ByteCodeOp::IntrinsicAtomicXorS8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load8_Indirect(0, RAX, RCX);
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ8_LOCK_CAB(CPUOp::XOR);
            break;
        case ByteCodeOp::IntrinsicAtomicXorS16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load16_Indirect(0, RAX, RCX);
            pp.emit_Store16_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ16_LOCK_CAB(CPUOp::XOR);
            break;
        case ByteCodeOp::IntrinsicAtomicXorS32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load32_Indirect(0, RAX, RCX);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ32_LOCK_CAB(CPUOp::XOR);
            break;
        case ByteCodeOp::IntrinsicAtomicXorS64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(0, RAX, RCX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ64_LOCK_CAB(CPUOp::XOR);
            break;

        case ByteCodeOp::IntrinsicAtomicXchgS8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load8_Indirect(0, RAX, RCX);
            pp.emit_Store8_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ8_LOCK_CAB(CPUOp::XCHG);
            break;
        case ByteCodeOp::IntrinsicAtomicXchgS16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load16_Indirect(0, RAX, RCX);
            pp.emit_Store16_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ16_LOCK_CAB(CPUOp::XCHG);
            break;
        case ByteCodeOp::IntrinsicAtomicXchgS32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load32_Indirect(0, RAX, RCX);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ32_LOCK_CAB(CPUOp::XCHG);
            break;
        case ByteCodeOp::IntrinsicAtomicXchgS64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_Load64_Indirect(0, RAX, RCX);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RAX);
            MK_BINOPEQ64_LOCK_CAB(CPUOp::XCHG);
            break;

        case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            MK_IMMB_8(RAX);
            MK_IMMC_8(RDX);
            pp.emit_CmpXChg(RCX, RDX, CPUBits::B8);
            pp.emit_Store8_Indirect(REG_OFFSET(ip->d.u32), RAX);
            break;
        case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            MK_IMMB_16(RAX);
            MK_IMMC_16(RDX);
            pp.emit_CmpXChg(RCX, RDX, CPUBits::B16);
            pp.emit_Store16_Indirect(REG_OFFSET(ip->d.u32), RAX);
            break;
        case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            MK_IMMB_32(RAX);
            MK_IMMC_32(RDX);
            pp.emit_CmpXChg(RCX, RDX, CPUBits::B32);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->d.u32), RAX);
            break;
        case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            MK_IMMB_64(RAX);
            MK_IMMC_64(RDX);
            pp.emit_CmpXChg(RCX, RDX, CPUBits::B64);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->d.u32), RAX);
            break;

        case ByteCodeOp::IntrinsicS8x1:
        {
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                MK_IMMB_8(RAX);
                pp.emit_CopyN(RCX, RAX, CPUBits::B8);
                pp.emit_OpN_Immediate(RCX, 7, CPUOp::SAR, CPUBits::B8);
                pp.emit_OpN(RCX, RAX, CPUOp::XOR, CPUBits::B8);
                pp.emit_OpN(RCX, RAX, CPUOp::SUB, CPUBits::B8);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountNz:
                MK_IMMB_U8_TO_U32(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::POPCNT, CPUBits::B32);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountTz:
                MK_IMMB_U8_TO_U32(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::BSF, CPUBits::B32);
                pp.emit_Load32_Immediate(RCX, 8);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B32);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountLz:
                MK_IMMB_U8_TO_U32(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::BSR, CPUBits::B32);
                pp.emit_Load32_Immediate(RCX, 15);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B32);
                pp.emit_OpN_Immediate(RAX, 7, CPUOp::XOR, CPUBits::B32);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CopyN(RCX, RAX, CPUBits::B16);
                pp.emit_OpN_Immediate(RCX, 15, CPUOp::SAR, CPUBits::B16);
                pp.emit_OpN(RCX, RAX, CPUOp::XOR, CPUBits::B16);
                pp.emit_OpN(RCX, RAX, CPUOp::SUB, CPUBits::B16);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountNz:
                MK_IMMB_16(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::POPCNT, CPUBits::B16);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountTz:
                MK_IMMB_16(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::BSF, CPUBits::B16);
                pp.emit_Load16_Immediate(RCX, 16);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B16);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountLz:
                MK_IMMB_16(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::BSR, CPUBits::B16);
                pp.emit_Load16_Immediate(RCX, 31);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B16);
                pp.emit_OpN_Immediate(RAX, 15, CPUOp::XOR, CPUBits::B16);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicByteSwap:
                MK_IMMB_16(RAX);
                pp.emit_BSwapN(RAX, CPUBits::B16);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CopyN(RCX, RAX, CPUBits::B32);
                pp.emit_OpN_Immediate(RCX, 31, CPUOp::SAR, CPUBits::B32);
                pp.emit_OpN(RCX, RAX, CPUOp::XOR, CPUBits::B32);
                pp.emit_OpN(RCX, RAX, CPUOp::SUB, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountNz:
                MK_IMMB_32(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::POPCNT, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountTz:
                MK_IMMB_32(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::BSF, CPUBits::B32);
                pp.emit_Load32_Immediate(RCX, 32);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountLz:
                MK_IMMB_32(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::BSR, CPUBits::B32);
                pp.emit_Load32_Immediate(RCX, 63);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B32);
                pp.emit_OpN_Immediate(RAX, 31, CPUOp::XOR, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicByteSwap:
                MK_IMMB_32(RAX);
                pp.emit_BSwapN(RAX, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CopyN(RCX, RAX, CPUBits::B64);
                pp.emit_OpN_Immediate(RCX, 63, CPUOp::SAR, CPUBits::B64);
                pp.emit_OpN(RCX, RAX, CPUOp::XOR, CPUBits::B64);
                pp.emit_OpN(RCX, RAX, CPUOp::SUB, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountNz:
                MK_IMMB_64(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::POPCNT, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountTz:
                MK_IMMB_64(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::BSF, CPUBits::B64);
                pp.emit_Load32_Immediate(RCX, 64);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicBitCountLz:
                MK_IMMB_64(RAX);
                pp.emit_OpN(RAX, RAX, CPUOp::BSR, CPUBits::B64);
                pp.emit_Load64_Immediate(RCX, 127);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVE, CPUBits::B64);
                pp.emit_OpN_Immediate(RAX, 63, CPUOp::XOR, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicByteSwap:
                MK_IMMB_64(RAX);
                pp.emit_BSwapN(RAX, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CmpN(RCX, RAX, CPUBits::B32);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_S8_TO_S32(RAX);
                MK_IMMC_S8_TO_S32(RCX);
                pp.emit_CmpN(RAX, RCX, CPUBits::B32);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CmpN(RCX, RAX, CPUBits::B16);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_16(RAX);
                MK_IMMC_16(RCX);
                pp.emit_CmpN(RAX, RCX, CPUBits::B16);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CmpN(RCX, RAX, CPUBits::B32);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_32(RAX);
                MK_IMMC_32(RCX);
                pp.emit_CmpN(RAX, RCX, CPUBits::B32);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CmpN(RCX, RAX, CPUBits::B64);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_64(RAX);
                MK_IMMC_64(RCX);
                pp.emit_CmpN(RAX, RCX, CPUBits::B64);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVL, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CmpN(RCX, RAX, CPUBits::B32);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B32);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_U8_TO_U32(RAX);
                MK_IMMC_U8_TO_U32(RCX);
                pp.emit_CmpN(RAX, RCX, CPUBits::B32);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B32);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicRol:
                MK_IMMB_S8_TO_S32(RAX);
                MK_IMMC_S8_TO_S32(RCX);
                pp.emit_RotateN(RAX, RCX, CPUOp::ROL, CPUBits::B32);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicRor:
                MK_IMMB_S8_TO_S32(RAX);
                MK_IMMC_S8_TO_S32(RCX);
                pp.emit_RotateN(RAX, RCX, CPUOp::ROR, CPUBits::B32);
                pp.emit_Store8_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CmpN(RCX, RAX, CPUBits::B16);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B16);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_16(RAX);
                MK_IMMC_16(RCX);
                pp.emit_CmpN(RAX, RCX, CPUBits::B16);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B16);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicRol:
                MK_IMMB_16(RAX);
                MK_IMMC_16(RCX);
                pp.emit_RotateN(RAX, RCX, CPUOp::ROL, CPUBits::B32);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicRor:
                MK_IMMB_16(RAX);
                MK_IMMC_16(RCX);
                pp.emit_RotateN(RAX, RCX, CPUOp::ROR, CPUBits::B32);
                pp.emit_Store16_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CmpN(RCX, RAX, CPUBits::B32);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_32(RAX);
                MK_IMMC_32(RCX);
                pp.emit_CmpN(RAX, RCX, CPUBits::B32);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicRol:
                MK_IMMB_32(RAX);
                MK_IMMC_32(RCX);
                pp.emit_RotateN(RAX, RCX, CPUOp::ROL, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicRor:
                MK_IMMB_32(RAX);
                MK_IMMC_32(RCX);
                pp.emit_RotateN(RAX, RCX, CPUOp::ROR, CPUBits::B32);
                pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
                pp.emit_CmpN(RCX, RAX, CPUBits::B64);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_64(RAX);
                MK_IMMC_64(RCX);
                pp.emit_CmpN(RAX, RCX, CPUBits::B64);
                pp.emit_CMovN(RAX, RCX, CPUOp::CMOVB, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicRol:
                MK_IMMB_64(RAX);
                MK_IMMC_64(RCX);
                pp.emit_RotateN(RAX, RCX, CPUOp::ROL, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            case TokenId::IntrinsicRor:
                MK_IMMB_64(RAX);
                MK_IMMC_64(RCX);
                pp.emit_RotateN(RAX, RCX, CPUOp::ROR, CPUBits::B64);
                pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            break;
        }

        case ByteCodeOp::IntrinsicF32x2:
        {
            pp.pushParams.clear();
            if (ip->flags & BCI_IMM_B)
                pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u32});
            else
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
            if (ip->flags & BCI_IMM_C)
                pp.pushParams.push_back({CPUPushParamType::Imm, ip->c.u32});
            else
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});

            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_F32(XMM0);
                MK_IMMC_F32(XMM1);
                pp.emit_OpF32(XMM0, XMM1, CPUOp::FMIN);
                pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_F32(XMM0);
                MK_IMMC_F32(XMM1);
                pp.emit_OpF32(XMM0, XMM1, CPUOp::FMAX);
                pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
                break;

            case TokenId::IntrinsicPow:
                emitInternalCallExt(pp, g_LangSpec->name_powf, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;
            case TokenId::IntrinsicATan2:
                emitInternalCallExt(pp, g_LangSpec->name_atan2f, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicF64x2:
        {
            pp.pushParams.clear();
            if (ip->flags & BCI_IMM_B)
                pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u64});
            else
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});
            if (ip->flags & BCI_IMM_C)
                pp.pushParams.push_back({CPUPushParamType::Imm, ip->c.u64});
            else
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->c.u32});

            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                MK_IMMB_F64(XMM0);
                MK_IMMC_F64(XMM1);
                pp.emit_OpF64(XMM0, XMM1, CPUOp::FMIN);
                pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
                break;
            case TokenId::IntrinsicMax:
                MK_IMMB_F64(XMM0);
                MK_IMMC_F64(XMM1);
                pp.emit_OpF64(XMM0, XMM1, CPUOp::FMAX);
                pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
                break;

            case TokenId::IntrinsicPow:
                emitInternalCallExt(pp, g_LangSpec->name_pow, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;
            case TokenId::IntrinsicATan2:
                emitInternalCallExt(pp, g_LangSpec->name_atan2, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;
            default:
                ok = false;
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicF32x1:
        {
            pp.pushParams.clear();
            if (ip->flags & BCI_IMM_B)
                pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u32});
            else
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});

            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                MK_IMMB_F32(XMM0);
                pp.emit_OpF32(XMM0, XMM0, CPUOp::FSQRT);
                pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
                break;
            case TokenId::IntrinsicAbs:
                MK_IMMB_F32(XMM0);
                pp.emit_Load64_Immediate(RAX, 0x7FFFFFFF);
                pp.emit_CopyF64(XMM1, RAX);
                pp.emit_OpF32(XMM0, XMM1, CPUOp::FAND);
                pp.emit_StoreF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
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
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            break;
        }

        case ByteCodeOp::IntrinsicF64x1:
        {
            pp.pushParams.clear();
            if (ip->flags & BCI_IMM_B)
                pp.pushParams.push_back({CPUPushParamType::Imm, ip->b.u64});
            else
                pp.pushParams.push_back({CPUPushParamType::Reg, ip->b.u32});

            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                MK_IMMB_F64(XMM0);
                pp.emit_OpF64(XMM0, XMM0, CPUOp::FSQRT);
                pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
                break;
            case TokenId::IntrinsicAbs:
                MK_IMMB_F64(XMM0);
                pp.emit_Load64_Immediate(RAX, 0x7FFFFFFF'FFFFFFFF);
                pp.emit_CopyF64(XMM1, RAX);
                pp.emit_OpF64(XMM0, XMM1, CPUOp::FAND);
                pp.emit_StoreF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
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
                Report::internalError(buildParameters.module, FMT("unknown intrinsic [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
                break;
            }

            break;
        }

        case ByteCodeOp::InternalFailedAssume:
            emitInternalCall(pp, g_LangSpec->name__failedAssume, {ip->a.u32});
            break;
        case ByteCodeOp::IntrinsicGetErr:
            SWAG_ASSERT(ip->b.u32 == ip->a.u32 + 1);
            emitInternalCall(pp, g_LangSpec->name_aterr, {}, REG_OFFSET(ip->a.u32));
            break;
        case ByteCodeOp::InternalSetErr:
            emitInternalCall(pp, g_LangSpec->name__seterr, {ip->a.u32, ip->b.u32});
            break;
        case ByteCodeOp::InternalHasErr:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_Load32_Indirect(offsetof(SwagContext, hasError), RCX, RAX);
            pp.emit_Store32_Indirect(REG_OFFSET(ip->a.u32), RCX);
            break;
        case ByteCodeOp::JumpIfError:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(offsetof(SwagContext, hasError), RCX, RAX);
            pp.emit_TestN(RCX, RCX, CPUBits::B32);
            pp.emit_Jump(JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNoError:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Load32_Indirect(offsetof(SwagContext, hasError), RCX, RAX);
            pp.emit_TestN(RCX, RCX, CPUBits::B32);
            pp.emit_Jump(JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::InternalPushErr:
            pp.emit_Call(g_LangSpec->name__pusherr);
            break;
        case ByteCodeOp::InternalPopErr:
            pp.emit_Call(g_LangSpec->name__poperr);
            break;
        case ByteCodeOp::InternalCatchErr:
            pp.emit_Call(g_LangSpec->name__catcherr);
            break;
        case ByteCodeOp::InternalInitStackTrace:
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            pp.emit_Store32_Immediate(offsetof(SwagContext, traceIndex), 0, RAX);
            break;
        case ByteCodeOp::InternalStackTraceConst:
            pp.emit_Symbol_RelocationAddr(RAX, pp.symCSIndex, ip->b.u32);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->a.u32), RAX);
            emitInternalCall(pp, g_LangSpec->name__stackTrace, {ip->a.u32});
            break;
        case ByteCodeOp::InternalStackTrace:
            emitInternalCall(pp, g_LangSpec->name__stackTrace, {ip->a.u32});
            break;

        default:
            ok = false;
            Report::internalError(buildParameters.module, FMT("unknown instruction [[%s]] during backend generation", g_ByteCodeOpDesc[(int) ip->op].name));
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
