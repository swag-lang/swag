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

    pp.init(buildParameters);
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
    CPUBits                                     numBits = CPUBits::INVALID;
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

        numBits = CPUBits::INVALID;
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
            case ByteCodeOp::CopyRBtoRA16:
            case ByteCodeOp::CopyRBtoRA32:
            case ByteCodeOp::CopyRBtoRA64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64, numBits, false);
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
            case ByteCodeOp::CastBool16:
            case ByteCodeOp::CastBool32:
            case ByteCodeOp::CastBool64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, numBits);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::NE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::CastS8S16:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B16, CPUBits::B8, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B16);
                break;

            case ByteCodeOp::CastS8S32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32, CPUBits::B8, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::CastS16S32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32, CPUBits::B16, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                break;

            case ByteCodeOp::CastS8S64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64, CPUBits::B8, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::CastS16S64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64, CPUBits::B16, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::CastS32S64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64, CPUBits::B32, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::CastF32S32:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::F32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTF2I, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                break;
            case ByteCodeOp::CastF64S64:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::F64);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTF2I, CPUBits::F64, EMITF_B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::CastS8F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32, CPUBits::B8, true);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastS16F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32, CPUBits::B16, true);
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
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32, EMITF_B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastU8F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32, CPUBits::B8, false);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastU16F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32, CPUBits::B16, false);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastU32F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32, EMITF_B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastU64F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F32, EMITF_B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;
            case ByteCodeOp::CastF64F32:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::F64);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTF2F, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F32);
                break;

            case ByteCodeOp::CastS8F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32, CPUBits::B8, true);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::CastS16F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32, CPUBits::B16, true);
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
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F64, EMITF_B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::CastU8F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32, CPUBits::B8, false);
                pp.emitOp(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, CPUBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, CPUBits::F64);
                break;
            case ByteCodeOp::CastU16F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32, CPUBits::B16, false);
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
                pp.emitCast(CPUReg::XMM0, CPUReg::RAX, CPUBits::F64, CPUBits::B64, false);
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
            case ByteCodeOp::BinOpShiftLeftS16:
            case ByteCodeOp::BinOpShiftLeftU16:
            case ByteCodeOp::BinOpShiftLeftS32:
            case ByteCodeOp::BinOpShiftLeftU32:
            case ByteCodeOp::BinOpShiftLeftS64:
            case ByteCodeOp::BinOpShiftLeftU64:
                emitShiftLogical(pp, ip, CPUOp::SHL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpShiftRightS8:
            case ByteCodeOp::BinOpShiftRightS16:
            case ByteCodeOp::BinOpShiftRightS32:
            case ByteCodeOp::BinOpShiftRightS64:
                emitShiftRightArithmetic(pp, ip);
                break;
            case ByteCodeOp::BinOpShiftRightU8:
            case ByteCodeOp::BinOpShiftRightU16:
            case ByteCodeOp::BinOpShiftRightU32:
            case ByteCodeOp::BinOpShiftRightU64:
                emitShiftLogical(pp, ip, CPUOp::SHR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpXorU8:
            case ByteCodeOp::BinOpXorU16:
            case ByteCodeOp::BinOpXorU32:
            case ByteCodeOp::BinOpXorU64:
                emitBinOp(pp, ip, CPUOp::XOR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpMulS8:
            case ByteCodeOp::BinOpMulS16:
            case ByteCodeOp::BinOpMulS32:
            case ByteCodeOp::BinOpMulS64:
                emitBinOpOverflow(pp, ip, CPUOp::IMUL, SafetyMsg::Mul, SCBE_CPU::getCPUType(ip->op));
                break;
            case ByteCodeOp::BinOpMulU8:
            case ByteCodeOp::BinOpMulU16:
            case ByteCodeOp::BinOpMulU32:
            case ByteCodeOp::BinOpMulU64:
                emitBinOpOverflow(pp, ip, CPUOp::MUL, SafetyMsg::Mul, SCBE_CPU::getCPUType(ip->op));
                break;
            case ByteCodeOp::BinOpMulF32:
            case ByteCodeOp::BinOpMulF64:
                emitBinOp(pp, ip, CPUOp::FMUL);
                break;

            case ByteCodeOp::BinOpMulS8_Safe:
            case ByteCodeOp::BinOpMulS16_Safe:
            case ByteCodeOp::BinOpMulS32_Safe:
            case ByteCodeOp::BinOpMulS64_Safe:
                emitBinOp(pp, ip, CPUOp::IMUL);
                break;
            case ByteCodeOp::BinOpMulU8_Safe:
            case ByteCodeOp::BinOpMulU16_Safe:
            case ByteCodeOp::BinOpMulU32_Safe:
            case ByteCodeOp::BinOpMulU64_Safe:
                emitBinOp(pp, ip, CPUOp::MUL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpModuloS8:
            case ByteCodeOp::BinOpModuloS16:
            case ByteCodeOp::BinOpModuloS32:
            case ByteCodeOp::BinOpModuloS64:
                emitBinOp(pp, ip, CPUOp::IMOD);
                break;
            case ByteCodeOp::BinOpModuloU8:
            case ByteCodeOp::BinOpModuloU16:
            case ByteCodeOp::BinOpModuloU32:
            case ByteCodeOp::BinOpModuloU64:
                emitBinOp(pp, ip, CPUOp::MOD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpDivS8:
            case ByteCodeOp::BinOpDivS16:
            case ByteCodeOp::BinOpDivS32:
            case ByteCodeOp::BinOpDivS64:
                emitBinOp(pp, ip, CPUOp::IDIV);
                break;
            case ByteCodeOp::BinOpDivU8:
            case ByteCodeOp::BinOpDivU16:
            case ByteCodeOp::BinOpDivU32:
            case ByteCodeOp::BinOpDivU64:
                emitBinOp(pp, ip, CPUOp::DIV);
                break;
            case ByteCodeOp::BinOpDivF32:
            case ByteCodeOp::BinOpDivF64:
                emitBinOp(pp, ip, CPUOp::FDIV);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpPlusS8:
            case ByteCodeOp::BinOpPlusS16:
            case ByteCodeOp::BinOpPlusS32:
            case ByteCodeOp::BinOpPlusS64:
            case ByteCodeOp::BinOpPlusU8:
            case ByteCodeOp::BinOpPlusU16:
            case ByteCodeOp::BinOpPlusU32:
            case ByteCodeOp::BinOpPlusU64:
                emitBinOpOverflow(pp, ip, CPUOp::ADD, SafetyMsg::Plus, SCBE_CPU::getCPUType(ip->op));
                break;
            case ByteCodeOp::BinOpPlusF32:
            case ByteCodeOp::BinOpPlusF64:
                emitBinOp(pp, ip, CPUOp::FADD);
                break;

            case ByteCodeOp::BinOpPlusS8_Safe:
            case ByteCodeOp::BinOpPlusU8_Safe:
            case ByteCodeOp::BinOpPlusS16_Safe:
            case ByteCodeOp::BinOpPlusU16_Safe:
            case ByteCodeOp::BinOpPlusS32_Safe:
            case ByteCodeOp::BinOpPlusU32_Safe:
            case ByteCodeOp::BinOpPlusS64_Safe:
            case ByteCodeOp::BinOpPlusU64_Safe:
                emitBinOp(pp, ip, CPUOp::ADD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpMinusS8:
            case ByteCodeOp::BinOpMinusS16:
            case ByteCodeOp::BinOpMinusS32:
            case ByteCodeOp::BinOpMinusS64:
            case ByteCodeOp::BinOpMinusU8:
            case ByteCodeOp::BinOpMinusU16:
            case ByteCodeOp::BinOpMinusU32:
            case ByteCodeOp::BinOpMinusU64:
                emitBinOpOverflow(pp, ip, CPUOp::SUB, SafetyMsg::Minus, SCBE_CPU::getCPUType(ip->op));
                break;
            case ByteCodeOp::BinOpMinusF32:
            case ByteCodeOp::BinOpMinusF64:
                emitBinOp(pp, ip, CPUOp::FSUB);
                break;

            case ByteCodeOp::BinOpMinusS8_Safe:
            case ByteCodeOp::BinOpMinusU8_Safe:
            case ByteCodeOp::BinOpMinusS16_Safe:
            case ByteCodeOp::BinOpMinusU16_Safe:
            case ByteCodeOp::BinOpMinusS32_Safe:
            case ByteCodeOp::BinOpMinusU32_Safe:
            case ByteCodeOp::BinOpMinusS64_Safe:
            case ByteCodeOp::BinOpMinusU64_Safe:
                emitBinOp(pp, ip, CPUOp::SUB);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskAnd8:
            case ByteCodeOp::BinOpBitmaskAnd16:
            case ByteCodeOp::BinOpBitmaskAnd32:
            case ByteCodeOp::BinOpBitmaskAnd64:
                emitBinOp(pp, ip, CPUOp::AND);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskOr8:
            case ByteCodeOp::BinOpBitmaskOr16:
            case ByteCodeOp::BinOpBitmaskOr32:
            case ByteCodeOp::BinOpBitmaskOr64:
                emitBinOp(pp, ip, CPUOp::OR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpShiftLeftEqS8:
            case ByteCodeOp::AffectOpShiftLeftEqU8:
            case ByteCodeOp::AffectOpShiftLeftEqS16:
            case ByteCodeOp::AffectOpShiftLeftEqU16:
            case ByteCodeOp::AffectOpShiftLeftEqS32:
            case ByteCodeOp::AffectOpShiftLeftEqU32:
            case ByteCodeOp::AffectOpShiftLeftEqS64:
            case ByteCodeOp::AffectOpShiftLeftEqU64:
                emitShiftEqLogical(pp, ip, CPUOp::SHL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpShiftRightEqS8:
            case ByteCodeOp::AffectOpShiftRightEqS16:
            case ByteCodeOp::AffectOpShiftRightEqS32:
            case ByteCodeOp::AffectOpShiftRightEqS64:
                emitShiftRightEqArithmetic(pp, ip);
                break;
            case ByteCodeOp::AffectOpShiftRightEqU8:
            case ByteCodeOp::AffectOpShiftRightEqU16:
            case ByteCodeOp::AffectOpShiftRightEqU32:
            case ByteCodeOp::AffectOpShiftRightEqU64:
                emitShiftEqLogical(pp, ip, CPUOp::SHR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpXorEqU8:
            case ByteCodeOp::AffectOpXorEqU16:
            case ByteCodeOp::AffectOpXorEqU32:
            case ByteCodeOp::AffectOpXorEqU64:
                emitBinOpEq(pp, ip, 0, CPUOp::XOR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpOrEqU8:
            case ByteCodeOp::AffectOpOrEqU16:
            case ByteCodeOp::AffectOpOrEqU32:
            case ByteCodeOp::AffectOpOrEqU64:
                emitBinOpEq(pp, ip, 0, CPUOp::OR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpAndEqU8:
            case ByteCodeOp::AffectOpAndEqU16:
            case ByteCodeOp::AffectOpAndEqU32:
            case ByteCodeOp::AffectOpAndEqU64:
                emitBinOpEq(pp, ip, 0, CPUOp::AND);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMulEqS8:
            case ByteCodeOp::AffectOpMulEqS16:
            case ByteCodeOp::AffectOpMulEqS32:
            case ByteCodeOp::AffectOpMulEqS64:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::IMUL, SafetyMsg::MulEq, SCBE_CPU::getCPUType(ip->op));
                break;
            case ByteCodeOp::AffectOpMulEqU8:
            case ByteCodeOp::AffectOpMulEqU16:
            case ByteCodeOp::AffectOpMulEqU32:
            case ByteCodeOp::AffectOpMulEqU64:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::MUL, SafetyMsg::MulEq, SCBE_CPU::getCPUType(ip->op));
                break;
            case ByteCodeOp::AffectOpMulEqF32:
            case ByteCodeOp::AffectOpMulEqF64:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::FMUL);
                break;

            case ByteCodeOp::AffectOpMulEqS8_Safe:
            case ByteCodeOp::AffectOpMulEqS16_Safe:
            case ByteCodeOp::AffectOpMulEqS32_Safe:
            case ByteCodeOp::AffectOpMulEqS64_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::IMUL);
                break;
            case ByteCodeOp::AffectOpMulEqU8_Safe:
            case ByteCodeOp::AffectOpMulEqU16_Safe:
            case ByteCodeOp::AffectOpMulEqU32_Safe:
            case ByteCodeOp::AffectOpMulEqU64_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::MUL);
                break;

            case ByteCodeOp::AffectOpMulEqS8_SSafe:
            case ByteCodeOp::AffectOpMulEqS16_SSafe:
            case ByteCodeOp::AffectOpMulEqS32_SSafe:
            case ByteCodeOp::AffectOpMulEqS64_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::IMUL);
                break;
            case ByteCodeOp::AffectOpMulEqU8_SSafe:
            case ByteCodeOp::AffectOpMulEqU16_SSafe:
            case ByteCodeOp::AffectOpMulEqU32_SSafe:
            case ByteCodeOp::AffectOpMulEqU64_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::MUL);
                break;
            case ByteCodeOp::AffectOpMulEqF32_S:
            case ByteCodeOp::AffectOpMulEqF64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::FMUL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpDivEqS8:
            case ByteCodeOp::AffectOpDivEqS16:
            case ByteCodeOp::AffectOpDivEqS32:
            case ByteCodeOp::AffectOpDivEqS64:
                emitBinOpEq(pp, ip, 0, CPUOp::IDIV);
                break;
            case ByteCodeOp::AffectOpDivEqU8:
            case ByteCodeOp::AffectOpDivEqU16:
            case ByteCodeOp::AffectOpDivEqU32:
            case ByteCodeOp::AffectOpDivEqU64:
                emitBinOpEq(pp, ip, 0, CPUOp::DIV);
                break;
            case ByteCodeOp::AffectOpDivEqF32:
            case ByteCodeOp::AffectOpDivEqF64:
                emitBinOpEq(pp, ip, 0, CPUOp::FDIV);
                break;

            case ByteCodeOp::AffectOpDivEqS8_S:
            case ByteCodeOp::AffectOpDivEqS16_S:
            case ByteCodeOp::AffectOpDivEqS32_S:
            case ByteCodeOp::AffectOpDivEqS64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::IDIV);
                break;
            case ByteCodeOp::AffectOpDivEqU8_S:
            case ByteCodeOp::AffectOpDivEqU16_S:
            case ByteCodeOp::AffectOpDivEqU32_S:
            case ByteCodeOp::AffectOpDivEqU64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::DIV);
                break;
            case ByteCodeOp::AffectOpDivEqF32_S:
            case ByteCodeOp::AffectOpDivEqF64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::FDIV);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpModuloEqS8:
            case ByteCodeOp::AffectOpModuloEqS16:
            case ByteCodeOp::AffectOpModuloEqS32:
            case ByteCodeOp::AffectOpModuloEqS64:
                emitBinOpEq(pp, ip, 0, CPUOp::IMOD);
                break;
            case ByteCodeOp::AffectOpModuloEqU8:
            case ByteCodeOp::AffectOpModuloEqU16:
            case ByteCodeOp::AffectOpModuloEqU32:
            case ByteCodeOp::AffectOpModuloEqU64:
                emitBinOpEq(pp, ip, 0, CPUOp::MOD);
                break;

            case ByteCodeOp::AffectOpModuloEqS8_S:
            case ByteCodeOp::AffectOpModuloEqS16_S:
            case ByteCodeOp::AffectOpModuloEqS32_S:
            case ByteCodeOp::AffectOpModuloEqS64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::IMOD);
                break;
            case ByteCodeOp::AffectOpModuloEqU8_S:
            case ByteCodeOp::AffectOpModuloEqU16_S:
            case ByteCodeOp::AffectOpModuloEqU32_S:
            case ByteCodeOp::AffectOpModuloEqU64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::MOD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpPlusEqS8:
            case ByteCodeOp::AffectOpPlusEqS16:
            case ByteCodeOp::AffectOpPlusEqS32:
            case ByteCodeOp::AffectOpPlusEqS64:
            case ByteCodeOp::AffectOpPlusEqU8:
            case ByteCodeOp::AffectOpPlusEqU16:
            case ByteCodeOp::AffectOpPlusEqU32:
            case ByteCodeOp::AffectOpPlusEqU64:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::ADD, SafetyMsg::PlusEq, SCBE_CPU::getCPUType(ip->op));
                break;
            case ByteCodeOp::AffectOpPlusEqF32:
            case ByteCodeOp::AffectOpPlusEqF64:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::FADD);
                break;

            case ByteCodeOp::AffectOpPlusEqS8_Safe:
            case ByteCodeOp::AffectOpPlusEqS16_Safe:
            case ByteCodeOp::AffectOpPlusEqS32_Safe:
            case ByteCodeOp::AffectOpPlusEqS64_Safe:
            case ByteCodeOp::AffectOpPlusEqU8_Safe:
            case ByteCodeOp::AffectOpPlusEqU16_Safe:
            case ByteCodeOp::AffectOpPlusEqU32_Safe:
            case ByteCodeOp::AffectOpPlusEqU64_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::ADD);
                break;

            case ByteCodeOp::AffectOpPlusEqS8_SSafe:
            case ByteCodeOp::AffectOpPlusEqS16_SSafe:
            case ByteCodeOp::AffectOpPlusEqS32_SSafe:
            case ByteCodeOp::AffectOpPlusEqS64_SSafe:
            case ByteCodeOp::AffectOpPlusEqU8_SSafe:
            case ByteCodeOp::AffectOpPlusEqU16_SSafe:
            case ByteCodeOp::AffectOpPlusEqU32_SSafe:
            case ByteCodeOp::AffectOpPlusEqU64_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::ADD);
                break;
            case ByteCodeOp::AffectOpPlusEqF32_S:
            case ByteCodeOp::AffectOpPlusEqF64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::FADD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMinusEqS8:
            case ByteCodeOp::AffectOpMinusEqS16:
            case ByteCodeOp::AffectOpMinusEqS32:
            case ByteCodeOp::AffectOpMinusEqS64:
            case ByteCodeOp::AffectOpMinusEqU8:
            case ByteCodeOp::AffectOpMinusEqU16:
            case ByteCodeOp::AffectOpMinusEqU32:
            case ByteCodeOp::AffectOpMinusEqU64:
                emitBinOpEqOverflow(pp, ip, 0, CPUOp::SUB, SafetyMsg::MinusEq, SCBE_CPU::getCPUType(ip->op));
                break;
            case ByteCodeOp::AffectOpMinusEqF32:
            case ByteCodeOp::AffectOpMinusEqF64:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::FSUB);
                break;

            case ByteCodeOp::AffectOpMinusEqS8_Safe:
            case ByteCodeOp::AffectOpMinusEqS16_Safe:
            case ByteCodeOp::AffectOpMinusEqS32_Safe:
            case ByteCodeOp::AffectOpMinusEqS64_Safe:
            case ByteCodeOp::AffectOpMinusEqU8_Safe:
            case ByteCodeOp::AffectOpMinusEqU16_Safe:
            case ByteCodeOp::AffectOpMinusEqU32_Safe:
            case ByteCodeOp::AffectOpMinusEqU64_Safe:
                emitBinOpEq(pp, ip, ip->c.u32, CPUOp::SUB);
                break;

            case ByteCodeOp::AffectOpMinusEqS8_SSafe:
            case ByteCodeOp::AffectOpMinusEqS16_SSafe:
            case ByteCodeOp::AffectOpMinusEqS32_SSafe:
            case ByteCodeOp::AffectOpMinusEqS64_SSafe:
            case ByteCodeOp::AffectOpMinusEqU8_SSafe:
            case ByteCodeOp::AffectOpMinusEqU16_SSafe:
            case ByteCodeOp::AffectOpMinusEqU32_SSafe:
            case ByteCodeOp::AffectOpMinusEqU64_SSafe:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::SUB);
                break;
            case ByteCodeOp::AffectOpMinusEqF32_S:
            case ByteCodeOp::AffectOpMinusEqF64_S:
                emitBinOpEqS(pp, ip, offsetStack, CPUOp::FSUB);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ZeroToTrue:
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::E);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::LowerZeroToTrue:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                pp.emitOp(CPUReg::RAX, 31, CPUOp::SHR, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::LowerEqZeroToTrue:
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::LE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::GreaterZeroToTrue:
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, CPUBits::B32);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::G);
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
            case ByteCodeOp::CompareOpGreaterS16:
            case ByteCodeOp::CompareOpGreaterS32:
            case ByteCodeOp::CompareOpGreaterS64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::G);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterU8:
            case ByteCodeOp::CompareOpGreaterU16:
            case ByteCodeOp::CompareOpGreaterU32:
            case ByteCodeOp::CompareOpGreaterU64:
            case ByteCodeOp::CompareOpGreaterF32:
            case ByteCodeOp::CompareOpGreaterF64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::A);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::CompareOpGreaterEqS8:
            case ByteCodeOp::CompareOpGreaterEqS16:
            case ByteCodeOp::CompareOpGreaterEqS32:
            case ByteCodeOp::CompareOpGreaterEqS64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::GE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::CompareOpGreaterEqU8:
            case ByteCodeOp::CompareOpGreaterEqU16:
            case ByteCodeOp::CompareOpGreaterEqU32:
            case ByteCodeOp::CompareOpGreaterEqU64:
            case ByteCodeOp::CompareOpGreaterEqF32:
            case ByteCodeOp::CompareOpGreaterEqF64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::AE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpLowerS8:
            case ByteCodeOp::CompareOpLowerS16:
            case ByteCodeOp::CompareOpLowerS32:
            case ByteCodeOp::CompareOpLowerS64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::L);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::CompareOpLowerU8:
            case ByteCodeOp::CompareOpLowerU16:
            case ByteCodeOp::CompareOpLowerU32:
            case ByteCodeOp::CompareOpLowerU64:
            case ByteCodeOp::CompareOpLowerF32:
            case ByteCodeOp::CompareOpLowerF64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::B);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::CompareOpLowerEqS8:
            case ByteCodeOp::CompareOpLowerEqS16:
            case ByteCodeOp::CompareOpLowerEqS32:
            case ByteCodeOp::CompareOpLowerEqS64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::LE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

            case ByteCodeOp::CompareOpLowerEqU8:
            case ByteCodeOp::CompareOpLowerEqU16:
            case ByteCodeOp::CompareOpLowerEqU32:
            case ByteCodeOp::CompareOpLowerEqU64:
            case ByteCodeOp::CompareOpLowerEqF32:
            case ByteCodeOp::CompareOpLowerEqF64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::BE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOp3Way8:
            case ByteCodeOp::CompareOp3Way16:
            case ByteCodeOp::CompareOp3Way32:
            case ByteCodeOp::CompareOp3Way64:
                pp.emitClear(CPUReg::R8, CPUBits::B32);
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::R8, CPUCondFlag::G);
                pp.emitLoad(CPUReg::RAX, 0xFFFFFFFF, CPUBits::B32);
                pp.emitCMov(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVGE, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                break;

            case ByteCodeOp::CompareOp3WayF32:
            case ByteCodeOp::CompareOp3WayF64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitClear(CPUReg::R8, CPUBits::B32);
                emitIMMA(pp, ip, CPUReg::XMM0, numBits);
                emitIMMB(pp, ip, CPUReg::XMM1, numBits);
                pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::UCOMIF, numBits);
                pp.emitSet(CPUReg::R8, CPUCondFlag::A);
                pp.emitOp(CPUReg::XMM1, CPUReg::XMM0, CPUOp::UCOMIF, numBits);
                pp.emitLoad(CPUReg::RAX, 0xFFFFFFFF, CPUBits::B32);
                pp.emitCMov(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVBE, CPUBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B32);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpEqual8:
            case ByteCodeOp::CompareOpEqual16:
            case ByteCodeOp::CompareOpEqual32:
            case ByteCodeOp::CompareOpEqual64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::E);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpEqualF32:
            case ByteCodeOp::CompareOpEqualF64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::EP);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpNotEqual8:
            case ByteCodeOp::CompareOpNotEqual16:
            case ByteCodeOp::CompareOpNotEqual32:
            case ByteCodeOp::CompareOpNotEqual64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::NE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B8);
                break;
            case ByteCodeOp::CompareOpNotEqualF32:
            case ByteCodeOp::CompareOpNotEqualF64:
                emitCompareOp(pp, ip);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::NEP);
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
            case ByteCodeOp::InvertU16:
            case ByteCodeOp::InvertU32:
            case ByteCodeOp::InvertU64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                if (ip->a.u32 == ip->b.u32)
                    pp.emitNot(CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), numBits);
                    pp.emitNot(CPUReg::RAX, numBits);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, numBits);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpDyn8:
            case ByteCodeOp::JumpDyn16:
            case ByteCodeOp::JumpDyn32:
            case ByteCodeOp::JumpDyn64:
            {
                numBits            = SCBE_CPU::getCPUBits(ip->op);
                auto tableCompiler = reinterpret_cast<int32_t*>(buildParameters.module->compilerSegment.address(ip->d.u32));
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64, numBits, true);

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
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::ADD, CPUBits::B64);
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
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, CPUBits::B8);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfFalse:
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, CPUBits::B8);
                pp.emitJump(JZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfRTTrue:
                pp.emitCmp(CPUReg::RDI, offsetRT + REG_OFFSET(0), 0, CPUBits::B8);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfRTFalse:
                pp.emitCmp(CPUReg::RDI, offsetRT + REG_OFFSET(0), 0, CPUBits::B8);
                pp.emitJump(JZ, i, ip->b.s32);
                break;

            case ByteCodeOp::JumpIfNotZero8:
            case ByteCodeOp::JumpIfNotZero16:
            case ByteCodeOp::JumpIfNotZero32:
            case ByteCodeOp::JumpIfNotZero64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, numBits);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;

            case ByteCodeOp::JumpIfZero8:
            case ByteCodeOp::JumpIfZero16:
            case ByteCodeOp::JumpIfZero32:
            case ByteCodeOp::JumpIfZero64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, numBits);
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
            case ByteCodeOp::JumpIfLowerS16:
            case ByteCodeOp::JumpIfLowerS32:
            case ByteCodeOp::JumpIfLowerS64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitJumpCmp(pp, ip, i, JL, numBits);
                break;
            case ByteCodeOp::JumpIfLowerU8:
            case ByteCodeOp::JumpIfLowerU16:
            case ByteCodeOp::JumpIfLowerU32:
            case ByteCodeOp::JumpIfLowerU64:
            case ByteCodeOp::JumpIfLowerF32:
            case ByteCodeOp::JumpIfLowerF64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitJumpCmp(pp, ip, i, JB, numBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfLowerEqS8:
            case ByteCodeOp::JumpIfLowerEqS16:
            case ByteCodeOp::JumpIfLowerEqS32:
            case ByteCodeOp::JumpIfLowerEqS64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitJumpCmp(pp, ip, i, JLE, numBits);
                break;

            case ByteCodeOp::JumpIfLowerEqU8:
            case ByteCodeOp::JumpIfLowerEqU16:
            case ByteCodeOp::JumpIfLowerEqU32:
            case ByteCodeOp::JumpIfLowerEqU64:
            case ByteCodeOp::JumpIfLowerEqF32:
            case ByteCodeOp::JumpIfLowerEqF64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitJumpCmp(pp, ip, i, JBE, numBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfGreaterS8:
            case ByteCodeOp::JumpIfGreaterS16:
            case ByteCodeOp::JumpIfGreaterS32:
            case ByteCodeOp::JumpIfGreaterS64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitJumpCmp(pp, ip, i, JG, numBits);
                break;

            case ByteCodeOp::JumpIfGreaterU8:
            case ByteCodeOp::JumpIfGreaterU16:
            case ByteCodeOp::JumpIfGreaterU32:
            case ByteCodeOp::JumpIfGreaterU64:
            case ByteCodeOp::JumpIfGreaterF32:
            case ByteCodeOp::JumpIfGreaterF64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitJumpCmp(pp, ip, i, JA, numBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfGreaterEqS8:
            case ByteCodeOp::JumpIfGreaterEqS16:
            case ByteCodeOp::JumpIfGreaterEqS32:
            case ByteCodeOp::JumpIfGreaterEqS64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitJumpCmp(pp, ip, i, JGE, numBits);
                break;

            case ByteCodeOp::JumpIfGreaterEqU8:
            case ByteCodeOp::JumpIfGreaterEqU16:
            case ByteCodeOp::JumpIfGreaterEqU32:
            case ByteCodeOp::JumpIfGreaterEqU64:
            case ByteCodeOp::JumpIfGreaterEqF32:
            case ByteCodeOp::JumpIfGreaterEqF64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitJumpCmp(pp, ip, i, JAE, numBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::DecrementRA32:
            case ByteCodeOp::DecrementRA64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitDec(CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
                break;

            case ByteCodeOp::IncrementRA64:
                pp.emitInc(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::DeRef8:
            case ByteCodeOp::DeRef16:
            case ByteCodeOp::DeRef32:
            case ByteCodeOp::DeRef64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->c.u64, CPUBits::B64, numBits, false);
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
            case ByteCodeOp::GetFromBssSeg16:
            case ByteCodeOp::GetFromBssSeg32:
            case ByteCodeOp::GetFromBssSeg64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symBSIndex, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->b.u32, CPUBits::B64, numBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

            case ByteCodeOp::GetFromMutableSeg8:
            case ByteCodeOp::GetFromMutableSeg16:
            case ByteCodeOp::GetFromMutableSeg32:
            case ByteCodeOp::GetFromMutableSeg64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symMSIndex, 0);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->b.u32, CPUBits::B64, numBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8:
            case ByteCodeOp::GetFromStack16:
            case ByteCodeOp::GetFromStack32:
            case ByteCodeOp::GetFromStack64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64, numBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetIncFromStack64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                pp.emitOp(CPUReg::RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8x2:
            case ByteCodeOp::GetFromStack16x2:
            case ByteCodeOp::GetFromStack32x2:
            case ByteCodeOp::GetFromStack64x2:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64, numBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->d.u32, CPUBits::B64, numBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetIncFromStack64DeRef8:
                if (ip->c.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, static_cast<uint32_t>(ip->c.u64), CPUBits::B64, CPUBits::B8, false);
                }
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitOp(CPUReg::RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B64, CPUBits::B8, false);
                }
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;
            case ByteCodeOp::GetIncFromStack64DeRef16:
                if (ip->c.u64 <= 0x7FFFFFFF)
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, static_cast<uint32_t>(ip->c.u64), CPUBits::B64, CPUBits::B16, false);
                }
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, CPUBits::B64);
                    pp.emitOp(CPUReg::RAX, ip->c.u64, CPUOp::ADD, CPUBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B64, CPUBits::B16, false);
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
            case ByteCodeOp::CopyStack16:
            case ByteCodeOp::CopyStack32:
            case ByteCodeOp::CopyStack64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->b.u32, numBits);
                pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, CPUReg::RAX, numBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearMaskU32:
                if (ip->b.u32 == 0xFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32, CPUBits::B8, false);
                else if (ip->b.u32 == 0xFFFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32, CPUBits::B16, false);
                else
                    SWAG_ASSERT(false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B32);
                break;

            case ByteCodeOp::ClearMaskU64:
                if (ip->b.u32 == 0xFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64, CPUBits::B8, false);
                else if (ip->b.u32 == 0xFFFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64, CPUBits::B16, false);
                else if (ip->b.u32 == 0xFFFFFFFF)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
                else
                    SWAG_ASSERT(false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetZeroAtPointer8:
            case ByteCodeOp::SetZeroAtPointer16:
            case ByteCodeOp::SetZeroAtPointer32:
            case ByteCodeOp::SetZeroAtPointer64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitStore(CPUReg::RAX, ip->b.u32, 0, numBits);
                break;

            case ByteCodeOp::SetZeroAtPointerX:
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                if (ip->b.u32 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitClear(CPUReg::RAX, ip->c.u32, ip->b.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CPUPushParamType::RegAdd, .reg = ip->a.u32, .val = ip->c.u64});
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = 0});
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = ip->b.u64});
                    emitInternalCallExt(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            case ByteCodeOp::SetZeroAtPointerXRB:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->a.u32});
                pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = 0});
                pp.pushParams.push_back({.type = CPUPushParamType::RegMul, .reg = ip->b.u32, .val = ip->c.u64});
                emitInternalCallExt(pp, g_LangSpec->name_memset, pp.pushParams);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearRR8:
            case ByteCodeOp::ClearRR16:
            case ByteCodeOp::ClearRR32:
            case ByteCodeOp::ClearRR64:
            {
                numBits              = SCBE_CPU::getCPUBits(ip->op);
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);
                pp.emitStore(CPUReg::RAX, ip->c.u32, 0, numBits);
                break;
            }

            case ByteCodeOp::ClearRRX:
            {
                uint32_t stackOffset = SCBE_CPU::getParamStackOffset(cpuFct, typeFunc->numParamsRegisters());
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, stackOffset, CPUBits::B64);

                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                if (ip->b.u32 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitClear(CPUReg::RAX, ip->c.u32, ip->b.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CPUPushParamType::RAX});
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = 0});
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = ip->b.u64});
                    emitInternalCallExt(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetZeroStack8:
            case ByteCodeOp::SetZeroStack16:
            case ByteCodeOp::SetZeroStack32:
            case ByteCodeOp::SetZeroStack64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitStore(CPUReg::RDI, offsetStack + ip->a.u32, 0, numBits);
                break;
            case ByteCodeOp::SetZeroStackX:
                if (ip->b.u32 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                    pp.emitClear(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u32);
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CPUPushParamType::Addr, .reg = offsetStack + ip->a.u32});
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = 0});
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = ip->b.u32});
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
                pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, CPUBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetImmediate32:
            case ByteCodeOp::SetImmediate64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u64, numBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::MemCpy8:
            case ByteCodeOp::MemCpy16:
            case ByteCodeOp::MemCpy32:
            case ByteCodeOp::MemCpy64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RDX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                pp.emitCopy(CPUReg::RCX, CPUReg::RDX, SCBE_CPU::getBitsCount(numBits) / 8, 0);
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMemCpy:
                if (ip->hasFlag(BCI_IMM_C) &&
                    ip->c.u64 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitLoad(CPUReg::RDX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
                    pp.emitCopy(CPUReg::RCX, CPUReg::RDX, ip->c.u32, 0);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->a.u32});
                    pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->b.u32});
                    pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_C) ? CPUPushParamType::Imm : CPUPushParamType::Reg, .reg = ip->c.u64});
                    emitInternalCallExt(pp, g_LangSpec->name_memcpy, pp.pushParams);
                }
                break;
            case ByteCodeOp::IntrinsicMemSet:
                if (ip->hasFlag(BCI_IMM_B) &&
                    ip->hasFlag(BCI_IMM_C) &&
                    ip->b.u8 == 0 &&
                    ip->c.u64 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                    pp.emitClear(CPUReg::RCX, 0, ip->c.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->a.u32});
                    pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_B) ? CPUPushParamType::Imm : CPUPushParamType::Reg, .reg = ip->b.u8});
                    pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_C) ? CPUPushParamType::Imm : CPUPushParamType::Reg, .reg = ip->c.u64});
                    emitInternalCallExt(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            case ByteCodeOp::IntrinsicMemMove:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->a.u32});
                pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->b.u32});
                pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_C) ? CPUPushParamType::Imm : CPUPushParamType::Reg, .reg = ip->c.u64});
                emitInternalCallExt(pp, g_LangSpec->name_memmove, pp.pushParams);
                break;
            case ByteCodeOp::IntrinsicMemCmp:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->b.u32});
                pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->c.u32});
                pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_D) ? CPUPushParamType::Imm : CPUPushParamType::Reg, .reg = ip->d.u64});
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
                pp.pushParams.push_back({.type = CPUPushParamType::RelocV, .reg = pp.symTls_threadLocalId});
                pp.pushParams.push_back({.type = CPUPushParamType::Imm64, .reg = module->tlsSegment.totalCount});
                pp.pushParams.push_back({.type = CPUPushParamType::RelocAddr, .reg = pp.symTLSIndex});
                emitInternalCallExt(pp, g_LangSpec->name_priv_tlsGetPtr, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicGetContext:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::RelocV, .reg = pp.symPI_contextTlsId});
                emitInternalCallExt(pp, g_LangSpec->name_priv_tlsGetValue, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicSetContext:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::RelocV, .reg = pp.symPI_contextTlsId});
                pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->a.u32});
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
                    pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::ADD, CPUBits::B64);
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
            case ByteCodeOp::GetParam16:
            case ByteCodeOp::GetParam32:
            case ByteCodeOp::GetParam64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, numBits);
                break;
            case ByteCodeOp::GetParam64x2:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, CPUBits::B64);
                emitGetParam(pp, cpuFct, ip->c.u32, ip->d.mergeU64U32.high, CPUBits::B64);
                break;
            case ByteCodeOp::GetIncParam64:
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, CPUBits::B64, ip->d.u64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetParam64DeRef8:
            case ByteCodeOp::GetParam64DeRef16:
            case ByteCodeOp::GetParam64DeRef32:
            case ByteCodeOp::GetParam64DeRef64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, CPUBits::B64, 0, numBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetIncParam64DeRef8:
            case ByteCodeOp::GetIncParam64DeRef16:
            case ByteCodeOp::GetIncParam64DeRef32:
            case ByteCodeOp::GetIncParam64DeRef64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitGetParam(pp, cpuFct, ip->a.u32, ip->b.mergeU64U32.high, CPUBits::B64, ip->d.u64, numBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeLambda:
            {
                auto funcNode = castAst<AstFuncDecl>(reinterpret_cast<AstNode*>(ip->b.pointer), AstNodeKind::FuncDecl);
                SWAG_ASSERT(!ip->c.pointer || (funcNode && funcNode->hasExtByteCode() && funcNode->extByteCode()->bc == reinterpret_cast<ByteCode*>(ip->c.pointer)));
                Utf8 callName = funcNode->getCallName();
                pp.emitStore0Load64(CPUReg::RAX);

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
                pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::AND, CPUBits::B64);
                
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
            case ByteCodeOp::IntrinsicMulAddF64:
            {
                numBits = SCBE_CPU::getCPUBits(ip->op);
                emitIMMB(pp, ip, CPUReg::XMM0, numBits);
                emitIMMC(pp, ip, CPUReg::XMM1, numBits);
                emitIMMD(pp, ip, CPUReg::XMM2, numBits);
                pp.emitMulAdd(CPUReg::XMM0, CPUReg::XMM1, CPUReg::XMM2, numBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, numBits);
                break;
            }

            case ByteCodeOp::IntrinsicAtomicAddS8:
            case ByteCodeOp::IntrinsicAtomicAddS16:
            case ByteCodeOp::IntrinsicAtomicAddS32:
            case ByteCodeOp::IntrinsicAtomicAddS64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, numBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
                emitBinOpEqLock(pp, ip, CPUOp::ADD);
                break;

            case ByteCodeOp::IntrinsicAtomicAndS8:
            case ByteCodeOp::IntrinsicAtomicAndS16:
            case ByteCodeOp::IntrinsicAtomicAndS32:
            case ByteCodeOp::IntrinsicAtomicAndS64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, numBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
                emitBinOpEqLock(pp, ip, CPUOp::AND);
                break;

            case ByteCodeOp::IntrinsicAtomicOrS8:
            case ByteCodeOp::IntrinsicAtomicOrS16:
            case ByteCodeOp::IntrinsicAtomicOrS32:
            case ByteCodeOp::IntrinsicAtomicOrS64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, numBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
                emitBinOpEqLock(pp, ip, CPUOp::OR);
                break;

            case ByteCodeOp::IntrinsicAtomicXorS8:
            case ByteCodeOp::IntrinsicAtomicXorS16:
            case ByteCodeOp::IntrinsicAtomicXorS32:
            case ByteCodeOp::IntrinsicAtomicXorS64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, numBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
                emitBinOpEqLock(pp, ip, CPUOp::XOR);
                break;

            case ByteCodeOp::IntrinsicAtomicXchgS8:
            case ByteCodeOp::IntrinsicAtomicXchgS16:
            case ByteCodeOp::IntrinsicAtomicXchgS32:
            case ByteCodeOp::IntrinsicAtomicXchgS64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, numBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
                emitBinOpEqLock(pp, ip, CPUOp::XCHG);
                break;

            case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
                numBits = SCBE_CPU::getCPUBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                emitIMMB(pp, ip, CPUReg::RAX, numBits);
                emitIMMC(pp, ip, CPUReg::RDX, numBits);
                pp.emitCmpXChg(CPUReg::RCX, CPUReg::RDX, numBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUReg::RAX, numBits);
                break;

            case ByteCodeOp::IntrinsicS8x1:
            {
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8);
                        pp.emitCopy(CPUReg::RCX, CPUReg::RAX, CPUBits::B8);
                        pp.emitOp(CPUReg::RCX, 7, CPUOp::SAR, CPUBits::B8);
                        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::XOR, CPUBits::B8);
                        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::SUB, CPUBits::B8);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8, CPUBits::B32, false);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::POPCNT, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8, CPUBits::B32, false);
                        pp.emitOp(CPUReg::RAX, CPUReg::RAX, CPUOp::BSF, CPUBits::B32);
                        pp.emitLoad(CPUReg::RCX, 8, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8, CPUBits::B32, false);
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
                        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::XOR, CPUBits::B16);
                        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::SUB, CPUBits::B16);
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
                        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::XOR, CPUBits::B32);
                        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::SUB, CPUBits::B32);
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
                        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::XOR, CPUBits::B64);
                        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::SUB, CPUBits::B64);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8, CPUBits::B32, true);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B8, CPUBits::B32, true);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8, CPUBits::B32, true);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B8, CPUBits::B32, true);
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
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8, CPUBits::B32, false);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B8, CPUBits::B32, false);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8, CPUBits::B32, false);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B8, CPUBits::B32, false);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, CPUBits::B32);
                        pp.emitCMov(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicRol:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8, CPUBits::B32, true);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B8, CPUBits::B32, true);
                        pp.emitRotate(CPUReg::RAX, CPUReg::RCX, CPUOp::ROL, CPUBits::B32);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, CPUBits::B8);
                        break;
                    case TokenId::IntrinsicRor:
                        emitIMMB(pp, ip, CPUReg::RAX, CPUBits::B8, CPUBits::B32, true);
                        emitIMMC(pp, ip, CPUReg::RCX, CPUBits::B8, CPUBits::B32, true);
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
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = ip->b.u32});
                else
                    pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->b.u32});
                if (ip->hasFlag(BCI_IMM_C))
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = ip->c.u32});
                else
                    pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->c.u32});

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
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = ip->b.u64});
                else
                    pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->b.u32});
                if (ip->hasFlag(BCI_IMM_C))
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = ip->c.u64});
                else
                    pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->c.u32});

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
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = ip->b.u32});
                else
                    pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->b.u32});

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
                    pp.pushParams.push_back({.type = CPUPushParamType::Imm, .reg = ip->b.u64});
                else
                    pp.pushParams.push_back({.type = CPUPushParamType::Reg, .reg = ip->b.u32});

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
                pp.emitCmp(CPUReg::RAX, offsetof(SwagContext, hasError), 0, CPUBits::B32);
                pp.emitJump(JNZ, i, ip->b.s32);
                break;
            case ByteCodeOp::JumpIfNoError:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
                pp.emitCmp(CPUReg::RAX, offsetof(SwagContext, hasError), 0, CPUBits::B32);
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
