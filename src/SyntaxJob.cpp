#include "pch.h"
#include "Ast.h"
#include "Module.h"
#include "Parser.h"
#include "Report.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "Timer.h"
#include "ErrorIds.h"
#include "LoadSourceFileJob.h"
#include "JobThread.h"
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
