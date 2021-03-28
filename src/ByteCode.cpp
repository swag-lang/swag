#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "SourceFile.h"
#include "ByteCodeStack.h"
#include "ByteCodeRunContext.h"
#include "Module.h"
#include "TypeManager.h"

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
    *file     = ip && ip->node && ip->node->sourceFile ? ip->node->sourceFile : bc->sourceFile;
    *location = force ? ip->location : nullptr;

    if (!ip || !ip->node || !ip->node->ownerScope || ip->node->kind == AstNodeKind::FuncDecl)
        return;

    // When inside an inline block (and not a mixin), zap all inline chain to the caller
    if (!bc->sourceFile || !bc->sourceFile->module->buildCfg.byteCodeDebugInline)
    {
        if (ip->node->ownerInline && !(ip->node->flags & AST_IN_MIXIN) && ip->node->ownerInline->ownerFct == ip->node->ownerFct)
        {
            auto n = ip->node;
            while (n->ownerInline)
                n = n->ownerInline;
            *location = &n->token.startLocation;
            *file     = n->sourceFile;
            return;
        }
    }

    *location = ip->location;
}

Utf8 ByteCode::callName()
{
    // If this is an intrinsic that can be called by the compiler itself, it should not
    // have overloads, and the name will be the name alone (without the node address which is
    // used to differentiate overloads)
    if (node && node->sourceFile->isRuntimeFile && !(node->attributeFlags & ATTRIBUTE_CALLBACK) && !node->ownerStructScope)
    {
        SWAG_ASSERT(node->resolvedSymbolName);
        if (node->resolvedSymbolName->cptOverloadsInit == 1)
            return name;
    }

    Utf8 callName;
    if (name.empty())
        callName = node->getScopedName();
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
    if (context->bc && context->bc->node && context->bc->node->flags & AST_NO_CALLSTACK)
        return;

    ByteCodeStackStep stackStep;
    stackStep.bc = context->bc;
    stackStep.ip = context->ip - 1;
    g_byteCodeStack.push(stackStep);
}

void ByteCode::enterByteCode(ByteCodeRunContext* context, uint32_t popParamsOnRet, uint32_t returnReg)
{
    if (g_CommandLine.maxRecurse && context->curRC == (int) g_CommandLine.maxRecurse)
    {
        context->hasError = true;
        context->errorMsg = format("call level limit reached (maximum level is '--max-recurse:%d')", g_CommandLine.maxRecurse);
        return;
    }

    context->curRC++;
    if (context->curRC >= context->registersRC.size())
        context->registersRC.emplace_back(new VectorNative<Register>());

    context->registersRC[context->curRC]->reserve(maxReservedRegisterRC, false);
    context->registersRC[context->curRC]->count = maxReservedRegisterRC;

    context->popParamsOnRet.push_back(popParamsOnRet);
    context->returnRegOnRet.push_back(returnReg);
}

void ByteCode::leaveByteCode(ByteCodeRunContext* context, bool popCallStack)
{
    if (popCallStack)
        g_byteCodeStack.pop();
    context->curRC--;
}

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
        g_Log.setColor(LogColor::Gray);
        g_Log.print(format("%s:%d: ", file->name.c_str(), location->line + 1));
        if (lastLine)
            *lastLine = location->line;
        if (lastFile)
            *lastFile = file;
        auto s = file->getLine(location->line);
        s.trim();
        g_Log.setColor(LogColor::DarkYellow);
        if (s.empty())
            g_Log.print("<blank>");
        else
            g_Log.print(s);
        g_Log.print("\n");
    }
}

void ByteCode::printInstruction(ByteCodeInstruction* ip)
{
    static const wchar_t* bcNum = L"%08d ";
    int                   i     = (int) (ip - out);

    // Instruction rank
    if (ip->node && ip->node->ownerInline)
        g_Log.setColor(LogColor::DarkCyan);
    else
        g_Log.setColor(LogColor::Cyan);
    wprintf(bcNum, i);

    // Instruction
    if (ip->flags & BCI_SAFETY)
        g_Log.setColor(LogColor::DarkGreen);
    else if (ip->flags & BCI_TRYCATCH)
        g_Log.setColor(LogColor::DarkRed);
    else
        g_Log.setColor(LogColor::White);
    int len = (int) strlen(g_ByteCodeOpNames[(int) ip->op]);
    while (len++ < ALIGN_RIGHT_OPCODE)
        g_Log.print(" ");
    g_Log.print(g_ByteCodeOpNames[(int) ip->op]);
    g_Log.print("   ");

    // Parameters
    g_Log.setColor(LogColor::Gray);
    auto opFlags = g_ByteCodeOpFlags[(int) ip->op];
    if (opFlags & OPFLAG_WRITE_A)
        g_Log.print(format("A [%u] ", ip->a.u32));
    if (opFlags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
        g_Log.print(format("A (%u) ", ip->a.u32));
    if (opFlags & OPFLAG_READ_VAL32_A)
        g_Log.print(format("A {0x%X} ", ip->a.u32));
    else if (opFlags & OPFLAG_READ_VAL64_A || (ip->flags & BCI_IMM_A))
        g_Log.print(format("A {0x%llx} ", ip->a.u64));

    if (opFlags & OPFLAG_WRITE_B)
        g_Log.print(format("B [%u] ", ip->b.u32));
    if (opFlags & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B))
        g_Log.print(format("B (%u) ", ip->b.u32));
    if (opFlags & OPFLAG_READ_VAL32_B)
        g_Log.print(format("B {0x%x} ", ip->b.u32));
    else if (opFlags & OPFLAG_READ_VAL64_B || (ip->flags & BCI_IMM_B))
        g_Log.print(format("B {0x%llx} ", ip->b.u64));

    if (opFlags & OPFLAG_WRITE_C)
        g_Log.print(format("C [%u] ", ip->c.u32));
    if (opFlags & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C))
        g_Log.print(format("C (%u) ", ip->c.u32));
    if (opFlags & OPFLAG_READ_VAL32_C)
        g_Log.print(format("C {0x%x} ", ip->c.u32));
    else if (opFlags & OPFLAG_READ_VAL64_C || (ip->flags & BCI_IMM_C))
        g_Log.print(format("C {0x%llx} ", ip->c.u64));

    if (opFlags & OPFLAG_WRITE_D)
        g_Log.print(format("D [%u] ", ip->d.u32));
    if (opFlags & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D))
        g_Log.print(format("D (%u) ", ip->d.u32));
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

    if (ip->flags & BCI_START_STMT)
        g_Log.print("STMT ");
    if (ip->flags & BCI_UNPURE)
        g_Log.print("UNPURE ");

    switch (ip->op)
    {
    case ByteCodeOp::InternalPanic:
        if (ip->d.pointer)
            g_Log.print((const char*) ip->d.pointer);
        break;

    case ByteCodeOp::Jump:
    case ByteCodeOp::JumpIfZero8:
    case ByteCodeOp::JumpIfZero16:
    case ByteCodeOp::JumpIfZero32:
    case ByteCodeOp::JumpIfZero64:
    case ByteCodeOp::JumpIfNotZero8:
    case ByteCodeOp::JumpIfNotZero16:
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
        g_Log.print(g_TokenNames[ip->d.u32]);
        break;
    }

    g_Log.eol();
}

void ByteCode::print()
{
    g_Log.lock();

    g_Log.setColor(LogColor::DarkMagenta);
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

    uint32_t    lastLine = UINT32_MAX;
    SourceFile* lastFile = nullptr;
    auto        ip       = out;
    for (int i = 0; i < (int) numInstructions; i++)
    {
        printSourceCode(ip, &lastLine, &lastFile);
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
        case ByteCodeOp::JumpIfNotZero8:
        case ByteCodeOp::JumpIfNotZero16:
        case ByteCodeOp::JumpIfNotZero32:
        case ByteCodeOp::JumpIfNotZero64:
        case ByteCodeOp::JumpIfZero8:
        case ByteCodeOp::JumpIfZero16:
        case ByteCodeOp::JumpIfZero32:
        case ByteCodeOp::JumpIfZero64:
            ip[ip->b.s32 + 1].flags |= BCI_JUMP_DEST;
            ip[1].flags |= BCI_JUMP_DEST;
            count--;
            break;
        }
    }
}

bool ByteCode::isDoingNothing()
{
    if (callType()->returnType != g_TypeMgr.typeInfoVoid)
        return false;

    if (numInstructions == 2)
    {
        if (out[0].op == ByteCodeOp::Ret)
            return true;
    }

    if (numInstructions == 3)
    {
        if (out[0].op == ByteCodeOp::GetFromStackParam64 && out[1].op == ByteCodeOp::Ret)
            return true;
    }

    if (numInstructions == 3)
    {
        if (out[0].op == ByteCodeOp::DecSPBP &&
            out[2].op == ByteCodeOp::Ret)
        {
            return true;
        }
    }

    if (numInstructions == 4)
    {
        if (out[0].op == ByteCodeOp::ClearRA &&
            out[1].op == ByteCodeOp::IntrinsicSetErr &&
            out[2].op == ByteCodeOp::Ret)
        {
            return true;
        }
    }

    return false;
}
