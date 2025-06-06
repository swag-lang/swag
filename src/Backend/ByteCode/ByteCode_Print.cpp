#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/SyntaxColor.h"
#include "Syntax/Tokenizer/Tokenizer.h"
#include "Wmf/Module.h"

Utf8 ByteCode::getPrintName() const
{
    if (out && node)
        return node->getScopedName();
    return name;
}

Utf8 ByteCode::getPrintFileName() const
{
    if (sourceFile)
        return sourceFile->name;
    return "";
}

Utf8 ByteCode::getPrintRefName() const
{
    Utf8 str = Log::colorToVTS(LogColor::Name);
    str += getPrintName();

    const auto type = getCallType();
    if (type)
    {
        str += " ";
        str += Log::colorToVTS(LogColor::Type);
        str += type->getDisplayName();
    }

    if (!out)
        return str;

    const auto loc = getLocation(this, out);
    if (loc.file || loc.location)
        str += " ";

    if (loc.file)
    {
        str += Log::colorToVTS(LogColor::Location);
        str += loc.file->path;
    }

    if (loc.location)
    {
        str += Log::colorToVTS(LogColor::Location);
        str += form(":%d", loc.location->line);
    }

    return str;
}

void ByteCode::printSourceCode(const ByteCodePrintOptions& options, const ByteCode* bc, const ByteCodeInstruction* ip, uint32_t* lastLine, SourceFile** lastFile, AstNode** lastInline)
{
    if (ip->op == ByteCodeOp::End)
        return;

    // Print source code
    const bool forDbg = options.curIp != nullptr;
    const auto loc    = getLocation(bc, ip, true);
    const auto loc1   = getLocation(bc, ip);

    if (!loc.location)
        return;

    if (!lastLine || !lastFile || loc.location->line != *lastLine || loc.file != *lastFile)
    {
        const auto inl = ip->node ? ip->node->safeOwnerInline() : nullptr;
        if (loc.file != *lastFile || *lastInline != inl)
        {
            *lastInline = inl;
            if (loc1.file != loc.file || loc1.location->line != loc.location->line)
                g_Log.setColor(LogColor::DarkBlue);
            else
                g_Log.setColor(LogColor::Location);
            if (forDbg)
                g_Log.write("   ");
            g_Log.write(form("%08d ", loc.location->line + 1));
            g_Log.print(form("%s", loc.file->name.cstr()));
            if (inl)
                g_Log.print(form(" %s", inl->func->token.cstr()));
            g_Log.writeEol();
        }

        auto s = loc.file->getLine(loc.location->line);
        s.trim();
        s.replaceAll("  ", " ");

        g_Log.setColor(LogColor::SourceCode);

        if (forDbg)
            g_Log.write("   ");
        g_Log.write(form("%08d ", loc.location->line + 1));

        if (s.empty())
            g_Log.write("<blank>");
        else
            g_Log.print(s);

        g_Log.writeEol();
        *lastLine = loc.location->line;
        *lastFile = loc.file;
    }
}

Utf8 ByteCode::getPrettyInstruction(const ByteCodeInstruction* ip)
{
    Utf8       str   = g_ByteCodeOpDesc[static_cast<int>(ip->op)].display;
    const auto flags = opFlags(ip->op);

    if (ip->hasFlag(BCI_IMM_A) || flags.has(OPF_READ_VAL32_A) || flags.has(OPF_READ_VAL64_A))
    {
        str.replace("_rau8_", form("%u", ip->a.u8));
        str.replace("_rau16_", form("%u", ip->a.u16));
        str.replace("_rau32_", form("%u", ip->a.u32));
        str.replace("_rau64_", form("%llu", ip->a.u64));
        str.replace("_ras8_", form("%d", ip->a.s8));
        str.replace("_ras16_", form("%d", ip->a.s16));
        str.replace("_ras32_", form("%d", ip->a.s32));
        str.replace("_ras64_", form("%lld", ip->a.s64));
        str.replace("_raf32_", form("%f", ip->a.f32));
        str.replace("_raf64_", form("%lf", ip->a.f64));
        str.replace("_rax32_", form("0x%x", ip->a.u32));
        str.replace("_rax64_", form("0x%llx", ip->a.u64));
        str.replace("_rah32_", form("%u", ip->a.mergeU64U32.high));
        str.replace("_ral32_", form("%u", ip->a.mergeU64U32.low));
    }

    if (ip->hasFlag(BCI_IMM_B) || flags.has(OPF_READ_VAL32_B) || flags.has(OPF_READ_VAL64_B))
    {
        str.replace("_rbu8_", form("%u", ip->b.u8));
        str.replace("_rbu16_", form("%u", ip->b.u16));
        str.replace("_rbu32_", form("%u", ip->b.u32));
        str.replace("_rbu64_", form("%llu", ip->b.u64));
        str.replace("_rbs8_", form("%d", ip->b.s8));
        str.replace("_rbs16_", form("%d", ip->b.s16));
        str.replace("_rbs32_", form("%d", ip->b.s32));
        str.replace("_rbs64_", form("%lld", ip->b.s64));
        str.replace("_rbf32_", form("%f", ip->b.f32));
        str.replace("_rbf64_", form("%lf", ip->b.f64));
        str.replace("_rbx32_", form("0x%x", ip->b.u32));
        str.replace("_rbx64_", form("0x%llx", ip->b.u64));
        str.replace("_rbh32_", form("%u", ip->b.mergeU64U32.high));
        str.replace("_rbl32_", form("%u", ip->b.mergeU64U32.low));
    }

    if (ip->hasFlag(BCI_IMM_C) || flags.has(OPF_READ_VAL32_C) || flags.has(OPF_READ_VAL64_C))
    {
        str.replace("_rcu8_", form("%u", ip->c.u8));
        str.replace("_rcu16_", form("%u", ip->c.u16));
        str.replace("_rcu32_", form("%u", ip->c.u32));
        str.replace("_rcu64_", form("%llu", ip->c.u64));
        str.replace("_rcs8_", form("%d", ip->c.s8));
        str.replace("_rcs16_", form("%d", ip->c.s16));
        str.replace("_rcs32_", form("%d", ip->c.s32));
        str.replace("_rcs64_", form("%lld", ip->c.s64));
        str.replace("_rcf32_", form("%f", ip->c.f32));
        str.replace("_rcf64_", form("%lf", ip->c.f64));
        str.replace("_rcx32_", form("0x%x", ip->c.u32));
        str.replace("_rcx64_", form("0x%llx", ip->c.u64));
        str.replace("_rch32_", form("%u", ip->c.mergeU64U32.high));
        str.replace("_rcl32_", form("%u", ip->c.mergeU64U32.low));
    }

    if (ip->hasFlag(BCI_IMM_D) || flags.has(OPF_READ_VAL32_D) || flags.has(OPF_READ_VAL64_D))
    {
        str.replace("_rdu8_", form("%u", ip->d.u8));
        str.replace("_rdu16_", form("%u", ip->d.u16));
        str.replace("_rdu32_", form("%u", ip->d.u32));
        str.replace("_rdu64_", form("%llu", ip->d.u64));
        str.replace("_rds8_", form("%d", ip->d.s8));
        str.replace("_rds16_", form("%d", ip->d.s16));
        str.replace("_rds32_", form("%d", ip->d.s32));
        str.replace("_rds64_", form("%lld", ip->d.s64));
        str.replace("_rdf32_", form("%f", ip->d.f32));
        str.replace("_rdf64_", form("%lf", ip->d.f64));
        str.replace("_rdx32_", form("0x%x", ip->d.u32));
        str.replace("_rdx64_", form("0x%llx", ip->d.u64));
        str.replace("_rdh32_", form("%u", ip->d.mergeU64U32.high));
        str.replace("_rdl32_", form("%u", ip->d.mergeU64U32.low));
    }

    if (flags.has(OPF_READ_A | OPF_WRITE_A))
    {
        const auto ra = form("r%u", ip->a.u32);
        str.replace("_ra_", ra);
        str.replace("_rau8_", ra);
        str.replace("_rau16_", ra);
        str.replace("_rau32_", ra);
        str.replace("_rau64_", ra);
        str.replace("_ras8_", ra);
        str.replace("_ras16_", ra);
        str.replace("_ras32_", ra);
        str.replace("_ras64_", ra);
        str.replace("_raf32_", ra);
        str.replace("_raf64_", ra);
    }

    if (flags.has(OPF_READ_B | OPF_WRITE_B))
    {
        const auto rb = form("r%u", ip->b.u32);
        str.replace("_rb_", rb);
        str.replace("_rbu8_", rb);
        str.replace("_rbu16_", rb);
        str.replace("_rbu32_", rb);
        str.replace("_rbu64_", rb);
        str.replace("_rbs8_", rb);
        str.replace("_rbs16_", rb);
        str.replace("_rbs32_", rb);
        str.replace("_rbs64_", rb);
        str.replace("_rbf32_", rb);
        str.replace("_rbf64_", rb);
    }

    if (flags.has(OPF_READ_C | OPF_WRITE_C))
    {
        const auto rc = form("r%u", ip->c.u32);
        str.replace("_rc_", rc);
        str.replace("_rcu8_", rc);
        str.replace("_rcu16_", rc);
        str.replace("_rcu32_", rc);
        str.replace("_rcu64_", rc);
        str.replace("_rcs8_", rc);
        str.replace("_rcs16_", rc);
        str.replace("_rcs32_", rc);
        str.replace("_rcs64_", rc);
        str.replace("_rcf32_", rc);
        str.replace("_rcf64_", rc);
    }

    if (flags.has(OPF_READ_D | OPF_WRITE_D))
    {
        const auto rd = form("r%u", ip->d.u32);
        str.replace("_rd_", rd);
        str.replace("_rdu8_", rd);
        str.replace("_rdu16_", rd);
        str.replace("_rdu32_", rd);
        str.replace("_rdu64_", rd);
        str.replace("_rds8_", rd);
        str.replace("_rds16_", rd);
        str.replace("_rds32_", rd);
        str.replace("_rds64_", rd);
        str.replace("_rdf32_", rd);
        str.replace("_rdf64_", rd);
    }

    str += " ";
    str.replace(" + 0 ", " ");
    str.replace(" + 0,", ",");
    str.replace(" + 0]", "]");

    switch (ip->op)
    {
        case ByteCodeOp::IntrinsicS8x1:
        case ByteCodeOp::IntrinsicS16x1:
        case ByteCodeOp::IntrinsicS32x1:
        case ByteCodeOp::IntrinsicS64x1:
        case ByteCodeOp::IntrinsicF32x1:
        case ByteCodeOp::IntrinsicF64x1:
        case ByteCodeOp::IntrinsicS8x2:
        case ByteCodeOp::IntrinsicS16x2:
        case ByteCodeOp::IntrinsicS32x2:
        case ByteCodeOp::IntrinsicS64x2:
        case ByteCodeOp::IntrinsicU8x2:
        case ByteCodeOp::IntrinsicU16x2:
        case ByteCodeOp::IntrinsicU32x2:
        case ByteCodeOp::IntrinsicU64x2:
        case ByteCodeOp::IntrinsicF32x2:
        case ByteCodeOp::IntrinsicF64x2:
        {
            Utf8 name = TOKEN_NAMES[ip->d.u32];
            if (name.startsWith("Intrinsic"))
            {
                name.remove(0, 9);
                SWAG_ASSERT(name.length() > 1);
                name.buffer[0] = static_cast<char>(tolower(name[0]));
                name.insert(0, "@");
            }

            str.replace("_w0_", name);
            break;
        }
    }

    str.trim();
    return str;
}

Utf8 ByteCode::getInstructionReg(const char* name, const Register& reg, bool regW, bool regR, bool regImm)
{
    Utf8 str;
    if (regW)
        str += form("%s[%u] ", name, reg.u32);
    if (regR && !regImm)
        str += form("%s(%u) ", name, reg.u32);
    if (regImm)
        str += form("%s{0x%llX} ", name, reg.u64);
    return str;
}

void ByteCode::fillPrintInstruction(const ByteCodePrintOptions& options, const ByteCode* bc, const ByteCodeInstruction* ip, PrintInstructionLine& line)
{
    const uint32_t i      = static_cast<uint32_t>(ip - bc->out);
    const bool     forDbg = options.curIp != nullptr;

    // Print for bcdbg
    if (forDbg)
    {
        // Instruction index breakpoint
        const ByteCodeDebugger::DebugBreakpoint* hasBkp = nullptr;
        for (const auto& bkp : g_ByteCodeDebugger.breakpoints)
        {
            switch (bkp.type)
            {
                case ByteCodeDebugger::DebugBkpType::InstructionIndex:
                    if (bkp.bc == bc && bkp.line == i)
                        hasBkp = &bkp;
                    break;
                default:
                    break;
            }
        }

        if (hasBkp)
        {
            if (hasBkp->disabled)
                line.bkp += Log::colorToVTS(LogColor::Gray);
            else
                line.bkp += Log::colorToVTS(LogColor::Breakpoint);
            line.bkp += Utf8("\xe2\x96\xa0");

            if (ip == options.curIp)
                line.rank += "> ";
            else
                line.rank += "  ";
        }
        else if (ip == options.curIp)
            line.rank += "-> ";
        else
            line.rank += "   ";
    }

    // Instruction rank
    line.rank += form("%08d", i);

    // Instruction name
    line.name += opName(ip->op);

    // Parameters
    const auto flags = opFlags(ip->op);
    line.args += getInstructionReg("A", ip->a, flags.has(OPF_WRITE_A), flags.has(OPF_READ_A), flags.has(OPF_READ_VAL32_A | OPF_READ_VAL64_A) || (flags.has(OPF_IMM_A) && ip->hasFlag(BCI_IMM_A)));
    line.args += getInstructionReg("B", ip->b, flags.has(OPF_WRITE_B), flags.has(OPF_READ_B), flags.has(OPF_READ_VAL32_B | OPF_READ_VAL64_B) || (flags.has(OPF_IMM_B) && ip->hasFlag(BCI_IMM_B)));
    line.args += getInstructionReg("C", ip->c, flags.has(OPF_WRITE_C), flags.has(OPF_READ_C), flags.has(OPF_READ_VAL32_C | OPF_READ_VAL64_C) || (flags.has(OPF_IMM_C) && ip->hasFlag(BCI_IMM_C)));
    line.args += getInstructionReg("D", ip->d, flags.has(OPF_WRITE_D), flags.has(OPF_READ_D), flags.has(OPF_READ_VAL32_D | OPF_READ_VAL64_D) || (flags.has(OPF_IMM_D) && ip->hasFlag(BCI_IMM_D)));
    line.args.trim();

    // Flags
    line.flags += ip->hasFlag(BCI_SAFETY) ? "S" : ".";
    line.flags += ip->hasFlag(BCI_TRY_CATCH) ? "E" : ".";
    line.flags += ip->node && ip->node->hasOwnerInline() ? "I" : ".";
    line.flags += " ";
    line.flags += ip->hasFlag(BCI_IMM_A) ? "A" : ".";
    line.flags += ip->hasFlag(BCI_IMM_B) ? "B" : ".";
    line.flags += ip->hasFlag(BCI_IMM_C) ? "C" : ".";
    line.flags += ip->hasFlag(BCI_IMM_D) ? "D" : ".";
    line.flags += ip->hasFlag(BCI_START_STMT) ? "S" : ".";
    line.flags += ip->hasFlag(BCI_JUMP_DEST) ? "J" : ".";
    line.flags += ip->hasFlag(BCI_CAN_OVERFLOW) || (ip->node && ip->node->hasAttribute(ATTRIBUTE_CAN_OVERFLOW_ON)) ? "O" : ".";

    // Pretty
    line.pretty = getPrettyInstruction(ip);
    line.pretty += " ";

    if (ip->isJump())
        line.pretty += form("%08d ", ip->b.s32 + i + 1);

    switch (ip->op)
    {
        case ByteCodeOp::DecSPBP:
            line.pretty += form("%d", bc->stackSize);
            line.pretty += " ";
            break;

        case ByteCodeOp::InternalPanic:
            if (ip->d.pointer)
            {
                line.pretty += "\"";
                line.pretty += Utf8::truncateDisplay(reinterpret_cast<const char*>(ip->d.pointer), 30);
                line.pretty += "\"";
                line.pretty += " ";
            }
            break;

        case ByteCodeOp::MakeLambda:
        {
            const auto funcNode = reinterpret_cast<AstFuncDecl*>(ip->b.pointer);
            SWAG_ASSERT(funcNode);

            line.pretty += "// ";
            line.pretty += Utf8::truncateDisplay(funcNode->token.sourceFile->name, 30);
            line.pretty += "/";
            line.pretty += funcNode->token.text;
            line.pretty += " ";
            break;
        }
    }

    if (ip->isForeignCall())
    {
        const auto funcNode = castAst<AstFuncDecl>(reinterpret_cast<AstNode*>(ip->a.pointer), AstNodeKind::FuncDecl);
        line.pretty += "// ";
        line.pretty += Utf8::truncateDisplay(funcNode->getCallName(), 30);
        line.pretty += "()";
        line.pretty += " ";
    }
    else if (ip->isLocalCall())
    {
        const auto bcCall = reinterpret_cast<ByteCode*>(ip->a.pointer);
        SWAG_ASSERT(bcCall);

        line.pretty += "// ";
        line.pretty += bcCall->getCallName();
        line.pretty += "()";

        if (bcCall->node && bcCall->node->typeInfo)
        {
            line.pretty += " ";
            line.pretty += bcCall->node->typeInfo->name;
        }

        line.pretty += " ";
    }

    // DevMode
#ifdef SWAG_DEV_MODE
    if (!forDbg && g_CommandLine.dbgPrintBcExt)
    {
        // line.devMode = form("%08d %08X %08d ", ip->treeNode, ip->crc, ip->serial);
        if (ip->sourceFile)
        {
            const Path sf = ip->sourceFile;
            line.devMode  = form("(%s:%d)", sf.filename().cstr(), ip->sourceLine);
        }
    }
#endif
}

namespace
{
    void printTransparent(const Utf8& str)
    {
        for (uint32_t i = 0; i < str.length(); ++i)
            g_Log.print(" ");
    }
}

void ByteCode::printInstruction(const ByteCodePrintOptions& options, const ByteCodeInstruction* ip, const PrintInstructionLine& line)
{
    const bool forDbg = ip && options.curIp != nullptr;

    // Bkp
    g_Log.print(line.bkp);

    // Rank
    if (options.rankColor != LogColor::Transparent)
    {
        if (options.rankColor != LogColor::Undefined)
            g_Log.setColor(options.rankColor);
        else if (forDbg && ip == options.curIp)
            g_Log.setColor(LogColor::CurInstruction);
        else
            g_Log.setColor(LogColor::Index);
        g_Log.print(line.rank);
    }
    else
    {
        printTransparent(line.rank);
    }

    // Instruction name
    if (options.nameColor != LogColor::Transparent)
    {
        if (options.nameColor != LogColor::Undefined)
            g_Log.setColor(options.nameColor);
        else if (forDbg && ip == options.curIp)
            g_Log.setColor(LogColor::CurInstruction);
        else
            g_Log.setColor(LogColor::White);
        g_Log.print(line.name);
    }
    else
    {
        printTransparent(line.name);
    }

    // Parameters
    if (options.argsColor != LogColor::Transparent)
    {
        if (options.argsColor != LogColor::Undefined)
            g_Log.setColor(options.argsColor);
        else if (forDbg && ip == options.curIp)
            g_Log.setColor(LogColor::CurInstruction);
        else
            g_Log.setColor(LogColor::White);
        g_Log.print(line.args);
    }
    else
    {
        printTransparent(line.args);
    }

    // Flags
    if (options.flagsColor != LogColor::Transparent)
    {
        if (options.flagsColor != LogColor::Undefined)
            g_Log.setColor(options.flagsColor);
        else if (forDbg && ip == options.curIp)
            g_Log.setColor(LogColor::CurInstruction);
        else
            g_Log.setColor(LogColor::InstructionFlags);
        g_Log.print(line.flags);
    }
    else
    {
        printTransparent(line.flags);
    }

    // Pretty
    if (options.prettyColor != LogColor::Transparent)
    {
        if (options.prettyColor != LogColor::Undefined)
        {
            LogWriteContext logCxt;
            logCxt.raw = true;
            g_Log.setColor(options.prettyColor);
            g_Log.print(line.pretty, &logCxt);
        }
        else if (forDbg && ip == options.curIp)
        {
            LogWriteContext logCxt;
            logCxt.raw = true;
            g_Log.setColor(LogColor::CurInstruction);
            g_Log.print(line.pretty, &logCxt);
        }
        else
        {
            SyntaxColorContext synCxt;
            LogWriteContext    logCxt;
            synCxt.forByteCode = true;
            logCxt.raw         = true;
            g_Log.setColor(LogColor::White);
            g_Log.print(doSyntaxColor(line.pretty, synCxt), &logCxt);
        }
    }
    else
    {
        printTransparent(line.pretty);
    }

#ifdef SWAG_DEV_MODE
    if (!forDbg)
    {
        if (options.devModeColor != LogColor::Transparent)
        {
            if (options.devModeColor != LogColor::Undefined)
                g_Log.setColor(options.devModeColor);
            else
                g_Log.setColor(LogColor::DarkMagenta);
            g_Log.print(line.devMode);
        }
    }
#endif

    g_Log.writeEol();
}

enum class RankStr
{
    Rank,
    Name,
    InstRef,
    Flags,
    Pretty,
#ifdef SWAG_DEV_MODE
    DevMode,
#endif
};

namespace
{
    void align(Vector<ByteCode::PrintInstructionLine>& lines, RankStr what, bool alignLeft, uint32_t defaultLen = 0)
    {
        uint32_t len = 0;
        for (auto& line : lines)
        {
            switch (what)
            {
                case RankStr::Rank:
                    len = std::max(len, line.rank.length());
                    break;
                case RankStr::Name:
                    len = std::max(len, line.name.length());
                    break;
                case RankStr::InstRef:
                    len = std::max(len, line.args.length());
                    break;
                case RankStr::Flags:
                    len = std::max(len, line.flags.length());
                    break;
                case RankStr::Pretty:
                    len = std::max(len, line.pretty.length());
                    break;
#ifdef SWAG_DEV_MODE
                case RankStr::DevMode:
                    len = std::max(len, line.devMode.length());
                    break;
#endif
            }
        }

        len = std::max(len, defaultLen);

        for (auto& line : lines)
        {
            Utf8* str = nullptr;
            switch (what)
            {
                case RankStr::Rank:
                    str = &line.rank;
                    break;
                case RankStr::Name:
                    str = &line.name;
                    break;
                case RankStr::InstRef:
                    str = &line.args;
                    break;
                case RankStr::Flags:
                    str = &line.flags;
                    break;
                case RankStr::Pretty:
                    str = &line.pretty;
                    break;
#ifdef SWAG_DEV_MODE
                case RankStr::DevMode:
                    str = &line.devMode;
                    break;
#endif
                default:
                    SWAG_ASSERT(false);
                    break;
            }

            if (alignLeft)
            {
                while (str->length() < len)
                    *str = " " + *str;
            }
            else
            {
                while (str->length() < len)
                    *str += " ";
            }

            *str += "  ";
        }
    }
}

void ByteCode::alignPrintInstructions(const ByteCodePrintOptions& options, Vector<PrintInstructionLine>& lines, bool defaultLen)
{
    align(lines, RankStr::Rank, false, defaultLen ? 10 : 0);
    align(lines, RankStr::Name, true, defaultLen ? 25 : 0);
    align(lines, RankStr::InstRef, false, defaultLen ? 40 : 0);
    align(lines, RankStr::Flags, false, defaultLen ? 12 : 0);
#ifdef SWAG_DEV_MODE
    align(lines, RankStr::DevMode, false);
#endif
}

void ByteCode::printInstruction(const ByteCodePrintOptions& options, const ByteCodeInstruction* ip) const
{
    PrintInstructionLine line;

    fillPrintInstruction(options, this, ip, line);

    Vector<PrintInstructionLine> lines;
    lines.push_back(line);
    alignPrintInstructions(options, lines, true);

    printInstruction(options, ip, lines[0]);
}

void ByteCode::print(const ByteCodePrintOptions& options, uint32_t start, uint32_t count) const
{
    Vector<PrintInstructionLine> lines;

    auto ip = out + start;
    for (uint32_t i = 0; i < count; i++)
    {
        if (ip->op == ByteCodeOp::End)
            break;

        PrintInstructionLine line;
        fillPrintInstruction(options, this, ip++, line);
        lines.push_back(line);
    }

    // End
    PrintInstructionLine line;
    fillPrintInstruction(options, this, ip, line);
    lines.push_back(line);

    alignPrintInstructions(options, lines);

    uint32_t    lastLine   = UINT32_MAX;
    SourceFile* lastFile   = nullptr;
    AstNode*    lastInline = nullptr;

    ip = out + start;
    for (uint32_t i = 0; i < count; i++)
    {
        if (ip->op == ByteCodeOp::End)
            break;
        if (options.printSourceCode)
            printSourceCode(options, this, ip, &lastLine, &lastFile, &lastInline);
        printInstruction(options, ip++, lines[i]);
    }

    // End
    printInstruction(options, ip, lines.back());
}

void ByteCode::printName() const
{
    const auto str = getPrintRefName();
    g_Log.print(str);
    g_Log.writeEol();
}

void ByteCode::print(const ByteCodePrintOptions& options) const
{
    g_Log.lock();

    // Header
    printName();

    // Instructions
    print(options, 0, numInstructions);

    g_Log.writeEol();
    g_Log.setDefaultColor();
    g_Log.unlock();
}
