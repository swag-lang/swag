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

bool BackendX64::emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* bc)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    if (bc->node->attributeFlags & ATTRIBUTE_PRINTBYTECODE)
        bc = bc;

    node->computeFullNameForeign(true);

    // Symbol
    getOrAddSymbol(pp, node->fullnameForeign, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
    pp.directives += format("/EXPORT:%s ", node->fullnameForeign.c_str());

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
        if (typeParam->kind == TypeInfoKind::Variadic)
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

    concat.addU8(0x57); // push rdi
    while (sizeStack % 16)
        sizeStack += 8; // Align to 16 bytes
    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, sizeStack);
    BackendX64Inst::emit_Copy64(pp, RDI, RSP);

    // Need to save return register if needed
    if (!returnByCopy && typeFunc->numReturnRegisters() == 2)
    {
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
                concat.addString3("\x48\x8d\x87"); // lea rax, [rdi + ????????]
                concat.addU32(offsetRetCopy);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(i), RAX, RDI);
                setCalleeParameter(pp, typeParam, numCallRegisters, offsetRetCopy, sizeStack);
            }
            else
            {
                concat.addString3("\x48\x8d\x87"); // lea rax, [rdi + ????????]
                concat.addU32(regOffset(i));
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

    BackendX64Inst::emit_Add_Cst32_To_RSP(pp, sizeStack);
    concat.addU8(0x5F); // pop rdi
    concat.addU8(0xC3); // ret
    return true;
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
    uint32_t offsetRR    = 0;

    // For float load
    // (should be reserved only if we have floating point operations in that function)
    // In order we have :
    // RC at 0 (bc->maxReservedRegisterRC)
    // RT (max call results)
    // Local function stack
    // FLT
    offsetRT    = bc->maxReservedRegisterRC * sizeof(Register);
    offsetStack = offsetRT + bc->maxCallResults * sizeof(Register);
    offsetRR    = offsetStack + typeFunc->stackSize;
    offsetFLT   = offsetRR + 2 * sizeof(Register);
    sizeStack   = offsetFLT + 8;

    // RDI will be a pointer to the stack, and the list of registers is stored at the start
    // of the stack
    concat.addU8(0x57); // push rdi
    while (sizeStack % 16)
        sizeStack++; // Align to 16 bytes
    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, sizeStack);
    BackendX64Inst::emit_Copy64(pp, RDI, RSP);

    // C calling convention, space for at least 4 parameters when calling a function
    // (should be reserved only if we have a call)
    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 4 * sizeof(uint64_t));

    auto                   ip = bc->out;
    VectorNative<uint32_t> pushRAParams;
    uint32_t               isVariadic = 0;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
        {
            SWAG_ASSERT(!(ip->flags & BCI_JUMP_DEST));
            continue;
        }

        if (ip->flags & BCI_JUMP_DEST)
            getOrCreateLabel(pp, i);

        //concat.addU8(0x90); // NOP TO REMOVE

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
        case ByteCodeOp::AddVBtoRA32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u32 += ", ip->b.u32, ";");
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, RBX);
            concat.addString2("\x01\x18"); // add dword ptr [rax], ebx
            break;

        case ByteCodeOp::ClearRA:
            BackendX64Inst::emit_Clear64(pp, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CopyRBtoRA:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = r[", ip->b.u32, "];");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CopyRBAddrToRA:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = (__u8_t*) &r[", ip->b.u32, "];");
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CastBool8:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u8 ? 1 : 0;");
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_AL_With_AL(pp);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastBool16:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u16 ? 1 : 0;");
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_AX_With_AX(pp);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastBool32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u32 ? 1 : 0;");
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_EAX_With_EAX(pp);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastBool64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u64 ? 1 : 0;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_RAX_With_RAX(pp);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CastS8S16:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16 = (__s16_t) r[", ip->a.u32, "].s8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastS16S32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = (__s32_t) r[", ip->a.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AX_To_EAX(pp);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastS32S64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (__f64_t) r[", ip->a.u32, "].f32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_EAX_To_RAX(pp);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastS32F32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (__f32_t) r[", ip->a.u32, "].s32;");
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString4("\xf3\x0f\x2a\xc0"); // cvtsi2ss xmm0, eax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastU32F32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (__f32_t) r[", ip->a.u32, "].u32;");
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\xf3\x48\x0f\x2a\xc0"); // cvtsi2ss xmm0, rax
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastF32S32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = (__s32_t) r[", ip->a.u32, "].f32;");
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString4("\xf3\x0f\x2c\xc0"); // cvttss2si eax, xmm0
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastF32F64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (__f64_t) r[", ip->a.u32, "].f32;");
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString4("\xf3\x0f\x5a\xc0"); // cvtss2sd xmm0, xmm0
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastF64F32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (__f32_t) r[", ip->a.u32, "].f64;");
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString4("\xf2\x0f\x5a\xc0"); // cvtsd2ss xmm0, xmm0
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastS64F64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (__f64_t) r[", ip->a.u32, "].s64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\xf2\x48\x0f\x2a\xc0"); // cvtsi2sd xmm0, rax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::CastF64S64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = (__s32_t) r[", ip->a.u32, "].f64;");
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            concat.addString5("\xf2\x48\x0f\x2c\xc0"); // cvttss2si rax, xmm0
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::CastU64F64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u64 = (__f32_t) r[", ip->a.u32, "].f64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString5("\xf2\x48\x0f\x2a\xc0"); // cvtsi2ss xmm0, rax
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;

        case ByteCodeOp::BinOpShiftLeftU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd3\xe0"); // shl eax, cl
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::BinOpShiftLeftU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xd3\xe0"); // shl rax, cl
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::BinOpShiftRightS32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd3\xf8"); // sar eax, cl
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::BinOpShiftRightS64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xd3\xf8"); // sar rax, cl
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::BinOpShiftRightU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd3\xe8"); // shr eax, cl
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::BinOpShiftRightU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xd3\xe8"); // shr rax, cl
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::AffectOpShiftLeftEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd2\x20"); // sal byte ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xd3\x20"); // sal word ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS32:
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd3\x20"); // sal dword ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xd3\x20"); // sal qword ptr [rax], cl
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd2\x38"); // sar byte ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xd3\x38"); // sar word ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd3\x38"); // sar dword ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xd3\x38"); // sar qword ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftRightEqU8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd2\x28"); // shr byte ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x66\xd3\x28"); // shr word ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString2("\xd3\x28"); // shr dword ptr [rax], cl
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            concat.addString3("\x48\xd3\x28"); // shr qword ptr [rax], cl
            break;

        case ByteCodeOp::BinOpShiftRightU64VB:
            //concat.addStringFormat("r[%u].u64 >>= %u;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, RCX);
            concat.addString3("\x48\xd3\xe8"); // shr rax, cl
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
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
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x30\x03"); // xor [rbx], al
            break;
        case ByteCodeOp::AffectOpXOrEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x66\x31\x03"); // xor [rbx], ax
            break;
        case ByteCodeOp::AffectOpXOrEqS32:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x31\x03"); // xor [rbx], eax
            break;
        case ByteCodeOp::AffectOpXOrEqS64:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x48\x31\x03"); // xor [rbx], rax
            break;

        case ByteCodeOp::AffectOpOrEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x08\x03"); // xor [rbx], al
            break;
        case ByteCodeOp::AffectOpOrEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x66\x09\x03"); // or [rbx], ax
            break;
        case ByteCodeOp::AffectOpOrEqS32:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x09\x03"); // or [rbx], eax
            break;
        case ByteCodeOp::AffectOpOrEqS64:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x48\x09\x03"); // or [rbx], rax
            break;

        case ByteCodeOp::AffectOpAndEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x20\x03"); // and [rbx], al
            break;
        case ByteCodeOp::AffectOpAndEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x66\x21\x03"); // and [rbx], ax
            break;
        case ByteCodeOp::AffectOpAndEqS32:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x21\x03"); // and [rbx], eax
            break;
        case ByteCodeOp::AffectOpAndEqS64:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x48\x21\x03"); // and [rbx], rax
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString2("\xf6\xeb"); // imul bl
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x66\xf7\xeb"); // imul bx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString2("\xf7\xeb"); // imul ebx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x48\xf7\xeb"); // imul rbx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpMulEqF32:
            //CONCAT_STR_2(concat, "*(__f32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].f32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf3\x0f\x59\xc1"); // mulss xmm0, xmm1
            BackendX64Inst::emit_StoreF32_Indirect(pp, 0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMulEqF64:
            //CONCAT_STR_2(concat, "*(__f64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].f64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf2\x0f\x59\xc1"); // mulss xmm0, xmm1
            BackendX64Inst::emit_StoreF64_Indirect(pp, 0, XMM0, RAX);
            break;

        case ByteCodeOp::AffectOpModuloEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString2("\xf6\xfb"); // idiv al, bl
            concat.addString2("\x88\xe0"); // mov al, ah
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpModuloEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            concat.addString2("\x66\x99"); // cwd
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x66\xf7\xfb"); // idiv ax, bx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RDX, RBX);
            break;
        case ByteCodeOp::AffectOpModuloEqS32:
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            concat.addString1("\x99"); // cdq
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString2("\xf7\xfb"); // idiv eax, ebx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RDX, RBX);
            break;
        case ByteCodeOp::AffectOpModuloEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            concat.addString2("\x48\x99"); // cqo
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x48\xf7\xfb"); // idiv rax, rbx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RDX, RBX);
            break;
        case ByteCodeOp::AffectOpModuloEqU8:
            //CONCAT_STR_2(concat, "*(__u8_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString2("\xf6\xf3"); // idiv al, bl
            concat.addString2("\x88\xe0"); // mov al, ah
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpModuloEqU16:
            //CONCAT_STR_2(concat, "*(__u16_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear16(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x66\xf7\xf3"); // idiv ax, bx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RDX, RBX);
            break;
        case ByteCodeOp::AffectOpModuloEqU32:
            //CONCAT_STR_2(concat, "*(__u32_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear32(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString2("\xf7\xf3"); // idiv eax, ebx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RDX, RBX);
            break;
        case ByteCodeOp::AffectOpModuloEqU64:
            //CONCAT_STR_2(concat, "*(__u64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear64(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x48\xf7\xf3"); // idiv rax, rbx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RDX, RBX);
            break;

        case ByteCodeOp::AffectOpDivEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString2("\xf6\xfb"); // idiv al, bl
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpDivEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            concat.addString2("\x66\x99"); // cwd
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x66\xf7\xfb"); // idiv ax, bx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpDivEqS32:
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            concat.addString1("\x99"); // cdq
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString2("\xf7\xfb"); // idiv eax, ebx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpDivEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            concat.addString2("\x48\x99"); // cqo
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x48\xf7\xfb"); // idiv rax, rbx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpDivEqU8:
            //CONCAT_STR_2(concat, "*(__u8_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString2("\xf6\xf3"); // div al, bl
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpDivEqU16:
            //CONCAT_STR_2(concat, "*(__u16_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear16(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x66\xf7\xf3"); // div ax, bx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpDivEqU32:
            //CONCAT_STR_2(concat, "*(__u32_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear32(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString2("\xf7\xf3"); // div eax, ebx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpDivEqU64:
            //CONCAT_STR_2(concat, "*(__u64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Clear64(pp, RDX);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x48\xf7\xf3"); // div rax, rbx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RBX);
            break;
        case ByteCodeOp::AffectOpDivEqF32:
            //CONCAT_STR_2(concat, "*(__f32_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].f32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf3\x0f\x5e\xc1"); // divss xmm0, xmm1
            BackendX64Inst::emit_StoreF32_Indirect(pp, 0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            //CONCAT_STR_2(concat, "*(__f64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].f64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf2\x0f\x5e\xc1"); // divsd xmm0, xmm1
            BackendX64Inst::emit_StoreF64_Indirect(pp, 0, XMM0, RAX);
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x28\x03"); // sub [rbx], al
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x66\x29\x03"); // sub [rbx], ax
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            //CONCAT_STR_2(concat, "*(__s16_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s16;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x29\x03"); // sub [rbx], eax
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x48\x29\x03"); // sub [rbx], rax
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            //CONCAT_STR_2(concat, "*(__f32_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].f32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf3\x0f\x5c\xc1"); // subss xmm0, xmm1
            BackendX64Inst::emit_StoreF32_Indirect(pp, 0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            //CONCAT_STR_2(concat, "*(__f64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].f64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf2\x0f\x5c\xc1"); // subss xmm0, xmm1
            BackendX64Inst::emit_StoreF64_Indirect(pp, 0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpMinusEqPointer:
            //CONCAT_STR_2(concat, "*(__u8_t**)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x48\x29\x03"); // sub [rbx], rax
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x00\x03"); // add [rbx], al
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            //CONCAT_STR_2(concat, "*(__s8_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s8;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x66\x01\x03"); // add [rbx], ax
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x01\x03"); // add [rbx], eax
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            //CONCAT_STR_2(concat, "*(__s64_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s64;");
            //CONCAT_STR_2(concat, "*(__s32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x48\x01\x03"); // add [rbx], rax
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            //CONCAT_STR_2(concat, "*(__f32_t**)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].f32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf3\x0f\x58\xc1"); // addss xmm0, xmm1
            BackendX64Inst::emit_StoreF32_Indirect(pp, 0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            //CONCAT_STR_2(concat, "*(__f64_t**)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].f64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, 0, XMM0, RAX);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            concat.addString4("\xf2\x0f\x58\xc1"); // addss xmm0, xmm1
            BackendX64Inst::emit_StoreF64_Indirect(pp, 0, XMM0, RAX);
            break;
        case ByteCodeOp::AffectOpPlusEqPointer:
            //CONCAT_STR_2(concat, "*(__u8_t**)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RBX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString3("\x48\x01\x03"); // add [rbx], rax
            break;

        case ByteCodeOp::LowerZeroToTrue:
            //concat.addStringFormat("r[%u].b = r[%u].s32 < 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\xc1\xe8\x1f"); // shr eax, 31
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::LowerEqZeroToTrue:
            //concat.addStringFormat("r[%u].b = r[%u].s32 < 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_EAX_With_EAX(pp);
            concat.addString3("\x0f\x9e\xc0"); // setle al
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::GreaterZeroToTrue:
            //concat.addStringFormat("r[%u].b = r[%u].s32 > 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_EAX_With_EAX(pp);
            BackendX64Inst::emit_SetG(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::GreaterEqZeroToTrue:
            //concat.addStringFormat("r[%u].b = r[%u].s32 > 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString2("\xf7\xd0");     // not eax
            concat.addString3("\xc1\xe8\x1f"); // shr eax, 31
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CompareOpGreaterU32:
            //concat.addStringFormat("r[%u].b = r[%u].u32 > r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_EAX_With_EBX(pp);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            //concat.addStringFormat("r[%u].b = r[%u].u64 > r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_RAX_With_RBX(pp);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterS32:
            //concat.addStringFormat("r[%u].b = r[%u].s32 > r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_EAX_With_EBX(pp);
            BackendX64Inst::emit_SetG(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            //concat.addStringFormat("r[%u].b = r[%u].s64 > r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_RAX_With_RBX(pp);
            BackendX64Inst::emit_SetG(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            //concat.addStringFormat("r[%u].b = r[%u].f32 > r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM0, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM1, RDI);
            BackendX64Inst::emit_Cmp_XMM0_With_XMM1_F32(pp);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            //concat.addStringFormat("r[%u].b = r[%u].f64 > r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM0, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM1, RDI);
            BackendX64Inst::emit_Cmp_XMM0_With_XMM1_F64(pp);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::CompareOpLowerU32:
            //concat.addStringFormat("r[%u].b = r[%u].u32 < r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_EAX_With_EBX(pp);
            BackendX64Inst::emit_SetB(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerU64:
            //concat.addStringFormat("r[%u].b = r[%u].u64 < r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_RAX_With_RBX(pp);
            BackendX64Inst::emit_SetB(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerS32:
            //concat.addStringFormat("r[%u].b = r[%u].s32 < r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_EAX_With_EBX(pp);
            BackendX64Inst::emit_SetL(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerS64:
            //concat.addStringFormat("r[%u].b = r[%u].s64 < r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_RAX_With_RBX(pp);
            BackendX64Inst::emit_SetL(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerF32:
            //concat.addStringFormat("r[%u].b = r[%u].f32 < r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            BackendX64Inst::emit_Cmp_XMM0_With_XMM1_F32(pp);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpLowerF64:
            //concat.addStringFormat("r[%u].b = r[%u].f64 < r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            BackendX64Inst::emit_Cmp_XMM0_With_XMM1_F64(pp);
            BackendX64Inst::emit_SetA(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::CompareOpEqual8:
            //concat.addStringFormat("r[%u].b = r[%u].u8 == r[%u].u8;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_AL_With_BL(pp);
            concat.addString3("\x0F\x94\xC0"); // sete al
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpEqual16:
            //concat.addStringFormat("r[%u].b = r[%u].u16 == r[%u].u16;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_AX_With_BX(pp);
            concat.addString3("\x0F\x94\xC0"); // sete al
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpEqual32:
            //concat.addStringFormat("r[%u].b = r[%u].u32 == r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_EAX_With_EBX(pp);
            concat.addString3("\x0F\x94\xC0"); // sete al
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpEqual64:
            //concat.addStringFormat("r[%u].b = r[%u].u64 == r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Cmp_RAX_With_RBX(pp);
            concat.addString3("\x0F\x94\xC0"); // sete al
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpEqualString:
            //concat.addStringFormat("r[%u].b = swag_runtime_compareString(r[%u].pointer, r[%u].pointer, r[%u].u32, r[%u].u32);", ip->c.u32, ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->c.u32), R8, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->d.u32), R9, RDI);
            emitCall(pp, "swag_runtime_compareString");
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::CompareOpEqualTypeInfo:
            //concat.addStringFormat("r[%u].b = swag_runtime_compareType(r[%u].pointer, r[%u].pointer);", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            emitCall(pp, "swag_runtime_compareType");
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::TestNotZero8:
            //concat.addStringFormat("r[%u].b=r[%u].u8!=0;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Test_AL_With_AL(pp);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::TestNotZero16:
            //concat.addStringFormat("r[%u].b=r[%u].u16!=0;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Test_AX_With_AX(pp);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::TestNotZero32:
            //concat.addStringFormat("r[%u].b=r[%u].u32!=0;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Test_EAX_With_EAX(pp);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::TestNotZero64:
            //concat.addStringFormat("r[%u].b=r[%u].u64!=0;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Test_RAX_With_RAX(pp);
            BackendX64Inst::emit_SetNE(pp);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::NegBool:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].b ^= 1;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString2("\x34\x01"); // xor al, 1
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::NegS32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = -r[", ip->a.u32, "].s32;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString2("\xf7\xd8"); // neg eax
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::NegS64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = -r[", ip->a.u32, "].s64;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\x48\xf7\xd8"); // neg rax
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::NegF32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = -r[", ip->a.u32, "].f32;");
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, 0x80000000, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetFLT, RAX, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, offsetFLT, XMM1, RDI);
            concat.addString3("\x0f\x57\xc1"); // xorps xmm0, xmm1
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;
        case ByteCodeOp::NegF64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = -r[", ip->a.u32, "].f64;");
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, 0x80000000'00000000, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetFLT, RAX, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, offsetFLT, XMM1, RDI);
            concat.addString4("\x66\x0f\x57\xc1"); // xorpd xmm0, xmm1
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            break;

        case ByteCodeOp::InvertS8:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s8 = ~r[", ip->a.u32, "].s8;");
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString2("\xf6\xd0"); // not al
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::InvertS16:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16 = ~r[", ip->a.u32, "].s16;");
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\x66\xf7\xd0"); // not ax
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::InvertS32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = ~r[", ip->a.u32, "].s32;");
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString2("\xf7\xd0"); // not eax
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::InvertS64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = ~r[", ip->a.u32, "].s64;");
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\x48\xf7\xd0"); // not rax
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::JumpIfTrue:
            //CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_AL_With_AL(pp);
            BackendX64Inst::emitJump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfFalse:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_AL_With_AL(pp);
            BackendX64Inst::emitJump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero32:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_EAX_With_EAX(pp);
            BackendX64Inst::emitJump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfNotZero64:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u64) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_RAX_With_RAX(pp);
            BackendX64Inst::emitJump(pp, BackendX64Inst::JNZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero32:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Test_EAX_With_EAX(pp);
            BackendX64Inst::emitJump(pp, BackendX64Inst::JZ, i, ip->b.s32);
            break;
        case ByteCodeOp::JumpIfZero64:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u64) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
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
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRef16:
            //concat.addStringFormat("r[%u].u16 = *(__u16_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRef32:
            //concat.addStringFormat("r[%u].u32 = *(__u32_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRef64:
            //concat.addStringFormat("r[%u].u64 = *(__u64_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRefPointer:
            //concat.addStringFormat("r[%u].pointer = *(__u8_t**) (r[%u].pointer + %u);", ip->b.u32, ip->a.u32, ip->c.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->c.u32, RBX);
            concat.addString3("\x48\x01\xD8"); // add rax, rbx
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            break;
        case ByteCodeOp::DeRefStringSlice:
            //concat.addStringFormat("r[%u].u64 = *(__u64_t*) (r[%u].pointer + 8); ", ip->b.u32, ip->a.u32);
            //concat.addStringFormat("r[%u].pointer = *(__u8_t**) r[%u].pointer; ", ip->a.u32, ip->a.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 8, RDX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::GetFromBssSeg64:
            //concat.addStringFormat("r[%u].u64 = *(__u64_t*) (__bs + %u);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symBSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::GetFromMutableSeg64:
            //concat.addStringFormat("r[%u].u64 = *(__u64_t*) (__ms + %u);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symMSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::GetFromStack64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u64 = *(__u64_t*) (stack + ", ip->b.u32, ");");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::ClearMaskU32:
            //concat.addStringFormat("r[%u].u32 &= 0x%x;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, RBX);
            concat.addString2("\x21\xd8"); // and eax, ebx
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::ClearMaskU64:
            //concat.addStringFormat("r[%u].u64 &= 0x%llx;", ip->a.u32, ip->b.u64);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RBX);
            concat.addString3("\x48\x21\xd8"); // and rax, rbx
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::SetZeroAtPointer8:
            //CONCAT_STR_2(concat, "*(__u8_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store8_Immediate(pp, regOffset(ip->b.u32), 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer16:
            //CONCAT_STR_2(concat, "*(__u16_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store16_Immediate(pp, regOffset(ip->b.u32), 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer32:
            //CONCAT_STR_2(concat, "*(__u32_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store32_Immediate(pp, regOffset(ip->b.u32), 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointer64:
            //CONCAT_STR_2(concat, "*(__u64_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->b.u32), 0, RAX);
            break;
        case ByteCodeOp::SetZeroAtPointerX:
            //concat.addStringFormat("memset(r[%u].pointer, 0, %u);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Clear64(pp, RDX);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, R8);
            emitCall(pp, "memset");
            break;
        case ByteCodeOp::SetZeroAtPointerXRB:
            //concat.addStringFormat("memset(r[%u].pointer, 0, r[%u].u32 * %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Clear64(pp, RDX);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            concat.addString2("\x69\xc0"); // imul eax, ????????
            concat.addU32(ip->c.u32);
            BackendX64Inst::emit_Copy64(pp, R8, RAX);
            emitCall(pp, "memset");
            break;

        case ByteCodeOp::SetZeroStack8:
            //CONCAT_STR_1(concat, "*(__u8_t*)(s+", ip->a.u32, ")=0;");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->a.u32);
            BackendX64Inst::emit_Store8_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::SetZeroStack16:
            //CONCAT_STR_1(concat, "*(__u16_t*)(s+", ip->a.u32, ")=0;");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->a.u32);
            BackendX64Inst::emit_Store16_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::SetZeroStack32:
            //CONCAT_STR_1(concat, "*(__u32_t*)(s+", ip->a.u32, ")=0;");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->a.u32);
            BackendX64Inst::emit_Store32_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::SetZeroStack64:
            //CONCAT_STR_1(concat, "*(__u64_t*)(s+", ip->a.u32, ")=0;");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->a.u32);
            BackendX64Inst::emit_Store64_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::SetZeroStackX:
            //concat.addStringFormat("memset(stack + %u, 0, %u);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Lea_Stack_In_RCX(pp, offsetStack + ip->a.u32);
            BackendX64Inst::emit_Clear64(pp, RDX);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, R8);
            emitCall(pp, "memset");
            break;

        case ByteCodeOp::SetAtPointer8:
            //concat.addStringFormat("*(__u8_t*)(r[%u].pointer + %u) = r[%u].u8;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->c.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, 0, RBX, RAX);
            break;
        case ByteCodeOp::SetAtPointer16:
            //concat.addStringFormat("*(__u16_t*)(r[%u].pointer + %u) = r[%u].u16;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->c.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, 0, RBX, RAX);
            break;
        case ByteCodeOp::SetAtPointer32:
            //concat.addStringFormat("*(__u32_t*)(r[%u].pointer + %u) = r[%u].u32;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->c.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, 0, RBX, RAX);
            break;
        case ByteCodeOp::SetAtPointer64:
            //concat.addStringFormat("*(__u64_t*)(r[%u].pointer + %u) = r[%u].u64;", ip->a.u32, ip->c.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->c.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RBX, RAX);
            break;

        case ByteCodeOp::MakeStackPointer:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = stack + ", ip->b.u32, ";");
            BackendX64Inst::emit_Lea_Stack_In_RAX(pp, offsetStack + ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::MakeTypeSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__ts + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symTSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::MakeMutableSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__ms + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symMSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::MakeBssSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__bs + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symBSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::MakeConstantSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__cs + %u); ", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symCSIndex);
            BackendX64Inst::emit_Add_Cst32_To_RAX(pp, ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::IncPointerVB32:
            //concat.addStringFormat("r[%u].pointer += %u;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->a.u32);
            concat.addString3("\x48\x81\x00"); // add [rax], ????????
            concat.addU32(ip->b.s32);
            break;

        case ByteCodeOp::IncPointer32:
            //concat.addStringFormat("r[%u].pointer = r[%u].pointer + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x48\x01\xD8"); // add rax, rbx
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case ByteCodeOp::DecPointer32:
            //concat.addStringFormat("r[%u].pointer = r[%u].pointer - r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            concat.addString3("\x48\x29\xd8"); // sub rax, rbx
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::Mul64byVB32:
            //concat.addStringFormat("r[%u].s64 *= %u;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            concat.addString3("\x48\x69\xC0"); // imul rax, rax, ????????
            concat.addU32(ip->b.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::Div64byVB32:
            //concat.addStringFormat("r[%u].s64 /= %u;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, RBX);
            BackendX64Inst::emit_Clear64(pp, RDX);
            concat.addString3("\x48\xf7\xfb"); // idiv rax, rbx
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopyVBtoRA32:
            //concat.addStringFormat("r[%u].u32 = 0x%x;", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Store32_Immediate(pp, regOffset(ip->a.u32), ip->b.u32, RDI);
            break;
        case ByteCodeOp::CopyVBtoRA64:
            //concat.addStringFormat("r[%u].u64 = 0x%I64x;", ip->a.u32, ip->b.u64);
            if (ip->b.u64 <= 0x7FFFFFFF)
                BackendX64Inst::emit_Store64_Immediate(pp, regOffset(ip->a.u32), ip->b.u64, RDI);
            else
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RAX);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            }
            break;

        case ByteCodeOp::MemCpyVC32:
            //concat.addStringFormat("memcpy(r[%u].pointer, r[%u].pointer, %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->c.u32, R8);
            emitCall(pp, "memcpy");
            break;

        case ByteCodeOp::MemCpy:
            //concat.addStringFormat("memcpy(r[%u].pointer, r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->c.u32), R8, RDI);
            emitCall(pp, "memcpy");
            break;
        case ByteCodeOp::MemSet:
            //concat.addStringFormat("memset(r[%u].pointer, r[%u].u8, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->c.u32), R8, RDI);
            emitCall(pp, "memset");
            break;
        case ByteCodeOp::MemCmp:
            //concat.addStringFormat("r[%u].s32 = swag_runtime_memcmp(r[%u].pointer, r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->d.u32), R8, RDI);
            emitCall(pp, "memcmp");
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::IntrinsicAssert:
            //concat.addStringFormat("swag_runtime_assert(r[%u].b, \"%s\", %d, \"%s\");", ip->a.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1, ip->d.pointer);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            emitGlobalString(pp, precompileIndex, normalizePath(ip->node->sourceFile->path), RDX);
            BackendX64Inst::emit_Load64_Immediate(pp, ip->node->token.startLocation.line + 1, R8);
            if (ip->d.pointer)
                emitGlobalString(pp, precompileIndex, (const char*) ip->d.pointer, R9);
            else
                BackendX64Inst::emit_Clear64(pp, R9);
            emitCall(pp, "swag_runtime_assert");
            break;

        case ByteCodeOp::IntrinsicGetContext:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) swag_runtime_tlsGetValue(__process_infos.contextTlsId);", ip->a.u32);
            BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symPI_contextTlsId);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RCX, RAX);
            emitCall(pp, "swag_runtime_tlsGetValue");
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicSetContext:
            //concat.addStringFormat("swag_runtime_tlsSetValue(__process_infos.contextTlsId, r[%u].pointer);", ip->a.u32);
            concat.addString3("\x48\x8b\x0d"); // mov rcx, qword ptr ????????[rip]
            BackendX64Inst::emit_Symbol_Relocation(pp, pp.symPI_contextTlsId);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RDX, RDI);
            emitCall(pp, "swag_runtime_tlsSetValue");
            break;

        case ByteCodeOp::IntrinsicArguments:
            //concat.addStringFormat("r[%u].pointer = __process_infos.arguments.addr;", ip->a.u32);
            //concat.addStringFormat("r[%u].u64 = __process_infos.arguments.count;", ip->b.u32);
            BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symPI_args_addr);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symPI_args_count);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            break;

        case ByteCodeOp::IntrinsicAlloc:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) malloc(r[%u].u32);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            emitCall(pp, "malloc");
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicRealloc:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) realloc(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
            emitCall(pp, "realloc");
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::IntrinsicFree:
            //concat.addStringFormat("free(r[%u].pointer);", ip->a.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            emitCall(pp, "free");
            break;

        case ByteCodeOp::IntrinsicCompiler:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].p = 0;");
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Store64_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::IntrinsicIsByteCode:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].b = 0;");
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->a.u32);
            BackendX64Inst::emit_Store32_Immediate(pp, 0, 0, RAX);
            break;
        case ByteCodeOp::IntrinsicPrintString:
            //swag_runtime_print_n(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            emitCall(pp, "swag_runtime_print_n");
            break;
        case ByteCodeOp::IntrinsicPrintS64:
            //CONCAT_STR_1(concat, "swag_runtime_print_i64(r[", ip->a.u32, "].s64);");
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            emitCall(pp, "swag_runtime_print_i64");
            break;
        case ByteCodeOp::IntrinsicPrintF64:
            //CONCAT_STR_1(concat, "swag_runtime_print_i64(r[", ip->a.u32, "].f64);");
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            emitCall(pp, "swag_runtime_print_f64");
            break;
        case ByteCodeOp::IntrinsicInterfaceOf:
            //concat.addStringFormat("r[%u].p=(__u8_t*)swag_runtime_interfaceof(r[%u].p,r[%u].p);", ip->c.u32, ip->a.u32, ip->b.u32);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RDX, RDI);
            emitCall(pp, "swag_runtime_interfaceof");
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopyRCtoRR:
            //CONCAT_STR_2(concat, "*rr", ip->a.u32, " = r[", ip->b.u32, "];");
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            BackendX64Inst::emit_Load64_Indirect(pp, 16 + sizeStack + regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RBX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RBX, RAX);
            break;

        case ByteCodeOp::CopyRCtoRT:
            // CONCAT_STR_2(concat, "rt[", ip->a.u32, "] = r[", ip->b.u32, "];");
            SWAG_ASSERT(ip->a.u32 <= 1); // Can only return 2 registers
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetRT + regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopyRRtoRC:
            SWAG_ASSERT(ip->b.u32 <= 1); // Can only return 2 registers
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = *rr", ip->b.u32, ";");
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            BackendX64Inst::emit_Load64_Indirect(pp, 16 + sizeStack + regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopyRTtoRC:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = rt[", ip->b.u32, "];");
            BackendX64Inst::emit_Load64_Indirect(pp, offsetRT + regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopySP:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) &r[%u];", ip->a.u32, ip->c.u32);
            BackendX64Inst::emit_Lea_Reg_In_RAX(pp, ip->c.u32);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::CopySPVaargs:
            //concat.addStringFormat("__r_t vaargs%u[] = { 0, ", vaargsIdx);
            //int idxParam = (int) pushRAParams.size() - 1;
            //while (idxParam >= 0)
            //{
            //    concat.addStringFormat("r[%u], ", pushRAParams[idxParam]);
            //    idxParam--;
            //}
            isVariadic = 8 + ((int) pushRAParams.size() * sizeof(Register));
            BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, isVariadic);
            for (int idxParam = (int) pushRAParams.size() - 1, offset = 8; idxParam >= 0; idxParam--, offset += 8)
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(pushRAParams[idxParam]), RAX, RDI);
                concat.addString4("\x48\x89\x84\x24"); // mov [rsp + ????????], rax
                concat.addU32(offset);
            }

            //concat.addStringFormat("r[%u].pointer = sizeof(__r_t) + (__u8_t*) &vaargs%u; ", ip->a.u32, vaargsIdx);
            concat.addString4("\x48\x8d\x44\x24"); // lea rax, [rsp + ??]
            concat.addU8(8);
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);

            //concat.addStringFormat("vaargs%u[0].pointer = r[%u].pointer;", vaargsIdx, ip->a.u32);
            concat.addString4("\x48\x89\x04\x24"); // mov [rsp], rax
            break;

        case ByteCodeOp::PushRAParam:
            pushRAParams.push_back(ip->a.u32);
            break;

        case ByteCodeOp::GetFromStackParam64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = *rp", ip->c.u32, ";");
            concat.addString3("\x48\x8b\x87"); // mov rax, [rdi + ????????]
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            concat.addU32(16 + sizeStack + ip->c.u32 * sizeof(Register) + typeFunc->numReturnRegisters() * sizeof(Register));
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case ByteCodeOp::MakeStackPointerParam:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = (__u8_t*) &rp", ip->c.u32, "->pointer;");
            concat.addString3("\x48\x8d\x87"); // lea rax, [rdi + ????????]
            // We need to add 8 because the call has pushed one register on the stack
            // We need to add 8 again, because of the first 'push edi' at the start of the function
            // Se we add 16 in total to get the offset of the parameter in the stack
            concat.addU32(16 + sizeStack + ip->c.u32 * sizeof(Register) + typeFunc->numReturnRegisters() * sizeof(Register));
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;

        case ByteCodeOp::MakeLambda:
        {
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) &%s;", ip->a.u32, funcBC->callName().c_str());
            auto funcBC = (ByteCode*) ip->b.pointer;
            SWAG_ASSERT(funcBC);
            concat.addString2("\x48\xb8"); // mov rax, ????????_????????
            concat.addU64(0);

            CoffRelocation reloc;
            reloc.virtualAddress = concat.totalCount() - sizeof(uint64_t) - pp.textSectionOffset;
            auto callSym         = getOrAddSymbol(pp, funcBC->callName(), CoffSymbolKind::Extern);
            reloc.symbolIndex    = callSym->index;
            reloc.type           = IMAGE_REL_AMD64_ADDR64;
            pp.relocTableTextSection.table.push_back(reloc);

            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }

        case ByteCodeOp::LambdaCall:
        {
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;
            //concat.addStringFormat("if(r[%u].u64 & 0x%llx) { ", ip->a.u32, SWAG_LAMBDA_MARKER);

            // Test if it's a native lambda or a bytecode one
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            BackendX64Inst::emit_Load64_Immediate(pp, SWAG_LAMBDA_MARKER, RBX);
            concat.addString3("\x48\x21\xc3"); // and rbx, rax
            BackendX64Inst::emit_Test_RBX_With_RBX(pp);
            concat.addString2("\x0f\x85"); // jnz ???????? => jump to bytecode lambda
            auto jumpToBCAddr = (uint32_t*) concat.getSeekPtr();
            concat.addU32(0);
            auto jumpToBCOffset = concat.totalCount();

            // Native lambda
            //////////////////
            uint32_t sizeCallStack = emitLocalCallParameters(pp, typeFuncBC, offsetRT, pushRAParams);
            concat.addString2("\xff\xd0"); // call rax
            BackendX64Inst::emit_Add_Cst32_To_RSP(pp, sizeCallStack + isVariadic);

            concat.addString1("\xe9"); // jmp ???????? => jump after bytecode lambda
            auto jumpToAfterAddr = (uint32_t*) concat.getSeekPtr();
            concat.addU32(0);
            auto jumpToAfterOffset = concat.totalCount();

            // ByteCode lambda
            //////////////////
            *jumpToBCAddr = concat.totalCount() - jumpToBCOffset;
            concat.addString1("\x90");

            // End
            //////////////////
            *jumpToAfterAddr = concat.totalCount() - jumpToAfterOffset;

            isVariadic = 0;
            pushRAParams.clear();
            break;
        }

        case ByteCodeOp::LocalCall:
        {
            auto              funcBC     = (ByteCode*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;
            if (funcBC->name.find("variadic2") != -1)
                funcBC = funcBC;
            uint32_t sizeCallStack = emitLocalCallParameters(pp, typeFuncBC, offsetRT, pushRAParams);
            emitCall(pp, funcBC->callName());

            BackendX64Inst::emit_Add_Cst32_To_RSP(pp, sizeCallStack + isVariadic);
            isVariadic = 0;
            pushRAParams.clear();
            break;
        }

        case ByteCodeOp::ForeignCall:
            emitForeignCall(pp, moduleToGen, ip, offsetRT, pushRAParams);
            break;

        case ByteCodeOp::Ret:
            if (sizeStack)
            {
                BackendX64Inst::emit_Add_Cst32_To_RSP(pp, sizeStack + 4 * sizeof(uint64_t));
                concat.addU8(0x5F); // pop rdi
            }

            concat.addU8(0xc3); // ret
            break;

        case ByteCodeOp::IntrinsicS8x1:
        {
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AL_To_AX(pp);
            BackendX64Inst::emit_SignedExtend_AX_To_EAX(pp);
            concat.addString2("\x89\xc1"); // mov ecx, eax
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                emitCall(pp, "abs");
                break;
            }
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicS16x1:
        {
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            BackendX64Inst::emit_SignedExtend_AX_To_EAX(pp);
            concat.addString2("\x89\xc1"); // mov ecx, eax
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                emitCall(pp, "abs");
                break;
            }
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicS32x1:
        {
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                emitCall(pp, "abs");
                break;
            }
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicS64x1:
        {
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                emitCall(pp, "llabs");
                break;
            }
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        }
        case ByteCodeOp::IntrinsicF32x2:
        {
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM0, RDI);
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->c.u32), XMM1, RDI);
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
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM0, RDI);
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->c.u32), XMM1, RDI);
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
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM0, RDI);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                emitCall(pp, "sqrtf");
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
                emitCall(pp, "fabsf");
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
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM0, RDI);
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                emitCall(pp, "sqrt");
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
                emitCall(pp, "fabs");
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

uint32_t BackendX64::emitLocalCallParameters(X64PerThread& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t stackRR, VectorNative<uint32_t>& pushRAParams)
{
    auto& concat = pp.concat;

    int popRAidx      = (int) pushRAParams.size();
    int numCallParams = (int) typeFuncBC->parameters.size();
    for (int idxCall = numCallParams - 1; idxCall >= 0; idxCall--)
    {
        auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
        typeParam      = TypeManager::concreteReferenceType(typeParam);
        for (int j = 0; j < typeParam->numRegisters(); j++)
        {
            SWAG_ASSERT(popRAidx);
            popRAidx--;
        }
    }

    uint32_t sizeStack = (uint32_t)((pushRAParams.size() - popRAidx) * sizeof(Register));
    sizeStack += typeFuncBC->numReturnRegisters() * sizeof(Register);

    // Be sure stack remains align to 16 bytes
    if (sizeStack % 16 == 8)
    {
        sizeStack += 8;
        BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 8);
    }

    for (int iParam = popRAidx; iParam < pushRAParams.size(); iParam++)
    {
        concat.addString2("\xff\xb7"); // push [rdi + ????????]
        concat.addU32(pushRAParams[iParam] * sizeof(Register));
        //sizeStack += 8;
    }

    // Return registers are push first
    for (int j = (int) typeFuncBC->numReturnRegisters() - 1; j >= 0; j--)
    {
        concat.addString3("\x4c\x8d\xb7"); // lea r14, [rdi + ????????]
        concat.addU32(stackRR + (j * sizeof(Register)));
        concat.addString2("\x41\x56"); // push r14
        //sizeStack += 8;
    }

    return sizeStack;
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
        funcName = nodeFunc->name;

    uint32_t exceededStack = 0;
    SWAG_CHECK(emitForeignCallParameters(pp, exceededStack, moduleToGen, ip, offsetRT, typeFuncBC, pushRAParams));
    emitCall(pp, funcName);
    BackendX64Inst::emit_Add_Cst32_To_RSP(pp, exceededStack);

    // Store result to rt0
    auto returnType = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    if (returnType != g_TypeMgr.typeInfoVoid)
    {
        if ((returnType->kind == TypeInfoKind::Slice) ||
            (returnType->isNative(NativeTypeKind::Any)) ||
            (returnType->isNative(NativeTypeKind::String)) ||
            (returnType->flags & TYPEINFO_RETURN_BY_COPY))
        {
            // Return by parameter
        }
        else
        {
            //CONCAT_FIXED_STR(concat, "rt[0].pointer = (__u8_t*) ");
            BackendX64Inst::emit_Store64_Indirect(pp, offsetRT, RAX, RDI);
        }
    }

    pushRAParams.clear();
    return true;
}

bool BackendX64::emitForeignCallParameters(X64PerThread& pp, uint32_t& exceededStack, Module* moduleToGen, ByteCodeInstruction* ip, uint32_t offsetRT, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams)
{
    auto& concat        = pp.concat;
    int   numCallParams = (int) typeFuncBC->parameters.size();

    VectorNative<uint32_t>  paramsRegisters;
    VectorNative<TypeInfo*> paramsTypes;

    // Variadic are first
    if (numCallParams)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters.back()->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic)
        {
            auto index = pushRAParams.back();
            pushRAParams.pop_back();
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);

            index = pushRAParams.back();
            pushRAParams.pop_back();
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU32);
            numCallParams--;
        }
    }

    // All parameters
    for (int i = 0; i < (int) numCallParams; i++)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters[i]->typeInfo);

        auto index = pushRAParams.back();
        pushRAParams.pop_back();

        if (typeParam->kind == TypeInfoKind::Pointer ||
            typeParam->kind == TypeInfoKind::Struct ||
            typeParam->kind == TypeInfoKind::Interface ||
            typeParam->kind == TypeInfoKind::Array)
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);
        }
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);
            index = pushRAParams.back();
            pushRAParams.pop_back();
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU32);
        }
        else if (typeParam->isNative(NativeTypeKind::Any))
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);
            index = pushRAParams.back();
            pushRAParams.pop_back();
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr.typeInfoU64);
        }
        else
        {
            if (typeParam->sizeOf > sizeof(void*))
                return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid parameter type");
            paramsRegisters.push_back(index);
            paramsTypes.push_back(typeParam);
        }
    }

    // Return by parameter
    auto returnType   = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    bool returnByCopy = returnType->flags & TYPEINFO_RETURN_BY_COPY;
    if (returnType->kind == TypeInfoKind::Slice ||
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

    for (int i = 0; i < min(4, (int) paramsRegisters.size()); i++)
    {
        auto type = paramsTypes[i];
        auto r    = paramsRegisters[i];
        if (type == g_TypeMgr.typeInfoUndefined)
        {
            if (returnByCopy)
            {
                switch (i)
                {
                case 0:
                    BackendX64Inst::emit_Load64_Indirect(pp, r, RCX, RDI);
                    break;
                case 1:
                    BackendX64Inst::emit_Load64_Indirect(pp, r, RDX, RDI);
                    break;
                case 2:
                    BackendX64Inst::emit_Load64_Indirect(pp, r, R8, RDI);
                    break;
                case 3:
                    BackendX64Inst::emit_Load64_Indirect(pp, r, R9, RDI);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
                }
            }
            else
            {
                switch (i)
                {
                case 0:
                    concat.addString3("\x48\x8d\x8f"); // lea rcx, [rdi + ????????]
                    concat.addU32(r);
                    break;
                case 1:
                    concat.addString3("\x48\x8d\x97"); // lea rdx, [rdi + ????????]
                    concat.addU32(r);
                    break;
                case 2:
                    concat.addString3("\x4c\x8d\x87"); // lea r8, [rdi + ????????]
                    concat.addU32(r);
                    break;
                case 3:
                    concat.addString3("\x4c\x8d\x8f"); // lea r9, [rdi + ????????]
                    concat.addU32(r);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
                }
            }

            continue;
        }

        if (type->flags & TYPEINFO_INTEGER)
        {
            switch (i)
            {
            case 0:
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(r), RCX, RDI);
                break;
            case 1:
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(r), RDX, RDI);
                break;
            case 2:
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(r), R8, RDI);
                break;
            case 3:
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(r), R9, RDI);
                break;
            }

            continue;
        }

        if (type->flags & TYPEINFO_FLOAT)
        {
            switch (i)
            {
            case 0:
                BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(r), XMM0, RDI);
                break;
            case 1:
                BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(r), XMM1, RDI);
                break;
            case 2:
                BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(r), XMM2, RDI);
                break;
            case 3:
                BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(r), XMM3, RDI);
                break;
            }

            continue;
        }
    }

    exceededStack = 0;
    if (paramsRegisters.size() > 4)
    {
        // Need to reserve additional room in the stack for all other parameters
        // (remember: we already have reserved 4 x uint64_t for the first four parameters)
        for (int i = 4; i < (int) paramsRegisters.size(); i++)
            exceededStack += 8;
        BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, exceededStack);

        // Then we store all the parameters on the stack, with an offset of 4 * sizeof(uint64_t)
        // because the first 4 x uint64_t are for the first 4 parameters (even if they are passed in
        // registers, this is the x64 cdecl convention...)
        uint32_t offsetStack = 4 * sizeof(uint64_t);
        for (int i = 4; i < (int) paramsRegisters.size(); i++)
        {
            if (paramsTypes[i] == g_TypeMgr.typeInfoUndefined)
            {
                if (returnByCopy)
                    BackendX64Inst::emit_Load64_Indirect(pp, paramsRegisters[i], RAX, RDI);
                else
                    BackendX64Inst::emit_Lea_Stack_In_RAX(pp, paramsRegisters[i]);
                concat.addString4("\x48\x89\x84\x24"); // mov [rsp + ????????], rax
                concat.addU32(offsetStack);
            }
            else
            {
                auto sizeOf = paramsTypes[i]->sizeOf;
                switch (sizeOf)
                {
                case 1:
                    BackendX64Inst::emit_Load8_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    concat.addString3("\x88\x84\x24"); // mov [rsp + ????????], al
                    concat.addU32(offsetStack);
                    break;
                case 2:
                    BackendX64Inst::emit_Load16_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    concat.addString4("\x66\x89\x84\x24"); // mov [rsp + ????????], ax
                    concat.addU32(offsetStack);
                    break;
                case 4:
                    BackendX64Inst::emit_Load32_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    concat.addString3("\x89\x84\x24"); // mov [rsp + ????????], eax
                    concat.addU32(offsetStack);
                    break;
                case 8:
                    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    concat.addString4("\x48\x89\x84\x24"); // mov [rsp + ????????], rax
                    concat.addU32(offsetStack);
                    break;
                default:
                    return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid parameter type");
                }
            }

            // Push is always aligned
            offsetStack += 8;
        }
    }

    return true;
}
