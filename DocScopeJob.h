#pragma once
#include "Job.h"
struct Scope;

struct DocScopeJob : public Job
{
    JobResult execute() override;
    Scope*    scope = nullptr;
};

extern thread_local Pool<DocScopeJob> g_Pool_docScopeJob;