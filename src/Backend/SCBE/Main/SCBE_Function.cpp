#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/SCBE/Encoder/SCBE_Micro.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Report/Diagnostic.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool SCBE::emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc)
{
    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && bc->node->hasAttribute(ATTRIBUTE_TEST_FUNC) && buildParameters.compileType != Test)
        return true;

    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    auto&       ppCPU           = encoder<SCBE_CPU>(ct, precompileIndex);
    const auto  typeFunc        = bc->getCallType();
    const auto  returnType      = typeFunc->concreteReturnType();
    const bool  debug           = buildParameters.buildCfg->backendDebugInfos;
    const auto& cc              = typeFunc->getCallConv();
    bool        ok              = true;

    ppCPU.init(buildParameters);
    bc->markLabels();

    // Get function name
    const auto&        funcName   = bc->getCallNameFromDecl();
    const AstFuncDecl* bcFuncNode = bc->node ? castAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl) : nullptr;

    // Export symbol
    if (bcFuncNode && bcFuncNode->hasAttribute(ATTRIBUTE_PUBLIC))
    {
        if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
            ppCPU.directives += form("/EXPORT:%s ", funcName.cstr());
    }

    // In order, starting at RSP, we have :
    //
    // sizeStackCallParams to store function call parameters
    // ...padding to 16...

    // all registers = RDI:             bc->maxReservedRegisterRC, contains all Swag bytecode registers
    // offsetRT:                        bc->maxCallResults, contains registers for result
    // offsetParamsAsRegisters:         To store all parameters passed as register in the current calling convention
    // offsetResult:                    sizeof(Register) to store return value
    // offsetStack:                     stackSize, function local stack
    // offsetFLT:                       sizeof(Register) to make some float computation
    // ...padding to 16... => total is sizeStack

    const uint32_t offsetRT                = bc->maxReservedRegisterRC * sizeof(Register);
    const uint32_t offsetParamsAsRegisters = offsetRT + bc->maxCallResults * sizeof(Register);
    const uint32_t offsetResult            = offsetParamsAsRegisters + cc.paramByRegisterCount * sizeof(Register);
    const uint32_t offsetByteCodeStack     = offsetResult + sizeof(Register);
    const uint32_t offsetFLT               = offsetByteCodeStack + bc->stackSize; // For float load (should be reserved only if we have floating point operations in that function)
    const uint32_t sizeStack               = offsetFLT + 8;

    // Register function
    ppCPU.cpuFct = ppCPU.addFunction(funcName, &cc, bc);
    if (debug)
        SCBE_Debug::setLocation(ppCPU.cpuFct, nullptr, 0);

    ppCPU.cpuFct->offsetFLTReg            = CPUReg::RDI;
    ppCPU.cpuFct->offsetFLT               = offsetFLT;
    ppCPU.cpuFct->offsetRT                = offsetRT;
    ppCPU.cpuFct->offsetByteCodeStack     = offsetByteCodeStack;
    ppCPU.cpuFct->offsetParamsAsRegisters = offsetParamsAsRegisters;

    SCBE_Micro pp;
    pp.init(buildParameters);
    pp.cpuFct               = ppCPU.cpuFct;
    pp.symCOIndex           = ppCPU.symCOIndex;
    pp.symBSIndex           = ppCPU.symBSIndex;
    pp.symMSIndex           = ppCPU.symMSIndex;
    pp.symCSIndex           = ppCPU.symCSIndex;
    pp.symTLSIndex          = ppCPU.symTLSIndex;
    pp.symXDIndex           = ppCPU.symXDIndex;
    pp.symTls_threadLocalId = ppCPU.symTls_threadLocalId;
    pp.symPI_contextTlsId   = ppCPU.symPI_contextTlsId;
    pp.symPI_byteCodeRun    = ppCPU.symPI_byteCodeRun;
    pp.symPI_processInfos   = ppCPU.symPI_processInfos;
    pp.symPI_makeCallback   = ppCPU.symPI_makeCallback;
    pp.symCst_U64F64        = ppCPU.symCst_U64F64;

    // RDI will be a pointer to the stack, and the list of registers is stored at the start of the stack
    pp.cpuFct->unwindRegs.push_back(CPUReg::RDI);
    pp.emitEnter(sizeStack);

    // Registers are stored after the sizeParamsStack area, which is used to store parameters for function calls
    pp.emitLoadAddress(CPUReg::RDI, CPUReg::RSP, pp.cpuFct->sizeStackCallParams);

    // Save register parameters
    uint32_t idxReg = 0;
    while (idxReg < std::min(cc.paramByRegisterCount, typeFunc->numParamsRegisters()))
    {
        const auto typeParam = typeFunc->registerIdxToType(idxReg);
        if (cc.useRegisterFloat && typeParam->isNativeFloat())
            pp.emitStoreParam(idxReg, cc.paramByRegisterFloat[idxReg], OpBits::F64, false);
        else
            pp.emitStoreParam(idxReg, cc.paramByRegisterInteger[idxReg], OpBits::B64, false);
        idxReg++;
    }

    // Save pointer to return value if this is a return by copy
    if (idxReg < cc.paramByRegisterCount && typeFunc->returnByAddress())
    {
        pp.emitStoreParam(idxReg, cc.paramByRegisterInteger[idxReg], OpBits::B64, false);
        idxReg++;
    }

    // Save C variadic on the caller stack, to be sure that everything is stored there, even if it's
    // passed by registers
    if (idxReg < cc.paramByRegisterCount && typeFunc->isFctCVariadic())
    {
        while (idxReg < cc.paramByRegisterCount)
        {
            pp.emitStoreParam(idxReg, cc.paramByRegisterInteger[idxReg], OpBits::B64, true);
            idxReg++;
        }
    }

    // Use R11 as base pointer for capture parameters
    // This is used to debug and have access to capture parameters, even if we "lose" rcx
    // which is the register that will have a pointer to the capture buffer (but rcx is volatile)
    if (typeFunc->isClosure() && debug)
        pp.emitLoad(CPUReg::R12, CPUReg::RCX, OpBits::B64);

    auto ip     = bc->out;
    auto opBits = OpBits::Zero;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->hasAstFlag(AST_NO_BACKEND))
            continue;
        if (ip->hasFlag(BCI_NO_BACKEND))
            continue;

        pp.ip      = ip;
        pp.ipIndex = static_cast<int32_t>(i);

        if (debug)
            pp.emitDebug(ip);
        if (ip->hasFlag(BCI_JUMP_DEST))
            pp.emitLabel(i);

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

        opBits = OpBits::Zero;
        switch (ip->op)
        {
            case ByteCodeOp::DebugNop:
                pp.emitNop();
                break;

                /////////////////////////////////////

            case ByteCodeOp::MulAddVC64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                pp.emitOpBinary(CPUReg::RCX, ip->c.u8, CPUOp::ADD, OpBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::MUL, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::Add32byVB32:
            case ByteCodeOp::Add64byVB64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitOpBinary(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u64, CPUOp::ADD, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearRA:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B64);
                break;
            case ByteCodeOp::ClearRAx2:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, OpBits::B64);
                break;
            case ByteCodeOp::ClearRAx3:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), 0, OpBits::B64);
                break;
            case ByteCodeOp::ClearRAx4:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), 0, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->d.u32), 0, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyRBtoRA8:
            case ByteCodeOp::CopyRBtoRA16:
            case ByteCodeOp::CopyRBtoRA32:
            case ByteCodeOp::CopyRBtoRA64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64, opBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::CopyRBtoRA64x2:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->d.u32), OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::CopyRBAddrToRA:
                pp.emitLoadAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32));
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CastBool8:
            case ByteCodeOp::CastBool16:
            case ByteCodeOp::CastBool32:
            case ByteCodeOp::CastBool64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, opBits);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::NE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B8);
                break;

            case ByteCodeOp::CastS8S16:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B16, OpBits::B8, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B16);
                break;

            case ByteCodeOp::CastS8S32:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32, OpBits::B8, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B32);
                break;
            case ByteCodeOp::CastS16S32:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32, OpBits::B16, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B32);
                break;

            case ByteCodeOp::CastS8S64:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64, OpBits::B8, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;
            case ByteCodeOp::CastS16S64:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64, OpBits::B16, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;
            case ByteCodeOp::CastS32S64:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64, OpBits::B32, true);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::CastF32S32:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::F32);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTF2I, OpBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B32);
                break;
            case ByteCodeOp::CastF64S64:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::F64);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTF2I, OpBits::F64, EMITF_B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::CastS8F32:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32, OpBits::B8, true);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F32);
                break;
            case ByteCodeOp::CastS16F32:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32, OpBits::B16, true);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F32);
                break;
            case ByteCodeOp::CastS32F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F32);
                break;
            case ByteCodeOp::CastS64F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F32, EMITF_B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F32);
                break;
            case ByteCodeOp::CastU8F32:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32, OpBits::B8, false);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F32);
                break;
            case ByteCodeOp::CastU16F32:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32, OpBits::B16, false);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F32);
                break;
            case ByteCodeOp::CastU32F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F32, EMITF_B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F32);
                break;
            case ByteCodeOp::CastU64F32:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F32, EMITF_B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F32);
                break;
            case ByteCodeOp::CastF64F32:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::F64);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTF2F, OpBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F32);
                break;

            case ByteCodeOp::CastS8F64:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32, OpBits::B8, true);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F64);
                break;
            case ByteCodeOp::CastS16F64:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32, OpBits::B16, true);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F64);
                break;
            case ByteCodeOp::CastS32F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F64);
                break;
            case ByteCodeOp::CastS64F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F64, EMITF_B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F64);
                break;
            case ByteCodeOp::CastU8F64:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32, OpBits::B8, false);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F64);
                break;
            case ByteCodeOp::CastU16F64:
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32, OpBits::B16, false);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F64);
                break;
            case ByteCodeOp::CastU32F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTI2F, OpBits::F64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F64);
                break;
            case ByteCodeOp::CastU64F64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                pp.emitLoadExtend(CPUReg::XMM0, CPUReg::RAX, OpBits::F64, OpBits::B64, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F64);
                break;

            case ByteCodeOp::CastF32F64:
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::F32);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::RAX, CPUOp::CVTF2F, OpBits::F32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, OpBits::F64);
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
                emitShiftLogical(pp, CPUOp::SHL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpShiftRightS8:
            case ByteCodeOp::BinOpShiftRightS16:
            case ByteCodeOp::BinOpShiftRightS32:
            case ByteCodeOp::BinOpShiftRightS64:
                emitShiftRightArithmetic(pp);
                break;
            case ByteCodeOp::BinOpShiftRightU8:
            case ByteCodeOp::BinOpShiftRightU16:
            case ByteCodeOp::BinOpShiftRightU32:
            case ByteCodeOp::BinOpShiftRightU64:
                emitShiftLogical(pp, CPUOp::SHR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpXor8:
            case ByteCodeOp::BinOpXor16:
            case ByteCodeOp::BinOpXor32:
            case ByteCodeOp::BinOpXor64:
                emitBinOp(pp, CPUOp::XOR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpMulS8:
            case ByteCodeOp::BinOpMulS16:
            case ByteCodeOp::BinOpMulS32:
            case ByteCodeOp::BinOpMulS64:
                emitBinOpOverflow(pp, CPUOp::IMUL, SafetyMsg::Mul, SCBE_CPU::getOpType(ip->op));
                break;
            case ByteCodeOp::BinOpMulU8:
            case ByteCodeOp::BinOpMulU16:
            case ByteCodeOp::BinOpMulU32:
            case ByteCodeOp::BinOpMulU64:
                emitBinOpOverflow(pp, CPUOp::MUL, SafetyMsg::Mul, SCBE_CPU::getOpType(ip->op));
                break;
            case ByteCodeOp::BinOpMulF32_Safe:
            case ByteCodeOp::BinOpMulF64_Safe:
                emitBinOp(pp, CPUOp::FMUL);
                break;

            case ByteCodeOp::BinOpMulS8_Safe:
            case ByteCodeOp::BinOpMulS16_Safe:
            case ByteCodeOp::BinOpMulS32_Safe:
            case ByteCodeOp::BinOpMulS64_Safe:
                emitBinOp(pp, CPUOp::IMUL);
                break;
            case ByteCodeOp::BinOpMulU8_Safe:
            case ByteCodeOp::BinOpMulU16_Safe:
            case ByteCodeOp::BinOpMulU32_Safe:
            case ByteCodeOp::BinOpMulU64_Safe:
                emitBinOp(pp, CPUOp::MUL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpModuloS8:
            case ByteCodeOp::BinOpModuloS16:
            case ByteCodeOp::BinOpModuloS32:
            case ByteCodeOp::BinOpModuloS64:
                emitBinOp(pp, CPUOp::IMOD);
                break;
            case ByteCodeOp::BinOpModuloU8:
            case ByteCodeOp::BinOpModuloU16:
            case ByteCodeOp::BinOpModuloU32:
            case ByteCodeOp::BinOpModuloU64:
                emitBinOp(pp, CPUOp::MOD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpDivS8:
            case ByteCodeOp::BinOpDivS16:
            case ByteCodeOp::BinOpDivS32:
            case ByteCodeOp::BinOpDivS64:
                emitBinOp(pp, CPUOp::IDIV);
                break;
            case ByteCodeOp::BinOpDivU8:
            case ByteCodeOp::BinOpDivU16:
            case ByteCodeOp::BinOpDivU32:
            case ByteCodeOp::BinOpDivU64:
                emitBinOp(pp, CPUOp::DIV);
                break;
            case ByteCodeOp::BinOpDivF32:
            case ByteCodeOp::BinOpDivF64:
                emitBinOp(pp, CPUOp::FDIV);
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
                emitBinOpOverflow(pp, CPUOp::ADD, SafetyMsg::Plus, SCBE_CPU::getOpType(ip->op));
                break;
            case ByteCodeOp::BinOpPlusF32_Safe:
            case ByteCodeOp::BinOpPlusF64_Safe:
                emitBinOp(pp, CPUOp::FADD);
                break;

            case ByteCodeOp::BinOpPlusS8_Safe:
            case ByteCodeOp::BinOpPlusU8_Safe:
            case ByteCodeOp::BinOpPlusS16_Safe:
            case ByteCodeOp::BinOpPlusU16_Safe:
            case ByteCodeOp::BinOpPlusS32_Safe:
            case ByteCodeOp::BinOpPlusU32_Safe:
            case ByteCodeOp::BinOpPlusS64_Safe:
            case ByteCodeOp::BinOpPlusU64_Safe:
                emitBinOp(pp, CPUOp::ADD);
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
                emitBinOpOverflow(pp, CPUOp::SUB, SafetyMsg::Minus, SCBE_CPU::getOpType(ip->op));
                break;

            case ByteCodeOp::BinOpMinusS8_Safe:
            case ByteCodeOp::BinOpMinusU8_Safe:
            case ByteCodeOp::BinOpMinusS16_Safe:
            case ByteCodeOp::BinOpMinusU16_Safe:
            case ByteCodeOp::BinOpMinusS32_Safe:
            case ByteCodeOp::BinOpMinusU32_Safe:
            case ByteCodeOp::BinOpMinusS64_Safe:
            case ByteCodeOp::BinOpMinusU64_Safe:
                emitBinOp(pp, CPUOp::SUB);
                break;
            case ByteCodeOp::BinOpMinusF32_Safe:
            case ByteCodeOp::BinOpMinusF64_Safe:
                emitBinOp(pp, CPUOp::FSUB);
                break;
                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskAnd8:
            case ByteCodeOp::BinOpBitmaskAnd16:
            case ByteCodeOp::BinOpBitmaskAnd32:
            case ByteCodeOp::BinOpBitmaskAnd64:
                emitBinOp(pp, CPUOp::AND);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskOr8:
            case ByteCodeOp::BinOpBitmaskOr16:
            case ByteCodeOp::BinOpBitmaskOr32:
            case ByteCodeOp::BinOpBitmaskOr64:
                emitBinOp(pp, CPUOp::OR);
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
                emitShiftEqLogical(pp, CPUOp::SHL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpShiftRightEqS8:
            case ByteCodeOp::AffectOpShiftRightEqS16:
            case ByteCodeOp::AffectOpShiftRightEqS32:
            case ByteCodeOp::AffectOpShiftRightEqS64:
                emitShiftRightEqArithmetic(pp);
                break;
            case ByteCodeOp::AffectOpShiftRightEqU8:
            case ByteCodeOp::AffectOpShiftRightEqU16:
            case ByteCodeOp::AffectOpShiftRightEqU32:
            case ByteCodeOp::AffectOpShiftRightEqU64:
                emitShiftEqLogical(pp, CPUOp::SHR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpXorEq8:
            case ByteCodeOp::AffectOpXorEq16:
            case ByteCodeOp::AffectOpXorEq32:
            case ByteCodeOp::AffectOpXorEq64:
                emitBinOpEq(pp, 0, CPUOp::XOR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpOrEq8:
            case ByteCodeOp::AffectOpOrEq16:
            case ByteCodeOp::AffectOpOrEq32:
            case ByteCodeOp::AffectOpOrEq64:
                emitBinOpEq(pp, 0, CPUOp::OR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpAndEq8:
            case ByteCodeOp::AffectOpAndEq16:
            case ByteCodeOp::AffectOpAndEq32:
            case ByteCodeOp::AffectOpAndEq64:
                emitBinOpEq(pp, 0, CPUOp::AND);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMulEqS8:
            case ByteCodeOp::AffectOpMulEqS16:
            case ByteCodeOp::AffectOpMulEqS32:
            case ByteCodeOp::AffectOpMulEqS64:
                emitBinOpEqOverflow(pp, CPUOp::IMUL, SafetyMsg::MulEq, SCBE_CPU::getOpType(ip->op));
                break;
            case ByteCodeOp::AffectOpMulEqU8:
            case ByteCodeOp::AffectOpMulEqU16:
            case ByteCodeOp::AffectOpMulEqU32:
            case ByteCodeOp::AffectOpMulEqU64:
                emitBinOpEqOverflow(pp, CPUOp::MUL, SafetyMsg::MulEq, SCBE_CPU::getOpType(ip->op));
                break;

            case ByteCodeOp::AffectOpMulEqS8_Safe:
            case ByteCodeOp::AffectOpMulEqS16_Safe:
            case ByteCodeOp::AffectOpMulEqS32_Safe:
            case ByteCodeOp::AffectOpMulEqS64_Safe:
                emitBinOpEq(pp, ip->c.u32, CPUOp::IMUL);
                break;
            case ByteCodeOp::AffectOpMulEqU8_Safe:
            case ByteCodeOp::AffectOpMulEqU16_Safe:
            case ByteCodeOp::AffectOpMulEqU32_Safe:
            case ByteCodeOp::AffectOpMulEqU64_Safe:
                emitBinOpEq(pp, ip->c.u32, CPUOp::MUL);
                break;
            case ByteCodeOp::AffectOpMulEqF32_Safe:
            case ByteCodeOp::AffectOpMulEqF64_Safe:
                emitBinOpEq(pp, ip->c.u32, CPUOp::FMUL);
                break;

            case ByteCodeOp::AffectOpMulEqS8_SSafe:
            case ByteCodeOp::AffectOpMulEqS16_SSafe:
            case ByteCodeOp::AffectOpMulEqS32_SSafe:
            case ByteCodeOp::AffectOpMulEqS64_SSafe:
                emitBinOpEqS(pp, CPUOp::IMUL);
                break;
            case ByteCodeOp::AffectOpMulEqU8_SSafe:
            case ByteCodeOp::AffectOpMulEqU16_SSafe:
            case ByteCodeOp::AffectOpMulEqU32_SSafe:
            case ByteCodeOp::AffectOpMulEqU64_SSafe:
                emitBinOpEqS(pp, CPUOp::MUL);
                break;
            case ByteCodeOp::AffectOpMulEqF32_SSafe:
            case ByteCodeOp::AffectOpMulEqF64_SSafe:
                emitBinOpEqS(pp, CPUOp::FMUL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpDivEqS8:
            case ByteCodeOp::AffectOpDivEqS16:
            case ByteCodeOp::AffectOpDivEqS32:
            case ByteCodeOp::AffectOpDivEqS64:
                emitBinOpEq(pp, 0, CPUOp::IDIV);
                break;
            case ByteCodeOp::AffectOpDivEqU8:
            case ByteCodeOp::AffectOpDivEqU16:
            case ByteCodeOp::AffectOpDivEqU32:
            case ByteCodeOp::AffectOpDivEqU64:
                emitBinOpEq(pp, 0, CPUOp::DIV);
                break;
            case ByteCodeOp::AffectOpDivEqF32:
            case ByteCodeOp::AffectOpDivEqF64:
                emitBinOpEq(pp, 0, CPUOp::FDIV);
                break;

            case ByteCodeOp::AffectOpDivEqS8_SSafe:
            case ByteCodeOp::AffectOpDivEqS16_SSafe:
            case ByteCodeOp::AffectOpDivEqS32_SSafe:
            case ByteCodeOp::AffectOpDivEqS64_SSafe:
                emitBinOpEqS(pp, CPUOp::IDIV);
                break;
            case ByteCodeOp::AffectOpDivEqU8_SSafe:
            case ByteCodeOp::AffectOpDivEqU16_SSafe:
            case ByteCodeOp::AffectOpDivEqU32_SSafe:
            case ByteCodeOp::AffectOpDivEqU64_SSafe:
                emitBinOpEqS(pp, CPUOp::DIV);
                break;
            case ByteCodeOp::AffectOpDivEqF32_SSafe:
            case ByteCodeOp::AffectOpDivEqF64_SSafe:
                emitBinOpEqS(pp, CPUOp::FDIV);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpModuloEqS8:
            case ByteCodeOp::AffectOpModuloEqS16:
            case ByteCodeOp::AffectOpModuloEqS32:
            case ByteCodeOp::AffectOpModuloEqS64:
                emitBinOpEq(pp, 0, CPUOp::IMOD);
                break;
            case ByteCodeOp::AffectOpModuloEqU8:
            case ByteCodeOp::AffectOpModuloEqU16:
            case ByteCodeOp::AffectOpModuloEqU32:
            case ByteCodeOp::AffectOpModuloEqU64:
                emitBinOpEq(pp, 0, CPUOp::MOD);
                break;

            case ByteCodeOp::AffectOpModuloEqS8_SSafe:
            case ByteCodeOp::AffectOpModuloEqS16_SSafe:
            case ByteCodeOp::AffectOpModuloEqS32_SSafe:
            case ByteCodeOp::AffectOpModuloEqS64_SSafe:
                emitBinOpEqS(pp, CPUOp::IMOD);
                break;
            case ByteCodeOp::AffectOpModuloEqU8_SSafe:
            case ByteCodeOp::AffectOpModuloEqU16_SSafe:
            case ByteCodeOp::AffectOpModuloEqU32_SSafe:
            case ByteCodeOp::AffectOpModuloEqU64_SSafe:
                emitBinOpEqS(pp, CPUOp::MOD);
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
                emitBinOpEqOverflow(pp, CPUOp::ADD, SafetyMsg::PlusEq, SCBE_CPU::getOpType(ip->op));
                break;
            case ByteCodeOp::AffectOpPlusEqF32_Safe:
            case ByteCodeOp::AffectOpPlusEqF64_Safe:
                emitBinOpEq(pp, ip->c.u32, CPUOp::FADD);
                break;

            case ByteCodeOp::AffectOpPlusEqS8_Safe:
            case ByteCodeOp::AffectOpPlusEqS16_Safe:
            case ByteCodeOp::AffectOpPlusEqS32_Safe:
            case ByteCodeOp::AffectOpPlusEqS64_Safe:
            case ByteCodeOp::AffectOpPlusEqU8_Safe:
            case ByteCodeOp::AffectOpPlusEqU16_Safe:
            case ByteCodeOp::AffectOpPlusEqU32_Safe:
            case ByteCodeOp::AffectOpPlusEqU64_Safe:
                emitBinOpEq(pp, ip->c.u32, CPUOp::ADD);
                break;

            case ByteCodeOp::AffectOpPlusEqS8_SSafe:
            case ByteCodeOp::AffectOpPlusEqS16_SSafe:
            case ByteCodeOp::AffectOpPlusEqS32_SSafe:
            case ByteCodeOp::AffectOpPlusEqS64_SSafe:
            case ByteCodeOp::AffectOpPlusEqU8_SSafe:
            case ByteCodeOp::AffectOpPlusEqU16_SSafe:
            case ByteCodeOp::AffectOpPlusEqU32_SSafe:
            case ByteCodeOp::AffectOpPlusEqU64_SSafe:
                emitBinOpEqS(pp, CPUOp::ADD);
                break;
            case ByteCodeOp::AffectOpPlusEqF32_SSafe:
            case ByteCodeOp::AffectOpPlusEqF64_SSafe:
                emitBinOpEqS(pp, CPUOp::FADD);
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
                emitBinOpEqOverflow(pp, CPUOp::SUB, SafetyMsg::MinusEq, SCBE_CPU::getOpType(ip->op));
                break;
            case ByteCodeOp::AffectOpMinusEqF32_Safe:
            case ByteCodeOp::AffectOpMinusEqF64_Safe:
                emitBinOpEq(pp, ip->c.u32, CPUOp::FSUB);
                break;

            case ByteCodeOp::AffectOpMinusEqS8_Safe:
            case ByteCodeOp::AffectOpMinusEqS16_Safe:
            case ByteCodeOp::AffectOpMinusEqS32_Safe:
            case ByteCodeOp::AffectOpMinusEqS64_Safe:
            case ByteCodeOp::AffectOpMinusEqU8_Safe:
            case ByteCodeOp::AffectOpMinusEqU16_Safe:
            case ByteCodeOp::AffectOpMinusEqU32_Safe:
            case ByteCodeOp::AffectOpMinusEqU64_Safe:
                emitBinOpEq(pp, ip->c.u32, CPUOp::SUB);
                break;

            case ByteCodeOp::AffectOpMinusEqS8_SSafe:
            case ByteCodeOp::AffectOpMinusEqS16_SSafe:
            case ByteCodeOp::AffectOpMinusEqS32_SSafe:
            case ByteCodeOp::AffectOpMinusEqS64_SSafe:
            case ByteCodeOp::AffectOpMinusEqU8_SSafe:
            case ByteCodeOp::AffectOpMinusEqU16_SSafe:
            case ByteCodeOp::AffectOpMinusEqU32_SSafe:
            case ByteCodeOp::AffectOpMinusEqU64_SSafe:
                emitBinOpEqS(pp, CPUOp::SUB);
                break;
            case ByteCodeOp::AffectOpMinusEqF32_SSafe:
            case ByteCodeOp::AffectOpMinusEqF64_SSafe:
                emitBinOpEqS(pp, CPUOp::FSUB);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ZeroToTrue:
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B32);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::E);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B8);
                break;
            case ByteCodeOp::LowerZeroToTrue:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B32);
                pp.emitOpBinary(CPUReg::RAX, 31, CPUOp::SHR, OpBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B8);
                break;
            case ByteCodeOp::LowerEqZeroToTrue:
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B32);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::LE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B8);
                break;
            case ByteCodeOp::GreaterZeroToTrue:
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B32);
                pp.emitSet(CPUReg::RAX, CPUCondFlag::G);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B8);
                break;
            case ByteCodeOp::GreaterEqZeroToTrue:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B32);
                pp.emitOpBinary(CPUReg::RAX, CPUReg::RAX, CPUOp::NOT, OpBits::B32);
                pp.emitOpBinary(CPUReg::RAX, 31, CPUOp::SHR, OpBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpGreaterS8:
            case ByteCodeOp::CompareOpGreaterS16:
            case ByteCodeOp::CompareOpGreaterS32:
            case ByteCodeOp::CompareOpGreaterS64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::G);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterU8:
            case ByteCodeOp::CompareOpGreaterU16:
            case ByteCodeOp::CompareOpGreaterU32:
            case ByteCodeOp::CompareOpGreaterU64:
            case ByteCodeOp::CompareOpGreaterF32:
            case ByteCodeOp::CompareOpGreaterF64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::A);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;

            case ByteCodeOp::CompareOpGreaterEqS8:
            case ByteCodeOp::CompareOpGreaterEqS16:
            case ByteCodeOp::CompareOpGreaterEqS32:
            case ByteCodeOp::CompareOpGreaterEqS64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::GE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;

            case ByteCodeOp::CompareOpGreaterEqU8:
            case ByteCodeOp::CompareOpGreaterEqU16:
            case ByteCodeOp::CompareOpGreaterEqU32:
            case ByteCodeOp::CompareOpGreaterEqU64:
            case ByteCodeOp::CompareOpGreaterEqF32:
            case ByteCodeOp::CompareOpGreaterEqF64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::AE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpLowerS8:
            case ByteCodeOp::CompareOpLowerS16:
            case ByteCodeOp::CompareOpLowerS32:
            case ByteCodeOp::CompareOpLowerS64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::L);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;

            case ByteCodeOp::CompareOpLowerU8:
            case ByteCodeOp::CompareOpLowerU16:
            case ByteCodeOp::CompareOpLowerU32:
            case ByteCodeOp::CompareOpLowerU64:
            case ByteCodeOp::CompareOpLowerF32:
            case ByteCodeOp::CompareOpLowerF64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::B);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;

            case ByteCodeOp::CompareOpLowerEqS8:
            case ByteCodeOp::CompareOpLowerEqS16:
            case ByteCodeOp::CompareOpLowerEqS32:
            case ByteCodeOp::CompareOpLowerEqS64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::LE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;

            case ByteCodeOp::CompareOpLowerEqU8:
            case ByteCodeOp::CompareOpLowerEqU16:
            case ByteCodeOp::CompareOpLowerEqU32:
            case ByteCodeOp::CompareOpLowerEqU64:
            case ByteCodeOp::CompareOpLowerEqF32:
            case ByteCodeOp::CompareOpLowerEqF64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::BE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOp3Way8:
            case ByteCodeOp::CompareOp3Way16:
            case ByteCodeOp::CompareOp3Way32:
            case ByteCodeOp::CompareOp3Way64:
                pp.emitClear(CPUReg::R8, OpBits::B32);
                emitCompareOp(pp, CPUReg::R8, CPUCondFlag::G);
                pp.emitLoad(CPUReg::RAX, 0xFFFFFFFF, OpBits::B32);
                pp.emitOpBinary(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVGE, OpBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B32);
                break;

            case ByteCodeOp::CompareOp3WayF32:
            case ByteCodeOp::CompareOp3WayF64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitClear(CPUReg::R8, OpBits::B32);
                emitIMMA(pp, CPUReg::XMM0, opBits);
                emitIMMB(pp, CPUReg::XMM1, opBits);
                pp.emitOpBinary(CPUReg::XMM0, CPUReg::XMM1, CPUOp::UCOMIF, opBits);
                pp.emitSet(CPUReg::R8, CPUCondFlag::A);
                pp.emitOpBinary(CPUReg::XMM1, CPUReg::XMM0, CPUOp::UCOMIF, opBits);
                pp.emitLoad(CPUReg::RAX, 0xFFFFFFFF, OpBits::B32);
                pp.emitOpBinary(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVBE, OpBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B32);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpEqual8:
            case ByteCodeOp::CompareOpEqual16:
            case ByteCodeOp::CompareOpEqual32:
            case ByteCodeOp::CompareOpEqual64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::E);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;
            case ByteCodeOp::CompareOpEqualF32:
            case ByteCodeOp::CompareOpEqualF64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::EP);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpNotEqual8:
            case ByteCodeOp::CompareOpNotEqual16:
            case ByteCodeOp::CompareOpNotEqual32:
            case ByteCodeOp::CompareOpNotEqual64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::NE);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;
            case ByteCodeOp::CompareOpNotEqualF32:
            case ByteCodeOp::CompareOpNotEqualF64:
                emitCompareOp(pp, CPUReg::RAX, CPUCondFlag::NEP);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicDbgAlloc:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_dbgalloc, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicSysAlloc:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_sysalloc, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicRtFlags:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_rtflags, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicStringCmp:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_strcmp, {ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32}, REG_OFFSET(ip->d.u32));
                break;
            case ByteCodeOp::IntrinsicTypeCmp:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_typecmp, {ip->a.u32, ip->b.u32, ip->c.u32}, REG_OFFSET(ip->d.u32));
                break;
            case ByteCodeOp::IntrinsicIs:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_is, {ip->a.u32, ip->b.u32}, REG_OFFSET(ip->c.u32));
                break;
            case ByteCodeOp::IntrinsicAs:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_as, {ip->a.u32, ip->b.u32, ip->c.u32}, REG_OFFSET(ip->d.u32));
                break;

                /////////////////////////////////////

            case ByteCodeOp::NegBool:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B8);
                pp.emitOpBinary(CPUReg::RAX, 1, CPUOp::XOR, OpBits::B8);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::NegS32:
            case ByteCodeOp::NegS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                if (ip->a.u32 == ip->b.u32)
                    pp.emitOpUnary(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUOp::NEG, opBits);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), opBits);
                    pp.emitOpUnary(CPUReg::RAX, CPUOp::NEG, opBits);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                }
                break;

            case ByteCodeOp::NegF32:
            case ByteCodeOp::NegF64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), opBits);
                pp.emitOpUnary(CPUReg::XMM0, CPUOp::NEG, opBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::InvertU8:
            case ByteCodeOp::InvertU16:
            case ByteCodeOp::InvertU32:
            case ByteCodeOp::InvertU64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                if (ip->a.u32 == ip->b.u32)
                    pp.emitOpUnary(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUOp::NOT, opBits);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), opBits);
                    pp.emitOpUnary(CPUReg::RAX, CPUOp::NOT, opBits);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpDyn8:
            case ByteCodeOp::JumpDyn16:
            case ByteCodeOp::JumpDyn32:
            case ByteCodeOp::JumpDyn64:
                emitJumpDyn(pp);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfTrue:
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B8);
                pp.emitJump(JNZ, pp.ipIndex + ip->b.s32 + 1);
                break;
            case ByteCodeOp::JumpIfFalse:
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B8);
                pp.emitJump(JZ, pp.ipIndex + ip->b.s32 + 1);
                break;
            case ByteCodeOp::JumpIfRTTrue:
                pp.emitCmp(CPUReg::RDI, offsetRT + REG_OFFSET(0), 0, OpBits::B8);
                pp.emitJump(JNZ, pp.ipIndex + ip->b.s32 + 1);
                break;
            case ByteCodeOp::JumpIfRTFalse:
                pp.emitCmp(CPUReg::RDI, offsetRT + REG_OFFSET(0), 0, OpBits::B8);
                pp.emitJump(JZ, pp.ipIndex + ip->b.s32 + 1);
                break;

            case ByteCodeOp::JumpIfNotZero8:
            case ByteCodeOp::JumpIfNotZero16:
            case ByteCodeOp::JumpIfNotZero32:
            case ByteCodeOp::JumpIfNotZero64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, opBits);
                pp.emitJump(JNZ, pp.ipIndex + ip->b.s32 + 1);
                break;

            case ByteCodeOp::JumpIfZero8:
            case ByteCodeOp::JumpIfZero16:
            case ByteCodeOp::JumpIfZero32:
            case ByteCodeOp::JumpIfZero64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, opBits);
                pp.emitJump(JZ, pp.ipIndex + ip->b.s32 + 1);
                break;

            case ByteCodeOp::Jump:
                pp.emitJump(JUMP, pp.ipIndex + ip->b.s32 + 1);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfNotEqual8:
                emitJumpCmp(pp, JNZ, OpBits::B8);
                break;
            case ByteCodeOp::JumpIfNotEqual16:
                emitJumpCmp(pp, JNZ, OpBits::B16);
                break;
            case ByteCodeOp::JumpIfNotEqual32:
                emitJumpCmp(pp, JNZ, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfNotEqual64:
                emitJumpCmp(pp, JNZ, OpBits::B64);
                break;
            case ByteCodeOp::JumpIfNotEqualF32:
                emitJumpCmp2(pp, JP, JNZ, OpBits::F32);
                break;
            case ByteCodeOp::JumpIfNotEqualF64:
                emitJumpCmp2(pp, JP, JNZ, OpBits::F64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfEqual8:
                emitJumpCmp(pp, JZ, OpBits::B8);
                break;
            case ByteCodeOp::JumpIfEqual16:
                emitJumpCmp(pp, JZ, OpBits::B16);
                break;
            case ByteCodeOp::JumpIfEqual32:
                emitJumpCmp(pp, JZ, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfEqual64:
                emitJumpCmp(pp, JZ, OpBits::B64);
                break;
            case ByteCodeOp::JumpIfEqualF32:
                emitJumpCmp2(pp, JP, JZ, OpBits::F32);
                break;
            case ByteCodeOp::JumpIfEqualF64:
                emitJumpCmp2(pp, JP, JZ, OpBits::F64);
                break;
            case ByteCodeOp::IncJumpIfEqual64:
                pp.emitOpBinary(CPUReg::RDI, REG_OFFSET(ip->a.u32), 1, CPUOp::ADD, OpBits::B64);
                emitJumpCmp(pp, JZ, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfStackEqual8:
                emitJumpCmpAddr(pp, JZ, CPUReg::RDI, offsetByteCodeStack + ip->a.u32, OpBits::B8);
                break;
            case ByteCodeOp::JumpIfStackNotEqual8:
                emitJumpCmpAddr(pp, JNZ, CPUReg::RDI, offsetByteCodeStack + ip->a.u32, OpBits::B8);
                break;

            case ByteCodeOp::JumpIfStackEqual16:
                emitJumpCmpAddr(pp, JZ, CPUReg::RDI, offsetByteCodeStack + ip->a.u32, OpBits::B16);
                break;
            case ByteCodeOp::JumpIfStackNotEqual16:
                emitJumpCmpAddr(pp, JNZ, CPUReg::RDI, offsetByteCodeStack + ip->a.u32, OpBits::B16);
                break;

            case ByteCodeOp::JumpIfStackEqual32:
                emitJumpCmpAddr(pp, JZ, CPUReg::RDI, offsetByteCodeStack + ip->a.u32, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfStackNotEqual32:
                emitJumpCmpAddr(pp, JNZ, CPUReg::RDI, offsetByteCodeStack + ip->a.u32, OpBits::B32);
                break;

            case ByteCodeOp::JumpIfStackEqual64:
                emitJumpCmpAddr(pp, JZ, CPUReg::RDI, offsetByteCodeStack + ip->a.u32, OpBits::B64);
                break;
            case ByteCodeOp::JumpIfStackNotEqual64:
                emitJumpCmpAddr(pp, JNZ, CPUReg::RDI, offsetByteCodeStack + ip->a.u32, OpBits::B64);
                break;

            case ByteCodeOp::JumpIfDeRefEqual8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, JZ, CPUReg::RCX, ip->d.u32, OpBits::B8);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual8:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, JNZ, CPUReg::RCX, ip->d.u32, OpBits::B8);
                break;

            case ByteCodeOp::JumpIfDeRefEqual16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, JZ, CPUReg::RCX, ip->d.u32, OpBits::B16);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual16:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, JNZ, CPUReg::RCX, ip->d.u32, OpBits::B16);
                break;

            case ByteCodeOp::JumpIfDeRefEqual32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, JZ, CPUReg::RCX, ip->d.u32, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual32:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, JNZ, CPUReg::RCX, ip->d.u32, OpBits::B32);
                break;

            case ByteCodeOp::JumpIfDeRefEqual64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, JZ, CPUReg::RCX, ip->d.u32, OpBits::B64);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual64:
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, JNZ, CPUReg::RCX, ip->d.u32, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfLowerS8:
            case ByteCodeOp::JumpIfLowerS16:
            case ByteCodeOp::JumpIfLowerS32:
            case ByteCodeOp::JumpIfLowerS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitJumpCmp(pp, JL, opBits);
                break;
            case ByteCodeOp::JumpIfLowerU8:
            case ByteCodeOp::JumpIfLowerU16:
            case ByteCodeOp::JumpIfLowerU32:
            case ByteCodeOp::JumpIfLowerU64:
            case ByteCodeOp::JumpIfLowerF32:
            case ByteCodeOp::JumpIfLowerF64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitJumpCmp(pp, JB, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfLowerEqS8:
            case ByteCodeOp::JumpIfLowerEqS16:
            case ByteCodeOp::JumpIfLowerEqS32:
            case ByteCodeOp::JumpIfLowerEqS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitJumpCmp(pp, JLE, opBits);
                break;

            case ByteCodeOp::JumpIfLowerEqU8:
            case ByteCodeOp::JumpIfLowerEqU16:
            case ByteCodeOp::JumpIfLowerEqU32:
            case ByteCodeOp::JumpIfLowerEqU64:
            case ByteCodeOp::JumpIfLowerEqF32:
            case ByteCodeOp::JumpIfLowerEqF64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitJumpCmp(pp, JBE, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfGreaterS8:
            case ByteCodeOp::JumpIfGreaterS16:
            case ByteCodeOp::JumpIfGreaterS32:
            case ByteCodeOp::JumpIfGreaterS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitJumpCmp(pp, JG, opBits);
                break;

            case ByteCodeOp::JumpIfGreaterU8:
            case ByteCodeOp::JumpIfGreaterU16:
            case ByteCodeOp::JumpIfGreaterU32:
            case ByteCodeOp::JumpIfGreaterU64:
            case ByteCodeOp::JumpIfGreaterF32:
            case ByteCodeOp::JumpIfGreaterF64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitJumpCmp(pp, JA, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfGreaterEqS8:
            case ByteCodeOp::JumpIfGreaterEqS16:
            case ByteCodeOp::JumpIfGreaterEqS32:
            case ByteCodeOp::JumpIfGreaterEqS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitJumpCmp(pp, JGE, opBits);
                break;

            case ByteCodeOp::JumpIfGreaterEqU8:
            case ByteCodeOp::JumpIfGreaterEqU16:
            case ByteCodeOp::JumpIfGreaterEqU32:
            case ByteCodeOp::JumpIfGreaterEqU64:
            case ByteCodeOp::JumpIfGreaterEqF32:
            case ByteCodeOp::JumpIfGreaterEqF64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitJumpCmp(pp, JAE, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::DecrementRA32:
            case ByteCodeOp::DecrementRA64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitOpBinary(CPUReg::RDI, REG_OFFSET(ip->a.u32), 1, CPUOp::SUB, opBits);
                break;

            case ByteCodeOp::IncrementRA64:
                pp.emitOpBinary(CPUReg::RDI, REG_OFFSET(ip->a.u32), 1, CPUOp::ADD, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::DeRef8:
            case ByteCodeOp::DeRef16:
            case ByteCodeOp::DeRef32:
            case ByteCodeOp::DeRef64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RAX, ip->c.u64, OpBits::B64, opBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::DeRefStringSlice:
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFFF);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RAX, ip->c.u32 + 8, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUReg::RCX, OpBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, ip->c.u32 + 0, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromBssSeg8:
            case ByteCodeOp::GetFromBssSeg16:
            case ByteCodeOp::GetFromBssSeg32:
            case ByteCodeOp::GetFromBssSeg64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symBSIndex, 0);
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RAX, ip->b.u32, OpBits::B64, opBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::GetFromMutableSeg8:
            case ByteCodeOp::GetFromMutableSeg16:
            case ByteCodeOp::GetFromMutableSeg32:
            case ByteCodeOp::GetFromMutableSeg64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symMSIndex, 0);
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RAX, ip->b.u32, OpBits::B64, opBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8:
            case ByteCodeOp::GetFromStack16:
            case ByteCodeOp::GetFromStack32:
            case ByteCodeOp::GetFromStack64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, offsetByteCodeStack + ip->b.u32, OpBits::B64, opBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;
            case ByteCodeOp::GetIncFromStack64:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetByteCodeStack + ip->b.u32, OpBits::B64);
                if (ip->c.u64)
                    pp.emitOpBinary(CPUReg::RAX, ip->c.u64, CPUOp::ADD, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8x2:
            case ByteCodeOp::GetFromStack16x2:
            case ByteCodeOp::GetFromStack32x2:
            case ByteCodeOp::GetFromStack64x2:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, offsetByteCodeStack + ip->b.u32, OpBits::B64, opBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, offsetByteCodeStack + ip->d.u32, OpBits::B64, opBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetIncFromStack64DeRef8:
            case ByteCodeOp::GetIncFromStack64DeRef16:
            case ByteCodeOp::GetIncFromStack64DeRef32:
            case ByteCodeOp::GetIncFromStack64DeRef64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetByteCodeStack + ip->b.u32, OpBits::B64);
                pp.emitLoadExtend(CPUReg::RAX, CPUReg::RAX, ip->c.u64, OpBits::B64, opBits, false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyStack8:
            case ByteCodeOp::CopyStack16:
            case ByteCodeOp::CopyStack32:
            case ByteCodeOp::CopyStack64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetByteCodeStack + ip->b.u32, opBits);
                pp.emitStore(CPUReg::RDI, offsetByteCodeStack + ip->a.u32, CPUReg::RAX, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearMaskU32:
            case ByteCodeOp::ClearMaskU64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                if (ip->b.u32 == 0xFF)
                    pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), opBits, OpBits::B8, false);
                else if (ip->b.u32 == 0xFFFF)
                    pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), opBits, OpBits::B16, false);
                else if (ip->b.u32 == 0xFFFFFFFF)
                    pp.emitLoadExtend(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), opBits, OpBits::B32, false);
                else
                    SWAG_ASSERT(false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetZeroAtPointer8:
            case ByteCodeOp::SetZeroAtPointer16:
            case ByteCodeOp::SetZeroAtPointer32:
            case ByteCodeOp::SetZeroAtPointer64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitStore(CPUReg::RAX, ip->b.u32, 0, opBits);
                break;

            case ByteCodeOp::SetZeroAtPointerX:
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                if (ip->b.u32 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                    pp.emitClear(CPUReg::RAX, ip->c.u32, ip->b.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CPUPushParamType::SwagRegisterAdd, .value = ip->a.u32, .value2 = ip->c.u64});
                    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = 0});
                    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = ip->b.u64});
                    emitInternalCallCPUParams(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            case ByteCodeOp::SetZeroAtPointerXRB:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .value = ip->a.u32});
                pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = 0});
                pp.pushParams.push_back({.type = CPUPushParamType::SwagRegisterMul, .value = ip->b.u32, .value2 = ip->c.u64});
                emitInternalCallCPUParams(pp, g_LangSpec->name_memset, pp.pushParams);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearRR8:
            case ByteCodeOp::ClearRR16:
            case ByteCodeOp::ClearRR32:
            case ByteCodeOp::ClearRR64:
            {
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoadParam(CPUReg::RAX, typeFunc->numParamsRegisters(), OpBits::B64);
                pp.emitStore(CPUReg::RAX, ip->c.u32, 0, opBits);
                break;
            }

            case ByteCodeOp::ClearRRX:
            {
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                pp.emitLoadParam(CPUReg::RAX, typeFunc->numParamsRegisters(), OpBits::B64);
                if (ip->b.u32 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitClear(CPUReg::RAX, ip->c.u32, ip->b.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CPUPushParamType::CPURegister, .value = static_cast<uint64_t>(CPUReg::RAX)});
                    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = 0});
                    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = ip->b.u64});
                    emitInternalCallCPUParams(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetZeroStack8:
            case ByteCodeOp::SetZeroStack16:
            case ByteCodeOp::SetZeroStack32:
            case ByteCodeOp::SetZeroStack64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitStore(CPUReg::RDI, offsetByteCodeStack + ip->a.u32, 0, opBits);
                break;
            case ByteCodeOp::SetZeroStackX:
                if (ip->b.u32 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                    pp.emitClear(CPUReg::RDI, offsetByteCodeStack + ip->a.u32, ip->b.u32);
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CPUPushParamType::LoadAddress, .value = offsetByteCodeStack + ip->a.u32});
                    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = 0});
                    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = ip->b.u32});
                    emitInternalCallCPUParams(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtPointer8:
            case ByteCodeOp::SetAtPointer16:
            case ByteCodeOp::SetAtPointer32:
            case ByteCodeOp::SetAtPointer64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RAX, ip->c.u32, ip->b.u64, opBits);
                else
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), opBits);
                    pp.emitStore(CPUReg::RAX, ip->c.u32, CPUReg::RCX, opBits);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8:
            case ByteCodeOp::SetAtStackPointer16:
            case ByteCodeOp::SetAtStackPointer32:
            case ByteCodeOp::SetAtStackPointer64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RDI, offsetByteCodeStack + ip->a.u32, ip->b.u64, opBits);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), opBits);
                    pp.emitStore(CPUReg::RDI, offsetByteCodeStack + ip->a.u32, CPUReg::RAX, opBits);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8x2:
            case ByteCodeOp::SetAtStackPointer16x2:
            case ByteCodeOp::SetAtStackPointer32x2:
            case ByteCodeOp::SetAtStackPointer64x2:
                opBits = SCBE_CPU::getOpBits(ip->op);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitStore(CPUReg::RDI, offsetByteCodeStack + ip->a.u32, ip->b.u64, opBits);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), opBits);
                    pp.emitStore(CPUReg::RDI, offsetByteCodeStack + ip->a.u32, CPUReg::RAX, opBits);
                }

                if (ip->hasFlag(BCI_IMM_D))
                    pp.emitStore(CPUReg::RDI, offsetByteCodeStack + ip->c.u32, ip->d.u64, opBits);
                else
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->d.u32), opBits);
                    pp.emitStore(CPUReg::RDI, offsetByteCodeStack + ip->c.u32, CPUReg::RAX, opBits);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeStackPointer:
                pp.emitLoadAddress(CPUReg::RAX, CPUReg::RDI, offsetByteCodeStack + ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;
            case ByteCodeOp::MakeStackPointerx2:
                pp.emitLoadAddress(CPUReg::RAX, CPUReg::RDI, offsetByteCodeStack + ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                pp.emitLoadAddress(CPUReg::RAX, CPUReg::RDI, offsetByteCodeStack + ip->d.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B64);
                break;
            case ByteCodeOp::MakeStackPointerRT:
                pp.emitLoadAddress(CPUReg::RAX, CPUReg::RDI, offsetByteCodeStack + ip->a.u32);
                pp.emitStore(CPUReg::RDI, offsetRT + REG_OFFSET(0), CPUReg::RAX, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeMutableSegPointer:
                pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symMSIndex, ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;
            case ByteCodeOp::MakeBssSegPointer:
                pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symBSIndex, ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;
            case ByteCodeOp::MakeConstantSegPointer:
                pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symCSIndex, ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;
            case ByteCodeOp::MakeCompilerSegPointer:
                break;

                /////////////////////////////////////

            case ByteCodeOp::IncPointer64:
                emitAddSubMul64(pp, 1, CPUOp::ADD);
                break;
            case ByteCodeOp::DecPointer64:
                emitAddSubMul64(pp, 1, CPUOp::SUB);
                break;
            case ByteCodeOp::IncMulPointer64:
                emitAddSubMul64(pp, ip->d.u64, CPUOp::ADD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::Mul64byVB64:
                pp.emitOpBinary(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u64, CPUOp::IMUL, OpBits::B64);
                break;
            case ByteCodeOp::Div64byVB64:
                pp.emitOpBinary(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u64, CPUOp::IDIV, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetImmediate32:
            case ByteCodeOp::SetImmediate64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u64, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::MemCpy8:
            case ByteCodeOp::MemCpy16:
            case ByteCodeOp::MemCpy32:
            case ByteCodeOp::MemCpy64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitLoad(CPUReg::RDX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                pp.emitCopy(CPUReg::RCX, CPUReg::RDX, SCBE_CPU::getNumBits(opBits) / 8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMemCpy:
                if (ip->hasFlag(BCI_IMM_C) &&
                    ip->c.u64 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                    pp.emitLoad(CPUReg::RDX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                    pp.emitCopy(CPUReg::RCX, CPUReg::RDX, ip->c.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .value = ip->a.u32});
                    pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .value = ip->b.u32});
                    pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_C) ? CPUPushParamType::Constant : CPUPushParamType::SwagRegister, .value = ip->c.u64});
                    emitInternalCallCPUParams(pp, g_LangSpec->name_memcpy, pp.pushParams);
                }
                break;
            case ByteCodeOp::IntrinsicMemSet:
                if (ip->hasFlag(BCI_IMM_B) &&
                    ip->hasFlag(BCI_IMM_C) &&
                    ip->b.u8 == 0 &&
                    ip->c.u64 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                    pp.emitClear(CPUReg::RCX, 0, ip->c.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .value = ip->a.u32});
                    pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_B) ? CPUPushParamType::Constant : CPUPushParamType::SwagRegister, .value = ip->b.u8});
                    pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_C) ? CPUPushParamType::Constant : CPUPushParamType::SwagRegister, .value = ip->c.u64});
                    emitInternalCallCPUParams(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            case ByteCodeOp::IntrinsicMemMove:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .value = ip->a.u32});
                pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .value = ip->b.u32});
                pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_C) ? CPUPushParamType::Constant : CPUPushParamType::SwagRegister, .value = ip->c.u64});
                emitInternalCallCPUParams(pp, g_LangSpec->name_memmove, pp.pushParams);
                break;
            case ByteCodeOp::IntrinsicMemCmp:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .value = ip->b.u32});
                pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .value = ip->c.u32});
                pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_D) ? CPUPushParamType::Constant : CPUPushParamType::SwagRegister, .value = ip->d.u64});
                emitInternalCallCPUParams(pp, g_LangSpec->name_memcmp, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;

            case ByteCodeOp::IntrinsicStrLen:
                emitInternalCallRAParams(pp, g_LangSpec->name_strlen, {ip->b.u32}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicStrCmp:
                emitInternalCallRAParams(pp, g_LangSpec->name_strcmp, {ip->b.u32, ip->c.u32}, REG_OFFSET(ip->a.u32));
                break;

            case ByteCodeOp::IntrinsicAlloc:
                emitInternalCallRAParams(pp, g_LangSpec->name_malloc, {ip->b.u32}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicRealloc:
                emitInternalCallRAParams(pp, g_LangSpec->name_realloc, {ip->b.u32, ip->c.u32}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicFree:
                emitInternalCallRAParams(pp, g_LangSpec->name_free, {ip->a.u32});
                break;

            case ByteCodeOp::InternalPanic:
                emitInternalPanic(pp, reinterpret_cast<const char*>(ip->d.pointer));
                break;
            case ByteCodeOp::InternalUnreachable:
                break;
            case ByteCodeOp::Unreachable:
                emitInternalPanic(pp, "executing unreachable code");
                break;

            case ByteCodeOp::InternalGetTlsPtr:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::SymbolRelocationValue, .value = pp.symTls_threadLocalId});
                pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = module->tlsSegment.totalCount, .typeInfo = g_TypeMgr->typeInfoU64});
                pp.pushParams.push_back({.type = CPUPushParamType::SymbolRelocationAddress, .value = pp.symTLSIndex});
                emitInternalCallCPUParams(pp, g_LangSpec->name_priv_tlsGetPtr, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicGetContext:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::SymbolRelocationValue, .value = pp.symPI_contextTlsId});
                emitInternalCallCPUParams(pp, g_LangSpec->name_priv_tlsGetValue, pp.pushParams, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicSetContext:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CPUPushParamType::SymbolRelocationValue, .value = pp.symPI_contextTlsId});
                pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .value = ip->a.u32});
                emitInternalCallCPUParams(pp, g_LangSpec->name_priv_tlsSetValue, pp.pushParams);
                break;

            case ByteCodeOp::IntrinsicGetProcessInfos:
                pp.emitSymbolRelocationAddress(CPUReg::RCX, pp.symPI_processInfos, 0);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RCX, OpBits::B64);
                break;

            case ByteCodeOp::IntrinsicCVaStart:
            {
                const uint32_t paramIdx = typeFunc->numParamsRegisters() + (typeFunc->returnByAddress() ? 1 : 0);
                pp.emitLoadAddressParam(CPUReg::RAX, paramIdx, true);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, OpBits::B64);
                break;
            }
            case ByteCodeOp::IntrinsicCVaEnd:
                break;
            case ByteCodeOp::IntrinsicCVaArg:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RAX, 0, OpBits::B64);
                pp.emitLoadExtend(CPUReg::RCX, CPUReg::RCX, 0, OpBits::B64, static_cast<OpBits>(ip->c.u32 * 8), false);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUReg::RCX, OpBits::B64);
                pp.emitOpBinary(CPUReg::RAX, 0, 8, CPUOp::ADD, OpBits::B64);
                break;

            case ByteCodeOp::IntrinsicArguments:
                SWAG_ASSERT(ip->b.u32 == ip->a.u32 + 1);
                emitInternalCallRAParams(pp, g_LangSpec->name_at_args, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicModules:
                if (buildParameters.module->modulesSliceOffset == UINT32_MAX)
                {
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B64);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, OpBits::B64);
                }
                else
                {
                    pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symCSIndex, buildParameters.module->modulesSliceOffset);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), buildParameters.module->moduleDependencies.count + 1, OpBits::B64);
                }
                break;
            case ByteCodeOp::IntrinsicGvtd:
                if (buildParameters.module->globalVarsToDropSliceOffset == UINT32_MAX)
                {
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B64);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, OpBits::B64);
                }
                else
                {
                    pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symMSIndex, buildParameters.module->globalVarsToDropSliceOffset);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), buildParameters.module->globalVarsToDrop.count, OpBits::B64);
                }
                break;

            case ByteCodeOp::IntrinsicCompiler:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), 0, OpBits::B64);
                break;
            case ByteCodeOp::IntrinsicIsByteCode:
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, OpBits::B32);
                break;
            case ByteCodeOp::IntrinsicCompilerError:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_compilererror, {ip->a.u32, ip->b.u32, ip->c.u32});
                break;
            case ByteCodeOp::IntrinsicCompilerWarning:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_compilerwarning, {ip->a.u32, ip->b.u32, ip->c.u32});
                break;
            case ByteCodeOp::IntrinsicPanic:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_panic, {ip->a.u32, ip->b.u32, ip->c.u32});
                break;
            case ByteCodeOp::IntrinsicItfTableOf:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_itftableof, {ip->a.u32, ip->b.u32}, REG_OFFSET(ip->c.u32));
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyRAtoRR:
                if (ip->hasFlag(BCI_IMM_A))
                {
                    pp.emitStore(CPUReg::RDI, offsetResult, ip->a.u64, OpBits::B64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                    pp.emitStore(CPUReg::RDI, offsetResult, CPUReg::RCX, OpBits::B64);
                }

                break;

            case ByteCodeOp::CopyRARBtoRR2:
            {
                pp.emitLoadParam(CPUReg::RAX, typeFunc->numParamsRegisters(), OpBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, OpBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                pp.emitStore(CPUReg::RAX, 8, CPUReg::RCX, OpBits::B64);
                break;
            }

            case ByteCodeOp::CopyRAtoRT:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitStore(CPUReg::RDI, offsetRT + REG_OFFSET(0), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::SaveRRtoRA:
            {
                pp.emitLoadParam(CPUReg::RAX, typeFunc->numParamsRegisters(), OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;
            }
            case ByteCodeOp::CopyRRtoRA:
            {
                pp.emitLoadParam(CPUReg::RAX, typeFunc->numParamsRegisters(), OpBits::B64);
                if (ip->b.u64)
                {
                    pp.emitLoad(CPUReg::RCX, ip->b.u64, OpBits::B64);
                    pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::ADD, OpBits::B64);
                }

                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;
            }

            case ByteCodeOp::CopyRTtoRA:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetRT + REG_OFFSET(0), OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::CopyRT2toRARB:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetRT + REG_OFFSET(0), OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetRT + REG_OFFSET(1), OpBits::B64);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::CopySP:
                pp.emitLoadAddress(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->c.u32));
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                break;

            case ByteCodeOp::CopySPVaargs:
                emitCopyVaargs(pp);
                break;

                /////////////////////////////////////

            case ByteCodeOp::PushRVParam:
                pp.pushRVParams.push_back({ip->a.u32, ip->b.u32});
                break;
            case ByteCodeOp::PushRAParam:
            case ByteCodeOp::PushRAParamCond:
                pp.pushRAParams.push_back(ip->a.u32);
                break;
            case ByteCodeOp::PushRAParam2:
                pp.pushRAParams.push_back(ip->a.u32);
                pp.pushRAParams.push_back(ip->b.u32);
                break;
            case ByteCodeOp::PushRAParam3:
                pp.pushRAParams.push_back(ip->a.u32);
                pp.pushRAParams.push_back(ip->b.u32);
                pp.pushRAParams.push_back(ip->c.u32);
                break;
            case ByteCodeOp::PushRAParam4:
                pp.pushRAParams.push_back(ip->a.u32);
                pp.pushRAParams.push_back(ip->b.u32);
                pp.pushRAParams.push_back(ip->c.u32);
                pp.pushRAParams.push_back(ip->d.u32);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetParam8:
            case ByteCodeOp::GetParam16:
            case ByteCodeOp::GetParam32:
            case ByteCodeOp::GetParam64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitGetParam(pp, ip->a.u32, ip->b.mergeU64U32.high, opBits);
                break;
            case ByteCodeOp::GetParam64x2:
                emitGetParam(pp, ip->a.u32, ip->b.mergeU64U32.high, OpBits::B64);
                emitGetParam(pp, ip->c.u32, ip->d.mergeU64U32.high, OpBits::B64);
                break;
            case ByteCodeOp::GetIncParam64:
                emitGetParam(pp, ip->a.u32, ip->b.mergeU64U32.high, OpBits::B64, ip->d.u64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetParam64DeRef8:
            case ByteCodeOp::GetParam64DeRef16:
            case ByteCodeOp::GetParam64DeRef32:
            case ByteCodeOp::GetParam64DeRef64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitGetParam(pp, ip->a.u32, ip->b.mergeU64U32.high, OpBits::B64, 0, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetIncParam64DeRef8:
            case ByteCodeOp::GetIncParam64DeRef16:
            case ByteCodeOp::GetIncParam64DeRef32:
            case ByteCodeOp::GetIncParam64DeRef64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitGetParam(pp, ip->a.u32, ip->b.mergeU64U32.high, OpBits::B64, ip->d.u64, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeLambda:
                emitMakeLambda(pp);
                break;

            case ByteCodeOp::IntrinsicMakeCallback:
                emitMakeCallback(pp);
                break;

                /////////////////////////////////////

            case ByteCodeOp::LocalCall:
            case ByteCodeOp::LocalCallPop:
            case ByteCodeOp::LocalCallPop16:
            case ByteCodeOp::LocalCallPop48:
            case ByteCodeOp::LocalCallPopRC:
                emitLocalCall(pp);
                break;
            case ByteCodeOp::LocalCallPopParam:
                pp.pushRAParams.push_back(ip->d.u32);
                emitLocalCall(pp);
                break;
            case ByteCodeOp::LocalCallPop0Param2:
            case ByteCodeOp::LocalCallPop16Param2:
            case ByteCodeOp::LocalCallPop48Param2:
                pp.pushRAParams.push_back(ip->c.u32);
                pp.pushRAParams.push_back(ip->d.u32);
                emitLocalCall(pp);
                break;

            case ByteCodeOp::ForeignCall:
            case ByteCodeOp::ForeignCallPop:
                emitForeignCall(pp);
                break;
            case ByteCodeOp::ForeignCallPopParam:
                pp.pushRAParams.push_back(ip->d.u32);
                emitForeignCall(pp);
                break;
            case ByteCodeOp::ForeignCallPop0Param2:
            case ByteCodeOp::ForeignCallPop16Param2:
            case ByteCodeOp::ForeignCallPop48Param2:
                pp.pushRAParams.push_back(ip->c.u32);
                pp.pushRAParams.push_back(ip->d.u32);
                emitForeignCall(pp);
                break;

            case ByteCodeOp::LambdaCall:
            case ByteCodeOp::LambdaCallPop:
                emitLambdaCall(pp);
                break;
            case ByteCodeOp::LambdaCallPopParam:
                pp.pushRAParams.push_back(ip->d.u32);
                emitLambdaCall(pp);
                break;

                /////////////////////////////////////

            case ByteCodeOp::IncSPPostCall:
            case ByteCodeOp::IncSPPostCallCond:
                pp.pushRAParams.clear();
                pp.pushRVParams.clear();
                break;

            case ByteCodeOp::CopyRAtoRRRet:
                if (ip->hasFlag(BCI_IMM_A))
                {
                    pp.emitStore(CPUReg::RDI, offsetResult, ip->a.u64, OpBits::B64);
                }
                else
                {
                    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                    pp.emitStore(CPUReg::RDI, offsetResult, CPUReg::RCX, OpBits::B64);
                }
                [[fallthrough]];

            case ByteCodeOp::Ret:

                // Emit result
                if (!returnType->isVoid() && !typeFunc->returnByAddress())
                {
                    pp.emitLoad(cc.returnByRegisterInteger, CPUReg::RDI, offsetResult, OpBits::B64);
                    if (returnType->isNative(NativeTypeKind::F32))
                        pp.emitLoad(cc.returnByRegisterFloat, CPUReg::RAX, OpBits::F32);
                    else if (returnType->isNative(NativeTypeKind::F64))
                        pp.emitLoad(cc.returnByRegisterFloat, CPUReg::RAX, OpBits::F64);
                }

                pp.emitLeave();
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMulAddF32:
            case ByteCodeOp::IntrinsicMulAddF64:
            {
                opBits = SCBE_CPU::getOpBits(ip->op);
                emitIMMB(pp, CPUReg::XMM0, opBits);
                emitIMMC(pp, CPUReg::XMM1, opBits);
                emitIMMD(pp, CPUReg::XMM2, opBits);
                pp.emitMulAdd(CPUReg::XMM0, CPUReg::XMM1, CPUReg::XMM2, opBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, opBits);
                break;
            }

            case ByteCodeOp::IntrinsicAtomicAddS8:
            case ByteCodeOp::IntrinsicAtomicAddS16:
            case ByteCodeOp::IntrinsicAtomicAddS32:
            case ByteCodeOp::IntrinsicAtomicAddS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, opBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
                emitBinOpEqLock(pp, CPUOp::ADD);
                break;

            case ByteCodeOp::IntrinsicAtomicAndS8:
            case ByteCodeOp::IntrinsicAtomicAndS16:
            case ByteCodeOp::IntrinsicAtomicAndS32:
            case ByteCodeOp::IntrinsicAtomicAndS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, opBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
                emitBinOpEqLock(pp, CPUOp::AND);
                break;

            case ByteCodeOp::IntrinsicAtomicOrS8:
            case ByteCodeOp::IntrinsicAtomicOrS16:
            case ByteCodeOp::IntrinsicAtomicOrS32:
            case ByteCodeOp::IntrinsicAtomicOrS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, opBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
                emitBinOpEqLock(pp, CPUOp::OR);
                break;

            case ByteCodeOp::IntrinsicAtomicXorS8:
            case ByteCodeOp::IntrinsicAtomicXorS16:
            case ByteCodeOp::IntrinsicAtomicXorS32:
            case ByteCodeOp::IntrinsicAtomicXorS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, opBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
                emitBinOpEqLock(pp, CPUOp::XOR);
                break;

            case ByteCodeOp::IntrinsicAtomicXchgS8:
            case ByteCodeOp::IntrinsicAtomicXchgS16:
            case ByteCodeOp::IntrinsicAtomicXchgS32:
            case ByteCodeOp::IntrinsicAtomicXchgS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RCX, 0, opBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
                emitBinOpEqLock(pp, CPUOp::XCHG);
                break;

            case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
                opBits = SCBE_CPU::getOpBits(ip->op);
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                emitIMMB(pp, CPUReg::RAX, opBits);
                emitIMMC(pp, CPUReg::RDX, opBits);
                pp.emitOpBinary(CPUReg::RCX, CPUReg::RDX, CPUOp::CMPXCHG, opBits);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUReg::RAX, opBits);
                break;

            case ByteCodeOp::IntrinsicS8x1:
            case ByteCodeOp::IntrinsicS16x1:
            case ByteCodeOp::IntrinsicS32x1:
            case ByteCodeOp::IntrinsicS64x1:
            {
                opBits = SCBE_CPU::getOpBits(ip->op);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        pp.emitLoad(CPUReg::RCX, CPUReg::RAX, opBits);
                        pp.emitOpBinary(CPUReg::RCX, SCBE_CPU::getNumBits(opBits) - 1, CPUOp::SAR, opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::XOR, opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::SUB, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RAX, CPUOp::POPCNT, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RAX, CPUOp::BSF, opBits);
                        pp.emitLoad(CPUReg::RCX, SCBE_CPU::getNumBits(opBits), opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RAX, CPUOp::BSR, opBits);
                        pp.emitLoad(CPUReg::RCX, (SCBE_CPU::getNumBits(opBits) * 2) - 1, opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVE, opBits);
                        pp.emitOpBinary(CPUReg::RAX, SCBE_CPU::getNumBits(opBits) - 1, CPUOp::XOR, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        pp.emitOpUnary(CPUReg::RAX, CPUOp::BSWAP, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
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
                opBits = SCBE_CPU::getOpBits(ip->op);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        emitIMMC(pp, CPUReg::RCX, opBits);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        emitIMMC(pp, CPUReg::RCX, opBits);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVL, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
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
                opBits = SCBE_CPU::getOpBits(ip->op);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        emitIMMC(pp, CPUReg::RCX, opBits);
                        pp.emitCmp(CPUReg::RCX, CPUReg::RAX, opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        emitIMMC(pp, CPUReg::RCX, opBits);
                        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, opBits);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::CMOVB, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                        break;
                    case TokenId::IntrinsicRol:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        emitIMMC(pp, CPUReg::RCX, OpBits::B8);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::ROL, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
                        break;
                    case TokenId::IntrinsicRor:
                        emitIMMB(pp, CPUReg::RAX, opBits);
                        emitIMMC(pp, CPUReg::RCX, OpBits::B8);
                        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::ROR, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, opBits);
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
                opBits          = SCBE_CPU::getOpBits(ip->op);
                const bool is32 = opBits == OpBits::F32;
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_B) ? CPUPushParamType::Constant : CPUPushParamType::SwagRegister, .value = ip->b.u64});
                pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_C) ? CPUPushParamType::Constant : CPUPushParamType::SwagRegister, .value = ip->c.u64});

                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        emitIMMB(pp, CPUReg::XMM0, opBits);
                        emitIMMC(pp, CPUReg::XMM1, opBits);
                        pp.emitOpBinary(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FMIN, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, opBits);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, CPUReg::XMM0, opBits);
                        emitIMMC(pp, CPUReg::XMM1, opBits);
                        pp.emitOpBinary(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FMAX, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, opBits);
                        break;

                    case TokenId::IntrinsicPow:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_powf : g_LangSpec->name_pow, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicATan2:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_atan2f : g_LangSpec->name_atan2, pp.pushParams, REG_OFFSET(ip->a.u32));
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
                opBits          = SCBE_CPU::getOpBits(ip->op);
                const bool is32 = opBits == OpBits::F32;
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = ip->hasFlag(BCI_IMM_B) ? CPUPushParamType::Constant : CPUPushParamType::SwagRegister, .value = ip->b.u64});

                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicSqrt:
                        emitIMMB(pp, CPUReg::XMM0, opBits);
                        pp.emitOpBinary(CPUReg::XMM0, CPUReg::XMM0, CPUOp::FSQRT, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, opBits);
                        break;
                    case TokenId::IntrinsicAbs:
                        emitIMMB(pp, CPUReg::XMM0, opBits);
                        pp.emitLoad(CPUReg::RAX, opBits == OpBits::F32 ? 0x7FFFFFFF : 0x7FFFFFFF'FFFFFFFF, OpBits::B64);
                        pp.emitLoad(CPUReg::XMM1, CPUReg::RAX, OpBits::F64);
                        pp.emitOpBinary(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FAND, opBits);
                        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::XMM0, opBits);
                        break;

                    case TokenId::IntrinsicSin:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_sinf : g_LangSpec->name_sin, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCos:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_cosf : g_LangSpec->name_cos, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTan:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_tanf : g_LangSpec->name_tan, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicSinh:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_sinhf : g_LangSpec->name_sinh, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCosh:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_coshf : g_LangSpec->name_cosh, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTanh:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_tanhf : g_LangSpec->name_tanh, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicASin:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_asinf : g_LangSpec->name_asin, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicACos:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_acosf : g_LangSpec->name_acos, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicATan:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_atanf : g_LangSpec->name_atan, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_logf : g_LangSpec->name_log, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog2:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_log2f : g_LangSpec->name_log2, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog10:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_log10f : g_LangSpec->name_log10, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicFloor:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_floorf : g_LangSpec->name_floor, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCeil:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_ceilf : g_LangSpec->name_ceil, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTrunc:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_truncf : g_LangSpec->name_trunc, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicRound:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_roundf : g_LangSpec->name_round, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicExp:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_expf : g_LangSpec->name_exp, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    case TokenId::IntrinsicExp2:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_exp2f : g_LangSpec->name_exp2, pp.pushParams, REG_OFFSET(ip->a.u32));
                        break;
                    default:
                        ok = false;
                        Report::internalError(buildParameters.module, form("unknown intrinsic [[%s]] during backend generation", ByteCode::opName(ip->op)));
                        break;
                }

                break;
            }

            case ByteCodeOp::InternalFailedAssume:
                emitInternalCallRAParams(pp, g_LangSpec->name_priv_failedAssume, {ip->a.u32});
                break;
            case ByteCodeOp::IntrinsicGetErr:
                SWAG_ASSERT(ip->b.u32 == ip->a.u32 + 1);
                emitInternalCallRAParams(pp, g_LangSpec->name_at_err, {}, REG_OFFSET(ip->a.u32));
                break;
            case ByteCodeOp::InternalSetErr:
                emitInternalCallRAParams(pp, g_LangSpec->name_priv_seterr, {ip->a.u32, ip->b.u32});
                break;
            case ByteCodeOp::InternalHasErr:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
                pp.emitLoad(CPUReg::RCX, CPUReg::RAX, offsetof(SwagContext, hasError), OpBits::B32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RCX, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfError:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitCmp(CPUReg::RAX, offsetof(SwagContext, hasError), 0, OpBits::B32);
                pp.emitJump(JNZ, pp.ipIndex + ip->b.s32 + 1);
                break;
            case ByteCodeOp::JumpIfNoError:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitCmp(CPUReg::RAX, offsetof(SwagContext, hasError), 0, OpBits::B32);
                pp.emitJump(JZ, pp.ipIndex + ip->b.s32 + 1);
                break;
            case ByteCodeOp::InternalPushErr:
                pp.emitCallLocal(g_LangSpec->name_priv_pusherr);
                break;
            case ByteCodeOp::InternalPopErr:
                pp.emitCallLocal(g_LangSpec->name_priv_poperr);
                break;
            case ByteCodeOp::InternalCatchErr:
                pp.emitCallLocal(g_LangSpec->name_priv_catcherr);
                break;
            case ByteCodeOp::InternalInitStackTrace:
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
                pp.emitStore(CPUReg::RAX, offsetof(SwagContext, traceIndex), 0, OpBits::B32);
                break;
            case ByteCodeOp::InternalStackTraceConst:
                pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symCSIndex, ip->b.u32);
                pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, OpBits::B64);
                emitInternalCallRAParams(pp, g_LangSpec->name_priv_stackTrace, {ip->a.u32});
                break;
            case ByteCodeOp::InternalStackTrace:
                emitInternalCallRAParams(pp, g_LangSpec->name_priv_stackTrace, {ip->a.u32});
                break;

            default:
                ok = false;
                Report::internalError(buildParameters.module, form("unknown instruction [[%s]] during backend generation", ByteCode::opName(ip->op)));
                break;
        }
    }

    pp.process();
    pp.encode(ppCPU);

    ppCPU.endFunction();
    return ok;
}
