#include "pch.h"
#include "Syntax/SyntaxJob.h"
#include "Format/FormatAst.h"
#include "Jobs/LoadSourceFileJob.h"
#include "Syntax/Parser/Parser.h"
#include "Wmf/SourceFile.h"

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

    Parser      parser;
    ParserFlags parseFlags = 0;
    if (g_CommandLine.genDoc)
        parseFlags.add(PARSER_TRACK_DOCUMENTATION);
    parser.setup(&context, sourceFile->module, sourceFile, parseFlags);
    parser.generateAst();

#ifdef SWAG_DEV_MODE
    if (!sourceFile->numErrors)
    {
        FormatAst     fmt;
        FormatContext fmtCxt;
        fmt.outputNode(fmtCxt, sourceFile->astRoot);
    }
#endif

    return JobResult::ReleaseJob;
}
