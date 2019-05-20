#pragma once
#include "Pool.h"
#include "SyntaxJob.h"

struct PoolFactory
{
    Pool<SourceFile> m_sourceFile;
    Pool<SyntaxJob>  m_syntaxJob;
};