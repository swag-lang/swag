#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Backend/SCBE/Obj/SCBE_Coff.h"
#include "Report/Diagnostic.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

void SCBE::computeUnwind(const VectorNative<CPUReg>&   unwindRegs,
                         const VectorNative<uint32_t>& unwindOffsetRegs,
                         uint32_t                      sizeStack,
                         uint32_t                      offsetSubRSP,
                         VectorNative<uint16_t>&       unwind) const
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

    VectorNative<CPUReg>   unwindRegs;
    VectorNative<uint32_t> unwindOffsetRegs;

    // RDI will be a pointer to the stack, and the list of registers is stored at the start
    // of the stack
    pp.emitPush(CPUReg::RDI);
    unwindRegs.push_back(CPUReg::RDI);
    unwindOffsetRegs.push_back(concat.totalCount() - beforeProlog);

    // Push on scratch register per parameter
    while (cpuFct->numScratchRegs < min(cc.numScratchRegisters, min(cc.paramByRegisterCount, numTotalRegs)))
    {
        auto cpuReg = static_cast<CPUReg>(static_cast<int>(cc.firstScratchRegister) + cpuFct->numScratchRegs);
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
            pp.emitLoad(CPUReg::RAX, sizeStack + sizeParamsStack, CPUBits::B64);
            pp.emitCall(R"(__chkstk)");
        }
    }

    pp.emitOp(CPUReg::RSP, sizeStack + sizeParamsStack, CPUOp::SUB, CPUBits::B64);

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
    pp.emitSetAddress(CPUReg::RDI, CPUReg::RSP, sizeParamsStack);

    // Save register parameters
    uint32_t iReg = 0;
    while (iReg < min(cc.paramByRegisterCount, numTotalRegs))
    {
        auto     typeParam   = typeFunc->registerIdxToType(iReg);
        uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, iReg);
        if (cc.useRegisterFloat && typeParam->isNativeFloat())
            pp.emitStore(CPUReg::RDI, stackOffset, cc.paramByRegisterFloat[iReg], CPUBits::F64);
        else
            pp.emitStore(CPUReg::RDI, stackOffset, cc.paramByRegisterInteger[iReg], CPUBits::B64);

        if (iReg < cpuFct->numScratchRegs)
            pp.emitLoad(static_cast<CPUReg>(static_cast<int>(cc.firstScratchRegister) + iReg), CPUReg::RDI, stackOffset, CPUBits::B64);

        iReg++;
    }

    // Save pointer to return value if this is a return by copy
    if (CallConv::returnByAddress(typeFunc) && iReg < cc.paramByRegisterCount)
    {
        uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, iReg);
        pp.emitStore(CPUReg::RDI, stackOffset, cc.paramByRegisterInteger[iReg], CPUBits::B64);
        iReg++;
    }

    // Save C variadic
    if (typeFunc->isFctCVariadic())
    {
        while (iReg < cc.paramByRegisterCount)
        {
            uint32_t stackOffset = cpuFct->offsetCallerStackParams + REG_OFFSET(iReg);
            pp.emitStore(CPUReg::RDI, stackOffset, cc.paramByRegisterInteger[iReg], CPUBits::B64);
            iReg++;
        }
    }

    // Use R11 as base pointer for capture parameters
    // This is used to debug and have access to capture parameters, even if we "lose" rcx
    // which is the register that will have a pointer to the capture buffer (but rcx is volatile)
    if (typeFunc->isClosure() && debug)
        pp.emitCopy(CPUReg::R12, CPUReg::RCX, CPUBits::B64);

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
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitOp(CPUReg::RCX, ip->c.u8, CPUOp::ADD, CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::MUL, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::Add32byVB32:
                pp.emitOp(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u64, CPUOp::ADD, CPUBits::B32);
                break;
            case ByteCodeOp::Add64byVB64:
                pp.emitOp(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u64, CPUOp::ADD, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearRA:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, CPUBits::B64);
                break;
            case ByteCodeOp::ClearRAx2:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, CPUBits::B64);
                break;
            case ByteCodeOp::ClearRAx3:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), 0, CPUBits::B64);
                break;
            case ByteCodeOp::ClearRAx4:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->d.u32), 0, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyRBtoRA8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::U8, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::CopyRBtoRA16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::U16, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::CopyRBtoRA32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::CopyRBtoRA64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::CopyRBtoRA64x2:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::CopyRBAddrToRA:
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32));
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CastBool8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B8);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CastBool16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B16);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CastBool32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CastBool64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::CastS8S16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::S8, CPUSignedType::S16);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                break;

            case ByteCodeOp::CastS8S32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::S8, CPUSignedType::S32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::CastS16S32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::S16, CPUSignedType::S32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                break;

            case ByteCodeOp::CastS8S64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::S8, CPUSignedType::S64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::CastS16S64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::S16, CPUSignedType::S64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::CastS32S64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::S32, CPUSignedType::S64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::CastF32S32:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::F32);
                pp.emitOp(CPUReg::RAX, CPUReg::XMM0, CPUOp::CVTF2I, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::CastF64S64:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::F64);
                pp.emitOp(CPUReg::RAX, CPUReg::XMM0, CPUOp::CVTF2I, CPUBits::F64, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::CastS8F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::S8, CPUSignedType::S32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastS16F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::S16, CPUSignedType::S32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastS32F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastS64F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastU8F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::U8, CPUSignedType::U32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastU16F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::U16, CPUSignedType::U32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastU32F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastU64F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastF64F32:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::F64);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTF2F, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;

            case ByteCodeOp::CastS8F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::S8, CPUSignedType::S32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::CastS16F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::S16, CPUSignedType::S32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::CastS32F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::CastS64F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F64, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::CastU8F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::U8, CPUSignedType::U32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::CastU16F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUSignedType::U16, CPUSignedType::U32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::CastU32F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::CastU64F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitCast(CPUReg::XMM0, CPUReg::RAX, CPUSignedType::F64, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;

            case ByteCodeOp::CastF32F64:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::F32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTF2F, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
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

                /////////////////////////////////////

            case ByteCodeOp::BinOpXorU8:
                emitBinOpAtReg(pp, ip, CPUOp::XOR, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpXorU16:
                emitBinOpAtReg(pp, ip, CPUOp::XOR, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpXorU32:
                emitBinOpAtReg(pp, ip, CPUOp::XOR, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpXorU64:
                emitBinOpAtReg(pp, ip, CPUOp::XOR, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpMulS8:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::IMUL, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS8), true);
                break;
            case ByteCodeOp::BinOpMulS16:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::IMUL, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS16), true);
                break;
            case ByteCodeOp::BinOpMulS32:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::IMUL, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS32), true);
                break;
            case ByteCodeOp::BinOpMulS64:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::IMUL, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS64), true);
                break;
            case ByteCodeOp::BinOpMulU8:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::MUL, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU8), false);
                break;
            case ByteCodeOp::BinOpMulU16:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::MUL, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU16), false);
                break;
            case ByteCodeOp::BinOpMulU32:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::MUL, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU32), false);
                break;
            case ByteCodeOp::BinOpMulU64:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::MUL, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU64), false);
                break;
            case ByteCodeOp::BinOpMulF32:
                emitBinOpAtReg(pp, ip, CPUOp::FMUL, CPUBits::F32);
                break;
            case ByteCodeOp::BinOpMulF64:
                emitBinOpAtReg(pp, ip, CPUOp::FMUL, CPUBits::F64);
                break;

            case ByteCodeOp::BinOpMulS8_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::IMUL, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpMulS16_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::IMUL, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpMulS32_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::IMUL, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpMulS64_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::IMUL, CPUBits::B64);
                break;
            case ByteCodeOp::BinOpMulU8_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::MUL, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpMulU16_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::MUL, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpMulU32_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::MUL, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpMulU64_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::MUL, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpModuloS8:
                emitBinOpDivAtReg(pp, ip, CPUOp::IMOD, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpModuloS16:
                emitBinOpDivAtReg(pp, ip, CPUOp::IMOD, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpModuloS32:
                emitBinOpDivAtReg(pp, ip, CPUOp::IMOD, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpModuloS64:
                emitBinOpDivAtReg(pp, ip, CPUOp::IMOD, CPUBits::B64);
                break;
            case ByteCodeOp::BinOpModuloU8:
                emitBinOpDivAtReg(pp, ip, CPUOp::MOD, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpModuloU16:
                emitBinOpDivAtReg(pp, ip, CPUOp::MOD, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpModuloU32:
                emitBinOpDivAtReg(pp, ip, CPUOp::MOD, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpModuloU64:
                emitBinOpDivAtReg(pp, ip, CPUOp::MOD, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpDivS8:
                emitBinOpDivAtReg(pp, ip, CPUOp::IDIV, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpDivS16:
                emitBinOpDivAtReg(pp, ip, CPUOp::IDIV, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpDivS32:
                emitBinOpDivAtReg(pp, ip, CPUOp::IDIV, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpDivS64:
                emitBinOpDivAtReg(pp, ip, CPUOp::IDIV, CPUBits::B64);
                break;
            case ByteCodeOp::BinOpDivU8:
                emitBinOpDivAtReg(pp, ip, CPUOp::DIV, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpDivU16:
                emitBinOpDivAtReg(pp, ip, CPUOp::DIV, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpDivU32:
                emitBinOpDivAtReg(pp, ip, CPUOp::DIV, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpDivU64:
                emitBinOpDivAtReg(pp, ip, CPUOp::DIV, CPUBits::B64);
                break;
            case ByteCodeOp::BinOpDivF32:
                emitBinOpAtReg(pp, ip, CPUOp::FDIV, CPUBits::F32);
                break;
            case ByteCodeOp::BinOpDivF64:
                emitBinOpAtReg(pp, ip, CPUOp::FDIV, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpPlusS8:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::ADD, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS8), true);
                break;
            case ByteCodeOp::BinOpPlusS16:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::ADD, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS16), true);
                break;
            case ByteCodeOp::BinOpPlusS32:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::ADD, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS32), true);
                break;
            case ByteCodeOp::BinOpPlusS64:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::ADD, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS64), true);
                break;
            case ByteCodeOp::BinOpPlusU8:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::ADD, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU8), false);
                break;
            case ByteCodeOp::BinOpPlusU16:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::ADD, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU16), false);
                break;
            case ByteCodeOp::BinOpPlusU32:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::ADD, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU32), false);
                break;
            case ByteCodeOp::BinOpPlusU64:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::ADD, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU64), false);
                break;
            case ByteCodeOp::BinOpPlusF32:
                emitBinOpAtReg(pp, ip, CPUOp::FADD, CPUBits::F32);
                break;
            case ByteCodeOp::BinOpPlusF64:
                emitBinOpAtReg(pp, ip, CPUOp::FADD, CPUBits::F64);
                break;

            case ByteCodeOp::BinOpPlusS8_Safe:
            case ByteCodeOp::BinOpPlusU8_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::ADD, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpPlusS16_Safe:
            case ByteCodeOp::BinOpPlusU16_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::ADD, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpPlusS32_Safe:
            case ByteCodeOp::BinOpPlusU32_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::ADD, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpPlusS64_Safe:
            case ByteCodeOp::BinOpPlusU64_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::ADD, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpMinusS8:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::SUB, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS8), true);
                break;
            case ByteCodeOp::BinOpMinusS16:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::SUB, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS16), true);
                break;
            case ByteCodeOp::BinOpMinusS32:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::SUB, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS32), true);
                break;
            case ByteCodeOp::BinOpMinusS64:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::SUB, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS64), true);
                break;
            case ByteCodeOp::BinOpMinusU8:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::SUB, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU8), false);
                break;
            case ByteCodeOp::BinOpMinusU16:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::SUB, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU16), false);
                break;
            case ByteCodeOp::BinOpMinusU32:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::SUB, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU32), false);
                break;
            case ByteCodeOp::BinOpMinusU64:
                emitBinOpAtRegOverflow(pp, ip, CPUOp::SUB, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU64), false);
                break;
            case ByteCodeOp::BinOpMinusF32:
                emitBinOpAtReg(pp, ip, CPUOp::FSUB, CPUBits::F32);
                break;
            case ByteCodeOp::BinOpMinusF64:
                emitBinOpAtReg(pp, ip, CPUOp::FSUB, CPUBits::F64);
                break;

            case ByteCodeOp::BinOpMinusS8_Safe:
            case ByteCodeOp::BinOpMinusU8_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::SUB, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpMinusS16_Safe:
            case ByteCodeOp::BinOpMinusU16_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::SUB, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpMinusS32_Safe:
            case ByteCodeOp::BinOpMinusU32_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::SUB, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpMinusS64_Safe:
            case ByteCodeOp::BinOpMinusU64_Safe:
                emitBinOpAtReg(pp, ip, CPUOp::SUB, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskAnd8:
                emitBinOpAtReg(pp, ip, CPUOp::AND, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpBitmaskAnd16:
                emitBinOpAtReg(pp, ip, CPUOp::AND, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpBitmaskAnd32:
                emitBinOpAtReg(pp, ip, CPUOp::AND, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpBitmaskAnd64:
                emitBinOpAtReg(pp, ip, CPUOp::AND, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskOr8:
                emitBinOpAtReg(pp, ip, CPUOp::OR, CPUBits::B8);
                break;
            case ByteCodeOp::BinOpBitmaskOr16:
                emitBinOpAtReg(pp, ip, CPUOp::OR, CPUBits::B16);
                break;
            case ByteCodeOp::BinOpBitmaskOr32:
                emitBinOpAtReg(pp, ip, CPUOp::OR, CPUBits::B32);
                break;
            case ByteCodeOp::BinOpBitmaskOr64:
                emitBinOpAtReg(pp, ip, CPUOp::OR, CPUBits::B64);
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

                /////////////////////////////////////

            case ByteCodeOp::AffectOpXorEqU8:
                emitBinOpEq(pp, ip, 0, CPUOp::XOR, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpXorEqU16:
                emitBinOpEq(pp, ip, 0, CPUOp::XOR, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpXorEqU32:
                emitBinOpEq(pp, ip, 0, CPUOp::XOR, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpXorEqU64:
                emitBinOpEq(pp, ip, 0, CPUOp::XOR, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpOrEqU8:
                emitBinOpEq(pp, ip, 0, CPUOp::OR, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpOrEqU16:
                emitBinOpEq(pp, ip, 0, CPUOp::OR, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpOrEqU32:
                emitBinOpEq(pp, ip, 0, CPUOp::OR, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpOrEqU64:
                emitBinOpEq(pp, ip, 0, CPUOp::OR, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpAndEqU8:
                emitBinOpEq(pp, ip, 0, CPUOp::AND, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpAndEqU16:
                emitBinOpEq(pp, ip, 0, CPUOp::AND, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpAndEqU32:
                emitBinOpEq(pp, ip, 0, CPUOp::AND, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpAndEqU64:
                emitBinOpEq(pp, ip, 0, CPUOp::AND, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMulEqS8:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::IMUL, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS8), true);
                break;
            case ByteCodeOp::AffectOpMulEqS16:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::IMUL, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS16), true);
                break;
            case ByteCodeOp::AffectOpMulEqS32:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::IMUL, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS32), true);
                break;
            case ByteCodeOp::AffectOpMulEqS64:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::IMUL, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS64), true);
                break;
            case ByteCodeOp::AffectOpMulEqU8:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::MUL, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU8), false);
                break;
            case ByteCodeOp::AffectOpMulEqU16:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::MUL, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU16), false);
                break;
            case ByteCodeOp::AffectOpMulEqU32:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::MUL, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU32), false);
                break;
            case ByteCodeOp::AffectOpMulEqU64:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::MUL, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU64), false);
                break;
            case ByteCodeOp::AffectOpMulEqF32:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::FMUL, CPUBits::F32);
                break;
            case ByteCodeOp::AffectOpMulEqF64:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::FMUL, CPUBits::F64);
                break;

            case ByteCodeOp::AffectOpMulEqS8_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::IMUL, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpMulEqS16_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::IMUL, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpMulEqS32_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::IMUL, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpMulEqS64_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::IMUL, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpMulEqU8_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::MUL, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpMulEqU16_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::MUL, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpMulEqU32_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::MUL, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpMulEqU64_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::MUL, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpMulEqS8_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::IMUL, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpMulEqS16_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::IMUL, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpMulEqS32_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::IMUL, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpMulEqS64_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::IMUL, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpMulEqU8_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::MUL, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpMulEqU16_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::MUL, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpMulEqU32_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::MUL, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpMulEqU64_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::MUL, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpMulEqF32_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::FMUL, CPUBits::F32);
                break;
            case ByteCodeOp::AffectOpMulEqF64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::FMUL, CPUBits::F64);
                break;

            case ByteCodeOp::AffectOpMulEqS8_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::IMUL, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpMulEqS16_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::IMUL, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpMulEqS32_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::IMUL, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpMulEqS64_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::IMUL, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpMulEqU8_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::MUL, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpMulEqU16_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::MUL, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpMulEqU32_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::MUL, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpMulEqU64_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::MUL, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpMulEqF32_SS:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::FMUL, CPUBits::F32);
                break;
            case ByteCodeOp::AffectOpMulEqF64_SS:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::FMUL, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpDivEqS8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUSignedType::S8, CPUSignedType::S16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpDivEqS8_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::S8, CPUSignedType::S16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpDivEqS8_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::S8, CPUSignedType::S16);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::AffectOpDivEqS16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B16);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpDivEqS16_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B16);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpDivEqS16_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B16);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B16);
                break;

            case ByteCodeOp::AffectOpDivEqS32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B32);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpDivEqS32_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B32);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpDivEqS32_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B32);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B32);
                break;

            case ByteCodeOp::AffectOpDivEqS64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B64);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpDivEqS64_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B64);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpDivEqS64_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B64);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpDivEqU8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUSignedType::U8, CPUSignedType::U32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpDivEqU8_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::U8, CPUSignedType::U32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpDivEqU8_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::U8, CPUSignedType::U32);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::AffectOpDivEqU16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B16);
                pp.emitClear(CPUReg::RDX, CPUBits::B16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpDivEqU16_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B16);
                pp.emitClear(CPUReg::RDX, CPUBits::B16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpDivEqU16_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B16);
                pp.emitClear(CPUReg::RDX, CPUBits::B16);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B16);
                break;

            case ByteCodeOp::AffectOpDivEqU32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B32);
                pp.emitClear(CPUReg::RDX, CPUBits::B32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpDivEqU32_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B32);
                pp.emitClear(CPUReg::RDX, CPUBits::B32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpDivEqU32_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B32);
                pp.emitClear(CPUReg::RDX, CPUBits::B32);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B32);
                break;

            case ByteCodeOp::AffectOpDivEqU64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B64);
                pp.emitClear(CPUReg::RDX, CPUBits::B64);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpDivEqU64_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B64);
                pp.emitClear(CPUReg::RDX, CPUBits::B64);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpDivEqU64_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B64);
                pp.emitClear(CPUReg::RDX, CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpDivEqF32:
                emitIMMB(pp, ip, CPUReg::XMM1, CPUBits::F32);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::XMM0, CPUReg::RAX, 0, CPUBits::F32);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FDIV, CPUBits::F32);
                pp.emitStore(CPUReg::RAX, 0, CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::AffectOpDivEqF32_S:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::F32);
                emitIMMB(pp, ip, CPUReg::XMM1, CPUBits::F32);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FDIV, CPUBits::F32);
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RAX, 0, CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::AffectOpDivEqF32_SS:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::F32);
                pp.emitLoad(CPUReg::XMM1, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::F32);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FDIV, CPUBits::F32);
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RAX, 0, CPUReg::XMM0, CPUBits::F32);
                break;

            case ByteCodeOp::AffectOpDivEqF64:
                emitIMMB(pp, ip, CPUReg::XMM1, CPUBits::F64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::XMM0, CPUReg::RAX, 0, CPUBits::F64);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FDIV, CPUBits::F64);
                pp.emitStore(CPUReg::RAX, 0, CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::AffectOpDivEqF64_S:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::F64);
                emitIMMB(pp, ip, CPUReg::XMM1, CPUBits::F64);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FDIV, CPUBits::F64);
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RAX, 0, CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::AffectOpDivEqF64_SS:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::F64);
                pp.emitLoad(CPUReg::XMM1, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::F64);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FDIV, CPUBits::F64);
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RAX, 0, CPUReg::XMM0, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpModuloEqS8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUSignedType::S8, CPUSignedType::S16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitCopyDownUp(CPUReg::RAX, CPUBits::B8);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpModuloEqS8_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::S8, CPUSignedType::S16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitCopyDownUp(CPUReg::RAX, CPUBits::B8);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpModuloEqS8_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::S8, CPUSignedType::S16);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B8);
                pp.emitCopyDownUp(CPUReg::RAX, CPUBits::B8);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::AffectOpModuloEqS16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B16);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpModuloEqS16_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B16);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpModuloEqS16_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B16);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B16);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B16);
                break;

            case ByteCodeOp::AffectOpModuloEqS32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B32);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpModuloEqS32_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B32);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpModuloEqS32_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B32);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B32);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B32);
                break;

            case ByteCodeOp::AffectOpModuloEqS64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B64);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpModuloEqS64_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B64);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpModuloEqS64_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B64);
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpModuloEqU8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUSignedType::S8, CPUSignedType::S16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitCopyDownUp(CPUReg::RAX, CPUBits::B8);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpModuloEqU8_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::U8, CPUSignedType::U32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitCopyDownUp(CPUReg::RAX, CPUBits::B8);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpModuloEqU8_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::U8, CPUSignedType::U32);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B8);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B8);
                pp.emitCopyDownUp(CPUReg::RAX, CPUBits::B8);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::AffectOpModuloEqU16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B16);
                pp.emitClear(CPUReg::RDX, CPUBits::B16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpModuloEqU16_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B16);
                pp.emitClear(CPUReg::RDX, CPUBits::B16);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpModuloEqU16_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B16);
                pp.emitClear(CPUReg::RDX, CPUBits::B16);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B16);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B16);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B16);
                break;

            case ByteCodeOp::AffectOpModuloEqU32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B32);
                pp.emitClear(CPUReg::RDX, CPUBits::B32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpModuloEqU32_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B32);
                pp.emitClear(CPUReg::RDX, CPUBits::B32);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpModuloEqU32_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B32);
                pp.emitClear(CPUReg::RDX, CPUBits::B32);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B32);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B32);
                break;

            case ByteCodeOp::AffectOpModuloEqU64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B64);
                pp.emitClear(CPUReg::RDX, CPUBits::B64);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpModuloEqU64_S:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B64);
                pp.emitClear(CPUReg::RDX, CPUBits::B64);
                emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpModuloEqU64_SS:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B64);
                pp.emitClear(CPUReg::RDX, CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::DIV, CPUBits::B64);
                pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RDX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpPlusEqS8:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::ADD, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS8), true);
                break;
            case ByteCodeOp::AffectOpPlusEqS8_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::ADD, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpPlusEqS8_SSafe:
            case ByteCodeOp::AffectOpPlusEqU8_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::ADD, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpPlusEqS8_SSSafe:
            case ByteCodeOp::AffectOpPlusEqU8_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::ADD, CPUBits::B8);
                break;

            case ByteCodeOp::AffectOpPlusEqS16:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::ADD, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS16), true);
                break;
            case ByteCodeOp::AffectOpPlusEqS16_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::ADD, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpPlusEqS16_SSafe:
            case ByteCodeOp::AffectOpPlusEqU16_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::ADD, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpPlusEqS16_SSSafe:
            case ByteCodeOp::AffectOpPlusEqU16_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::ADD, CPUBits::B16);
                break;

            case ByteCodeOp::AffectOpPlusEqS32:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::ADD, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS32), true);
                break;
            case ByteCodeOp::AffectOpPlusEqS32_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::ADD, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpPlusEqS32_SSafe:
            case ByteCodeOp::AffectOpPlusEqU32_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::ADD, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpPlusEqS32_SSSafe:
            case ByteCodeOp::AffectOpPlusEqU32_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::ADD, CPUBits::B32);
                break;

            case ByteCodeOp::AffectOpPlusEqS64:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::ADD, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS64), true);
                break;
            case ByteCodeOp::AffectOpPlusEqS64_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::ADD, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpPlusEqS64_SSafe:
            case ByteCodeOp::AffectOpPlusEqU64_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::ADD, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpPlusEqS64_SSSafe:
            case ByteCodeOp::AffectOpPlusEqU64_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::ADD, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpPlusEqU8:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::ADD, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU8), false);
                break;
            case ByteCodeOp::AffectOpPlusEqU8_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::ADD, CPUBits::B8);
                break;

            case ByteCodeOp::AffectOpPlusEqU16:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::ADD, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU16), false);
                break;
            case ByteCodeOp::AffectOpPlusEqU16_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::ADD, CPUBits::B16);
                break;

            case ByteCodeOp::AffectOpPlusEqU32:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::ADD, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU32), false);
                break;
            case ByteCodeOp::AffectOpPlusEqU32_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::ADD, CPUBits::B32);
                break;

            case ByteCodeOp::AffectOpPlusEqU64:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::ADD, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU64), false);
                break;
            case ByteCodeOp::AffectOpPlusEqU64_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::ADD, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpPlusEqF32:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::FADD, CPUBits::F32);
                break;
            case ByteCodeOp::AffectOpPlusEqF32_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::FADD, CPUBits::F32);
                break;
            case ByteCodeOp::AffectOpPlusEqF32_SS:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::FADD, CPUBits::F32);
                break;

            case ByteCodeOp::AffectOpPlusEqF64:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::FADD, CPUBits::F64);
                break;
            case ByteCodeOp::AffectOpPlusEqF64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::FADD, CPUBits::F64);
                break;
            case ByteCodeOp::AffectOpPlusEqF64_SS:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::FADD, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMinusEqS8:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::SUB, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS8), true);
                break;
            case ByteCodeOp::AffectOpMinusEqS8_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::SUB, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpMinusEqS8_SSafe:
            case ByteCodeOp::AffectOpMinusEqU8_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::SUB, CPUBits::B8);
                break;
            case ByteCodeOp::AffectOpMinusEqS8_SSSafe:
            case ByteCodeOp::AffectOpMinusEqU8_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::SUB, CPUBits::B8);
                break;

            case ByteCodeOp::AffectOpMinusEqS16:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::SUB, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS16), true);
                break;
            case ByteCodeOp::AffectOpMinusEqS16_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::SUB, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpMinusEqS16_SSafe:
            case ByteCodeOp::AffectOpMinusEqU16_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::SUB, CPUBits::B16);
                break;
            case ByteCodeOp::AffectOpMinusEqS16_SSSafe:
            case ByteCodeOp::AffectOpMinusEqU16_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::SUB, CPUBits::B16);
                break;

            case ByteCodeOp::AffectOpMinusEqS32:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::SUB, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS32), true);
                break;
            case ByteCodeOp::AffectOpMinusEqS32_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::SUB, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpMinusEqS32_SSafe:
            case ByteCodeOp::AffectOpMinusEqU32_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::SUB, CPUBits::B32);
                break;
            case ByteCodeOp::AffectOpMinusEqS32_SSSafe:
            case ByteCodeOp::AffectOpMinusEqU32_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::SUB, CPUBits::B32);
                break;

            case ByteCodeOp::AffectOpMinusEqS64:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::SUB, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS64), true);
                break;
            case ByteCodeOp::AffectOpMinusEqS64_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::SUB, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpMinusEqS64_SSafe:
            case ByteCodeOp::AffectOpMinusEqU64_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::SUB, CPUBits::B64);
                break;
            case ByteCodeOp::AffectOpMinusEqS64_SSSafe:
            case ByteCodeOp::AffectOpMinusEqU64_SSSafe:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::SUB, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpMinusEqU8:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::SUB, CPUBits::B8, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU8), false);
                break;
            case ByteCodeOp::AffectOpMinusEqU8_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::SUB, CPUBits::B8);
                break;

            case ByteCodeOp::AffectOpMinusEqU16:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::SUB, CPUBits::B16, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16), false);
                break;
            case ByteCodeOp::AffectOpMinusEqU16_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::SUB, CPUBits::B16);
                break;

            case ByteCodeOp::AffectOpMinusEqU32:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::SUB, CPUBits::B32, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU32), false);
                break;
            case ByteCodeOp::AffectOpMinusEqU32_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::SUB, CPUBits::B32);
                break;

            case ByteCodeOp::AffectOpMinusEqU64:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::SUB, CPUBits::B64, ByteCodeGen::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU64), false);
                break;
            case ByteCodeOp::AffectOpMinusEqU64_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::SUB, CPUBits::B64);
                break;

            case ByteCodeOp::AffectOpMinusEqF32:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::FSUB, CPUBits::F32);
                break;
            case ByteCodeOp::AffectOpMinusEqF32_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::FSUB, CPUBits::F32);
                break;
            case ByteCodeOp::AffectOpMinusEqF32_SS:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::FSUB, CPUBits::F32);
                break;

            case ByteCodeOp::AffectOpMinusEqF64:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::FSUB, CPUBits::F64);
                break;
            case ByteCodeOp::AffectOpMinusEqF64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::FSUB, CPUBits::F64);
                break;
            case ByteCodeOp::AffectOpMinusEqF64_SS:
                emitBinOpEqSS(pp, ip, offsetStack, CPUOp::FSUB, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ZeroToTrue:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::LowerZeroToTrue:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                pp.emitOp(CPUReg::RAX, 31, CPUOp::SHR, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::LowerEqZeroToTrue:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetLE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::GreaterZeroToTrue:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetG);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::GreaterEqZeroToTrue:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                pp.emitNot(CPUReg::RAX, CPUBits::B32);
                pp.emitOp(CPUReg::RAX, 31, CPUOp::SHR, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpGreaterS8:
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetG);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterS16:
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetG);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterS32:
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetG);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterS64:
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetG);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterU8:
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetA);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterU16:
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetA);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterU32:
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetA);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterU64:
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetA);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterF32:
                emitBinOp(pp, ip, CPUBits::F32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetA);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterF64:
                emitBinOp(pp, ip, CPUBits::F64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetA);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::CompareOpGreaterEqS8:
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetGE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterEqS16:
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetGE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterEqS32:
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetGE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterEqS64:
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetGE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterEqU8:
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetAE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterEqU16:
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetAE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterEqU32:
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetAE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterEqU64:
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetAE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterEqF32:
                emitBinOp(pp, ip, CPUBits::F32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetAE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterEqF64:
                emitBinOp(pp, ip, CPUBits::F64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetAE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpLowerS8:
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetL);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerS16:
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetL);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerS32:
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetL);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerS64:
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetL);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerU8:
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetB);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerU16:
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetB);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerU32:
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetB);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerU64:
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetB);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerF32:
                emitBinOp(pp, ip, CPUBits::F32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetB);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerF64:
                emitBinOp(pp, ip, CPUBits::F64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetB);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::CompareOpLowerEqS8:
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetLE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerEqS16:
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetLE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerEqS32:
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetLE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerEqS64:
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetLE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerEqU8:
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetBE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerEqU16:
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetBE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerEqU32:
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetBE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerEqU64:
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetBE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerEqF32:
                emitBinOp(pp, ip, CPUBits::F32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetBE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpLowerEqF64:
                emitBinOp(pp, ip, CPUBits::F64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetBE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOp3Way8:
                pp.emitClear(CPUReg::R8, CPUBits::B32);
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::R8, CPUSet::SetG);
                pp.emitLoad(CPUReg::RAX, 0xFFFFFFFF, CPUBits::B32);
                pp.emitCMov(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVGE, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::CompareOp3Way16:
                pp.emitClear(CPUReg::R8, CPUBits::B32);
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::R8, CPUSet::SetG);
                pp.emitLoad(CPUReg::RAX, 0xFFFFFFFF, CPUBits::B32);
                pp.emitCMov(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVGE, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::CompareOp3Way32:
                pp.emitClear(CPUReg::R8, CPUBits::B32);
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::R8, CPUSet::SetG);
                pp.emitLoad(CPUReg::RAX, 0xFFFFFFFF, CPUBits::B32);
                pp.emitCMov(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVGE, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::CompareOp3Way64:
                pp.emitClear(CPUReg::R8, CPUBits::B32);
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::R8, CPUSet::SetG);
                pp.emitLoad(CPUReg::RAX, 0xFFFFFFFF, CPUBits::B32);
                pp.emitCMov(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVGE, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::CompareOp3WayF32:
                pp.emitClear(CPUReg::R8, CPUBits::B32);
                emitIMMA(pp, ip, CPUReg::XMM0, CPUBits::F32);
                emitIMMB(pp, ip, CPUReg::XMM1, CPUBits::F32);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::UCOMIF, CPUBits::F32);
                pp.emitSet(CPUReg::R8, CPUSet::SetA);
                pp.emitOp(CPUReg::XMM1, CPUReg::XMM0, CPUOp::UCOMIF, CPUBits::F32);
                pp.emitLoad(CPUReg::RAX, 0xFFFFFFFF, CPUBits::B32);
                pp.emitCMov(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVBE, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::CompareOp3WayF64:
                pp.emitClear(CPUReg::R8, CPUBits::B32);
                emitIMMA(pp, ip, CPUReg::XMM0, CPUBits::F64);
                emitIMMB(pp, ip, CPUReg::XMM1, CPUBits::F64);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::UCOMIF, CPUBits::F64);
                pp.emitSet(CPUReg::R8, CPUSet::SetA);
                pp.emitOp(CPUReg::XMM1, CPUReg::XMM0, CPUOp::UCOMIF, CPUBits::F64);
                pp.emitLoad(CPUReg::RAX, 0xFFFFFFFF, CPUBits::B32);
                pp.emitCMov(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVBE, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpEqual8:
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpEqual16:
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpEqual32:
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpEqual64:
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpEqualF32:
                emitBinOp(pp, ip, CPUBits::F32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetEP);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpEqualF64:
                emitBinOp(pp, ip, CPUBits::F64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetEP);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpNotEqual8:
                emitBinOp(pp, ip, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpNotEqual16:
                emitBinOp(pp, ip, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpNotEqual32:
                emitBinOp(pp, ip, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpNotEqual64:
                emitBinOp(pp, ip, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpNotEqualF32:
                emitBinOp(pp, ip, CPUBits::F32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNEP);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpNotEqualF64:
                emitBinOp(pp, ip, CPUBits::F64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNEP);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
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
                emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::TestNotZero16:
                emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::TestNotZero32:
                emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::TestNotZero64:
                emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B64);
                pp.emitSet(CPUReg::RAX, CPUSet::SetNE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::NegBool:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B8);
                pp.emitOp(CPUReg::RAX, 1, CPUOp::XOR, CPUBits::B8);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::NegS32:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNeg(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                    pp.emitNeg(CPUReg::RAX, CPUBits::B64);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                }
                break;
            case ByteCodeOp::NegS64:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNeg(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                    pp.emitNeg(CPUReg::RAX, CPUBits::B64);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                }
                break;
            case ByteCodeOp::NegF32:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::F32);
                pp.emitLoad(CPUReg::RAX, 0x80000000, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, offsetFLT, CPUReg::RAX, CPUBits::B64);
                pp.emitLoad(CPUReg::XMM1, CPUReg::RDI, offsetFLT, CPUBits::F32);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FXOR, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::NegF64:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::F64);
                pp.emitLoad(CPUReg::RAX, 0x80000000'00000000, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, offsetFLT, CPUReg::RAX, CPUBits::B64);
                pp.emitLoad(CPUReg::XMM1, CPUReg::RDI, offsetFLT, CPUBits::F64);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FXOR, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::InvertU8:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNot(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B8);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B8);
                    pp.emitNot(CPUReg::RAX, CPUBits::B8);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                }
                break;
            case ByteCodeOp::InvertU16:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNot(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B16);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B16);
                    pp.emitNot(CPUReg::RAX, CPUBits::B16);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                }
                break;
            case ByteCodeOp::InvertU32:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNot(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                    pp.emitNot(CPUReg::RAX, CPUBits::B32);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                }
                break;
            case ByteCodeOp::InvertU64:
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNot(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                    pp.emitNot(CPUReg::RAX, CPUBits::B64);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
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
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::S8, CPUSignedType::S64);
                else if (ip->op == ByteCodeOp::JumpDyn16)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::S16, CPUSignedType::S64);
                else if (ip->op == ByteCodeOp::JumpDyn32)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::S32, CPUSignedType::S64);
                else
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);

                // Note:
                //
                // This is not optimal yet.
                // The sub could be removed by baking it in the 'cmp', and by changing the jump table address by subtracting the min value
                // Also, if the jump table was encoded in the text segment, then there will be no need to have two relocations
                //
                // We could in the end remove two instructions and be as the llvm generation

                pp.emitOp(CPUReg::RAX, ip->b.u64 - 1, CPUOp::SUB, CPUBits::B64);
                pp.emitCmp(CPUReg::RAX, ip->c.u64, CPUBits::B64);
                pp.emitJump(JAE, i, tableCompiler[0]);

                uint8_t* addrConstant        = nullptr;
                auto     offsetTableConstant = buildParameters.module->constantSegment.reserve(static_cast<uint32_t>(ip->c.u64) * sizeof(uint32_t), &addrConstant);

                pp.emitSymbolRelocationAddr(CPUReg::RCX, pp.symCSIndex, offsetTableConstant); // rcx = jump table
                pp.emitJumpTable(CPUReg::RCX, CPUReg::RAX);

                // + 5 for the two following instructions
                // + 7 for this instruction
                pp.emitSymbolRelocationAddr(CPUReg::RAX, symbolFuncIndex, concat.totalCount() - startAddress + 5 + 7);
                pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::ADD, CPUBits::B64);
                pp.emitJump(CPUReg::RAX);

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
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B8);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfRTTrue:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetRT + REG_OFFSET(0), CPUBits::B8);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfFalse:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B8);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfRTFalse:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetRT + REG_OFFSET(0), CPUBits::B8);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNotZero8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B8);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNotZero16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B16);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNotZero32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNotZero64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B64);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfZero8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B8);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfZero16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B16);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfZero32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfZero64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B64);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::Jump:
                pp.emitJump(JUMP, i, ip->b.s32);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfNotEqual8:
                emitJumpCmp(pp, ip, i, JNZ, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfNotEqual16:
                emitJumpCmp(pp, ip, i, JNZ, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfNotEqual32:
                emitJumpCmp(pp, ip, i, JNZ, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfNotEqual64:
                emitJumpCmp(pp, ip, i, JNZ, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfNotEqualF32:
                emitJumpCmp2(pp, ip, i, JP, JNZ, CPUBits::F32);
                break;
            case ByteCodeOp::JumpIfNotEqualF64:
                emitJumpCmp2(pp, ip, i, JP, JNZ, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfEqual8:
                emitJumpCmp(pp, ip, i, JZ, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfEqual16:
                emitJumpCmp(pp, ip, i, JZ, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfEqual32:
                emitJumpCmp(pp, ip, i, JZ, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfEqual64:
                emitJumpCmp(pp, ip, i, JZ, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfEqualF32:
                emitJumpCmp2(pp, ip, i, JP, JZ, CPUBits::F32);
                break;
            case ByteCodeOp::JumpIfEqualF64:
                emitJumpCmp2(pp, ip, i, JP, JZ, CPUBits::F64);
                break;
            case ByteCodeOp::IncJumpIfEqual64:
                pp.emitInc(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitJumpCmp(pp, ip, i, JZ, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfStackEqual8:
                emitJumpCmpAddr(pp, ip, i, JZ, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfStackNotEqual8:
                emitJumpCmpAddr(pp, ip, i, JNZ, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B8);
                break;

            case ByteCodeOp::JumpIfStackEqual16:
                emitJumpCmpAddr(pp, ip, i, JZ, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfStackNotEqual16:
                emitJumpCmpAddr(pp, ip, i, JNZ, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B16);
                break;

            case ByteCodeOp::JumpIfStackEqual32:
                emitJumpCmpAddr(pp, ip, i, JZ, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfStackNotEqual32:
                emitJumpCmpAddr(pp, ip, i, JNZ, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B32);
                break;

            case ByteCodeOp::JumpIfStackEqual64:
                emitJumpCmpAddr(pp, ip, i, JZ, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfStackNotEqual64:
                emitJumpCmpAddr(pp, ip, i, JNZ, CPUReg::RDI, offsetStack + ip->a.u32, CPUBits::B64);
                break;

            case ByteCodeOp::JumpIfDeRefEqual8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitJumpCmpAddr(pp, ip, i, JZ, CPUReg::RCX, ip->d.u32, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitJumpCmpAddr(pp, ip, i, JNZ, CPUReg::RCX, ip->d.u32, CPUBits::B8);
                break;

            case ByteCodeOp::JumpIfDeRefEqual16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitJumpCmpAddr(pp, ip, i, JZ, CPUReg::RCX, ip->d.u32, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitJumpCmpAddr(pp, ip, i, JNZ, CPUReg::RCX, ip->d.u32, CPUBits::B16);
                break;

            case ByteCodeOp::JumpIfDeRefEqual32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitJumpCmpAddr(pp, ip, i, JZ, CPUReg::RCX, ip->d.u32, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitJumpCmpAddr(pp, ip, i, JNZ, CPUReg::RCX, ip->d.u32, CPUBits::B32);
                break;

            case ByteCodeOp::JumpIfDeRefEqual64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitJumpCmpAddr(pp, ip, i, JZ, CPUReg::RCX, ip->d.u32, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitJumpCmpAddr(pp, ip, i, JNZ, CPUReg::RCX, ip->d.u32, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfLowerS8:
                emitJumpCmp(pp, ip, i, JL, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfLowerS16:
                emitJumpCmp(pp, ip, i, JL, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfLowerS32:
                emitJumpCmp(pp, ip, i, JL, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfLowerS64:
                emitJumpCmp(pp, ip, i, JL, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfLowerU8:
                emitJumpCmp(pp, ip, i, JB, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfLowerU16:
                emitJumpCmp(pp, ip, i, JB, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfLowerU32:
                emitJumpCmp(pp, ip, i, JB, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfLowerU64:
                emitJumpCmp(pp, ip, i, JB, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfLowerF32:
                emitJumpCmp(pp, ip, i, JB, CPUBits::F32);
                break;
            case ByteCodeOp::JumpIfLowerF64:
                emitJumpCmp(pp, ip, i, JB, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfLowerEqS8:
                emitJumpCmp(pp, ip, i, JLE, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfLowerEqS16:
                emitJumpCmp(pp, ip, i, JLE, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfLowerEqS32:
                emitJumpCmp(pp, ip, i, JLE, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfLowerEqS64:
                emitJumpCmp(pp, ip, i, JLE, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfLowerEqU8:
                emitJumpCmp(pp, ip, i, JBE, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfLowerEqU16:
                emitJumpCmp(pp, ip, i, JBE, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfLowerEqU32:
                emitJumpCmp(pp, ip, i, JBE, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfLowerEqU64:
                emitJumpCmp(pp, ip, i, JBE, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfLowerEqF32:
                emitJumpCmp(pp, ip, i, JBE, CPUBits::F32);
                break;
            case ByteCodeOp::JumpIfLowerEqF64:
                emitJumpCmp(pp, ip, i, JBE, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfGreaterS8:
                emitJumpCmp(pp, ip, i, JG, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfGreaterS16:
                emitJumpCmp(pp, ip, i, JG, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfGreaterS32:
                emitJumpCmp(pp, ip, i, JG, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfGreaterS64:
                emitJumpCmp(pp, ip, i, JG, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfGreaterU8:
                emitJumpCmp(pp, ip, i, JA, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfGreaterU16:
                emitJumpCmp(pp, ip, i, JA, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfGreaterU32:
                emitJumpCmp(pp, ip, i, JA, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfGreaterU64:
                emitJumpCmp(pp, ip, i, JA, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfGreaterF32:
                emitJumpCmp(pp, ip, i, JA, CPUBits::F32);
                break;
            case ByteCodeOp::JumpIfGreaterF64:
                emitJumpCmp(pp, ip, i, JA, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfGreaterEqS8:
                emitJumpCmp(pp, ip, i, JGE, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfGreaterEqS16:
                emitJumpCmp(pp, ip, i, JGE, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfGreaterEqS32:
                emitJumpCmp(pp, ip, i, JGE, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfGreaterEqS64:
                emitJumpCmp(pp, ip, i, JGE, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfGreaterEqU8:
                emitJumpCmp(pp, ip, i, JAE, CPUBits::B8);
                break;
            case ByteCodeOp::JumpIfGreaterEqU16:
                emitJumpCmp(pp, ip, i, JAE, CPUBits::B16);
                break;
            case ByteCodeOp::JumpIfGreaterEqU32:
                emitJumpCmp(pp, ip, i, JAE, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfGreaterEqU64:
                emitJumpCmp(pp, ip, i, JAE, CPUBits::B64);
                break;
            case ByteCodeOp::JumpIfGreaterEqF32:
                emitJumpCmp(pp, ip, i, JAE, CPUBits::F32);
                break;
            case ByteCodeOp::JumpIfGreaterEqF64:
                emitJumpCmp(pp, ip, i, JAE, CPUBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::DecrementRA32:
                pp.emitDec(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                break;
            case ByteCodeOp::IncrementRA64:
                pp.emitInc(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                break;
            case ByteCodeOp::DecrementRA64:
                pp.emitDec(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::DeRef8:
                SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->c.u32, CPUSignedType::U8, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::DeRef16:
                SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->c.u32, CPUSignedType::U16, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::DeRef32:
                SWAG_ASSERT(ip->c.u64 <= 0x7FFFFFFF); // If this triggers, see DeRef64 below
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->c.u32, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::DeRef64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                if (ip->c.u64 <= 0x7FFFFFFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->c.u32, CPUBits::B64);
                else
                {
                    pp.emitLoad(CPUReg::RCX, ip->c.u64, CPUBits::B64);
                    pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B64);
                }
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::DeRefStringSlice:
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFFF);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RAX, ip->c.u32 + 8, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUReg::RCX, CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->c.u32 + 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromBssSeg8:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symBSIndex, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->b.u32, CPUSignedType::U8, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromBssSeg16:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symBSIndex, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->b.u32, CPUSignedType::U16, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromBssSeg32:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symBSIndex, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->b.u32, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromBssSeg64:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symBSIndex, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->b.u32, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::GetFromMutableSeg8:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symMSIndex, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->b.u32, CPUSignedType::U8, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromMutableSeg16:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symMSIndex, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->b.u32, CPUSignedType::U16, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromMutableSeg32:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symMSIndex, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->b.u32, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromMutableSeg64:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symMSIndex, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->b.u32, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUSignedType::U8, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromStack16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUSignedType::U16, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromStack32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromStack64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetIncFromStack64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8x2:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUSignedType::U8, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->d.u32, CPUSignedType::U8, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromStack16x2:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUSignedType::U16, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->d.u32, CPUSignedType::U16, CPUSignedType::U64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromStack32x2:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->d.u32, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetFromStack64x2:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->d.u32, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetIncFromStack64DeRef8:
                if (ip->c.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, static_cast<uint32_t>(ip->c.u64), CPUSignedType::U8, CPUSignedType::U64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitOp(CPUReg::RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUSignedType::U8, CPUSignedType::U64);
                }
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetIncFromStack64DeRef16:
                if (ip->c.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, static_cast<uint32_t>(ip->c.u64), CPUSignedType::U16, CPUSignedType::U64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitOp(CPUReg::RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUSignedType::U16, CPUSignedType::U64);
                }
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetIncFromStack64DeRef32:
                if (ip->c.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, static_cast<uint32_t>(ip->c.u64), CPUBits::B32);
                }
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitOp(CPUReg::RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B32);
                }
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetIncFromStack64DeRef64:
                if (ip->c.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, static_cast<uint32_t>(ip->c.u64), CPUBits::B64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitOp(CPUReg::RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B64);
                }
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyStack8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B8);
                pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CopyStack16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B16);
                pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B16);
                break;
            case ByteCodeOp::CopyStack32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::CopyStack64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearMaskU32:
                if (ip->b.u32 == 0xFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::U8, CPUSignedType::U32);
                else if (ip->b.u32 == 0xFFFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::U16, CPUSignedType::U32);
                else
                    SWAG_ASSERT(false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                break;

            case ByteCodeOp::ClearMaskU64:
                if (ip->b.u32 == 0xFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::U8, CPUSignedType::U64);
                else if (ip->b.u32 == 0xFFFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::U16, CPUSignedType::U64);
                else if (ip->b.u32 == 0xFFFFFFFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                else
                    SWAG_ASSERT(false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetZeroAtPointer8:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RAX, ip->b.u32, 0, CPUBits::B8);
                break;
            case ByteCodeOp::SetZeroAtPointer16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RAX, ip->b.u32, 0, CPUBits::B16);
                break;
            case ByteCodeOp::SetZeroAtPointer32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RAX, ip->b.u32, 0, CPUBits::B32);
                break;
            case ByteCodeOp::SetZeroAtPointer64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RAX, ip->b.u32, 0, CPUBits::B64);
                break;
            case ByteCodeOp::SetZeroAtPointerX:
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                if (ip->b.u32 <= 128 && !buildParameters.isDebug())
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitClear(CPUReg::RAX, ip->c.u32, ip->b.u32);
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
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);
                pp.emitStore(CPUReg::RAX, ip->c.u32, 0, CPUBits::B8);
                break;
            }
            case ByteCodeOp::ClearRR16:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);
                pp.emitStore(CPUReg::RAX, ip->c.u32, 0, CPUBits::B16);
                break;
            }
            case ByteCodeOp::ClearRR32:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);
                pp.emitStore(CPUReg::RAX, ip->c.u32, 0, CPUBits::B32);
                break;
            }
            case ByteCodeOp::ClearRR64:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);
                pp.emitStore(CPUReg::RAX, ip->c.u32, 0, CPUBits::B64);
                break;
            }
            case ByteCodeOp::ClearRRX:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);

                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                if (ip->b.u32 <= 128 && !buildParameters.isDebug())
                {
                    pp.emitClear(CPUReg::RAX, ip->c.u32, ip->b.u32);
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
                pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, 0, CPUBits::B8);
                break;
            case ByteCodeOp::SetZeroStack16:
                pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, 0, CPUBits::B16);
                break;
            case ByteCodeOp::SetZeroStack32:
                pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, 0, CPUBits::B32);
                break;
            case ByteCodeOp::SetZeroStack64:
                pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, 0, CPUBits::B64);
                break;
            case ByteCodeOp::SetZeroStackX:
                if (ip->b.u32 <= 128 && !buildParameters.isDebug())
                    pp.emitClear(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u32);
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
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RAX, ip->c.u32, ip->b.u8, CPUBits::B8);
                else
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B8);
                    pp.emitStore(CPUReg::RAX, ip->c.u32, CPUReg::RCX, CPUBits::B8);
                }
                break;
            case ByteCodeOp::SetAtPointer16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RAX, ip->c.u32, ip->b.u16, CPUBits::B16);
                else
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B16);
                    pp.emitStore(CPUReg::RAX, ip->c.u32, CPUReg::RCX, CPUBits::B16);
                }
                break;
            case ByteCodeOp::SetAtPointer32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RAX, ip->c.u32, ip->b.u32, CPUBits::B32);
                else
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                    pp.emitStore(CPUReg::RAX, ip->c.u32, CPUReg::RCX, CPUBits::B32);
                }
                break;
            case ByteCodeOp::SetAtPointer64:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitStore(CPUReg::RAX, ip->c.u32, ip->b.u64, CPUBits::B64);
                }
                else if (ip->hasFlag(BCI_IMM_B))
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitLoad(CPUReg::RCX, ip->b.u64, CPUBits::B64);
                    pp.emitStore(CPUReg::RAX, ip->c.u32, CPUReg::RCX, CPUBits::B64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitStore(CPUReg::RCX, ip->c.u32, CPUReg::RAX, CPUBits::B64);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u8, CPUBits::B8);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B8);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B8);
                }
                break;
            case ByteCodeOp::SetAtStackPointer16:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u16, CPUBits::B16);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B16);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B16);
                }
                break;
            case ByteCodeOp::SetAtStackPointer32:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u32, CPUBits::B32);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B32);
                }
                break;
            case ByteCodeOp::SetAtStackPointer64:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF)
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u64, CPUBits::B64);
                else if (ip->hasFlag(BCI_IMM_B))
                {
                    pp.emitLoad(CPUReg::RAX, ip->b.u64, CPUBits::B64);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B64);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8x2:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u8, CPUBits::B8);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B8);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B8);
                }

                if (ip->hasFlag(BCI_IMM_D))
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->c.u32, ip->d.u8, CPUBits::B8);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUBits::B8);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->c.u32, CPUReg::RAX, CPUBits::B8);
                }
                break;
            case ByteCodeOp::SetAtStackPointer16x2:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u16, CPUBits::B16);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B16);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B16);
                }

                if (ip->hasFlag(BCI_IMM_D))
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->c.u32, ip->d.u16, CPUBits::B16);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUBits::B16);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->c.u32, CPUReg::RAX, CPUBits::B16);
                }
                break;
            case ByteCodeOp::SetAtStackPointer32x2:
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u32, CPUBits::B32);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B32);
                }

                if (ip->hasFlag(BCI_IMM_D))
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->c.u32, ip->d.u32, CPUBits::B32);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUBits::B32);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->c.u32, CPUReg::RAX, CPUBits::B32);
                }
                break;
            case ByteCodeOp::SetAtStackPointer64x2:
                if (ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF)
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u64, CPUBits::B64);
                else if (ip->hasFlag(BCI_IMM_B))
                {
                    pp.emitLoad(CPUReg::RAX, ip->b.u64, CPUBits::B64);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, CPUBits::B64);
                }

                if (ip->hasFlag(BCI_IMM_D) && ip->d.u64 <= 0x7FFFFFFF)
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->c.u32, ip->d.u64, CPUBits::B64);
                else if (ip->hasFlag(BCI_IMM_D))
                {
                    pp.emitLoad(CPUReg::RAX, ip->d.u64, CPUBits::B64);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->c.u32, CPUReg::RAX, CPUBits::B64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUBits::B64);
                    pp.emitStore(CPUReg::RDI, offsetStack + ip->c.u32, CPUReg::RAX, CPUBits::B64);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeStackPointer:
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::MakeStackPointerx2:
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->d.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::MakeStackPointerRT:
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32);
                pp.emitStore(CPUReg::RDI, offsetRT + REG_OFFSET(0), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeMutableSegPointer:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symMSIndex, ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::MakeBssSegPointer:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symBSIndex, ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::MakeConstantSegPointer:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symCSIndex, ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
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
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitOp(CPUReg::RAX, ip->b.u64, CPUOp::IMUL, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::Div64byVB64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, ip->b.u64, CPUBits::B64);
                pp.emitClear(CPUReg::RDX, CPUBits::B64);
                pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::IDIV, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetImmediate32:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u32, CPUBits::B32);
                break;
            case ByteCodeOp::SetImmediate64:
                if (ip->b.u64 <= 0x7FFFFFFF || ip->b.u64 >> 32 == 0xFFFFFFFF)
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u32, CPUBits::B64);
                else
                {
                    pp.emitLoad(CPUReg::RAX, ip->b.u64, CPUBits::B64);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::MemCpy8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RDX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitCopy(CPUReg::RCX, CPUReg::RDX, 1, 0);
                break;
            case ByteCodeOp::MemCpy16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RDX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitCopy(CPUReg::RCX, CPUReg::RDX, 2, 0);
                break;
            case ByteCodeOp::MemCpy32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RDX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitCopy(CPUReg::RCX, CPUReg::RDX, 4, 0);
                break;
            case ByteCodeOp::MemCpy64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RDX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitCopy(CPUReg::RCX, CPUReg::RDX, 8, 0);
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMemCpy:
                if (ip->hasFlag(BCI_IMM_C) && ip->c.u64 <= 128 && !buildParameters.isDebug())
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitLoad(CPUReg::RDX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                    pp.emitCopy(CPUReg::RCX, CPUReg::RDX, ip->c.u32, 0);
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
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitClear(CPUReg::RCX, 0, ip->c.u32);
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
                pp.emitSymbolRelocationAddr(CPUReg::RCX, pp.symPI_processInfos, 0);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RCX, CPUBits::B64);
                break;

            case ByteCodeOp::IntrinsicCVaStart:
            {
                uint32_t paramIdx = typeFunc->numParamsRegisters();
                if (CallConv::returnByAddress(typeFunc))
                    paramIdx += 1;
                uint32_t stackOffset = cpuFct->offsetCallerStackParams + REG_OFFSET(paramIdx);
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, stackOffset);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, CPUBits::B64);
                break;
            }
            case ByteCodeOp::IntrinsicCVaEnd:
                break;
            case ByteCodeOp::IntrinsicCVaArg:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RAX, 0, CPUBits::B64);
                switch (ip->c.u32)
                {
                    case 1:
                        pp.emitClear(CPUReg::RDX, CPUBits::B64);
                        pp.emitLoad(CPUReg::RDX, CPUReg::RCX, 0, CPUBits::B8);
                        break;
                    case 2:
                        pp.emitClear(CPUReg::RDX, CPUBits::B64);
                        pp.emitLoad(CPUReg::RDX, CPUReg::RCX, 0, CPUBits::B16);
                        break;
                    case 4:
                        pp.emitLoad(CPUReg::RDX, CPUReg::RCX, 0, CPUBits::B32);
                        break;
                    case 8:
                        pp.emitLoad(CPUReg::RDX, CPUReg::RCX, 0, CPUBits::B64);
                        break;
                    default:
                        break;
                }

                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUReg::RDX, CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, 8, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, 0, CPUReg::RCX, CPUOp::ADD, CPUBits::B64);
                break;

            case ByteCodeOp::IntrinsicArguments:
                SWAG_ASSERT(ip->b.u32 == ip->a.u32 + 1);
                emitInternalCall(pp, g_LangSpec->name_at_args, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicModules:
                if (buildParameters.module->modulesSliceOffset == UINT32_MAX)
                {
                    pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32));
                    pp.emitStore(CPUReg::RAX, 0, 0, CPUBits::B64);
                    pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32));
                    pp.emitStore(CPUReg::RAX, 0, 0, CPUBits::B64);
                }
                else
                {
                    pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symCSIndex, buildParameters.module->modulesSliceOffset);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                    pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32));
                    pp.emitStore(CPUReg::RAX, 0, buildParameters.module->moduleDependencies.count + 1, CPUBits::B64);
                }
                break;
            case ByteCodeOp::IntrinsicGvtd:
                if (buildParameters.module->globalVarsToDropSliceOffset == UINT32_MAX)
                {
                    pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32));
                    pp.emitStore(CPUReg::RAX, 0, 0, CPUBits::B64);
                    pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32));
                    pp.emitStore(CPUReg::RAX, 0, 0, CPUBits::B64);
                }
                else
                {
                    pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symMSIndex, buildParameters.module->globalVarsToDropSliceOffset);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                    pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32));
                    pp.emitStore(CPUReg::RAX, 0, buildParameters.module->globalVarsToDrop.count, CPUBits::B64);
                }
                break;

            case ByteCodeOp::IntrinsicCompiler:
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32));
                pp.emitStore(CPUReg::RAX, 0, 0, CPUBits::B64);
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32));
                pp.emitStore(CPUReg::RAX, 0, 0, CPUBits::B64);
                break;
            case ByteCodeOp::IntrinsicIsByteCode:
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32));
                pp.emitStore(CPUReg::RAX, 0, 0, CPUBits::B32);
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
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetResult);
                if (ip->hasFlag(BCI_IMM_A) && ip->a.u64 <= 0x7FFFFFFF)
                {
                    pp.emitStore(CPUReg::RAX, 0, ip->a.u64, CPUBits::B64);
                }
                else if (ip->hasFlag(BCI_IMM_A))
                {
                    pp.emitLoad(CPUReg::RCX, ip->a.u64, CPUBits::B64);
                    pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, CPUBits::B64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, CPUBits::B64);
                }

                break;

            case ByteCodeOp::CopyRARBtoRR2:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RAX, 8, CPUReg::RCX, CPUBits::B64);
                break;
            }

            case ByteCodeOp::CopyRAtoRT:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RDI, offsetRT + REG_OFFSET(0), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::SaveRRtoRA:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            }
            case ByteCodeOp::CopyRRtoRA:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);
                if (ip->b.u64)
                {
                    pp.emitLoad(CPUReg::RCX, ip->b.u64, CPUBits::B64);
                    pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::ADD, CPUBits::B64);
                }

                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            }

            case ByteCodeOp::CopyRTtoRA:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetRT + REG_OFFSET(0), CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::CopyRT2toRARB:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetRT + REG_OFFSET(0), CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetRT + REG_OFFSET(1), CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::CopySP:
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->c.u32));
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
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
                                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B8);
                                pp.emitStore(CPUReg::RSP, offset, CPUReg::RAX, CPUBits::B8);
                                break;
                            case 2:
                                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B16);
                                pp.emitStore(CPUReg::RSP, offset, CPUReg::RAX, CPUBits::B16);
                                break;
                            case 4:
                                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B32);
                                pp.emitStore(CPUReg::RSP, offset, CPUReg::RAX, CPUBits::B32);
                                break;
                            case 8:
                                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B64);
                                pp.emitStore(CPUReg::RSP, offset, CPUReg::RAX, CPUBits::B64);
                                break;
                            default:
                                break;
                        }

                        idxParam--;
                        offset += sizeOf;
                    }

                    pp.emitSetAddress(CPUReg::RAX, CPUReg::RSP, sizeParamsStack - variadicStackSize);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
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
                        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(pushRAParams[idxParam]), CPUBits::B64);
                        pp.emitStore(CPUReg::RSP, offset, CPUReg::RAX, CPUBits::B64);
                        idxParam--;
                        offset += 8;
                    }

                    pp.emitSetAddress(CPUReg::RAX, CPUReg::RSP, sizeParamsStack - variadicStackSize);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
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
                pp.emitLoad(CPUReg::RAX, 0, CPUBits::B64, true);

                CPURelocation reloc;
                reloc.virtualAddress = concat.totalCount() - sizeof(uint64_t) - pp.textSectionOffset;
                auto callSym         = pp.getOrAddSymbol(callName, CPUSymbolKind::Extern);
                reloc.symbolIndex    = callSym->index;
                reloc.type           = IMAGE_REL_AMD64_ADDR64;
                pp.relocTableTextSection.table.push_back(reloc);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            }

            case ByteCodeOp::IntrinsicMakeCallback:
            {
                // Test if it's a bytecode lambda
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, SWAG_LAMBDA_BC_MARKER, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::AND, CPUBits::B64);
                pp.emitTest(CPUReg::RCX, CPUReg::RCX, CPUBits::B64);
                auto jumpBCToAfterAddr   = pp.emitJumpLong(JZ);
                auto jumpBCToAfterOffset = concat.totalCount();

                // ByteCode lambda
                //////////////////

                pp.emitCopy(CPUReg::RCX, CPUReg::RAX, CPUBits::B64);
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symPI_makeCallback, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B64);
                pp.emitCallIndirect(CPUReg::RAX);

                // End
                //////////////////
                *jumpBCToAfterAddr = concat.totalCount() - jumpBCToAfterOffset;
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);

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
                pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetResult);
                if (ip->hasFlag(BCI_IMM_A) && ip->a.u64 <= 0x7FFFFFFF)
                {
                    pp.emitStore(CPUReg::RAX, 0, ip->a.u64, CPUBits::B64);
                }
                else if (ip->hasFlag(BCI_IMM_A))
                {
                    pp.emitLoad(CPUReg::RCX, ip->a.u64, CPUBits::B64);
                    pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, CPUBits::B64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, CPUBits::B64);
                }
                [[fallthrough]];

            case ByteCodeOp::Ret:

                // Emit result
                if (!returnType->isVoid() && !CallConv::returnByAddress(typeFunc))
                {
                    pp.emitLoad(cc.returnByRegisterInteger, CPUReg::RDI, offsetResult, CPUBits::B64);
                    if (returnType->isNative(NativeTypeKind::F32))
                        pp.emitCopy(cc.returnByRegisterFloat, CPUReg::RAX, CPUBits::F32);
                    else if (returnType->isNative(NativeTypeKind::F64))
                        pp.emitCopy(cc.returnByRegisterFloat, CPUReg::RAX, CPUBits::F64);
                }

                pp.emitOp(CPUReg::RSP, sizeStack + sizeParamsStack, CPUOp::ADD, CPUBits::B64);
                for (int32_t rRet = static_cast<int32_t>(unwindRegs.size()) - 1; rRet >= 0; rRet--)
                    pp.emitPop(unwindRegs[rRet]);
                pp.emitRet();
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMulAddF32:
            {
                emitIMMB(pp, ip, CPUReg::XMM0, CPUBits::F32);
                emitIMMC(pp, ip, CPUReg::XMM1, CPUBits::F32);
                emitIMMD(pp, ip, CPUReg::XMM2, CPUBits::F32);
                pp.emitMulAdd(CPUReg::XMM0, CPUReg::XMM1, CPUReg::XMM2, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            }
            case ByteCodeOp::IntrinsicMulAddF64:
            {
                emitIMMB(pp, ip, CPUReg::XMM0, CPUBits::F64);
                emitIMMC(pp, ip, CPUReg::XMM1, CPUBits::F64);
                emitIMMD(pp, ip, CPUReg::XMM2, CPUBits::F64);
                pp.emitMulAdd(CPUReg::XMM0, CPUReg::XMM1, CPUReg::XMM2, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            }

            case ByteCodeOp::IntrinsicAtomicAddS8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B8);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                emitBinOpEqLock(pp, ip, CPUOp::ADD, CPUBits::B8);
                break;
            case ByteCodeOp::IntrinsicAtomicAddS16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B16);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B16);
                emitBinOpEqLock(pp, ip, CPUOp::ADD, CPUBits::B16);
                break;
            case ByteCodeOp::IntrinsicAtomicAddS32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                emitBinOpEqLock(pp, ip, CPUOp::ADD, CPUBits::B32);
                break;
            case ByteCodeOp::IntrinsicAtomicAddS64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                emitBinOpEqLock(pp, ip, CPUOp::ADD, CPUBits::B64);
                break;

            case ByteCodeOp::IntrinsicAtomicAndS8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B8);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                emitBinOpEqLock(pp, ip, CPUOp::AND, CPUBits::B8);
                break;
            case ByteCodeOp::IntrinsicAtomicAndS16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B16);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B16);
                emitBinOpEqLock(pp, ip, CPUOp::AND, CPUBits::B16);
                break;
            case ByteCodeOp::IntrinsicAtomicAndS32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                emitBinOpEqLock(pp, ip, CPUOp::AND, CPUBits::B32);
                break;
            case ByteCodeOp::IntrinsicAtomicAndS64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                emitBinOpEqLock(pp, ip, CPUOp::AND, CPUBits::B64);
                break;

            case ByteCodeOp::IntrinsicAtomicOrS8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B8);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                emitBinOpEqLock(pp, ip, CPUOp::OR, CPUBits::B8);
                break;
            case ByteCodeOp::IntrinsicAtomicOrS16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B16);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B16);
                emitBinOpEqLock(pp, ip, CPUOp::OR, CPUBits::B16);
                break;
            case ByteCodeOp::IntrinsicAtomicOrS32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                emitBinOpEqLock(pp, ip, CPUOp::OR, CPUBits::B32);
                break;
            case ByteCodeOp::IntrinsicAtomicOrS64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                emitBinOpEqLock(pp, ip, CPUOp::OR, CPUBits::B64);
                break;

            case ByteCodeOp::IntrinsicAtomicXorS8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B8);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                emitBinOpEqLock(pp, ip, CPUOp::XOR, CPUBits::B8);
                break;
            case ByteCodeOp::IntrinsicAtomicXorS16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B16);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B16);
                emitBinOpEqLock(pp, ip, CPUOp::XOR, CPUBits::B16);
                break;
            case ByteCodeOp::IntrinsicAtomicXorS32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                emitBinOpEqLock(pp, ip, CPUOp::XOR, CPUBits::B32);
                break;
            case ByteCodeOp::IntrinsicAtomicXorS64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                emitBinOpEqLock(pp, ip, CPUOp::XOR, CPUBits::B64);
                break;

            case ByteCodeOp::IntrinsicAtomicXchgS8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B8);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                emitBinOpEqLock(pp, ip, CPUOp::XCHG, CPUBits::B8);
                break;
            case ByteCodeOp::IntrinsicAtomicXchgS16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B16);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B16);
                emitBinOpEqLock(pp, ip, CPUOp::XCHG, CPUBits::B16);
                break;
            case ByteCodeOp::IntrinsicAtomicXchgS32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                emitBinOpEqLock(pp, ip, CPUOp::XCHG, CPUBits::B32);
                break;
            case ByteCodeOp::IntrinsicAtomicXchgS64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                emitBinOpEqLock(pp, ip, CPUOp::XCHG, CPUBits::B64);
                break;

            case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8);
                emitIMMC(pp, ip, CPUReg::RDX, CPUBits::B8);
                pp.emitCmpXChg(CPUReg::RCX, CPUReg::RDX, CPUBits::B8);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                emitIMMC(pp, ip, CPUReg::RDX, CPUBits::B16);
                pp.emitCmpXChg(CPUReg::RCX, CPUReg::RDX, CPUBits::B16);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUReg::RAX, CPUBits::B16);
                break;
            case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                emitIMMC(pp, ip, CPUReg::RDX, CPUBits::B32);
                pp.emitCmpXChg(CPUReg::RCX, CPUReg::RDX, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                emitIMMC(pp, ip, CPUReg::RDX, CPUBits::B64);
                pp.emitCmpXChg(CPUReg::RCX, CPUReg::RDX, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::IntrinsicS8x1:
            {
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8);
                        pp.emitCopy(CPUReg::RCX, CPUReg::RAX, CPUBits::B8);
                        pp.emitOp(CPUReg::RCX, 7, CPUOp::SAR, CPUBits::B8);
                        pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::XOR, CPUBits::B8);
                        pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::SUB, CPUBits::B8);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUSignedType::U8, CPUSignedType::U32);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::POPCNT, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUSignedType::U8, CPUSignedType::U32);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::BSF, CPUBits::B32);
                        pp.emitLoad(CPUReg::RCX, 8, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUSignedType::U8, CPUSignedType::U32);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::BSR, CPUBits::B32);
                        pp.emitLoad(CPUReg::RCX, 15, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, CPUBits::B32);
                        pp.emitOp(CPUReg::RAX, 7, CPUOp::XOR, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        pp.emitCopy(CPUReg::RCX, CPUReg::RAX, CPUBits::B16);
                        pp.emitOp(CPUReg::RCX, 15, CPUOp::SAR, CPUBits::B16);
                        pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::XOR, CPUBits::B16);
                        pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::SUB, CPUBits::B16);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::POPCNT, CPUBits::B16);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::BSF, CPUBits::B16);
                        pp.emitLoad(CPUReg::RCX, 16, CPUBits::B16);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, CPUBits::B16);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::BSR, CPUBits::B16);
                        pp.emitLoad(CPUReg::RCX, 31, CPUBits::B16);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, CPUBits::B16);
                        pp.emitOp(CPUReg::RAX, 15, CPUOp::XOR, CPUBits::B16);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        pp.emitBSwap(CPUReg::RAX, CPUBits::B16);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        pp.emitCopy(CPUReg::RCX, CPUReg::RAX, CPUBits::B32);
                        pp.emitOp(CPUReg::RCX, 31, CPUOp::SAR, CPUBits::B32);
                        pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::XOR, CPUBits::B32);
                        pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::SUB, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::POPCNT, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::BSF, CPUBits::B32);
                        pp.emitLoad(CPUReg::RCX, 32, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::BSR, CPUBits::B32);
                        pp.emitLoad(CPUReg::RCX, 63, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, CPUBits::B32);
                        pp.emitOp(CPUReg::RAX, 31, CPUOp::XOR, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        pp.emitBSwap(CPUReg::RAX, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        pp.emitCopy(CPUReg::RCX, CPUReg::RAX, CPUBits::B64);
                        pp.emitOp(CPUReg::RCX, 63, CPUOp::SAR, CPUBits::B64);
                        pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::XOR, CPUBits::B64);
                        pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::SUB, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::POPCNT, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::BSF, CPUBits::B64);
                        pp.emitLoad(CPUReg::RCX, 64, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::BSR, CPUBits::B64);
                        pp.emitLoad(CPUReg::RCX, 127, CPUBits::B64);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, CPUBits::B64);
                        pp.emitOp(CPUReg::RAX, 63, CPUOp::XOR, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        pp.emitBSwap(CPUReg::RAX, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUSignedType::S8, CPUSignedType::S32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUSignedType::S8, CPUSignedType::S32);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUSignedType::S8, CPUSignedType::S32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUSignedType::S8, CPUSignedType::S32);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B16);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, CPUBits::B16);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B16);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, CPUBits::B16);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B32);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B32);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B64);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, CPUBits::B64);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B64);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, CPUBits::B64);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUSignedType::U8, CPUSignedType::U32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUSignedType::U8, CPUSignedType::U32);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUSignedType::U8, CPUSignedType::U32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUSignedType::U8, CPUSignedType::U32);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicRol:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUSignedType::S8, CPUSignedType::S32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUSignedType::S8, CPUSignedType::S32);
                        pp.emitRotate(CPUReg::RAX, CPUReg::RCX, CPUOp::ROL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicRor:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUSignedType::S8, CPUSignedType::S32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUSignedType::S8, CPUSignedType::S32);
                        pp.emitRotate(CPUReg::RAX, CPUReg::RCX, CPUOp::ROR, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B16);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, CPUBits::B16);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, CPUBits::B16);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B16);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, CPUBits::B16);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, CPUBits::B16);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                        break;
                    case TokenId::IntrinsicRol:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B16);
                        pp.emitRotate(CPUReg::RAX, CPUReg::RCX, CPUOp::ROL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                        break;
                    case TokenId::IntrinsicRor:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B16);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B16);
                        pp.emitRotate(CPUReg::RAX, CPUReg::RCX, CPUOp::ROR, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B32);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B32);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                        break;
                    case TokenId::IntrinsicRol:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B32);
                        pp.emitRotate(CPUReg::RAX, CPUReg::RCX, CPUOp::ROL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                        break;
                    case TokenId::IntrinsicRor:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B32);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B32);
                        pp.emitRotate(CPUReg::RAX, CPUReg::RCX, CPUOp::ROR, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B64);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, CPUBits::B64);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B64);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, CPUBits::B64);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                        break;
                    case TokenId::IntrinsicRol:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B64);
                        pp.emitRotate(CPUReg::RAX, CPUReg::RCX, CPUOp::ROL, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                        break;
                    case TokenId::IntrinsicRor:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B64);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B64);
                        pp.emitRotate(CPUReg::RAX, CPUReg::RCX, CPUOp::ROR, CPUBits::B64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
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
                        emitIMMB(pp, ip, CPUReg::XMM0, CPUBits::F32);
                        emitIMMC(pp, ip, CPUReg::XMM1, CPUBits::F32);
                        pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FMIN, CPUBits::F32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::XMM0, CPUBits::F32);
                        emitIMMC(pp, ip, CPUReg::XMM1, CPUBits::F32);
                        pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FMAX, CPUBits::F32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
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
                        emitIMMB(pp, ip, CPUReg::XMM0, CPUBits::F64);
                        emitIMMC(pp, ip, CPUReg::XMM1, CPUBits::F64);
                        pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FMIN, CPUBits::F64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::XMM0, CPUBits::F64);
                        emitIMMC(pp, ip, CPUReg::XMM1, CPUBits::F64);
                        pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FMAX, CPUBits::F64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
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
                        emitIMMB(pp, ip, CPUReg::XMM0, CPUBits::F32);
                        pp.emitOp(CPUReg::XMM0, CPUReg::XMM0, CPUOp::FSQRT, CPUBits::F32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                        break;
                    case TokenId::IntrinsicAbs:
                        emitIMMB(pp, ip, CPUReg::XMM0, CPUBits::F32);
                        pp.emitLoad(CPUReg::RAX, 0x7FFFFFFF, CPUBits::B64);
                        pp.emitCopy(CPUReg::XMM1, CPUReg::RAX, CPUBits::F64);
                        pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FAND, CPUBits::F32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
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
                        emitIMMB(pp, ip, CPUReg::XMM0, CPUBits::F64);
                        pp.emitOp(CPUReg::XMM0, CPUReg::XMM0, CPUOp::FSQRT, CPUBits::F64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                        break;
                    case TokenId::IntrinsicAbs:
                        emitIMMB(pp, ip, CPUReg::XMM0, CPUBits::F64);
                        pp.emitLoad(CPUReg::RAX, 0x7FFFFFFF'FFFFFFFF, CPUBits::B64);
                        pp.emitCopy(CPUReg::XMM1, CPUReg::RAX, CPUBits::F64);
                        pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FAND, CPUBits::F64);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
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
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RAX, offsetof(SwagContext, hasError), CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RCX, CPUBits::B32);
                break;
            case ByteCodeOp::JumpIfError:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RAX, offsetof(SwagContext, hasError), CPUBits::B32);
                pp.emitTest(CPUReg::RCX, CPUReg::RCX, CPUBits::B32);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNoError:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RAX, offsetof(SwagContext, hasError), CPUBits::B32);
                pp.emitTest(CPUReg::RCX, CPUReg::RCX, CPUBits::B32);
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
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RAX, offsetof(SwagContext, traceIndex), 0, CPUBits::B32);
                break;
            case ByteCodeOp::InternalStackTraceConst:
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symCSIndex, ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
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
