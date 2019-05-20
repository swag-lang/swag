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
    file->module = this;
    files.push_back(file);
}

void Module::removeFile(SourceFile* file)
{
    assert(file->module == this);
    for (auto it = files.begin(); it != files.end(); ++it)
    {
        if (*it == file)
        {
            file->module = nullptr;
            files.erase(it);
            return;
        }
    }

    assert(false);
}
