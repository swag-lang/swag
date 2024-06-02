#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"
#include "Semantic/Symbol/Symbol.h"
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

void ByteCodeDebugger::appendLiteralValueProtected(ByteCodeRunContext* /*context*/, Utf8& result, const ValueFormat& fmt, const void* addr)
{
    switch (fmt.bitCount)
    {
        case 8:
        default:
            if (fmt.isSigned)
                result = form("%4d ", getAddrValue<int8_t>(addr));
            else if (!fmt.isHexa)
                result = form("%3u ", getAddrValue<uint8_t>(addr));
            else if (fmt.print0X)
                result = form("0x%02llx ", getAddrValue<uint8_t>(addr));
            else
                result = form("%02llx ", getAddrValue<uint8_t>(addr));
            break;

        case 16:
            if (fmt.isSigned)
                result = form("%6d ", getAddrValue<int16_t>(addr));
            else if (!fmt.isHexa)
                result = form("%5u ", getAddrValue<uint16_t>(addr));
            else if (fmt.print0X)
                result = form("0x%04llx ", getAddrValue<uint16_t>(addr));
            else
                result = form("%04llx ", getAddrValue<uint16_t>(addr));
            break;

        case 32:
            if (fmt.isFloat)
                result = form("%16.5g ", getAddrValue<float>(addr));
            else if (fmt.isSigned)
                result = form("%11d ", getAddrValue<int32_t>(addr));
            else if (!fmt.isHexa)
                result = form("%10u ", getAddrValue<uint32_t>(addr));
            else if (fmt.print0X)
                result = form("0x%08llx ", getAddrValue<uint32_t>(addr));
            else
                result = form("%08llx ", getAddrValue<uint32_t>(addr));
            break;

        case 64:
            if (fmt.isFloat)
                result = form("%16.5g ", getAddrValue<double>(addr));
            else if (fmt.isSigned)
                result = form("%21lld ", getAddrValue<int64_t>(addr));
            else if (!fmt.isHexa)
                result = form("%20llu ", getAddrValue<uint64_t>(addr));
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

void ByteCodeDebugger::appendTypedValueProtected(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, uint32_t level, uint32_t indent)
{
    auto typeInfo = res.type->getConcreteAlias();
    auto addr     = res.addr;

    if (!addr && res.value)
        addr = &res.value->reg;

    if (typeInfo->isPointerRef())
    {
        auto ptr = static_cast<uint8_t**>(addr)[0];
        if (ptr == nullptr)
            str += "null";
        else
        {
            str += form("0x%016llx ", ptr);
            auto res1 = res;
            res1.type = TypeManager::concretePtrRef(typeInfo);
            res1.addr = *static_cast<void**>(res1.addr);
            appendTypedValue(context, str, res1, level, indent + 1);
        }

        return;
    }

    if (typeInfo->isEnum())
    {
        Register reg;
        auto     ptr = static_cast<uint8_t**>(addr)[0];
        reg.pointer  = ptr;
        str += Ast::enumToString(typeInfo, res.value ? res.value->text : Utf8{}, reg, false);
        return;
    }

    if (typeInfo->isPointerToTypeInfo())
    {
        auto ptr = static_cast<ExportedTypeInfo**>(addr)[0];
        if (!ptr)
            str += "null";
        else
        {
            str += form("0x%016llx", ptr);
            str += " ";
            Utf8 str1;
            str1.setView(static_cast<const char*>(ptr->name.buffer), static_cast<uint32_t>(ptr->name.count));
            str += Log::colorToVTS(LogColor::Type);
            str += str1;
            str += Log::colorToVTS(LogColor::Default);
        }

        return;
    }

    if (typeInfo->isPointer())
    {
        auto ptr = static_cast<uint8_t**>(addr)[0];
        if (ptr == nullptr)
            str += "null";
        else
            str += form("0x%016llx", ptr);
        return;
    }

    if (typeInfo->isLambdaClosure())
    {
        auto ptr = static_cast<uint8_t**>(addr)[0];
        if (ptr == nullptr)
            str += "null";
        else
        {
            str += form("0x%016llx ", ptr);
            if (ByteCode::isByteCodeLambda(ptr))
            {
                str += "(bytecode) ";
                auto bc = static_cast<ByteCode*>(ByteCode::undoByteCodeLambda(ptr));
                str += Log::colorToVTS(LogColor::Name);
                str += bc->name;
                str += " ";
                str += Log::colorToVTS(LogColor::Type);
                str += bc->getCallType()->getDisplayNameC();
                str += Log::colorToVTS(LogColor::Default);
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
        auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        if (!g_ByteCodeDebugger.printStruct && level)
        {
            str += "<hidden>";
        }
        else
        {
            str += "\n";
            for (auto p : typeStruct->fields)
            {
                for (uint32_t i = 0; i < indent + 1; i++)
                    str += "   ";
                str += form("(%s%s%s) %s%s%s = ",
                            Log::colorToVTS(LogColor::Type).c_str(),
                            p->typeInfo->getDisplayNameC(),
                            Log::colorToVTS(LogColor::Default).c_str(),
                            Log::colorToVTS(LogColor::Name).c_str(),
                            p->name.c_str(),
                            Log::colorToVTS(LogColor::Default).c_str());
                EvaluateResult res1;
                res1.type = p->typeInfo;
                res1.addr = static_cast<uint8_t*>(addr) + p->offset;
                appendTypedValue(context, str, res1, level + 1, indent + 1);
                if (str.back() != '\n')
                    str += "\n";
            }
        }

        return;
    }

    if (typeInfo->isArray())
    {
        auto typeArray = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        str += form("0x%016llx ", addr);
        if (!g_ByteCodeDebugger.printArray && level)
        {
            str += "<hidden>";
        }
        else
        {
            str += "\n";
            for (uint32_t idx = 0; idx < typeArray->count; idx++)
            {
                for (uint32_t i = 0; i < indent; i++)
                    str += "   ";
                str += form(" [%d] ", idx);
                EvaluateResult res1;
                res1.type = typeArray->pointedType;
                res1.addr = static_cast<uint8_t*>(addr) + static_cast<size_t>(idx * typeArray->pointedType->sizeOf);
                appendTypedValue(context, str, res1, level + 1, indent + 1);
                if (str.back() != '\n')
                    str += "\n";
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
            str += "null";
        else
        {
            str += form("(0x%016llx ", ptr);
            str += form("%llu) ", count);
            if (!g_ByteCodeDebugger.printArray)
            {
                str += "<hidden>";
            }
            else
            {
                str += "\n";
                for (uint64_t idx = 0; idx < count; idx++)
                {
                    for (uint32_t i = 0; i < indent; i++)
                        str += "   ";
                    str += form(" [%d] ", idx);
                    EvaluateResult res1;
                    res1.type = typeSlice->pointedType;
                    res1.addr = ptr + idx * typeSlice->pointedType->sizeOf;
                    appendTypedValue(context, str, res1, level + 1, indent + 1);
                    if (str.back() != '\n')
                        str += "\n";
                }
            }
        }

        return;
    }

    if (typeInfo->isInterface())
    {
        auto ptr = static_cast<uint8_t**>(addr)[0];
        if (ptr == nullptr)
            str += "null";
        else
        {
            str += form("(0x%016llx ", ptr);
            str += form("0x%016llx)", static_cast<void**>(addr)[1]);
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
                    str += "null";
                else
                {
                    str += form("(0x%016llx ", static_cast<void**>(addr)[0]);
                    str += form("0x%016llx)", static_cast<void**>(addr)[1]);
                    EvaluateResult res1;
                    res1.type = g_Workspace->swagScope.regTypeInfo;
                    res1.addr = static_cast<void**>(addr)[1];
                    appendTypedValue(context, str, res1, level, indent + 1);
                    if (str.back() != '\n')
                        str += "\n";
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
                    len = static_cast<uint64_t*>(addr)[1];
                }

                if (!ptr)
                    str += "null";
                else
                {
                    Utf8 str1;
                    str1.resize(static_cast<uint32_t>(len));
                    std::copy_n(ptr, len, str1.buffer);
                    str += "\"";
                    str += str1;
                    str += "\"";
                }

                return;
            }

            case NativeTypeKind::Bool:
                str += form("%s", *static_cast<bool*>(addr) ? "true" : "false");
                return;

            case NativeTypeKind::S8:
                str += form("%d", *static_cast<int8_t*>(addr));
                return;
            case NativeTypeKind::S16:
                str += form("%d", *static_cast<int16_t*>(addr));
                return;
            case NativeTypeKind::S32:
                str += form("%d", *static_cast<int32_t*>(addr));
                return;
            case NativeTypeKind::S64:
                str += form("%lld", *static_cast<int64_t*>(addr));
                return;

            case NativeTypeKind::U8:
                str += form("%u (0x%x)", *static_cast<uint8_t*>(addr), *static_cast<uint8_t*>(addr));
                return;
            case NativeTypeKind::U16:
                str += form("%u (0x%x)", *static_cast<uint16_t*>(addr), *static_cast<uint16_t*>(addr));
                return;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                str += form("%u (0x%x)", *static_cast<uint32_t*>(addr), *static_cast<uint32_t*>(addr));
                return;
            case NativeTypeKind::U64:
                str += form("%llu (0x%llx)", *static_cast<uint64_t*>(addr), *static_cast<uint64_t*>(addr));
                return;

            case NativeTypeKind::F32:
                str += form("%f", *static_cast<float*>(addr));
                return;
            case NativeTypeKind::F64:
                str += form("%lf", *static_cast<double*>(addr));
                return;
            default:
                break;
        }
    }

    str += "?";
}

void ByteCodeDebugger::appendTypedValue(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, uint32_t level, uint32_t indent)
{
    SWAG_TRY
    {
        appendTypedValueProtected(context, str, res, level, indent);
    }
    SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
    {
        str += "<error>";
    }
}

void ByteCodeDebugger::appendTypedValue(ByteCodeRunContext* context, const Utf8& filter, const AstNode* node, uint8_t* baseAddr, uint8_t* realAddr, Utf8& result)
{
    const auto over = node->resolvedSymbolOverload();
    if (!over)
        return;
    if (over->symbol->name.length() > 2 && over->symbol->name[0] == '_' && over->symbol->name[1] == '_') // Generated
        return;
    if (!testNameFilter(over->symbol->name, filter))
        return;

    Utf8 str = form("(%s%s%s) %s%s%s = ",
                    Log::colorToVTS(LogColor::Type).c_str(),
                    over->typeInfo->getDisplayNameC(),
                    Log::colorToVTS(LogColor::Default).c_str(),
                    Log::colorToVTS(LogColor::Name).c_str(),
                    over->symbol->name.c_str(),
                    Log::colorToVTS(LogColor::Default).c_str());

    EvaluateResult res;
    res.type = over->typeInfo;
    res.addr = realAddr ? realAddr : baseAddr + over->computedValue.storageOffset;
    appendTypedValue(context, str, res, 0, 0);
    str.trim();
    while (str.back() == '\n')
        str.removeBack();
    result += str;
    result += "\n";
}
