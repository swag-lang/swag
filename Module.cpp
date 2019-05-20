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

bool Module::build()
{
	// One ast root to rule them all
	m_astRoot = Ast::newNode(&g_Pool, AstNodeType::RootModule, nullptr);
	for (auto file : m_files)
	{
		m_astRoot->childs.push_back(file->m_astRoot);
	}

	return true;
}