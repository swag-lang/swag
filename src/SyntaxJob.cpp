#include "pch.h"
#include "SyntaxJob.h"
#include "Concat.h"
#include "FormatAst.h"
#include "LoadSourceFileJob.h"
#include "Parser.h"
#include "SourceFile.h"

void SyntaxJob::release()
{
    Allocator::free<SyntaxJob>(this);
}

JobResult SyntaxJob::execute()
{
    baseContext        = &context;
    context.job        = this;
    context.sourceFile = sourceFile;

    // Error when loading
    if (sourceFile->numErrors)
        return JobResult::ReleaseJob;

    // Load the file if not already done
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

#ifdef SWAG_DEV_MODE
    /*
    FormatAst::OutputContext cxt;
    Concat                   concat;
    concat.init(4 * 1024);
    FormatAst::outputNode(cxt, concat, sourceFile->astRoot);
    const Path path = R"(c:/temp/)" + sourceFile->path.filename();
    concat.flushToFile(path);
    */
#endif

    return JobResult::ReleaseJob;
}
