#include "pch.h"
#include "SyntaxJob.h"
#include "LoadSourceFileJob.h"
#include "Parser.h"
#include "SourceFile.h"

JobResult SyntaxJob::execute()
{
    baseContext        = &context;
    context.job        = this;
    context.sourceFile = sourceFile;

    // Error when loading
    if (sourceFile->numErrors)
        return JobResult::ReleaseJob;

    // Then load the file
    if (!sourceFile->buffer)
    {
        const auto loadJob  = Allocator::alloc<LoadSourceFileJob>();
        loadJob->sourceFile = sourceFile;
        loadJob->addDependentJob(this);
        jobsToAdd.push_back(loadJob);
        return JobResult::KeepJobAlive;
    }

    Parser parser;
    parser.setup(&context, sourceFile->module, sourceFile);
    parser.generateAst();

    return JobResult::ReleaseJob;
}
