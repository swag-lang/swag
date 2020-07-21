#include "pch.h"
#include "BackendC.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "BackendCFunctionBodyJob.h"

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
            cType = "swag_bool_t";
            return true;
        case NativeTypeKind::S8:
            cType = "swag_int8_t";
            return true;
        case NativeTypeKind::S16:
            cType = "swag_int16_t";
            return true;
        case NativeTypeKind::S32:
            cType = "swag_int32_t";
            return true;
        case NativeTypeKind::S64:
            cType = "swag_int64_t";
            return true;
        case NativeTypeKind::U8:
            cType = "swag_uint8_t";
            return true;
        case NativeTypeKind::U16:
            cType = "swag_uint16_t";
            return true;
        case NativeTypeKind::U32:
            cType = "swag_uint32_t";
            return true;
        case NativeTypeKind::U64:
            cType = "swag_uint64_t";
            return true;
        case NativeTypeKind::F32:
            cType = "swag_float32_t";
            return true;
        case NativeTypeKind::F64:
            cType = "swag_float64_t";
            return true;
        case NativeTypeKind::Char:
            cType = "swag_char32_t";
            return true;
        case NativeTypeKind::Void:
            cType = "void";
            return true;
        }
    }

    return moduleToGen->internalError(format("swagTypeToCType, invalid type '%s'", typeInfo->name.c_str()));
}

bool BackendC::emitForeignCall(Concat& concat, Module* moduleToGen, ByteCodeInstruction* ip, VectorNative<uint32_t>& pushParams)
{
    auto              nodeFunc   = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

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
            CONCAT_FIXED_STR(concat, "rt[0].pointer = (swag_uint8_t*) ");
        }
        else if (returnType->kind == TypeInfoKind::Native)
        {
            switch (returnType->nativeType)
            {
            case NativeTypeKind::S8:
                CONCAT_FIXED_STR(concat, "rt[0].s8 = (swag_int8_t) ");
                break;
            case NativeTypeKind::U8:
                CONCAT_FIXED_STR(concat, "rt[0].u8 = (swag_int8_t) ");
                break;
            case NativeTypeKind::S16:
                CONCAT_FIXED_STR(concat, "rt[0].s16 = (swag_int16_t) ");
                break;
            case NativeTypeKind::U16:
                CONCAT_FIXED_STR(concat, "rt[0].u16 = (swag_uint16_t) ");
                break;
            case NativeTypeKind::S32:
                CONCAT_FIXED_STR(concat, "rt[0].s32 = (swag_int32_t) ");
                break;
            case NativeTypeKind::U32:
                CONCAT_FIXED_STR(concat, "rt[0].u32 = (swag_uint32_t) ");
                break;
            case NativeTypeKind::S64:
                CONCAT_FIXED_STR(concat, "rt[0].s64 = (swag_int64_t) ");
                break;
            case NativeTypeKind::U64:
                CONCAT_FIXED_STR(concat, "rt[0].u64 = (swag_uint64_t) ");
                break;
            case NativeTypeKind::Bool:
                CONCAT_FIXED_STR(concat, "rt[0].b = (swag_bool_t) ");
                break;
            case NativeTypeKind::F32:
                CONCAT_FIXED_STR(concat, "rt[0].f32 = (float) ");
                break;
            case NativeTypeKind::F64:
                CONCAT_FIXED_STR(concat, "rt[0].f64 = (double) ");
                break;
            default:
                return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid return type");
            }
        }
        else
        {
            return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid return type");
        }
    }

    ComputedValue foreignValue;
    if (typeFuncBC->attributes.getValue("swag.foreign", "function", foreignValue) && !foreignValue.text.empty())
        concat.addString(foreignValue.text);
    else
        concat.addString(nodeFunc->name);
    concat.addChar('(');

    int numCallParams = (int) typeFuncBC->parameters.size();

    // Variadic are first
    bool first = true;
    if (numCallParams)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters.back()->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic)
        {
            auto index = pushParams.back();
            pushParams.pop_back();
            CONCAT_STR_1(concat, "(void*)r[", index, "].pointer");
            index = pushParams.back();
            pushParams.pop_back();
            CONCAT_STR_1(concat, ", r[", index, "].u32");
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

        auto index = pushParams.back();
        pushParams.pop_back();

        // Access to the content of the register
        if (typeParam->kind == TypeInfoKind::Struct ||
            typeParam->kind == TypeInfoKind::Interface ||
            typeParam->kind == TypeInfoKind::Array ||
            typeParam->kind == TypeInfoKind::Pointer)
        {
            CONCAT_STR_1(concat, "(void*)r[", index, "].pointer");
        }
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            CONCAT_STR_1(concat, "(void*)r[", index, "].pointer");
            index = pushParams.back();
            pushParams.pop_back();
            CONCAT_STR_1(concat, ", r[", index, "].u32");
        }
        else if (typeParam->isNative(NativeTypeKind::Any))
        {
            CONCAT_STR_1(concat, "(void*)r[", index, "].pointer");
            index = pushParams.back();
            pushParams.pop_back();
            CONCAT_STR_1(concat, ", (void*)r[", index, "].pointer");
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
                return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid param native type");
            }
        }
        else
        {
            return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid param type");
        }
    }

    // Return by parameter
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
        CONCAT_FIXED_STR(concat, "rt[0].pointer");
    }

    CONCAT_FIXED_STR(concat, ");");

    return true;
}

bool BackendC::emitFuncWrapperPublic(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one)
{
    CONCAT_FIXED_STR(concat, "SWAG_EXPORT ");
    SWAG_CHECK(emitForeignFuncSignature(concat, moduleToGen, typeFunc, node, true));
    CONCAT_FIXED_STR(concat, " {\n");

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
        CONCAT_FIXED_STR(concat, "\tswag_register_t ");
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
        CONCAT_FIXED_STR(concat, "\trr0.pointer = result;\n");
    }

    auto numParams = typeFunc->parameters.size();

    // Variadic must be pushed first
    if (numParams)
    {
        auto param     = typeFunc->parameters.back();
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic)
        {
            concat.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s;\n", idx, param->namedParam.c_str());
            concat.addStringFormat("\trr%d.u32 = %s_count;\n", idx + 1, param->namedParam.c_str());
            idx += 2;
            numParams--;
        }
    }

    for (int i = 0; i < numParams; i++)
    {
        auto param     = typeFunc->parameters[i];
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (typeParam->kind == TypeInfoKind::Pointer)
        {
            concat.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s;\n", idx, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            concat.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s;\n", idx, param->namedParam.c_str());
            concat.addStringFormat("\trr%d.u32 = %s_count;\n", idx + 1, param->namedParam.c_str());
        }
        else if (typeParam->isNative(NativeTypeKind::Any))
        {
            concat.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s_value;\n", idx, param->namedParam.c_str());
            concat.addStringFormat("\trr%d.pointer = %s_type;\n", idx + 1, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Struct ||
                 typeParam->kind == TypeInfoKind::Array ||
                 typeParam->kind == TypeInfoKind::Interface)
        {
            concat.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s;\n", idx, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            switch (typeParam->nativeType)
            {
            case NativeTypeKind::U8:
                concat.addStringFormat("\trr%d.u8 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::U16:
                concat.addStringFormat("\trr%d.u16 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::U32:
                concat.addStringFormat("\trr%d.u32 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::U64:
                concat.addStringFormat("\trr%d.u64 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S8:
                concat.addStringFormat("\trr%d.s8 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S16:
                concat.addStringFormat("\trr%d.s16 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S32:
                concat.addStringFormat("\trr%d.s32 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S64:
                concat.addStringFormat("\trr%d.s64 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::F32:
                concat.addStringFormat("\trr%d.f32 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::F64:
                concat.addStringFormat("\trr%d.f64 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::Bool:
                concat.addStringFormat("\trr%d.b = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::Char:
                concat.addStringFormat("\trr%d.ch = %s;\n", idx, param->namedParam.c_str());
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
    concat.addChar('\t');
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
            CONCAT_FIXED_STR(concat, "\t*((void **) result) = rr0.pointer;\n");
            CONCAT_FIXED_STR(concat, "\t*((void **) result + 1) = rr1.pointer;\n");
        }
        else if (returnType->kind == TypeInfoKind::Interface)
        {
            CONCAT_FIXED_STR(concat, "\t*((void **) result) = rr0.pointer;\n");
            CONCAT_FIXED_STR(concat, "\t*((void **) result + 1) = rr1.pointer;\n");
        }
        else if (returnType->kind == TypeInfoKind::Pointer)
        {
            Utf8 returnCast;
            SWAG_CHECK(swagTypeToCType(moduleToGen, typeFunc->returnType, returnCast));
            concat.addStringFormat("\treturn (%s) rr0.pointer;\n", returnCast.c_str());
        }
        else if (returnType->kind == TypeInfoKind::Native)
        {
            switch (returnType->nativeType)
            {
            case NativeTypeKind::U8:
                CONCAT_FIXED_STR(concat, "\treturn rr0.u8;\n");
                break;
            case NativeTypeKind::U16:
                CONCAT_FIXED_STR(concat, "\treturn rr0.u16;\n");
                break;
            case NativeTypeKind::U32:
                CONCAT_FIXED_STR(concat, "\treturn rr0.u32;\n");
                break;
            case NativeTypeKind::U64:
                CONCAT_FIXED_STR(concat, "\treturn rr0.u64;\n");
                break;
            case NativeTypeKind::S8:
                CONCAT_FIXED_STR(concat, "\treturn rr0.s8;\n");
                break;
            case NativeTypeKind::S16:
                CONCAT_FIXED_STR(concat, "\treturn rr0.s16;\n");
                break;
            case NativeTypeKind::S32:
                CONCAT_FIXED_STR(concat, "\treturn rr0.s32;\n");
                break;
            case NativeTypeKind::S64:
                CONCAT_FIXED_STR(concat, "\treturn rr0.s64;\n");
                break;
            case NativeTypeKind::F32:
                CONCAT_FIXED_STR(concat, "\treturn rr0.f32;\n");
                break;
            case NativeTypeKind::F64:
                CONCAT_FIXED_STR(concat, "\treturn rr0.f64;\n");
                break;
            case NativeTypeKind::Char:
                CONCAT_FIXED_STR(concat, "\treturn rr0.ch;\n");
                break;
            case NativeTypeKind::Bool:
                CONCAT_FIXED_STR(concat, "\treturn rr0.b;\n");
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

    CONCAT_FIXED_STR(concat, "}\n\n");
    return true;
}

bool BackendC::emitForeignFuncSignature(Concat& buffer, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, bool forExport)
{
    Utf8 returnType;
    bool returnByCopy = typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY;

    SWAG_CHECK(swagTypeToCType(moduleToGen, typeFunc->returnType, returnType));
    if (returnByCopy)
        CONCAT_FIXED_STR(buffer, "void");
    else if (typeFunc->numReturnRegisters() <= 1)
        buffer.addString(returnType);
    else
        CONCAT_FIXED_STR(buffer, "void");
    buffer.addChar(' ');
    auto name = Ast::computeFullNameForeign(node, forExport);
    buffer.addString(name);
    buffer.addChar('(');

    bool first = true;
    if (node->parameters)
    {
        // Variadic parameters are always first
        auto numParams = node->parameters->childs.size();
        if (numParams)
        {
            auto param = node->parameters->childs.back();
            if (param->typeInfo->kind == TypeInfoKind::Variadic)
            {
                CONCAT_FIXED_STR(buffer, "void* ");
                buffer.addString(param->name);
                CONCAT_FIXED_STR(buffer, ",swag_uint32_t ");
                buffer.addString(param->name);
                CONCAT_FIXED_STR(buffer, "_count");
                numParams--;
                first = false;
            }
        }

        Utf8 cType;
        for (int i = 0; i < numParams; i++)
        {
            auto param = node->parameters->childs[i];
            if (!first)
                buffer.addChar(',');
            first = false;

            SWAG_CHECK(swagTypeToCType(moduleToGen, param->typeInfo, cType));
            buffer.addString(cType);
            buffer.addChar(' ');
            buffer.addString(param->name);

            if (param->typeInfo->kind == TypeInfoKind::Slice || param->typeInfo->isNative(NativeTypeKind::String))
            {
                CONCAT_FIXED_STR(buffer, ",swag_uint32_t ");
                buffer.addString(param->name);
                CONCAT_FIXED_STR(buffer, "_count");
            }
            else if (param->typeInfo->isNative(NativeTypeKind::Any))
            {
                CONCAT_FIXED_STR(buffer, "_value,void* ");
                buffer.addString(param->name);
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
        CONCAT_FIXED_STR(buffer, " result");
    }

    CONCAT_FIXED_STR(buffer, ")");
    return true;
}

void BackendC::emitFuncSignatureInternalC(Concat& concat, ByteCode* bc)
{
    auto typeFunc = bc->callType();
    auto name     = bc->callName();

    CONCAT_FIXED_STR(concat, "void ");
    concat.addString(name);
    concat.addChar('(');

    // Result
    for (int i = 0; i < typeFunc->numReturnRegisters(); i++)
    {
        if (i)
            concat.addChar(',');
        concat.addStringFormat("swag_register_t* rr%d", i);
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
            concat.addStringFormat("swag_register_t* rp%u", index++);
        }
    }

    CONCAT_FIXED_STR(concat, ")");
}

bool BackendC::emitAllFuncSignatureInternalC(OutputFile& bufferC, Module* moduleToGen)
{
    SWAG_ASSERT(moduleToGen);

    for (auto one : moduleToGen->byteCodeFunc)
    {
        TypeInfoFuncAttr* typeFunc = one->typeInfoFunc;
        AstFuncDecl*      node     = nullptr;

        if (one->node)
        {
            node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

            // Do we need to generate that function ?
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
                continue;
            if (node->attributeFlags & ATTRIBUTE_FOREIGN)
                continue;

            typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        }

        emitFuncSignatureInternalC(bufferC, one);
        CONCAT_FIXED_STR(bufferC, ";\n");
    }

    bufferC.addEol();
    return true;
}

bool BackendC::emitAllFuncSignatureInternalC(OutputFile& bufferC)
{
    emitSeparator(bufferC, "PROTOTYPES");
    if (!emitAllFuncSignatureInternalC(bufferC, g_Workspace.bootstrapModule))
        return false;
    if (!emitAllFuncSignatureInternalC(bufferC, module))
        return false;

    // Import functions
    for (auto node : module->allForeign)
    {
        if (node->flags & AST_USED_FOREIGN)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            CONCAT_FIXED_STR(bufferC, "SWAG_IMPORT ");
            SWAG_CHECK(emitForeignFuncSignature(bufferC, module, typeFunc, node, false));
            CONCAT_FIXED_STR(bufferC, ";\n");
        }
    }

    return true;
}

bool BackendC::emitFunctionBody(Concat& concat, Module* moduleToGen, ByteCode* bc)
{
    concat.addEol();

    bool ok       = true;
    auto typeFunc = bc->callType();

    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC))
        CONCAT_FIXED_STR(concat, "#ifdef SWAG_HAS_TEST\n");

    // Signature
    emitFuncSignatureInternalC(concat, bc);
    CONCAT_FIXED_STR(concat, " {\n");

    // Generate one local variable per used register
    if (bc->maxReservedRegisterRC)
    {
        CONCAT_STR_1(concat, "swag_register_t r[", bc->maxReservedRegisterRC, "];\n");
    }

    // For function call results
    if (bc->maxCallResults)
    {
        CONCAT_STR_1(concat, "swag_register_t rt[", bc->maxCallResults, "];\n");
    }

    // Local stack
    if (typeFunc->stackSize)
    {
        CONCAT_STR_1(concat, "swag_uint8_t stack[", typeFunc->stackSize, "];\n");
    }

    // Generate bytecode
    int                    vaargsIdx = 0;
    auto                   ip        = bc->out;
    int                    lastLine  = -1;
    VectorNative<uint32_t> pushRAParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
            continue;

        // Print source code
        if (!bc->compilerGenerated)
        {
            if (moduleToGen->buildParameters.target.backendC.writeSourceCode || g_CommandLine.debug)
            {
                if (ip->node->token.startLocation.line != lastLine)
                {
                    if (ip->node->token.startLocation.column != ip->node->token.endLocation.column)
                    {
                        lastLine = ip->node->token.startLocation.line;
                        auto s   = bc->sourceFile->getLine(ip->node->token.startLocation.seekStartLine[REPORT_NUM_CODE_LINES - 1]);
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

        CONCAT_FIXED_STR(concat, "_");
        concat.addS32Str8(i);
        CONCAT_FIXED_STR(concat, ":; ");

        // Write the bytecode instruction name
        if (moduleToGen->buildParameters.target.backendC.writeByteCodeInstruction || g_CommandLine.debug)
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
        case ByteCodeOp::DecSP:
        case ByteCodeOp::IncSP:
        case ByteCodeOp::CopySPtoBP:
            break;

        case ByteCodeOp::BoundCheckString:
            concat.addStringFormat("swag_runtime_assert(r[%u].u32 <= r[%u].u32 + 1, \"%s\", %d, \": index out of range\");", ip->a.u32, ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            break;
        case ByteCodeOp::BoundCheck:
            concat.addStringFormat("swag_runtime_assert(r[%u].u32 < r[%u].u32, \"%s\", %d, \": index out of range\");", ip->a.u32, ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            break;
        case ByteCodeOp::IncPointerVB32:
            concat.addStringFormat("r[%u].pointer += %d;", ip->a.u32, ip->b.s32);
            break;
        case ByteCodeOp::IncPointer32:
            concat.addStringFormat("r[%u].pointer = r[%u].pointer + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DecPointer32:
            concat.addStringFormat("r[%u].pointer = r[%u].pointer - r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DeRef8:
            concat.addStringFormat("r[%u].u64 = *(swag_uint8_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef16:
            concat.addStringFormat("r[%u].u64 = *(swag_uint16_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef32:
            concat.addStringFormat("r[%u].u64 = *(swag_uint32_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef64:
            concat.addStringFormat("r[%u].u64 = *(swag_uint64_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRefPointer:
            concat.addStringFormat("r[%u].pointer = *(swag_uint8_t**) (r[%u].pointer + %u);", ip->b.u32, ip->a.u32, ip->c.u32);
            break;
        case ByteCodeOp::DeRefStringSlice:
            concat.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (r[%u].pointer + 8); ", ip->b.u32, ip->a.u32);
            concat.addStringFormat("r[%u].pointer = *(swag_uint8_t**) r[%u].pointer; ", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::MulRAVB:
            concat.addStringFormat("r[%u].s64 *= %u;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DivRAVB:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].u32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("r[%u].s64 /= %u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::GetFromDataSeg8:
            concat.addStringFormat("r[%u].u8 = *(swag_uint8_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromDataSeg16:
            concat.addStringFormat("r[%u].u16 = *(swag_uint16_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromDataSeg32:
            concat.addStringFormat("r[%u].u32 = *(swag_uint32_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromDataSeg64:
            concat.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::GetFromBssSeg8:
            concat.addStringFormat("r[%u].u8 = *(swag_uint8_t*) (__bssseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromBssSeg16:
            concat.addStringFormat("r[%u].u16 = *(swag_uint16_t*) (__bssseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromBssSeg32:
            concat.addStringFormat("r[%u].u32 = *(swag_uint32_t*) (__bssseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromBssSeg64:
            concat.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (__bssseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearXVar:
            concat.addStringFormat("swag_runtime_memset(r[%u].pointer, 0, r[%u].u32 * %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::SetZeroStack8:
            CONCAT_STR_1(concat, "*(swag_uint8_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroStack16:
            CONCAT_STR_1(concat, "*(swag_uint16_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroStack32:
            CONCAT_STR_1(concat, "*(swag_uint32_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroStack64:
            CONCAT_STR_1(concat, "*(swag_uint64_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroStackX:
            concat.addStringFormat("swag_runtime_memset(stack + %u, 0, %u);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::MakeDataSegPointer:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = __mutableseg + ", ip->b.u32, ";");
            break;
        case ByteCodeOp::MakeBssSegPointer:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = __bssseg + ", ip->b.u32, ";");
            break;
        case ByteCodeOp::MakeConstantSegPointer:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) (__constantseg + %u); ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::MakeConstantSegPointerOC:
            concat.addStringFormat("r[%u].pointer = __constantseg + %u; ", ip->a.u32, (uint32_t)(ip->c.u64 >> 32));
            concat.addStringFormat("r[%u].u64 = %u;", ip->b.u32, (ip->c.u64) & 0xFFFFFFFF);
            break;

        case ByteCodeOp::MakePointerToStack:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = stack + ", ip->b.u32, ";");
            break;
        case ByteCodeOp::GetFromStack8:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].u8 = *(swag_uint8_t*) (stack + ", ip->b.u32, ");");
            break;
        case ByteCodeOp::GetFromStack16:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].u16 = *(swag_uint16_t*) (stack + ", ip->b.u32, ");");
            break;
        case ByteCodeOp::GetFromStack32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].u32 = *(swag_uint32_t*) (stack + ", ip->b.u32, ");");
            break;
        case ByteCodeOp::GetFromStack64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].u64 = *(swag_uint64_t*) (stack + ", ip->b.u32, ");");
            break;

        case ByteCodeOp::MemCpy:
            concat.addStringFormat("swag_runtime_memcpy((void*) r[%u].pointer, (void*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::MemSet:
            concat.addStringFormat("swag_runtime_memset((void*) r[%u].pointer, r[%u].u8, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::MemCmp:
            concat.addStringFormat("r[%u].s32 = swag_runtime_memcmp((void*) r[%u].pointer, (void*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32);
            break;

        case ByteCodeOp::CopyVC:
            concat.addStringFormat("swag_runtime_memcpy(r[%u].pointer, r[%u].pointer, %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::CopyVBtoRA32:
            concat.addStringFormat("r[%u].u32 = 0x%x;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyVBtoRA64:
            concat.addStringFormat("r[%u].u64 = 0x%I64x;", ip->a.u32, ip->b.u64);
            break;
        case ByteCodeOp::CopyRBtoRA:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "] = r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRBAddrToRA:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = (swag_uint8_t*) &r[", ip->b.u32, "];");
            break;

        case ByteCodeOp::ClearRA:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].u64 = 0;");
            break;
        case ByteCodeOp::DecrementRA32:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32--;");
            break;
        case ByteCodeOp::IncrementRA32:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32++;");
            break;
        case ByteCodeOp::IncrementRA64:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].u64++;");
            break;
        case ByteCodeOp::AddVBtoRA32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].u32 += ", ip->b.u32, ";");
            break;

        case ByteCodeOp::SetAtPointer8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) = r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::SetAtPointer16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) = r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::SetAtPointer32:
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer + %u) = r[%u].u32;", ip->a.u32, ip->c.u32, ip->b.u32);
            break;
        case ByteCodeOp::SetAtPointer64:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer + %u) = r[%u].u64;", ip->a.u32, ip->c.u32, ip->b.u32);
            break;
        case ByteCodeOp::SetZeroAtPointer8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroAtPointer16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroAtPointer32:
            CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroAtPointer64:
            CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroAtPointerX:
            concat.addStringFormat("swag_runtime_memset(r[%u].pointer, 0, %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::SetPointerAtPointer:
            CONCAT_STR_2(concat, "*(void**)(r[", ip->a.u32, "].pointer) = r[", ip->b.u32, "].pointer;");
            break;

        case ByteCodeOp::BinOpPlusS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 + r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 + r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusF32:
            concat.addStringFormat("r[%u].f32 = r[%u].f32 + r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusF64:
            concat.addStringFormat("r[%u].f64= r[%u].f64 + r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpMinusS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 - r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 - r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusF32:
            concat.addStringFormat("r[%u].f32 = r[%u].f32 - r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusF64:
            concat.addStringFormat("r[%u].f64 = r[%u].f64 - r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpMulS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 * r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 * r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulF32:
            concat.addStringFormat("r[%u].f32 = r[%u].f32 * r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulF64:
            concat.addStringFormat("r[%u].f64= r[%u].f64 * r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpXorS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 ^ r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpXorS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 ^ r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpShiftLeft32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpShiftLeft64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpShiftRightU32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpShiftRightU64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpShiftRightU64VB:
            concat.addStringFormat("r[%u].u64 >>= %u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpModuloS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 %% r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 %% r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloU32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 %% r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloU64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 %% r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpDivS32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].s32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("r[%u].s32 = r[%u].s32 / r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivS64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].s64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("r[%u].s64 = r[%u].s64 / r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivU32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].u32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("r[%u].u32 = r[%u].u32 / r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivU64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].u64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("r[%u].u64 = r[%u].u64 / r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivF32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].f32 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("r[%u].f32 = r[%u].f32 / r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivF64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].f64 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("r[%u].f64 = r[%u].f64 / r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpMinusEqU8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpMinusEqU16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpMinusEqU32:
            CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpMinusEqU64:
            CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].u64;");
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            CONCAT_STR_2(concat, "*(swag_float32_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].f32;");
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            CONCAT_STR_2(concat, "*(swag_float64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].f64;");
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpPlusEqU8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpPlusEqU16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpPlusEqU32:
            CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpPlusEqU64:
            CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].u64;");
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            CONCAT_STR_2(concat, "*(swag_float32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].f32;");
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            CONCAT_STR_2(concat, "*(swag_float64_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].f64;");
            break;

        case ByteCodeOp::AffectOpPlusEqPointer:
            CONCAT_STR_2(concat, "*(swag_uint8_t**)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpMinusEqPointer:
            CONCAT_STR_2(concat, "*(swag_uint8_t**)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s32;");
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpMulEqU8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpMulEqU16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpMulEqU32:
            CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpMulEqU64:
            CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].u64;");
            break;
        case ByteCodeOp::AffectOpMulEqF32:
            CONCAT_STR_2(concat, "*(swag_float32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].f32;");
            break;
        case ByteCodeOp::AffectOpMulEqF64:
            CONCAT_STR_2(concat, "*(swag_float64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].f64;");
            break;

        case ByteCodeOp::AffectOpDivEqS8:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].s8, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) /= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqS16:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].s16, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) /= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqS32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].s32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) /= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqS64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].s64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) /= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU8:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].u8, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) /= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU16:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].u16, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) /= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].u32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) /= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].u64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) /= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqF32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].f32 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("*(swag_float32_t*)(r[%u].pointer) /= r[%u].f32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("swag_runtime_assert(r[%u].f64 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            concat.addStringFormat("*(swag_float64_t*)(r[%u].pointer) /= r[%u].f64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpAndEqS8:
            CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpAndEqS16:
            CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpAndEqS32:
            CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpAndEqS64:
            CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpAndEqU8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpAndEqU16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpAndEqU32:
            CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpAndEqU64:
            CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u64;");
            break;

        case ByteCodeOp::AffectOpOrEqS8:
            CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpOrEqS16:
            CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpOrEqS32:
            CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpOrEqS64:
            CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpOrEqU8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpOrEqU16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpOrEqU32:
            CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpOrEqU64:
            CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].u64;");
            break;

        case ByteCodeOp::AffectOpShiftLeftEqS8:
            CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS16:
            CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS32:
            CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS64:
            CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;

        case ByteCodeOp::AffectOpPercentEqS8:
            CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpPercentEqS16:
            CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpPercentEqS32:
            CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpPercentEqS64:
            CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpPercentEqU8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpPercentEqU16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpPercentEqU32:
            CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpPercentEqU64:
            CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u64;");
            break;

        case ByteCodeOp::AffectOpXOrEqS8:
            CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpXOrEqS16:
            CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpXOrEqS32:
            CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpXOrEqS64:
            CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpXOrEqU8:
            CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpXOrEqU16:
            CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpXOrEqU32:
            CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpXOrEqU64:
            CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].u64;");
            break;

        case ByteCodeOp::CompareOpGreaterS32:
            concat.addStringFormat("r[%u].b = r[%u].s32 > r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            concat.addStringFormat("r[%u].b = r[%u].s64 > r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterU32:
            concat.addStringFormat("r[%u].b = r[%u].u32 > r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            concat.addStringFormat("r[%u].b = r[%u].u64 > r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            concat.addStringFormat("r[%u].b = r[%u].f32 > r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            concat.addStringFormat("r[%u].b = r[%u].f64 > r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterPointer:
            concat.addStringFormat("r[%u].b = r[%u].pointer > r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CompareOpLowerS32:
            concat.addStringFormat("r[%u].b = r[%u].s32 < r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerS64:
            concat.addStringFormat("r[%u].b = r[%u].s64 < r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerU32:
            concat.addStringFormat("r[%u].b = r[%u].u32 < r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerU64:
            concat.addStringFormat("r[%u].b = r[%u].u64 < r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerF32:
            concat.addStringFormat("r[%u].b = r[%u].f32 < r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerF64:
            concat.addStringFormat("r[%u].b = r[%u].f64 < r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerPointer:
            concat.addStringFormat("r[%u].b = r[%u].pointer < r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CompareOpEqual8:
            concat.addStringFormat("r[%u].b = r[%u].u8 == r[%u].u8;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual16:
            concat.addStringFormat("r[%u].b = r[%u].u16 == r[%u].u16;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual32:
            concat.addStringFormat("r[%u].b = r[%u].u32 == r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual64:
            concat.addStringFormat("r[%u].b = r[%u].u64 == r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualPointer:
            concat.addStringFormat("r[%u].b = r[%u].pointer == r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualInterface:
            concat.addStringFormat("r[%u].b = (((void **) r[%u].pointer)[0] == ((void **) r[%u].pointer)[0]) && (((void **) r[%u].pointer)[1] == ((void **) r[%u].pointer)[1]);", ip->c.u32, ip->a.u32, ip->b.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualBool:
            concat.addStringFormat("r[%u].b = r[%u].b == r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualString:
            concat.addStringFormat("r[%u].b = swag_runtime_strcmp((const char*) r[%u].pointer, (const char*) r[%u].pointer, r[%u].u32);", ip->c.u32, ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::IsNullString:
            concat.addStringFormat("r[%u].b = r[%u].pointer == 0;", ip->b.u32, ip->a.u32);
            break;

        case ByteCodeOp::BinOpAnd:
            concat.addStringFormat("r[%u].b = r[%u].b && r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpOr:
            concat.addStringFormat("r[%u].b = r[%u].b || r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BitmaskAndS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 & r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskAndS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 & r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskAndU32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 & r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskAndU64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 & r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 | r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 | r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrU32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 | r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrU64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 | r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::Jump:
            CONCAT_FIXED_STR(concat, "goto _");
            concat.addS32Str8(ip->a.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfNotTrue:
            CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].b) goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfTrue:
            CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].b) goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfZero32:
            CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u32) goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfZero64:
            CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u64) goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfNotZero32:
            CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].u32) goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfNotZero64:
            CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].u64) goto _");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::Ret:
            CONCAT_FIXED_STR(concat, "return;");
            break;

        case ByteCodeOp::IntrinsicPrintS64:
            CONCAT_STR_1(concat, "swag_runtime_print_i64(r[", ip->a.u32, "].s64);");
            break;
        case ByteCodeOp::IntrinsicPrintF64:
            CONCAT_STR_1(concat, "swag_runtime_print_f64(r[", ip->a.u32, "].f64);");
            break;
        case ByteCodeOp::IntrinsicPrintString:
            concat.addStringFormat("swag_runtime_print_n((const char*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::IntrinsicAssert:
            concat.addStringFormat("swag_runtime_assert(r[%u].b, \"%s\", %d, 0);", ip->a.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            break;
        case ByteCodeOp::IntrinsicAssertCastAny:
            CONCAT_FIXED_STR(concat, "{ ");
            concat.addStringFormat("const char* typeTo = *(char**)r[%u].pointer; ", ip->b.u32);
            concat.addStringFormat("const char* typeFrom = *(char**)r[%u].pointer; ", ip->c.u32);
            concat.addStringFormat("swag_runtime_assert(r[%u].b, \"%s\", %d, \"invalid cast from 'any'\");", ip->a.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            CONCAT_FIXED_STR(concat, "}");
            break;
        case ByteCodeOp::IntrinsicTarget:
            concat.addStringFormat("r[%u].pointer = (void*) 0;", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicAlloc:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) swag_runtime_malloc(r[%u].u32);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::IntrinsicRealloc:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) swag_runtime_realloc(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::IntrinsicFree:
            concat.addStringFormat("swag_runtime_free(r[%u].pointer);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicGetContext:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) swag_runtime_tlsGetValue(__process_infos.contextTlsId);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicSetContext:
            concat.addStringFormat("swag_runtime_tlsSetValue(__process_infos.contextTlsId, r[%u].pointer);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicArguments:
            concat.addStringFormat("r[%u].pointer = __process_infos.arguments.addr;", ip->a.u32);
            concat.addStringFormat("r[%u].u64 = __process_infos.arguments.count;", ip->b.u32);
            break;
        case ByteCodeOp::IntrinsicIsByteCode:
            CONCAT_STR_1(concat, "r[", ip->a.u32, "].b = 0;");
            break;

        case ByteCodeOp::NegBool:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].b ? 0 : 1;");
            break;
        case ByteCodeOp::NegF32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = -r[", ip->a.u32, "].f32;");
            break;
        case ByteCodeOp::NegF64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = -r[", ip->a.u32, "].f64;");
            break;
        case ByteCodeOp::NegS32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = -r[", ip->a.u32, "].s32;");
            break;
        case ByteCodeOp::NegS64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = -r[", ip->a.u32, "].s64;");
            break;

        case ByteCodeOp::InvertS8:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s8 = ~r[", ip->a.u32, "].s8;");
            break;
        case ByteCodeOp::InvertS16:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16 = ~r[", ip->a.u32, "].s16;");
            break;
        case ByteCodeOp::InvertS32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = ~r[", ip->a.u32, "].s32;");
            break;
        case ByteCodeOp::InvertS64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = ~r[", ip->a.u32, "].s64;");
            break;
        case ByteCodeOp::InvertU8:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].u8 = ~r[", ip->a.u32, "].u8;");
            break;
        case ByteCodeOp::InvertU16:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].u16 = ~r[", ip->a.u32, "].u16;");
            break;
        case ByteCodeOp::InvertU32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].u32 = ~r[", ip->a.u32, "].u32;");
            break;
        case ByteCodeOp::InvertU64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].u64 = ~r[", ip->a.u32, "].u64;");
            break;

        case ByteCodeOp::ClearMaskU32:
            concat.addStringFormat("r[%u].u32 &= 0x%x;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearMaskU64:
            concat.addStringFormat("r[%u].u64 &= 0x%llx;", ip->a.u32, ip->b.u64);
            break;

        case ByteCodeOp::CastBool8:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u8 ? 1 : 0;");
            break;
        case ByteCodeOp::CastBool16:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u16 ? 1 : 0;");
            break;
        case ByteCodeOp::CastBool32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u32 ? 1 : 0;");
            break;
        case ByteCodeOp::CastBool64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u64 ? 1 : 0;");
            break;

        case ByteCodeOp::CastS8S16:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16 = (swag_int16_t) r[", ip->a.u32, "].s8;");
            break;
        case ByteCodeOp::CastS16S32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = (swag_int32_t) r[", ip->a.u32, "].s16;");
            break;
        case ByteCodeOp::CastS32S64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = (swag_int64_t) r[", ip->a.u32, "].s32;");
            break;
        case ByteCodeOp::CastS32F32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (swag_float32_t) r[", ip->a.u32, "].s32;");
            break;
        case ByteCodeOp::CastS64F32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (swag_float32_t) r[", ip->a.u32, "].s64;");
            break;
        case ByteCodeOp::CastS64F64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (swag_float64_t) r[", ip->a.u32, "].s64;");
            break;
        case ByteCodeOp::CastU32F32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (swag_float32_t) r[", ip->a.u32, "].u32;");
            break;
        case ByteCodeOp::CastU64F32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (swag_float32_t) r[", ip->a.u32, "].u64;");
            break;
        case ByteCodeOp::CastU64F64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (swag_float64_t) r[", ip->a.u32, "].u64;");
            break;
        case ByteCodeOp::CastF32S32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = (swag_int32_t) r[", ip->a.u32, "].f32;");
            break;
        case ByteCodeOp::CastF32F64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (swag_float64_t) r[", ip->a.u32, "].f32;");
            break;
        case ByteCodeOp::CastF64S64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = (swag_int64_t) r[", ip->a.u32, "].f64;");
            break;
        case ByteCodeOp::CastF64F32:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (swag_float32_t) r[", ip->a.u32, "].f64;");
            break;

        case ByteCodeOp::CopyRCtoRR:
            CONCAT_STR_2(concat, "*rr", ip->a.u32, " = r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRRtoRC:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "] = *rr", ip->b.u32, ";");
            break;
        case ByteCodeOp::CopyRCtoRRCall:
            CONCAT_STR_2(concat, "rt[", ip->a.u32, "] = r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRRtoRCCall:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "] = rt[", ip->b.u32, "];");
            break;
        case ByteCodeOp::GetFromStackParam64:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "] = *rp", ip->c.u32, ";");
            break;
        case ByteCodeOp::MakePointerToStackParam:
            CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = (swag_uint8_t*) &rp", ip->c.u32, "->pointer;");
            break;
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
            break;

        case ByteCodeOp::MinusToTrue:
            concat.addStringFormat("r[%u].b = r[%u].s32 < 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::MinusZeroToTrue:
            concat.addStringFormat("r[%u].b = r[%u].s32 <= 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::PlusToTrue:
            concat.addStringFormat("r[%u].b = r[%u].s32 > 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::PlusZeroToTrue:
            concat.addStringFormat("r[%u].b = r[%u].s32 >= 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::MakeLambda:
        {
            auto funcBC = (ByteCode*) ip->b.pointer;
            SWAG_ASSERT(funcBC);
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) &%s;", ip->a.u32, funcBC->callName().c_str());
        }
        break;

        case ByteCodeOp::MakeLambdaForeign:
        {
            auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->b.pointer, AstNodeKind::FuncDecl);
            SWAG_ASSERT(funcNode);
            SWAG_ASSERT(funcNode->attributeFlags & ATTRIBUTE_FOREIGN);
            TypeInfoFuncAttr* typeFuncNode = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
            ComputedValue     foreignValue;
            typeFuncNode->attributes.getValue("swag.foreign", "function", foreignValue);
            SWAG_ASSERT(!foreignValue.text.empty());
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) &%s;", ip->a.u32, foreignValue.text.c_str());
            break;
        }

        case ByteCodeOp::PushRAParam:
            pushRAParams.push_back(ip->a.u32);
            break;
        case ByteCodeOp::CopySP:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) &r[%u];", ip->a.u32, ip->c.u32);
            break;
        case ByteCodeOp::CopySPVaargs:
        {
            concat.addStringFormat("swag_register_t vaargs%u[] = { 0, ", vaargsIdx);
            int idxParam = (int) pushRAParams.size() - 1;
            while (idxParam >= 0)
            {
                concat.addStringFormat("r[%u], ", pushRAParams[idxParam]);
                idxParam--;
            }

            CONCAT_FIXED_STR(concat, "}; ");
            concat.addStringFormat("r[%u].pointer = sizeof(swag_register_t) + (swag_uint8_t*) &vaargs%u; ", ip->a.u32, vaargsIdx);
            concat.addStringFormat("vaargs%u[0].pointer = r[%u].pointer;", vaargsIdx, ip->a.u32);
            vaargsIdx++;
            break;
        }

        case ByteCodeOp::LambdaCall:
        {
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;
            concat.addStringFormat("if(r[%u].u64 & 0x%llx) { ", ip->a.u32, SWAG_LAMBDA_MARKER);

            CONCAT_STR_1(concat, "__process_infos.byteCodeRun(r[", ip->a.u32, "].pointer");
            if (typeFuncBC->numReturnRegisters() + typeFuncBC->numParamsRegisters())
                concat.addChar(',');
            addCallParameters(concat, typeFuncBC, pushRAParams);
            CONCAT_FIXED_STR(concat, ");");

            // Need to output the function prototype too
            CONCAT_FIXED_STR(concat, "} else { ((void(*)(");
            for (int j = 0; j < typeFuncBC->numReturnRegisters() + typeFuncBC->numParamsRegisters(); j++)
            {
                if (j)
                    concat.addChar(',');
                CONCAT_FIXED_STR(concat, "swag_register_t*");
            }

            CONCAT_FIXED_STR(concat, "))");

            // Then the call
            CONCAT_STR_1(concat, " r[", ip->a.u32, "].pointer)");

            // Then the parameters
            concat.addChar('(');
            addCallParameters(concat, typeFuncBC, pushRAParams);
            CONCAT_FIXED_STR(concat, "); }");
            pushRAParams.clear();
        }
        break;

        case ByteCodeOp::LocalCall:
        {
            auto              funcBC     = (ByteCode*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;
            concat.addString(funcBC->callName());
            concat.addChar('(');
            addCallParameters(concat, typeFuncBC, pushRAParams);
            pushRAParams.clear();
            CONCAT_FIXED_STR(concat, ");");
        }
        break;

        case ByteCodeOp::ForeignCall:
            SWAG_CHECK(emitForeignCall(concat, moduleToGen, ip, pushRAParams));
            break;

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