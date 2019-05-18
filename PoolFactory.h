#pragma once
#include "Pool.h"
#include "LexerJob.h"

struct PoolFactory
{
    Pool<SourceFile>  m_sourceFile;
    Pool<LexerJob> m_readFileJob;
};