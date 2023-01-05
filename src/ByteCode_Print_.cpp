#include "pch.h"
#include "ByteCode.h"
#include "Ast.h"
#include "Module.h"

void ByteCode::printSourceCode(ByteCodeInstruction* ip, uint32_t* lastLine, SourceFile** lastFile)
{
    if (ip->op == ByteCodeOp::End)
        return;

    // Print source code
    SourceFile*     file;
    SourceLocation* location;
    ByteCode::getLocation(this, ip, &file, &location);

    if (!location)
        return;

    if (!lastLine || !lastFile || location->line != *lastLine || file != *lastFile)
    {
        if (lastLine)
            *lastLine = location->line;
        if (lastFile)
            *lastFile = file;
        auto s = file->getLine(location->line);
        s.trim();

        g_Log.setColor(LogColor::Yellow);
        g_Log.print("         ");
        if (s.empty())
            g_Log.print("<blank>");
        else
            g_Log.print(s);

        // g_Log.setColor(LogColor::Gray);
        // g_Log.print(Fmt("  (%s:%d)", file->name.c_str(), location->line + 1));

        g_Log.eol();
    }
}

void ByteCode::printPrettyInstruction(ByteCodeInstruction* ip)
{
    Utf8 str   = g_ByteCodeOpDesc[(int) ip->op].display;
    auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;

    if (ip->flags & BCI_IMM_A || flags & OPFLAG_READ_VAL32_A || flags & OPFLAG_READ_VAL64_A)
    {
        str.replace("_rau8_", Fmt("%u", ip->a.u8));
        str.replace("_rau16_", Fmt("%u", ip->a.u16));
        str.replace("_rau32_", Fmt("%u", ip->a.u32));
        str.replace("_rau64_", Fmt("%llu", ip->a.u64));
        str.replace("_ras8_", Fmt("%d", ip->a.s8));
        str.replace("_ras16_", Fmt("%d", ip->a.s16));
        str.replace("_ras32_", Fmt("%d", ip->a.s32));
        str.replace("_ras64_", Fmt("%lld", ip->a.s64));
        str.replace("_raf32_", Fmt("%f", ip->a.f32));
        str.replace("_raf64_", Fmt("%lf", ip->a.f64));
        str.replace("_rax32_", Fmt("0x%x", ip->a.u32));
        str.replace("_rax64_", Fmt("0x%llx", ip->a.u64));
    }

    if (ip->flags & BCI_IMM_B || flags & OPFLAG_READ_VAL32_B || flags & OPFLAG_READ_VAL64_B)
    {
        str.replace("_rbu8_", Fmt("%u", ip->b.u8));
        str.replace("_rbu16_", Fmt("%u", ip->b.u16));
        str.replace("_rbu32_", Fmt("%u", ip->b.u32));
        str.replace("_rbu64_", Fmt("%llu", ip->b.u64));
        str.replace("_rbs8_", Fmt("%d", ip->b.s8));
        str.replace("_rbs16_", Fmt("%d", ip->b.s16));
        str.replace("_rbs32_", Fmt("%d", ip->b.s32));
        str.replace("_rbs64_", Fmt("%lld", ip->b.s64));
        str.replace("_rbf32_", Fmt("%f", ip->b.f32));
        str.replace("_rbf64_", Fmt("%lf", ip->b.f64));
        str.replace("_rbx32_", Fmt("0x%x", ip->b.u32));
        str.replace("_rbx64_", Fmt("0x%llx", ip->b.u64));
    }

    if (ip->flags & BCI_IMM_C || flags & OPFLAG_READ_VAL32_C || flags & OPFLAG_READ_VAL64_C)
    {
        str.replace("_rcu8_", Fmt("%u", ip->c.u8));
        str.replace("_rcu16_", Fmt("%u", ip->c.u16));
        str.replace("_rcu32_", Fmt("%u", ip->c.u32));
        str.replace("_rcu64_", Fmt("%llu", ip->c.u64));
        str.replace("_rcs8_", Fmt("%d", ip->c.s8));
        str.replace("_rcs16_", Fmt("%d", ip->c.s16));
        str.replace("_rcs32_", Fmt("%d", ip->c.s32));
        str.replace("_rcs64_", Fmt("%lld", ip->c.s64));
        str.replace("_rcf32_", Fmt("%f", ip->c.f32));
        str.replace("_rcf64_", Fmt("%lf", ip->c.f64));
        str.replace("_rcx32_", Fmt("0x%x", ip->c.u32));
        str.replace("_rcx64_", Fmt("0x%llx", ip->c.u64));
    }

    if (ip->flags & BCI_IMM_D || flags & OPFLAG_READ_VAL32_D || flags & OPFLAG_READ_VAL64_D)
    {
        str.replace("_rdu8_", Fmt("%u", ip->d.u8));
        str.replace("_rdu16_", Fmt("%u", ip->d.u16));
        str.replace("_rdu32_", Fmt("%u", ip->d.u32));
        str.replace("_rdu64_", Fmt("%llu", ip->d.u64));
        str.replace("_rds8_", Fmt("%d", ip->d.s8));
        str.replace("_rds16_", Fmt("%d", ip->d.s16));
        str.replace("_rds32_", Fmt("%d", ip->d.s32));
        str.replace("_rds64_", Fmt("%lld", ip->d.s64));
        str.replace("_rdf32_", Fmt("%f", ip->d.f32));
        str.replace("_rdf64_", Fmt("%lf", ip->d.f64));
        str.replace("_rdx32_", Fmt("0x%x", ip->d.u32));
        str.replace("_rdx64_", Fmt("0x%llx", ip->d.u64));
    }

    if (flags & (OPFLAG_READ_A | OPFLAG_WRITE_A))
    {
        auto ra = Fmt("r%u", ip->a.u32);
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

    if (flags & (OPFLAG_READ_B | OPFLAG_WRITE_B))
    {
        auto rb = Fmt("r%u", ip->b.u32);
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

    if (flags & (OPFLAG_READ_C | OPFLAG_WRITE_C))
    {
        auto rc = Fmt("r%u", ip->c.u32);
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

    if (flags & (OPFLAG_READ_D | OPFLAG_WRITE_D))
    {
        auto rd = Fmt("r%u", ip->d.u32);
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
        str.replace("_w0_", g_TokenNames[ip->d.u32]);
        break;
    }

    str.trim();
    g_Log.print(str);
}

void ByteCode::printInstructionReg(const char* header, const Register& reg, bool regW, bool regR, bool regImm)
{
    if (regW)
        g_Log.print(Fmt("%s[%u] ", header, reg.u32));
    if (regR && !regImm)
        g_Log.print(Fmt("%s(%u) ", header, reg.u32));
    if (regImm)
        g_Log.print(Fmt("%s{0x%llX} ", header, reg.u64));
}

void ByteCode::printInstruction(ByteCodeInstruction* ip, ByteCodeInstruction* curIp)
{
    static const int ALIGN_OPCODE = 25;
    static const int ALIGN_FLAGS1 = 65;
    static const int ALIGN_FLAGS2 = 70;
    static const int ALIGN_PRETTY = 80;
    static const int ALIGN_SOURCE = 125;

    static const wchar_t* bcNum  = L"%08d";
    int                   i      = (int) (ip - out);
    bool                  forDbg = curIp != nullptr;

    // Instruction rank
    if (forDbg)
    {
        if (ip == curIp)
            g_Log.setColor(LogColor::Green);
        else
            g_Log.setColor(LogColor::Gray);

        if (ip == curIp)
            g_Log.print("-> ");
        else
            g_Log.print("   ");
        wprintf(bcNum, i);
        g_Log.print(" ");
    }
    else
    {
        g_Log.setColor(LogColor::Cyan);
        wprintf(bcNum, i);
        g_Log.print(" ");
    }

    g_Log.setCountLength(true);

    // Instruction
    if (!forDbg)
        g_Log.setColor(LogColor::White);
    int len = (int) strlen(g_ByteCodeOpDesc[(int) ip->op].name);
    while (len++ < ALIGN_OPCODE)
        g_Log.print(" ");
    g_Log.print(g_ByteCodeOpDesc[(int) ip->op].name);
    g_Log.print("   ");

    // Parameters
    if (!forDbg)
        g_Log.setColor(LogColor::Gray);
    auto opFlags = g_ByteCodeOpDesc[(int) ip->op].flags;
    printInstructionReg("A", ip->a, opFlags & OPFLAG_WRITE_A, opFlags & OPFLAG_READ_A, opFlags & (OPFLAG_READ_VAL32_A | OPFLAG_READ_VAL64_A) || ip->flags & BCI_IMM_A);
    printInstructionReg("B", ip->b, opFlags & OPFLAG_WRITE_B, opFlags & OPFLAG_READ_B, opFlags & (OPFLAG_READ_VAL32_B | OPFLAG_READ_VAL64_B) || ip->flags & BCI_IMM_B);
    printInstructionReg("C", ip->c, opFlags & OPFLAG_WRITE_C, opFlags & OPFLAG_READ_C, opFlags & (OPFLAG_READ_VAL32_C | OPFLAG_READ_VAL64_C) || ip->flags & BCI_IMM_C);
    printInstructionReg("D", ip->d, opFlags & OPFLAG_WRITE_D, opFlags & OPFLAG_READ_D, opFlags & (OPFLAG_READ_VAL32_D | OPFLAG_READ_VAL64_D) || ip->flags & BCI_IMM_D);

    // Flags 1
    while (g_Log.length < ALIGN_FLAGS1)
        g_Log.print(" ");
    g_Log.print(ip->flags & BCI_SAFETY ? "S" : ".");
    g_Log.print(ip->flags & BCI_TRYCATCH ? "E" : ".");
    g_Log.print(ip->node && ip->node->ownerInline ? "I" : ".");

    // Flags 2
    while (g_Log.length < ALIGN_FLAGS2)
        g_Log.print(" ");
    if (!forDbg)
        g_Log.print(ip->flags & BCI_IMM_A ? "A" : ".");
    g_Log.print(ip->flags & BCI_IMM_B ? "B" : ".");
    g_Log.print(ip->flags & BCI_IMM_C ? "C" : ".");
    g_Log.print(ip->flags & BCI_IMM_D ? "D" : ".");
    g_Log.print(ip->flags & BCI_START_STMT ? "S" : ".");
    g_Log.print(ip->flags & BCI_UNPURE ? "U" : ".");

    // Tree Node
#ifdef SWAG_DEV_MODE
    if (!forDbg)
    {
        g_Log.setColor(LogColor::Magenta);
        g_Log.print("  ");
        wprintf(bcNum, ip->treeNode);
        g_Log.print("  ");
        g_Log.print(Fmt("%08X", ip->crc));
        g_Log.print("  ");
        g_Log.setColor(LogColor::Gray);
    }
#endif

    if (!forDbg)
        g_Log.setColor(LogColor::White);
    while (g_Log.length < ALIGN_PRETTY)
        g_Log.print(" ");
    printPrettyInstruction(ip);

    if (!forDbg)
        g_Log.setColor(LogColor::Gray);
    g_Log.print(" ");

    // Jump offset
    if (ByteCode::isJump(ip))
    {
        wprintf(bcNum, ip->b.s32 + i + 1);
        g_Log.length += 8;
    }

    switch (ip->op)
    {
    case ByteCodeOp::InternalPanic:
        if (ip->d.pointer)
            g_Log.print(Utf8::truncateDisplay((const char*) ip->d.pointer, 30));
        break;

    case ByteCodeOp::MakeLambda:
    {
        auto funcNode = (AstFuncDecl*) ip->b.pointer;
        SWAG_ASSERT(funcNode);
        g_Log.print(Utf8::truncateDisplay(funcNode->sourceFile->name, 30));
        g_Log.print("/");
        g_Log.print(funcNode->token.text);
        break;
    }

    case ByteCodeOp::ForeignCall:
    case ByteCodeOp::ForeignCallPop:
    {
        auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
        g_Log.print(Utf8::truncateDisplay(funcNode->token.text, 30));
        break;
    }

    case ByteCodeOp::LocalCall:
    case ByteCodeOp::LocalCallPop:
    case ByteCodeOp::LocalCallPopRC:
    {
        auto bc = (ByteCode*) ip->a.pointer;
        SWAG_ASSERT(bc);
        g_Log.print(bc->name);
        if (bc->node && bc->node->typeInfo)
        {
            g_Log.print(" ");
            g_Log.print(bc->node->typeInfo->name);
        }
        break;
    }
    }

#ifdef SWAG_DEV_MODE
    if (!forDbg)
    {
        g_Log.setColor(LogColor::Magenta);
        while (g_Log.length < ALIGN_SOURCE)
            g_Log.print(" ");
        g_Log.print(Fmt("%08d ", ip->serial));
        if (ip->sourceFile)
        {
            Utf8 sf = Utf8::normalizePath(ip->sourceFile);
            auto pz = strrchr(sf.buffer, '/');
            g_Log.print(Fmt("%s:%d", pz ? pz + 1 : sf.c_str(), ip->sourceLine));
        }
    }
#endif

    g_Log.setCountLength(false);
    g_Log.eol();
}

void ByteCode::print(ByteCodeInstruction* curIp)
{
    g_Log.lock();

    g_Log.setColor(LogColor::Magenta);
    g_Log.print(sourceFile->path);
    g_Log.print(", ");
    g_Log.print(name);

    auto callt = getCallType();
    if (callt)
    {
        g_Log.print(", ");
        g_Log.print(callt->name.c_str());
        g_Log.eol();
    }

    uint32_t    lastLine = UINT32_MAX;
    SourceFile* lastFile = nullptr;
    auto        ip       = out;
    for (int i = 0; i < (int) numInstructions; i++)
    {
        printSourceCode(ip, &lastLine, &lastFile);
        printInstruction(ip++, curIp);
    }

    g_Log.setDefaultColor();
    g_Log.unlock();
}
