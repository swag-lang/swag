#include "pch.h"
#include "BackendC.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "BackendCFunctionBodyJob.h"

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
        if (typeInfoPointer->finalType->kind == TypeInfoKind::Struct || typeInfoPointer->finalType->kind == TypeInfoKind::Slice)
            ptrCount--;
        for (uint32_t i = 0; i < ptrCount; i++)
            cType += "*";
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Struct || typeInfo->kind == TypeInfoKind::Interface)
    {
        cType = "void*";
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
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
        case NativeTypeKind::String:
            cType = "const char*";
            return true;
        case NativeTypeKind::Void:
            cType = "void";
            return true;
        }
    }

    return moduleToGen->internalError(format("swagTypeToCType, invalid type '%s'", typeInfo->name.c_str()));
}

bool BackendC::emitForeignCall(Concat& concat, Module* moduleToGen, ByteCodeInstruction* ip, vector<uint32_t>& pushParams)
{
    auto              nodeFunc   = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

    auto returnType = TypeManager::concreteType(typeFuncBC->returnType);
    if (returnType != g_TypeMgr.typeInfoVoid)
    {
        if ((returnType->kind == TypeInfoKind::Slice) || (returnType->flags & TYPEINFO_RETURN_BY_COPY))
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
                return moduleToGen->internalError(ip->sourceFileIdx, ip->startLocation, ip->endLocation, "emitForeignCall, invalid return type");
            }
        }
        else
        {
            return moduleToGen->internalError(ip->sourceFileIdx, ip->startLocation, ip->endLocation, "emitForeignCall, invalid return type");
        }
    }

    auto it = typeFuncBC->attributes.values.find("swag.foreign.function");
    if (it != typeFuncBC->attributes.values.end())
        concat.addString(it->second.second.text);
    else
        concat.addString(nodeFunc->name);
    concat.addChar('(');

    int numCallParams = (int) typeFuncBC->parameters.size();
    for (int idxCall = 0; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = TypeManager::concreteType(typeFuncBC->parameters[idxCall]->typeInfo);
        if (idxCall)
            concat.addChar(',');

        auto index = pushParams.back();
        pushParams.pop_back();

        // Access to the content of the register
        if (typeParam->kind == TypeInfoKind::Pointer || typeParam->kind == TypeInfoKind::Struct)
        {
            CONCAT_STR_INT_STR(concat, "(void*)r[", index, "].pointer");
        }
        else if (typeParam->isNative(NativeTypeKind::String))
        {
            CONCAT_STR_INT_STR(concat, "(void*)r[", index, "].pointer");
        }
        else if (typeParam->kind == TypeInfoKind::Slice)
        {
            CONCAT_STR_INT_STR(concat, "(void*)r[", index, "].pointer");
            index = pushParams.back();
            pushParams.pop_back();
            CONCAT_STR_INT_STR(concat, ", r[", index, "].u32");
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            CONCAT_STR_INT_STR(concat, "r[", index, "]");
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
                return moduleToGen->internalError(ip->sourceFileIdx, ip->startLocation, ip->endLocation, "emitForeignCall, invalid param native type");
            }
        }
        else
        {
            return moduleToGen->internalError(ip->sourceFileIdx, ip->startLocation, ip->endLocation, "emitForeignCall, invalid param type");
        }
    }

    // Return by parameter
    if (returnType->kind == TypeInfoKind::Slice)
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
    SWAG_CHECK(emitForeignFuncSignature(moduleToGen, concat, typeFunc, node, true));
    CONCAT_FIXED_STR(concat, " {\n");

    // Compute number of registers
    auto n = typeFunc->numReturnRegisters();
    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteType(param->typeInfo);
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

    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteType(param->typeInfo);
        if (typeParam->kind == TypeInfoKind::Pointer)
        {
            concat.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s;\n", idx, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Slice)
        {
            concat.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s;\n", idx, param->namedParam.c_str());
            concat.addStringFormat("\trr%d.u32 = %s_count;\n", idx + 1, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Interface)
        {
            concat.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s;\n", idx, param->namedParam.c_str());
            concat.addStringFormat("\trr%d.pointer = %s_itable;\n", idx + 1, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Struct)
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
            case NativeTypeKind::String:
                concat.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s; ", idx, param->namedParam.c_str());
                concat.addStringFormat("rr%d.u32 = %s ? __strlen(%s) : 0;\n", idx + 1, param->namedParam.c_str(), param->namedParam.c_str());
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
        if (returnType->kind == TypeInfoKind::Slice)
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
            CONCAT_FIXED_STR(concat, "\treturn rr0.pointer;\n");
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

bool BackendC::emitForeignFuncSignature(Module* moduleToGen, Concat& buffer, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, bool forExport)
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
        Utf8 cType;
        for (auto param : node->parameters->childs)
        {
            if (!first)
                buffer.addChar(',');
            first = false;

            SWAG_CHECK(swagTypeToCType(moduleToGen, param->typeInfo, cType));
            buffer.addString(cType);
            buffer.addChar(' ');
            buffer.addString(param->name);

            if (param->typeInfo->kind == TypeInfoKind::Slice)
            {
                CONCAT_FIXED_STR(buffer, ", swag_uint32_t ");
                buffer.addString(param->name);
                CONCAT_FIXED_STR(buffer, "_count");
            }
            else if (param->typeInfo->kind == TypeInfoKind::Interface)
            {
                CONCAT_FIXED_STR(buffer, ", void* ");
                buffer.addString(param->name);
                CONCAT_FIXED_STR(buffer, "_itable");
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
        auto typeParam = TypeManager::concreteType(param->typeInfo);
        for (int i = 0; i < typeParam->numRegisters(); i++)
        {
            if (index || i || typeFunc->numReturnRegisters())
                concat.addChar(',');
            concat.addStringFormat("swag_register_t* rp%u", index++);
        }
    }

    CONCAT_FIXED_STR(concat, ")");
}

bool BackendC::emitAllFuncSignatureInternalC(Module* moduleToGen)
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

        CONCAT_FIXED_STR(bufferC, "static ");
        emitFuncSignatureInternalC(bufferC, one);
        CONCAT_FIXED_STR(bufferC, ";\n");
    }

    bufferC.addEol();
    return true;
}

bool BackendC::emitAllFuncSignatureInternalC()
{
    emitSeparator(bufferC, "PROTOTYPES");
    if (!emitAllFuncSignatureInternalC(g_Workspace.bootstrapModule))
        return false;
    if (!emitAllFuncSignatureInternalC(module))
        return false;

    // Import functions
    for (auto node : module->allForeign)
    {
        if (node->flags & AST_USED_FOREIGN)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            CONCAT_FIXED_STR(bufferC, "SWAG_IMPORT ");
            SWAG_CHECK(emitForeignFuncSignature(module, bufferC, typeFunc, node, false));
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
    CONCAT_FIXED_STR(concat, "static ");
    emitFuncSignatureInternalC(concat, bc);
    CONCAT_FIXED_STR(concat, " {\n");

    // Generate one local variable per used register
    if (bc->maxReservedRegisterRC)
    {
        CONCAT_STR_INT_STR(concat, "swag_register_t r[", bc->maxReservedRegisterRC, "];\n");
    }

    // For function call results
    if (bc->maxCallResults)
    {
        CONCAT_STR_INT_STR(concat, "swag_register_t rt[", bc->maxCallResults, "];\n");
    }

    // Local stack
    if (typeFunc->stackSize)
    {
        CONCAT_STR_INT_STR(concat, "swag_uint8_t stack[", typeFunc->stackSize, "];\n");
    }

    // Generate bytecode
    int              vaargsIdx = 0;
    auto             ip        = bc->out;
    int              lastLine  = -1;
    vector<uint32_t> pushRAParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        // Print source code
        if (!bc->compilerGenerated)
        {
            if (moduleToGen->buildParameters.target.backendC.writeSourceCode || g_CommandLine.debug)
            {
                if (ip->startLocation.line != lastLine)
                {
                    if (ip->startLocation.column != ip->endLocation.column)
                    {
                        lastLine = ip->startLocation.line;
                        auto s   = bc->sourceFile->getLine(ip->startLocation.seekStartLine[REPORT_NUM_CODE_LINES - 1]);
                        s.erase(0, s.find_first_not_of("\t\n\v\f\r "));
                        if (!s.empty())
                            s.pop_back();
                        CONCAT_FIXED_STR(concat, "/* ");
                        concat.addString(s);
                        CONCAT_FIXED_STR(concat, " */\n");
                    }
                }
            }
        }

        CONCAT_FIXED_STR(concat, "lbl");
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
        case ByteCodeOp::MovSPBP:
            break;

        case ByteCodeOp::BoundCheckString:
            concat.addStringFormat("__assert(r[%u].u32 <= r[%u].u32 + 1, \"%s\", %d, \": error: index out of range\");", ip->a.u32, ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            break;
        case ByteCodeOp::BoundCheck:
            concat.addStringFormat("__assert(r[%u].u32 < r[%u].u32, \"%s\", %d, \": error: index out of range\");", ip->a.u32, ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            break;
        case ByteCodeOp::IncPointerVB:
            concat.addStringFormat("r[%u].pointer += %d;", ip->a.u32, ip->b.s32);
            break;
        case ByteCodeOp::IncPointer:
            concat.addStringFormat("r[%u].pointer = r[%u].pointer + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DecPointer:
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
            concat.addStringFormat("r[%u].s32 *= %u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::RAFromDataSeg8:
            concat.addStringFormat("r[%u].u8 = *(swag_uint8_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromDataSeg16:
            concat.addStringFormat("r[%u].u16 = *(swag_uint16_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromDataSeg32:
            concat.addStringFormat("r[%u].u32 = *(swag_uint32_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromDataSeg64:
            concat.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::Clear8:
            CONCAT_STR_INT_STR(concat, "*(swag_uint8_t*)r[", ip->a.u32, "].pointer = 0;");
            break;
        case ByteCodeOp::Clear16:
            CONCAT_STR_INT_STR(concat, "*(swag_uint16_t*)r[", ip->a.u32, "].pointer = 0;");
            break;
        case ByteCodeOp::Clear32:
            CONCAT_STR_INT_STR(concat, "*(swag_uint32_t*)r[", ip->a.u32, "].pointer = 0;");
            break;
        case ByteCodeOp::Clear64:
            CONCAT_STR_INT_STR(concat, "*(swag_uint64_t*)r[", ip->a.u32, "].pointer = 0;");
            break;
        case ByteCodeOp::ClearX:
            concat.addStringFormat("__memset(r[%u].pointer, 0, %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearXVar:
            concat.addStringFormat("__memset(r[%u].pointer, 0, r[%u].u32 * %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::ClearRefFromStack8:
            CONCAT_STR_INT_STR(concat, "*(swag_uint8_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::ClearRefFromStack16:
            CONCAT_STR_INT_STR(concat, "*(swag_uint16_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::ClearRefFromStack32:
            CONCAT_STR_INT_STR(concat, "*(swag_uint32_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::ClearRefFromStack64:
            CONCAT_STR_INT_STR(concat, "*(swag_uint64_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::ClearRefFromStackX:
            concat.addStringFormat("__memset(stack + %u, 0, %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RARefFromDataSeg:
            concat.addStringFormat("r[%u].pointer = __mutableseg + %u;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAAddrFromConstantSeg:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) (__constantseg + %u); ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RARefFromConstantSeg:
            concat.addStringFormat("r[%u].pointer = __constantseg + %u; ", ip->a.u32, (uint32_t)(ip->c.u64 >> 32));
            concat.addStringFormat("r[%u].u64 = %u;", ip->b.u32, (ip->c.u64) & 0xFFFFFFFF);
            break;
        case ByteCodeOp::RARefFromStack:
            concat.addStringFormat("r[%u].pointer = stack + %u;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromStack8:
            concat.addStringFormat("r[%u].u8 = *(swag_uint8_t*) (stack + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromStack16:
            concat.addStringFormat("r[%u].u16 = *(swag_uint16_t*) (stack + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromStack32:
            concat.addStringFormat("r[%u].u32 = *(swag_uint32_t*) (stack + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromStack64:
            concat.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (stack + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::MemCpy:
            concat.addStringFormat("__memcpy((void*) r[%u].pointer, (void*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::MemSet:
            concat.addStringFormat("__memset((void*) r[%u].pointer, r[%u].u8, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::MemCmp:
            concat.addStringFormat("r[%u].s32 = __memcmp((void*) r[%u].pointer, (void*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32);
            break;
        case ByteCodeOp::CopyVC:
            concat.addStringFormat("__memcpy(r[%u].pointer, r[%u].pointer, %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::CopyRAVB32:
            concat.addStringFormat("r[%u].u32 = 0x%x;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyRAVB64:
            concat.addStringFormat("r[%u].u64 = 0x%I64x;", ip->a.u32, ip->b.u64);
            break;
        case ByteCodeOp::CopyRARB:
            concat.addStringFormat("r[%u] = r[%u]; ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyRARBAddr:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) &r[%u]; ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearRA:
            CONCAT_STR_INT_STR(concat, "r[", ip->a.u32, "].u64 = 0;");
            break;
        case ByteCodeOp::DecRA:
            CONCAT_STR_INT_STR(concat, "r[", ip->a.u32, "].u32--;");
            break;
        case ByteCodeOp::IncRA:
            CONCAT_STR_INT_STR(concat, "r[", ip->a.u32, "].u32++;");
            break;
        case ByteCodeOp::IncRA64:
            CONCAT_STR_INT_STR(concat, "r[", ip->a.u32, "].u64++;");
            break;
        case ByteCodeOp::IncRAVB:
            concat.addStringFormat("r[%u].u32 += %u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOp8:
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) = r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOp16:
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) = r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOp32:
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer + %u) = r[%u].u32;", ip->a.u32, ip->c.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOp64:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer + %u) = r[%u].u64;", ip->a.u32, ip->c.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOp64Null:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer + %u) = 0;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPointer:
            concat.addStringFormat("*(void**)(r[%u].pointer) = r[%u].pointer;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpPlusS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 + r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 + r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusU32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusU64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 + r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
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
        case ByteCodeOp::BinOpMinusU32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 - r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusU64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 - r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
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
        case ByteCodeOp::BinOpMulU32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 * r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulU64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 * r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulF32:
            concat.addStringFormat("r[%u].f32 = r[%u].f32 * r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulF64:
            concat.addStringFormat("r[%u].f64= r[%u].f64 * r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::XorS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 ^ r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::XorS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 ^ r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::XorU32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 ^ r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::XorU64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 ^ r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::ShiftLeftS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 << r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftLeftS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 << r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftLeftU32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftLeftU64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 << r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::ShiftRightS32:
            concat.addStringFormat("r[%u].s32 = r[%u].s32 >> r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightS64:
            concat.addStringFormat("r[%u].s64 = r[%u].s64 >> r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightU32:
            concat.addStringFormat("r[%u].u32 = r[%u].u32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightU64:
            concat.addStringFormat("r[%u].u64 = r[%u].u64 >> r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightU64VB:
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
                concat.addStringFormat("__assert(r[%u].s32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("r[%u].s32 = r[%u].s32 / r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivS64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].s64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("r[%u].s64 = r[%u].s64 / r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivU32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].u32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("r[%u].u32 = r[%u].u32 / r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivU64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].u64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("r[%u].u64 = r[%u].u64 / r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivF32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].f32 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("r[%u].f32 = r[%u].f32 / r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivF64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].f64 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("r[%u].f64 = r[%u].f64 / r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) -= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) -= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) -= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) -= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqU8:
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) -= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqU16:
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) -= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqU32:
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) -= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqU64:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) -= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            concat.addStringFormat("*(swag_float32_t*)(r[%u].pointer) -= r[%u].f32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            concat.addStringFormat("*(swag_float64_t*)(r[%u].pointer) -= r[%u].f64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) += r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) += r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) += r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) += r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqU8:
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) += r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqU16:
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) += r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqU32:
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) += r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqU64:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) += r[%u].u64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            concat.addStringFormat("*(swag_float32_t*)(r[%u].pointer) += r[%u].f32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            concat.addStringFormat("*(swag_float64_t*)(r[%u].pointer) += r[%u].f64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqPointer:
            concat.addStringFormat("*(swag_uint8_t**)(r[%u].pointer) += r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqPointer:
            concat.addStringFormat("*(swag_uint8_t**)(r[%u].pointer) -= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) *= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) *= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) *= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) *= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqU8:
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) *= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqU16:
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) *= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqU32:
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) *= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqU64:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) *= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqF32:
            concat.addStringFormat("*(swag_float32_t*)(r[%u].pointer) *= r[%u].f32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqF64:
            concat.addStringFormat("*(swag_float64_t*)(r[%u].pointer) *= r[%u].f64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpDivEqS8:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].s8, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) /= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqS16:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].s16, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) /= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqS32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].s32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) /= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqS64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].s64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) /= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU8:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].u8, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) /= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU16:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].u16, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) /= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].u32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) /= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].u64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) /= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqF32:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].f32 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("*(swag_float32_t*)(r[%u].pointer) /= r[%u].f32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
                concat.addStringFormat("__assert(r[%u].f64 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            concat.addStringFormat("*(swag_float64_t*)(r[%u].pointer) /= r[%u].f64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpAndEqS8:
            CONCAT_STR_INT_STR_INT_STR(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpAndEqS16:
            CONCAT_STR_INT_STR_INT_STR(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpAndEqS32:
            CONCAT_STR_INT_STR_INT_STR(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpAndEqS64:
            CONCAT_STR_INT_STR_INT_STR(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpAndEqU8:
            CONCAT_STR_INT_STR_INT_STR(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpAndEqU16:
            CONCAT_STR_INT_STR_INT_STR(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpAndEqU32:
            CONCAT_STR_INT_STR_INT_STR(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpAndEqU64:
            CONCAT_STR_INT_STR_INT_STR(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u64;");
            break;

        case ByteCodeOp::AffectOpOrEqS8:
            concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) |= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqS16:
            concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) |= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqS32:
            concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) |= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqS64:
            concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) |= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqU8:
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) |= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqU16:
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) |= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqU32:
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) |= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqU64:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) |= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpShiftLeftEqS8:
            concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS16:
            concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS32:
            concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS64:
            concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU8:
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU8:
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpPercentEqS8:
            concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) %%= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqS16:
            concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) %%= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqS32:
            concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) %%= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqS64:
            concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) %%= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqU8:
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) %%= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqU16:
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) %%= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqU32:
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) %%= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqU64:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) %%= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpXOrEqS8:
            concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) ^= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqS16:
            concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) ^= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqS32:
            concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) ^= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqS64:
            concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) ^= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqU8:
            concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) ^= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqU16:
            concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) ^= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqU32:
            concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) ^= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqU64:
            concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) ^= r[%u].u64;", ip->a.u32, ip->b.u32);
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
            concat.addStringFormat("r[%u].b = __strcmp((const char*) r[%u].pointer, (const char*) r[%u].pointer, r[%u].u32);", ip->c.u32, ip->a.u32, ip->b.u32, ip->c.u32);
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
            CONCAT_FIXED_STR(concat, "goto lbl");
            concat.addS32Str8(ip->a.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpNotTrue:
            CONCAT_STR_INT_STR(concat, "if(!r[", ip->a.u32, "].b) goto lbl");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpTrue:
            CONCAT_STR_INT_STR(concat, "if(r[", ip->a.u32, "].b) goto lbl");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpZero32:
            CONCAT_STR_INT_STR(concat, "if(!r[", ip->a.u32, "].u32) goto lbl");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::JumpNotZero32:
            CONCAT_STR_INT_STR(concat, "if(r[", ip->a.u32, "].u32) goto lbl");
            concat.addS32Str8(ip->b.s32 + i + 1);
            concat.addChar(';');
            break;
        case ByteCodeOp::Ret:
            CONCAT_FIXED_STR(concat, "return;");
            break;

        case ByteCodeOp::IntrinsicPrintS64:
            CONCAT_STR_INT_STR(concat, "__print_i64(r[", ip->a.u32, "].s64);");
            break;
        case ByteCodeOp::IntrinsicPrintF64:
            CONCAT_STR_INT_STR(concat, "__print_f64(r[", ip->a.u32, "].f64);");
            break;
        case ByteCodeOp::IntrinsicPrintString:
            concat.addStringFormat("__print_n((const char*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::IntrinsicAssert:
            if (ip->c.pointer)
                concat.addStringFormat("__assert(r[%u].b, \"%s\", %d, \"%s\");", ip->a.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1, ip->c.pointer);
            else
                concat.addStringFormat("__assert(r[%u].b, \"%s\", %d, 0);", ip->a.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            break;
        case ByteCodeOp::IntrinsicAlloc:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) __malloc(r[%u].u32);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::IntrinsicRealloc:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) __realloc(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::IntrinsicFree:
            concat.addStringFormat("__free(r[%u].pointer);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicGetContext:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) __tlsGetValue(__process_infos.contextTlsId);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicSetContext:
            concat.addStringFormat("__tlsSetValue(__process_infos.contextTlsId, r[%u].pointer);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicArguments:
            concat.addStringFormat("r[%u].pointer = __process_infos.arguments.addr; ", ip->a.u32);
            concat.addStringFormat("r[%u].u64 = __process_infos.arguments.count; ", ip->b.u32);
            break;

        case ByteCodeOp::NegBool:
            concat.addStringFormat("r[%u].b = !r[%u].b;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::NegF32:
            concat.addStringFormat("r[%u].f32 = -r[%u].f32;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::NegF64:
            concat.addStringFormat("r[%u].f64 = -r[%u].f64;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::NegS32:
            concat.addStringFormat("r[%u].s32 = -r[%u].s32;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::NegS64:
            concat.addStringFormat("r[%u].s64 = -r[%u].s64;", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::InvertS8:
            concat.addStringFormat("r[%u].s8 = ~r[%u].s8;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::InvertS16:
            concat.addStringFormat("r[%u].s16 = ~r[%u].s16;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::InvertS32:
            concat.addStringFormat("r[%u].s32 = ~r[%u].s32;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::InvertS64:
            concat.addStringFormat("r[%u].s64 = ~r[%u].s64;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::InvertU8:
            concat.addStringFormat("r[%u].u8 = ~r[%u].u8;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::InvertU16:
            concat.addStringFormat("r[%u].u16 = ~r[%u].u16;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::InvertU32:
            concat.addStringFormat("r[%u].u32 = ~r[%u].u32;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::InvertU64:
            concat.addStringFormat("r[%u].u64 = ~r[%u].u64;", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::ClearMaskU32:
            concat.addStringFormat("r[%u].u32 &= 0x%x;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearMaskU64:
            concat.addStringFormat("r[%u].u64 &= 0xFFFFFFFF | ((swag_uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearMaskU32U64:
            concat.addStringFormat("r[%u].u32 &= 0x%x; ", ip->a.u32, ip->b.u32);
            concat.addStringFormat("r[%u].u64 &= 0xFFFFFFFF | ((swag_uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CastBool8:
            concat.addStringFormat("r[%u].b = r[%u].u8 ? 1 : 0; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastBool16:
            concat.addStringFormat("r[%u].b = r[%u].u16 ? 1 : 0; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastBool32:
            concat.addStringFormat("r[%u].b = r[%u].u32 ? 1 : 0; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastBool64:
            concat.addStringFormat("r[%u].b = r[%u].u64 ? 1 : 0; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS8S16:
            concat.addStringFormat("r[%u].s16 = (swag_int16_t) r[%u].s8; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS16S32:
            concat.addStringFormat("r[%u].s32 = (swag_int32_t) r[%u].s16; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS32S8:
            concat.addStringFormat("r[%u].s8 = (swag_int8_t) r[%u].s32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS32S16:
            concat.addStringFormat("r[%u].s16 = (swag_int16_t) r[%u].s32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS32S64:
            concat.addStringFormat("r[%u].s64 = (swag_int64_t) r[%u].s32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS32F32:
            concat.addStringFormat("r[%u].f32 = (swag_float32_t) r[%u].s32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS64S32:
            concat.addStringFormat("r[%u].s32 = (swag_int32_t) r[%u].s64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS64F32:
            concat.addStringFormat("r[%u].f32 = (swag_float32_t) r[%u].s64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS64F64:
            concat.addStringFormat("r[%u].f64 = (swag_float64_t) r[%u].s64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastU32F32:
            concat.addStringFormat("r[%u].f32 = (swag_float32_t) r[%u].u32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastU64F32:
            concat.addStringFormat("r[%u].f32 = (swag_float32_t) r[%u].u64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastU64F64:
            concat.addStringFormat("r[%u].f64 = (swag_float64_t) r[%u].u64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastF32S32:
            concat.addStringFormat("r[%u].s32 = (swag_int32_t) r[%u].f32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastF32S64:
            concat.addStringFormat("r[%u].s64 = (swag_int64_t) r[%u].f32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastF32F64:
            concat.addStringFormat("r[%u].f64 = (swag_float64_t) r[%u].f32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastF64S64:
            concat.addStringFormat("r[%u].s64 = (swag_int64_t) r[%u].f64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastF64F32:
            concat.addStringFormat("r[%u].f32 = (swag_float32_t) r[%u].f64; ", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::CopyRRxRCx:
            CONCAT_STR_INT_STR_INT_STR(concat, "*rr", ip->a.u32, " = r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRCxRRx:
            CONCAT_STR_INT_STR_INT_STR(concat, "r[", ip->a.u32, "] = *rr", ip->b.u32, ";");
            break;
        case ByteCodeOp::CopyRRxRCxCall:
            CONCAT_STR_INT_STR_INT_STR(concat, "rt[", ip->a.u32, "] = r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRCxRRxCall:
            CONCAT_STR_INT_STR_INT_STR(concat, "r[", ip->a.u32, "] = rt[", ip->b.u32, "];");
            break;
        case ByteCodeOp::RAFromStackParam64:
            CONCAT_STR_INT_STR_INT_STR(concat, "r[", ip->a.u32, "] = *rp", ip->c.u32, ";");
            break;
        case ByteCodeOp::RARefFromStackParam:
            CONCAT_STR_INT_STR_INT_STR(concat, "r[", ip->a.u32, "].pointer = (swag_uint8_t*) &rp", ip->c.u32, "->pointer;");
            break;
        case ByteCodeOp::PushRRSaved:
        case ByteCodeOp::PopRRSaved:
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
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) &%s;", ip->a.u32, funcBC->callName().c_str());
            break;
        }

        case ByteCodeOp::PushRAParam:
            pushRAParams.push_back(ip->a.u32);
            break;
        case ByteCodeOp::MovRASP:
            concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) &r[%u];", ip->a.u32, ip->c.u32);
            break;
        case ByteCodeOp::MovRASPVaargs:
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
        case ByteCodeOp::LocalCall:
        {
            auto              funcBC     = (ByteCode*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

            // Normal function call
            if (ip->op == ByteCodeOp::LocalCall)
            {
                concat.addString(funcBC->callName());
            }

            // Lambda call
            else
            {
                // Need to output the function prototype too
                CONCAT_FIXED_STR(concat, "((void(*)(");
                for (int j = 0; j < typeFuncBC->numReturnRegisters() + typeFuncBC->numParamsRegisters(); j++)
                {
                    if (j)
                        concat.addChar(',');
                    CONCAT_FIXED_STR(concat, "swag_register_t*");
                }

                CONCAT_FIXED_STR(concat, "))");

                // Then the call
                CONCAT_STR_INT_STR(concat, " r[", ip->a.u32, "].pointer)");
            }

            concat.addChar('(');
            for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
            {
                if (j)
                    concat.addChar(',');
                CONCAT_STR_INT_STR(concat, "&rt[", j, "]");
            }

            int numCallParams = (int) typeFuncBC->parameters.size();
            for (int idxCall = numCallParams - 1; idxCall >= 0; idxCall--)
            {
                auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
                typeParam      = TypeManager::concreteType(typeParam);
                for (int j = 0; j < typeParam->numRegisters(); j++)
                {
                    if ((idxCall != (int) numCallParams - 1) || j || typeFuncBC->numReturnRegisters())
                        concat.addChar(',');
                    auto index = pushRAParams.back();
                    pushRAParams.pop_back();
                    CONCAT_STR_INT_STR(concat, "&r[", index, "]");
                }
            }

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
            moduleToGen->internalError(format("unknown instruction '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
            break;
        }

        concat.addEol();
    }

    CONCAT_FIXED_STR(concat, "}\n");

    if (bc->node && bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC)
        CONCAT_FIXED_STR(concat, "#endif\n");

    return ok;
}

bool BackendC::emitAllFunctionBody(Job* ownerJob)
{
    if (!emitAllFunctionBody(g_Workspace.bootstrapModule, ownerJob))
        return false;
    if (!emitAllFunctionBody(module, ownerJob))
        return false;
    return true;
}

bool BackendC::emitAllFunctionBody(Module* moduleToGen, Job* ownerJob)
{
    SWAG_ASSERT(moduleToGen);

    bool                     ok         = true;
    int                      batchCount = 8;
    BackendCFunctionBodyJob* job        = nullptr;
    for (auto one : moduleToGen->byteCodeFunc)
    {
        if (one->node)
        {
            auto node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

            // Do we need to generate that function ?
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
                continue;
            if (node->attributeFlags & ATTRIBUTE_FOREIGN)
                continue;
            if (!node->content)
                continue;
        }

        if (!job)
        {
            job               = g_Pool_backendCFunctionBodyJob.alloc();
            job->module       = moduleToGen;
            job->dependentJob = ownerJob;
            job->backend      = this;
        }

        job->byteCodeFunc.push_back(one);
        if (job->byteCodeFunc.size() == batchCount)
        {
            ownerJob->jobsToAdd.push_back(job);
            job = nullptr;
        }
    }

    if (job)
        ownerJob->jobsToAdd.push_back(job);

    return ok;
}

bool BackendC::emitPublic(Module* moduleToGen, Scope* scope)
{
    if (!(scope->flags & SCOPE_FLAG_HAS_EXPORTS))
        return true;

    for (auto child : scope->childScopes)
        SWAG_CHECK(emitPublic(moduleToGen, child));

    return true;
}