#pragma once
#include "Pool.h"
#include "ReadFileJob.h"

struct PoolFactory
{
    Pool<SourceFile>  m_sourceFile;
    Pool<ReadFileJob> m_readFileJob;
};