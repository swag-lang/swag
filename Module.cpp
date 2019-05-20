#include "pch.h"
#include "Module.h"
#include "SourceFile.h"
#include "Pool.h"
#include "PoolFactory.h"
#include "Global.h"
#include "SemanticJob.h"
#include "ThreadManager.h"

void Module::addFile(SourceFile* file)
{
    file->m_module = this;
    files.push_back(file);
}

void Module::removeFile(SourceFile* file)
{
    assert(file->m_module == this);
    for (auto it = files.begin(); it != files.end(); ++it)
    {
        if (*it == file)
        {
            file->m_module = nullptr;
            files.erase(it);
            return;
        }
    }

    assert(false);
}
