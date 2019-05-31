#include "pch.h"
#include "Global.h"
#include "ByteCodeGen.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "SourceFile.h"

bool ByteCodeGen::emitRawNode(ByteCodeGenContext* context, AstNode* node)
{
    switch (node->kind)
    {
    case AstNodeKind::Literal:
        SWAG_CHECK(emitLiteral(context, node));
        break;
    case AstNodeKind::BinaryOp:
        SWAG_CHECK(emitBinaryOp(context, node));
        break;

    default:
        for (auto child : node->childs)
        {
            SWAG_CHECK(emitRawNode(context, child));
        }
        break;
    }

    return true;
}

bool ByteCodeGen::emitNode(ByteCodeGenContext* context, AstNode* node)
{
    context->bc->hasDebugInfos = context->debugInfos;
    SWAG_CHECK(emitRawNode(context, node));
    emitInstruction(context, nullptr, ByteCodeNodeId::End);
    return true;
}

bool ByteCodeGen::internalError(ByteCodeGenContext* context, AstNode* node)
{
    context->semantic->sourceFile->report({context->semantic->sourceFile, node->token, "internal compiler error during bytecode generation"});
    return false;
}

void ByteCodeGen::emitInstruction(ByteCodeGenContext* context, AstNode* node, ByteCodeNodeId id)
{
    if (context->debugInfos && node && node->sourceFileIdx != UINT32_MAX)
    {
        context->bc->out.addU16((uint16_t) ByteCodeNodeId::DebugInfos);
        context->bc->out.addU32(node->sourceFileIdx);
        context->bc->out.addU32(node->token.startLocation.line);
        context->bc->out.addU32(node->token.startLocation.column);
		context->bc->out.addU32(node->token.startLocation.seekStartLine);
        context->bc->out.addU32(node->token.endLocation.line);
        context->bc->out.addU32(node->token.endLocation.column);
		context->bc->out.addU32(node->token.endLocation.seekStartLine);
    }

    context->bc->out.addU16((uint16_t) id);
}