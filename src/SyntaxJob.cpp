#include "pch.h"
#include "Ast.h"
#include "LoadSourceFileJob.h"
#include "SyntaxJob.h"

JobResult SyntaxJob::execute()
{
    baseContext        = &context;
    context.job        = this;
    context.sourceFile = sourceFile;

    // Then load the file
    if (!sourceFile->buffer)
    {
        auto loadJob        = g_Allocator.alloc<LoadSourceFileJob>();
        loadJob->sourceFile = sourceFile;
        loadJob->addDependentJob(this);
        jobsToAdd.push_back(loadJob);
        return JobResult::KeepJobAlive;
    }

    Parser parser;
    parser.sourceFile = sourceFile;
    parser.context    = &context;

    parser.execute();

    return JobResult::ReleaseJob;
}
