#include "pch.h"
#include "FormatJob.h"
#include "ErrorIds.h"
#include "FormatAst.h"
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
    if (!tmpFile.load())
        return JobResult::ReleaseJob;

    // Generate AST
    SyntaxContext context;
    Parser        parser;
    parser.setup(&context, &tmpModule, &tmpFile, PARSER_TRACK_FORMAT);
    if (!parser.generateAst())
        return JobResult::ReleaseJob;

    // Format
    FormatAst fmt;
    fmt.fmtFlags.add(FORMAT_FOR_BEAUTIFY);
    fmt.outputNode(tmpFile.astRoot);

    // Write to file
    if (!g_CommandLine.output)
        return JobResult::ReleaseJob;

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
        (void) fputc('\n', f);
    }

    (void) fflush(f);
    (void) fclose(f);

    return JobResult::ReleaseJob;
}
