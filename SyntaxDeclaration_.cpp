#include "pch.h"
#include "SyntaxJob.h"
#include "SourceFile.h"
#include "Tokenizer.h"
#include "Diagnostic.h"
#include "Global.h"
#include "CommandLine.h"
#include "Workspace.h"
#include "Stats.h"
#include "SourceFile.h"
#include "Module.h"
#include "Pool.h"
#include "PoolFactory.h"

bool SyntaxJob::doVarDecl(AstNode* parent)
{
	auto node = Ast::newNode(&m_file->m_poolFactory->m_astVarDecl, AstNodeType::VarDecl, parent, false);
	SWAG_CHECK(m_tokenizer.getToken(m_token));
	SWAG_VERIFY(m_token.id == TokenId::Identifier, syntaxError(format("invalid variable name '%s'", m_token.text.c_str())));
	SWAG_CHECK(eatToken(TokenId::SymColon));
    node->name = m_token.text;
    return true;
}

bool SyntaxJob::doTopLevel(AstNode* parent)
{
    switch (m_token.id)
    {
	case TokenId::SymSemiColon:
		break;
    case TokenId::KwdVar:
        SWAG_CHECK(doVarDecl(parent));
        break;
    default:
        syntaxError(format("invalid token '%s'", m_token.text.c_str()));
        return false;
    }

    return true;
}
