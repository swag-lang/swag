#include "pch.h"
#include "BackendC.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "BackendCFunctionBodyJob.h"
#include "swag_runtime.h"
#include "Module.h"

BackendFunctionBodyJob* BackendC::newFunctionJob()
{
    return g_Pool_backendCFunctionBodyJob.alloc();
}

void BackendC::addCallParameters(Concat& concat, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams)
{
    for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
    {
        if (j)
            concat.addChar(',');
        CONCAT_STR_1(concat, "&rt[", j, "]");
    }

    int popRAidx      = (int) pushRAParams.size() - 1;
    int numCallParams = (int) typeFuncBC->parameters.size();
    for (int idxCall = numCallParams - 1; idxCall >= 0; idxCall--)
    {
        auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
        typeParam      = TypeManager::concreteReferenceType(typeParam);
        for (int j = 0; j < typeParam->numRegisters(); j++)
        {
            if ((idxCall != (int) numCallParams - 1) || j || typeFuncBC->numReturnRegisters())
                concat.addChar(',');
            auto index = pushRAParams[popRAidx--];
            CONCAT_STR_1(concat, "&r[", index, "]");
        }
    }
}

bool BackendC::swagTypeToCType(Module* moduleToGen, TypeInfo* typeInfo, Utf8& cType)
{
    typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS);
    cType.clear();

    if (typeInfo->kind == TypeInfoKind::Enum)
    {
        auto typeInfoEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        SWAG_CHECK(swagTypeToCType(moduleToGen, typeInfoEnum->rawType, cType));
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typeInfoPointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        SWAG_CHECK(swagTypeToCType(moduleToGen, typeInfoPointer->finalType, cType));
        uint32_t ptrCount = typeInfoPointer->ptrCount;
        if (typeInfoPointer->finalType->kind == TypeInfoKind::Struct ||
            typeInfoPointer->finalType->isNative(NativeTypeKind::Any) ||
            typeInfoPointer->finalType->kind == TypeInfoKind::Slice)
            ptrCount--;
        for (uint32_t i = 0; i < ptrCount; i++)
            cType += "*";
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice ||
        typeInfo->kind == TypeInfoKind::Array ||
        typeInfo->kind == TypeInfoKind::Struct ||
        typeInfo->kind == TypeInfoKind::Interface ||
        typeInfo->kind == TypeInfoKind::Lambda ||
        typeInfo->isNative(NativeTypeKind::Any) ||
        typeInfo->isNative(NativeTypeKind::String) ||
        typeInfo->kind == TypeInfoKind::Reference)
    {
        cType = "void*";
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            cType = "__b_t";
            return true;
        case NativeTypeKind::S8:
            cType = "__s8_t";
            return true;
        case NativeTypeKind::S16:
            cType = "__s16_t";
            return true;
        case NativeTypeKind::S32:
            cType = "__s32_t";
            return true;
        case NativeTypeKind::S64:
            cType = "__s64_t";
            return true;
        case NativeTypeKind::U8:
            cType = "__u8_t";
            return true;
        case NativeTypeKind::U16:
            cType = "__u16_t";
            return true;
        case NativeTypeKind::U32:
            cType = "__u32_t";
            return true;
        case NativeTypeKind::U64:
            cType = "__u64_t";
            return true;
        case NativeTypeKind::F32:
            cType = "__f32_t";
            return true;
        case NativeTypeKind::F64:
            cType = "__f64_t";
            return true;
        case NativeTypeKind::Char:
            cType = "__ch_t";
            return true;
        case NativeTypeKind::Void:
            cType = "void";
            return true;
        }
    }

    return moduleToGen->internalError(format("swagTypeToCType, invalid type '%s'", typeInfo->name.c_str()));
}

bool BackendC::emitForeignCallReturn(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC)
{
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
        else if (returnType->kind == TypeInfoKind::Pointer)
        {
            CONCAT_FIXED_STR(concat, "rt[0].p=(__u8_t*)");
        }
        else if (returnType->kind == TypeInfoKind::Native)
        {
            switch (returnType->nativeType)
            {
            case NativeTypeKind::S8:
                CONCAT_FIXED_STR(concat, "rt[0].s8=(__s8_t)");
                break;
            case NativeTypeKind::U8:
                CONCAT_FIXED_STR(concat, "rt[0].u8=(__u8_t)");
                break;
            case NativeTypeKind::S16:
                CONCAT_FIXED_STR(concat, "rt[0].s16=(__s16_t)");
                break;
            case NativeTypeKind::U16:
                CONCAT_FIXED_STR(concat, "rt[0].u16=(__u16_t)");
                break;
            case NativeTypeKind::S32:
                CONCAT_FIXED_STR(concat, "rt[0].s32=(__s32_t)");
                break;
            case NativeTypeKind::U32:
            case NativeTypeKind::Char:
                CONCAT_FIXED_STR(concat, "rt[0].u32=(__u32_t)");
                break;
            case NativeTypeKind::S64:
                CONCAT_FIXED_STR(concat, "rt[0].s64=(__s64_t)");
                break;
            case NativeTypeKind::U64:
                CONCAT_FIXED_STR(concat, "rt[0].u64=(__u64_t)");
                break;
            case NativeTypeKind::Bool:
                CONCAT_FIXED_STR(concat, "rt[0].b=(__b_t)");
                break;
            case NativeTypeKind::F32:
                CONCAT_FIXED_STR(concat, "rt[0].f32=(float)");
                break;
            case NativeTypeKind::F64:
                CONCAT_FIXED_STR(concat, "rt[0].f64=(double)");
                break;
            default:
                return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid return type");
            }
        }
        else
        {
            return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid return type");
        }
    }

    return true;
}

bool BackendC::emitForeignCallParameters(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, const VectorNative<uint32_t>& pushParams)
{
    concat.addChar('(');

    int numCallParams = (int) typeFuncBC->parameters.size();

    // Variadic are first
    bool first    = true;
    int  idxParam = (int) pushParams.size() - 1;
    if (numCallParams)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters.back()->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic)
        {
            auto index = pushParams[idxParam--];
            CONCAT_STR_1(concat, "(void*)r[", index, "].p");
            index = pushParams[idxParam--];
            CONCAT_STR_1(concat, ",r[", index, "].u32");
            numCallParams--;
            first = false;
        }
    }

    for (int idxCall = 0; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters[idxCall]->typeInfo);
        if (!first)
            concat.addChar(',');
        first = false;

        auto index = pushParams[idxParam--];

        // Access to the content of the register
        if (typeParam->kind == TypeInfoKind::Struct ||
            typeParam->kind == TypeInfoKind::Interface ||
            typeParam->kind == TypeInfoKind::Array ||
            typeParam->kind == TypeInfoKind::Lambda ||
            typeParam->kind == TypeInfoKind::Pointer)
        {
            CONCAT_STR_1(concat, "(void*)r[", index, "].p");
        }
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            CONCAT_STR_1(concat, "(void*)r[", index, "].p");
            index = pushParams[idxParam--];
            CONCAT_STR_1(concat, ",r[", index, "].u32");
        }
        else if (typeParam->isNative(NativeTypeKind::Any))
        {
            CONCAT_STR_1(concat, "(void*)r[", index, "].p");
            index = pushParams[idxParam--];
            CONCAT_STR_1(concat, ",(void*)r[", index, "].p");
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            CONCAT_STR_1(concat, "r[", index, "]");
            switch (typeParam->nativeType)
            {
            case NativeTypeKind::Bool:
                CONCAT_FIXED_STR(concat, ".b");
                break;
            case NativeTypeKind::S8:
                CONCAT_FIXED_STR(concat, ".s8");
                break;
            case NativeTypeKind::U8:
                CONCAT_FIXED_STR(concat, ".u8");
                break;
            case NativeTypeKind::S16:
                CONCAT_FIXED_STR(concat, ".s16");
                break;
            case NativeTypeKind::U16:
                CONCAT_FIXED_STR(concat, ".u16");
                break;
            case NativeTypeKind::S32:
                CONCAT_FIXED_STR(concat, ".s32");
                break;
            case NativeTypeKind::U32:
                CONCAT_FIXED_STR(concat, ".u32");
                break;
            case NativeTypeKind::S64:
                CONCAT_FIXED_STR(concat, ".s64");
                break;
            case NativeTypeKind::U64:
                CONCAT_FIXED_STR(concat, ".u64");
                break;
            case NativeTypeKind::F32:
                CONCAT_FIXED_STR(concat, ".f32");
                break;
            case NativeTypeKind::F64:
                CONCAT_FIXED_STR(concat, ".f64");
                break;
            case NativeTypeKind::Char:
                CONCAT_FIXED_STR(concat, ".ch");
                break;
            default:
                return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid param native type");
            }
        }
        else
        {
            return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid param type");
        }
    }

    // Return by parameter
    auto returnType = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    if (returnType->kind == TypeInfoKind::Slice ||
        returnType->isNative(NativeTypeKind::Any) ||
        returnType->isNative(NativeTypeKind::String))
    {
        if (numCallParams)
            concat.addChar(',');
        CONCAT_FIXED_STR(concat, "&rt[0]");
    }
    else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
    {
        if (numCallParams)
            concat.addChar(',');
        CONCAT_FIXED_STR(concat, "rt[0].p");
    }

    CONCAT_FIXED_STR(concat, ")");
    return true;
}

bool BackendC::emitForeignCall(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, AstFuncDecl* nodeFunc, VectorNative<uint32_t>& pushParams)
{
    // Signature
    emitForeignFuncSignature(concat, moduleToGen, typeFuncBC, nodeFunc, false);
    CONCAT_FIXED_STR(concat, ";\n");

    // Return value
    SWAG_CHECK(emitForeignCallReturn(concat, moduleToGen, typeFuncBC));

    // Name
    ComputedValue foreignValue;
    if (typeFuncBC->attributes.getValue("swag.foreign", "function", foreignValue) && !foreignValue.text.empty())
        concat.addString(foreignValue.text);
    else
        concat.addString(nodeFunc->name);

    // Parameters
    SWAG_CHECK(emitForeignCallParameters(concat, moduleToGen, typeFuncBC, pushParams));
    CONCAT_FIXED_STR(concat, ";");

    return true;
}

bool BackendC::emitFuncWrapperPublic(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one)
{
    CONCAT_FIXED_STR(concat, "SWAG_EXPORT ");
    SWAG_CHECK(emitForeignFuncSignature(concat, moduleToGen, typeFunc, node, true));
    CONCAT_FIXED_STR(concat, "{\n");

    // Compute number of registers
    auto n = typeFunc->numReturnRegisters();
    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        n += typeParam->numRegisters();
    }

    // Declare registers
    if (n)
    {
        CONCAT_FIXED_STR(concat, "__r_t ");
        for (int i = 0; i < n; i++)
        {
            if (i)
                concat.addChar(',');
            concat.addChar('r');
            concat.addChar('r');
            concat.addU32Str(i);
        }
        CONCAT_FIXED_STR(concat, ";\n");
    }

    // Affect registers
    int idx = typeFunc->numReturnRegisters();

    // Return by copy
    bool returnByCopy = typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY;
    if (returnByCopy)
    {
        CONCAT_FIXED_STR(concat, "rr0.p=result;\n");
    }

    auto numParams = typeFunc->parameters.size();

    // Variadic must be pushed first
    if (numParams)
    {
        auto param     = typeFunc->parameters.back();
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic)
        {
            concat.addStringFormat("rr%d.p=(__u8_t*)%s;\n", idx, param->namedParam.c_str());
            concat.addStringFormat("rr%d.u32=%s_count;\n", idx + 1, param->namedParam.c_str());
            idx += 2;
            numParams--;
        }
    }

    for (int i = 0; i < numParams; i++)
    {
        auto param     = typeFunc->parameters[i];
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (typeParam->kind == TypeInfoKind::Pointer || typeParam->kind == TypeInfoKind::Lambda)
        {
            concat.addStringFormat("rr%d.p=(__u8_t*)%s;\n", idx, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            concat.addStringFormat("rr%d.p=(__u8_t*)%s;\n", idx, param->namedParam.c_str());
            concat.addStringFormat("rr%d.u32=%s_count;\n", idx + 1, param->namedParam.c_str());
        }
        else if (typeParam->isNative(NativeTypeKind::Any))
        {
            concat.addStringFormat("rr%d.p=(__u8_t*)%s_value;\n", idx, param->namedParam.c_str());
            concat.addStringFormat("rr%d.p=%s_type;\n", idx + 1, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Struct ||
                 typeParam->kind == TypeInfoKind::Array ||
                 typeParam->kind == TypeInfoKind::Interface)
        {
            concat.addStringFormat("rr%d.p=(__u8_t*)%s;\n", idx, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            switch (typeParam->nativeType)
            {
            case NativeTypeKind::U8:
                concat.addStringFormat("rr%d.u8=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::U16:
                concat.addStringFormat("rr%d.u16=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::U32:
                concat.addStringFormat("rr%d.u32=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::U64:
                concat.addStringFormat("rr%d.u64=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S8:
                concat.addStringFormat("rr%d.s8=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S16:
                concat.addStringFormat("rr%d.s16=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S32:
                concat.addStringFormat("rr%d.s32=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S64:
                concat.addStringFormat("rr%d.s64=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::F32:
                concat.addStringFormat("rr%d.f32=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::F64:
                concat.addStringFormat("rr%d.f64=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::Bool:
                concat.addStringFormat("rr%d.b=%s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::Char:
                concat.addStringFormat("rr%d.ch=%s;\n", idx, param->namedParam.c_str());
                break;
            default:
                return moduleToGen->internalError("emitFuncWrapperPublic, invalid param type");
            }
        }
        else
        {
            return moduleToGen->internalError(format("emitFuncWrapperPublic, invalid param type '%s'", typeParam->name.c_str()));
        }

        idx += typeParam->numRegisters();
    }

    // Make the call
    concat.addString(one->callName());
    concat.addChar('(');
    for (int i = 0; i < n; i++)
    {
        if (i)
            concat.addChar(',');
        CONCAT_FIXED_STR(concat, "&rr");
        concat.addU32Str(i);
    }

    concat.addChar(')');
    concat.addChar(';');
    concat.addEol();

    // Return
    if (typeFunc->numReturnRegisters() && !returnByCopy)
    {
        auto returnType = TypeManager::concreteType(typeFunc->returnType, CONCRETE_ALIAS | CONCRETE_ENUM);
        if (returnType->kind == TypeInfoKind::Slice ||
            returnType->isNative(NativeTypeKind::Any) ||
            returnType->isNative(NativeTypeKind::String))
        {
            CONCAT_FIXED_STR(concat, "*((void**)result)=rr0.p;\n");
            CONCAT_FIXED_STR(concat, "*((void**)result+1)=rr1.p;\n");
        }
        else if (returnType->kind == TypeInfoKind::Interface)
        {
            CONCAT_FIXED_STR(concat, "*((void**)result)=rr0.p;\n");
            CONCAT_FIXED_STR(concat, "*((void**)result+1)=rr1.p;\n");
        }
        else if (returnType->kind == TypeInfoKind::Pointer)
        {
            Utf8 returnCast;
            SWAG_CHECK(swagTypeToCType(moduleToGen, typeFunc->returnType, returnCast));
            concat.addStringFormat("return(%s)rr0.p;\n", returnCast.c_str());
        }
        else if (returnType->kind == TypeInfoKind::Native)
        {
            switch (returnType->nativeType)
            {
            case NativeTypeKind::U8:
                CONCAT_FIXED_STR(concat, "return rr0.u8;\n");
                break;
            case NativeTypeKind::U16:
                CONCAT_FIXED_STR(concat, "return rr0.u16;\n");
                break;
            case NativeTypeKind::U32:
                CONCAT_FIXED_STR(concat, "return rr0.u32;\n");
                break;
            case NativeTypeKind::U64:
                CONCAT_FIXED_STR(concat, "return rr0.u64;\n");
                break;
            case NativeTypeKind::S8:
                CONCAT_FIXED_STR(concat, "return rr0.s8;\n");
                break;
            case NativeTypeKind::S16:
                CONCAT_FIXED_STR(concat, "return rr0.s16;\n");
                break;
            case NativeTypeKind::S32:
                CONCAT_FIXED_STR(concat, "return rr0.s32;\n");
                break;
            case NativeTypeKind::S64:
                CONCAT_FIXED_STR(concat, "return rr0.s64;\n");
                break;
            case NativeTypeKind::F32:
                CONCAT_FIXED_STR(concat, "return rr0.f32;\n");
                break;
            case NativeTypeKind::F64:
                CONCAT_FIXED_STR(concat, "return rr0.f64;\n");
                break;
            case NativeTypeKind::Char:
                CONCAT_FIXED_STR(concat, "return rr0.ch;\n");
                break;
            case NativeTypeKind::Bool:
                CONCAT_FIXED_STR(concat, "return rr0.b;\n");
                break;
            default:
                return moduleToGen->internalError("emitFuncWrapperPublic, invalid return type");
            }
        }
        else
        {
            return moduleToGen->internalError(format("emitFuncWrapperPublic, invalid return type '%s'", returnType->name.c_str()));
        }
    }

    CONCAT_FIXED_STR(concat, "}\n");
    return true;
}

bool BackendC::emitForeignFuncSignature(Concat& buffer, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, bool forWrapper)
{
    Utf8 returnType;
    bool returnByCopy = typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY;

    // Return type
    SWAG_CHECK(swagTypeToCType(moduleToGen, typeFunc->returnType, returnType));
    if (returnByCopy)
        CONCAT_FIXED_STR(buffer, "void");
    else if (typeFunc->numReturnRegisters() <= 1)
        buffer.addString(returnType);
    else
        CONCAT_FIXED_STR(buffer, "void");
    buffer.addChar(' ');

    // Name
    if (node)
    {
        node->computeFullNameForeign(forWrapper);
        buffer.addString(node->fullnameForeign);
    }
    else
    {
        // A type cast
        CONCAT_FIXED_STR(buffer, "(*)");
    }

    // Parameters
    buffer.addChar('(');

    bool first     = true;
    auto numParams = typeFunc->parameters.size();

    // Variadic parameters are always first
    if (numParams)
    {
        auto param = typeFunc->parameters.back();
        if (param->typeInfo->kind == TypeInfoKind::Variadic)
        {
            CONCAT_FIXED_STR(buffer, "void*");
            if (forWrapper)
                buffer.addString(param->namedParam);
            CONCAT_FIXED_STR(buffer, ",__u32_t ");
            if (forWrapper)
            {
                buffer.addString(param->namedParam);
                CONCAT_FIXED_STR(buffer, "_count");
            }
            numParams--;
            first = false;
        }
    }

    Utf8 cType;
    for (int i = 0; i < numParams; i++)
    {
        auto param = typeFunc->parameters[i];
        if (!first)
            buffer.addChar(',');
        first = false;

        SWAG_CHECK(swagTypeToCType(moduleToGen, param->typeInfo, cType));
        buffer.addString(cType);
        buffer.addChar(' ');
        if (forWrapper)
            buffer.addString(param->namedParam);

        if (param->typeInfo->kind == TypeInfoKind::Slice || param->typeInfo->isNative(NativeTypeKind::String))
        {
            CONCAT_FIXED_STR(buffer, ",__u32_t ");
            if (forWrapper)
            {
                buffer.addString(param->namedParam);
                CONCAT_FIXED_STR(buffer, "_count");
            }
        }
        else if (param->typeInfo->isNative(NativeTypeKind::Any))
        {
            CONCAT_FIXED_STR(buffer, "_value,void*");
            if (forWrapper)
            {
                buffer.addString(param->namedParam);
                CONCAT_FIXED_STR(buffer, "_type");
            }
        }
    }

    // Return value
    if (typeFunc->numReturnRegisters() > 1 || returnByCopy)
    {
        if (!first)
            buffer.addChar(',');
        buffer.addString(returnType);
        if (forWrapper)
            CONCAT_FIXED_STR(buffer, " result");
    }

    CONCAT_FIXED_STR(buffer, ")");
    return true;
}

void BackendC::emitLocalFuncSignature(Concat& concat, TypeInfoFuncAttr* typeFunc, const Utf8& name, bool withNames)
{
    CONCAT_FIXED_STR(concat, "void ");
    concat.addString(name);
    concat.addChar('(');

    // Result
    for (int i = 0; i < typeFunc->numReturnRegisters(); i++)
    {
        if (i)
            concat.addChar(',');
        if (withNames)
            concat.addStringFormat("__r_t*rr%d", i);
        else
            concat.addStringFormat("__r_t*", i);
    }

    // Parameters
    int index = 0;
    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        for (int i = 0; i < typeParam->numRegisters(); i++)
        {
            if (index || i || typeFunc->numReturnRegisters())
                concat.addChar(',');
            if (withNames)
                concat.addStringFormat("__r_t*rp%u", index++);
            else
                concat.addStringFormat("__r_t*", index++);
        }
    }

    CONCAT_FIXED_STR(concat, ")");
}

bool BackendC::emitFunctionBody(Concat& concat, Module* moduleToGen, ByteCode* bc)
{
    concat.addEol();

    bool ok       = true;
    auto typeFunc = bc->callType();

    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC))
        CONCAT_FIXED_STR(concat, "#ifdef SWAG_HAS_TEST\n");

    // Signature
    emitLocalFuncSignature(concat, bc->callType(), bc->callName(), true);
    CONCAT_FIXED_STR(concat, " {\n");

    // Generate one local variable per used register
    if (bc->maxReservedRegisterRC)
    {
        CONCAT_STR_1(concat, "__r_t r[", bc->maxReservedRegisterRC, "];\n");
    }

    // For function call results
    if (bc->maxCallResults)
    {
        CONCAT_STR_1(concat, "__r_t rt[", bc->maxCallResults, "];\n");
    }

    // Local stack
    if (typeFunc->stackSize)
    {
        CONCAT_STR_1(concat, "__u8_t s[", typeFunc->stackSize, "];\n");
    }

    // Generate bytecode
    int                    vaargsIdx = 0;
    auto                   ip        = bc->out;
    uint32_t               lastLine  = UINT32_MAX;
    VectorNative<uint32_t> pushRAParams;

    // To write the labels
    bc->markLabels();

    ip = bc->out;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
            continue;

        // Print source code
        if (!bc->compilerGenerated)
        {
            if (moduleToGen->buildParameters.buildCfg->backendC.writeSourceCode)
            {
                if (ip->node->token.startLocation.line != lastLine)
                {
                    if (ip->node->token.startLocation.column != ip->node->token.endLocation.column)
                    {
                        lastLine = ip->node->token.startLocation.line;
                        auto s   = bc->sourceFile->getLine(lastLine);
                        s.trimLeft();
                        if (!s.empty())
                            s.pop_back();
                        CONCAT_FIXED_STR(concat, "/* ");
                        concat.addString(s);
                        CONCAT_FIXED_STR(concat, " */\n");
                    }
                }
            }
        }

        // Label
        if (ip->flags & BCI_JUMP_DEST)
        {
            CONCAT_FIXED_STR(concat, "_");
            concat.addS32Str8(i);
            CONCAT_FIXED_STR(concat, ":;\n");
        }

        // Write the bytecode instruction name
        if (moduleToGen->buildParameters.buildCfg->backendC.writeByteCodeInstruction)
        {
            CONCAT_FIXED_STR(concat, "/* ");
            for (int dec = g_ByteCodeOpNamesLen[(int) ip->op]; dec < ByteCode::ALIGN_RIGHT_OPCODE; dec++)
                concat.addChar(' ');
            concat.addString(g_ByteCodeOpNames[(int) ip->op]);
            CONCAT_FIXED_STR(concat, " */ ");
        }

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::Nop:
        case ByteCodeOp::DecSP:
        case ByteCodeOp::IncSP:
        case ByteCodeOp::IncSPPostCall:
        case ByteCodeOp::CopySPtoBP:
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
            if (moduleToGen->buildParameters.buildCfg->backendC.writeByteCodeInstruction)
                concat.addEol();
            continue;

#define MK_ASSERT(__expr, __msg)                                              \
    concat.addStringFormat("swag_runtime_assert(%s,\"%s\",%d,\"%s\");",       \
                           __expr,                                            \
                           normalizePath(ip->node->sourceFile->path).c_str(), \
                           ip->node->token.startLocation.line + 1,            \
                           __msg);

        case ByteCodeOp::IncPointer32:
            if (ip->flags & BCI_IMM_B)
                concat.addStringFormat("r[%u].p=r[%u].p+%u;", ip->c.u32, ip->a.u32, ip->b.u32);
            else
                concat.addStringFormat("r[%u].p=r[%u].p+r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DecPointer32:
            concat.addStringFormat("r[%u].p=r[%u].p-r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DeRef8:
            concat.addStringFormat("r[%u].u64=*(__u8_t*)r[%u].p;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef16:
            concat.addStringFormat("r[%u].u64=*(__u16_t*)r[%u].p;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef32:
            concat.addStringFormat("r[%u].u64=*(__u32_t*)r[%u].p;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef64:
            concat.addStringFormat("r[%u].u64=*(__u64_t*)r[%u].p;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRefPointer:
            if (ip->c.u32)
                concat.addStringFormat("r[%u].p=*(__u8_t**)(r[%u].p+%u);", ip->b.u32, ip->a.u32, ip->c.u32);
            else
                concat.addStringFormat("r[%u].p=*(__u8_t**)r[%u].p;", ip->b.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRefStringSlice:
            concat.addStringFormat("r[%u].u64=*(__u64_t*)(r[%u].p+8);", ip->b.u32, ip->a.u32);
            concat.addStringFormat("r[%u].p=*(__u8_t**)r[%u].p;", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::Mul64byVB32:
            concat.addStringFormat("r[%u].s64*=%u;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::Div64byVB32:
            concat.addStringFormat("r[%u].s64/=%u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::GetFromMutableSeg64:
            concat.addStringFormat("r[%u].u64=*(__u64_t*)((__u8_t*)__ms+%u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromBssSeg64:
            concat.addStringFormat("r[%u].u64=*(__u64_t*)(__bs+%u);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::SetZeroStack8:
            if (ip->a.u32)
                CONCAT_STR_1(concat, "*(__u8_t*)(s+", ip->a.u32, ")=0;");
            else
                CONCAT_FIXED_STR(concat, "*(__u8_t*)s=0;");
            break;
        case ByteCodeOp::SetZeroStack16:
            if (ip->a.u32)
                CONCAT_STR_1(concat, "*(__u16_t*)(s+", ip->a.u32, ")=0;");
            else
                CONCAT_FIXED_STR(concat, "*(__u16_t*)s= 0;");
            break;
        case ByteCodeOp::SetZeroStack32:
            if (ip->a.u32)
                CONCAT_STR_1(concat, "*(__u32_t*)(s+", ip->a.u32, ")=0;");
            else
                CONCAT_FIXED_STR(concat, "*(__u32_t*)s=0;");
            break;
        case ByteCodeOp::SetZeroStack64:
            if (ip->a.u32)
                CONCAT_STR_1(concat, "*(__u64_t*)(s+", ip->a.u32, ")=0;");
            else
                CONCAT_FIXED_STR(concat, "*(__u64_t*)s=0;");
            break;
        case ByteCodeOp::SetZeroStackX:
            if (ip->a.u32)
                concat.addStringFormat("memset(s+%u,0,%u);", ip->a.u32, ip->b.u32);
            else
                concat.addStringFormat("memset(s,0,%u);", ip->b.u32);
            break;

        case ByteCodeOp::MakeMutableSegPointer:
            if (ip->b.u32)
                CONCAT_STR_2(concat, "r[", ip->a.u32, "].p=(__u8_t*)__ms+", ip->b.u32, ";");
            else
                CONCAT_STR_1(concat, "r[", ip->a.u32, "].p=(__u8_t*)__ms;");
            break;
        case ByteCodeOp::MakeBssSegPointer:
            if (ip->b.u32)
                CONCAT_STR_2(concat, "r[", ip->a.u32, "].p=__bs+", ip->b.u32, ";");
            else
                CONCAT_STR_1(concat, "r[", ip->a.u32, "].p=__bs;");
            break;
        case ByteCodeOp::MakeConstantSegPointer:
            if (ip->b.u32)
                CONCAT_STR_2(concat, "r[", ip->a.u32, "].p=(__u8_t*)__cs+", ip->b.u32, ";");
            else
                CONCAT_STR_1(concat, "r[", ip->a.u32, "].p=(__u8_t*)__cs;");
            break;
        case ByteCodeOp::MakeTypeSegPointer:
            if (ip->b.u32)
                CONCAT_STR_2(concat, "r[", ip->a.u32, "].p=(__u8_t*)__ts+", ip->b.u32, ";");
            else
                CONCAT_STR_1(concat, "r[", ip->a.u32, "].p=(__u8_t*)__ts;");
            break;

        case ByteCodeOp::MakeStackPointer:
            if (ip->b.u32)
                CONCAT_STR_2(concat, "r[", ip->a.u32, "].p=s+", ip->b.u32, ";");
            else
                CONCAT_STR_1(concat, "r[", ip->a.u32, "].p=s;");
            break;
        case ByteCodeOp::GetFromStack64:
            if (ip->b.u32)
                CONCAT_STR_2(concat, "r[", ip->a.u32, "].u64=*(__u64_t*)(s+", ip->b.u32, ");");
            else
                CONCAT_STR_1(concat, "r[", ip->a.u32, "].u64=*(__u64_t*)s;");
            break;

        case ByteCodeOp::MemCpy:
            if (ip->flags & BCI_IMM_C)
                concat.addStringFormat("memcpy(r[%u].p,r[%u].p,%u);", ip->a.u32, ip->b.u32, ip->c.u32);
            else
                concat.addStringFormat("memcpy(r[%u].p,r[%u].p,r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::MemSet:
            concat.addStringFormat("memset(r[%u].p,r[%u].u8,r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::MemCmp:
            concat.addStringFormat("r[%u].s32=memcmp(r[%u].p,r[%u].p,r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32);
            break;

        case ByteCodeOp::CopyRAVB32:
            concat.addStringFormat("r[%u].u32=0x%x;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyRAVB64:
            concat.addStringFormat("r[%u].u64=0x%I64x;", ip->a.u32, ip->b.u64);
            break;
        case ByteCodeOp::CopyRBtoRA:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "]=r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRBAddrToRA:
        case ByteCodeOp::CopyRBAddrToRA2:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].p=(__u8_t*)&r[", ip->b.u32, "];");
            break;

        case ByteCodeOp::ClearRA:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].u64=0;");
            break;
        case ByteCodeOp::DecrementRA32:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32--;");
            break;
        case ByteCodeOp::IncrementRA32:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32++;");
            break;
        case ByteCodeOp::AddRAVB32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].u32+=", ip->b.u32, ";");
            break;

        case ByteCodeOp::SetAtPointer8:
            concat.addStringFormat("*(__u8_t*)r[%u].p=r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::SetAtPointer16:
            concat.addStringFormat("*(__u16_t*)r[%u].p=r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::SetAtPointer32:
            concat.addStringFormat("*(__u32_t*)r[%u].p=r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::SetAtPointer64:
            concat.addStringFormat("*(__u64_t*)r[%u].p=r[%u].u64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::SetZeroAtPointer8:
            if (ip->b.u32)
                CONCAT_STR_2(concat, "*(__u8_t*)(r[", ip->a.u32, "].p+", ip->b.u32, ")=0;");
            else
                CONCAT_STR_1(concat, "*(__u8_t*)r[", ip->a.u32, "].p=0;");
            break;
        case ByteCodeOp::SetZeroAtPointer16:
            if (ip->b.u32)
                CONCAT_STR_2(concat, "*(__u16_t*)(r[", ip->a.u32, "].p+", ip->b.u32, ")=0;");
            else
                CONCAT_STR_1(concat, "*(__u16_t*)r[", ip->a.u32, "].p=0;");
            break;
        case ByteCodeOp::SetZeroAtPointer32:
            if (ip->b.u32)
                CONCAT_STR_2(concat, "*(__u32_t*)(r[", ip->a.u32, "].p+", ip->b.u32, ")=0;");
            else
                CONCAT_STR_1(concat, "*(__u32_t*)r[", ip->a.u32, "].p=0;");
            break;
        case ByteCodeOp::SetZeroAtPointer64:
            if (ip->b.u32)
                CONCAT_STR_2(concat, "*(__u64_t*)(r[", ip->a.u32, "].p+", ip->b.u32, ")=0;");
            else
                CONCAT_STR_1(concat, "*(__u64_t*)r[", ip->a.u32, "].p=0;");
            break;
        case ByteCodeOp::SetZeroAtPointerX:
            concat.addStringFormat("memset(r[%u].p,0,%u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::SetZeroAtPointerXRB:
            concat.addStringFormat("memset(r[%u].p,0,r[%u].u32*%u);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::BinOpPlusS32:
            concat.addStringFormat("r[%u].s32=r[%u].s32+r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusS64:
            concat.addStringFormat("r[%u].s64=r[%u].s64+r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusF32:
            concat.addStringFormat("r[%u].f32=r[%u].f32+r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusF64:
            concat.addStringFormat("r[%u].f64=r[%u].f64+r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpMinusS32:
            concat.addStringFormat("r[%u].s32=r[%u].s32-r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusS64:
            concat.addStringFormat("r[%u].s64=r[%u].s64-r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusF32:
            concat.addStringFormat("r[%u].f32=r[%u].f32-r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusF64:
            concat.addStringFormat("r[%u].f64=r[%u].f64-r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpMulS32:
            concat.addStringFormat("r[%u].s32=r[%u].s32*r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulS64:
            concat.addStringFormat("r[%u].s64=r[%u].s64*r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulF32:
            concat.addStringFormat("r[%u].f32=r[%u].f32*r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulF64:
            concat.addStringFormat("r[%u].f64=r[%u].f64*r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpXorU32:
            concat.addStringFormat("r[%u].u32=r[%u].u32^r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpXorU64:
            concat.addStringFormat("r[%u].u64=r[%u].u64^r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpShiftLeftU32:
            concat.addStringFormat("r[%u].u32=r[%u].u32<<r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpShiftLeftU64:
            concat.addStringFormat("r[%u].u64=r[%u].u64<<r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpShiftRightS32:
            concat.addStringFormat("r[%u].s32=r[%u].s32>>r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpShiftRightS64:
            concat.addStringFormat("r[%u].s64=r[%u].s64>>r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpShiftRightU32:
            concat.addStringFormat("r[%u].u32=r[%u].u32>>r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpShiftRightU64:
            concat.addStringFormat("r[%u].u64=r[%u].u64>>r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpShiftRightU64VB:
            concat.addStringFormat("r[%u].u64>>=%u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpModuloS32:
            concat.addStringFormat("r[%u].s32=r[%u].s32%%r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloS64:
            concat.addStringFormat("r[%u].s64=r[%u].s64%%r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloU32:
            concat.addStringFormat("r[%u].u32=r[%u].u32%%r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloU64:
            concat.addStringFormat("r[%u].u64=r[%u].u64%%r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpDivS32:
            concat.addStringFormat("r[%u].s32=r[%u].s32/r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivS64:
            concat.addStringFormat("r[%u].s64=r[%u].s64/r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivU32:
            concat.addStringFormat("r[%u].u32=r[%u].u32/r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivU64:
            concat.addStringFormat("r[%u].u64=r[%u].u64/r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivF32:
            concat.addStringFormat("r[%u].f32=r[%u].f32/r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivF64:
            concat.addStringFormat("r[%u].f64=r[%u].f64/r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            CONCAT_STR_2(concat, "*(__s8_t*)r[", ip->a.u32, "].p-=r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            CONCAT_STR_2(concat, "*(__s16_t*)r[", ip->a.u32, "].p-=r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            CONCAT_STR_2(concat, "*(__s32_t*)r[", ip->a.u32, "].p-=r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            CONCAT_STR_2(concat, "*(__s64_t*)r[", ip->a.u32, "].p-=r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            CONCAT_STR_2(concat, "*(__f32_t*)r[", ip->a.u32, "].p-=r[", ip->b.u32, "].f32;");
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            CONCAT_STR_2(concat, "*(__f64_t*)r[", ip->a.u32, "].p-=r[", ip->b.u32, "].f64;");
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            CONCAT_STR_2(concat, "*(__s8_t*)r[", ip->a.u32, "].p+=r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            CONCAT_STR_2(concat, "*(__s16_t*)r[", ip->a.u32, "].p+=r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            CONCAT_STR_2(concat, "*(__s32_t*)r[", ip->a.u32, "].p+=r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            CONCAT_STR_2(concat, "*(__s64_t*)r[", ip->a.u32, "].p+=r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            CONCAT_STR_2(concat, "*(__f32_t*)r[", ip->a.u32, "].p+=r[", ip->b.u32, "].f32;");
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            CONCAT_STR_2(concat, "*(__f64_t*)r[", ip->a.u32, "].p+=r[", ip->b.u32, "].f64;");
            break;

        case ByteCodeOp::AffectOpPlusEqPointer:
            CONCAT_STR_2(concat, "*(__u8_t**)r[", ip->a.u32, "].p+=r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpMinusEqPointer:
            CONCAT_STR_2(concat, "*(__u8_t**)r[", ip->a.u32, "].p-=r[", ip->b.u32, "].s32;");
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            CONCAT_STR_2(concat, "*(__s8_t*)r[", ip->a.u32, "].p*=r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            CONCAT_STR_2(concat, "*(__s16_t*)r[", ip->a.u32, "].p*=r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            CONCAT_STR_2(concat, "*(__s32_t*)r[", ip->a.u32, "].p*=r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            CONCAT_STR_2(concat, "*(__s64_t*)r[", ip->a.u32, "].p*=r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpMulEqF32:
            CONCAT_STR_2(concat, "*(__f32_t*)r[", ip->a.u32, "].p*=r[", ip->b.u32, "].f32;");
            break;
        case ByteCodeOp::AffectOpMulEqF64:
            CONCAT_STR_2(concat, "*(__f64_t*)r[", ip->a.u32, "].p*=r[", ip->b.u32, "].f64;");
            break;

        case ByteCodeOp::AffectOpDivEqS8:
            CONCAT_STR_2(concat, "*(__s8_t*)r[", ip->a.u32, "].p/=r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpDivEqS16:
            CONCAT_STR_2(concat, "*(__s16_t*)r[", ip->a.u32, "].p/=r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpDivEqS32:
            CONCAT_STR_2(concat, "*(__s32_t*)r[", ip->a.u32, "].p/=r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpDivEqS64:
            CONCAT_STR_2(concat, "*(__s64_t*)r[", ip->a.u32, "].p/=r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpDivEqU8:
            CONCAT_STR_2(concat, "*(__u8_t*)r[", ip->a.u32, "].p/=r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpDivEqU16:
            CONCAT_STR_2(concat, "*(__u16_t*)r[", ip->a.u32, "].p/=r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpDivEqU32:
            CONCAT_STR_2(concat, "*(__u32_t*)r[", ip->a.u32, "].p/=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpDivEqU64:
            CONCAT_STR_2(concat, "*(__u64_t*)r[", ip->a.u32, "].p/=r[", ip->b.u32, "].u64;");
            break;
        case ByteCodeOp::AffectOpDivEqF32:
            CONCAT_STR_2(concat, "*(__f32_t*)r[", ip->a.u32, "].p/=r[", ip->b.u32, "].f32;");
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            CONCAT_STR_2(concat, "*(__f64_t*)r[", ip->a.u32, "].p/=r[", ip->b.u32, "].f64;");
            break;

        case ByteCodeOp::AffectOpAndEqS8:
            CONCAT_STR_2(concat, "*(__s8_t*)r[", ip->a.u32, "].p&=r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpAndEqS16:
            CONCAT_STR_2(concat, "*(__s16_t*)r[", ip->a.u32, "].p&=r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpAndEqS32:
            CONCAT_STR_2(concat, "*(__s32_t*)r[", ip->a.u32, "].p&=r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpAndEqS64:
            CONCAT_STR_2(concat, "*(__s64_t*)r[", ip->a.u32, "].p&=r[", ip->b.u32, "].s64;");
            break;

        case ByteCodeOp::AffectOpOrEqS8:
            CONCAT_STR_2(concat, "*(__s8_t*)r[", ip->a.u32, "].p|=r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpOrEqS16:
            CONCAT_STR_2(concat, "*(__s16_t*)r[", ip->a.u32, "].p|=r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpOrEqS32:
            CONCAT_STR_2(concat, "*(__s32_t*)r[", ip->a.u32, "].p|=r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpOrEqS64:
            CONCAT_STR_2(concat, "*(__s64_t*)r[", ip->a.u32, "].p|=r[", ip->b.u32, "].s64;");
            break;

        case ByteCodeOp::AffectOpShiftLeftEqS8:
            CONCAT_STR_2(concat, "*(__s8_t*)r[", ip->a.u32, "].p<<=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS16:
            CONCAT_STR_2(concat, "*(__s16_t*)r[", ip->a.u32, "].p<<=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS32:
            CONCAT_STR_2(concat, "*(__s32_t*)r[", ip->a.u32, "].p<<=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS64:
            CONCAT_STR_2(concat, "*(__s64_t*)r[", ip->a.u32, "].p<<=r[", ip->b.u32, "].u32;");
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            CONCAT_STR_2(concat, "*(__s8_t*)r[", ip->a.u32, "].p>>=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            CONCAT_STR_2(concat, "*(__s16_t*)r[", ip->a.u32, "].p>>=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            CONCAT_STR_2(concat, "*(__s32_t*)r[", ip->a.u32, "].p>>=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            CONCAT_STR_2(concat, "*(__s64_t*)r[", ip->a.u32, "].p>>=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU8:
            CONCAT_STR_2(concat, "*(__u8_t*)r[", ip->a.u32, "].p>>=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            CONCAT_STR_2(concat, "*(__u16_t*)r[", ip->a.u32, "].p>>=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            CONCAT_STR_2(concat, "*(__u32_t*)r[", ip->a.u32, "].p>>=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            CONCAT_STR_2(concat, "*(__u64_t*)r[", ip->a.u32, "].p>>=r[", ip->b.u32, "].u32;");
            break;

        case ByteCodeOp::AffectOpModuloEqS8:
            CONCAT_STR_2(concat, "*(__s8_t*)r[", ip->a.u32, "].p%=r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpModuloEqS16:
            CONCAT_STR_2(concat, "*(__s16_t*)r[", ip->a.u32, "].p%=r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpModuloEqS32:
            CONCAT_STR_2(concat, "*(__s32_t*)r[", ip->a.u32, "].p%=r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpModuloEqS64:
            CONCAT_STR_2(concat, "*(__s64_t*)r[", ip->a.u32, "].p%=r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpModuloEqU8:
            CONCAT_STR_2(concat, "*(__u8_t*)r[", ip->a.u32, "].p%=r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpModuloEqU16:
            CONCAT_STR_2(concat, "*(__u16_t*)r[", ip->a.u32, "].p%=r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpModuloEqU32:
            CONCAT_STR_2(concat, "*(__u32_t*)r[", ip->a.u32, "].p%=r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpModuloEqU64:
            CONCAT_STR_2(concat, "*(__u64_t*)r[", ip->a.u32, "].p%=r[", ip->b.u32, "].u64;");
            break;

        case ByteCodeOp::AffectOpXOrEqS8:
            CONCAT_STR_2(concat, "*(__s8_t*)r[", ip->a.u32, "].p^=r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpXOrEqS16:
            CONCAT_STR_2(concat, "*(__s16_t*)r[", ip->a.u32, "].p^=r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpXOrEqS32:
            CONCAT_STR_2(concat, "*(__s32_t*)r[", ip->a.u32, "].p^=r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpXOrEqS64:
            CONCAT_STR_2(concat, "*(__s64_t*)r[", ip->a.u32, "].p^=r[", ip->b.u32, "].s64;");
            break;

        case ByteCodeOp::CompareOpGreaterS32:
            concat.addStringFormat("r[%u].b=r[%u].s32>r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            concat.addStringFormat("r[%u].b=r[%u].s64>r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterU32:
            concat.addStringFormat("r[%u].b=r[%u].u32>r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            concat.addStringFormat("r[%u].b=r[%u].u64>r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            concat.addStringFormat("r[%u].b=r[%u].f32>r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            concat.addStringFormat("r[%u].b=r[%u].f64>r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CompareOpLowerS32:
            concat.addStringFormat("r[%u].b=r[%u].s32<r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerS64:
            concat.addStringFormat("r[%u].b=r[%u].s64<r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerU32:
            concat.addStringFormat("r[%u].b=r[%u].u32<r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerU64:
            concat.addStringFormat("r[%u].b=r[%u].u64<r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerF32:
            concat.addStringFormat("r[%u].b=r[%u].f32<r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerF64:
            concat.addStringFormat("r[%u].b=r[%u].f64<r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CompareOpEqual8:
            concat.addStringFormat("r[%u].b=r[%u].u8==r[%u].u8;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual16:
            concat.addStringFormat("r[%u].b=r[%u].u16==r[%u].u16;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual32:
            concat.addStringFormat("r[%u].b=r[%u].u32==r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual64:
            concat.addStringFormat("r[%u].b=r[%u].u64==r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualString:
            concat.addStringFormat("r[%u].b=swag_runtime_compareString(r[%u].p,r[%u].p,r[%u].u32,r[%u].u32);", ip->c.u32, ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32);
            break;
        case ByteCodeOp::CompareOpEqualTypeInfo:
            concat.addStringFormat("r[%u].b=swag_runtime_compareType(r[%u].p,r[%u].p);", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpAnd:
            concat.addStringFormat("r[%u].b=r[%u].b&&r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpOr:
            concat.addStringFormat("r[%u].b=r[%u].b||r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpBitmaskAndS32:
            concat.addStringFormat("r[%u].s32=r[%u].s32&r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpBitmaskAndS64:
            concat.addStringFormat("r[%u].s64=r[%u].s64&r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpBitmaskOrS32:
            concat.addStringFormat("r[%u].s32=r[%u].s32|r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpBitmaskOrS64:
            concat.addStringFormat("r[%u].s64=r[%u].s64|r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::TestNotZero8:
            concat.addStringFormat("r[%u].b=r[%u].u8!=0;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::TestNotZero16:
            concat.addStringFormat("r[%u].b=r[%u].u16!=0;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::TestNotZero32:
            concat.addStringFormat("r[%u].b=r[%u].u32!=0;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::TestNotZero64:
            concat.addStringFormat("r[%u].b=r[%u].u64!=0;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::Jump:
            CONCAT_FIXED_STR(concat, "goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfFalse:
            CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].b)goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfTrue:
            CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].b)goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfZero32:
            CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u32)goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfZero64:
            CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u64)goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfNotZero32:
            CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].u32)goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfNotZero64:
            CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].u64)goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::Ret:
            CONCAT_FIXED_STR(concat, "return;");
            break;

        case ByteCodeOp::IntrinsicMkInterface:
        {
            concat.addStringFormat("r[%u].p=(__u8_t*)swag_runtime_interfaceof(r[%u].p,r[%u].p);", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        }
        case ByteCodeOp::IntrinsicPrintS64:
            CONCAT_STR_1(concat, "swag_runtime_print_i64(r[", ip->a.u32, "].s64);");
            break;
        case ByteCodeOp::IntrinsicPrintF64:
            CONCAT_STR_1(concat, "swag_runtime_print_f64(r[", ip->a.u32, "].f64);");
            break;
        case ByteCodeOp::IntrinsicPrintString:
            concat.addStringFormat("swag_runtime_print_n(r[%u].p,r[%u].u32);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::IntrinsicAssert:
            MK_ASSERT(format("r[%u].b", ip->a.u32).c_str(), ip->d.pointer);
            break;
        case ByteCodeOp::IntrinsicAlloc:
            concat.addStringFormat("r[%u].p=(__u8_t*)malloc(r[%u].u32);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::IntrinsicRealloc:
            concat.addStringFormat("r[%u].p=(__u8_t*)realloc(r[%u].p,r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::IntrinsicFree:
            concat.addStringFormat("free(r[%u].p);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicGetContext:
            concat.addStringFormat("r[%u].p=(__u8_t*)swag_runtime_tlsGetValue(__process_infos.contextTlsId);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicSetContext:
            concat.addStringFormat("swag_runtime_tlsSetValue(__process_infos.contextTlsId,r[%u].p);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicArguments:
            concat.addStringFormat("r[%u].p=__process_infos.arguments.addr;", ip->a.u32);
            concat.addStringFormat("r[%u].u64=__process_infos.arguments.count;", ip->b.u32);
            break;
        case ByteCodeOp::IntrinsicIsByteCode:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].b=0;");
            break;
        case ByteCodeOp::IntrinsicCompiler:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].p=0;");
            break;

        case ByteCodeOp::NegBool:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].b^=1;");
            break;
        case ByteCodeOp::NegF32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32=-r[", ip->a.u32, "].f32;");
            break;
        case ByteCodeOp::NegF64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64=-r[", ip->a.u32, "].f64;");
            break;
        case ByteCodeOp::NegS32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32=-r[", ip->a.u32, "].s32;");
            break;
        case ByteCodeOp::NegS64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64=-r[", ip->a.u32, "].s64;");
            break;

        case ByteCodeOp::InvertS8:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s8=~r[", ip->a.u32, "].s8;");
            break;
        case ByteCodeOp::InvertS16:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16=~r[", ip->a.u32, "].s16;");
            break;
        case ByteCodeOp::InvertS32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32=~r[", ip->a.u32, "].s32;");
            break;
        case ByteCodeOp::InvertS64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64=~r[", ip->a.u32, "].s64;");
            break;

        case ByteCodeOp::ClearMaskU32:
            concat.addStringFormat("r[%u].u32&=0x%x;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearMaskU64:
            concat.addStringFormat("r[%u].u64&=0x%llx;", ip->a.u32, ip->b.u64);
            break;

        case ByteCodeOp::CastBool8:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].b=r[", ip->a.u32, "].u8?1:0;");
            break;
        case ByteCodeOp::CastBool16:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].b=r[", ip->a.u32, "].u16?1:0;");
            break;
        case ByteCodeOp::CastBool32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].b=r[", ip->a.u32, "].u32?1:0;");
            break;
        case ByteCodeOp::CastBool64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].b=r[", ip->a.u32, "].u64?1:0;");
            break;

        case ByteCodeOp::CastS8S16:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16=(__s16_t)r[", ip->a.u32, "].s8;");
            break;
        case ByteCodeOp::CastS16S32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32=(__s32_t)r[", ip->a.u32, "].s16;");
            break;
        case ByteCodeOp::CastS32S64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64=(__s64_t)r[", ip->a.u32, "].s32;");
            break;
        case ByteCodeOp::CastS32F32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32=(__f32_t)r[", ip->a.u32, "].s32;");
            break;
        case ByteCodeOp::CastS64F32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32=(__f32_t)r[", ip->a.u32, "].s64;");
            break;
        case ByteCodeOp::CastS64F64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64=(__f64_t)r[", ip->a.u32, "].s64;");
            break;
        case ByteCodeOp::CastU32F32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32=(__f32_t)r[", ip->a.u32, "].u32;");
            break;
        case ByteCodeOp::CastU64F64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64=(__f64_t)r[", ip->a.u32, "].u64;");
            break;
        case ByteCodeOp::CastF32S32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32=(__s32_t)r[", ip->a.u32, "].f32;");
            break;
        case ByteCodeOp::CastF32F64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64=(__f64_t)r[", ip->a.u32, "].f32;");
            break;
        case ByteCodeOp::CastF64S64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64=(__s64_t)r[", ip->a.u32, "].f64;");
            break;
        case ByteCodeOp::CastF64F32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32=(__f32_t)r[", ip->a.u32, "].f64;");
            break;

        case ByteCodeOp::CopyRCtoRR:
            CONCAT_STR_2(concat, "*rr", ip->a.u32, "=r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRRtoRC:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "]=*rr", ip->b.u32, ";");
            break;
        case ByteCodeOp::CopyRCtoRT:
            CONCAT_STR_2(concat, "rt[", ip->a.u32, "]=r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRTtoRC:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "]=rt[", ip->b.u32, "];");
            break;
        case ByteCodeOp::GetFromStackParam64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "]=*rp", ip->c.u32, ";");
            break;
        case ByteCodeOp::MakeStackPointerParam:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].p=(__u8_t*)&rp", ip->c.u32, "->p;");
            break;

        case ByteCodeOp::LowerZeroToTrue:
            concat.addStringFormat("r[%u].b=r[%u].s32<0?1:0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::LowerEqZeroToTrue:
            concat.addStringFormat("r[%u].b=r[%u].s32<=0?1:0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::GreaterZeroToTrue:
            concat.addStringFormat("r[%u].b=r[%u].s32>0?1:0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::GreaterEqZeroToTrue:
            concat.addStringFormat("r[%u].b=r[%u].s32>=0?1:0;", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::PushRAParam:
            pushRAParams.push_back(ip->a.u32);
            if (moduleToGen->buildParameters.buildCfg->backendC.writeByteCodeInstruction)
                concat.addEol();
            continue;

        case ByteCodeOp::CopySP:
            concat.addStringFormat("r[%u].p=(__u8_t*)&r[%u];", ip->a.u32, ip->c.u32);
            break;
        case ByteCodeOp::CopySPVaargs:
        {
            concat.addStringFormat("__r_t vaargs%u[]={0,", vaargsIdx);
            int idxParam = (int) pushRAParams.size() - 1;
            while (idxParam >= 0)
            {
                concat.addStringFormat("r[%u],", pushRAParams[idxParam]);
                idxParam--;
            }

            CONCAT_FIXED_STR(concat, "};");
            concat.addStringFormat("r[%u].p=sizeof(__r_t)+(__u8_t*)&vaargs%u;", ip->a.u32, vaargsIdx);
            concat.addStringFormat("vaargs%u[0].p=r[%u].p;", vaargsIdx, ip->a.u32);
            vaargsIdx++;
            break;
        }

        case ByteCodeOp::MakeLambda:
        {
            auto funcBC = (ByteCode*) ip->b.pointer;
            SWAG_ASSERT(funcBC);

            emitLocalFuncSignature(concat, funcBC->callType(), funcBC->callName(), false);
            CONCAT_FIXED_STR(concat, ";\n");

            concat.addStringFormat("r[%u].p=(__u8_t*)&%s;", ip->a.u32, funcBC->callName().c_str());
            break;
        }
        case ByteCodeOp::MakeLambdaForeign:
        {
            auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->b.pointer, AstNodeKind::FuncDecl);
            SWAG_ASSERT(funcNode);
            SWAG_ASSERT(funcNode->attributeFlags & ATTRIBUTE_FOREIGN);
            TypeInfoFuncAttr* typeFuncNode = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
            ComputedValue     foreignValue;
            typeFuncNode->attributes.getValue("swag.foreign", "function", foreignValue);
            SWAG_ASSERT(!foreignValue.text.empty());

            emitForeignFuncSignature(concat, moduleToGen, typeFuncNode, funcNode, false);
            CONCAT_FIXED_STR(concat, ";\n");

            concat.addStringFormat("r[%u].u64=((__u64_t)&%s)|%llu;", ip->a.u32, foreignValue.text.c_str(), SWAG_LAMBDA_FOREIGN_MARKER);
            break;
        }
        case ByteCodeOp::LambdaCall:
        {
            TypeInfoFuncAttr* typeFuncNode = (TypeInfoFuncAttr*) ip->b.pointer;

            // Bytecode lambda call
            ///////////////////////////
            concat.addStringFormat("if(r[%u].u64&0x%llx){", ip->a.u32, SWAG_LAMBDA_BC_MARKER);

            CONCAT_STR_1(concat, "__process_infos.byteCodeRun(r[", ip->a.u32, "].p");
            if (typeFuncNode->numReturnRegisters() + typeFuncNode->numParamsRegisters())
                concat.addChar(',');
            addCallParameters(concat, typeFuncNode, pushRAParams);
            CONCAT_FIXED_STR(concat, ");");

            // Foreign lambda call
            ///////////////////////////
            concat.addStringFormat("}else if(r[%u].u64&0x%llx){", ip->a.u32, SWAG_LAMBDA_FOREIGN_MARKER);
            concat.addStringFormat("r[%u].u64&=~0x%llx;", ip->a.u32, SWAG_LAMBDA_FOREIGN_MARKER);
            SWAG_CHECK(emitForeignCallReturn(concat, moduleToGen, typeFuncNode));
            CONCAT_FIXED_STR(concat, "((");
            emitForeignFuncSignature(concat, moduleToGen, typeFuncNode, nullptr, false);
            CONCAT_FIXED_STR(concat, ")");
            concat.addStringFormat("r[%u].p)", ip->a.u32);
            emitForeignCallParameters(concat, moduleToGen, typeFuncNode, pushRAParams);
            CONCAT_FIXED_STR(concat, ";");

            // Local lambda call
            ///////////////////////////
            CONCAT_FIXED_STR(concat, "}else{");

            CONCAT_FIXED_STR(concat, "((");
            emitLocalFuncSignature(concat, typeFuncNode, "(*)", false);
            CONCAT_FIXED_STR(concat, ")");

            CONCAT_STR_1(concat, "r[", ip->a.u32, "].p)");
            concat.addChar('(');
            addCallParameters(concat, typeFuncNode, pushRAParams);
            CONCAT_FIXED_STR(concat, ");}");
            pushRAParams.clear();
            break;
        }

        case ByteCodeOp::LocalCall:
        {
            auto funcBC = (ByteCode*) ip->a.pointer;

            emitLocalFuncSignature(concat, funcBC->callType(), funcBC->callName(), false);
            CONCAT_FIXED_STR(concat, ";\n");

            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;
            concat.addString(funcBC->callName());
            concat.addChar('(');
            addCallParameters(concat, typeFuncBC, pushRAParams);
            pushRAParams.clear();
            CONCAT_FIXED_STR(concat, ");");
        }
        break;

        case ByteCodeOp::ForeignCall:
        {
            auto              nodeFunc     = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
            TypeInfoFuncAttr* typeFuncNode = (TypeInfoFuncAttr*) ip->b.pointer;
            SWAG_CHECK(emitForeignCall(concat, moduleToGen, typeFuncNode, nodeFunc, pushRAParams));
            break;
        }

        case ByteCodeOp::IntrinsicS8x1:
        {
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                concat.addStringFormat("r[%u].s8=(__s8_t)abs(r[%u].s8);", ip->a.u32, ip->b.u32);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS16x1:
        {
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                concat.addStringFormat("r[%u].s16=(__s16_t)abs(r[%u].s16);", ip->a.u32, ip->b.u32);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS32x1:
        {
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                concat.addStringFormat("r[%u].s32=abs(r[%u].s32);", ip->a.u32, ip->b.u32);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS64x1:
        {
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                concat.addStringFormat("r[%u].s64=llabs(r[%u].s64);", ip->a.u32, ip->b.u32);
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicF32x1:
        {
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                concat.addStringFormat("r[%u].f32=sqrtf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicSin:
                concat.addStringFormat("r[%u].f32=sinf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicCos:
                concat.addStringFormat("r[%u].f32=cosf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicTan:
                concat.addStringFormat("r[%u].f32=tanf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicSinh:
                concat.addStringFormat("r[%u].f32=sinhf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicCosh:
                concat.addStringFormat("r[%u].f32=coshf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicTanh:
                concat.addStringFormat("r[%u].f32=tanhf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicASin:
                concat.addStringFormat("r[%u].f32=asinf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicACos:
                concat.addStringFormat("r[%u].f32=acosf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicATan:
                concat.addStringFormat("r[%u].f32=atanf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicLog:
                concat.addStringFormat("r[%u].f32=logf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicLog2:
                concat.addStringFormat("r[%u].f32=log2f(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicLog10:
                concat.addStringFormat("r[%u].f32=log10f(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicFloor:
                concat.addStringFormat("r[%u].f32=floorf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicCeil:
                concat.addStringFormat("r[%u].f32=ceilf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicTrunc:
                concat.addStringFormat("r[%u].f32=truncf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicRound:
                concat.addStringFormat("r[%u].f32=roundf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicAbs:
                concat.addStringFormat("r[%u].f32=fabsf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicExp:
                concat.addStringFormat("r[%u].f32=expf(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicExp2:
                concat.addStringFormat("r[%u].f32=exp2f(r[%u].f32);", ip->a.u32, ip->b.u32);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicF64x1:
        {
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                concat.addStringFormat("r[%u].f64=sqrt(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicSin:
                concat.addStringFormat("r[%u].f64=sin(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicCos:
                concat.addStringFormat("r[%u].f64=cos(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicTan:
                concat.addStringFormat("r[%u].f64=tan(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicSinh:
                concat.addStringFormat("r[%u].f64=sinh(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicCosh:
                concat.addStringFormat("r[%u].f64=cosh(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicTanh:
                concat.addStringFormat("r[%u].f64=tanh(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicASin:
                concat.addStringFormat("r[%u].f64=asin(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicACos:
                concat.addStringFormat("r[%u].f64=acos(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicATan:
                concat.addStringFormat("r[%u].f64=atan(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicLog:
                concat.addStringFormat("r[%u].f64=log(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicLog2:
                concat.addStringFormat("r[%u].f64=log2(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicLog10:
                concat.addStringFormat("r[%u].f64=log10(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicFloor:
                concat.addStringFormat("r[%u].f64=floor(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicCeil:
                concat.addStringFormat("r[%u].f64=ceil(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicTrunc:
                concat.addStringFormat("r[%u].f64=trunc(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicRound:
                concat.addStringFormat("r[%u].f64=round(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicAbs:
                concat.addStringFormat("r[%u].f64=fabs(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicExp:
                concat.addStringFormat("r[%u].f64=exp(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            case TokenId::IntrinsicExp2:
                concat.addStringFormat("r[%u].f64=exp2(r[%u].f64);", ip->a.u32, ip->b.u32);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicF32x2:
        {
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicPow:
                concat.addStringFormat("r[%u].f32=powf(r[%u].f32,r[%u].f32);", ip->a.u32, ip->b.u32, ip->c.u32);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicF64x2:
        {
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicPow:
                concat.addStringFormat("r[%u].f64=pow(r[%u].f64,r[%u].f64);", ip->a.u32, ip->b.u32, ip->c.u32);
                break;
            }
            break;
        }

        default:
            ok = false;
            CONCAT_FIXED_STR(concat, "// ");
            concat.addString(g_ByteCodeOpNames[(int) ip->op]);
            moduleToGen->internalError(format("unknown instruction '%s' during C backend generation", g_ByteCodeOpNames[(int) ip->op]));
            break;
        }

        concat.addEol();
    }

    CONCAT_FIXED_STR(concat, "}\n");

    if (bc->node && bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC)
        CONCAT_FIXED_STR(concat, "#endif\n");

    return ok;
}

bool BackendC::emitPublic(OutputFile& buffeC, Module* moduleToGen, Scope* scope)
{
    if (!(scope->flags & SCOPE_FLAG_HAS_EXPORTS))
        return true;

    for (auto child : scope->childScopes)
        SWAG_CHECK(emitPublic(buffeC, moduleToGen, child));

    return true;
}