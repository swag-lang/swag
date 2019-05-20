#include "pch.h"
#include "Module.h"
#include "SourceFile.h"
#include "Pool.h"
#include "PoolFactory.h"
#include "Global.h"

void Module::addFile(SourceFile* file)
{
    file->m_module = this;
    m_files.push_back(file);
}

void Module::removeFile(SourceFile* file)
{
    assert(file->m_module == this);
    for (auto it = m_files.begin(); it != m_files.end(); ++it)
    {
        if (*it == file)
        {
            file->m_module = nullptr;
            m_files.erase(it);
            return;
        }
    }

    assert(false);
}

bool Module::semanticNode(SourceFile* file, AstNode* node)
{
	return true;
}

bool Module::semantic()
{
    // One ast root to rule them all
    m_astRoot = Ast::newNode(&g_Pool.m_astNode, AstNodeType::RootModule);
	for (auto file : m_files)
	{
		if (file->m_buildPass < BuildPass::Semantic)
			continue;
		Ast::addChild(m_astRoot, file->m_astRoot, false);
		semanticNode(file, file->m_astRoot);
	}

	return true;
}
