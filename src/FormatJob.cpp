#include "pch.h"
#include "FormatJob.h"
#include "FormatAst.h"
#include "Module.h"
#include "Parser.h"
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

    tmpFile.load();
    if (tmpFile.numErrors)
        return JobResult::ReleaseJob;

    SyntaxContext context;
    Parser        parser;
    parser.setup(&context, &tmpModule, &tmpFile, PARSER_TRACK_FORMAT);
    if (!parser.generateAst())
        return JobResult::ReleaseJob;

    FormatAst fmt;
    fmt.fmtFlags.add(FORMAT_FOR_BEAUTIFY);
    fmt.outputNode(tmpFile.astRoot);

    if (g_CommandLine.output)
        fmt.concat->flushToFile(tmpFile.path);

    return JobResult::ReleaseJob;
}
