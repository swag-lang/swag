#include "pch.h"
#include "Format/FormatJob.h"
#include "Format/FormatAst.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Syntax/Parser/Parser.h"
#include "Syntax/SyntaxJob.h"
#include "Wmf/Module.h"

JobResult FormatJob::execute()
{
    Module     tmpModule;
    SourceFile tmpFile;

    tmpModule.kind = ModuleKind::Fake;
    tmpModule.setup("<fake>", "<fake>");
    tmpFile.path   = fileName;
    tmpFile.module = &tmpModule;
    if (tmpFile.path.extension() == ".swgs")
        tmpFile.flags.add(FILE_SCRIPT);
    tmpFile.flags.add(FILE_FOR_FORMAT);

    // Load source file
    if (g_CommandLine.verboseStages)
        g_Log.messageVerbose(form("[%s] -- loading file", fileName.cstr()));
    if (!tmpFile.load())
        return JobResult::ReleaseJob;

    // Generate AST
    {
        if (!g_CommandLine.verboseErrors)
            g_SilentError++;
        SyntaxContext synContext;
        Parser        parser;
        if (g_CommandLine.verboseStages)
            g_Log.messageVerbose(form("[%s] -- generating AST", fileName.cstr()));
        parser.setup(&synContext, &tmpModule, &tmpFile, PARSER_TRACK_FORMAT);
        const bool ok = parser.generateAst();
        if (!g_CommandLine.verboseErrors)
            g_SilentError--;
        if (!ok)
        {
            if (g_CommandLine.verboseStages)
                g_Log.messageVerbose(form("[%s] -- AST has errors ! Cancel", fileName.cstr()));
            return JobResult::ReleaseJob;
        }
    }

    if (tmpFile.hasFlag(FILE_NO_FORMAT))
    {
        if (g_CommandLine.verboseStages)
            g_Log.messageVerbose(form("[%s] -- #global skip format detected ! Cancel", fileName.cstr()));
        return JobResult::ReleaseJob;
    }

    if (g_CommandLine.verboseStages)
        g_Log.messageVerbose(form("[%s] -- formatting", fileName.cstr()));

    // Format
    FormatContext context;
    context.setDefaultBeautify();

    FormatAst fmt;
    if (!fmt.outputNode(context, tmpFile.astRoot))
        return JobResult::ReleaseJob;
    if (!fmt.writeResult(fileName))
        return JobResult::ReleaseJob;

    return JobResult::ReleaseJob;
}
