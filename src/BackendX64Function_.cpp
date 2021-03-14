#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64FunctionBodyJob.h"
#include "BackendX64FunctionMacros_.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"

#define UWOP_PUSH_NONVOL 0
#define UWOP_ALLOC_LARGE 1
#define UWOP_ALLOC_SMALL 2

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

void BackendX64::setCalleeParameter(X64PerThread& pp, TypeInfo* typeParam, int calleeIndex, int stackOffset, uint32_t sizeStack)
{
    if (calleeIndex < 4)
    {
        switch (calleeIndex)
        {
        case 0:
            if (typeParam->flags & TYPEINFO_FLOAT)
                BackendX64Inst::emit_StoreF64_Indirect(pp, stackOffset, XMM0, RDI);
            else
                BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, RCX, RDI);
            break;
        case 1:
            if (typeParam->flags & TYPEINFO_FLOAT)
                BackendX64Inst::emit_StoreF64_Indirect(pp, stackOffset, XMM1, RDI);
            else
                BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, RDX, RDI);
            break;
        case 2:
            if (typeParam->flags & TYPEINFO_FLOAT)
                BackendX64Inst::emit_StoreF64_Indirect(pp, stackOffset, XMM2, RDI);
            else
                BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, R8, RDI);
            break;
        case 3:
            if (typeParam->flags & TYPEINFO_FLOAT)
                BackendX64Inst::emit_StoreF64_Indirect(pp, stackOffset, XMM3, RDI);
            else
                BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, R9, RDI);
            break;
        }
    }
    else
    {
        // Get parameter from the stack (aligned to 8 bytes)
        auto offset = calleeIndex * (int) sizeof(Register);
        offset += sizeStack;
        offset += 16;
        BackendX64Inst::emit_Load64_Indirect(pp, offset, RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, RAX, RDI);
    }
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
        auto unwind0 = (uint16_t)(UWOP_ALLOC_SMALL | (sizeStack << 4));
        unwind0 <<= 8;
        unwind0 |= (uint16_t) offsetSubRSP;
        unwind.push_back(unwind0);
    }
    else
    {
        SWAG_ASSERT(sizeStack <= (512 * 1024) - 8);
        auto unwind0 = (uint16_t)(UWOP_ALLOC_LARGE);
        unwind0 <<= 8;
        unwind0 |= (uint16_t) offsetSubRSP;
        unwind.push_back(unwind0);
        unwind0 = (uint16_t)(sizeStack / 8);
        unwind.push_back(unwind0);
    }
}

bool BackendX64::emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* bc)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;
    bool  debug           = buildParameters.buildCfg->backendDebugInformations;

    alignConcat(concat, 16);
    uint32_t startAddress = concat.totalCount();

    node->computeFullNameForeign(true);

    // Symbol
    uint32_t symbolFuncIndex = getOrAddSymbol(pp, node->fullnameForeign, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    pp.directives += format("/EXPORT:%s ", node->fullnameForeign.c_str());
    auto coffFct     = registerFunction(pp, node, symbolFuncIndex);
    coffFct->wrapper = true;

    if (debug)
    {
        DbgLine dbgLine;
        dbgLine.line       = node->exportForeignLine;
        dbgLine.byteOffset = 0;
        DbgLines dbgLines;
        dbgLines.sourceFile = node->sourceFile;
        dbgLines.dbgLines.push_back(dbgLine);
        coffFct->dbgLines.push_back(dbgLines);
    }

    VectorNative<TypeInfo*> pushRAParams;

    // Total number of registers
    auto numCallRegisters = 0;
    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        numCallRegisters += typeParam->numRegisters();
    }

    // First we have return values
    bool returnByCopy = typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY;
    if (returnByCopy)
    {
        pushRAParams.push_back(g_TypeMgr.typeInfoPVoid);
    }
    else if (typeFunc->returnType->numRegisters() == 1)
    {
        pushRAParams.push_back(typeFunc->returnType);
    }
    else if (typeFunc->returnType->numRegisters() == 2)
    {
        pushRAParams.push_back(g_TypeMgr.typeInfoPVoid);
        pushRAParams.push_back(g_TypeMgr.typeInfoPVoid);
    }

    // Then variadic
    int numParams = (int) typeFunc->parameters.size();
    if (numParams)
    {
        auto param     = typeFunc->parameters.back();
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic || typeParam->kind == TypeInfoKind::TypedVariadic)
        {
            pushRAParams.push_back(g_TypeMgr.typeInfoU64);
            pushRAParams.push_back(g_TypeMgr.typeInfoU64);
            numParams--;
        }
    }

    // Then parameters
    for (int i = 0; i < numParams; i++)
    {
        auto param     = typeFunc->parameters[i];
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (typeParam->numRegisters() == 1)
        {
            pushRAParams.push_back(typeParam);
        }
        else
        {
            pushRAParams.push_back(g_TypeMgr.typeInfoU64);
            pushRAParams.push_back(g_TypeMgr.typeInfoU64);
        }
    }

    uint32_t offsetRetCopy = 0;
    uint32_t sizeStack     = (uint32_t) pushRAParams.size() * sizeof(Register);
    if (returnByCopy)
    {
        offsetRetCopy = sizeStack;
        sizeStack += sizeof(Register);
    }

    // Prolog
    VectorNative<uint16_t> unwind;
    auto                   beforeProlog = concat.totalCount();
    auto                   sizeProlog   = 0;
    if (sizeStack)
    {
        BackendX64Inst::emit_Push(pp, RDI);
        sizeProlog       = concat.totalCount() - beforeProlog;
        uint16_t unwind0 = computeUnwindPushRDI(sizeProlog);

        MK_ALIGN16(sizeStack);
        sizeStack += 4 * sizeof(Register);
        BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, sizeStack);
        sizeProlog = concat.totalCount() - beforeProlog;
        computeUnwindStack(sizeStack, sizeProlog, unwind);
        BackendX64Inst::emit_Copy64(pp, RSP, RDI);

        // At the end because array must be sorted in 'offset in prolog' descending order
        unwind.push_back(unwind0);
    }
    else
    {
        BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 5 * sizeof(Register));
        sizeProlog = concat.totalCount() - beforeProlog;
        computeUnwindStack(5 * sizeof(Register), sizeProlog, unwind);
    }

    // Need to save return register if needed
    if (!returnByCopy && typeFunc->numReturnRegisters() == 2)
    {
        SWAG_ASSERT(sizeStack);
        SWAG_ASSERT(pushRAParams.size() >= 2);
        int offset = (int) pushRAParams.size() - 2;
        switch (offset)
        {
        case 0:
            BackendX64Inst::emit_Store64_Indirect(pp, sizeStack + 16, RCX, RDI);
            break;
        case 1:
            BackendX64Inst::emit_Store64_Indirect(pp, sizeStack + 16 + 8, RDX, RDI);
            break;
        case 2:
            BackendX64Inst::emit_Store64_Indirect(pp, sizeStack + 16 + 16, R8, RDI);
            break;
        case 3:
            BackendX64Inst::emit_Store64_Indirect(pp, sizeStack + 16 + 24, R9, RDI);
            break;
        }
    }

    // Push all
    auto numReturnRegs = typeFunc->numReturnRegisters();
    for (int i = 0; i < (int) pushRAParams.size(); i++)
    {
        auto typeParam = pushRAParams[i];
        if (i < numReturnRegs)
        {
            if (returnByCopy)
            {
                BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetRetCopy, RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(i), RAX, RDI);
                setCalleeParameter(pp, typeParam, numCallRegisters, offsetRetCopy, sizeStack);
            }
            else
            {
                BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(i), RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(i), RAX, RDI);
            }
        }
        else
        {
            setCalleeParameter(pp, typeParam, i - numReturnRegs, regOffset(i), sizeStack);
        }
    }

    emitCall(pp, bc->callName());

    // Return
    if (returnByCopy)
    {
        // Done
    }
    else if (typeFunc->numReturnRegisters() == 1)
    {
        BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RDI);
    }
    else if (typeFunc->numReturnRegisters() == 2)
    {
        // Get the results in rax & rcx
        BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RDI);
        BackendX64Inst::emit_Load64_Indirect(pp, regOffset(1), RCX, RDI);

        // Get the pointer to store the result
        SWAG_ASSERT(pushRAParams.size() >= 2);
        int offset = (int) pushRAParams.size() - 2;
        offset *= sizeof(Register);
        offset += sizeStack + 16;
        BackendX64Inst::emit_Load64_Indirect(pp, offset, RDX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RDX);
        BackendX64Inst::emit_Store64_Indirect(pp, 8, RCX, RDX);
    }

    if (sizeStack)
    {
        BackendX64Inst::emit_Add_Cst32_To_RSP(pp, sizeStack);
        BackendX64Inst::emit_Pop(pp, RDI);
    }
    else
    {
        BackendX64Inst::emit_Add_Cst32_To_RSP(pp, 5 * sizeof(Register));
    }

    BackendX64Inst::emit_Ret(pp);

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}

void BackendX64::emitOverflowSigned(const BuildParameters& buildParameters, Concat& concat, AstNode* node)
{
    if (!module->mustEmitSafetyOF(node))
        return;
    concat.addString2("\x0f\x81"); // jno
    concat.addU32(0);
    auto addr      = (uint32_t*) concat.getSeekPtr() - 1;
    auto prevCount = concat.totalCount();
    emitInternalPanic(buildParameters, node, "[safety] integer overflow");
    *addr = concat.totalCount() - prevCount;
}

void BackendX64::emitOverflowUnsigned(const BuildParameters& buildParameters, Concat& concat, AstNode* node)
{
    if (!module->mustEmitSafetyOF(node))
        return;
    concat.addString2("\x0f\x83"); // jnc
    concat.addU32(0);
    auto addr      = (uint32_t*) concat.getSeekPtr() - 1;
    auto prevCount = concat.totalCount();
    emitInternalPanic(buildParameters, node, "[safety] integer overflow");
    *addr = concat.totalCount() - prevCount;
}

void BackendX64::emitInternalPanic(const BuildParameters& buildParameters, AstNode* node, const char* msg)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];

    emitGlobalString(pp, precompileIndex, normalizePath(node->sourceFile->path), RAX);
    BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
    BackendX64Inst::emit_Load64_Immediate(pp, node->token.startLocation.line, RAX);
    BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
    BackendX64Inst::emit_Load64_Immediate(pp, node->token.startLocation.column, RAX);
    BackendX64Inst::emit_Store64_Indirect(pp, 16, RAX, RSP);
    if (msg)
        emitGlobalString(pp, precompileIndex, msg, RAX);
    else
        BackendX64Inst::emit_Clear64(pp, RAX);
    BackendX64Inst::emit_Store64_Indirect(pp, 24, RAX, RSP);
    emitCall(pp, "__panic");
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
    auto  typeFunc        = bc->callType();
    bool  ok              = true;
    bool  debug           = buildParameters.buildCfg->backendDebugInformations;

    alignConcat(concat, 16);
    uint32_t startAddress = concat.totalCount();

    pp.labels.clear();
    pp.labelsToSolve.clear();
    bc->markLabels();

    // Symbol
    auto symbolFuncIndex = getOrAddSymbol(pp, bc->callName(), CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct         = registerFunction(pp, bc->node, symbolFuncIndex);
    if (debug)
        dbgSetLocation(coffFct, bc, nullptr, 0);

    // In order, starting at RSP, we have :
    //
    // sizeParamsStack to store function call parameters
    // ...padding to 16...

    // All registers:       bc->maxReservedRegisterRC
    // offsetRT:            bc->maxCallResults
    // offsetStack:         stackSize, function local stack
    // offsetRR:            2 * sizeof(Register), the result registers of a function call
    // offsetFLT:           sizeof(Register)
    // ...padding to 16... => total is sizeStack

    uint32_t offsetRT    = bc->maxReservedRegisterRC * sizeof(Register);
    uint32_t offsetStack = offsetRT + bc->maxCallResults * sizeof(Register);
    uint32_t offsetRR    = offsetStack + typeFunc->stackSize;

    // For float load
    // (should be reserved only if we have floating point operations in that function)
    uint32_t offsetFLT = offsetRR + 2 * sizeof(Register);

    uint32_t sizeStack = offsetFLT + 8;
    MK_ALIGN16(sizeStack);

    // x64 calling convention, space for at least 4 parameters when calling a function
    // (should ideally be reserved only if we have a call)
    uint32_t sizeParamsStack = max(4 * sizeof(Register), (bc->maxCallParams + 1) * sizeof(Register));
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

    auto                                   ip = bc->out;
    VectorNative<uint32_t>                 pushRAParams;
    VectorNative<pair<uint32_t, uint32_t>> pushRVParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
            continue;

        if (debug)
            dbgSetLocation(coffFct, bc, ip, concat.totalCount() - beforeProlog);

        if (ip->flags & BCI_JUMP_DEST)
            getOrCreateLabel(pp, i);

        //concat.addU8(0x90); // NOP TO REMOVE

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::Nop:
        case ByteCodeOp::DecSP:
        case ByteCodeOp::DecSPBP:
        case ByteCodeOp::IncSP:
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
            continue;
        }

        switch (ip->op)
        {
        case ByteCodeOp::DebugNop:
            concat.addU8(0x90); // nop
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

        case ByteCodeOp::CopyRBtoRA:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CopyRBAddrToRA:
        case ByteCodeOp::CopyRBAddrToRA2:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CastBool8:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastBool16:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test16(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastBool32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test32(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastBool64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test64(pp, RAX, RAX);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CastInvBool8:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_SetE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastInvBool16:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test16(pp, RAX, RAX);
            BackendX64Inst::emit_SetE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastInvBool32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test32(pp, RAX, RAX);
            BackendX64Inst::emit_SetE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastInvBool64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test64(pp, RAX, RAX);
            BackendX64Inst::emit_SetE(pp);
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
        case ByteCodeOp::CastS8S64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AL_To_RAX(pp);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastS16S32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AX_To_EAX(pp);
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
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
        case ByteCodeOp::CastS32F32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastU32F32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\xf3\x48\x0f\x2a\xc0"); // cvtsi2ss xmm0, rax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastF32S32:
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString4("\xf3\x0f\x2c\xc0"); // cvttss2si eax, xmm0
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastF32F64:
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString4("\xf3\x0f\x5a\xc0"); // cvtss2sd xmm0, xmm0
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastF64F32:
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString4("\xf2\x0f\x5a\xc0"); // cvtsd2ss xmm0, xmm0
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastS64F64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\xf2\x48\x0f\x2a\xc0"); // cvtsi2sd xmm0, rax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastF64S64:
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString5("\xf2\x48\x0f\x2c\xc0"); // cvttss2si rax, xmm0
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastU64F64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\xf2\x48\x0f\x2a\xc0"); // cvtsi2ss xmm0, rax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;

        case ByteCodeOp::BinOpShiftLeftU32:
            if (ip->flags & BCI_IMM_A)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u32, RAX);
            else
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u8, RCX);
            else
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd3\xe0"); // shl eax, cl
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::BinOpShiftLeftU64:
            if (ip->flags & BCI_IMM_A)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u64, RAX);
            else
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u8, RCX);
            else
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xd3\xe0"); // shl rax, cl
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::BinOpShiftRightU32:
            if (ip->flags & BCI_IMM_A)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u32, RAX);
            else
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u8, RCX);
            else
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd3\xe8"); // shr eax, cl
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::BinOpShiftRightU64:
            if (ip->flags & BCI_IMM_A)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u64, RAX);
            else
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u8, RCX);
            else
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xd3\xe8"); // shr rax, cl
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::BinOpXorU32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::XOR);
            break;
        case ByteCodeOp::BinOpXorU64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::XOR);
            break;

        case ByteCodeOp::BinOpMulS32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::IMUL);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpMulU32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::MUL);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpMulS64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::IMUL);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpMulU64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::MUL);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
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
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpPlusU32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpPlusS64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpPlusU64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpPlusF32:
            BackendX64Inst::emit_BinOpFloat32_At_Reg(pp, ip, X64Op::FADD);
            break;
        case ByteCodeOp::BinOpPlusF64:
            BackendX64Inst::emit_BinOpFloat64_At_Reg(pp, ip, X64Op::FADD);
            break;

        case ByteCodeOp::BinOpMinusS32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::SUB);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpMinusU32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpMinusS64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::SUB);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpMinusU64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::BinOpMinusF32:
            BackendX64Inst::emit_BinOpFloat32_At_Reg(pp, ip, X64Op::FSUB);
            break;
        case ByteCodeOp::BinOpMinusF64:
            BackendX64Inst::emit_BinOpFloat64_At_Reg(pp, ip, X64Op::FSUB);
            break;

        case ByteCodeOp::BinOpBitmaskAndS32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::AND);
            break;
        case ByteCodeOp::BinOpBitmaskAndS64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::AND);
            break;

        case ByteCodeOp::BinOpBitmaskOrS32:
            BackendX64Inst::emit_BinOpInt32_At_Reg(pp, ip, X64Op::OR);
            break;
        case ByteCodeOp::BinOpBitmaskOrS64:
            BackendX64Inst::emit_BinOpInt64_At_Reg(pp, ip, X64Op::OR);
            break;

        case ByteCodeOp::AffectOpShiftLeftEqU8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd2\x20"); // sal byte ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xd3\x20"); // sal word ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd3\x20"); // sal dword ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xd3\x20"); // sal qword ptr [rax], cl
            break;

        case ByteCodeOp::AffectOpShiftRightEqU8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd2\x28"); // shr byte ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xd3\x28"); // shr word ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd3\x28"); // shr dword ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xd3\x28"); // shr qword ptr [rax], cl
            break;

        case ByteCodeOp::AffectOpXorEqS8:
            MK_BINOPEQ8_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::AffectOpXorEqS16:
            MK_BINOPEQ16_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::AffectOpXorEqS32:
            MK_BINOPEQ32_CAB(X64Op::XOR);
            break;
        case ByteCodeOp::AffectOpXorEqS64:
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
            emitOverflowSigned(buildParameters, concat, ip->node);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf6\xe1"); // mul cl
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xf7\xe9"); // imul cx
            emitOverflowSigned(buildParameters, concat, ip->node);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xf7\xe1"); // mul cx
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf7\xe9"); // imul ecx
            emitOverflowSigned(buildParameters, concat, ip->node);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xf7\xe1"); // mul ecx
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xf7\xe9"); // imul rcx
            emitOverflowSigned(buildParameters, concat, ip->node);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::AffectOpMulEqU64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xf7\xe1"); // mul rcx
            emitOverflowUnsigned(buildParameters, concat, ip->node);
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
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpMinusEqU8:
            MK_BINOPEQ8_CAB(X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            MK_BINOPEQ16_CAB(X64Op::SUB);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpMinusEqU16:
            MK_BINOPEQ16_CAB(X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            MK_BINOPEQ32_CAB(X64Op::SUB);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpMinusEqU32:
            MK_BINOPEQ32_CAB(X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            emitOverflowSigned(buildParameters, concat, ip->node);
            MK_BINOPEQ64_CAB(X64Op::SUB);
            break;
        case ByteCodeOp::AffectOpMinusEqU64:
            MK_BINOPEQ64_CAB(X64Op::SUB);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            MK_BINOPEQF32_CAB(X64Op::FSUB);
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            MK_BINOPEQF64_CAB(X64Op::FSUB);
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            MK_BINOPEQ8_CAB(X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpPlusEqU8:
            MK_BINOPEQ8_CAB(X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            MK_BINOPEQ16_CAB(X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpPlusEqU16:
            MK_BINOPEQ16_CAB(X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            MK_BINOPEQ32_CAB(X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpPlusEqU32:
            MK_BINOPEQ32_CAB(X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            MK_BINOPEQ64_CAB(X64Op::ADD);
            emitOverflowSigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpPlusEqU64:
            MK_BINOPEQ64_CAB(X64Op::ADD);
            emitOverflowUnsigned(buildParameters, concat, ip->node);
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            MK_BINOPEQF32_CAB(X64Op::FADD);
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            MK_BINOPEQF64_CAB(X64Op::FADD);
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
            MK_BINOP32_CAB(emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetG(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            MK_BINOP64_CAB(emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetG(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterU32:
            MK_BINOP32_CAB(emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            MK_BINOP64_CAB(emit_Cmp64_Indirect, emit_Cmp64);
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
            MK_BINOP32_CAB(emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetGE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterEqS64:
            MK_BINOP64_CAB(emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetGE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterEqU32:
            MK_BINOP32_CAB(emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetAE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterEqU64:
            MK_BINOP64_CAB(emit_Cmp64_Indirect, emit_Cmp64);
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
            MK_BINOP32_CAB(emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetL(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerS64:
            MK_BINOP64_CAB(emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetL(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerU32:
            MK_BINOP32_CAB(emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetB(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerU64:
            MK_BINOP64_CAB(emit_Cmp64_Indirect, emit_Cmp64);
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
            MK_BINOP32_CAB(emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetLE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerEqS64:
            MK_BINOP64_CAB(emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetLE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerEqU32:
            MK_BINOP32_CAB(emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetBE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerEqU64:
            MK_BINOP64_CAB(emit_Cmp64_Indirect, emit_Cmp64);
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
            MK_BINOP32_CAB(emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpEqual64:
            MK_BINOP64_CAB(emit_Cmp64_Indirect, emit_Cmp64);
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
            MK_BINOP32_CAB(emit_Cmp32_Indirect, emit_Cmp32);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpNotEqual64:
            MK_BINOP64_CAB(emit_Cmp64_Indirect, emit_Cmp64);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::IntrinsicStrCmp:
            SWAG_ASSERT(sizeParamsStack >= 5 * sizeof(Register));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 16, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 24, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 32, RAX, RSP);
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            emitCall(pp, "@strcmp");
            break;
        case ByteCodeOp::IntrinsicTypeCmp:
            SWAG_ASSERT(sizeParamsStack >= 4 * sizeof(Register));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 16, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 24, RAX, RSP);
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->d.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            emitCall(pp, "@typecmp");
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
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString2("\x34\x01"); // xor al, 1
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::NegS32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
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
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\x48\xf7\xd0"); // not rax
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::JumpIfTrue:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfFalse:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test16(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test32(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test64(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test8(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test16(pp, RAX, RAX);
            BackendX64Inst::emit_Jump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
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
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRef16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRef32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRef64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRefPointer:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Add64_Immediate(pp, ip->c.u32, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRefStringSlice:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 8, RCX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
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
        case ByteCodeOp::GetFromStack64x2:
            BackendX64Inst::emit_Load64_Indirect(pp, offsetStack + ip->b.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, offsetStack + ip->d.u32, RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::ClearMaskU32:
            if (ip->b.u32 == 0xFF)
            {
                concat.addString2("\x0F\xB6"); // movzx
                BackendX64Inst::emit_ModRM(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            else if (ip->b.u32 == 0xFFFF)
            {
                concat.addString2("\x0F\xB7"); // movzx
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
                concat.addString3("\x48\x0F\xB6"); // movzx
                BackendX64Inst::emit_ModRM(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            else if (ip->b.u32 == 0xFFFF)
            {
                concat.addString3("\x48\x0F\xB7"); // movzx
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
            BackendX64Inst::emit_Store8_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store16_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store32_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer64OffVB32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, ip->a.u32, 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointerX:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Clear64(pp, RDX);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, R8);
            emitCall(pp, "memset");
            break;
        case ByteCodeOp::SetZeroAtPointerXRB:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Clear64(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_imul64_RAX(pp, ip->c.u64);
            BackendX64Inst::emit_Copy64(pp, RAX, R8);
            emitCall(pp, "memset");
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
            if (ip->b.u32 <= 128)
            {
                auto toClear = ip->b.u32;
                auto offset  = ip->a.u32;
                while (toClear >= 8)
                {
                    BackendX64Inst::emit_Store64_Immediate(pp, offsetStack + offset, 0, RDI);
                    toClear -= 8;
                    offset += 8;
                }
                while (toClear >= 4)
                {
                    BackendX64Inst::emit_Store32_Immediate(pp, offsetStack + offset, 0, RDI);
                    toClear -= 4;
                    offset += 4;
                }
                while (toClear >= 2)
                {
                    BackendX64Inst::emit_Store16_Immediate(pp, offsetStack + offset, 0, RDI);
                    toClear -= 2;
                    offset += 2;
                }
                while (toClear >= 1)
                {
                    BackendX64Inst::emit_Store8_Immediate(pp, offsetStack + offset, 0, RDI);
                    toClear -= 1;
                    offset += 1;
                }
            }
            else
            {
                BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetStack + ip->a.u32, RCX, RDI);
                BackendX64Inst::emit_Clear64(pp, RDX);
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, R8);
                emitCall(pp, "memset");
            }
            break;
        }

        case ByteCodeOp::SetAtPointer8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store8_Immediate(pp, 0, ip->b.u8, RAX);
            else
            {
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
                BackendX64Inst::emit_Store8_Indirect(pp, 0, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store16_Immediate(pp, 0, ip->b.u16, RAX);
            else
            {
                BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
                BackendX64Inst::emit_Store16_Indirect(pp, 0, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Store32_Immediate(pp, 0, ip->b.u32, RAX);
            else
            {
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
                BackendX64Inst::emit_Store32_Indirect(pp, 0, RCX, RAX);
            }
            break;
        case ByteCodeOp::SetAtPointer64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)
                BackendX64Inst::emit_Store64_Immediate(pp, 0, ip->b.u64, RAX);
            else if (ip->flags & BCI_IMM_B)
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RCX);
                BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RAX);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RAX);
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
        case ByteCodeOp::MakeTypeSegPointer:
            BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symTSIndex, ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
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
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RDX);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::MemCpy16:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RDX);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::MemCpy32:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RDX);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::MemCpy64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RDX);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);
            break;
        case ByteCodeOp::MemCpyX:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->c.u64, R8);
            emitCall(pp, "memcpy");
            break;

        case ByteCodeOp::IntrinsicMemCpy:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), R8, RDI);
            emitCall(pp, "memcpy");
            break;

        case ByteCodeOp::IntrinsicMemMove:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), R8, RDI);
            emitCall(pp, "memmove");
            break;

        case ByteCodeOp::IntrinsicMemSet:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), R8, RDI);
            emitCall(pp, "memset");
            break;

        case ByteCodeOp::IntrinsicMemCmp:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->d.u32), R8, RDI);
            emitCall(pp, "memcmp");
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::IntrinsicCStrLen:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            emitCall(pp, "strlen");
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::IntrinsicAlloc:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            emitCall(pp, "malloc");
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicRealloc:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
            emitCall(pp, "realloc");
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicFree:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            emitCall(pp, "free");
            break;

        case ByteCodeOp::InternalInitStackTrace:
            emitCall(pp, "__initStackTrace");
            break;
        case ByteCodeOp::InternalStackTrace:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RSP);
            emitCall(pp, "__stackTrace");
            break;
        case ByteCodeOp::InternalPanic:
            emitInternalPanic(buildParameters, ip->node, (const char*) ip->d.pointer);
            break;

        case ByteCodeOp::IntrinsicGetContext:
            BackendX64Inst::emit_Symbol_RelocationValue(pp, RAX, pp.symPI_contextTlsId, 0);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            emitCall(pp, "__tlsGetValue");
            break;
        case ByteCodeOp::IntrinsicSetContext:
            BackendX64Inst::emit_Symbol_RelocationValue(pp, RAX, pp.symPI_contextTlsId, 0);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            emitCall(pp, "__tlsSetValue");
            break;

        case ByteCodeOp::IntrinsicArguments:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            emitCall(pp, "@args");
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
        case ByteCodeOp::IntrinsicPrintString:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            emitCall(pp, ((AstFuncDecl*) ip->node->resolvedSymbolOverload->node)->extension->bc->callName());
            break;
        case ByteCodeOp::IntrinsicPrintS64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            emitCall(pp, ((AstFuncDecl*) ip->node->resolvedSymbolOverload->node)->extension->bc->callName());
            break;
        case ByteCodeOp::IntrinsicPrintF64:
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            BackendX64Inst::emit_StoreF64_Indirect(pp, 0, XMM0, RSP);
            emitCall(pp, ((AstFuncDecl*) ip->node->resolvedSymbolOverload->node)->extension->bc->callName());
            break;
        case ByteCodeOp::IntrinsicErrorMsg:
            SWAG_ASSERT(sizeParamsStack >= 3 * sizeof(Register));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 16, RAX, RSP);
            emitCall(pp, "@errormsg");
            break;
        case ByteCodeOp::IntrinsicPanic:
            SWAG_ASSERT(sizeParamsStack >= 3 * sizeof(Register));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 16, RAX, RSP);
            emitCall(pp, "@panic");
            break;
        case ByteCodeOp::IntrinsicInterfaceOf:
            SWAG_ASSERT(sizeParamsStack >= 3 * sizeof(Register));
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 16, RAX, RSP);
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            emitCall(pp, "@interfaceof");
            break;

        case ByteCodeOp::CopyRCtoRR:
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            BackendX64Inst::emit_Load64_Indirect(pp, 16 + sizeStack + regOffset(0), RAX, RDI);
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
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            BackendX64Inst::emit_Load64_Indirect(pp, 16 + sizeStack + regOffset(0), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RAX);

            BackendX64Inst::emit_Load64_Indirect(pp, 16 + sizeStack + regOffset(1), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RAX);
            break;

        case ByteCodeOp::CopyRCtoRT:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetRT + regOffset(0), RAX, RDI);
            break;

        case ByteCodeOp::CopyRRtoRC:
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            BackendX64Inst::emit_Load64_Indirect(pp, 16 + sizeStack, RAX, RDI);
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
            auto typeFuncCall = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->d.pointer, TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);
            bool foreign      = ip->c.b;
            if (!foreign)
            {
                // We are close to the byte code, as all PushRaParams are already in the correct order for variadics.
                // We need the RAX register to address the stack where all are stored.
                // There's 2 more PushRAParam to come after CopySPVaargs, so offset is 16.
                // We also need to take care of real parameters (offset is ip->b.u32)
                // We also need to take care of the return registers, which are always first
                BackendX64Inst::emit_LoadAddress_Indirect(pp, (uint8_t)(16 + ip->b.u32 + (typeFuncCall->numReturnRegisters() * 8)), RAX, RSP);
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
                // We need to flatten all variadic registers, in order, in the stack, and emit the address of that array
                // We compute the number of variadic registers by removing registers of normal parameters (ip->b.u32)
                int      idxParam          = (int) pushRAParams.size() - (ip->b.u32 / sizeof(Register)) - 1;
                uint32_t variadicStackSize = idxParam * sizeof(Register);
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
        case ByteCodeOp::GetFromStackParam64:
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            BackendX64Inst::emit_Load64_Indirect(pp, 16 + sizeStack + regOffset(ip->c.u32) + regOffset(typeFunc->numReturnRegisters()), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::MakeStackPointerParam:
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            BackendX64Inst::emit_LoadAddress_Indirect(pp, 16 + sizeStack + regOffset(ip->c.u32) + regOffset(typeFunc->numReturnRegisters()), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::MakeLambda:
        {
            auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->b.pointer, AstNodeKind::FuncDecl);
            SWAG_ASSERT(funcNode);

            Utf8 name;

            if (funcNode->attributeFlags & ATTRIBUTE_FOREIGN)
            {
                TypeInfoFuncAttr* typeFuncNode = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
                ComputedValue     foreignValue;
                typeFuncNode->attributes.getValue("swag.foreign", "function", foreignValue);
                SWAG_ASSERT(!foreignValue.text.empty());
                name = foreignValue.text;
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
                name = funcBC->callName();
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
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, SWAG_LAMBDA_BC_MARKER, RCX);
            BackendX64Inst::emit_Op64(pp, RAX, RCX, X64Op::AND);
            BackendX64Inst::emit_Test64(pp, RCX, RCX);
            concat.addString2("\x0f\x85"); // jnz ???????? => jump to bytecode lambda
            concat.addU32(0);
            auto jumpToBCAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpToBCOffset = concat.totalCount();

            // Test if it's a foreign lambda
            BackendX64Inst::emit_Load64_Immediate(pp, SWAG_LAMBDA_FOREIGN_MARKER, RCX);
            BackendX64Inst::emit_Op64(pp, RAX, RCX, X64Op::AND);
            BackendX64Inst::emit_Test64(pp, RCX, RCX);
            concat.addString2("\x0f\x85"); // jnz ???????? => jump to foreign lambda
            concat.addU32(0);
            auto jumpToForeignAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpToForeignOffset = concat.totalCount();

            // Local lambda
            //////////////////
            BackendX64Inst::emit_Copy64(pp, RAX, R10);
            emitLocalCallParameters(pp, sizeParamsStack, typeFuncBC, offsetRT, pushRAParams, pushRVParams);
            concat.addString3("\x41\xFF\xD2"); // call r10

            concat.addString1("\xe9"); // jmp ???????? => jump after bytecode lambda
            concat.addU32(0);
            auto jumpBCToAfterAddr   = (uint32_t*) concat.getSeekPtr() - 1;
            auto jumpBCToAfterOffset = concat.totalCount();

            // Foreign lambda
            //////////////////
            *jumpToForeignAddr = concat.totalCount() - jumpToForeignOffset;

            BackendX64Inst::emit_Load64_Immediate(pp, ~SWAG_LAMBDA_FOREIGN_MARKER, RCX);
            BackendX64Inst::emit_Op64(pp, RCX, RAX, X64Op::AND);
            BackendX64Inst::emit_Copy64(pp, RAX, R10);
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

            BackendX64Inst::emit_Copy64(pp, RAX, RCX);
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
            pushRAParams.clear();
            pushRVParams.clear();
            break;

        case ByteCodeOp::LocalCall:
        {
            auto              funcBC     = (ByteCode*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;
            emitLocalCallParameters(pp, sizeParamsStack, typeFuncBC, offsetRT, pushRAParams, pushRVParams);
            emitCall(pp, funcBC->callName());
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
                emitCall(pp, "powf");
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
                emitCall(pp, "pow");
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
                emitCall(pp, "sinf");
                break;
            case TokenId::IntrinsicCos:
                emitCall(pp, "cosf");
                break;
            case TokenId::IntrinsicTan:
                emitCall(pp, "tanf");
                break;
            case TokenId::IntrinsicSinh:
                emitCall(pp, "sinhf");
                break;
            case TokenId::IntrinsicCosh:
                emitCall(pp, "coshf");
                break;
            case TokenId::IntrinsicTanh:
                emitCall(pp, "tanhf");
                break;
            case TokenId::IntrinsicASin:
                emitCall(pp, "asinf");
                break;
            case TokenId::IntrinsicACos:
                emitCall(pp, "acosf");
                break;
            case TokenId::IntrinsicATan:
                emitCall(pp, "atanf");
                break;
            case TokenId::IntrinsicLog:
                emitCall(pp, "logf");
                break;
            case TokenId::IntrinsicLog2:
                emitCall(pp, "log2f");
                break;
            case TokenId::IntrinsicLog10:
                emitCall(pp, "log10f");
                break;
            case TokenId::IntrinsicFloor:
                emitCall(pp, "floorf");
                break;
            case TokenId::IntrinsicCeil:
                emitCall(pp, "ceilf");
                break;
            case TokenId::IntrinsicTrunc:
                emitCall(pp, "truncf");
                break;
            case TokenId::IntrinsicRound:
                emitCall(pp, "roundf");
                break;
            case TokenId::IntrinsicAbs:
                BackendX64Inst::emit_Load64_Immediate(pp, 0x7FFFFFFF, RAX);
                BackendX64Inst::emit_CopyF64(pp, RAX, XMM1);
                concat.addString3("\x0F\x54\xC1"); // andps xmm0, xmm1
                break;
            case TokenId::IntrinsicExp:
                emitCall(pp, "expf");
                break;
            case TokenId::IntrinsicExp2:
                emitCall(pp, "exp2f");
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
                emitCall(pp, "sin");
                break;
            case TokenId::IntrinsicCos:
                emitCall(pp, "cos");
                break;
            case TokenId::IntrinsicTan:
                emitCall(pp, "tan");
                break;
            case TokenId::IntrinsicSinh:
                emitCall(pp, "sinh");
                break;
            case TokenId::IntrinsicCosh:
                emitCall(pp, "cosh");
                break;
            case TokenId::IntrinsicTanh:
                emitCall(pp, "tanh");
                break;
            case TokenId::IntrinsicASin:
                emitCall(pp, "asin");
                break;
            case TokenId::IntrinsicACos:
                emitCall(pp, "acos");
                break;
            case TokenId::IntrinsicATan:
                emitCall(pp, "atan");
                break;
            case TokenId::IntrinsicLog:
                emitCall(pp, "log");
                break;
            case TokenId::IntrinsicLog2:
                emitCall(pp, "log2");
                break;
            case TokenId::IntrinsicLog10:
                emitCall(pp, "log10");
                break;
            case TokenId::IntrinsicFloor:
                emitCall(pp, "floor");
                break;
            case TokenId::IntrinsicCeil:
                emitCall(pp, "ceil");
                break;
            case TokenId::IntrinsicTrunc:
                emitCall(pp, "trunc");
                break;
            case TokenId::IntrinsicRound:
                emitCall(pp, "round");
                break;
            case TokenId::IntrinsicAbs:
                BackendX64Inst::emit_Load64_Immediate(pp, 0x7FFFFFFF'FFFFFFFF, RAX);
                BackendX64Inst::emit_CopyF64(pp, RAX, XMM1);
                concat.addString4("\x66\x0F\x54\xC1"); // andpd xmm0, xmm1
                break;
            case TokenId::IntrinsicExp:
                emitCall(pp, "exp");
                break;
            case TokenId::IntrinsicExp2:
                emitCall(pp, "exp2");
                break;
            }

            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        }

        case ByteCodeOp::IntrinsicGetErr:
        {
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            emitCall(pp, "__geterr");
            break;
        }

        case ByteCodeOp::IntrinsicSetErr:
        {
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
            emitCall(pp, "@seterr");
            break;
        }

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

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    return ok;
}

void BackendX64::emitLocalCallParameters(X64PerThread& pp, uint32_t sizeParamsStack, TypeInfoFuncAttr* typeFuncBC, uint32_t stackRR, const VectorNative<uint32_t>& pushRAParams, const VectorNative<pair<uint32_t, uint32_t>>& pushRVParams)
{
    uint32_t sizeStack = (uint32_t)(pushRAParams.size() * sizeof(Register));
    sizeStack += typeFuncBC->numReturnRegisters() * sizeof(Register);

    // pushRVParams are for variadics. It contains registers like pushRAParams, but also the sizeof of what needs to be pushed
    if (pushRVParams.size())
    {
        auto sizeOf = pushRVParams[0].second;
        sizeStack += (uint32_t) pushRVParams.size() * sizeOf;
    }

    auto offset = sizeStack;
    MK_ALIGN16(sizeStack);
    SWAG_ASSERT(sizeStack <= sizeParamsStack);

    // Emit all push params
    for (int iParam = 0; iParam < pushRVParams.size(); iParam++)
    {
        auto sizeOf = pushRVParams[0].second;
        auto reg    = pushRVParams[iParam].first;
        offset -= sizeOf;
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
    }

    // Emit all push params
    for (int iParam = 0; iParam < pushRAParams.size(); iParam++)
    {
        offset -= 8;
        BackendX64Inst::emit_Load64_Indirect(pp, regOffset(pushRAParams[iParam]), RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, offset, RAX, RSP);
    }

    // Return registers are push first
    for (int j = (int) typeFuncBC->numReturnRegisters() - 1; j >= 0; j--)
    {
        offset -= 8;
        BackendX64Inst::emit_LoadAddress_Indirect(pp, stackRR + regOffset(j), RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, offset, RAX, RSP);
    }

    SWAG_ASSERT(offset == 0);
}

void BackendX64::emitForeignCallResult(X64PerThread& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT)
{
    // Store result to rt0
    auto returnType = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    if (returnType != g_TypeMgr.typeInfoVoid)
    {
        if ((returnType->kind == TypeInfoKind::Slice) ||
            (returnType->kind == TypeInfoKind::Interface) ||
            (returnType->kind == TypeInfoKind::TypeSet) ||
            (returnType->isNative(NativeTypeKind::Any)) ||
            (returnType->isNative(NativeTypeKind::String)) ||
            (returnType->flags & TYPEINFO_RETURN_BY_COPY))
        {
            // Return by parameter
        }
        else
        {
            BackendX64Inst::emit_Store64_Indirect(pp, offsetRT, RAX, RDI);
        }
    }
}

bool BackendX64::emitForeignCall(X64PerThread& pp, Module* moduleToGen, ByteCodeInstruction* ip, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
{
    auto              nodeFunc   = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

    // Get function name
    ComputedValue foreignValue;
    Utf8          funcName;
    if (typeFuncBC->attributes.getValue("swag.foreign", "function", foreignValue) && !foreignValue.text.empty())
        funcName = foreignValue.text;
    else
        funcName = nodeFunc->token.text;

    // Push parameters
    SWAG_CHECK(emitForeignCallParameters(pp, moduleToGen, offsetRT, typeFuncBC, pushRAParams));

    auto& concat = pp.concat;

    // Need to make a far call
    concat.addU8(0xFF); // call
    concat.addU8(0x15);

    if (funcName == "Argb2039.extract")
        funcName = funcName;

    // Dll imported function name will have "__imp_" before (imported mangled name)
    CoffRelocation reloc;
    reloc.virtualAddress = concat.totalCount() - pp.textSectionOffset;
    auto callSym         = getOrAddSymbol(pp, "__imp_" + funcName, CoffSymbolKind::Extern);
    reloc.symbolIndex    = callSym->index;
    reloc.type           = IMAGE_REL_AMD64_REL32;
    pp.relocTableTextSection.table.push_back(reloc);
    concat.addU32(0);

    // Store result
    emitForeignCallResult(pp, typeFuncBC, offsetRT);

    return true;
}

bool BackendX64::emitForeignCallParameters(X64PerThread& pp, Module* moduleToGen, uint32_t offsetRT, TypeInfoFuncAttr* typeFuncBC, const VectorNative<uint32_t>& pushRAParams)
{
    int numCallParams = (int) typeFuncBC->parameters.size();

    VectorNative<uint32_t>  paramsRegisters;
    VectorNative<TypeInfo*> paramsTypes;

    int indexParam = (int) pushRAParams.size() - 1;

    // Variadic are first
    if (numCallParams)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters.back()->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic || typeParam->kind == TypeInfoKind::TypedVariadic)
        {
            auto index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);

            index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);
            numCallParams--;
        }
    }

    // All parameters
    for (int i = 0; i < (int) numCallParams; i++)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters[i]->typeInfo);

        auto index = pushRAParams[indexParam--];

        if (typeParam->kind == TypeInfoKind::Pointer ||
            typeParam->kind == TypeInfoKind::Struct ||
            typeParam->kind == TypeInfoKind::Lambda ||
            typeParam->kind == TypeInfoKind::Array)
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);
        }
        else if (typeParam->kind == TypeInfoKind::Slice ||
                 typeParam->isNative(NativeTypeKind::String))
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);
            index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);
        }
        else if (typeParam->isNative(NativeTypeKind::Any) ||
                 typeParam->kind == TypeInfoKind::Interface ||
                 typeParam->kind == TypeInfoKind::TypeSet)
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);
            index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);
        }
        else
        {
            if (typeParam->sizeOf > sizeof(void*))
                return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid parameter type");
            paramsRegisters.push_back(index);
            paramsTypes.push_back(typeParam);
        }
    }

    // Return by parameter
    auto returnType   = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    bool returnByCopy = returnType->flags & TYPEINFO_RETURN_BY_COPY;
    if (returnType->kind == TypeInfoKind::Slice ||
        returnType->kind == TypeInfoKind::Interface ||
        returnType->kind == TypeInfoKind::TypeSet ||
        returnType->isNative(NativeTypeKind::Any) ||
        returnType->isNative(NativeTypeKind::String))
    {
        paramsRegisters.push_back(offsetRT);
        paramsTypes.push_back(g_TypeMgr.typeInfoUndefined);
    }
    else if (returnByCopy)
    {
        paramsRegisters.push_back(offsetRT);
        paramsTypes.push_back(g_TypeMgr.typeInfoUndefined);
    }

    // Set the first 4 parameters. Can be return register, or function parameter.
    for (int i = 0; i < min(4, (int) paramsRegisters.size()); i++)
    {
        auto type = paramsTypes[i];
        auto r    = paramsRegisters[i];

        static const uint8_t reg4[]  = {RCX, RDX, R8, R9};
        static const uint8_t regf4[] = {XMM0, XMM1, XMM2, XMM3};

        // This is a return register
        if (type == g_TypeMgr.typeInfoUndefined)
        {
            if (returnByCopy)
                BackendX64Inst::emit_Load64_Indirect(pp, r, reg4[i], RDI);
            else
                BackendX64Inst::emit_LoadAddress_Indirect(pp, r, reg4[i], RDI);
        }

        // This is a normal parameter, which can be float or integer
        else
        {
            if (type->flags & TYPEINFO_FLOAT)
                BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(r), regf4[i], RDI);
            else
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(r), reg4[i], RDI);
        }
    }

    // Store all parameters after 4 on the stack, with an offset of 4 * sizeof(uint64_t)
    // because the first 4 x uint64_t are for the first 4 parameters (even if they are passed in
    // registers, this is the x64 cdecl convention...)
    if (paramsRegisters.size() > 4)
    {
        uint32_t offsetStack = 4 * sizeof(uint64_t);
        for (int i = 4; i < (int) paramsRegisters.size(); i++)
        {
            // This is for a return value
            if (paramsTypes[i] == g_TypeMgr.typeInfoUndefined)
            {
                if (returnByCopy)
                    BackendX64Inst::emit_Load64_Indirect(pp, paramsRegisters[i], RAX, RDI);
                else
                    BackendX64Inst::emit_LoadAddress_Indirect(pp, paramsRegisters[i], RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);
            }

            // This is for a normal parameter
            else
            {
                auto sizeOf = paramsTypes[i]->sizeOf;
                switch (sizeOf)
                {
                case 1:
                    BackendX64Inst::emit_Load8_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    BackendX64Inst::emit_Store8_Indirect(pp, offsetStack, RAX, RSP);
                    break;
                case 2:
                    BackendX64Inst::emit_Load16_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    BackendX64Inst::emit_Store16_Indirect(pp, offsetStack, RAX, RSP);
                    break;
                case 4:
                    BackendX64Inst::emit_Load32_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    BackendX64Inst::emit_Store32_Indirect(pp, offsetStack, RAX, RSP);
                    break;
                case 8:
                    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);
                    break;
                default:
                    return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid parameter type");
                }
            }

            // Push is always aligned
            offsetStack += 8;
        }
    }

    return true;
}

void BackendX64::emitByteCodeLambdaParams(X64PerThread& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
{
    int idxReg = 0;
    for (int idxParam = typeFuncBC->numReturnRegisters() - 1; idxParam >= 0; idxParam--, idxReg++)
    {
        switch (idxReg)
        {
        case 0:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetRT, RDX, RDI);
            break;
        case 1:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetRT + sizeof(Register), R8, RDI);
            break;
        }
    }

    // When calling a bytecode callback in x64, we must respect the passByValue convention, even if it's not
    // the case internaly for local calls.
    uint32_t stackOffset = 0;
    for (int idxParam = (int) pushRAParams.size() - 1; idxParam >= 0; idxParam--, idxReg++)
    {
        auto typeParam = typeFuncBC->registerIdxToType(idxReg - typeFuncBC->numReturnRegisters());

        static const uint8_t idxToReg[4] = {RDX, R8, R9};

        // Pass by value
        if (passByValue(typeParam))
        {
            stackOffset += sizeof(Register);
            if (idxReg <= 2)
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(pushRAParams[idxParam]), idxToReg[idxReg], RDI);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(pushRAParams[idxParam]), RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, RAX, RSP);
            }
        }

        // Pass by register pointer
        else
        {
            stackOffset += sizeof(Register);
            if (idxReg <= 2)
            {
                BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(pushRAParams[idxParam]), idxToReg[idxReg], RDI);
            }
            else
            {
                BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(pushRAParams[idxParam]), RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, RAX, RSP);
            }
        }
    }
}