#include "pch.h"
#include "CommandLine.h"
#include "ErrorIds.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Os.h"
#include "Parser.h"
#include "Report.h"
#include "SourceFile.h"
#include "SyntaxJob.h"
#include "TypeManager.h"
#include "Workspace.h"

void Workspace::formatCommand()
{
    g_LangSpec = Allocator::alloc<LanguageSpec>();
    g_LangSpec->setup();
    g_TypeMgr = Allocator::alloc<TypeManager>();
    g_TypeMgr->setup();

    Module     module;
    SourceFile file;

    module.kind = ModuleKind::Fake;
    module.setup("<format>", "<format>");
    file.path   = g_CommandLine.fileName;
    file.module = &module;

    std::error_code err;
    if (!std::filesystem::exists(file.path, err))
    {
        file.path            = std::filesystem::absolute(file.path);
        const auto filePath1 = std::filesystem::canonical(file.path, err);
        if (!err)
            file.path = filePath1;
        if (!std::filesystem::exists(file.path, err))
        {
            Report::error(formErr(Fat0030, file.path.c_str()));
            OS::exit(-1);
        }
    }

    file.load();

    SyntaxContext context;
    Parser        parser;
    ParseFlags    parseFlags = 0;
    parseFlags.add(PARSER_TRACK_COMMENTS);
    parser.setup(&context, &module, &file, parseFlags);
    if (!parser.generateAst())
        return;

    FormatAst fmt;
    fmt.fmtFlags.add(FORMAT_FOR_BEAUTIFY);
    fmt.outputNode(file.astRoot);

    if (g_CommandLine.output)
        fmt.concat->flushToFile(file.path);
}
