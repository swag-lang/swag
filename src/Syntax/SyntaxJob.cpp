#include "pch.h"
#include "Syntax/SyntaxJob.h"
#include "Format/FormatJob.h"
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

#ifdef SWAG_DEV_MODE
    if (g_CommandLine.forceFormat)
    {
        FormatContext fmtContext;
        fmtContext.setDefaultBeautify();

        Utf8 result;
        if (FormatJob::getFormattedCode(fmtContext, sourceFile->path, result))
        {
            sourceFile->buffer            = result.buffer;
            sourceFile->bufferSize        = result.count;
            sourceFile->offsetStartBuffer = 0;
            result.buffer                 = nullptr;
        }
    }
#endif

    Parser      parser;
    ParserFlags parseFlags = 0;
    if (g_CommandLine.genDoc)
        parseFlags.add(PARSER_TRACK_DOCUMENTATION);
    else if (g_CommandLine.patchMode)
        parseFlags.add(PARSER_TRACK_PATCH);
    parser.setup(&context, sourceFile->module, sourceFile, parseFlags);
    parser.generateAst();

    return JobResult::ReleaseJob;
}
