#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "CompilerTarget.h"
#include "Scope.h"

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

    if (typeInfo->kind == TypeInfoKind::Struct)
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

    return module->internalError("swagTypeToCType, invalid type");
}

bool BackendC::emitForeignCall(ByteCodeInstruction* ip, vector<uint32_t>& pushParams)
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
            CONCAT_FIXED_STR(bufferC, "rt[0].pointer = (swag_uint8_t*) ");
        }
        else if (returnType->kind == TypeInfoKind::Native)
        {
            switch (returnType->nativeType)
            {
            case NativeTypeKind::S8:
                CONCAT_FIXED_STR(bufferC, "rt[0].s8 = (swag_int8_t) ");
                break;
            case NativeTypeKind::U8:
                CONCAT_FIXED_STR(bufferC, "rt[0].u8 = (swag_int8_t) ");
                break;
            case NativeTypeKind::S16:
                CONCAT_FIXED_STR(bufferC, "rt[0].s16 = (swag_int16_t) ");
                break;
            case NativeTypeKind::U16:
                CONCAT_FIXED_STR(bufferC, "rt[0].u16 = (swag_uint16_t) ");
                break;
            case NativeTypeKind::S32:
                CONCAT_FIXED_STR(bufferC, "rt[0].s32 = (swag_int32_t) ");
                break;
            case NativeTypeKind::U32:
                CONCAT_FIXED_STR(bufferC, "rt[0].u32 = (swag_uint32_t) ");
                break;
            case NativeTypeKind::S64:
                CONCAT_FIXED_STR(bufferC, "rt[0].s64 = (swag_int64_t) ");
                break;
            case NativeTypeKind::U64:
                CONCAT_FIXED_STR(bufferC, "rt[0].u64 = (swag_uint64_t) ");
                break;
            case NativeTypeKind::Bool:
                CONCAT_FIXED_STR(bufferC, "rt[0].b = (swag_bool_t) ");
                break;
            case NativeTypeKind::F32:
                CONCAT_FIXED_STR(bufferC, "rt[0].f32 = (float) ");
                break;
            case NativeTypeKind::F64:
                CONCAT_FIXED_STR(bufferC, "rt[0].f64 = (double) ");
                break;
            default:
                return module->internalError(ip->sourceFileIdx, ip->startLocation, ip->endLocation, "emitForeignCall, invalid return type");
            }
        }
        else
        {
            return module->internalError(ip->sourceFileIdx, ip->startLocation, ip->endLocation, "emitForeignCall, invalid return type");
        }
    }

    auto it = typeFuncBC->attributes.values.find("swag.foreign.function");
    if (it != typeFuncBC->attributes.values.end())
        bufferC.addString(it->second.second.text);
    else
        bufferC.addString(nodeFunc->name);
    bufferC.addChar('(');

    int numCallParams = (int) typeFuncBC->parameters.size();
    for (int idxCall = 0; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = TypeManager::concreteType(typeFuncBC->parameters[idxCall]->typeInfo);
        if (idxCall)
            bufferC.addChar(',');

        auto index = pushParams.back();
        pushParams.pop_back();

        // Access to the content of the register
        if (typeParam->kind == TypeInfoKind::Pointer || typeParam->kind == TypeInfoKind::Struct)
        {
            bufferC.addStringFormat("(void*) r[%u].pointer", index);
        }
        else if (typeParam->isNative(NativeTypeKind::String))
        {
            bufferC.addStringFormat("(void*) r[%u].pointer", index);
        }
        else if (typeParam->kind == TypeInfoKind::Slice)
        {
            bufferC.addStringFormat("(void*) r[%u].pointer", index);
            index = pushParams.back();
            pushParams.pop_back();
            bufferC.addStringFormat(", r[%u].u32", index);
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            bufferC.addStringFormat("r[%u]", index);
            switch (typeParam->nativeType)
            {
            case NativeTypeKind::Bool:
                CONCAT_FIXED_STR(bufferC, ".b");
                break;
            case NativeTypeKind::S8:
                CONCAT_FIXED_STR(bufferC, ".s8");
                break;
            case NativeTypeKind::U8:
                CONCAT_FIXED_STR(bufferC, ".u8");
                break;
            case NativeTypeKind::S16:
                CONCAT_FIXED_STR(bufferC, ".s16");
                break;
            case NativeTypeKind::U16:
                CONCAT_FIXED_STR(bufferC, ".u16");
                break;
            case NativeTypeKind::S32:
                CONCAT_FIXED_STR(bufferC, ".s32");
                break;
            case NativeTypeKind::U32:
                CONCAT_FIXED_STR(bufferC, ".u32");
                break;
            case NativeTypeKind::S64:
                CONCAT_FIXED_STR(bufferC, ".s64");
                break;
            case NativeTypeKind::U64:
                CONCAT_FIXED_STR(bufferC, ".u64");
                break;
            case NativeTypeKind::F32:
                CONCAT_FIXED_STR(bufferC, ".f32");
                break;
            case NativeTypeKind::F64:
                CONCAT_FIXED_STR(bufferC, ".f64");
                break;
            case NativeTypeKind::Char:
                CONCAT_FIXED_STR(bufferC, ".ch");
                break;
            default:
                return module->internalError(ip->sourceFileIdx, ip->startLocation, ip->endLocation, "emitForeignCall, invalid param native type");
            }
        }
        else
        {
            return module->internalError(ip->sourceFileIdx, ip->startLocation, ip->endLocation, "emitForeignCall, invalid param type");
        }
    }

    // Return by parameter
    if (returnType->kind == TypeInfoKind::Slice)
    {
        if (numCallParams)
            bufferC.addChar(',');
        CONCAT_FIXED_STR(bufferC, "&rt[0]");
    }
    else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
    {
        if (numCallParams)
            bufferC.addChar(',');
        CONCAT_FIXED_STR(bufferC, "rt[0].pointer");
    }

    CONCAT_FIXED_STR(bufferC, ");");

    return true;
}

bool BackendC::emitFuncWrapperPublic(Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one)
{
    CONCAT_FIXED_STR(bufferC, "SWAG_EXPORT ");
    SWAG_CHECK(emitForeignFuncSignature(moduleToGen, bufferC, typeFunc, node));
    CONCAT_FIXED_STR(bufferC, " {\n");

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
        CONCAT_FIXED_STR(bufferC, "\tswag_register_t ");
        for (int i = 0; i < n; i++)
        {
            if (i)
                bufferC.addChar(',');
            bufferC.addStringFormat("rr%d", i);
        }
        CONCAT_FIXED_STR(bufferC, ";\n");
    }

    // Affect registers
    int idx = typeFunc->numReturnRegisters();

    // Return by copy
    bool returnByCopy = typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY;
    if (returnByCopy)
    {
        CONCAT_FIXED_STR(bufferC, "\trr0.pointer = result;\n");
    }

    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteType(param->typeInfo);
        if (typeParam->kind == TypeInfoKind::Pointer)
        {
            bufferC.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s;\n", idx, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Slice)
        {
            bufferC.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s;\n", idx, param->namedParam.c_str());
            bufferC.addStringFormat("\trr%d.u32 = %s_count;\n", idx + 1, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Struct)
        {
            bufferC.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s;\n", idx, param->namedParam.c_str());
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            switch (typeParam->nativeType)
            {
            case NativeTypeKind::U8:
                bufferC.addStringFormat("\trr%d.u8 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::U16:
                bufferC.addStringFormat("\trr%d.u16 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::U32:
                bufferC.addStringFormat("\trr%d.u32 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::U64:
                bufferC.addStringFormat("\trr%d.u64 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S8:
                bufferC.addStringFormat("\trr%d.s8 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S16:
                bufferC.addStringFormat("\trr%d.s16 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S32:
                bufferC.addStringFormat("\trr%d.s32 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::S64:
                bufferC.addStringFormat("\trr%d.s64 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::F32:
                bufferC.addStringFormat("\trr%d.f32 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::F64:
                bufferC.addStringFormat("\trr%d.f64 = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::Bool:
                bufferC.addStringFormat("\trr%d.b = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::Char:
                bufferC.addStringFormat("\trr%d.ch = %s;\n", idx, param->namedParam.c_str());
                break;
            case NativeTypeKind::String:
                bufferC.addStringFormat("\trr%d.pointer = (swag_uint8_t*) %s; ", idx, param->namedParam.c_str());
                bufferC.addStringFormat("rr%d.u32 = %s ? strlen(%s) : 0;\n", idx + 1, param->namedParam.c_str(), param->namedParam.c_str());
                break;
            default:
                return module->internalError("emitFuncWrapperPublic, invalid param type");
            }
        }
        else
        {
            return module->internalError("emitFuncWrapperPublic, invalid param type");
        }

        idx += typeParam->numRegisters();
    }

    // Make the call
    bufferC.addStringFormat("\t%s", one->callName().c_str());
    CONCAT_FIXED_STR(bufferC, "(");
    for (int i = 0; i < n; i++)
    {
        if (i)
            bufferC.addChar(',');
        bufferC.addStringFormat("&rr%d", i);
    }

    CONCAT_FIXED_STR(bufferC, ");\n");

    // Return
    if (typeFunc->numReturnRegisters() && !returnByCopy)
    {
        auto returnType = TypeManager::concreteType(typeFunc->returnType, CONCRETE_ALIAS | CONCRETE_ENUM);
        if (returnType->kind == TypeInfoKind::Slice)
        {
            CONCAT_FIXED_STR(bufferC, "\t*((void **) result) = rr0.pointer;\n");
            CONCAT_FIXED_STR(bufferC, "\t*((void **) result + 1) = rr1.pointer;\n");
        }
        else if (returnType->kind == TypeInfoKind::Native)
        {
            switch (returnType->nativeType)
            {
            case NativeTypeKind::U8:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.u8;\n");
                break;
            case NativeTypeKind::U16:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.u16;\n");
                break;
            case NativeTypeKind::U32:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.u32;\n");
                break;
            case NativeTypeKind::U64:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.u64;\n");
                break;
            case NativeTypeKind::S8:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.s8;\n");
                break;
            case NativeTypeKind::S16:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.s16;\n");
                break;
            case NativeTypeKind::S32:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.s32;\n");
                break;
            case NativeTypeKind::S64:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.s64;\n");
                break;
            case NativeTypeKind::F32:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.f32;\n");
                break;
            case NativeTypeKind::F64:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.f64;\n");
                break;
            case NativeTypeKind::Char:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.ch;\n");
                break;
            case NativeTypeKind::Bool:
                CONCAT_FIXED_STR(bufferC, "\treturn rr0.b;\n");
                break;
            default:
                return module->internalError("emitFuncWrapperPublic, invalid return type");
            }
        }
        else
        {
            return module->internalError("emitFuncWrapperPublic, invalid return type");
        }
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");
    return true;
}

bool BackendC::emitForeignFuncSignature(Module* moduleToGen, Concat& buffer, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
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
    CONCAT_FIXED_STR(buffer, " ");
    node->computeFullNameForeign();
    buffer.addString(node->fullnameForeign.c_str());
    CONCAT_FIXED_STR(buffer, "(");

    bool first = true;
    if (node->parameters)
    {
        Utf8 cType;
        for (auto param : node->parameters->childs)
        {
            if (!first)
                CONCAT_FIXED_STR(buffer, ", ");
            first = false;

            SWAG_CHECK(swagTypeToCType(moduleToGen, param->typeInfo, cType));
            buffer.addString(cType);
            CONCAT_FIXED_STR(buffer, " ");
            buffer.addString(param->name.c_str());

            if (param->typeInfo->kind == TypeInfoKind::Slice)
            {
                CONCAT_FIXED_STR(buffer, ", swag_uint32_t ");
                buffer.addString(param->name.c_str());
                CONCAT_FIXED_STR(buffer, "_count");
            }
        }
    }

    // Return value
    if (typeFunc->numReturnRegisters() > 1 || returnByCopy)
    {
        if (!first)
            CONCAT_FIXED_STR(buffer, ", ");
        buffer.addString(returnType);
        CONCAT_FIXED_STR(buffer, " result");
    }

    CONCAT_FIXED_STR(buffer, ")");
    return true;
}

void BackendC::emitFuncSignatureInternalC(ByteCode* bc)
{
    auto typeFunc = bc->callType();
    auto name     = bc->callName();

    CONCAT_FIXED_STR(bufferC, "void ");
    bufferC.addString(name.c_str());
    CONCAT_FIXED_STR(bufferC, "(");

    // Result
    for (int i = 0; i < typeFunc->numReturnRegisters(); i++)
    {
        if (i)
            bufferC.addChar(',');
        bufferC.addStringFormat("swag_register_t* rr%d", i);
    }

    // Parameters
    int index = 0;
    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteType(param->typeInfo);
        for (int i = 0; i < typeParam->numRegisters(); i++)
        {
            if (index || i || typeFunc->numReturnRegisters())
                bufferC.addChar(',');
            bufferC.addStringFormat("swag_register_t* rp%u", index++);
        }
    }

    CONCAT_FIXED_STR(bufferC, ")");
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
        emitFuncSignatureInternalC(one);
        CONCAT_FIXED_STR(bufferC, ";\n");
    }

    bufferC.addEol();
    return true;
}

bool BackendC::emitAllFuncSignatureInternalC()
{
    emitSeparator(bufferC, "PROTOTYPES");
    if (!emitAllFuncSignatureInternalC(g_Workspace.runtimeModule))
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
            SWAG_CHECK(emitForeignFuncSignature(module, bufferC, typeFunc, node));
            CONCAT_FIXED_STR(bufferC, ";\n");
        }
    }

    return true;
}

bool BackendC::emitFunctionBody(Module* moduleToGen, ByteCode* bc)
{
    bufferC.addEol();

    bool ok       = true;
    auto typeFunc = bc->callType();

    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC))
        CONCAT_FIXED_STR(bufferC, "#ifdef SWAG_HAS_TEST\n");

    // Signature
    CONCAT_FIXED_STR(bufferC, "static ");
    emitFuncSignatureInternalC(bc);
    CONCAT_FIXED_STR(bufferC, " {\n");

    // Generate one local variable per used register
    if (bc->maxReservedRegisterRC)
        bufferC.addStringFormat("swag_register_t r[%u];\n", bc->maxReservedRegisterRC);

    // For function call results
    if (bc->maxCallResults)
        bufferC.addStringFormat("swag_register_t rt[%u];\n", bc->maxCallResults);

    // Local stack
    if (typeFunc->stackSize)
    {
        bufferC.addStringFormat("swag_uint8_t stack[%u];\n", typeFunc->stackSize);
    }

    // Generate bytecode
    int              vaargsIdx = 0;
    auto             ip        = bc->out;
    int              lastLine  = -1;
    vector<uint32_t> pushRAParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        // Print source code
        if (module->buildParameters.target.backendC.writeSourceCode && !bc->compilerGenerated)
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
                    CONCAT_FIXED_STR(bufferC, "/* ");
                    bufferC.addString(s);
                    CONCAT_FIXED_STR(bufferC, " */\n");
                }
            }
        }

        bufferC.addStringFormat("lbl%08u:; ", i);

        if (module->buildParameters.target.backendC.writeByteCodeInstruction)
        {
            CONCAT_FIXED_STR(bufferC, "/* ");
            for (int dec = g_ByteCodeOpNamesLen[(int) ip->op]; dec < ByteCode::ALIGN_RIGHT_OPCODE; dec++)
                CONCAT_FIXED_STR(bufferC, " ");
            bufferC.addString(g_ByteCodeOpNames[(int) ip->op]);
            CONCAT_FIXED_STR(bufferC, " */ ");
        }

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::DecSP:
        case ByteCodeOp::IncSP:
        case ByteCodeOp::MovSPBP:
            break;

        case ByteCodeOp::BoundCheckString:
            bufferC.addStringFormat("__assert(r[%u].u32 <= r[%u].u32 + 1, \"%s\", %d, \": error: index out of range\");", ip->a.u32, ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            break;
        case ByteCodeOp::BoundCheck:
            bufferC.addStringFormat("__assert(r[%u].u32 < r[%u].u32, \"%s\", %d, \": error: index out of range\");", ip->a.u32, ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            break;
        case ByteCodeOp::IncPointerVB:
            bufferC.addStringFormat("r[%u].pointer += %d;", ip->a.u32, ip->b.s32);
            break;
        case ByteCodeOp::IncPointer:
            bufferC.addStringFormat("r[%u].pointer = r[%u].pointer + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DecPointer:
            bufferC.addStringFormat("r[%u].pointer = r[%u].pointer - r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DeRef8:
            bufferC.addStringFormat("r[%u].u64 = *(swag_uint8_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef16:
            bufferC.addStringFormat("r[%u].u64 = *(swag_uint16_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef32:
            bufferC.addStringFormat("r[%u].u64 = *(swag_uint32_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef64:
            bufferC.addStringFormat("r[%u].u64 = *(swag_uint64_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRefPointer:
            bufferC.addStringFormat("r[%u].pointer = *(swag_uint8_t**) (r[%u].pointer + %u);", ip->b.u32, ip->a.u32, ip->c.u32);
            break;
        case ByteCodeOp::DeRefStringSlice:
            bufferC.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (r[%u].pointer + 8); ", ip->b.u32, ip->a.u32);
            bufferC.addStringFormat("r[%u].pointer = *(swag_uint8_t**) r[%u].pointer; ", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::MulRAVB:
            bufferC.addStringFormat("r[%u].s32 *= %u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::RAFromDataSeg8:
            bufferC.addStringFormat("r[%u].u8 = *(swag_uint8_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromDataSeg16:
            bufferC.addStringFormat("r[%u].u16 = *(swag_uint16_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromDataSeg32:
            bufferC.addStringFormat("r[%u].u32 = *(swag_uint32_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromDataSeg64:
            bufferC.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::Clear8:
            bufferC.addStringFormat("*(swag_uint8_t*)r[%u].pointer = 0;", ip->a.u32);
            break;
        case ByteCodeOp::Clear16:
            bufferC.addStringFormat("*(swag_uint16_t*)r[%u].pointer = 0;", ip->a.u32);
            break;
        case ByteCodeOp::Clear32:
            bufferC.addStringFormat("*(swag_uint32_t*)r[%u].pointer = 0;", ip->a.u32);
            break;
        case ByteCodeOp::Clear64:
            bufferC.addStringFormat("*(swag_uint64_t*)r[%u].pointer = 0;", ip->a.u32);
            break;
        case ByteCodeOp::ClearX:
            bufferC.addStringFormat("memset(r[%u].pointer, 0, %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearXVar:
            bufferC.addStringFormat("memset(r[%u].pointer, 0, r[%u].u32 * %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::ClearRefFromStack8:
            bufferC.addStringFormat("*(swag_uint8_t*)(stack + %u) = 0;", ip->a.u32);
            break;
        case ByteCodeOp::ClearRefFromStack16:
            bufferC.addStringFormat("*(swag_uint16_t*)(stack + %u) = 0;", ip->a.u32);
            break;
        case ByteCodeOp::ClearRefFromStack32:
            bufferC.addStringFormat("*(swag_uint32_t*)(stack + %u) = 0;", ip->a.u32);
            break;
        case ByteCodeOp::ClearRefFromStack64:
            bufferC.addStringFormat("*(swag_uint64_t*)(stack + %u) = 0;", ip->a.u32);
            break;
        case ByteCodeOp::ClearRefFromStackX:
            bufferC.addStringFormat("memset(stack + %u, 0, %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RARefFromDataSeg:
            bufferC.addStringFormat("r[%u].pointer = __mutableseg + %u;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAAddrFromConstantSeg:
            bufferC.addStringFormat("r[%u].pointer = (swag_uint8_t*) (__constantseg + %u); ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RARefFromConstantSeg:
            bufferC.addStringFormat("r[%u].pointer = __constantseg + %u; ", ip->a.u32, (uint32_t)(ip->c.u64 >> 32));
            bufferC.addStringFormat("r[%u].u64 = %u;", ip->b.u32, (ip->c.u64) & 0xFFFFFFFF);
            break;
        case ByteCodeOp::RARefFromStack:
            bufferC.addStringFormat("r[%u].pointer = stack + %u;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromStack8:
            bufferC.addStringFormat("r[%u].u8 = *(swag_uint8_t*) (stack + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromStack16:
            bufferC.addStringFormat("r[%u].u16 = *(swag_uint16_t*) (stack + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromStack32:
            bufferC.addStringFormat("r[%u].u32 = *(swag_uint32_t*) (stack + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromStack64:
            bufferC.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (stack + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::Copy:
            bufferC.addStringFormat("memcpy(r[%u].pointer, r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::CopyVC:
            bufferC.addStringFormat("memcpy(r[%u].pointer, r[%u].pointer, %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::CopyRAVB32:
            bufferC.addStringFormat("r[%u].u32 = 0x%x;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyRAVB64:
            bufferC.addStringFormat("r[%u].u64 = 0x%I64x;", ip->a.u32, ip->b.u64);
            break;
        case ByteCodeOp::CopyRARBStr:
            bufferC.addStringFormat("r[%u].pointer = __string%u; ", ip->a.u32, ip->c.u32);
            bufferC.addStringFormat("r[%u].u32 = %u;", ip->b.u32, moduleToGen->strBuffer[ip->c.u32].size());
            break;
        case ByteCodeOp::CopyRARB:
            bufferC.addStringFormat("r[%u] = r[%u]; ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyRARBAddr:
            bufferC.addStringFormat("r[%u].pointer = (swag_uint8_t*) &r[%u]; ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearRA:
            bufferC.addStringFormat("r[%u].u64 = 0;", ip->a.u32);
            break;
        case ByteCodeOp::DecRA:
            bufferC.addStringFormat("r[%u].u32--;", ip->a.u32);
            break;
        case ByteCodeOp::IncRA:
            bufferC.addStringFormat("r[%u].u32++;", ip->a.u32);
            break;
        case ByteCodeOp::IncRA64:
            bufferC.addStringFormat("r[%u].u64++;", ip->a.u32);
            break;
        case ByteCodeOp::IncRAVB:
            bufferC.addStringFormat("r[%u].u32 += %u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOp8:
            bufferC.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) = r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOp16:
            bufferC.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) = r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOp32:
            bufferC.addStringFormat("*(swag_uint32_t*)(r[%u].pointer + %u) = r[%u].u32;", ip->a.u32, ip->c.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOp64:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer + %u) = r[%u].u64;", ip->a.u32, ip->c.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOp64Null:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer + %u) = 0;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPointer:
            bufferC.addStringFormat("*(void**)(r[%u].pointer) = r[%u].pointer;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpPlusS32:
            bufferC.addStringFormat("r[%u].s32 = r[%u].s32 + r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusS64:
            bufferC.addStringFormat("r[%u].s64 = r[%u].s64 + r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusU32:
            bufferC.addStringFormat("r[%u].u32 = r[%u].u32 + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusU64:
            bufferC.addStringFormat("r[%u].u64 = r[%u].u64 + r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusF32:
            bufferC.addStringFormat("r[%u].f32 = r[%u].f32 + r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusF64:
            bufferC.addStringFormat("r[%u].f64= r[%u].f64 + r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpMinusS32:
            bufferC.addStringFormat("r[%u].s32 = r[%u].s32 - r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusS64:
            bufferC.addStringFormat("r[%u].s64 = r[%u].s64 - r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusU32:
            bufferC.addStringFormat("r[%u].u32 = r[%u].u32 - r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusU64:
            bufferC.addStringFormat("r[%u].u64 = r[%u].u64 - r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusF32:
            bufferC.addStringFormat("r[%u].f32 = r[%u].f32 - r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusF64:
            bufferC.addStringFormat("r[%u].f64 = r[%u].f64 - r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpMulS32:
            bufferC.addStringFormat("r[%u].s32 = r[%u].s32 * r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulS64:
            bufferC.addStringFormat("r[%u].s64 = r[%u].s64 * r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulU32:
            bufferC.addStringFormat("r[%u].u32 = r[%u].u32 * r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulU64:
            bufferC.addStringFormat("r[%u].u64 = r[%u].u64 * r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulF32:
            bufferC.addStringFormat("r[%u].f32 = r[%u].f32 * r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMulF64:
            bufferC.addStringFormat("r[%u].f64= r[%u].f64 * r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::XorS32:
            bufferC.addStringFormat("r[%u].s32 = r[%u].s32 ^ r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::XorS64:
            bufferC.addStringFormat("r[%u].s64 = r[%u].s64 ^ r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::XorU32:
            bufferC.addStringFormat("r[%u].u32 = r[%u].u32 ^ r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::XorU64:
            bufferC.addStringFormat("r[%u].u64 = r[%u].u64 ^ r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::ShiftLeftS32:
            bufferC.addStringFormat("r[%u].s32 = r[%u].s32 << r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftLeftS64:
            bufferC.addStringFormat("r[%u].s64 = r[%u].s64 << r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftLeftU32:
            bufferC.addStringFormat("r[%u].u32 = r[%u].u32 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftLeftU64:
            bufferC.addStringFormat("r[%u].u64 = r[%u].u64 << r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::ShiftRightS32:
            bufferC.addStringFormat("r[%u].s32 = r[%u].s32 >> r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightS64:
            bufferC.addStringFormat("r[%u].s64 = r[%u].s64 >> r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightU32:
            bufferC.addStringFormat("r[%u].u32 = r[%u].u32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightU64:
            bufferC.addStringFormat("r[%u].u64 = r[%u].u64 >> r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightU64VB:
            bufferC.addStringFormat("r[%u].u64 >>= %u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpModuloS32:
            bufferC.addStringFormat("r[%u].s32 = r[%u].s32 %% r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloS64:
            bufferC.addStringFormat("r[%u].s64 = r[%u].s64 %% r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloU32:
            bufferC.addStringFormat("r[%u].u32 = r[%u].u32 %% r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloU64:
            bufferC.addStringFormat("r[%u].u64 = r[%u].u64 %% r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpDivF32:
            bufferC.addStringFormat("r[%u].f32 = r[%u].f32 / r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivF64:
            bufferC.addStringFormat("r[%u].f64 = r[%u].f64 / r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            bufferC.addStringFormat("*(swag_int8_t*)(r[%u].pointer) -= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            bufferC.addStringFormat("*(swag_int16_t*)(r[%u].pointer) -= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            bufferC.addStringFormat("*(swag_int32_t*)(r[%u].pointer) -= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            bufferC.addStringFormat("*(swag_int64_t*)(r[%u].pointer) -= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqU8:
            bufferC.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) -= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqU16:
            bufferC.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) -= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqU32:
            bufferC.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) -= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqU64:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) -= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            bufferC.addStringFormat("*(swag_float32_t*)(r[%u].pointer) -= r[%u].f32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            bufferC.addStringFormat("*(swag_float64_t*)(r[%u].pointer) -= r[%u].f64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            bufferC.addStringFormat("*(swag_int8_t*)(r[%u].pointer) += r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            bufferC.addStringFormat("*(swag_int16_t*)(r[%u].pointer) += r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            bufferC.addStringFormat("*(swag_int32_t*)(r[%u].pointer) += r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            bufferC.addStringFormat("*(swag_int64_t*)(r[%u].pointer) += r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqU8:
            bufferC.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) += r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqU16:
            bufferC.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) += r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqU32:
            bufferC.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) += r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqU64:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) += r[%u].u64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            bufferC.addStringFormat("*(swag_float32_t*)(r[%u].pointer) += r[%u].f32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            bufferC.addStringFormat("*(swag_float64_t*)(r[%u].pointer) += r[%u].f64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPlusEqPointer:
            bufferC.addStringFormat("*(swag_uint8_t**)(r[%u].pointer) += r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMinusEqPointer:
            bufferC.addStringFormat("*(swag_uint8_t**)(r[%u].pointer) -= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            bufferC.addStringFormat("*(swag_int8_t*)(r[%u].pointer) *= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            bufferC.addStringFormat("*(swag_int16_t*)(r[%u].pointer) *= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            bufferC.addStringFormat("*(swag_int32_t*)(r[%u].pointer) *= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            bufferC.addStringFormat("*(swag_int64_t*)(r[%u].pointer) *= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqU8:
            bufferC.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) *= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqU16:
            bufferC.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) *= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqU32:
            bufferC.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) *= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqU64:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) *= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqF32:
            bufferC.addStringFormat("*(swag_float32_t*)(r[%u].pointer) *= r[%u].f32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpMulEqF64:
            bufferC.addStringFormat("*(swag_float64_t*)(r[%u].pointer) *= r[%u].f64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpDivEqF32:
            bufferC.addStringFormat("*(swag_float32_t*)(r[%u].pointer) /= r[%u].f32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            bufferC.addStringFormat("*(swag_float64_t*)(r[%u].pointer) /= r[%u].f64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpAndEqS8:
            bufferC.addStringFormat("*(swag_int8_t*)(r[%u].pointer) &= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpAndEqS16:
            bufferC.addStringFormat("*(swag_int16_t*)(r[%u].pointer) &= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpAndEqS32:
            bufferC.addStringFormat("*(swag_int32_t*)(r[%u].pointer) &= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpAndEqS64:
            bufferC.addStringFormat("*(swag_int64_t*)(r[%u].pointer) &= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpAndEqU8:
            bufferC.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) &= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpAndEqU16:
            bufferC.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) &= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpAndEqU32:
            bufferC.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) &= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpAndEqU64:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) &= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpOrEqS8:
            bufferC.addStringFormat("*(swag_int8_t*)(r[%u].pointer) |= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqS16:
            bufferC.addStringFormat("*(swag_int16_t*)(r[%u].pointer) |= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqS32:
            bufferC.addStringFormat("*(swag_int32_t*)(r[%u].pointer) |= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqS64:
            bufferC.addStringFormat("*(swag_int64_t*)(r[%u].pointer) |= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqU8:
            bufferC.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) |= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqU16:
            bufferC.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) |= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqU32:
            bufferC.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) |= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpOrEqU64:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) |= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpShiftLeftEqS8:
            bufferC.addStringFormat("*(swag_int8_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS16:
            bufferC.addStringFormat("*(swag_int16_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS32:
            bufferC.addStringFormat("*(swag_int32_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS64:
            bufferC.addStringFormat("*(swag_int64_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU8:
            bufferC.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            bufferC.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            bufferC.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            bufferC.addStringFormat("*(swag_int8_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            bufferC.addStringFormat("*(swag_int16_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            bufferC.addStringFormat("*(swag_int32_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            bufferC.addStringFormat("*(swag_int64_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU8:
            bufferC.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            bufferC.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            bufferC.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpPercentEqS8:
            bufferC.addStringFormat("*(swag_int8_t*)(r[%u].pointer) %%= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqS16:
            bufferC.addStringFormat("*(swag_int16_t*)(r[%u].pointer) %%= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqS32:
            bufferC.addStringFormat("*(swag_int32_t*)(r[%u].pointer) %%= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqS64:
            bufferC.addStringFormat("*(swag_int64_t*)(r[%u].pointer) %%= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqU8:
            bufferC.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) %%= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqU16:
            bufferC.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) %%= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqU32:
            bufferC.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) %%= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpPercentEqU64:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) %%= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpXOrEqS8:
            bufferC.addStringFormat("*(swag_int8_t*)(r[%u].pointer) ^= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqS16:
            bufferC.addStringFormat("*(swag_int16_t*)(r[%u].pointer) ^= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqS32:
            bufferC.addStringFormat("*(swag_int32_t*)(r[%u].pointer) ^= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqS64:
            bufferC.addStringFormat("*(swag_int64_t*)(r[%u].pointer) ^= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqU8:
            bufferC.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) ^= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqU16:
            bufferC.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) ^= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqU32:
            bufferC.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) ^= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpXOrEqU64:
            bufferC.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) ^= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CompareOpGreaterS32:
            bufferC.addStringFormat("r[%u].b = r[%u].s32 > r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            bufferC.addStringFormat("r[%u].b = r[%u].s64 > r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterU32:
            bufferC.addStringFormat("r[%u].b = r[%u].u32 > r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            bufferC.addStringFormat("r[%u].b = r[%u].u64 > r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            bufferC.addStringFormat("r[%u].b = r[%u].f32 > r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            bufferC.addStringFormat("r[%u].b = r[%u].f64 > r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterPointer:
            bufferC.addStringFormat("r[%u].b = r[%u].pointer > r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CompareOpLowerS32:
            bufferC.addStringFormat("r[%u].b = r[%u].s32 < r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerS64:
            bufferC.addStringFormat("r[%u].b = r[%u].s64 < r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerU32:
            bufferC.addStringFormat("r[%u].b = r[%u].u32 < r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerU64:
            bufferC.addStringFormat("r[%u].b = r[%u].u64 < r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerF32:
            bufferC.addStringFormat("r[%u].b = r[%u].f32 < r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerF64:
            bufferC.addStringFormat("r[%u].b = r[%u].f64 < r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerPointer:
            bufferC.addStringFormat("r[%u].b = r[%u].pointer < r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CompareOpEqual8:
            bufferC.addStringFormat("r[%u].b = r[%u].u8 == r[%u].u8;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual16:
            bufferC.addStringFormat("r[%u].b = r[%u].u16 == r[%u].u16;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual32:
            bufferC.addStringFormat("r[%u].b = r[%u].u32 == r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual64:
            bufferC.addStringFormat("r[%u].b = r[%u].u64 == r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualPointer:
            bufferC.addStringFormat("r[%u].b = r[%u].pointer == r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualInterface:
            bufferC.addStringFormat("r[%u].b = (((void **) r[%u].pointer)[0] == ((void **) r[%u].pointer)[0]) && (((void **) r[%u].pointer)[1] == ((void **) r[%u].pointer)[1]);", ip->c.u32, ip->a.u32, ip->b.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualBool:
            bufferC.addStringFormat("r[%u].b = r[%u].b == r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualString:
            bufferC.addStringFormat("r[%u].b = __strcmp((const char*) r[%u].pointer, (const char*) r[%u].pointer, r[%u].u32);", ip->c.u32, ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::IsNullString:
            bufferC.addStringFormat("r[%u].b = r[%u].pointer == 0;", ip->b.u32, ip->a.u32);
            break;

        case ByteCodeOp::BinOpAnd:
            bufferC.addStringFormat("r[%u].b = r[%u].b && r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpOr:
            bufferC.addStringFormat("r[%u].b = r[%u].b || r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BitmaskAndS32:
            bufferC.addStringFormat("r[%u].s32 = r[%u].s32 & r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskAndS64:
            bufferC.addStringFormat("r[%u].s64 = r[%u].s64 & r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskAndU32:
            bufferC.addStringFormat("r[%u].u32 = r[%u].u32 & r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskAndU64:
            bufferC.addStringFormat("r[%u].u64 = r[%u].u64 & r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrS32:
            bufferC.addStringFormat("r[%u].s32 = r[%u].s32 | r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrS64:
            bufferC.addStringFormat("r[%u].s64 = r[%u].s64 | r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrU32:
            bufferC.addStringFormat("r[%u].u32 = r[%u].u32 | r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrU64:
            bufferC.addStringFormat("r[%u].u64 = r[%u].u64 | r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::Jump:
            bufferC.addStringFormat("goto lbl%08u;", ip->a.s32 + i + 1);
            break;
        case ByteCodeOp::JumpNotTrue:
            bufferC.addStringFormat("if(!r[%u].b) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1);
            break;
        case ByteCodeOp::JumpTrue:
            bufferC.addStringFormat("if(r[%u].b) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1);
            break;
        case ByteCodeOp::JumpZero32:
            bufferC.addStringFormat("if(!r[%u].u32) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1);
            break;
        case ByteCodeOp::JumpNotZero32:
            bufferC.addStringFormat("if(r[%u].u32) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1);
            break;
        case ByteCodeOp::Ret:
            CONCAT_FIXED_STR(bufferC, "return;");
            break;

        case ByteCodeOp::IntrinsicPrintS64:
            bufferC.addStringFormat("__print_i64(r[%u].s64);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicPrintF64:
            bufferC.addStringFormat("__print_f64(r[%u].f64);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicPrintString:
            bufferC.addStringFormat("__print_n((const char*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::IntrinsicAssert:
            if (ip->c.pointer)
                bufferC.addStringFormat("__assert(r[%u].b, \"%s\", %d, \"%s\");", ip->a.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1, ip->c.pointer);
            else
                bufferC.addStringFormat("__assert(r[%u].b, \"%s\", %d, 0);", ip->a.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1);
            break;
        case ByteCodeOp::IntrinsicAlloc:
            bufferC.addStringFormat("r[%u].pointer = (swag_uint8_t*) malloc(r[%u].u32);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::IntrinsicRealloc:
            bufferC.addStringFormat("r[%u].pointer = (swag_uint8_t*) realloc(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::IntrinsicFree:
            bufferC.addStringFormat("free(r[%u].pointer);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicGetContext:
            bufferC.addStringFormat("r[%u].pointer = (swag_uint8_t*) __tlsGetValue(__process_infos.contextTlsId);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicSetContext:
            bufferC.addStringFormat("__tlsSetValue(__process_infos.contextTlsId, r[%u].pointer);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicArguments:
            bufferC.addStringFormat("r[%u].pointer = __process_infos.arguments.addr; ", ip->a.u32);
            bufferC.addStringFormat("r[%u].u64 = __process_infos.arguments.count; ", ip->b.u32);
            break;

        case ByteCodeOp::NegBool:
            bufferC.addStringFormat("r[%u].b = !r[%u].b;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::NegF32:
            bufferC.addStringFormat("r[%u].f32 = -r[%u].f32;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::NegF64:
            bufferC.addStringFormat("r[%u].f64 = -r[%u].f64;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::NegS32:
            bufferC.addStringFormat("r[%u].s32 = -r[%u].s32;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::NegS64:
            bufferC.addStringFormat("r[%u].s64 = -r[%u].s64;", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::InvertS32:
            bufferC.addStringFormat("r[%u].s32 = ~r[%u].s32;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::InvertS64:
            bufferC.addStringFormat("r[%u].s64 = ~r[%u].s64;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::InvertU32:
            bufferC.addStringFormat("r[%u].u32 = ~r[%u].u32;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::InvertU64:
            bufferC.addStringFormat("r[%u].u64 = ~r[%u].u64;", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::ClearMaskU32:
            bufferC.addStringFormat("r[%u].u32 &= 0x%x;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearMaskU64:
            bufferC.addStringFormat("r[%u].u64 &= 0xFFFFFFFF | ((swag_uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearMaskU32U64:
            bufferC.addStringFormat("r[%u].u32 &= 0x%x; ", ip->a.u32, ip->b.u32);
            bufferC.addStringFormat("r[%u].u64 &= 0xFFFFFFFF | ((swag_uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CastBool8:
            bufferC.addStringFormat("r[%u].b = r[%u].u8 ? 1 : 0; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastBool16:
            bufferC.addStringFormat("r[%u].b = r[%u].u16 ? 1 : 0; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastBool32:
            bufferC.addStringFormat("r[%u].b = r[%u].u32 ? 1 : 0; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastBool64:
            bufferC.addStringFormat("r[%u].b = r[%u].u64 ? 1 : 0; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS8S16:
            bufferC.addStringFormat("r[%u].s16 = (swag_int16_t) r[%u].s8; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS16S32:
            bufferC.addStringFormat("r[%u].s32 = (swag_int32_t) r[%u].s16; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS32S8:
            bufferC.addStringFormat("r[%u].s8 = (swag_int8_t) r[%u].s32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS32S16:
            bufferC.addStringFormat("r[%u].s16 = (swag_int16_t) r[%u].s32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS32S64:
            bufferC.addStringFormat("r[%u].s64 = (swag_int64_t) r[%u].s32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS32F32:
            bufferC.addStringFormat("r[%u].f32 = (swag_float32_t) r[%u].s32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS64S32:
            bufferC.addStringFormat("r[%u].s32 = (swag_int32_t) r[%u].s64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS64F32:
            bufferC.addStringFormat("r[%u].f32 = (swag_float32_t) r[%u].s64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastS64F64:
            bufferC.addStringFormat("r[%u].f64 = (swag_float64_t) r[%u].s64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastU32F32:
            bufferC.addStringFormat("r[%u].f32 = (swag_float32_t) r[%u].u32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastU64F32:
            bufferC.addStringFormat("r[%u].f32 = (swag_float32_t) r[%u].u64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastU64F64:
            bufferC.addStringFormat("r[%u].f64 = (swag_float64_t) r[%u].u64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastF32S32:
            bufferC.addStringFormat("r[%u].s32 = (swag_int32_t) r[%u].f32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastF32S64:
            bufferC.addStringFormat("r[%u].s64 = (swag_int64_t) r[%u].f32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastF32F64:
            bufferC.addStringFormat("r[%u].f64 = (swag_float64_t) r[%u].f32; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastF64S64:
            bufferC.addStringFormat("r[%u].s64 = (swag_int64_t) r[%u].f64; ", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::CastF64F32:
            bufferC.addStringFormat("r[%u].f32 = (swag_float32_t) r[%u].f64; ", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::CopyRRxRCx:
            bufferC.addStringFormat("*rr%u = r[%u];", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyRCxRRx:
            bufferC.addStringFormat("r[%u] = *rr%u;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyRRxRCxCall:
            bufferC.addStringFormat("rt[%u] = r[%u];", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyRCxRRxCall:
            bufferC.addStringFormat("r[%u] = rt[%u];", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::RAFromStackParam64:
            bufferC.addStringFormat("r[%u] = *rp%u;", ip->a.u32, ip->c.u32);
            break;
        case ByteCodeOp::RARefFromStackParam:
            bufferC.addStringFormat("r[%u].pointer = (swag_uint8_t*) &rp%u->pointer;", ip->a.u32, ip->c.u32);
            break;
        case ByteCodeOp::PushRRSaved:
        case ByteCodeOp::PopRRSaved:
            break;

        case ByteCodeOp::MinusToTrue:
            bufferC.addStringFormat("r[%u].b = r[%u].s32 < 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::MinusZeroToTrue:
            bufferC.addStringFormat("r[%u].b = r[%u].s32 <= 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::PlusToTrue:
            bufferC.addStringFormat("r[%u].b = r[%u].s32 > 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::PlusZeroToTrue:
            bufferC.addStringFormat("r[%u].b = r[%u].s32 >= 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::MakeLambda:
        {
            auto funcBC = (ByteCode*) ip->b.pointer;
            bufferC.addStringFormat("r[%u].pointer = (swag_uint8_t*) &%s;", ip->a.u32, funcBC->callName().c_str());
            break;
        }

        case ByteCodeOp::PushRAParam:
            pushRAParams.push_back(ip->a.u32);
            break;
        case ByteCodeOp::MovRASP:
            bufferC.addStringFormat("r[%u].pointer = (swag_uint8_t*) &r[%u];", ip->a.u32, ip->c.u32);
            break;
        case ByteCodeOp::MovRASPVaargs:
        {
            bufferC.addStringFormat("swag_register_t vaargs%u[] = { 0, ", vaargsIdx);
            int idxParam = (int) pushRAParams.size() - 1;
            while (idxParam >= 0)
            {
                bufferC.addStringFormat("r[%u], ", pushRAParams[idxParam]);
                idxParam--;
            }

            CONCAT_FIXED_STR(bufferC, "}; ");
            bufferC.addStringFormat("r[%u].pointer = sizeof(swag_register_t) + (swag_uint8_t*) &vaargs%u; ", ip->a.u32, vaargsIdx);
            bufferC.addStringFormat("vaargs%u[0].pointer = r[%u].pointer;", vaargsIdx, ip->a.u32);
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
                bufferC.addStringFormat("{ %s", funcBC->callName().c_str());
            }

            // Lambda call
            else
            {
                // Need to output the function prototype too
                CONCAT_FIXED_STR(bufferC, "{ typedef void(*tfn)(");
                for (int j = 0; j < typeFuncBC->numReturnRegisters() + typeFuncBC->numParamsRegisters(); j++)
                {
                    if (j)
                        bufferC.addChar(',');
                    CONCAT_FIXED_STR(bufferC, "swag_register_t*");
                }

                CONCAT_FIXED_STR(bufferC, "); ");

                // Then the call
                bufferC.addStringFormat("((tfn)r[%u].pointer)", ip->a.u32);
            }

            CONCAT_FIXED_STR(bufferC, "(");
            for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
            {
                if (j)
                    bufferC.addChar(',');
                bufferC.addStringFormat("&rt[%u]", j);
            }

            int numCallParams = (int) typeFuncBC->parameters.size();
            for (int idxCall = numCallParams - 1; idxCall >= 0; idxCall--)
            {
                auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
                typeParam      = TypeManager::concreteType(typeParam);
                for (int j = 0; j < typeParam->numRegisters(); j++)
                {
                    if ((idxCall != (int) numCallParams - 1) || j || typeFuncBC->numReturnRegisters())
                        bufferC.addChar(',');
                    auto index = pushRAParams.back();
                    pushRAParams.pop_back();
                    bufferC.addStringFormat("&r[%u]", index);
                }
            }

            pushRAParams.clear();
            CONCAT_FIXED_STR(bufferC, "); }");
        }
        break;

        case ByteCodeOp::ForeignCall:
            SWAG_CHECK(emitForeignCall(ip, pushRAParams));
            break;

        default:
            ok = false;
            CONCAT_FIXED_STR(bufferC, "// ");
            bufferC.addString(g_ByteCodeOpNames[(int) ip->op]);
            moduleToGen->internalError(format("unknown instruction '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
            break;
        }

        bufferC.addEol();
    }

    CONCAT_FIXED_STR(bufferC, "}\n");

    if (bc->node && bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC)
        CONCAT_FIXED_STR(bufferC, "#endif\n");

    return ok;
}

bool BackendC::emitAllFunctionBody()
{
    emitSeparator(bufferC, "SWAG FUNCTIONS");
    if (!emitAllFunctionBody(g_Workspace.runtimeModule))
        return false;
    emitSeparator(bufferC, "INTERNAL FUNCTIONS");
    if (!emitAllFunctionBody(module))
        return false;
    return true;
}

bool BackendC::emitAllFunctionBody(Module* moduleToGen)
{
    SWAG_ASSERT(moduleToGen);

    bool ok = true;
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
            if (!node->content)
                continue;

            typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        }

        ok &= emitFunctionBody(moduleToGen, one);

        // Emit public function wrapper, from real C prototype to swag registers
        if (node && node->attributeFlags & ATTRIBUTE_PUBLIC)
        {
            SWAG_CHECK(emitFuncWrapperPublic(moduleToGen, typeFunc, node, one));
        }
    }

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