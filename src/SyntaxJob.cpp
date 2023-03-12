#include "pch.h"
#include "Ast.h"
#include "LoadSourceFileJob.h"
#include "SyntaxJob.h"
#include "Parser.h"

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
        auto loadJob        = Allocator::alloc<LoadSourceFileJob>();
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
