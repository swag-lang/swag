#include "pch.h"
#include "FormatJob.h"
#include "FormatAst.h"
#include "Module.h"
#include "Parser.h"
#include "SyntaxJob.h"

JobResult FormatJob::execute()
{
    Module     module;
    SourceFile file;

    module.kind = ModuleKind::Fake;
    module.setup("<format>", "<format>");
    file.path   = fileName;
    file.module = &module;

    file.load();

    SyntaxContext context;
    Parser        parser;
    ParseFlags    parseFlags = 0;
    parseFlags.add(PARSER_TRACK_FORMAT);
    parser.setup(&context, &module, &file, parseFlags);
    if (!parser.generateAst())
        return JobResult::ReleaseJob;

    FormatAst fmt;
    fmt.fmtFlags.add(FORMAT_FOR_BEAUTIFY);
    fmt.outputNode(file.astRoot);

    if (g_CommandLine.output)
        fmt.concat->flushToFile(file.path);

    return JobResult::ReleaseJob;
}
