#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "SourceFile.h"
#include "ByteCodeStack.h"
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

void ByteCode::getLocation(ByteCode* bc, ByteCodeInstruction* ip, SourceFile** file, SourceLocation** location, bool force)
{
    *file     = bc->sourceFile;
    *location = force ? ip->location : nullptr;

    if (!ip || !ip->node || !ip->node->ownerScope || ip->node->kind == AstNodeKind::FuncDecl || (ip->flags & BCI_SAFETY))
        return;
    if (ip->node->ownerInline && !(ip->node->flags & AST_IN_MIXIN))
        return;

    *location = ip->location;

    if (ip->flags & BCI_LOCATION_IS_BC)
    {
        if (ip->locationBC < bc->numInstructions)
            *location = bc->out[ip->locationBC].location;
    }
}

Utf8 ByteCode::callName()
{
    // If this is an intrinsic that can be called by the compiler itself, it should not
    // have overloads, and the name will be the name alone (without the node address which is
    // used to differentiate overloads)
    if (node && node->sourceFile->isRuntimeFile)
    {
        SWAG_ASSERT(node->resolvedSymbolName);
        if (node->resolvedSymbolName->overloads.size() == 1)
            return name;
    }

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
    if (node && node->typeInfo->kind == TypeInfoKind::FuncAttr)
        return CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    return typeInfoFunc;
}

void ByteCode::addCallStack(ByteCodeRunContext* context)
{
    ByteCodeStackStep stackStep;
    stackStep.bc = context->bc;
    stackStep.ip = context->ip;
    g_byteCodeStack.push(stackStep);
}

void ByteCode::enterByteCode(ByteCodeRunContext* context)
{
    auto module = context->sourceFile->module;
    if (context->curRC == (int) module->buildParameters.buildCfg->byteCodeMaxRecurse)
    {
        context->hasError = true;
        context->errorMsg = format("recursive overflow during bytecode execution (max recursion is '--bc-max-recurse:%d')", module->buildParameters.buildCfg->byteCodeMaxRecurse);
        return;
    }

    context->curRC++;
    if (context->curRC >= context->registersRC.size())
    {
        VectorNative<Register> rc;
        context->registersRC.emplace_back(rc);
    }

    context->registersRC[context->curRC].reserve(maxReservedRegisterRC, false);
    context->registersRC[context->curRC].count = maxReservedRegisterRC;
}

void ByteCode::leaveByteCode(ByteCodeRunContext* context, bool popCallStack)
{
    if (popCallStack)
        g_byteCodeStack.pop();
    context->curRC--;
}

void ByteCode::printInstruction(ByteCodeInstruction* ip)
{
    static const wchar_t* bcNum = L"%08d ";
    int                   i     = (int) (ip - out);

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
    auto opFlags = g_ByteCodeOpFlags[(int) ip->op];
    if (opFlags & OPFLAG_WRITE_A)
        g_Log.print(format("A [%u] ", ip->a.u32));
    if (opFlags & OPFLAG_WRITE_B)
        g_Log.print(format("B [%u] ", ip->b.u32));
    if (opFlags & OPFLAG_WRITE_C)
        g_Log.print(format("C [%u] ", ip->c.u32));
    if (opFlags & OPFLAG_WRITE_D)
        g_Log.print(format("D [%u] ", ip->d.u32));

    if (opFlags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
        g_Log.print(format("A (%u) ", ip->a.u32));
    if (opFlags & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B))
        g_Log.print(format("B (%u) ", ip->b.u32));
    if (opFlags & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C))
        g_Log.print(format("C (%u) ", ip->c.u32));
    if (opFlags & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D))
        g_Log.print(format("D (%u) ", ip->d.u32));

    if (opFlags & OPFLAG_READ_VAL32_A)
        g_Log.print(format("A {0x%X} ", ip->a.u32));
    else if (opFlags & OPFLAG_READ_VAL64_A || (ip->flags & BCI_IMM_A))
        g_Log.print(format("A {0x%llx} ", ip->a.u64));

    if (opFlags & OPFLAG_READ_VAL32_B)
        g_Log.print(format("B {0x%x} ", ip->b.u32));
    else if (opFlags & OPFLAG_READ_VAL64_B || (ip->flags & BCI_IMM_B))
        g_Log.print(format("B {0x%llx} ", ip->b.u64));

    if (opFlags & OPFLAG_READ_VAL32_C)
        g_Log.print(format("C {0x%x} ", ip->c.u32));
    else if (opFlags & OPFLAG_READ_VAL64_C || (ip->flags & BCI_IMM_C))
        g_Log.print(format("C {0x%llx} ", ip->c.u64));

    if (opFlags & OPFLAG_READ_VAL32_D)
        g_Log.print(format("D {0x%x} ", ip->d.u32));
    else if (opFlags & OPFLAG_READ_VAL64_D || (ip->flags & BCI_IMM_D))
        g_Log.print(format("D {0x%llx} ", ip->d.u64));

    g_Log.setColor(LogColor::DarkMagenta);
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

    case ByteCodeOp::MakeLambda:
    {
        auto funcNode = (AstFuncDecl*) ip->b.pointer;
        SWAG_ASSERT(funcNode);
        g_Log.print(funcNode->sourceFile->path);
        g_Log.print(" ");
        g_Log.print(funcNode->token.text);
        break;
    }

    case ByteCodeOp::ForeignCall:
    {
        auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
        g_Log.print(funcNode->token.text);
        break;
    }

    case ByteCodeOp::LocalCall:
    {
        auto bc = (ByteCode*) ip->a.pointer;
        SWAG_ASSERT(bc);
        g_Log.print(bc->node ? bc->node->token.text : bc->name);
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

void ByteCode::print()
{
    g_Log.lock();

    g_Log.setColor(LogColor::Magenta);
    g_Log.print(sourceFile->path);
    g_Log.print(", ");
    if (node)
    {
        g_Log.print(AstNode::getKindName(node).c_str());
        g_Log.print(" ");
        g_Log.print(node->token.text.c_str());
    }
    else
    {
        g_Log.print(" ");
        g_Log.print(name.c_str());
    }

    auto callt = callType();
    if (callt)
    {
        g_Log.print(" ");
        g_Log.print(callt->name.c_str());
        g_Log.eol();
    }

    uint32_t lastLine = UINT32_MAX;
    auto     ip       = out;
    for (int i = 0; i < (int) numInstructions; i++)
    {
        // Print source code
        SourceFile*     file;
        SourceLocation* location;
        ByteCode::getLocation(this, ip, &file, &location);

        if (location && location->line != lastLine && ip->op != ByteCodeOp::End)
        {
            lastLine = location->line;
            auto s   = file->getLine(lastLine);
            s.trimLeft();
            g_Log.setColor(LogColor::DarkYellow);
            for (int idx = 0; idx < 9; idx++)
                g_Log.print(" ");
            g_Log.print(s);
            g_Log.print("\n");
        }

        printInstruction(ip++);
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
