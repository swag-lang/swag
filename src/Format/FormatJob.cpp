#include "pch.h"
#include "Format/FormatJob.h"
#include "Format/FormatAst.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Syntax/Parser/Parser.h"
#include "Syntax/SyntaxJob.h"
#include "Wmf/Module.h"

bool FormatJob::writeResult(const Path& fileName, const Utf8& content)
{
    if (!g_CommandLine.output)
    {
        if (g_CommandLine.verboseStages)
            g_Log.messageVerbose(form("[%s] -- Done (commandline --output:false)", fileName.c_str()));
        return true;
    }

    if (g_CommandLine.verboseStages)
        g_Log.messageVerbose(form("[%s] -- Writing file", fileName.c_str()));

    FILE* f = nullptr;
    if (fopen_s(&f, fileName, "wb"))
    {
        Report::errorOS(formErr(Err0096, fileName.c_str()));
        return false;
    }

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
    return true;
}

bool FormatJob::getFormattedCode(FormatContext& context, const Path& fileName, Utf8& result)
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
        return false;

    // Generate AST
    {
        if (!g_CommandLine.verboseErrors)
            g_SilentError++;
        SyntaxContext synContext;
        Parser        parser;
        if (g_CommandLine.verboseStages)
            g_Log.messageVerbose(form("[%s] -- generating AST", fileName.c_str()));
        parser.setup(&synContext, &tmpModule, &tmpFile, PARSER_TRACK_FORMAT);
        const bool ok = parser.generateAst();
        if (!g_CommandLine.verboseErrors)
            g_SilentError--;
        if (!ok)
        {
            if (g_CommandLine.verboseStages)
                g_Log.messageVerbose(form("[%s] -- AST has errors ! Cancel", fileName.c_str()));
            return false;
        }
    }

    if (tmpFile.hasFlag(FILE_NO_FORMAT))
    {
        if (g_CommandLine.verboseStages)
            g_Log.messageVerbose(form("[%s] -- #global skip format detected ! Cancel", fileName.c_str()));
        return false;
    }

    if (g_CommandLine.verboseStages)
        g_Log.messageVerbose(form("[%s] -- formatting", fileName.c_str()));

    // Format
    FormatAst fmt;
    fmt.outputNode(context, tmpFile.astRoot);

    // Get result
    result = fmt.concat->getUtf8();
    return true;
}

JobResult FormatJob::execute()
{
    Utf8          result;
    FormatContext context;
    context.setDefaultBeautify();

    // Do it !
    if (!getFormattedCode(context, fileName, result))
        return JobResult::ReleaseJob;

    // Write file
    if (!writeResult(fileName, result))
        return JobResult::ReleaseJob;

    return JobResult::ReleaseJob;
}
