#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeGen.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "Diagnostic.h"

bool ByteCodeGen::emitRawNode(ByteCodeGenContext* context, AstNode* node)
{
    switch (node->kind)
    {
    case AstNodeKind::Literal:
        SWAG_CHECK(emitLiteral(context, node));
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
    SWAG_CHECK(emitRawNode(context, node));
	addInstruction(context, ByteCodeNodeId::End);
    return true;
}

bool ByteCodeGen::internalError(ByteCodeGenContext* context, AstNode* node)
{
    context->semantic->sourceFile->report({context->semantic->sourceFile, node->token, "internal compiler error during bytecode generation"});
    return false;
}

void ByteCodeGen::addInstruction(ByteCodeGenContext* context, ByteCodeNodeId id)
{
	context->bc.out.addU16((uint16_t) id);
}