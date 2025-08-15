#include "pch.h"
#include "Format/FormatJob.h"
#include "Format/FormatAst.h"
#include "Report/Report.h"
#include "Syntax/Parser/Parser.h"
#include "Syntax/SyntaxJob.h"
#include "Wmf/Module.h"

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
    if (!tmpFile.load())
        return false;

    // Generate AST
    {
        if (!g_CommandLine.verboseErrors)
            g_SilentError++;
        SyntaxContext synContext;
        Parser        parser;
        parser.setup(&synContext, &tmpModule, &tmpFile, PARSER_TRACK_FORMAT);
        const bool ok = parser.generateAst();
        if (!g_CommandLine.verboseErrors)
            g_SilentError--;
        if (!ok)
            return false;
    }

    if (tmpFile.hasFlag(FILE_NO_FORMAT))
        return false;

    // Format
    FormatAst fmt;
    if (!fmt.outputNode(context, tmpFile.astRoot))
        return false;

    result = fmt.concat->getUtf8();
    return true;
}

JobResult FormatJob::execute()
{
    Utf8          result;
    FormatContext context;
    context.setDefaultBeautify();

    // Do it!
    if (!getFormattedCode(context, fileName, result))
        return JobResult::ReleaseJob;

    // Write file
    if (!FormatAst::writeResult(fileName, result))
        return JobResult::ReleaseJob;

    return JobResult::ReleaseJob;
}
