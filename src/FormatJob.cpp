#include "pch.h"
#include "FormatJob.h"
#include "ErrorIds.h"
#include "FormatAst.h"
#include "Log.h"
#include "Module.h"
#include "Parser.h"
#include "Report.h"
#include "SyntaxJob.h"

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
        g_Log.messageVerbose(form("[%s] -- loading file", fileName.c_str()));
    if (!tmpFile.load())
        return JobResult::ReleaseJob;

    // Generate AST
    {
        if (!g_CommandLine.verboseErrors)
            g_SilentError++;
        SyntaxContext context;
        Parser        parser;
        if (g_CommandLine.verboseStages)
            g_Log.messageVerbose(form("[%s] -- generating AST", fileName.c_str()));
        parser.setup(&context, &tmpModule, &tmpFile, PARSER_TRACK_FORMAT);
        const bool result = parser.generateAst();
        if (!g_CommandLine.verboseErrors)
            g_SilentError--;
        if (!result)
        {
            if (g_CommandLine.verboseStages)
                g_Log.messageVerbose(form("[%s] -- AST has errors ! Cancel", fileName.c_str()));
            return JobResult::ReleaseJob;
        }
    }

    if (tmpFile.hasFlag(FILE_NO_FORMAT))
    {
        if (g_CommandLine.verboseStages)
            g_Log.messageVerbose(form("[%s] -- #global skip format detected ! Cancel", fileName.c_str()));
        return JobResult::ReleaseJob;
    }

    // Format
    if (g_CommandLine.verboseStages)
        g_Log.messageVerbose(form("[%s] -- formatting", fileName.c_str()));
    FormatAst fmt;
    fmt.fmtFlags.add(FORMAT_FOR_BEAUTIFY);
    fmt.outputNode(tmpFile.astRoot);

    // Write to file
    if (!g_CommandLine.output)
    {
        if (g_CommandLine.verboseStages)
            g_Log.messageVerbose(form("[%s] -- Done (commandline --output:false)", fileName.c_str()));
        return JobResult::ReleaseJob;
    }

    if (g_CommandLine.verboseStages)
        g_Log.messageVerbose(form("[%s] -- Writing file", fileName.c_str()));

    FILE* f = nullptr;
    if (fopen_s(&f, tmpFile.path, "wb"))
    {
        Report::errorOS(formErr(Err0096, tmpFile.path.c_str()));
        return JobResult::ReleaseJob;
    }

    const auto   content = fmt.concat->getUtf8();
    Vector<Utf8> lines;
    Utf8::tokenize(content, '\n', lines, true);

    for (auto& l : lines)
    {
        l.trimRight();
        (void) fwrite(l.data(), 1, l.length(), f);
#ifdef _WIN32
        (void) fputc('\r', f);
#endif
        (void) fputc('\n', f);
    }

    (void) fflush(f);
    (void) fclose(f);

    return JobResult::ReleaseJob;
}
