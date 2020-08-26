#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "SourceFile.h"
#include "DiagnosticInfos.h"
#include "ByteCodeRunContext.h"
#include "Module.h"

#undef BYTECODE_OP
#define BYTECODE_OP(__op, __flags) #__op,
const char* g_ByteCodeOpNames[] = {
#include "ByteCodeOpList.h"
};

#undef BYTECODE_OP
#define BYTECODE_OP(__op, __flags) (int) strlen(#__op),
int g_ByteCodeOpNamesLen[] = {
#include "ByteCodeOpList.h"
};

#undef BYTECODE_OP
#define BYTECODE_OP(__op, __flags) __flags,
uint32_t g_ByteCodeOpFlags[] = {
#include "ByteCodeOpList.h"
};

SourceLocation* ByteCodeInstruction::getLocation(ByteCode* bc)
{
    if (flags & BCI_LOCATION_IS_BC)
    {
        if (locationBC >= bc->numInstructions)
            return nullptr;
        return bc->out[locationBC].location;
    }

    return location;
}

Utf8 ByteCode::callName()
{
    Utf8 callName;
    if (name.empty())
        callName = node->computeScopedName();
    else
        callName = name;
    callName += format("_%lX", (uint64_t) this);
    return callName;
}

TypeInfoFuncAttr* ByteCode::callType()
{
    if (node)
        return CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    return typeInfoFunc;
}

void ByteCode::enterByteCode(ByteCodeRunContext* context)
{
#ifdef SWAG_HAS_ASSERT
    if (g_CommandLine.devMode)
    {
        g_diagnosticInfos.push();
        g_diagnosticInfos.last().message    = context->bc->name;
        g_diagnosticInfos.last().sourceFile = context->sourceFile;
        g_diagnosticInfos.last().node       = context->bc->node;
    }
#endif

    auto module = context->sourceFile->module;
    if (curRC == (int) module->buildParameters.buildCfg->byteCodeMaxRecurse)
    {
        context->hasError = true;
        context->errorMsg = format("recursive overflow during bytecode execution (max recursion is '--bc-max-recurse:%d')", module->buildParameters.buildCfg->byteCodeMaxRecurse);
        return;
    }

    curRC++;
    if (curRC >= registersRC.size())
    {
        auto rc = maxReservedRegisterRC ? (Register*) g_Allocator.alloc(maxReservedRegisterRC * sizeof(Register)) : nullptr;
        registersRC.push_back(rc);
    }
}

void ByteCode::leaveByteCode()
{
#ifdef SWAG_HAS_ASSERT
    if (g_CommandLine.devMode)
    {
        g_diagnosticInfos.pop();
    }
#endif

    curRC--;
}

void ByteCode::print()
{
    static const wchar_t* bcNum = L"%08d ";

    g_Log.lock();

    g_Log.setColor(LogColor::Magenta);
    g_Log.print(sourceFile->path);
    g_Log.print(", ");
    if (node)
    {
        g_Log.print(AstNode::getNakedKindName(node).c_str());
        g_Log.print(" ");
        g_Log.print(node->name.c_str());
    }
    else
    {
        g_Log.print(" ");
        g_Log.print(name.c_str());
    }

    g_Log.eol();

    uint32_t lastLine = UINT32_MAX;
    for (int i = 0; i < (int) numInstructions; i++)
    {
        auto ip = out + i;

        // Print source code
        auto location = ip->getLocation(this);
        if (location && location->line != lastLine && ip->op != ByteCodeOp::End)
        {
            lastLine = location->line;
            auto s   = sourceFile->getLine(lastLine);
            s.trimLeft();
            g_Log.setColor(LogColor::DarkYellow);
            for (int idx = 0; idx < 9; idx++)
                g_Log.print(" ");
            g_Log.print(s);
            g_Log.print("\n");
        }

        // Instruction rank
        g_Log.setColor(LogColor::Cyan);
        wprintf(bcNum, i);

        // Instruction
        if (ip->flags & BCI_SAFETY)
            g_Log.setColor(LogColor::DarkGreen);
        else
            g_Log.setColor(LogColor::White);
        int len = (int) strlen(g_ByteCodeOpNames[(int) ip->op]);
        while (len++ < ALIGN_RIGHT_OPCODE)
            g_Log.print(" ");
        g_Log.print(g_ByteCodeOpNames[(int) ip->op]);
        g_Log.print(" ");

        // Parameters
        g_Log.setColor(LogColor::Gray);
        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_WRITE_A)
            g_Log.print(format("A [%u] ", ip->a.u32));
        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_WRITE_B)
            g_Log.print(format("B [%u] ", ip->b.u32));
        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_WRITE_C)
            g_Log.print(format("C [%u] ", ip->c.u32));
        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_WRITE_D)
            g_Log.print(format("D [%u] ", ip->d.u32));

        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
            g_Log.print(format("A (%u) ", ip->a.u32));
        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B))
            g_Log.print(format("B (%u) ", ip->b.u32));
        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C))
            g_Log.print(format("C (%u) ", ip->c.u32));
        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D))
            g_Log.print(format("D (%u) ", ip->d.u32));

        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_VAL32_A || (ip->flags & BCI_IMM_A))
            g_Log.print(format("A {0x%X} ", ip->a.u32));
        else if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_VAL64_A || (ip->flags & BCI_IMM_A))
            g_Log.print(format("A {0x%llx} ", ip->a.u32));

        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_VAL32_B || (ip->flags & BCI_IMM_B))
            g_Log.print(format("B {0x%x} ", ip->b.u32));
        else if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_VAL64_B || (ip->flags & BCI_IMM_B))
            g_Log.print(format("B {0x%llx} ", ip->b.u32));

        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_VAL32_C || (ip->flags & BCI_IMM_C))
            g_Log.print(format("C {0x%x} ", ip->c.u32));
        else if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_VAL64_C || (ip->flags & BCI_IMM_C))
            g_Log.print(format("C {0x%llx} ", ip->c.u32));

        if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_VAL32_D || (ip->flags & BCI_IMM_D))
            g_Log.print(format("D {0x%x} ", ip->d.u32));
        else if (g_ByteCodeOpFlags[(int) ip->op] & OPFLAG_READ_VAL64_D || (ip->flags & BCI_IMM_D))
            g_Log.print(format("D {0x%llx} ", ip->d.u32));

        if (ip->flags & BCI_IMM_A)
            g_Log.print("IMMA ");
        if (ip->flags & BCI_IMM_B)
            g_Log.print("IMMB ");
        if (ip->flags & BCI_IMM_C)
            g_Log.print("IMMC ");
        if (ip->flags & BCI_IMM_D)
            g_Log.print("IMMD ");

        switch (ip->op)
        {
        case ByteCodeOp::IntrinsicAssert:
            if (ip->d.pointer)
                g_Log.print((const char*) ip->d.pointer);
            break;

        case ByteCodeOp::Jump:
        case ByteCodeOp::JumpIfZero32:
        case ByteCodeOp::JumpIfZero64:
        case ByteCodeOp::JumpIfNotZero32:
        case ByteCodeOp::JumpIfNotZero64:
        case ByteCodeOp::JumpIfFalse:
        case ByteCodeOp::JumpIfTrue:
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->b.s32 + i + 1);
            break;

        case ByteCodeOp::MakeLambdaForeign:
        {
            auto func = (AstFuncDecl*) ip->b.pointer;
            SWAG_ASSERT(func);
            g_Log.print(func->sourceFile->path);
            g_Log.print(" ");
            g_Log.print(func->name);
            break;
        }

        case ByteCodeOp::MakeLambda:
        {
            auto bc = (ByteCode*) ip->b.pointer;
            SWAG_ASSERT(bc);
            g_Log.print(bc->sourceFile->path);
            g_Log.print(" ");
            g_Log.print(bc->node->name);
            break;
        }

        case ByteCodeOp::ForeignCall:
        {
            auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
            g_Log.print(funcNode->name);
            break;
        }

        case ByteCodeOp::LocalCall:
        {
            auto bc = (ByteCode*) ip->a.pointer;
            SWAG_ASSERT(bc);
            g_Log.print(bc->node ? bc->node->name : bc->name);
            if (bc->node && bc->node->typeInfo)
                g_Log.print(bc->node->typeInfo->name);
            break;
        }

        case ByteCodeOp::IntrinsicS8x1:
        case ByteCodeOp::IntrinsicS16x1:
        case ByteCodeOp::IntrinsicS32x1:
        case ByteCodeOp::IntrinsicS64x1:
        case ByteCodeOp::IntrinsicF32x1:
        case ByteCodeOp::IntrinsicF64x1:
            g_Log.print(g_TokenNames[ip->d.u32]);
            break;

        case ByteCodeOp::IntrinsicF32x2:
        case ByteCodeOp::IntrinsicF64x2:
            g_Log.print(g_TokenNames[ip->d.u32]);
            break;
        }

        g_Log.eol();
    }

    g_Log.setDefaultColor();
    g_Log.unlock();
}

void ByteCode::markLabels()
{
    uint32_t count = numJumps;
    auto     ip    = out;
    for (uint32_t i = 0; i < numInstructions && count; i++, ip++)
    {
        switch (ip->op)
        {
        case ByteCodeOp::Jump:
        case ByteCodeOp::JumpIfTrue:
        case ByteCodeOp::JumpIfFalse:
        case ByteCodeOp::JumpIfNotZero32:
        case ByteCodeOp::JumpIfNotZero64:
        case ByteCodeOp::JumpIfZero32:
        case ByteCodeOp::JumpIfZero64:
            ip[ip->b.s32 + 1].flags |= BCI_JUMP_DEST;
            ip[1].flags |= BCI_JUMP_DEST;
            count--;
            break;
        }
    }
}
