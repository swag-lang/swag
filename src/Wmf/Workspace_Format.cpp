#include "pch.h"
#include "Format/FormatJob.h"
#include "Main/CommandLine.h"
#include "Os/Os.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Workspace.h"

void Workspace::formatCommand()
{
    if (g_CommandLine.fileName.empty())
    {
        Report::error(formErr(Fat0037));
        OS::exit(-1);
    }

    Path filePath = g_CommandLine.fileName;

    // Verify that the file/folder exists
    std::error_code err;
    if (!std::filesystem::exists(filePath, err))
    {
        filePath             = std::filesystem::absolute(filePath);
        const auto filePath1 = std::filesystem::canonical(filePath, err);
        if (!err)
            filePath = filePath1;
        if (!std::filesystem::exists(filePath, err))
        {
            Report::error(formErr(Fat0030, filePath.cstr()));
            OS::exit(-1);
        }
    }

    g_ThreadMgr.init();
    g_LangSpec = Allocator::alloc<LanguageSpec>();
    g_LangSpec->setup();
    g_TypeMgr = Allocator::alloc<TypeManager>();
    g_TypeMgr->setup();

    // File
    if (std::filesystem::is_regular_file(filePath))
    {
        if (filePath.extension() != ".swg" && filePath.extension() != ".swgs")
        {
            Report::error(formErr(Fat0038, filePath.cstr()));
            OS::exit(-1);
        }

        FormatJob job;
        job.fileName = filePath;
        job.execute();
    }

    // Folder
    else if (std::filesystem::is_directory(filePath))
    {
        Vector<Utf8> dstFiles;
        OS::visitFilesRec(filePath, [&](const char* fileName) {
            const Path n = fileName;
            if (n.extension() != ".swg" && n.extension() != ".swgs")
                return;
            const auto job = Allocator::alloc<FormatJob>();
            job->fileName  = fileName;
            g_ThreadMgr.addJob(job);
        });

        g_ThreadMgr.waitEndJobs();
    }

    // Something else
    else
    {
        Report::error(formErr(Fat0038, filePath.cstr()));
        OS::exit(-1);
    }
}
