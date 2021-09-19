#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64_Macros.h"
#include "TypeManager.h"

void BackendX64::emitLocalCallParameters(X64PerThread& pp, uint32_t sizeParamsStack, TypeInfoFuncAttr* typeFuncBC, uint32_t stackRR, const VectorNative<uint32_t>& pushRAParams, const VectorNative<pair<uint32_t, uint32_t>>& pushRVParams)
{
    uint32_t sizeStack = (uint32_t) (pushRAParams.size() * sizeof(Register));
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

    int numReturnRegs = typeFuncBC->numReturnRegisters();

    // Return value
    int callerIndex = 0;
    int offsetStack = 0;
    for (int j = 0; j < numReturnRegs; j++)
    {
        BackendX64Inst::emit_LoadAddress_Indirect(pp, stackRR + regOffset(j), RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);

        // :CConvLocal
        storeRAXToCDeclParam(pp, nullptr, callerIndex);
        callerIndex++;

        offsetStack += 8;
    }

    int popRAidx      = (int) pushRAParams.size() - 1;
    int numCallParams = (int) typeFuncBC->parameters.size();

    // Two registers for variadics first
    if (typeFuncBC->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        auto index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        BackendX64Inst::emit_Load64_Indirect(pp, regOffset(index), RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);

        // :CConvLocal
        storeRAXToCDeclParam(pp, nullptr, callerIndex);
        callerIndex++;

        offsetStack += 8;
        index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        BackendX64Inst::emit_Load64_Indirect(pp, regOffset(index), RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);

        // :CConvLocal
        storeRAXToCDeclParam(pp, nullptr, callerIndex);
        callerIndex++;

        offsetStack += 8;
        numCallParams--;
    }

    // Func call parameters
    for (int idxCall = 0; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
        typeParam      = TypeManager::concreteReferenceType(typeParam);
        for (int j = 0; j < typeParam->numRegisters(); j++)
        {
            auto index = pushRAParams[popRAidx--];
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(index), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);

            // :CConvLocal
            if (!passByValue(typeParam))
                BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetStack, RAX, RSP);
            storeRAXToCDeclParam(pp, typeParam, callerIndex);
            callerIndex++;

            offsetStack += 8;
        }
    }

    // Emit the rest
    for (int j = popRAidx; j >= 0; j--)
    {
        BackendX64Inst::emit_Load64_Indirect(pp, regOffset(pushRAParams[j]), RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);

        // :CConvLocal
        storeRAXToCDeclParam(pp, nullptr, callerIndex);
        callerIndex++;

        offsetStack += 8;
    }
}

void BackendX64::storeRAXToCDeclParam(X64PerThread& pp, TypeInfo* typeParam, int callerIndex)
{
    if (callerIndex < 4)
    {
        if (typeParam && typeParam->isNativeFloat())
        {
            static uint8_t x64Reg[] = {XMM0, XMM1, XMM2, XMM3};
            BackendX64Inst::emit_CopyF64(pp, RAX, x64Reg[callerIndex]);
        }
        else if (!typeParam)
        {
            static uint8_t x64Reg[] = {RCX, RDX, R8, R9};
            BackendX64Inst::emit_Copy64(pp, RAX, x64Reg[callerIndex]);
        }
        else
        {
            static uint8_t x64Reg[] = {RCX, RDX, R8, R9};
            switch (typeParam->sizeOf)
            {
            case 1:
                BackendX64Inst::emit_Clear64(pp, x64Reg[callerIndex]);
                BackendX64Inst::emit_Copy8(pp, RAX, x64Reg[callerIndex]);
                break;
            case 2:
                BackendX64Inst::emit_Clear64(pp, x64Reg[callerIndex]);
                BackendX64Inst::emit_Copy16(pp, RAX, x64Reg[callerIndex]);
                break;
            case 4:
                BackendX64Inst::emit_Copy32(pp, RAX, x64Reg[callerIndex]);
                break;
            default:
                BackendX64Inst::emit_Copy64(pp, RAX, x64Reg[callerIndex]);
                break;
            }
        }
    }
}

void BackendX64::emitLocalParam(X64PerThread& pp, TypeInfoFuncAttr* typeFunc, int reg, int paramIdx, int sizeOf, int sizeStack)
{
    // We need to add 8 because the call has pushed one register on the stack
    // We need to add 8 again, because of the first 'push edi' at the start of the function
    // Se we add 16 in total to get the offset of the parameter in the stack
    int stackOffset = 16 + sizeStack + regOffset(paramIdx) + regOffset(typeFunc->numReturnRegisters());

    switch (sizeOf)
    {
    case 1:
        BackendX64Inst::emit_Clear32(pp, RAX);
        BackendX64Inst::emit_Load8_Indirect(pp, stackOffset, RAX, RDI);
        BackendX64Inst::emit_Store32_Indirect(pp, regOffset(reg), RAX, RDI);
        break;
    case 2:
        BackendX64Inst::emit_Clear32(pp, RAX);
        BackendX64Inst::emit_Load16_Indirect(pp, stackOffset, RAX, RDI);
        BackendX64Inst::emit_Store32_Indirect(pp, regOffset(reg), RAX, RDI);
        break;
    case 4:
        BackendX64Inst::emit_Load32_Indirect(pp, stackOffset, RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, regOffset(reg), RAX, RDI);
        break;
    default:
        BackendX64Inst::emit_Load64_Indirect(pp, stackOffset, RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, regOffset(reg), RAX, RDI);
        break;
    }
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