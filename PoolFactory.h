#pragma once
#include "Pool.h"
#include "ReadFileJob.h"

struct PoolFactory
{
	mutex m_mutex;
	Pool<SourceFile>  m_sourceFile;
	Pool<ReadFileJob> m_readFileJob;
};