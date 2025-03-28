#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"
#include "Semantic/DataSegment.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Wmf/Workspace.h"

bool ByteCodeDebugger::getValueFormat(const Utf8& cmd, ValueFormat& fmt)
{
    if (cmd.length() <= 2 || cmd[0] != '-' || cmd[1] != '-')
        return false;

    auto cmd1 = cmd;
    cmd1.remove(0, 2);

    // Format
    if (cmd1 == "s8")
    {
        fmt.bitCount = 8;
        fmt.isSigned = true;
        fmt.isFloat  = false;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "s16")
    {
        fmt.bitCount = 16;
        fmt.isSigned = true;
        fmt.isFloat  = false;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "s32")
    {
        fmt.bitCount = 32;
        fmt.isSigned = true;
        fmt.isFloat  = false;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "s64")
    {
        fmt.bitCount = 64;
        fmt.isSigned = true;
        fmt.isFloat  = false;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "u8")
    {
        fmt.bitCount = 8;
        fmt.isSigned = false;
        fmt.isFloat  = false;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "u16")
    {
        fmt.bitCount = 16;
        fmt.isSigned = false;
        fmt.isFloat  = false;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "u32")
    {
        fmt.bitCount = 32;
        fmt.isSigned = false;
        fmt.isFloat  = false;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "u64")
    {
        fmt.bitCount = 64;
        fmt.isSigned = false;
        fmt.isFloat  = false;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "x8")
    {
        fmt.bitCount = 8;
        fmt.isSigned = false;
        fmt.isFloat  = false;
        fmt.isHexa   = true;
        return true;
    }

    if (cmd1 == "x16")
    {
        fmt.bitCount = 16;
        fmt.isSigned = false;
        fmt.isFloat  = false;
        fmt.isHexa   = true;
        return true;
    }

    if (cmd1 == "x32")
    {
        fmt.bitCount = 32;
        fmt.isSigned = false;
        fmt.isFloat  = false;
        fmt.isHexa   = true;
        return true;
    }

    if (cmd1 == "x64")
    {
        fmt.bitCount = 64;
        fmt.isSigned = false;
        fmt.isFloat  = false;
        fmt.isHexa   = true;
        return true;
    }

    if (cmd1 == "f32")
    {
        fmt.bitCount = 32;
        fmt.isSigned = true;
        fmt.isFloat  = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd1 == "f64")
    {
        fmt.bitCount = 64;
        fmt.isSigned = true;
        fmt.isFloat  = true;
        fmt.isHexa   = false;
        return true;
    }

    return false;
}

void ByteCodeDebugger::appendLiteralValueProtected(ByteCodeRunContext* /*context*/, Utf8& result, const ValueFormat& fmt, const void* addr)
{
    switch (fmt.bitCount)
    {
        case 8:
        default:
            if (fmt.isSigned && fmt.align)
                result = form("%4d ", getAddrValue<int8_t>(addr));
            else if (fmt.isSigned)
                result = form("%d ", getAddrValue<int8_t>(addr));
            else if (!fmt.isHexa && fmt.align)
                result = form("%3u ", getAddrValue<uint8_t>(addr));
            else if (!fmt.isHexa)
                result = form("%u ", getAddrValue<uint8_t>(addr));
            else if (fmt.print0X)
                result = form("0x%02llx ", getAddrValue<uint8_t>(addr));
            else
                result = form("%02llx ", getAddrValue<uint8_t>(addr));
            break;

        case 16:
            if (fmt.isSigned && fmt.align)
                result = form("%6d ", getAddrValue<int16_t>(addr));
            else if (fmt.isSigned)
                result = form("%d ", getAddrValue<int16_t>(addr));
            else if (!fmt.isHexa && fmt.align)
                result = form("%5u ", getAddrValue<uint16_t>(addr));
            else if (!fmt.isHexa)
                result = form("%u ", getAddrValue<uint16_t>(addr));
            else if (fmt.print0X)
                result = form("0x%04llx ", getAddrValue<uint16_t>(addr));
            else
                result = form("%04llx ", getAddrValue<uint16_t>(addr));
            break;

        case 32:
            if (fmt.isFloat && fmt.align)
                result = form("%16.5g ", getAddrValue<float>(addr));
            else if (fmt.isFloat)
                result = form("%f ", getAddrValue<double>(addr));
            else if (fmt.isSigned && fmt.align)
                result = form("%11d ", getAddrValue<int32_t>(addr));
            else if (fmt.isSigned)
                result = form("%d ", getAddrValue<int32_t>(addr));
            else if (!fmt.isHexa && fmt.align)
                result = form("%10u ", getAddrValue<uint32_t>(addr));
            else if (!fmt.isHexa)
                result = form("%u ", getAddrValue<uint32_t>(addr));
            else if (fmt.print0X)
                result = form("0x%08llx ", getAddrValue<uint32_t>(addr));
            else
                result = form("%08llx ", getAddrValue<uint32_t>(addr));
            break;

        case 64:
            if (fmt.isFloat && fmt.align)
                result = form("%16.5g ", getAddrValue<double>(addr));
            else if (fmt.isFloat)
                result = form("%f ", getAddrValue<double>(addr));
            else if (fmt.isSigned && fmt.align)
                result = form("%21lld ", getAddrValue<int64_t>(addr));
            else if (fmt.isSigned)
                result = form("%d ", getAddrValue<int64_t>(addr));
            else if (!fmt.isHexa && fmt.align)
                result = form("%20llu ", getAddrValue<uint64_t>(addr));
            else if (!fmt.isHexa)
                result = form("%u ", getAddrValue<uint64_t>(addr));
            else if (fmt.print0X)
                result = form("0x%016llx ", getAddrValue<uint64_t>(addr));
            else
                result = form("%016llx ", getAddrValue<uint64_t>(addr));
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

void ByteCodeDebugger::appendTypedValueProtected(ByteCodeRunContext* context, Utf8& result, Utf8& resultForExpression, EvaluateResult& res, uint32_t level, uint32_t indent)
{
    auto typeInfo = res.type->getConcreteAlias();
    auto addr     = res.addr;

    if (!addr && res.value)
    {
        if (res.value->storageSegment && res.value->storageOffset != UINT32_MAX)
        {
            res.addr = res.value->storageSegment->address(res.value->storageOffset);
            if (typeInfo->isStruct())
                addr = res.addr;
            else
                addr = &res.addr;
        }
        else if (typeInfo->isStruct())
            addr = res.value->reg.pointer;
        else
            addr = &res.value->reg;
    }

    if (typeInfo->isPointerRef())
    {
        auto ptr = static_cast<uint8_t**>(addr)[0];
        if (ptr == nullptr)
            result += "null";
        else
        {
            result += form("0x%016llx ", ptr);
            resultForExpression += form("0x%016llx ", ptr);
            auto res1 = res;
            res1.type = TypeManager::concretePtrRef(typeInfo);
            res1.addr = *static_cast<void**>(res1.addr);
            Utf8 dum;
            appendTypedValue(context, result, dum, res1, level, indent + 1);
        }

        return;
    }

    if (typeInfo->isEnum())
    {
        Register reg;
        auto     ptr = static_cast<uint8_t**>(addr)[0];
        reg.pointer  = ptr;
        result += Ast::enumToString(typeInfo, res.value ? res.value->text : Utf8{}, reg, false);
        return;
    }

    if (typeInfo->isPointerToTypeInfo())
    {
        auto ptr = static_cast<ExportedTypeInfo**>(addr)[0];
        if (!ptr)
            result += "null";
        else
        {
            result += form("0x%016llx", ptr);
            result += " ";
            Utf8 str1;
            str1.setView(static_cast<const char*>(ptr->name.buffer), static_cast<uint32_t>(ptr->name.count));
            result += Log::colorToVTS(LogColor::Type);
            result += str1;
            result += Log::colorToVTS(LogColor::Default);
        }

        return;
    }

    if (typeInfo->isPointer())
    {
        auto ptr = static_cast<uint8_t**>(addr)[0];
        if (ptr == nullptr)
            result += "null";
        else
            result += form("0x%016llx", ptr);
        return;
    }

    if (typeInfo->isLambdaClosure())
    {
        auto ptr = static_cast<uint8_t**>(addr)[0];
        if (ptr == nullptr)
            result += "null";
        else
        {
            result += form("0x%016llx ", ptr);
            if (ByteCode::isByteCodeLambda(ptr))
            {
                result += "(bytecode) ";
                auto bc = static_cast<ByteCode*>(ByteCode::undoByteCodeLambda(ptr));
                result += Log::colorToVTS(LogColor::Name);
                result += bc->name;
                result += " ";
                result += Log::colorToVTS(LogColor::Type);
                result += bc->getCallType()->getDisplayNameC();
                result += Log::colorToVTS(LogColor::Default);
            }
            else
            {
                result += "(native) ";
            }
        }

        return;
    }

    if (typeInfo->isStruct())
    {
        auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        if (!g_ByteCodeDebugger.printStruct && level)
        {
            result += "<hidden>";
        }
        else
        {
            result += "\n";
            for (auto p : typeStruct->fields)
            {
                for (uint32_t i = 0; i < indent + 1; i++)
                    result += "   ";
                result += getPrintValue(p->name, p->typeInfo);
                result += " = ";
                EvaluateResult res1;
                res1.type = p->typeInfo;
                res1.addr = static_cast<uint8_t*>(addr) + p->offset;
                Utf8 dum;
                appendTypedValue(context, result, dum, res1, level + 1, indent + 1);
                if (result.back() != '\n')
                    result += "\n";
            }
        }

        return;
    }

    if (typeInfo->isArray())
    {
        auto typeArray = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        result += form("0x%016llx ", addr);
        if (!g_ByteCodeDebugger.printArray && level)
        {
            result += "<hidden>";
        }
        else
        {
            result += "\n";
            for (uint32_t idx = 0; idx < typeArray->count; idx++)
            {
                for (uint32_t i = 0; i < indent; i++)
                    result += "   ";
                result += form(" [%d] ", idx);
                EvaluateResult res1;
                res1.type = typeArray->pointedType;
                res1.addr = static_cast<uint8_t*>(addr) + static_cast<size_t>(idx * typeArray->pointedType->sizeOf);
                Utf8 dum;
                appendTypedValue(context, result, dum, res1, level + 1, indent + 1);
                if (result.back() != '\n')
                    result += "\n";
            }
        }

        return;
    }

    if (typeInfo->isSlice())
    {
        auto typeSlice = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto ptr       = static_cast<uint8_t**>(addr)[0];
        auto count     = static_cast<uint64_t*>(addr)[1];
        if (ptr == nullptr)
            result += "null";
        else
        {
            result += form("(0x%016llx ", ptr);
            result += form("%llu) ", count);
            if (!g_ByteCodeDebugger.printArray)
            {
                result += "<hidden>";
            }
            else
            {
                result += "\n";
                for (uint64_t idx = 0; idx < count; idx++)
                {
                    for (uint32_t i = 0; i < indent; i++)
                        result += "   ";
                    result += form(" [%d] ", idx);
                    EvaluateResult res1;
                    res1.type = typeSlice->pointedType;
                    res1.addr = ptr + idx * typeSlice->pointedType->sizeOf;
                    Utf8 dum;
                    appendTypedValue(context, result, dum, res1, level + 1, indent + 1);
                    if (result.back() != '\n')
                        result += "\n";
                }
            }
        }

        return;
    }

    if (typeInfo->isInterface())
    {
        auto ptr = static_cast<uint8_t**>(addr)[0];
        if (ptr == nullptr)
            result += "null";
        else
        {
            result += form("(0x%016llx ", ptr);
            result += form("0x%016llx)", static_cast<void**>(addr)[1]);
        }
        return;
    }

    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::Any:
            {
                auto ptr = static_cast<uint8_t**>(addr)[0];
                if (ptr == nullptr)
                    result += "null";
                else
                {
                    result += form("(0x%016llx ", static_cast<void**>(addr)[0]);
                    result += form("0x%016llx)", static_cast<void**>(addr)[1]);
                    EvaluateResult res1;
                    res1.type = g_Workspace->swagScope.regTypeInfo;
                    res1.addr = static_cast<void**>(addr)[1];
                    Utf8 dum;
                    appendTypedValue(context, result, dum, res1, level, indent + 1);
                    if (result.back() != '\n')
                        result += "\n";
                }
                return;
            }

            case NativeTypeKind::String:
            {
                char*    ptr;
                uint64_t len;

                if (res.value)
                {
                    ptr = res.value->text.buffer;
                    len = res.value->text.length();
                }
                else
                {
                    ptr = static_cast<char**>(addr)[0];
                    len = std::min(static_cast<uint64_t*>(addr)[1], static_cast<uint64_t>(128));
                }

                if (!ptr)
                    result += "null";
                else
                {
                    Utf8 str1;
                    str1.resize(static_cast<uint32_t>(len));
                    std::copy_n(ptr, len, str1.buffer);
                    result += "\"";
                    result += str1;
                    result += "\"";
                }

                return;
            }

            case NativeTypeKind::Bool:
                result += form("%s", *static_cast<bool*>(addr) ? "true" : "false");
                resultForExpression = form("%s", *static_cast<bool*>(addr) ? "true" : "false");
                return;

            case NativeTypeKind::S8:
                result += form("%d", *static_cast<int8_t*>(addr));
                resultForExpression = form("%d", *static_cast<int8_t*>(addr));
                return;
            case NativeTypeKind::S16:
                result += form("%d", *static_cast<int16_t*>(addr));
                resultForExpression = form("%d", *static_cast<int16_t*>(addr));
                return;
            case NativeTypeKind::S32:
                result += form("%d", *static_cast<int32_t*>(addr));
                resultForExpression = form("%d", *static_cast<int32_t*>(addr));
                return;
            case NativeTypeKind::S64:
                result += form("%lld", *static_cast<int64_t*>(addr));
                resultForExpression = form("%lld", *static_cast<int64_t*>(addr));
                return;

            case NativeTypeKind::U8:
                result += form("%u (0x%x)", *static_cast<uint8_t*>(addr), *static_cast<uint8_t*>(addr));
                resultForExpression = form("0x%x", *static_cast<uint8_t*>(addr));
                return;
            case NativeTypeKind::U16:
                result += form("%u (0x%x)", *static_cast<uint16_t*>(addr), *static_cast<uint16_t*>(addr));
                resultForExpression = form("0x%x", *static_cast<uint16_t*>(addr));
                return;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                result += form("%u (0x%x)", *static_cast<uint32_t*>(addr), *static_cast<uint32_t*>(addr));
                resultForExpression += form("0x%x", *static_cast<uint32_t*>(addr));
                return;
            case NativeTypeKind::U64:
                result += form("%llu (0x%llx)", *static_cast<uint64_t*>(addr), *static_cast<uint64_t*>(addr));
                resultForExpression += form("0x%llx", *static_cast<uint64_t*>(addr));
                return;

            case NativeTypeKind::F32:
                result += form("%f", *static_cast<float*>(addr));
                resultForExpression = form("%f", *static_cast<float*>(addr));
                return;
            case NativeTypeKind::F64:
                result += form("%lf", *static_cast<double*>(addr));
                resultForExpression += form("%lf", *static_cast<double*>(addr));
                return;
            default:
                break;
        }
    }

    result += "?";
}

void ByteCodeDebugger::appendTypedValue(ByteCodeRunContext* context, Utf8& result, Utf8& resultLight, EvaluateResult& res, uint32_t level, uint32_t indent)
{
    SWAG_TRY
    {
        appendTypedValueProtected(context, result, resultLight, res, level, indent);
    }
    SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
    {
        result += "<error>";
    }
}
