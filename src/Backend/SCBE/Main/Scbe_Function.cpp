#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Report/Diagnostic.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool Scbe::emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc)
{
    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && bc->node->hasAttribute(ATTRIBUTE_TEST_FUNC) && buildParameters.compileType != Test)
        return true;

    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    auto&       ppCPU           = encoder<ScbeCpu>(ct, precompileIndex);
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

    // all registers:                   bc->maxReservedRegisterRC, contains all Swag bytecode registers
    // offsetRT:                        bc->maxCallResults, contains registers for result
    // offsetParamsAsRegisters:         To store all parameters passed as register in the current calling convention
    // offsetResult:                    sizeof(Register) to store return value
    // offsetStack:                     stackSize, function local stack
    // offsetFLT:                       sizeof(Register) to make some float computation
    // ...padding to 16... => total is sizeStack

    const uint32_t offsetRT                = bc->maxReservedRegisterRC * sizeof(Register);
    const uint32_t offsetParamsAsRegisters = offsetRT + bc->maxCallResults * sizeof(Register);
    const uint32_t offsetResult            = offsetParamsAsRegisters + cc.paramByRegisterInteger.size() * sizeof(Register);
    const uint32_t offsetByteCodeStack     = offsetResult + sizeof(Register);
    const uint32_t offsetFLT               = offsetByteCodeStack + bc->stackSize; // For float load (should be reserved only if we have floating point operations in that function)
    const uint32_t sizeStack               = offsetFLT + 8;

    // Register function
    ppCPU.cpuFct = ppCPU.addFunction(funcName, &cc, bc);
    ppCPU.cc     = &cc;
    if (debug)
        ScbeDebug::setLocation(ppCPU.cpuFct, nullptr, 0);

    ppCPU.cpuFct->offsetFLT               = offsetFLT;
    ppCPU.cpuFct->offsetRT                = offsetRT;
    ppCPU.cpuFct->offsetResult            = offsetResult;
    ppCPU.cpuFct->offsetByteCodeStack     = offsetByteCodeStack;
    ppCPU.cpuFct->offsetParamsAsRegisters = offsetParamsAsRegisters;

    ScbeMicro pp;
    pp.init(buildParameters);
    pp.cpuFct               = ppCPU.cpuFct;
    pp.cc                   = ppCPU.cpuFct->cc;
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

    pp.emitEnter(sizeStack);

    // Save register parameters
    uint32_t idxReg = 0;
    while (idxReg < std::min(cc.paramByRegisterInteger.size(), typeFunc->numParamsRegisters()))
    {
        const auto typeParam = typeFunc->registerIdxToType(idxReg);
        if (cc.useRegisterFloat && typeParam->isNativeFloat())
            pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetParam(idxReg), cc.paramByRegisterFloat[idxReg], OpBits::B64);
        else
            pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetParam(idxReg), cc.paramByRegisterInteger[idxReg], OpBits::B64);
        idxReg++;
    }

    // Save pointer to return value if this is a return by copy
    if (idxReg < cc.paramByRegisterInteger.size() && typeFunc->returnByAddress())
    {
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetParam(idxReg), cc.paramByRegisterInteger[idxReg], OpBits::B64);
        idxReg++;
    }

    // Save C variadic on the caller stack, to be sure that everything is stored there, even if it's
    // passed by registers
    if (idxReg < cc.paramByRegisterInteger.size() && typeFunc->isFctCVariadic())
    {
        while (idxReg < cc.paramByRegisterInteger.size())
        {
            pp.emitStoreCallerParam(idxReg, cc.paramByRegisterInteger[idxReg], OpBits::B64);
            idxReg++;
        }
    }

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
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitOpBinaryRegImm(cc.computeRegI1, ip->c.u8, CpuOp::ADD, OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::MUL, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

            case ByteCodeOp::Add32byVB32:
            case ByteCodeOp::Add64byVB64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitOpBinaryMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), ip->b.u64, CpuOp::ADD, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearRA:
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B64);
                break;
            case ByteCodeOp::ClearRAx2:
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B64);
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), 0, OpBits::B64);
                break;
            case ByteCodeOp::ClearRAx3:
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B64);
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), 0, OpBits::B64);
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), 0, OpBits::B64);
                break;
            case ByteCodeOp::ClearRAx4:
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B64);
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), 0, OpBits::B64);
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), 0, OpBits::B64);
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->d.u32), 0, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyRBtoRA8:
            case ByteCodeOp::CopyRBtoRA16:
            case ByteCodeOp::CopyRBtoRA32:
            case ByteCodeOp::CopyRBtoRA64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

            case ByteCodeOp::CopyRBtoRA64x2:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->d.u32), OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CastBool8:
            case ByteCodeOp::CastBool16:
            case ByteCodeOp::CastBool32:
            case ByteCodeOp::CastBool64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), 0, opBits);
                pp.emitSetCond(cc.computeRegI0, CpuCondFlag::NE);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B8);
                break;

            case ByteCodeOp::CastS8S16:
                pp.emitLoadSignedExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B16, OpBits::B8);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B16);
                break;

            case ByteCodeOp::CastS8S32:
                pp.emitLoadSignedExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32, OpBits::B8);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B32);
                break;
            case ByteCodeOp::CastS16S32:
                pp.emitLoadSignedExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32, OpBits::B16);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B32);
                break;

            case ByteCodeOp::CastS8S64:
                pp.emitLoadSignedExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64, OpBits::B8);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;
            case ByteCodeOp::CastS16S64:
                pp.emitLoadSignedExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64, OpBits::B16);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;
            case ByteCodeOp::CastS32S64:
                pp.emitLoadSignedExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

            case ByteCodeOp::CastF32S32:
                pp.emitLoadRegMem(cc.computeRegF0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTF2I, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B32);
                break;
            case ByteCodeOp::CastF64S64:
                pp.emitLoadRegMem(cc.computeRegF0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTF2I, OpBits::B64, EMITF_B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

            case ByteCodeOp::CastS8F32:
                pp.emitLoadSignedExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32, OpBits::B8);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B32);
                break;
            case ByteCodeOp::CastS16F32:
                pp.emitLoadSignedExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32, OpBits::B16);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B32);
                break;
            case ByteCodeOp::CastS32F32:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B32);
                break;
            case ByteCodeOp::CastS64F32:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B32, EMITF_B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B32);
                break;
            case ByteCodeOp::CastU8F32:
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32, OpBits::B8);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B32);
                break;
            case ByteCodeOp::CastU16F32:
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32, OpBits::B16);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B32);
                break;
            case ByteCodeOp::CastU32F32:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B32, EMITF_B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B32);
                break;
            case ByteCodeOp::CastU64F32:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B32, EMITF_B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B32);
                break;
            case ByteCodeOp::CastF64F32:
                pp.emitLoadRegMem(cc.computeRegF0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTF2F, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B32);
                break;

            case ByteCodeOp::CastS8F64:
                pp.emitLoadSignedExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32, OpBits::B8);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B64);
                break;
            case ByteCodeOp::CastS16F64:
                pp.emitLoadSignedExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32, OpBits::B16);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B64);
                break;
            case ByteCodeOp::CastS32F64:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B64);
                break;
            case ByteCodeOp::CastS64F64:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B64, EMITF_B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B64);
                break;
            case ByteCodeOp::CastU8F64:
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32, OpBits::B8);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B64);
                break;
            case ByteCodeOp::CastU16F64:
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32, OpBits::B16);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B64);
                break;
            case ByteCodeOp::CastU32F64:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTI2F, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B64);
                break;
            case ByteCodeOp::CastU64F64:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitLoadZeroExtendRegReg(cc.computeRegF0, cc.computeRegI0, OpBits::B64, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B64);
                break;

            case ByteCodeOp::CastF32F64:
                pp.emitLoadRegMem(cc.computeRegF0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegI0, CpuOp::CVTF2F, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, OpBits::B64);
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
                emitShiftLogical(pp, CpuOp::SHL);
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
                emitShiftLogical(pp, CpuOp::SHR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpXor8:
            case ByteCodeOp::BinOpXor16:
            case ByteCodeOp::BinOpXor32:
            case ByteCodeOp::BinOpXor64:
                emitBinOp(pp, CpuOp::XOR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpMulS8:
            case ByteCodeOp::BinOpMulS16:
            case ByteCodeOp::BinOpMulS32:
            case ByteCodeOp::BinOpMulS64:
                emitBinOpOverflow(pp, CpuOp::IMUL, SafetyMsg::Mul, ScbeCpu::getOpType(ip->op));
                break;
            case ByteCodeOp::BinOpMulU8:
            case ByteCodeOp::BinOpMulU16:
            case ByteCodeOp::BinOpMulU32:
            case ByteCodeOp::BinOpMulU64:
                emitBinOpOverflow(pp, CpuOp::MUL, SafetyMsg::Mul, ScbeCpu::getOpType(ip->op));
                break;
            case ByteCodeOp::BinOpMulF32_Safe:
            case ByteCodeOp::BinOpMulF64_Safe:
                emitBinOp(pp, CpuOp::FMUL);
                break;

            case ByteCodeOp::BinOpMulS8_Safe:
            case ByteCodeOp::BinOpMulS16_Safe:
            case ByteCodeOp::BinOpMulS32_Safe:
            case ByteCodeOp::BinOpMulS64_Safe:
                emitBinOp(pp, CpuOp::IMUL);
                break;
            case ByteCodeOp::BinOpMulU8_Safe:
            case ByteCodeOp::BinOpMulU16_Safe:
            case ByteCodeOp::BinOpMulU32_Safe:
            case ByteCodeOp::BinOpMulU64_Safe:
                emitBinOp(pp, CpuOp::MUL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpModuloS8:
            case ByteCodeOp::BinOpModuloS16:
            case ByteCodeOp::BinOpModuloS32:
            case ByteCodeOp::BinOpModuloS64:
                emitBinOp(pp, CpuOp::IMOD);
                break;
            case ByteCodeOp::BinOpModuloU8:
            case ByteCodeOp::BinOpModuloU16:
            case ByteCodeOp::BinOpModuloU32:
            case ByteCodeOp::BinOpModuloU64:
                emitBinOp(pp, CpuOp::MOD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpDivS8:
            case ByteCodeOp::BinOpDivS16:
            case ByteCodeOp::BinOpDivS32:
            case ByteCodeOp::BinOpDivS64:
                emitBinOp(pp, CpuOp::IDIV);
                break;
            case ByteCodeOp::BinOpDivU8:
            case ByteCodeOp::BinOpDivU16:
            case ByteCodeOp::BinOpDivU32:
            case ByteCodeOp::BinOpDivU64:
                emitBinOp(pp, CpuOp::DIV);
                break;
            case ByteCodeOp::BinOpDivF32:
            case ByteCodeOp::BinOpDivF64:
                emitBinOp(pp, CpuOp::FDIV);
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
                emitBinOpOverflow(pp, CpuOp::ADD, SafetyMsg::Plus, ScbeCpu::getOpType(ip->op));
                break;
            case ByteCodeOp::BinOpPlusF32_Safe:
            case ByteCodeOp::BinOpPlusF64_Safe:
                emitBinOp(pp, CpuOp::FADD);
                break;

            case ByteCodeOp::BinOpPlusS8_Safe:
            case ByteCodeOp::BinOpPlusU8_Safe:
            case ByteCodeOp::BinOpPlusS16_Safe:
            case ByteCodeOp::BinOpPlusU16_Safe:
            case ByteCodeOp::BinOpPlusS32_Safe:
            case ByteCodeOp::BinOpPlusU32_Safe:
            case ByteCodeOp::BinOpPlusS64_Safe:
            case ByteCodeOp::BinOpPlusU64_Safe:
                emitBinOp(pp, CpuOp::ADD);
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
                emitBinOpOverflow(pp, CpuOp::SUB, SafetyMsg::Minus, ScbeCpu::getOpType(ip->op));
                break;

            case ByteCodeOp::BinOpMinusS8_Safe:
            case ByteCodeOp::BinOpMinusU8_Safe:
            case ByteCodeOp::BinOpMinusS16_Safe:
            case ByteCodeOp::BinOpMinusU16_Safe:
            case ByteCodeOp::BinOpMinusS32_Safe:
            case ByteCodeOp::BinOpMinusU32_Safe:
            case ByteCodeOp::BinOpMinusS64_Safe:
            case ByteCodeOp::BinOpMinusU64_Safe:
                emitBinOp(pp, CpuOp::SUB);
                break;
            case ByteCodeOp::BinOpMinusF32_Safe:
            case ByteCodeOp::BinOpMinusF64_Safe:
                emitBinOp(pp, CpuOp::FSUB);
                break;
                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskAnd8:
            case ByteCodeOp::BinOpBitmaskAnd16:
            case ByteCodeOp::BinOpBitmaskAnd32:
            case ByteCodeOp::BinOpBitmaskAnd64:
                emitBinOp(pp, CpuOp::AND);
                break;

                /////////////////////////////////////

            case ByteCodeOp::BinOpBitmaskOr8:
            case ByteCodeOp::BinOpBitmaskOr16:
            case ByteCodeOp::BinOpBitmaskOr32:
            case ByteCodeOp::BinOpBitmaskOr64:
                emitBinOp(pp, CpuOp::OR);
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
                emitShiftEqLogical(pp, CpuOp::SHL);
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
                emitShiftEqLogical(pp, CpuOp::SHR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpXorEq8:
            case ByteCodeOp::AffectOpXorEq16:
            case ByteCodeOp::AffectOpXorEq32:
            case ByteCodeOp::AffectOpXorEq64:
                emitBinOpEq(pp, 0, CpuOp::XOR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpOrEq8:
            case ByteCodeOp::AffectOpOrEq16:
            case ByteCodeOp::AffectOpOrEq32:
            case ByteCodeOp::AffectOpOrEq64:
                emitBinOpEq(pp, 0, CpuOp::OR);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpAndEq8:
            case ByteCodeOp::AffectOpAndEq16:
            case ByteCodeOp::AffectOpAndEq32:
            case ByteCodeOp::AffectOpAndEq64:
                emitBinOpEq(pp, 0, CpuOp::AND);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpMulEqS8:
            case ByteCodeOp::AffectOpMulEqS16:
            case ByteCodeOp::AffectOpMulEqS32:
            case ByteCodeOp::AffectOpMulEqS64:
                emitBinOpEqOverflow(pp, CpuOp::IMUL, SafetyMsg::MulEq, ScbeCpu::getOpType(ip->op));
                break;
            case ByteCodeOp::AffectOpMulEqU8:
            case ByteCodeOp::AffectOpMulEqU16:
            case ByteCodeOp::AffectOpMulEqU32:
            case ByteCodeOp::AffectOpMulEqU64:
                emitBinOpEqOverflow(pp, CpuOp::MUL, SafetyMsg::MulEq, ScbeCpu::getOpType(ip->op));
                break;

            case ByteCodeOp::AffectOpMulEqS8_Safe:
            case ByteCodeOp::AffectOpMulEqS16_Safe:
            case ByteCodeOp::AffectOpMulEqS32_Safe:
            case ByteCodeOp::AffectOpMulEqS64_Safe:
                emitBinOpEq(pp, ip->c.u32, CpuOp::IMUL);
                break;
            case ByteCodeOp::AffectOpMulEqU8_Safe:
            case ByteCodeOp::AffectOpMulEqU16_Safe:
            case ByteCodeOp::AffectOpMulEqU32_Safe:
            case ByteCodeOp::AffectOpMulEqU64_Safe:
                emitBinOpEq(pp, ip->c.u32, CpuOp::MUL);
                break;
            case ByteCodeOp::AffectOpMulEqF32_Safe:
            case ByteCodeOp::AffectOpMulEqF64_Safe:
                emitBinOpEq(pp, ip->c.u32, CpuOp::FMUL);
                break;

            case ByteCodeOp::AffectOpMulEqS8_SSafe:
            case ByteCodeOp::AffectOpMulEqS16_SSafe:
            case ByteCodeOp::AffectOpMulEqS32_SSafe:
            case ByteCodeOp::AffectOpMulEqS64_SSafe:
                emitBinOpEqS(pp, CpuOp::IMUL);
                break;
            case ByteCodeOp::AffectOpMulEqU8_SSafe:
            case ByteCodeOp::AffectOpMulEqU16_SSafe:
            case ByteCodeOp::AffectOpMulEqU32_SSafe:
            case ByteCodeOp::AffectOpMulEqU64_SSafe:
                emitBinOpEqS(pp, CpuOp::MUL);
                break;
            case ByteCodeOp::AffectOpMulEqF32_SSafe:
            case ByteCodeOp::AffectOpMulEqF64_SSafe:
                emitBinOpEqS(pp, CpuOp::FMUL);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpDivEqS8:
            case ByteCodeOp::AffectOpDivEqS16:
            case ByteCodeOp::AffectOpDivEqS32:
            case ByteCodeOp::AffectOpDivEqS64:
                emitBinOpEq(pp, 0, CpuOp::IDIV);
                break;
            case ByteCodeOp::AffectOpDivEqU8:
            case ByteCodeOp::AffectOpDivEqU16:
            case ByteCodeOp::AffectOpDivEqU32:
            case ByteCodeOp::AffectOpDivEqU64:
                emitBinOpEq(pp, 0, CpuOp::DIV);
                break;
            case ByteCodeOp::AffectOpDivEqF32:
            case ByteCodeOp::AffectOpDivEqF64:
                emitBinOpEq(pp, 0, CpuOp::FDIV);
                break;

            case ByteCodeOp::AffectOpDivEqS8_SSafe:
            case ByteCodeOp::AffectOpDivEqS16_SSafe:
            case ByteCodeOp::AffectOpDivEqS32_SSafe:
            case ByteCodeOp::AffectOpDivEqS64_SSafe:
                emitBinOpEqS(pp, CpuOp::IDIV);
                break;
            case ByteCodeOp::AffectOpDivEqU8_SSafe:
            case ByteCodeOp::AffectOpDivEqU16_SSafe:
            case ByteCodeOp::AffectOpDivEqU32_SSafe:
            case ByteCodeOp::AffectOpDivEqU64_SSafe:
                emitBinOpEqS(pp, CpuOp::DIV);
                break;
            case ByteCodeOp::AffectOpDivEqF32_SSafe:
            case ByteCodeOp::AffectOpDivEqF64_SSafe:
                emitBinOpEqS(pp, CpuOp::FDIV);
                break;

                /////////////////////////////////////

            case ByteCodeOp::AffectOpModuloEqS8:
            case ByteCodeOp::AffectOpModuloEqS16:
            case ByteCodeOp::AffectOpModuloEqS32:
            case ByteCodeOp::AffectOpModuloEqS64:
                emitBinOpEq(pp, 0, CpuOp::IMOD);
                break;
            case ByteCodeOp::AffectOpModuloEqU8:
            case ByteCodeOp::AffectOpModuloEqU16:
            case ByteCodeOp::AffectOpModuloEqU32:
            case ByteCodeOp::AffectOpModuloEqU64:
                emitBinOpEq(pp, 0, CpuOp::MOD);
                break;

            case ByteCodeOp::AffectOpModuloEqS8_SSafe:
            case ByteCodeOp::AffectOpModuloEqS16_SSafe:
            case ByteCodeOp::AffectOpModuloEqS32_SSafe:
            case ByteCodeOp::AffectOpModuloEqS64_SSafe:
                emitBinOpEqS(pp, CpuOp::IMOD);
                break;
            case ByteCodeOp::AffectOpModuloEqU8_SSafe:
            case ByteCodeOp::AffectOpModuloEqU16_SSafe:
            case ByteCodeOp::AffectOpModuloEqU32_SSafe:
            case ByteCodeOp::AffectOpModuloEqU64_SSafe:
                emitBinOpEqS(pp, CpuOp::MOD);
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
                emitBinOpEqOverflow(pp, CpuOp::ADD, SafetyMsg::PlusEq, ScbeCpu::getOpType(ip->op));
                break;
            case ByteCodeOp::AffectOpPlusEqF32_Safe:
            case ByteCodeOp::AffectOpPlusEqF64_Safe:
                emitBinOpEq(pp, ip->c.u32, CpuOp::FADD);
                break;

            case ByteCodeOp::AffectOpPlusEqS8_Safe:
            case ByteCodeOp::AffectOpPlusEqS16_Safe:
            case ByteCodeOp::AffectOpPlusEqS32_Safe:
            case ByteCodeOp::AffectOpPlusEqS64_Safe:
            case ByteCodeOp::AffectOpPlusEqU8_Safe:
            case ByteCodeOp::AffectOpPlusEqU16_Safe:
            case ByteCodeOp::AffectOpPlusEqU32_Safe:
            case ByteCodeOp::AffectOpPlusEqU64_Safe:
                emitBinOpEq(pp, ip->c.u32, CpuOp::ADD);
                break;

            case ByteCodeOp::AffectOpPlusEqS8_SSafe:
            case ByteCodeOp::AffectOpPlusEqS16_SSafe:
            case ByteCodeOp::AffectOpPlusEqS32_SSafe:
            case ByteCodeOp::AffectOpPlusEqS64_SSafe:
            case ByteCodeOp::AffectOpPlusEqU8_SSafe:
            case ByteCodeOp::AffectOpPlusEqU16_SSafe:
            case ByteCodeOp::AffectOpPlusEqU32_SSafe:
            case ByteCodeOp::AffectOpPlusEqU64_SSafe:
                emitBinOpEqS(pp, CpuOp::ADD);
                break;
            case ByteCodeOp::AffectOpPlusEqF32_SSafe:
            case ByteCodeOp::AffectOpPlusEqF64_SSafe:
                emitBinOpEqS(pp, CpuOp::FADD);
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
                emitBinOpEqOverflow(pp, CpuOp::SUB, SafetyMsg::MinusEq, ScbeCpu::getOpType(ip->op));
                break;
            case ByteCodeOp::AffectOpMinusEqF32_Safe:
            case ByteCodeOp::AffectOpMinusEqF64_Safe:
                emitBinOpEq(pp, ip->c.u32, CpuOp::FSUB);
                break;

            case ByteCodeOp::AffectOpMinusEqS8_Safe:
            case ByteCodeOp::AffectOpMinusEqS16_Safe:
            case ByteCodeOp::AffectOpMinusEqS32_Safe:
            case ByteCodeOp::AffectOpMinusEqS64_Safe:
            case ByteCodeOp::AffectOpMinusEqU8_Safe:
            case ByteCodeOp::AffectOpMinusEqU16_Safe:
            case ByteCodeOp::AffectOpMinusEqU32_Safe:
            case ByteCodeOp::AffectOpMinusEqU64_Safe:
                emitBinOpEq(pp, ip->c.u32, CpuOp::SUB);
                break;

            case ByteCodeOp::AffectOpMinusEqS8_SSafe:
            case ByteCodeOp::AffectOpMinusEqS16_SSafe:
            case ByteCodeOp::AffectOpMinusEqS32_SSafe:
            case ByteCodeOp::AffectOpMinusEqS64_SSafe:
            case ByteCodeOp::AffectOpMinusEqU8_SSafe:
            case ByteCodeOp::AffectOpMinusEqU16_SSafe:
            case ByteCodeOp::AffectOpMinusEqU32_SSafe:
            case ByteCodeOp::AffectOpMinusEqU64_SSafe:
                emitBinOpEqS(pp, CpuOp::SUB);
                break;
            case ByteCodeOp::AffectOpMinusEqF32_SSafe:
            case ByteCodeOp::AffectOpMinusEqF64_SSafe:
                emitBinOpEqS(pp, CpuOp::FSUB);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ZeroToTrue:
                pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B32);
                pp.emitSetCond(cc.computeRegI0, CpuCondFlag::E);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B8);
                break;
            case ByteCodeOp::LowerZeroToTrue:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B32);
                pp.emitOpBinaryRegImm(cc.computeRegI0, 31, CpuOp::SHR, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B8);
                break;
            case ByteCodeOp::LowerEqZeroToTrue:
                pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B32);
                pp.emitSetCond(cc.computeRegI0, CpuCondFlag::LE);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B8);
                break;
            case ByteCodeOp::GreaterZeroToTrue:
                pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B32);
                pp.emitSetCond(cc.computeRegI0, CpuCondFlag::G);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B8);
                break;
            case ByteCodeOp::GreaterEqZeroToTrue:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B32);
                pp.emitOpUnaryReg(cc.computeRegI0, CpuOp::NOT, OpBits::B32);
                pp.emitOpBinaryRegImm(cc.computeRegI0, 31, CpuOp::SHR, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpGreaterS8:
            case ByteCodeOp::CompareOpGreaterS16:
            case ByteCodeOp::CompareOpGreaterS32:
            case ByteCodeOp::CompareOpGreaterS64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::G);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;
            case ByteCodeOp::CompareOpGreaterU8:
            case ByteCodeOp::CompareOpGreaterU16:
            case ByteCodeOp::CompareOpGreaterU32:
            case ByteCodeOp::CompareOpGreaterU64:
            case ByteCodeOp::CompareOpGreaterF32:
            case ByteCodeOp::CompareOpGreaterF64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::A);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;

            case ByteCodeOp::CompareOpGreaterEqS8:
            case ByteCodeOp::CompareOpGreaterEqS16:
            case ByteCodeOp::CompareOpGreaterEqS32:
            case ByteCodeOp::CompareOpGreaterEqS64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::GE);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;

            case ByteCodeOp::CompareOpGreaterEqU8:
            case ByteCodeOp::CompareOpGreaterEqU16:
            case ByteCodeOp::CompareOpGreaterEqU32:
            case ByteCodeOp::CompareOpGreaterEqU64:
            case ByteCodeOp::CompareOpGreaterEqF32:
            case ByteCodeOp::CompareOpGreaterEqF64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::AE);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpLowerS8:
            case ByteCodeOp::CompareOpLowerS16:
            case ByteCodeOp::CompareOpLowerS32:
            case ByteCodeOp::CompareOpLowerS64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::L);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;

            case ByteCodeOp::CompareOpLowerU8:
            case ByteCodeOp::CompareOpLowerU16:
            case ByteCodeOp::CompareOpLowerU32:
            case ByteCodeOp::CompareOpLowerU64:
            case ByteCodeOp::CompareOpLowerF32:
            case ByteCodeOp::CompareOpLowerF64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::B);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;

            case ByteCodeOp::CompareOpLowerEqS8:
            case ByteCodeOp::CompareOpLowerEqS16:
            case ByteCodeOp::CompareOpLowerEqS32:
            case ByteCodeOp::CompareOpLowerEqS64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::LE);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;

            case ByteCodeOp::CompareOpLowerEqU8:
            case ByteCodeOp::CompareOpLowerEqU16:
            case ByteCodeOp::CompareOpLowerEqU32:
            case ByteCodeOp::CompareOpLowerEqU64:
            case ByteCodeOp::CompareOpLowerEqF32:
            case ByteCodeOp::CompareOpLowerEqF64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::BE);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOp3Way8:
            case ByteCodeOp::CompareOp3Way16:
            case ByteCodeOp::CompareOp3Way32:
            case ByteCodeOp::CompareOp3Way64:
                pp.emitClearReg(cc.computeRegI2, OpBits::B32);
                emitCompareOp(pp, cc.computeRegI2, CpuCondFlag::G);
                pp.emitLoadRegImm(cc.computeRegI0, 0xFFFFFFFF, OpBits::B32);
                pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI2, CpuOp::CMOVGE, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B32);
                break;

            case ByteCodeOp::CompareOp3WayF32:
            case ByteCodeOp::CompareOp3WayF64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitClearReg(cc.computeRegI2, OpBits::B32);
                emitIMMA(pp, cc.computeRegF0, opBits);
                emitIMMB(pp, cc.computeRegF1, opBits);
                pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegF1, CpuOp::UCOMIF, opBits);
                pp.emitSetCond(cc.computeRegI2, CpuCondFlag::A);
                pp.emitOpBinaryRegReg(cc.computeRegF1, cc.computeRegF0, CpuOp::UCOMIF, opBits);
                pp.emitLoadRegImm(cc.computeRegI0, 0xFFFFFFFF, OpBits::B32);
                pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI2, CpuOp::CMOVBE, OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B32);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpEqual8:
            case ByteCodeOp::CompareOpEqual16:
            case ByteCodeOp::CompareOpEqual32:
            case ByteCodeOp::CompareOpEqual64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::E);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;
            case ByteCodeOp::CompareOpEqualF32:
            case ByteCodeOp::CompareOpEqualF64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::EP);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CompareOpNotEqual8:
            case ByteCodeOp::CompareOpNotEqual16:
            case ByteCodeOp::CompareOpNotEqual32:
            case ByteCodeOp::CompareOpNotEqual64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::NE);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;
            case ByteCodeOp::CompareOpNotEqualF32:
            case ByteCodeOp::CompareOpNotEqualF64:
                emitCompareOp(pp, cc.computeRegI0, CpuCondFlag::NEP);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicDbgAlloc:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_dbgalloc, {}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicSysAlloc:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_sysalloc, {}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicRtFlags:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_rtflags, {}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicStringCmp:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_strcmp, {ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->d.u32));
                break;
            case ByteCodeOp::IntrinsicTypeCmp:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_typecmp, {ip->a.u32, ip->b.u32, ip->c.u32}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->d.u32));
                break;
            case ByteCodeOp::IntrinsicIs:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_is, {ip->a.u32, ip->b.u32}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32));
                break;
            case ByteCodeOp::IntrinsicAs:
                emitInternalCallRAParams(pp, g_LangSpec->name_at_as, {ip->a.u32, ip->b.u32, ip->c.u32}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->d.u32));
                break;

                /////////////////////////////////////

            case ByteCodeOp::NegBool:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B8);
                pp.emitOpBinaryRegImm(cc.computeRegI0, 1, CpuOp::XOR, OpBits::B8);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

            case ByteCodeOp::NegS32:
            case ByteCodeOp::NegS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                if (ip->a.u32 == ip->b.u32)
                    pp.emitOpUnaryMem(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), CpuOp::NEG, opBits);
                else
                {
                    pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), opBits);
                    pp.emitOpUnaryReg(cc.computeRegI0, CpuOp::NEG, opBits);
                    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
                }
                break;

            case ByteCodeOp::NegF32:
            case ByteCodeOp::NegF64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegF0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), opBits);
                pp.emitOpUnaryReg(cc.computeRegF0, CpuOp::NEG, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::InvertU8:
            case ByteCodeOp::InvertU16:
            case ByteCodeOp::InvertU32:
            case ByteCodeOp::InvertU64:
                opBits = ScbeCpu::getOpBits(ip->op);
                if (ip->a.u32 == ip->b.u32)
                    pp.emitOpUnaryMem(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), CpuOp::NOT, opBits);
                else
                {
                    pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), opBits);
                    pp.emitOpUnaryReg(cc.computeRegI0, CpuOp::NOT, opBits);
                    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
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
                pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B8);
                pp.emitJumpCondImm(CpuCondJump::JNZ, pp.ipIndex + ip->b.s32 + 1);
                break;
            case ByteCodeOp::JumpIfFalse:
                pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B8);
                pp.emitJumpCondImm(CpuCondJump::JZ, pp.ipIndex + ip->b.s32 + 1);
                break;
            case ByteCodeOp::JumpIfRTTrue:
                pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(0), 0, OpBits::B8);
                pp.emitJumpCondImm(CpuCondJump::JNZ, pp.ipIndex + ip->b.s32 + 1);
                break;
            case ByteCodeOp::JumpIfRTFalse:
                pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(0), 0, OpBits::B8);
                pp.emitJumpCondImm(CpuCondJump::JZ, pp.ipIndex + ip->b.s32 + 1);
                break;

            case ByteCodeOp::JumpIfNotZero8:
            case ByteCodeOp::JumpIfNotZero16:
            case ByteCodeOp::JumpIfNotZero32:
            case ByteCodeOp::JumpIfNotZero64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, opBits);
                pp.emitJumpCondImm(CpuCondJump::JNZ, pp.ipIndex + ip->b.s32 + 1);
                break;

            case ByteCodeOp::JumpIfZero8:
            case ByteCodeOp::JumpIfZero16:
            case ByteCodeOp::JumpIfZero32:
            case ByteCodeOp::JumpIfZero64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, opBits);
                pp.emitJumpCondImm(CpuCondJump::JZ, pp.ipIndex + ip->b.s32 + 1);
                break;

            case ByteCodeOp::Jump:
                pp.emitJumpCondImm(CpuCondJump::JUMP, pp.ipIndex + ip->b.s32 + 1);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfNotEqual8:
                emitJumpCmp(pp, CpuCondJump::JNZ, OpBits::B8);
                break;
            case ByteCodeOp::JumpIfNotEqual16:
                emitJumpCmp(pp, CpuCondJump::JNZ, OpBits::B16);
                break;
            case ByteCodeOp::JumpIfNotEqual32:
                emitJumpCmp(pp, CpuCondJump::JNZ, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfNotEqual64:
                emitJumpCmp(pp, CpuCondJump::JNZ, OpBits::B64);
                break;
            case ByteCodeOp::JumpIfNotEqualF32:
                emitJumpCmp2(pp, CpuCondJump::JP, CpuCondJump::JNZ, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfNotEqualF64:
                emitJumpCmp2(pp, CpuCondJump::JP, CpuCondJump::JNZ, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfEqual8:
                emitJumpCmp(pp, CpuCondJump::JZ, OpBits::B8);
                break;
            case ByteCodeOp::JumpIfEqual16:
                emitJumpCmp(pp, CpuCondJump::JZ, OpBits::B16);
                break;
            case ByteCodeOp::JumpIfEqual32:
                emitJumpCmp(pp, CpuCondJump::JZ, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfEqual64:
                emitJumpCmp(pp, CpuCondJump::JZ, OpBits::B64);
                break;
            case ByteCodeOp::JumpIfEqualF32:
                emitJumpCmp2(pp, CpuCondJump::JP, CpuCondJump::JZ, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfEqualF64:
                emitJumpCmp2(pp, CpuCondJump::JP, CpuCondJump::JZ, OpBits::B64);
                break;
            case ByteCodeOp::IncJumpIfEqual64:
                pp.emitOpBinaryMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 1, CpuOp::ADD, OpBits::B64);
                emitJumpCmp(pp, CpuCondJump::JZ, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfStackEqual8:
                emitJumpCmpAddr(pp, CpuCondJump::JZ, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, OpBits::B8);
                break;
            case ByteCodeOp::JumpIfStackNotEqual8:
                emitJumpCmpAddr(pp, CpuCondJump::JNZ, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, OpBits::B8);
                break;

            case ByteCodeOp::JumpIfStackEqual16:
                emitJumpCmpAddr(pp, CpuCondJump::JZ, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, OpBits::B16);
                break;
            case ByteCodeOp::JumpIfStackNotEqual16:
                emitJumpCmpAddr(pp, CpuCondJump::JNZ, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, OpBits::B16);
                break;

            case ByteCodeOp::JumpIfStackEqual32:
                emitJumpCmpAddr(pp, CpuCondJump::JZ, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfStackNotEqual32:
                emitJumpCmpAddr(pp, CpuCondJump::JNZ, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, OpBits::B32);
                break;

            case ByteCodeOp::JumpIfStackEqual64:
                emitJumpCmpAddr(pp, CpuCondJump::JZ, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, OpBits::B64);
                break;
            case ByteCodeOp::JumpIfStackNotEqual64:
                emitJumpCmpAddr(pp, CpuCondJump::JNZ, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, OpBits::B64);
                break;

            case ByteCodeOp::JumpIfDeRefEqual8:
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, CpuCondJump::JZ, cc.computeRegI1, ip->d.u32, OpBits::B8);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual8:
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, CpuCondJump::JNZ, cc.computeRegI1, ip->d.u32, OpBits::B8);
                break;

            case ByteCodeOp::JumpIfDeRefEqual16:
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, CpuCondJump::JZ, cc.computeRegI1, ip->d.u32, OpBits::B16);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual16:
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, CpuCondJump::JNZ, cc.computeRegI1, ip->d.u32, OpBits::B16);
                break;

            case ByteCodeOp::JumpIfDeRefEqual32:
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, CpuCondJump::JZ, cc.computeRegI1, ip->d.u32, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual32:
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, CpuCondJump::JNZ, cc.computeRegI1, ip->d.u32, OpBits::B32);
                break;

            case ByteCodeOp::JumpIfDeRefEqual64:
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, CpuCondJump::JZ, cc.computeRegI1, ip->d.u32, OpBits::B64);
                break;
            case ByteCodeOp::JumpIfDeRefNotEqual64:
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                emitJumpCmpAddr(pp, CpuCondJump::JNZ, cc.computeRegI1, ip->d.u32, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfLowerS8:
            case ByteCodeOp::JumpIfLowerS16:
            case ByteCodeOp::JumpIfLowerS32:
            case ByteCodeOp::JumpIfLowerS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                emitJumpCmp(pp, CpuCondJump::JL, opBits);
                break;
            case ByteCodeOp::JumpIfLowerU8:
            case ByteCodeOp::JumpIfLowerU16:
            case ByteCodeOp::JumpIfLowerU32:
            case ByteCodeOp::JumpIfLowerU64:
            case ByteCodeOp::JumpIfLowerF32:
            case ByteCodeOp::JumpIfLowerF64:
                opBits = ScbeCpu::getOpBits(ip->op);
                emitJumpCmp(pp, CpuCondJump::JB, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfLowerEqS8:
            case ByteCodeOp::JumpIfLowerEqS16:
            case ByteCodeOp::JumpIfLowerEqS32:
            case ByteCodeOp::JumpIfLowerEqS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                emitJumpCmp(pp, CpuCondJump::JLE, opBits);
                break;

            case ByteCodeOp::JumpIfLowerEqU8:
            case ByteCodeOp::JumpIfLowerEqU16:
            case ByteCodeOp::JumpIfLowerEqU32:
            case ByteCodeOp::JumpIfLowerEqU64:
            case ByteCodeOp::JumpIfLowerEqF32:
            case ByteCodeOp::JumpIfLowerEqF64:
                opBits = ScbeCpu::getOpBits(ip->op);
                emitJumpCmp(pp, CpuCondJump::JBE, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfGreaterS8:
            case ByteCodeOp::JumpIfGreaterS16:
            case ByteCodeOp::JumpIfGreaterS32:
            case ByteCodeOp::JumpIfGreaterS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                emitJumpCmp(pp, CpuCondJump::JG, opBits);
                break;

            case ByteCodeOp::JumpIfGreaterU8:
            case ByteCodeOp::JumpIfGreaterU16:
            case ByteCodeOp::JumpIfGreaterU32:
            case ByteCodeOp::JumpIfGreaterU64:
            case ByteCodeOp::JumpIfGreaterF32:
            case ByteCodeOp::JumpIfGreaterF64:
                opBits = ScbeCpu::getOpBits(ip->op);
                emitJumpCmp(pp, CpuCondJump::JA, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::JumpIfGreaterEqS8:
            case ByteCodeOp::JumpIfGreaterEqS16:
            case ByteCodeOp::JumpIfGreaterEqS32:
            case ByteCodeOp::JumpIfGreaterEqS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                emitJumpCmp(pp, CpuCondJump::JGE, opBits);
                break;

            case ByteCodeOp::JumpIfGreaterEqU8:
            case ByteCodeOp::JumpIfGreaterEqU16:
            case ByteCodeOp::JumpIfGreaterEqU32:
            case ByteCodeOp::JumpIfGreaterEqU64:
            case ByteCodeOp::JumpIfGreaterEqF32:
            case ByteCodeOp::JumpIfGreaterEqF64:
                opBits = ScbeCpu::getOpBits(ip->op);
                emitJumpCmp(pp, CpuCondJump::JAE, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::DecrementRA32:
            case ByteCodeOp::DecrementRA64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitOpBinaryMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 1, CpuOp::SUB, opBits);
                break;

            case ByteCodeOp::IncrementRA64:
                pp.emitOpBinaryMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 1, CpuOp::ADD, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::DeRef8:
            case ByteCodeOp::DeRef16:
            case ByteCodeOp::DeRef32:
            case ByteCodeOp::DeRef64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, cc.computeRegI0, ip->c.u64, OpBits::B64, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

            case ByteCodeOp::DeRefStringSlice:
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFFF);
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI1, cc.computeRegI0, ip->c.u32 + 8, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), cc.computeRegI1, OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI0, cc.computeRegI0, ip->c.u32 + 0, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromBssSeg8:
            case ByteCodeOp::GetFromBssSeg16:
            case ByteCodeOp::GetFromBssSeg32:
            case ByteCodeOp::GetFromBssSeg64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitSymbolRelocationAddress(cc.computeRegI0, pp.symBSIndex, 0);
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, cc.computeRegI0, ip->b.u32, OpBits::B64, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

            case ByteCodeOp::GetFromMutableSeg8:
            case ByteCodeOp::GetFromMutableSeg16:
            case ByteCodeOp::GetFromMutableSeg32:
            case ByteCodeOp::GetFromMutableSeg64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitSymbolRelocationAddress(cc.computeRegI0, pp.symMSIndex, 0);
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, cc.computeRegI0, ip->b.u32, OpBits::B64, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8:
            case ByteCodeOp::GetFromStack16:
            case ByteCodeOp::GetFromStack32:
            case ByteCodeOp::GetFromStack64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->b.u32, OpBits::B64, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;
            case ByteCodeOp::GetIncFromStack64:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->b.u32, OpBits::B64);
                if (ip->c.u64)
                    pp.emitOpBinaryRegImm(cc.computeRegI0, ip->c.u64, CpuOp::ADD, OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetFromStack8x2:
            case ByteCodeOp::GetFromStack16x2:
            case ByteCodeOp::GetFromStack32x2:
            case ByteCodeOp::GetFromStack64x2:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->b.u32, OpBits::B64, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->d.u32, OpBits::B64, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetIncFromStack64DeRef8:
            case ByteCodeOp::GetIncFromStack64DeRef16:
            case ByteCodeOp::GetIncFromStack64DeRef32:
            case ByteCodeOp::GetIncFromStack64DeRef64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->b.u32, OpBits::B64);
                pp.emitLoadZeroExtendRegMem(cc.computeRegI0, cc.computeRegI0, ip->c.u64, OpBits::B64, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyStack8:
            case ByteCodeOp::CopyStack16:
            case ByteCodeOp::CopyStack32:
            case ByteCodeOp::CopyStack64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->b.u32, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, cc.computeRegI0, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearMaskU32:
            case ByteCodeOp::ClearMaskU64:
                opBits = ScbeCpu::getOpBits(ip->op);
                if (ip->b.u32 == 0xFF)
                    pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits, OpBits::B8);
                else if (ip->b.u32 == 0xFFFF)
                    pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits, OpBits::B16);
                else if (ip->b.u32 == 0xFFFFFFFF)
                    pp.emitLoadZeroExtendRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits, OpBits::B32);
                else
                    SWAG_ASSERT(false);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetZeroAtPointer8:
            case ByteCodeOp::SetZeroAtPointer16:
            case ByteCodeOp::SetZeroAtPointer32:
            case ByteCodeOp::SetZeroAtPointer64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadMemImm(cc.computeRegI0, ip->b.u32, 0, opBits);
                break;

            case ByteCodeOp::SetZeroAtPointerX:
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                if (ip->b.u32 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                    pp.emitClearMem(cc.computeRegI0, ip->c.u32, ip->b.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegisterAdd, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->a.u32), .value2 = ip->c.u64});
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = 0});
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->b.u64});
                    emitInternalCallCPUParams(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            case ByteCodeOp::SetZeroAtPointerXRB:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->a.u32)});
                pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = 0});
                pp.pushParams.push_back({.type = CpuPushParamType::SwagRegisterMul, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->b.u32), .value2 = ip->c.u64});
                emitInternalCallCPUParams(pp, g_LangSpec->name_memset, pp.pushParams);
                break;

                /////////////////////////////////////

            case ByteCodeOp::ClearRR8:
            case ByteCodeOp::ClearRR16:
            case ByteCodeOp::ClearRR32:
            case ByteCodeOp::ClearRR64:
            {
                opBits = ScbeCpu::getOpBits(ip->op);
                emitLoadParam(pp, cc.computeRegI0, typeFunc->numParamsRegisters(), OpBits::B64);
                pp.emitLoadMemImm(cc.computeRegI0, ip->c.u32, 0, opBits);
                break;
            }

            case ByteCodeOp::ClearRRX:
            {
                SWAG_ASSERT(ip->c.s64 >= 0 && ip->c.s64 <= 0x7FFFFFFF);
                emitLoadParam(pp, cc.computeRegI0, typeFunc->numParamsRegisters(), OpBits::B64);
                if (ip->b.u32 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitClearMem(cc.computeRegI0, ip->c.u32, ip->b.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CpuPushParamType::CpuRegister, .baseReg = cc.computeRegI0});
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = 0});
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->b.u64});
                    emitInternalCallCPUParams(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            }

                /////////////////////////////////////

            case ByteCodeOp::SetZeroStack8:
            case ByteCodeOp::SetZeroStack16:
            case ByteCodeOp::SetZeroStack32:
            case ByteCodeOp::SetZeroStack64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, 0, opBits);
                break;
            case ByteCodeOp::SetZeroStackX:
                if (ip->b.u32 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitClearMem(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, ip->b.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CpuPushParamType::LoadAddress, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetBCStack() + ip->a.u32});
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = 0});
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->b.u32});
                    emitInternalCallCPUParams(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtPointer8:
            case ByteCodeOp::SetAtPointer16:
            case ByteCodeOp::SetAtPointer32:
            case ByteCodeOp::SetAtPointer64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitLoadMemImm(cc.computeRegI0, ip->c.u32, ip->b.u64, opBits);
                else
                {
                    pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), opBits);
                    pp.emitLoadMemReg(cc.computeRegI0, ip->c.u32, cc.computeRegI1, opBits);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8:
            case ByteCodeOp::SetAtStackPointer16:
            case ByteCodeOp::SetAtStackPointer32:
            case ByteCodeOp::SetAtStackPointer64:
                opBits = ScbeCpu::getOpBits(ip->op);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, ip->b.u64, opBits);
                else
                {
                    pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), opBits);
                    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, cc.computeRegI0, opBits);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetAtStackPointer8x2:
            case ByteCodeOp::SetAtStackPointer16x2:
            case ByteCodeOp::SetAtStackPointer32x2:
            case ByteCodeOp::SetAtStackPointer64x2:
                opBits = ScbeCpu::getOpBits(ip->op);
                if (ip->hasFlag(BCI_IMM_B))
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, ip->b.u64, opBits);
                else
                {
                    pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), opBits);
                    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, cc.computeRegI0, opBits);
                }

                if (ip->hasFlag(BCI_IMM_D))
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->c.u32, ip->d.u64, opBits);
                else
                {
                    pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->d.u32), opBits);
                    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->c.u32, cc.computeRegI0, opBits);
                }
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeStackPointer:
                pp.emitLoadAddressMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->b.u32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;
            case ByteCodeOp::MakeStackPointerx2:
                pp.emitLoadAddressMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->b.u32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                pp.emitLoadAddressMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->d.u32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, OpBits::B64);
                break;
            case ByteCodeOp::MakeStackPointerRT:
                pp.emitLoadAddressMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(0), cc.computeRegI0, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::MakeMutableSegPointer:
                pp.emitSymbolRelocationAddress(cc.computeRegI0, pp.symMSIndex, ip->b.u32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;
            case ByteCodeOp::MakeBssSegPointer:
                pp.emitSymbolRelocationAddress(cc.computeRegI0, pp.symBSIndex, ip->b.u32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;
            case ByteCodeOp::MakeConstantSegPointer:
                pp.emitSymbolRelocationAddress(cc.computeRegI0, pp.symCSIndex, ip->b.u32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;
            case ByteCodeOp::MakeCompilerSegPointer:
                break;

                /////////////////////////////////////

            case ByteCodeOp::IncPointer64:
                emitAddSubMul64(pp, 1, CpuOp::ADD);
                break;
            case ByteCodeOp::DecPointer64:
                emitAddSubMul64(pp, 1, CpuOp::SUB);
                break;
            case ByteCodeOp::IncMulPointer64:
                emitAddSubMul64(pp, ip->d.u64, CpuOp::ADD);
                break;

                /////////////////////////////////////

            case ByteCodeOp::Mul64byVB64:
                pp.emitOpBinaryMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), ip->b.u64, CpuOp::IMUL, OpBits::B64);
                break;
            case ByteCodeOp::Div64byVB64:
                pp.emitOpBinaryMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), ip->b.u64, CpuOp::IDIV, OpBits::B64);
                break;

                /////////////////////////////////////

            case ByteCodeOp::SetImmediate32:
            case ByteCodeOp::SetImmediate64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), ip->b.u64, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::MemCpy8:
            case ByteCodeOp::MemCpy16:
            case ByteCodeOp::MemCpy32:
            case ByteCodeOp::MemCpy64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitCopy(cc.computeRegI0, cc.computeRegI1, ScbeCpu::getNumBits(opBits) / 8);
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMemCpy:
                if (ip->hasFlag(BCI_IMM_C) &&
                    ip->c.u64 <= buildParameters.buildCfg->backendSCBE.unrollMemLimit &&
                    buildParameters.buildCfg->backendOptimize > BuildCfgBackendOptim::O1)
                {
                    pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                    pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                    pp.emitCopy(cc.computeRegI0, cc.computeRegI1, ip->c.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->a.u32)});
                    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->b.u32)});
                    if (ip->hasFlag(BCI_IMM_C))
                        pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->c.u64});
                    else
                        pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->c.u32)});
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
                    pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                    pp.emitClearMem(cc.computeRegI1, 0, ip->c.u32);
                }
                else
                {
                    pp.pushParams.clear();
                    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->a.u32)});
                    if (ip->hasFlag(BCI_IMM_B))
                        pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->b.u64});
                    else
                        pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->b.u32)});
                    if (ip->hasFlag(BCI_IMM_C))
                        pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->c.u64});
                    else
                        pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->c.u32)});
                    emitInternalCallCPUParams(pp, g_LangSpec->name_memset, pp.pushParams);
                }
                break;
            case ByteCodeOp::IntrinsicMemMove:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->a.u32)});
                pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->b.u32)});
                if (ip->hasFlag(BCI_IMM_C))
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->c.u64});
                else
                    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->c.u32)});
                emitInternalCallCPUParams(pp, g_LangSpec->name_memmove, pp.pushParams);
                break;
            case ByteCodeOp::IntrinsicMemCmp:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->b.u32)});
                pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->c.u32)});
                if (ip->hasFlag(BCI_IMM_D))
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->d.u64});
                else
                    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->d.u32)});
                emitInternalCallCPUParams(pp, g_LangSpec->name_memcmp, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;

            case ByteCodeOp::IntrinsicStrLen:
                emitInternalCallRAParams(pp, g_LangSpec->name_strlen, {ip->b.u32}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicStrCmp:
                emitInternalCallRAParams(pp, g_LangSpec->name_strcmp, {ip->b.u32, ip->c.u32}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;

            case ByteCodeOp::IntrinsicAlloc:
                emitInternalCallRAParams(pp, g_LangSpec->name_malloc, {ip->b.u32}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicRealloc:
                emitInternalCallRAParams(pp, g_LangSpec->name_realloc, {ip->b.u32, ip->c.u32}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
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
                pp.pushParams.push_back({.type = CpuPushParamType::SymbolRelocationValue, .value = pp.symTls_threadLocalId});
                pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = module->tlsSegment.totalCount, .typeInfo = g_TypeMgr->typeInfoU64});
                pp.pushParams.push_back({.type = CpuPushParamType::SymbolRelocationAddress, .value = pp.symTLSIndex});
                emitInternalCallCPUParams(pp, g_LangSpec->name_priv_tlsGetPtr, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicGetContext:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CpuPushParamType::SymbolRelocationValue, .value = pp.symPI_contextTlsId});
                emitInternalCallCPUParams(pp, g_LangSpec->name_priv_tlsGetValue, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicSetContext:
                pp.pushParams.clear();
                pp.pushParams.push_back({.type = CpuPushParamType::SymbolRelocationValue, .value = pp.symPI_contextTlsId});
                pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->a.u32)});
                emitInternalCallCPUParams(pp, g_LangSpec->name_priv_tlsSetValue, pp.pushParams);
                break;

            case ByteCodeOp::IntrinsicGetProcessInfos:
                pp.emitSymbolRelocationAddress(cc.computeRegI1, pp.symPI_processInfos, 0);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI1, OpBits::B64);
                break;

            case ByteCodeOp::IntrinsicCVaStart:
            {
                const uint32_t paramIdx = typeFunc->numParamsRegisters() + (typeFunc->returnByAddress() ? 1 : 0);
                pp.emitLoadCallerAddressParam(cc.computeRegI0, paramIdx);
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadMemReg(cc.computeRegI1, 0, cc.computeRegI0, OpBits::B64);
                break;
            }
            case ByteCodeOp::IntrinsicCVaEnd:
                break;
            case ByteCodeOp::IntrinsicCVaArg:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI1, cc.computeRegI0, 0, OpBits::B64);
                pp.emitLoadZeroExtendRegMem(cc.computeRegI1, cc.computeRegI1, 0, OpBits::B64, static_cast<OpBits>(ip->c.u32 * 8));
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), cc.computeRegI1, OpBits::B64);
                pp.emitOpBinaryMemImm(cc.computeRegI0, 0, 8, CpuOp::ADD, OpBits::B64);
                break;

            case ByteCodeOp::IntrinsicArguments:
                SWAG_ASSERT(ip->b.u32 == ip->a.u32 + 1);
                emitInternalCallRAParams(pp, g_LangSpec->name_at_args, {}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicModules:
                if (buildParameters.module->modulesSliceOffset == UINT32_MAX)
                {
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B64);
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), 0, OpBits::B64);
                }
                else
                {
                    pp.emitSymbolRelocationAddress(cc.computeRegI0, pp.symCSIndex, buildParameters.module->modulesSliceOffset);
                    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), buildParameters.module->moduleDependencies.count + 1, OpBits::B64);
                }
                break;
            case ByteCodeOp::IntrinsicGvtd:
                if (buildParameters.module->globalVarsToDropSliceOffset == UINT32_MAX)
                {
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B64);
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), 0, OpBits::B64);
                }
                else
                {
                    pp.emitSymbolRelocationAddress(cc.computeRegI0, pp.symMSIndex, buildParameters.module->globalVarsToDropSliceOffset);
                    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), buildParameters.module->globalVarsToDrop.count, OpBits::B64);
                }
                break;

            case ByteCodeOp::IntrinsicCompiler:
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B64);
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), 0, OpBits::B64);
                break;
            case ByteCodeOp::IntrinsicIsByteCode:
                pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), 0, OpBits::B32);
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
                emitInternalCallRAParams(pp, g_LangSpec->name_at_itftableof, {ip->a.u32, ip->b.u32}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32));
                break;

                /////////////////////////////////////

            case ByteCodeOp::CopyRAtoRR:
                if (ip->hasFlag(BCI_IMM_A))
                {
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetResult(), ip->a.u64, OpBits::B64);
                }
                else
                {
                    pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetResult(), cc.computeRegI0, OpBits::B64);
                }

                break;

            case ByteCodeOp::CopyRARBtoRR2:
            {
                emitLoadParam(pp, cc.computeRegI0, typeFunc->numParamsRegisters(), OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadMemReg(cc.computeRegI0, 0, cc.computeRegI1, OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitLoadMemReg(cc.computeRegI0, 8, cc.computeRegI1, OpBits::B64);
                break;
            }

            case ByteCodeOp::CopyRAtoRT:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(0), cc.computeRegI0, OpBits::B64);
                break;

            case ByteCodeOp::SaveRRtoRA:
            {
                emitLoadParam(pp, cc.computeRegI0, typeFunc->numParamsRegisters(), OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;
            }
            case ByteCodeOp::CopyRRtoRA:
            {
                emitLoadParam(pp, cc.computeRegI0, typeFunc->numParamsRegisters(), OpBits::B64);
                if (ip->b.u64)
                {
                    pp.emitLoadRegImm(cc.computeRegI1, ip->b.u64, OpBits::B64);
                    pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::ADD, OpBits::B64);
                }

                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;
            }

            case ByteCodeOp::CopyRTtoRA:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(0), OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                break;

            case ByteCodeOp::CopyRT2toRARB:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(0), OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(1), OpBits::B64);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), cc.computeRegI0, OpBits::B64);
                break;

            case ByteCodeOp::CopySP:
                pp.emitLoadAddressMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32));
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
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
                opBits = ScbeCpu::getOpBits(ip->op);
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
                opBits = ScbeCpu::getOpBits(ip->op);
                emitGetParam(pp, ip->a.u32, ip->b.mergeU64U32.high, OpBits::B64, 0, opBits);
                break;

                /////////////////////////////////////

            case ByteCodeOp::GetIncParam64DeRef8:
            case ByteCodeOp::GetIncParam64DeRef16:
            case ByteCodeOp::GetIncParam64DeRef32:
            case ByteCodeOp::GetIncParam64DeRef64:
                opBits = ScbeCpu::getOpBits(ip->op);
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
                opBits = ScbeCpu::getOpBitsByBytes(returnType->sizeOf);
                if (ip->hasFlag(BCI_IMM_A))
                {
                    pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetResult(), ip->a.u64, opBits);
                }
                else
                {
                    pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
                    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetResult(), cc.computeRegI0, opBits);
                }
                [[fallthrough]];

            case ByteCodeOp::Ret:

                // Emit result
                if (!returnType->isVoid() && !typeFunc->returnByAddress())
                {
                    opBits = ScbeCpu::getOpBitsByBytes(returnType->sizeOf);
                    if (returnType->isNative(NativeTypeKind::F32))
                        pp.emitLoadRegMem(cc.returnByRegisterFloat, CpuReg::Rsp, pp.cpuFct->getStackOffsetResult(), OpBits::B32);
                    else if (returnType->isNative(NativeTypeKind::F64))
                        pp.emitLoadRegMem(cc.returnByRegisterFloat, CpuReg::Rsp, pp.cpuFct->getStackOffsetResult(), OpBits::B64);
                    else
                        pp.emitLoadRegMem(cc.returnByRegisterInteger, CpuReg::Rsp, pp.cpuFct->getStackOffsetResult(), opBits);
                }

                pp.emitLeave();
                break;

                /////////////////////////////////////

            case ByteCodeOp::IntrinsicMulAddF32:
            case ByteCodeOp::IntrinsicMulAddF64:
            {
                opBits = ScbeCpu::getOpBits(ip->op);
                emitIMMB(pp, cc.computeRegF0, opBits);
                emitIMMC(pp, cc.computeRegF1, opBits);
                emitIMMD(pp, cc.computeRegF2, opBits);
                pp.emitOpMulAdd(cc.computeRegF0, cc.computeRegF1, cc.computeRegF2, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, opBits);
                break;
            }

            case ByteCodeOp::IntrinsicAtomicAddS8:
            case ByteCodeOp::IntrinsicAtomicAddS16:
            case ByteCodeOp::IntrinsicAtomicAddS32:
            case ByteCodeOp::IntrinsicAtomicAddS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI0, cc.computeRegI1, 0, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, opBits);
                emitBinOpEqLock(pp, CpuOp::ADD);
                break;

            case ByteCodeOp::IntrinsicAtomicAndS8:
            case ByteCodeOp::IntrinsicAtomicAndS16:
            case ByteCodeOp::IntrinsicAtomicAndS32:
            case ByteCodeOp::IntrinsicAtomicAndS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI0, cc.computeRegI1, 0, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, opBits);
                emitBinOpEqLock(pp, CpuOp::AND);
                break;

            case ByteCodeOp::IntrinsicAtomicOrS8:
            case ByteCodeOp::IntrinsicAtomicOrS16:
            case ByteCodeOp::IntrinsicAtomicOrS32:
            case ByteCodeOp::IntrinsicAtomicOrS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI0, cc.computeRegI1, 0, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, opBits);
                emitBinOpEqLock(pp, CpuOp::OR);
                break;

            case ByteCodeOp::IntrinsicAtomicXorS8:
            case ByteCodeOp::IntrinsicAtomicXorS16:
            case ByteCodeOp::IntrinsicAtomicXorS32:
            case ByteCodeOp::IntrinsicAtomicXorS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI0, cc.computeRegI1, 0, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, opBits);
                emitBinOpEqLock(pp, CpuOp::XOR);
                break;

            case ByteCodeOp::IntrinsicAtomicXchgS8:
            case ByteCodeOp::IntrinsicAtomicXchgS16:
            case ByteCodeOp::IntrinsicAtomicXchgS32:
            case ByteCodeOp::IntrinsicAtomicXchgS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI0, cc.computeRegI1, 0, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc.computeRegI0, opBits);
                emitBinOpEqLock(pp, CpuOp::XCHG);
                break;

            case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
            case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
                opBits = ScbeCpu::getOpBits(ip->op);
                pp.emitLoadRegMem(cc.computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                emitIMMB(pp, cc.computeRegI0, opBits);
                emitIMMC(pp, cc.computeRegI2, opBits);
                pp.emitOpBinaryRegReg(cc.computeRegI1, cc.computeRegI2, CpuOp::CMPXCHG, opBits);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->d.u32), cc.computeRegI0, opBits);
                break;

            case ByteCodeOp::IntrinsicS8x1:
            case ByteCodeOp::IntrinsicS16x1:
            case ByteCodeOp::IntrinsicS32x1:
            case ByteCodeOp::IntrinsicS64x1:
            {
                opBits = ScbeCpu::getOpBits(ip->op);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicAbs:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        pp.emitLoadRegReg(cc.computeRegI1, cc.computeRegI0, opBits);
                        pp.emitOpBinaryRegImm(cc.computeRegI1, ScbeCpu::getNumBits(opBits) - 1, CpuOp::SAR, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::XOR, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::SUB, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
                        break;
                    case TokenId::IntrinsicBitCountNz:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI0, CpuOp::POPCNT, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
                        break;
                    case TokenId::IntrinsicBitCountTz:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI0, CpuOp::BSF, opBits);
                        pp.emitLoadRegImm(cc.computeRegI1, ScbeCpu::getNumBits(opBits), opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::CMOVE, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
                        break;
                    case TokenId::IntrinsicBitCountLz:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI0, CpuOp::BSR, opBits);
                        pp.emitLoadRegImm(cc.computeRegI1, (ScbeCpu::getNumBits(opBits) * 2) - 1, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::CMOVE, opBits);
                        pp.emitOpBinaryRegImm(cc.computeRegI0, ScbeCpu::getNumBits(opBits) - 1, CpuOp::XOR, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
                        break;
                    case TokenId::IntrinsicByteSwap:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        pp.emitOpUnaryReg(cc.computeRegI0, CpuOp::BSWAP, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
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
                opBits = ScbeCpu::getOpBits(ip->op);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        emitIMMC(pp, cc.computeRegI1, opBits);
                        pp.emitCmpRegReg(cc.computeRegI1, cc.computeRegI0, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::CMOVL, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        emitIMMC(pp, cc.computeRegI1, opBits);
                        pp.emitCmpRegReg(cc.computeRegI0, cc.computeRegI1, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::CMOVL, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
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
                opBits = ScbeCpu::getOpBits(ip->op);
                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        emitIMMC(pp, cc.computeRegI1, opBits);
                        pp.emitCmpRegReg(cc.computeRegI1, cc.computeRegI0, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::CMOVB, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        emitIMMC(pp, cc.computeRegI1, opBits);
                        pp.emitCmpRegReg(cc.computeRegI0, cc.computeRegI1, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::CMOVB, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
                        break;
                    case TokenId::IntrinsicRol:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        emitIMMC(pp, cc.computeRegI1, OpBits::B8);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::ROL, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
                        break;
                    case TokenId::IntrinsicRor:
                        emitIMMB(pp, cc.computeRegI0, opBits);
                        emitIMMC(pp, cc.computeRegI1, OpBits::B8);
                        pp.emitOpBinaryRegReg(cc.computeRegI0, cc.computeRegI1, CpuOp::ROR, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, opBits);
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
                opBits          = ScbeCpu::getOpBits(ip->op);
                const bool is32 = opBits == OpBits::B32;
                pp.pushParams.clear();
                if (ip->hasFlag(BCI_IMM_B))
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->b.u64});
                else
                    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->b.u32)});
                if (ip->hasFlag(BCI_IMM_C))
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->c.u64});
                else
                    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->c.u32)});

                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicMin:
                        emitIMMB(pp, cc.computeRegF0, opBits);
                        emitIMMC(pp, cc.computeRegF1, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegF1, CpuOp::FMIN, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, opBits);
                        break;
                    case TokenId::IntrinsicMax:
                        emitIMMB(pp, cc.computeRegF0, opBits);
                        emitIMMC(pp, cc.computeRegF1, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegF1, CpuOp::FMAX, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, opBits);
                        break;

                    case TokenId::IntrinsicPow:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_powf : g_LangSpec->name_pow, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicATan2:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_atan2f : g_LangSpec->name_atan2, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
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
                opBits          = ScbeCpu::getOpBits(ip->op);
                const bool is32 = opBits == OpBits::B32;
                pp.pushParams.clear();
                if (ip->hasFlag(BCI_IMM_B))
                    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = ip->b.u64});
                else
                    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(ip->b.u32)});

                switch (static_cast<TokenId>(ip->d.u32))
                {
                    case TokenId::IntrinsicSqrt:
                        emitIMMB(pp, cc.computeRegF0, opBits);
                        pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegF0, CpuOp::FSQRT, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, opBits);
                        break;
                    case TokenId::IntrinsicAbs:
                        emitIMMB(pp, cc.computeRegF0, opBits);
                        pp.emitLoadRegImm(cc.computeRegI0, opBits == OpBits::B32 ? 0x7FFFFFFF : 0x7FFFFFFF'FFFFFFFF, OpBits::B64);
                        pp.emitLoadRegReg(cc.computeRegF1, cc.computeRegI0, OpBits::B64);
                        pp.emitOpBinaryRegReg(cc.computeRegF0, cc.computeRegF1, CpuOp::FAND, opBits);
                        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegF0, opBits);
                        break;

                    case TokenId::IntrinsicSin:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_sinf : g_LangSpec->name_sin, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCos:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_cosf : g_LangSpec->name_cos, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTan:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_tanf : g_LangSpec->name_tan, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicSinh:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_sinhf : g_LangSpec->name_sinh, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCosh:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_coshf : g_LangSpec->name_cosh, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTanh:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_tanhf : g_LangSpec->name_tanh, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicASin:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_asinf : g_LangSpec->name_asin, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicACos:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_acosf : g_LangSpec->name_acos, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicATan:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_atanf : g_LangSpec->name_atan, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_logf : g_LangSpec->name_log, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog2:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_log2f : g_LangSpec->name_log2, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicLog10:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_log10f : g_LangSpec->name_log10, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicFloor:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_floorf : g_LangSpec->name_floor, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicCeil:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_ceilf : g_LangSpec->name_ceil, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicTrunc:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_truncf : g_LangSpec->name_trunc, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicRound:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_roundf : g_LangSpec->name_round, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicExp:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_expf : g_LangSpec->name_exp, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                        break;
                    case TokenId::IntrinsicExp2:
                        emitInternalCallCPUParams(pp, is32 ? g_LangSpec->name_exp2f : g_LangSpec->name_exp2, pp.pushParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
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
                emitInternalCallRAParams(pp, g_LangSpec->name_at_err, {}, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32));
                break;
            case ByteCodeOp::InternalSetErr:
                emitInternalCallRAParams(pp, g_LangSpec->name_priv_seterr, {ip->a.u32, ip->b.u32});
                break;
            case ByteCodeOp::InternalHasErr:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
                pp.emitLoadRegMem(cc.computeRegI1, cc.computeRegI0, offsetof(SwagContext, hasError), OpBits::B32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI1, OpBits::B32);
                break;
            case ByteCodeOp::JumpIfError:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitCmpMemImm(cc.computeRegI0, offsetof(SwagContext, hasError), 0, OpBits::B32);
                pp.emitJumpCondImm(CpuCondJump::JNZ, pp.ipIndex + ip->b.s32 + 1);
                break;
            case ByteCodeOp::JumpIfNoError:
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitCmpMemImm(cc.computeRegI0, offsetof(SwagContext, hasError), 0, OpBits::B32);
                pp.emitJumpCondImm(CpuCondJump::JZ, pp.ipIndex + ip->b.s32 + 1);
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
                pp.emitLoadRegMem(cc.computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
                pp.emitLoadMemImm(cc.computeRegI0, offsetof(SwagContext, traceIndex), 0, OpBits::B32);
                break;
            case ByteCodeOp::InternalStackTraceConst:
                pp.emitSymbolRelocationAddress(cc.computeRegI0, pp.symCSIndex, ip->b.u32);
                pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc.computeRegI0, OpBits::B64);
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

    pp.process(ppCPU);
    pp.encode(ppCPU);

    ppCPU.endFunction();
    return ok;
}
