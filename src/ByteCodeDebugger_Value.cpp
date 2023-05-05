#include "pch.h"
#include "TypeManager.h"
#include "Ast.h"
#include "ByteCodeDebugger.h"
#include "ByteCode.h"
#include "Workspace.h"

bool ByteCodeDebugger::getValueFormat(const Utf8& cmd, ValueFormat& fmt)
{
    if (cmd.empty() || cmd[0] != '/')
        return false;

    auto cmd1 = cmd;
    cmd1.remove(0, 1);

    // Format
    if (cmd1 == "s8")
    {
        fmt.bitCount = 8;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "s16")
    {
        fmt.bitCount = 16;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "s32")
    {
        fmt.bitCount = 32;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "s64")
    {
        fmt.bitCount = 64;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "u8")
    {
        fmt.bitCount = 8;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "u16")
    {
        fmt.bitCount = 16;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "u32")
    {
        fmt.bitCount = 32;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "u64")
    {
        fmt.bitCount = 64;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "x8")
    {
        fmt.bitCount = 8;
        return true;
    }

    if (cmd1 == "x16")
    {
        fmt.bitCount = 16;
        return true;
    }

    if (cmd1 == "x32")
    {
        fmt.bitCount = 32;
        return true;
    }

    if (cmd1 == "x64")
    {
        fmt.bitCount = 64;
        return true;
    }

    if (cmd1 == "f32")
    {
        fmt.bitCount = 32;
        fmt.isFloat  = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "f64")
    {
        fmt.bitCount = 64;
        fmt.isFloat  = true;
        fmt.isHexa   = false;
        return true;
    }

    return false;
}

void ByteCodeDebugger::appendLiteralValueProtected(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr)
{
    switch (fmt.bitCount)
    {
    case 8:
    default:
        if (fmt.isSigned)
            result = Fmt("%4d ", getAddrValue<int8_t>(addr));
        else if (!fmt.isHexa)
            result = Fmt("%3u ", getAddrValue<uint8_t>(addr));
        else if (fmt.print0x)
            result = Fmt("0x%02llx ", getAddrValue<uint8_t>(addr));
        else
            result = Fmt("%02llx ", getAddrValue<uint8_t>(addr));
        break;

    case 16:
        if (fmt.isSigned)
            result = Fmt("%6d ", getAddrValue<int16_t>(addr));
        else if (!fmt.isHexa)
            result = Fmt("%5u ", getAddrValue<uint16_t>(addr));
        else if (fmt.print0x)
            result = Fmt("0x%04llx ", getAddrValue<uint16_t>(addr));
        else
            result = Fmt("%04llx ", getAddrValue<uint16_t>(addr));
        break;

    case 32:
        if (fmt.isFloat)
            result = Fmt("%16.5g ", getAddrValue<float>(addr));
        else if (fmt.isSigned)
            result = Fmt("%11d ", getAddrValue<int32_t>(addr));
        else if (!fmt.isHexa)
            result = Fmt("%10u ", getAddrValue<uint32_t>(addr));
        else if (fmt.print0x)
            result = Fmt("0x%08llx ", getAddrValue<uint32_t>(addr));
        else
            result = Fmt("%08llx ", getAddrValue<uint32_t>(addr));
        break;

    case 64:
        if (fmt.isFloat)
            result = Fmt("%16.5g ", getAddrValue<double>(addr));
        else if (fmt.isSigned)
            result = Fmt("%21lld ", getAddrValue<int64_t>(addr));
        else if (!fmt.isHexa)
            result = Fmt("%20llu ", getAddrValue<uint64_t>(addr));
        else if (fmt.print0x)
            result = Fmt("0x%016llx ", getAddrValue<uint64_t>(addr));
        else
            result = Fmt("%016llx ", getAddrValue<uint64_t>(addr));
        break;
    }
}

void ByteCodeDebugger::appendLiteralValue(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr)
{
    SWAG_TRY
    {
        appendLiteralValueProtected(context, result, fmt, addr);
    }
    SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
    {
        result = "<error>";
    }
}

void ByteCodeDebugger::appendTypedValueProtected(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent)
{
    auto typeInfo = TypeManager::concreteType(res.type, CONCRETE_FORCEALIAS);
    auto addr     = res.addr;

    if (!addr && res.value)
        addr = &res.value->reg;

    if (typeInfo->isPointerRef())
    {
        auto ptr = ((uint8_t**) addr)[0];
        if (ptr == nullptr)
            str += "null";
        else
        {
            str += Fmt("0x%016llx", ptr);
            auto res1 = res;
            res1.type = TypeManager::concretePtrRef(typeInfo);
            res1.addr = *(void**) res1.addr;
            appendTypedValue(context, str, res1, indent + 1);
        }

        return;
    }

    if (typeInfo->isEnum())
    {
        Register reg;
        auto     ptr = ((uint8_t**) addr)[0];
        reg.pointer  = ptr;
        str += Ast::enumToString(typeInfo, res.value ? res.value->text : Utf8{}, reg, false);
        return;
    }

    if (typeInfo->isPointerToTypeInfo())
    {
        auto ptr = ((ExportedTypeInfo**) addr)[0];
        if (!ptr)
            str += "null";
        else
        {
            str += Fmt("0x%016llx", ptr);
            str += " ";
            Utf8 str1;
            str1.setView((const char*) ptr->name.buffer, (int) ptr->name.count);
            str += COLOR_TYPE;
            str += str1;
            str += COLOR_DEFAULT;
        }

        return;
    }

    if (typeInfo->isPointer())
    {
        auto ptr = ((uint8_t**) addr)[0];
        if (ptr == nullptr)
            str += "null";
        else
            str += Fmt("0x%016llx", ptr);
        return;
    }

    if (typeInfo->isLambdaClosure())
    {
        auto ptr = ((uint8_t**) addr)[0];
        if (ptr == nullptr)
            str += "null";
        else
        {
            str += Fmt("0x%016llx ", ptr);
            if (ByteCode::isByteCodeLambda(ptr))
            {
                str += "(bytecode) ";
                auto bc = (ByteCode*) ByteCode::undoByteCodeLambda(ptr);
                str += COLOR_NAME;
                str += bc->name.c_str();
                str += " ";
                str += COLOR_TYPE;
                str += bc->getCallType()->getDisplayNameC();
                str += COLOR_DEFAULT;
            }
            else
            {
                str += "(native) ";
            }
        }

        return;
    }

    if (typeInfo->isStruct())
    {
        if (res.value)
            addr = res.value->reg.pointer;
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        str += "\n";
        for (auto p : typeStruct->fields)
        {
            for (int i = 0; i < indent + 1; i++)
                str += "   ";
            str += Fmt("(%s%s%s) %s%s%s = ", COLOR_TYPE, p->typeInfo->getDisplayNameC(), COLOR_DEFAULT, COLOR_NAME, p->name.c_str(), COLOR_DEFAULT);
            EvaluateResult res1;
            res1.type = p->typeInfo;
            res1.addr = ((uint8_t*) addr) + p->offset;
            appendTypedValue(context, str, res1, indent + 1);
            if (str.back() != '\n')
                str += "\n";
        }

        return;
    }

    if (typeInfo->isArray())
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        str += Fmt("0x%016llx\n", addr);
        for (uint32_t idx = 0; idx < typeArray->count; idx++)
        {
            for (int i = 0; i < indent; i++)
                str += "   ";
            str += Fmt(" [%d] ", idx);
            EvaluateResult res1;
            res1.type = typeArray->pointedType;
            res1.addr = ((uint8_t*) addr) + (idx * typeArray->pointedType->sizeOf);
            appendTypedValue(context, str, res1, indent + 1);
            if (str.back() != '\n')
                str += "\n";
        }

        return;
    }

    if (typeInfo->isSlice())
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto ptr       = ((uint8_t**) addr)[0];
        auto count     = ((uint64_t*) addr)[1];
        if (ptr == nullptr)
            str += "null";
        else
        {
            str += Fmt("(0x%016llx ", ptr);
            str += Fmt("%llu)\n", count);
            for (uint64_t idx = 0; idx < count; idx++)
            {
                for (int i = 0; i < indent; i++)
                    str += "   ";
                str += Fmt(" [%d] ", idx);
                EvaluateResult res1;
                res1.type = typeSlice->pointedType;
                res1.addr = ptr + (idx * typeSlice->pointedType->sizeOf);
                appendTypedValue(context, str, res1, indent + 1);
                if (str.back() != '\n')
                    str += "\n";
            }
        }

        return;
    }

    if (typeInfo->isInterface())
    {
        auto ptr = ((uint8_t**) addr)[0];
        if (ptr == nullptr)
            str += "null";
        else
        {
            str += Fmt("(0x%016llx ", ptr);
            str += Fmt("0x%016llx)", ((void**) addr)[1]);
        }
        return;
    }

    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Any:
        {
            auto ptr = ((uint8_t**) addr)[0];
            if (ptr == nullptr)
                str += "null";
            else
            {
                str += Fmt("(0x%016llx ", ((void**) addr)[0]);
                str += Fmt("0x%016llx)", ((void**) addr)[1]);
                EvaluateResult res1;
                res1.type = g_Workspace->swagScope.regTypeInfo;
                res1.addr = ((void**) addr)[1];
                appendTypedValue(context, str, res1, indent + 1);
                if (str.back() != '\n')
                    str += "\n";
            }
            return;
        }

        case NativeTypeKind::String:
        {
            void*    ptr;
            uint64_t len;

            if (res.value)
            {
                ptr = res.value->text.buffer;
                len = res.value->text.length();
            }
            else
            {
                ptr = ((void**) addr)[0];
                len = ((uint64_t*) addr)[1];
            }

            if (!ptr)
                str += "null";
            else
            {
                Utf8 str1;
                str1.resize((int) len);
                memcpy(str1.buffer, ptr, len);
                str += "\"";
                str += str1;
                str += "\"";
            }

            return;
        }

        case NativeTypeKind::Bool:
            str += Fmt("%s", *(bool*) addr ? "true" : "false");
            return;

        case NativeTypeKind::S8:
            str += Fmt("%d", *(int8_t*) addr);
            return;
        case NativeTypeKind::S16:
            str += Fmt("%d", *(int16_t*) addr);
            return;
        case NativeTypeKind::S32:
            str += Fmt("%d", *(int32_t*) addr);
            return;
        case NativeTypeKind::S64:
            str += Fmt("%lld", *(int64_t*) addr);
            return;

        case NativeTypeKind::U8:
            str += Fmt("%u (0x%x)", *(uint8_t*) addr, *(uint8_t*) addr);
            return;
        case NativeTypeKind::U16:
            str += Fmt("%u (0x%x)", *(uint16_t*) addr, *(uint16_t*) addr);
            return;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            str += Fmt("%u (0x%x)", *(uint32_t*) addr, *(uint32_t*) addr);
            return;
        case NativeTypeKind::U64:
            str += Fmt("%llu (0x%llx)", *(uint64_t*) addr, *(uint64_t*) addr);
            return;

        case NativeTypeKind::F32:
            str += Fmt("%f", *(float*) addr);
            return;
        case NativeTypeKind::F64:
            str += Fmt("%lf", *(double*) addr);
            return;
        default:
            break;
        }
    }

    str += "?";
}

void ByteCodeDebugger::appendTypedValue(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent)
{
    SWAG_TRY
    {
        appendTypedValueProtected(context, str, res, indent);
    }
    SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
    {
        str += "<error>";
    }
}
